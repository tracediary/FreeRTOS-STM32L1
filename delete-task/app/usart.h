#ifndef __USART_H
#define __USART_H
 
#include "stm32l1xx.h"
#include "string.h"
#include "stdio.h"
#include "Hardware_Version.h"





/*=========UART1=========*/
void Usart_Init(unsigned int bound);
void USART1_Putc(uint8_t c);
void USART1_Puts(uint8_t *str);
void Usart_Disable( void );




#endif

