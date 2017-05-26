#include "driver/gpio.h"
#include "hx711.h"
#include <stdio.h>

#define PD_SCK    21
#define GPIO_OUTPUT_PIN_SEL  (1<<PD_SCK)
#define DOUT     34
#define GPIO_INPUT_PIN_SEL  (1<<DOUT) 


static void nop(int x)

{

  for(int i=x;i>0;i--)
  {
	;
  }

}
void hx711_init()
{
	gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;  
    io_conf.mode = GPIO_MODE_INPUT;
    gpio_config(&io_conf);

}
uint32_t hx711_read()
{
	gpio_set_level(PD_SCK, 0);
	uint32_t count=0;
	printf("start\n");
	while(gpio_get_level(DOUT));
	for(int i=0;i<24;i++)
	{
	 	gpio_set_level(PD_SCK, 1);
	 	nop(5);
	 	count=count<<1;
	 	gpio_set_level(PD_SCK, 0);
	 	if(gpio_get_level(DOUT))
	 		count++;
	 	nop(5);
	}
	gpio_set_level(PD_SCK, 1);
	count=count^0x800000;
	nop(5);
	gpio_set_level(PD_SCK, 0);
	return count;
}


