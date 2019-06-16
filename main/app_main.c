#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

#include "Nvs.h"
#include "RtcUsr.h"
#include "Wallkey.h"
#include "E2prom.h"
#include "SD25RTC.h"
#include "Pwm.h"

void timer_periodic_cb(void *arg); 
esp_timer_handle_t timer_periodic_handle = 0; //定时器句柄


esp_timer_create_args_t timer_periodic_arg = {
    .callback =
        &timer_periodic_cb, 
    .arg = NULL,            
    .name = "PeriodicTimer" 
};

void timer_periodic_cb(void *arg) //1ms中断一次
{
  static int64_t timer_count = 0;
  timer_count++;
  if (timer_count >= 3000) //3s
  {
    timer_count = 0;
    printf("[APP] Free memory: %d bytes\n", esp_get_free_heap_size());
  }
}

void app_main(void)
{
  
  ESP_ERROR_CHECK( nvs_flash_init() );
  ESP_LOGI("MAIN", "[APP] IDF version: %s", esp_get_idf_version());
  Wallkey_Init();
  E2prom_Init();
  sd25rtc_init();
  Pwm_Init();

  /*******************************timer 1s init**********************************************/
  esp_err_t err = esp_timer_create(&timer_periodic_arg, &timer_periodic_handle);
  err = esp_timer_start_periodic(timer_periodic_handle, 1000); //创建定时器，单位us，定时1ms
  if (err != ESP_OK)
  {
    printf("timer periodic create err code:%d\n", err);
  }


}
