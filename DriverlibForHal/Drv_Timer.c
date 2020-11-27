/*
 * Drv_Timer.c
 *
 *  Created on: 2019年11月18日
 *      Author: Z.ChunHui CN(菜鸟实验室)
 */
#include <APP_MQTT_CONFIG.h>
#include <Drv_Timer.h>
#include "timer_if.h"
/* 定义1s 启动定时中断 */
#define TIMER_Value 1000
static volatile unsigned long g_ulBase;
unsigned long g_ulSeconds = 0;

//*****************************************************************************
//
//! The interrupt handler for the first timer interrupt.
//!
//! \param  None
//!
//! \return none
//
//*****************************************************************************
void
TimerBaseIntHandler(void)
{

    /* Clear the timer interrupt. */
    Timer_IF_InterruptClear(g_ulBase);

    g_ulSeconds ++;

    if(g_ulSeconds == 60 )
    {
        g_ulSeconds = 0;
#if TMP006
        event_msg msg;
        msg.event = SEND_TMP006_EVENT;
        msg.hndl = NULL;

        /* write message indicating exit from sending loop */
        osi_MsgQWrite(&g_PBQueue,&msg,OSI_NO_WAIT);
#endif
    }
    TMP006_TIMER_Rest(TIMER_Value);
}

void TMP006_TIMER_Init(void)
{
    /* Base address for first timer */
    g_ulBase = TIMERA0_BASE;

    /* Configuring the timers */
    Timer_IF_Init(PRCM_TIMERA0, g_ulBase, TIMER_CFG_PERIODIC, TIMER_A, 0);

    /* Setup the interrupts for the timer timeouts. */
    Timer_IF_IntSetup(g_ulBase, TIMER_A, TimerBaseIntHandler);

    /* Turn on the timers feeding values in mSec */
    Timer_IF_Start(g_ulBase, TIMER_A, TIMER_Value);
}

void TMP006_TIMER_Rest(unsigned long ulValue)
{
    Timer_IF_Stop(g_ulBase,TIMERA0_BASE);

    Timer_IF_Start(g_ulBase, TIMER_A, ulValue);
}
