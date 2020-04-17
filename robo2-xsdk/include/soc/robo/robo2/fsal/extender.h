/*
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/**
 * @file
 * Coronado Bridge
 * Forwarding Services Abstraction Layer (FSAL) API
 */

#ifndef CBX_API_EXTENDER_H_
#define CBX_API_EXTENDER_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup cbx_extender CBX Extender
 * Forwarding Services Abstraction Layer (FSAL) Port Extender operations.
 *
 * <h3>Port Extender initialization</h3>
 *
 * Enable SLIC rules:
 * - Upstream Port:  E-TAG required
 *                   forwarding ARL lookup on e_cid field
 *                   HDT strip E_TAG
 * - Cascade Port:   E-TAG required
 *                   forwarding rule SLI->DLI
 *                   HDT keep E_TAG
 * - Extender Port:  no E-TAG
 *                   forwarding rule SLI->DLI
 *
 * DLI usage:
 * - Upstream Port:  DLI with no egress editing (packets from Cascade Ports)
 *                   DLI per local Extender Ports (upto 32)
 *                   - adds E_TAG with ingress_e_cid per DLI
 * - Cascade Port:   DLI per remote Extender Ports (upto 32)
 *                   - adds E_TAG with e_cid per DLI
 *                     created when e_cid is added to ARL with Dest=Cascade Port
 * - Extender Port:  DLI with no egress editing
 *
 *
 *
 *  @{
 */

/**
 * @typedef cbx_extender_t
 * Port Extender Identifier
 */

/**
 * Initializes the Port Extender subsystem
 * - setup of Port Extender module
 *
 * @param void
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_extender_init ( void );


/**
 * Detach Port Extender previously initialized by cbx_extender_init()
 * - shut down the Port Extender subsystem
 *
 * @param void
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_extender_detach ( void );




/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif
