/*
 *         
 * $Id: phymod.xml,v 1.1.2.5 Broadcom SDK $
 * 
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *         
 *     
 * DO NOT EDIT THIS FILE!
 *
 */

#ifndef _EAGLE_PHY_2_COMMON_H__H_
#define _EAGLE_PHY_2_COMMON_H__H_

#include <phymod/phymod.h>
#include "tier1/common/srds_api_err_code.h"
#include "tier1/common/srds_api_enum.h"
#include "tier1/common/srds_api_uc_common.h"
#include "tier1/eagle_phy_2_usr_includes.h"

#define PHYMOD_STDK_PRINT
//#define PHYMOD_STDK_PRINT post_log
//#define post_log(x)

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef uint32              uint32_t;
typedef uint16              uint16_t;
typedef uint8               uint8_t;

typedef signed char         int8;
typedef signed short        int16;
typedef signed int         int32;

typedef uint16_t err_code_t;

/* Translate eagle osr_mode register value to phymod enum*/
int eagle_phy_2_osr_mode_to_enum(int osr_mode, phymod_osr_mode_t* osr_mode_en);

#endif /*_EAGLE_PHY_2_COMMON_H__H_*/
