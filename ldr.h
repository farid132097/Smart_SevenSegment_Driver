

#ifndef  _LDR_H_
#define  _LDR_H_

#include "stm32g030xx.h"

#define  LDR_FILTER_EN


void     LDR_Struct_Init(void);
void     LDR_GPIO_Init(void);
void     LDR_Reg_Init(void);
uint16_t LDR_Read_Brightness(void);

void     LDR_Init(void);












#endif