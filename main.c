

#include "stm32g030xx.h"
#include "app.h"
#include "wdt.h"

int main(void){
	
	App_Config();
	WDT_Init(3000);
	
	while(1){
		
		while(1){
			
			WDT_Reload();
			App_Mainloop();
			
		}
		
	}
	
}



