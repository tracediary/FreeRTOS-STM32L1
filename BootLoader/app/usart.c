#include "usart.h"
#include "string.h"
#include "stdio.h"	
	
	

/**************************************************
*函数功能：printf函数重映射
*函数形参：输出字符，输出流
*返回值：	无
**************************************************/

int fputc(int ch,FILE *f)
{
	USART_SendData(USART1,(unsigned char)ch);
		while(!(USART1->SR & USART_FLAG_TXE));

	return ch;
}


/**************************************************
*函数功能：串口1初始化
*参数说明：波特率
*返 回 值：无
**************************************************/
void Usart_Init(unsigned int bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	
	UASRT1_PORT_RCC_EN;
	UASRT1_RCC_EN;

//将串口1设为默认值
	USART_DeInit(USART1);			
	
	/*USART1 Init*/
	USART_InitStructure.USART_BaudRate = bound;	
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式

	/*Connect PXx to USARTx_Tx*/
	GPIO_PinAFConfig(UASRT1_PORT,USART1_TX_PIN_SOUR,GPIO_AF_USART1);		//PA9-TX
	GPIO_PinAFConfig(UASRT1_PORT,USART1_RX_PIN_SOUR,GPIO_AF_USART1);	//PA10-RX
	
	/*Configure USART Tx as alternate function pull-pull*/
	GPIO_InitStructure.GPIO_Pin = USART1_TX_PIN |USART1_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(UASRT1_PORT,&GPIO_InitStructure);
	
	
	/*USART Configuration*/
	USART_Init(USART1,&USART_InitStructure);


	/*Enable USART*/
	USART_Cmd(USART1,ENABLE);//使能串口
	//USART_ClearFlag(USART1, USART_FLAG_TC);
}

/**************************************************
*函数功能：串口1发送字符
*参数说明：字符
*返 回 值：无
**************************************************/
void USART1_Putc(uint8_t c)
{
	USART_SendData(USART1,c);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
}

/**************************************************
函数功能：串口1发送字符串
参数说明：字符串
返 回 值：无
**************************************************/
void USART1_Puts(uint8_t *str)
{
	while(*str)
	{
		USART_SendData(USART1,*str++);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
	}
	

}


