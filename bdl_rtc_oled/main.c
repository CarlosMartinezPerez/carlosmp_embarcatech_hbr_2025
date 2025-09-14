#include <stdio.h>
#include <string.h>
#include <time.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/i2c.h"
#include "lwip/udp.h"
#include "lwip/dns.h"
#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"
#include "ds3231.h"
#include "ssd1306.h"
#include "ssd1306_i2c.h"

#define WIFI_SSID "sua rede wifi"
#define WIFI_PASS "sua senha wifi"

#define I2C_PORT_RTC i2c0
#define I2C_PORT_OLED i2c1
#define SDA_PIN_RTC  0
#define SCL_PIN_RTC  1
#define SDA_PIN_OLED 14
#define SCL_PIN_OLED  15


#define NTP_SERVER "pool.ntp.org"
#define NTP_PORT 123
#define NTP_MSG_LEN 48
#define NTP_DELTA 2208988800u // diferença 1900 → 1970

// UTC offset Brasil (horário de Brasília)
#define TZ_OFFSET -3  

static struct udp_pcb *ntp_pcb;

// Buffer para renderizar
uint8_t ssd[ssd1306_buffer_length];
struct render_area area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
};

// ==================== NTP callback =====================
static void ntp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p,
                     const ip_addr_t *addr, u16_t port) {
    if (p != NULL && p->tot_len >= NTP_MSG_LEN) {
        uint8_t buf[NTP_MSG_LEN];
        pbuf_copy_partial(p, buf, NTP_MSG_LEN, 0);
        pbuf_free(p);

        // Transmit Timestamp começa no byte 40
        uint32_t secs_since_1900 = (buf[40] << 24) | (buf[41] << 16) |
                                   (buf[42] << 8) | buf[43];

        time_t epoch = secs_since_1900 - NTP_DELTA;
        struct tm *tm_info = gmtime(&epoch);

        // Ajusta para UTC-3 (Brasil)
        tm_info->tm_hour += TZ_OFFSET;
        if (tm_info->tm_hour < 0) {
            tm_info->tm_hour += 24;
            tm_info->tm_mday -= 1;
        } else if (tm_info->tm_hour >= 24) {
            tm_info->tm_hour -= 24;
            tm_info->tm_mday += 1;
        }

        printf("✅ Hora Brasil: %02d:%02d:%02d %02d/%02d/%04d\n",
               tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec,
               tm_info->tm_mday, tm_info->tm_mon + 1, tm_info->tm_year + 1900);

        // Atualiza o RTC DS3231
        rtc_datetime_t now = {
            .second = tm_info->tm_sec,
            .minute = tm_info->tm_min,
            .hour   = tm_info->tm_hour,
            .day    = tm_info->tm_wday == 0 ? 7 : tm_info->tm_wday, // DS3231: 1=domingo
            .date   = tm_info->tm_mday,
            .month  = tm_info->tm_mon + 1,
            .year   = tm_info->tm_year + 1900
        };
        ds3231_set_datetime(I2C_PORT_RTC, &now);
        printf("⏰ RTC atualizado com hora local!\n");
    }
}

static void ntp_request() {
    ip_addr_t ntp_ip;
    err_t err = dns_gethostbyname(NTP_SERVER, &ntp_ip, NULL, NULL);

    if (err == ERR_OK) {
        printf("DNS ok: %s\n", ipaddr_ntoa(&ntp_ip));
    } else {
        printf("⚠️ Falha DNS, usando IP fixo\n");
        ipaddr_aton("200.160.7.186", &ntp_ip); // NTP Brasil
    }

    uint8_t msg[NTP_MSG_LEN] = {0};
    msg[0] = 0x1B; // LI=0, VN=3, Mode=3 (client)

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, NTP_MSG_LEN, PBUF_RAM);
    memcpy(p->payload, msg, NTP_MSG_LEN);

    udp_sendto(ntp_pcb, p, &ntp_ip, NTP_PORT);
    pbuf_free(p);
}

// ==================== MAIN =====================
int main() {
    stdio_init_all();
    sleep_ms(3000);

    // Inicializa I2C (RTC DS3231e OLED SSD1306)
    i2c_init(I2C_PORT_RTC, 400 * 1000);
    i2c_init(I2C_PORT_OLED, 400 * 1000);
    gpio_set_function(SDA_PIN_RTC, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN_RTC, GPIO_FUNC_I2C);
    gpio_set_function(SDA_PIN_OLED, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN_OLED, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN_RTC);
    gpio_pull_up(SCL_PIN_RTC);
    gpio_pull_up(SDA_PIN_OLED);
    gpio_pull_up(SCL_PIN_OLED);
    printf("I2C iniciado para RTC DS3231 e OLED SSD1306\n");
    
    ssd1306_init(I2C_PORT_OLED, 0x3C, 128, 64);
    calculate_render_area_buffer_length(&area);

    // Mensagem inicial
    memset(ssd, 0, sizeof(ssd));
    ssd1306_draw_string(ssd, 0, 0, "Iniciando...");
    render_on_display(ssd, &area);

    // Inicializa Wi-Fi
    bool wifi_ok = false;
    if (cyw43_arch_init()) {
        printf("Erro ao inicializar Wi-Fi\n");
    } else {
        cyw43_arch_enable_sta_mode();
        printf("Conectando ao Wi-Fi...\n");
        if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS,
            CYW43_AUTH_WPA2_AES_PSK, 10000)) {
            printf("⚠️ Falha Wi-Fi. Usando só RTC.\n");
        } else {
            printf("✅ Wi-Fi conectado!\n");
            wifi_ok = true;
        }
    }

    // Se Wi-Fi funcionou → sincroniza via NTP
    if (wifi_ok) {
        ntp_pcb = udp_new();
        udp_bind(ntp_pcb, IP_ADDR_ANY, 0);
        udp_recv(ntp_pcb, ntp_recv, NULL);
        ntp_request();
    }

    // Loop principal → lê do RTC e mostra
    while (true) {
        rtc_datetime_t dt;
        ds3231_get_datetime(I2C_PORT_RTC, &dt);

        memset(ssd, 0, sizeof(ssd));

        printf("RTC: %02d/%02d/%04d %02d:%02d:%02d\n",
               dt.date, dt.month, dt.year,
               dt.hour, dt.minute, dt.second);

        char line[32];
        snprintf(line, sizeof(line), "%02d/%02d/%04d", dt.date, dt.month, dt.year);
        ssd1306_draw_string(ssd, 0, 0, line);

        snprintf(line, sizeof(line), "%02d:%02d:%02d", dt.hour, dt.minute, dt.second);
        ssd1306_draw_string(ssd, 0, 16, line);

        render_on_display(ssd, &area);
        sleep_ms(1000);
    }
}