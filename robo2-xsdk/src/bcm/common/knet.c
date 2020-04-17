/*
 * $Id: knet.c,v 1.1 Broadcom SDK $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:    knet.c
 * Purpose: Manages common KNET functions
 */

#include <sal/core/libc.h>

#include <bcm/knet.h>

/*
 * Function:
 *      bcm_knet_netif_t_init
 * Purpose:
 *      Initialize the bcm_knet_netif_t structure.
 * Parameters:
 *      netif - Pointer to the struct to be init'ed
 */

void
bcm_knet_netif_t_init(bcm_knet_netif_t *netif)
{
    if (netif != NULL) {
        sal_memset(netif, 0, sizeof(*netif));
    }
}

/*
 * Function:
 *      bcm_knet_filter_t_init
 * Purpose:
 *      Initialize the bcm_knet_filter_t structure.
 * Parameters:
 *      filter - Pointer to the struct to be init'ed
 */

void
bcm_knet_filter_t_init(bcm_knet_filter_t *filter)
{
    if (filter != NULL) {
        sal_memset(filter, 0, sizeof(*filter));
    }
}
