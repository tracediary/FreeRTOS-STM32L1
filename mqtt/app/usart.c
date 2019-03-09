#include "usart.h"
#include "string.h"
#include "stdio.h"	
#include "FreeRTOS.h"
	

/**************************************************
*Function: printf remap
*Description: 
*Input: ch, *f
*Return: int
**************************************************/
int fputc(int ch,FILE *f)
{
	USART_SendData(USART1,(unsigned char)ch);
		while(!(USART1->SR & USART_FLAG_TXE));

	return ch;
}



/**************************************************
*Function: init usart1
*Description: 
*Input: bound
*Return: NA
**************************************************/
void Usart_Init(unsigned int bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;

    

//使能串口接收
#if EN_USART1_RX			
	NVIC_InitTypeDef NVIC_InitStructure;
#endif
	
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
	GPIO_PinAFConfig(UASRT1_PORT,USART1_TX_PIN_SOUR,GPIO_AF_USART1);	//PA9-TX
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

#if EN_USART1_RX				//使能串口接收
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		//设置中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 5;	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//开启中断
#endif	
	
	/*Enable USART*/
	USART_Cmd(USART1,ENABLE);//使能串口
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
}


/**************************************************
*Function: usart1 send char
*Description: 
*Input: c
*Return: NA
**************************************************/
void USART1_Putc(uint8_t c)
{
	USART_SendData(USART1,c);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
}



/**************************************************
*Function: usart1 send string
*Description: 
*Input: *str
*Return: NA
**************************************************/
void USART1_Puts(uint8_t *str)
{
	while(*str)
	{
		USART_SendData(USART1,*str++);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
	}
}
/**************************************************
*Function: deinit usart1
*Description: 
*Input: NA
*Return: NA
**************************************************/
void Usart_Disable( void )
{	
	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);
	USART_Cmd(USART1,DISABLE);//失能串口
	
	UASRT1_RCC_DIS;
}


