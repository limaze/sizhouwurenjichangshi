#include "Pid.h"

extern float Kp,Ki,Kd;  	//正向直立环参数
float err;              	//此次误差
float last_err;         	//上次误差
float err_sum=0;         	//误差累加
float err_difference;    	//误差的差值

extern float Kdp,Kdi,Kdd; 	//侧向直立环参数
float derr;               	//此次误差
float dlast_err;          	//上次误差
float derr_sum=0;         	//误差累加
float derr_difference;   	//误差的差值

extern float TKp,TKd;    	//转向环的参数
int terr;                	//此次的误差
int tlast_err;           	//上一次的误差
int terr_difference;     	//误差的差值

extern float HKp,HKd;    	//高度控制的参数
double herr;                //此次的误差
double hlast_err;           //上一次的误差
double herr_difference;     //误差的差值

uint8_t over_flag(float jiaodu)
{
	uint8_t off_flag;
	if(jiaodu>35||jiaodu<-35)
	{
		off_flag=1;
		GPIO_ResetBits(GPIOA,GPIO_Pin_4);GPIO_ResetBits(GPIOA,GPIO_Pin_5);
		GPIO_ResetBits(GPIOB,GPIO_Pin_14);GPIO_ResetBits(GPIOB,GPIO_Pin_15);
	}
	else off_flag=0;
	return off_flag;
}

//正向直立环
int zzhilihuan(float lilun,float shiji)
{
	err=lilun-shiji;
	err_sum+=err;
	err_difference=err-last_err;
	last_err=err;
	return Kp*err+Ki*err_sum+Kd*err_difference;
}

//侧向直立环
int czhilihuan(float lilun,float shiji)
{
	derr=lilun-shiji;
	derr_sum+=err;
	derr_difference=err-dlast_err;
	dlast_err=err;
	return Kdp*derr+Kdi*derr_sum+Kdd*derr_difference;
}

//转向环
int zhuanxianghuan(int tlilun,int tshiji)
{
	terr=tshiji-tlilun;
	if(terr>180){terr=terr+360;}
	if(terr<-180){terr=terr-360;}
	terr_difference=terr-tlast_err;
	tlast_err=terr;
	return TKp*terr+TKd*terr_difference;
}

//高度控制
int gdkz(double lilun,double shiji)
{
	derr=lilun-shiji;
	derr_sum+=err;
	derr_difference=err-dlast_err;
	dlast_err=err;
	return Kp*derr+Ki*derr_sum+Kd*derr_difference;
}
