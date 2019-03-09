#include "FreeRTOS.h"
#include "task.h"
#include "systick.h"
#include "Debug.h"

#include "event_groups.h"
#include <stdbool.h>
#include "dma.h"
#include "main.h"
#include "mqtt_usart.h"


extern uint8_t sendbuf[USART_TX_LEN];




void NetworkUsartInit(uint32_t bound,uint32_t* revbuf)
{	
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	
	IWDG_Feed();

	/*Enable network Clock*/
	MQTT_USART_PORT_RCC_EN;
	/*Enable network usart Clock*/
	MQTT_USART_RCC_EN;
	
	USART_DeInit(MQTT_USART);			//将网络串口设为默认值
	
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(MQTT_USART,&USART_InitStructure);
	

	/*Connect PXx to USARTx_Tx*/
	GPIO_PinAFConfig(MQTT_USART_PORT,MQTT_USART_TX_PIN_SOUR,MQTT_AF_USART);
	GPIO_PinAFConfig(MQTT_USART_PORT,MQTT_USART_RX_PIN_SOUR,MQTT_AF_USART);
	
	GPIO_InitStructure.GPIO_Pin = MQTT_USART_RX_PIN | MQTT_USART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(MQTT_USART_PORT,&GPIO_InitStructure);//设置为输入
	
	
	
	NVIC_InitStructure.NVIC_IRQChannel = MQTT_USART_IRQn;		//设置中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1;	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = MQTT_DMA_IRQn;
	NVIC_Init(&NVIC_InitStructure);	

	USART_ClearFlag(MQTT_USART, USART_FLAG_TC);

	USART_Cmd(MQTT_USART,ENABLE);//使能串口	
	
	USART_DMACmd(MQTT_USART,USART_DMAReq_Rx|USART_DMAReq_Tx,ENABLE);
	
	USART_ITConfig(MQTT_USART,USART_IT_IDLE,ENABLE);
	USART_ClearFlag(MQTT_USART, USART_FLAG_TC);
	USART_ClearFlag(MQTT_USART, USART_FLAG_IDLE);
	
	
	USART_ClearITPendingBit(MQTT_USART,USART_IT_IDLE);
	USART_ClearITPendingBit(MQTT_USART,USART_IT_TC);
	
	DMA_NETUSART_Config(revbuf);
	Debug("\n\rnetwork usart init ok");
	
}

#if 1
extern Cache_S usartData;
void usart_dma_recvdata()
{
	uint32_t bleBasepri = 0;
	EventBits_t EventValue;
	BaseType_t xHigherPriorityTaskWoken ;	
	
	bleBasepri = taskENTER_CRITICAL_FROM_ISR();	
	
	USART_ReceiveData(MQTT_USART);

	DMA_Cmd(MQTT_RX_DMA,DISABLE);
	DMA_GetCurrDataCounter(MQTT_RX_DMA);
	
	MQTT_RX_DMA->CNDTR=USART_RX_LEN;
	
	usartData.index = !usartData.index;
	
	MQTT_RX_DMA->CMAR = (uint32_t)usartData.rxBuffer[usartData.index];
	
	
	Clr_Buffer(usartData.rxBuffer[usartData.index],USART_RX_LEN);
	
	DMA_Cmd(MQTT_RX_DMA,ENABLE);	
	
	taskEXIT_CRITICAL_FROM_ISR(bleBasepri);
	
}

#endif
extern TaskHandle_t recv_task_Handle ;
void USART_RecIRQHandler
{
	BaseType_t xHigherPriorityTaskWoken ;
	
    if(USART_GetITStatus(MQTT_USART,USART_IT_IDLE) != RESET)
    {
		USART_ClearITPendingBit(MQTT_USART,USART_IT_IDLE);
		USART_ClearFlag(MQTT_USART,USART_IT_IDLE);
		
		Debug("\n\rnet usart get recv irq");
		usart_dma_recvdata();
		vTaskNotifyGiveFromISR(recv_task_Handle,&xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
				
    }
	else if(USART_GetITStatus(MQTT_USART,USART_IT_TC) != RESET)
	{
		USART_ClearITPendingBit(MQTT_USART,USART_IT_TC);
		Debug("\n\rnet usart send over");
	}	
}


void USART_TxIRQHandler
{
	if(DMA_GetITStatus(DMA1_IT_TC7)==SET)
	{
		DMA_ClearFlag(DMA1_IT_TC7);
		DMA_ClearITPendingBit(DMA1_IT_TC7);
	}
}

uint32_t transport_sendPacketBuffer(uint8_t *buffer, int len)
{
	int32_t size = len;	
	int32_t lenth = size;	
	uint32_t loop = 0;
	if(!size)
	{
		return 0;
	}

	while(1)
	{
		if(lenth > (USART_TX_LEN - 1) )
		{
			while(DMA_GetCurrDataCounter(MQTT_TX_DMA));
			
			memcpy(sendbuf, (buffer + (USART_TX_LEN - 1) * loop),(USART_TX_LEN - 1));
			DMA_Cmd(MQTT_TX_DMA, DISABLE);
			MQTT_TX_DMA->CNDTR = USART_TX_LEN - 1;
			DMA_Cmd(MQTT_TX_DMA, ENABLE);
			loop++;
		}
		else
		{
			while(DMA_GetCurrDataCounter(MQTT_TX_DMA));
			
			memcpy(sendbuf, (buffer + (USART_TX_LEN - 1) * loop),lenth);			
			//sendbuf[lenth] = 0x0D;
			
			DMA_Cmd(MQTT_TX_DMA, DISABLE);
			MQTT_TX_DMA->CNDTR = lenth;
			DMA_Cmd(MQTT_TX_DMA, ENABLE);
			break;
			
		}
		//lenth = strlen(buffer + (USART_TX_LEN - 1) * loop);
		
		size = size - (USART_TX_LEN - 1) ;
		
		if(size >= 0 )
		{
			lenth =  size;
		}
		else
		{
			lenth = - size;
		}
	}

	return (size + 1);
}

