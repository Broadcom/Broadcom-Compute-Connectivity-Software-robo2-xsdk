/*
 * $Id: $
 *
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Time sync Module Implementation
 */

#include <bcm/error.h>
#include <bcm/types.h>
#include <soc/drv.h>


/*
 * Function: bcm_robo_custom_ts_init
 * Purpose:
 *     This routine is used to initialize Time Sync module
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_custom_ts_init(int unit)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_TS_INIT(unit));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_ts_gen_config_set
 * Purpose:
 *     This routine is used to set Time Sync generator configuration
 * Parameters:
 *     unit - (IN) BCM device number
 *     generator_id - (IN) Generator Id
 *     config - (IN) Pointer to Generator configuration parameters
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_custom_ts_gen_config_set(
    int unit,
    int generator_id,
    bcm_custom_ts_gen_config_t *config)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_TS_GEN_CONFIG_SET(unit,
                                                     generator_id,
                                                     config));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_ts_event_config_set
 * Purpose:
 *     This routine is used to configure exeternal Time Sync events
 * Parameters:
 *     unit - (IN) BCM device number
 *     event_id - (IN) Event Id
 *     event_source - (IN) Source Id of event
 *     config - (IN) Pointer to event configuration parameters
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_custom_ts_event_config_set(
    int unit,
    int event_id,
    int event_source,
    bcm_custom_ts_event_config_t *config)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_TS_EVENT_CONFIG_SET(unit,
                                                       event_id,
                                                       event_source,
                                                       config));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_ts_tod_get
 * Purpose:
 *     This routine is used to get current TOD and time stamp
 * Parameters:
 *     unit - (IN) BCM device number
 *     time - (INOUT) Time of Day
 *     timestamp - (INOUT) Timestamp value
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_custom_ts_tod_get(
    int unit,
    bcm_custom_ts_time_t *time,
    uint64 *timestamp)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_TS_TOD_GET(unit,
                                              time,
                                              timestamp));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_ts_tod_set
 * Purpose:
 *     This routine is used to set current TOD and time stamp
 * Parameters:
 *     unit - (IN) BCM device number
 *     time - (IN) Time of Day
 *     timestamp - (IN) Timestamp value
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_custom_ts_tod_set(
    int unit,
    bcm_custom_ts_time_t time,
    uint64 timestamp)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_TS_TOD_SET(unit,
                                              time,
                                              timestamp));
    return BCM_E_NONE;
}
