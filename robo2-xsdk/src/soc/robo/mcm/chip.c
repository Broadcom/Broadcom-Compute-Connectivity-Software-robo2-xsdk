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
 * File:	chip.c
 * Purpose:	Chip driver table defined
 * Supports:	bcm53242_a0
 *		bcm53262_a0
 *		bcm53115_a0
 *		bcm53118_a0
 *		bcm53280_a0
 *		bcm53280_b0
 *		bcm53101_a0
 *		bcm53125_a0
 *		bcm53128_a0
 *		bcm53134_a0
 *		bcm53134_b0
 *		bcm53600_a0
 *		bcm89500_a0
 *		bcm53010_a0
 *		bcm5389_a0
 *		bcm53020_a0
 *		bcm5396_a0
 *		bcm53158_b0
 *		bcm53158_a0
 */


#include <soc/defs.h>
#include <soc/robo/mcm/driver.h>

/*
 * A null driver to indicate a given chip is not supported.
 *
 * This assumes that the chip is identified by device/rev id
 * and this will be 0 for the null driver, thus not match.
 */
#if !defined(BCM_ALL_CHIPS)
static soc_driver_t soc_driver_none;
#endif

/*
 * The table of all known drivers
 * Indexed by supported chips
 */
soc_driver_t *soc_robo_base_driver_table[] = {


#if defined(BCM_53242_A0)
    &soc_driver_bcm53242_a0,
#else
    &soc_driver_none,
#endif

#if defined(BCM_53262_A0)
    &soc_driver_bcm53262_a0,
#else
    &soc_driver_none,
#endif

#if defined(BCM_53115_A0)
    &soc_driver_bcm53115_a0,
#else
    &soc_driver_none,
#endif

#if defined(BCM_53118_A0)
    &soc_driver_bcm53118_a0,
#else
    &soc_driver_none,
#endif

#if defined(BCM_53280_A0)
    &soc_driver_bcm53280_a0,
#else
    &soc_driver_none,
#endif

#if defined(BCM_53280_B0)
    &soc_driver_bcm53280_b0,
#else
    &soc_driver_none,
#endif

#if defined(BCM_53101_A0)
    &soc_driver_bcm53101_a0,
#else
    &soc_driver_none,
#endif

#if defined(BCM_53125_A0)
    &soc_driver_bcm53125_a0,
#else
    &soc_driver_none,
#endif

#if defined(BCM_53128_A0)
    &soc_driver_bcm53128_a0,
#else
    &soc_driver_none,
#endif

#if defined(BCM_53134_A0)
    &soc_driver_bcm53134_a0,
#else
    &soc_driver_none,
#endif

#if defined(BCM_53134_B0)
    &soc_driver_bcm53134_b0,
#else
    &soc_driver_none,
#endif

#if defined(BCM_53600_A0)
    &soc_driver_bcm53600_a0,
#else
    &soc_driver_none,
#endif

#if defined(BCM_89500_A0)
    &soc_driver_bcm89500_a0,
#else
    &soc_driver_none,
#endif

#if defined(BCM_53010_A0)
    &soc_driver_bcm53010_a0,
#else
    &soc_driver_none,
#endif

#if defined(BCM_5389_A0)
    &soc_driver_bcm5389_a0,
#else
    &soc_driver_none,
#endif

#if defined(BCM_53020_A0)
    &soc_driver_bcm53020_a0,
#else
    &soc_driver_none,
#endif

#if defined(BCM_5396_A0)
    &soc_driver_bcm5396_a0,
#else
    &soc_driver_none,
#endif

#if defined(BCM_53158_B0)
    &soc_driver_bcm53158_b0,
#else
    &soc_driver_none,
#endif

#if defined(BCM_53158_A0)
    &soc_driver_bcm53158_a0,
#else
    &soc_driver_none,
#endif

};

/* Function to translate SOC_CHIP_ to supported idx*/
int soc_robo_chip_type_to_index(soc_chip_types chip)
{
    switch(chip) {
    case SOC_CHIP_BCM53242_A0:  return 0;
    case SOC_CHIP_BCM53262_A0:  return 1;
    case SOC_CHIP_BCM53115_A0:  return 2;
    case SOC_CHIP_BCM53118_A0:  return 3;
    case SOC_CHIP_BCM53280_A0:  return 4;
    case SOC_CHIP_BCM53280_B0:  return 5;
    case SOC_CHIP_BCM53101_A0:  return 6;
    case SOC_CHIP_BCM53125_A0:  return 7;
    case SOC_CHIP_BCM53128_A0:  return 8;
    case SOC_CHIP_BCM53134_A0:  return 9;
    case SOC_CHIP_BCM53134_B0:  return 10;
    case SOC_CHIP_BCM53600_A0:  return 11;
    case SOC_CHIP_BCM89500_A0:  return 12;
    case SOC_CHIP_BCM53010_A0:  return 13;
    case SOC_CHIP_BCM5389_A0:  return 14;
    case SOC_CHIP_BCM53020_A0:  return 15;
    case SOC_CHIP_BCM5396_A0:  return 16;
    case SOC_CHIP_BCM53158_B0:  return 17;
    case SOC_CHIP_BCM53158_A0:  return 18;
    default:  assert(0);
    }
    return -1;
}

/* Reverse translation of above. */
soc_chip_types soc_robo_index_to_chip_type(int chip)
{
    switch(chip) {
    case 0: return SOC_CHIP_BCM53242_A0;
    case 1: return SOC_CHIP_BCM53262_A0;
    case 2: return SOC_CHIP_BCM53115_A0;
    case 3: return SOC_CHIP_BCM53118_A0;
    case 4: return SOC_CHIP_BCM53280_A0;
    case 5: return SOC_CHIP_BCM53280_B0;
    case 6: return SOC_CHIP_BCM53101_A0;
    case 7: return SOC_CHIP_BCM53125_A0;
    case 8: return SOC_CHIP_BCM53128_A0;
    case 9: return SOC_CHIP_BCM53134_A0;
    case 10: return SOC_CHIP_BCM53134_B0;
    case 11: return SOC_CHIP_BCM53600_A0;
    case 12: return SOC_CHIP_BCM89500_A0;
    case 13: return SOC_CHIP_BCM53010_A0;
    case 14: return SOC_CHIP_BCM5389_A0;
    case 15: return SOC_CHIP_BCM53020_A0;
    case 16: return SOC_CHIP_BCM5396_A0;
    case 17: return SOC_CHIP_BCM53158_B0;
    case 18: return SOC_CHIP_BCM53158_A0;
    default:  assert(0);
    }
    return -1;
}

