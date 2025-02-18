

#include "stm32g030xx.h"
#include "ldr.h"
#include "adc.h"
#include "lpf.h"


void LDR_Struct_Init(void){
	
	
}


void LDR_GPIO_Init(void){
	//Enable clock to GPIOB
	RCC->IOPENR  |= RCC_IOPENR_GPIOBEN;
	//Select B0 as Analog Input, ADC channel 8
	GPIOB->MODER |= GPIO_MODER_MODE0_Msk;
}

void LDR_Reg_Init(void){
	ADC_Init();
	//GPIOB0 is ADC_IN8, Dummy Read
	ADC_Read_Channel(8);
}


uint16_t LDR_Read_Brightness(void){
	//Add filter if necessary
	int32_t temp;
	temp = ADC_Read_Channel(8);
	#ifdef LDR_FILTER_EN
	temp = LPF_Get_Filtered_Value(0, temp);
	#endif
	return (uint16_t)temp;
}



void LDR_Init(void){
	LDR_Struct_Init();
	LDR_GPIO_Init();
	LDR_Reg_Init();
	LPF_Init();
	LPF_Set_Alpha(0, 10);
}



