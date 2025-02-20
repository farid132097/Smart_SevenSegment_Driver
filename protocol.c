

#include "stm32g030xx.h"
#include "protocol.h"


/*
Frame Format : Header (1 byte), Len (1 byte), CMD (1 byte), Data, CRC16 (2 byte)
Header       : 1 Byte (Master 0x53, Slave 0x55)
Length       : N bytes + 3
CMD          : 1 Byte [Query (0x00), Config (0x01), Set Digits (0x02), SetBrightness(0x03)]
Data         : Variable
CRC16        : 2 Byte
*/

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
	Protocol.TxPacket.Header = 0xA5;
	Protocol.TxPacket.Header = 0xA5;
}








