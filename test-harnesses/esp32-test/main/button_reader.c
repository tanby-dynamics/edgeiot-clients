#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_system.h"

#define LOW false
#define HIGH true
#define LED 14
#define SW 33
#define WIFI_SSID "dlink-7525"
#define WIFI_PASS "gabnt94694"

static const char *TAG = "Edge IoT ESP32";

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void button_reader(void *pvParameter);
void initialize_wifi();
void initialize_nvs();
void initialize_buttons();
void initialize_leds();

void app_main() {
    initialize_nvs();
    initialize_wifi();
    initialize_buttons();
    initialize_leds();

    xTaskCreate(&button_reader, "button_reader",
                configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}

void initialize_nvs() {
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    switch (event_id) {
        case WIFI_EVENT_STA_START:
            ESP_LOGI(TAG, "Connecting to %s", WIFI_SSID);
            esp_wifi_connect();
            break;
        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "Connected to %s", WIFI_SSID);
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "Disconnected from %s", WIFI_SSID);
            esp_wifi_connect();
            break;
        case IP_EVENT_STA_GOT_IP:
            ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
            char ip_str[16];
            esp_ip4addr_ntoa(&event->ip_info.ip, ip_str, sizeof(ip_str));
            ESP_LOGI(TAG, "Got IP %s", ip_str);
            break;
        default:
            break;
    }
}

void initialize_wifi() {
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));

    esp_netif_set_hostname(ESP_IF_WIFI_STA, "EdgeIoT-1");

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    esp_netif_ip_info_t ip_info = {
        .ip = esp_ip4addr_aton("192.168.0.120"),
        .gw = esp_ip4addr_aton("192.168.0.1"),
        .netmask = esp_ip4addr_aton("255.255.255.0")
    };
    wifi_config.sta.ip_info = ip_info;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void initialize_buttons() {
    esp_rom_gpio_pad_select_gpio(SW);
    gpio_set_direction(SW, GPIO_MODE_INPUT);
    gpio_set_pull_mode(SW, GPIO_PULLUP_ONLY);
}

void initialize_leds() {
    esp_rom_gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
}

void button_reader(void *pvParameter) {
    bool ledState = LOW;
    bool prevButtonState = LOW;

    while (1) {
        bool buttonState = gpio_get_level(SW);

        if (buttonState != prevButtonState) {
            if (buttonState == HIGH) {
                ledState = !ledState;
                gpio_set_level(LED, ledState);
            }
            prevButtonState = buttonState;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
