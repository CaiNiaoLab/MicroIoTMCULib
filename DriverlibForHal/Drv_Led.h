/*
 * dyBaseLed.h
 *
 *  Created on: 2018年3月5日
 *      Author: Z.ChunHui CN(菜鸟实验室)
 */

#ifndef HARDWARE_DYBASELED_H_
#define HARDWARE_DYBASELED_H_

#include "hw_memmap.h"

static unsigned long gpioBasePort[]=
{
    GPIOA0_BASE,
    GPIOA1_BASE,
    GPIOA2_BASE,
    GPIOA3_BASE,
    GPIOA4_BASE
};
/* 对应的管脚编号，配置不同管脚做GPIO的时候 */
#define gpioGreLed  4
#define gpioRedLed  9
#define gpioOraLed  12

/* 使用枚举 对相关的变量进行申明 */
typedef enum
{
	ALL_LED = 0x7,
	/* 红灯的GPIO 的编号 D7/GP9/Pin64 */
	LED_RED = 0x1,
	/* 黄灯的GPIO的编号LED D6/GP10/Pin1 */
	LED_ORA = 0x2,
	/* 绿的的GPIO的编号 LED D5/GP11/Pin2 */
	LED_GREEN = 0x4

}ledEnumNum;

extern void GPIOInit(void);
extern void GPIO_Config(void);
extern void getGPIOPortAndPin(unsigned char pin , unsigned int *pinGpioPort ,unsigned char *pinGpioPin);
extern void setGPIOLedOn(unsigned char ucPins, unsigned int gpioLed);
extern void setGPIOLedOff(unsigned char ucPins, unsigned int gpioLed);
extern void ToggleLedState(ledEnumNum LedNum);
extern void TimerPeriodicIntHandler(void);
extern void LedTimerConfigNStart();
extern void LedTimerDeinitStop();
unsigned char GPIO_IF_LedStatus(unsigned char ucGPIONum);

#endif /* HARDWARE_DYBASELED_H_ */
