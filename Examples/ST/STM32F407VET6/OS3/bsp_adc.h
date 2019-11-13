#ifndef __BSP_ADC_H
#define	__BSP_ADC_H

#include "stm32f4xx.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dma.h"
extern __IO uint16_t ADC_ConvertedValue[20];
extern __IO uint16_t IVOUT[2];

/* please note that your resistor */
#define  current_static_val (2.5395*2/4)


#define times_1000000_vol_per 806
#define times_1e8_vol_per 80566
#define times_1e1_vol_per 30

#define RHEOSTAT_NOFCHANEL      2

/*=====================ͨ��1 IO======================*/
// ADC IO�궨�壬�ɼ�����
#define RHEOSTAT_ADC_GPIO_PORT1    GPIOA
#define RHEOSTAT_ADC_GPIO_PIN1     GPIO_Pin_0
#define RHEOSTAT_ADC_GPIO_CLK1     RCC_AHB1Periph_GPIOA
#define RHEOSTAT_ADC_CHANNEL1      ADC_Channel_0

/*=====================ͨ��2 IO ======================*/
// ADC IO�궨�壬�ɼ���ѹ
#define RHEOSTAT_ADC_GPIO_PORT3    GPIOA
#define RHEOSTAT_ADC_GPIO_PIN3     GPIO_Pin_2
#define RHEOSTAT_ADC_GPIO_CLK3     RCC_AHB1Periph_GPIOA
#define RHEOSTAT_ADC_CHANNEL3     ADC_Channel_2

// ADC ��ź궨��
#define RHEOSTAT_ADC              ADC1
#define RHEOSTAT_ADC_CLK          RCC_APB2Periph_ADC1
// ADC DR�Ĵ����궨�壬ADCת���������ֵ����������
#define RHEOSTAT_ADC_DR_ADDR    ((u32)ADC1+0x4c)

// ADC DMA ͨ���궨�壬��������ʹ��DMA����
#define RHEOSTAT_ADC_DMA_CLK      RCC_AHB1Periph_DMA2
#define RHEOSTAT_ADC_DMA_CHANNEL  DMA_Channel_0
#define RHEOSTAT_ADC_DMA_STREAM   DMA2_Stream0



void Rheostat_Init(void);

#endif /* __BSP_ADC_H */



