/*
 * $Id: topo.h,v 1.4 Broadcom SDK $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef   _BCM_TOPO_INT_H_
#define   _BCM_TOPO_INT_H_

#include <bcm/port.h>

extern int _bcm_topo_port_get(int unit,int dest_modid,bcm_port_t *exit_port);

#endif /* !_BCM_TOPO_INT_H_ */
