#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "main";

EventGroupHandle_t key_event_handle = NULL;
#define KEY1_SHORT_PRESS_EVENT (0x01 << 1)

void key_event_create(void){
    key_event_handle =  xEventGroupCreate();
    if (NULL != key_event_handle){
        printf("key event handle ok\n");
    }else{
        printf("key event handle err\n");
    }
}

void key_scan_task(void *arg){
    gpio_pad_select_gpio(GPIO_NUM_0);
    gpio_set_direction(GPIO_NUM_0, GPIO_MODE_INPUT);

    while(1){
        if(gpio_get_level(GPIO_NUM_0) == 0){
            xEventGroupSetBits(key_event_handle, KEY1_SHORT_PRESS_EVENT);
        }else{

        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void test_task(void *arg){
    while(1){
        EventBits_t r_event = xEventGroupWaitBits(key_event_handle, KEY1_SHORT_PRESS_EVENT, pdTRUE, pdFALSE, 1000 / portTICK_PERIOD_MS);
        if (r_event & KEY1_SHORT_PRESS_EVENT){
            ESP_LOGI(TAG, "有事件发生\n");
        }else{
            ESP_LOGI(TAG, "无事件发生\n");
        }
    }
}

void app_main(void)
{
    key_event_create();
    xTaskCreatePinnedToCore(key_scan_task, "key_scan_task", 2048, NULL, 1, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(test_task, "test_task", 2048, NULL, 1, NULL, tskNO_AFFINITY);
}