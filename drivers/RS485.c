/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-12-11     30802       the first version
 */
#include "stm32f1xx_hal.h"
#include <rtthread.h>
#include <string.h>
#include <uart.h>
#include <channel.h>
#define DBG_TAG "uart"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
rt_thread_t rs485_tid;//lcd接收线程
extern UART_HandleTypeDef huart2,huart1;
rt_uint8_t address = 1;//定义本机
#define REG_HOLDING_NREGS 30 //寄存器个数
unsigned char M_CRC[2];//定义数组
rt_uint8_t usart3sendbuf[50];
rt_uint8_t usRegHoldingBuf[30];//定义寄存器
extern struct channel_control control_mode;
extern rt_mq_t uart2_rx_mq,uart1_rx_mq;
extern float Temp1,Temp2;
void crc16(unsigned char *ptr,unsigned int len)//计算通信校验位
{
    unsigned long wcrc=0XFFFF;//预置16位crc寄存器，初值全部为1
    unsigned char temp;//定义中间变量
    int i=0,j=0;//定义计数
    for(i=0;i<len;i++)//循环计算每个数据
    {
        temp=*ptr&0X00FF;//将八位数据与crc寄存器亦或
        ptr++;//指针地址增加，指向下个数据
        wcrc^=temp;//将数据存入crc寄存器
        for(j=0;j<8;j++)//循环计算数据的
        {
            if(wcrc&0X0001)//判断右移出的是不是1，如果是1则与多项式进行异或。
            {
                wcrc>>=1;//先将数据右移一位
                wcrc^=0XA001;//与上面的多项式进行异或
            }
            else//如果不是1，则直接移出
            {
                wcrc>>=1;//直接移出
            }
        }
    }
    temp=wcrc;//crc的值
    M_CRC[0]=wcrc;//crc的低八位
    M_CRC[1]=wcrc>>8;//crc的高八位
}
void rs485_thread_entry(void *parameter)
{
    rt_uint8_t usart1_buffer[64];
    rt_uint8_t i;
    while(1)
    {
        if(rt_mq_recv(uart1_rx_mq, &usart1_buffer, sizeof(usart1_buffer), RT_WAITING_FOREVER)==RT_EOK)
        {
            if((usart1_buffer[0]==address||usart1_buffer[0]==0)&&(usart1_buffer[1]==0x03||usart1_buffer[1]==0x06))//比较地址码
            {
                crc16(usart1_buffer,6);
                if(usart1_buffer[6]==M_CRC[0]&&usart1_buffer[7]==M_CRC[1])//比较校验码
                {
                    if((usart1_buffer[2]*256+usart1_buffer[3])<REG_HOLDING_NREGS)//比较寄存器地址不能超过寄存器数量
                    {
                        if(usart1_buffer[1]==0x03)//读
                        {

                            usRegHoldingBuf[1]=channel1.power;//读取功率
                            usRegHoldingBuf[2]=control_mode.control_mode;//读取控制模式
                            usRegHoldingBuf[3]=channel1.time;//读取照射时间
                            usRegHoldingBuf[4]=channel1.alarm_temperature;//报警温度
                            usRegHoldingBuf[5]=Temp1;//读取温度
                            //usRegHoldingBuf[7]=Channel1EN;//通道1使能
                            usRegHoldingBuf[9]=channel1.time_hour;//通道1总计时小时
                            usRegHoldingBuf[10]=channel1.time_minutes;//通道1总计时分钟

                            usRegHoldingBuf[11]=channel2.power;//读取功率
                            usRegHoldingBuf[12]=control_mode.control_mode;//读取控制模式
                            usRegHoldingBuf[13]=channel2.time;//读取照射时间
                            usRegHoldingBuf[14]=channel2.alarm_temperature;//报警温度
                            usRegHoldingBuf[15]=Temp2;//读取温度
                            //usRegHoldingBuf[17]=Channel2EN;//通道1使能
                            usRegHoldingBuf[18]=channel2.time_hour;//通道1总计时小时
                            usRegHoldingBuf[19]=channel2.time_minutes;//通道1总计时分钟


                            //data
                            usart3sendbuf[0]=usart1_buffer[0];//填充发送字节
                            usart3sendbuf[1]=usart1_buffer[1];//填充
                            usart3sendbuf[2]=usart1_buffer[5]*2;//填充
                            for(i=0;i<usart1_buffer[5];i++)//按协议回复数据
                            {
                                usart3sendbuf[3+i*2]=usRegHoldingBuf[usart1_buffer[3]+i]>>8;
                                usart3sendbuf[3+i*2+1]=usRegHoldingBuf[usart1_buffer[3]+i];
                            }
                            crc16(usart3sendbuf,usart1_buffer[5]*2+3);//计算CRC
                            usart3sendbuf[usart1_buffer[5]*2+3+0]=M_CRC[0];//填充CRC
                            usart3sendbuf[usart1_buffer[5]*2+3+1]=M_CRC[1];//填充CRC
                            HAL_UART_Transmit(&huart1, usart3sendbuf, usart1_buffer[5]*2+3+2, HAL_MAX_DELAY);
                        }else if(usart1_buffer[1]==0x06)//写入
                                    {
                                        usRegHoldingBuf[usart1_buffer[3]]=usart1_buffer[4]*256+usart1_buffer[5];//提取写入位置
                                        memcpy(usart3sendbuf,usart1_buffer,8);//数据回复拷贝
                                        HAL_UART_Transmit(&huart1, usart3sendbuf, usart1_buffer[5]*2+3+2, HAL_MAX_DELAY);
                                        switch(usart1_buffer[3])//选择写入得寄存器
                                        {
                                            case 0://寄存器0
                                            address=usRegHoldingBuf[0];//修改地址
                                            save();
                                            break;
                                            case 1://寄存器1
                                            channel1.power=usRegHoldingBuf[1];//通道1功率
                                            save();
                                            break;
                                            case 2://寄存器2
                                            control_mode.control_mode = usRegHoldingBuf[2];//通道1功率
                                            save();
                                            break;
                                            case 3://set Save countdown time//时间设置
                                            channel1.time=usRegHoldingBuf[3];//读取照射时间
                                            save();
                                            break;
                                            case 4:
                                            channel1.alarm_temperature=usRegHoldingBuf[4];//读取设置温度
                                            save();
                                            break;


                                            case 7:
                                            if(usRegHoldingBuf[7]==1)
                                            {
                                                //shine2();//照射模式
                                            }
                                            break;
                                            case 8:

                                            break;
                                            case 9:
                                            channel1.time_hour=usRegHoldingBuf[9];//设置总时长小时
                                            save();
                                            break;
                                            case 10:
                                            channel1.time_minutes=usRegHoldingBuf[10];//设置总时长小时
                                            save();
                                            break;

                                            case 11:
                                            channel2.power=usRegHoldingBuf[11];//通道2功率
                                            save();
                                            break;

                                            case 12:
                                            control_mode.control_mode=usRegHoldingBuf[12];//通道2功率
                                            save();
                                            break;

                                            case 13:
                                            channel2.time=usRegHoldingBuf[13];//通道2功率
                                            save();
                                            break;

                                            case 14:
                                            channel2.alarm_temperature=usRegHoldingBuf[14];//通道2功率
                                            save();
                                            break;
                                            case 17:
                                            if(usRegHoldingBuf[17]==1)
                                            {
                                                //shine2();//照射模式
                                            }
                                            break;
                                            case 18:
                                            channel2.power = usRegHoldingBuf[18];//通道2功率
                                            save();
                                            break;

                                            case 19:
                                            channel2.time_minutes=usRegHoldingBuf[19];//通道2功率
                                            save();
                                            break;

                                            default:
                                            break;
                                        }

                                    }
                    }
                }
            }
        }else {
            rt_thread_mdelay(100);
        }
    }
}
void rs485_init()
{
    rs485_tid = rt_thread_create("rs485_receive", rs485_thread_entry, RT_NULL, 1024, 15, 10);
    rt_thread_startup(rs485_tid);//启动线程
    MX_USART1_UART_Init();//LCD串口初始化
}

