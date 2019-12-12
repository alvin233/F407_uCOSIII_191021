#ifndef	_W5500_H_
#define	_W5500_H_

/* typedef unsigned char SOCKET;			//�Զ���˿ں��������� */
#include "W5500_common.h"
/***************----- W5500 GPIO���� -----***************/
#define SPI_SEL   SPI2
#define W5500_SPI_CLK_Pin GPIO_Pin_13
#define W5500_SPI_CLK_PORT	GPIOB

#define W5500_SPI_MISO_Pin GPIO_Pin_14
#define W5500_SPI_MISO_PORT	GPIOB

#define W5500_SPI_MOSI_Pin GPIO_Pin_15
#define W5500_SPI_MOSI_PORT	GPIOB

#define W5500_SCS_Pin		GPIO_Pin_12	
#define W5500_SCS_PORT	GPIOB
	
#define W5500_RST_Pin		GPIO_Pin_11	
#define W5500_RST_PORT	GPIOB

#define W5500_INT_Pin		GPIO_Pin_4	
#define W5500_INT_PORT	GPIOC

/***************** Common Register *****************/
/* see w5500_common.h */
/********************* Socket Register *******************/
/* see w5500_common.h */
/*******************************************************************/
/************************ SPI Control Byte *************************/
/*******************************************************************/
/* see w5500_common.h */
extern unsigned char Gateway_IP[4];	//����IP��ַ 
extern unsigned char Sub_Mask[4];	//�������� 
extern unsigned char Phy_Addr[6];	//������ַ(MAC) 
extern unsigned char IP_Addr[4];	//����IP��ַ 

extern unsigned char S0_Port[2];	//�˿�0�Ķ˿ں�(5000) 
extern unsigned char S0_DIP[4];		//�˿�0Ŀ��IP��ַ 
extern unsigned char S0_DPort[2];	//�˿�0Ŀ�Ķ˿ں�(6000) 

extern unsigned char UDP_DIPR[4];	//UDP(�㲥)ģʽ,Ŀ������IP��ַ
extern unsigned char UDP_DPORT[2];	//UDP(�㲥)ģʽ,Ŀ�������˿ں�

/***************----- �˿ڵ�����ģʽ -----***************/
extern unsigned char S0_Mode;	//�˿�0������ģʽ,0:TCP������ģʽ,1:TCP�ͻ���ģʽ,2:UDP(�㲥)ģʽ
/*
#define TCP_SERVER		0x00	//TCP������ģʽ
#define TCP_CLIENT		0x01	//TCP�ͻ���ģʽ 
#define UDP_MODE		0x02	//UDP(�㲥)ģʽ 
*/

/***************----- �˿ڵ�����״̬ -----***************/
extern unsigned char S0_State;	//�˿�0״̬��¼,1:�˿���ɳ�ʼ��,2�˿��������(����������������) 
/*
#define S_INIT			0x01	
#define S_CONN			0x02	
*/

/***************----- �˿��շ����ݵ�״̬ -----***************/
extern unsigned char S0_Data;		//�˿�0���պͷ������ݵ�״̬,1:�˿ڽ��յ�����,2:�˿ڷ���������� 
/*
#define S_RECEIVE		0x01		
#define S_TRANSMITOK	0x02		
*/

/***************----- �˿����ݻ����� -----***************/
extern unsigned char Rx_Buffer[2048];	//�˿ڽ������ݻ����� 
extern unsigned char Tx_Buffer[2048];	//�˿ڷ������ݻ����� 

extern unsigned char W5500_Interrupt;	//W5500�жϱ�־(0:���ж�,1:���ж�)

extern void W5500_GPIO_Configuration(void);//W5500 GPIO��ʼ������
extern void SPI_Configuration(void);//W5500 SPI��ʼ������(STM32 SPI1)
extern int W5500_Hardware_Reset(void);//Ӳ����λW5500
extern void W5500_Init(void);//��ʼ��W5500�Ĵ�������
extern unsigned char Detect_Gateway(void);//������ط�����
extern void Socket_Init(SOCKET s);//ָ��Socket(0~7)��ʼ��
extern unsigned char Socket_Connect(SOCKET s);//����ָ��Socket(0~7)Ϊ�ͻ�����Զ�̷���������
extern unsigned char Socket_Listen(SOCKET s);//����ָ��Socket(0~7)��Ϊ�������ȴ�Զ������������
extern unsigned char Socket_UDP(SOCKET s);//����ָ��Socket(0~7)ΪUDPģʽ
extern unsigned short Read_SOCK_Data_Buffer(SOCKET s, unsigned char *dat_ptr);//ָ��Socket(0~7)�������ݴ���
extern void Write_SOCK_Data_Buffer(SOCKET s, unsigned char *dat_ptr, unsigned short size); //ָ��Socket(0~7)�������ݴ���
extern void W5500_Interrupt_Process(void);//W5500�жϴ���������
extern void Load_Net_Parameters(void);
extern void W5500_Initialization(void);
extern void W5500_Init(void);
extern void Write_W5500_1Byte(unsigned short reg, unsigned char dat);
extern void Write_W5500_nByte(unsigned short reg, unsigned char *dat_ptr, unsigned short size);
extern void Write_W5500_SOCK_1Byte(SOCKET s, unsigned short reg, unsigned char dat);
extern void SPI2_Send_Byte(unsigned char dat);
extern void SPI2_Send_Short(unsigned short dat);
extern void Write_W5500_2Byte(unsigned short reg, unsigned short dat);
extern void Write_W5500_SOCK_4Byte(SOCKET s, unsigned short reg, unsigned char *dat_ptr);
extern unsigned char Read_W5500_SOCK_1Byte(SOCKET s, unsigned short reg);
extern void Write_W5500_SOCK_2Byte(SOCKET s, unsigned short reg, unsigned short dat);
extern unsigned char Read_W5500_1Byte(unsigned short reg);
extern void W5500_Socket_Set(void);
extern void Process_Socket_Data(SOCKET s);
extern unsigned short Read_W5500_SOCK_2Byte(SOCKET s, unsigned short reg);
extern void EXTI4_IRQHandler(void);
extern void W5500_Initial(void);
#endif

