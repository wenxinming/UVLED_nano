/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-03-18     30802       the first version
 */
#include <rtthread.h>
#include <board.h>
#include <uart.h>
#include <channel.h>
#include <temperature.h>
#include <gpio.h>
rt_thread_t shine,l_off1,l_off2;
extern rt_uint8_t on_shine_page;


void light_relay_delay_off1(void *parameter)
{
    rt_thread_mdelay(channel1.delay_time*1000);
    Light1_OFF;
}
void light_relay_delay_off2(void *parameter)
{
    rt_thread_mdelay(channel2.delay_time*1000);
    Light2_OFF;
}
void LightOFF1()
{
    l_off1 = rt_thread_create("lightoff1", light_relay_delay_off1, RT_NULL, 1024, 20, 10);
    rt_thread_startup(l_off1);//�����߳�
}
void LightOFF2()
{
    l_off2 = rt_thread_create("lightoff2", light_relay_delay_off2, RT_NULL, 1024, 20, 10);
    rt_thread_startup(l_off2);//�����߳�
}
void LightON1()
{
    if(l_off1!=0)
    rt_thread_delete(l_off1);
    Light1_ON;
}
void LightON2()
{
    if(l_off2!=0)
    rt_thread_delete(l_off2);
    Light2_ON;
}
void shine_entry_manual(void *parameter)//manual mode
{
    UpdataLcdDataU8(7,2,channel1.power);
    UpdataLcdDataU8(7,10,channel2.power);
    rt_uint8_t channel1_old_status = 0;
    rt_uint8_t channel2_old_status = 0;
    rt_uint32_t all_time1,all_time2;
    rt_uint8_t last_state1,last_state2;//�ⲿ����IO״̬
    BEEP_OFF;
    Alarm1_OFF;
    Alarm2_OFF;
    Light1_OFF;
    Light2_OFF;
    last_state1 = Read_LedOn1;
    last_state2 = Read_LedOn2;
    Complete1_OFF;
    Complete2_OFF;
    while(1)
    {
        /*
         *
        rt_thread_mdelay(1000);
        if(on_shine_page)//Exposure interface
        {
            if(Channel1mulSwitch==0)//Fixed irradiation
            {
                now_power1 = Channel1Power;
                now_power2 = Channel2Power;
            }else {//Step irradiation
                now_power1 = Channel1PowerSet[0];
                now_power2 = Channel2PowerSet[0];
            }
            UpdataLcdDataU8(7,2,now_power1);//channel1 power
            UpdataLcdDataU8(7,10,now_power2);//channel2 power
            UpdataLcdDataU8(7,8,0);//channel1 time
            UpdataLcdDataU8(7,12,0);//channel2 time
            strat_led1 = 0;
            strat_led2 = 0;
            updatarunbutton(0,1);
            updatarunbutton(0,0x10);
            while(1)//Enter irradiation cycle
            {

                rt_thread_mdelay(1000);
            }
        }*/
        if(Read_LedOn1 != last_state1)//״̬�仯
        {
            if(Read_LedOn1 == 0)//�͵�ƽ
            {
                if(channel1.status == 1)
                {
                    //channel1.status = 0;
                }else {
                    channel1.status = 1;
                }
            }else {
                    channel1.status = 0;
                   }
            last_state1 = Read_LedOn1;
        }
        if(Read_LedOn2 != last_state2)//״̬�仯
        {
            if(Read_LedOn2 == 0)//�͵�ƽ
            {
                if(channel2.status == 1)
                {

                }else {
                    channel2.status = 1;
                }
            }else {
                    channel2.status = 0;
                   }
            last_state2 = Read_LedOn2;
        }
        if(channel1.status == 1)//channel open
        {

            if(channel1_old_status == 0 )
            {
                channel1.now_time_s = 0;
                UpdataLcdString(7, 17, "��������");
                EN1_ON;
                set_ch1_output(channel1.power);//����PWMռ�ձ�
                ch1_output_on();//ʹ��PWM���
                channel1_old_status = 1;
                LightON1();
            }
            if(Temp1>=channel1.alarm_temperature || Temp1 == 0)//�¶ȱ���
            {
                BEEP_ON;
                Alarm1_ON;
                channel1.status = 0;
            }
            if(channel1.now_time_s>=3)//��鿪���ź�
              {
                  if(Read_CheckLed1==0)//����
                  {
                    BEEP_ON;
                    Alarm1_ON;
                    channel1.status = 0;
                  }
              }
            if(Temp1>=channel1.alarm_temperature || Temp1 == 0 || (channel1.now_time_s>=3 && Read_CheckLed1==0));
            else BEEP_OFF;
            channel1.now_time_s++;
            UpdataLcdDataU8(7,8,channel1.now_time_s);//ͨ��1ʱ��
        }else {
            if(channel1_old_status)
            {
                if(Temp1>=channel1.alarm_temperature || Temp1 == 0)//�¶ȱ���
                {
                    UpdataLcdString(7,17,"�¶ȱ���");
                }else if(Read_CheckLed1==0)//����
                {
                    UpdataLcdString(7, 17, "�����쳣");
                }else {
                    UpdataLcdString(7,17,"�ر�����");
                }
                EN1_OFF;
                set_ch1_output(0);//����PWMռ�ձ�
                ch1_output_off();//ʹ��PWM���
                LightON1();
                LightOFF1();
                channel1_old_status=0;
                all_time1 = (channel1.time_hour*60*60)+(channel1.time_minutes*60)+channel1.time_seconds;
                all_time1 += channel1.now_time_s;
                channel1.time_hour = all_time1/3600;
                channel1.time_minutes = (all_time1%3600)/60;
                channel1.time_seconds = (all_time1%3600)%60;
                updatarunbutton(0,1,7);//ͨ��1��ť
                save();
            }
        }
        if(channel2.status == 1)//ͨ��1����
        {
            if(channel2_old_status == 0 )
            {
                channel2.now_time_s = 0;
                UpdataLcdString(7,19,"��������");
                EN2_ON;
                set_ch2_output(channel2.power);//����PWMռ�ձ�
                ch2_output_on();//ʹ��PWM���
                channel2_old_status = 1;
                LightON2();
            }
            if(Temp2>=channel2.alarm_temperature || Temp2 == 0)//Temp Alarm
            {
                BEEP_ON;
                Alarm2_ON;
                channel2.status = 0;
            }
            if(channel2.now_time_s>=3)//��鿪���ź�
              {
                  if(Read_CheckLed2==0)//����
                  {
                    BEEP_ON;
                    Alarm2_ON;
                    channel2.status = 0;
                  }
              }
            if(Temp2>=channel2.alarm_temperature || Temp2 == 0 || (channel2.now_time_s>=3 && Read_CheckLed2==0));
            else BEEP_OFF;
            channel2.now_time_s++;
            UpdataLcdDataU8(7,12,channel2.now_time_s);//ͨ��1ʱ��
        }else {
            if(channel2_old_status)
            {
                if(Temp2>=channel2.alarm_temperature || Temp2 == 0) //�¶ȱ���
                {
                    UpdataLcdString(7,19,"�¶ȱ���");
                }else if(Read_CheckLed2==0)//����
                {
                    UpdataLcdString(7,19,"�����쳣");
                }else {
                    UpdataLcdString(7,19,"�ر�����");
                }
                EN2_OFF;
                set_ch2_output(0);//����PWMռ�ձ�
                ch2_output_off();//ʹ��PWM���
                LightON2();
                LightOFF2();
                channel2_old_status=0;
                all_time2 = (channel2.time_hour*60*60)+(channel2.time_minutes*60)+channel2.time_seconds;
                all_time2 += channel2.now_time_s;
                channel2.time_hour = all_time2/3600;
                channel2.time_minutes = (all_time2%3600)/60;
                channel2.time_seconds = (all_time2%3600)%60;
                updatarunbutton(0,0x10,7);//ͨ��1��ť
                save();
            }
         }
        if(read_temp_running==0)//�˳�
        {
            UpdataLcdString(7,17,"�ر�����");
            UpdataLcdString(7,19,"�ر�����");
            EN1_OFF;
            EN2_OFF;
            set_ch1_output(0);//����PWMռ�ձ�
            set_ch2_output(0);//����PWMռ�ձ�
            ch1_output_off();//ʹ��PWM���
            ch2_output_off();//ʹ��PWM���
            channel1_old_status=0;
            channel2_old_status=0;
            if(channel1.status)
            {
                all_time1 = (channel1.time_hour*60*60)+(channel1.time_minutes*60)+channel1.time_seconds;
                all_time1 += channel1.now_time_s;
                channel1.time_hour = all_time1/3600;
                channel1.time_minutes = (all_time1%3600)/60;
                channel1.time_seconds = (all_time1%3600)%60;
            }
            if(channel2.status)
            {
                all_time2 = (channel2.time_hour*60*60)+(channel2.time_minutes*60)+channel2.time_seconds;
                all_time2 += channel2.now_time_s;
                channel2.time_hour = all_time2/3600;
                channel2.time_minutes = (all_time2%3600)/60;
                channel2.time_seconds = (all_time2%3600)%60;
            }
            if(channel1.status||channel2.status)
            {
                save();
            }
            Alarm1_OFF;
            Alarm2_OFF;
            LightON1();
            LightOFF1();
            LightON2();
            LightOFF2();
            channel1.status = 0;
            channel2.status = 0;
            BEEP_OFF;
            on_shine_page = 0;
            break;
        }
        rt_thread_mdelay(1000);
    }
    //rt_thread_delete(shine);
}
void shine_entry_auto(void *parameter)//�Զ�ģʽ
{
    UpdataLcdDataU8(7,2,channel1.power);//ͨ��1����
    UpdataLcdDataU8(7,10,channel2.power);//ͨ��1����
    rt_uint8_t channel1_old_status = 0;
    rt_uint8_t channel2_old_status = 0;
    rt_uint32_t all_time1,all_time2;
    rt_uint8_t last_state1,last_state2;//�ⲿ����IO״̬
    Alarm1_OFF;
    Alarm2_OFF;
    Light1_OFF;
    Light2_OFF;
    last_state1 = Read_LedOn1;
    last_state2 = Read_LedOn2;
    Complete1_OFF;
    Complete2_OFF;
    while(1)
    {
        if(Read_LedOn1 != last_state1)//״̬�仯
        {
            if(Read_LedOn1 == 0)//�͵�ƽ
            {
                if(channel1.status == 1)
                {
                    //���¼�ʱ
                    channel1.now_time_s = 0;
                    //channel1.status = 0;
                }else {
                    channel1.status = 1;
                }
            }else {
                   }
            last_state1 = Read_LedOn1;
        }
        if(Read_LedOn2 != last_state2)//״̬�仯
        {
            if(Read_LedOn2 == 0)//�͵�ƽ
            {
                if(channel2.status == 1)
                {
                    //���¼�ʱ
                    channel2.now_time_s = 0;
                    //channel2.status = 0;
                }else {
                    channel2.status = 1;
                }
            }else {

                   }
            last_state2 = Read_LedOn2;
        }
        if(channel1.status == 1)//ͨ��1����
        {
            if(channel1_old_status == 0 )
            {
                Complete1_OFF;
                channel1.now_time_s = 0;
                UpdataLcdString(7,17,"��������");
                EN1_ON;
                set_ch1_output(channel1.power);//����PWMռ�ձ�
                ch1_output_on();//ʹ��PWM���
                channel1_old_status = 1;
                LightON1();
            }
            if(Temp1>=channel1.alarm_temperature || Temp1 == 0)//�¶ȱ���
            {
                BEEP_ON;
                Alarm1_ON;
                channel1.status = 0;
            }
            if(channel1.now_time_s>=3)//��鿪���ź�
              {
                  if(Read_CheckLed1==0)//����
                  {
                    BEEP_ON;
                    Alarm1_ON;
                    channel1.status = 0;
                  }
              }
            if(Temp1>=channel1.alarm_temperature || Temp1 == 0 || (channel1.now_time_s>=3 && Read_CheckLed1==0));
            else BEEP_OFF;
            channel1.now_time_s++;
            if(channel1.now_time_s>=channel1.time)
            {
                channel1.status = 0;
                Complete1_ON;
            }
            UpdataLcdDataU8(7,8,channel1.time-channel1.now_time_s);//ͨ��1ʱ��
        }else {
            if(channel1_old_status)
            {

                if(Temp1>=channel1.alarm_temperature || Temp1 == 0)//�¶ȱ���
                {
                    UpdataLcdString(7,17,"�¶ȱ���");
                }else if(Read_CheckLed1==0)//����
                {
                 UpdataLcdString(7,17,"�����쳣");
                }else {
                    UpdataLcdString(7,17,"�ر�����");
                }
                EN1_OFF;
                set_ch1_output(0);//����PWMռ�ձ�
                ch1_output_off();//ʹ��PWM���
                LightON1();
                LightOFF1();
                channel1_old_status=0;
                all_time1 = (channel1.time_hour*60*60)+(channel1.time_minutes*60)+channel1.time_seconds;
                all_time1 += channel1.now_time_s;
                channel1.time_hour = all_time1/3600;
                channel1.time_minutes = (all_time1%3600)/60;
                channel1.time_seconds = (all_time1%3600)%60;
                updatarunbutton(0,1,7);//ͨ��1��ť
                save();
            }
        }
        if(channel2.status == 1)//ͨ��1����
        {
            if(channel2_old_status == 0 )
            {
                Complete2_OFF;
                channel2.now_time_s = 0;
                UpdataLcdString(7,19,"��������");
                EN2_ON;
                set_ch2_output(channel2.power);//����PWMռ�ձ�
                ch2_output_on();//ʹ��PWM���
                channel2_old_status = 1;
                LightON2();
            }
            channel2.now_time_s++;
            if(Temp2>=channel2.alarm_temperature || Temp2 == 0)//�¶ȱ���
            {
                BEEP_ON;
                Alarm2_ON;
                channel2.status = 0;
            }
            if(channel2.now_time_s>=3)//��鿪���ź�
              {
                  if(Read_CheckLed2==0)//����
                  {
                    BEEP_ON;
                    Alarm2_ON;
                    channel2.status = 0;
                  }
              }
            if(channel2.now_time_s>=channel2.time)
            {
                channel2.status = 0;
                Complete2_ON;
            }
            if(Temp2>=channel2.alarm_temperature || Temp2 == 0 || (channel2.now_time_s>=3 && Read_CheckLed2==0));
            else BEEP_OFF;
            UpdataLcdDataU8(7,12,channel2.time-channel2.now_time_s);//ͨ��1ʱ��
        }else {
            if(channel2_old_status)
            {

                if(Temp2>=channel2.alarm_temperature || Temp2 == 0)//�¶ȱ���
                {
                    UpdataLcdString(7,19,"�¶ȱ���");
                }else if(Read_CheckLed2==0)//����
                  {
                    UpdataLcdString(7,19,"�����쳣");
                  }else {
                      UpdataLcdString(7,19,"�ر�����");
                }
                EN2_OFF;
                set_ch2_output(0);//����PWMռ�ձ�
                ch2_output_off();//ʹ��PWM���
                LightON2();
                LightOFF2();
                channel2_old_status=0;
                all_time2 = (channel2.time_hour*60*60)+(channel2.time_minutes*60)+channel2.time_seconds;
                all_time2 += channel2.now_time_s;
                channel2.time_hour = all_time2/3600;
                channel2.time_minutes = (all_time2%3600)/60;
                channel2.time_seconds = (all_time2%3600)%60;
                updatarunbutton(0,0x10,7);//ͨ��1��ť
                save();
            }
         }
        if(read_temp_running==0)//�˳�
        {
            UpdataLcdString(7,17,"�ر�����");
            UpdataLcdString(7,19,"�ر�����");
            EN1_OFF;
            EN2_OFF;
            set_ch1_output(0);//����PWMռ�ձ�
            set_ch2_output(0);//����PWMռ�ձ�
            ch1_output_off();//ʹ��PWM���
            ch2_output_off();//ʹ��PWM���
            channel1_old_status=0;
            channel2_old_status=0;
            if(channel1.status)
            {
                all_time1 = (channel1.time_hour*60*60)+(channel1.time_minutes*60)+channel1.time_seconds;
                all_time1 += channel1.now_time_s;
                channel1.time_hour = all_time1/3600;
                channel1.time_minutes = (all_time1%3600)/60;
                channel1.time_seconds = (all_time1%3600)%60;
            }
            if(channel2.status)
            {
                all_time2 = (channel2.time_hour*60*60)+(channel2.time_minutes*60)+channel2.time_seconds;
                all_time2 += channel2.now_time_s;
                channel2.time_hour = all_time2/3600;
                channel2.time_minutes = (all_time2%3600)/60;
                channel2.time_seconds = (all_time2%3600)%60;
            }
            if(channel1.status||channel2.status)
            {
                save();
            }
            channel1.status = 0;
            channel2.status = 0;
            Alarm1_OFF;
            Alarm2_OFF;
            LightON1();
            LightOFF1();
            LightON2();
            LightOFF2();
            BEEP_OFF;
            on_shine_page = 0;
            break;
        }
        rt_thread_mdelay(1000);
    }
    //rt_thread_delete(shine);
}
void shine_entry_multistage(void *parameter)//���
{
    UpdataLcdDataU8(7,2,channel1.multistage_power[0]);//ͨ��1����
    UpdataLcdDataU8(7,10,channel2.multistage_power[0]);//ͨ��1����
    rt_uint8_t channel1_old_status = 0;
    rt_uint8_t channel2_old_status = 0;
    rt_uint32_t all_time1,all_time2;
    rt_uint8_t cycle1,cycle2,large_cycle1,large_cycle2;
    rt_uint8_t last_state1,last_state2;//�ⲿ����IO״̬
    BEEP_OFF;
    cycle1 = 0;
    cycle2 = 0;
    large_cycle1 =0;
    large_cycle2 =0;
    Alarm1_OFF;
    Alarm2_OFF;
    Light1_OFF;
    Light2_OFF;
    last_state1 = Read_LedOn1;
    last_state2 = Read_LedOn2;
    Complete1_OFF;
    Complete2_OFF;
    while(1)
    {
        if(Read_LedOn1 != last_state1)//״̬�仯
        {
            if(Read_LedOn1 == 0)//�͵�ƽ
            {
                if(channel1.status == 1)
                {
                    channel1.status = 0;
                }else {
                    channel1.status = 1;
                }
            }else {
                   }
            last_state1 = Read_LedOn1;
        }
        if(Read_LedOn2 != last_state2)//״̬�仯
        {
            if(Read_LedOn2 == 0)//�͵�ƽ
            {
                if(channel2.status == 1)
                {
                    channel2.status = 0;
                }else {
                    channel2.status = 1;
                }
            }else {

                   }
            last_state2 = Read_LedOn2;
        }
       if(channel1.status == 1)//ͨ��1����
       {
           if(channel1_old_status == 0 )
           {
               channel1.now_time_s = 0;
               UpdataLcdString(7,17,"��������");
               EN1_ON;
               set_ch1_output(channel1.multistage_power[cycle1]);//����PWMռ�ձ�
               UpdataLcdDataU8(7,2,channel1.multistage_power[cycle1]);//ͨ��1����
               ch1_output_on();//ʹ��PWM���
               channel1_old_status = 1;
               LightON1();
               cycle1=0;
               large_cycle1 = 0;
           }
           if(Temp1>=channel1.alarm_temperature || Temp1 == 0)//�¶ȱ���
           {
               BEEP_ON;
               Alarm1_ON;
               channel1.status = 0;
           }
           if(channel1.now_time_s>=3)//��鿪���ź�
            {
                 if(Read_CheckLed1==0)//����
                 {
                   BEEP_ON;
                   Alarm1_ON;
                   channel1.status = 0;
                 }
            }
           if(Temp1>=channel1.alarm_temperature || Temp1 == 0 || (channel1.now_time_s>=3 && Read_CheckLed1==0));
           else BEEP_OFF;
           channel1.now_time_s++;
           if(channel1.now_time_s>=channel1.multistage_time[cycle1])
           {
               cycle1++;
               all_time1 = (channel1.time_hour*60*60)+(channel1.time_minutes*60)+channel1.time_seconds;
               all_time1 += channel1.now_time_s;
               channel1.time_hour = all_time1/3600;
               channel1.time_minutes = (all_time1%3600)/60;
               channel1.time_seconds = (all_time1%3600)%60;
               channel1.now_time_s=0;
               if(cycle1>9)
               {
                 large_cycle1++;
                 if(large_cycle1>=control_mode.cycle)
                  {
                      channel1.status = 0;
                  }
                  cycle1=0;
               }
               set_ch1_output(channel1.multistage_power[cycle1]);//����PWMռ�ձ�
               UpdataLcdDataU8(7,2,channel1.multistage_power[cycle1]);//ͨ��1����
           }
           if(channel1.multistage_time[cycle1]>=channel1.now_time_s)
           UpdataLcdDataU8(7,8,channel1.multistage_time[cycle1]-channel1.now_time_s);//ͨ��1ʱ��
       }else {
           if(channel1_old_status)
           {
               UpdataLcdString(7,17,"�ر�����");
               if(Temp1>=channel1.alarm_temperature || Temp1 == 0)//�¶ȱ���
               {
                   UpdataLcdString(7,17,"�¶ȱ���");
               }else if(Read_CheckLed1==0)//����
               {
                   UpdataLcdString(7,17,"�����쳣");
               }else {
                   UpdataLcdString(7,17,"�ر�����");
            }
               EN1_OFF;
               set_ch1_output(0);//����PWMռ�ձ�
               ch1_output_off();//ʹ��PWM���
               LightON1();
               LightOFF1();
               channel1_old_status=0;
               all_time1 = (channel1.time_hour*60*60)+(channel1.time_minutes*60)+channel1.time_seconds;
               all_time1 += channel1.now_time_s;
               channel1.time_hour = all_time1/3600;
               channel1.time_minutes = (all_time1%3600)/60;
               channel1.time_seconds = (all_time1%3600)%60;
               updatarunbutton(0,1,7);//ͨ��1��ť
               save();
           }
       }

       if(channel2.status == 1)//ͨ��1����
      {
          if(channel2_old_status == 0 )
          {
              channel2.now_time_s = 0;
              UpdataLcdString(7,19,"��������");
              EN2_ON;
              set_ch2_output(channel2.multistage_power[cycle2]);//����PWMռ�ձ�
              UpdataLcdDataU8(7,10,channel2.multistage_power[cycle2]);//ͨ��1����
              ch2_output_on();//ʹ��PWM���
              channel2_old_status = 1;
              LightON2();
              cycle2=0;
              large_cycle2 = 0;
          }
          if(Temp2>=channel2.alarm_temperature || Temp2 == 0)//�¶ȱ���
          {
              BEEP_ON;
              Alarm2_ON;
              channel2.status = 0;
          }
          if(channel2.now_time_s>=3)//��鿪���ź�
          {
              if(Read_CheckLed2==0)//����
              {
                BEEP_ON;
                Alarm2_ON;
                channel2.status = 0;
              }
          }
          if(Temp2>=channel2.alarm_temperature || Temp2 == 0 || (channel2.now_time_s>=3 && Read_CheckLed2==0));
          else BEEP_OFF;
          channel2.now_time_s++;
          if(channel2.now_time_s>=channel2.multistage_time[cycle2])
          {
              cycle2++;
              all_time2 = (channel2.time_hour*60*60)+(channel2.time_minutes*60)+channel2.time_seconds;
              all_time2 += channel2.now_time_s;
              channel2.time_hour = all_time2/3600;
              channel2.time_minutes = (all_time2%3600)/60;
              channel2.time_seconds = (all_time2%3600)%60;
              channel2.now_time_s=0;
              if(cycle2>9)
              {
                large_cycle2++;
                if(large_cycle2>=control_mode.cycle)
                 {
                     channel2.status = 0;
                 }
                 cycle2=0;
              }
              set_ch2_output(channel2.multistage_power[cycle2]);//����PWMռ�ձ�
              UpdataLcdDataU8(7,10,channel2.multistage_power[cycle2]);//ͨ��1����
          }
          if(channel2.multistage_time[cycle2]>=channel2.now_time_s)
          UpdataLcdDataU8(7,12,channel2.multistage_time[cycle2]-channel2.now_time_s);//ͨ��1ʱ��
      }else {
          if(channel2_old_status)
          {
              if(Temp2>=channel2.alarm_temperature || Temp2 == 0)//�¶ȱ���
              {
                  UpdataLcdString(7,19,"�¶ȱ���");
              }else if(Read_CheckLed2==0)//����
              {
                UpdataLcdString(7,19,"�����쳣");
              }else {
                  UpdataLcdString(7,19,"�ر�����");
            }
              EN2_OFF;
              set_ch2_output(0);//����PWMռ�ձ�
              ch2_output_off();//ʹ��PWM���
              LightON2();
              LightOFF2();
              channel2_old_status=0;
              all_time2 = (channel2.time_hour*60*60)+(channel2.time_minutes*60)+channel2.time_seconds;
              all_time2 += channel2.now_time_s;
              channel2.time_hour = all_time2/3600;
              channel2.time_minutes = (all_time2%3600)/60;
              channel2.time_seconds = (all_time2%3600)%60;
              updatarunbutton(0,0x10,7);//ͨ��1��ť
              save();
          }
      }
        if(read_temp_running==0)//�˳�
        {
            UpdataLcdString(7,17,"�ر�����");
            UpdataLcdString(7,19,"�ر�����");
            EN1_OFF;
            EN2_OFF;
            set_ch1_output(0);//����PWMռ�ձ�
            set_ch2_output(0);//����PWMռ�ձ�
            ch1_output_off();//ʹ��PWM���
            ch2_output_off();//ʹ��PWM���
            channel1_old_status=0;
            channel2_old_status=0;
            if(channel1.status)
            {
                all_time1 = (channel1.time_hour*60*60)+(channel1.time_minutes*60)+channel1.time_seconds;
                all_time1 += channel1.now_time_s;
                channel1.time_hour = all_time1/3600;
                channel1.time_minutes = (all_time1%3600)/60;
                channel1.time_seconds = (all_time1%3600)%60;
            }
            if(channel2.status)
            {
                all_time2 = (channel2.time_hour*60*60)+(channel2.time_minutes*60)+channel2.time_seconds;
                all_time2 += channel2.now_time_s;
                channel2.time_hour = all_time2/3600;
                channel2.time_minutes = (all_time2%3600)/60;
                channel2.time_seconds = (all_time2%3600)%60;
            }
            if(channel1.status||channel2.status)
            {
                save();
            }
            channel1.status = 0;
            channel2.status = 0;
            Alarm1_OFF;
            Alarm2_OFF;
            LightON1();
            LightOFF1();
            LightON2();
            LightOFF2();
            BEEP_OFF;
            on_shine_page = 0;
            break;
        }
       rt_thread_mdelay(1000);
    }
}
void shine_init()//�����ʼ��
{
    UpdataLcdString(7,17,"");//�ַ�����ʼ��
    UpdataLcdString(7,19,"");//�ַ�����ʼ��
    updatarunbutton(0,1,7);//ͨ��1��ť
    updatarunbutton(0,0x10,7);//ͨ��1��ť
    UpdataLcdDataU8(7,8,0);//ͨ��1ʱ��
    UpdataLcdDataU8(7,12,0);//ͨ��2ʱ��
    on_shine_page = 1;
    if(control_mode.control_mode == 0)//�ֶ�ģʽ
    {
        //UpdataLcdDataU8(7,8,0);//ͨ��1ʱ��
        //UpdataLcdDataU8(7,12,0);//ͨ��2ʱ��
        shine = rt_thread_create("shine", shine_entry_manual, RT_NULL, 1024, 20, 10);
        rt_thread_startup(shine);//�����߳�
    }else {//�Զ�ģʽ

        if(control_mode.light_mode == 0)//�̶�����
        {
            shine = rt_thread_create("shine", shine_entry_auto, RT_NULL, 1024, 20, 10);
            rt_thread_startup(shine);//�����߳�
        }else {//��������
            shine = rt_thread_create("shine", shine_entry_multistage, RT_NULL, 1024, 20, 10);
            rt_thread_startup(shine);//�����߳�
        }
        //UpdataLcdDataU8(7,8,channel1.time);//ͨ��1ʱ��
        //UpdataLcdDataU8(7,12,channel1.time);//ͨ��2ʱ��
    }
}
