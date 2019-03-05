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
	int task1_priority = 0;
	char *task1name = NULL;
	int task1_runtime = 1;
	while(1)
	{
		task1name = pcTaskGetName(Task1_Handle);
		task1_priority = uxTaskPriorityGet(Task1_Handle);
		Debug("\n\rthis is %s runtime: %d. my priority is %d",task1name,task1_runtime,task1_priority);
		task1_runtime++;
		vTaskDelay(2000);
	}
}



void task2(void *pvParameters)
{
	int task2_priority = 0;
	char *task2name = NULL;
	int task2_runtime = 1;
	int deleteTime = 0;
	uint8_t del = 0;
	
	while(1)
	{
		task2_priority = uxTaskPriorityGet(Task2_Handle);
		task2name = pcTaskGetName(Task2_Handle);
		Debug("\n\rthis is %s runtime: %d. my priority is %d",task2name,task2_runtime,task2_priority);
		LedTwinkle();
		vTaskDelay(1500);
		task2_runtime++;
		if(deleteTime > 3)
		{
			if(!del)
			{
				Debug("\n\rtask3 have been deleted");
				vTaskDelete(Task3_Handle);
				del = 1;
				
			}
		}
		else
		{
			deleteTime++;
		}
		
	}
	
}

void task3(void *pvParameters)
{
	int task3_priority = 0;
	char *task3name = NULL;
	int task3_runtime = 1;
	while(1)
	{
		task3_priority = uxTaskPriorityGet(Task3_Handle);
		task3name = pcTaskGetName(Task3_Handle);
		Debug("\n\rthis is %s runtime: %d. my priority is %d,",task3name,task3_runtime,task3_priority);
		task3_runtime++;

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


