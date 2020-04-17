/*
 * $Id: xstp.h,v 1.3 Broadcom SDK $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:     xstp.h
 * Purpose:
 *
 */

#ifndef _SOC_EA_XSTP_H
#define _SOC_EA_XSTP_H

#include <soc/ea/tk371x/TkXstpApi.h>

typedef TagRstpBridgeCfg 	_soc_ea_tag_rstp_bridge_cfg_t;
typedef TagRstpPortCfg		_soc_ea_tag_rstp_port_cfg_t;

#define _soc_ea_rstp_bridge_set TkExtOamSetRstpBridge
#define _soc_ea_rstp_bridge_get TkExtOamGetRstpBridge
#define _soc_ea_rstp_port_set	TkExtOamSetRstpPort
#define _soc_ea_rstp_port_get	TkExtOamGetRstpPort


#endif /* _SOC_EA_XSTP_H */
