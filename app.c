

#include "stm32g030xx.h"
#include "app.h"
#include "defs.h"
#include "ldr.h"
#include "dispcom.h"
#include "sevensegment.h"


uint16_t br = 0;

void App_Config(void){
	LDR_Init();
	SevenSegment_Init();
	DispCom_Init(38400);
	SevenSegment_Set_Value(0, 2);
	SevenSegment_Set_Value(1, 1);
	SevenSegment_Set_Value(2, 0);
	SevenSegment_Set_Value(3, 2);
	
	SevenSegment_Set_Dp(1, 1);
	SevenSegment_Set_Dp(2, 1);
	
	/*
	SevenSegment_Set_Brightness(0, 40);
	SevenSegment_Set_Brightness(1, 40);
	SevenSegment_Set_Brightness(2, 6);
	SevenSegment_Set_Brightness(3, 40);
	*/
}

void App_Mainloop(void){
  
	
	for(uint32_t i=0; i<160000; i++){
		__NOP();
	}
	SevenSegment_Set_Brightness(0, br);
	SevenSegment_Set_Brightness(1, br);
	SevenSegment_Set_Brightness(2, br);
	SevenSegment_Set_Brightness(3, br);
	
	br++;
	if(br >= 198){
		br=0;
	}
	
}



