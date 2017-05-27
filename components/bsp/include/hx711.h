#ifndef HX711_H
#define HX711_H

#include "driver/gpio.h"


void hx711_init();
float hx711_read();
#endif