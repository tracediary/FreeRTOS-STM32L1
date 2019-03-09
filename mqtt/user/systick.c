/******************************************************************************
  * @file    stm32L1xx/app/sysytick.c 
  * @author  tracediary
  * @version V4.0.0
  * @date    2017-06-24
  * @brief   systick
  ******************************************************************************
  * @attention
  *
  */

#include "string.h"
#include "stdio.h"
#include "systick.h"
#include "usart.h"
#include "led.h"
#include "debug.h"
#include "FreeRTOS.h"
#include "task.h"

#define TMR_COUNT	6	
SOFT_TMR g_Tmr[TMR_COUNT];

void StartTimer(uint8_t _id, uint32_t _period);
uint8_t CheckTimer(uint8_t _id);
uint16_t sys_count = 0;

static void SoftTimerDec(SOFT_TMR *_tmr)
{
	if (_tmr->flag == 0)
	{
		if (_tmr->count > 0)
		{
			if ((--(_tmr->count) ) == 0)
			{
				_tmr->flag = 1;
			}
		}
	}
	
}


void vApplicationTickHook( void )
{
	uint8_t i;	

	for (i = 0; i < TMR_COUNT; i++)
	{
		SoftTimerDec(&g_Tmr[i]);
	}
	
	if(sys_count > 150000)
	{
		sys_count = 1;
	}
}




void StartTimer(uint8_t _id, uint32_t _period)
{
	if (_id >= TMR_COUNT)
	{
		return;
	}

	g_Tmr[_id].count = _period;
	g_Tmr[_id].flag = 0;
}


uint8_t CheckTimer(uint8_t _id)
{
	if (_id >= TMR_COUNT)
	{
		return 0;
	}

	if (g_Tmr[_id].flag == 1)
	{
		g_Tmr[_id].flag = 0;
		return 1;
	}
	return 0;
}






