/*
 * APP_MQTT.c
 *
 *  Created on: 2019年10月10日
 *      Author: CaiNiaoLab.ZhaoSQ
 */
#include <Drv_uart.h>
#include "APP_MQTT.h"

/* 建立一个缓冲 */
#include <ringbuffer.h>
#include "../../NetWork/network_Smartconfig.h"
#include <json.h>

#if jsmn
#include "jsmn.h"
#endif

#include <string.h>

/* 增加设备驱动库 */
#include <DevManagement.h>
#include <Drv_Timer.h>
#include <tmp006drv.h>
#include "common.h"

/*Operate Lib in MQTT 3.1 mode.*/
#define MQTT_3_1_1              false/*MQTT 3.1.1 */
#define MQTT_3_1                true /*MQTT 3.1*/

#define WILL_TOPIC              "Client"
#define WILL_MSG                "Client Stopped"
#define WILL_QOS                QOS
#define WILL_RETAIN             false

/*Defining Publish Topic*/
#define PUB_TOPIC_FOR_SW3       "woozhao/cc3200/ButtonPressEvtSw3"
#define PUB_TOPIC_FOR_SW2       "woozhao/cc3200/ButtonPressEvtSw2"
#define PUB_TOPIC_FOR_UART1     "woozhao/cc3200/Uart1"
#define PUB_TOPIC_FOR_REGISTER  "/MncWP0jkui3SJIlifUFbYho4Olv0pioC/fNap4m6c3j0E2muLav9D7XhV2XH9LdI1/vertex/0"

/*Defining Number of topics*/
#define TOPIC_COUNT             3

/*Defining Subscription Topic Values*/
#define TOPIC1                  "woozhao/cc3200/ToggleLEDCmdL1"
#define TOPIC2                  "woozhao/cc3200/ToggleLEDCmdL2"
#define TOPIC3                  "woozhao/cc3200/ToggleLEDCmdL3"

/*Defining QOS levels*/
#define QOS0                    0
#define QOS1                    1
#define QOS2                    2

#define QOS                    QOS0
/*Defining Broker IP address and port Number*/
#if USE_IP_ADDRESS
#define SERVER_ADDRESS          "broker.hivemq.com"//"probielab.com"
#else
#define SERVER_ADDRESS          "112.74.165.209"
#endif

#define Rabbit_NUBMER           1885
#define MQTT_NUMBER             1883
#define PORT_NUMBER             MQTT_NUMBER

#define MAX_BROKER_CONN         1

#define SERVER_MODE             MQTT_3_1_1

/*Specifying Receive time out for the Receive task*/
#define RCV_TIMEOUT             60

/*Background receive task priority*/
#define TASK_PRIORITY           4

/* 心跳间隔时间(Keep Alive Timer value) */
#define KEEP_ALIVE_TIMER        500

/* Clean session flag  */
#define CLEAN_SESSION           true

/* (保留标志) Retain Flag. Used in publish message. */
#define RETAIN                  1

#define UART_PRINT              Report

#if UART1
unsigned char g_ucUARTBuffer[UART_IF_BUFFER];
bool is_init_done = false;
bool RegisterInit = false;
#endif

typedef struct connection_config{
    SlMqttClientCtxCfg_t broker_config;
    void *clt_ctx;
    unsigned char *client_id;
    unsigned char *usr_name;
    unsigned char *usr_pwd;
    bool is_clean;
    unsigned int keep_alive_time;
    SlMqttClientCbs_t CallBAcks;
    int num_topics;
    char *topic[TOPIC_COUNT];
    unsigned char qos[TOPIC_COUNT];
    SlMqttWill_t will_params;
    bool is_connected;
}connect_config;

/* （MQTT发布主题相关内容）Publishing topics and messages */
const char *pub_topic_sw2 = PUB_TOPIC_FOR_SW2;
const char *pub_topic_sw3 = PUB_TOPIC_FOR_SW3;
const char *pub_topic_uart1 = PUB_TOPIC_FOR_UART1;
const char *pub_topic_regidter = PUB_TOPIC_FOR_REGISTER;
#if TMP006
/* 4.将发布的主题进行赋值，使之通过mqtt发出 */
const char *pub_topic_tmp006 = PUB_TOPIC_FOR_TMP006;
#endif
unsigned char *data_sw2={"Push button sw2 "};
unsigned char *data_sw3={"Push button sw3 "};

#if RegisterInformation
unsigned char *data_Register= {"{\"uid\":\"MncWP0jkui3SJIlifUFbYho4Olv0pioC\",\"service\":{\"output\":[{\"serviceName\":\"temp\",\"serviveTopic\":\"\",\"valueFormat\":{\"temp\":\"float\"}}],\"input\":[{\"serviceName\":\"lightControl\",\"serviceTopic\":\"\",\"allowValue\":{\"open\":\"open lights\",\"close\":\"close lights\",\"number\":\"change light's volta\"}}]}}"};
#endif

#if TMP006
char data_tmp006[300] = {"TMP006 Data "};
#endif

/* AP Security Parameters */
SlSecParams_t SecurityParams = {0};

#if USE_SSL
char *security_file_list[] = {"/cert/cacert.der"};
#endif

/* connection configuration */
connect_config usr_connect_config[] =
{
    {
        {
#if USE_SSL
             {
                 SL_MQTT_NETCONN_URL,
                 SERVER_ADDRESS,
                 PORT_NUMBER,
                 SL_SO_SEC_METHOD_SSLv3_TLSV1_2,
                 SL_SEC_MASK_SECURE_DEFAULT,
                 1,
                 security_file_list
             },
#else
            {
                SL_MQTT_NETCONN_URL,
                SERVER_ADDRESS,
                PORT_NUMBER,
                0,
                0,
                0,
                NULL
            },
#endif
            SERVER_MODE,
            true,
        },
        NULL,
        ClientID,
        USE_NAME,
        USE_PWD,
        true,
        KEEP_ALIVE_TIMER,
        {Mqtt_Recv, sl_MqttEvt, sl_MqttDisconnect},
        TOPIC_COUNT,
        {TOPIC1, TOPIC2, TOPIC3},
        {QOS2, QOS2, QOS2},
        {WILL_TOPIC,WILL_MSG,WILL_QOS,WILL_RETAIN},
        false
    }
};
void *app_hndl = (void*)usr_connect_config;

/* library configuration */
SlMqttClientLibCfg_t Mqtt_Client={
    1883,
    TASK_PRIORITY,
    300,
    true,
    (long(*)(const char *, ...))UART_PRINT
};

#if jsmn
static int jsoneq(const char *json, jsmntok_t *tok, const char *s)
{
    if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
            strncmp(json + tok->start, s, tok->end - tok->start) == 0)
    {
        return 0;
    }
    return -1;
}
#endif

//****************************************************************************
//! Defines Mqtt_Pub_Message_Receive event handler.
//! Client App needs to register this event handler with sl_ExtLib_mqtt_Init
//! API. Background receive task invokes this handler whenever MQTT Client
//! receives a Publish Message from the broker.
//!
//!\param[out]     topstr => pointer to topic of the message
//!\param[out]     top_len => topic length
//!\param[out]     payload => pointer to payload
//!\param[out]     pay_len => payload length
//!\param[out]     retain => Tells whether its a Retained message or not
//!\param[out]     dup => Tells whether its a duplicate message or not
//!\param[out]     qos => Tells the Qos level
//!
//!\return none
//****************************************************************************
static void
Mqtt_Recv(void *app_hndl, const char  *topstr, long top_len, const void *payload,
                       long pay_len, bool dup,unsigned char qos, bool retain)
{

    unsigned char LEDSet = 0x00, gpioPin = 0;
    char *output_str=(char*)malloc(top_len+1);
    memset(output_str,'\0',top_len+1);
    strncpy(output_str, (char*)topstr, top_len);
    output_str[top_len]='\0';

    if(strncmp(topstr,TOPIC1, top_len) == 0)
    {
        LEDSet = LED_RED;
        gpioPin = gpioRedLed;
    }
    else if(strncmp(topstr,TOPIC2, top_len) == 0)
    {
        LEDSet = LED_ORA;
        gpioPin = gpioOraLed;
    }
    else if(strncmp(topstr,TOPIC3, top_len) == 0)
    {
        LEDSet = LED_GREEN;
        gpioPin = gpioGreLed;
    }

    UART_PRINT("\n\rPublish Message Received");
    UART_PRINT("\n\rTopic: ");
    UART_PRINT("%s",output_str);
    free(output_str);
    UART_PRINT(" [Qos: %d] \n\r",qos);
    if(retain)
    {
      UART_PRINT(" [Retained] \n\r");
    }
    if(dup)
    {
      UART_PRINT(" [Duplicate] \n\r");
    }

    output_str=(char*)malloc(pay_len+1);
    memset(output_str,'\0',pay_len+1);
    strncpy(output_str, (char*)payload, pay_len);
    output_str[pay_len]='\0';

    jsonParser parser;  //创建JSON解析器jsonParser，存储JSON中数据位置信息
    jsonObj object;

    /* 创建JSON解释器，初始化 */
    object = json_parser_init(&parser, (char*)output_str);
    if( parser.noOfToken < 0 )
    {
        UART_PRINT("\n\r Failed to parse JSON : %d \n\r",parser.noOfToken);
        json_parser_deinit(&parser);
        free(output_str);
        return;
    }
    /* Assume the top-level element is an object */
    if ( parser.noOfToken < 1 || object == JSON_INVALID_OBJECT)
    {
        UART_PRINT(" Object expected \n\r");
        json_parser_deinit(&parser);
        free(output_str);
        return ;
    }
    else
    {
        UART_PRINT("Json Data is:");
        UART_PRINT("%s",output_str);
        UART_PRINT("\n\r");
        char *rValue = (char*)malloc(pay_len+1);
        memset(rValue, NULL, pay_len+1);

        /* 运行JSON解释器，将一个JSON数据字符串转换为一个符号数组，每个数组描述一个JSON对象 */
        rValue = json_object_get_string(&parser, object, "identifier", rValue);
        UART_PRINT("-identifier: %s", rValue);
        UART_PRINT("\n\r");

        memset(rValue, NULL, pay_len+1);
        rValue = json_object_get_string(&parser, object, "name", rValue);
        UART_PRINT("-name: %s", rValue);
        UART_PRINT("\n\r");

        memset(rValue, NULL, pay_len+1);
        rValue = json_object_get_string(&parser, object, "desc", rValue);
        UART_PRINT("-desc: %s", rValue);
        UART_PRINT("\n\r");

        memset(rValue, NULL, pay_len+1);
        rValue = json_object_get_string(&parser, object, "type", rValue);
        UART_PRINT("-type: %s", rValue);
        UART_PRINT("\n\r");

        memset(rValue, NULL, pay_len+1);
        rValue = json_object_get_string(&parser, object, "data", rValue);
        UART_PRINT("-data: %s", rValue);
        UART_PRINT("\n\r");
        if(!strcmp(rValue, "true"))
        {
            setGPIOLedOn(LEDSet, gpioPin);
        }
        if(!strcmp(rValue, "false"))
        {
            setGPIOLedOff(LEDSet, gpioPin);
        }
        json_parser_deinit(&parser);
        free(rValue);
        free(output_str);
    }

    return;
}

//****************************************************************************
//! Defines sl_MqttEvt event handler.
//! Client App needs to register this event handler with sl_ExtLib_mqtt_Init
//! API. Background receive task invokes this handler whenever MQTT Client
//! receives an ack(whenever user is in non-blocking mode) or encounters an error.
//!
//! param[out]      evt => Event that invokes the handler. Event can be of the
//!                        following types:
//!                        MQTT_ACK - Ack Received
//!                        MQTT_ERROR - unknown error
//!
//!
//! \param[out]     buf => points to buffer
//! \param[out]     len => buffer length
//!
//! \return none
//****************************************************************************
static void
sl_MqttEvt(void *app_hndl, long evt, const void *buf,unsigned long len)
{
    int i;
    switch(evt)
    {
      case SL_MQTT_CL_EVT_PUBACK:
        UART_PRINT("PubAck:\n\r");
        UART_PRINT("%s\n\r",buf);
        break;

      case SL_MQTT_CL_EVT_SUBACK:
        UART_PRINT("\n\rGranted QoS Levels are:\n\r");

        for(i=0;i<len;i++)
        {
          UART_PRINT("QoS %d\n\r",((unsigned char*)buf)[i]);
        }
        break;

      case SL_MQTT_CL_EVT_UNSUBACK:
        UART_PRINT("UnSub Ack \n\r");
        UART_PRINT("%s\n\r",buf);
        break;

      default:
        break;

    }
}

//****************************************************************************
//
//! callback event in case of MQTT disconnection
//!
//! \param app_hndl is the handle for the disconnected connection
//!
//! return none
//
//****************************************************************************
static void
sl_MqttDisconnect(void *app_hndl)
{
    connect_config *local_con_conf;
    event_msg msg;
    local_con_conf = app_hndl;
    msg.hndl = app_hndl;
    msg.event = BROKER_DISCONNECTION;

    UART_PRINT("disconnect from broker %s\r\n",
           (local_con_conf->broker_config).server_info.server_addr);
    local_con_conf->is_connected = false;
    //
    // write message indicating publish message
    //
    osi_MsgQWrite(&g_PBQueue,&msg,OSI_NO_WAIT);

}

//*****************************************************************************
//
//! Task implementing MQTT client communication to other web client through
//!    a broker
//!
//! \param  none
//!
//! This function
//!    1. Initializes network driver and connects to the default AP
//!    2. Initializes the mqtt library and set up MQTT connection configurations
//!    3. set up the button events and their callbacks(for publishing)
//!    4. handles the callback signals
//!
//! \return None
//!
//*****************************************************************************
void MqttClient(void *pvParameters)
{

    long lRetVal = -1;
    int iCount = 0;
    int iNumBroker = 0;
    int iConnBroker = 0;
    int bokerflag = 5;
    event_msg RecvQue;
    connect_config *local_con_conf = (connect_config *)app_hndl;

    /* 配置LED */
    GPIO_Config();

    /* 关闭所有GPIO */
    setGPIOLedOff(ALL_LED,gpioGreLed|gpioOraLed|gpioRedLed);

    /* 初始化intertnetork 数据流 */
    InitializeAppVariables();

    lRetVal = ConfigureSimpleLinkToDefaultState();
    if(lRetVal < 0)
    {
        if (DEVICE_NOT_IN_STATION_MODE == lRetVal)
           UART_PRINT("Failed to configure the device in its "
                             "default state \n\r");

           LOOP_FOREVER();
    }

    UART_PRINT("Device is configured in default state \n\r");

    CLR_STATUS_BIT_ALL(g_ulStatus);

    /* Start simplelink */
    lRetVal = sl_Start(0,0,0);
    if (lRetVal < 0 || ROLE_STA != lRetVal)
    {
        UART_PRINT("Failed to start the device \n\r");
        LOOP_FOREVER();
    }

    UART_PRINT("Device started as STATION \n\r");

    /* Start Timer to blink Red LED till AP connection */
    LedTimerConfigNStart();

#if SmartConfig
    /* Connect to our AP using SmartConfig method */
    lRetVal = SmartConfigConnect();
    if(lRetVal < 0)
    {
      ERR_PRINT(lRetVal);
    }
#else
    SecurityParams.Key = (signed char *) SECURITY_KEY;
    SecurityParams.KeyLen = strlen(SECURITY_KEY);
    SecurityParams.Type = SECURITY_TYPE;


    lRetVal = Network_IF_ConnectAP(SSID_NAME, SecurityParams);
    if (lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }

#endif
    /* Disable the LED blinking Timer as Device is connected to AP */
    LedTimerDeinitStop();

    /* Switch ON RED LED to indicate that Device acquired an IP */
    setGPIOLedOn(LED_RED,gpioRedLed);

    UtilsDelay(20000000);

    setGPIOLedOff(LED_RED,gpioRedLed);

    /* 启动TMP006定时写入消息队列 */
    TMP006_TIMER_Init();

    /* Register Push Button Handlers */
    Button_IF_Init(pushButtonInterruptHandler2,pushButtonInterruptHandler3);

    /* Initialze MQTT client lib */
    lRetVal = sl_ExtLib_MqttClientInit(&Mqtt_Client);
    if(lRetVal != 0)
    {
        /* lib initialization failed */
        UART_PRINT("MQTT Client lib initialization failed\n\r");
        LOOP_FOREVER();
    }

    /* connection to the broker */
    iNumBroker = sizeof(usr_connect_config)/sizeof(connect_config);
    if(iNumBroker > MAX_BROKER_CONN)
    {
        UART_PRINT("Num of brokers are more then max num of brokers\n\r");
        LOOP_FOREVER();
    }

connect_to_broker:
    while(iCount < iNumBroker)
    {
        /* create client context */
        local_con_conf[iCount].clt_ctx =
        sl_ExtLib_MqttClientCtxCreate(&local_con_conf[iCount].broker_config,
                                      &local_con_conf[iCount].CallBAcks,
                                      &(local_con_conf[iCount]));

        /* Set Client ID */
        sl_ExtLib_MqttClientSet((void*)local_con_conf[iCount].clt_ctx,
                            SL_MQTT_PARAM_CLIENT_ID,
                            local_con_conf[iCount].client_id,
                            strlen((char*)(local_con_conf[iCount].client_id)));

        /* Set will Params */
        if(local_con_conf[iCount].will_params.will_topic != NULL)
        {
            sl_ExtLib_MqttClientSet((void*)local_con_conf[iCount].clt_ctx,
                                    SL_MQTT_PARAM_WILL_PARAM,
                                    &(local_con_conf[iCount].will_params),
                                    sizeof(SlMqttWill_t));
        }

        /* setting username and password */
        if(local_con_conf[iCount].usr_name != NULL)
        {
            sl_ExtLib_MqttClientSet((void*)local_con_conf[iCount].clt_ctx,
                                SL_MQTT_PARAM_USER_NAME,
                                local_con_conf[iCount].usr_name,
                                strlen((char*)local_con_conf[iCount].usr_name));

            if(local_con_conf[iCount].usr_pwd != NULL)
            {
                sl_ExtLib_MqttClientSet((void*)local_con_conf[iCount].clt_ctx,
                                SL_MQTT_PARAM_PASS_WORD,
                                local_con_conf[iCount].usr_pwd,
                                strlen((char*)local_con_conf[iCount].usr_pwd));
            }
        }

        /* connectin to the broker */
        if((sl_ExtLib_MqttClientConnect((void*)local_con_conf[iCount].clt_ctx,
                            local_con_conf[iCount].is_clean,
                            local_con_conf[iCount].keep_alive_time) & 0xFF) != 0)
        {
            UART_PRINT("\n\rBroker connect fail for conn no. %d \n\r",iCount+1);

            /* delete the context for this connection */
            sl_ExtLib_MqttClientCtxDelete(local_con_conf[iCount].clt_ctx);

            break;
        }
        else
        {
            UART_PRINT("\n\rSuccess: conn to Broker no. %d\n\r ", iCount+1);
            local_con_conf[iCount].is_connected = true;
            iConnBroker++;
        }

        /* Subscribe to topics */
        if(sl_ExtLib_MqttClientSub((void*)local_con_conf[iCount].clt_ctx,
                                   local_con_conf[iCount].topic,
                                   local_con_conf[iCount].qos, TOPIC_COUNT) < 0)
        {
            UART_PRINT("\n\r Subscription Error for conn no. %d\n\r", iCount+1);
            UART_PRINT("Disconnecting from the broker\r\n");

            sl_ExtLib_MqttClientDisconnect(local_con_conf[iCount].clt_ctx);
            local_con_conf[iCount].is_connected = false;

            /* delete the context for this connection */
            sl_ExtLib_MqttClientCtxDelete(local_con_conf[iCount].clt_ctx);
            iConnBroker--;
            break;
        }
        else
        {
            int iSub;
            UART_PRINT("Client subscribed on following topics:\n\r");
            for(iSub = 0; iSub < local_con_conf[iCount].num_topics; iSub++)
            {
                UART_PRINT("%s\n\r", local_con_conf[iCount].topic[iSub]);
            }
        }

        iCount++;
    }

    if((iConnBroker < 1) && (bokerflag < 0))
    {
        /* no succesful connection to broker */
        goto end;
    }

    iCount = 0;
#if RegisterInformation
    is_init_done = true;

    sl_ExtLib_MqttClientSend((void*)local_con_conf[iCount].clt_ctx,
            PUB_TOPIC_FOR_REGISTER,data_Register,strlen((char*)data_Register),QOS,RETAIN);
    UART_PRINT("\n\r CC3200 Publishes Register Information message \n\r");
    UART_PRINT("Topic: %s\n\r",PUB_TOPIC_FOR_REGISTER);
    UART_PRINT("Data: %s\n\r",data_Register);

#endif
    for(;;)
    {
        osi_MsgQRead( &g_PBQueue, &RecvQue, OSI_WAIT_FOREVER);

        if(PUSH_BUTTON_SW2_PRESSED == RecvQue.event)
        {
            Button_IF_EnableInterrupt(SW2);

            /* send publish message */
            sl_ExtLib_MqttClientSend((void*)local_con_conf[iCount].clt_ctx,
                    pub_topic_sw2,data_sw2,strlen((char*)data_sw2),QOS,RETAIN);
            UART_PRINT("\n\r CC3200 Publishes the following message \n\r");
            UART_PRINT("Topic: %s\n\r",pub_topic_sw2);
            UART_PRINT("Data: %s\n\r",data_sw2);
        }
        else if(PUSH_BUTTON_SW3_PRESSED == RecvQue.event)
        {
            Button_IF_EnableInterrupt(SW3);

            /* send publish message */
            sl_ExtLib_MqttClientSend((void*)local_con_conf[iCount].clt_ctx,
                    pub_topic_sw3,data_sw3,strlen((char*)data_sw3),QOS,RETAIN);
            UART_PRINT("\n\r CC3200 Publishes the following message \n\r");
            UART_PRINT("Topic: %s\n\r",pub_topic_sw3);
            UART_PRINT("Data: %s\n\r",data_sw3);
        }
#if UART1
        else if(UART1_RECV_EVENT == RecvQue.event)
        {
            sl_ExtLib_MqttClientSend((void *)local_con_conf[iCount].clt_ctx,
                                     pub_topic_uart1, g_ucUARTBuffer, UART_IF_BUFFER , QOS, RETAIN);
            memset(g_ucUARTBuffer, '\0', UART_IF_BUFFER);
        }
#endif
#if TMP006
        /* 3. 判断MQTT消息队列，增加判断处理程序 */
        else if(SEND_TMP006_EVENT == RecvQue.event)
        {
            Data_Put_string(data_tmp006);
            sl_ExtLib_MqttClientSend((void*)local_con_conf[iCount].clt_ctx,
                                     pub_topic_tmp006,data_tmp006,strlen((char*)data_tmp006),QOS,RETAIN);

            UART_PRINT("\n\r CC3200 Publishes the following message \n\r");
            UART_PRINT("Topic: %s\n\r",pub_topic_tmp006);
            UART_PRINT("Data: %s\n\r",data_tmp006);
        }
#endif
        else if(BROKER_DISCONNECTION == RecvQue.event)
        {
            iConnBroker--;
            /* Derive the value of the local_con_conf or clt_ctx from the message */
            sl_ExtLib_MqttClientCtxDelete(((connect_config*)(RecvQue.hndl))->clt_ctx);

            if(!IS_CONNECTED(g_ulStatus))
            {
                UART_PRINT("device has disconnected from AP \n\r");

                UART_PRINT("retry connection to the AP\n\r");

                while(!(IS_CONNECTED(g_ulStatus)) || !(IS_IP_ACQUIRED(g_ulStatus)))
                {
                    osi_Sleep(10);
                }
                goto connect_to_broker;

            }
            if(iConnBroker < 1 || bokerflag > 0)
            {
                UART_PRINT("\n\r bokerflag > 0 \n\r");
                bokerflag --;
                osi_Sleep(10);

                /* device not connected to any broker */
                goto connect_to_broker;

            }
            if ((iConnBroker < 1) && (bokerflag < 0))
            {
                UART_PRINT("\n\r bokerflag < 0 \n\r");
                goto end;
            }

        }
    }
end:

    /* Deinitializating the client library */
    sl_ExtLib_MqttClientExit();
    UART_PRINT("\n\r Exiting the Application\n\r");

    LOOP_FOREVER();
}

#if UART1
void UartRecvTask(void *pvParameters)
{
    unsigned char cChar;
    unsigned long bytes = 0;

    for(;;)
    {
        if(is_init_done)
        {
            while(MAP_UARTCharsAvail(UARTA1_BASE) == false)
            {
                osi_Sleep(1);
            }
            cChar = MAP_UARTCharGetNonBlocking(UARTA1_BASE);
            g_ucUARTBuffer[bytes++] = cChar;
            if(cChar == '\n')
            {
                bytes = 0;
                /* 增加延时，防止消息队列存满后，响应其他任务 */
                // osi_Sleep(60000);
                event_msg msg;
                msg.event = UART1_RECV_EVENT;
                msg.hndl = NULL;

                /* write message indicating exit from sending loop */
                osi_MsgQWrite(&g_PBQueue,&msg,OSI_NO_WAIT);

            }
        }
    }
}
#endif

//****************************************************************************
//
//! Push Button Handler1(GPIOS2). Press push button2 (GPIOSW2) Whenever user
//! wants to publish a message. Write message into message queue signaling the
//!    event publish messages
//!
//! \param none
//!
//! return none
//
//****************************************************************************
void pushButtonInterruptHandler2()
{
    event_msg msg;

    msg.event = PUSH_BUTTON_SW2_PRESSED;
    msg.hndl = NULL;

    /* write message indicating publish message */
    osi_MsgQWrite(&g_PBQueue,&msg,OSI_NO_WAIT);
}

//****************************************************************************
//
//! Push Button Handler3(GPIOS3). Press push button3 (GPIOSW3) Whenever user
//! wants to publish a message. Write message into message queue signaling the
//!    event publish messages
//!
//! \param none
//!
//! return none
//
//****************************************************************************
void pushButtonInterruptHandler3()
{
    event_msg msg;
    msg.event = PUSH_BUTTON_SW3_PRESSED;
    msg.hndl = NULL;

    /* write message indicating exit from sending loop */
    osi_MsgQWrite(&g_PBQueue,&msg,OSI_NO_WAIT);

}
