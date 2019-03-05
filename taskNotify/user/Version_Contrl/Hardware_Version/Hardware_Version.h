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
#define ActiveDevice_PIN 				GPIO_Pin_15
#define ActiveDevice_AHBPeriph 			RCC_AHBPeriph_GPIOA
#define ActiveDevice_PORT 				GPIOA

#define ActiveDevice_EXTI_PORT 			EXTI_PortSourceGPIOA
#define ActiveDevice_EXTI_PinSource 	EXTI_PinSource15
#define ActiveDevice_EXTI_Line			EXTI_Line15
#define ActiveDevice_IRQChannel			EXTI15_10_IRQn

#define ActiveDevice_IRQHandler			EXTI15_10_IRQHandler(void)






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

/************************ (C) COPYRIGHT  *****END OF FILE****/



