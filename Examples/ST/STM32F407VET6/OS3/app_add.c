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

void App_TaskW5500_function (void  *p_arg)
{
  OS_ERR  err;
	CPU_TS ts;
  //char str[160];
  W5500_Initial();	
  while (DEF_TRUE) {
  W5500_Socket_Set();
	OSFlagPend(&W5500IntFlagGrp, /* (1) Pointer to event flag group*/
						(OS_FLAGS)(W5500_IntFlag), /* Which bits to wait on*/
						(1000*10), /* Maximum time to wait, 0 means inf */
						OS_OPT_PEND_BLOCKING + /* wait for the flags comming */
						OS_OPT_PEND_FLAG_SET_AND + /* all the bit should be set */
						OS_OPT_PEND_FLAG_CONSUME, /* toggle the comming flags */
						&ts, /* Timestamp of when posted to*/
						&err); 
	if (OS_ERR_TIMEOUT == err)
	{
		/* timeout */
		continue;
	
	}
	/* Interrupt happend */
  W5500_Interrupt_Process();
  if((S0_Data & S_RECEIVE) == S_RECEIVE)
  {
    /* socket0 received data */
    S0_Data&=~S_RECEIVE;
    /* receive data and re-send it */
    Process_Socket_Data(0);
  }
  /* send data */
  if(S0_State == (S_INIT|S_CONN))
  {
    S0_Data&=~S_TRANSMITOK;
    // memcpy(Tx_Buffer, "\r\nWelcome To W5500_0!\r\n", 23);	
    memcpy(Tx_Buffer, "\r\nWelcome To W5500_0!", 21);	
    /* socket 0 send data, size 23 byte */
    Write_SOCK_Data_Buffer(0, Tx_Buffer, (21));
    /* output current */
     //sprintf(str, "\r\n V_I = %f V \r\n V_V = %f V \r\n Cal_I = %f A \r\n Cal_V = %f V \r\n ", V_I_output, V_V_output, Cal_V_I_output, Cal_V_V_output);
    // memcpy(Tx_Buffer, str, 160);        
    // Write_SOCK_Data_Buffer(0, Tx_Buffer, 160);
  }			
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
  OS_ERR err_1;
  CPU_TS ts;
  //OS_FLAGS which_flags;
	/* no more than 559 */ 
  /* static int PWM_ratio = 558; */
  TIM1_GPIO_Config();
  Buck_GPIO_Config();
  BUCK(0);
  Tim1__Config();
  LED_GPIO_Config();
	  /* create a FLAG to wait for data comming */
  static int up = 5;
 TIM_SetCompare1(TIM1,125);
  TIM_SetCompare1(TIM8,125);
  while (DEF_TRUE) {
  OSFlagPend(&MyEventFlagGrp, /* (1) Pointer to event flag group*/
                            (OS_FLAGS)(TimeComming), /* Which bits to wait on*/
                            0, /* Maximum time to wait, 0 means inf */
                            OS_OPT_PEND_BLOCKING + /* wait for the flags comming */
                            OS_OPT_PEND_FLAG_SET_AND + /* the bit should be set */
														OS_OPT_PEND_FLAG_CONSUME, /* toggle the comming flags */
                            &ts, /* Timestamp of when posted to*/
                            &err_1); 
    /* do something here */
#if 1
  while (DEF_TRUE) {
        BUCK(0);
		OSTimeDlyHMSM(0u, 0u, 0u, 40u,
									OS_OPT_TIME_HMSM_STRICT,
									&err);
        LED1_TOGGLE;
        //BUCK(1);
        OSTimeDlyHMSM(0u, 0u, 0u, 10u,
                            OS_OPT_TIME_HMSM_STRICT,
                            &err);

  }
#endif
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
void  App_TaskW5500 (void  *p_arg)
{
  OS_ERR  err;
  CPU_TS ts;
  //char str[160];
  W5500_Initial();	
  while (DEF_TRUE) {
  W5500_Socket_Set();
  /* wait for interruption happen */
	OSFlagPend(&W5500IntFlagGrp, /* (1) Pointer to event flag group*/
							(OS_FLAGS)(W5500_IntFlag), /* Which bits to wait on*/
							(1000*10), /* Maximum time to wait, 0 means inf */
							OS_OPT_PEND_BLOCKING + /* wait for the flags comming */
							OS_OPT_PEND_FLAG_SET_AND + /* all the bit should be set */
							OS_OPT_PEND_FLAG_CONSUME, /* toggle the comming flags */
							&ts, /* Timestamp of when posted to*/
							&err); 
		if (OS_ERR_TIMEOUT == err)
		{
			/* timeout */
			continue;
		
		}
		/* Interrupt happend */
		W5500_Interrupt_Process();
  if((S0_Data & S_RECEIVE) == S_RECEIVE)
  {
    /* socket0 received data */
    S0_Data&=~S_RECEIVE;
    /* receive data and re-send it */
    Process_Socket_Data(0);
  }
  /* send data back  */
  if(S0_State == (S_INIT|S_CONN))
  {
    S0_Data&=~S_TRANSMITOK;
    // memcpy(Tx_Buffer, "\r\nWelcome To W5500_0!\r\n", 23);	
    memcpy(Tx_Buffer, "\r\nWelcome To W5500_0!", 21);	
    /* socket 0 send data, size 23 byte */
    Write_SOCK_Data_Buffer(0, Tx_Buffer, (21));
    /* output current */
     //sprintf(str, "\r\n V_I = %f V \r\n V_V = %f V \r\n Cal_I = %f A \r\n Cal_V = %f V \r\n ", V_I_output, V_V_output, Cal_V_I_output, Cal_V_V_output);
    // memcpy(Tx_Buffer, str, 160);        
    // Write_SOCK_Data_Buffer(0, Tx_Buffer, 160);
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
	CPU_TS ts;
	W5500_1_Initial();
	while (DEF_TRUE) {
	W5500_1_Socket_Set();
	OSFlagPend(&W5500IntFlagGrp, /* (1) Pointer to event flag group*/
				(OS_FLAGS)(W5500_1_IntFlag), /* Which bits to wait on*/
				(1000*10), /* Maximum time to wait, 0 means inf */
				OS_OPT_PEND_BLOCKING + /* wait for the flags comming */
				OS_OPT_PEND_FLAG_SET_AND + /* all the bit should be set */
				OS_OPT_PEND_FLAG_CONSUME, /* toggle the comming flags */
				&ts, /* Timestamp of when posted to*/
				&err); 
	if (OS_ERR_TIMEOUT == err)
	{
		/* timeout */
		continue;
	}
    /* Interrupt happened */
    W5500_1_Interrupt_Process();
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
    memcpy(Tx_1_Buffer, "\r\nWelcome To W5500_1!", 21);	
    /* socket 0 send data, size 23 byte */
    Write_1_SOCK_Data_Buffer(0, Tx_1_Buffer, (21));
    /* output current */
     //sprintf(str, "\r\n V_I = %f V \r\n V_V = %f V \r\n Cal_I = %f A \r\n Cal_V = %f V \r\n ", tmp_1028, data_out_V_V, data_out_Cal_I, data_out_Cal_V);
     //memcpy(Tx_1_Buffer, str, 160);        
    // Write_1_SOCK_Data_Buffer(0, Tx_1_Buffer, 160);
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




