/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-03-26     30802       the first version
 */
#ifndef DRIVERS_GPIO_H_
#define DRIVERS_GPIO_H_

//#define Light1_ON_OFF_PIN GET_PIN(A, 4) //�����������
//#define Light2_ON_OFF_PIN GET_PIN(A, 5) //�����������

#define PIN_HIGH 1
#define PIN_LOW 0
#define EN1_OFF HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10,PIN_HIGH)
#define EN1_ON HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10,PIN_LOW)
#define EN2_OFF HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11,PIN_HIGH)
#define EN2_ON HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11,PIN_LOW)

#define Light2_ON HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7, PIN_HIGH)
#define Light2_OFF HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7, PIN_LOW)
#define Light1_ON HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, PIN_HIGH)
#define Light1_OFF HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9, PIN_LOW)

#define Complete2_ON HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8, PIN_HIGH)
#define Complete2_OFF HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8, PIN_LOW)
#define Complete1_ON HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9, PIN_HIGH)
#define Complete1_OFF HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9, PIN_LOW)

#define Alarm2_ON HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8, PIN_LOW)
#define Alarm2_OFF HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8, PIN_HIGH)
#define Alarm1_ON HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13, PIN_LOW)
#define Alarm1_OFF HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13, PIN_HIGH)

#define BEEP_ON HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6, PIN_HIGH)
#define BEEP_OFF HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6, PIN_LOW)

#define Read_CheckLed1 HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_12)
#define Read_CheckLed2 HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_11)
#define Read_LedOn2 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4)
#define Read_LedOn1 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5)

#endif /* DRIVERS_GPIO_H_ */
