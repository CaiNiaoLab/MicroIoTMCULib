

#include "../../NetWork/network_Smartconfig.h"

/* App Includes */
#include <board.h>
#include <Drv_Led.h>
#include <Drv_IIC.h>
#include <Drv_uart.h>
#include <APP_MQTT.h>
#include <Drv_Timer.h>
#include <tmp006drv.h>
#include <ringbuffer.h>
#include <DevManagement.h>
#include "jsmn.h"

/*
 * OS includes
 * TODO: 头文件需要放在 <string.h>、<stdlib.h>后边，否则编译会报错。
 *
 * */
#include "osi.h"

/* json的c语言库进行封装 */
#define CJSON 0
#if CJSON
#include <json.h>
#endif

#define APPLICATION_VERSION 	"1.4.0"

void DisplayBanner(char * AppName);

static OsiTaskHandle MqttClient_Handle=NULL;              //任务句柄
#if UART1
static OsiTaskHandle UartRecvTask_Handle = NULL;
#endif

//*****************************************************************************
//
//! Application startup display on UART
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void
DisplayBanner(char * AppName)
{

    UART_PRINT("\n\n\n\r");
    UART_PRINT("\t\t *************************************************\n\r");
    UART_PRINT("\t\t    CC3200 %s Application       \n\r", AppName);
    UART_PRINT("\t\t *************************************************\n\r");
    UART_PRINT("\n\n\n\r");
}



//*****************************************************************************
//
//! Main 
//!
//! \param  none
//!
//! This function
//!    1. Invokes the SLHost task
//!    2. Invokes the MqttClient
//!    3. Incokes the Uart1 Recv task
//!
//! \return None
//!
//*****************************************************************************
void main()
{ 
    long lRetVal = -1;
#if CJSON
    struct json_tokener *tok;
    struct json_object *my_string, *my_int, *my_object, *my_array;
    struct json_object *new_obj;
#endif
    /* Initialize the board configurations */
    BoardInit();

    /* GPIO初始化 */
    GPIOInit();

    /* 初始化IIC外设 */
    IIC_Init();

    /* 初始化UART外设 */
    UartInit();

    /* Display Application Banner */
    DisplayBanner("MQTT_Client");

#if TMP006

    /* Init Temprature Sensor */
    lRetVal = TMP006DrvOpen();
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }

#endif

    /* 初始化数据信息 */
    Data_Init();

    /* Start the SimpleLink Host */
    lRetVal = VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY);
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }

    /* Start the MQTT Client task */
    osi_MsgQCreate(&g_PBQueue,"PBQueue",sizeof(event_msg),10);
    lRetVal = osi_TaskCreate(MqttClient,(const signed char *)"Mqtt Client App",2048, NULL, 3, (OsiTaskHandle*)&MqttClient_Handle );
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }

#if UART1

    /* 创建一个UART1接收的任务 */
    lRetVal = osi_TaskCreate((P_OSI_TASK_ENTRY)UartRecvTask, (const signed char *)"Uart1 Recv APP", 512, NULL, 1, (OsiTaskHandle*)&UartRecvTask_Handle);
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }
#endif

    /* Start the task scheduler */
    osi_start();
}

