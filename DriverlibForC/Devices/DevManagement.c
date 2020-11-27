/*
 * DevManagement.c
 *
 *  Created on: 2019��10��12��
 *      Author: Z.ChunHui CN(����ʵ����)
 */
#include <stdio.h>
#include <string.h>
#include <DevManagement.h>
#include <tmp006drv.h>

/* key�� ÿ���û�ֻ��һ����Ϊ���ʶ�𣬿��Զ�Ӧ���ҽ����  */
#define API_KEY         ""

#define GatewayDev_For_CC3200       "CC3200"

/* ���ﶨ����Ķ�Ӧ�豸 */
Sensor TMP006;
float Temp;
char tempch[12];
const char     pcDigits[] = "0123456789"; /* variable used by itoa function */

/*
************************************************************
*   �������ƣ�   Data_Init
*
*   �������ܣ�   Device��ʼ��
*
*   ��ڲ�����   ��
*
*   ���ز�����   ��
*
*   ˵����     ��ʼ���豸���
************************************************************
*/
void Data_Init(void)
{
    TMP006.DeviceId = GetTMP006DevID();
    TMP006.ManufacID = GetTMP006ManufacID();
    TMP006.Mcu = GatewayDev_For_CC3200;
    TMP006.Status = SENSOR_OUTLINE;
}

void Data_Get_string()
{

}

unsigned long Data_Put_string(char buf[])
{
    char *DataBuffer = (char*)malloc(128);
    long lRetVal = -1;

    lRetVal = TMP006DrvGetTemp(&TMP006.Data);
    if(lRetVal < 0)
    {
        /* ��ȡʧ�ܺ��жϴ���1.Ҫ�ϱ���������2.�˳�����ִ���������� */
        TMP006.Status = SENSOR_OUTLINE;
    }
    else
    {
        TMP006.Status = SENSOR_ONLINE;
    }

    strcpy(buf, "{\"services\":[");

    memset(DataBuffer, 0, sizeof(DataBuffer));
    sprintf(DataBuffer, "{\"events\":[{\"identifier\":\"%s\",\"name\":\"inputData\",\"desc\":\"Report\",\"type\":\"info\",\"callType\":\"async\"}],", TMP006.Mcu);
    strcat(buf, DataBuffer);

    memset(DataBuffer, 0, sizeof(DataBuffer));
    sprintf(DataBuffer, "\"inputData\":[{\"name\":\"Temperature\",\"dataType\":{\"type\":\"float\",\"specs\":{\"data\":\"%0.3f\",",TMP006.Data);
    strcat(buf, DataBuffer);

    memset(DataBuffer, 0, sizeof(DataBuffer));
    sprintf(DataBuffer, "\"status\":\"%d\",\"DevID\":\"0x%x\",\"ManufacturerID\":\"0x%x\"}}}]}",TMP006.Status,TMP006.DeviceId,TMP006.ManufacID);
    strcat(buf, DataBuffer);

    strcat(buf, "]}");
    free(DataBuffer);
    return strlen(buf);

}

//*****************************************************************************
//
//! itoa
//!
//!    @brief  Convert integer to ASCII in decimal base
//!
//!     @param  cNum is input integer number to convert
//!     @param  cString is output string
//!
//!     @return number of ASCII parameters
//!
//!
//
//*****************************************************************************
unsigned short itoa(short cNum, char *cString)
{
    char* ptr;
    short uTemp = cNum;
    unsigned short length;

    // value 0 is a special case
    if (cNum == 0)
    {
        length = 1;
        *cString = '0';

        return length;
    }

    // Find out the length of the number, in decimal base
    length = 0;
    while (uTemp > 0)
    {
        uTemp /= 10;
        length++;
    }

    // Do the actual formatting, right to left
    uTemp = cNum;
    ptr = cString + length;
    while (uTemp > 0)
    {
        --ptr;
        *ptr = pcDigits[uTemp % 10];
        uTemp /= 10;
    }

    return length;
}
char * ftoa(float dValue, char * chBuffer , int size)
{
    char * pch = chBuffer;
    int temp;
    int i;
    if(!pch)
      return 0;

    if(!(dValue <= 1E-307 && dValue >= -1E-307)){

        if(dValue < 0){
            *pch++ = '-';
            dValue = -dValue;
        }

        temp = (int)dValue;
        itoa(temp , pch);
        unsigned char ucLen = strlen(pch);
        pch += ucLen;
        *pch++ = '.';
        dValue -= (int)dValue;
        ucLen = size - ucLen - 1;

        for(i = 0; i < ucLen; i++){
            dValue = dValue  * 10;
            temp = (int)dValue;
            itoa(temp, pch);
            pch += strlen(pch);
            dValue -= (int)dValue;
        }
    }else
        *pch++ = '0';

    pch--;
    return chBuffer;
}

