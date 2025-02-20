

#include "stm32g030xx.h"
#include "sevensegment.h"

//Not implemented yet
#define  SEVENSEGMENT_DISP_TYPE_CA
//#define  SEVENSEGMENT_DISP_TYPE_CC

//Uncomment if Segments are driven by NPN BJTs
#define  SEVENSEGMENT_SEG_DRV_INV
//Uncomment if Common Pins are driven by PNP BJTs
#define  SEVENSEGMENT_DIG_DRV_INV

#define  SEVENSEGMENT_TOTAL_DIGITS      (   4U)
#define  SEVENSEGMENT_INV_DIGITS_STRT   (   2U)
#define  SEVENSEGMENT_INT_PRIORITY      (   0U)
#define  SEVENSEGMENT_VAL_OFFSET        (  12U)



static uint8_t segment_val[24] = {
	//For Non-Inverted Digits
	0x3F, //0b00111111, //index 0, char '0'
	0x06, //0b00000110, //index 1, char '1'
	0x5B, //0b01011011, //index 2, char '2'
	0x4F, //0b01001111, //index 3, char '3'
	0x66, //0b01100110, //index 4, char '4'
	0x6D, //0b01101101, //index 5, char '5'
	0x7D, //0b01111101, //index 6, char '6'
	0x07, //0b00000111, //index 7, char '7'
	0x7F, //0b01111111, //index 8, char '8'
	0x6F, //0b01101111, //index 9, char '9'
	0x00, //0b00000000, //index 10, blank
	0x00, //0b00000000, //index 11, custom
	
	//For Inverted Digits
	0x3F, //0b00111111, //index 0, char '0'
	0x30, //0b00110000, //index 1, char '1'
	0x5B, //0b01011011, //index 2, char '2'
	0x79, //0b01111001, //index 3, char '3'
	0x74, //0b01110100, //index 4, char '4'
	0x6D, //0b01101101, //index 5, char '5'
	0x6F, //0b01101111, //index 6, char '6'
	0x38, //0b00111000, //index 7, char '7'
	0x7F, //0b01111111, //index 8, char '8'
	0x7D, //0b01111101, //index 9, char '9'
	0x00, //0b00000000, //index 10, blank
	0x00, //0b00000000, //index 11, custom
};

typedef struct seven_segment_t{
	volatile uint8_t  CurrentDigitIndex;
	volatile uint8_t  Reserved0;
	volatile uint16_t InterruptTickCount;
	volatile uint16_t AntiGhostingCycle;
	volatile uint16_t DigitBrightnessTopVal;
	volatile uint16_t DigitBrightness[4];
	volatile uint8_t  SegmentValues[4];
	volatile uint8_t  DpValues[4];
}seven_segment_t;

static seven_segment_t SevenSegment;


void SevenSegment_Struct_Init(void){
	SevenSegment.CurrentDigitIndex = 0;
	SevenSegment.InterruptTickCount = 0;
	SevenSegment.AntiGhostingCycle = 2;
	SevenSegment.DigitBrightnessTopVal = 100;
	SevenSegment.DigitBrightness[0] = 0;
	SevenSegment.DigitBrightness[1] = 0;
	SevenSegment.DigitBrightness[2] = 0;
	SevenSegment.DigitBrightness[3] = 0;
	SevenSegment.SegmentValues[0] = 0;
	SevenSegment.SegmentValues[1] = 0;
	SevenSegment.SegmentValues[2] = 0;
	SevenSegment.SegmentValues[3] = 0;
	SevenSegment.DpValues[0] = 0;
	SevenSegment.DpValues[1] = 0;
	SevenSegment.DpValues[2] = 0;
	SevenSegment.DpValues[3] = 0;
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
	#ifdef SEVENSEGMENT_SEG_DRV_INV
	GPIOA->ODR   &=~GPIO_ODR_OD0;
	GPIOA->ODR   &=~GPIO_ODR_OD1;
	GPIOA->ODR   &=~GPIO_ODR_OD2;
	GPIOA->ODR   &=~GPIO_ODR_OD3;
	GPIOA->ODR   &=~GPIO_ODR_OD4;
	GPIOA->ODR   &=~GPIO_ODR_OD5;
	GPIOA->ODR   &=~GPIO_ODR_OD6;
	GPIOA->ODR   &=~GPIO_ODR_OD7;
	#else
	GPIOA->ODR   |= GPIO_ODR_OD0;
	GPIOA->ODR   |= GPIO_ODR_OD1;
	GPIOA->ODR   |= GPIO_ODR_OD2;
	GPIOA->ODR   |= GPIO_ODR_OD3;
	GPIOA->ODR   |= GPIO_ODR_OD4;
	GPIOA->ODR   |= GPIO_ODR_OD5;
	GPIOA->ODR   |= GPIO_ODR_OD6;
	GPIOA->ODR   |= GPIO_ODR_OD7;
	#endif
	
	//Turn off all digit transistors (PNP)
	#ifdef SEVENSEGMENT_DIG_DRV_INV
	GPIOB->ODR   |= GPIO_ODR_OD9;
	GPIOC->ODR   |= GPIO_ODR_OD15;
	GPIOA->ODR   |= GPIO_ODR_OD11;
	GPIOA->ODR   |= GPIO_ODR_OD12;
	#else
	GPIOB->ODR   &=~GPIO_ODR_OD9;
	GPIOC->ODR   &=~GPIO_ODR_OD15;
	GPIOA->ODR   &=~GPIO_ODR_OD11;
	GPIOA->ODR   &=~GPIO_ODR_OD12;
	#endif
	
	//Set Analog Input ->PB0, LDR as light sensor
	GPIOB->MODER |= GPIO_MODER_MODE0_Msk;
}


void SevenSegment_Timer_Init(uint32_t update_rate){
  RCC->APBENR2 |= RCC_APBENR2_TIM14EN;
	TIM14->PSC    = 0;
	//Need to update according to core clock
	TIM14->ARR    = (16000000/update_rate);
	TIM14->DIER  |= TIM_DIER_UIE;
	TIM14->CR1   |= TIM_CR1_CEN;
	NVIC_EnableIRQ(TIM14_IRQn);
	NVIC_SetPriority(TIM14_IRQn, SEVENSEGMENT_INT_PRIORITY);
}


void TIM14_IRQHandler(void){
	TIM14->SR &=~ TIM_SR_UIF;
  SevenSegment_ISR_Executables();
}









void SevenSegment_Set_Segment_Pins(uint8_t val){
	uint32_t temp = GPIOA->ODR;
	temp &= 0xFF00;
	#ifndef SEVENSEGMENT_SEG_DRV_INV
	val=~val;
  #endif
	temp |= val;
	GPIOA->ODR = temp;
}

void SevenSegment_Assign_Segment_Value(uint8_t index){
	if( index <= 3 ){
		//Update values from segment buffer
	  SevenSegment_Set_Segment_Pins(SevenSegment.SegmentValues[index]);
	}
	else{
		//Turn off all segments
		SevenSegment_Set_Segment_Pins(0);
	}
}

//Valid digit values are from 0 to 3, 4 to 255 for Anti-ghosting
void SevenSegment_Activate_Digit(uint8_t index){
  if(index == 0){
		#ifdef SEVENSEGMENT_DIG_DRV_INV
		GPIOC->ODR   |= GPIO_ODR_OD15;
	  GPIOA->ODR   |= GPIO_ODR_OD11;
	  GPIOA->ODR   |= GPIO_ODR_OD12;
		GPIOB->ODR   &=~GPIO_ODR_OD9;
		#else
		GPIOC->ODR   &=~GPIO_ODR_OD15;
	  GPIOA->ODR   &=~GPIO_ODR_OD11;
	  GPIOA->ODR   &=~GPIO_ODR_OD12;
		GPIOB->ODR   |= GPIO_ODR_OD9;
		#endif
	}
	else if(index == 1){
		#ifdef SEVENSEGMENT_DIG_DRV_INV
		GPIOB->ODR   |= GPIO_ODR_OD9;
	  GPIOA->ODR   |= GPIO_ODR_OD11;
	  GPIOA->ODR   |= GPIO_ODR_OD12;
		GPIOC->ODR   &=~GPIO_ODR_OD15;
		#else
		GPIOB->ODR   &=~GPIO_ODR_OD9;
	  GPIOA->ODR   &=~GPIO_ODR_OD11;
	  GPIOA->ODR   &=~GPIO_ODR_OD12;
		GPIOC->ODR   |= GPIO_ODR_OD15;
		#endif
	}
	else if(index == 2){
		#ifdef SEVENSEGMENT_DIG_DRV_INV
		GPIOB->ODR   |= GPIO_ODR_OD9;
	  GPIOC->ODR   |= GPIO_ODR_OD15;
	  GPIOA->ODR   |= GPIO_ODR_OD12;
		GPIOA->ODR   &=~GPIO_ODR_OD11;
		#else
		GPIOB->ODR   &=~GPIO_ODR_OD9;
	  GPIOC->ODR   &=~GPIO_ODR_OD15;
	  GPIOA->ODR   &=~GPIO_ODR_OD12;
		GPIOA->ODR   |= GPIO_ODR_OD11;
		#endif
	}
	else if(index == 3){
		#ifdef SEVENSEGMENT_DIG_DRV_INV
		GPIOB->ODR   |= GPIO_ODR_OD9;
	  GPIOC->ODR   |= GPIO_ODR_OD15;
	  GPIOA->ODR   |= GPIO_ODR_OD11;
	  GPIOA->ODR   &=~GPIO_ODR_OD12;
		#else
		GPIOB->ODR   &=~GPIO_ODR_OD9;
	  GPIOC->ODR   &=~GPIO_ODR_OD15;
	  GPIOA->ODR   &=~GPIO_ODR_OD11;
	  GPIOA->ODR   |= GPIO_ODR_OD12;
		#endif
	}
	else{
		#ifdef SEVENSEGMENT_DIG_DRV_INV
		GPIOB->ODR   |= GPIO_ODR_OD9;
	  GPIOC->ODR   |= GPIO_ODR_OD15;
	  GPIOA->ODR   |= GPIO_ODR_OD11;
	  GPIOA->ODR   |= GPIO_ODR_OD12;
		#else
		GPIOB->ODR   &=~GPIO_ODR_OD9;
	  GPIOC->ODR   &=~GPIO_ODR_OD15;
	  GPIOA->ODR   &=~GPIO_ODR_OD11;
	  GPIOA->ODR   &=~GPIO_ODR_OD12;
		#endif
	}
}

void SevenSegment_Assign_Digit_Value(uint8_t index){
	SevenSegment_Assign_Segment_Value( index );
	SevenSegment_Activate_Digit( index );
}

void SevenSegment_Brightness_Handler(void){
	
	if(SevenSegment.InterruptTickCount <= 1){
		//Anti-ghosting
		SevenSegment_Set_Segment_Pins(0);
		SevenSegment_Activate_Digit(4);
	}
	else if(SevenSegment.InterruptTickCount <= SevenSegment.AntiGhostingCycle){
		//Assign new value
		SevenSegment_Assign_Digit_Value(SevenSegment.CurrentDigitIndex);
	}
	else if(SevenSegment.InterruptTickCount >= SevenSegment.DigitBrightness[SevenSegment.CurrentDigitIndex]){
		//Turn off current digit
		SevenSegment_Set_Segment_Pins(0);
		SevenSegment_Activate_Digit(4);
	}
	
	
	SevenSegment.InterruptTickCount++;
	if(SevenSegment.InterruptTickCount >= SevenSegment.DigitBrightnessTopVal){
		//Switch to next digit
		SevenSegment.CurrentDigitIndex++;
		if(SevenSegment.CurrentDigitIndex >= SEVENSEGMENT_TOTAL_DIGITS){
			SevenSegment.CurrentDigitIndex = 0;
		}
		
		//Reset Tick Counter
		SevenSegment.InterruptTickCount = 0;
	}
	
}


void SevenSegment_ISR_Executables(void){
	SevenSegment_Brightness_Handler();
}


void SevenSegment_Set_Dp(uint8_t digit, uint8_t val){
	if(digit < SEVENSEGMENT_TOTAL_DIGITS){
		if(val == 0){
	    SevenSegment.SegmentValues[digit] &=~ (1<<7);
		}
		else if(val == 1){
			SevenSegment.SegmentValues[digit] |=  (1<<7);
		}
	}
}

void SevenSegment_Set_Value(uint8_t digit, uint8_t val){
	uint8_t temp;
	if(digit < SEVENSEGMENT_TOTAL_DIGITS){
	  if(digit >= SEVENSEGMENT_INV_DIGITS_STRT ){
		  //add offset for inverted digits
			val += SEVENSEGMENT_VAL_OFFSET;
	  }
		temp  = SevenSegment.SegmentValues[digit];
		temp &= 0x80;
	  SevenSegment.SegmentValues[digit] = segment_val[val] | temp;
	}
}

void SevenSegment_Set_Brightness(uint8_t digit, int16_t val){
	int16_t temp;
	if(val < 0){
		val = 0;
	}
	temp = SevenSegment.AntiGhostingCycle + 1 + val;
	if(temp >= (SevenSegment.DigitBrightnessTopVal - 2) ){
		temp = SevenSegment.DigitBrightnessTopVal - 2;
	}
	SevenSegment.DigitBrightness[digit] = temp;
}





void SevenSegment_Init(void){
	#ifdef SEVENSEGMENT_DISP_TYPE_CA
	  #ifdef SEVENSEGMENT_DISP_TYPE_CC
	  #error define macro SEVENSEGMENT_DISP_TYPE_CA & SEVENSEGMENT_DISP_TYPE_CC cannot be true at the same time
		#endif
	#endif
	SevenSegment_Struct_Init();
	SevenSegment_GPIO_Init();
	SevenSegment_Timer_Init(48000);
}


