/*
 * $Id: sysconf.h,v 1.7 Broadcom SDK $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef __SYSCONF_H__
#define __SYSCONF_H__

#include <sal/types.h>
#include <soc/cmtypes.h>

extern int
sysconf_init(void);


/* Configuration */
extern int
sysconf_probe(void);

extern int
sysconf_attach(int pci_device_n);

extern int
sysconf_detach(int pci_device_n);

extern char*
sysconf_get_property(const char* property);

extern void
sysconf_chip_override(int unit, uint16 *devID, uint8 *revID);
#endif	/*! __SYSCONF_H__ */
