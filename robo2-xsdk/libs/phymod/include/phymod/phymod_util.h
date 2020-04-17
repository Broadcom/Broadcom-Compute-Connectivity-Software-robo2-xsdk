/*
 *         
 * $Id: phymod_definitions.h,v 1.2.2.12 Broadcom SDK $
 * 
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *         
 * Shell diagnostics of Phymod    
 *
 */

#ifndef _PHYMOD_UTIL_H_
#define _PHYMOD_UTIL_H_

#include <phymod/phymod.h>


/******************************************************************************
Functions
******************************************************************************/


int phymod_util_lane_config_get(const phymod_access_t *phys, int *start_lane, int *num_of_lane);
#define _SHR_LANEBMP_WBIT(_lane)                  (1U << (_lane))
#define PHYMOD_LANEPBMP_MEMBER(_bmp, _lane)       (((_bmp) & (_SHR_LANEBMP_WBIT(_lane))) != 0)

#endif /*_PHYMOD_UTIL_H_*/
