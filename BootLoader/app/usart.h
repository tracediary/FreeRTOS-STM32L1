#ifndef __USART_H
#define __USART_H
#include "sys.h" 
#include "string.h"
#include "stdio.h"



//使能（1）/禁止（0）串口1接收
#define EN_USART1_RX 			1

#define USART1_REC_LEN			20


#define USART1_RX_PIN			GPIO_Pin_10
#define USART1_TX_PIN			GPIO_Pin_9

#define USART1_TX_PIN_SOUR		GPIO_PinSource9
#define USART1_RX_PIN_SOUR		GPIO_PinSource10

#define UASRT1_PORT_RCC_EN		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE)
#define UASRT1_PORT_RCC_DIS		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,DISABLE)
#define UASRT1_PORT				GPIOA
#define UASRT1_RCC_EN			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE)

#define UASRT1_RCC_DIS			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,DISABLE)

/* =============================================================== */
#define UASRT2_PIN_CLOCK_EN		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE)
#define UASRT2_PIN_CLOCK_DIS		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,DISABLE)

#define UASRT2_CLOCK_EN			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE)
#define UASRT2_CLOCK_DIS			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,DISABLE)

#define UASRT2_PORT				GPIOA

#define USART2_RX_PIN				GPIO_Pin_3
#define USART2_TX_PIN				GPIO_Pin_2

#define USART2_TX_PIN_SOUR		GPIO_PinSource2
#define USART2_RX_PIN_SOUR		GPIO_PinSource3

/* ====================UART 3==================== */
#define UASRT3_PIN_CLOCK_EN		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE)
#define UASRT3_PIN_CLOCK_DIS		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,DISABLE)

#define UASRT3_CLOCK_EN			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE)
#define UASRT3_CLOCK_DIS			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,DISABLE)

#define UASRT3_PORT				GPIOB

#define USART3_TX_PIN				GPIO_Pin_10
#define USART3_RX_PIN				GPIO_Pin_11

#define USART3_TX_PIN_SOUR		GPIO_PinSource10
#define USART3_RX_PIN_SOUR		GPIO_PinSource11


/*=========UART1=========*/
void Usart_Init(unsigned int bound);
void USART1_Putc(uint8_t c);
void USART1_Puts(uint8_t *str);
void Usart_Disable( void );



#endif

