#include "NRF2401.h"                  // Device header
#include "MySPI.h"
#include "Delay.h"

const uint8_t TX_ADDRESS[TX_ADD_Width]={0x34,0x43,0x10,0x10,0x01}; //发送地址
const uint8_t RX_ADDRESS[RX_ADD_Width]={0x34,0x43,0x10,0x10,0x01}; //接收地址

void NRF2401_Init(void)
{
	MySPI_Init();
}

uint8_t NRF2401_Write_Reg(uint8_t add,uint8_t dat)
{
	uint8_t dats;
	MySPI_start();
	dats=MySPI_swapbyte(add);
	MySPI_swapbyte(dat);
	MySPI_stop();
	return dats;//返回寄存器的状态值
}

uint8_t NRF2401_Read_Reg(uint8_t add)
{
	uint8_t dats;
	MySPI_start();
	MySPI_swapbyte(add);
	dats=MySPI_swapbyte(0xFF);
	MySPI_stop();
	return dats;//返回读取到的数值
}

uint8_t NRF2401_Write_Data(uint8_t add,uint8_t *pdat,uint8_t len)
{
	uint8_t statu,temp;
	MySPI_start();
	statu=MySPI_swapbyte(add);
	for(temp=0;temp<len;temp++)
	{
		MySPI_swapbyte(*pdat++);
	}
	MySPI_stop();
	return statu;
}

uint8_t NRF2401_Read_Data(uint8_t add,uint8_t *pdat,uint8_t len)
{
	uint8_t statu,temp;
	MySPI_start();
	statu=MySPI_swapbyte(add);
	for(temp=0;temp<len;temp++)
	{
		pdat[temp]=MySPI_swapbyte(0xFF);
	}
	MySPI_stop();
	return statu;
}

void Set_TXMode(void)
{
	MySPI_start();	    
	//写TX节点地址 
	NRF2401_Write_Data(SPI_WRITE_REG+TX_ADDR,(uint8_t *)TX_ADDRESS,TX_ADD_Width);    
	//设置RX节点地址,使能ACK（应答）		  
	NRF2401_Write_Data(SPI_WRITE_REG+RX_ADDR_P0,(uint8_t *)RX_ADDRESS,RX_ADD_Width); 
	//使能通道0的自动应答    
	NRF2401_Write_Reg(SPI_WRITE_REG+EN_AA,0x01);     
	//使能通道0的接收地址  
	NRF2401_Write_Reg(SPI_WRITE_REG+EN_RXADDR,0x01); 
	//设置自动重发间隔时间:750us+86us ;最大自动重发次数:5次
	NRF2401_Write_Reg(SPI_WRITE_REG+SETUP_RETR,0x25);
	//设置RF通道工作频率	
	NRF2401_Write_Reg(SPI_WRITE_REG+RF_CH,40);       
	//设置TX发射参数,0db增益,250Kbps,低噪声增益开启   
	NRF2401_Write_Reg(SPI_WRITE_REG+RF_SETUP,0x26);  
	//设置为发送模式PTX,使能PWR_UP、EN_CRC,使能CRC为2字节，开启RX_DR、TX_DS、MAX_RT中断引脚 
	NRF2401_Write_Reg(SPI_WRITE_REG+CONFIG,0x0e); 
	MySPI_stop();	
	Delay_ms(12);
}

void Set_RXMode(void)
{
	MySPI_start();	  
	//写RX节点地址
	NRF2401_Write_Data(SPI_WRITE_REG+RX_ADDR_P0,(uint8_t *)RX_ADDRESS,RX_ADD_Width);
	//使能通道0的自动应答    
	NRF2401_Write_Reg(SPI_WRITE_REG+EN_AA,0x01);    
	//使能通道0的接收地址  	 
	NRF2401_Write_Reg(SPI_WRITE_REG+EN_RXADDR,0x01);
	//设置RF通道工作频率		  
	NRF2401_Write_Reg(SPI_WRITE_REG+RF_CH,40);	     
	//选择通道0的有效数据宽度 	    
	NRF2401_Write_Reg(SPI_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);
	//设置TX发射参数,0db增益,250Kbps,低噪声增益开启  
	NRF2401_Write_Reg(SPI_WRITE_REG+RF_SETUP,0x26);
	//设置为接收模式PRX,使能PWR_UP、EN_CRC,使能CRC为2字节，开启RX_DR、TX_DS、MAX_RT中断引脚 
	NRF2401_Write_Reg(SPI_WRITE_REG+CONFIG, 0x0f); 
	//CE为高,进入接收模式 
	MySPI_stop();
}

uint8_t NRF2401_TXPACKET(uint8_t *dat)
{
	uint8_t state;
	MySPI_start();
	NRF2401_Write_Data(WR_TX_PLOAD,dat,TX_PLOAD_WIDTH);
	MySPI_stop();
	
	while(NRF2401_IRQ!=0);
	state=NRF2401_Read_Reg(STATUS);
	NRF2401_Write_Reg(SPI_WRITE_REG+STATUS,state);
	if(state&MAX_TX)
	{
		NRF2401_Write_Reg(FLUSH_TX,0xFF);
		return MAX_TX;
	}
	if(state&TX_OK)
	{
		return TX_OK;
	}
	return 0xFF;
}

uint8_t NRF2401_RXPACKER(uint8_t *dat)
{
	uint8_t state;
	MySPI_stop();
	while(NRF2401_IRQ!=0);
	MySPI_start();
	state=NRF2401_Read_Reg(STATUS);
	NRF2401_Write_Reg(SPI_WRITE_REG+STATUS,state);
	if(state&RX_OK)
	{
		NRF2401_Read_Data(RD_RX_PLOAD,dat,RX_PLOAD_WIDTH);
		NRF2401_Write_Reg(FLUSH_RX,0xFF);
		return RX_OK;
	}
	return 0;
}
