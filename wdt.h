


#ifndef  _WDT_H_
#define  _WDT_H_

#include "stm32g030xx.h"


void WDT_Init(uint32_t timeout_ms);
void WDT_Reload(void);

#endif


