

#include "stm32g030xx.h"
#include "protocol.h"
#include "sevensegment.h"
#include "comm.h"
#include "ldr.h"
#include "defs.h"



/*
 * Read/Write Transmit Frame Format (Data Sent from Master to Slave):
 * ============================================================================================================================================================
 * Header (Byte0), Len (Byte1), CMD (Byte2), Reg (Byte3), Data (Byte4~ByteN) , CRC16 (ByteN+1~ByteN+2)
 * 
 * DispStatusReg       : [Header: 0xA5] [Len: 0x06] [Read: 0x01]  [RegAddress: 0x00] [CRC16H] [CRC16L]
 * DispStatusReg       : No Write Operation is allowed
 *
 * FuncEnReg           : [Header: 0xA5] [Len: 0x06] [Read: 0x01]  [RegAddress: 0x01] [CRC16H] [CRC16L]
 * FuncEnReg           : [Header: 0xA5] [Len: 0x07] [Write:0x00]  [RegAddress: 0x01] [Data0: 0x00~0x03] [CRC16H] [CRC16L]
 * 
 * DigitSingleReg      : [Header: 0xA5] [Len: 0x07] [Read: 0x01]  [RegAddress: 0x02] [Digit: 0x00~0x03] [CRC16H] [CRC16L]
 * DigitSingleReg      : [Header: 0xA5] [Len: 0x08] [Write:0x00]  [RegAddress: 0x02] [Digit: 0x00~0x03] [Data0: 0x00~0x0A] [CRC16H] [CRC16L]
 *
 * DigitMultipleReg    : [Header: 0xA5] [Len: 0x06] [Read: 0x01]  [RegAddress: 0x03] [CRC16H] [CRC16L]
 * DigitMultipleReg    : [Header: 0xA5] [Len: 0x0A] [Write:0x00]  [RegAddress: 0x03] [Data0: 0x00~0x0A] [Data1: 0x00~0x0A] 
                         [Data2: 0x00~0x0A] [Data3: 0x00~0x0A] [CRC16H] [CRC16L]
 *
 * DpSingleReg         : [Header: 0xA5] [Len: 0x07] [Read: 0x01]  [RegAddress: 0x04] [Digit: 0x00~0x03] [CRC16H] [CRC16L]
 * DpSingleReg         : [Header: 0xA5] [Len: 0x08] [Write:0x00]  [RegAddress: 0x04] [Digit: 0x00~0x03] [Data0: 0x00~0x01] [CRC16H] [CRC16L]
 * 
 * DpMultipleReg       : [Header: 0xA5] [Len: 0x06] [Read: 0x01]  [RegAddress: 0x05] [CRC16H] [CRC16L]
 * DpMultipleReg       : [Header: 0xA5] [Len: 0x0A] [Write:0x00]  [RegAddress: 0x05] [Data0: 0x00~0x01] [Data1: 0x00~0x01] 
                         [Data2: 0x00~0x01] [Data3: 0x00~0x01] [CRC16H] [CRC16L]
 * 
 * ManualBrightnessReg : [Header: 0xA5] [Len: 0x06] [Read: 0x01]  [RegAddress: 0x06] [CRC16H] [CRC16L]
 * ManualBrightnessReg : [Header: 0xA5] [Len: 0x07] [Write:0x00]  [RegAddress: 0x06] [Data0: 0x00~0x64] [CRC16H] [CRC16L]
 * 
 * AutoBrightnessADCReg: [Header: 0xA5] [Len: 0x06] [Read: 0x01]  [RegAddress: 0x07] [CRC16H] [CRC16L]
 * AutoBrightnessADCReg: No Write Operation is allowed
 *
 *
 * Read Response Packet Foramt (Data Sent from Slave to Master after a Read Request): 
 * ============================================================================================================================================================
 * Header (Byte0), Len (Byte1), ErrSts (Byte2), Data (Byte3~ByteN) , CRC16 (ByteN+1 ~ ByteN+2)
 *
 * DispStatusReg       : [Header: 0x5A] [Len: 0x0F] [ErrSts: 0x00] [RegAddress: 0x00] [FuncEn: 0x00~0x03] [Digit0Val: 0x00~0x0A] [Digit1Val: 0x00~0x0A] 
 *                       [Digit2Val: 0x00~0x0A] [Digit3Val: 0x00~0x0A] [DpVal: 0x00~0x0F] [LDRADCH: 0x00~0xFF] [LDRADCL: 0x00~0xFF] [CurrBrightness: 0x00~0x64] 
 *                       [CRC16H] [CRC16L]
 * FuncEnReg           : [Header: 0x5A] [Len: 0x07] [ErrSts: 0x00] [RegAddress: 0x01] [FuncEn: 0x00~0x03] [CRC16H] [CRC16L]
 * DigitSingleReg      : [Header: 0x5A] [Len: 0x07] [ErrSts: 0x00] [RegAddress: 0x02] [DigitSingle: 0x00~0x0A] [CRC16H] [CRC16L]
 * DigitMultipleReg    : [Header: 0x5A] [Len: 0x0A] [ErrSts: 0x00] [RegAddress: 0x03] [Digit0Val: 0x00~0x0A] [Digit1Val: 0x00~0x0A] [Digit2Val: 0x00~0x0A] 
 *                       [Digit3Val: 0x00~0x0A] [CRC16H] [CRC16L]
 * DpSingleReg         : [Header: 0x5A] [Len: 0x07] [ErrSts: 0x00] [RegAddress: 0x04] [DpVal: 0x00~0x01] [CRC16H] [CRC16L]
 * DpMultipleReg       : [Header: 0x5A] [Len: 0x0A] [ErrSts: 0x00] [RegAddress: 0x05] [Dp0Val: 0x00~0x01] [Dp1Val: 0x00~0x01] [Dp2Val: 0x00~0x01] 
 *                       [Dp3Val: 0x00~0x01] [CRC16H] [CRC16L]
 * ManualBrightnessReg : [Header: 0x5A] [Len: 0x07] [ErrSts: 0x00] [RegAddress: 0x06] [Brightness: 0x00~0x64] [CRC16H] [CRC16L]
 * AutoBrightnessADCReg: [Header: 0x5A] [Len: 0x08] [ErrSts: 0x00] [RegAddress: 0x07] [ADCH: 0x00~0xFF] [ADCL: 0x00~0xFF] [CRC16H] [CRC16L]
 * 
 * 
 * Write Response Packet Foramt (Data Sent from Slave to Master after a Write Reuest):
 * ============================================================================================================================================================
 * Header (Byte0), Len (Byte1), ErrSts (Byte2) , CRC16 (ByteN+1 ~ ByteN+2)
 * Common Response     : [Header: 0x5A] [Len: 0x05] [ErrSts: 0x00] [CRC16H] [CRC16L]
 */




typedef struct packet_t{
	volatile uint8_t  Header;
	volatile uint8_t  Len;
	volatile uint8_t  CMD;
	volatile uint8_t  Reg;
	volatile uint8_t  Status;
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
	volatile uint8_t  SingleIndex;
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
	Protocol.TxPacket.Header  = PROTOCOL_HEADER_SLAVE;
	Protocol.TxPacket.Len     = 0x00;
	Protocol.TxPacket.CMD     = 0x00;
	Protocol.TxPacket.CRC16   = 0x00;
	
	Protocol.RxPacket.Status  = FALSE; 
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
	Protocol.RxPacket.SingleIndex = NULL;
}

uint8_t Protocol_Disp_Sts_Get(void){
	return (uint8_t)((LDR_Automic_Brightness_Sts_Get()<<1) | SevenSegment_Display_Enable_Sts());
}

void Protocol_Build_Ack_Nack_Packet(void){
	Protocol.TxBuf[0]  = Protocol.TxPacket.Header;
	Protocol.TxBuf[1]  = 5;
	Protocol.TxBuf[2]  = Protocol.RxPacket.Status;
  Protocol.TxPacket.CRC16 = COMM_CRC_Calculate_Block(Protocol.TxBuf, 3);
	Protocol.TxBuf[3]  = (Protocol.TxPacket.CRC16 >> 8);
	Protocol.TxBuf[4]  = (Protocol.TxPacket.CRC16 & 0xFF);
}


void Protocol_Build_Status_Packet(void){
	Protocol.TxBuf[0]  = Protocol.TxPacket.Header;
	Protocol.TxBuf[1]  = 15;   //Len
	Protocol.TxBuf[2]  = 0x00; //ErrSts
	Protocol.TxBuf[3]  = 0x00; //Reg
	Protocol.TxBuf[4]  = Protocol_Disp_Sts_Get();
	Protocol.TxBuf[5]  = SevenSegment_Segment_Char_Values_Get(0);
	Protocol.TxBuf[6]  = SevenSegment_Segment_Char_Values_Get(1);
	Protocol.TxBuf[7]  = SevenSegment_Segment_Char_Values_Get(2);
	Protocol.TxBuf[8]  = SevenSegment_Segment_Char_Values_Get(3);
	Protocol.TxBuf[9]  = SevenSegment_Dp_Byte_Get();
	Protocol.TxBuf[10] = (uint8_t)(LDR_Get_ADC_Val() >> 8);
	Protocol.TxBuf[11] = (LDR_Get_ADC_Val() & 0xFF);
	Protocol.TxBuf[12] = (LDR_Get_Current_Brightness() & 0xFF);
	
	
	Protocol.TxPacket.CRC16 = COMM_CRC_Calculate_Block(Protocol.TxBuf, 13);
	Protocol.TxBuf[13] = (Protocol.TxPacket.CRC16 >> 8);
	Protocol.TxBuf[14] = (Protocol.TxPacket.CRC16 & 0xFF);
}

void Protocol_Build_Func_En_Packet(void){
	Protocol.TxBuf[0] = Protocol.TxPacket.Header;
	Protocol.TxBuf[1] = 7;    //Len
	Protocol.TxBuf[2] = 0x00; //ErrSts
	Protocol.TxBuf[3] = 0x01; //Reg
	Protocol.TxBuf[4] = Protocol_Disp_Sts_Get();

	Protocol.TxPacket.CRC16 = COMM_CRC_Calculate_Block(Protocol.TxBuf, 5);
	Protocol.TxBuf[5] = (Protocol.TxPacket.CRC16 >> 8);
	Protocol.TxBuf[6] = (Protocol.TxPacket.CRC16 & 0xFF);
}

void Protocol_Build_Digit_Single_Packet(void){
	Protocol.TxBuf[0] = Protocol.TxPacket.Header;
	Protocol.TxBuf[1] = 7;    //Len
	Protocol.TxBuf[2] = 0x00; //ErrSts
	Protocol.TxBuf[3] = 0x02; //Reg
	Protocol.TxBuf[4] = SevenSegment_Segment_Char_Values_Get(Protocol.RxPacket.SingleIndex);

	Protocol.TxPacket.CRC16 = COMM_CRC_Calculate_Block(Protocol.TxBuf, 5);
	Protocol.TxBuf[5] = (Protocol.TxPacket.CRC16 >> 8);
	Protocol.TxBuf[6] = (Protocol.TxPacket.CRC16 & 0xFF);
}

void Protocol_Build_Digit_Multiple_Packet(void){
	Protocol.TxBuf[0] = Protocol.TxPacket.Header;
	Protocol.TxBuf[1] = 10;   //Len
	Protocol.TxBuf[2] = 0x00; //ErrSts
	Protocol.TxBuf[3] = 0x03; //Reg
	Protocol.TxBuf[4] = SevenSegment_Segment_Char_Values_Get(0);
	Protocol.TxBuf[5] = SevenSegment_Segment_Char_Values_Get(1);
	Protocol.TxBuf[6] = SevenSegment_Segment_Char_Values_Get(2);
	Protocol.TxBuf[7] = SevenSegment_Segment_Char_Values_Get(3);

	Protocol.TxPacket.CRC16 = COMM_CRC_Calculate_Block(Protocol.TxBuf, 8);
	Protocol.TxBuf[8] = (Protocol.TxPacket.CRC16 >> 8);
	Protocol.TxBuf[9] = (Protocol.TxPacket.CRC16 & 0xFF);
}

void Protocol_Build_Dp_Single_Packet(void){
	Protocol.TxBuf[0] = Protocol.TxPacket.Header;
	Protocol.TxBuf[1] = 7;    //Len
	Protocol.TxBuf[2] = 0x00; //ErrSts
	Protocol.TxBuf[3] = 0x04; //Reg
	Protocol.TxBuf[4] = SevenSegment_Dp_Values_Get(Protocol.RxPacket.SingleIndex);

	Protocol.TxPacket.CRC16 = COMM_CRC_Calculate_Block(Protocol.TxBuf, 5);
	Protocol.TxBuf[5] = (Protocol.TxPacket.CRC16 >> 8);
	Protocol.TxBuf[6] = (Protocol.TxPacket.CRC16 & 0xFF);
}

void Protocol_Build_Dp_Multiple_Packet(void){
	Protocol.TxBuf[0] = Protocol.TxPacket.Header;
	Protocol.TxBuf[1] = 10;   //Len
	Protocol.TxBuf[2] = 0x00; //ErrSts
	Protocol.TxBuf[3] = 0x05; //Reg
	Protocol.TxBuf[4] = SevenSegment_Dp_Values_Get(0);
	Protocol.TxBuf[5] = SevenSegment_Dp_Values_Get(1);
	Protocol.TxBuf[6] = SevenSegment_Dp_Values_Get(2);
	Protocol.TxBuf[7] = SevenSegment_Dp_Values_Get(3);

	Protocol.TxPacket.CRC16 = COMM_CRC_Calculate_Block(Protocol.TxBuf, 8);
	Protocol.TxBuf[8] = (Protocol.TxPacket.CRC16 >> 8);
	Protocol.TxBuf[9] = (Protocol.TxPacket.CRC16 & 0xFF);
}

void Protocol_Build_Manual_Brightness_Val_Packet(void){
	Protocol.TxBuf[0] = Protocol.TxPacket.Header;
	Protocol.TxBuf[1] = 7;    //Len
	Protocol.TxBuf[2] = 0x00; //ErrSts
	Protocol.TxBuf[3] = 0x06; //Reg
	Protocol.TxBuf[4] = (uint8_t)LDR_Manual_Brightness_Get();

	Protocol.TxPacket.CRC16 = COMM_CRC_Calculate_Block(Protocol.TxBuf, 5);
	Protocol.TxBuf[5] = (Protocol.TxPacket.CRC16 >> 8);
	Protocol.TxBuf[6] = (Protocol.TxPacket.CRC16 & 0xFF);
}

void Protocol_Build_Auto_Brightness_ADC_Val_Packet(void){
	Protocol.TxBuf[0] = Protocol.TxPacket.Header;
	Protocol.TxBuf[1] = 8;    //Len
	Protocol.TxBuf[2] = 0x00; //ErrSts
	Protocol.TxBuf[3] = 0x07; //Reg
	Protocol.TxBuf[4] = (uint8_t)(LDR_Get_ADC_Val() >> 8);
	Protocol.TxBuf[5] = (LDR_Get_ADC_Val() & 0xFF);

	Protocol.TxPacket.CRC16 = COMM_CRC_Calculate_Block(Protocol.TxBuf, 6);
	Protocol.TxBuf[6] = (Protocol.TxPacket.CRC16 >> 8);
	Protocol.TxBuf[7] = (Protocol.TxPacket.CRC16 & 0xFF);
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
	COMM_Tx_Buf(Protocol.TxBuf, Protocol.TxBuf[1]);
}






void Protocol_Response_Display_Status(uint8_t cmd){
	if(cmd == PROTOCOL_CMD_WRITE_REG){
		//No Write Access
		Protocol.RxPacket.Status |= PROTOCOL_ERROR_INVALID_CMD;
		Protocol.RxPacket.NackReturn = TRUE;
	}
	else if(cmd == PROTOCOL_CMD_READ_REG){
		//if len didn't matched, nack, generate error
		if(COMM_Buf_Get(1) != 0x06){
			Protocol.RxPacket.Status |= PROTOCOL_ERROR_LEN_MISMATCH;
			Protocol.RxPacket.NackReturn = TRUE;
		}
		else{
		  Protocol.RxPacket.ReadDisplaySts = TRUE;
		}
	}
	else{
		Protocol.RxPacket.Status |= PROTOCOL_ERROR_INVALID_CMD;
		Protocol.RxPacket.NackReturn = TRUE;
	}
}

void Protocol_Response_Function_Enable(uint8_t cmd, uint8_t data){
	if(cmd == PROTOCOL_CMD_WRITE_REG){
		//Check length is 7 bytes
		if(COMM_Buf_Get(1) != 0x07){
			Protocol.RxPacket.Status |= PROTOCOL_ERROR_LEN_MISMATCH;
			Protocol.RxPacket.NackReturn = TRUE;
		}
		else{
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
	}
	else if(cmd == PROTOCOL_CMD_READ_REG){
		//if len didn't matched, nack, generate error
		if(COMM_Buf_Get(1) != 0x06){
			Protocol.RxPacket.Status |= PROTOCOL_ERROR_LEN_MISMATCH;
			Protocol.RxPacket.NackReturn = TRUE;
		}
		else{
		  Protocol.RxPacket.ReadFuncEnable = TRUE;
		}
	}
	else{
		Protocol.RxPacket.Status |= PROTOCOL_ERROR_INVALID_CMD;
		Protocol.RxPacket.NackReturn = TRUE;
	}
}

void Protocol_Response_Digit_Single(uint8_t cmd, uint8_t data1, uint8_t data2){
	if(cmd == PROTOCOL_CMD_WRITE_REG){
		//Check length is 8 bytes
		if(COMM_Buf_Get(1) != 0x08){
			Protocol.RxPacket.Status |= PROTOCOL_ERROR_LEN_MISMATCH;
			Protocol.RxPacket.NackReturn = TRUE;
		}
		else{
			//digits are 0~3, values are 0~10 
			if((data1 < 4) && (data2 <11)){
				SevenSegment_Set_Value(data1, data2);
		    Protocol.RxPacket.AckReturn = TRUE;
			}
			else{
				Protocol.RxPacket.Status |= PROTOCOL_ERROR_DATA_OUT_OF_RANGE;
				Protocol.RxPacket.NackReturn = TRUE;
			}
		}
	}
	else if(cmd == PROTOCOL_CMD_READ_REG){
		//if len didn't matched, nack, generate error
		if(COMM_Buf_Get(1) != 0x07){
			Protocol.RxPacket.Status |= PROTOCOL_ERROR_LEN_MISMATCH;
			Protocol.RxPacket.NackReturn = TRUE;
		}
		else{
			Protocol.RxPacket.SingleIndex = data1;
			Protocol.RxPacket.ReadDigitSingle = TRUE;
		}
	}
	else{
		Protocol.RxPacket.Status |= PROTOCOL_ERROR_INVALID_CMD;
		Protocol.RxPacket.NackReturn = TRUE;
	}
}

void Protocol_Response_Digit_Multiple(uint8_t cmd, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4){
	if(cmd == PROTOCOL_CMD_WRITE_REG){
		//Check length is 10 bytes
		if(COMM_Buf_Get(1) != 0x0A){
			Protocol.RxPacket.Status |= PROTOCOL_ERROR_LEN_MISMATCH;
			Protocol.RxPacket.NackReturn = TRUE;
		}
		else{
			if( (data1 < 11) && (data2 < 11) && (data3 < 11) && (data4 < 11) ){
				SevenSegment_Set_Value(0, data1);
				SevenSegment_Set_Value(1, data2);
				SevenSegment_Set_Value(2, data3);
				SevenSegment_Set_Value(3, data4);
				Protocol.RxPacket.AckReturn = TRUE;
			}
			else{
				Protocol.RxPacket.Status |= PROTOCOL_ERROR_DATA_OUT_OF_RANGE;
				Protocol.RxPacket.NackReturn = TRUE;
			}
		}
	}
	else if(cmd == PROTOCOL_CMD_READ_REG){
		//if len didn't matched, nack, generate error
		if(COMM_Buf_Get(1) != 0x06){
			Protocol.RxPacket.Status |= PROTOCOL_ERROR_LEN_MISMATCH;
			Protocol.RxPacket.NackReturn = TRUE;
		}
		else{
			Protocol.RxPacket.ReadDigitMultiple = TRUE;
		}
	}
	else{
		Protocol.RxPacket.Status |= PROTOCOL_ERROR_INVALID_CMD;
		Protocol.RxPacket.NackReturn = TRUE;
	}
}

void Protocol_Response_Decimal_Point_Single(uint8_t cmd, uint8_t data1, uint8_t data2){
	if(cmd == PROTOCOL_CMD_WRITE_REG){
		//Check length is 8 bytes
		if(COMM_Buf_Get(1) != 0x08){
			Protocol.RxPacket.Status |= PROTOCOL_ERROR_LEN_MISMATCH;
			Protocol.RxPacket.NackReturn = TRUE;
		}
		else{
			if((data1 < 4) && (data2 <11)){
				SevenSegment_Set_Dp(data1, data2);
				Protocol.RxPacket.AckReturn = TRUE;
			}
			else{
				Protocol.RxPacket.Status |= PROTOCOL_ERROR_DATA_OUT_OF_RANGE;
				Protocol.RxPacket.NackReturn = TRUE;
			}
		}
	}
	else if(cmd == PROTOCOL_CMD_READ_REG){
		//if len didn't matched, nack, generate error
		if(COMM_Buf_Get(1) != 0x07){
			Protocol.RxPacket.Status |= PROTOCOL_ERROR_LEN_MISMATCH;
			Protocol.RxPacket.NackReturn = TRUE;
		}
		else{
			Protocol.RxPacket.SingleIndex = data1;
			Protocol.RxPacket.ReadDpSingle = TRUE;
		}
	}
	else{
		Protocol.RxPacket.Status |= PROTOCOL_ERROR_INVALID_CMD;
		Protocol.RxPacket.NackReturn = TRUE;
	}
}

void Protocol_Response_Decimal_Point_Multiple(uint8_t cmd, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4){
	if(cmd == PROTOCOL_CMD_WRITE_REG){
		//Check length is 10 bytes
		if(COMM_Buf_Get(1) != 0x0A){
			Protocol.RxPacket.Status |= PROTOCOL_ERROR_LEN_MISMATCH;
			Protocol.RxPacket.NackReturn = TRUE;
		}
		else{
			if( (data1 < 11) && (data2 < 11) && (data3 < 11) && (data4 < 11) ){
				SevenSegment_Set_Dp(0, data1);
				SevenSegment_Set_Dp(1, data2);
				SevenSegment_Set_Dp(2, data3);
				SevenSegment_Set_Dp(3, data4);
				Protocol.RxPacket.AckReturn = TRUE;
			}
			else{
				Protocol.RxPacket.Status |= PROTOCOL_ERROR_DATA_OUT_OF_RANGE;
				Protocol.RxPacket.NackReturn = TRUE;
			}
		}
	}
	else if(cmd == PROTOCOL_CMD_READ_REG){
		//if len didn't matched, nack, generate error
		if(COMM_Buf_Get(1) != 0x06){
			Protocol.RxPacket.Status |= PROTOCOL_ERROR_LEN_MISMATCH;
			Protocol.RxPacket.NackReturn = TRUE;
		}
		else{
			Protocol.RxPacket.ReadDpMultiple = TRUE;
		}
	}
	else{
		Protocol.RxPacket.Status |= PROTOCOL_ERROR_INVALID_CMD;
		Protocol.RxPacket.NackReturn = TRUE;
	}
}

void Protocol_Response_Manual_Brightness(uint8_t cmd, uint8_t data){
	if(cmd == PROTOCOL_CMD_WRITE_REG){
		//Check length is 7 bytes
		if(COMM_Buf_Get(1) != 0x07){
			Protocol.RxPacket.Status |= PROTOCOL_ERROR_LEN_MISMATCH;
			Protocol.RxPacket.NackReturn = TRUE;
		}
		else{
			if(data <= 100){
				LDR_Manual_Brightness_Set(data);
				Protocol.RxPacket.AckReturn = TRUE;
			}
			else{
				Protocol.RxPacket.Status |= PROTOCOL_ERROR_DATA_OUT_OF_RANGE;
				Protocol.RxPacket.NackReturn = TRUE;
			}
		}
	}
	else if(cmd == PROTOCOL_CMD_READ_REG){
		//if len didn't matched, nack, generate error
		if(COMM_Buf_Get(1) != 0x06){
			Protocol.RxPacket.Status |= PROTOCOL_ERROR_LEN_MISMATCH;
			Protocol.RxPacket.NackReturn = TRUE;
		}
		else{
			Protocol.RxPacket.ReadManualBrightness = TRUE;
		}
	}
	else{
		Protocol.RxPacket.Status |= PROTOCOL_ERROR_INVALID_CMD;
		Protocol.RxPacket.NackReturn = TRUE;
	}
}

void Protocol_Response_Auto_Brightness_ADC(uint8_t cmd){
	if(cmd == PROTOCOL_CMD_WRITE_REG){
		//No Write Access
		Protocol.RxPacket.Status |= PROTOCOL_ERROR_INVALID_CMD;
		Protocol.RxPacket.NackReturn = TRUE;
	}
	else if(cmd == PROTOCOL_CMD_READ_REG){
		//if len didn't matched, nack, generate error
		if(COMM_Buf_Get(1) != 0x06){
			Protocol.RxPacket.Status |= PROTOCOL_ERROR_LEN_MISMATCH;
			Protocol.RxPacket.NackReturn = TRUE;
		}
		else{
			Protocol.RxPacket.ReadBrightnessADC = TRUE;
		}
	}
	else{
		Protocol.RxPacket.Status |= PROTOCOL_ERROR_INVALID_CMD;
		Protocol.RxPacket.NackReturn = TRUE;
	}
}


void Protocol_Error_Check(void){
	//Clear All Errors
	Protocol.RxPacket.Status = 0x00;
	
	//Check CRC Mismatch, BIT0
	if(COMM_Data_CRC_Status_Get() == FALSE){
		Protocol.RxPacket.Status |= PROTOCOL_ERROR_CRC_MISMATCH;
	}
	
	//Check Invalid Command, BIT1
	if(COMM_Buf_Get(3) > 0x0B){
	  Protocol.RxPacket.Status |= PROTOCOL_ERROR_INVALID_CMD;
	}
	
	//Check Length Mismatch, BIT2
	if( (COMM_Buf_Get(1) != COMM_Data_Len_Get()) || (COMM_Data_Len_Get()<3) ){
	  Protocol.RxPacket.Status |= PROTOCOL_ERROR_LEN_MISMATCH;
	}
	
	//Check Header Mismatch, BIT3
	if(COMM_Buf_Get(0) != PROTOCOL_HEADER_MASTER){
	  Protocol.RxPacket.Status |= PROTOCOL_ERROR_HEADER_MISMATCH;
	}
	
	//Check Data Out Of Range, BIT4
	//Need to implement
	
	//Check Invalid Parameter, BIT5
	//Need to implement
	
}

void Protocol_Disassemble_Packet(void){
	
	Protocol_Error_Check();
	
	if(Protocol.RxPacket.Status == 0x00){
		switch(COMM_Buf_Get(3)){
			case PROTOCOL_REG_DISPLAY_STATUS:
				Protocol_Response_Display_Status(COMM_Buf_Get(2));
			  break;
			
			case PROTOCOL_REG_FUNC_ENABLE:
				Protocol_Response_Function_Enable(COMM_Buf_Get(2), COMM_Buf_Get(4));
			  break;
			
			case PROTOCOL_REG_DIGIT_SINGLE:
				Protocol_Response_Digit_Single(COMM_Buf_Get(2), COMM_Buf_Get(4), COMM_Buf_Get(5));
			  break;
		
		  case PROTOCOL_REG_DIGIT_MULTIPLE:
				Protocol_Response_Digit_Multiple(COMM_Buf_Get(2), COMM_Buf_Get(4), COMM_Buf_Get(5), COMM_Buf_Get(6), COMM_Buf_Get(7));
			  break;

		  case PROTOCOL_REG_DECIMAL_POINTS_SINGLE:
				Protocol_Response_Decimal_Point_Single(COMM_Buf_Get(2), COMM_Buf_Get(4), COMM_Buf_Get(5));
			  break;
			
			case PROTOCOL_REG_DECIMAL_POINTS_MULTIPLE:
				Protocol_Response_Decimal_Point_Multiple(COMM_Buf_Get(2), COMM_Buf_Get(4), COMM_Buf_Get(5), COMM_Buf_Get(6), COMM_Buf_Get(7));
			  break;
      
			case PROTOCOL_REG_MANUAL_BRGHTNSS_VAL:
				Protocol_Response_Manual_Brightness(COMM_Buf_Get(2), COMM_Buf_Get(4));
			  break;
			
			case PROTOCOL_REG_AUTO_BRGHTNSS_ADC_VAL:
				Protocol_Response_Auto_Brightness_ADC(COMM_Buf_Get(2));
			  break;
			
			case PROTOCOL_REG_AUTO_BRGHTNSS_SLOPE_ADCH:
				//add handler
			  break;
			
			case PROTOCOL_REG_AUTO_BRGHTNSS_SLOPE_ADCL:
				//add handler
			  break;
			
			case PROTOCOL_REG_AUTO_BRGHTNSS_SLOPE_VALH:
				//add handler
			  break;
			
			case PROTOCOL_REG_AUTO_BRGHTNSS_SLOPE_VALL:
				//add handler
			  break;
		}
	}
	if(Protocol.RxPacket.Status != 0x00){
		Protocol.RxPacket.NackReturn = TRUE;
	}
}




void Protocol_Response_Mainloop(void){
	
	if(COMM_Data_CRC_Status_Get() == FALSE){
		COMM_RX_Packet_Read_Complete();
	}
	
	if( (Protocol.RxPacket.AckReturn == TRUE) || (Protocol.RxPacket.NackReturn == TRUE)){
		Protocol_Build_Ack_Nack_Packet();
		Protocol_Transmit_Packet();
		Protocol.RxPacket.AckReturn  = FALSE;
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
		Protocol_Build_Digit_Single_Packet();
		Protocol_Transmit_Packet();
		Protocol.RxPacket.ReadDigitSingle = FALSE;
	}
	else if(Protocol.RxPacket.ReadDigitMultiple == TRUE){
		Protocol_Build_Digit_Multiple_Packet();
		Protocol_Transmit_Packet();
		Protocol.RxPacket.ReadDigitMultiple = FALSE;
	}
	else if(Protocol.RxPacket.ReadDpSingle == TRUE){
		Protocol_Build_Dp_Single_Packet();
		Protocol_Transmit_Packet();
		Protocol.RxPacket.ReadDpSingle = FALSE;
	}
	else if(Protocol.RxPacket.ReadDpMultiple == TRUE){
		Protocol_Build_Dp_Multiple_Packet();
		Protocol_Transmit_Packet();
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
	
