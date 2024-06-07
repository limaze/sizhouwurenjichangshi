#ifndef __MYI2C_H
#define __MYI2C_H

#include "stm32f10x.h"                  // Device header

void MyI2C_Init(void);
void MyI2C_Start(void);
void MyI2C_Stop(void);
int8_t MyI2C_WaitAsk(void);
void MyI2C_SendByte(uint8_t byte);
uint8_t MyI2C_ReadByte(uint8_t ask);

#endif
