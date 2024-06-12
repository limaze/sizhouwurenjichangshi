#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "AD.h"
#include "Key.h"
#include "NRF2401.h"

uint8_t keynum,xs,ys;
uint8_t a1=1,a2=2,a3=3,a4=4,a5=5,a6=6;
int8_t x,y;
int16_t x1=0,y1=0;

int main(void)
{
	OLED_Init();
	AD_Init();
	Key_Init();
	NRF2401_Init();
	Set_TXMode();
	OLED_ShowString(1,1,"High:");
	OLED_ShowString(2,1,"X:");
	OLED_ShowString(3,1,"Y:");
	while(1)
	{
		keynum=Key_Getnum();
		switch(keynum)
		{
			case 1:NRF2401_TXPACKET(&a1);break;//前进
			case 2:NRF2401_TXPACKET(&a2);break;//后退
			case 3:NRF2401_TXPACKET(&a3);break;//向左
			case 4:NRF2401_TXPACKET(&a4);break;//向右
			case 5:NRF2401_TXPACKET(&a5);break;//向前
			case 6:NRF2401_TXPACKET(&a6);break;//向后
		}
		x=ADx_huoqu();
		y=ADy_huoqu();
		if(x>=0)
		{
			if((x-x1)<-20||(x-x1)>20)
			{
				xs=x;
				NRF2401_TXPACKET(&xs);
			}
		}
		else{
			if((x-x1)<-20||(x-x1)>20)
			{
				xs=2*x;
				NRF2401_TXPACKET(&xs);
			}
		}
		if(y>=0)
		{
			if((y-y1)<-20||(y-y1)>20)
			{
				ys=y;
				NRF2401_TXPACKET(&ys);
			}
		}
		else{
			if((y-y1)<-20||(y-y1)>20)
			{
				ys=2*y;
				NRF2401_TXPACKET(&ys);
			}
		}
		x1=x;
		y1=y;
	}
}
