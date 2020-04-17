/*
 * $Id: ts.c $
 *
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File: ts.c
 * This file implements Driver services wrappers for Time sync module
 *
 */

#include <fsal_int/shared_types.h>
#include <soc/drv.h>
#include <fsal/ts.h>
#include <avenger_service.h>

/**
 * Function:
 *    drv_avgr_custom_ts_init
 * Purpose:
 *    This routine is used to initialize Time Sync module
 * Parameters:
 *    unit         (IN) Unit number.
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */
int
drv_avgr_custom_ts_init(int unit) {
    return ts_init();
}

/**
 * Function:
 *    drv_avgr_custom_ts_gen_config_set
 * Purpose:
 *    This routine is used to set Time Sync generator configuration
 * Parameters:
 *    unit         (IN) Unit number.
 *    generator_id (IN) Generator Id
 *    config       (IN) Pointer to Generator configuration parameters
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */
int
drv_avgr_custom_ts_gen_config_set(
    int unit,
    int generator_id,
    bcm_custom_ts_gen_config_t *config) {
    cbx_ts_gen_config_t cbx_ts_gen_cfg;

    cbx_ts_gen_cfg.flags = config->flags;
    cbx_ts_gen_cfg.start_time.sec = config->start_time.sec;
    cbx_ts_gen_cfg.start_time.nsec = config->start_time.nsec;
    cbx_ts_gen_cfg.high_time_nsec = config->high_time_nsec;
    cbx_ts_gen_cfg.low_time_nsec = config->low_time_nsec;

    return(cbx_ts_gen_config_set(generator_id, &cbx_ts_gen_cfg));
}

/**
 * Function:
 *    drv_avgr_custom_ts_event_config_set
 * Purpose:
 *    This routine is used to configure exeternal Time Sync events
 * Parameters:
 *    unit         (IN) Unit number.
 *    event_id     (IN) Event Id
 *    event_source (IN) Source Id of event
 *    config       (IN) Pointer to event configuration parameters
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */
int
drv_avgr_custom_ts_event_config_set(
    int unit,
    int event_id,
    int event_source,
    bcm_custom_ts_event_config_t *config) {
    cbx_ts_event_config_t cbx_ts_event_cfg;

    cbx_ts_event_cfg.flags = config->flags;
    cbx_ts_event_cfg.divider = config->divider;
    cbx_ts_event_cfg.event_cb_func = (cbx_ts_event_cb_t)config->event_cb_func;
    return(cbx_ts_event_config_set(event_id, event_source, &cbx_ts_event_cfg));
}

/**
 * Function:
 *    drv_avgr_custom_ts_tod_get
 * Purpose:
 *    This routine is used to get current TOD and time stamp
 * Parameters:
 *    unit      (IN) Unit number.
 *    time      (INOUT) Time of Day
 *    timestamp (INOUT) Timestamp value
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */
int
drv_avgr_custom_ts_tod_get(
    int unit,
    bcm_custom_ts_time_t *time,
    uint64_t *timestamp) {
    return(cbx_ts_tod_get((cbx_ts_time_t*)time, timestamp));
}

/**
 * Function:
 *    drv_avgr_custom_ts_tod_set
 * Purpose:
 *    This routine is used to set current TOD and time stamp
 * Parameters:
 *    unit      (IN) Unit number.
 *    time      (IN) Time of Day
 *    timestamp (IN) Timestamp value
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */
int
drv_avgr_custom_ts_tod_set(
    int unit,
    bcm_custom_ts_time_t time,
    uint64_t timestamp) {
    cbx_ts_time_t cbx_time;

    cbx_time.sec = time.sec;
    cbx_time.nsec = time.nsec;
    return(cbx_ts_tod_set(&cbx_time, timestamp));
}
