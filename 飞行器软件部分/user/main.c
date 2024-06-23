#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "MPU6050.h"
#include "inv_mpu.h"
#include "NRF2401.h"
#include "EXTI.h"
#include "EXTI2.h"
#include "MOTOR.h"
#include "MS5611.h"
#include "PID.h"

float Pitch,Roll,Yaw;								//俯仰角默认跟中值一样，翻滚角，偏航角
int16_t ax,ay,az,gx,gy,gz;							//加速度，陀螺仪角速度

u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz);
u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az);

float Kp,Ki,Kd;//正向直立环参数
float Kdp,Kdi,Kdd;//侧向直立环参数
float TKp,TKd;//转向环的参数

float zzlshiji,zzllilun,zhongzhi=0;//正向直立环实际值和理论值
float czlshiji,czllilun;//侧向直立环实际值和理论值
int tlilun,tshiji=0;//转向环理论值和实际值
double hshiji,hlilun;//无人机所处高度实际值和理论值

uint8_t mingling;//对传回的数据进行接收

int main(void)
{
	OLED_Init();
	MPU6050_Init();
	MPU6050_DMP_Init();
	NRF2401_Init();
	EXTI1_Init();
	EXTI2_Init();
	Motor_Init();
	Set_RXMode();
	MS5611_Init();
	while(1)
	{
		hshiji=gaodu();
		OLED_ShowNum(2, 1, hshiji, 5);
		OLED_ShowNum(3, 1, Roll, 5);
//		switch(mingling)
//		{
//			case 1:qianjin;break;
//			case 2:houtui;break;
//			case 3:xiangzuo;break;
//			case 4:xiangyou;break;
//			case 5:xiangqian;break;
//			case 6:xianghou;break;
//		}
	}
}

void EXTI15_10_IRQHandler(void)
{
	int pwm_zhuan,pwm_out,pwm1_out,pwm2_out,pwm3_out,pwm4_out;
	if(EXTI_GetITStatus(EXTI_Line13)==SET)
	{
		MPU6050_DMP_Get_Data(&Pitch,&Roll,&Yaw);				
		MPU_Get_Gyroscope(&gx,&gy,&gz);
		hshiji=gaodu();
		pwm_out=gdkz(hlilun,hshiji);
		zzlshiji=Roll;
		zzllilun=zhongzhi;
		if(zzlshiji>zzllilun)
		{
			pwm2_out=pwm4_out=pwm_out-zzhilihuan(zzllilun,zzlshiji);
			pwm1_out=pwm3_out=pwm_out+zzhilihuan(zzllilun,zzlshiji);
		}
		else{
			pwm1_out=pwm3_out=pwm_out-zzhilihuan(zzllilun,zzlshiji);
			pwm2_out=pwm4_out=pwm_out+zzhilihuan(zzllilun,zzlshiji);
		}
		czlshiji=Pitch;
		czllilun=zhongzhi;
		if(czlshiji>czllilun)
		{
			pwm3_out-=zzhilihuan(zzllilun,zzlshiji);
			pwm4_out-=zzhilihuan(zzllilun,zzlshiji);
			pwm1_out+=zzhilihuan(zzllilun,zzlshiji);
			pwm2_out+=zzhilihuan(zzllilun,zzlshiji);
		}
		else{
			pwm3_out+=zzhilihuan(zzllilun,zzlshiji);
			pwm4_out+=zzhilihuan(zzllilun,zzlshiji);
			pwm1_out-=zzhilihuan(zzllilun,zzlshiji);
			pwm2_out-=zzhilihuan(zzllilun,zzlshiji);
		}
		pwm_zhuan=zhuanxianghuan(tlilun,tshiji);
		if(tlilun-tshiji>=0)
		{
			pwm1_out+=pwm_zhuan;
			pwm4_out+=pwm_zhuan;
			pwm2_out-=pwm_zhuan;
			pwm3_out-=pwm_zhuan;
		}
		else{
			pwm1_out-=pwm_zhuan;
			pwm4_out-=pwm_zhuan;
			pwm2_out+=pwm_zhuan;
			pwm3_out+=pwm_zhuan;
		}
		pwxianfu(7000,&pwm1_out);
		pwxianfu(7000,&pwm2_out);
		pwxianfu(7000,&pwm3_out);
		pwxianfu(7000,&pwm4_out);
		SETPWM_1(pwm1_out);
		SETPWM_2(pwm2_out);
		SETPWM_3(pwm3_out);
		SETPWM_4(pwm4_out);
		EXTI_ClearITPendingBit(EXTI_Line13);
	}
}

void EXTI9_5_IRQHandler(void)
{
	uint8_t a;
	if(EXTI_GetITStatus(EXTI_Line9)==SET)
	{
		if(NRF2401_RXPACKER(&a)==0x40)
		{
			mingling=a;
		}
		EXTI_ClearITPendingBit(EXTI_Line9);
	}
}
