#include "flags_ctrl.h"
//OS_FLAG_GRP W5500IntFlagGRP;


OS_FLAG_GRP MyEventFlagGrp;
OS_FLAG_GRP W5500IntFlagGrp;
void FLAGS_CTRL_Init(void)
{
	OS_ERR err;
	OSFlagCreate(&MyEventFlagGrp,
							(CPU_CHAR     *)"My PWM ctrl Flag Group",
							(OS_FLAGS)0,
							&err);
	/* check err */
	OSFlagCreate(&W5500IntFlagGrp,
							(CPU_CHAR     *)"W5500 Int Flag Grp",
							(OS_FLAGS)0,
							&err);
}

