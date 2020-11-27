/*
 * uart.h
 *
 *  Created on: 2019��11��7��
 *      Author: Z.ChunHui CN(����ʵ����)
 */

#ifndef DRIVERLIBFORHAL_DRV_UART_H_
#define DRIVERLIBFORHAL_DRV_UART_H_
#include "../../DriverlibForC/RingBuffer/ringbuffer.h"
#include "board.h"
void UartInit(void);
void UartInterruptInit(P_INT_HANDLER A1InterruptHdl);
void uart1InterruptHandler1();
void UART1_EnableInterrupt();
int Uart1_Send(const char *pcFormat, ...);
void UART1_Message(const char *str);
extern ring_buffer uart1_t;
#endif /* DRIVERLIBFORHAL_DRV_UART_H_ */
