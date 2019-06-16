#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "Pwm.h"
#include "driver/ledc.h"


static const char *TAG = "Pwm";


#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_GPIO       (16)
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0
#define LEDC_HS_CH1_GPIO       (4)
#define LEDC_HS_CH1_CHANNEL    LEDC_CHANNEL_1
#define LEDC_HS_CH2_GPIO       (5)
#define LEDC_HS_CH2_CHANNEL    LEDC_CHANNEL_2
#define LEDC_HS_CH3_GPIO       (17)
#define LEDC_HS_CH3_CHANNEL    LEDC_CHANNEL_3

#define LEDC_TEST_CH_NUM       (4)
//#define LEDC_TEST_DUTY         (8192)
#define LEDC_TEST_FADE_TIME    (4000)

ledc_channel_config_t ledc_channel[LEDC_TEST_CH_NUM] = {
    {
        .channel    = LEDC_HS_CH0_CHANNEL,
        .duty       = 0,
        .gpio_num   = LEDC_HS_CH0_GPIO,
        .speed_mode = LEDC_HS_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_HS_TIMER
    },
    {
        .channel    = LEDC_HS_CH1_CHANNEL,
        .duty       = 0,
        .gpio_num   = LEDC_HS_CH1_GPIO,
        .speed_mode = LEDC_HS_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_HS_TIMER
    },
    {
        .channel    = LEDC_HS_CH2_CHANNEL,
        .duty       = 0,
        .gpio_num   = LEDC_HS_CH2_GPIO,
        .speed_mode = LEDC_HS_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_HS_TIMER
    },
    {
        .channel    = LEDC_HS_CH3_CHANNEL,
        .duty       = 0,
        .gpio_num   = LEDC_HS_CH3_GPIO,
        .speed_mode = LEDC_HS_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_HS_TIMER
    },

};

void Pwm_Init(void)
{
    int ch;

    //PWM频率 5KHZ  占空比个数 0-8192
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
        .freq_hz = 5000,                      // frequency of PWM signal
        .speed_mode = LEDC_HS_MODE,           // timer mode
        .timer_num = LEDC_HS_TIMER            // timer index
    };
    // Set configuration of timer0 for high speed channels
    ledc_timer_config(&ledc_timer);

    // Set LED Controller with previously prepared configuration
    for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
        ledc_channel_config(&ledc_channel[ch]);
    }

    // Initialize fade service.
    ledc_fade_func_install(0);

    printf("0.LEDC set duty = %d without fade（all off）\n", 8192);
    for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) 
    {
        ledc_set_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, 8192);
        ledc_update_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel);
    }
    //vTaskDelay(1000 / portTICK_RATE_MS);

}

void Led_UP_W(uint16_t duty)//上白光控制，duty0-100
{
    uint16_t ctl_duty=8192-(uint16_t)(40.96*(float)duty);//将0-100变为8192-4096
    ledc_set_fade_with_time(ledc_channel[0].speed_mode,ledc_channel[0].channel, ctl_duty, LEDC_TEST_FADE_TIME);
    ledc_fade_start(ledc_channel[0].speed_mode,ledc_channel[0].channel, LEDC_FADE_NO_WAIT);
}
void Led_UP_Y(uint16_t duty)//上黄光控制，duty0-100
{
    uint16_t ctl_duty=8192-(uint16_t)(40.96*(float)duty);//将0-100变为8192-4096
    ledc_set_fade_with_time(ledc_channel[1].speed_mode,ledc_channel[1].channel, ctl_duty, LEDC_TEST_FADE_TIME);
    ledc_fade_start(ledc_channel[1].speed_mode,ledc_channel[1].channel, LEDC_FADE_NO_WAIT);
}
void Led_DOWN_W(uint16_t duty)//下白光控制，duty0-100
{
    uint16_t ctl_duty=8192-(uint16_t)(40.96*(float)duty);//将0-100变为8192-4096
    ledc_set_fade_with_time(ledc_channel[2].speed_mode,ledc_channel[2].channel, ctl_duty, LEDC_TEST_FADE_TIME);
    ledc_fade_start(ledc_channel[2].speed_mode,ledc_channel[2].channel, LEDC_FADE_NO_WAIT);
}
void Led_DOWN_Y(uint16_t duty)//下黄光控制，duty0-100
{
    uint16_t ctl_duty=8192-(uint16_t)(40.96*(float)duty);//将0-100变为8192-4096
    ledc_set_fade_with_time(ledc_channel[3].speed_mode,ledc_channel[3].channel, ctl_duty, LEDC_TEST_FADE_TIME);
    ledc_fade_start(ledc_channel[3].speed_mode,ledc_channel[3].channel, LEDC_FADE_NO_WAIT);
}

