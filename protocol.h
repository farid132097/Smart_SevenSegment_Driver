

#ifndef  _PROTOCOL_H_
#define  _PROTOCOL_H_

#include "stm32g030xx.h"

/*
Frame Format : Header (1 byte), Len (1 byte), CMD (1 byte), Reg (1 Byte), Data, CRC16 (2 byte)
Header       : 1 Byte (Master 0xA5, Slave 0x5A)
Length       : N bytes + 3
CMD          : 1 Byte
Data         : Variable
CRC16        : 2 Byte
*/


enum{
	CMD_WRITE_REG                = 0x00U,
	CMD_READ_REG                 = 0x01U
};

enum{
	REG_DISPLAY_STATUS           = 0x00U,
	REG_FUNC_ENABLE              = 0x01U,
	REG_DIGIT_SINGLE             = 0x02U,
	REG_DIGIT_MULTIPLE           = 0x03U,
	REG_DECIMAL_POINTS_SINGLE    = 0x04U,
	REG_DECIMAL_POINTS_MULTIPLE  = 0x05U,
	REG_MANUAL_BRGHTNSS_VAL      = 0x06U,
	REG_AUTO_BRGHTNSS_ADC_VAL    = 0x07U,
	REG_AUTO_BRGHTNSS_SLOPE_ADCH = 0x08U,
	REG_AUTO_BRGHTNSS_SLOPE_ADCL = 0x09U,
	REG_AUTO_BRGHTNSS_SLOPE_VALH = 0x0AU,
	REG_AUTO_BRGHTNSS_SLOPE_VALL = 0x0BU,
};


void     Protocol_Struct_Init(void);

uint8_t  Protocol_Disp_Sts_Get(void);

void     Protocol_Build_Status_Packet(void);
void     Protocol_Build_Func_En_Packet(void);
void     Protocol_Build_Manual_Brightness_Val_Packet(void);
void     Protocol_Build_Auto_Brightness_ADC_Val_Packet(void);
void     Protocol_Build_Auto_Brightness_Slope_ADCH_Packet(void);
void     Protocol_Build_Auto_Brightness_Slope_ADCL_Packet(void);
void     Protocol_Build_Auto_Brightness_Slope_ValH_Packet(void);
void     Protocol_Build_Auto_Brightness_Slope_ValL_Packet(void);


void     Protocol_Transmit_Packet(void);



void     Protocol_Response_Display_Status(uint8_t cmd);
void     Protocol_Response_Function_Enable(uint8_t cmd, uint8_t data);
void     Protocol_Response_Digit_Single(uint8_t cmd, uint8_t data1, uint8_t data2);
void     Protocol_Response_Digit_Multiple(uint8_t cmd, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4);
void     Protocol_Response_Decimal_Point_Single(uint8_t cmd, uint8_t data1, uint8_t data2);
void     Protocol_Response_Decimal_Point_Multiple(uint8_t cmd, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4);
void     Protocol_Response_Manual_Brightness(uint8_t cmd, uint8_t data);
void     Protocol_Response_Auto_Brightness_ADC(uint8_t cmd);


void     Protocol_Init(void);









#endif





