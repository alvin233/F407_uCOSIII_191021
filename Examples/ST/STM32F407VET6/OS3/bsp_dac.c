#include "bsp_dac.h"
#include "stm32f4xx_dac.h"
void dac_config (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef  DAC_InitStructure;
	 /* 使能GPIOA时钟 */
  RCC_AHB1PeriphClockCmd(DAC_CH1_GPIO_CLK|DAC_CH2_GPIO_CLK, ENABLE);	
	/* 使能DAC时钟 */	
  RCC_APB1PeriphClockCmd(DAC_CLK, ENABLE);
	/* DAC的GPIO配置，模拟输入 */
  GPIO_InitStructure.GPIO_Pin =  DAC_CH1_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(DAC_CH1_GPIO_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin =  DAC_CH2_GPIO_PIN;
  GPIO_Init(DAC_CH2_GPIO_PORT, &GPIO_InitStructure);
	/* 配置DAC 通道1 */
  DAC_InitStructure.DAC_Trigger = DAC_TRIGGER;						//使用硬件作为触发源就挺好啊
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;	//不使用波形发生器
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;	//不使用DAC输出缓冲
	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_4095;
	//三角波振幅（本实验没有用到，可配置成任意值，但本结构体成员不能为空）
  DAC_Init(DAC_CH1_CHANNEL, &DAC_InitStructure);
  /* 配置DAC 通道2 */
  DAC_Init(DAC_CH2_CHANNEL, &DAC_InitStructure);
	/* 使能DAC 通道1、2 */
  DAC_Cmd(DAC_Channel_1, ENABLE);
  DAC_Cmd(DAC_Channel_2, ENABLE);
	
/*给这两个通道设置一个初值*/
  DAC_SetDualChannelData(DAC_Align_12b_R,1000,2000);

}
