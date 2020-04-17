/*
 * $Id: vulcan_service.h,v 1.4 Broadcom SDK $
 *
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
 
#ifndef _ROBO_COMMON_H
#define _ROBO_COMMON_H

extern uint32 drv_robo_port_sample_rate_get(int unit, uint32 val);
extern int drv_robo_port_sw_detach(int unit);
extern int drv_robo_port_probe(int unit, soc_port_t p, int *okay);
extern int drv_robo_port_sw_init_status_get(int unit);

#endif
