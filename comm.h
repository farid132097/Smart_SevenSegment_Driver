

/*
 * File:   comm.h
 * Author: MD. Faridul Islam
 * faridmdislam@gmail.com
 * LL Driver -> Communication Library (Based on UART)
 * Rev 3.2
 * Created on December 12, 2024, 9:44 PM
 */
 



#ifndef  _COMM_H_
#define  _COMM_H_

#include "stm32g030xx.h"

void     COMM_Struct_Init(void);
void     COMM_RX_Packet_Struct_Init(void);

void     COMM_Config_GPIO(void);
void     COMM_Config_Clock(void);
void     COMM_Config_BAUD_Rate(uint32_t baud_rate);
void     COMM_Config_Tx(void);
void     COMM_Config_Rx(void);
void     COMM_Config_Rx_Interrupt(void);
void     COMM_Clear_Interrupt_Flag(void);
void     COMM_Tx_Byte(uint8_t val);
uint8_t  COMM_Rx_Byte(void);
void     USART1_IRQHandler(void);

void     COMM_Timer_Struct_Init(void);
void     COMM_Timer_Init(void);
void     COMM_Timer_Enable(void);
void     COMM_Timer_Disable(void);
uint8_t  COMM_Timer_Get_Status(void);
uint16_t COMM_Timer_Get_Val(void);
void     COMM_Timer_Value_Reset(void);
void     COMM_Timer_Clear_Interrupt_Flag(void);
void     TIM16_IRQHandler(void);
void     COMM_Tx_Buf(volatile uint8_t *data, uint8_t len);


void     COMM_Tx_NL(void);
void     COMM_Tx_SP(void);
void     COMM_Tx_CM(void);


void     COMM_Tx_Text(char *str);
void     COMM_Tx_Text_NL(char *str);
void     COMM_Tx_Text_SP(char *str);
void     COMM_Tx_Text_CM(char *str);


void     COMM_Determine_Digit_Numbers(uint32_t num);
void     COMM_Tx_Number_Digits(void);
void     COMM_Tx_Number(int32_t num);

void     COMM_Tx_Number_Hex32_Raw(uint32_t val);
void     COMM_Tx_Number_Hex32(uint32_t val);
void     COMM_Tx_Number_Hex(uint64_t val);

void     COMM_Tx_Number_Bin32_Raw(uint32_t val);
void     COMM_Tx_Number_Bin32(uint32_t val);
void     COMM_Tx_Number_Bin(uint64_t val);


void     COMM_Tx_Number_NL(int32_t num);
void     COMM_Tx_Number_SP(int32_t num);
void     COMM_Tx_Number_CM(int32_t num);


void     COMM_Tx_Number_Hex_NL(uint64_t num);
void     COMM_Tx_Number_Hex_SP(uint64_t num);
void     COMM_Tx_Number_Hex_CM(uint64_t num);


void     COMM_Tx_Number_Bin_NL(uint64_t num);
void     COMM_Tx_Number_Bin_SP(uint64_t num);
void     COMM_Tx_Number_Bin_CM(uint64_t num);


void     COMM_Tx_Parameter_NL(char *name, int32_t num);
void     COMM_Tx_Parameter_SP(char *name, int32_t num);
void     COMM_Tx_Parameter_CM(char *name, int32_t num);


void     COMM_Tx_Parameter_Hex_NL(char *name, uint64_t num);
void     COMM_Tx_Parameter_Hex_SP(char *name, uint64_t num);
void     COMM_Tx_Parameter_Hex_CM(char *name, uint64_t num);


void     COMM_Tx_Parameter_Bin_NL(char *name, uint64_t num);
void     COMM_Tx_Parameter_Bin_SP(char *name, uint64_t num);
void     COMM_Tx_Parameter_Bin_CM(char *name, uint64_t num);

//Receiver Functions
void     COMM_Buf_Flush(void);
uint8_t  COMM_Buf_Get(uint16_t index);
uint16_t COMM_Buf_Get_Index(void);


//COMM Data Functions
uint8_t  COMM_Data_Available(void);
uint16_t COMM_Data_Len_Get(void);

uint16_t COMM_Data_Calculated_CRC_Get(void);
uint16_t COMM_Data_Received_CRC_Get(void);
uint8_t  COMM_Data_CRC_Status_Get(void);
uint8_t  COMM_Data_Read_Complete_Status(void);

void     COMM_Data_Clear_Available_Flag(void);
void     COMM_Data_Clear_Read_Complete_Flag(void);

void     COMM_Data_Copy_Buf(uint8_t *buf);
void     COMM_Data_Print_Buf(void);

uint8_t  COMM_Error_Code_Get(void);
void     COMM_Error_Code_Clear(void);

void     COMM_ISR_Handler(void);
void     COMM_Timer_ISR_Handler(void);


uint16_t COMM_CRC_Calculate_Byte(uint16_t crc, uint8_t data);
uint16_t COMM_CRC_Calculate_Block(volatile uint8_t *buf, uint8_t len);

void     COMM_RX_Packet_CRC_Check(void);
void     COMM_RX_Packet_Disassemble(void);
void     COMM_RX_Packet_Read_Complete(void);

void     COMM_Init(uint32_t baud);

#endif
