/* *
 * * $ID: $
 * *
 * * 
 * * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 * * 
* * $Copyright: (c) 2020 Broadcom Inc.
* * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef CBX_INT_LIN_H
#define CBX_INT_LIN_H

#ifdef MANAGED_MODE
#include <sal/types.h>
#include <fsal_int/shared_types.h>
#else
#include <sal_types.h>
#endif
#include <shared/types.h>
#include <soc/types.h>
#include <fsal_int/types.h>

#define CBXI_LOGICAL_IF_MIN            1
#define CBXI_LOGICAL_IF_MAX            2048

extern int cbxi_lin_init( void );
extern int cbxi_lin_slot_get(int *linid );
extern int cbxi_lin_slot_release(int linid);
extern int cbxi_lin_dest_get(int unit, int linid, cbxi_pgid_t *lpgid);
extern int cbxi_lin_dest_set(int unit, int linid, cbxi_pgid_t lpgid);
extern int cbxi_ptp_lin_source_set(int unit, int linid,
                         cbxi_pgid_t lpgid, uint32_t vsiid, uint8_t slic_type);
extern int cbxi_ptp_lin_source_release(int unit, int linid, cbxi_pgid_t lpgid,
                                       uint32_t vsiid, uint8_t slic_type);
#endif /* CBX_INT_LIN_H */
