#include "stm32f10x.h"                  // Device header

#include "PWM.h"

void Motor_Init(void)
{
	PWM_Init();
}

void pwxianfu(int max,int*pw)
{
	if(*pw>max)*pw=max;
	if(*pw<-max)*pw=-max;
}

void SETPWM_1(int pw)
{
	if(pw>0)
	{
		PWM_Setcompare1(pw);
	}
	else{
		PWM_Setcompare1(-pw);
	}
}

void SETPWM_2(int pw)
{
	if(pw>0)
	{
		PWM_Setcompare2(pw);
	}
	else{
		PWM_Setcompare2(-pw);
	}
}

void SETPWM_3(int pw)
{
	if(pw>0)
	{
		PWM_Setcompare3(pw);
	}
	else{
		PWM_Setcompare3(-pw);
	}
}

void SETPWM_4(int pw)
{
	if(pw>0)
	{
		PWM_Setcompare4(pw);
	}
	else{
		PWM_Setcompare4(-pw);
	}
}
