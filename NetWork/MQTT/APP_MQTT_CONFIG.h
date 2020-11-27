/*
 * APP_MQTT_CONFIG.h
 *
 *  Created on: 2019年10月15日
 *      Author: CaiNiaoLab.ZhaoSQ
 */

#ifndef NETWORK_MQTT_APP_MQTT_CONFIG_H_
#define NETWORK_MQTT_APP_MQTT_CONFIG_H_

#include <board.h>
#include "sl_mqtt_client.h"
#include "../network_if.h"

/* TODO:用户登录信息（老杜下一步增加，用户名和密码） */
#define ClientID                "WOOZhao-Dev1"
#if 0
#define USE_NAME                "jjolie"
#define USE_PWD                 "123456"
#else
#define USE_NAME                NULL
#define USE_PWD                 NULL
#endif

/* TODO:是否想使用连接rabbitmq的ssl端口的配置（还没开发完善） */
#define USE_SSL                 (0)

/* 0:使用IP 1:使用网址 */
#define USE_IP_ADDRESS          (1)
#define UART1                   (1)
#define UART1_IntHandle         (1)

#define MQTT_SEND_LEN           (UART_IF_BUFFER + 1)
/* 使用TMP006 进行温度采集，通过发布新主题方式 */
#define TMP006                  (1)
/* 0:启用直连模式(输入指定的WIFI名称和密码)  1: 启用SmartConfig功能*/
#define SmartConfig             (0)
#define RegisterInformation     (0)
/*
 * TODO:<如何使用，定义一个新的主题>
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
    /* 2. 在MQTT消息队列中，加入Test的消息，在调用的时候在将它写入FREERTOS消息队列中 */
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
