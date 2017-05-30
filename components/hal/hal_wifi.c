#include "esp_wifi.h"
#include "hal_wifi.h"


#include <string.h>


static void hal_wifi_sta()
{
	 ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA));
}

SemaphoreHandle_t wifi_done;


static void hal_smartconfig_callback(smartconfig_status_t status, void *pdata)
{
	switch(status)
	{
		case SC_STATUS_WAIT:{
			printf("SC_STATUS_WAIT\n");
			break;
		}
		case SC_STATUS_FIND_CHANNEL:{
			printf("Find Channel\n");
			break;
		}
		case SC_STATUS_GETTING_SSID_PSWD:{
			smartconfig_type_t *a=pdata;
			if (*a==SC_TYPE_ESPTOUCH)
				printf("SC_TYPE:SC_TYPE_ESPTOUCH");
			else if(*a==SC_TYPE_AIRKISS)
				printf("SC_TYPE:SC_TYPE_ESPAIRKISS");
		
			break;
		}
		case SC_STATUS_LINK:{
			printf("SC_STATUS_LINK\n");
			wifi_config_t wifi_config;
			wifi_config.sta=*((wifi_sta_config_t *)pdata);

			ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
			esp_wifi_disconnect();
			esp_wifi_connect();
			//ESP_ERROR_CHECK(esp_wifi_start());
			break;
		}
		case SC_STATUS_LINK_OVER:{
			gpio_set_level(GPIO_OUTPUT_IO_1,1);
			printf("LINK OVER\n");
			if(pdata!=NULL){
				uint8_t addr[4];
				memcpy(addr,pdata,4);
				printf("phone ip addr:%02X.%02x.%02x.%02x\n",addr[0],addr[1],addr[2],addr[3]);
			}
			esp_smartconfig_stop();
			xSemaphoreGive(wifi_done);
		}
		default:break;
	}
}
void hal_smartconfig_start()
{
	
	esp_smartconfig_set_type(SC_TYPE_ESPTOUCH_AIRKISS);
	ESP_ERROR_CHECK(esp_smartconfig_start(hal_smartconfig_callback));

}
void hal_wifi_init()
{
	wifi_init_config_t cfg=WIFI_INIT_CONFIG_DEFAULT();
	wifi_done=xSemaphoreCreateBinary();
	//xSemaphoreGive(wifi_done);
    //xSemaphoreTake(wifi_done,(portTickType)portMAX_DELAY);
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	hal_wifi_sta();
	esp_wifi_start();

}
