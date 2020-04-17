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
#include <phymod/phymod.h>
#include <phymod/phymod_dispatch.h>

#ifdef PHYMOD_QUADRA28_SUPPORT

#include <phymod/chip/quadra28.h>


__phymod__dispatch__t__ phymod_quadra28_driver = {

    quadra28_core_identify,
    quadra28_core_info_get,
    quadra28_core_lane_map_set,
    quadra28_core_lane_map_get,
    quadra28_core_reset_set,
    quadra28_core_reset_get,
    quadra28_core_firmware_info_get,
    quadra28_phy_firmware_core_config_set,
    quadra28_phy_firmware_core_config_get,
    quadra28_phy_firmware_lane_config_set,
    quadra28_phy_firmware_lane_config_get,
    quadra28_core_pll_sequencer_restart,
    NULL, /* phymod_core_wait_event */
    quadra28_phy_rx_restart,
    quadra28_phy_polarity_set,
    quadra28_phy_polarity_get,
    quadra28_phy_tx_set,
    quadra28_phy_tx_get,
    quadra28_phy_media_type_tx_get,
    NULL, /* phymod_phy_tx_override_set */
    NULL, /* phymod_phy_tx_override_get */
    quadra28_phy_rx_set,
    quadra28_phy_rx_get,
    quadra28_phy_rx_adaptation_resume,
    quadra28_phy_reset_set,
    quadra28_phy_reset_get,
    quadra28_phy_power_set,
    quadra28_phy_power_get,
    NULL, /* phymod_phy_hg2_codec_control_set */
    NULL, /* phymod_phy_hg2_codec_control_get */
    quadra28_phy_tx_lane_control_set,
    quadra28_phy_tx_lane_control_get,
    quadra28_phy_rx_lane_control_set,
    quadra28_phy_rx_lane_control_get,
    quadra28_phy_fec_enable_set,
    quadra28_phy_fec_enable_get,
    NULL, /* phymod_phy_forced_speed_line_side_set */
    NULL, /* phymod_phy_forced_speed_line_side_get */
    NULL, /* phymod_phy_flow_control_mode_set */
    NULL, /* phymod_phy_flow_control_mode_get */
    NULL, /* phymod_phy_psm_cos_bmp_set */
    NULL, /* phymod_phy_psm_cos_bmp_get */
    NULL, /* phymod_phy_autoneg_oui_set */
    NULL, /* phymod_phy_autoneg_oui_get */
    NULL, /* phymod_phy_eee_set */
    NULL, /* phymod_phy_eee_get */
    quadra28_phy_interface_config_set,
    quadra28_phy_interface_config_get,
    quadra28_phy_cl72_set,
    quadra28_phy_cl72_get,
    quadra28_phy_cl72_status_get,
    quadra28_phy_autoneg_ability_set,
    quadra28_phy_autoneg_ability_get,
    quadra28_phy_autoneg_remote_ability_get,
    quadra28_phy_autoneg_set,
    quadra28_phy_autoneg_get,
    NULL, /* phymod_phy_autoneg_restart_set */
    NULL, /* phymod_phy_autoneg_status_get */
    quadra28_core_init,
    NULL, /* phymod_phy_pll_multiplier_get */
    quadra28_phy_init,
    quadra28_phy_loopback_set,
    quadra28_phy_loopback_get,
    quadra28_phy_rx_pmd_locked_get,
    NULL, /* phymod_phy_rx_signal_detect_get */
    quadra28_phy_link_status_get,
    quadra28_phy_status_dump,
    NULL, /* phymod_phy_pcs_userspeed_set */
    NULL, /* phymod_phy_pcs_userspeed_get */
    quadra28_phy_reg_read,
    quadra28_phy_reg_write,
    NULL, /* phymod_phy_private_read */
    NULL, /* phymod_phy_private_write */
    NULL, /* phymod_phy_rev_id */
    NULL, /* phymod_phy_lane_cross_switch_map_set */
    NULL, /* phymod_phy_lane_cross_switch_map_get */
    NULL, /* phymod_phy_intr_enable_set */
    NULL, /* phymod_phy_intr_enable_get */
    NULL, /* phymod_phy_intr_status_get */
    NULL, /* phymod_phy_intr_status_clear */
    quadra28_phy_i2c_read,
    quadra28_phy_i2c_write,
    quadra28_phy_gpio_config_set,
    quadra28_phy_gpio_config_get,
    quadra28_phy_gpio_pin_value_set,
    quadra28_phy_gpio_pin_value_get,
    NULL, /* phymod_timesync_config_set */
    NULL, /* phymod_timesync_config_get */
    NULL, /* phymod_timesync_enable_set */
    NULL, /* phymod_timesync_enable_get */
    NULL, /* phymod_timesync_nco_addend_set */
    NULL, /* phymod_timesync_nco_addend_get */
    NULL, /* phymod_timesync_framesync_mode_set */
    NULL, /* phymod_timesync_framesync_mode_get */
    NULL, /* phymod_timesync_local_time_set */
    NULL, /* phymod_timesync_local_time_get */
    NULL, /* phymod_timesync_load_ctrl_set */
    NULL, /* phymod_timesync_load_ctrl_get */
    NULL, /* phymod_timesync_tx_timestamp_offset_set */
    NULL, /* phymod_timesync_tx_timestamp_offset_get */
    NULL, /* phymod_timesync_rx_timestamp_offset_set */
    NULL, /* phymod_timesync_rx_timestamp_offset_get */
    NULL, /* phymod_timesync_capture_timestamp_get */
    NULL, /* phymod_timesync_heartbeat_timestamp_get */
    NULL, /* phymod_timesync_do_sync */
    quadra28_edc_config_set,
    quadra28_edc_config_get,
    NULL, /* phymod_phy_core_mode_set */
    NULL, /* phymod_phy_core_mode_get */
    quadra28_failover_mode_set,
    quadra28_failover_mode_get,
    NULL, /* phymod_port_init */
    NULL, /* phymod_phy_autoneg_try_enable */
    NULL, /* phymod_debug_ether */
    NULL, /* phymod_phy_short_chn_mode_enable_set */
    NULL, /* phymod_phy_short_chn_mode_enable_get */
    NULL, /* phymod_port_enable_set */
    NULL, /* phymod_port_enable_get */
    NULL, /* phymod_phy_eye_margin_est_get */
    NULL, /* phymod_phy_multi_get */
    NULL, /* phymod_phy_rescal_set */
    NULL, /* phymod_phy_rescal_get */
    NULL, /* phymod_phy_sw_an_control_status_get */
    NULL, /* phymod_phy_sw_an_base_page_exchange_handler */
    NULL, /* phymod_phy_sw_an_lp_page_rdy_handler */
    NULL, /* phymod_phy_sw_an_advert_set */
    NULL, /* phymod_phy_sw_autoneg_enable */
};

#endif /* PHYMOD_QUADRA28_SUPPORT */
