/*
 * $Id: TkNetIf.h,v 1.3 Broadcom SDK $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:     TkNetIf.h
 * Purpose: 
 *
 */
 
#ifndef _SOC_EA_TkNetIf_H
#define _SOC_EA_TkNetIf_H

#if defined(__cplusplus)
extern "C"  {
#endif

#include <soc/ea/tk371x/TkTypes.h>

Bool    TkOsDataTx (uint8 pathId, uint8 * buf, uint16 len);
void    TkOsGetIfMac (uint8 * mac);

#if defined(__cplusplus)
}
#endif

#endif /* _SOC_EA_TkNetIf_H */
