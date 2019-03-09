#ifndef __SYSTICK_H
#define __SYSTICK_H
#include "stm32l1xx.h"


/* 定时器结构体 */
typedef struct
{
	volatile uint32_t count;	/* 计数器*/
	volatile uint8_t flag;		/* 定时达到标志 */
}SOFT_TMR;


/************定时器ID**********************/
#define NET_SYSTICK_ID								1

#define MOTO_SYSTICK_ID								(NET_SYSTICK_ID + 1)

#define SILT_WAITING_ID								(MOTO_SYSTICK_ID + 1)

#define ONLINE_WAITING_ID							(SILT_WAITING_ID + 1)

/************定时器ID**********************/
void StartNetTimer(uint32_t _period);
void StartWkModeTimer(uint32_t _period);

uint8_t CheckNetTimer(void);
uint8_t CheckWkModeTimer(void);

uint8_t CheckMotoTimer(void);
void StartMotoTimer(uint32_t _period);

uint8_t CheckOnlineTimer(void);
void StartOnlineTimer(uint32_t _period);

void Silt_Waiting_Delay(uint32_t _period);
uint8_t Check_Silt_Waiting_Timer(void);

void BT_Waiting_Delay(uint32_t _period);
uint8_t Check_BT_Waiting_Timer(void);

#endif
