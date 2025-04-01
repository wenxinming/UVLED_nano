/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-03-17     30802       the first version
 */
#include <rtthread.h>
#include <board.h>
#include <uart.h>
#include <channel.h>
#include <temperature.h>
#include <gpio.h>
rt_uint8_t now_channel;//当前操作的通道
rt_thread_t lcd_receive_tid;//lcd接收线程
rt_mq_t lcd_command_mq,lcd_command_send_mq;//创建lcd命令消息队列
rt_thread_t lcdcommand,lcdsend;
rt_uint8_t on_shine_page;
//rt_err_t sl;
extern void shine_entry(void *parameter);//lcd命令执行
typedef struct
{
    rt_uint8_t send_uart[24];
    rt_uint8_t len;
} lcd_uart_send;
int Myatoi(const char *str)         //字符串转换成数字的函数
{
    int i = 1;
    int tmp = 0;

    //assert(str!=NULL);             //断言str不能为空指针
    while(*str=='-'||*str=='+'||*str==' ')      //利用while循环来去除干扰字符
    {
        if(*str=='-')
        {
            i = -i;
        }
        str++;
    }
    while(isdigit(*str))     //实现一个将一个字符串转换成数字且遇到非数字字符提前结束
    {
        tmp = tmp*10+(*str-'0');
        str++;
    }

    return tmp*i;
}
void lcd_com_thread_entry(void *parameter)
{
    uart2msg msg;
    rt_uint8_t i,j;
    rt_size_t rx_len;
    rt_uint8_t buf[30];
    while(1)
    {
        rt_memset(&msg, 0, sizeof(msg));
        if(rt_mq_recv(uart2_rx_mq, &msg, sizeof(msg), RT_WAITING_FOREVER)==RT_EOK)
        {
            rx_len = msg.len;
            if((rx_len>10)&&(msg.send_uart[0]==0xee)&&(msg.send_uart[1]==0xb1))//长度大于10 按键按下
            {
                //ee b1 01 02 03 04 05 06 ee b1 01 02 03 04 05 06 ee b1 01 02 03 04
                i=0;
                while(1)
                {
                    j = 0;
                    rt_memset(&buf, 0, sizeof(buf));
                    for(i=i;i<rx_len;i++)
                    {
                        buf[j] = msg.send_uart[i];
                        j++;
                        if((msg.send_uart[i+1]==0xee)&&(msg.send_uart[i+2]==0xb1))
                        {
                            break;
                        }
                    }
                    i++;
                    //msg2.len = j;
                    rt_mq_send(lcd_command_mq, &buf, j);
                    if(i>=rx_len)
                    {
                        break;
                    }
                }
            }
        }
    }
}
void updatarunbutton(rt_uint8_t button_state,rt_uint8_t buttonID,rt_uint8_t page)//更新照射界面按钮
{
    lcd_uart_send buff;
    rt_memset(&buff, 0, sizeof(buff));
    buff.send_uart[0]=0xEE;
    buff.send_uart[1]=0xB1;
    buff.send_uart[2]=0x10;
    buff.send_uart[3]=0x00;
    buff.send_uart[4]=page;
    buff.send_uart[5]=0x00;
    buff.send_uart[6]=buttonID;
    buff.send_uart[7]=button_state;
    buff.send_uart[8]=0xFF;
    buff.send_uart[9]=0xFC;
    buff.send_uart[10]=0xFF;
    buff.send_uart[11]=0xFF;
    buff.len = 12;
    //HAL_UART_Transmit(&huart2, send, len, HAL_MAX_DELAY);
    rt_mq_send(lcd_command_send_mq, &buff, sizeof(buff));

}
void UpdataLcdDataU8(rt_uint8_t page,rt_uint8_t channel,rt_uint16_t num)//¸üÐÂÆÁÄ»ÏÔÊ¾ 8Î»ÊýÖµ
{
    lcd_uart_send buff;
    rt_uint8_t len,i;
    rt_memset(&buff, 0, sizeof(buff));
    rt_uint8_t turn[10];
    buff.send_uart[0]=0xEE;
    buff.send_uart[1]=0xB1;
    buff.send_uart[2]=0x10;
    buff.send_uart[3]=0x00;
    buff.send_uart[4]=page;//µÚ¶þÒ³
    buff.send_uart[5]=0x00;
    buff.send_uart[6]=channel;//Í¨µÀ1ÕÕÉä¹¦ÂÊ
    rt_sprintf(turn,"%d",num);
    len=strlen(turn);
    for(i=0;i<len;i++)
    {
        buff.send_uart[7+i]=turn[i];
    }
    buff.send_uart[7+i+0]=0xFF;
    buff.send_uart[7+i+1]=0xFC;
    buff.send_uart[7+i+2]=0xFF;
    buff.send_uart[7+i+3]=0xFF;
    buff.len = 7+i+1+3;
    //HAL_UART_Transmit(&huart2, send, len, HAL_MAX_DELAY);
    rt_mq_send(lcd_command_send_mq, &buff, sizeof(buff));
}

void updata_control_mode()
{
    updatarunbutton(control_mode.control_mode,0x01,0x01);
    updatarunbutton(control_mode.light_mode,0x02,0x01);
}
void UpdataShineScreen()//更新照射方式界面
{
    UpdataLcdDataU8(2,1,channel1.power);//通道1功率
    UpdataLcdDataU8(2,3,channel2.power);//通道2功率
    UpdataLcdDataU8(2,2,channel1.time);//通道1时间
    UpdataLcdDataU8(2,4,channel2.time);//通道2时间
    UpdataLcdDataU8(2,5,channel1.time_hour);//通道1照射总时间小时
    UpdataLcdDataU8(2,7,channel2.time_hour);//通道2照射总时间小时
    UpdataLcdDataU8(2,6,channel1.time_minutes);//通道1照射总时间分钟
    UpdataLcdDataU8(2,8,channel2.time_minutes);//通道2照射总时间分钟
    UpdataLcdDataU8(2,13,channel1.delay_time);//通道1关闭延时
    UpdataLcdDataU8(2,20,channel2.delay_time);//通道2关闭延时
}
void UpdataSetTemp()//更新设置温度
{
    UpdataLcdDataU8(3,1,channel1.alarm_temperature);
    UpdataLcdDataU8(3,3,channel2.alarm_temperature);
}
void updata_parameter()//照射参数
{
    UpdataLcdDataU8(15,2,channel1.power);//通道1功率
    UpdataLcdDataU8(15,6,channel2.power);//通道2功率
    UpdataLcdDataU8(15,3,channel1.time);//通道1时间
    UpdataLcdDataU8(15,7,channel2.time);//通道2时间
    UpdataLcdDataU8(15,4,channel1.alarm_temperature);//通道1设置温度
    UpdataLcdDataU8(15,8,channel2.alarm_temperature);//通道2设置温度
}
void updata_multistage()//更新多段照射
{
    rt_uint8_t i;
    //通道1
    for(i=0;i<10;i++)
    {
        UpdataLcdDataU8(31,i+1,channel1.multistage_power[i]);
    }
    for(i=0;i<10;i++)
    {
        UpdataLcdDataU8(31,i+11,channel1.multistage_time[i]);
    }
    for(i=0;i<10;i++)
    {
        UpdataLcdDataU8(31,i+0x30,channel2.multistage_power[i]);
    }
    for(i=0;i<10;i++)
    {
        UpdataLcdDataU8(31,i+0x3A,channel2.multistage_time[i]);
    }
    UpdataLcdDataU8(31,0x20,control_mode.cycle);
}
void switch_show(rt_int8_t page)//切换画面
{
    lcd_uart_send buff;
    rt_uint8_t len;
    rt_memset(&buff, 0, sizeof(buff));
    buff.send_uart[0]=0xEE;
    buff.send_uart[1]=0xB1;
    buff.send_uart[2]=0x00;
    buff.send_uart[3]=0x00;
    buff.send_uart[4]=page;
    buff.send_uart[5]=0xFF;
    buff.send_uart[6]=0xFC;
    buff.send_uart[7]=0xFF;
    buff.send_uart[8]=0xFF;
    buff.len = 9;
    //HAL_UART_Transmit(&huart2, send, len, HAL_MAX_DELAY);
    rt_mq_send(lcd_command_send_mq, &buff, sizeof(buff));
}
void UpdataLcdString(uint8_t page,uint8_t channel,uint8_t s[])//更新液晶屏字符串
{
    lcd_uart_send buff;
    rt_uint8_t len,i;
    rt_memset(&buff, 0, sizeof(buff));
    buff.send_uart[0]=0xEE;
    buff.send_uart[1]=0xB1;
    buff.send_uart[2]=0x10;
    buff.send_uart[3]=0x00;
    buff.send_uart[4]=page;
    buff.send_uart[5]=0x00;
    buff.send_uart[6]=channel;
    len=strlen(s);
    for(i=0;i<len;i++)
    {
        buff.send_uart[7+i]=s[i];
    }
    buff.send_uart[7+i+0]=0xFF;
    buff.send_uart[7+i+1]=0xFC;
    buff.send_uart[7+i+2]=0xFF;
    buff.send_uart[7+i+3]=0xFF;
    buff.len = 7+i+1+3;
    //HAL_UART_Transmit(&huart2, send, len, HAL_MAX_DELAY);
    rt_mq_send(lcd_command_send_mq, &buff, sizeof(buff));
}
void lcd_command_entry(void *parameter)//lcd命令执行
{
    rt_uint8_t temp[10];
    rt_uint8_t i;
    rt_uint8_t buff[64];
    rt_uint8_t last_state1,last_state2;
    i=0;
    last_state1 = Read_LedOn1;
    last_state2 = Read_LedOn2;
    while(1)
    {
        rt_memset(&buff, 0, sizeof(buff));
        if(Read_LedOn1 != last_state1)//状态变化
        {
            if(Read_LedOn1 == 0)//低电平
            {
                if(on_shine_page == 0)
                {
                    shine_init();//照射初始化
                    read_temp_running = 1;
                    switch_show(7);
                }
            }else {

                   }
            last_state1 = Read_LedOn1;
        }
        if(Read_LedOn2 != last_state2)//状态变化
        {
            if(Read_LedOn2 == 0)//低电平
            {
                if(on_shine_page == 0)
               {
                   shine_init();//照射初始化
                   read_temp_running = 1;
                   switch_show(7);
               }
            }else {

                   }
            last_state2 = Read_LedOn2;
        }
        if(rt_mq_recv(lcd_command_mq, &buff, sizeof(buff), 200)==RT_EOK)
        {
            //rt_device_write(RS485_com, 0, &msg.send_uart, msg.len);
            switch(buff[4])
            {
                case 0://第0页
                    switch(buff[6])
                    {
                        case 1://控制方式
                            updata_control_mode();
                        break;
                        case 2://照射方式
                            UpdataShineScreen();
                        break;
                        case 3://温度监控
                            UpdataSetTemp();
                            read_temp_watch =1;
                        break;
                        case 5://进入照射界面
                            on_shine_page = 1;
                            read_temp_running = 1;
                            shine_init();//照射初始化
                        break;
                        case 6://查看照射参数
                            updata_parameter();
                        break;
                        default:

                        break;
                    }
                break;
                case 1://第1页
                    switch(buff[6])
                    {
                        case 1://通道1控制模式按钮按下
                            control_mode.control_mode = buff[9];
                            save();
                        break;
                        case 2://通道2控制模式按钮按下
                            control_mode.light_mode = buff[9];
                            save();
                        break;
                        case 7://通道1使能
                            //Channel1EN = buf[9];
                            //save();
                        break;
                        case 9://通道2使能
                            //Channel2EN = buf[9];
                            //save();
                        break;
                        case 0x0a://通道1阶梯照射
                            //Channel1mulSwitch = buf[9];
                            //save();
                        break;
                        case 0x0b://通道1阶梯照射
                            //Channel2mulSwitch = buf[9];
                            //save();
                        break;
                        case 0x0c://进入多段设置界面
                            updata_multistage();
                        break;
                        default:
                        break;
                    }
                break;
                case 0x02://照射方式页面
                    i = 0;
                    if((buff[6]!=0x2b)&&(buff[6]!=0x1b)&&(buff[6]!=0x0b))
                    {
                        for(i=0;i<10;i++)//
                        {
                            if(buff[8+i]==0x00)
                            {
                                break;
                            }
                            temp[i]=buff[8+i];
                        }
                    }
                    temp[i]='\0';
                    switch(buff[6])
                    {
                        case 1://通道1照射功率
                            channel1.power = Myatoi(temp);
                            save();
                        break;
                        case 2://通道1照射时间
                            channel1.time = Myatoi(temp);
                            save();
                        break;
                        case 3://通道2照射功率
                            channel2.power = Myatoi(temp);
                            save();
                        break;
                        case 4://通道2照射时间
                            channel2.time = Myatoi(temp);
                            save();
                        break;
                        case 0x0d://通道1延迟关闭时间
                            channel1.delay_time = Myatoi(temp);
                            save();
                        break;
                        case 20://通道2延迟关闭时间
                            channel2.delay_time = Myatoi(temp);
                            save();
                        break;
                        case 0x1b://通道2清空计时按下
                            //channel2.now_time_s = 0;
                            now_channel = 1;
                        break;
                        case 0x0b://通道1清空计时按下
                            //channel1.now_time_s = 0;
                            now_channel = 0;
                        break;
                        default:
                        break;
                    }
                break;
                case 0x03://温度设置页面
                    if(buff[6]!=0x0b)
                    {
                        i = 0;
                        if(buff[6]!=0x2b)
                        {
                            for(i=0;i<10;i++)//
                            {
                                if(buff[8+i]==0x00)
                                {
                                    break;
                                }
                                temp[i]=buff[8+i];
                            }
                        }
                        temp[i]='\0';
                        if(buff[6] == 1)//通道1温度
                        {
                            channel1.alarm_temperature = Myatoi(temp);
                            save();
                        }
                        if(buff[6] == 3)//通道2温度
                        {
                            channel2.alarm_temperature = Myatoi(temp);
                            save();
                        }
                    }else {
                        read_temp_watch =0;
                    }
                break;
                case 5://恢复出厂
                    if(buff[6]==0x01)
                    {
                        switch_show(16);
                        channel1.alarm_temperature = 65;
                        channel2.alarm_temperature = 65;
                        channel1.power = 80;
                        channel2.power = 80;
                        channel1.time = 60;
                        channel2.time = 60;
                        control_mode.control_mode = 0;
                        //Channel2CtrlMode=0;
                        save();//保存设置
                        rt_thread_mdelay(1000);
                        switch_show(17);
                    }
                break;
                case 0x1d://跳转恢复出厂界面
                    if(buff[6]==0x01)
                    {
                        switch_show(4);
                    }
                break;
                case 0x06://清空通道1照射时间
                    if(buff[6]==0x01)//密码正确
                    {
                        if(now_channel==0)//通道1
                        {
                            channel1.time_hour = 0;
                            channel1.time_minutes = 0;
                            save();
                        }else {//通道2
                            channel2.time_hour = 0;
                            channel2.time_minutes = 0;
                            save();
                        }
                    }
                break;
                case 7://照射界面
                    if(buff[6]==0x01)//通道1开关
                    {
                        if(buff[9]==0x01)//通道1开启
                        {
                            channel1.status = 1;//通道1开启
                            updatarunbutton(1,1,7);
                            /*
                            strat_led1 = 1;
                            updatarunbutton(1,1);
                            if(English)
                            {
                                UpdataLcdString(7,17,"Open the light");
                            }else
                                    {
                                        UpdataLcdString(7,17,"开启照射");
                                    }
                            rt_pwm_set(pwm_dev_c1, PWM_DEV_C1, PWM_Period, 0);//2000HZ
                            rt_pwm_enable(pwm_dev_c1, PWM_DEV_C1);*/
                        }else
                            {

                                //strat_led1 = 0;
                                updatarunbutton(0,1,7);
                                channel1.status = 0;//通道1开启
                            }
                    }
                    if(buff[6]==0x15)//返回
                    {
                        read_temp_running = 0;
                        updatarunbutton(0,1,7);
                        updatarunbutton(1,0x10,7);
                    }


                    if(buff[6]==0x10)//通道2开关
                    {
                        if(buff[9]==0x01)//通道1开启
                        {
                            channel2.status = 1;//通道1开启
                            updatarunbutton(1,0x10,7);
                        }else
                            {
                                channel2.status = 0;//通道1开启
                                updatarunbutton(0,0x10,7);
                            }
                    }
                break;
                case 0x1f://通道1多端设置
                    i = 0;
                    if(buff[6]!=0x2b)
                    {
                        for(i=0;i<10;i++)//
                        {
                            if(buff[8+i]==0x00)
                            {
                                break;
                            }
                            temp[i]=buff[8+i];
                        }
                    }
                    temp[i]='\0';
                    switch(buff[6])
                    {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                        case 4:
                        case 5:
                        case 6:
                        case 7:
                        case 8:
                        case 9:
                        case 10:
                            channel1.multistage_power[buff[6]-1]=Myatoi(temp);
                            save();
                        break;
                        case 11:
                        case 12:
                        case 13:
                        case 14:
                        case 15:
                        case 16:
                        case 17:
                        case 18:
                        case 19:
                        case 20:
                            channel1.multistage_time[buff[6]-11]=Myatoi(temp);
                            save();
                        break;
                        case 0x20:
                            control_mode.cycle=Myatoi(temp);
                            save();
                        break;
                        case 0x30:
                        case 0x31:
                        case 0x32:
                        case 0x33:
                        case 0x34:
                        case 0x35:
                        case 0x36:
                        case 0x37:
                        case 0x38:
                        case 0x39:
                            channel2.multistage_power[buff[6]-0x30]=Myatoi(temp);
                            save();
                        break;
                        case 0x3A:
                        case 0x3B:
                        case 0x3C:
                        case 0x3D:
                        case 0x3E:
                        case 0x3F:
                        case 0x40:
                        case 0x41:
                        case 0x42:
                        case 0x43:
                            channel2.multistage_time[buff[6]-0x3A]=Myatoi(temp);
                            save();
                        break;
                        default:
                        break;
                    }
                break;
                case 30://温度校准
                    if((buff[6]==2)||buff[6]==4)
                    {
                        if(buff[7]==0x11)//液晶屏发来设置数值
                        {
                            for(i=0;i<10;i++)//
                            {
                                if(buff[8+i]==0x00)
                                {
                                    break;
                                }
                                temp[i]=buff[8+i];
                            }
                            temp[i]='\0';
                            if(buff[6]==2)
                            {
                                //calibration1=read_temperature(0)/Myatoi(temp);
                            }else {
                                //calibration2=read_temperature(1)/Myatoi(temp);
                            }
                            save();//保存设置
                        }

                    }
                break;
                default:
                break;
            }
        }
    }
}
void lcd_command_send_entry(void *parameter)//lcd命令执行
{
    lcd_uart_send lcdsendbuf;
    while(1)
    {
        rt_memset(&lcdsendbuf, 0, sizeof(lcdsendbuf));
        if(rt_mq_recv(lcd_command_send_mq, &lcdsendbuf, sizeof(lcdsendbuf), RT_WAITING_FOREVER)==RT_EOK)
        {
            HAL_UART_Transmit(&huart2, lcdsendbuf.send_uart, lcdsendbuf.len, HAL_MAX_DELAY);
        }
    }
}
void lcd_init()
{
    lcd_receive_tid = rt_thread_create("lcd_receive", lcd_com_thread_entry, RT_NULL, 1024, 15, 10);
    rt_thread_startup(lcd_receive_tid);//启动线程
    lcd_command_mq = rt_mq_create("lcd_commandsend_mq", 30, 48, RT_IPC_FLAG_FIFO);
    lcd_command_send_mq = rt_mq_create("lcd_commandsend_mq", 32, 48, RT_IPC_FLAG_FIFO);
    lcdcommand = rt_thread_create("lcd_command", lcd_command_entry, RT_NULL, 1024, 20, 10);
    lcdsend = rt_thread_create("lcd_send", lcd_command_send_entry, RT_NULL, 1024, 20, 10);
    rt_thread_startup(lcdcommand);//启动线程
    rt_thread_startup(lcdsend);//启动线程
    MX_USART2_UART_Init();//LCD串口初始化
    switch_show(0);
}
