

#include "stm32g030xx.h"
#include "app.h"

int main(void){
	
	App_Config();
	//add WDT Config
	
	while(1){
		
		while(1){
			
			//reload WDT
			App_Mainloop();
			
		}
		
	}
	
}



