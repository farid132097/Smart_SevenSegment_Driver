

#include "stm32g030xx.h"
#include "app.h"
#include "defs.h"
#include "ldr.h"
#include "dispcom.h"
#include "sevensegment.h"




void App_Config(void){
	LDR_Init();
	SevenSegment_Init();
	DispCom_Init(38400);
}

void App_Mainloop(void){
		
	
}



