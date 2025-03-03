

#include "stm32g030xx.h"
#include "app.h"
#include "defs.h"
#include "ldr.h"
#include "dispcom.h"
#include "sevensegment.h"
#include "protocol.h"


void App_Config(void){
	LDR_Init();
	SevenSegment_Init();
	DispCom_Init(38400);
	Protocol_Init();
	
	SevenSegment_Set_Value(0, 2);
	SevenSegment_Set_Value(1, 1);
	SevenSegment_Set_Value(2, 0);
	SevenSegment_Set_Value(3, 2);
	
	SevenSegment_Set_Dp(1, 1);
	SevenSegment_Set_Dp(2, 1);
	
	
	LDR_Max_Brightness_Set(90);
	LDR_Automic_Brightness_On();
	LDR_Manual_Brightness_Set(0);
}

void App_Mainloop(void){
  
	
	for(uint32_t i=0; i<40000; i++){
		__NOP();
	}
	
	LDR_Control_Brightness();
	
	Protocol_Build_Status_Packet();
	Protocol_Transmit_Packet();
	
	
	
}



