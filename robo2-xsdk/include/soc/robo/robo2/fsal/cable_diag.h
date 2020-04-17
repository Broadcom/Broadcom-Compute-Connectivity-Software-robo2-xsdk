/*
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/**
 * @file
 * Forwarding Services Abstraction Layer (FSAL) API
 */

#ifndef CBX_API_CDIAG_H_
#define CBX_API_CDIAG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define PHY_REG_READ_WRITE_TIMEOUT_COUNT 100
/* Cable diag phy register definition */
#define PHY_DATA_REG            0x15
#define PHY_ADDR_REG            0x17
/* Registers for RDB access method */
#define RDB_EN_REG              0x7E
#define RDB_DATA_REG            0x1F
#define RDB_ADDR_REG            0x1E
#define ECD_CONTROL_REG         0x2A0
#define ECD_FAULT_REG           0x2A1
#define ECD_PAIR_A_LEN_REG      0x2A2
#define ECD_PAIR_B_LEN_REG      0x2A3
#define ECD_PAIR_C_LEN_REG      0x2A4
#define ECD_PAIR_D_LEN_REG      0x2A5
/* ECD_CTRL is 0x9400 to ensure cable diag executed immediately as per phy team input */
#define  ECD_CTRL             0x9400 /* Length in meters */

typedef enum cbx_cdiag_result_e {
    cbxCdiagResultNill         = 0,
    cbxCdiagResultBusy         = 1,
    cbxCdiagResultInvalid      = 2,
    cbxCdiagResultPass         = 3,
    cbxCdiagResultFail         = 4
} cbx_cdiag_result_t;

typedef enum cbx_cdiag_state_e {
    cbxCableStateInvalid       = 0,
    cbxCableStateOK            = 1,
    cbxCableStateOpen          = 2,
    cbxCableStateIntraShort    = 3,
    cbxCableStateInterShort    = 4,
    cbxCableStateBusy          = 5
} cbx_cdiag_state_t;

typedef struct cbx_port_cable_diag_s {
    cbx_cdiag_result_t         port_cdiag_result;
    cbx_cdiag_state_t          pair_state[4];
    uint32_t                   pair_len[4];
} cbx_port_cable_diag_t;



/**
 * Run cable diag on given port.
 *
 * @param portid       (IN)  portid on which Cable diag has to run
 *                           This always represent physical port.
 * @param port_result  (OUT) Port cable diag result
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_port_cable_diag (
                cbx_portid_t portid, cbx_port_cable_diag_t *port_result);

/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif
