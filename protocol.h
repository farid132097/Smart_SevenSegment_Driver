

#ifndef  _PROTOCOL_H_
#define  _PROTOCOL_H_

#include "stm32g030xx.h"



enum{
	PROTOCOL_CMD_WRITE_REG                    = 0x00U,
	PROTOCOL_CMD_READ_REG                     = 0x01U,
	PROTOCOL_HEADER_MASTER                    = 0xA5U,
	PROTOCOL_HEADER_SLAVE                     = 0x5AU
};

enum{
	PROTOCOL_ERROR_NO_ERROR                   = 0x00,
	PROTOCOL_ERROR_CRC_MISMATCH               = 0x01,
	PROTOCOL_ERROR_INVALID_CMD                = 0x02,
	PROTOCOL_ERROR_LEN_MISMATCH               = 0x04,
	PROTOCOL_ERROR_HEADER_MISMATCH            = 0x08,
	PROTOCOL_ERROR_DATA_OUT_OF_RANGE          = 0x10,
	PROTOCOL_ERROR_INVALID_PARAMETER          = 0x20
};

enum{
	PROTOCOL_REG_DISPLAY_STATUS               = 0x00U,
	PROTOCOL_REG_FUNC_ENABLE                  = 0x01U,
	PROTOCOL_REG_DIGIT_SINGLE                 = 0x02U,
	PROTOCOL_REG_DIGIT_MULTIPLE               = 0x03U,
	PROTOCOL_REG_DECIMAL_POINTS_SINGLE        = 0x04U,
	PROTOCOL_REG_DECIMAL_POINTS_MULTIPLE      = 0x05U,
	PROTOCOL_REG_MANUAL_BRGHTNSS_VAL          = 0x06U,
	PROTOCOL_REG_AUTO_BRGHTNSS_ADC_VAL        = 0x07U,
	PROTOCOL_REG_AUTO_BRGHTNSS_SLOPE_ADCH     = 0x08U,
	PROTOCOL_REG_AUTO_BRGHTNSS_SLOPE_ADCL     = 0x09U,
	PROTOCOL_REG_AUTO_BRGHTNSS_SLOPE_VALH     = 0x0AU,
	PROTOCOL_REG_AUTO_BRGHTNSS_SLOPE_VALL     = 0x0BU,
};


void     Protocol_Struct_Init(void);

uint8_t  Protocol_Disp_Sts_Get(void);

void     Protocol_Build_Ack_Nack_Packet(void);
void     Protocol_Build_Status_Packet(void);
void     Protocol_Build_Func_En_Packet(void);
void     Protocol_Build_Digit_Single_Packet(void);
void     Protocol_Build_Digit_Multiple_Packet(void);
void     Protocol_Build_Dp_Single_Packet(void);
void     Protocol_Build_Dp_Multiple_Packet(void);
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

void     Protocol_Error_Check(void);
void     Protocol_Disassemble_Packet(void);
void     Protocol_Response_Mainloop(void);

void     Protocol_Init(void);









#endif





