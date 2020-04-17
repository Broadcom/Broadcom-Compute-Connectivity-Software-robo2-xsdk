/*
 * $Id: cosq.h,v 1.4 Broadcom SDK $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * This file defines COSQ constants 
 *
 * Its contents are not used directly by applications; it is used only
 * by header files of parent APIs which need to share  COSQ constants.
 */

#ifndef _SHR_COSQ_H
#define _SHR_COSQ_H

#define _SHR_COSQ_CONTROL_FABRIC_CONNECT_MIN_UTIL_WHOLE_SHIFT    0
#define _SHR_COSQ_CONTROL_FABRIC_CONNECT_MIN_UTIL_WHOLE_MASK     0xFFFF
#define _SHR_COSQ_CONTROL_FABRIC_CONNECT_MIN_UTIL_FRACT_SHIFT    (16)
#define _SHR_COSQ_CONTROL_FABRIC_CONNECT_MIN_UTIL_FRACT_MASK     0xFFFF

#define _SHR_COSQ_GPORT_PRIORITY_PROFILE_SHIFT                  16
#define _SHR_COSQ_GPORT_PRIORITY_PROFILE_MASK			        0x07

/* 
 *  COSQ macros for bcm_cosq_control_t types
 */
#define _SHR_COSQ_CONTROL_FABRIC_CONNECT_MIN_UTILIZATION_SET(whole_utilization, fractional_utilization) \
              (((whole_utilization & _SHR_COSQ_CONTROL_FABRIC_CONNECT_MIN_UTIL_WHOLE_MASK) <<           \
                                        _SHR_COSQ_CONTROL_FABRIC_CONNECT_MIN_UTIL_WHOLE_SHIFT) |        \
               ((fractional_utilization & _SHR_COSQ_CONTROL_FABRIC_CONNECT_MIN_UTIL_FRACT_MASK) <<      \
                                        _SHR_COSQ_CONTROL_FABRIC_CONNECT_MIN_UTIL_FRACT_SHIFT))

#define _SHR_COSQ_CONTROL_FABRIC_CONNECT_MIN_UTILIZATION_WHOLE_GET(utilization)                         \
                ((utilization >> _SHR_COSQ_CONTROL_FABRIC_CONNECT_MIN_UTIL_WHOLE_SHIFT) & _SHR_COSQ_CONTROL_FABRIC_CONNECT_MIN_UTIL_WHOLE_MASK)


#define _SHR_COSQ_CONTROL_FABRIC_CONNECT_MIN_UTILIZATION_FRACTIONAL_GET(utilization)                    \
                ((utilization >> _SHR_COSQ_CONTROL_FABRIC_CONNECT_MIN_UTIL_FRACT_SHIFT) & _SHR_COSQ_CONTROL_FABRIC_CONNECT_MIN_UTIL_FRACT_MASK)


/*
 * priority profile template
 */
#define _SHR_COSQ_GPORT_PRIORITY_PROFILE_SET(_flags, _profile)             \
    _flags = _flags | ((_profile & _SHR_COSQ_GPORT_PRIORITY_PROFILE_MASK) << _SHR_COSQ_GPORT_PRIORITY_PROFILE_SHIFT)
#define _SHR_COSQ_GPORT_PRIORITY_PROFILE_GET(_flags)                       \
    ((_flags >> _SHR_COSQ_GPORT_PRIORITY_PROFILE_SHIFT) & _SHR_COSQ_GPORT_PRIORITY_PROFILE_MASK)

typedef enum _shr_cosq_threshold_type_e {
    _SHR_COSQ_THRESHOLD_BYTES,
    _SHR_COSQ_THRESHOLD_PACKET_DESCRIPTORS,
    _SHR_COSQ_THRESHOLD_PACKETS,
    _SHR_COSQ_THRESHOLD_DATA_BUFFERS,
    _SHR_COSQ_THRESHOLD_AVAILABLE_PACKET_DESCRIPTORS,
    _SHR_COSQ_THRESHOLD_AVAILABLE_DATA_BUFFERS,
    _SHR_COSQ_THRESHOLD_BUFFER_DESCRIPTOR_BUFFERS,
    _SHR_COSQ_THRESHOLD_BUFFER_DESCRIPTORS,
    _SHR_COSQ_THRESHOLD_DBUFFS,
    _SHR_COSQ_THRESHOLD_FULL_DBUFFS,
    _SHR_COSQ_THRESHOLD_MINI_DBUFFS,
    _SHR_COSQ_THRESHOLD_DYNAMIC_WEIGHT,
    _SHR_COSQ_THRESHOLD_PACKET_DESCRIPTORS_MIN,
    _SHR_COSQ_THRESHOLD_PACKET_DESCRIPTORS_MAX,
    _SHR_COSQ_THRESHOLD_PACKET_DESCRIPTORS_ALPHA,
    _SHR_COSQ_THRESHOLD_DATA_BUFFERS_MIN,
    _SHR_COSQ_THRESHOLD_DATA_BUFFERS_MAX,
    _SHR_COSQ_THERESHOLD_DATA_BUFFERS_ALPHA,
    _SHR_COSQ_THRESHOLD_OCB_DBUFFS,
    _SHR_COSQ_THRESHOLD_PACKET_DESCRIPTOR_BUFFERS,
    _SHR_COSQ_THRESHOLD_BUNDLE_DESCRIPTOR_BUFFERS,
    _SHR_COSQ_THRESHOLD_NUM
} _shr_cosq_threshold_type_t;

#endif	/* !_SHR_COSQ_H */
