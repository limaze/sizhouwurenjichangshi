#ifndef __NRF2401_H
#define __NRF2401_H

#include "stm32f10x.h"                  // Device header
#include "NFR2401_FRONT.h"

#define NRF2401_IRQ  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)

#define TX_ADD_Width 5      //发送方地址长度
#define RX_ADD_Width 5      //接收方方地址长度
#define TX_USE_Width 32     //发送方用户数据长度
#define RX_USE_Width 32     //发送方用户数据长度

void NRF2401_Init(void);
uint8_t NRF2401_Write_Reg(uint8_t add,uint8_t dat);
uint8_t NRF2401_Read_Reg(uint8_t add);
uint8_t NRF2401_Write_Data(uint8_t add,uint8_t *pdat,uint8_t len);
uint8_t NRF2401_Read_Data(uint8_t add,uint8_t *pdat,uint8_t len);
void Set_TXMode(void);
void Set_RXMode(void);
uint8_t NRF2401_TXPACKET(uint8_t *dat);
uint8_t NRF2401_RXPACKER(uint8_t *dat);

#endif
