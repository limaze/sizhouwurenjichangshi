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

float zzlshiji,zzllilun,zhongzhi=0;
float czlshiji,czllilun;

int main(void)
{
	OLED_Init();
	MPU6050_Init();
	MPU6050_DMP_Init();
	NRF2401_Init();
	EXTI1_Init();
	EXTI2_Init();
	Motor_Init();
	while(1)
	{
		MPU6050_DMP_Get_Data(&Pitch,&Roll,&Yaw);				//读取姿态信息(其中偏航角有飘移是正常现象)
		MPU_Get_Gyroscope(&gx,&gy,&gz);
		MPU_Get_Accelerometer(&ax,&ay,&az);
		OLED_ShowSignedNum(2, 1, Pitch, 5);
		OLED_ShowSignedNum(3, 1, Roll, 5);
		OLED_ShowSignedNum(4, 1, Yaw, 5);
		OLED_ShowSignedNum(2, 8, gx, 5);
		OLED_ShowSignedNum(3, 8, gy, 5);
		OLED_ShowSignedNum(4, 8, gz, 5);
	}
}

void EXTI15_10_IRQHandler(void)
{
	int pwm_zhuan,pwm1_out,pwm2_out;
	if(EXTI_GetITStatus(EXTI_Line13)==SET)
	{
		MPU6050_DMP_Get_Data(&Pitch,&Roll,&Yaw);				
		MPU_Get_Gyroscope(&gx,&gy,&gz);
		zzlshiji=Roll;
		zzllilun=zhongzhi;
		pwm1_out=zzhilihuan(zzllilun,zzlshiji);
		czlshiji=Pitch;
		czllilun=zhongzhi;
		pwm2_out=czhilihuan(czllilun,czlshiji);
		pwxianfu(7000,&pwm1_out);
		pwxianfu(7000,&pwm2_out);
		SETPWM_1(pwm1_out);
		SETPWM_2(pwm2_out);
		EXTI_ClearITPendingBit(EXTI_Line13);
	}
}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line9)==SET)
	{
		
		EXTI_ClearITPendingBit(EXTI_Line9);
	}
}
