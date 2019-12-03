#include "bsp_pid.h"
#include "stm32f4xx_dac.h"
extern double tmp_1028_1357;
float setVout_temp_f;
float setVout_temp_i;
float setVout;
typedef struct{		
	float set;		
	float actual;//实际值
	float Kp;	
	float Ki;	
	float e_0;		
	float e_1;
	float in_sum;//积分项累加值
	float ctr;	
}PID;
PID pid_v;
void pidinit_v(void)
{
	pid_v.actual=0;
	pid_v.ctr=0;
	pid_v.e_0=0;
	pid_v.Ki=0.1;
	pid_v.Kp=170.625;
	pid_v.set=0;
	pid_v.in_sum=0;
}
void pidcalc_v(void)
{
	pid_v.e_0= pid_v.set-pid_v.actual;
	pid_v.in_sum+=pid_v.e_0;
	pid_v.ctr = pid_v.Kp*pid_v.e_0 + pid_v.Ki*pid_v.in_sum ;
	if(pid_v.in_sum>(float)3.3)
		pid_v.in_sum=3.3;
	if(pid_v.in_sum<0)
		pid_v.in_sum=0;
	if(pid_v.ctr>4095)
		pid_v.ctr=4095;
	if(pid_v.ctr<0)
		pid_v.ctr=0;
}
void pidrun(void)
{
	    pid_v.set = setVout;
			pid_v.actual=tmp_1028_1357;
			pidcalc_v( );
			setVout_temp_f+=pid_v.ctr;
	    setVout_temp_i=setVout_temp_f;
	    DAC_SetChannel2Data(DAC_Align_12b_R, setVout_temp_i);
}

