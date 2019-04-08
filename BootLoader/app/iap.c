/****************************************************
*本程序使用的BootLoader思路如下
*
*使用STM32L151RCT6，内存(RAM) 32k(0x20000000 start)，Flash 256k(0x08000000~0x0803FFFF)，eeprom 8k(0x08080000~0X08081FFF)
*
*
  --------------------------------------------------------------------------------------------------------------
 |     start     |     end        |      size     |                               
 |---------------|----------------|---------------|-------------------------------------------------------------
 |  0x08000000   |   0x08001FFF   |     8K		  |   bootloader 程序代码		  
 |---------------|----------------|---------------|-------------------------------------------------------------
 |  0x08002000   |   0x08002FFF   |     4K        |   系统参数，第一个参数确定使用哪个地址作为APP的起始地址
 |---------------|----------------|---------------|-------------------------------------------------------------
 |  0x08003000   |   0x08014FFF   |     72k       |   第一个程序空间  
 |--------------------------------------------------------------------------------------------------------------
 |  0x08015000   |   0x08015FFF   |     4k        |   冗余
 |--------------------------------------------------------------------------------------------------------------
 |  0x08016000   |   0x08027FFF   |     72k       |   第二个程序空间  
 |--------------------------------------------------------------------------------------------------------------
 |  0x08028000   |   0x08028FFF   |     4k        |   冗余
 |--------------------------------------------------------------------------------------------------------------
 |  0x08029000   |   0x0803AFFF   |     72k       |   第三个程序空间  
 |--------------------------------------------------------------------------------------------------------------
 |  0x0803B000   |   0x0803FFFF   |     20k       |  冗余
 |--------------------------------------------------------------------------------------------------------------

系统加载哪部分应用程序，由flash地位为0x08002000的值确定，该值(appoffset)只能为(1, 2, 3),如果不为上述值，默认使用1.
读取到该值后，在0x08002000的基础上，加上偏移量，读取到应用程序的地址，偏移量为offset = (appoffset * 8),每个值由4字节组成，中间空4字节为冗余
在EEPROM中，0x0808000A，存放了系统重启的失败次数，达到一定阈值，系统自动恢复出厂设置
(FOTA时，可能导致应用程序启动失败，导致看门狗重启系统，在进入应用程序后，需要立即将该值设置为1，表示已经进入应用程序成功)
在FTOA时，第一个程序不建议覆盖，后续的升级程序就在第二和第三个空间使用。如果程序过大，可以重新分配程序空间，需要自己保证FOTA代码校验成功
在应用程序中，系统启动成功后，需要将0x0808000A值设置为1，此BootLoader中，系统最大失败重启次数为50次。
可以将恢复出厂设置代码注释掉，修改sys.h中参数RESET_APP为0，即可
*
*
*
*
*/

#include "sys.h"
#include "stdio.h"
#include "iap.h"
#include "Debug.h"
#include "eeprom.h"

iapfun jump2app; 
uint16_t iapbuf[1024];


uint32_t bootInfo[BOOT_INFO_LEN] = {1,0x08003000,0x08016000,0x08029000};

//跳转到应用程序段
//appxaddr:用户代码起始地址.
void iap_load_app(uint32_t appxaddr)
{
	if(((*(uint32_t*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{
		jump2app=(iapfun)*(uint32_t*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(uint32_t*)appxaddr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		for(int i = 0; i < 8; i++)
		{
			NVIC->ICER[i] = 0xFFFFFFFF;	/* 关闭中断*/
			NVIC->ICPR[i] = 0xFFFFFFFF;	/* 清除中断标志位 */
		}
		
		Debug("\n\rget the right addr");
		jump2app();									//跳转到APP.
	}
}		 


void iap_load_addr(uint32_t appOffsetAddr)
{
	uint32_t offset = 1;
	uint32_t appxaddr = 0;
	uint8_t bootCount = 0;	
	
	if( ((*(uint32_t*)appOffsetAddr)&0x00000003)==0x00000000
		|| ((*(uint32_t*)appOffsetAddr)&0xFFFEFFFF)==0xFFFFFFFF)
	{
		offset = 1;
		Debug("\n\r the offset default");
	}
	else
	{
		offset = *(uint32_t*)appOffsetAddr;
		Debug("\n\rget the offset %d",offset);
	}
#if (RESET_APP == 1)	
	EEPROM_ReadBytes(BOOT_ADDR_COUNT,&bootCount, 1);
	
	Debug("\n\rreboot count %d",bootCount);
	
	if(bootCount != 0xFF)
	{
		bootCount++;
	}
	else
	{
		bootCount = 1;
	}
	
	EEPROM_WriteBytes(BOOT_ADDR_COUNT,&bootCount, 1);
	
	if(bootCount > RESET_COUNT)
	{
		if(offset != 1)
		{
			offset = 1;
			bootInfo[0] = 1;
			bootInfo[1] = STMFLASH_ReadWord(appOffsetAddr + 8);
			bootInfo[2] = STMFLASH_ReadWord(appOffsetAddr + 2*8);
			bootInfo[3] = STMFLASH_ReadWord(appOffsetAddr + 3*8);
			
			STMFLASH_WriteBootInfo(appOffsetAddr,bootInfo);
			
			bootCount = 1;
			
		}

		Debug("\n\rreboot too much, the sys is err, reset to factory setting");
	}
#endif	
	appxaddr = *(uint32_t*)(appOffsetAddr + offset * 8);
	Debug("\n\rthe app addr is: %d",appxaddr);
	
	
	iap_load_app(appxaddr);
	
	
}
