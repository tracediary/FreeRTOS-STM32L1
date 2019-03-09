/**
  ******************************************************************************
  * @file    stm32L1xx/user/main.c 
  * @author  tracediary
  * @version V4.0.0
  * @date    2017-06-24
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  */
/* Includes------------------------------*/
#include "main.h"

#include "FreeRTOS.h"
#include "Debug.h"
#include "task.h"
#include "semphr.h"
#include "MQTTPacket.h"


#include "timers.h"
#include "event_groups.h"
#include "led.h"
#include "iwdg.h"
#include "rtc.h"


/**task param start**/
#define MQTT_CON_EVENT			(1<<0)
#define MQTT_DISCON_EVENT		(1<<1)
#define MQTT_SUB_EVENT			(1<<2)

#define MQTT_HEARTBEAT_EVENT	(1<<3)

#define EVENT_ALL				(MQTT_CON_EVENT|MQTT_DISCON_EVENT|MQTT_SUB_EVENT|MQTT_HEARTBEAT_EVENT)


EventGroupHandle_t mqStatusHandler;
SemaphoreHandle_t SendBufMutexSemaphore;
TimerHandle_t 	HeartbeatTimer_Handle;

TaskHandle_t recv_task_Handle = NULL, conn_sub_ping_task_Handle = NULL, pub_task_Handle = NULL, dealRecv_task_Handle = NULL, IWDG_Handle = NULL;
/**task param end**/

/**task fun start**/
void recv_task(void *pvParameters);
void conn_sub_ping_task(void *pvParameters);
void pub_task(void *pvParameters);
void dealRecv_task(void *pvParameters);
void iwdg_task(void *pvParameters);

void HeartbeatCallback(TimerHandle_t xTimer);
/**task fun end**/


/**user fun start**/
int transport_getdata(unsigned char* buffer, int count);
void mqtt_connect(void);
void heartbeat(int* heartbeatTimes);
//void usart_dma_recvdata();

/**user fun end**/

/**user param start**/
extern uint16_t sys_count;

//串口接收双缓冲
Cache_S usartData={0,{'\0'}};

//MQTT处理双缓冲
Cache_S revMqttBuf = {0,{'\0'}};
unsigned char sendMqttBuf[MQTT_MAX_TX_BUF_SIZE] = {'\0'};

uint32_t read_len = 0;


/**user param end**/




/**************************************************
*Function: init sys
*Description: 
*Input: NA
*Return: NA
**************************************************/
void sys_init(void)
{
	SystemInit();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	Usart_Init(115200);
	LedInit();
	RTC_Config();
	IWDG_Init(IWDG_Prescaler_256,0x0FFF);
	IWDG_Feed();
}

/**************************************************
*Function: init freeRTOS task and param
*Description: NA
*Input: NA
*Return: NA
**************************************************/
void freertos_init(void)
{
	xTaskCreate(recv_task,"recv_task",(configMINIMAL_STACK_SIZE * 4) ,NULL,(tskIDLE_PRIORITY + 5),&recv_task_Handle);
	xTaskCreate(dealRecv_task,"dealRecv_task",(configMINIMAL_STACK_SIZE * 4) ,NULL,(tskIDLE_PRIORITY + 4),&dealRecv_task_Handle);
	
	xTaskCreate(conn_sub_ping_task,"conn_sub_ping_task",(configMINIMAL_STACK_SIZE * 2) ,NULL,(tskIDLE_PRIORITY + 3),&conn_sub_ping_task_Handle);
	
	xTaskCreate(pub_task,"pub_task",(configMINIMAL_STACK_SIZE * 2) ,NULL,(tskIDLE_PRIORITY + 2),&pub_task_Handle);
	xTaskCreate(iwdg_task,"iwdg",(configMINIMAL_STACK_SIZE * 2),NULL,tskIDLE_PRIORITY,&IWDG_Handle);
	SendBufMutexSemaphore=xSemaphoreCreateMutex();
	
	mqStatusHandler = xEventGroupCreate();
	
	xEventGroupClearBits(mqStatusHandler,EVENT_ALL);
	
	//心跳时间,45s
	HeartbeatTimer_Handle=xTimerCreate((const char*		)"HeartbeatTimer",
									    (TickType_t			)(45000),
							            (UBaseType_t		)pdTRUE,
							            (void*				)1,
							            (TimerCallbackFunction_t)HeartbeatCallback);
}


int main(void)
{
	sys_init();
	freertos_init();
	Debug("stm32L1xx init ok.");
	
	NetworkUsartInit(9600,usartData.rxBuffer[0]);
	vTaskStartScheduler();
	while(1)
	{
		;
	}
}



void recv_task(void *pvParameters)
{
	int task_priority = 0;
	char *taskname = NULL;
	int task_runtime = 1;
	
	int mqtt_status = 0;
	taskname = pcTaskGetName(recv_task_Handle);
	task_priority = uxTaskPriorityGet(recv_task_Handle);	
	
	while(1)
	{
		Debug("\n\rthis is %s runtime: %d. priority is %d",taskname,task_runtime,task_priority);
		task_runtime++;
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		IWDG_Feed();
		read_len = 0;
		mqtt_status = MQTTPacket_read(revMqttBuf.rxBuffer[revMqttBuf.index], MQTT_MAX_TX_BUF_SIZE, transport_getdata);
		revMqttBuf.index = !revMqttBuf.index;

		
		switch(mqtt_status)
		{
			case CONNECT:
				Debug("\n\rnot support!");
				break;
			case CONNACK:
				xTaskNotifyGive(conn_sub_ping_task_Handle);
				break;
			case PUBLISH:
				xTaskNotifyGive(dealRecv_task_Handle);
				break;
			case PUBACK:
				Debug("\n\rnot support!");
				break;
			case PUBREC:
				Debug("\n\rnot support!");
				break;
			case PUBREL:
				Debug("\n\rnot support!");
				break;
			case PUBCOMP:
				Debug("\n\rnot support!");
				break;
			case SUBSCRIBE:
				Debug("\n\rnot support!");
				break;
			case SUBACK:
				xTaskNotifyGive(conn_sub_ping_task_Handle);
				break;
			case UNSUBSCRIBE:
				Debug("\n\rnot support!");
				break;
			case UNSUBACK:
				Debug("\n\rnot support!");
				break;
			case PINGREQ:
				Debug("\n\rnot support!");
				break;
			case PINGRESP:
				xTaskNotifyGive(conn_sub_ping_task_Handle);
				break;
			case DISCONNECT:
				Debug("\n\rnot support!");
				break;
			
			default:
				Error("\n\rerror mqtt status! the status is %d",mqtt_status);
				break;
		}
		
		
		

	}
}



void conn_sub_ping_task(void *pvParameters)
{
	int task_priority = 0;
	char *taskname = NULL;
	int task_runtime = 1;
	int heartbeatTimes = 0;
	TaskStatus_t TaskStatus;
	task_priority = uxTaskPriorityGet(conn_sub_ping_task_Handle);
	taskname = pcTaskGetName(conn_sub_ping_task_Handle);	

	while(1)
	{		
		Debug("\n\rthis is %s runtime: %d. my priority is %d",taskname,task_runtime,task_priority);
		vTaskDelay( 3000 );
		IWDG_Feed();
		
		Info("\n\rstart to connect mqtt!");
		mqtt_connect();		
		
		while(1)
		{
			xEventGroupWaitBits((EventGroupHandle_t	)mqStatusHandler,
							   (EventBits_t			)MQTT_HEARTBEAT_EVENT,
							   (BaseType_t			)pdTRUE,				
							   (BaseType_t			)pdFALSE,
							   (TickType_t			)portMAX_DELAY);
			
			Debug("\n\rheartbeat start.");
			heartbeat(&heartbeatTimes);			
			if(heartbeatTimes > 2)
			{
				heartbeatTimes = 0;
				Error("\n\rheartbeat failed.");				
				break;
			}
		}
		
		xEventGroupClearBits(mqStatusHandler,MQTT_CON_EVENT);
		xEventGroupSetBits(mqStatusHandler,MQTT_DISCON_EVENT);
		xTimerStop(HeartbeatTimer_Handle,0);
		Info("\n\rreconnect mqtt!");

		task_runtime++;
	}
	
}

void pub_task(void *pvParameters)
{
	int task_priority = 0;
	char *taskname = NULL;
	int task_runtime = 1;
	
	MQTTString pubTopicStr = MQTTString_initializer;

	const char* payload = "tracediarypayload";
	int payloadlen = strlen(payload);
	int len;
	BaseType_t err=pdFALSE;
	task_priority = uxTaskPriorityGet(pub_task_Handle);
	taskname = pcTaskGetName(pub_task_Handle);
	pubTopicStr.cstring = "td/pub";
	while(1)
	{		
		Debug("\n\rthis is %s runtime: %d. priority is %d,",taskname,task_runtime,task_priority);
		task_runtime++;
pubFlow:		
		xEventGroupWaitBits((EventGroupHandle_t	)mqStatusHandler,
						   (EventBits_t			)MQTT_CON_EVENT,
						   (BaseType_t			)pdFALSE,				
						   (BaseType_t			)pdFALSE,
						   (TickType_t			)portMAX_DELAY);		

		vTaskDelay( sys_count+35000 );
		IWDG_Feed();
		
		err = xSemaphoreTake(SendBufMutexSemaphore,60000);
		if(err == pdFALSE)
		{
			Error("\n\rpub task get the send buf mutexsema failed");
			goto pubFlow;
		}
		IWDG_Feed();
		Clr_Buffer(sendMqttBuf, MQTT_MAX_TX_BUF_SIZE);
		len = MQTTSerialize_publish(sendMqttBuf, MQTT_MAX_TX_BUF_SIZE, 0, 0, 0, 0, pubTopicStr, (unsigned char*)payload, payloadlen);		
		transport_sendPacketBuffer((uint8_t *)&sendMqttBuf,len);		
		xSemaphoreGive(SendBufMutexSemaphore);
		Debug("\n\rpublish topic suc");
	}
	
}


void dealRecv_task(void *pvParameters)
{
	int task_priority = 0;
	char *taskname = NULL;
	int task_runtime = 1;
	
	int qos,payloadlen_in;
	unsigned char dup,retained;
	unsigned short msgid;
	unsigned char* payload_in;
	MQTTString receivedTopic;
	
	
	while(1)
	{
		task_priority = uxTaskPriorityGet(pub_task_Handle);
		taskname = pcTaskGetName(pub_task_Handle);
		Debug("\n\rthis is %s runtime: %d. my priority is %d,",taskname,task_runtime,task_priority);
		task_runtime++;
		
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		
		MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
					&payload_in, &payloadlen_in, revMqttBuf.rxBuffer[!revMqttBuf.index], USART_RX_LEN);
		
		Debug("\n\rmessage arrived %d: and load is: %s", payloadlen_in,payload_in);
		
	}
	
}


void iwdg_task(void *pvParameters)
{
	EventBits_t EventValue = 0;
	uint16_t acitive = 0;	
	RTC_TimeTypeDef rtc_set_wakeup_time;	
	
	rtc_set_wakeup_time.RTC_Minutes = 0;
	rtc_set_wakeup_time.RTC_Hours =0;
	rtc_set_wakeup_time.RTC_Seconds = 20;
	Debug("\n\rwkrtc sec:%d",rtc_set_wakeup_time.RTC_Seconds);
	RTC_Set_WKUPTime(rtc_set_wakeup_time);

	while(1)
	{		
		//Debug("\n\rfeed dog ");
		IWDG_Feed();
		//vTaskDelay(25000);
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		Debug("\n\riwdg task wait irq ");
	}
}


void heartbeat(int* heartbeatTimes)
{
	BaseType_t err=pdFALSE;
	EventBits_t EventValue;
	int NotifyValue = 0;
	int len = 0;
	
	IWDG_Feed();	
	err = xSemaphoreTake(SendBufMutexSemaphore,10000);
	if(err == pdFALSE)
	{
		Error("\n\rheartbeat apply send buf mutexsema failded!");
		(*heartbeatTimes)++;
		return;
	}
	Clr_Buffer(sendMqttBuf, MQTT_MAX_TX_BUF_SIZE);	
	len = MQTTSerialize_pingreq(sendMqttBuf, MQTT_MAX_TX_BUF_SIZE);
	transport_sendPacketBuffer((uint8_t *)&sendMqttBuf,len);
	xSemaphoreGive(SendBufMutexSemaphore);
	
	
	NotifyValue = ulTaskNotifyTake(pdTRUE,5000);
	if(!NotifyValue)
	{
		Error("\n\rheartbeat failed, retry time is %d",*heartbeatTimes);
		(*heartbeatTimes)++;
		return;
		
	}
	*heartbeatTimes = 0;
}
void mqtt_connect(void)
{
	int NotifyValue = 0;
	EventBits_t EventValue;
	BaseType_t err=pdFALSE;
	MQTTString subTopicStr = MQTTString_initializer;
	int retryTime = 0;
	
	unsigned short submsgid;
	unsigned char sessionPresent, connack_rc;	
	int ret = 0,len = 0,req_qos = 0,msgid = 1,loop = 1,granted_qos,subcount;	
	
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;	

	data.clientID.cstring = "TraceDiary";
	data.keepAliveInterval = 60;
	data.cleansession = 1;
	data.username.cstring = "admin";
	data.password.cstring = "password";
	
	/*************connect flow*************/

connect:	
	xSemaphoreTake(SendBufMutexSemaphore,portMAX_DELAY);
	IWDG_Feed();
	
	Clr_Buffer(sendMqttBuf, MQTT_MAX_TX_BUF_SIZE);
	len = MQTTSerialize_connect(sendMqttBuf, MQTT_MAX_TX_BUF_SIZE, &data);	
	Debug("\n\rthe conn serialize len is: %d",len);	
	transport_sendPacketBuffer((uint8_t *)&sendMqttBuf,len);
	
	xSemaphoreGive(SendBufMutexSemaphore);

	NotifyValue = ulTaskNotifyTake(pdTRUE,5000);
	IWDG_Feed();
	if(!NotifyValue)
	{
		Error("\n\rsend msg failed, retry!");
		goto connect;
	}

	if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, revMqttBuf.rxBuffer[!revMqttBuf.index], MQTT_MAX_TX_BUF_SIZE) != 1 || connack_rc != 0)
	{
		Error("\n\runable connect mqtt");
		goto connect;
	}
	Debug("\n\rconnect mqtt suc, set connect event, clear disconnect event");
	
	/*************set connect suc event*************/
	xEventGroupSetBits(mqStatusHandler,MQTT_CON_EVENT);
	xEventGroupClearBits(mqStatusHandler,MQTT_DISCON_EVENT);
	xTimerStart(HeartbeatTimer_Handle,0);

	
	/*************sub flow*************/
subscribe:
	IWDG_Feed();
	if(retryTime > 10)
	{
		Error("\n\rsubscrib failed, do nothing");
		return;
	}
	
	EventValue=xEventGroupGetBits(mqStatusHandler);
	if(EventValue & MQTT_CON_EVENT == 0x00)
	{
		Error("\n\rmq disconnect, try to connect!");
		goto connect;
	}	
	
	subTopicStr.cstring = "subs/topic";
	err = xSemaphoreTake(SendBufMutexSemaphore,60000);
	if(err == pdFALSE)
	{
		Error("\n\rapply mutexsema failed, retry!");
		retryTime++;
		goto subscribe;
	}
	Clr_Buffer(sendMqttBuf, MQTT_MAX_TX_BUF_SIZE);
	len = MQTTSerialize_subscribe(sendMqttBuf, MQTT_MAX_TX_BUF_SIZE, 0, msgid, 1, &subTopicStr, &req_qos);	
	Debug("\n\rthe sub serialize len is: %d",len);
	transport_sendPacketBuffer((uint8_t *)&sendMqttBuf,len);
	
	xSemaphoreGive(SendBufMutexSemaphore);

	NotifyValue = ulTaskNotifyTake(pdTRUE,60000);
	if(!NotifyValue)
	{
		Error("\n\rsub failed, retry!");
		retryTime++;
		goto subscribe;
	}
	MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, revMqttBuf.rxBuffer[!revMqttBuf.index], MQTT_MAX_TX_BUF_SIZE);
	if (granted_qos != 0)
	{
		Error("\n\rerror mqtt qos is %d",granted_qos);
		//goto subscribe;
	}
	else
	{
		Debug("\n\rsub mqtt ok");
		xEventGroupSetBits(mqStatusHandler,MQTT_SUB_EVENT);
	}
}



int transport_getdata(unsigned char* buffer, int count)
{
	memcpy(buffer,usartData.rxBuffer[!usartData.index]+read_len,count);
	read_len += count;
	return count;
}

void HeartbeatCallback(TimerHandle_t xTimer)
{
	xEventGroupSetBits(mqStatusHandler,MQTT_HEARTBEAT_EVENT);
}

void Clr_Buffer(uint8_t *str,int16_t len)
{
	while(len--)
	{
		*str++ = '\0';
	}
}

/**************************************************
*Function: do something before sleep
*Description: 
*Input: vParameters
*Return: NA
**************************************************/
void OS_PreSleepProcessing(uint32_t vParameters)
{
    (void)vParameters;
	//Debug("\n\rpre stop-------------- ");
	//LedDeInit();

	//IWDG_Feed();
    //PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
}


/**************************************************
*Function: do something after sleep
*Description: 
*Input: vParameters
*Return: NA
**************************************************/
void OS_PostSleepProcessing(uint32_t vParameters)
{
	SystemInit();
	RTC_Config();
	Usart_Init(115200);
	LedInit();
	//Debug("\n\rpost stop++++++++++++++");
}


