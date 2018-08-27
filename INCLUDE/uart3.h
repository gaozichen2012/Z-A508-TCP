#ifndef __UART_H
#define __UART_H

#include "stm8s_uart3.h"

#define countof(a)   (sizeof(a) / sizeof(*(a)))
#define RxBufferSize 64

typedef enum {					//UART string error code
	UART_RECEIVE	= 0x01,		//UART MCU receive data error code 
	UART_TIMEOVER	= 0x02,		//UART MCU run time over error code
	UART_PASSWORD	= 0x03,		//UART MCU password error code
	
	UART_START		= 0x05,		//UART MCU start ACK signal code
	UART_ACK		= 0x06,		//UART MCU ACK right code	
	UART_READWRITE	= 0x07,		//UART MCU read/write data error code
	UART_KEYERR		= 0x08,		//UART MCU key data error code
	UART_ADERR		= 0x09,		//UART MCU A/D error code
	UART_DAERR		= 0x0A,		//UART MCU D/A error code

	UART_FUNCTION	= 0xFE,		//UART MCU function error code 
	UART_FAILI		= 0xFF		//UART MCU failial code
}UART_ERROR;
extern bool WriteFreq_Flag;

extern void Uart3_Init(void);
extern void UART3_Interrupt(void);
extern void UART3_ToMcuMain(void);
extern void UART3_SendByte(u8 data);
extern void UART3_SendString(u8* Data,u16 len);
extern u8 UART3_ReceiveByte(void);
extern bool FILE_Write(u16 iAdr,u16 iLen,u8 *pBuf);//如果pBuf是字符串,则最后一位为\0
extern bool FILE_Read(u16 iAdr,u16 iLen,u8 *pBuf);
#endif