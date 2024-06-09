#ifndef __MS5611_H
#define __MS5611_H

#include "MyI2C.h"  
#include "Delay.h"

#define SCTemperature    			0x01		  //��ʼ�¶�ת��
#define CTemperatureing  			0x02 		  //����ת���¶�
#define SCPressure  	 			0x03	      //��ʼ��ѹת��
#define SCPressureing    			0x04	  	  //����ת����ѹ
#define MS561101BA_ADC_RD      	    0x00
#define	MS561101BA_PROM_RD 	        0xA0
#define MS561101BA_PROM_CRC         0xAE

#define MS561101BA_SlaveAddress     0xEE  		  //MS5611�ĵ�ַ
#define MS561101BA_RST              0x1E  		  //cmd ��λ
 
#define	MS561101BA_D2_OSR_4096   	0x58	      // 9.04 mSec conversion time ( 110.62 Hz)
#define	MS561101BA_D1_OSR_4096   	0x48
 
#define MS5611_OSR256				0x40
#define MS5611_OSR512				0x42
#define MS5611_OSR1024				0x44
#define MS5611_OSR2048				0x46
#define MS5611_OSR4096				0x48
#define FILTER_num 20
 
uint8_t  MS5611_Init(void);
void MS5611_RESET(void);
unsigned long MS5611_getConversion(uint8_t command);
void MS5611_getPressure(void);
void MS5611_getTemperature(void);
extern uint64_t dT,TEMP;
extern uint32_t yali;
extern uint32_t  Cal_C[7];
double xddaodu(void);
double gaodu(void);

#endif
