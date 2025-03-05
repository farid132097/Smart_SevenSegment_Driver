

#ifndef  _SEVEN_SEGMENT_H_
#define  _SEVEN_SEGMENT_H_

#include "stm32g030xx.h"


void     SevenSegment_Struct_Init(void);
void     SevenSegment_GPIO_Init(void);
void     SevenSegment_Timer_Init(uint32_t update_rate);
void     TIM14_IRQHandler(void);
void     SevenSegment_Auto_Brightness_Timer_Init(uint32_t update_rate);
void     TIM17_IRQHandler(void);

void     SevenSegment_Set_Segment_Pins(uint8_t val);
void     SevenSegment_Assign_Segment_Value(uint8_t index);
void     SevenSegment_Activate_Digit(uint8_t index);
void     SevenSegment_Assign_Digit_Value(uint8_t index);
void     SevenSegment_Brightness_Handler(void);
void     SevenSegment_ISR_Executables(void);

void     SevenSegment_Set_Dp(uint8_t digit, uint8_t val);
void     SevenSegment_Set_Value(uint8_t digit, uint8_t val);
void     SevenSegment_Set_Brightness(uint8_t digit, int16_t val);

uint8_t  SevenSegment_Segment_Values_Get(uint8_t digit);
uint8_t  SevenSegment_Segment_Char_Values_Get(uint8_t digit);
uint8_t  SevenSegment_Dp_Values_Get(uint8_t digit);
uint8_t  SevenSegment_Dp_Byte_Get(void);

uint8_t  SevenSegment_Display_Enable_Sts(void);
void     SevenSegment_Display_Enable(void);
void     SevenSegment_Display_Disable(void);


void     SevenSegment_Init(void);

#endif


