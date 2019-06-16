#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "Pwm.h"
#include "RtcUsr.h"
#include "driver/ledc.h"


static const char *TAG = "Pwm";


#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_GPIO       (4)
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0
#define LEDC_HS_CH1_GPIO       (16)
#define LEDC_HS_CH1_CHANNEL    LEDC_CHANNEL_1
#define LEDC_HS_CH2_GPIO       (17)
#define LEDC_HS_CH2_CHANNEL    LEDC_CHANNEL_2
#define LEDC_HS_CH3_GPIO       (5)
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

    temp_hour=-1;
    Up_Light_Status=1;
    Down_Light_Status=1;
    color_temp=0;

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

    for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) 
    {
        ledc_set_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, 8192);
        ledc_update_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel);
    }

    printf("led start on\r\n");
    Led_UP_W(50,5000);
    Led_UP_Y(50,5000);
    Led_DOWN_W(50,5000);
    Led_DOWN_Y(50,5000);

    vTaskDelay(10000 / portTICK_RATE_MS);
    printf("led start on over\r\n");
    

    xTaskCreate(Led_Time_Ctl_Task, "Led_Time_Ctl_Task", 2048, NULL, 10, NULL);

    /*printf("0.LEDC set duty = %d without fade（all off）\n", 8192);
    for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) 
    {
        ledc_set_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, 8192);
        ledc_update_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel);
    }*/
    //vTaskDelay(1000 / portTICK_RATE_MS);

}

void Led_Time_Ctl_Task(void* arg)
{
    
    while(1)
    {
        if(Up_Light_Status==1)
        {
            Led_Time_Ctl();
        }
        vTaskDelay(100 / portTICK_RATE_MS);
    }  
}

void Led_Color_CTL(uint16_t color_temp)
{
    if(color_temp==3000)
    {
        Led_UP_W(0,10000);
        Led_UP_Y(100,10000);
    }
    else if(color_temp==3500)
    {
        Led_UP_W(23,10000);
        Led_UP_Y(67,10000);
    }  
    else if(color_temp==4000)
    {
        Led_UP_W(45,10000);
        Led_UP_Y(55,10000);
    } 
    else if(color_temp==4500)
    {
        Led_UP_W(72,10000);
        Led_UP_Y(28,10000);
    } 
    else if(color_temp==5000)
    {
        Led_UP_W(100,10000);
        Led_UP_Y(0,10000);
    } 
    printf("Led_Color_CTL\r\n"); 
}

void Led_Time_Ctl(void)
{
    int year,month,day,hour,min,sec;
    
    Rtc_Read(&year,&month,&day,&hour,&min,&sec);

    if(temp_hour!=hour)
    {
        if((hour>=0)&&(hour<=7))
        {
            color_temp=3000;
        }
        else if(hour==8)
        {
            color_temp=3500;
        }
        else if((hour>=9)&&(hour<=10))
        {
            color_temp=4500;
        }
        else if((hour>=11)&&(hour<=13))
        {
            color_temp=5000;
        }
        else if((hour>=14)&&(hour<=15))
        {
            color_temp=4500;
        }
        else if((hour>=16)&&(hour<=21))
        {
            color_temp=4000;
        }
        else if((hour>=22)&&(hour<=23))
        {
            color_temp=3500;
        }  
        printf("color_temp=%d\r\n",color_temp);
        temp_hour=hour; 
        Led_Color_CTL(color_temp); 
    }
}



void Led_UP_W(uint16_t duty,uint16_t fade_time)//上白光控制，duty0-100
{
    uint16_t ctl_duty=8192-(uint16_t)(81.92*(float)duty);//将0-100变为8192-0
    printf("Led_UP_W duty=%d,ctl_duty=%d\r\n",duty,ctl_duty);
    ledc_set_fade_with_time(ledc_channel[0].speed_mode,ledc_channel[0].channel, ctl_duty, fade_time);
    ledc_fade_start(ledc_channel[0].speed_mode,ledc_channel[0].channel, LEDC_FADE_NO_WAIT);
}
void Led_UP_Y(uint16_t duty,uint16_t fade_time)//上黄光控制，duty0-100
{
    uint16_t ctl_duty=8192-(uint16_t)(81.92*(float)duty);//将0-100变为8192-0
    printf("Led_UP_Y duty=%d,ctl_duty=%d\r\n",duty,ctl_duty);
    ledc_set_fade_with_time(ledc_channel[1].speed_mode,ledc_channel[1].channel, ctl_duty, fade_time);
    ledc_fade_start(ledc_channel[1].speed_mode,ledc_channel[1].channel, LEDC_FADE_NO_WAIT);
}
void Led_DOWN_W(uint16_t duty,uint16_t fade_time)//下白光控制，duty0-100
{
    uint16_t ctl_duty=8192-(uint16_t)(81.92*(float)duty);//将0-100变为8192-0
    printf("Led_DOWN_W duty=%d,ctl_duty=%d\r\n",duty,ctl_duty);
    ledc_set_fade_with_time(ledc_channel[2].speed_mode,ledc_channel[2].channel, ctl_duty, fade_time);
    ledc_fade_start(ledc_channel[2].speed_mode,ledc_channel[2].channel, LEDC_FADE_NO_WAIT);
}
void Led_DOWN_Y(uint16_t duty,uint16_t fade_time)//下黄光控制，duty0-100
{
    uint16_t ctl_duty=8192-(uint16_t)(81.92*(float)duty);//将0-100变为8192-0
    printf("Led_DOWN_Y duty=%d,ctl_duty=%d\r\n",duty,ctl_duty);
    ledc_set_fade_with_time(ledc_channel[3].speed_mode,ledc_channel[3].channel, ctl_duty, fade_time);
    ledc_fade_start(ledc_channel[3].speed_mode,ledc_channel[3].channel, LEDC_FADE_NO_WAIT);
}

