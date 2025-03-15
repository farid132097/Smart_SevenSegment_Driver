

/*
 * File:   comm.c
 * Author: MD. Faridul Islam
 * faridmdislam@gmail.com
 * LL Driver -> Communication Library (Based on UART)
 * Rev 3.2
 * Created on December 12, 2024, 9:44 PM
 */


//add headers according to hardware
#include "stm32g030xx.h"
#include "protocol.h"

//must be included
#include "comm.h"


#define  COMM_DOUBLE_SPEED
#define  COMM_ENABLE_TX    
#define  COMM_ENABLE_RX    
#define  COMM_ENABLE_RX_INT

#define  COMM_BUFFER_SIZE               (  64U)
#define  COMM_RX_PCKT_CMPLT_DELAY       (   5U)
#define  COMM_INT_PRIORITY              (   1U)
#define  COMM_TIMEOUT_INT_PRIORITY      (   2U)


//#define  COMM_CRC_ENABLE     //Uncomment if packet validation by CRC is needed
#define  COMM_CRC_XMODEM     //Uncomment if CRC is by X-MODEM Protocol
//#define  COMM_CRC_ALTERNATE     //Uncomment if CRC is by Supplier Protocol




//Define Software Error Codes
#define  COMM_RX_ERR_NO_ERR             (0x00U)
#define  COMM_RX_ERR_FRAMING            (0x01U)
#define  COMM_RX_ERR_OVERRUN            (0x02U)
#define  COMM_RX_ERR_READ_INCOMPLETE    (0x10U)


typedef struct comm_timer_t{
  volatile uint8_t   Enabled;
  volatile uint8_t   ResetVal;
}comm_timer_t;

typedef struct comm_rx_packet_t{
  volatile uint16_t  CalculatedCRC;
  volatile uint16_t  ReceivedCRC;
  volatile uint8_t   CRCStatus;
  volatile uint8_t   DataAvailable;
  volatile uint8_t   DataReadComplete;
  volatile uint8_t   Reserved;
}comm_rx_packet_t;

typedef struct comm_t{
  volatile uint8_t   Error;
  uint8_t            Digits[8];
  uint8_t            InputNumDigits;

  volatile uint8_t   LastRxByte;
  volatile uint8_t   Buf[COMM_BUFFER_SIZE];
  volatile uint8_t   Reserved;
  volatile uint16_t  BufSize;
  volatile uint16_t  BufIndex;
  
  comm_timer_t       Timer;
  
  comm_rx_packet_t   RxPacket;
}comm_t;


enum{
  COMM_FALSE = 0,
  COMM_TRUE  = 1,
  COMM_NULL  = 0
};


static comm_t COMM;






/*******************COMM Structure Functions Start****************/

void COMM_Struct_Init(void){
  COMM.Error = 0;
  for(uint8_t i = 0; i < 8; i++){
    COMM.Digits[i] = COMM_NULL;
  }
  COMM.InputNumDigits = COMM_NULL;
  COMM.LastRxByte = COMM_NULL;
  COMM.BufSize = COMM_BUFFER_SIZE;
  COMM.BufIndex = 0;
  for(uint8_t i = 0; i < COMM.BufSize; i++){
    COMM.Buf[i] = COMM_NULL;
  }
}

void COMM_RX_Packet_Struct_Init(void){
  COMM.RxPacket.CalculatedCRC    = COMM_NULL;
  COMM.RxPacket.ReceivedCRC      = COMM_NULL;
  COMM.RxPacket.CRCStatus        = COMM_FALSE;
  COMM.RxPacket.DataAvailable    = COMM_FALSE;
  COMM.RxPacket.DataReadComplete = COMM_TRUE;
}

/********************COMM Structure Functions End*****************/









/*********************COMM Init Functions Start******************/

void COMM_Config_GPIO(void){
  if((RCC->IOPENR & RCC_IOPENR_GPIOBEN) != RCC_IOPENR_GPIOBEN){
		RCC->IOPENR |=  RCC_IOPENR_GPIOBEN;
	}
	//PB6 alternate function, USART1->TX
	GPIOB->MODER  &=~ GPIO_MODER_MODE6_Msk;
	GPIOB->MODER  |=  GPIO_MODER_MODE6_1;
	
	//PB7 alternate function, USART1->RX
	GPIOB->MODER  &=~ GPIO_MODER_MODE7_Msk;
	GPIOB->MODER  |=  GPIO_MODER_MODE7_1;
}

void COMM_Config_Clock(void){
  RCC->APBENR2  |=  RCC_APBENR2_USART1EN; 
}

void COMM_Config_BAUD_Rate(uint32_t baud_rate){
  if(USART1->CR1 &  USART_CR1_UE){
    USART1->CR1 &=~ USART_CR1_UE;
	}
  USART1->BRR = (uint16_t)(16000000/baud_rate);
}


void COMM_Config_Tx(void){
  USART1->CR1   |= USART_CR1_TE;
	if((USART1->CR1 & USART_CR1_UE) != USART_CR1_UE){
	  USART1->CR1 |= USART_CR1_UE;
	}
}


void COMM_Config_Rx(void){
  USART1->CR1   |= USART_CR1_RE;
	if((USART1->CR1 & USART_CR1_UE) != USART_CR1_UE){
	  USART1->CR1 |= USART_CR1_UE;
	}
}

void COMM_Config_Rx_Interrupt(void){
  USART1->CR1  |= USART_CR1_RXNEIE_RXFNEIE;
	NVIC_EnableIRQ(USART1_IRQn);
	NVIC_SetPriority(USART1_IRQn, COMM_INT_PRIORITY);
}

void COMM_Clear_Interrupt_Flag(void){
  //Clear flag if necessary
}

void COMM_Tx_Byte(uint8_t val){
  USART1->TDR = val;
	while((USART1->ISR & USART_ISR_TC) != USART_ISR_TC){
		//Add timeout
	}
	USART1->ICR |= USART_ICR_TCCF;                                                                                                  
}

uint8_t COMM_Rx_Byte(void){
  volatile uint8_t val = 0;
  val = (uint8_t)USART1->RDR;
  return val;
}


//add comm interrupt handler vector
//call COMM_ISR_Handler() inside ISR


void USART1_IRQHandler(void){
	COMM_ISR_Handler();
}

/**********************COMM Init Functions End*******************/









/********************COMM Timer Functions Start*****************/

void COMM_Timer_Struct_Init(void){
  COMM.Timer.Enabled = COMM_FALSE;
  COMM.Timer.ResetVal = COMM_NULL;
}

void COMM_Timer_Init(void){
  //config DispCom timer for auto packet validation
	//calculate DispCom.Timer.ResetVal if overflow int is used
	//timer resolution should be 1ms
	//if other than 1ms is used, adjust DISPCOM_RX_PCKT_CMPLT_DELAY
	//TIM14 for Timebase
	RCC->APBENR2 |= RCC_APBENR2_TIM16EN;
	TIM16->PSC    = 15999;
	TIM16->ARR    = COMM_RX_PCKT_CMPLT_DELAY;
	TIM16->DIER  |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM16_IRQn);
	NVIC_SetPriority(TIM16_IRQn, COMM_TIMEOUT_INT_PRIORITY);
	TIM16->SR &=~ TIM_SR_CC1IF;
	TIM16->SR &=~ TIM_SR_UIF;
}

void COMM_Timer_Enable(void){
  TIM16->CR1   |=  TIM_CR1_CEN;
}

void COMM_Timer_Disable(void){ 
  TIM16->CR1   &=~ TIM_CR1_CEN;
}

uint8_t COMM_Timer_Get_Status(void){
  return COMM.Timer.Enabled;
}

uint16_t COMM_Timer_Get_Val(void){
  //return current timer val
	return (uint16_t)TIM16->CNT;
}


void COMM_Timer_Value_Reset(void){
  //reset timer val if compare mode selected
	TIM16->CNT = 0;
	//set timer val to DispCom.Timer.ResetVal if Overflow int is used
}

void COMM_Timer_Clear_Interrupt_Flag(void){
  //Clear flag if necessary
	TIM16->SR &=~ TIM_SR_CC1IF;
	TIM16->SR &=~ TIM_SR_UIF;
}

//add comm timer interrupt handler vector
//call COMM_Timer_ISR_Handler() inside ISR

void TIM16_IRQHandler(void){
	COMM_Timer_ISR_Handler();
}


/*********************COMM Timer Functions End******************/









/********************Buffer Tx Functions Start*******************/

void COMM_Tx_Buf(uint8_t *data, uint8_t len){
  for(uint16_t i = 0; i < len; i++){
	COMM_Tx_Byte( data[i] );
  }
}

/*********************Buffer Tx Functions End********************/









/*******************End Char Functions Start******************/

void COMM_Tx_NL(void){
  COMM_Tx_Byte('\r');
  COMM_Tx_Byte('\n');
}

void COMM_Tx_SP(void){
  COMM_Tx_Byte(' ');
}

void COMM_Tx_CM(void){
  COMM_Tx_Byte(',');
}

/*******************End Char Functions End*******************/









/*********************Text Functions Start*******************/

void COMM_Tx_Text(char *str){
  uint8_t i = 0;
  while(str[i] != '\0'){
    COMM_Tx_Byte(str[i]);
    i++;
  }
}

void COMM_Tx_Text_NL(char *str){
  COMM_Tx_Text(str);
  COMM_Tx_NL();
}

void COMM_Tx_Text_SP(char *str){
  COMM_Tx_Text(str);
  COMM_Tx_SP();
}

void COMM_Tx_Text_CM(char *str){
  COMM_Tx_Text(str);
  COMM_Tx_CM();
}

/*********************Text Functions End********************/









/*********************Number Functions Start********************/

void COMM_Determine_Digit_Numbers(uint32_t num){
  uint8_t i = 0;
  if(num == 0){
    COMM.Digits[0] = 0;
    COMM.InputNumDigits = 1;
  }else{
    while(num != 0){
      COMM.Digits[i] = num%10;
      num /= 10;
      i++;
    }
	COMM.InputNumDigits = i;
  }
}

void COMM_Tx_Number_Digits(void){
  for(uint8_t i = COMM.InputNumDigits; i > 0; i--){
    uint8_t temp = i;
    temp -= 1;
    temp  = COMM.Digits[temp];
    temp += 48;
    COMM_Tx_Byte(temp);
  }
}

void COMM_Tx_Number(int32_t num){
  if(num < 0){
    COMM_Tx_Byte('-');
	  num = -num;
  }
  COMM_Determine_Digit_Numbers((uint32_t)num);
  COMM_Tx_Number_Digits();
}

void COMM_Tx_Number_Hex(uint32_t val){
  uint16_t hex_digit, index = 0, loop_counter = 0;
  if(val <= 0xFF){
    index = 8;
    loop_counter = 2;
  }else if(val <= 0xFFFF){
    index = 16;
    loop_counter = 4;     
  }else{
    index = 32;
    loop_counter = 8;
  }
  COMM_Tx_Byte('0');
  COMM_Tx_Byte('x');
  for(uint8_t i = 0; i < loop_counter; i++){
	index -= 4;
	hex_digit = (uint8_t)((val>>index) & 0x0F);
	if(hex_digit > 9){
	  hex_digit += 55;
	}
	else{
	  hex_digit += 48;
	}
	COMM_Tx_Byte((uint8_t)hex_digit);
  }
}

void COMM_Tx_Number_Bin(uint32_t val){
  uint8_t loop_counter = 0;
  if(val <= 0xFF){
    loop_counter = 7;
  }else if(val <= 0xFFFF){
    loop_counter = 15;     
  }else{
    loop_counter = 31;
  }
  
  COMM_Tx_Byte('0');
  COMM_Tx_Byte('b');
  for(int i = loop_counter; i >= 0; i--){
    if( (val>>i) & 1){
      COMM_Tx_Byte( 49 );   
    }else{
      COMM_Tx_Byte( 48 );         
    }
  }
}

/*********************Number Functions End*********************/









/************Number with End Char Functions Start**************/

void COMM_Tx_Number_NL(int32_t num){
  COMM_Tx_Number(num);
  COMM_Tx_NL();
}

void COMM_Tx_Number_SP(int32_t num){
  COMM_Tx_Number(num);
  COMM_Tx_SP();
}

void COMM_Tx_Number_CM(int32_t num){
  COMM_Tx_Number(num);
  COMM_Tx_CM();
}

/*************Number with End Char Functions End***************/









/**********Hex Number with End Char Functions Start************/

void COMM_Tx_Number_Hex_NL(uint32_t num){
  COMM_Tx_Number_Hex(num);
  COMM_Tx_NL();
}

void COMM_Tx_Number_Hex_SP(uint32_t num){
  COMM_Tx_Number_Hex(num);
  COMM_Tx_SP();
}

void COMM_Tx_Number_Hex_CM(uint32_t num){
  COMM_Tx_Number_Hex(num);
  COMM_Tx_CM();
}

/***********Hex Number with End Char Functions End*************/









/**********Bin Number with End Char Functions Start************/

void COMM_Tx_Number_Bin_NL(uint32_t num){
  COMM_Tx_Number_Bin(num);
  COMM_Tx_NL();
}

void COMM_Tx_Number_Bin_SP(uint32_t num){
  COMM_Tx_Number_Bin(num);
  COMM_Tx_SP();
}

void COMM_Tx_Number_Bin_CM(uint32_t num){
  COMM_Tx_Number_Bin(num);
  COMM_Tx_CM();
}

/***********Bin Number with End Char Functions End*************/







/************Number with Parameter Functions Start*************/

void COMM_Tx_Parameter_NL(char *name, int32_t num){
  COMM_Tx_Text(name);
  COMM_Tx_SP();
  COMM_Tx_Number_NL(num);
}

void COMM_Tx_Parameter_SP(char *name, int32_t num){
  COMM_Tx_Text(name);
  COMM_Tx_SP();
  COMM_Tx_Number_SP(num);
}

void COMM_Tx_Parameter_CM(char *name, int32_t num){
  COMM_Tx_Text(name);
  COMM_Tx_SP();
  COMM_Tx_Number_CM(num);
}

/*************Number with Parameter Functions End**************/









/**********Hex Number with Parameter Functions Start***********/

void COMM_Tx_Parameter_Hex_NL(char *name, uint32_t num){
  COMM_Tx_Text(name);
  COMM_Tx_SP();
  COMM_Tx_Number_Hex_NL(num);
}

void COMM_Tx_Parameter_Hex_SP(char *name, uint32_t num){
  COMM_Tx_Text(name);
  COMM_Tx_SP();
  COMM_Tx_Number_Hex_SP(num);
}

void COMM_Tx_Parameter_Hex_CM(char *name, uint32_t num){
  COMM_Tx_Text(name);
  COMM_Tx_SP();
  COMM_Tx_Number_Hex_CM(num);
}

/***********Hex Number with Parameter Functions End************/









/**********Bin Number with Parameter Functions Start***********/

void COMM_Tx_Parameter_Bin_NL(char *name, uint32_t num){
  COMM_Tx_Text(name);
  COMM_Tx_SP();
  COMM_Tx_Number_Bin_NL(num);
}

void COMM_Tx_Parameter_Bin_SP(char *name, uint32_t num){
  COMM_Tx_Text(name);
  COMM_Tx_SP();
  COMM_Tx_Number_Bin_SP(num);
}

void COMM_Tx_Parameter_Bin_CM(char *name, uint32_t num){
  COMM_Tx_Text(name);
  COMM_Tx_SP();
  COMM_Tx_Number_Bin_CM(num);
}

/***********Bin Number with Parameter Functions End************/









/*******************COMM Buffer Functions Start***************/

void COMM_Buf_Flush(void){
  for(uint8_t i = 0; i < COMM_BUFFER_SIZE; i++){
	COMM.Buf[i] = 0;
  }
  COMM.BufIndex = 0;
}

uint8_t COMM_Buf_Get(uint16_t index){
  return COMM.Buf[index];
}

uint16_t COMM_Buf_Get_Index(void){
  return COMM.BufIndex;
}

/********************COMM Buffer Functions End****************/









/*******************COMM Data Functions Start****************/

uint8_t COMM_Data_Available(void){
  return COMM.RxPacket.DataAvailable;
}

uint16_t COMM_Data_Len_Get(void){
  return COMM_Buf_Get_Index();
}

uint16_t COMM_Data_Calculated_CRC_Get(void){
  return COMM.RxPacket.CalculatedCRC;
}

uint16_t COMM_Data_Received_CRC_Get(void){
  return COMM.RxPacket.ReceivedCRC;
}

uint8_t COMM_Data_CRC_Status_Get(void){
  return COMM.RxPacket.CRCStatus;
}

uint8_t COMM_Data_Read_Complete_Status(void){
  return COMM.RxPacket.DataReadComplete;
}

void COMM_Data_Clear_Available_Flag(void){
  COMM.RxPacket.DataAvailable = COMM_FALSE;
}

void COMM_Data_Clear_Read_Complete_Flag(void){
  COMM.RxPacket.DataReadComplete = COMM_TRUE;
}



void COMM_Data_Copy_Buf(uint8_t *buf){
  for(uint16_t i = 0; i < COMM_Data_Len_Get(); i++){
	buf[i] = COMM_Buf_Get(i);
  }
}


void COMM_Data_Print_Buf(void){
  for(uint16_t i = 0; i < COMM_Data_Len_Get(); i++){
	COMM_Tx_Byte( COMM_Buf_Get(i) );
  }
  COMM_Tx_NL();
}

/********************COMM Data Functions End*****************/









/******************Error Code Functions Start****************/

uint8_t COMM_Error_Code_Get(void){
  return COMM.Error;
}

void COMM_Error_Code_Clear(void){
  COMM.Error = 0;
}

/******************Error Code Functions End******************/









/***************COMM ISR Handler Functions Start************/

void COMM_ISR_Handler(void){
  COMM_Clear_Interrupt_Flag();
  COMM.LastRxByte = (uint8_t)COMM_Rx_Byte();
  if(COMM.Error == 0x00){
    COMM.Buf[COMM.BufIndex] = COMM.LastRxByte;
    COMM.BufIndex++;
    if(COMM.BufIndex >= COMM.BufSize){
      COMM.BufIndex = 0;
    }
  }
  else{
    COMM.LastRxByte = COMM_NULL;
  }
  
  COMM_Timer_Value_Reset();
  if(COMM.Timer.Enabled == COMM_FALSE){
	COMM_Timer_Enable();
	COMM.Timer.Enabled = COMM_TRUE;
  }
  
}

void COMM_Timer_ISR_Handler(void){
  COMM_Timer_Clear_Interrupt_Flag();
  if(COMM.Timer.Enabled == COMM_TRUE){
    COMM_Timer_Disable();
	  COMM.Timer.Enabled = COMM_FALSE;
  }
  
  if(COMM_Buf_Get_Index() != COMM_NULL){
	  if(COMM.RxPacket.DataReadComplete == COMM_FALSE){
	    COMM.Error = COMM_RX_ERR_READ_INCOMPLETE;
	  }
    COMM_RX_Packet_CRC_Check();
    #ifdef COMM_CRC_ENABLE
	  if(COMM.RxPacket.CRCStatus == COMM_TRUE){
			COMM_RX_Packet_Disassemble();
	    COMM.RxPacket.DataAvailable = COMM_TRUE;
	  }
	  else{
	    COMM_RX_Packet_Read_Complete();
	    COMM.RxPacket.DataAvailable = COMM_FALSE;
	  }
	  #else
		COMM_RX_Packet_Disassemble();
	  COMM.RxPacket.DataAvailable = COMM_TRUE;
	  #endif
	
	  COMM.RxPacket.DataReadComplete = COMM_FALSE;
  }
}

/****************COMM ISR Handler Functions End*************/









/******************COMM CRC Functions Start****************/

#ifdef   COMM_CRC_XMODEM

uint16_t COMM_CRC_Calculate_Byte(uint16_t crc, uint8_t data){
	uint16_t temp = data;
	temp <<= 8;
  crc = crc ^ temp;
  for(uint8_t i = 0; i < 8; i++){
    if(crc & 0x8000){
			temp   = crc;
			temp <<= 0x01;
			temp  ^= 0x1021;
	    crc = temp;
	  }
    else{
	    crc <<= 1;
	  }
  }
  return crc;
}

uint16_t COMM_CRC_Calculate_Block(uint8_t *buf, uint8_t len){
  uint16_t crc = 0;
  for(uint8_t i = 0; i < len; i++){
    crc = COMM_CRC_Calculate_Byte(crc,buf[i]);
  }
  return crc;
}
#endif

#ifdef   COMM_CRC_ALTERNATE

uint16_t CRCTalbe[16] = {
 0x0000, 0xCC01, 0xD801, 0x1400,
 0xF001, 0x3C00, 0x2800, 0xE401,
 0xA001, 0x6C00, 0x7800, 0xB401,
 0x5000, 0x9C01, 0x8801, 0x4400
};


uint16_t COMM_CRC_Calculate_Block(uint8_t *buf, uint8_t len){
 uint16_t crc = 0xFFFF, i;
 uint8_t  Data;
 for (i = 0; i < len; i++) {
  Data = *buf++;
  crc = CRCTalbe[(Data ^ crc) & 0x0f] ^ (crc >> 4);
  crc = CRCTalbe[((Data >> 4) ^ crc) & 0x0f] ^ (crc >> 4);
 }
 crc = ((crc & 0xFF) << 8) | ((crc >> 8) & 0xFF);
 return crc;
}

#endif

/*******************COMM CRC Functions End*****************/









/*************COMM RX Packet Functions Start***************/

void COMM_RX_Packet_CRC_Check(void){
	uint8_t  temp = 0;
  uint16_t crc_calc = 0, crc_recv = 0;
	COMM.RxPacket.CRCStatus = COMM_FALSE;
  if( COMM_Data_Len_Get() > 2){
		temp  = (uint8_t)COMM_Data_Len_Get();
		temp -= 2;
    crc_calc   =  COMM_CRC_Calculate_Block((uint8_t*)COMM.Buf, temp);
    crc_recv   =  COMM_Buf_Get(COMM_Data_Len_Get() - 2);
    crc_recv <<= 8;
    crc_recv  |= COMM_Buf_Get(COMM_Data_Len_Get() - 1);
		COMM.RxPacket.CalculatedCRC = crc_calc;
    COMM.RxPacket.ReceivedCRC = crc_recv;
    if( COMM.RxPacket.CalculatedCRC == COMM.RxPacket.ReceivedCRC ){
      COMM.RxPacket.CRCStatus = COMM_TRUE;
	  }
  }
}

void COMM_RX_Packet_Disassemble(void){
	Protocol_Disassemble_Packet();
}


void COMM_RX_Packet_Read_Complete(void){
  COMM_Buf_Flush();
  COMM_Data_Clear_Available_Flag();
  COMM_Data_Clear_Read_Complete_Flag();
  COMM_Error_Code_Clear();
}

/**************COMM RX Packet Functions End****************/









/*****************COMM Init Functions Start****************/

void COMM_Init(uint32_t baud){
  COMM_Struct_Init();
  COMM_RX_Packet_Struct_Init();
  COMM_Timer_Struct_Init();
  
  COMM_Config_GPIO();
  COMM_Config_Clock();
  COMM_Config_BAUD_Rate(baud);
  
  #ifdef COMM_ENABLE_TX  
  COMM_Config_Tx();
  #endif
  
  #ifdef COMM_ENABLE_RX
  COMM_Config_Rx();
  #endif
  
  #ifdef COMM_ENABLE_RX_INT
  COMM_Config_Rx_Interrupt();
  #endif
  
  COMM_Timer_Init();
	COMM_Timer_Value_Reset();
	COMM_Timer_Enable();
  COMM_Buf_Flush();
}

/******************COMM Init Functions End*****************/






