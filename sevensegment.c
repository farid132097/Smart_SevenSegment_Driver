

#include "stm32g030xx.h"
#include "sevensegment.h"


static uint8_t segment_val[24] = {
	0b00111111, //index 0, char '0'
	0b00000110, //index 1, char '1'
	0b01011011, //index 2, char '2'
	0b01001111, //index 3, char '3'
	0b01100110, //index 4, char '4'
	0b01101101, //index 5, char '5'
	0b01111101, //index 6, char '6'
	0b00000111, //index 7, char '7'
	0b01111111, //index 8, char '8'
	0b01101111, //index 9, char '9'
	
	//inverted
	0b00111111, //index 0, char '0'
	0b00110000, //index 1, char '1'
	0b01011011, //index 2, char '2'
	0b01111001, //index 3, char '3'
	0b01110100, //index 4, char '4'
	0b01101101, //index 5, char '5'
	0b01101111, //index 6, char '6'
	0b00111000, //index 7, char '7'
	0b01111111, //index 8, char '8'
	0b01111101, //index 9, char '9'
};


void SevenSegment_Struct_Init(void){
	
	
	
}

void SevenSegment_GPIO_Init(void){
	RCC->IOPENR  |= RCC_IOPENR_GPIOAEN;
	RCC->IOPENR  |= RCC_IOPENR_GPIOBEN;
	RCC->IOPENR  |= RCC_IOPENR_GPIOCEN;
	
	//Set GP Output ->Segment Pins
	GPIOA->MODER &=~GPIO_MODER_MODE0_Msk;
	GPIOA->MODER &=~GPIO_MODER_MODE1_Msk;
	GPIOA->MODER &=~GPIO_MODER_MODE2_Msk;
	GPIOA->MODER &=~GPIO_MODER_MODE3_Msk;
	GPIOA->MODER &=~GPIO_MODER_MODE4_Msk;
	GPIOA->MODER &=~GPIO_MODER_MODE5_Msk;
	GPIOA->MODER &=~GPIO_MODER_MODE6_Msk;
	GPIOA->MODER &=~GPIO_MODER_MODE7_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE0_0;
	GPIOA->MODER |= GPIO_MODER_MODE1_0;
	GPIOA->MODER |= GPIO_MODER_MODE2_0;
	GPIOA->MODER |= GPIO_MODER_MODE3_0;
	GPIOA->MODER |= GPIO_MODER_MODE4_0;
	GPIOA->MODER |= GPIO_MODER_MODE5_0;
	GPIOA->MODER |= GPIO_MODER_MODE6_0;
	GPIOA->MODER |= GPIO_MODER_MODE7_0;
	
	//Set GP Output ->Digit1 & 2 Pins
	GPIOB->MODER &=~GPIO_MODER_MODE9_Msk;
	GPIOB->MODER |= GPIO_MODER_MODE9_0;
	GPIOC->MODER &=~GPIO_MODER_MODE15_Msk;
	GPIOC->MODER |= GPIO_MODER_MODE15_0;
  
	//Set GP Output ->Digit3 & 4 Pins
	GPIOA->MODER &=~GPIO_MODER_MODE11_Msk;
	GPIOA->MODER &=~GPIO_MODER_MODE12_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE11_0;
	GPIOA->MODER |= GPIO_MODER_MODE12_0;
	
	//Turn off all segment transistors (NPN)
	GPIOA->ODR   &=~GPIO_ODR_OD0;
	GPIOA->ODR   &=~GPIO_ODR_OD1;
	GPIOA->ODR   &=~GPIO_ODR_OD2;
	GPIOA->ODR   &=~GPIO_ODR_OD3;
	GPIOA->ODR   &=~GPIO_ODR_OD4;
	GPIOA->ODR   &=~GPIO_ODR_OD5;
	GPIOA->ODR   &=~GPIO_ODR_OD6;
	GPIOA->ODR   &=~GPIO_ODR_OD7;
	
	//Turn off all digit transistors (PNP)
	GPIOB->ODR   |= GPIO_ODR_OD9;
	GPIOC->ODR   |= GPIO_ODR_OD15;
	GPIOA->ODR   |= GPIO_ODR_OD11;
	GPIOA->ODR   |= GPIO_ODR_OD12;
	
	
	//Set Analog Input ->PB0, LDR as light sensor
	GPIOB->MODER |= GPIO_MODER_MODE0_Msk;
}

void SevenSegment_Set_Segment_Pins(uint8_t val){
	uint32_t temp = GPIOA->ODR;
	temp &= 0xFF00;
	temp |= val;
	GPIOA->ODR = temp;
}

void SevenSegment_Activate_Digit(uint8_t index){
  if(index == 1){
		GPIOC->ODR   |= GPIO_ODR_OD15;
	  GPIOA->ODR   |= GPIO_ODR_OD11;
	  GPIOA->ODR   |= GPIO_ODR_OD12;
		GPIOB->ODR   &=~GPIO_ODR_OD9;
	}
	else if(index == 2){
		GPIOB->ODR   |= GPIO_ODR_OD9;
	  GPIOA->ODR   |= GPIO_ODR_OD11;
	  GPIOA->ODR   |= GPIO_ODR_OD12;
		GPIOC->ODR   &=~GPIO_ODR_OD15;
	}
	else if(index == 3){
		GPIOB->ODR   |= GPIO_ODR_OD9;
	  GPIOC->ODR   |= GPIO_ODR_OD15;
	  GPIOA->ODR   |= GPIO_ODR_OD12;
		GPIOA->ODR   &=~GPIO_ODR_OD11;
	}
	else if(index == 4){
		GPIOB->ODR   |= GPIO_ODR_OD9;
	  GPIOC->ODR   |= GPIO_ODR_OD15;
	  GPIOA->ODR   |= GPIO_ODR_OD11;
	  GPIOA->ODR   &=~GPIO_ODR_OD12;
	}
	else{
		GPIOB->ODR   |= GPIO_ODR_OD9;
	  GPIOC->ODR   |= GPIO_ODR_OD15;
	  GPIOA->ODR   |= GPIO_ODR_OD11;
	  GPIOA->ODR   |= GPIO_ODR_OD12;
	}
}





