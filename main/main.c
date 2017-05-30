
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "hal_gpio.h"
#include "hal_wifi.h"
#include "hx711.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include "tcpip_adapter.h"
#include "lwip/sockets.h"
#include "cJSON.h"
#include "esp_wifi.h"

#define port 8080



static esp_err_t event_handler(void *ctx, system_event_t * event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        //esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        printf("ip: " IPSTR "\n", IP2STR(&event->event_info.got_ip.ip_info.ip));
        printf("netmask: " IPSTR "\n", IP2STR(&event->event_info.got_ip.ip_info.netmask));
        printf("gw: " IPSTR "\n", IP2STR(&event->event_info.got_ip.ip_info.gw));
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        //esp_wifi_connect();
        //xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static TaskHandle_t smart_config_handle;
void smart_config_task(void* pvParameters)
{
    printf("%s\n","start smart_config" );
    hal_smartconfig_start();
    int cnt=1;
    while(1){
        vTaskDelay(1000 / portTICK_RATE_MS);
        gpio_set_level(GPIO_OUTPUT_IO_1, cnt % 2);    
        cnt++;
    }
}


struct sockaddr_in server, client;
int creat_socket_server(in_port_t in_port, in_addr_t in_addr)
{
    int socket_fd;

    server.sin_family = AF_INET;
    server.sin_port = in_port;
    server.sin_addr.s_addr = in_addr;

    if((socket_fd = socket(AF_INET, SOCK_STREAM, 0))<0) {
        printf("listen socket uninit\n");
        return -1;
    }
    if((bind(socket_fd, (struct sockaddr *)&server, sizeof(server)))<0) {
        printf("cannot bind srv socket\n");
        return -1;
    }

    if(listen(socket_fd, 5)<0) {
        printf("cannot listen");
        close(socket_fd);
        return -1;
    }

    return socket_fd;
}
void weigher_task(void* pvParameters)
{
    int socket_fd,client_fd;
    int wlen;
    cJSON* root=NULL;

    printf("%s\n","weigher task");
    socket_fd=creat_socket_server(htons(port),htonl(INADDR_ANY));  
    int client_size;
    client_size=sizeof(client);
    do{
        client_fd=accept(socket_fd,(struct sockaddr*)&client,(socklen_t*)&client_size);
    }while(client_fd<0);

    printf("connected to %s:%u\n",inet_ntoa(client.sin_addr), ntohs(client.sin_port));   
    gpio_set_level(GPIO_OUTPUT_IO_0,1);
    while(1){
        vTaskDelay(1000 / portTICK_RATE_MS);
        root=cJSON_CreateObject();
        if(root==NULL){
            printf("json create failed\n");
            return;
        }
        float a=hx711_read();
        cJSON_AddNumberToObject(root, "weight", (int)a);
        char* out = cJSON_PrintUnformatted(root);
        wlen=send(client_fd,out,strlen(out),MSG_DONTWAIT);
        if(wlen<0){
            close(client_fd);
            gpio_set_level(GPIO_OUTPUT_IO_0,0);
            do{
                client_fd=accept(socket_fd,(struct sockaddr*)&client,(socklen_t*)&client_size); 
            }while(client_fd<0);  
            printf("connected to %s:%u\n",inet_ntoa(client.sin_addr), ntohs(client.sin_port)); 
            gpio_set_level(GPIO_OUTPUT_IO_0,1);  
        }
        cJSON_Delete(root);
        free(out);
    }  
}
void app_main()
{
    esp_err_t err;

    tcpip_adapter_init();
    nvs_flash_init();
    esp_event_loop_init(event_handler, NULL);
    hx711_init();
    hal_gpio_init(GPIO_OUTPUT_PIN_SEL);
    hal_wifi_init();
    
    err=esp_wifi_connect();
    if(err!=ESP_OK)
    {
        xTaskCreate(&smart_config_task, "smart_config_task", 2048, NULL, 5, &smart_config_handle);
        //vTaskDelete(1000/portTICK_RATE_MS);
        xSemaphoreTake(wifi_done,(portTickType)portMAX_DELAY);
        vTaskDelete(smart_config_handle);
    }
    gpio_set_level(GPIO_OUTPUT_IO_1,1);
      
    xTaskCreate(&weigher_task, "weigher_task", 2048, NULL, 5, NULL);
    while(1) {
        vTaskDelay(5000 / portTICK_RATE_MS);
        //gpio_set_level(GPIO_OUTPUT_IO_1, cnt % 2);
        // float a=hx711_read();
        // printf("weigher:%f\n",a)

    }
}

