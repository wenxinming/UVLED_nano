/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-03-26     30802       the first version
 */
//#define EN1_ON_OFF_PIN GET_PIN(A, 11) //定义输出� �
//#define EN2_ON_OFF_PIN GET_PIN(A, 12) //定义输出� �
#include <rtthread.h>
#include <board.h>
#include <channel.h>
#include <lcd.h>
#include <gpio.h>
/*
#define EN1_ON HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2,PIN_HIGH)
#define EN1_OFF HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2,PIN_LOW)
#define EN2_ON HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3,PIN_HIGH)
#define EN2_OFF HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3,PIN_LOW)

#define Light1_ON HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7, PIN_HIGH)
#define Light1_OFF HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7, PIN_LOW)
#define Light2_ON HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, PIN_HIGH)
#define Light2_OFF HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, PIN_LOW)

#define BEEP_ON HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6, PIN_HIGH)
#define BEEP_OFF HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6, PIN_LOW)
*/
void gpio_init()
{
    GPIO_InitTypeDef gpio_init_struct;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    gpio_init_struct.Pin = GPIO_PIN_10|GPIO_PIN_11;                   /* LED0引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;            /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_NOPULL;                    /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高�   */
    HAL_GPIO_Init(GPIOB , &gpio_init_struct);       /* 初始化LED0引脚 */
    gpio_init_struct.Pin = GPIO_PIN_2;                   /* LED0引脚 */
    HAL_GPIO_Init(GPIOD, &gpio_init_struct);       /* 初始化LED0引脚 */
    gpio_init_struct.Pin = GPIO_PIN_3;                   /* LED0引脚 */
    HAL_GPIO_Init(GPIOB , &gpio_init_struct);       /* 初始化LED0引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* 推挽输出 */
    gpio_init_struct.Pin = GPIO_PIN_9|GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_8;
    HAL_GPIO_Init(GPIOB, &gpio_init_struct);       /* 初始化LED0引脚 */
    gpio_init_struct.Pin = GPIO_PIN_13|GPIO_PIN_9;
    HAL_GPIO_Init(GPIOC, &gpio_init_struct);       /* 初始化LED0引脚 */
    gpio_init_struct.Pin = GPIO_PIN_8;
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);       /* 初始化LED0引脚 */

    gpio_init_struct.Pin = GPIO_PIN_4|GPIO_PIN_5;                   /* LED0引脚 */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;            /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_NOPULL;                    /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高�   */
    HAL_GPIO_Init(GPIOB , &gpio_init_struct);       /* 初始化LED0引脚 */

    gpio_init_struct.Pin = GPIO_PIN_11|GPIO_PIN_12;                   /* LED0引脚 */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;            /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_NOPULL;                    /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高�   */
    HAL_GPIO_Init(GPIOC , &gpio_init_struct);       /* 初始化LED0引脚 */

    EN1_OFF;
    EN2_OFF;
    Alarm2_OFF;
    Alarm1_OFF;
    BEEP_OFF;
}
