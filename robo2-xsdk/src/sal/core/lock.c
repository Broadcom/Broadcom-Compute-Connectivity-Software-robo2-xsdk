/* 
 * $Id:$
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        lock.c
 * Purpose:     SAL Global lock
 *
 */

#include <sal/core/libc.h>
#include <sal/core/sync.h>

/* SAL Global Lock */
sal_mutex_t                     sal_global_lock = NULL;

int
sal_global_lock_init(void)
{
    /* Initialize CM System lock */
    if (sal_global_lock == NULL) {
        sal_global_lock = sal_mutex_create("SAL Global Lock");
        if (sal_global_lock == NULL) {
            return -1;
        }
    }

    return 0;
}
