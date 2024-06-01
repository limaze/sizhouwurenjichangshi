#include "stm32f10x.h"                  // Device header
#include "MySPI.h"
#include "NFR2401_FRONT.h"

void NRF2401_Init(void)
{
	MySPI_Init();
}
