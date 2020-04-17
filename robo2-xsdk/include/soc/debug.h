/*
 * $Id: debug.h,v 1.24 Broadcom SDK $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _SOC_DEBUG_H
#define _SOC_DEBUG_H

#include <soc/cmdebug.h>

extern void soc_reg_watch_set(int unit, int value);
extern void soc_mem_watch_set(int unit, int value);

/****************************************************************
 *
 * SOC specific debugging flags
 *
 ****************************************************************/

#if defined(BROADCOM_DEBUG)

#define _SOC_MSG(string) "%s[%d]%s unit %d: " string "\n", __FILE__, __LINE__, FUNCTION_NAME(), unit

#define _BSL_SOC_MSG(string) _ERR_MSG_MODULE_NAME, unit, "%s[%d]%s unit %d: " string "\n", __FILE__, __LINE__, FUNCTION_NAME(), unit

#else

#define _SOC_MSG(string) string

#define _BSL_SOC_MSG(string) _ERR_MSG_MODULE_NAME, unit, string

#endif   /* defined(BROADCOM_DEBUG) */

#endif  /* !_SOC_DEBUG_H */
