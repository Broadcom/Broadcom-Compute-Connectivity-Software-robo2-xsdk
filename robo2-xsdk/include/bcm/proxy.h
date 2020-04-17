/*
 * $Id: $
 * 
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * 
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated.
 * Edits to this file will be lost when it is regenerated.
 */

#ifndef __BCM_PROXY_H__
#define __BCM_PROXY_H__

#if defined(INCLUDE_L3)

#include <bcm/types.h>
#include <bcm/port.h>
#include <bcm/field.h>

/* Proxy server operational modes. */
typedef enum bcm_proxy_mode_e {
    BCM_PROXY_MODE_NONE = 0,        /* Invalid. */
    BCM_PROXY_MODE_LOOPBACK = 1, 
    BCM_PROXY_MODE_HIGIG = 2, 
    BCM_PROXY_MODE_HYBRID = 3, 
    BCM_PROXY_MODE_SECOND_PASS = 4 
} bcm_proxy_mode_t;

/* bcm_proxy_proto_type_s */
typedef enum bcm_proxy_proto_type_e {
    BCM_PROXY_PROTO_INVALID = 0, 
    BCM_PROXY_PROTO_IP4_ALL = 1, 
    BCM_PROXY_PROTO_IP4_UCAST = 2, 
    BCM_PROXY_PROTO_IP4_MCAST = 3, 
    BCM_PROXY_PROTO_IP6_ALL = 4, 
    BCM_PROXY_PROTO_IP6_UCAST = 5, 
    BCM_PROXY_PROTO_IP6_MCAST = 6, 
    BCM_PROXY_PROTO_MPLS_UCAST = 7, 
    BCM_PROXY_PROTO_MPLS_MCAST = 8, 
    BCM_PROXY_PROTO_MPLS_ALL = 9, 
    BCM_PROXY_PROTO_IP6_IN_IP4 = 10, 
    BCM_PROXY_PROTO_IP_IN_IP = 11,      /* IPv4-in-IPv4. */
    BCM_PROXY_PROTO_GRE_IN_IP = 12, 
    BCM_PROXY_PROTO_UNKNOWN_IP4_UCAST = 13, /* IPv4 table expansion. */
    BCM_PROXY_PROTO_UNKNOWN_IP6_UCAST = 14 /* IPv6 table expansion. */
} bcm_proxy_proto_type_t;

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Configure a proxy client for the given unit and port. The server
 * module id and port must be valid.
 */
extern int bcm_proxy_client_set(
    int unit, 
    bcm_port_t client_port, 
    bcm_proxy_proto_type_t proto_type, 
    bcm_module_t server_modid, 
    bcm_port_t server_port, 
    int enable);

/* 
 * This API enables various kinds of lookups on XGS3 device on behalf of
 * remote legacy or XGS3 devices.
 */
extern int bcm_proxy_server_set(
    int unit, 
    bcm_port_t server_port, 
    bcm_proxy_mode_t mode, 
    int enable);

/* 
 * Get the lookup status (enabled/disabled) of XGS3 Proxy server device,
 * given the Server mode.
 */
extern int bcm_proxy_server_get(
    int unit, 
    bcm_port_t server_port, 
    bcm_proxy_mode_t mode, 
    int *enable);

#endif /* BCM_HIDE_DISPATCHABLE */

/* Proxy Server Definitions. */
#define BCM_PROXY_SERVER_MASK_PORT      0x0001     /* Wildcard server
                                                      destination port match */
#define BCM_PROXY_SERVER_KEEP_PRIORITY  0x0002     /* Keep the original packet
                                                      priority */
#define BCM_PROXY_SERVER_KEEP_VID       0x0004     /* Keep the original packet
                                                      VID */

/* The second pass flow types */
typedef enum bcm_proxy_second_pass_flow_type_e {
    bcmProxySecondPassFlowTypeNone = 0, 
    bcmProxySecondPassFlowTypeL2greTermination = 1, 
    bcmProxySecondPassFlowTypeVxlanTermination = 2, 
    bcmProxySecondPassFlowTypeMimTermination = 3, 
    bcmProxySecondPassFlowTypeTrillInitiation = 4, 
    bcmProxySecondPassFlowTypeTrillTermination = 5 
} bcm_proxy_second_pass_flow_type_t;

/* Proxy server Config Structure. */
typedef struct bcm_proxy_server_s {
    uint32 flags;                       /* BCM_PROXY_SERVER_xxx. */
    bcm_gport_t server_gport;           /* Proxy GPORT identifier. */
    bcm_proxy_mode_t mode;              /* Selected proxy server operational
                                           mode. */
    bcm_proxy_second_pass_flow_type_t flow_type; /* Flow type identifier. */
} bcm_proxy_server_t;

/* bcm_proxy_server_t_init */
extern void bcm_proxy_server_t_init(
    bcm_proxy_server_t *proxy_server);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * This API enables various kinds of lookups on XGS3 device on behalf of
 * remote legacy or XGS3 devices on a per ingress port basis.  The local
 * ingress port is expected to be a Higig stack port.  The server_gport
 * in the proxy_server structure is the target BCM_GPORT_PROXY
 * (modid,port) destination which indicates that hybrid proxy lookup is
 * required.
 */
extern int bcm_proxy_server_port_set(
    int unit, 
    bcm_gport_t local_ingress_port, 
    bcm_proxy_server_t *proxy_server, 
    int enable);

/* 
 * This API retrieves the current proxy server configuration on a given
 * Higig stack port, if any.
 */
extern int bcm_proxy_server_port_get(
    int unit, 
    bcm_gport_t local_ingress_port, 
    bcm_proxy_server_t *proxy_server, 
    int *enable);

/* Initialize the BCM Proxy subsystem. */
extern int bcm_proxy_init(
    int unit);

/* Detach the BCM Proxy subsystem. */
extern int bcm_proxy_cleanup(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

/*  Proxy Egress flags. */
#define BCM_PROXY_WITH_ID       (1 << 0)   /* Create Proxy Entry WITH_ID. */
#define BCM_PROXY_REPLACE       (1 << 1)   /* Replace Existing Proxy Entry */
#define BCM_PROXY_L3_FORCE      (1 << 2)   /* Explicitly Set Higig L3-bit */
#define BCM_PROXY_LEARN_DISABLE (1 << 3)   /* Explicitly Set Higig Do-Not-Learn
                                              Bit */
#define BCM_PROXY_RSVD_VLAN_ADD (1 << 4)   /* Explicitly Set Higig System
                                              Reserved Vlan */
#define BCM_PROXY_PACKET_MODIFY (1 << 5)   /* Permit packet modification on
                                              Stack port */
#define BCM_PROXY_PACKET_DROP   (1 << 6)   /* Drop Higig Proxy Packet */

/* Proxy Forwarding destination. */
typedef struct bcm_proxy_egress_s {
    uint32 flags;       /* Proxy flags. */
    bcm_gport_t gport;  /* Stack port to direct the packet */
} bcm_proxy_egress_t;

/* bcm_proxy_egress_traverse_cb */
typedef int (*bcm_proxy_egress_traverse_cb)(
    int unit, 
    bcm_if_t proxy_if_id, 
    bcm_proxy_egress_t *info, 
    void *user_data);

/* Initialize proxy_egress_t. */
extern void bcm_proxy_egress_t_init(
    bcm_proxy_egress_t *proxy_egress);

#ifndef BCM_HIDE_DISPATCHABLE

/* Create Proxy forwarding destination. */
extern int bcm_proxy_egress_create(
    int unit, 
    uint32 flags, 
    bcm_proxy_egress_t *proxy_egress, 
    bcm_if_t *proxy_if_id);

/* Destroy Proxy forwarding destination. */
extern int bcm_proxy_egress_destroy(
    int unit, 
    bcm_if_t proxy_if_id);

/* Get Proxy forwarding destination. */
extern int bcm_proxy_egress_get(
    int unit, 
    bcm_if_t proxy_if_id, 
    bcm_proxy_egress_t *proxy_egress);

/* 
 * Traverse through proxy egress objects and run callback at each valid
 * entry.
 */
extern int bcm_proxy_egress_traverse(
    int unit, 
    bcm_proxy_egress_traverse_cb trav_fn, 
    void *user_data);

#endif /* defined(INCLUDE_L3) */

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_PROXY_H__ */
