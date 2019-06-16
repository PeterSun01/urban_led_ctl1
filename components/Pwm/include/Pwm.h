#ifndef _PWM_H_
#define _PWM_H_
#include "freertos/FreeRTOS.h"

extern void Pwm_Init(void);
void Led_UP_W(uint16_t duty,uint16_t fade_time);
void Led_UP_Y(uint16_t duty,uint16_t fade_time);
void Led_DOWN_W(uint16_t duty,uint16_t fade_time);
void Led_DOWN_Y(uint16_t duty,uint16_t fade_time);
void Led_Color_CTL(uint16_t color_temp);
void Led_Time_Ctl(void);
void Led_Time_Ctl_Task(void* arg);

int temp_hour;
uint8_t Up_Light_Status,Down_Light_Status;
uint16_t color_temp;


#endif

