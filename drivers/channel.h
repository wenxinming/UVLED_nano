/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-03-18     30802       the first version
 */
#ifndef DRIVERS_CHANNEL_H_
#define DRIVERS_CHANNEL_H_
extern struct channel channel1;
extern struct channel channel2;
extern struct channel_control control_mode;
struct channel
{
    rt_uint8_t power;//功率
    rt_uint16_t time;//时间
    rt_uint16_t delay_time;//延时关闭时间
    rt_uint8_t alarm_temperature;//报警温度
    rt_uint16_t multistage_time[10];//多段时间
    rt_uint8_t multistage_power[10];//多段功率
    rt_uint16_t time_hour;//运行小时
    rt_uint8_t time_minutes;//运行分钟
    rt_uint8_t time_seconds;
    rt_uint8_t status;//0关闭 1开启
    rt_uint32_t now_time_s;//本次计时单位秒
};
struct channel_control
{
    rt_uint8_t control_mode;//模式 0手动 1自动
    rt_uint8_t light_mode;//0固定 1阶梯
    rt_uint8_t cycle;//多段循环次数
};
#endif /* DRIVERS_CHANNEL_H_ */
