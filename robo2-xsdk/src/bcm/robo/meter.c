/*
 * $Id: $
 * 
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Meter Module Implementation 
 */

#include <shared/bsl.h>

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/field.h>
#include <sal/types.h>
#include <soc/drv.h>
#include <bcm_int/robo_dispatch.h>
/*
 * Function: bcm_robo_custom_meter_init
 *
 * Purpose:
 *     Init meter module
 *
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 *
 */
int 
bcm_robo_custom_meter_init(
    int unit)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_METER_INIT(unit));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_meter_profile_params_t_init
 *
 * Purpose:
 *     Init profile params structure
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     params - (OUT) Pointer to profile parameters
 * Returns:
 *     BCM_E_XXX
 *
 */
int
bcm_robo_custom_meter_profile_params_t_init(
    int unit,
    bcm_custom_meter_profile_params_t *params)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_METER_PROFILE_PARAMS_T_INIT(unit, params));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_meter_profile_create
 *
 * Purpose:
 *     Create a Meter profile
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     profile_params - (IN) Pointer to profile parameters
 *     profile_id - (OUT) Pointer to profile_id
 * Returns:
 *     BCM_E_XXX
 *
 */
int
bcm_robo_custom_meter_profile_create(
    int unit, 
    bcm_custom_meter_profile_params_t *profile_params, 
    bcm_custom_meter_profile_t *profile_id)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_METER_PROFILE_CREATE(unit, 
                                                        profile_params,
                                                        profile_id));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_meter_profile_destroy
 *
 * Purpose:
 *     Destroy meter profile previously created by
 *     bcm_custom_meter_profile_create()
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     profile_id - (IN) Profile Id
 * Returns:
 *     BCM_E_XXX
 *
 */
int
bcm_robo_custom_meter_profile_destroy(
    int unit, 
    bcm_custom_meter_profile_t profile_id)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_METER_PROFILE_DESTROY(unit, profile_id));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_meter_profile_get
 *
 * Purpose:
 *     This routine is used to get a meter profile
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     profile_id - (IN) Profile Id
 *     profile_params - (OUT) Pointer to profile parameters
 * Returns:
 *     BCM_E_XXX
 *
 */
int
bcm_robo_custom_meter_profile_get(
    int unit, 
    bcm_custom_meter_profile_t profile_id, 
    bcm_custom_meter_profile_params_t *profile_params)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_METER_PROFILE_GET(unit, profile_id, profile_params));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_meter_profile_set
 *
 * Purpose:
 *     This routine is used to set a meter profile
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     profile_id - (IN) Profile Id
 *     profile_params - (IN) Pointer to profile parameters
 * Returns:
 *     BCM_E_XXX
 *
 */
int
bcm_robo_custom_meter_profile_set(
    int unit, 
    bcm_custom_meter_profile_t profile_id, 
    bcm_custom_meter_profile_params_t *profile_params)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_METER_PROFILE_SET(unit,
                                                     profile_id,
                                                     profile_params));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_meter_params_t_init
 *
 * Purpose:
 *     Init meter params structure
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     params - (OUT) Pointer to meter parameters
 * Returns:
 *     BCM_E_XXX
 *
 */
int 
bcm_robo_custom_meter_params_t_init(
    int unit, 
    bcm_custom_meter_params_t *params)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_METER_PARAMS_T_INIT(unit, params));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_meter_create
 *
 * Purpose:
 *     
 *     This routine is used to create a meter
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     profile_id - (IN) Meter Profile Id
 *     meter_params - (IN) Pointer to meter parameters
 *     meter_id - (OUT) Meter id
 * Returns:
 *     BCM_E_XXX
 *
 */
int
bcm_robo_custom_meter_create(
    int unit, 
    bcm_custom_meter_profile_t profile_id, 
    bcm_custom_meter_params_t *meter_params, 
    bcm_custom_meterid_t *meter_id)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_METER_CREATE(unit,
                                                profile_id,
                                                meter_params,
                                                meter_id));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_meter_destroy
 *
 * Purpose:
 *      Destroy a meter previously created by custom_meter_create()
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     meter_id - (IN) Meter id
 * Returns:
 *     BCM_E_XXX
 *
 */
int
bcm_robo_custom_meter_destroy(
    int unit, 
    bcm_custom_meterid_t meter_id)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_METER_DESTROY(unit, meter_id));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_meter_pm_params_t_init
 *
 * Purpose:
 *     Init meter performance monitor params structure
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     params - (OUT) Pointer to meter pm parameters
 * Returns:
 *     BCM_E_XXX
 *
 */
int 
bcm_robo_custom_meter_pm_params_t_init(
    int unit, 
    bcm_custom_meter_pm_params_t *params)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_METER_PM_PARAMS_T_INIT(unit, params));
    return BCM_E_NONE;
}
/*
 * Function: bcm_robo_custom_meter_pm_enable
 *
 * Purpose:
 *     Enable Meter Performance monitoring
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     meter_id - (IN) Meter id
 * Returns:
 *     BCM_E_XXX
 *
 */
int
bcm_robo_custom_meter_pm_enable(
    int unit, 
    bcm_custom_meterid_t meter_id)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_METER_PM_ENABLE(unit, meter_id));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_meter_pm_disable
 *
 * Purpose:
 *     Disable Meter Performance monitoring counter collection
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     meter_id - (IN) Meter id
 * Returns:
 *     BCM_E_XXX
 *
 */
int
bcm_robo_custom_meter_pm_disable(
    int unit, 
    bcm_custom_meterid_t meter_id)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_METER_PM_DISABLE(unit, meter_id));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_meter_pm_get
 *
 * Purpose:
 *     This routine gets the Meter Performance monitoring counters
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     meter_id - (IN) Meter Id
 *     pm_params - (OUT) Pointer to performance monitor counters
 * Returns:
 *     BCM_E_XXX
 *
 */
int
bcm_robo_custom_meter_pm_get(
    int unit, 
    bcm_custom_meterid_t meter_id, 
    bcm_custom_meter_pm_params_t *pm_params)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_METER_PM_GET(unit, meter_id, pm_params));
    return BCM_E_NONE;
}

