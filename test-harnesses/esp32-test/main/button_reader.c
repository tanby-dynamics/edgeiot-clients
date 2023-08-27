#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define LOW false
#define HIGH true

#define LED1 26
#define LED2 14
#define SW1 32
#define SW2 33

void buttonReader(void *pvParameter);

void app_main() {
    xTaskCreate(&buttonReader, "button_reader",
        configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}

void buttonReader(void *pvParameter) {
    esp_rom_gpio_pad_select_gpio(LED1);
    gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
    esp_rom_gpio_pad_select_gpio(LED2);
    gpio_set_direction(LED2, GPIO_MODE_OUTPUT);

    esp_rom_gpio_pad_select_gpio(SW1);
    gpio_set_direction(SW1, GPIO_MODE_INPUT);
    gpio_set_pull_mode(SW1, GPIO_PULLUP_ONLY);
    esp_rom_gpio_pad_select_gpio(SW2);
    gpio_set_direction(SW2, GPIO_MODE_INPUT);
    gpio_set_pull_mode(SW2, GPIO_PULLUP_ONLY);

    bool ledState1 = LOW;
    bool ledState2 = LOW;

    bool prevButtonState1 = LOW;
    bool prevButtonState2 = LOW;

    while (1) {
        bool buttonState1 = gpio_get_level(SW1);
        bool buttonState2 = gpio_get_level(SW2);

        if (buttonState1 != prevButtonState1) {
            if (buttonState1 == HIGH) {
                ledState1 = !ledState1;
                gpio_set_level(LED1, ledState1);
            }
            prevButtonState1 = buttonState1;
        }

        if (buttonState2 != prevButtonState2) {
            if (buttonState2 == HIGH) {
                ledState2 = !ledState2;
                gpio_set_level(LED2, ledState2);
            }
            prevButtonState2 = buttonState2;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

