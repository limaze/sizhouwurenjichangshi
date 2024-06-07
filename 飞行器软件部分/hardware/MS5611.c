#include "MS5611.h"
#include "math.h"

/*
C1 压力灵敏度 SENS|T1
C2  压力补偿  OFF|T1
C3	温度压力灵敏度系数 TCS
C4	温度系数的压力补偿 TCO
C5	参考温度 T|REF
C6 	温度系数的温度 TEMPSENS
*/
uint32_t  Cal_C[7];	        //用于存放PROM中的6组数据1-6
double MSbaro_h;
double OFF_;
float Aux;
/*
dT 实际和参考温度之间的差异
TEMP 实际温度	
*/
uint64_t dT,TEMP;
 
uint32_t yali,Pressure_old,qqp;				//大气压
/*
OFF 实际温度补偿
SENS 实际温度灵敏度
*/
uint64_t OFf,SENS;
uint32_t D1_Pres,D2_Temp;	// 数字压力值,数字温度值
 
uint32_t TEMP2,T2,OFF2,SENS2;	//温度校验值
uint32_t Pres_BUFFER[20];     //数据组
uint32_t Temp_BUFFER[10];     //数据组

void MS5611_RESET(void)
{
	MyI2C_Start();
	MyI2C_SendByte(0xEE);//CSB接地，主机地址：0XEE，否则 0X77
	MyI2C_WaitAsk();
    MyI2C_SendByte(0x1E);//发送复位命令
	MyI2C_WaitAsk();
    MyI2C_Stop();
}

uint8_t MS5611_init(void)
 {	 
  uint8_t  inth,intl;
  int i;
  for (i=1;i<=6;i++) 
	{
		MyI2C_Start();
        MyI2C_SendByte(0xEE);
		MyI2C_WaitAsk();
		MyI2C_SendByte(0xA0 + (i*2));
		MyI2C_WaitAsk();
		MyI2C_Stop();
		Delay_us(5);
		MyI2C_Start();
		MyI2C_SendByte(0xEE+0x01);  //进入接收模式
		Delay_us(1);
		MyI2C_WaitAsk();
		inth=MyI2C_ReadByte(1);  		//带ACK的读数据
		delay_us(1);
		intl = MyI2C_ReadByte(0); 			//最后一个字节NACK
		MyI2C_Stop();
		Cal_C[i]=(((uint16_t)inth << 8) | intl);
	}
    return !Cal_C[0];
 }
/**************************实现函数********************************************
*函数原型:unsigned long MS561101BA_getConversion(void)
*功　　能:    读取 MS561101B 的转换结果 
*******************************************************************************/
unsigned long MS5611_getConversion(uint8_t command)
{
	unsigned long conversion = 0;
	uint8_t temp[3];
	MyI2C_Start();
	MyI2C_SendByte(0xEE); 		//写地址
	MyI2C_WaitAsk();
	MyI2C_SendByte(command); //写转换命令
	MyI2C_WaitAsk();
	MyI2C_Stop();
	delay_ms(10);
	MyI2C_Start();
	MyI2C_SendByte(0xEE); 		//写地址
	MyI2C_WaitAsk();
	MyI2C_SendByte(0);				// start read sequence
	MyI2C_WaitAsk();
	MyI2C_Stop();
	MyI2C_Start();
	MyI2C_SendByte(0xEE+0x01);  //进入接收模式
	MyI2C_WaitAsk();
	temp[0]=MyI2C_ReadByte(1);  //带ACK的读数据  bit 23-16
	temp[1]=MyI2C_ReadByte(1);  //带ACK的读数据  bit 8-15
	temp[2]=MyI2C_ReadByte(0);  //带NACK的读数据 bit 0-7
	MyI2C_Stop();
	conversion=(unsigned long)temp[0]*65536+(unsigned long)temp[1]* 256+(unsigned long)temp[2];
	return conversion;
}
/**************************实现函数********************************************
*函数原型:void MS561101BA_GetTemperature(void)
*功　　能:    读取 温度转换结果 
*******************************************************************************/
void MS5611_GetTemperature(void)
{
	D2_Temp = MS5611_getConversion(0x58);
	delay_ms(10);
	dT=D2_Temp - (((uint32_t)Cal_C[5])<<8);
	TEMP=2000+dT*((uint32_t)Cal_C[6])/8388608;
}
///***********************************************
//  * @brief  读取气压
//  * @param  None
//  * @retval None
//************************************************/
void MS5611_getPressure(void)
{
	D1_Pres= MS5611_getConversion(0x48);
	delay_ms(10);	
	OFF_=(uint32_t)Cal_C[2]*65536+((uint32_t)Cal_C[4]*dT)/128;
	SENS=(uint32_t)Cal_C[1]*32768+((uint32_t)Cal_C[3]*dT)/256;
	if(TEMP<2000)
	{
		Aux = (2000-TEMP)*(2000-TEMP);
		T2 = (dT*dT) / 0x80000000; 
		OFF2 = 2.5f*Aux;
		SENS2 = 1.25f*Aux;
		TEMP = TEMP - T2;
		OFF_ = OFF_ - OFF2;
		SENS = SENS - SENS2;	
	}
    yali= (D1_Pres*SENS/2097152-OFF_)/32768;
	MSbaro_h = 44330*(1-pow(((double)(yali))/((double)(101325)),1.0/5.255));
}

