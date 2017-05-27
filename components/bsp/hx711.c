#include "driver/gpio.h"
#include "hx711.h"
#include <stdio.h>

#define PD_SCK    21
#define GPIO_OUTPUT_PIN_SEL  (1<<PD_SCK)


static void nop(int x)

{

  for(int i=x;i>0;i--)
  {
	;
  }

}
static uint32_t hx711_raw_read()
{
	gpio_set_level(PD_SCK, 0);
	uint32_t count=0;
	while(gpio_get_level(GPIO_NUM_34));
	for(int i=0;i<24;i++)
	{
	 	gpio_set_level(PD_SCK, 1);
	 	count=count<<1;
	 	gpio_set_level(PD_SCK, 0);
	 	if(gpio_get_level(GPIO_NUM_34))
	 		count++;
	 	//nop(5);
	}
	gpio_set_level(PD_SCK, 1);
	count=count^0x800000;
	nop(5);
	gpio_set_level(PD_SCK, 0);
	return count;
}
static uint32_t first;
void hx711_init()
{
	gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = ((uint64_t)1)<<34;  
    io_conf.mode = GPIO_MODE_INPUT;
    gpio_config(&io_conf);
    first=hx711_raw_read();
}
float hx711_read()
{
	uint32_t a=hx711_raw_read();
	if(a>first)
		a=a-first;
	else
		a=0;
	return a/400.0;
}



