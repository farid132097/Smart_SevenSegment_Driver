

#include "stm32g030xx.h"
#include "adc.h"

void ADC_Struct_Init(void){
	
}


void ADC_Reg_Init(void){
	//ADC Clock Enable
	RCC->APBENR2 |= RCC_APBENR2_ADCEN;
	//ADC Clock HSI16
	RCC->CCIPR   &=~RCC_CCIPR_ADCSEL_Msk;
	RCC->CCIPR   |= RCC_CCIPR_ADCSEL_1;
  //Disable ADC before config
	if(ADC1->CR & ADC_CR_ADEN){
	  ADC1->CR|=ADC_CR_ADDIS;
	}
  while((ADC1->CR & ADC_CR_ADEN)==ADC_CR_ADEN){
	  //add timeout
	}
	//ADC Reinitialize
	ADC1->CR = 0x00;
	//Enable ADC Regulator
	ADC1->CR|= ADC_CR_ADVREGEN;
	//Wait for ADC Regulator to stabilize
	for(uint32_t i=0; i<1600000; i++){
		__NOP();
	}
	//Start ADC calibration
  ADC1->CR|= ADC_CR_ADCAL;
	//Wait until calibration is completed
	while(ADC1->CR & ADC_CR_ADCAL){
	  //add timeout
	}
	ADC1->CFGR1=0x00;
	ADC1->CFGR2=0x00;
	ADC1->SMPR=0x00;
	ADC1->SMPR|=ADC_SMPR_SMP1_0|ADC_SMPR_SMP1_1|ADC_SMPR_SMP1_2;
	ADC1->SMPR|=ADC_SMPR_SMP2_0|ADC_SMPR_SMP2_1|ADC_SMPR_SMP2_2;
	//Select Channel VrefInt
	ADC1->CHSELR=(1<<13);
	while((ADC1->ISR & ADC_ISR_CCRDY)==0){
		//add timeout
	}
	//Prescale ADC clock to 2MHz, Enable Vref
	ADC->CCR = (0x08<<ADC_CCR_PRESC_Pos) | ADC_CCR_VREFEN;
	//Clear ADC ready flag
	if(ADC1->ISR & ADC_ISR_ADRDY){
		ADC1->ISR|=ADC_ISR_ADRDY;
	}
	//Enable ADC
	ADC1->CR|=ADC_CR_ADEN;
	//Wait Until ADC is ready
  while((ADC1->ISR & ADC_ISR_ADRDY) == 0){
		//add timeout
	}
	//ADC->CCR|=ADC_CCR_VREFEN;
}


uint16_t ADC_Read_Channel(uint8_t channel){
	//Select channel
	if(ADC1->CHSELR != (1<<channel)){
	  ADC1->CHSELR=(1<<channel);
		//Wait until channel change is effective
		while((ADC1->ISR & ADC_ISR_CCRDY)==0){
		  //add timeout
		}
	}
	//Start ADC conversion
	ADC1->CR|=ADC_CR_ADSTART;
	//Wait for ADC End of Conversion
  while((ADC1->ISR & ADC_ISR_EOC) == 0){
		//add timeout
	}
	uint16_t data=(uint16_t)ADC1->DR;
	return data;
}


void ADC_Init(void){
	ADC_Struct_Init();
	ADC_Reg_Init();
}

