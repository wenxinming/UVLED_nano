/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-03-14     RT-Thread    first version
 */

#include <rtthread.h>
#include <board.h>
#include <channel.h>
#include <stdio.h>
#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#define UART_NAME "uart2"
#include <lcd.h>

uint32_t uuu;
void ftoa(float num,char * buff)
{
    rt_uint8_t s1[10];
    rt_uint8_t s2[10];
    rt_memset(&s1, 0, 10);
    rt_memset(&s2, 0, 10);
    int integer = (int)num;
    int decimal = (int)((num - integer) * 10);
    itoa(integer,s1,10);
    itoa(decimal,s2,10);
    strcpy(buff,s1);
    strcat(buff,".");
    strcat(buff,s2);
    //rt_kprintf(str, "%d.%03d", integer, decimal);
}
int main(void)
{
    gpio_init();
    lcd_init();//
    channel_init();//
    temperature_init();
    adc_init();
    pwm_init();
    rs485_init();
    //LOG_D("control mode == %d",control_mode.control_mode);
    //LOG_D("light mode == %f",0.01);
    while (1)
    {
        rt_thread_mdelay(1000);
    }
    return RT_EOK;
}
