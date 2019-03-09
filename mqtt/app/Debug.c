#include "system.h"
#include <stdarg.h>
#include "Debug.h"
#include "util.h"
#if DEBUG_DMA
uint8_t print_buffer[LOG_BUF_LEN];

uint16_t USART1_SendBuffer(const uint8_t*  buffer, uint16_t length)
{

	uint32_t lenth = length;	
	uint32_t loop = 0;	
	int32_t positive = length;
	
	int32_t negative = 0;
	if(!length)
	{		
		return 0;
	}
	
	while(1)
	{
		if(lenth > (LOG_BUF_LEN ) )
		{
			while(DMA_GetCurrDataCounter(LOG_TX_DMA));			
			
			DMA_Cmd(LOG_TX_DMA, DISABLE);
			LOG_TX_DMA->CNDTR = LOG_BUF_LEN ;			
			LOG_TX_DMA->CMAR = (uint32_t)(buffer + (LOG_BUF_LEN ) * loop);
			
			DMA_Cmd(LOG_TX_DMA, ENABLE);
			loop++;
			
		}
		else
		{
			while(DMA_GetCurrDataCounter(LOG_TX_DMA));
			
			DMA_Cmd(LOG_TX_DMA, DISABLE);
			LOG_TX_DMA->CNDTR = lenth;
			
			LOG_TX_DMA->CMAR = (uint32_t)(buffer + (LOG_BUF_LEN ) * loop);			
			DMA_Cmd(LOG_TX_DMA, ENABLE);			
			break;
			
		}
		positive = length - (LOG_BUF_LEN * loop);		
		negative = 0 - positive;
		lenth = (positive > 0)? positive:negative;
	}	
	return (length);
}


/**
*Printf
* @param format	Printf
*/
void Debug_Printf(const char *format, ...)
{
	while(DMA_GetCurrDataCounter(LOG_TX_DMA));
	
	Clr_Buffer(print_buffer,LOG_BUF_LEN);
	
	uint32_t length;
	va_list args;
	va_start(args, format);
	length = vsnprintf((char*)print_buffer, sizeof(print_buffer), (char*)format, args);
	va_end(args);
	
	print_buffer[LOG_BUF_LEN - 1] = '\0';
	USART1_SendBuffer(print_buffer,length);

}


/**
* 16
* @param hex			
* @param hex_length	
*/
void Printf_Hex(const uint8_t* hex, uint16_t hex_length)
{
	const uint8_t char_table[] = "0123456789ABCDEF";
	uint16_t j=0;
	for(uint16_t i=0;(i<hex_length)&&j<sizeof(print_buffer);i++)
	{
		print_buffer[j++] = char_table[(hex[i]&0xF0)>>4];
		print_buffer[j++] = char_table[hex[i]&0x0F];
		print_buffer[j++] = ' ';
	}
	print_buffer[j++] = '\n';
	USART1_SendBuffer(print_buffer,j);
}

#endif


