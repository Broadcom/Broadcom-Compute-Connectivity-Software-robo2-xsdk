/*
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * File:
 *    robo2.h
 * Description
 *    robo2.h
 */

#ifndef _SOC_ROBO2_H_
#define _SOC_ROBO2_H_

#include <soc/drv.h>

extern int soc_robo2_misc_init(int);
extern int soc_robo2_mmu_init(int);
extern int soc_robo2_age_timer_get(int, int *, int *);
extern int soc_robo2_age_timer_max_get(int, int *);
extern int soc_robo2_age_timer_set(int, int, int);
extern soc_functions_t soc_robo2_drv_funs;

#endif  /* !_SOC_ROBO2_H_ */
