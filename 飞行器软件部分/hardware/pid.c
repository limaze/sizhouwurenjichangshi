#include "Pid.h"

extern float Kp,Ki,Kd;  	 //����ֱ��������
float err;              	 //�˴����
float last_err;         	 //�ϴ����
float err_sum=0;         	//����ۼ�
float err_difference;    	//���Ĳ�ֵ

extern float VKp,VKi;    	//�����ٶȻ�����
float filt_velocity;     	//�˲�����ٶ�
float last_filt_velocity;	//��һ�ε��˲�����ٶ�
float velocity_sum=0;    	//�ٶȵ��ۼ�
int verr;                	//�˴����
int vlast_err;           	//�ϴ����

extern float Kdp,Kdi,Kdd; 	//����ֱ��������
float derr;               	//�˴����
float dlast_err;          	//�ϴ����
float derr_sum=0;         	//����ۼ�
float derr_difference;   	//���Ĳ�ֵ

extern float VdKp,VdKi;   	//�����ٶȻ�����
float filt_dvelocity;     	//�˲�����ٶ�
float last_dfilt_velocity;	//��һ�ε��˲�����ٶ�
float dvelocity_sum=0;    	//�ٶȵ��ۼ�
int dverr;                	//�˴����
int dvlast_err;           	//�ϴ����

extern float TKp,TKd;    	//ת�򻷵Ĳ���
int terr;                	//�˴ε����
int tlast_err;           	//��һ�ε����
int terr_difference;     	//���Ĳ�ֵ

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

void Xianfu(int max)
{
	if(velocity_sum>max){velocity_sum=max;}
	if(velocity_sum<-max){velocity_sum=-max;}
}

//����ֱ����
int zzhilihuan(float lilun,float shiji)
{
	err=lilun-shiji;
	err_sum+=err;
	err_difference=err-last_err;
	last_err=err;
	return Kp*err+Ki*err_sum+Kd*err_difference;
}

//����ֱ����
int czhilihuan(float lilun,float shiji)
{
	derr=lilun-shiji;
	derr_sum+=err;
	derr_difference=err-dlast_err;
	dlast_err=err;
	return Kp*derr+Ki*derr_sum+Kd*derr_difference;
}

//ת��
int zhuanxianghuan(int tlilun,int tshiji)
{
	terr=tshiji-tlilun;
	if(terr>180){terr=terr+360;}
	if(terr<-180){terr=terr-360;}
	terr_difference=terr-tlast_err;
	tlast_err=terr;
	return TKp*terr+TKd*terr_difference;
}
