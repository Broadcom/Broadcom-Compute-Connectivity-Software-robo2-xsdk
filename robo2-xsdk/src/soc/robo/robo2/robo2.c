/*
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        robo2.c
 * Purpose:
 * Requires:
 */
#include <shared/bsl.h>
#include <soc/drv.h>
#include "robo2.h"
#include <soc/error.h>
#include <soc/debug.h>


#ifdef BCM_ROBO2_SUPPORT

/*
 * Robo chip driver functions.  Common across Robo devices for now.
 * These may get broken out by chip in the future, but not needed yet.
 */
soc_functions_t soc_robo2_drv_funs = {
    soc_robo2_misc_init,
    soc_robo2_mmu_init,
    soc_robo2_age_timer_get,
    soc_robo2_age_timer_max_get,
    soc_robo2_age_timer_set
};


/* soc_misc_init() :
 *  - allowed user to do the init by chip dependant configuration.
 *
 *  Note :
 *   1. below routine is for all Roob chip related init routine.
 *   2. different robo2 chip init section may separated by
 *      "SOC_IS_ROBO53xx(unit)"
 */
int
soc_robo2_misc_init(int unit)
{
    return SOC_E_NONE;
}

int
soc_robo2_mmu_init(int unit)
{
    return SOC_E_NONE;
}

int
soc_robo2_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    return SOC_E_NONE;
}

int
soc_robo2_age_timer_max_get(int unit, int *max_seconds)
{
    return SOC_E_NONE;
}

int
soc_robo2_age_timer_set(int unit, int age_seconds, int enable)
{
    return SOC_E_NONE;
}

#endif  /* BCM_ROBO2_SUPPORT */
