/**
  ******************************************************************************
  * @file    stm32L1xx/app/Hardware_Version
  * @author  tracediary
  * @version V4.0.1
  * @date    2017-07-26
  * @brief   Hardware Version Control
  ******************************************************************************
  * @attention
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HARDWARE_VERSION_H
#define __HARDWARE_VERSION_H
#include "stm32l1xx_rcc.h"




/** 
  * @brief  LED definition
  */ 
#define LED_RCC_EN						RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE)
#define LED_RCC_DIS						RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,DISABLE)
#define LED_PORT						GPIOB
#define LED_PIN							GPIO_Pin_5


/** 
  * @brief  KEY definition
  */ 



/****************激活按键****************/
#define Active_PIN 				GPIO_Pin_15
#define Active_AHBPeriph 			RCC_AHBPeriph_GPIOA
#define Active_PORT 				GPIOA

#define Active_EXTI_PORT 			EXTI_PortSourceGPIOA
#define Active_EXTI_PinSource 	EXTI_PinSource15
#define Active_EXTI_Line			EXTI_Line15
#define Active_IRQChannel			EXTI15_10_IRQn

#define Active_IRQHandler			EXTI15_10_IRQHandler(void)






/***************USART1 **********/
//使能（1）/禁止（0）串口1接收
#define EN_USART1_RX 			0
#define USART1_REC_LEN			20

#define USART1_RX_PIN			GPIO_Pin_10
#define USART1_TX_PIN			GPIO_Pin_9

#define USART1_TX_PIN_SOUR		GPIO_PinSource9
#define USART1_RX_PIN_SOUR		GPIO_PinSource10

#define UASRT1_PORT_RCC_EN		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE)

#define UASRT1_PORT				GPIOA
#define UASRT1_RCC_EN			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE)
#define UASRT1_RCC_DIS			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,DISABLE)


#endif /* __HARDWARE_VERSION_H */






#define USART_RecIRQHandler				USART2_IRQHandler( void )
#define USART_TxIRQHandler				DMA1_Channel7_IRQHandler( void )

#define MQTT_USART_RX_PIN			GPIO_Pin_3
#define MQTT_USART_TX_PIN			GPIO_Pin_2

#define MQTT_USART_TX_PIN_SOUR		GPIO_PinSource2
#define MQTT_USART_RX_PIN_SOUR		GPIO_PinSource3

#define MQTT_USART_PORT_RCC_EN		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE)
#define MQTT_USART_PORT_RCC_DIS		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,DISABLE)
#define MQTT_USART_PORT				GPIOA
#define MQTT_USART_RCC_EN			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE)
#define MQTT_USART_RCC_DIS			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,DISABLE)

#define MQTT_USART					USART2
#define MQTT_USART_IRQn				USART2_IRQn
#define MQTT_AF_USART				GPIO_AF_USART2

#define MQTT_TX_DMA					DMA1_Channel7
#define MQTT_RX_DMA					DMA1_Channel6
#define MQTT_DMA_IRQn				DMA1_Channel7_IRQn







/************************ (C) COPYRIGHT  *****END OF FILE****/



