#ifndef __SYS_H
#define __SYS_H	
#include "stm32l1xx.h"

#include "Debug.h"



#define BOOTLOADER_SIZE		(8*1024)		/*8K bootloader*/


#define EEPROM_BASE_ADDR			0x08080000
#define BOOT_ADDR_COUNT				10

#define RESET_COUNT					50

#define BOOT_INFO_LEN				4


#define FIRMWARE_START_ADDR (FLASH_BASE + BOOTLOADER_SIZE)

#define RESET_APP			1

//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void);	//开启所有中断
void MSR_MSP(uint32_t addr);	//设置堆栈地址



#endif
