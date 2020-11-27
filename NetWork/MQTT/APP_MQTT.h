/*
 * APP_MQTT.h
 *
 *  Created on: 2019Äê10ÔÂ10ÈÕ
 *      Author: DELL
 */

#ifndef NETWORK_MQTT_APP_MQTT_H_
#define NETWORK_MQTT_APP_MQTT_H_

#include "APP_MQTT_CONFIG.h"

static void
Mqtt_Recv(void *app_hndl, const char  *topstr, long top_len, const void *payload,
          long pay_len, bool dup,unsigned char qos, bool retain);
static void sl_MqttEvt(void *app_hndl,long evt, const void *buf,
                       unsigned long len);
static void sl_MqttDisconnect(void *app_hndl);
extern void MqttClient(void *pvParameters);
extern void UartRecvTask(void *pvParameters);
extern void pushButtonInterruptHandler2();
extern void pushButtonInterruptHandler3();
extern int Uart1_Send(const char *pcFormat, ...);
extern void UARTA1IntHandler(void);
extern void Uart1IntHandler();
#endif /* NETWORK_MQTT_APP_MQTT_H_ */
