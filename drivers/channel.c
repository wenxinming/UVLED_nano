/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-03-18     ThinkPad       the first version
 */
#include "stm32f1xx_hal.h"
#include <rtthread.h>
#include <string.h>
#include <uart.h>
#include <channel.h>
struct channel channel1;
struct channel channel2;
struct channel_control control_mode;
rt_uint8_t k;
void channel_init()
{
    rt_uint8_t i;
    channel1.power = (*(unsigned int*)(0x0800fc00+4));
    channel1.time = (*(unsigned int*)(0x0800fc00+8));
    channel1.delay_time = (*(unsigned int*)(0x0800fc00+42));
    channel1.alarm_temperature = (*(unsigned int*)(0x0800fc00+24));
    for(i=0;i<10;i++)
    {
        channel1.multistage_time[i] = (*(unsigned int*)(0x0800fc00+54+i*2));
    }
    for(i=0;i<10;i++)
    {
        channel1.multistage_power[i] = (*(unsigned int*)(0x0800fc00+94+i*2));
    }
    channel1.time_hour = (*(unsigned int*)(0x0800fc00+32));
    channel1.time_minutes = (*(unsigned int*)(0x0800fc00+36));
    channel1.status = 0;
    channel1.now_time_s = 0;

    channel2.power = (*(unsigned int*)(0x0800fc00+6));
    channel2.time = (*(unsigned int*)(0x0800fc00+10));
    channel2.delay_time = (*(unsigned int*)(0x0800fc00+44));
    channel2.alarm_temperature =(*(unsigned int*)(0x0800fc00+26));
    for(i=0;i<10;i++)
    {
        channel2.multistage_time[i] = (*(unsigned int*)(0x0800fc00+74+i*2));
    }
    for(i=0;i<10;i++)
    {
        channel2.multistage_power[i] = (*(unsigned int*)(0x0800fc00+114+i*2));
    }
    channel2.time_hour = (*(unsigned int*)(0x0800fc00+34));
    channel2.time_minutes = (*(unsigned int*)(0x0800fc00+38));
    channel2.status = 0;
    channel2.now_time_s = 0;
    control_mode.control_mode = (*(unsigned int*)(0x0800fc00));
    control_mode.light_mode = (*(unsigned int*)(0x0800fc00+134));
    control_mode.cycle = (*(unsigned int*)(0x0800fc00+136));
    channel1.time_seconds = (*(unsigned int*)(0x0800fc00+20));
    channel2.time_seconds = (*(unsigned int*)(0x0800fc00+22));
}
