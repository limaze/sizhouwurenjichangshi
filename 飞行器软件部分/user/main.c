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

float Pitch,Roll,Yaw;								//������Ĭ�ϸ���ֵһ���������ǣ�ƫ����
int16_t ax,ay,az,gx,gy,gz;							//���ٶȣ������ǽ��ٶ�

u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz);
u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az);

float Kp,Ki,Kd;//����ֱ��������
float Kdp,Kdi,Kdd;//����ֱ��������
float TKp,TKd;//ת�򻷵Ĳ���

float zzlshiji,zzllilun,zhongzhi=0;
float czlshiji,czllilun;
int tlilun,tshiji=0;

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
	while(1)
	{
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
	int pwm_zhuan,pwm_out,pwm1_out,pwm2_out,pwm3_out,pwm4_out;
	if(EXTI_GetITStatus(EXTI_Line13)==SET)
	{
		MPU6050_DMP_Get_Data(&Pitch,&Roll,&Yaw);				
		MPU_Get_Gyroscope(&gx,&gy,&gz);
		zzlshiji=Roll;
		zzllilun=zhongzhi;
		pwm_out=zzhilihuan(zzllilun,zzlshiji);
		czlshiji=Pitch;
		czllilun=zhongzhi;
		pwm_out=czhilihuan(czllilun,czlshiji);
		pwm_zhuan=zhuanxianghuan(tlilun,tshiji);
		if(tlilun-tshiji>=0)
		{
			pwm1_out=pwm4_out=pwm_out+pwm_zhuan;
			pwm2_out=pwm3_out=pwm_out-pwm_zhuan;
		}
		else{
			pwm1_out=pwm4_out=pwm_out-pwm_zhuan;
			pwm2_out=pwm3_out=pwm_out+pwm_zhuan;
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
	if(EXTI_GetITStatus(EXTI_Line9)==SET)
	{
		
		EXTI_ClearITPendingBit(EXTI_Line9);
	}
}
