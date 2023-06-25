#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define LED1 12
#define LED2 14
#define LED3 26

void blinkingTask(void *pvParameter);
void turnOnLed(int led);

void app_main() {
    xTaskCreate(&blinkingTask, "blinking_task",
        configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}

void blinkingTask(void *pvParameter) {
    esp_rom_gpio_pad_select_gpio(LED1);
    gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
    esp_rom_gpio_pad_select_gpio(LED2);
    gpio_set_direction(LED2, GPIO_MODE_OUTPUT);
    esp_rom_gpio_pad_select_gpio(LED3);
    gpio_set_direction(LED3, GPIO_MODE_OUTPUT);

    int currentLed = 1;
    while (1) {
        turnOnLed(currentLed);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        currentLed ++;
        if (currentLed > 3) {
            currentLed = 1;
        }
    }
}

void turnOnLed(int led) {
    gpio_set_level(LED1, 0);
    gpio_set_level(LED2, 0);
    gpio_set_level(LED3, 0);

    switch (led) {
        case 1:
            gpio_set_level(LED1, 1);
            break;
        case 2:
            gpio_set_level(LED2, 1);
            break;
        case 3:
            gpio_set_level(LED3, 1);
            break;
    }
}
