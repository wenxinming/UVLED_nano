/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-03-17     30802       the first version
 */
#include "stm32f1xx_hal.h"
#include <rtthread.h>
#include <string.h>
#ifndef UART_H_
#define UART_H_
extern void MX_USART2_UART_Init(void);
extern UART_HandleTypeDef huart2;
// USART2初始化函数
extern rt_uint8_t lcd_rx_buf[64];
extern rt_mq_t uart2_rx_mq;//创建串口2消息队列
typedef struct
{
    rt_uint8_t send_uart[64];
    rt_uint8_t len;
} uart2msg;
#endif /* DRIVERS_UART_H_ */
