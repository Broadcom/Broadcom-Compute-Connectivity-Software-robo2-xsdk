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

#ifndef __BCM_RANGE_H__
#define __BCM_RANGE_H__

#include <bcm/types.h>

/* Range checker Types. */
typedef enum bcm_range_type_e {
    bcmRangeTypeL4SrcPort = 0,      /* Check L4 Source port range. */
    bcmRangeTypeL4DstPort = 1,      /* Check L4 Destination port range. */
    bcmRangeTypeOuterVlan = 2,      /* Check Outer Vlan range. */
    bcmRangeTypePacketLength = 3,   /* Check IP payload length range. */
    bcmRangeTypeUdf = 4,            /* Check Udf range. */
    bcmRangeTypeCount = 5           /* Always last. Not a usable value. */
} bcm_range_type_t;

#define BCM_RANGE_TYPE \
{ \
    "L4SrcPort", \
    "L4DstPort", \
    "OuterVlan", \
    "PacketLength", \
    "Udf"  \
}

/* bcm_range_oper_mode_e */
typedef enum bcm_range_oper_mode_e {
    bcmRangeOperModeGlobal = 0,     /* Range Checkers Operate in Global Mode
                                       Value. i.e. for a given packet on any
                                       port from any pipe will have the same
                                       range check Result. */
    bcmRangeOperModePipeLocal = 1,  /* Range Checkers Operate in Per Pipe Mode
                                       Value. i.e. for a given packet on ports
                                       from different pipes can have the
                                       different range check Result. */
    bcmRangeOperModeCount = 2       /* Always last. Not a usable value. */
} bcm_range_oper_mode_t;

#define BCM_RANGE_OPER_MODE \
{ \
    "Global", \
    "PipeLocal", \
    "Count"  \
}

/* Opaque handle to a range. */
typedef uint32 bcm_range_t;

/* 
 * Range configuration structure used to create a range with specific
 * attributes.
 */
typedef struct bcm_range_config_s {
    bcm_range_t rid;        /* Range Check Id */
    bcm_range_type_t rtype; /* Range Check Type(bcmRangeTypeXXX) */
    uint32 min;             /* Lower limit of Range inclusive. */
    uint32 max;             /* Upper limit of Range inclusive. */
    uint8 offset;           /* Offset (in bits) for mask to apply before range
                               check comparison. */
    uint8 width;            /* Width(in bits) from offset specified for mask to
                               apply before range check comparison. */
    bcm_udf_id_t udf_id;    /* UDF id created using bcm_udf_create API. */
    bcm_pbmp_t ports;       /* Port bitmap to decide XGS pipe */
} bcm_range_config_t;

/* 
 * Range Module Flags. These flags can be passed to bcm_range_create API
 * via flags parameter.
 */
#define BCM_RANGE_CREATE_WITH_ID    (1 << 0)   

/* 
 * Initialize all members in bcm_range_config_t structure to default
 * values.
 */
extern void bcm_range_config_t_init(
    bcm_range_config_t *range_cfg);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * This API is to create a Layer 4 TCP/UDP port range checker or a VLAN
 * ranger checker or a packet length range checker or a UDF range checker
 * and returns an identifier that can be passed to
 * 'bcm_field_qualify_RangeCheck' API, to qualify a Field entry with
 * Range Checker match criteria.
 */
extern int bcm_range_create(
    int unit, 
    int flags, 
    bcm_range_config_t *range_cfg);

/* 
 * This API is to retrieve the operational configuration for a given
 * range id.
 */
extern int bcm_range_get(
    int unit, 
    bcm_range_config_t *range_cfg);

/* 
 * Destroys a range checker and releases resources allocated to the range
 * checker. Hardware resources will also be freed. But this API will
 * return BCM_E_BUSY, ifthe range checker is in use.
 */
extern int bcm_range_destroy(
    int unit, 
    bcm_range_t rid);

/* 
 * Configure Operational mode of Range Module to either Global or Pipe
 * Local.
 */
extern int bcm_range_oper_mode_set(
    int unit, 
    bcm_range_oper_mode_t oper_mode);

/* Get current operational mode of the Range Module. */
extern int bcm_range_oper_mode_get(
    int unit, 
    bcm_range_oper_mode_t *oper_mode);

#endif /* BCM_HIDE_DISPATCHABLE */

/* 
 * Device Range Id's traverse callback function. Customer Applications
 * can create function of this prototype and pass that function pointer
 * to bcm_range_travers API, so that for every valid range id ,  function
 * pointer will be called with some meta data
 */
typedef int (*bcm_range_traverse_cb)(
    int unit, 
    bcm_range_config_t *range_cfg, 
    void *user_data);

#ifndef BCM_HIDE_DISPATCHABLE

/* 
 * Traverse all the range checkers in the system, calling a specified
 * callback for each one
 */
extern int bcm_range_traverse(
    int unit, 
    bcm_range_traverse_cb callback, 
    void *user_data);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_RANGE_H__ */
