/*
 * APP_MQTT_CONFIG.h
 *
 *  Created on: 2019��10��15��
 *      Author: CaiNiaoLab.ZhaoSQ
 */

#ifndef NETWORK_MQTT_APP_MQTT_CONFIG_H_
#define NETWORK_MQTT_APP_MQTT_CONFIG_H_

#include <board.h>
#include "sl_mqtt_client.h"
#include "../network_if.h"

/* TODO:�û���¼��Ϣ���϶���һ�����ӣ��û��������룩 */
#define ClientID                "WOOZhao-Dev1"
#if 0
#define USE_NAME                "jjolie"
#define USE_PWD                 "123456"
#else
#define USE_NAME                NULL
#define USE_PWD                 NULL
#endif

/* TODO:�Ƿ���ʹ������rabbitmq��ssl�˿ڵ����ã���û�������ƣ� */
#define USE_SSL                 (0)

/* 0:ʹ��IP 1:ʹ����ַ */
#define USE_IP_ADDRESS          (1)
#define UART1                   (1)
#define UART1_IntHandle         (1)

#define MQTT_SEND_LEN           (UART_IF_BUFFER + 1)
/* ʹ��TMP006 �����¶Ȳɼ���ͨ�����������ⷽʽ */
#define TMP006                  (1)
/* 0:����ֱ��ģʽ(����ָ����WIFI���ƺ�����)  1: ����SmartConfig����*/
#define SmartConfig             (0)
#define RegisterInformation     (0)
/*
 * TODO:<���ʹ�ã�����һ���µ�����>
 * 1. Created PUB
 */
#if TMP006
#define PUB_TOPIC_FOR_TMP006      "woozhao/cc3200/TMP006"
#endif

typedef enum
{
    PUSH_BUTTON_SW2_PRESSED,
    PUSH_BUTTON_SW3_PRESSED,
    UART1_RECV_EVENT,
#if TMP006
    /* 2. ��MQTT��Ϣ�����У�����Test����Ϣ���ڵ��õ�ʱ���ڽ���д��FREERTOS��Ϣ������ */
    SEND_TMP006_EVENT,
#endif
    BROKER_DISCONNECTION
}events;

typedef struct
{
    void * hndl;
    events event;
}event_msg;

/* Message Queue */
OsiMsgQ_t g_PBQueue;
#endif /* NETWORK_MQTT_APP_MQTT_CONFIG_H_ */
