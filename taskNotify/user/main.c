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
	xTaskCreate(task2,"task2",(configMINIMAL_STACK_SIZE * 2) ,NULL,(tskIDLE_PRIORITY + 3),&Task2_Handle);
	xTaskCreate(task3,"task3",(configMINIMAL_STACK_SIZE * 2) ,NULL,(tskIDLE_PRIORITY + 5),&Task3_Handle);
	
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
		
		vTaskDelay(1000);
		
		if(task1_runtime %9 == 0)
		{
			Debug("\n\rtask1 notify the Task2_Handle.");
			xTaskNotifyGive(Task2_Handle);
		}
		if(task1_runtime % 2 == 0)
		{
			Debug("\n\rtask1 notify the Task3_Handle.");
			xTaskNotifyGive(Task3_Handle);
		}
		task1_runtime++;
		
	}
}



void task2(void *pvParameters)
{
	int task2_priority = 0;
	char *task2name = NULL;
	int task2_runtime = 1;
	BaseType_t err=pdFALSE;
	TaskStatus_t TaskStatus;
	
	EventBits_t EventValue;
	int NotifyValue;

	while(1)
	{
		task2_priority = uxTaskPriorityGet(Task2_Handle);
		task2name = pcTaskGetName(Task2_Handle);
		Debug("\n\rthis is %s runtime: %d. my priority is %d",task2name,task2_runtime,task2_priority);
		LedTwinkle();
		
		vTaskGetInfo((TaskHandle_t	)Task3_Handle, 		//任务句柄
				 (TaskStatus_t*	)&TaskStatus, 			//任务信息结构体
				 (BaseType_t	)pdTRUE,				//允许统计任务堆栈历史最小剩余大小
			     (eTaskState	)eInvalid);				//函数自己获取任务运行壮态
		
		Debug("\r\ntask name: %s",TaskStatus.pcTaskName);
		Debug("\r\ntask number: %d",(int)TaskStatus.xTaskNumber);
		Debug("\r\ntask current state: %d",TaskStatus.eCurrentState);
		Debug("\r\ntask current priority: %d",(int)TaskStatus.uxCurrentPriority);
		Debug("\r\ntask base priority: %d",(int)TaskStatus.uxBasePriority);
		Debug("\r\ntask stack base: %#x",(int)TaskStatus.pxStackBase);
		Debug("\r\ntask stack high water mark: %d",TaskStatus.usStackHighWaterMark);
		
		
		if(TaskStatus.eCurrentState == eSuspended)
		{
			Debug("\n\rresume task3");
			vTaskResume(Task3_Handle);			
		}
		Debug("\n\rtask2 wait the NotifyValue.");
		NotifyValue=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);


		Debug("\n\rtask2 get the NotifyValue: %d.",NotifyValue);
		task2_runtime++;
		
	}
	
}

void task3(void *pvParameters)
{
	int task3_priority = 0;
	char *task3name = NULL;
	int task3_runtime = 1;
	EventBits_t EventValue;
	int NotifyValue;
	while(1)
	{
		task3_priority = uxTaskPriorityGet(Task3_Handle);
		task3name = pcTaskGetName(Task3_Handle);
		Debug("\n\rthis is %s runtime: %d. my priority is %d,",task3name,task3_runtime,task3_priority);
		
		if(task3_runtime % 3 == 0)
		{
			Debug("\n\rsuspend task3");
			vTaskSuspend(Task3_Handle);
		}
		task3_runtime++;
		Debug("\n\rtask3 wait the NotifyValue.");
		NotifyValue=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);

		Debug("\n\rtask3 get the NotifyValue: %d.",NotifyValue);
				
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


