#include "stm32f4xx.h"
#include "stm32f4xx_spi.h"	
#include "computerw5500.h"
#define   BSP_MODULE
#include  <bsp.h>
#include  <os.h>
#include <string.h>
#include "W5500.h"
extern void EXTI3_IRQHandler(void);
/**********************************************************************************
 * File name : computerW5500.c
 * Description   : driver      
 * Version : ST_v3.5

 * Link : http://yixindianzikeji.taobao.com/
**********************************************************************************/


/***************----- Network parameter definition -----***************/
/* gate way IP address */ 
unsigned char Gateway_1_IP[4];
/* submask ip address */
unsigned char Sub_1_Mask[4];	
/* MAC address */
unsigned char Phy_1_Addr[6];	
/* self IP address */ 
unsigned char IP_1_Addr[4];	 
/* port number of W5500's port 0 */
unsigned char S0_1_Port[2];	
/* port 0 of W5500's destination IP */
unsigned char S0_1_DIP[4];	
/* port 0 of W5500's destination port */
unsigned char S0_1_DPort[2];	
/* UDP broadcast mode, destination IP address */
unsigned char UDP_1_DIPR[4];	
/* UDP broadcast mode, destination IP port */
unsigned char UDP_1_DPORT[2];	

/**********************************************************************************
 * S0_1_Mode, port 0 operation mode
 * S0_1_Mode = TCP_SERVER; TCP servo mode;
 * S0_1_Mode = TCP_CLIENT; TCP client mode;    
 * S0_1_Mode = UDP_MODE; UDP broadcast mode;
**********************************************************************************/
unsigned char S0_1_Mode =3;	
/* TCP servo mode */
#define TCP_SERVER	0x00
/* TCP client mode */
#define TCP_CLIENT	0x01
/* UDP broadcast mode */
#define UDP_MODE	0x02
/**********************************************************************************
 * S0_1_State, port 0 state
 * S0_1_State = S_INIT; port init finished;
 * S0_1_State = S_CONN; port connected finished;
**********************************************************************************/
unsigned char S0_1_State =0;	
/* port has been inited */
#define S_INIT		0x01	 
/* port has been connected */
#define S_CONN		0x02	 
/**********************************************************************************
 * S0_1_Data, port 0 data state
 * S0_1_Data = S_RECEIVE; port has received one data pack;
 * S0_1_Data = S_TRANSMITOK; port has sended one data pack;
**********************************************************************************/
unsigned char S0_1_Data;	
#define S_RECEIVE	 0x01	 
#define S_TRANSMITOK 0x02	 
/***************----- Data buffer area -----***************/
/* RX buffer area */
unsigned char Rx_1_Buffer[2048];	 
/* TX buffer area */
unsigned char Tx_1_Buffer[2048];	 
/**********************************************************************************
 * W5500 interrupt state flag 
 * W5500_Interrupt = 0; no interruption
 * W5500_Interrupt = 1; interruption happened
**********************************************************************************/
unsigned char W5500_1_Interrupt;
/*******************************************************************************
 * Funtion : W5500_GPIO_Configuration
 * Description : W5500 GPIO configuration
 * Input : None
 * Output : None
 * Return : None
 * Others : None
*******************************************************************************/
void W5500_1_GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  EXTI_InitTypeDef   EXTI_InitStructure; 
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable Port B clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  /* W5500_INT_Pin Init */
  GPIO_InitStructure.GPIO_Pin = W5500_1_INT_Pin;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	/* GPIO_Mode_IPU */ 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
	/* inner pull up */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(W5500_1_INT_PORT, &GPIO_InitStructure);
  /* W5500_RST_Pin Init */
  GPIO_InitStructure.GPIO_Pin  = W5500_1_RST_Pin;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_25MHz;
	/* GPIO_Mode_Out_PP */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	/* GPIO_Mode_Out_PP */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	/* floating */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(W5500_1_RST_PORT, &GPIO_InitStructure);
  GPIO_ResetBits(W5500_1_RST_PORT, W5500_1_RST_Pin);
  /* W5500_SCS_Pin Init */
  GPIO_InitStructure.GPIO_Pin  = W5500_1_SCS_Pin;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_25MHz;
	/* GPIO_Mode_Out_PP */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	/* GPIO_Mode_Out_PP */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	/* floating */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(W5500_1_SCS_PORT, &GPIO_InitStructure);
  GPIO_ResetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin);
	/* EXTI configuration */
  /* for EXTI, must enable RCC_APB2Periph_SYSCFG */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  /* Connect EXTI Line4 to PC4 */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, GPIO_PinSource3);
  /* PC4 as W5500 interrupt input */
  EXTI_InitStructure.EXTI_Line = EXTI_Line3;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	/* falling edge trigger interrupt */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  /* NVIC Configuration */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02; 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
  NVIC_Init(&NVIC_InitStructure);
	/* must configure these two steps when using uC/OS-III */
	/* binding EXTI4 to function EXTI4_IRQHandler*/
  BSP_IntVectSet(BSP_INT_ID_EXTI3, EXTI3_IRQHandler);
	/* enable interruption of ucos iii */
  BSP_IntEn(BSP_INT_ID_EXTI3);
}

/*******************************************************************************
 * Funtion : SPI_Configuration
 * Description : SPI GPIO and AF function configuration
 * Input : None
 * Output : None
 * Return : None
 * Others : None
*******************************************************************************/
void SPI_1_Configuration(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	/* SPI GPIO configuration */
	/* enable AHB1 of port B*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	/* pin selection */
	GPIO_InitStructure.GPIO_Pin = W5500_1_SPI_CLK_Pin |W5500_1_SPI_MISO_Pin | W5500_1_SPI_MOSI_Pin;
	/* push pull output, for better power perfomance */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	/* inner pull down */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	/* AF mode*/	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	/* GPIO operation speed */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	/* enable selected GPIO to SPI2 AF function */
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_SPI3);  
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_SPI3);
	/* SPI function configuration */
	/* enable SPI2's clk */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE); 
	/* SPI running mode */
	SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
	/* make this SPI, master mode */
	SPI_InitStructure.SPI_Mode=SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;		
	/* CLK low when do nothing */				
	SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low; 
	/* extra data at the first CLK edge */
	SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge;						
	SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;							
	/* baudarate */
	SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;				
	SPI_InitStructure.SPI_CRCPolynomial=7;							
	SPI_Init(SPI3,&SPI_InitStructure);							
	SPI_Cmd(SPI3,ENABLE);	
}
/*******************************************************************************
 * Funtion : SPI2_Send_Byte
 * Description : SPI2 send one byte 
 * Input : None
 * Output : None
 * Return : None
 * Others : None
*******************************************************************************/
void SPI1_Send_Byte(unsigned char dat)
{
	SPI_I2S_SendData(SPI_1_SEL,dat);
	while(SPI_I2S_GetFlagStatus(SPI_1_SEL, SPI_I2S_FLAG_TXE) == RESET);
}
/*******************************************************************************
 * Funtion : Load_Net_Parameters
 * Description : net work parameter setting
 * Input : None
 * Output : None
 * Return : None
 * Others : None
*******************************************************************************/
void Load_1_Net_Parameters(void)
{
	/* gateway IP address */
	Gateway_1_IP[0] = 192;
	Gateway_1_IP[1] = 168;
	Gateway_1_IP[2] = 15;
	Gateway_1_IP[3] = 1;
	/* sub mask IP */
	Sub_1_Mask[0]=255;
	Sub_1_Mask[1]=255;
	Sub_1_Mask[2]=255;
	Sub_1_Mask[3]=0;
	/* MAC address */
	Phy_1_Addr[0]=0x0c;
	Phy_1_Addr[1]=0x29;
	Phy_1_Addr[2]=0xab;
	Phy_1_Addr[3]=0x7c;
	Phy_1_Addr[4]=0x00;
	Phy_1_Addr[5]=0x01;
	/* self IP address */
	IP_1_Addr[0]=192;
	IP_1_Addr[1]=168;
	IP_1_Addr[2]=15;
	IP_1_Addr[3]=3;
	/* port 0, port number. 0x1388 = 0d5000*/
	S0_1_Port[0] = 0x13;
	S0_1_Port[1] = 0x88;
	/* port 0, destination IP address */
	S0_1_DIP[0]=192;
	S0_1_DIP[1]=168;
	S0_1_DIP[2]=15;
	S0_1_DIP[3]=100;
	/* port 0, destination port number. 0x1770 = 0d6000 */
	S0_1_DPort[0] = 0x17;
	S0_1_DPort[1] = 0x70;
	/* port 0, operation mode. TCP client mode */
	S0_1_Mode = TCP_CLIENT;
}
/*******************************************************************************
 * Funtion : W5500_Initialization
 * Description : Init W5500
 * Input : None
 * Output : None
 * Return : None
 * Others : None
*******************************************************************************/
void W5500_1_Initialization(void)
{
	/* Init all register of W5500 */
	W5500_1_Init();	
	/* Check gateway connection */
	Detect_1_Gateway();	
	/* init the selected port, socket 0 */ 
	Socket_1_Init(0);	
}
/*******************************************************************************
 * Funtion : W5500_Init
 * Description : Init W5500's register 
 * Input : None
 * Output : None
 * Return : None
 * Others : None
*******************************************************************************/
void W5500_1_Init(void)
{
	OS_ERR  err;
	u8 i=0;
	/* soft-reset of W5500, set 1 enable, after the reset of W5500, register auto reset 0 */
	Write_1_W5500_1Byte(MR, RST);
	/* delay 10ms */
	OSTimeDlyHMSM(0u, 0u, 0u, 10u,
                OS_OPT_TIME_HMSM_STRICT,
                &err);
	/* set gateway IP, for access other net, such as Internet */
	Write_1_W5500_nByte(GAR, Gateway_1_IP, 4);			
	/* set sub net mask */
	Write_1_W5500_nByte(SUBR,Sub_1_Mask,4);			
	/* set MAC address, follow IEEE standard, for self-defination, the first byte must be odd number */
	Write_1_W5500_nByte(SHAR,Phy_1_Addr,6);		
	/* set self IP address, must make the IP belongs to the sub-net */
	/* for example, the forehead three bytes of gateway and IP must agree */
	Write_1_W5500_nByte(SIPR,IP_1_Addr,4);		
	/* Set socket 0~7 TX buffer and RX buffer to 2K, follow user guide of W5500 */
	for(i=0;i<8;i++)
	{
		/* RX buffer size of socket 0~7 */
		Write_1_W5500_SOCK_1Byte(i,Sn_RXBUF_SIZE, 0x02);
		/* TX buffer size of socket 0~7 */
		Write_1_W5500_SOCK_1Byte(i,Sn_TXBUF_SIZE, 0x02);
	}
	/* set reconnect timeout, bu default 200ms = 2000(0x07d0)*100us, 100us per ticket */
	Write_1_W5500_2Byte(RTR, 0x07d0);
	/* set reconnect times, by default 8. if times out, register Sn_IR TIMEOUT bit auto SET 1 */
	Write_1_W5500_1Byte(RCR,8);
	/* enable interrupt, refer to user guide of W5500, set intterrupt style */
	/* IMR_CONFLICT, IP conflict interrupt */
	/* IMR_UNREACH, address cannot access when using UDP mode */
	Write_1_W5500_1Byte(IMR,IM_IR7 | IM_IR6);
	Write_1_W5500_1Byte(SIMR,S0_IMR);
	Write_1_W5500_SOCK_1Byte(0, Sn_IMR, IMR_SENDOK | IMR_TIMEOUT | IMR_RECV | IMR_DISCON | IMR_CON);
}
/*******************************************************************************
 * Funtion : Write_W5500_1Byte
 * Description : write one byte to the selected registor of W5500
 * Input : reg, 16bit registor; dat, data;
 * Output : None
 * Return : None
 * Others : None
*******************************************************************************/
void Write_1_W5500_1Byte(unsigned short reg, unsigned char dat)
{
	/* reset pin SCS, enable W5500 */
	GPIO_ResetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin);
	/* use SPI send to the registor */
	SPI1_Send_Short(reg);
	/* write control word, 1 byte length */
	SPI1_Send_Byte(FDM1|RWB_WRITE|COMMON_R);
	/* write data */
	SPI1_Send_Byte(dat);
	/* set pin SCS, disable W5500 */
	GPIO_SetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin);
}
/*******************************************************************************
 * Funtion : Write_W5500_nByte
 * Description : write multi-byte to the selected registor of W5500
 * Input : reg, 16bit registor; *dat_ptr, data buffer ptr; size, data size;
 * Output : None
 * Return : None
 * Others : None
*******************************************************************************/
void Write_1_W5500_nByte(unsigned short reg, unsigned char *dat_ptr, unsigned short size)
{
	unsigned short i;
	/* reset pin SCS, enable W5500 */
	GPIO_ResetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin);
	/* use SPI send to the registor */	
	SPI1_Send_Short(reg);
	/* write control word, n byte length */
	SPI1_Send_Byte(VDM|RWB_WRITE|COMMON_R);
	/* write all data to W5500 */
	for(i=0;i<size;i++)
	{
		SPI1_Send_Byte(*dat_ptr++);
	}
	/* set pin SCS, disable W5500 */
	GPIO_SetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin);
}
/*******************************************************************************
 * Funtion : SPI2_Send_Short
 * Description : write 2-byte to the selected registor of W5500
 * Input : dat, data;
 * Output : None
 * Return : None
 * Others : None
*******************************************************************************/
void SPI1_Send_Short(unsigned short dat)
{
	/* MSB */
	SPI1_Send_Byte(dat/256);
	/* LSB */
	SPI1_Send_Byte(dat);
}
/*******************************************************************************
 * Funtion : Write_W5500_SOCK_1Byte
 * Description : write 1-byte to the selected socket of W5500
 * Input : s, socket port; reg, 16 bit registor address; dat, data;
 * Output : None
 * Return : None
 * Others : None
*******************************************************************************/
void Write_1_W5500_SOCK_1Byte(SOCKET s, unsigned short reg, unsigned char dat)
{
	/* reset pin SCS, enable W5500 */
	GPIO_ResetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin);
	/* 16 bit registro addr. */		
	SPI1_Send_Short(reg);
	/* control byte */
	SPI1_Send_Byte(FDM1|RWB_WRITE|(s*0x20+0x08));
	/* send data */
	SPI1_Send_Byte(dat);
	/* set pin SCS, disable W5500 */
	GPIO_SetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin); 
}
/*******************************************************************************
 * Funtion : Write_W5500_2Byte
 * Description : write 2-byte to the selected registor of W5500
 * Input : reg, 16 bit registor address; dat, data;
 * Output : None
 * Return : None
 * Others : None
*******************************************************************************/
void Write_1_W5500_2Byte(unsigned short reg, unsigned short dat)
{
	/* reset pin SCS, enable W5500 */
	GPIO_ResetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin);
	/* 16 bit registro addr. */	
	SPI1_Send_Short(reg);
	/* control byte */
	SPI1_Send_Byte(FDM2|RWB_WRITE|COMMON_R);
	/* send data */
	SPI1_Send_Short(dat);
	/* set pin SCS, disable W5500 */
	GPIO_SetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin);
}
/*******************************************************************************
 * Funtion : Write_W5500_2Byte
 * Description : write 2-byte to the selected registor of W5500
 * Input : reg, 16 bit registor address; dat, data;
 * Output : None
 * Return : None
 * Others : None
*******************************************************************************/
# if 0
void SPI1_Send_Byte(unsigned char dat)
{
	SPI_I2S_SendData(SPI_1_SEL,dat);//д1���ֽ�����
	while(SPI_I2S_GetFlagStatus(SPI_1_SEL, SPI_I2S_FLAG_TXE) == RESET);//�ȴ����ݼĴ�����
}
#endif
/*******************************************************************************
 * Funtion : Detect_Gateway
 * Description : check gateway connection status
 * Input : None
 * Output : None
 * Return : None
 * Others : None
*******************************************************************************/
unsigned char Detect_1_Gateway(void)
{
	unsigned char ip_adde[4];
	OS_ERR  err;
	ip_adde[0]=IP_1_Addr[0]+1;
	ip_adde[1]=IP_1_Addr[1]+1;
	ip_adde[2]=IP_1_Addr[2]+1;
	ip_adde[3]=IP_1_Addr[3]+1;
	/* check gateway and gateway's MAC */
	/* write an IP differ from self IP to the destination IP by socket 0 */
	Write_1_W5500_SOCK_4Byte(0,Sn_DIPR,ip_adde);
	/* set socket working in TCP mode */
	Write_1_W5500_SOCK_1Byte(0,Sn_MR,MR_TCP);
	/* open socket 0 */
	Write_1_W5500_SOCK_1Byte(0,Sn_CR,OPEN);
	/* delay 5ms */	
  OSTimeDlyHMSM(0u, 0u, 0u, 5u,
                OS_OPT_TIME_HMSM_STRICT,
                &err);
	if(Read_1_W5500_SOCK_1Byte(0,Sn_SR) != SOCK_INIT)//���socket��ʧ��
	{
		/* open socket failed, close socket */
		Write_1_W5500_SOCK_1Byte(0,Sn_CR,CLOSE);
		return FALSE;
	}
	/* set socket 0 to connect state */
	Write_1_W5500_SOCK_1Byte(0,Sn_CR,CONNECT);				
	do
	{
		u8 j=0;
		/* read W5500 Interruption registor state */
		j=Read_1_W5500_SOCK_1Byte(0,Sn_IR);
		if(j!=0)
		{
			Write_1_W5500_SOCK_1Byte(0,Sn_IR,j);
		}		
		/* delay 5ms */
	  OSTimeDlyHMSM(0u, 0u, 0u, 5u,
                  OS_OPT_TIME_HMSM_STRICT,
                  &err);
		if((j&IR_TIMEOUT) == IR_TIMEOUT)
		{
			/* connect timeout */
			return FALSE;	
		}
		else if(Read_1_W5500_SOCK_1Byte(0,Sn_DHAR) != 0xff)
		{
			/* close socket 0 */
			Write_1_W5500_SOCK_1Byte(0,Sn_CR,CLOSE);
			return TRUE;							
		}
	}while(1);
}
/*******************************************************************************
 * Funtion : Write_W5500_SOCK_4Byte
 * Description : write 4 byte to the selected registor by selected socket
 * Input : s, socket port; reg, 16 bit registor; *dat_ptr, 4 byte data buffer ptr;
 * Output : None
 * Return : None
 * Others : None
*******************************************************************************/
void Write_1_W5500_SOCK_4Byte(SOCKET s, unsigned short reg, unsigned char *dat_ptr)
{
	/* enable W5500 */
	GPIO_ResetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin);
	/* select registor */			
	SPI1_Send_Short(reg);
	/* ctrl byte */
	SPI1_Send_Byte(FDM4|RWB_WRITE|(s*0x20+0x08));
	/* write 4 byte data */
	SPI1_Send_Byte(*dat_ptr++);
	SPI1_Send_Byte(*dat_ptr++);
	SPI1_Send_Byte(*dat_ptr++);
	SPI1_Send_Byte(*dat_ptr++);
	/* disable W5500 */
	GPIO_SetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin);
}
/*******************************************************************************
 * Funtion : Read_W5500_SOCK_1Byte
 * Description : write 1 byte to the selected registor by selected socket
 * Input : s, socket port; reg, 16 bit registor;
 * Output : None
 * Return : None
 * Others : None
*******************************************************************************/
unsigned char Read_1_W5500_SOCK_1Byte(SOCKET s, unsigned short reg)
{
	unsigned char rev_data;
	/* enable W5500 */
	GPIO_ResetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin);
	/* select registor */	
	SPI1_Send_Short(reg);
	/* ctrl byte */
	SPI1_Send_Byte(FDM1|RWB_READ|(s*0x20+0x08));
	rev_data = SPI_I2S_ReceiveData(SPI_1_SEL);
	/* write one test data */
	SPI1_Send_Byte(0x00);
	rev_data = SPI_I2S_ReceiveData(SPI_1_SEL);
	/* disable W55000 */
	GPIO_SetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin);
	return (rev_data);
}
/*******************************************************************************
 * Funtion : Socket_Init
 * Description : Init the selected socket port
 * Input : s, socket port; 
 * Output : None
 * Return : None
 * Others : None
*******************************************************************************/
void Socket_1_Init(SOCKET s)
{
	/* set piece length, refer to user guide of W5500 */
	Write_1_W5500_SOCK_2Byte(0, Sn_MSSR, 1460);
	switch(s)
	{
		case 0:
			/* setting of socket 0 */
			/* set socket 0's port number (self IP) */
			Write_1_W5500_SOCK_2Byte(0, Sn_PORT, S0_1_Port[0]*256+S0_1_Port[1]);
			/* set socket 0's port number (destination IP) */
			Write_1_W5500_SOCK_2Byte(0, Sn_DPORTR, S0_1_DPort[0]*256+S0_1_DPort[1]);
			/* set socket 0's destination IP */
			Write_1_W5500_SOCK_4Byte(0, Sn_DIPR, S0_1_DIP);	
			break;
		case 1:
			/* setting of socket 1 */
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		default:
			break;
	}
}
/*******************************************************************************
 * Funtion : Write_W5500_SOCK_2Byte
 * Description : write 2 byte to the selected registor of W5500;
 * Input : s, socket selection; reg, registor address; dat, data, 2 bytes;  
 * Output : None
 * Return : None
 * Others : None
*******************************************************************************/
void Write_1_W5500_SOCK_2Byte(SOCKET s, unsigned short reg, unsigned short dat)
{
	/* pull SCS down, enable W5500 */
	GPIO_ResetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin);
	/* 16 bit reg address */
	SPI1_Send_Short(reg);
	/* control word */
	SPI1_Send_Byte(FDM2|RWB_WRITE|(s*0x20+0x08));
	/* data, 2 byte */
	SPI1_Send_Short(dat);
	/* pull SCS up, disable W5500 */
	GPIO_SetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin); 
}
/*******************************************************************************
 * Funtion : W5500_Hardware_Reset
 * Description : reset W5500
 * Input : None
 * Output : None
 * Return : None
 * Others : None
*******************************************************************************/
int W5500_1_Hardware_Reset(void)
{
	OS_ERR  err;
	/* pull down reset IO pin */
	GPIO_ResetBits(W5500_1_RST_PORT, W5500_1_RST_Pin);
	/* delay 50ms */
	OSTimeDlyHMSM(0u, 0u, 0u, 50u,
					OS_OPT_TIME_HMSM_STRICT,
					&err);
	/* pull up reset IO pin */
	GPIO_SetBits(W5500_1_RST_PORT, W5500_1_RST_Pin);
	/* delay 200ms */
	OSTimeDlyHMSM(0u, 0u, 0u, 200u,
					OS_OPT_TIME_HMSM_STRICT,
					&err);
	/* wait for link connection, had better re-write this function */
	if((Read_1_W5500_1Byte(PHYCFGR)&LINK)==0)
	{
		return (1);
	}
	else
	{
		return (0);
	}
}
/*******************************************************************************
 * Funtion : Read_W5500_1Byte
 * Description : get data from selected registor of W5500
 * Input : reg, registor address;
 * Output : None
 * Return : None
 * Others : None
*******************************************************************************/
unsigned char Read_1_W5500_1Byte(unsigned short reg)
{
	unsigned char recv_data;
	/* pull down SCS pin, enable W5500 */
	GPIO_ResetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin);
	/* select registor */			
	SPI1_Send_Short(reg);
	/* control byte */
	SPI1_Send_Byte(FDM1|RWB_READ|COMMON_R);
	recv_data = SPI_I2S_ReceiveData(SPI_1_SEL);
	/* send one test data */
	SPI1_Send_Byte(0x00);
	recv_data = SPI_I2S_ReceiveData(SPI_1_SEL);
	/* disable W5500 */
	GPIO_SetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin);
	return (recv_data);
}
/*******************************************************************************
 * Funtion : W5500_Socket_Set
 * Description : socket configuration
 * Input : None
 * Output : None
 * Return : None
 * Others : None
*******************************************************************************/
void W5500_1_Socket_Set(void)
{
	/* socket 0 configuration */
	if(S0_1_State==0)
	{
		if(S0_1_Mode==TCP_SERVER) 
		{
			/* TCP server mode */
			if(Socket_1_Listen(0)==TRUE)
			{
				S0_1_State=S_INIT;
			}				
			else
			{
				S0_1_State=0;
			}				
		}
		else 
		if(S0_1_Mode==TCP_CLIENT) 
		{
			/* TCP client mode */
			if(Socket_1_Connect(0)==TRUE)
			{
				S0_1_State=S_INIT;
			}				
			else
			{
				S0_1_State=0;
			}				
		}
		else 
		{
			/* UDP mode */
			if(Socket_1_UDP(0) == TRUE)
			{
				S0_1_State = S_INIT|S_CONN;
			}				
			else
			{
				S0_1_State = 0;
			}	
		}
	}
}
/*******************************************************************************
 * Funtion : Socket_Connect
 * Description : make connection of selected socket;
 * Input : s, socket port to establish connection;
 * Output : None
 * Return : None
 * Others : None
*******************************************************************************/
unsigned char Socket_1_Connect(SOCKET s)
{
	OS_ERR  err;
	/* set socket work as TCP mode */
	Write_1_W5500_SOCK_1Byte(s,Sn_MR,MR_TCP);
	/* open socket port s */
	Write_1_W5500_SOCK_1Byte(s,Sn_CR,OPEN);
	/* delay 8ms */
	OSTimeDlyHMSM(0u, 0u, 0u, 8u,
					OS_OPT_TIME_HMSM_STRICT,
					&err);
	if(Read_1_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_INIT)
	{
		/* open socket failed, close socket */
		Write_1_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);
		return FALSE;
	}
	/* establish socket connection state */
	Write_1_W5500_SOCK_1Byte(s,Sn_CR,CONNECT);
	return TRUE;
}
/*******************************************************************************
 * Funtion : Socket_Listen
 * Description : set the socket port as server waiting for connection
 * Input : s, socket port to wait connection;
 * Output : None
 * Return : TRUE, sucesss; FALSE, fail;
 * Others : when socket work as server mode, 
 * just call the function once to put W5500 working as server mdoe;
*******************************************************************************/
unsigned char Socket_1_Listen(SOCKET s)
{
	OS_ERR  err;
	/* set socket work as TCP mode */
	Write_1_W5500_SOCK_1Byte(s,Sn_MR,MR_TCP); 
	/* open socket */
	Write_1_W5500_SOCK_1Byte(s,Sn_CR,OPEN);	
	/* delay 5ms */
	OSTimeDlyHMSM(0u, 0u, 0u, 5u,
					OS_OPT_TIME_HMSM_STRICT,
					&err);
	if(Read_1_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_INIT)
	{
		/* open socket failed, close socket */
		Write_1_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);
		return FALSE;
	}	
	/* set socket listen connection */
	Write_1_W5500_SOCK_1Byte(s,Sn_CR,LISTEN);	
	/* delay 5ms */
	OSTimeDlyHMSM(0u, 0u, 0u, 5u,
				OS_OPT_TIME_HMSM_STRICT,
				&err);
	if(Read_1_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_LISTEN)
	{
		/* setting failed, close socket */
		Write_1_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);
		return FALSE;
	}
	return TRUE;
	/* wait socket interruption to ensure the socket connection */
	/* when using listen mode, no need seting destnation IP and port number */
}
/*******************************************************************************
 * Funtion : Socket_UDP
 * Description : set the selected socket port UDP mode
 * Input : s, socket port to wait connection;
 * Output : None
 * Return : TRUE, sucesss; FALSE, fail;
 * Others : 
*******************************************************************************/
unsigned char Socket_1_UDP(SOCKET s)
{
	OS_ERR  err;
	/* set W5500 working as UDP mode */
	Write_1_W5500_SOCK_1Byte(s,Sn_MR,MR_UDP);
	/* open socket */
	Write_1_W5500_SOCK_1Byte(s,Sn_CR,OPEN);
	OSTimeDlyHMSM(0u, 0u, 0u, 5u,
					OS_OPT_TIME_HMSM_STRICT,
					&err);
	if(Read_1_W5500_SOCK_1Byte(s,Sn_SR)!=SOCK_UDP)
	{
		/* socket open failed, close socket port */
		Write_1_W5500_SOCK_1Byte(s,Sn_CR,CLOSE);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
	/* in UDP mode, no need of connection before send data */
	/* in UDP mode, set IP addr and socket port before send data is ok */
	/* you can set you desitnation IP and socket port here if they are fixed */
}
/*******************************************************************************
 * Funtion : W5500_Interrupt_Process
 * Description : ;
 * Input : None
 * Output : None
 * Return : None
 * Others : None
*******************************************************************************/
void W5500_1_Interrupt_Process(void)
{
	unsigned char i,j;
IntDispose:
	/* clear interruption flag*/
	W5500_1_Interrupt=0;
	/* get intertuption registor */
	i = Read_1_W5500_1Byte(IR);
	/* clear interruption registor */
	Write_1_W5500_1Byte(IR, (i&0xf0));
	if((i & CONFLICT) == CONFLICT)//IP��ַ��ͻ�쳣����
	{
		 //�Լ����Ӵ���
	}
	if((i & UNREACH) == UNREACH)//UDPģʽ�µ�ַ�޷������쳣����
	{
		//�Լ����Ӵ���
	}
	i=Read_1_W5500_1Byte(SIR);//��ȡ�˿��жϱ�־�Ĵ���	
	if((i & S0_INT) == S0_INT)//Socket0�¼����� 
	{
		j=Read_1_W5500_SOCK_1Byte(0,Sn_IR);//��ȡSocket0�жϱ�־�Ĵ���
		Write_1_W5500_SOCK_1Byte(0,Sn_IR,j);
		if(j&IR_CON)//��TCPģʽ��,Socket0�ɹ����� 
		{
			S0_1_State|=S_CONN;//��������״̬0x02,�˿�������ӣ�����������������?
		}
		if(j&IR_DISCON)//��TCPģʽ��Socket�Ͽ����Ӵ���
		{
			Write_1_W5500_SOCK_1Byte(0,Sn_CR,CLOSE);//�رն˿�,�ȴ����´����� 
			Socket_1_Init(0);		//ָ��Socket(0~7)��ʼ��,��ʼ���˿�0
			S0_1_State=0;//��������״̬0x00,�˿�����ʧ��
		}
		if(j&IR_SEND_OK)//Socket0���ݷ������?�����ٴ�����S_tx_process()������������ 
		{
			S0_1_Data|=S_TRANSMITOK;//�˿ڷ���һ�����ݰ����?
		}
		if(j&IR_RECV)//Socket���յ�����,��������S_rx_process()���� 
		{
			S0_1_Data|=S_RECEIVE;//�˿ڽ��յ�һ�����ݰ�
		}
		if(j&IR_TIMEOUT)//Socket���ӻ����ݴ��䳬ʱ���� 
		{
			Write_1_W5500_SOCK_1Byte(0,Sn_CR,CLOSE);// �رն˿�,�ȴ����´����� 
			S0_1_State=0;//��������״̬0x00,�˿�����ʧ��
		}
	}
	if(Read_1_W5500_1Byte(SIR) != 0) 
		goto IntDispose;
}
/*******************************************************************************
* ������  : Process_Socket_Data
* ����    : W5500���ղ����ͽ��յ�������
* ����    : s:�˿ں�
* ���?   : ��
* ����ֵ  : ��
* ˵��    : �������ȵ���S_rx_process()��W5500�Ķ˿ڽ������ݻ�������ȡ����,
*			Ȼ�󽫶�ȡ�����ݴ�Rx_Buffer������Temp_Buffer���������д�����
*			������ϣ������ݴ�Temp_Buffer������Tx_1_Buffer������������S_tx_process()
*			�������ݡ�
*******************************************************************************/
void Process_1_Socket_Data(SOCKET s)
{
	unsigned short size;
	size=Read_1_SOCK_Data_Buffer(s, Rx_1_Buffer);
	memcpy(Tx_1_Buffer, Rx_1_Buffer, size);			
	Write_1_SOCK_Data_Buffer(s, Tx_1_Buffer, size);
}

/*******************************************************************************
* ������  : Write_SOCK_Data_Buffer
* ����    : ������д��W5500�����ݷ��ͻ�����
* ����    : s:�˿ں�,*dat_ptr:���ݱ��滺����ָ��,size:��д�����ݵĳ���
* ���?   : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void Write_1_SOCK_Data_Buffer(SOCKET s, unsigned char *dat_ptr, unsigned short size)
{
	unsigned short offset,offset1;
	unsigned short i;

	//�����UDPģʽ,�����ڴ�����Ŀ��������IP�Ͷ˿ں�
	if((Read_1_W5500_SOCK_1Byte(s,Sn_MR)&0x0f) != SOCK_UDP)//���Socket��ʧ��
	{		
		Write_1_W5500_SOCK_4Byte(s, Sn_DIPR, UDP_1_DIPR);//����Ŀ������IP  		
		Write_1_W5500_SOCK_2Byte(s, Sn_DPORTR, UDP_1_DPORT[0]*256+UDP_1_DPORT[1]);//����Ŀ�������˿ں�				
	}

	offset=Read_1_W5500_SOCK_2Byte(s,Sn_TX_WR);
	offset1=offset;
	offset&=(S_TX_SIZE-1);//����ʵ�ʵ�������ַ

	GPIO_ResetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin);//��W5500��SCSΪ�͵�ƽ

	SPI1_Send_Short(offset);//д16λ��ַ
	SPI1_Send_Byte(VDM|RWB_WRITE|(s*0x20+0x10));//д�����ֽ�,N���ֽ����ݳ���,д����,ѡ��˿�s�ļĴ���

	if((offset+size)<S_TX_SIZE)//�������ַδ����W5500���ͻ������Ĵ���������ַ
	{
		for(i=0;i<size;i++)//ѭ��д��size���ֽ�����
		{
			SPI1_Send_Byte(*dat_ptr++);//д��һ���ֽڵ�����		
		}
	}
	else//�������ַ����W5500���ͻ������Ĵ���������ַ
	{
		offset=S_TX_SIZE-offset;
		for(i=0;i<offset;i++)//ѭ��д��ǰoffset���ֽ�����
		{
			SPI1_Send_Byte(*dat_ptr++);//д��һ���ֽڵ�����
		}
		GPIO_SetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin); //��W5500��SCSΪ�ߵ�ƽ

		GPIO_ResetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin);//��W5500��SCSΪ�͵�ƽ

		SPI1_Send_Short(0x00);//д16λ��ַ
		SPI1_Send_Byte(VDM|RWB_WRITE|(s*0x20+0x10));//д�����ֽ�,N���ֽ����ݳ���,д����,ѡ��˿�s�ļĴ���

		for(;i<size;i++)//ѭ��д��size-offset���ֽ�����
		{
			SPI1_Send_Byte(*dat_ptr++);//д��һ���ֽڵ�����
		}
	}
	GPIO_SetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin); //��W5500��SCSΪ�ߵ�ƽ

	offset1+=size;//����ʵ��������ַ,���´�д���������ݵ��������ݻ���������ʼ��ַ
	Write_1_W5500_SOCK_2Byte(s, Sn_TX_WR, offset1);
	Write_1_W5500_SOCK_1Byte(s, Sn_CR, SEND);//����������������				
}
/*******************************************************************************
* ������  : Read_W5500_SOCK_2Byte
* ����    : ��W5500ָ���˿ڼĴ�����2���ֽ�����
* ����    : s:�˿ں�,reg:16λ�Ĵ�����ַ
* ���?   : ��
* ����ֵ  : ��ȡ���Ĵ�����2���ֽ�����(16λ)
* ˵��    : ��
*******************************************************************************/
unsigned short Read_1_W5500_SOCK_2Byte(SOCKET s, unsigned short reg)
{
	unsigned short i;

	GPIO_ResetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin);//��W5500��SCSΪ�͵�ƽ
			
	SPI1_Send_Short(reg);//ͨ��SPI1д16λ�Ĵ�����ַ
	SPI1_Send_Byte(FDM2|RWB_READ|(s*0x20+0x08));//ͨ��SPI1д�����ֽ�,2���ֽ����ݳ���,������,ѡ��˿�s�ļĴ���

	i=SPI_I2S_ReceiveData(SPI_1_SEL);
	SPI1_Send_Byte(0x00);//����һ��������
	i=SPI_I2S_ReceiveData(SPI_1_SEL);//��ȡ��λ����
	SPI1_Send_Byte(0x00);//����һ��������
	i*=256;
	i+=SPI_I2S_ReceiveData(SPI_1_SEL);//��ȡ��λ����

	GPIO_SetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin);//��W5500��SCSΪ�ߵ�ƽ
	return i;//���ض�ȡ���ļĴ�������
}

/*******************************************************************************
* ������  : Read_SOCK_Data_Buffer
* ����    : ��W5500�������ݻ������ж�ȡ����
* ����    : s:�˿ں�,*dat_ptr:���ݱ��滺����ָ��
* ���?   : ��
* ����ֵ  : ��ȡ�������ݳ���,rx_size���ֽ�
* ˵��    : ��
*******************************************************************************/
unsigned short Read_1_SOCK_Data_Buffer(SOCKET s, unsigned char *dat_ptr)
{
	unsigned short rx_size;
	unsigned short offset, offset1;
	unsigned short i;
	unsigned char j;

	rx_size=Read_1_W5500_SOCK_2Byte(s,Sn_RX_RSR);
	if(rx_size==0) return 0;//û���յ������򷵻�
	if(rx_size>1460) rx_size=1460;

	offset=Read_1_W5500_SOCK_2Byte(s,Sn_RX_RD);
	offset1=offset;
	offset&=(S_RX_SIZE-1);//����ʵ�ʵ�������ַ

	GPIO_ResetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin);//��W5500��SCSΪ�͵�ƽ

	SPI1_Send_Short(offset);//д16λ��ַ
	SPI1_Send_Byte(VDM|RWB_READ|(s*0x20+0x18));//д�����ֽ�,N���ֽ����ݳ���,������,ѡ��˿�s�ļĴ���
	j=SPI_I2S_ReceiveData(SPI_1_SEL);
	
	if((offset+rx_size)<S_RX_SIZE)//�������ַδ����W5500���ջ������Ĵ���������ַ
	{
		for(i=0;i<rx_size;i++)//ѭ����ȡrx_size���ֽ�����
		{
			SPI1_Send_Byte(0x00);//����һ��������
			j=SPI_I2S_ReceiveData(SPI_1_SEL);//��ȡ1���ֽ�����
			*dat_ptr=j;//����ȡ�������ݱ��浽���ݱ��滺����
			dat_ptr++;//���ݱ��滺����ָ���ַ����?
		}
	}
	else//�������ַ����W5500���ջ������Ĵ���������ַ
	{
		offset=S_RX_SIZE-offset;
		for(i=0;i<offset;i++)//ѭ����ȡ��ǰoffset���ֽ�����
		{
			SPI1_Send_Byte(0x00);//����һ��������
			j=SPI_I2S_ReceiveData(SPI_1_SEL);//��ȡ1���ֽ�����
			*dat_ptr=j;//����ȡ�������ݱ��浽���ݱ��滺����
			dat_ptr++;//���ݱ��滺����ָ���ַ����?
		}
		GPIO_SetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin); //��W5500��SCSΪ�ߵ�ƽ

		GPIO_ResetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin);//��W5500��SCSΪ�͵�ƽ

		SPI1_Send_Short(0x00);//д16λ��ַ
		SPI1_Send_Byte(VDM|RWB_READ|(s*0x20+0x18));//д�����ֽ�,N���ֽ����ݳ���,������,ѡ��˿�s�ļĴ���
		j=SPI_I2S_ReceiveData(SPI_1_SEL);

		for(;i<rx_size;i++)//ѭ����ȡ��rx_size-offset���ֽ�����
		{
			SPI1_Send_Byte(0x00);//����һ��������
			j=SPI_I2S_ReceiveData(SPI_1_SEL);//��ȡ1���ֽ�����
			*dat_ptr=j;//����ȡ�������ݱ��浽���ݱ��滺����
			dat_ptr++;//���ݱ��滺����ָ���ַ����?
		}
	}
	GPIO_SetBits(W5500_1_SCS_PORT, W5500_1_SCS_Pin); //��W5500��SCSΪ�ߵ�ƽ

	offset1+=rx_size;//����ʵ��������ַ,���´ζ�ȡ���յ������ݵ���ʼ��ַ
	Write_1_W5500_SOCK_2Byte(s, Sn_RX_RD, offset1);
	Write_1_W5500_SOCK_1Byte(s, Sn_CR, RECV);//����������������
	return rx_size;//���ؽ��յ����ݵĳ���
}



