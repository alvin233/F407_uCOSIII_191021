#include  <includes.h>
#include "app_add.h"

 double V_I_output;
 double V_V_output;
 double Cal_V_I_output;
 double Cal_V_V_output;
/*
*********************************************************************************************************
*                                             App_TaskW5500()
*
* Description : This task init W5500, and connect send message.
*               
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskEq0Fp' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void  App_TaskW5500 (void  *p_arg)
{
  OS_ERR  err;
	char str[160];
/* SPI configuration */
  SPI_Configuration();	
  /* GPIO Init */
  W5500_GPIO_Configuration();
  /* Setting Net Parameter */
  Load_Net_Parameters();
  /* Reset */
  W5500_Hardware_Reset();
	
  W5500_Initialization();		
  while (DEF_TRUE) {
  W5500_Socket_Set();
  if(W5500_Interrupt)	
  {
    /* Interrupt happened */
    W5500_Interrupt_Process();
  }
  if((S0_Data & S_RECEIVE) == S_RECEIVE)
  {
    /* socket0 received data */
    S0_Data&=~S_RECEIVE;
    /* receive data and re-send it */
    Process_Socket_Data(0);
  }
  /* send every 500ms */
  if(S0_State == (S_INIT|S_CONN))
  {
    S0_Data&=~S_TRANSMITOK;
    memcpy(Tx_Buffer, "\r\nWelcome To CQU!\r\n", 19);	
    /* socket 0 send data, size 23 byte */
    Write_SOCK_Data_Buffer(0, Tx_Buffer, (15 + 4));
    /* output current */
     sprintf(str, "\r\n V_I = %f V \r\n V_V = %f V \r\n Cal_I = %f A \r\n Cal_V = %f V \r\n ", V_I_output, V_V_output, Cal_V_I_output, Cal_V_V_output);
     memcpy(Tx_Buffer, str, 160);        
     Write_SOCK_Data_Buffer(0, Tx_Buffer, 160);
/*
     sprintf(str_1, "\r\n V_V = %f V \r\n", data_out_V_V);
     memcpy(Tx_Buffer, str_1, 80);
     Write_SOCK_Data_Buffer(0, Tx_Buffer, 80);
*/
  }			
  /* do something here */      
  OSTimeDlyHMSM(0u, 0u, 1u, 10u,
  OS_OPT_TIME_HMSM_STRICT,
  &err);
  /* output your data by terminal */ 
  }
}

/*
*********************************************************************************************************
*                                             App_TaskW5500()
*
* Description : This task init W5500, and connect send message.
*               
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskEq0Fp' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  App_TaskW5500_1 (void  *p_arg)
{
  OS_ERR  err;
	char str[160];
  //char str_1[160];
/* SPI configuration */
  SPI_1_Configuration();	
  /* GPIO Init */
  W5500_1_GPIO_Configuration();
  /* Setting Net Parameter */
  Load_1_Net_Parameters();
  /* Reset */
  while(!W5500_1_Hardware_Reset())
	{
		/* trap CPU here while W5500 hardware reset wrong */
  OSTimeDlyHMSM(0u, 0u, 2u, 100u,
		OS_OPT_TIME_HMSM_STRICT,
		&err);
	}
	/* W5500 hardware reset OK */
  W5500_1_Initialization();		
  while (DEF_TRUE) {
  W5500_1_Socket_Set();
  if(W5500_1_Interrupt)	
  {
    /* Interrupt happened */
    W5500_1_Interrupt_Process();
  }
  if((S0_1_Data & S_RECEIVE) == S_RECEIVE)
  {
    /* socket0 received data */
    S0_1_Data&=~S_RECEIVE;
    /* receive data and re-send it */
    Process_1_Socket_Data(0);
  }
  /* send every 500ms */
  if(S0_1_State == (S_INIT|S_CONN))
  {
    S0_1_Data&=~S_TRANSMITOK;
    memcpy(Tx_1_Buffer, "\r\nWelcome To CQU!\r\n", 19);	
    /* socket 0 send data, size 23 byte */
    Write_1_SOCK_Data_Buffer(0, Tx_1_Buffer, (15 + 4));
    /* output current */
     //sprintf(str, "\r\n V_I = %f V \r\n V_V = %f V \r\n Cal_I = %f A \r\n Cal_V = %f V \r\n ", tmp_1028, data_out_V_V, data_out_Cal_I, data_out_Cal_V);
     memcpy(Tx_1_Buffer, str, 160);        
     Write_1_SOCK_Data_Buffer(0, Tx_1_Buffer, 160);
/*
     sprintf(str_1, "\r\n V_V = %f V \r\n", data_out_V_V);
     memcpy(Tx_Buffer, str_1, 80);
     Write_SOCK_Data_Buffer(0, Tx_Buffer, 80);
*/
  }			
  /* do something here */      
  OSTimeDlyHMSM(0u, 0u, 1u, 10u,
  OS_OPT_TIME_HMSM_STRICT,
  &err);
  /* output your data by terminal */ 
  }
}
/*
*********************************************************************************************************
*                                             App_TaskPWM()
*
* Description : This task init App_TaskPWM.
*               
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskEq0Fp' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void  App_TaskPWM (void  *p_arg)
{
	OS_ERR  err;
	/* no more than 559 */ 
  /* static int PWM_ratio = 558; */
	TIM1_GPIO_Config();
	Tim1__Config();      //??'??tim1??tim8
  while (DEF_TRUE) {
    /* do something here */
		if(ccr_temp<559)
					ccr_temp+=5;
		else
					ccr_temp=560;  
		TIM_SetCompare1(TIM1,ccr_temp);
		TIM_SetCompare1(TIM8,ccr_temp);
		if (ccr_temp < 250)
		{
			/* first start up*/
		OSTimeDlyHMSM(0u, 0u, 0u, 50u,
      OS_OPT_TIME_HMSM_STRICT,
      &err);
		}
		else
		{
		OSTimeDlyHMSM(0u, 0u, 0u, 100u,
      OS_OPT_TIME_HMSM_STRICT,
      &err);
		}
  }
}

