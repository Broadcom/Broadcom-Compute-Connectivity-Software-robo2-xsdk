/*
 * $Id: xgxs.h,v 1.7 Broadcom SDK $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        xgxs.h
 * Purpose:     Defines common PHY driver routines for Broadcom XGXS core.
 */
#ifndef _PHY_XGXS_H
#define _PHY_XGXS_H

extern int
phy_xgxs_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data);

extern int
phy_xgxs_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data);

extern int
phy_xgxs_reg_modify(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data,
                    uint32 phy_data_mask);

#endif /* _PHY_XGXS_H */

