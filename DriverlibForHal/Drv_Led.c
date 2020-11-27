/*
 *  dyBaseLed.c
 *
 *  Created on: 2018��3��5��
 *      Author: Z.ChunHui CN(����ʵ����)
 */
// �Լ�����������
#include <board.h>
#include <Drv_Led.h>
#include <stdio.h>

// TI�ٷ�����
#include "gpio.h"
#include "pin.h"

// LED�Ķ˿�
unsigned int redLedPort = 0,oraLedPort = 0,greLedPort = 0;
// LED�����ű��
unsigned char redLedPin,oraLedPin,greLedPin;
unsigned short g_usTimerInts;

void GPIOInit(void)
{

    /* Enable Peripheral Clocks */
    PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK);
    PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);
    PRCMPeripheralClkEnable(PRCM_GPIOA2, PRCM_RUN_MODE_CLK);

    /* Configure PIN_59 for GPIO Output */
    PinTypeGPIO(PIN_59, PIN_MODE_0, false);
    GPIODirModeSet(GPIOA0_BASE, 0x10, GPIO_DIR_MODE_OUT);

    /* RED LED ����GPIO 09 (PIN64)������ģʽ Ϊģʽ0�����رտ�©��� */
    PinTypeGPIO(PIN_64, PIN_MODE_0, false);
    GPIODirModeSet(GPIOA1_BASE, 0x2, GPIO_DIR_MODE_OUT);

    /* Configure PIN_03 for GPIO Output */
    PinTypeGPIO(PIN_03, PIN_MODE_0, false);
    GPIODirModeSet(GPIOA1_BASE, 0x10, GPIO_DIR_MODE_OUT);

    /* Configure PIN_04 for GPIO Input */
    PinTypeGPIO(PIN_04, PIN_MODE_0, false);
    GPIODirModeSet(GPIOA1_BASE, 0x20, GPIO_DIR_MODE_IN);

    /* Configure PIN_15 for GPIO Input */
    PinTypeGPIO(PIN_15, PIN_MODE_0, false);
    GPIODirModeSet(GPIOA2_BASE, 0x40, GPIO_DIR_MODE_IN);
}
/*
 * Description: �����ⲿPIN�ı�Ż�ȡ���ŵĻ���ַ�Ͷ˿�
 *
 * param pin �ⲿ���ű��
 * param pinGpioPort ���Ŷ˿�ֵ
 * param pinGpioPin �����ڲ����
 *
 * return ��
 */
void getGPIOPortAndPin(unsigned char pin, unsigned int *pinGpioPort, unsigned char *pinGpioPin)
{
	//���ⲿ���źŻ�ȡGPIO���ű��
	*pinGpioPin = 1 << (pin % 8);

	//���ⲿ���źŻ�ȡGPIO�˿ڱ��
	*pinGpioPort = (pin / 8);
	*pinGpioPort = gpioBasePort[*pinGpioPort];

}

/*
 * Description: GPIO����
 *
 * param None
 *
 * return ��
 *
 */
void GPIO_Config(void)
{
	getGPIOPortAndPin(gpioRedLed,&redLedPort,&redLedPin);
	getGPIOPortAndPin(gpioOraLed,&oraLedPort,&oraLedPin);
	getGPIOPortAndPin(gpioGreLed,&greLedPort,&greLedPin);

}

/*
 * Description: ����led GPIO������ߵ�ƽ
 *
 * param ucPins ѡ����Ƶĵ�
 * param gpioLed LED��PIN��Ӧ���
 *
 * return ��
 *
 */
void setGPIOLedOn(unsigned char ucPins, unsigned int gpioLed)
{
	 unsigned char ucGPIOValue = 1 << (gpioLed % 8);
	 if(ucPins & LED_GREEN)
	 {
		 GPIOPinWrite(greLedPort,greLedPin,ucGPIOValue);
	 }

	 if(ucPins & LED_ORA)
	 {
		 GPIOPinWrite(oraLedPort,oraLedPin,ucGPIOValue);
	 }


	 if(ucPins & LED_RED)
	 {
		 GPIOPinWrite(redLedPort,redLedPin,ucGPIOValue);
	 }

}

/*
 * Description: ����led GPIO�Ĺر�
 *
 * param ucPins ѡ����Ƶĵ�
 * param gpioLed LED��PIN��Ӧ���
 *
 * return ��
 *
 */
void setGPIOLedOff(unsigned char ucPins, unsigned int gpioLed)
{
	unsigned char ucGPIOValue = 0 << (gpioLed % 8);;
	 if(ucPins & LED_GREEN)
	 {
		 GPIOPinWrite(greLedPort,greLedPin,ucGPIOValue);
	 }

	 if(ucPins & LED_ORA)
	 {
		 GPIOPinWrite(oraLedPort,oraLedPin,ucGPIOValue);
	 }

	 if(ucPins & LED_RED)
	 {
		 GPIOPinWrite(redLedPort,redLedPin,ucGPIOValue);
	 }

}


//****************************************************************************
//
//! Set a value to the specified GPIO pin
//!
//! \param ucPin is the GPIO pin to be set (0:39)
//! \param uiGPIOPort is the GPIO port address
//! \param ucGPIOPin is the GPIO pin of the specified port
//!
//! This function
//!    1. Gets a value of the specified GPIO pin
//!
//! \return value of the GPIO pin
//
//****************************************************************************
unsigned char GPIO_IF_Get(unsigned char ucPin,
             unsigned int uiGPIOPort,
             unsigned char ucGPIOPin)
{
    unsigned char ucGPIOValue;
    long lGPIOStatus;

    //
    // Invoke the API to Get the value
    //
    lGPIOStatus =  MAP_GPIOPinRead(uiGPIOPort,ucGPIOPin);

    //
    // Set the corresponding bit in the bitmask
    //
    ucGPIOValue = lGPIOStatus >> (ucPin % 8);
    return ucGPIOValue;
}



unsigned char GPIO_IF_LedStatus(unsigned char ucGPIONum)
{
  unsigned char ucLEDStatus;
  switch(ucGPIONum)
  {
    case LED_GREEN:
    {
      ucLEDStatus = GPIO_IF_Get(ucGPIONum, greLedPort, greLedPin);
      break;
    }
    case LED_ORA:
    {
      ucLEDStatus = GPIO_IF_Get(ucGPIONum, oraLedPort,oraLedPin);
      break;
    }
    case LED_RED:
    {
      ucLEDStatus = GPIO_IF_Get(ucGPIONum,redLedPort,redLedPin);
      break;
    }
    default:
        ucLEDStatus = 0;
  }
  return ucLEDStatus;
}


//****************************************************************************
//
//!    Toggles the state of GPIOs(LEDs)
//!
//! \param LedNum is the enumeration for the GPIO to be toggled
//!
//!    \return none
//
//****************************************************************************
void ToggleLedState(ledEnumNum LedNum)
{
    unsigned char ledstate = 0;
    switch(LedNum)
    {
    case LED_RED:
        ledstate = GPIO_IF_LedStatus(LED_RED);
        if(!ledstate)
        {
            setGPIOLedOn(LED_RED,gpioRedLed);
        }
        else
        {
            setGPIOLedOff(LED_RED,gpioRedLed);
        }
        break;
    case LED_ORA:
        ledstate = GPIO_IF_LedStatus(LED_ORA);
        if(!ledstate)
        {
            setGPIOLedOn(LED_ORA,gpioOraLed);
        }
        else
        {
            setGPIOLedOff(LED_ORA,gpioOraLed);
        }
        break;
    case LED_GREEN:
        ledstate = GPIO_IF_LedStatus(LED_GREEN);
        if(!ledstate)
        {
            setGPIOLedOn(LED_GREEN,gpioGreLed);
        }
        else
        {
            setGPIOLedOff(LED_GREEN,gpioGreLed);
        }
        break;
    default:
        break;
    }
}


//*****************************************************************************
//
//! Periodic Timer Interrupt Handler
//!
//! \param None
//!
//! \return None
//
//*****************************************************************************
void
TimerPeriodicIntHandler(void)
{
    unsigned long ulInts;

    /* Clear all pending interrupts from the timer we are currently using. */
    ulInts = MAP_TimerIntStatus(TIMERA0_BASE, true);
    MAP_TimerIntClear(TIMERA0_BASE, ulInts);

    /* Increment our interrupt counter. */
    g_usTimerInts++;
    if(!(g_usTimerInts & 0x1))
    {
        /* Off Led */
        setGPIOLedOff(LED_RED,gpioRedLed);
    }
    else
    {
        /* On Led */
        setGPIOLedOn(LED_RED,gpioRedLed);
    }
}

//****************************************************************************
//
//! Function to configure and start timer to blink the LED while device is
//! trying to connect to an AP
//!
//! \param none
//!
//! return none
//
//****************************************************************************
void LedTimerConfigNStart()
{
    /* Configure Timer for blinking the LED for IP acquisition */
    Timer_IF_Init(PRCM_TIMERA0,TIMERA0_BASE,TIMER_CFG_PERIODIC,TIMER_A,0);
    Timer_IF_IntSetup(TIMERA0_BASE,TIMER_A,TimerPeriodicIntHandler);
    Timer_IF_Start(TIMERA0_BASE,TIMER_A,100);
}

//****************************************************************************
//
//! Disable the LED blinking Timer as Device is connected to AP
//!
//! \param none
//!
//! return none
//
//****************************************************************************
void LedTimerDeinitStop()
{
    /* Disable the LED blinking Timer as Device is connected to AP */
    Timer_IF_Stop(TIMERA0_BASE,TIMER_A);
    Timer_IF_DeInit(TIMERA0_BASE,TIMER_A);
}
