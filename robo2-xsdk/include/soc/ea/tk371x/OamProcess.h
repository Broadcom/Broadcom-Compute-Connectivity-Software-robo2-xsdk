/*
 * $Id: OamProcess.h,v 1.4 Broadcom SDK $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        name.h
 * Purpose:     Purpose of the file
 */

#ifndef _SOC_EA_OamProcess_H_
#define _SOC_EA_OamProcess_H_

#include <soc/ea/tk371x/TkTypes.h>
#include <soc/ea/tk371x/TkMsg.h>

Bool    TkDataProcessHandle (uint8 pathId, TkMsgBuff * pMsg, uint16 len);

void    TkAlmHandlerRegister (uint8, uint8 (*funp) (uint8 , uint8 *, short));

void    TkAlarmRxThread (void *cookie);

#endif	/* !_SOC_EA_OamProcess_H_ */
