/*
 * DevManagement.h
 *
 *  Created on: 2019年10月12日
 *      Author: Z.ChunHui CN(菜鸟实验室)
 */
#ifndef _DEVMANAG_H
#define _DEVMANAG_H

#include "board.h"


enum
{
	SENSOR_OUTLINE = 0,
	SENSOR_ONLINE = 1,
	SENSOR_STATUS
};

typedef struct      
{
    const char *Mcu;
	unsigned short Status;
	float Data;
	unsigned short DeviceId;
	unsigned short ManufacID;
	long DataLen;
}Sensor;

typedef struct
{
    unsigned int pos; /* offset in the JSON string */
    unsigned int toknext; /* next token to allocate */
    int toksuper; /* superior token node, e.g parent object or array */
} data_encapsulation;

extern unsigned short itoa(short cNum, char *cString);
extern void Data_Init(void);
extern unsigned long Data_Put_string(char *buf);

#endif
