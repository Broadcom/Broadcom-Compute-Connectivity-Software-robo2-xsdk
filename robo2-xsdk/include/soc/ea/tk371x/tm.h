/*
 * $Id: tm.h,v 1.3 Broadcom SDK $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:     tm.h
 * Purpose:
 *
 */

#ifndef _SOC_EA_TM_H
#define _SOC_EA_TM_H

#include <soc/ea/tk371x/TkTmApi.h>

#define _soc_ea_ena_user_traffic_set 	TkExtOamSetEnaUserTraffic
#define _soc_ea_dis_user_traffic_set	TkExtOamSetDisUserTraffic
#define _soc_ea_bcast_rate_limit_get	TkExtOamGetBcastRateLimit
#define _soc_ea_bcast_rate_limit_set	TkExtOamSetBcastRateLimit


#endif /* _SOC_EA_TM_H */
