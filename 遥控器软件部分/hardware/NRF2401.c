#include "NRF2401.h"                  // Device header
#include "MySPI.h"
#include "Delay.h"

const uint8_t TX_ADDRESS[TX_ADD_Width]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ
const uint8_t RX_ADDRESS[RX_ADD_Width]={0x34,0x43,0x10,0x10,0x01}; //���յ�ַ

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
	return dats;//���ؼĴ�����״ֵ̬
}

uint8_t NRF2401_Read_Reg(uint8_t add)
{
	uint8_t dats;
	MySPI_start();
	MySPI_swapbyte(add);
	dats=MySPI_swapbyte(0xFF);
	MySPI_stop();
	return dats;//���ض�ȡ������ֵ
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
	//дTX�ڵ��ַ 
	NRF2401_Write_Data(SPI_WRITE_REG+TX_ADDR,(uint8_t *)TX_ADDRESS,TX_ADD_Width);    
	//����RX�ڵ��ַ,ʹ��ACK��Ӧ��		  
	NRF2401_Write_Data(SPI_WRITE_REG+RX_ADDR_P0,(uint8_t *)RX_ADDRESS,RX_ADD_Width); 
	//ʹ��ͨ��0���Զ�Ӧ��    
	NRF2401_Write_Reg(SPI_WRITE_REG+EN_AA,0x01);     
	//ʹ��ͨ��0�Ľ��յ�ַ  
	NRF2401_Write_Reg(SPI_WRITE_REG+EN_RXADDR,0x01); 
	//�����Զ��ط����ʱ��:750us+86us ;����Զ��ط�����:5��
	NRF2401_Write_Reg(SPI_WRITE_REG+SETUP_RETR,0x25);
	//����RFͨ������Ƶ��	
	NRF2401_Write_Reg(SPI_WRITE_REG+RF_CH,40);       
	//����TX�������,0db����,250Kbps,���������濪��   
	NRF2401_Write_Reg(SPI_WRITE_REG+RF_SETUP,0x26);  
	//����Ϊ����ģʽPTX,ʹ��PWR_UP��EN_CRC,ʹ��CRCΪ2�ֽڣ�����RX_DR��TX_DS��MAX_RT�ж����� 
	NRF2401_Write_Reg(SPI_WRITE_REG+CONFIG,0x0e); 
	MySPI_stop();	
	Delay_ms(12);
}

void Set_RXMode(void)
{
	MySPI_start();	  
	//дRX�ڵ��ַ
	NRF2401_Write_Data(SPI_WRITE_REG+RX_ADDR_P0,(uint8_t *)RX_ADDRESS,RX_ADD_Width);
	//ʹ��ͨ��0���Զ�Ӧ��    
	NRF2401_Write_Reg(SPI_WRITE_REG+EN_AA,0x01);    
	//ʹ��ͨ��0�Ľ��յ�ַ  	 
	NRF2401_Write_Reg(SPI_WRITE_REG+EN_RXADDR,0x01);
	//����RFͨ������Ƶ��		  
	NRF2401_Write_Reg(SPI_WRITE_REG+RF_CH,40);	     
	//ѡ��ͨ��0����Ч���ݿ�� 	    
	NRF2401_Write_Reg(SPI_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);
	//����TX�������,0db����,250Kbps,���������濪��  
	NRF2401_Write_Reg(SPI_WRITE_REG+RF_SETUP,0x26);
	//����Ϊ����ģʽPRX,ʹ��PWR_UP��EN_CRC,ʹ��CRCΪ2�ֽڣ�����RX_DR��TX_DS��MAX_RT�ж����� 
	NRF2401_Write_Reg(SPI_WRITE_REG+CONFIG, 0x0f); 
	//CEΪ��,�������ģʽ 
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
