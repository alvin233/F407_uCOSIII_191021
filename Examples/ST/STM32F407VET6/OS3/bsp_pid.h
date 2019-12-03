#ifndef BSP_PID_H
#define BSP_PID_H
#include "stm32f4xx.h"
void pidinit_v(void);
void pidcalc_v(void);
void pidrun(void);
extern float setVout_temp_f;
extern int setVout_temp_i;
extern float setVout;
#endif

