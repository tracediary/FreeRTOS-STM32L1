/**
  ******************************************************************************
  * @file    stm32L1xx/app/main.c 
  * @author  tracediary Application Team
  * @version V0.0.1
  * @date    2017-06-24
  * @brief   Main program
  ******************************************************************************
  * @attention
  ******************************************************************************
  */
                                
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
#include "stm32l1xx.h"


#define USART_TX_LEN						256
#define USART_RX_LEN						256

#define MQTT_MAX_TX_BUF_SIZE				256


typedef struct _CACHE_DATA
{	
	unsigned char index;	
	char rxBuffer[2][USART_RX_LEN];
	
}Cache_S;

void Clr_Buffer(uint8_t *str,int16_t len);
/* Includes ------------------------------------------------------------------*/

 

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT csf *****END OF FILE****/
