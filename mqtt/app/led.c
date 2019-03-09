#include "led.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"
#include "stm32l1xx.h"




/**************************************************
*Function: init LED
*Description: PB3 port, init is low
*Input: NA
*Return: NA
**************************************************/
void LedInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	LED_RCC_EN;

	GPIO_InitStructure.GPIO_Pin = LED_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LED_PORT,&GPIO_InitStructure);
	
	GPIO_ResetBits(LED_PORT,LED_PIN);

	
}

/**************************************************
*Function: LED ON
*Description: NA
*Input: NA
*Return: NA
**************************************************/
void LedOn(void)
{
	GPIO_SetBits(LED_PORT,LED_PIN);
}


/**************************************************
*Function: LED OFF
*Description: NA
*Input: NA
*Return: NA
**************************************************/
void LedOff(void)
{
	GPIO_ResetBits(LED_PORT,LED_PIN);
}



/**************************************************
*Function: LED twinkle
*Description: NA
*Input: NA
*Return: NA
**************************************************/
void LedTwinkle(void)
{
	GPIO_ToggleBits(LED_PORT,LED_PIN);
}


/**************************************************
*Function: deinit LED
*Description: set PB3 mode is AN
*Input: NA
*Return: NA
**************************************************/
void LedDeInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = LED_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LED_PORT,&GPIO_InitStructure);
	
	GPIO_ResetBits(LED_PORT,LED_PIN);

	
}
