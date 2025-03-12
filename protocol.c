

#include "stm32g030xx.h"
#include "protocol.h"
#include "sevensegment.h"
#include "comm.h"
#include "ldr.h"
#include "defs.h"



/*
Frame Format : Header (1 byte), Len (1 byte), CMD (1 byte), Reg (1 Byte), Data, CRC16 (2 byte)
Header       : 1 Byte (Master 0xA5, Slave 0x5A)
Length       : N bytes + 3
CMD          : 1 Byte
Data         : Variable
CRC16        : 2 Byte
*/

enum{
	PROTOCOL_ERROR_NO_ERROR          = 0x00,
	PROTOCOL_ERROR_CRC_MISMATCH      = 0x01,
	PROTOCOL_ERROR_INVALID_CMD       = 0x02,
	PROTOCOL_ERROR_LEN_MISMATCH      = 0x04,
	PROTOCOL_ERROR_HEADER_MISMATCH   = 0x08,
	PROTOCOL_ERROR_DATA_OUT_OF_RANGE = 0x10,
	PROTOCOL_ERROR_INVALID_PARAMETER = 0x20
};




typedef struct packet_t{
	volatile uint8_t  Header;
	volatile uint8_t  Len;
	volatile uint8_t  CMD;
	volatile uint8_t  Reg;
	volatile uint8_t  AckReturn;
	volatile uint8_t  NackReturn;
	volatile uint8_t  ReadDisplaySts;
	volatile uint8_t  ReadFuncEnable;
	volatile uint8_t  ReadDigitSingle;
	volatile uint8_t  ReadDigitMultiple;
	volatile uint8_t  ReadDpSingle;
	volatile uint8_t  ReadDpMultiple;
	volatile uint8_t  ReadManualBrightness;
	volatile uint8_t  ReadBrightnessADC;
	volatile uint8_t  ReadBrightnessSlopeADCH;
	volatile uint8_t  ReadBrightnessSlopeADCL;
	volatile uint8_t  ReadBrightnessValADCH;
	volatile uint8_t  ReadBrightnessValADCL;
	volatile uint16_t CRC16;
}packet_t;

typedef struct protocol_t{
	volatile uint8_t RxBuf[20];
	packet_t         RxPacket;
	volatile uint8_t TxBuf[20];
	packet_t         TxPacket;
}protocol_t;

static protocol_t Protocol;

void Protocol_Struct_Init(void){
	for(uint8_t i=0; i<10; i++){
	  Protocol.RxBuf[i] = 0;
		Protocol.TxBuf[i] = 0;
	}
	Protocol.TxPacket.Header  = 0x5A;
	Protocol.TxPacket.Len     = 0x00;
	Protocol.TxPacket.CMD     = 0x00;
	Protocol.TxPacket.CRC16   = 0x00;
	
	Protocol.RxPacket.AckReturn = FALSE;
	Protocol.RxPacket.NackReturn = FALSE;
	Protocol.RxPacket.ReadDisplaySts = FALSE;
	Protocol.RxPacket.ReadFuncEnable = FALSE;
	Protocol.RxPacket.ReadDigitSingle = FALSE;
	Protocol.RxPacket.ReadDigitMultiple = FALSE;
	Protocol.RxPacket.ReadDpSingle = FALSE;
	Protocol.RxPacket.ReadDpMultiple = FALSE;
	Protocol.RxPacket.ReadManualBrightness = FALSE;
	Protocol.RxPacket.ReadBrightnessADC = FALSE;
	Protocol.RxPacket.ReadBrightnessSlopeADCH = FALSE;
	Protocol.RxPacket.ReadBrightnessSlopeADCL = FALSE;
	Protocol.RxPacket.ReadBrightnessValADCH = FALSE;
	Protocol.RxPacket.ReadBrightnessValADCL = FALSE;
}

uint8_t Protocol_Disp_Sts_Get(void){
	return (uint8_t)((LDR_Automic_Brightness_Sts_Get()<<1) | SevenSegment_Display_Enable_Sts());
}

void Protocol_Build_Ack_Packet(void){
	Protocol.TxBuf[0]  = Protocol.TxPacket.Header;
	Protocol.TxBuf[1]  = 5;
	Protocol.TxBuf[2]  = 0x01;
	
  Protocol.TxPacket.CRC16 = COMM_CRC_Calculate_Block((uint8_t*)Protocol.TxBuf, 3);
	Protocol.TxBuf[3]  = (Protocol.TxPacket.CRC16 >> 8);
	Protocol.TxBuf[4]  = (Protocol.TxPacket.CRC16 & 0xFF);
}

void Protocol_Build_Nack_Packet(void){
	Protocol.TxBuf[0]  = Protocol.TxPacket.Header;
	Protocol.TxBuf[1]  = 5;
	Protocol.TxBuf[2]  = 0x00;
	
	Protocol.TxPacket.CRC16 = COMM_CRC_Calculate_Block((uint8_t*)Protocol.TxBuf, 3);
	Protocol.TxBuf[3]  = (Protocol.TxPacket.CRC16 >> 8);
	Protocol.TxBuf[4]  = (Protocol.TxPacket.CRC16 & 0xFF);
}

void Protocol_Build_Status_Packet(void){
	Protocol.TxBuf[0]  = Protocol.TxPacket.Header;
	Protocol.TxBuf[1]  = 13;
	Protocol.TxBuf[2]  = SevenSegment_Segment_Char_Values_Get(0);
	Protocol.TxBuf[3]  = SevenSegment_Segment_Char_Values_Get(1);
	Protocol.TxBuf[4]  = SevenSegment_Segment_Char_Values_Get(2);
	Protocol.TxBuf[5]  = SevenSegment_Segment_Char_Values_Get(3);
	Protocol.TxBuf[6]  = SevenSegment_Dp_Byte_Get();
	Protocol.TxBuf[7]  = (uint8_t)(LDR_Get_ADC_Val() >> 8);
	Protocol.TxBuf[8]  = (LDR_Get_ADC_Val() & 0xFF);
	Protocol.TxBuf[9]  = (LDR_Get_Current_Brightness() & 0xFF);
	Protocol.TxBuf[10] = Protocol_Disp_Sts_Get();
	
	Protocol.TxPacket.CRC16 = COMM_CRC_Calculate_Block((uint8_t*)Protocol.TxBuf, 10);
	Protocol.TxBuf[11] = (Protocol.TxPacket.CRC16 >> 8);
	Protocol.TxBuf[12] = (Protocol.TxPacket.CRC16 & 0xFF);
}

void Protocol_Build_Func_En_Packet(void){
	Protocol.TxBuf[0] = Protocol.TxPacket.Header;
	Protocol.TxBuf[1] = 5;
	Protocol.TxBuf[2] = Protocol_Disp_Sts_Get();

	Protocol.TxPacket.CRC16 = COMM_CRC_Calculate_Block((uint8_t*)Protocol.TxBuf, 3);
	Protocol.TxBuf[3] = (Protocol.TxPacket.CRC16 >> 8);
	Protocol.TxBuf[4] = (Protocol.TxPacket.CRC16 & 0xFF);
}

void Protocol_Build_Manual_Brightness_Val_Packet(void){
	Protocol.TxBuf[0] = Protocol.TxPacket.Header;
	Protocol.TxBuf[1] = 5;
	Protocol.TxBuf[2] = (uint8_t)LDR_Manual_Brightness_Get();

	Protocol.TxPacket.CRC16 = COMM_CRC_Calculate_Block((uint8_t*)Protocol.TxBuf, 3);
	Protocol.TxBuf[3] = (Protocol.TxPacket.CRC16 >> 8);
	Protocol.TxBuf[4] = (Protocol.TxPacket.CRC16 & 0xFF);
}

void Protocol_Build_Auto_Brightness_ADC_Val_Packet(void){
	Protocol.TxBuf[0] = Protocol.TxPacket.Header;
	Protocol.TxBuf[1] = 6;
	Protocol.TxBuf[2] = (uint8_t)(LDR_Get_ADC_Val() >> 8);
	Protocol.TxBuf[3] = (LDR_Get_ADC_Val() & 0xFF);

	Protocol.TxPacket.CRC16 = COMM_CRC_Calculate_Block((uint8_t*)Protocol.TxBuf, 3);
	Protocol.TxBuf[4] = (Protocol.TxPacket.CRC16 >> 8);
	Protocol.TxBuf[5] = (Protocol.TxPacket.CRC16 & 0xFF);
}

void Protocol_Build_Auto_Brightness_Slope_ADCH_Packet(void){
	
}

void Protocol_Build_Auto_Brightness_Slope_ADCL_Packet(void){
	
}

void Protocol_Build_Auto_Brightness_Slope_ValH_Packet(void){
	
}

void Protocol_Build_Auto_Brightness_Slope_ValL_Packet(void){
	
}

void Protocol_Transmit_Packet(void){
	COMM_Tx_Buf((uint8_t*)Protocol.TxBuf, Protocol.TxBuf[1]);
}






void Protocol_Response_Display_Status(uint8_t cmd){
	if(cmd == CMD_WRITE_REG){
		//No Write Access
		Protocol.RxPacket.NackReturn = TRUE;
	}
	else if(cmd == CMD_READ_REG){
		Protocol.RxPacket.ReadDisplaySts = TRUE;
	}
}

void Protocol_Response_Function_Enable(uint8_t cmd, uint8_t data){
	if(cmd == CMD_WRITE_REG){
		if(data & (1<<0)){
			SevenSegment_Display_Enable();
		}
		else{
			SevenSegment_Display_Disable();
		}
		if(data & (1<<1)){
			LDR_Automic_Brightness_On();
		}
		else{
			LDR_Automic_Brightness_Off();
		}
		Protocol.RxPacket.AckReturn = TRUE;
	}
	else if(cmd == CMD_READ_REG){
		Protocol.RxPacket.ReadFuncEnable = TRUE;
	}
}

void Protocol_Response_Digit_Single(uint8_t cmd, uint8_t data1, uint8_t data2){
	if(cmd == CMD_WRITE_REG){
		SevenSegment_Set_Value(data1, data2);
		Protocol.RxPacket.AckReturn = TRUE;
	}
	else if(cmd == CMD_READ_REG){
		Protocol.RxPacket.ReadDigitSingle = TRUE;
	}
}

void Protocol_Response_Digit_Multiple(uint8_t cmd, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4){
	if(cmd == CMD_WRITE_REG){
		SevenSegment_Set_Value(0, data1);
		SevenSegment_Set_Value(1, data2);
		SevenSegment_Set_Value(2, data3);
		SevenSegment_Set_Value(3, data4);
		Protocol.RxPacket.AckReturn = TRUE;
	}
	else if(cmd == CMD_READ_REG){
		Protocol.RxPacket.ReadDigitMultiple = TRUE;
	}
}

void Protocol_Response_Decimal_Point_Single(uint8_t cmd, uint8_t data1, uint8_t data2){
	if(cmd == CMD_WRITE_REG){
		SevenSegment_Set_Dp(data1, data2);
		Protocol.RxPacket.AckReturn = TRUE;
	}
	else if(cmd == CMD_READ_REG){
		Protocol.RxPacket.ReadDpSingle = TRUE;
	}
}

void Protocol_Response_Decimal_Point_Multiple(uint8_t cmd, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4){
	if(cmd == CMD_WRITE_REG){
		SevenSegment_Set_Dp(0, data1);
		SevenSegment_Set_Dp(1, data2);
		SevenSegment_Set_Dp(2, data3);
		SevenSegment_Set_Dp(3, data4);
		Protocol.RxPacket.AckReturn = TRUE;
	}
	else if(cmd == CMD_READ_REG){
		Protocol.RxPacket.ReadDpMultiple = TRUE;
	}
}

void Protocol_Response_Manual_Brightness(uint8_t cmd, uint8_t data){
	if(cmd == CMD_WRITE_REG){
		LDR_Manual_Brightness_Set(data);
		Protocol.RxPacket.AckReturn = TRUE;
	}
	else if(cmd == CMD_READ_REG){
		Protocol.RxPacket.ReadManualBrightness = TRUE;
	}
}

void Protocol_Response_Auto_Brightness_ADC(uint8_t cmd){
	if(cmd == CMD_WRITE_REG){
		//No Write Access
		Protocol.RxPacket.NackReturn = TRUE;
	}
	else if(cmd == CMD_READ_REG){
		Protocol.RxPacket.ReadBrightnessADC = TRUE;
	}
}



void Protocol_Response_Mainloop(void){
	if(Protocol.RxPacket.AckReturn == TRUE){
		Protocol_Build_Ack_Packet();
		Protocol_Transmit_Packet();
		Protocol.RxPacket.AckReturn = FALSE;
	}
	else if(Protocol.RxPacket.NackReturn == TRUE){
		Protocol_Build_Nack_Packet();
		Protocol_Transmit_Packet();
		Protocol.RxPacket.NackReturn = FALSE;
	}
	else if(Protocol.RxPacket.ReadDisplaySts == TRUE){
		Protocol_Build_Status_Packet();
		Protocol_Transmit_Packet();
		Protocol.RxPacket.ReadDisplaySts = FALSE;
	}
	else if(Protocol.RxPacket.ReadFuncEnable == TRUE){
		Protocol_Build_Func_En_Packet();
		Protocol_Transmit_Packet();
		Protocol.RxPacket.ReadFuncEnable = FALSE;
	}
	else if(Protocol.RxPacket.ReadDigitSingle == TRUE){
		//add packet handler
		Protocol.RxPacket.ReadDigitSingle = FALSE;
	}
	else if(Protocol.RxPacket.ReadDigitMultiple == TRUE){
		//add packet handler
		Protocol.RxPacket.ReadDigitMultiple = FALSE;
	}
	else if(Protocol.RxPacket.ReadDpSingle == TRUE){
		//add packet handler
		Protocol.RxPacket.ReadDpSingle = FALSE;
	}
	else if(Protocol.RxPacket.ReadDpMultiple == TRUE){
		//add packet handler
		Protocol.RxPacket.ReadDpMultiple = FALSE;
	}
	else if(Protocol.RxPacket.ReadManualBrightness == TRUE){
		Protocol_Build_Manual_Brightness_Val_Packet();
		Protocol_Transmit_Packet();
		Protocol.RxPacket.ReadManualBrightness = FALSE;
	}
	else if(Protocol.RxPacket.ReadBrightnessADC == TRUE){
		Protocol_Build_Auto_Brightness_ADC_Val_Packet();
		Protocol_Transmit_Packet();
		Protocol.RxPacket.ReadBrightnessADC = FALSE;
	}
	else if(Protocol.RxPacket.ReadBrightnessSlopeADCH == TRUE){
		//add packet handler
		Protocol.RxPacket.ReadBrightnessSlopeADCH = FALSE;
	}
	else if(Protocol.RxPacket.ReadBrightnessSlopeADCL == TRUE){
		//add packet handler
		Protocol.RxPacket.ReadBrightnessSlopeADCL = FALSE;
	}
	else if(Protocol.RxPacket.ReadBrightnessValADCH == TRUE){
		//add packet handler
		Protocol.RxPacket.ReadBrightnessValADCH = FALSE;
	}
	else if(Protocol.RxPacket.ReadBrightnessValADCL == TRUE){
		//add packet handler
		Protocol.RxPacket.ReadBrightnessValADCL = FALSE;
	}
}





void Protocol_Init(void){
	Protocol_Struct_Init();
}
	
