/*
 * $Id: lb_util.h,v 1.1 Broadcom SDK $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Utilities for Loopback Tests 
 *
 */
#ifndef __LB_UTIL__H
#define __LB_UTIL__H

int lbu_setup_port(int unit, bcm_port_t port, 
                          int req_speed, int autoneg);

#endif /*!__LB_UTIL__H */
