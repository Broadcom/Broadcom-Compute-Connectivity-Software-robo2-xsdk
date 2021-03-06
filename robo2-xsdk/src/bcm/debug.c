/*
 * $Id: debug.c,v 1.13 Broadcom SDK $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        debug.c
 * Purpose:	BCM API Debug declarations
 * Requires:    
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/cm.h>
#include <bcm/error.h>

#if defined(BROADCOM_DEBUG)

/*
 * API Debug Message Helper.
 * Called from the BCM_API macro in <bcm/debug.h>
 * The BCM_API macro is called from the bcm api dispatcher.
 */
void
_bcm_debug_api(int log_src, char *api,
	       int nargs, int ninargs,
	       int arg1, int arg2, int arg3, int rv)
{
    char	*rvstr;

    if (rv < 0) {
	rvstr = bcm_errmsg(rv);
    } else {
	rvstr = bcm_errmsg(BCM_E_NONE);
    }

    switch (ninargs) {
    case 0:
	LOG_VERBOSE(log_src,
                    (BSL_META("API: %s(%s) -> %d %s\n"),
                     api,
                     nargs > 0 ? "..." : "",
                     rv, rvstr));
	break;
    case 1:
	LOG_VERBOSE(log_src,
                    (BSL_META("API: %s(%d%s) -> %d %s\n"),
                     api, arg1,
                     nargs > 1 ? ",..." : "",
                     rv, rvstr));
	break;
    case 2:
	LOG_VERBOSE(log_src,
                    (BSL_META("API: %s(%d,%d%s) -> %d %s\n"),
                     api, arg1, arg2,
                     nargs > 2 ? ",..." : "",
                     rv, rvstr));
	break;
    default:
	LOG_VERBOSE(log_src,
                    (BSL_META("API: %s(%d,%d,%d%s) -> %d %s\n"),
                     api, arg1, arg2, arg3,
                     nargs > 3 ? ",..." : "",
                     rv, rvstr));
	break;
    }
}

#endif /* BROADCOM_DEBUG */
