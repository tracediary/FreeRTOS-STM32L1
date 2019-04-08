/**
  ******************************************************************************
  * @file    bootloader/app/main.c 
  * @author  bootloader Application Team
  * @version V4.0.0
  * @date    2017-06-24
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  */
/* Includes------------------------------*/
#include "string.h"
#include "stdio.h"
#include "main.h"
#include "iap.h"
#include "sys.h"
#include "Debug.h"
#include "usart.h"


void sys_init(void);


int main(void)
{
	
	//系统初始化
	sys_init();	
	
	iap_load_addr(FIRMWARE_START_ADDR);
	
	Debug("\n\rboot error");
	
	while(1)
		;


}


/**************************************************
*函数功能：系统初始化
*参数说明：无
*返 回 值：无
**************************************************/
void sys_init(void)
{
	//系统初始化
	SystemInit();

	Usart_Init(115200);

}


