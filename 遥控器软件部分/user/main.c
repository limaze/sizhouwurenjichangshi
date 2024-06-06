#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"

int main(void)
{
	OLED_Init();
	OLED_ShowNum(2,1,520,3);
	OLED_ShowSignedNum(2,6,520,3);
	OLED_ShowHexNum(3,1,0x32,4);
	OLED_ShowBinNum(4,1,0x32,8);
	while(1)
	{
		
	}
}
