/*
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated from the registers file.
 * Edits to this file will be lost when it is regenerated.
 *
 * $Id: $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:	blocks.c
 * Purpose:	Common block info for all chips.
 */

#include <soc/chip.h>

int soc_robo_block_list[][4] = {
    { SOC_BLK_SYS, SOC_BLOCK_TYPE_INVALID }, /* 0 */
    { SOC_BLK_EPIC, SOC_BLOCK_TYPE_INVALID }, /* 1 */
    { SOC_BLK_CPIC, SOC_BLOCK_TYPE_INVALID }, /* 2 */
    { SOC_BLK_GPIC, SOC_BLOCK_TYPE_INVALID }, /* 3 */
    { SOC_BLK_SPI, SOC_BLOCK_TYPE_INVALID }, /* 4 */
    { SOC_BLK_CRU_OTP, SOC_BLOCK_TYPE_INVALID }, /* 5 */
    { SOC_BLK_CB, SOC_BLOCK_TYPE_INVALID }, /* 6 */
    { SOC_BLK_MDIO, SOC_BLOCK_TYPE_INVALID }, /* 7 */
    { SOC_BLK_CRU_AVS, SOC_BLOCK_TYPE_INVALID }, /* 8 */
    { SOC_BLK_SMBUS, SOC_BLOCK_TYPE_INVALID }, /* 9 */
    { SOC_BLK_CRU_TS_TOP, SOC_BLOCK_TYPE_INVALID }, /* 10 */
    { SOC_BLK_PORT_MACRO, SOC_BLOCK_TYPE_INVALID }, /* 11 */
    { SOC_BLK_CRU_CRU, SOC_BLOCK_TYPE_INVALID }, /* 12 */
    { SOC_BLK_CRU_TS_CORE, SOC_BLOCK_TYPE_INVALID }, /* 13 */
    { SOC_BLK_NPA, SOC_BLOCK_TYPE_INVALID }, /* 14 */
    { SOC_BLK_PWM, SOC_BLOCK_TYPE_INVALID }, /* 15 */
    { SOC_BLK_UNIMAC, SOC_BLOCK_TYPE_INVALID }, /* 16 */
    { SOC_BLK_XLPORT, SOC_BLOCK_TYPE_INVALID }, /* 17 */
    { SOC_BLK_IDM_CB, SOC_BLOCK_TYPE_INVALID }, /* 18 */
    { SOC_BLK_LED, SOC_BLOCK_TYPE_INVALID }, /* 19 */
    { SOC_BLK_QSPI, SOC_BLOCK_TYPE_INVALID }, /* 20 */
    { SOC_BLK_HPA, SOC_BLOCK_TYPE_INVALID }, /* 21 */
    { SOC_BLK_UART, SOC_BLOCK_TYPE_INVALID }, /* 22 */
    { SOC_BLK_WDT, SOC_BLOCK_TYPE_INVALID }, /* 23 */
    { SOC_BLK_GPIO, SOC_BLOCK_TYPE_INVALID }, /* 24 */
    { SOC_BLK_TIM, SOC_BLOCK_TYPE_INVALID }, /* 25 */
    { SOC_BLK_RNG, SOC_BLOCK_TYPE_INVALID }, /* 26 */
    { SOC_BLK_M7SC, SOC_BLOCK_TYPE_INVALID }, /* 27 */
    { SOC_BLK_IDM_DS0, SOC_BLOCK_TYPE_INVALID }, /* 28 */
    { SOC_BLK_IDM_SPIM, SOC_BLOCK_TYPE_INVALID }, /* 29 */
    { SOC_BLK_GPHY_DIG, SOC_BLOCK_TYPE_INVALID }, /* 30 */
    { SOC_BLK_CPIC, SOC_BLK_GPIC, SOC_BLOCK_TYPE_INVALID }, /* 31 */
    { SOC_BLK_CPIC, SOC_BLK_EPIC, SOC_BLK_GPIC, SOC_BLOCK_TYPE_INVALID }, /* 32 */
    { SOC_BLK_CPIC, SOC_BLK_EPIC, SOC_BLOCK_TYPE_INVALID }, /* 33 */
    { SOC_BLOCK_TYPE_INVALID } /* end */
};

int _soc_robo_max_blocks_per_entry_get(void)
{
    return 4;
}

