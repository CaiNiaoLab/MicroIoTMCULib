/*
 * Drv_IIC.c
 *
 *  Created on: 2019年11月21日
 *      Author: DELL
 */
// driverlib includes
#include "hw_types.h"
#include "pin.h"
#include "rom_map.h"
#include <i2c_if.h>
/* 自定义驱动库*/
#include <Drv_IIC.h>
#include <APP_MQTT.h>

void IIC_Init(void)
{
#if TMP006
    long lRetVal = -1;

    MAP_PRCMPeripheralClkEnable(PRCM_I2CA0, PRCM_RUN_MODE_CLK);
    //
    // Configure PIN_01 for I2C0 I2C_SCL
    //
    MAP_PinTypeI2C(PIN_01, PIN_MODE_1);

    //
    // Configure PIN_02 for I2C0 I2C_SDA
    //
    MAP_PinTypeI2C(PIN_02, PIN_MODE_1);


    lRetVal = I2C_IF_Open(I2C_MASTER_MODE_STD);
    if(lRetVal < 0)
    {
        ERR_PRINT(lRetVal);
        LOOP_FOREVER();
    }
#endif
}

