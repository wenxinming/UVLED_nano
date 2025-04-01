/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-03-17     30802       the first version
 */
#ifndef DRIVERS_LCD_H_
#define DRIVERS_LCD_H_
extern rt_thread_t lcd_receive_tid;//lcd接收线程
extern void lcd_init();

extern rt_mq_t lcd_command_mq;//创建lcd命令消息队列
#endif /* DRIVERS_LCD_H_ */
