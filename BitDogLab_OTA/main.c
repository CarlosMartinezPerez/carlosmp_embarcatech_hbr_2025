#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/i2c.h"
#include "ssd1306_i2c.h"
#include "inc/ssd1306.h"
#include "lwip/apps/tftp_server.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include <string.h>

// Credenciais da sua rede aqui
#define WIFI_SSID "VIVOFIBRA-8991_EXT" // "nome-da-rede-wifi"
#define WIFI_PASS "cajuca1801" // "senha-da-rede-wifi"

#define FIRMWARE_OFFSET 0x80000
#define RED_LED_PIN 13
#define GREEN_LED_PIN 11
#define BLUE_LED_PIN 12
#define FLASH_TARGET_ADDR (XIP_BASE + FIRMWARE_OFFSET)
#define FLASH_SECTOR_SIZE (1u << 12) // 4 KB
#define MAX_FILE_SIZE (16 * 1024)
#define TFTP_BLOCK_SIZE 512

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define I2C_FREQ 400000
#define SSD1306_ADDR 0x3C

#define WIFI_CREDENTIALS_OFFSET 0x1FF000
#define WIFI_CREDENTIALS_ADDR (XIP_BASE + WIFI_CREDENTIALS_OFFSET)
#define MAX_SSID_LEN 32
#define MAX_PASS_LEN 64
#define WIFI_CREDENTIALS_SIZE (MAX_SSID_LEN + MAX_PASS_LEN) // 96 bytes
#define MAX_CREDENTIALS (FLASH_SECTOR_SIZE / WIFI_CREDENTIALS_SIZE) // 42 redes

static struct tftp_context tftp_ctx;
static uint8_t flash_buffer[MAX_FILE_SIZE];
static size_t flash_offset = 0;
static uint32_t flash_write_addr = FLASH_TARGET_ADDR;
bool transfer_completed = false;
static uint8_t display_buffer[8192];
static uint8_t wifi_buffer[WIFI_CREDENTIALS_SIZE];

static inline void jump_to_firmware() {
    asm volatile (
    "mov r0, %[start]\n"
    "ldr r1, =%[vtable]\n"
    "str r0, [r1]\n"
    "ldmia r0, {r0, r1}\n"
    "msr msp, r0\n"
    "bx r1\n"
    :
    : [start] "r" (XIP_BASE + FIRMWARE_OFFSET), [vtable] "X" (PPB_BASE + M0PLUS_VTOR_OFFSET)
    :
    );
}

static void red_led_blink() {
    for (int i = 0; i < 10; i++) {
        gpio_put(RED_LED_PIN, 1);
        sleep_ms(250);
        gpio_put(RED_LED_PIN, 0);
        sleep_ms(200);
    }
}

static void indicate_wifi_connected() {
    for (int i = 0; i < 2; i++) {
        gpio_put(RED_LED_PIN, 1);
        sleep_ms(250);
        gpio_put(RED_LED_PIN, 0);
        sleep_ms(100);
        gpio_put(GREEN_LED_PIN, 1);
        sleep_ms(250);
        gpio_put(GREEN_LED_PIN, 0);
        sleep_ms(100);
        gpio_put(BLUE_LED_PIN, 1);
        sleep_ms(250);
        gpio_put(BLUE_LED_PIN, 0);
        sleep_ms(100);
    }
}

static void* tftp_open(const char *fname, const char *mode, u8_t write) {
    if (strcmp(fname, "wifi_config") == 0 || strcmp(fname, "wifi_config.txt") == 0) {
        flash_offset = 0;
        memset(wifi_buffer, 0, WIFI_CREDENTIALS_SIZE);
        return (void*)2;
    }
    flash_offset = 0;
    flash_write_addr = FLASH_TARGET_ADDR;
    return (void*)1;
}

static int tftp_write(void *handle, struct pbuf *p) {
    if (!p) {
        return -1;
    }

    if ((int)handle == 2) {
        if (p->tot_len > WIFI_CREDENTIALS_SIZE) {
            return -1;
        }
        pbuf_copy_partial(p, wifi_buffer, p->tot_len, 0);
        flash_offset = p->tot_len;
        return 0;
    }

    int remaining = p->tot_len;
    int offset = 0;
    while (remaining > 0) {
        int chunk_size = (flash_offset + remaining <= FLASH_SECTOR_SIZE) ? remaining : (FLASH_SECTOR_SIZE - flash_offset);
        pbuf_copy_partial(p, flash_buffer + flash_offset, chunk_size, offset);
        flash_offset += chunk_size;
        remaining -= chunk_size;
        offset += chunk_size;
        if (flash_offset == FLASH_SECTOR_SIZE) {
            uint32_t ints = save_and_disable_interrupts();
            flash_range_erase(flash_write_addr - XIP_BASE, FLASH_SECTOR_SIZE);
            flash_range_program(flash_write_addr - XIP_BASE, flash_buffer, FLASH_SECTOR_SIZE);
            restore_interrupts(ints);
            flash_write_addr += FLASH_SECTOR_SIZE;
            flash_offset = 0;
        }
    }
    return 0;
}

static void tftp_close(void *handle) {
    if ((int)handle == 2) {
        if (flash_offset > 0) {
            // Parsing da nova credencial
            char temp[WIFI_CREDENTIALS_SIZE];
            memcpy(temp, wifi_buffer, flash_offset);
            temp[flash_offset] = '\0';

            char *ssid = temp;
            char *pass = NULL;
            for (int i = 0; i < flash_offset; i++) {
                if (temp[i] == ' ' || temp[i] == '\t' || temp[i] == '\r' || temp[i] == '\n') {
                    temp[i] = '\0';
                    pass = &temp[i + 1];
                    break;
                }
            }
            if (pass) {
                for (int i = 0; i < strlen(pass); i++) {
                    if (pass[i] == '\r' || pass[i] == '\n') {
                        pass[i] = '\0';
                        break;
                    }
                }
            }

            // Buffer temporário pra nova credencial
            uint8_t new_cred[WIFI_CREDENTIALS_SIZE];
            memset(new_cred, 0, WIFI_CREDENTIALS_SIZE);
            if (ssid && pass) {
                strncpy((char*)new_cred, ssid, MAX_SSID_LEN - 1);
                strncpy((char*)new_cred + MAX_SSID_LEN, pass, MAX_PASS_LEN - 1);
            }

            // Ler setor atual da Flash
            uint8_t sector_buffer[FLASH_SECTOR_SIZE];
            memcpy(sector_buffer, (const uint8_t *)WIFI_CREDENTIALS_ADDR, FLASH_SECTOR_SIZE);

            // Deslocar credenciais existentes pra frente
            for (int i = MAX_CREDENTIALS - 2; i >= 0; i--) {
                memcpy(sector_buffer + (i + 1) * WIFI_CREDENTIALS_SIZE,
                       sector_buffer + i * WIFI_CREDENTIALS_SIZE,
                       WIFI_CREDENTIALS_SIZE);
            }

            // Inserir nova credencial no início
            memcpy(sector_buffer, new_cred, WIFI_CREDENTIALS_SIZE);

            // Gravar setor ajustado
            flash_range_erase(WIFI_CREDENTIALS_OFFSET, FLASH_SECTOR_SIZE);
            flash_range_program(WIFI_CREDENTIALS_OFFSET, sector_buffer, FLASH_SECTOR_SIZE);

            // Feedback
            for (int i = 0; i < 3; i++) {
                gpio_put(BLUE_LED_PIN, 1);
                sleep_ms(200);
                gpio_put(BLUE_LED_PIN, 0);
                sleep_ms(200);
            }
        }
        return;
    }

    if (flash_offset > 0) {
        uint32_t ints = save_and_disable_interrupts();
        flash_range_erase(flash_write_addr - XIP_BASE, FLASH_SECTOR_SIZE);
        flash_range_program(flash_write_addr - XIP_BASE, flash_buffer, flash_offset);
        restore_interrupts(ints);
    }
    transfer_completed = true;
}

static void ssd1306_write_command(i2c_inst_t *i2c, uint8_t cmd) {
    uint8_t buffer[2] = {0x00, cmd};
    i2c_write_blocking(i2c, SSD1306_ADDR, buffer, 2, false);
}

static void ssd1306_write_data(i2c_inst_t *i2c, uint8_t *data, size_t len) {
    uint8_t buffer[len + 1];
    buffer[0] = 0x40;
    for (size_t i = 0; i < len; i++) {
        buffer[i + 1] = data[i];
    }
    i2c_write_blocking(i2c, SSD1306_ADDR, buffer, len + 1, false);
}

void clear_oled_display(i2c_inst_t *i2c) {
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    ssd1306_write_command(i2c, 0x21);
    ssd1306_write_command(i2c, 0);
    ssd1306_write_command(i2c, 127);
    ssd1306_write_command(i2c, 0x22);
    ssd1306_write_command(i2c, 0);
    ssd1306_write_command(i2c, 63);
    ssd1306_write_data(i2c, display_buffer, sizeof(display_buffer));
}

int main() {
    stdio_init_all();
    gpio_init(RED_LED_PIN);
    gpio_init(GREEN_LED_PIN);
    gpio_init(BLUE_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);
    gpio_set_dir(BLUE_LED_PIN, GPIO_OUT);

    i2c_init(I2C_PORT, I2C_FREQ);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init();

    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);

    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);

    clear_oled_display(I2C_PORT);

    if (cyw43_arch_init()) {
        ssd1306_draw_string(ssd, 0, 0, "FALHA WIFI");
        render_on_display(ssd, &frame_area);
        red_led_blink();
        return 1;
    }

    cyw43_arch_enable_sta_mode();

    const uint8_t *flash_data = (const uint8_t *)WIFI_CREDENTIALS_ADDR;
    char ssid[MAX_SSID_LEN];
    char pass[MAX_PASS_LEN];
    char connected_ssid[MAX_SSID_LEN];
    bool connected = false;

    // Tenta credenciais hardcoded primeiro
    gpio_put(GREEN_LED_PIN, 1);
    sleep_ms(200);
    gpio_put(GREEN_LED_PIN, 0);
    sleep_ms(200);
    if (!cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 20000)) {
        connected = true;
        strncpy(connected_ssid, WIFI_SSID, MAX_SSID_LEN - 1);
        connected_ssid[MAX_SSID_LEN - 1] = '\0';
    }

    // Se falhar, busca na Flash
    if (!connected) {
        for (int i = 0; i < MAX_CREDENTIALS; i++) {
            strncpy(ssid, (const char *)(flash_data + i * WIFI_CREDENTIALS_SIZE), MAX_SSID_LEN - 1);
            ssid[MAX_SSID_LEN - 1] = '\0';
            strncpy(pass, (const uint8_t *)(flash_data + i * WIFI_CREDENTIALS_SIZE + MAX_SSID_LEN), MAX_PASS_LEN - 1);
            pass[MAX_PASS_LEN - 1] = '\0';

            if (ssid[0] == '\0' || ssid[0] == 0xFF) {
                break;
            }

            gpio_put(GREEN_LED_PIN, 1);
            sleep_ms(200);
            gpio_put(GREEN_LED_PIN, 0);
            sleep_ms(200);

            if (!cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, 20000)) {
                connected = true;
                strncpy(connected_ssid, ssid, MAX_SSID_LEN - 1);
                connected_ssid[MAX_SSID_LEN - 1] = '\0';
                break;
            }
        }
    }

    memset(ssd, 0, ssd1306_buffer_length);
    if (!connected) {
        ssd1306_draw_string(ssd, 0, 0, "FALHA CONEXAO");
        ssd1306_draw_string(ssd, 0, 16, "AGUARDANDO CONFIG");
        render_on_display(ssd, &frame_area);
        red_led_blink();
    } else {
        indicate_wifi_connected();
        ip_addr_t *ip = &cyw43_state.netif[CYW43_ITF_STA].ip_addr;
        char ip_display[20];
        snprintf(ip_display, sizeof(ip_display), "%s", ip4addr_ntoa(ip));
        for (int i = 0; ip_display[i]; i++) {
            if (ip_display[i] == '.') {
                ip_display[i] = ' ';
            }
        }
        ssd1306_draw_string(ssd, 0, 0, ip_display);
        ssd1306_draw_string(ssd, 0, 16, connected_ssid);
        ssd1306_draw_string(ssd, 0, 40, "SERVIDOR TFTP ON");
        render_on_display(ssd, &frame_area);

        tftp_ctx.open = tftp_open;
        tftp_ctx.write = tftp_write;
        tftp_ctx.close = tftp_close;

        err_t err = tftp_init_server(&tftp_ctx);
        if (err != ERR_OK) {
            memset(ssd, 0, ssd1306_buffer_length);
            ssd1306_draw_string(ssd, 0, 0, "ERRO TFTP");
            render_on_display(ssd, &frame_area);
            return 1;
        }
    }

    while (true) {
        cyw43_arch_poll();
        sleep_ms(10);
        if (transfer_completed) {
            clear_oled_display(I2C_PORT);
            cyw43_arch_deinit();
            jump_to_firmware();
        }
    }
}