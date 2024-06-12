#include "stm32f10x.h"                  // Device header

int16_t AD_value[2];
int16_t ADs_value[2];

void AD_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,2,ADC_SampleTime_28Cycles5);
	
	ADC_InitTypeDef ADC_InitStucture;
	ADC_InitStucture.ADC_ContinuousConvMode=ENABLE;
	ADC_InitStucture.ADC_DataAlign=ADC_DataAlign_Right;
	ADC_InitStucture.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
	ADC_InitStucture.ADC_Mode=ADC_Mode_Independent;
	ADC_InitStucture.ADC_NbrOfChannel=2;
	ADC_InitStucture.ADC_ScanConvMode=ENABLE;
	ADC_Init(ADC1,&ADC_InitStucture);
	
	DMA_InitTypeDef DMAInitStructure;
	DMAInitStructure.DMA_BufferSize=3;
	DMAInitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;
	DMAInitStructure.DMA_M2M=DMA_M2M_Disable;
	DMAInitStructure.DMA_MemoryBaseAddr=(uint32_t)&AD_value;
	DMAInitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
	DMAInitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
	DMAInitStructure.DMA_Mode=DMA_Mode_Circular;
	DMAInitStructure.DMA_PeripheralBaseAddr=(uint32_t)&ADC1->DR;
	DMAInitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
	DMAInitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	DMAInitStructure.DMA_Priority=DMA_Priority_Medium;
	DMA_Init(DMA1_Channel1,&DMAInitStructure);
	
	DMA_Cmd(DMA1_Channel1,ENABLE);
	
	ADC_DMACmd(ADC1,ENABLE);
	
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1)==SET);
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1)==SET);
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
}

int16_t ADx_huoqu(void)//获取摇杆的左右摇晃
{
	ADs_value[0]=AD_value[0]/4095*100;
	return ADs_value[0];
}

int16_t ADy_huoqu(void)//获取摇杆的前后摇晃
{
	ADs_value[1]=AD_value[1]/4095*100;
	return ADs_value[1];
}
