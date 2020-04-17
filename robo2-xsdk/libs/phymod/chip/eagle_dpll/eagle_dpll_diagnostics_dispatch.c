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

#include <phymod/phymod.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_diagnostics_dispatch.h>

#ifdef PHYMOD_EAGLE_DPLL_SUPPORT

#include <phymod/chip/eagle_dpll_diagnostics.h>


__phymod_diagnostics__dispatch__t__ phymod_diagnostics_eagle_dpll_diagnostics_driver = {

    eagle_dpll_phy_rx_slicer_position_set,
    eagle_dpll_phy_rx_slicer_position_get,
    eagle_dpll_phy_rx_slicer_position_max_get,
    eagle_dpll_phy_prbs_config_set,
    eagle_dpll_phy_prbs_config_get,
    eagle_dpll_phy_prbs_enable_set,
    eagle_dpll_phy_prbs_enable_get,
    eagle_dpll_phy_prbs_status_get,
    eagle_dpll_phy_pattern_config_set,
    eagle_dpll_phy_pattern_config_get,
    eagle_dpll_phy_pattern_enable_set,
    eagle_dpll_phy_pattern_enable_get,
    eagle_dpll_core_diagnostics_get,
    eagle_dpll_phy_diagnostics_get,
    eagle_dpll_phy_pmd_info_dump,
    NULL, /* phymod_phy_pcs_info_dump */
    eagle_dpll_phy_eyescan_run,
    NULL, /* phymod_phy_link_mon_enable_set */
    NULL, /* phymod_phy_link_mon_enable_get */
    NULL, /* phymod_phy_link_mon_status_get */
    NULL, /* phymod_phy_fec_correctable_counter_get */
    NULL, /* phymod_phy_fec_uncorrectable_counter_get */
};

#endif /* PHYMOD_EAGLE_DPLL_SUPPORT */
