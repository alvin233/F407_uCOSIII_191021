/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                             (c) Copyright 2013; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                       IAR Development Kits
*                                              on the
*
*                                    STM32F429II-SK KICKSTART KIT
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : YS
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <includes.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  APP_TASK_EQ_0_ITERATION_NBR              16u


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

                                                                /* ----------------- APPLICATION GLOBALS -------------- */
static  OS_TCB   AppTaskStartTCB;
static  CPU_STK  AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];
                                                                /* ------------ FLOATING POINT TEST TASK -------------- */
static  OS_TCB       App_TaskEq0FpTCB;
static  CPU_STK      App_TaskEq0FpStk[APP_CFG_TASK_EQ_STK_SIZE];

static  OS_TCB       App_TaskW5500TCB;
static  CPU_STK      App_TaskW5500Stk[APP_CFG_TASK_W5500_STK_SIZE];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart          (void     *p_arg);
static  void  AppTaskCreate         (void);
static  void  AppObjCreate          (void);

static  void  App_TaskEq0Fp         (void  *p_arg);             /* Floating Point Equation 0 task.                      */
static  void  App_TaskW5500         (void  *p_arg); 
/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

	double tmp_1028 = 0;
	double tmp_1028_1357 = 0;
        double data_out_V_V = 0;
        double data_out_Cal_I = 0;
        double data_out_Cal_V = 0;
	static double data=0;
	
int main(void)
{
    OS_ERR  err;


    BSP_IntDisAll();                                            /* Disable all interrupts.                              */
    
    CPU_Init();                                                 /* Initialize the uC/CPU Services                       */
    Mem_Init();                                                 /* Initialize Memory Management Module                  */
    Math_Init();                                                /* Initialize Mathematical Module                       */

    OSInit(&err);                                               /* Init uC/OS-III.                                      */

    OSTaskCreate((OS_TCB       *)&AppTaskStartTCB,              /* Create the start task                                */
                 (CPU_CHAR     *)"App Task Start",
                 (OS_TASK_PTR   )AppTaskStart,
                 (void         *)0u,
                 (OS_PRIO       )APP_CFG_TASK_START_PRIO,
                 (CPU_STK      *)&AppTaskStartStk[0u],
                 (CPU_STK_SIZE  )AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY    )0u,
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */

    (void)&err;

    return (0u);
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    OS_ERR  err;


   (void)p_arg;

    BSP_Init();                                                 /* Initialize BSP functions                             */
    BSP_Tick_Init();                                            /* Initialize Tick Services.                            */


#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

    BSP_LED_Off(0u);                                            /* Turn Off LEDs after initialization                   */

    APP_TRACE_DBG(("Creating Application Kernel Objects\n\r"));
    AppObjCreate();                                             /* Create Applicaiton kernel objects                    */

    APP_TRACE_DBG(("Creating Application Tasks\n\r"));
    AppTaskCreate();                                            /* Create Application tasks                             */


    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */

        OSTimeDlyHMSM(0u, 0u, 2u, 100u,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
        /* do something here */


    }
}


/*
*********************************************************************************************************
*                                          AppTaskCreate()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
    OS_ERR  os_err;
    
                                                                /* ------------- CREATE FLOATING POINT TASK ----------- */
    OSTaskCreate((OS_TCB      *)&App_TaskEq0FpTCB,
                 (CPU_CHAR    *)"FP  Equation 1",
                 (OS_TASK_PTR  ) App_TaskEq0Fp, 
                 (void        *) 0,
                 (OS_PRIO      ) APP_CFG_TASK_EQ_PRIO,
                 (CPU_STK     *)&App_TaskEq0FpStk[0],
                 (CPU_STK_SIZE ) App_TaskEq0FpStk[APP_CFG_TASK_EQ_STK_SIZE / 10u],
                 (CPU_STK_SIZE ) APP_CFG_TASK_EQ_STK_SIZE,
                 (OS_MSG_QTY   ) 0u,
                 (OS_TICK      ) 0u,
                 (void        *) 0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 (OS_ERR      *)&os_err);
								 
		OSTaskCreate((OS_TCB      *)&App_TaskW5500TCB,
                (CPU_CHAR     *)"W5500",
                (OS_TASK_PTR   ) App_TaskW5500, 
                (void        *) 0,
                (OS_PRIO      ) APP_CFG_TASK_W5500_PRIO,
                (CPU_STK     *)&App_TaskW5500Stk[0],
                (CPU_STK_SIZE ) App_TaskEq0FpStk[APP_CFG_TASK_W5500_STK_SIZE / 10u],
                (CPU_STK_SIZE ) APP_CFG_TASK_W5500_STK_SIZE,
                (OS_MSG_QTY   ) 0u,
                (OS_TICK      ) 0u,
                (void        *) 0,
                (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                (OS_ERR      *)&os_err);						 
}
/*
*********************************************************************************************************
*                                          AppObjCreate()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  AppObjCreate (void)
{

}
/*
*********************************************************************************************************
*                                             App_TaskEq0Fp()
*
* Description : This task finds the root of the following equation.
*               f(x) =  e^-x(3.2 sin(x) - 0.5 cos(x)) using the bisection mehtod
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskEq0Fp' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  App_TaskEq0Fp (void  *p_arg)
{

  OS_ERR  err;
	#if 0
	double tmp_1028 = 0;
	double tmp_1028_1357 = 0;
	static double data=0;
	#endif
#if 0
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
    memcpy(Tx_Buffer, "\r\nWelcome To YiXinElec!\r\n", 23);	
    /* socket 0 send data, size 23 byte */
    Write_SOCK_Data_Buffer(0, Tx_Buffer, 23);
  }			
  /* do something here */      
  OSTimeDlyHMSM(0u, 0u, 0u, 10u,
  OS_OPT_TIME_HMSM_STRICT,
  &err);
  /* output your data by terminal */
  //APP_TRACE_INFO(("Eq0 Task Running ....\n"));     
  }
#endif 
  Debug_USART_Config();
  Rheostat_Init();
  while (DEF_TRUE) {
      OSTimeDlyHMSM(0u, 0u, 2u, 10u,
        OS_OPT_TIME_HMSM_STRICT,
        &err);	
		average();
			tmp_1028 = (float)IVOUT[0]*times_1e1_vol_per;
			tmp_1028 = tmp_1028 /10;
			tmp_1028 = tmp_1028/4096;
	  // ADC_ConvertedValueLocal[0] =(float)IVOUT[0]*times_1000000_vol_per;
		printf("\r\n V_I = %f V \r\n",tmp_1028);				
		data = (tmp_1028-2.588)*10;// 15*I_p - 2.33*15
                data_out_Cal_I = data;
		printf("\r\n Cal_I value = %f A \r\n",data);
		#if 1
		tmp_1028_1357 =(float) IVOUT[1]*times_1e1_vol_per ;
			tmp_1028_1357 = tmp_1028_1357/10;
			data_out_V_V = tmp_1028_1357/4096;
                        data_out_Cal_V = data_out_V_V*16;
		printf("\r\n V_V = %f V \r\n",data_out_V_V);
			
		tmp_1028_1357=tmp_1028_1357*16;
		printf("\r\n Cal_V = %f V \r\n",tmp_1028_1357);
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
  char str[160];
  char str_1[160];
	#if 1
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
#if 0
		printf("\r\n Cal_I value = %f A \r\n",data);


		printf("\r\n V_V = %f V \r\n",tmp_1028_1357);
			
		tmp_1028_1357=tmp_1028_1357*16;
		printf("\r\n Cal_V = %f V \r\n",tmp_1028_1357);
#endif
    memcpy(Tx_Buffer, "\r\nWelcome To CQU!\r\n", 19);	
    /* socket 0 send data, size 23 byte */
    Write_SOCK_Data_Buffer(0, Tx_Buffer, (15 + 4));
    /* output current */
     sprintf(str, "\r\n V_I = %f V \r\n V_V = %f V \r\n Cal_I = %f A \r\n Cal_V = %f V \r\n ", tmp_1028, data_out_V_V, data_out_Cal_I, data_out_Cal_V);
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
	#else
	while (DEF_TRUE) {
  /* do something here */      
  OSTimeDlyHMSM(0u, 0u, 0u, 10u,
  OS_OPT_TIME_HMSM_STRICT,
  &err);
  }
	#endif
}

