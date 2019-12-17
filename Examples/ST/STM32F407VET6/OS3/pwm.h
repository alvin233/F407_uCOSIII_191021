#ifndef _TIMER_H
#define _TIMER_H
#include  "stm32f4xx.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/6/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
#define Sys_Freq (168000000)
#define Freq (150000)
#define Max_Half_Period (Sys_Freq/Freq)

extern uint16_t DeadTime;
extern uint32_t ccr_temp;
#define BUCK(a)	if (a)	\
					GPIO_SetBits(GPIOD,GPIO_Pin_4);\
					else		\
					GPIO_ResetBits(GPIOD,GPIO_Pin_4)
                      
void TIM1_PWM_Init(u32 arr,u32 psc);
void TIM1_GPIO_Config(void);
void Buck_GPIO_Config(void);
void Tim1__Config(void);

#endif
