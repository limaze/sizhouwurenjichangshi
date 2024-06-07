#include "MS5611.h"
#include "math.h"

/*
C1 ѹ�������� SENS|T1
C2  ѹ������  OFF|T1
C3	�¶�ѹ��������ϵ�� TCS
C4	�¶�ϵ����ѹ������ TCO
C5	�ο��¶� T|REF
C6 	�¶�ϵ�����¶� TEMPSENS
*/
uint32_t  Cal_C[7];	        //���ڴ��PROM�е�6������1-6
double MSbaro_h;
double OFF_;
float Aux;
/*
dT ʵ�ʺͲο��¶�֮��Ĳ���
TEMP ʵ���¶�	
*/
uint64_t dT,TEMP;
 
uint32_t yali,Pressure_old,qqp;				//����ѹ
/*
OFF ʵ���¶Ȳ���
SENS ʵ���¶�������
*/
uint64_t OFf,SENS;
uint32_t D1_Pres,D2_Temp;	// ����ѹ��ֵ,�����¶�ֵ
 
uint32_t TEMP2,T2,OFF2,SENS2;	//�¶�У��ֵ
uint32_t Pres_BUFFER[20];     //������
uint32_t Temp_BUFFER[10];     //������

void MS5611_RESET(void)
{
	MyI2C_Start();
	MyI2C_SendByte(0xEE);//CSB�ӵأ�������ַ��0XEE������ 0X77
	MyI2C_WaitAsk();
    MyI2C_SendByte(0x1E);//���͸�λ����
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
		MyI2C_SendByte(0xEE+0x01);  //�������ģʽ
		Delay_us(1);
		MyI2C_WaitAsk();
		inth=MyI2C_ReadByte(1);  		//��ACK�Ķ�����
		delay_us(1);
		intl = MyI2C_ReadByte(0); 			//���һ���ֽ�NACK
		MyI2C_Stop();
		Cal_C[i]=(((uint16_t)inth << 8) | intl);
	}
    return !Cal_C[0];
 }
/**************************ʵ�ֺ���********************************************
*����ԭ��:unsigned long MS561101BA_getConversion(void)
*��������:    ��ȡ MS561101B ��ת����� 
*******************************************************************************/
unsigned long MS5611_getConversion(uint8_t command)
{
	unsigned long conversion = 0;
	uint8_t temp[3];
	MyI2C_Start();
	MyI2C_SendByte(0xEE); 		//д��ַ
	MyI2C_WaitAsk();
	MyI2C_SendByte(command); //дת������
	MyI2C_WaitAsk();
	MyI2C_Stop();
	delay_ms(10);
	MyI2C_Start();
	MyI2C_SendByte(0xEE); 		//д��ַ
	MyI2C_WaitAsk();
	MyI2C_SendByte(0);				// start read sequence
	MyI2C_WaitAsk();
	MyI2C_Stop();
	MyI2C_Start();
	MyI2C_SendByte(0xEE+0x01);  //�������ģʽ
	MyI2C_WaitAsk();
	temp[0]=MyI2C_ReadByte(1);  //��ACK�Ķ�����  bit 23-16
	temp[1]=MyI2C_ReadByte(1);  //��ACK�Ķ�����  bit 8-15
	temp[2]=MyI2C_ReadByte(0);  //��NACK�Ķ����� bit 0-7
	MyI2C_Stop();
	conversion=(unsigned long)temp[0]*65536+(unsigned long)temp[1]* 256+(unsigned long)temp[2];
	return conversion;
}
/**************************ʵ�ֺ���********************************************
*����ԭ��:void MS561101BA_GetTemperature(void)
*��������:    ��ȡ �¶�ת����� 
*******************************************************************************/
void MS5611_GetTemperature(void)
{
	D2_Temp = MS5611_getConversion(0x58);
	delay_ms(10);
	dT=D2_Temp - (((uint32_t)Cal_C[5])<<8);
	TEMP=2000+dT*((uint32_t)Cal_C[6])/8388608;
}
///***********************************************
//  * @brief  ��ȡ��ѹ
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

