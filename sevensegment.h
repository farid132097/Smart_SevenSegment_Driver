

#ifndef  _SEVEN_SEGMENT_H_
#define  _SEVEN_SEGMENT_H_

#include "stm32g030xx.h"


void     SevenSegment_Struct_Init(void);
void     SevenSegment_GPIO_Init(void);
void     SevenSegment_Timer_Init(uint32_t update_rate);
void     SevenSegment_Set_Segment_Pins(uint8_t val);
void     SevenSegment_Assign_Segment_Value(uint8_t index);
void     SevenSegment_Activate_Digit(uint8_t index);
void     SevenSegment_Assign_Digit_Value(uint8_t index);
void     SevenSegment_Brightness_Handler(void);
void     SevenSegment_ISR_Executables(void);

void     SevenSegment_Set_Brightness(uint8_t digit, uint16_t val);


void     SevenSegment_Init(void);

#endif


