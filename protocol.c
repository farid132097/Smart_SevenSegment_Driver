

#include "stm32g030xx.h"
#include "protocol.h"
#include "sevensegment.h"
#include "dispcom.h"
#include "ldr.h"

/*
Frame Format : Header (1 byte), Len (1 byte), CMD (1 byte), Data, CRC16 (2 byte)
Header       : 1 Byte (Master 0xA5, Slave 0x5A)
Length       : N bytes + 3
CMD          : 1 Byte [Status (0x00), Config (0x01), Set Digits (0x02), SetBrightness(0x03)]
Data         : Variable
CRC16        : 2 Byte
*/



typedef struct packet_t{
	volatile uint8_t  Header;
	volatile uint8_t  Len;
	volatile uint8_t  CMD;
	volatile uint8_t  Reg;
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
	Protocol.TxPacket.Header = 0x5A;
	Protocol.TxPacket.Len    = 0x00;
	Protocol.TxPacket.CMD    = 0x00;
	Protocol.TxPacket.CRC16  = 0x00;
}

uint8_t Protocol_Disp_Sts_Get(void){
	return (LDR_Automic_Brightness_Sts_Get()<<1);
}

void Protocol_Build_Status_Packet(void){
	Protocol.TxBuf[0]  = Protocol.TxPacket.Header;
	Protocol.TxBuf[1]  = 13;
	Protocol.TxBuf[2]  = SevenSegment_Segment_Char_Values_Get(0);
	Protocol.TxBuf[3]  = SevenSegment_Segment_Char_Values_Get(1);
	Protocol.TxBuf[4]  = SevenSegment_Segment_Char_Values_Get(2);
	Protocol.TxBuf[5]  = SevenSegment_Segment_Char_Values_Get(3);
	Protocol.TxBuf[6]  = SevenSegment_Dp_Byte_Get();
	Protocol.TxBuf[7]  = (LDR_Get_ADC_Val() >> 8);
	Protocol.TxBuf[8]  = (LDR_Get_ADC_Val() & 0xFF);
	Protocol.TxBuf[9]  = (LDR_Get_Current_Brightness() & 0xFF);
	Protocol.TxBuf[10] = Protocol_Disp_Sts_Get();
	
	Protocol.TxPacket.CRC16 = DispCom_CRC_Calculate_Block(Protocol.TxBuf, 10);
	Protocol.TxBuf[11] = (Protocol.TxPacket.CRC16 >> 8);
	Protocol.TxBuf[12] = (Protocol.TxPacket.CRC16 & 0xFF);
}

void Protocol_Build_Func_En_Packet(void){
	Protocol.TxBuf[0] = Protocol.TxPacket.Header;
	Protocol.TxBuf[1] = 5;
	Protocol.TxBuf[2] = Protocol_Disp_Sts_Get();

	Protocol.TxPacket.CRC16 = DispCom_CRC_Calculate_Block(Protocol.TxBuf, 3);
	Protocol.TxBuf[3] = (Protocol.TxPacket.CRC16 >> 8);
	Protocol.TxBuf[4] = (Protocol.TxPacket.CRC16 & 0xFF);
}

void Protocol_Build_Manual_Brightness_Val_Packet(void){
	Protocol.TxBuf[0] = Protocol.TxPacket.Header;
	Protocol.TxBuf[1] = 5;
	Protocol.TxBuf[2] = (uint8_t)LDR_Manual_Brightness_Get();

	Protocol.TxPacket.CRC16 = DispCom_CRC_Calculate_Block(Protocol.TxBuf, 3);
	Protocol.TxBuf[3] = (Protocol.TxPacket.CRC16 >> 8);
	Protocol.TxBuf[4] = (Protocol.TxPacket.CRC16 & 0xFF);
}

void Protocol_Build_Auto_Brightness_ADC_Val_Packet(void){
	Protocol.TxBuf[0] = Protocol.TxPacket.Header;
	Protocol.TxBuf[1] = 6;
	Protocol.TxBuf[2] = (LDR_Get_ADC_Val() >> 8);
	Protocol.TxBuf[3] = (LDR_Get_ADC_Val() & 0xFF);

	Protocol.TxPacket.CRC16 = DispCom_CRC_Calculate_Block(Protocol.TxBuf, 3);
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
	DispCom_Tx_Buf((uint8_t*)Protocol.TxBuf, Protocol.TxBuf[1]);
}







void Protocol_Init(void){
	Protocol_Struct_Init();
}
	
