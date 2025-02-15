
#include "stm32g030xx.h"
#include "wdt.h"


void WDT_Init(uint32_t timeout_ms){
	if((RCC->CSR & RCC_CSR_LSION) != RCC_CSR_LSION){
		RCC->CSR |= RCC_CSR_LSION;
		while((RCC->CSR & RCC_CSR_LSIRDY) != RCC_CSR_LSIRDY);
	}
	IWDG->KR = 0x0000CCCC;
	IWDG->KR = 0x00005555;
	while(IWDG->SR & IWDG_SR_PVU);
	IWDG->PR = 0x00000003;
	while(IWDG->SR & IWDG_SR_PVU);
	IWDG->RLR = timeout_ms;
	while(IWDG->SR != 0);
	IWDG->KR = 0x0000AAAA;
}

void WDT_Reload(void){
	while(IWDG->SR != 0);
	IWDG->KR = 0x0000AAAA;
}


