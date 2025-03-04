

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
	DispCom_Init(9600);
	Protocol_Init();
	
	SevenSegment_Set_Value(0, 2);
	SevenSegment_Set_Value(1, 1);
	SevenSegment_Set_Value(2, 0);
	SevenSegment_Set_Value(3, 2);
	
	SevenSegment_Set_Dp(1, 1);
	SevenSegment_Set_Dp(2, 1);
	
	
	LDR_Max_Brightness_Set(100);
	LDR_Automic_Brightness_On();
	LDR_Manual_Brightness_Set(0);
}

void App_Mainloop(void){
  
	
	for(uint32_t i=0; i<80000; i++){
		__NOP();
	}
	if(DispCom_Data_Available()){
		Protocol_Response_Mainloop();
	  DispCom_RX_Packet_Read_Complete();
	}
	LDR_Control_Brightness();
	
}



