/*
 * $Id: meter.c $
 *
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File: meter.c
 * This file implements Driver services wrappers for Meter
 *
 */
#include <shared/bsl.h>
#include <sal/types.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <fsal_int/shared_types.h>
#include <fsal_int/types.h>
#include <fsal/cosq.h>
#include <fsal/meter.h>
#include <fsal_int/meter.h>
#include <avenger_service.h>

/*
 * Function: drv_avgr_custom_meter_init
 *
 * Purpose:
 *     Init meter module
 *
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     SOC_E_XXX
 *
 */
int
drv_avgr_custom_meter_init(
    int unit)
{
   return (cbx_meter_init());
}

/*
 * Function: drv_avgr_custom_meter_profile_params_t_init
 *
 * Purpose:
 *     Init meter profile params struct
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     params - (OUT) Pointer to profile parameters
 * Returns:
 *     SOC_E_XXX
 *
 */
int
drv_avgr_custom_meter_profile_params_t_init(
    int unit,
    bcm_custom_meter_profile_params_t *profile_params)
{
    cbx_meter_profile_params_t cb_prof_params;
    cbxi_meter_profile_params_t_init(&cb_prof_params);
    profile_params->flags = cb_prof_params.flags;
    profile_params->type = cb_prof_params.type;
    profile_params->mode = cb_prof_params.mode;
    profile_params->ckbits_sec = cb_prof_params.ckbits_sec;
    profile_params->ckbits_burst = cb_prof_params.ckbits_burst;
    profile_params->pkbits_sec = cb_prof_params.pkbits_sec;
    profile_params->pkbits_burst = cb_prof_params.pkbits_burst;
    return SOC_E_NONE;
}
/*
 * Function: drv_avgr_custom_meter_profile_create
 *
 * Purpose:
 *     Create a Meter profile
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     profile_params - (IN) Pointer to profile parameters
 *     profile_id - (OUT) Pointer to profile_id
 * Returns:
 *     SOC_E_XXX
 *
 */
int
drv_avgr_custom_meter_profile_create(
    int unit,
    bcm_custom_meter_profile_params_t * profile_params,
    bcm_custom_meter_profile_t *profile_id)
{
    int rv = SOC_E_NONE;
    cbx_meter_profile_params_t cb_prof_params;

    sal_memset(&cb_prof_params, 0, sizeof(cbx_meter_profile_params_t));

    cb_prof_params.flags        = profile_params->flags;
    cb_prof_params.type         = profile_params->type;
    cb_prof_params.mode         = profile_params->mode;
    cb_prof_params.ckbits_sec   = profile_params->ckbits_sec;
    cb_prof_params.ckbits_burst = profile_params->ckbits_burst;
    cb_prof_params.pkbits_sec   = profile_params->pkbits_sec;
    cb_prof_params.pkbits_burst = profile_params->pkbits_burst;
    rv = cbx_meter_profile_create(&cb_prof_params,
                                  (cbx_meter_profile_t*)profile_id);
    return rv;
}

/*
 * Function: drv_avgr_custom_meter_profile_destroy
 *
 * Purpose:
 *     Destroy meter profile previously created by
 *     bcm_custom_meter_profile_create()
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     profile_id - (IN) Profile Id
 * Returns:
 *     SOC_E_XXX
 *
 */
int
drv_avgr_custom_meter_profile_destroy(
    int unit,
    bcm_custom_meter_profile_t profile_id)
{
    int rv = SOC_E_NONE;

    rv = cbx_meter_profile_destroy((cbx_meter_profile_t)profile_id);

    return rv;
}

/*
 * Function: drv_avgr_custom_meter_profile_get
 *
 * Purpose:
 *     This routine is used to get a meter profile
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     profile_id - (IN) Profile Id
 *     profile_params - (OUT) Pointer to profile parameters
 * Returns:
 *     SOC_E_XXX
 *
 */
int
drv_avgr_custom_meter_profile_get(
    int unit,
    bcm_custom_meter_profile_t profile_id,
    bcm_custom_meter_profile_params_t *profile_params)
{
    int rv = SOC_E_NONE;
    cbx_meter_profile_params_t cb_prof_params;

    if (profile_params == NULL) {
        return SOC_E_PARAM;
    }
    rv = cbx_meter_profile_get((cbx_meter_profile_t)profile_id,
                                &cb_prof_params);
    if (rv == SOC_E_NONE) {
        profile_params->flags = cb_prof_params.flags;
        profile_params->type = cb_prof_params.type;
        profile_params->mode = cb_prof_params.mode;
        profile_params->ckbits_sec = cb_prof_params.ckbits_sec;
        profile_params->ckbits_burst = cb_prof_params.ckbits_burst;
        profile_params->pkbits_sec = cb_prof_params.pkbits_sec;
        profile_params->pkbits_burst = cb_prof_params.pkbits_burst;
    }
    return rv;
}

/*
 * Function: drv_avgr_custom_meter_profile_set
 *
 * Purpose:
 *     This routine is used to set a meter profile
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     profile_id - (IN) Profile Id
 *     profile_params - (IN) Pointer to profile parameters
 * Returns:
 *     SOC_E_XXX
 *
 */
int
drv_avgr_custom_meter_profile_set(
    int unit,
    bcm_custom_meter_profile_t profile_id,
    bcm_custom_meter_profile_params_t *profile_params)
{
    int rv = SOC_E_NONE;
    cbx_meter_profile_params_t cb_prof_params;

    if (profile_params == NULL) {
        return SOC_E_PARAM;
    }

    sal_memset(&cb_prof_params, 0, sizeof(cbx_meter_profile_params_t));

    cb_prof_params.flags        = profile_params->flags;
    cb_prof_params.type         = profile_params->type;
    cb_prof_params.mode         = profile_params->mode;
    cb_prof_params.ckbits_sec   = profile_params->ckbits_sec;
    cb_prof_params.ckbits_burst = profile_params->ckbits_burst;
    cb_prof_params.pkbits_sec   = profile_params->pkbits_sec;
    cb_prof_params.pkbits_burst = profile_params->pkbits_burst;
    rv = cbx_meter_profile_set((cbx_meter_profile_t)profile_id,
                                &cb_prof_params);
    return rv;
}

/*
 * Function: drv_avgr_custom_meter_params_t_init
 *
 * Purpose:
 *     Init meter params struct
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     params - (OUT) Pointer to meter parameters
 * Returns:
 *     SOC_E_XXX
 *
 */
int
drv_avgr_custom_meter_params_t_init(
    int unit,
    bcm_custom_meter_params_t *params)
{
    cbx_meter_params_t cb_meter_params;
    cbxi_meter_params_t_init(&cb_meter_params);
    params->portid = cb_meter_params.portid;
    params->traffic_type = cb_meter_params.traffic_type;
    params->tc.int_pri = cb_meter_params.tc.int_pri;
    params->tc.int_dp = cb_meter_params.tc.int_dp;
    params->trap_group_index = cb_meter_params.trap_group_index;
    return SOC_E_NONE;
}

/*
 * Function: drv_avgr_custom_meter_create
 *
 * Purpose:
 *     This routine is used to create a meter
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     profile_id - (IN) Meter Profile Id
 *     meter_params - (IN) Pointer to meter parameters
 *     meter_id - (OUT) Meter id
 * Returns:
 *     SOC_E_XXX
 *
 */
int
drv_avgr_custom_meter_create(
    int unit,
    bcm_custom_meter_profile_t profile_id,
    bcm_custom_meter_params_t *meter_params,
    bcm_custom_meterid_t *meter_id)
{
    int rv = SOC_E_NONE;
    cbx_meter_params_t cb_meter_params;

    sal_memset(&cb_meter_params, 0, sizeof(cbx_meter_params_t));

    cb_meter_params.portid           = meter_params->portid;
    cb_meter_params.traffic_type     = meter_params->traffic_type;
    cb_meter_params.tc.int_pri       = meter_params->tc.int_pri;
    cb_meter_params.tc.int_dp        = meter_params->tc.int_dp;
    cb_meter_params.trap_group_index = meter_params->trap_group_index;

    rv = cbx_meter_create((cbx_meter_profile_t) profile_id,
                          &cb_meter_params,
                          (cbx_meterid_t*)meter_id);
    return rv;

}

/*
 * Function: drv_avgr_custom_meter_destroy
 *
 * Purpose:
 *      Destroy a meter previously created by custom_meter_create()
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     meter_id - (IN) Meter id
 * Returns:
 *     SOC_E_XXX
 *
 */
int
drv_avgr_custom_meter_destroy(
    int unit,
    bcm_custom_meterid_t meter_id)
{
    int rv = SOC_E_NONE;

    rv = cbx_meter_destroy((cbx_meterid_t)meter_id);

    return rv;
}

/*
 * Function: drv_avgr_custom_meter_pm_params_t_init
 *
 * Purpose:
 *     Init meter performance monitor counters struct
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     params - (OUT) Pointer to pm counters struct
 * Returns:
 *     SOC_E_XXX
 *
 */
int
drv_avgr_custom_meter_pm_params_t_init(
    int unit,
    bcm_custom_meter_pm_params_t *pm_params)
{
    cbx_meter_pm_params_t cb_pm_params;
    cbxi_meter_pm_params_t_init(&cb_pm_params);
    COMPILER_64_COPY(pm_params->custom_meter_pm_green.packet,
                     cb_pm_params.cbx_meter_pm_green.packet);
    COMPILER_64_COPY(pm_params->custom_meter_pm_yellow.packet,
                     cb_pm_params.cbx_meter_pm_yellow.packet);
    COMPILER_64_COPY(pm_params->custom_meter_pm_red.packet,
                     cb_pm_params.cbx_meter_pm_red.packet);
    COMPILER_64_COPY(pm_params->custom_meter_pm_reserved.packet,
                     cb_pm_params.cbx_meter_pm_reserved.packet);
    COMPILER_64_COPY(pm_params->custom_meter_pm_g2y.packet,
                     cb_pm_params.cbx_meter_pm_g2y.packet);
    COMPILER_64_COPY(pm_params->custom_meter_pm_y2r.packet,
                     cb_pm_params.cbx_meter_pm_y2r.packet);
    COMPILER_64_COPY(pm_params->custom_meter_pm_g2r.packet,
                     cb_pm_params.cbx_meter_pm_g2r.packet);
    COMPILER_64_COPY(pm_params->custom_meter_pm_y2g.packet,
                     cb_pm_params.cbx_meter_pm_y2g.packet);
    COMPILER_64_COPY(pm_params->custom_meter_pm_r2g.packet,
                     cb_pm_params.cbx_meter_pm_r2g.packet);
    COMPILER_64_COPY(pm_params->custom_meter_pm_r2y.packet,
                     cb_pm_params.cbx_meter_pm_r2y.packet);
    COMPILER_64_COPY(pm_params->custom_meter_pm_g2d.packet,
                     cb_pm_params.cbx_meter_pm_g2d.packet);
    COMPILER_64_COPY(pm_params->custom_meter_pm_y2d.packet,
                     cb_pm_params.cbx_meter_pm_y2d.packet);
    COMPILER_64_COPY(pm_params->custom_meter_pm_r2d.packet,
                     cb_pm_params.cbx_meter_pm_r2d.packet);

    COMPILER_64_COPY(pm_params->custom_meter_pm_green.byte,
                     cb_pm_params.cbx_meter_pm_green.byte);
    COMPILER_64_COPY(pm_params->custom_meter_pm_yellow.byte,
                     cb_pm_params.cbx_meter_pm_yellow.byte);
    COMPILER_64_COPY(pm_params->custom_meter_pm_red.byte,
                     cb_pm_params.cbx_meter_pm_red.byte);
    COMPILER_64_COPY(pm_params->custom_meter_pm_reserved.byte,
                     cb_pm_params.cbx_meter_pm_reserved.byte);
    COMPILER_64_COPY(pm_params->custom_meter_pm_g2y.byte,
                     cb_pm_params.cbx_meter_pm_g2y.byte);
    COMPILER_64_COPY(pm_params->custom_meter_pm_y2r.byte,
                     cb_pm_params.cbx_meter_pm_y2r.byte);
    COMPILER_64_COPY(pm_params->custom_meter_pm_g2r.byte,
                     cb_pm_params.cbx_meter_pm_g2r.byte);
    COMPILER_64_COPY(pm_params->custom_meter_pm_y2g.byte,
                     cb_pm_params.cbx_meter_pm_y2g.byte);
    COMPILER_64_COPY(pm_params->custom_meter_pm_r2g.byte,
                     cb_pm_params.cbx_meter_pm_r2g.byte);
    COMPILER_64_COPY(pm_params->custom_meter_pm_r2y.byte,
                     cb_pm_params.cbx_meter_pm_r2y.byte);
    COMPILER_64_COPY(pm_params->custom_meter_pm_g2d.byte,
                     cb_pm_params.cbx_meter_pm_g2d.byte);
    COMPILER_64_COPY(pm_params->custom_meter_pm_y2d.byte,
                     cb_pm_params.cbx_meter_pm_y2d.byte);
    COMPILER_64_COPY(pm_params->custom_meter_pm_r2d.byte,
                     cb_pm_params.cbx_meter_pm_r2d.byte);
    
    return SOC_E_NONE;
}

/*
 * Function: drv_avgr_custom_meter_pm_enable
 *
 * Purpose:
 *     Enable Meter Performance monitoring
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     meter_id - (IN) Meter id
 * Returns:
 *     SOC_E_XXX
 *
 */
int
drv_avgr_custom_meter_pm_enable(
    int unit,
    bcm_custom_meterid_t meter_id)
{
    int rv = SOC_E_NONE;

    rv = cbx_meter_pm_enable((cbx_meterid_t)meter_id);

    return rv;
}

/*
 * Function: drv_avgr_custom_meter_pm_disable
 *
 * Purpose:
 *     Disable Meter Performance monitoring counter collection
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     meter_id - (IN) Meter id
 * Returns:
 *     SOC_E_XXX
 *
 */
int
drv_avgr_custom_meter_pm_disable(
    int unit,
    bcm_custom_meterid_t meter_id)
{
    int rv = SOC_E_NONE;

    rv = cbx_meter_pm_disable((cbx_meterid_t)meter_id);

    return rv;
}

/*
 * Function: drv_avgr_custom_meter_pm_get
 *
 * Purpose:
 *     This routine gets the Meter Performance monitoring counters
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     meter_id - (IN) Meter Id
 *     pm_params - (OUT) Pointer to performance monitor counters
 * Returns:
 *     SOC_E_XXX
 *
 */
int
drv_avgr_custom_meter_pm_get(
    int unit,
    bcm_custom_meterid_t meter_id,
    bcm_custom_meter_pm_params_t *pm_params)
{
    int rv = SOC_E_NONE;
    cbx_meter_pm_params_t  cb_pm_params;

    if (pm_params == NULL) {
        return SOC_E_PARAM;
    }

    /* Get the counters and copy out if successful */
    rv = cbx_meter_pm_get((cbx_meterid_t)meter_id, &cb_pm_params);

    if (rv == SOC_E_NONE) {
       COMPILER_64_COPY(pm_params->custom_meter_pm_green.packet,
                        cb_pm_params.cbx_meter_pm_green.packet);
       COMPILER_64_COPY(pm_params->custom_meter_pm_yellow.packet,
                        cb_pm_params.cbx_meter_pm_yellow.packet);
       COMPILER_64_COPY(pm_params->custom_meter_pm_red.packet,
                        cb_pm_params.cbx_meter_pm_red.packet);
       COMPILER_64_COPY(pm_params->custom_meter_pm_reserved.packet,
                        cb_pm_params.cbx_meter_pm_reserved.packet);
       COMPILER_64_COPY(pm_params->custom_meter_pm_g2y.packet,
                        cb_pm_params.cbx_meter_pm_g2y.packet);
       COMPILER_64_COPY(pm_params->custom_meter_pm_y2r.packet,
                        cb_pm_params.cbx_meter_pm_y2r.packet);
       COMPILER_64_COPY(pm_params->custom_meter_pm_g2r.packet,
                        cb_pm_params.cbx_meter_pm_g2r.packet);
       COMPILER_64_COPY(pm_params->custom_meter_pm_y2g.packet,
                        cb_pm_params.cbx_meter_pm_y2g.packet);
       COMPILER_64_COPY(pm_params->custom_meter_pm_r2g.packet,
                        cb_pm_params.cbx_meter_pm_r2g.packet);
       COMPILER_64_COPY(pm_params->custom_meter_pm_r2y.packet,
                        cb_pm_params.cbx_meter_pm_r2y.packet);
       COMPILER_64_COPY(pm_params->custom_meter_pm_g2d.packet,
                        cb_pm_params.cbx_meter_pm_g2d.packet);
       COMPILER_64_COPY(pm_params->custom_meter_pm_y2d.packet,
                        cb_pm_params.cbx_meter_pm_y2d.packet);
       COMPILER_64_COPY(pm_params->custom_meter_pm_r2d.packet,
                        cb_pm_params.cbx_meter_pm_r2d.packet);

       COMPILER_64_COPY(pm_params->custom_meter_pm_green.byte,
                        cb_pm_params.cbx_meter_pm_green.byte);
       COMPILER_64_COPY(pm_params->custom_meter_pm_yellow.byte,
                        cb_pm_params.cbx_meter_pm_yellow.byte);
       COMPILER_64_COPY(pm_params->custom_meter_pm_red.byte,
                        cb_pm_params.cbx_meter_pm_red.byte);
       COMPILER_64_COPY(pm_params->custom_meter_pm_reserved.byte,
                        cb_pm_params.cbx_meter_pm_reserved.byte);
       COMPILER_64_COPY(pm_params->custom_meter_pm_g2y.byte,
                        cb_pm_params.cbx_meter_pm_g2y.byte);
       COMPILER_64_COPY(pm_params->custom_meter_pm_y2r.byte,
                        cb_pm_params.cbx_meter_pm_y2r.byte);
       COMPILER_64_COPY(pm_params->custom_meter_pm_g2r.byte,
                        cb_pm_params.cbx_meter_pm_g2r.byte);
       COMPILER_64_COPY(pm_params->custom_meter_pm_y2g.byte,
                        cb_pm_params.cbx_meter_pm_y2g.byte);
       COMPILER_64_COPY(pm_params->custom_meter_pm_r2g.byte,
                        cb_pm_params.cbx_meter_pm_r2g.byte);
       COMPILER_64_COPY(pm_params->custom_meter_pm_r2y.byte,
                        cb_pm_params.cbx_meter_pm_r2y.byte);
       COMPILER_64_COPY(pm_params->custom_meter_pm_g2d.byte,
                        cb_pm_params.cbx_meter_pm_g2d.byte);
       COMPILER_64_COPY(pm_params->custom_meter_pm_y2d.byte,
                        cb_pm_params.cbx_meter_pm_y2d.byte);
       COMPILER_64_COPY(pm_params->custom_meter_pm_r2d.byte,
                        cb_pm_params.cbx_meter_pm_r2d.byte);
    }
    return rv;
}
