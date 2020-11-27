/*
 * uart.c
 *
 *  Created on: 2019年11月7日
 *      Author: Z.ChunHui CN(菜鸟实验室)
 */


#include <Drv_uart.h>
#include "pin.h"
#include "APP_MQTT.h"

void UartInit(void)
{

    MAP_PRCMPeripheralClkEnable(PRCM_UARTA0, PRCM_RUN_MODE_CLK);

    //
    // Configure PIN_55 for UART0 UART0_TX
    //
    MAP_PinTypeUART(PIN_55, PIN_MODE_3);

    //
    // Configure PIN_57 for UART0 UART0_RX
    //
    MAP_PinTypeUART(PIN_57, PIN_MODE_3);

    /* UART0串口配置 */
    MAP_UARTConfigSetExpClk(CONSOLE,MAP_PRCMPeripheralClockGet(CONSOLE_PERIPH),
                  UART_BAUD_RATE, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

#if UART1
    MAP_PRCMPeripheralClkEnable(PRCM_UARTA1, PRCM_RUN_MODE_CLK);
    //
    // Configure PIN_58 for UART1 UART1_TX
    //
    MAP_PinTypeUART(PIN_58, PIN_MODE_6);

    //
    // Configure PIN_45 for UART1 UART1_RX
    //
    MAP_PinTypeUART(PIN_45, PIN_MODE_2);

    /* UART1串口配置 */
    MAP_UARTConfigSetExpClk( UARTA1_BASE, MAP_PRCMPeripheralClockGet(PRCM_UARTA1),
                 UART_BAUD_RATE,(UART_CONFIG_WLEN_8 |UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

#endif
}


int Uart1_Send(const char *pcFormat, ...)
{
    int iRet = 0;

    char *pcBuff, *pcTemp;
    int iSize = 256;

    va_list list;
    pcBuff = (char*)malloc(iSize);
    if(pcBuff == NULL)
    {
        return -1;
    }
    while(1)
    {
        va_start(list,pcFormat);
        iRet = vsnprintf(pcBuff,iSize,pcFormat,list);
        va_end(list);
        if(iRet > -1 && iRet < iSize)
        {
         break;
        }
        else
        {
         iSize*=2;
         if((pcTemp=realloc(pcBuff,iSize))==NULL)
         {
             UART1_Message("Could not reallocate memory\n\r");
             iRet = -1;
             break;
         }
         else
         {
             pcBuff=pcTemp;
         }

        }
    }
    UART1_Message(pcBuff);
    free(pcBuff);

     return iRet;
}

void
UART1_Message(const char *str)
{
#ifndef NOTERM
    if(str != NULL)
    {
        while(*str!='\0')
        {
            MAP_UARTCharPut(UARTA1_BASE,*str++);
        }
    }
#endif
}
