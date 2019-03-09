#include "rtc.h"
#include "string.h"
#include "stdio.h"
#include "debug.h"
#include "FreeRTOS.h"
#include <stdbool.h>
#include "task.h"
#include "semphr.h"



/**************************************************
*Function: set RTC init
*Description:
*Input: NA
*Return: NA
**************************************************/

void RTC_Config(void)
{
	__IO uint32_t AsynchPrediv = 0,SynchPrediv = 0;
	RTC_InitTypeDef RTC_InitStructure;
	RTC_TimeTypeDef rtc_set_wakeup_time;	

	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Allow access to RTC */

	PWR_RTCAccessCmd(ENABLE);


	if(RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x0A0A)
	{

#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
		/* The RTC Clock may varies due to LSI frequency dispersion. */
		//使能内部低速时钟源
		RCC_LSICmd(ENABLE);

		//等待RTC时钟源设置成功
		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
		{
		}

		//选择内部低速时钟作为RTC时钟源
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

		//设置RTC时钟分频系数为0x7fff (32768)  
		SynchPrediv = 0xFF;
		AsynchPrediv = 0x7F;

//选择外部低速时钟作为RTC时钟源
#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
		/* Enable the LSE OSC */
		RCC_LSEConfig(RCC_LSE_ON);

		/* Wait till LSE is ready */	
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
		{
		}

		/* Select the RTC Clock Source */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

		SynchPrediv = 0xFF;
		AsynchPrediv = 0x7F;

#endif /* RTC_CLOCK_SOURCE_LSI */

		//使能RTC时钟
		RCC_RTCCLKCmd(ENABLE);

		//等待RTC时钟与APB时钟同步
		RTC_WaitForSynchro();
		
		/* Configure the RTC data register and RTC prescaler */
		RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;
		RTC_InitStructure.RTC_SynchPrediv = SynchPrediv;
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;

		//设置RTC分频系数以及RTC计数格式
		RTC_Init(&RTC_InitStructure);
		//将开机上电标志置位(写入0x0A0A)
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x0A0A);
		RTC_WaitForSynchro();

	}
	else
	{
		/* Wait for RTC APB registers synchronisation */
		RTC_WaitForSynchro();

		/* Clear the RTC Alarm Flag */
		RTC_ClearFlag(RTC_FLAG_ALRAF); 
	}
}




/**************************************************
*Function: set RTC wkup for iwdg
*Description:
*Input: NA
*Return: NA
**************************************************/
void RTC_Set_WKUPTime( RTC_TimeTypeDef rtc_set_wakeuptime ){
	NVIC_InitTypeDef  NVIC_InitStructure;
	EXTI_InitTypeDef  EXTI_InitStructure;
	uint32_t 	rtc_set_wakeup_time_sec = 0;
	
	rtc_set_wakeup_time_sec += rtc_set_wakeuptime.RTC_Hours * HOUR_UNIT;
	rtc_set_wakeup_time_sec += rtc_set_wakeuptime.RTC_Minutes*MINUTE_UNIT;
	rtc_set_wakeup_time_sec += rtc_set_wakeuptime.RTC_Seconds;
	if( !IS_RTC_WAKEUP_COUNTER(rtc_set_wakeup_time_sec) ){
		Debug("\n\rRTC_Set_WKUP Error");
	}
	else
	{		

		EXTI_ClearITPendingBit(EXTI_Line20);
		EXTI_InitStructure.EXTI_Line = EXTI_Line20;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);


		NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 7;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		

		RTC_ClearITPendingBit(RTC_IT_WUT);
		RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);	//Clock = CK_SPRE(CK_SPRE=LSI/(RTC_AsynchPrediv+1)*(RTC_SynchPrediv+1))
		RTC_WakeUpCmd( DISABLE );
		RTC_SetWakeUpCounter( rtc_set_wakeup_time_sec-1 );
		RTC_ITConfig(RTC_IT_WUT,ENABLE);
		RTC_WakeUpCmd(ENABLE);
		
		Debug("\n\rRTC_WKUP iwdg");

	}
}

extern TaskHandle_t IWDG_Handle;
void RTC_WKUP_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken ;
	if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
	{
		RTC_ClearITPendingBit(RTC_IT_WUT);
		EXTI_ClearITPendingBit(EXTI_Line20);

		if(IWDG_Handle != NULL)
		{
			vTaskNotifyGiveFromISR(IWDG_Handle,&xHigherPriorityTaskWoken);
			
			Debug("\n\rRTC iwdg notify");
			portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
		}
		
	}
	
}


