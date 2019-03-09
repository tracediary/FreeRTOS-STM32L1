#include "dma.h"
#include "Debug.h"
#include "mqtt_usart.h"
#include "main.h"

/******************
´®¿Ú2dma³õÊ¼»¯
********************/

uint8_t sendbuf[USART_TX_LEN];
extern Cache_S usartData;

void DMA_NETUSART_Config(uint32_t* revbuf)
{
	DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	/****************Channel7 config****************/

    DMA_DeInit(MQTT_TX_DMA);  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)sendbuf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = USART_TX_LEN;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(MQTT_TX_DMA, &DMA_InitStructure);
	USART_DMACmd(MQTT_USART,USART_DMAReq_Tx,ENABLE);
	
	DMA_ITConfig(MQTT_TX_DMA, DMA_IT_TC,ENABLE);
	DMA_Cmd(MQTT_TX_DMA, ENABLE);
	DMA_ClearITPendingBit(DMA1_IT_TC7);
	/****************Channel7 config end****************/
	
	/****************Channel6 config****************/

    DMA_Cmd(MQTT_RX_DMA, DISABLE);
    DMA_DeInit(MQTT_RX_DMA);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)usartData.rxBuffer[0];
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                  
    DMA_InitStructure.DMA_BufferSize = USART_RX_LEN;                     
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;       
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;        
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;               
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(MQTT_RX_DMA, &DMA_InitStructure);
	USART_DMACmd(MQTT_USART,USART_DMAReq_Rx,ENABLE);
	DMA_Cmd(MQTT_RX_DMA, ENABLE);
	/****************Channel6 config end****************/
}


#if 0
extern uint8_t BT_Transfer_Buf[BLE_USART_RX_LEN];
void DMA_BLEUSART_Config()
{
	DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	/****************Channel2 config****************/
	DMA_Cmd(BLE_TX_DMA, DISABLE);
	

    DMA_DeInit(BLE_TX_DMA);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&BT_UART->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)bleSendbuf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = BLE_USART_TX_LEN;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(BLE_TX_DMA, &DMA_InitStructure);
	USART_DMACmd(BT_UART,USART_DMAReq_Tx,ENABLE);
	
	DMA_ITConfig(BLE_TX_DMA, DMA_IT_TC,ENABLE);
	DMA_Cmd(BLE_TX_DMA, ENABLE);
	DMA_ClearITPendingBit(DMA1_IT_TC2);
	/****************Channel2 config end****************/

	/****************Channel3 config****************/

    DMA_Cmd(BLE_RX_DMA, DISABLE);
    DMA_DeInit(BLE_RX_DMA);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART3->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(BT_Transfer_Buf);        
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = BLE_USART_RX_LEN;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(BLE_RX_DMA, &DMA_InitStructure);

	DMA_Cmd(BLE_RX_DMA, ENABLE);
	/****************Channel3 config end****************/
}

void DMA_BLEUSART_Deinit()
{
	DMA_Cmd(BLE_RX_DMA, DISABLE);
	DMA_Cmd(BLE_TX_DMA, DISABLE);
	
	DMA_DeInit(BLE_TX_DMA);
	DMA_DeInit(BLE_RX_DMA);
}

#endif

