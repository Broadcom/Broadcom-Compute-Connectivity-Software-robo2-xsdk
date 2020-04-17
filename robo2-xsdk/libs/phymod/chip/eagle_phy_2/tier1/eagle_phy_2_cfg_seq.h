/*----------------------------------------------------------------------
 * $Id: eagle_phy_2_cfg_seq.h,v 1.1.2.2 Broadcom SDK $ 
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : eagle_phy_2_cfg_seq.h
 * Description: c functions implementing Tier1s for TEMod Serdes Driver
 *---------------------------------------------------------------------*/
/*
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 *  $Copyright: (c) 2020 Broadcom Inc Corporation All Rights Reserved.$
 *  $Id$
*/


#ifndef EAGLE_CFG_SEQ_H 
#define EAGLE_CFG_SEQ_H

#include "common/srds_api_err_code.h"
#include "eagle_phy_2_enum.h"

/** Eagle PHY VCO Config Enum */
enum eagle_phy_2_phy_vco_enum {
     VCO_10p3125,
     VCO_12p5
};



typedef enum {
    EAGLE_PHY_2_SPD_10_SGMII = 0           ,  /*!< 10Mb SGMII (serial)             */
    EAGLE_PHY_2_SPD_100_SGMII              ,  /*!< 100Mb SGMII (serial)            */
    EAGLE_PHY_2_SPD_1000_SGMII             ,  /*!< 1Gb SGMII (serial)              */
    EAGLE_PHY_2_SPD_2500_SGMII             ,  /*!< 2.5Gb SGMII (serial)            */
    EAGLE_PHY_2_SPD_100_FX                 ,  /*!< 100Mb fiber                     */
    EAGLE_PHY_2_SPD_1000_X1                ,  /*!< 1Gb   fiber                     */
    EAGLE_PHY_2_SPD_2500                   ,  /*!< 2.5Gb  based on 1000BASE-X      */
    EAGLE_PHY_2_SPD_10000_CX4              ,  /*!< 10Gb CX4                        */
    EAGLE_PHY_2_SPD_10000_XFI              ,  /*!< 10Gb XFI                        */
    EAGLE_PHY_2_SPD_10000                  ,  /*!< 10Gb XAUI                       */
    EAGLE_PHY_2_SPD_ILLEGAL                   /*!< Illegal value (enum boundary)   */
} eagle_phy_2_spd_intfc_type_t;

typedef enum {
    EAGLE_PHY_2_SPD_10M = 0,        
    EAGLE_PHY_2_SPD_100M,         
    EAGLE_PHY_2_SPD_1G,         
    EAGLE_PHY_2_SPD_2p5G,         
    EAGLE_PHY_2_SPD_1G_KX1 = 13,         
    EAGLE_PHY_2_SPD_10G_KR1 = 15,         
    EAGLE_PHY_2_SPD_5G_X1 = 16,         
    EAGLE_PHY_2_SPD_10G_X1 = 27,         
    EAGLE_PHY_2_SPD_10G_SFI = 31,         
    EAGLE_PHY_2_SPD_10G_CX1 = 51,         
    EAGLE_PHY_2_SPD_1G_CX1 = 52,
    EAGLE_PHY_2_SPD_5G                      , 
    EAGLE_PHY_2_SPD_10G_FEC_OFF             , 
    EAGLE_PHY_2_SPD_10G_FEC_ON              ,      
    EAGLE_PHY_2_SPD_73AN_IEEE_KR_10G_FEC_ABL, 
    EAGLE_PHY_2_SPD_73AN_IEEE_KR_10G_FEC_REQ, 
    EAGLE_PHY_2_SPD_73AN_USER_KR_10G_FEC_ABL, 
    EAGLE_PHY_2_SPD_73AN_USER_KR_10G_FEC_REQ, 
    EAGLE_PHY_2_SPD_73AN_IEEE_KX_1G         , 
    EAGLE_PHY_2_SPD_73AN_USER_KX_1G         , 
    EAGLE_PHY_2_SPD_37AN_IEEE_KX_1G         , 
    EAGLE_PHY_2_SPD_37AN_USER_KX_1G         , 
    EAGLE_PHY_2_SPD_37AN_SGMII_M_1G         , 
    EAGLE_PHY_2_SPD_37AN_SGMII_M_100M       , 
    EAGLE_PHY_2_SPD_37AN_SGMII_M_10M        , 
    EAGLE_PHY_2_SPD_37AN_SGMII_S_10M_100M_1G 
} eagle_phy_2_actual_speed_id_t;

typedef struct {
  int8_t pll_pwrdn;
  int8_t tx_s_pwrdn;
  int8_t rx_s_pwrdn;
} power_status_t;

typedef struct {
  int8_t revid_model;
  int8_t revid_process;
  int8_t revid_bonding;
  int8_t revid_rev_number;
  int8_t revid_rev_letter;
} eagle_phy_2_rev_id0_t;


typedef enum {
  TX = 0,
  Rx
} tx_rx_t;

typedef struct {
  int8_t revid_eee;
  int8_t revid_llp;
  int8_t revid_pir;
  int8_t revid_cl72;
  int8_t revid_micro;
  int8_t revid_mdio;
  int8_t revid_multiplicity;
} eagle_phy_2_rev_id1_t;


typedef enum {
    EAGLE_PRBS_POLYNOMIAL_7 = 0,
    EAGLE_PRBS_POLYNOMIAL_9,
    EAGLE_PRBS_POLYNOMIAL_11,
    EAGLE_PRBS_POLYNOMIAL_15,
    EAGLE_PRBS_POLYNOMIAL_23,
    EAGLE_PRBS_POLYNOMIAL_31,
    EAGLE_PRBS_POLYNOMIAL_58,
    EAGLE_PRBS_POLYNOMIAL_TYPE_COUNT 
}eagle_phy_2_prbs_polynomial_type_t;

#define PATTERN_MAX_SIZE 8

err_code_t eagle_phy_2_phy_pcs_forced_speed_setup(const phymod_access_t *pa, eagle_phy_2_actual_speed_id_t speed);
err_code_t eagle_phy_2_phy_pcs_an_speed_setup(const phymod_access_t *pa, eagle_phy_2_actual_speed_id_t speed);


err_code_t eagle_phy_2_identify(const phymod_access_t *pa, eagle_phy_2_rev_id0_t *rev_id0, eagle_phy_2_rev_id1_t *rev_id1);

err_code_t eagle_phy_2_uc_active_set(const phymod_access_t *pa, uint32_t enable);           /* set microcontroller active or not  */
err_code_t eagle_phy_2_uc_active_get(const phymod_access_t *pa, uint32_t *enable);          /* get microcontroller active or not  */
//err_code_t eagle_phy_2_uc_reset(const phymod_access_t *pa, uint32_t enable);           /* set dw8501 reset  */
err_code_t eagle_phy_2_prbs_tx_inv_data_get(const phymod_access_t *pa, uint32_t *inv_data);
err_code_t eagle_phy_2_prbs_rx_inv_data_get(const phymod_access_t *pa, uint32_t *inv_data);
err_code_t eagle_phy_2_prbs_tx_poly_get(const phymod_access_t *pa, eagle_phy_2_prbs_polynomial_type_t *prbs_poly);
err_code_t eagle_phy_2_prbs_rx_poly_get(const phymod_access_t *pa, eagle_phy_2_prbs_polynomial_type_t *prbs_poly);
err_code_t eagle_phy_2_prbs_tx_enable_get(const phymod_access_t *pa, uint32_t *enable);
err_code_t eagle_phy_2_prbs_rx_enable_get(const phymod_access_t *pa, uint32_t *enable);
err_code_t eagle_phy_2_pll_mode_set(const phymod_access_t *pa, int pll_mode);   /* PLL divider set */
err_code_t eagle_phy_2_core_soft_reset_release(const phymod_access_t *pa, uint32_t enable);   /* release the pmd core soft reset */
err_code_t eagle_phy_2_lane_soft_reset_release(const phymod_access_t *pa, uint32_t enable);   /* release the pmd core soft reset */
err_code_t eagle_phy_2_pram_firmware_enable(const phymod_access_t *pa, int enable);   /* release the pmd core soft reset */
err_code_t eagle_phy_2_pmd_force_signal_detect(const phymod_access_t *pa, int enable); /*force the signal detect */   
err_code_t eagle_phy_2_pmd_loopback_get(const phymod_access_t *pa, uint32_t *enable);   
err_code_t eagle_phy_2_uc_init(const phymod_access_t *pa);
err_code_t eagle_phy_2_pram_flop_set(const phymod_access_t *pa, int value);
err_code_t eagle_phy_2_ucode_init( const phymod_access_t *pa );
err_code_t eagle_phy_2_pmd_ln_h_rstb_pkill_override( const phymod_access_t *pa, uint16_t val); 
err_code_t eagle_phy_2_pmd_cl72_enable_get(const phymod_access_t *pa, uint32_t *enable);
err_code_t eagle_phy_2_pmd_cl72_receiver_status(const phymod_access_t *pa, uint32_t *status);
err_code_t eagle_phy_2_tx_pi_control_get( const phymod_access_t *pa,  int16_t *value);
   

err_code_t eagle_phy_2_tx_rx_polarity_set(const phymod_access_t *pa, uint32_t tx_pol, uint32_t rx_pol);
err_code_t eagle_phy_2_tx_rx_polarity_get(const phymod_access_t *pa, uint32_t *tx_pol, uint32_t *rx_pol);
err_code_t eagle_phy_2_force_tx_set_rst (const phymod_access_t *pa, uint32_t rst);
err_code_t eagle_phy_2_force_tx_get_rst (const phymod_access_t *pa, uint32_t *rst);
err_code_t eagle_phy_2_force_rx_set_rst (const phymod_access_t *pa, uint32_t rst);
err_code_t eagle_phy_2_force_rx_get_rst (const phymod_access_t *pa, uint32_t *rst);
err_code_t eagle_phy_2_pll_mode_get(const phymod_access_t *pa, uint32_t *pll_mode);
err_code_t eagle_phy_2_osr_mode_set(const phymod_access_t *pa, int osr_mode);
err_code_t eagle_phy_2_osr_mode_get(const phymod_access_t *pa, int *osr_mode);
err_code_t eagle_phy_2_dig_lpbk_get(const phymod_access_t *pa, uint32_t *lpbk);
err_code_t eagle_phy_2_rmt_lpbk_get(const phymod_access_t *pa, uint32_t *lpbk);
err_code_t eagle_phy_2_core_soft_reset(const phymod_access_t *pa);
err_code_t eagle_phy_2_pwrdn_set(const phymod_access_t *pa, int tx_rx, int pwrdn);
err_code_t eagle_phy_2_pwrdn_get(const phymod_access_t *pa, power_status_t *pwrdn);
err_code_t eagle_phy_2_pcs_lane_swap_tx(const phymod_access_t *pa, uint32_t tx_lane_map); 
err_code_t eagle_phy_2_pmd_lane_swap (const phymod_access_t *pa, uint32_t lane_map); 
err_code_t eagle_phy_2_pmd_lane_swap_tx_get (const phymod_access_t *pa, uint32_t *lane_map); 
err_code_t eagle_phy_2_lane_hard_soft_reset_release(const phymod_access_t *pa, uint32_t enable); 
err_code_t eagle_phy_2_clause72_control(const phymod_access_t *pa, uint32_t cl_72_en);          
err_code_t eagle_phy_2_clause72_control_get(const phymod_access_t *pa, uint32_t *cl_72_en);    
err_code_t eagle_phy_2_pmd_reset_seq(const phymod_access_t *pa, int pmd_touched);
err_code_t eagle_phy_2_pll_reset_enable_set (const phymod_access_t *pa, int enable);
err_code_t eagle_phy_2_read_pll_range (const phymod_access_t *pa, uint32_t *pll_range);
err_code_t eagle_phy_2_signal_detect (const phymod_access_t *pa, uint32_t *signal_detect);
err_code_t eagle_phy_2_ladder_setting_to_mV(const phymod_access_t *pa, int8_t y, int16_t* level);
err_code_t eagle_phy_2_pll_config_get(const phymod_access_t *pa, enum eagle_phy_2_pll_enum *pll_mode);
err_code_t eagle_phy_2_tx_lane_control_set(const phymod_access_t *pa, uint32_t en);
err_code_t eagle_phy_2_tx_lane_control_get(const phymod_access_t *pa, uint32_t* en);
err_code_t eagle_phy_2_rx_lane_control_set(const phymod_access_t *pa, uint32_t en);
err_code_t eagle_phy_2_rx_lane_control_get(const phymod_access_t *pa, uint32_t* en);
err_code_t eagle_phy_2_get_vco (const phymod_phy_inf_config_t* config, uint32_t *vco_rate, uint32_t *new_pll_div, int16_t *new_os_mode);
err_code_t eagle_phy_2_pmd_tx_disable_pin_dis_set(const phymod_access_t *pa, uint32_t enable);
err_code_t eagle_phy_2_pmd_tx_disable_pin_dis_get(const phymod_access_t *pa, uint32_t *enable);
err_code_t eagle_phy_2_tx_shared_patt_gen_en_get( const phymod_access_t *pa, uint8_t *enable);
err_code_t eagle_phy_2_config_shared_tx_pattern_idx_set( const phymod_access_t *pa, const uint32_t *pattern_len);
err_code_t eagle_phy_2_config_shared_tx_pattern_idx_get( const phymod_access_t *pa, uint32_t *pattern_len, uint32_t *pattern);

err_code_t eagle_phy_2_phy_pcs_speed_get(const phymod_access_t *pa, uint32_t *speed);
err_code_t eagle_phy_2_phy_pcs_speed_setup(const phymod_access_t *pa, eagle_phy_2_actual_speed_id_t speed);
err_code_t eagle_phy_2_phy_change_speed_reset(const phymod_access_t *pa );
err_code_t eagle_phy_2_data_path_reset_release(const phymod_access_t *pa, uint32_t enable);
err_code_t eagle_phy_2_uc_active_check( const phymod_access_t *pa);
err_code_t eagle_phy_2_pmd_setup (const phymod_access_t *pa, int pmd_touched, const phymod_core_init_config_t* init_config);
int eagle_phy_2_heartbeat_count_1us(const phymod_access_t *pa, phymod_ref_clk_t ref_clock);
err_code_t eagle_phy_2_phy_pcs_12p5_vco_setup(const phymod_access_t *pa, uint32_t enable);
err_code_t eagle_phy_2_phy_ref_vco_setup(const phymod_access_t *pa, phymod_ref_clk_t ref_clk, enum eagle_phy_2_phy_vco_enum vco);
err_code_t eagle_phy_2_pmd_setup (const phymod_access_t *pa, int pmd_touched, const phymod_core_init_config_t* init_config);
err_code_t eagle_phy_2_core_init_vco(const phymod_access_t *pa, uint32_t vco_12p5);


#endif /* PHY_TSC_IBLK_H */
