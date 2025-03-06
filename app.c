

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
  
	LDR_Max_Brightness_Set(100);
	LDR_Automic_Brightness_On();
	LDR_Manual_Brightness_Set(0);
}

void App_Mainloop(void){
  
	if(DispCom_Data_Available()){
		Protocol_Response_Mainloop();
	  DispCom_RX_Packet_Read_Complete();
	}
	
}



