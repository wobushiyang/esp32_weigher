#ifndef HAL_WIFI_H
#define HAL_WIFI_H

#include "esp_err.h"
#include "esp_smartconfig.h"
#include "hal_gpio.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

void hal_wifi_init();
void hal_smartconfig_start();

extern SemaphoreHandle_t wifi_done;
#endif