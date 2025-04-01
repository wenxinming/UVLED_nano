/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-03-24     30802       the first version
 */
#include <rtthread.h>
#include <board.h>
#include <channel.h>
#include <lcd.h>
rt_uint16_t voltage;
//#define SDO_PIN GET_PIN(B, 13) //定义输出开关
#define SDO HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15)
#define SDO_H HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15,1)
#define SDO_L HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15,0)

//#define SCLK_PIN GET_PIN(B, 14) //定义输出开关
#define SCLK_H HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10,1)
#define SCLK_L HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10,0)
float Temp1,Temp2;//通道1温度 通道2温度
rt_uint8_t read_temp_running;//照射中温度更新
rt_uint8_t read_temp_watch;//温度设置中刷新
rt_thread_t read_temp;
extern void ftoa(float num,char * buff);
void SDOR()
{
   //rt_pin_mode(SDO_PIN, PIN_MODE_INPUT);

    GPIO_InitTypeDef gpio_init_struct;
    gpio_init_struct.Pin = GPIO_PIN_15;                   /* LED0òy?? */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;            /* í?íìê?3? */
    gpio_init_struct.Pull = GPIO_NOPULL;                    /* é?à- */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);       /* 3?ê??ˉLED0òy?? */
}
void SDOW()
{
    //rt_pin_mode(SDO_PIN, PIN_MODE_OUTPUT);
    GPIO_InitTypeDef gpio_init_struct;
    gpio_init_struct.Pin = GPIO_PIN_15;                   /* LED0òy?? */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* í?íìê?3? */
    gpio_init_struct.Pull = GPIO_NOPULL;                    /* é?à- */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);       /* 3?ê??ˉLED0òy?? */
}
//配置CS1237芯片
void Con_CS1238(unsigned char ch)
{
    unsigned char i;
    unsigned char dat=0;
    unsigned char count_i=0;//溢出计时器

    if(ch == 0)              //选择通道A
        dat = 0x40;//0x40;// 0100 1000
    else if(ch == 1)         //选择通道B
        dat = 0x41;//0x41;// 0100 1000
    SCLK_L;//SCK_0;//时钟拉低
    rt_thread_mdelay(10);
    SDOR();
    while(SDO)//芯片准备好数据输出  时钟已经为0，数据也需要等CS1237全部拉低为0才算都准备好
    {
        rt_thread_mdelay(5);
        count_i++;
        if(count_i > 150)
        {
            SCLK_H;//SCK_1;
            SDOW();
            SDO_H;//DAT_1;
            return;//超时，则直接退出程序
        }
    }
    for(i=0;i<29;i++)// 1 - 26
    {
//      One_CLK;
        SCLK_H;//SCK_1;
        rt_hw_us_delay(5);
        SCLK_L;//;SCK_0;
        rt_hw_us_delay(5);
    }
    SDOW();
    SCLK_H;rt_hw_us_delay(5);SDO_H;SCLK_L;rt_hw_us_delay(5);//30
    SCLK_H;rt_hw_us_delay(5);SDO_H;SCLK_L;rt_hw_us_delay(5);//31
    SCLK_H;rt_hw_us_delay(5);SDO_L;SCLK_L;rt_hw_us_delay(5);//32
    SCLK_H;rt_hw_us_delay(5);SDO_L;SCLK_L;rt_hw_us_delay(5);//33
    SCLK_H;rt_hw_us_delay(5);SDO_H;SCLK_L;rt_hw_us_delay(5);//34
    SCLK_H;rt_hw_us_delay(5);SDO_L;SCLK_L;rt_hw_us_delay(5);//35
    SCLK_H;rt_hw_us_delay(5);SDO_H;SCLK_L;rt_hw_us_delay(5);//36
    SCLK_H;rt_hw_us_delay(5);SCLK_L;
    rt_hw_us_delay(5);//37     写入了0x65
    for(i=0;i<8;i++)// 38 - 45个脉冲了，写8位数据
    {
        SCLK_H;
        if(dat&0x80)
            SDO_H;
        else
            SDO_L;
        dat <<= 1;
        SCLK_L;
    }
    SDOR();
    SCLK_H;rt_hw_us_delay(5);SCLK_L;rt_hw_us_delay(5);//46个脉冲拉高数据引脚
}

//读取ADC数据，返回的是一个有符号数据
unsigned long Read_CS1238(void)
{
    volatile unsigned char i;
    volatile unsigned long dat=0;//读取到的数据
    volatile unsigned char count_i=0;//溢出计时器
    SDO_H;//端口锁存1，51必备
    SCLK_L;//时钟拉低
    SDOR();
    rt_thread_mdelay(10);
    while(SDO)//芯片准备好数据输出  时钟已经为0，数据也需要等CS1237拉低为0才算都准备好
    {
        rt_thread_mdelay(2);
        count_i++;
        if(count_i > 150)
        {
            SCLK_H;
            SDO_H;
            return 0;//超时，则直接退出程序
        }
    }
    SDO_H;//端口锁存1，51必备
    dat=0;
    SDOR();
    for(i=0;i<24;i++)//获取24位有效转换
    {
        SCLK_H;
        rt_hw_us_delay(1);
        dat <<= 1;
        if(SDO)
            dat ++;
        SCLK_L;
        rt_hw_us_delay(1);
    }
    for(i=0;i<3;i++)//一共输入27个脉冲
    {
        SCLK_H;rt_hw_us_delay(10);;SCLK_L;rt_hw_us_delay(10);;//46个脉冲拉高数据引脚
    }
    SDOW();
    SDO_H;
    if((dat&0x800000) == 0x800000)  //最高位为1，表示输入为负值
    {
        dat = ~dat;
        dat&=0xFFFFFF;
    }
    else
    {

    }

    return dat;
}
float read_temperature(rt_uint8_t ch)
{
    float tempsensor;
    Con_CS1238(ch);
    rt_thread_mdelay(10);
    tempsensor=(float)Read_CS1238()/8388607.0*1.25/ 0.00042865f;
    if(tempsensor>100)
    {
        tempsensor=0;
    }
    return tempsensor;
}
void read_temp_entry(void *parameter)//温度刷新
{
    rt_uint8_t turn[10];
    rt_memset(&turn, 0, 10);
    while(1)
    {
        Temp1 = read_temperature(0);
        Temp2 = read_temperature(1);
        voltage = adc_get_result_average(0, 10);    /* 获取ADC通道的转换值，10次取平均 */
        if(read_temp_watch)
        {
            ftoa(Temp1,turn);
            UpdataLcdString(3,2,turn);
            ftoa(Temp2,turn);
            UpdataLcdString(3,4,turn);
        }
        if(read_temp_running)
        {
            //UpdataLcdDataFloat(7,3,Temp1);
            //UpdataLcdDataFloat(7,14,Temp2);
            ftoa(Temp1,turn);
            UpdataLcdString(7,3,turn);
            ftoa(Temp2,turn);
            UpdataLcdString(7,14,turn);
        }
        rt_thread_mdelay(1000);
    }
}
void temperature_init()
{
    GPIO_InitTypeDef gpio_init_struct;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_AFIO_REMAP_SWJ_NOJTAG();
    SDOR();
    gpio_init_struct.Pin = GPIO_PIN_10;                   /* LED0òy?? */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* í?íìê?3? */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* é?à- */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC, &gpio_init_struct);       /* 3?ê??ˉLED0òy?? */
    SCLK_H;//时钟拉低
    read_temp = rt_thread_create("read_temp", read_temp_entry, RT_NULL, 512, 20, 10);
    rt_thread_startup(read_temp);//启动线程
}
