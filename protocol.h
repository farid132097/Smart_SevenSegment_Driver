

#ifndef  _PROTOCOL_H_
#define  _PROTOCOL_H_

#include "stm32g030xx.h"


enum{
	CMD_WRITE_REG                = 0x00U,
	CMD_READ_REG                 = 0x01U
};

enum{
	REG_DISPLAY_STATUS           = 0x00U,
	REG_FUNC_ENABLE              = 0x01U,
	REG_DIGIT_SINGLE             = 0x02U,
	REG_DIGIT_MULTIPLE           = 0x03U,
	REG_DECIMAL_POINTS           = 0x04U,
	REG_MANUAL_BRGHTNSS_VAL      = 0x05U,
	REG_AUTO_BRGHTNSS_ADC_VAL    = 0x06U,
	REG_AUTO_BRGHTNSS_SLOPE_ADCH = 0x07U,
	REG_AUTO_BRGHTNSS_SLOPE_ADCL = 0x08U,
	REG_AUTO_BRGHTNSS_SLOPE_VALH = 0x09U,
	REG_AUTO_BRGHTNSS_SLOPE_VALL = 0x0AU,
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


void     Protocol_Init(void);









#endif





