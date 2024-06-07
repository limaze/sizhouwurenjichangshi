#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void MyI2C_W_SCL(uint8_t Bitvalue)
{
	GPIO_WriteBit(GPIOB,GPIO_Pin_6,(BitAction)Bitvalue);
	Delay_us(10);
}

void MyI2C_W_SDA(uint8_t Bitvalue)
{
	GPIO_WriteBit(GPIOB,GPIO_Pin_7,(BitAction)Bitvalue);
	Delay_us(10);
}

uint8_t MyI2C_R_SDA(void)
{
	uint8_t Bitvalue;
	Bitvalue=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7);
	Delay_us(10);
	return Bitvalue;
}

void MyI2C_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);
}

int MyI2C_Start()
{
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	if(!MyI2C_R_SDA())return 0;	//SDA线为低电平则总线忙,退出
	MyI2C_W_SDA(0);
	if(MyI2C_R_SDA()) return 0;	//SDA线为高电平则总线出错,退出
	MyI2C_W_SDA(0);
	return 1;	
}
//结束信号
void MyI2C_Stop()
{
	MyI2C_W_SCL(0);
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);
}
//应答
void MyI2C_Ask()
{
	MyI2C_W_SCL(0);
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(1);
	MyI2C_W_SCL(0);
}
//非应答
void MyI2C_NoAsk()
{
	MyI2C_W_SCL(0);
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	MyI2C_W_SCL(0);
}
// 等待回复
int8_t MyI2C_WaitAsk(void) 	 //返回为:=1无ASK,=0有ASK
{
    uint8_t ErrTime = 0;
	MyI2C_W_SCL(0);
	MyI2C_W_SDA(1);			
	MyI2C_W_SCL(1);
	while(MyI2C_R_SDA())
	{
		ErrTime++;
		if(ErrTime>50)
		{
			MyI2C_Stop();
			return 1;
		}
	}
	MyI2C_W_SCL(0);
	return 0;
}
// IIC发送一个字节
void MyI2C_SendByte(uint8_t SendByte) //数据从高位到低位//
{
    uint8_t i=8;
    while(i--)
    {
       MyI2C_W_SCL(0);
		if(SendByte&0x80)
			MyI2C_W_SDA(1); 
		else 
			MyI2C_W_SDA(0);   
        SendByte<<=1;
		MyI2C_W_SCL(1);
    }
    MyI2C_W_SCL(0);
}  
//读1个字节，ack=1时，发送ACK，ack=0，发送NACK
uint8_t MyI2C_ReadByte(uint8_t ask)  //数据从高位到低位//
{ 
    uint8_t i=8;
    uint8_t ReceiveByte=0;
    MyI2C_W_SDA(1); 			
    while(i--)
    {
		ReceiveByte<<=1;      
		MyI2C_W_SCL(0);
		MyI2C_W_SCL(1);
		if(MyI2C_R_SDA())
		{
			ReceiveByte|=0x01;
		}
    }
    MyI2C_W_SCL(0);
	if (ask)
		MyI2C_Ask();
	else
		MyI2C_NoAsk();  
    return ReceiveByte;
} 
