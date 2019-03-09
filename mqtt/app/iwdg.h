#ifndef __IWDG_H
#define __IWDG_H
#include "stm32l1xx.h"

void IWDG_Init(uint8_t prer,uint16_t rlr);
void IWDG_Feed(void);

#endif
