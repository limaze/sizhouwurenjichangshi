#ifndef __PID_H
#define __PID_H

#include "stm32f10x.h"                  // Device header

uint8_t over_flag(float jiaodu);
void Xianfu(int max);
int zzhilihuan(float lilun,float shiji);
int czhilihuan(float lilun,float shiji);
int zhuanxianghuan(int tlilun,int tshiji);

#endif
