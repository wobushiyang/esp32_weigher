#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#define GPIO_OUTPUT_IO_0    18
#define GPIO_OUTPUT_IO_1    19
#define GPIO_OUTPUT_PIN_SEL  ((1<<GPIO_OUTPUT_IO_0) | (1<<GPIO_OUTPUT_IO_1))



void hal_gpio_init(uint64_t gpio);

#endif