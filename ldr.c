

#include "stm32g030xx.h"
#include "sevensegment.h"
#include "defs.h"
#include "ldr.h"
#include "adc.h"
#include "lpf.h"


typedef struct ldr_t{
	int16_t ADCVal;
	int16_t CalculatedBrightness;
	int16_t CurrentBrightness;
	int16_t ManualBrightness;
	int16_t DiffBrightness;
	int16_t HystBrightness;
	int16_t AutomaticBrightness;
	int16_t MaxBrightness;
}ldr_t;

static ldr_t LDR;

void LDR_Struct_Init(void){
	LDR.ADCVal = 0;
	LDR.CalculatedBrightness = 0;
	LDR.CurrentBrightness = 0;
	LDR.ManualBrightness = 0;
	LDR.DiffBrightness = 0;
	LDR.HystBrightness = 5;
	LDR.AutomaticBrightness = 1;
	LDR.MaxBrightness = 80;
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


void LDR_Control_Brightness(void){
	if(LDR.AutomaticBrightness == TRUE){
		LDR.ADCVal = (int16_t)LDR_Read_Brightness();
	  LDR.CalculatedBrightness  = LDR.ADCVal;
	  LDR.CalculatedBrightness /= 27;
	  LDR.CalculatedBrightness -= 11;
	  LDR.DiffBrightness = LDR.CurrentBrightness - LDR.CalculatedBrightness;
	  if( (LDR.DiffBrightness >= LDR.HystBrightness) || (LDR.DiffBrightness <= -LDR.HystBrightness)){	
	    if(LDR.CurrentBrightness < LDR.CalculatedBrightness){
				if(LDR.CurrentBrightness < LDR.MaxBrightness){
		      LDR.CurrentBrightness++;
				}
	    }
	    else if(LDR.CurrentBrightness > LDR.CalculatedBrightness){
		    LDR.CurrentBrightness--;
	    }
    }
	  
	  SevenSegment_Set_Brightness(0, LDR.CurrentBrightness);
	  SevenSegment_Set_Brightness(1, LDR.CurrentBrightness);
	  SevenSegment_Set_Brightness(2, LDR.CurrentBrightness);
	  SevenSegment_Set_Brightness(3, LDR.CurrentBrightness);
  }
	else{
		if(LDR.CurrentBrightness < LDR.ManualBrightness){
		    if(LDR.CurrentBrightness < LDR.MaxBrightness){
		      LDR.CurrentBrightness++;
				}
	    }
	    else if(LDR.CurrentBrightness > LDR.ManualBrightness){
		    LDR.CurrentBrightness--;
	  }
		SevenSegment_Set_Brightness(0, LDR.CurrentBrightness);
	  SevenSegment_Set_Brightness(1, LDR.CurrentBrightness);
	  SevenSegment_Set_Brightness(2, LDR.CurrentBrightness);
	  SevenSegment_Set_Brightness(3, LDR.CurrentBrightness);
	}
}


int16_t LDR_Get_ADC_Val(void){
	return LDR.ADCVal;
}

int16_t LDR_Get_Current_Brightness(void){
	return LDR.CurrentBrightness;
}

int16_t LDR_Get_Calculated_Brightness(void){
	return LDR.CalculatedBrightness;
}

uint8_t LDR_Automic_Brightness_Sts_Get(void){
	return LDR.AutomaticBrightness;
}

void LDR_Automic_Brightness_On(void){
	LDR.AutomaticBrightness = TRUE;
}

void LDR_Automic_Brightness_Off(void){
	LDR.AutomaticBrightness = FALSE;
}

void LDR_Manual_Brightness_Set(uint16_t val){
	LDR.ManualBrightness = val;
}

uint16_t LDR_Manual_Brightness_Get(void){
	return LDR.ManualBrightness;
}

void LDR_Max_Brightness_Set(int16_t val){
	if(val > 99){
		val = 99;
	}
	else if(val < 0){
		val = 0;
	}
	LDR.MaxBrightness = val;
}

void LDR_Init(void){
	LDR_Struct_Init();
	LDR_GPIO_Init();
	LDR_Reg_Init();
	
	#ifdef LDR_FILTER_EN
	LPF_Init();
	LPF_Set_Alpha(0, LDR_FILTER_EN);
	#endif
}



