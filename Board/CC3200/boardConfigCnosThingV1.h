#ifndef __BOARDCONFIGBLUESKYV3_H__
#define __BOARDCONFIGBLUESKYV3_H__

// Standard includes
#include <stdlib.h>

// simplelink includes
#include "simplelink.h"

// driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "interrupt.h"
#include "rom_map.h"
#include "prcm.h"
#include "uart.h"
#include "timer.h"

// common interface includes

#ifndef NOTERM
#include <uart_if.h>
#endif

#include <button_if.h>
#include <timer_if.h>
#include <common.h>
#include <utils.h>

/* 自己写的驱动库 */
#include <Drv_Led.h>
#endif
