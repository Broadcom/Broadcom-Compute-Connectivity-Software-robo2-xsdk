/*
 * $Id: link.h,v 1.1 Broadcom SDK $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * This file contains Link module definitions internal to the BCM library.
 */

#ifndef _BCM_INT_LINK_H
#define _BCM_INT_LINK_H

extern int _bcm_robo_link_get(int unit, bcm_port_t port, int *link);
extern int _bcm_robo_link_force(int unit, bcm_port_t port, int force, int link);

#endif	/* !_BCM_INT_LINK_H */
