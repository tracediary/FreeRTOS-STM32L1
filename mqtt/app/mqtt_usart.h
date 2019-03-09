#ifndef __MQTT_USART_H
#define __MQTT_USART_H
#include "stm32l1xx.h"



void NetworkUsartInit(uint32_t bound,uint32_t* revbuf);
uint32_t transport_sendPacketBuffer(uint8_t *buffer, int len);

#endif