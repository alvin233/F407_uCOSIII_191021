#include "pwm.h"
#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"

/* dead time setting */
uint16_t DeadTime = 0x20;
/* starting output PWM ratio */
uint32_t ccr_temp = 50;

void TIM1_GPIO_Config(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	/* enable CLK */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOC,ENABLE);
	/* enable PIN's AF function */
	/* TIM1_CH1  PA8 */
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_TIM1);
	/* TIM1_CH1N PE8 */
	/* enable PE8's AF function as the clk source of pwm output negtive side */
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_TIM1);
	/* TIM8_CH1N PA5 */
  /* GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_TIM8); */
	/* TIM8_CH1 PC6 */
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM8);
	/* PIN configuration below share the same function as below, be careful */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;													//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;												//速度
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      											//推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;        											//上拉
	/* enable PC6's GPIO function as the pos-side of the output signal */
	/* PC6 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
#if 0
	/* PE8 */
	/* disable PE8's GPIO function because we use the neg-side of the output signal by connecting to PA8 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
	GPIO_Init(GPIOE,&GPIO_InitStructure);
#endif 
	/* PA8 */
	/* enable PA8's GPIO function as the neg-side of the output signal */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;											//引脚设定 
	GPIO_Init(GPIOA,&GPIO_InitStructure);			

}

void Tim1__Config(void)
{
	u32 TimerPeriod,ccr1;
	TimerPeriod=  (168000000 / Freq ) - 1;  
	ccr1 = TimerPeriod / 2;
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1|RCC_APB2Periph_TIM8,ENABLE);  

	TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
	TIM_TimeBaseInitStructure.TIM_Period = TimerPeriod;
	//计数方式，向上计数
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式
	//这里不用
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;//重复计数器
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //指定时钟分频
	//初始化定时器
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStructure);   //初始化定时器1
	TIM_TimeBaseInit(TIM8,&TIM_TimeBaseInitStructure);   //初始化定时器8


	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;				//指定在运行模式下使用的关闭状态选择
	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;				//指定空闲状态下使用的关闭状态
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;					//指定LOCK级别的参数
	TIM_BDTRInitStructure.TIM_DeadTime = DeadTime;							//指定关闭和输出接通之间的延迟时间  死区时间数值乘时钟周期
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;					//指定是否启用Break输入
	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;		//指定TIM中断引脚极性
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;	//指定是否启用TIM自动输出功能
	TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
	TIM_BDTRConfig(TIM8, &TIM_BDTRInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;          		//选择定时器模式:TIM脉冲宽度调制模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //制定TIM比较输出状态  使能
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; //制定TIM互补输出比较状态  使能
	TIM_OCInitStructure.TIM_Pulse = 50;								//占空比
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;       //输出极性:TIM输出比较极性高
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;      //输出极性:TIM互补输出比较极性高
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;		//指定空闲状态期间的TIM输出比较引脚状态(引脚1)
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;	//指定空闲状态期间的TIM输出比较引脚状态(引脚N)
	TIM_OC1Init(TIM8,&TIM_OCInitStructure);                         //根据T指定的参数初始化外设
	TIM_OC1Init(TIM1,&TIM_OCInitStructure);                         //根据T指定的参数初始化外设
	TIM_OCInitStructure.TIM_Pulse = ccr1;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;				//选择定时器模式:TIM脉冲宽度调制模式2	
	TIM_OC2Init(TIM1,&TIM_OCInitStructure);	
	
	TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_OC2Ref); 			//选择TIMx触发输出模式
	TIM_SelectInputTrigger(TIM8, TIM_TS_ITR0);    					//选择输入触发源  内部触发0
	TIM_SelectSlaveMode(TIM8, TIM_SlaveMode_Reset);     			//选择TIMx从模式
	
	TIM_Cmd(TIM1,ENABLE);                                          //使能TIM1
	TIM_Cmd(TIM8,ENABLE);                                          //使能TIM8
	TIM_CtrlPWMOutputs(TIM1,ENABLE);                               //使能PWM输出
	TIM_CtrlPWMOutputs(TIM8,ENABLE);                               //使能PWM输出
}

void Buck_GPIO_Config(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	/* enable CLK */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	/* PIN configuration below share the same function as below, be careful */
    /* Pin output configure */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;													//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;												//速度
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      											//推挽复用输出
    /* pull up for disable voltage output */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        				
	/* enable PC6's GPIO function as the pos-side of the output signal */
	/* PD4*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
}







