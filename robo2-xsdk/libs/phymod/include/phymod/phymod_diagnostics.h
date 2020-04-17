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

#ifndef _PHYMOD_DIAGNOSTICS_H__H_
#define _PHYMOD_DIAGNOSTICS_H__H_

#include <phymod/phymod_definitions.h>
#include <phymod/phymod.h>
typedef struct phymod_slicer_position_s {
    uint32_t horizontal;
    uint32_t vertical;
} phymod_slicer_position_t;

/* phymod_slicer_position_t initialization and validation */
int phymod_slicer_position_t_validate(const phymod_slicer_position_t* phymod_slicer_position);
int phymod_slicer_position_t_init(phymod_slicer_position_t* phymod_slicer_position);

/*! 
 * @brief Flags for phymod_phy_rx_slicer_position_set/get APIs 
 */ 
#define PHYMOD_SLICER_F_LIVE_LINK 0x1 /**< Use the live-link slicer */

/*! 
 * phymod_phy_rx_slicer_position_set
 *
 * @brief Set/get slicer position 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  flags           - 
 * @param [in]  position        - Slicer location to set
 */
int phymod_phy_rx_slicer_position_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position);
/*! 
 * phymod_phy_rx_slicer_position_get
 *
 * @brief Set/get slicer position 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  flags           - 
 * @param [out]  position        - Current slicer position
 */
int phymod_phy_rx_slicer_position_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_slicer_position_t* position);

/*! 
 * phymod_phy_rx_slicer_position_max_get
 *
 * @brief Get slicer range limitation 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  flags           - 
 * @param [in]  position_min    - Slicer minimum position
 * @param [in]  position_max    - Slicer maxumum position
 */
int phymod_phy_rx_slicer_position_max_get(const phymod_phy_access_t* phy, uint32_t flags, const phymod_slicer_position_t* position_min, const phymod_slicer_position_t* position_max);


/*!
 * @enum phymod_prbs_poly_e
 * @brief PRBS polynomial 
 */ 
typedef enum phymod_prbs_poly_e {
    phymodPrbsPoly7 = 0,
    phymodPrbsPoly9,
    phymodPrbsPoly11,
    phymodPrbsPoly15,
    phymodPrbsPoly23,
    phymodPrbsPoly31,
    phymodPrbsPoly58,
    phymodPrbsPolyCount
} phymod_prbs_poly_t;

#ifdef PHYMOD_DIAG
extern enum_mapping_t phymod_prbs_poly_t_mapping[];
#endif /*PHYMOD_DIAGNOSTICS_DIAG*/

/* phymod_prbs_poly_t validation */
int phymod_prbs_poly_t_validate(phymod_prbs_poly_t phymod_prbs_poly);
/*! 
 * @brief RX/TX selector. no flags means both 
 */ 
#define PHYMOD_PRBS_DIRECTION_RX 0x1 /**< Config RX */
#define PHYMOD_PRBS_DIRECTION_TX 0x2 /**< Config TX */

#define PHYMOD_PRBS_DIRECTION_RX_SET(flags) (flags |= PHYMOD_PRBS_DIRECTION_RX)
#define PHYMOD_PRBS_DIRECTION_TX_SET(flags) (flags |= PHYMOD_PRBS_DIRECTION_TX)

#define PHYMOD_PRBS_DIRECTION_RX_CLR(flags) (flags &= ~PHYMOD_PRBS_DIRECTION_RX)
#define PHYMOD_PRBS_DIRECTION_TX_CLR(flags) (flags &= ~PHYMOD_PRBS_DIRECTION_TX)

#define PHYMOD_PRBS_DIRECTION_RX_GET(flags) (flags & PHYMOD_PRBS_DIRECTION_RX ? 1 : 0)
#define PHYMOD_PRBS_DIRECTION_TX_GET(flags) (flags & PHYMOD_PRBS_DIRECTION_TX ? 1 : 0)


/*!
 * @struct phymod_prbs_s
 * @brief PRBS control 
 */ 
typedef struct phymod_prbs_s {
    phymod_prbs_poly_t poly;
    uint32_t invert;
} phymod_prbs_t;

/* phymod_prbs_t initialization and validation */
int phymod_prbs_t_validate(const phymod_prbs_t* phymod_prbs);
int phymod_prbs_t_init(phymod_prbs_t* phymod_prbs);

/*! 
 * phymod_phy_prbs_config_set
 *
 * @brief set/get PRBS configuration 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  flags           - see PHYMOD_PRBS_DIRECTION_
 * @param [in]  prbs            - PRBS configuration to set
 */
int phymod_phy_prbs_config_set(const phymod_phy_access_t* phy, uint32_t flags , const phymod_prbs_t* prbs);
/*! 
 * phymod_phy_prbs_config_get
 *
 * @brief set/get PRBS configuration 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  flags           - see PHYMOD_PRBS_DIRECTION_
 * @param [out]  prbs            - PRBS configuration
 */
int phymod_phy_prbs_config_get(const phymod_phy_access_t* phy, uint32_t flags , phymod_prbs_t* prbs);

/*! 
 * phymod_phy_prbs_enable_set
 *
 * @brief Set/get PRBS enable state 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  flags           - see PHYMOD_PRBS_DIRECTION_
 * @param [in]  enable          - Enable \ disable PRBS
 */
int phymod_phy_prbs_enable_set(const phymod_phy_access_t* phy, uint32_t flags , uint32_t enable);
/*! 
 * phymod_phy_prbs_enable_get
 *
 * @brief Set/get PRBS enable state 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  flags           - see PHYMOD_PRBS_DIRECTION_
 * @param [out]  enable          - PRBS state
 */
int phymod_phy_prbs_enable_get(const phymod_phy_access_t* phy, uint32_t flags , uint32_t* enable);


/*!
 * @struct phymod_prbs_status_s
 * @brief PRBS control 
 */ 
typedef struct phymod_prbs_status_s {
    uint32_t prbs_lock; /**< Whether PRBS is currently locked */
    uint32_t prbs_lock_loss; /**< Whether PRBS was unlocked since last call */
    uint32_t error_count; /**< PRBS errors count */
} phymod_prbs_status_t;

/* phymod_prbs_status_t initialization and validation */
int phymod_prbs_status_t_validate(const phymod_prbs_status_t* phymod_prbs_status);
int phymod_prbs_status_t_init(phymod_prbs_status_t* phymod_prbs_status);

/*! 
 * @brief Flags for phymod_phy_prbs_status_get API 
 */ 
#define PHYMOD_PRBS_STATUS_F_CLEAR_ON_READ 0x1 /**< Clear error counter on read */

/*! 
 * phymod_phy_prbs_status_get
 *
 * @brief Get PRBS Status 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  flags           - 
 * @param [out]  prbs_status     - PRBS status
 */
int phymod_phy_prbs_status_get(const phymod_phy_access_t* phy, uint32_t flags, phymod_prbs_status_t* prbs_status);

typedef struct phymod_pattern_s {
    uint32_t pattern_len;
    uint32_t* pattern; /**< Pattern to set */
} phymod_pattern_t;

/* phymod_pattern_t initialization and validation */
int phymod_pattern_t_validate(const phymod_pattern_t* phymod_pattern);
int phymod_pattern_t_init(phymod_pattern_t* phymod_pattern);

/*! 
 * phymod_phy_pattern_config_set
 *
 * @brief Set/get pattern state 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  pattern         - Pattern Configuration
 */
int phymod_phy_pattern_config_set(const phymod_phy_access_t* phy, const phymod_pattern_t* pattern);
/*! 
 * phymod_phy_pattern_config_get
 *
 * @brief Set/get pattern state 
 *
 * @param [in]  phy             - phy access information
 * @param [inout] pattern         - Pattern Configuration
 */
int phymod_phy_pattern_config_get(const phymod_phy_access_t* phy, phymod_pattern_t* pattern);

/*! 
 * phymod_phy_pattern_enable_set
 *
 * @brief Set/get pattern state 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  enable          - Enable \ disable pattern send \ receive
 * @param [in]  pattern         - Pattern Configuration
 */
int phymod_phy_pattern_enable_set(const phymod_phy_access_t* phy, uint32_t enable, const phymod_pattern_t* pattern);
/*! 
 * phymod_phy_pattern_enable_get
 *
 * @brief Set/get pattern state 
 *
 * @param [in]  phy             - phy access information
 * @param [out]  enable          - Pattern state
 */
int phymod_phy_pattern_enable_get(const phymod_phy_access_t* phy, uint32_t* enable);

typedef struct phymod_core_diagnostics_s {
    uint32_t temperature;
    uint32_t pll_range;
} phymod_core_diagnostics_t;

/* phymod_core_diagnostics_t initialization and validation */
int phymod_core_diagnostics_t_validate(const phymod_core_diagnostics_t* phymod_core_diagnostics);
int phymod_core_diagnostics_t_init(phymod_core_diagnostics_t* phymod_core_diagnostics);

/*! 
 * phymod_core_diagnostics_get
 *
 * @brief Get core diagnostics information 
 *
 * @param [in]  core            - core access information
 * @param [out]  diag            - Core diagnostics information
 */
int phymod_core_diagnostics_get(const phymod_core_access_t* core, phymod_core_diagnostics_t* diag);

typedef struct phymod_diag_slicer_offset_s {
    uint32_t offset_pe;
    uint32_t offset_ze;
    uint32_t offset_me;
    uint32_t offset_po;
    uint32_t offset_zo;
    uint32_t offset_mo;
} phymod_diag_slicer_offset_t;

/* phymod_diag_slicer_offset_t initialization and validation */
int phymod_diag_slicer_offset_t_validate(const phymod_diag_slicer_offset_t* phymod_diag_slicer_offset);
int phymod_diag_slicer_offset_t_init(phymod_diag_slicer_offset_t* phymod_diag_slicer_offset);

typedef struct phymod_diag_eyescan_s {
    uint32_t heye_left;
    uint32_t heye_right;
    uint32_t veye_upper;
    uint32_t veye_lower;
} phymod_diag_eyescan_t;

/* phymod_diag_eyescan_t initialization and validation */
int phymod_diag_eyescan_t_validate(const phymod_diag_eyescan_t* phymod_diag_eyescan);
int phymod_diag_eyescan_t_init(phymod_diag_eyescan_t* phymod_diag_eyescan);


/*!
 * @enum phymod_pmd_mode_e
 * @brief pmd modes 
 */ 
typedef enum phymod_pmd_mode_e {
    phymodPmdModeOs = 0,
    phymodPmdModeOsDfe,
    phymodPmdModeBrDfe,
    phymodPmdModeCount
} phymod_pmd_mode_t;

#ifdef PHYMOD_DIAG
extern enum_mapping_t phymod_pmd_mode_t_mapping[];
#endif /*PHYMOD_DIAGNOSTICS_DIAG*/

/* phymod_pmd_mode_t validation */
int phymod_pmd_mode_t_validate(phymod_pmd_mode_t phymod_pmd_mode);
typedef struct phymod_phy_diagnostics_s {
    uint32_t signal_detect;
    uint32_t vga_bias_reduced;
    uint32_t postc_metric;
    phymod_osr_mode_t osr_mode;
    phymod_pmd_mode_t pmd_mode;
    uint32_t rx_lock;
    uint32_t rx_ppm;
    uint32_t tx_ppm;
    uint32_t clk90_offset;
    uint32_t clkp1_offset;
    uint32_t p1_lvl;
    uint32_t m1_lvl;
    uint32_t dfe1_dcd;
    uint32_t dfe2_dcd;
    uint32_t slicer_target;
    phymod_diag_slicer_offset_t slicer_offset;
    phymod_diag_eyescan_t eyescan;
    uint32_t state_machine_status;
    uint32_t link_time;
    int pf_main;
    int pf_hiz;
    int pf_bst;
    int pf_low;
    int pf2_ctrl;
    int vga;
    int dc_offset;
    int p1_lvl_ctrl;
    int dfe1;
    int dfe2;
    int dfe3;
    int dfe4;
    int dfe5;
    int dfe6;
    int txfir_pre;
    int txfir_main;
    int txfir_post1;
    int txfir_post2;
    int txfir_post3;
    int tx_amp_ctrl;
    uint8_t br_pd_en;
} phymod_phy_diagnostics_t;

/* phymod_phy_diagnostics_t initialization and validation */
int phymod_phy_diagnostics_t_validate(const phymod_phy_diagnostics_t* phymod_phy_diagnostics);
int phymod_phy_diagnostics_t_init(phymod_phy_diagnostics_t* phymod_phy_diagnostics);

/*! 
 * phymod_phy_diagnostics_get
 *
 * @brief Get phy diagnostics information 
 *
 * @param [in]  phy             - phy access information
 * @param [out]  diag            - 
 */
int phymod_phy_diagnostics_get(const phymod_phy_access_t* phy, phymod_phy_diagnostics_t* diag);

/*! 
 * phymod_phy_pmd_info_dump
 *
 * @brief Get phy diagnostics information 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  type            - 
 */
int phymod_phy_pmd_info_dump(const phymod_phy_access_t* phy, char* type);

/*! 
 * phymod_phy_pcs_info_dump
 *
 * @brief Get phy diagnostics information 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  type            - 
 */
int phymod_phy_pcs_info_dump(const phymod_phy_access_t* phy, char* type);

typedef struct phymod_phy_eyescan_options_s {
    uint32_t linerate_in_khz;
    uint16_t timeout_in_milliseconds;
    int8_t horz_max;
    int8_t horz_min;
    int8_t hstep;
    int8_t vert_max;
    int8_t vert_min;
    int8_t vstep;
    uint8_t ber_proj_scan_mode; /**< For BER projection Controls the direction and polarity of the test. */
    uint8_t ber_proj_timer_cnt; /**< BER proj total measurement time in units of ~1.3 seconds. */
    uint8_t ber_proj_err_cnt; /**< BER proj the error threshold it uses to step to next measurement in units of 16. */
    int8_t mode;
} phymod_phy_eyescan_options_t;

/* phymod_phy_eyescan_options_t initialization and validation */
int phymod_phy_eyescan_options_t_validate(const phymod_phy_eyescan_options_t* phymod_phy_eyescan_options);
int phymod_phy_eyescan_options_t_init(phymod_phy_eyescan_options_t* phymod_phy_eyescan_options);


/*!
 * @enum phymod_eyescan_mode_e
 * @brief Eyescan modes 
 */ 
typedef enum phymod_eyescan_mode_e {
    phymodEyescanModeFast = 0, /**< Fast eyescan resoultion. */
    phymodEyescanModeLowBER, /**< Allow more aquirate BER resolution. */
    phymodEyescanModeBERProj, /**< Do BER projection. */
    phymodEyescanModeCount
} phymod_eyescan_mode_t;

#ifdef PHYMOD_DIAG
extern enum_mapping_t phymod_eyescan_mode_t_mapping[];
#endif /*PHYMOD_DIAGNOSTICS_DIAG*/

/* phymod_eyescan_mode_t validation */
int phymod_eyescan_mode_t_validate(phymod_eyescan_mode_t phymod_eyescan_mode);
/*! 
 * @brief Eyescan flags 
 */ 
#define PHYMOD_EYESCAN_F_ENABLE 0x1 /**< Enable eyescan */
#define PHYMOD_EYESCAN_F_PROCESS 0x2 /**< Process eyescan */
#define PHYMOD_EYESCAN_F_DONE 0x4 /**< Eyescan done */
#define PHYMOD_EYESCAN_F_ENABLE_DONT_WAIT 0x8 /**< Don't wait to finish enable operation */

#define PHYMOD_EYESCAN_F_ENABLE_SET(flags) ((flags) |= PHYMOD_EYESCAN_F_ENABLE)
#define PHYMOD_EYESCAN_F_PROCESS_SET(flags) ((flags) |= PHYMOD_EYESCAN_F_PROCESS)
#define PHYMOD_EYESCAN_F_DONE_SET(flags) ((flags) |= PHYMOD_EYESCAN_F_DONE)
#define PHYMOD_EYESCAN_F_ENABLE_DONT_WAIT_SET(flags) ((flags) |= PHYMOD_EYESCAN_F_ENABLE_DONT_WAIT)

#define PHYMOD_EYESCAN_F_ENABLE_CLR(flags) ((flags) &= ~PHYMOD_EYESCAN_F_ENABLE)
#define PHYMOD_EYESCAN_F_PROCESS_CLR(flags) ((flags) &= ~PHYMOD_EYESCAN_F_PROCESS)
#define PHYMOD_EYESCAN_F_DONE_CLR(flags) ((flags) &= ~PHYMOD_EYESCAN_F_DONE)
#define PHYMOD_EYESCAN_F_ENABLE_DONT_WAIT_CLR(flags) ((flags) &= ~PHYMOD_EYESCAN_F_ENABLE_DONT_WAIT)

#define PHYMOD_EYESCAN_F_ENABLE_GET(flags) ((flags) & PHYMOD_EYESCAN_F_ENABLE ? 1 : 0)
#define PHYMOD_EYESCAN_F_PROCESS_GET(flags) ((flags) & PHYMOD_EYESCAN_F_PROCESS ? 1 : 0)
#define PHYMOD_EYESCAN_F_DONE_GET(flags) ((flags) & PHYMOD_EYESCAN_F_DONE ? 1 : 0)
#define PHYMOD_EYESCAN_F_ENABLE_DONT_WAIT_GET(flags) ((flags) & PHYMOD_EYESCAN_F_ENABLE_DONT_WAIT ? 1 : 0)

/*! 
 * phymod_phy_eyescan_run
 *
 * @brief Display eyescan information 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  flags           - 
 * @param [in]  mode            - Select eyescan mode
 * @param [in]  eyescan_options   - Eyescan options. Set to NULL for eyescan types without options, or to use defaults.
 */
int phymod_phy_eyescan_run(const phymod_phy_access_t* phy, uint32_t flags, phymod_eyescan_mode_t mode, const phymod_phy_eyescan_options_t* eyescan_options);


/*!
 * @enum phymod_link_monitor_mode_e
 * @brief Link monitor modes 
 */ 
typedef enum phymod_link_monitor_mode_e {
    phymodLinkMonPCS49_1x10G = 0,
    phymodLinkMonPCS82_4x10G,
    phymodLinkMonPCS82_2x25G,
    phymodLinkMonPCS82_4x25G,
    phymodLinkMonFC4,
    phymodLinkMonFC8,
    phymodLinkMonFC16,
    phymodLinkMonFC32,
    phymodLinkMonCount
} phymod_link_monitor_mode_t;

#ifdef PHYMOD_DIAG
extern enum_mapping_t phymod_link_monitor_mode_t_mapping[];
#endif /*PHYMOD_DIAGNOSTICS_DIAG*/

/* phymod_link_monitor_mode_t validation */
int phymod_link_monitor_mode_t_validate(phymod_link_monitor_mode_t phymod_link_monitor_mode);
/*! 
 * phymod_phy_link_mon_enable_set
 *
 * @brief Get/Set link monitor enable/disable 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  link_mon_mode   - 
 * @param [in]  enable          - 
 */
int phymod_phy_link_mon_enable_set(const phymod_phy_access_t* phy, phymod_link_monitor_mode_t link_mon_mode, uint32_t enable);
/*! 
 * phymod_phy_link_mon_enable_get
 *
 * @brief Get/Set link monitor enable/disable 
 *
 * @param [in]  phy             - phy access information
 * @param [in]  link_mon_mode   - 
 * @param [out]  enable          - 
 */
int phymod_phy_link_mon_enable_get(const phymod_phy_access_t* phy, phymod_link_monitor_mode_t link_mon_mode, uint32_t* enable);

/*! 
 * phymod_phy_link_mon_status_get
 *
 * @brief Get link monitor Status 
 *
 * @param [in]  phy             - phy access information
 * @param [out]  lock_status     - link mon status
 * @param [out]  lock_lost_lh    - Whether link mon was unlocked since last call
 * @param [out]  error_count     - link mon error count
 */
int phymod_phy_link_mon_status_get(const phymod_phy_access_t* phy, uint32_t* lock_status, uint32_t* lock_lost_lh, uint32_t* error_count);

/*! 
 * phymod_phy_fec_correctable_counter_get
 *
 * @brief Get FEC Corrected Codeword Counter 
 *
 * @param [in]  phy             - phy access information
 * @param [out]  count           - FEC corrected codeword counter
 */
int phymod_phy_fec_correctable_counter_get(const phymod_phy_access_t* phy, uint32_t* count);

/*! 
 * phymod_phy_fec_uncorrectable_counter_get
 *
 * @brief Get FEC Uncorrected Codeword Counter 
 *
 * @param [in]  phy             - phy access information
 * @param [out]  count           - FEC uncorrected codeword counter
 */
int phymod_phy_fec_uncorrectable_counter_get(const phymod_phy_access_t* phy, uint32_t* count);

#endif /*_PHYMOD_DIAGNOSTICS_H_*/