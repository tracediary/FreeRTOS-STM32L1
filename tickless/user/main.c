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


TaskHandle_t Task1_Handle = NULL, Task2_Handle = NULL, Task3_Handle = NULL, IWDG_Handle = NULL;

void task1(void *pvParameters);
void task2(void *pvParameters);
void task3(void *pvParameters);
void iwdg_task(void *pvParameters);

void ActiveDevice_ExtiOpen(void);

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
	IWDG_Init(IWDG_Prescaler_256,0x0FFF);
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
	xTaskCreate(iwdg_task,"iwdg",(configMINIMAL_STACK_SIZE * 2),NULL,tskIDLE_PRIORITY,&IWDG_Handle);
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
	int NotifyValue;
	ActiveDevice_ExtiOpen();
	
	uint8_t  Before = 0;
	uint8_t  After = 0;
	
	
	while(1)
	{
		task1name = pcTaskGetName(Task1_Handle);
		task1_priority = uxTaskPriorityGet(Task1_Handle);
		Debug("\n\rthis is %s runtime: %d. my priority is %d",task1name,task1_runtime,task1_priority);
		task1_runtime++;
		
task1sleep:		
		NotifyValue=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		
		
		Before = 0;
		After = 0;
		
		vTaskDelay(100);
		
		Before = GPIO_ReadInputDataBit(ActiveDevice_PORT,ActiveDevice_PIN);
		vTaskDelay(1000);
		
		After = GPIO_ReadInputDataBit(ActiveDevice_PORT,ActiveDevice_PIN);
		Debug("\n\ractive get irq event: %d",NotifyValue);
		IWDG_Feed();
		if(After == 0 && Before == 0 )
		{
			;
		}
		else
		{
			goto task1sleep;
		}
	}
}







void iwdg_task(void *pvParameters)
{
	EventBits_t EventValue = 0;
	uint16_t acitive = 0;
	
	
	RTC_TimeTypeDef rtc_set_wakeup_time;	
	
	rtc_set_wakeup_time.RTC_Minutes = 0;
	rtc_set_wakeup_time.RTC_Hours =0;
	rtc_set_wakeup_time.RTC_Seconds = 20;
	Debug("\n\rwkrtc sec:%d",rtc_set_wakeup_time.RTC_Seconds);
	RTC_Set_WKUPTime(rtc_set_wakeup_time);
		
	while(1)
	{		
		Debug("\n\rfeed dog ");
		IWDG_Feed();
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		Debug("\n\riwdg task wait irq ");
	}
}


void ActiveDevice_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(ActiveDevice_AHBPeriph,ENABLE);

	GPIO_InitStructure.GPIO_Pin = ActiveDevice_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(ActiveDevice_PORT,&GPIO_InitStructure);
}

void ActiveDevice_ExtiOpen(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	ActiveDevice_Init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	SYSCFG_EXTILineConfig(ActiveDevice_EXTI_PORT,ActiveDevice_EXTI_PinSource);
	
	EXTI_InitStructure.EXTI_Line = ActiveDevice_EXTI_Line;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = ActiveDevice_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void ActiveDevice_ExtiClose(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,DISABLE);
	SYSCFG_EXTILineConfig(ActiveDevice_EXTI_PORT,ActiveDevice_EXTI_PinSource);
	
	EXTI_InitStructure.EXTI_Line 	= ActiveDevice_EXTI_Line;
	EXTI_InitStructure.EXTI_Mode 	= EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger 	= EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = ActiveDevice_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void ActiveDevice_IRQHandler
{
	BaseType_t xHigherPriorityTaskWoken ;
	
	if(EXTI_GetFlagStatus(ActiveDevice_EXTI_Line) == SET)
	{
		EXTI_ClearITPendingBit(ActiveDevice_EXTI_Line);
		if(Task1_Handle != NULL)
		{
			Debug("\n\rTask1_Handle IQR notify");
			vTaskNotifyGiveFromISR(Task1_Handle,&xHigherPriorityTaskWoken);
			
			portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
		}
		
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
	//do something to close ports and so on.

	IWDG_Feed();
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


