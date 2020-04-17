/*
 * $Id: robo_spi.h,v 1.2 Broadcom SDK $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * BCM5301X RoboSwitch utility functions
 */

#ifndef _robo_spi_h_
#define _robo_spi_h_

extern void * robo_attach_spi(void *sih);
extern void robo_detach_spi(void *robo);
extern void robo_spi_rreg(void *robo, uint8 cid, uint8 page, uint8 addr, uint8 *buf, uint len);
extern void robo_spi_wreg(void *robo, uint8 cid, uint8 page, uint8 addr, uint8 *buf, uint len);

#endif /* _robo_spi_h_ */

