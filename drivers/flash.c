/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-03-21     30802       the first version
 */
#include <rtthread.h>
#include <board.h>
#include <uart.h>
#include <channel.h>
void save()
{
    uint8_t i;
    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef FlashSet;
    FlashSet.TypeErase = FLASH_TYPEERASE_PAGES;
    FlashSet.PageAddress = 0x0800fc00;
    FlashSet.NbPages = 1;
    uint32_t PageError = 0;
    HAL_FLASHEx_Erase(&FlashSet, &PageError);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800fc00, control_mode.control_mode);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800fc00, 1);
    //HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800fc00+2, Channel2CtrlMode);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800fc00+4, channel1.power);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800fc00+6, channel2.power);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800fc00+8, channel1.time);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800fc00+10, channel2.time);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800fc00+42, channel1.delay_time);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800fc00+44, channel2.delay_time);
    //HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800fc00+20, control_mode.control_mode);
    //HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800fc00+22, Channel2CtrlMode);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800fc00+24, channel1.alarm_temperature);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800fc00+26, channel2.alarm_temperature);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800fc00+32, channel1.time_hour);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800fc00+34, channel2.time_hour);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800fc00+36, channel1.time_minutes);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800fc00+38, channel2.time_minutes);
    //HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 0x0800fc00+46, *(volatile uint32_t *)&calibration1);
    //HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 0x0800fc00+50, *(volatile uint32_t *)&calibration2);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800fc00+134, control_mode.light_mode);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800fc00+136, control_mode.cycle);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800fc00+20, channel1.time_seconds);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, 0x0800fc00+22, channel2.time_seconds);
    for(i=0;i<10;i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,0x0800fc00+54+i*2,channel1.multistage_time[i]);
    }
    for(i=0;i<10;i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,0x0800fc00+74+i*2,channel2.multistage_time[i]);
    }
    for(i=0;i<10;i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,0x0800fc00+94+i*2,channel1.multistage_power[i]);
    }
    for(i=0;i<10;i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,0x0800fc00+114+i*2,channel2.multistage_power[i]);
    }
    HAL_FLASH_Lock();
}
