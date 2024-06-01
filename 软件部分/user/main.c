#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "MPU6050.h"
#include "inv_mpu.h"
#include "NRF2401.h"
#include "EXTI.h"
#include "EXTI2.h"

float Pitch,Roll,Yaw;								//俯仰角默认跟中值一样，翻滚角，偏航角
int16_t ax,ay,az,gx,gy,gz;							//加速度，陀螺仪角速度

u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz);
u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az);

int main(void)
{
	OLED_Init();
	MPU6050_Init();
	MPU6050_DMP_Init();
	NRF2401_Init();
	EXTI1_Init();
	EXTI2_Init();
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
	if(EXTI_GetITStatus(EXTI_Line13)==SET)
	{
		MPU6050_DMP_Get_Data(&Pitch,&Roll,&Yaw);				
		MPU_Get_Gyroscope(&gx,&gy,&gz);
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
