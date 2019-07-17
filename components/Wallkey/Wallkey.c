#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "Wallkey.h"
#include "Pwm.h"


#define UART1_TXD  (UART_PIN_NO_CHANGE)
#define UART1_RXD  (GPIO_NUM_22)
#define UART1_RTS  (UART_PIN_NO_CHANGE)
#define UART1_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE    100
static const char *TAG = "WALLKEY";

uint8_t Key_Id[4]={0x86,0x6d,0x02,0x00};

static void Wallkey_Read_Task(void* arg);

void Wallkey_Init(void)
{
    //配置GPIO
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = 1 << UART1_RXD;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    
    
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, UART1_TXD, UART1_RXD, UART1_RTS, UART1_CTS);
    uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);

    xTaskCreate(Wallkey_Read_Task, "Wallkey_Read_Task", 2048, NULL, 10, NULL);
}


int8_t Wallkey_Read(void)
{
    uint8_t data_u1[100];
 
    int len1 = uart_read_bytes(UART_NUM_1, data_u1, BUF_SIZE, 20 / portTICK_RATE_MS);
    if(len1!=0)  //读取到按键数据
    {
        len1=0;
        if((data_u1[0]==0x7e)&&(data_u1[8]==0xef))//校验数据头和第9个字节固定为0XEF
        {
            if((data_u1[3]==Key_Id[0])&&(data_u1[4]==Key_Id[1])&&(data_u1[5]==Key_Id[2])&&(data_u1[6]==Key_Id[3]))    //校验KEY ID是否满足
            {
                if(data_u1[9]==KEY_DOU_LEFT_UP)
                {
                    ESP_LOGI(TAG, "KEY_LEFT");
                }
                else if(data_u1[9]==KEY_DOU_LEFT_DOWN)
                {
                    ESP_LOGI(TAG, "KEY_RIGHT");
                }
                else if(data_u1[9]==KEY_DOU_RIGHT_UP)
                {
                    ESP_LOGI(TAG, "KEY_DOU_RIGHT_UP");
                }                
                else if(data_u1[9]==KEY_DOU_RIGHT_DOWN)
                {
                    ESP_LOGI(TAG, "KEY_DOU_RIGHT_DOWN");
                }
                else if(data_u1[9]==KEY_RELEASE)
                {
                    ESP_LOGI(TAG, "KEY_RELEASE");
                }
                return data_u1[9]; 
            }
            else
            {
                 ESP_LOGE(TAG, "Key ID Refuse,ID=%02x-%02x-%02x-%02x",data_u1[3],data_u1[4],data_u1[5],data_u1[6]);
                 ESP_LOGE(TAG, "Key value=%02x",data_u1[9]);
                 return   -1;     
            }
        }                    
    }
    return   -1;       
}



static void Wallkey_Read_Task(void* arg)
{
    uint8_t key_id;
    while(1)
    {
        key_id=Wallkey_Read();
        if((key_id==KEY_DOU_LEFT_UP)&&(Up_Light_Status==1))
        {
            Led_UP_W(0,OFF_TIME);
            Led_UP_Y(0,OFF_TIME);
            //vTaskDelay(5000 / portTICK_RATE_MS);
            Up_Light_Status=0;
        }
        else if((key_id==KEY_DOU_LEFT_UP)&&(Up_Light_Status==0))
        {
            Up_Light_Status=1;
            temp_hour=-1;
            //vTaskDelay(10000 / portTICK_RATE_MS);
        }
        else if((key_id==KEY_DOU_LEFT_DOWN)&&(Down_Light_Status==1))
        {
           Led_DOWN_W(0,OFF_TIME);
           Led_DOWN_Y(0,OFF_TIME);
           Down_Light_Status=0;
        }
        else if((key_id==KEY_DOU_LEFT_DOWN)&&(Down_Light_Status==0))
        {
           Down_Light_Status=1;
           temp_hour=-1;
        }

        vTaskDelay(10 / portTICK_RATE_MS);
    }  
}



