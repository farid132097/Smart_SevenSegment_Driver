

#ifndef  _SEVEN_SEGMENT_H_
#define  _SEVEN_SEGMENT_H_

#include "stm32g030xx.h"


void     SevenSegment_Struct_Init(void);
void     SevenSegment_GPIO_Init(void);
void     SevenSegment_Set_Segment_Pins(uint8_t val);
void     SevenSegment_Activate_Digit(uint8_t index);


#endif


