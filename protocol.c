

#include "stm32g030xx.h"
#include "protocol.h"


/*
Frame Format : Header (1 byte), Len (1 byte), CMD (1 byte), Data, CRC16 (2 byte)
Header       : 1 Byte (Master 0xA5, Slave 0x5A)
Length       : N bytes + 3
CMD          : 1 Byte [Status (0x00), Config (0x01), Set Digits (0x02), SetBrightness(0x03)]
Data         : Variable
CRC16        : 2 Byte
*/

enum{
	CMD_STATUS         = 0x00,
	CMD_CONFIG         = 0x01,
	CMD_SET_DIGIT      = 0x02,
	CMD_SET_BRIGHTNESS = 0x03
};

typedef struct packet_t{
	volatile uint8_t Header;
	volatile uint8_t Len;
	volatile uint8_t CMD;
	volatile uint8_t CRC16;
}packet_t;

typedef struct protocol_t{
	volatile uint8_t RxBuf[10];
	packet_t         RxPacket;
	volatile uint8_t TxBuf[10];
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

void Protocol_Build_Status_Packet(void){
	Protocol.TxBuf[0] = Protocol.TxPacket.Header;
	Protocol.TxBuf[1] = 0x05;
	Protocol.TxBuf[2] = CMD_STATUS;
	Protocol.TxBuf[3] = Protocol.TxPacket.Header;
	Protocol.TxBuf[4] = Protocol.TxPacket.Header;
	Protocol.TxBuf[5] = Protocol.TxPacket.Header;
}












