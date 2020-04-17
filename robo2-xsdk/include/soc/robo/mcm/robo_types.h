/*
 * $Id:$
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Filename:
 *    robo_types.h
 * Description:
 *    File defines the various avenger datatypes and operations
 */

#ifndef __ROBO_TYPES_H
#define __ROBO_TYPES_H

#include <assert.h>

/*
 * Cascaded Mode
  */
#define SOC_ROBO2_CASCADED_ADDR (1 << 26)
#define SOC_IS_CASCADED(unit) (SOC_INFO(unit).cascaded_mode == 1)
#define drv_robo2_map_to_device_addr(unit, addr)  \
               ((SOC_IS_CASCADED(0) && (unit > 0)) ? (addr | SOC_ROBO2_CASCADED_ADDR) : \
                                      (addr & ~(SOC_ROBO2_CASCADED_ADDR)))

/* Block base address, stored in schan field to avoid additional
 * base field in soc_block_info_t structure. Done for comapatibility
 * with soc_block_info_t structure of XGS SDK code.
 */
#define SOC_BLOCK_BASE_ADDR(blk)         (blk->schan)

#define SAL_ASSERT  assert

/**
 * Table Operations
 *  Various supported Table Operations
 */
typedef enum {
     ROBO2_IA_OP_NOP              = 0x0,
     ROBO2_IA_OP_TABLE_READ       = 0x1,
     ROBO2_IA_OP_TABLE_WRITE      = 0x2,
     ROBO2_IA_OP_TABLE_ENABLE     = 0x3,  /**< Out of low power mode */
     ROBO2_IA_OP_TABLE_DISABLE    = 0x4,  /**< Put table in low power mode */
     ROBO2_IA_OP_TABLE_RESET      = 0x5,  /**< Reset table entries to deflt */
     ROBO2_IA_OP_TABLE_INIT       = 0x6,  /**< Init table with given data */
     ROBO2_IA_OP_TABLE_INIT_INC   = 0x7,  /**< Init table, inc 20b wide data */
     ROBO2_IA_OP_TABLE_INIT_START = 0x8,  /**< Set start index for init operation */
     ROBO2_IA_OP_ACK_ERROR        = 0xF,  /**< Clear error condition */
} soc_robo2_ia_operation_t;

/**
 * Table Operation Status
 *  The result of any indirect access operation 
 */
typedef enum {
    ROBO2_IA_STATUS_READY        = 0x0, /**< HW is ready to accept a IA command */ 
    ROBO2_IA_STATUS_BUSY         = 0x1, /**< HW is busy with the current command */
    ROBO2_IA_STATUS_TIMEOUT      = 0x2, /**< Current Command timed out */
    ROBO2_IA_STATUS_OVERRUN      = 0x3, /**< Command overrun, was issued when HW not ready */
    ROBO2_IA_STATUS_BAD_OFFSET   = 0x4, /**< Attemted to access illegal offset (NXM) */
    ROBO2_IA_STATUS_BAD_OPCODE   = 0x5, /**< Command opcode is not supported  (UOP) */
    ROBO2_IA_STATUS_POWERED_DOWN = 0x7, /**< Memory is in power down state */
} soc_robo2_table_ia_op_status_t;

/**
 * Destination Type
 *  Valid values for the DST_TYPE in arl fm table
 */
typedef enum {
   ARLFM_DST_IS_INVALID   = 0, /**< Invalid DST */
   ARLFM_DST_IS_MULTICAST = 1, /**< DST is Destination Group Identifier */
   ARLFM_DST_IS_N         = 2, /**< DST is N field of an N-type LI */
   ARLFM_DST_IS_DPG       = 3, /**< DST is Destination Port Group */
} arlfm_dst_type_e;

/**
 * Update Control
 *   Determines when the associated ARL entry is updated
 */
typedef enum {

  /**
   * When station movement is detected update the ARL entry's associated
   * destination data.
   */
  ARLFM_UPDATE_CTRL_UPDATE          = 0,

  /**
   * When movement is detected update the ARL entry and copy to the CPU.
   */
  ARLFM_UPDATE_CTRL_UPDATE_AND_COPY = 1,

  /**
   * When movement is detected don't update the ARL entry but do copy
   * the frame to the CPU.
   */
  ARLFM_UPDATE_CTRL_COPY            = 2,

  /**
   * Do not allow ARL entry update.
   */
  ARLFM_UPDATE_CTRL_DISABLE_UPDATE  = 3,

} arlfm_upd_ctrl_e;


/**
 * Forwarding Control
 *  Valid values for the Forwarding Control in arl fm table
 */
typedef enum {
    /**
     * Standard forwarding. When an SMAC lookup reveals a unicast entry
     * and the associated data provided for learning indicates that the
     * current SLI is not equal to the associated DLI the associated DLI will
     * be updated if and only if the fwd_ctrl field is set to PERMIT_SMAC
     * regardless of the entry being static.
     */
    ARLFM_FWD_CTRL_PERMIT               = 0,

    /**
     * If this is returned as the result of an SMAC lookup then the
     * frame will be discarded. Note that denying DMACs can be accomplished
     * by setting the dst_type field to DST_IS_INVALID in ARLFM0. If this is 
     * returned as the result of a DMAC lookup then the frame will be
     * forwarded normally
     */
    ARLFM_FWD_CTRL_DENY_SMAC            = 1,

    /**
     * Standard forwarding and unconditionally generate an additional
     * copy for the Control Plane.
     */
    ARLFM_FWD_CTRL_COPY_TO_CPU          = 2,

    /** 
     * Deny and unconditionally generate an additional copy for the 
     * Control Plane.
     */
    ARLFM_FWD_DENY_SMAC_AND_COPY_TO_CPU = 3,

} arlfm_fwd_ctrl_e;

/**
 * Validity of ARL FM entry
 */
typedef enum {
  /**
   * Indicates that the hash table entry is empty.
   */
   ARLFM_EMPTY    = 0,

  /**
   * Indicates the the hash table entry has been learned by hardware
   * but not sanctioned by software, eligible for aging. Counts as a miss
   * for forwarding purposes but suppresses continued notification of
   * software.
   */
   ARLFM_PENDING  = 1,

  /**
   * Indicates that this is a valid entry.
   */
   ARLFM_VALID    = 2,

  /**
   * Indicates that the entry belongs wholly to software, hardware
   * may not age it. Hardware updates the associated data
   * when a station move is detected if and only if fwd_ctrl is PERMIT.
   * Hardware will not update the associated data in any other case.
   */
   ARLFM_STATIC   = 3,

} arlfm_entry_valid_e;

/* These redefinitions are required after unifying the allenum header file
 * and other auto generated header files between avenger and other socs
 */
#define ROBO2_REG_READ                  ROBO_REG_READ
#define ROBO2_REG_WRITE                 ROBO_REG_WRITE
#define ROBO2_REG_READ_WITH_ID          ROBO_REG_READ_WITH_ID
#define ROBO2_REG_WRITE_WITH_ID         ROBO_REG_WRITE_WITH_ID
#define SOC_ROBO2_REG_NAME              SOC_ROBO_REG_NAME
#define SOC_ROBO2_MAX_NUM_BLKS          SOC_ROBO_MAX_NUM_BLKS

#define soc_robo2_reg_int_t             soc_robo_reg_int_t
#define soc_robo2_reg_name              soc_robo_reg_name
#define soc_robo2_base_driver_table     soc_robo_base_driver_table

#define NUM_SOC_ROBO2_REG               NUM_SOC_ROBO_REG
#define XLMAC_CTRLr_ROBO2               XLMAC_CTRLr_ROBO

/* ************* OTP flags *************** */
#define SOC_OTP_FLAG_GPHY_1G_DIS        0x0001
#define SOC_OTP_FLAG_RGMII_DIS          0x0002
#define SOC_OTP_FLAG_GPHY_0_3_DIS       0x0004
#define SOC_OTP_FLAG_GPHY_4_7_DIS       0x0008
#define SOC_OTP_FLAG_XFI_13_DIS         0x0010
#define SOC_OTP_FLAG_XFI_12_DIS         0x0020
#define SOC_OTP_FLAG_QSG_MODE_QSG       0x0040
#define SOC_OTP_FLAG_QSG_MODE_SGM       0x0080
#define SOC_OTP_FLAG_QSGMII_DIS         0x0100
#define SOC_OTP_FLAG_SGMII_2G5_DIS      0x0200
#define SOC_OTP_FLAG_SGMII_1G_DIS       0x0400
#define SOC_OTP_FLAG_ARL_SIZE_8K        0x0800
#define SOC_OTP_FLAG_CFP_DIS            0x1000
#define SOC_OTP_FLAG_XFI_13_MAX_2G5     0x2000
#define SOC_OTP_FLAG_XFI_12_MAX_2G5     0x4000
#define SOC_OTP_FLAG_XLPORT_DISABLE     0x8000
#define SOC_OTP_FLAG_PORT_9_DISABLE     0x10000


#endif /* ROBO_TYPES_H */
