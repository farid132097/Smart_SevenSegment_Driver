

#ifndef  _LDR_H_
#define  _LDR_H_

#include "stm32g030xx.h"

#define  LDR_FILTER_EN 60


void     LDR_Struct_Init(void);
void     LDR_GPIO_Init(void);
void     LDR_Reg_Init(void);
uint16_t LDR_Read_Brightness(void);
void     LDR_Control_Brightness(void);


int16_t  LDR_Get_Current_Brightness(void);
int16_t  LDR_Get_Calculated_Brightness(void);

void     LDR_Automic_Brightness_On(void);
void     LDR_Automic_Brightness_Off(void);
void     LDR_Manual_Brightness_Set(uint16_t val);

void     LDR_Init(void);












#endif

