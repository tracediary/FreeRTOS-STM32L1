/**
  ******************************************************************************
  * @file    stm32L1xx/user/main.c 
  * @author  tracediary
  * @version V4.0.0
  * @date    2017-06-24
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  */
/* Includes------------------------------*/
#include "main.h"
#include "stm32l1xx.h"
#include "FreeRTOS.h"
#include "Debug.h"
#include "task.h"
#include "semphr.h"

#include "timers.h"
#include "event_groups.h"
#include "led.h"
#include "iwdg.h"
#include "rtc.h"


TaskHandle_t Task1_Handle = NULL, Task2_Handle = NULL, Task3_Handle = NULL;

void task1(void *pvParameters);
void task2(void *pvParameters);
void task3(void *pvParameters);

/**************************************************
*Function: init sys
*Description: 
*Input: NA
*Return: NA
**************************************************/
void sys_init(void)
{
	SystemInit();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	Usart_Init(115200);
	LedInit();
	RTC_Config();
}

/**************************************************
*Function: init freeRTOS task and param
*Description: NA
*Input: NA
*Return: NA
**************************************************/
void freertos_init(void)
{
	xTaskCreate(task1,"task1",(configMINIMAL_STACK_SIZE * 2) ,NULL,(tskIDLE_PRIORITY + 1),&Task1_Handle);
	xTaskCreate(task2,"task2",(configMINIMAL_STACK_SIZE * 2) ,NULL,(tskIDLE_PRIORITY + 5),&Task2_Handle);
	xTaskCreate(task3,"task3",(configMINIMAL_STACK_SIZE * 2) ,NULL,(tskIDLE_PRIORITY + 3),&Task3_Handle);
}


int main(void)
{
	sys_init();
	freertos_init();
	Debug("stm32L1xx init ok.");
	vTaskStartScheduler();
	while(1)
	{
		;
	}
}



void task1(void *pvParameters)
{
	while(1)
	{
		Debug("\n\rthis is task1.");
		vTaskDelay(2000);
	}
}



void task2(void *pvParameters)
{
	while(1)
	{
		Debug("\n\rthis is task2.");
		LedTwinkle();
		vTaskDelay(1500);
		
	}
	
}

void task3(void *pvParameters)
{
	while(1)
	{
		Debug("\n\rthis is task3.");
		LedTwinkle();
		vTaskDelay(1000);
	}
	
}

/**************************************************
*Function: do something before sleep
*Description: 
*Input: vParameters
*Return: NA
**************************************************/
void OS_PreSleepProcessing(uint32_t vParameters)
{
    (void)vParameters;
	Debug("\n\rpre stop-------------- ");
	LedDeInit();

	//IWDG_Feed();
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
}


/**************************************************
*Function: do something after sleep
*Description: 
*Input: vParameters
*Return: NA
**************************************************/
void OS_PostSleepProcessing(uint32_t vParameters)
{
	SystemInit();
	RTC_Config();
	Usart_Init(115200);
	LedInit();
	Debug("\n\rpost stop++++++++++++++");
}


