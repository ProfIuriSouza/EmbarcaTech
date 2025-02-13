#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/i2c.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"
#include "lwip/init.h"


#define WIFI_SSID "wLabRedes2.4"
#define WIFI_PASS "labredes"
#define THINGSPEAK_HOST "api.thingspeak.com"
#define THINGSPEAK_PORT 80
#define API_KEY "ATY789W3QU7EPWY9"  // Chave de escrita do ThingSpeak

// I2C defines
#define I2C_PORT i2c1
#define PINO_SCL 14
#define PINO_SDA 15
#define BUTTON_PIN 5

//uint button_count = 0;

struct tcp_pcb *tcp_client_pcb;
ip_addr_t server_ip;

volatile uint32_t button_count = 0;

// Interrupção para contar as pressões do botão
void button_callback(uint gpio, uint32_t events) {
    button_count++;
}

// Callback quando recebe resposta do ThingSpeak
static err_t http_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        tcp_close(tpcb);
        return ERR_OK;
    }
    printf("Resposta do ThingSpeak: %.*s\n", p->len, (char *)p->payload);
    pbuf_free(p);
    return ERR_OK;
}

// Callback quando a conexão TCP é estabelecida
static err_t http_connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err != ERR_OK) {
        printf("Erro na conexão TCP\n");
        return err;
    }

    printf("Conectado ao ThingSpeak!\n");

    //float temperature = read_temperature();  // Lê a temperatura
    char request[256];
    snprintf(request, sizeof(request),
        "GET /update?api_key=%s&field2=%d HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "\r\n",
        API_KEY, button_count, THINGSPEAK_HOST);

    tcp_write(tpcb, request, strlen(request), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);
    tcp_recv(tpcb, http_recv_callback);

    return ERR_OK;
}

// Resolver DNS e conectar ao servidor
static void dns_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
    if (ipaddr) {
        printf("Endereço IP do ThingSpeak: %s\n", ipaddr_ntoa(ipaddr));
        tcp_client_pcb = tcp_new();
        tcp_connect(tcp_client_pcb, ipaddr, THINGSPEAK_PORT, http_connected_callback);
    } else {
        printf("Falha na resolução de DNS\n");
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(PINO_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PINO_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PINO_SDA);
    gpio_pull_up(PINO_SCL);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    
    if (cyw43_arch_init()) {
        printf("Falha ao iniciar Wi-Fi\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");

    if (cyw43_arch_wifi_connect_blocking(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_MIXED_PSK)) {
        printf("Falha ao conectar ao Wi-Fi\n");
        return 1;
    }

    printf("Wi-Fi conectado!\n");

    while (true) {
        if(gpio_get(BUTTON_PIN) == 0){
            button_count++;
            printf("O botão foi pressionado! Contagem: %d\n", button_count);
        }
       
        dns_gethostbyname(THINGSPEAK_HOST, &server_ip, dns_callback, NULL);
        sleep_ms(15000);  // Espera 15 segundos antes de enviar novamente
    }

    return 0;
}