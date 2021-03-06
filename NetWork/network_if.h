//*****************************************************************************
// network_if.h
//
// Networking interface macro and function prototypes for CC3200 device
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
// 
// 
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//    Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the   
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#ifndef __NETWORK_IF__H__
#define __NETWORK_IF__H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************

#ifdef __cplusplus
extern "C"
{
#endif



//*****************************************************************************
//
// API Function prototypes
//
//*****************************************************************************
extern long Network_IF_InitDriver(unsigned int uiMode);
extern long Network_IF_DeInitDriver(void);
extern long Network_IF_ConnectAP(char * pcSsid, SlSecParams_t SecurityParams);
extern long Network_IF_DisconnectFromAP();
extern long Network_IF_IpConfigGet(unsigned long *aucIP, unsigned long *aucSubnetMask,
                unsigned long *aucDefaultGateway, unsigned long *aucDNSServer);
extern long Network_IF_GetHostIP( char* pcHostName,unsigned long * pDestinationIP);
extern unsigned long Network_IF_CurrentMCUState();
extern void Network_IF_UnsetMCUMachineState(char stat);
extern void Network_IF_SetMCUMachineState(char stat);
extern void Network_IF_ResetMCUStateMachine();
extern unsigned short itoa(short cNum, char *cString);
extern void InitializeAppVariables();
// Network App specific status/error codes which are used only in this file
typedef enum{
     // Choosing this number to avoid overlap w/ host-driver's error codes
    DEVICE_NOT_IN_STATION_MODE = -0x7F0,
    DEVICE_NOT_IN_AP_MODE = DEVICE_NOT_IN_STATION_MODE - 1,
    DEVICE_NOT_IN_P2P_MODE = DEVICE_NOT_IN_AP_MODE - 1,

    STATUS_CODE_MAX = -0xBB8
}e_NetAppStatusCodes;

extern volatile unsigned long  g_ulStatus;
//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif
#endif


