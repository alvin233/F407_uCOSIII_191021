#ifndef  APP_ADD_H
#define  APP_ADD_H
#include "includes.h"
#include "app_cfg.h"
extern  double V_I_output;
extern double V_V_output;
extern double Cal_V_I_output;
extern double Cal_V_V_output;


static  OS_TCB       App_TaskPWMTCB;
static  CPU_STK      App_TaskPWMStk[APP_CFG_TASK_PWM_STK_SIZE];

static  OS_TCB       App_TaskW5500_1TCB;
static  CPU_STK      App_TaskW5500_1Stk[APP_CFG_TASK_W5500_1_STK_SIZE];

static  OS_TCB       App_TaskW5500TCB;
static  CPU_STK      App_TaskW5500Stk[APP_CFG_TASK_W5500_STK_SIZE];

void  App_TaskW5500_function         (void  *p_arg); 
/* Init W5500, make connection and send data to PC */
/* This prototype is in the primary side of the project */
void  App_TaskW5500_1         (void  *p_arg); 
/* Init W5500, make connection and send data to PC */
void  App_TaskW5500         (void  *p_arg); 
/* DC voltage output ctl */
void  App_Task_DCVolCtl          (void  *p_arg);
void  App_TaskPWM (void  *p_arg);



#endif
