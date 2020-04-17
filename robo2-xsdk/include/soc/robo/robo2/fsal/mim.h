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
 * CBX MAC-in-MAC
 */

#ifndef CBX_API_MIM_H_
#define CBX_API_MIM_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup cbx_mim CBX MiM
 * Forwarding Services Abstraction Layer (FSAL) MAC-in-MAC operations.
 *
 * MAC-in-MAC is a logical interface that can be added to a VLAN.
 *
 * A MAC-in-MAC interface consists of ingress and egress parameters
 * - ingress parameters are the "match" attributes
 *   these are used to associate a packet with an MAC-in-MAC interface
 * - egress parameters provide the editing actions on outgoing packets.
 *
 *
 *  Calling sequence:
 *  VLAN = Provider Backbone Bridging (PBB):
 *  1. Create vlan
 *     - Call vlan_create()
 *         Hardware: VSIT
 *  2. Create mac-in-mac interfaces
 *     - Call mim_create()
 *  3. Add mac-in-mac interfaces to vlan
 *     - Call vlan_port_add()
 *  4. Remove interfaces from vlan
 *     - Call vlan_port_delete()
 *  5. Destroy the mac-in-mac interfaces
 *     - Call mim_destroy()
 *  6. Destory the vlan
 *     - Call vlan_destory()
 *
 *  @{
 */


/**
 * @def CBX_MIM_INVALID
 * Invalid MAC-in-MAC
 */
#define CBX_MIM_INVALID   0xffffffff

/** @def CBX_MIM_MAX
 * Maximum MAC-in-MAC entries
 */
#define CBX_MIM_MAX  2048

/** MIM Interface Match mode
 */
typedef enum cbx_mim_match_e {
    cbxMimMatchNormal,
} cbx_mim_match_t;


/** MAC-in-MAC Flags */
#define  CBX_MIM_ENABLE        0x00000001

/**
 * MAC-in-MAC parameters struct
 *  information about a MAC-in-MAC interface
 */
typedef struct cbx_mim_params_t {
    uint32_t     flags;             /**< Flags:  CBX_MIM_XX */
    cbx_mim_match_t criteria;       /**< Match criteria */
    int          default_tc;        /**< default traffic class */
    int          default_dp;        /**< default drop presedence */
    cbx_portid_t portid;            /**< Physical Port or Port Group */

    uint32_t     isid;                /**< I-SID value */
    cbx_vlan_t   match_vlan;          /**< Outer vlan tag to match (S-Tag) */
    cbx_vlan_t   match_inner_vlan;    /**< Inner vlan tag to match (C-Tag) */
    cbx_vlan_t   match_tunnel_vlan;   /**< B-domain vlan tag to match */
    eth_mac_t    match_tunnel_srcmac; /**< Source MAC address. */

    cbx_vlan_t   egress_tunnel_vlan;   /**< Egress B-domain vlan tag */
    eth_mac_t    egress_tunnel_srcmac; /**< Egress Source MAC address */
    eth_mac_t    egress_tunnel_dstmac; /**< Egress Destination MAC address */
    uint32_t     egress_tunnel_isid;   /**< Egress I-SID translation. */
    cbx_vlan_t   egress_vlan;          /**< Egress Outer vlan tag */
    cbx_vlan_t   egress_inner_vlan;    /**< Egress Inner vlan tag */
    int          mtu;                  /**< MTU for egress logical port */
} cbx_mim_params_t;

/**
 * Initialize mac-in-mac interface entry parameters.
 *
 * @param  mim_params  parameter object
 */
void cbx_mim_params_t_init( cbx_mim_params_t *mim_params );


/**
 * Create a mim
 * This routine is used to create a MAC-in-MAC (mim)
 *
 * @param mim_params  (IN)  mim parameters.
 * @param mimid       (OUT) Handle of the mim created
 *                         CBX_MIM_INVALID  mim could not be created
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_mim_create ( cbx_mim_params_t *mim_params,
                     cbx_portid_t     *mimid );

/**
 * Destroy a mac-in-mac interface previously created by cbx_mim_create()
 *
 * @param mimid    Handle of the mim to be destroyed
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously created mac-in-mac interface.
 */

int cbx_mim_destroy ( cbx_portid_t mimid );

/**
 * Find a mac-in-mac interface previously created by cbx_mim_create()
 *  based on mim parameter match criteria
 *
 * @param mim_params    (IN)  mim parameters to match.
 * @param mimid         (OUT) Handle of the mim found
 *                           CBX_MIM_INVALID if not found
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 */

int cbx_mim_find ( cbx_mim_params_t *mim_params,
                   cbx_portid_t     *mimid );


/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif
