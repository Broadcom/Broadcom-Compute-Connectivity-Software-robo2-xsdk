/*
 * $Id: macipadr.c,v 1.2 Broadcom SDK $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        macipadr.c
 * Purpose:
 * Requires:    
 */

#include <soc/types.h>
#include <soc/macipadr.h>

/* Global declarations */

const sal_mac_addr_t _soc_mac_spanning_tree =
	{0x01, 0x80, 0xc2, 0x00, 0x00, 0x00};

const sal_mac_addr_t _soc_mac_all_routers =
	{0x01, 0x00, 0x5e, 0x00, 0x00, 0x02};

const sal_mac_addr_t _soc_mac_all_zeroes =
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const sal_mac_addr_t _soc_mac_all_ones =
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
