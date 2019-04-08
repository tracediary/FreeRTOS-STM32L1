#include "iwdg.h"

/**************************************************
*函数功能：看门狗初始化
*参数说明：无
*返 回 值：无
**************************************************/
void IWDG_Init(uint8_t prer,uint16_t rlr)
{
		IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);		
		IWDG_SetPrescaler(prer);	
		IWDG_SetReload(rlr);		
		IWDG_ReloadCounter();		
		IWDG_Enable();	
}

/**************************************************
*函数功能：喂狗
*参数说明：无
*返 回 值：无
**************************************************/
void IWDG_Feed(void)
{
	IWDG_ReloadCounter();
}


void IWDG_DeInit(uint8_t prer,uint16_t rlr)
{
	DBGMCU_Config(DBGMCU_IWDG_STOP, ENABLE);
}
