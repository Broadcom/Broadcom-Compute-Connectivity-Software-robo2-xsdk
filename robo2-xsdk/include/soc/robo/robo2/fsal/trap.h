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

#ifndef CBX_API_TRAP_H_
#define CBX_API_TRAP_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup cbx_trap CBX TRAP
 * Forwarding Services Abstraction Layer (FSAL) Trap operations.
 *
 * <h3>Trap Programming</h3>
 * Avenger implemenation details
 *
 *  Calling sequence:
 *   - cbx_trap_dest_create()   returns trap_destid
 *   - cbx_trap_group_create()  returns trap_groupid
 *   - cbx_trap_set()
 *
 * <h3>Mirror Trap Group Table (MTGT)</h3>
 *     DLI[17:0]
 *         Destination logical interface used for packet
 *     TC[2:0]
 *         Traffic class used for frame
 *     DP[1:0]
 *         Discard Precedence used for frame
 *     TRUNC
 *         If set, truncate trap copy of the frame
 *     SAMPLE[9:0]
 *         Sample probability: 0x0 = no sampling; always send packet
 *
 * <h3>Trap Control Table (TCT)</h3>
 *    DROP
 *        1  drop forwarding copy of frame
 *        0  Forward frame as usual
 *    TRAP_GROUP
 *        Specifies one of 15 trap groups to use (trap group = 0 is do not trap)
 *    TERM
 *        Terminate frame. If set no further processing of frame will be done (i.e. CFP)
 *    HIT
 *        Set when trap condition was encountered. This bit is write-one-to-clear.
 *
 * <h3>Traped Packets</h3>
 * Traping sends a copy of the frame as it was received on
 * the ingress port (prior to any edits) to a specific destination.
 *
 * The frame or a portion of the frame maybe encapsulated in another header.
 *
 * A frame may be traped based on:
 * - MAC
 * - TLV/TAG Parser
 * - SLIC
 * - Deep Frame Parser
 * - CFP match includes a 6bit trap field
 *
 * The traps have a unique Trap_ID.
 * The Trap_ID indexes the Trap Control Table (TCT)
 *
 * The TCT provides the Drop and Terminate control and the Trap Group_ID
 * The Trap Group_ID indexes the Mirror Trap Group Table (MTGT)
 *
 * The MTGT provides the TC, DP, Truncate, and Sample parameters for the trap.
 * The MTGT also provides the Destination Logical Interface (DLI) to uncast the packet.
 * This separate DLI (always N type) is used from the normal switched packet.
 *
 *  @{
 */

/**
 * @typedef cbx_trap_groupid_t
 * Trap Group Identifier
 */
typedef uint32_t cbx_trap_groupid_t;

/**
 * @def CBX_TRAP_GROUP_INVALID
 * Invalid TRAP GROUP
 */
#define CBX_TRAP_GROUP_INVALID   0xffffffff

/** @def CBX_TRAP_GROUP_MAX
 * Maximum Trap Group entires
 */
#define CBX_TRAP_GROUP_MAX  16

/**
 * @typedef cbx_trap_destid_t
 * Trap Destination Identifier
 */
typedef uint32_t cbx_trap_destid_t;

/**
 * @def CBX_TRAP_DEST_INVALID
 * Invalid TRAP Destination
 */
#define CBX_TRAP_DEST_INVALID   0xffffffff

/** Maximum Trap Destination entires  */
#define CBX_TRAP_DEST_MAX  16

/** Trap Destination Flags */
#define  CBX_TRAP_DEST_ENCAP        0x00000001

/**
 * @typedef cbx_trap_dest_t
 * Trap Destination struct
 * information about a trap destination
 */
typedef struct cbx_trap_dest_t {
    uint32_t         flags;          /**< Flags:  CBX_TRAP_DEST_XX */
    cbx_portid_t     port;           /**< Trap destination. */
    eth_mac_t        src_mac;        /**< encap L2 source MAC address. */
    eth_mac_t        dst_mac;        /**< encap L2 destination MAC address. */
    cbx_vlan_t       vlan_id;        /**< encap VLAN ID. */
    cbx_encap_info_t encap;          /**< L3/SPAN/ERSPAN encapsulation */
} cbx_trap_dest_t;

/**
 * Initialize trap destination parameters.
 *
 * @param trap_dest  trap destination parameter object
 */
void cbx_trap_dest_t_init( cbx_trap_dest_t *trap_dest );

/** @{
 * Trap Group Flags
 */
#define  CBX_TRAP_GROUP_TRUNCATE            0x00000001
/** @} */

/**
 * Trap Group struct
 * information about a trap group
 */
typedef struct cbx_trap_group_params_t {
    uint32_t         flags;         /**< Flags:  CBX_TRAP_GROUP_XX */
    int              trap_tc;       /**< TC to use for traping */
    int              trap_dp;       /**< DP to use for traping */
    uint32_t         sample_rate;   /**< sampling parameter */
} cbx_trap_group_params_t;

/**
 * Initialize trap group parameters.
 *
 * @param trap_group_params trap group parameter object
 */
void cbx_trap_group_params_t_init( cbx_trap_group_params_t *trap_group_params );

/** @{
 * Trap Flags
 */
#define  CBX_TRAP_DROP                0x00000001
#define  CBX_TRAP_TERMINATE           0x00000002
#define  CBX_TRAP_CLEAR_HIT           0x00000004
/** @} */

/**
 * Trap struct
 * information about a trap
 */
typedef struct cbx_trap_params_t {
    uint32_t         flags;           /**< Flags:  CBX_TRAP_XX */
    cbx_trap_groupid_t  trap_groupid; /**< Trap Group Identifier */
    int                 hit;          /**< Trap hit (read-only) */
} cbx_trap_params_t;

/**
 * Initialize trap parameters.
 *
 * @param trap_params trap parameter object
 */
void cbx_trap_params_t_init( cbx_trap_params_t *trap_params );

/**
 * Create a trap destination
 * This routine is used to create a trap destination.
 *
 * @param trap_dest   (IN)  Trap destination parameters.
 * @param trap_destid (OUT) Handle of the trap destination created
 *                      CBX_TRAP_DEST_INVALID: Trap destination could not be created
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_trap_dest_create ( cbx_trap_dest_t   *trap_dest,
                           cbx_trap_destid_t *trap_destid );


/**
 * Destroy a trap destination previously created by cbx_trap_dest_create()
 *
 * @param trap_destid    (IN)  Handle of the trap destination to be destroyed
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously created trap destination.
 */

int cbx_trap_dest_destroy ( cbx_trap_destid_t trap_destid );


/**
 * Create a trap group
 * This routine is used to create a trap group.
 *
 * @param trap_group_params (IN)  Trap parameters.
 * @param trap_destid       (IN)  Trap destination identifier
 * @param trap_groupid      (OUT) Handle of the trap group created
 *                            CBX_TRAP_INVALID: Trap could not be created
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_trap_group_create ( cbx_trap_group_params_t *trap_group_params,
                            cbx_trap_dest_t          trap_destid,
                            cbx_trap_groupid_t      *trap_groupid );


/**
 * Destroy a trap group previously created by cbx_trap_create()
 *
 * @param trap_groupid    (IN)  Handle of the trap group to be destroyed
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously created trap group.
 */

int cbx_trap_group_destroy ( cbx_trap_groupid_t trap_groupid );

/**
 * Trap set
 * This routine configures a Trap
 *
 * @param trap          (IN)  Trap Enumeration
 * @param trap_params   (IN)  Trap port parameters
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_trap_set ( cbx_trap_t         trap,
                   cbx_trap_params_t *trap_params );

/**
 * Trap get
 * This routine gets the configuration of a Trap
 *
 * @param trap          (IN)  Trap Enumeration
 * @param trap_params   (IN)  Trap port parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_trap_get ( cbx_trap_t         trap,
                   cbx_trap_params_t *trap_params );


/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif
