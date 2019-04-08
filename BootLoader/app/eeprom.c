#include "eeprom.h"
#include "string.h"
#include "stdio.h"
#include "stm32l1xx_flash.h"
#include "Debug.h"
/**************************************************/
//函数功能:		读取内部EEPROM N个字节
//形参说明:		地址，缓存，长度
//返回值  :		无
/**************************************************/
void EEPROM_ReadBytes(uint32_t Addr,uint8_t *Buffer,uint16_t Length)
{
	uint32_t *wAddr;
	
	wAddr = (uint32_t *)(EEPROM_BASE_ADDR+Addr);
	while(Length--)
	{
		*Buffer++ = *wAddr++;
	}
}


/**************************************************
函数功能:		写入N个字节到内部EEPROM
形参说明:		地址，缓存，长度
返回值  :		无
**************************************************/
void EEPROM_WriteBytes(uint32_t Addr,uint8_t *Buffer,uint16_t Length)
{
	uint32_t wAddr = EEPROM_BASE_ADDR+Addr;
	DIS_INT;
	DATA_EEPROM_Unlock();
	DATA_EEPROM_FixedTimeProgramCmd(ENABLE);
	
	FLASH_ClearFlag( FLASH_FLAG_EOP |  FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR|FLASH_FLAG_SIZERR|FLASH_FLAG_OPTVERR|FLASH_FLAG_OPTVERRUSR|FLASH_FLAG_RDERR );
	
	while(Length--)
	{
		DATA_EEPROM_ProgramByte(wAddr,*Buffer);
		Buffer++;
		wAddr++;

	}
	DATA_EEPROM_FixedTimeProgramCmd(DISABLE);
	DATA_EEPROM_Lock();
	EN_INT;
}



uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
	return *(uint32_t*)faddr; 
}


void STMFLASH_WriteBootInfo(uint32_t WriteAddr,uint32_t *pBuffer)
{ 			 		 
	uint32_t i;
	FLASH_Status flashStatus = 0;

	FLASH_Unlock();
	
	FLASH_ClearFlag( FLASH_FLAG_EOP |  FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR|FLASH_FLAG_SIZERR|FLASH_FLAG_OPTVERR|FLASH_FLAG_OPTVERRUSR|FLASH_FLAG_RDERR );
	flashStatus = FLASH_ErasePage(WriteAddr);
	
	Debug("\n\rerase flash data %d",flashStatus);	
	
	for(i=0;i<BOOT_INFO_LEN;i++)
	{
		Debug("\n\rwrite flash data %d, the i is %d, the addr is %d, the data is %d",flashStatus,i,WriteAddr,pBuffer[i]);
		flashStatus = FLASH_FastProgramWord(WriteAddr,pBuffer[i]);
		Debug("\n\r write the falsh is %d",flashStatus);
	    WriteAddr+=8;
		
	}
	FLASH_Lock();

}

