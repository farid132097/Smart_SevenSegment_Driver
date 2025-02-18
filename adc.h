

#ifndef  _ADC_H_
#define  _ADC_H_

#include "stm32g030xx.h"



void     ADC_Struct_Init(void);
void     ADC_Reg_Init(void);
uint16_t ADC_Read_Channel(uint8_t channel);

void     ADC_Init(void);














#endif