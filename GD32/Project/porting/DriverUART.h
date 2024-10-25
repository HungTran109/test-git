#ifndef __DRIVER_UART_H__
#define __DRIVER_UART_H__

//#include "stm32f0xx_usart.h"
#include "gd32e23x.h"
#include "gd32e23x_usart.h"

void UART_Init(void* USARTx, uint32_t BaudRate);
void UART_Putc(void* USARTx, uint8_t c);
void UART_Puts(void* USARTx, const char *str);
void UART_Putb(void* USARTx, uint8_t *Data, uint16_t Length);
void UART_Printf(void* USARTx, const char* str, ...);

#endif // __DRIVER_UART_H__

