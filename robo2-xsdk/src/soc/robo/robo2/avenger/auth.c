/*
 * $Id: auth.c $
 *
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File: auth.c
 * This file implements Driver services wrappers for Auth
 *
 */
#include <shared/bsl.h>
#include <sal/types.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <fsal_int/shared_types.h>
#include <fsal_int/types.h>
#include <fsal/auth.h>
#include <fsal_int/auth.h>
#include <avenger_service.h>

/**
 * Function:
 *      drv_avgr_custom_auth_params_t_init
 * Purpose:
 *      Initializes global authentication parameters.
 * Parameters:
 *      unit   (IN) Unit number.
 *      auth_params  (OUT) Auth Parameter object
 * Returns:
 *      SOC_E_XXX
 */
int 
drv_avgr_custom_auth_params_t_init(
    int unit,
    bcm_custom_auth_params_t *auth_params) {

    cbx_auth_params_t_init((cbx_auth_params_t *) auth_params); 
    return SOC_E_NONE;
}

/**
 * Function:
 *      drv_avgr_custom_auth_port_mode_t_init
 * Purpose:
 *      Initializes authentication port parameters.
 * Parameters:
 *      unit   (IN) Unit number.
 *      auth_port_mode - (IN)  Port auth parameter object
 * Returns:
 *      SOC_E_XXX
 */
int 
drv_avgr_custom_auth_port_mode_t_init(
    int unit,
    bcm_custom_auth_port_mode_t *auth_port_mode) {

    cbx_auth_port_mode_t_init((cbx_auth_port_mode_t *)auth_port_mode);
    return SOC_E_NONE;
}

/*
 * Function:
 *      drv_avgr_custom_auth_init
 * Purpose:
 *      Initializes the Auth module.
 * Parameters:
 *      unit   (IN) Unit number.
 *      auth_params - (IN)  Auth Parameter object
 * Returns:
 *      SOC_E_XXX
 */
int 
drv_avgr_custom_auth_init(
    int unit,
    bcm_custom_auth_params_t *auth_params) {

    return (cbx_auth_init((cbx_auth_params_t *) auth_params));
}

/*
 * Function:
 *      drv_avgr_custom_auth_detach
 * Purpose:
 *      Destroys previously initialized Auth module.
 * Parameters:
 *      unit   (IN) Unit number.
 *      auth_params - (IN)  Auth Parameter object
 * Returns:
 *      SOC_E_XXX
 */
int 
drv_avgr_custom_auth_detach(
    int unit,
    bcm_custom_auth_params_t *auth_params) {

    return (cbx_auth_detach((cbx_auth_params_t *) auth_params));
}

/**
 * Function:
 *      drv_avgr_custom_auth_mode_get
 * Purpose:
 *      Get Authentication configuration information.
 * Parameters:
 *      unit          (IN)  Unit number.
 *      auth_params   (OUT) Auth Parameters object
 * Returns:
 *      SOC_E_NONE Success
 *      SOC_E_XXXX Failure
 */
int 
drv_avgr_custom_auth_mode_get(
    int unit,
    bcm_custom_auth_params_t *auth_params) {

    return (cbx_auth_mode_get((cbx_auth_params_t *)auth_params));
}

/**
 * Function:
 *      drv_avgr_custom_auth_mode_set
 * Purpose:
 *      Set Authentication configuration information.
 * Parameters:
 *      unit          (IN)  Unit number.
 *      auth_params   (OUT) Auth Parameters object
 * Returns:
 *      SOC_E_NONE Success
 *      SOC_E_XXXX Failure
 */
int 
drv_avgr_custom_auth_mode_set(
    int unit,
    bcm_custom_auth_params_t *auth_params) {

    return (cbx_auth_mode_set((cbx_auth_params_t *)auth_params));
}

/**
 * Function:
 *      drv_avgr_custom_auth_port_get
 * Purpose:
 *      Get Authentication mode of a port.
 * Parameters:
 *      unit          (IN)  Unit number.
 *      portid        (IN) PORT Identifier
 *      port_mode     (IN) Port Auth Parameters object
 * Returns:
 *      SOC_E_NONE Success
 *      SOC_E_XXXX Failure
 */
int 
drv_avgr_custom_auth_port_get(
    int unit,
    bcm_gport_t portid, 
    bcm_custom_auth_port_mode_t *port_mode) {

    return((cbx_auth_port_get((cbx_portid_t)portid,
                             (cbx_auth_port_mode_t *) port_mode)));
}

/**
 * Function:
 *      drv_avgr_custom_auth_port_set
 * Purpose:
 *      Set Authentication mode of a port.
 * Parameters:
 *      unit          (IN)  Unit number.
 *      portid        (IN) PORT Identifier
 *      port_mode     (IN) Port Auth Parameters object
 * Returns:
 *      SOC_E_NONE Success
 *      SOC_E_XXXX Failure
 */
int 
drv_avgr_custom_auth_port_set(
    int unit,
    bcm_gport_t portid, 
    bcm_custom_auth_port_mode_t *port_mode) {

    return ((cbx_auth_port_set((cbx_portid_t)portid,
                             (cbx_auth_port_mode_t *) port_mode)));
}
