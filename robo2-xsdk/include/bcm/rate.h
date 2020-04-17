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

#ifndef __BCM_RATE_H__
#define __BCM_RATE_H__

#include <bcm/types.h>

/* Rate flags. */
#define BCM_RATE_DLF                        0x01       
#define BCM_RATE_MCAST                      0x02       
#define BCM_RATE_BCAST                      0x04       
#define BCM_RATE_UCAST                      0x08       
#define BCM_RATE_SALF                       0x10       
#define BCM_RATE_RSVD_MCAST                 0x20       
#define BCM_RATE_KNOWN_MCAST                0x40       
#define BCM_RATE_UNKNOWN_MCAST              0x80       
#define BCM_RATE_CTRL_BUCKET_1              0x00010000 
#define BCM_RATE_CTRL_BUCKET_2              0x00020000 
#define BCM_RATE_MODE_PACKETS               0x00040000 
#define BCM_RATE_COLOR_BLIND                0x00080000 
#define BCM_RATE_ALL                        (BCM_RATE_BCAST | BCM_RATE_MCAST | BCM_RATE_DLF | BCM_RATE_UCAST | BCM_RATE_SALF | BCM_RATE_RSVD_MCAST | BCM_RATE_KNOWN_MCAST | BCM_RATE_UNKNOWN_MCAST) 
#define BCM_RATE_PKT_ADJ_HEADER_TRUNCATE    0x00100000 

/* Rate Enable/Disable macros */
#define BCM_RATE_DISABLE         0         
#define BCM_RATE_BLOCK          -1         

/* bcm_rate_limit_s */
typedef struct bcm_rate_limit_s {
    int br_dlfbc_rate; 
    int br_mcast_rate; 
    int br_bcast_rate; 
    int flags; 
} bcm_rate_limit_t;

/* bcm_rate_limit_t_init */
extern void bcm_rate_limit_t_init(
    bcm_rate_limit_t *rate_limit);

/* bcm_rate_packet_s */
typedef struct bcm_rate_packet_s {
    uint32 flags; 
    int pps; 
    int kbits_burst; 
} bcm_rate_packet_t;

/* bcm_rate_packet_t_init */
extern void bcm_rate_packet_t_init(
    bcm_rate_packet_t *pkt_rate);

#ifndef BCM_HIDE_DISPATCHABLE

/* Front ends to bcm_rate_packet_set/get functions. */
extern int bcm_rate_packet_set(
    int unit, 
    bcm_port_t port, 
    bcm_rate_packet_t *pkt_rate);

/* Front ends to bcm_rate_packet_set/get functions. */
extern int bcm_rate_packet_get(
    int unit, 
    bcm_port_t port, 
    bcm_rate_packet_t *pkt_rate);

/* 
 * Configure/retrieve rate limit and on/off state of DLF, MCAST, and
 * BCAST limiting.
 */
extern int bcm_rate_set(
    int unit, 
    int pps, 
    int flags);

/* 
 * Configure/retrieve rate limit and on/off state of DLF, MCAST, and
 * BCAST limiting.
 */
extern int bcm_rate_get(
    int unit, 
    int *pps, 
    int *flags);

/* 
 * Configure or retrieve rate limit for specified packet type on given
 * port.
 */
extern int bcm_rate_mcast_set(
    int unit, 
    int pps, 
    int flags, 
    int port);

/* 
 * Configure or retrieve rate limit for specified packet type on given
 * port.
 */
extern int bcm_rate_mcast_get(
    int unit, 
    int *pps, 
    int *flags, 
    int port);

/* 
 * Configure or retrieve rate limit for specified packet type on given
 * port.
 */
extern int bcm_rate_dlfbc_set(
    int unit, 
    int pps, 
    int flags, 
    int port);

/* 
 * Configure or retrieve rate limit for specified packet type on given
 * port.
 */
extern int bcm_rate_dlfbc_get(
    int unit, 
    int *pps, 
    int *flags, 
    int port);

/* 
 * Configure or retrieve rate limit for specified packet type on given
 * port.
 */
extern int bcm_rate_bcast_set(
    int unit, 
    int pps, 
    int flags, 
    int port);

/* 
 * Configure or retrieve rate limit for specified packet type on given
 * port.
 */
extern int bcm_rate_bcast_get(
    int unit, 
    int *pps, 
    int *flags, 
    int port);

/* 
 * Front ends to bcm_*_rate_set/get functions. Uses a single data
 * structure to write into all the 3 rate control registers.
 */
extern int bcm_rate_type_set(
    int unit, 
    bcm_rate_limit_t *rl);

/* 
 * Front ends to bcm_*_rate_set/get functions. Uses a single data
 * structure to write into all the 3 rate control registers.
 */
extern int bcm_rate_type_get(
    int unit, 
    bcm_rate_limit_t *rl);

/* 
 * Configure/retrieve metering rate limit for specified packet type on
 * given port.
 */
extern int bcm_rate_bandwidth_set(
    int unit, 
    bcm_port_t port, 
    int flags, 
    uint32 kbits_sec, 
    uint32 kbits_burst);

/* 
 * Configure/retrieve metering rate limit for specified packet type on
 * given port.
 */
extern int bcm_rate_bandwidth_get(
    int unit, 
    bcm_port_t port, 
    int flags, 
    uint32 *kbits_sec, 
    uint32 *kbits_burst);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_RATE_H__ */