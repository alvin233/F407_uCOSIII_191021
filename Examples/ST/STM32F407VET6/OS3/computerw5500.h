#ifndef	COMPUTERW5500_H_
#define	COMPUTERW5500_H_

/* typedef unsigned char SOCKET;			//�Զ���˿ں��������� */
#include "W5500_common.h"

/***************** Common Register *****************/
/* see w5500_common.h */

/********************* Socket Register *******************/
/* see w5500_common.h */

/*******************************************************************/
/************************ SPI Control Byte *************************/
/*******************************************************************/
/* see w5500_common.h */

/***************----- W5500 GPIO���� -----***************/
#define SPI_1_SEL   SPI3
#define W5500_1_SPI_CLK_Pin GPIO_Pin_10
#define W5500_1_SPI_CLK_PORT	GPIOC

#define W5500_1_SPI_MISO_Pin GPIO_Pin_11
#define W5500_1_SPI_MISO_PORT	GPIOC

#define W5500_1_SPI_MOSI_Pin GPIO_Pin_12
#define W5500_1_SPI_MOSI_PORT	GPIOC

#define W5500_1_SCS_Pin		GPIO_Pin_7	//����W5500��CS����	 
#define W5500_1_SCS_PORT	GPIOC
	
#define W5500_1_RST_Pin		GPIO_Pin_8	//����W5500��RST����
#define W5500_1_RST_PORT	GPIOC

#define W5500_1_INT_Pin		GPIO_Pin_3	//����W5500��INT����
#define W5500_1_INT_PORT	GPIOC

/***************----- ��������������� -----***************/
extern unsigned char Gateway_1_IP[4];	//����IP��ַ 
extern unsigned char Sub_1_Mask[4];	//�������� 
extern unsigned char Phy_1_Addr[6];	//������ַ(MAC) 
extern unsigned char IP_1_Addr[4];	//����IP��ַ 

extern unsigned char S0_1_Port[2];	//�˿�0�Ķ˿ں�(5000) 
extern unsigned char S0_1_DIP[4];		//�˿�0Ŀ��IP��ַ 
extern unsigned char S0_1_DPort[2];	//�˿�0Ŀ�Ķ˿ں�(6000) 

extern unsigned char UDP_1_DIPR[4];	//UDP(�㲥)ģʽ,Ŀ������IP��ַ
extern unsigned char UDP_1_DPORT[2];	//UDP(�㲥)ģʽ,Ŀ�������˿ں�

/***************----- �˿ڵ�����ģʽ -----***************/
extern unsigned char S0_1_Mode;	//�˿�0������ģʽ,0:TCP������ģʽ,1:TCP�ͻ���ģʽ,2:UDP(�㲥)ģʽ
/* see w5500_common.h */
/* 
#define TCP_SERVER		0x00	//TCP������ģʽ
#define TCP_CLIENT		0x01	//TCP�ͻ���ģʽ 
#define UDP_MODE		0x02	//UDP(�㲥)ģʽ 
*/
/***************----- �˿ڵ�����״̬ -----***************/
extern unsigned char S0_1_State;	//�˿�0״̬��¼,1:�˿���ɳ�ʼ��,2�˿��������(����������������) 
/* see w5500_common.h */
/*
#define S_INIT			0x01	//�˿���ɳ�ʼ�� 
#define S_CONN			0x02	//�˿��������,���������������� 
*/
/***************----- �˿��շ����ݵ�״̬ -----***************/
extern unsigned char S0_1_Data;		//�˿�0���պͷ������ݵ�״̬,1:�˿ڽ��յ�����,2:�˿ڷ���������� 
/*
#define S_RECEIVE		0x01		//�˿ڽ��յ�һ�����ݰ� 
#define S_TRANSMITOK	0x02		//�˿ڷ���һ�����ݰ���� 
*/
/***************----- �˿����ݻ����� -----***************/
extern unsigned char Rx_1_Buffer[2048];	//�˿ڽ������ݻ����� 
extern unsigned char Tx_1_Buffer[2048];	//�˿ڷ������ݻ����� 

extern unsigned char W5500_1_Interrupt;	//W5500�жϱ�־(0:���ж�,1:���ж�)

extern void W5500_1_GPIO_Configuration(void);//W5500 GPIO��ʼ������
extern void SPI_1_Configuration(void);//W5500 SPI��ʼ������(STM32 SPI2)
extern void W5500_1_Init(void);//��ʼ��W5500�Ĵ�������
extern unsigned char Detect_1_Gateway(void);//������ط�����
extern void Socket_1_Init(SOCKET s);//ָ��Socket(0~7)��ʼ��
extern unsigned char Socket_1_Connect(SOCKET s);//����ָ��Socket(0~7)Ϊ�ͻ�����Զ�̷���������
extern unsigned char Socket_1_Listen(SOCKET s);//����ָ��Socket(0~7)��Ϊ�������ȴ�Զ������������
extern unsigned char Socket_1_UDP(SOCKET s);//����ָ��Socket(0~7)ΪUDPģʽ
extern unsigned short Read_1_SOCK_Data_Buffer(SOCKET s, unsigned char *dat_ptr);//ָ��Socket(0~7)�������ݴ���
extern void Write_1_SOCK_Data_Buffer(SOCKET s, unsigned char *dat_ptr, unsigned short size); //ָ��Socket(0~7)�������ݴ���
extern void W5500_1_Interrupt_Process(void);//W5500�жϴ���������
extern void Load_1_Net_Parameters(void);
extern void W5500_1_Initialization(void);
extern void Write_1_W5500_1Byte(unsigned short reg, unsigned char dat);
extern void Write_1_W5500_nByte(unsigned short reg, unsigned char *dat_ptr, unsigned short size);
extern void Write_1_W5500_SOCK_1Byte(SOCKET s, unsigned short reg, unsigned char dat);
extern void SPI1_Send_Byte(unsigned char dat);
extern void SPI1_Send_Short(unsigned short dat);
extern void Write_1_W5500_2Byte(unsigned short reg, unsigned short dat);
extern void Write_1_W5500_SOCK_4Byte(SOCKET s, unsigned short reg, unsigned char *dat_ptr);
extern unsigned char Read_1_W5500_SOCK_1Byte(SOCKET s, unsigned short reg);
extern void Write_1_W5500_SOCK_2Byte(SOCKET s, unsigned short reg, unsigned short dat);
extern int W5500_1_Hardware_Reset(void);
extern unsigned char Read_1_W5500_1Byte(unsigned short reg);
extern void W5500_1_Socket_Set(void);
extern unsigned char Socket_1_Listen(SOCKET s);
extern unsigned char Socket_1_UDP(SOCKET s);
extern void Process_1_Socket_Data(SOCKET s);
extern unsigned short Read_1_W5500_SOCK_2Byte(SOCKET s, unsigned short reg);
extern void EXTI3_IRQHandler(void);;
#endif

