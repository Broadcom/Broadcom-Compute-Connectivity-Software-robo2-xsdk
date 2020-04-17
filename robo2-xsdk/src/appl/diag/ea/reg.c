/*
 * $Id: reg.c,v 1.2 Broadcom SDK $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * socdiag register commands
 */

#include <sal/core/libc.h>
#include <soc/counter.h>
#include <sal/appl/pci.h>
#include <soc/debug.h>
#include <soc/cmic.h>
#include <soc/drv_if.h>

#include <appl/diag/system.h>
#include <appl/diag/sysconf.h>
#include <ibde.h>

cmd_result_t
cmd_ea_reg_list(int unit, args_t *a)
{
	return CMD_OK;
}
