/*
 * $Id: kconfig.h,v 1.2 Broadcom SDK $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

extern char *kconfig_get(const char *name);
extern int kconfig_get_next(char **name, char **value);
extern int kconfig_set(char *name, char *value);
