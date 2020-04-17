/*
 * $Id: cosq.c $
 *
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File: cosq.c
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
#include <fsal_int/cosq.h>
#include <fsal/ts.h>
#include <avenger_service.h>

/*
 * Function:
 *      drv_avgr_custom_cosq_init
 * Purpose:
 *      Initialize CoSQ module
 * Parameters:
 *      unit   (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
drv_avgr_custom_cosq_init(int unit) {

    return (cbx_cosq_init());
}


/**
 * Function:
 *      drv_avgr_custom_flowcontrol_params_t_init
 * Purpose:
 *      Initialize Flow control parameters.
 * Parameters:
 *      unit     (IN) Unit number.
 *      portid   (IN) Port Identifier
 *      flowcontrol_params - (OUT)  Parameter object
 * Returns:
 *      SOC_E_XXX
 */
int
drv_avgr_custom_flowcontrol_params_t_init(
    int unit,
    bcm_gport_t portid,
    bcm_custom_flowcontrol_params_t *flowcontrol_params) {

    cbx_flowcontrol_params_t_init((cbx_portid_t)portid,
                                  (cbx_flowcontrol_params_t *)flowcontrol_params);
    return SOC_E_NONE;
}

/**
 * Function:
 *      drv_avgr_custom_cosq_shaper_params_t_init
 * Purpose:
 *      Initialize Shaper parameters.
 * Parameters:
 *      unit     (IN) Unit number.
 *      portid   (IN) Port Identifier
 *      shaper_params - (OUT)  Parameter object
 * Returns:
 *      SOC_E_XXX
 */
int
drv_avgr_custom_cosq_shaper_params_t_init(
    int unit,
    bcm_gport_t portid,
    bcm_custom_cosq_shaper_params_t *shaper_params) {

    cbx_cosq_shaper_params_t_init((cbx_portid_t)portid,
                                  (cbx_cosq_shaper_params_t *)shaper_params);
    return SOC_E_NONE;

}

/**
 * Function:
 *    drv_avgr_custom_cosq_port_default_tc_set
 * Purpose:
 *    This routine is used to set the default ingress traffic class on a port
 * Parameters:
 *    unit     (IN) Unit number.
 *    portid   (IN)  PORT Identifier
 *    tc       (IN)  Traffic Class structure
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */
int
drv_avgr_custom_cosq_port_default_tc_set(
    int unit,
    bcm_gport_t portid,
    bcm_custom_tc_t tc) {
    int rv = SOC_E_NONE;
    cbx_tc_t cbx_tc;
    cbx_tc.int_pri = tc.int_pri;
    cbx_tc.int_dp = tc.int_dp;
    rv = cbx_cosq_port_default_tc_set((cbx_portid_t)portid,
                                                     (cbx_tc_t) cbx_tc);
    return rv;

}

/**
 * Function:
 *    drv_avgr_custom_cosq_port_default_tc_get
 * Purpose:
 *    This routine is used to get the default ingress traffic class on a port
 * Parameters:
 *    unit      (IN) Unit number.
 *    portid    (IN)  PORT Identifier
 *    tc        (OUT) Traffic Class structure
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */
int
drv_avgr_custom_cosq_port_default_tc_get(
    int unit,
    bcm_gport_t portid,
    bcm_custom_tc_t *tc) {

    int rv = SOC_E_NONE;
    rv = (cbx_cosq_port_default_tc_get((cbx_portid_t)portid,
                                                     (cbx_tc_t *) tc));
    return rv;

}

/**
 * Function:
 *    drv_avgr_custom_cosq_port_default_pcp_set
 * Purpose:
 *    This routine is used to set the default pcp,dei on a port
 * Parameters:
 *    unit     (IN) Unit number.
 *    portid   (IN)  PORT Identifier
 *    pcp      (IN)  Priority Code Point (3 bit encoding)
 *    dei      (IN)  Drop Eligibility Indicator (1 bit encoding)
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */
int
drv_avgr_custom_cosq_port_default_pcp_set(
    int unit,
    bcm_gport_t portid,
    int pcp,
    int dei) {

    int rv = SOC_E_NONE;
    rv = (cbx_cosq_port_default_pcp_set((cbx_portid_t)portid,
                                                      pcp,
                                                      dei));
    return rv;

}

/**
 * Function:
 *    drv_avgr_custom_cosq_port_default_pcp_get
 * Purpose:
 *    This routine is used to get the default egress pcp,dei on a port
 * Parameters:
 *    unit     (IN) Unit number.
 *    portid   (IN)  PORT Identifier
 *    pcp      (OUT)  Priority Code Point (3 bit encoding)
 *    dei      (OUT)  Drop Eligibility Indicator (1 bit encoding)
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */
int
drv_avgr_custom_cosq_port_default_pcp_get(
    int unit,
    bcm_gport_t portid,
    int *pcp,
    int *dei) {

    int rv = SOC_E_NONE;
    rv = (cbx_cosq_port_default_pcp_get((cbx_portid_t)portid,
                                                      pcp,
                                                      dei));
    return rv;

}

/**
 * Function:
 *    drv_avgr_custom_cosq_port_pcp2tc_set
 * Purpose:
 *    This routine is used to set the ingress mapping of {pcp,dei} to
 *    traffic class
 * Parameters:
 *    unit -   (IN) Unit number.
 *    portid   (IN)  PORT Identifier
 *    pcp      (IN)  Priority Code Point (3 bit encoding)
 *    dei      (IN)  Drop Eligibility Indicator (1 bit encoding)
 *    tc       (IN)  Traffic Class structure
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */
int
drv_avgr_custom_cosq_port_pcp2tc_set(
    int unit,
    bcm_gport_t portid,
    int pcp,
    int dei,
    bcm_custom_tc_t tc) {

    int rv = SOC_E_NONE;
    cbx_tc_t cbx_tc;
    cbx_tc.int_pri = tc.int_pri;
    cbx_tc.int_dp = tc.int_dp;
    rv = (cbx_cosq_port_pcp2tc_set((cbx_portid_t)portid,
                                                 pcp,
                                                 dei,
                                                 cbx_tc));
    return rv;

}

/**
 * Function:
 *    drv_avgr_custom_cosq_port_pcp2tc_get
 * Purpose:
 *    This routine is used to get the ingress mapping of {pcp,dei} to
 *    traffic class
 * Parameters:
 *    unit     (IN) Unit number.
 *    portid   (IN)  PORT Identifier
 *    pcp      (IN)  Priority Code Point (3 bit encoding)
 *    dei      (IN)  Drop Eligibility Indicator (1 bit encoding)
 *    tc       (OUT) Traffic Class structure
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */

int
drv_avgr_custom_cosq_port_pcp2tc_get(
    int unit,
    bcm_gport_t portid,
    int pcp,
    int dei,
    bcm_custom_tc_t *tc) {

    int rv = SOC_E_NONE;
    rv = (cbx_cosq_port_pcp2tc_get((cbx_portid_t)portid,
                                                 pcp,
                                                 dei,
                                                 (cbx_tc_t *) tc));
    return rv;

}

/**
 * Function:
 *    drv_avgr_custom_cosq_port_tc2pcp_set
 * Purpose:
 *    This routine is used to set the egress mapping of traffic class
 *    to {pcpi,dei}
 * Parameters:
 *    unit     (IN) Unit number.
 *    portid   (IN)  PORT Identifier
 *    tc       (IN)  Traffic Class structure
 *    pcp      (IN)  Priority Code Point (3 bit encoding)
 *    dei      (IN)  Drop Eligibility Indicator (1 bit encoding)
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */

int
drv_avgr_custom_cosq_port_tc2pcp_set(
    int unit,
    bcm_gport_t portid,
    bcm_custom_tc_t tc,
    int pcp,
    int dei) {

    int rv = SOC_E_NONE;
    cbx_tc_t cbx_tc;
    cbx_tc.int_pri = tc.int_pri;
    cbx_tc.int_dp = tc.int_dp;
    rv = (cbx_cosq_port_tc2pcp_set((cbx_portid_t)portid,
                                                  cbx_tc,
                                                  pcp,
                                                  dei));
    return rv;

}

/**
 * Function:
 *    drv_avgr_custom_cosq_port_tc2pcp_get
 * Purpose:
 *    This routine is used to get the egress mapping of traffic class
 *    to {pcp,dei}
 * Parameters:
 *    unit -   (IN) Unit number.
 *    portid   (IN)  PORT Identifier
 *    tc       (IN)  Traffic Class structure
 *    pcp      (OUT) Priority Code Point (3 bit encoding)
 *    dei      (OUT) Drop Eligibility Indicator (1 bit encoding)
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */

int
drv_avgr_custom_cosq_port_tc2pcp_get(
    int unit,
    bcm_gport_t portid,
    bcm_custom_tc_t tc,
    int *pcp,
    int *dei) {

    int rv = SOC_E_NONE;
    cbx_tc_t cbx_tc;
    cbx_tc.int_pri = tc.int_pri;
    cbx_tc.int_dp = tc.int_dp;
    rv = (cbx_cosq_port_tc2pcp_get((cbx_portid_t)portid,
                                                  cbx_tc,
                                                  pcp,
                                                  dei));
    return rv;

}

/**
 * Function:
 *    drv_avgr_custom_cosq_dscp2tc_set
 * Purpose:
 *    This routine is used to set the ingress mapping of dscp to traffic class
 * Parameters:
 *    unit     (IN) Unit number.
 *    dscp     (IN)  DiffServ structure
 *    tc       (OUT) Traffic Class structure
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */
int
drv_avgr_custom_cosq_dscp2tc_set(
    int unit,
    bcm_custom_dscp_t dscp,
    bcm_custom_tc_t tc) {

    int rv = SOC_E_NONE;
    cbx_tc_t cbx_tc;
    cbx_dscp_t cbx_dscp;
    cbx_dscp.is_ipv6 = dscp.is_ipv6;
    cbx_dscp.dscp_cp = dscp.dscp_cp;
    cbx_tc.int_pri = tc.int_pri;
    cbx_tc.int_dp = tc.int_dp;
    rv = (cbx_cosq_dscp2tc_set(cbx_dscp, cbx_tc));
    return rv;

}

/**
 * Function:
 *    drv_avgr_custom_cosq_dscp2tc_get
 * Purpose:
 *    This routine is used to get the ingress mapping of dscp to traffic class
 * Parameters:
 *    unit     (IN)  Unit number.
 *    dscp     (IN)  DiffServ structure
 *    tc       (OUT) Traffic Class structure
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */
int
drv_avgr_custom_cosq_dscp2tc_get(
    int unit,
    bcm_custom_dscp_t dscp,
    bcm_custom_tc_t *tc) {

    int rv = SOC_E_NONE;
    cbx_dscp_t cbx_dscp;
    cbx_dscp.is_ipv6 = dscp.is_ipv6;
    cbx_dscp.dscp_cp = dscp.dscp_cp;
    rv = (cbx_cosq_dscp2tc_get(cbx_dscp, (cbx_tc_t *) tc));
    return rv;

}

/**
 * Function:
 *    drv_avgr_custom_cosq_tc2dscp_set
 * Purpose:
 *    This routine is used to set the egress mapping of traffic class to dscp
 * Parameters:
 *    unit     (IN) Unit number.
 *    dscp     (IN)  DiffServ structure
 *    tc       (IN)  Traffic Class structure
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */
int
drv_avgr_custom_cosq_tc2dscp_set(
    int unit,
    bcm_custom_tc_t tc,
    bcm_custom_dscp_t dscp) {

    int rv = SOC_E_NONE;
    cbx_tc_t cbx_tc;
    cbx_dscp_t cbx_dscp;
    cbx_dscp.is_ipv6 = dscp.is_ipv6;
    cbx_dscp.dscp_cp = dscp.dscp_cp;
    cbx_tc.int_pri = tc.int_pri;
    cbx_tc.int_dp = tc.int_dp;
    rv = (cbx_cosq_tc2dscp_set(cbx_tc, cbx_dscp));
    return rv;

}

/**
 * Function:
 *    drv_avgr_custom_cosq_tc2dscp_get
 * Purpose:
 *    This routine is used to get the egress mapping of traffic class to dscp
 * Parameters:
 *    unit     (IN) Unit number.
 *    dscp     (IN)  DiffServ structure
 *    tc       (IN)  Traffic Class structure
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */
int
drv_avgr_custom_cosq_tc2dscp_get(
    int unit,
    bcm_custom_tc_t tc,
    bcm_custom_dscp_t *dscp) {

    int rv = SOC_E_NONE;
    cbx_tc_t cbx_tc;
    cbx_tc.int_pri = tc.int_pri;
    cbx_tc.int_dp = tc.int_dp;
    rv = (cbx_cosq_tc2dscp_get(cbx_tc, (cbx_dscp_t *)dscp));
    return rv;

}

/**
 * Function:
 *     drv_avgr_custom_cosq_port_sched_get
 * Purpose:
 *     This routine is used to get the COSQ scheduling of a port
 * Parameters:
 *     unit          (IN) Unit number.
 *     portid        (IN)  Port Identifier
 *     sched_mode    (OUT) COSQ Scheduling mode
 *     weights       (OUT) Array of scheduling weights
 * Returns:
 *     SOC_E_NONE Success
 *     SOC_E_XXXX Failure
 */
int
drv_avgr_custom_cosq_port_sched_get(
    int unit,
    bcm_gport_t portid,
    bcm_custom_schedule_mode_t *sched_mode,
    int *weights) {

    int rv = SOC_E_NONE;
    rv = (cbx_cosq_port_sched_get((cbx_portid_t)portid,
                                                 (cbx_schedule_mode_t *)sched_mode,
                                                 weights));
    return rv;

}

/**
 * Function:
 *     drv_avgr_custom_cosq_port_sched_set
 * Purpose:
 *     This routine is used to set the COSQ scheduling of a port
 * Parameters:
 *     unit          (IN) Unit number.
 *     portid        (IN)  Port Identifier
 *     sched_mode    (IN)  COSQ Scheduling mode
 *     weights       (IN)  Array of scheduling weights
 * Returns:
 *     SOC_E_NONE Success
 *     SOC_E_XXXX Failure
 */
int
drv_avgr_custom_cosq_port_sched_set(
    int unit,
    bcm_gport_t portid,
    bcm_custom_schedule_mode_t sched_mode,
    int *weights) {
    int rv = SOC_E_NONE;
    rv = (cbx_cosq_port_sched_set((cbx_portid_t)portid,
                                  sched_mode,
                                  weights));
    return rv;

}

/**
 * Function:
 *    drv_avgr_custom_cosq_flowcontrol_get
 * Purpose:
 *    This routine is used to get the global flowcontrol configuration
 * Parameters:
 *    unit               (IN) Unit number.
 *    flowcontrol_mode   (OUT)  Global Flow Control mode
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */

int
drv_avgr_custom_cosq_flowcontrol_get(
    int unit,
    bcm_custom_flowcontrol_t *flowcontrol_mode) {

    int rv = SOC_E_NONE;
    rv = (cbx_cosq_flowcontrol_get((cbx_flowcontrol_t *)flowcontrol_mode));
    return rv;

}

/**
 * Function:
 *    drv_avgr_custom_cosq_flowcontrol_set
 * Purpose:
 *    This routine is used to set the global flowcontrol configuration
 *
 *    There are three options in Avenger:
 *    1) Source Port based pause packets
 *      programming:
 *       PGT.fcd   set all fcd 0-7 to same value to have one count
 *       thresdholds and hysteresis in BMU_fcd_config
 *       scale: 128 of these, but only 16 (one per port) are used
 *
 *    2) PFC based on traffic class - global for all port together
 *      programming:
 *       PGT.fcd   set fcd 0-7 to traffic class 0-7
 *       thresholds and hysteresis in BMU_gfcd_config
 *       scale: 8 of these
 *    3) BMU Meters to shape per port flowcontrol
 * Parameters:
 *    unit               (IN) Unit number.
 *    flowcontrol_mode   (IN)  Global Flow Control mode
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */

int
drv_avgr_custom_cosq_flowcontrol_set(
    int unit,
    bcm_custom_flowcontrol_t flowcontrol_mode) {

    int rv = SOC_E_NONE;
    rv = (cbx_cosq_flowcontrol_set((cbx_flowcontrol_t)flowcontrol_mode));
    return rv;

}

/**
 * Function:
 *    drv_avgr_custom_cosq_port_flowcontrol_get
 * Purpose:
 *    This routine is used to get the port flowcontrol configuration
 * Parameters:
 *    Flow Control parameters include an enable, 4 XOFF/Drop thresholds,
 *    and a hysteresis (XON) threshold
 *    unit          (IN) Unit number.
 *    portid        (IN)  Port Identifier
 *    fc_params     (IN)  Flow Control parameters
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
*/
int
drv_avgr_custom_cosq_port_flowcontrol_get(
    int unit,
    bcm_gport_t portid,
    bcm_custom_flowcontrol_params_t *fc_params) {

    int rv = SOC_E_NONE;
    rv = (cbx_cosq_port_flowcontrol_get((cbx_portid_t)portid,
                                                      (cbx_flowcontrol_params_t *)fc_params));
    return rv;

}
/**
 * Function:
 *    drv_avgr_custom_cosq_port_flowcontrol_set
 * Purpose:
 *    This routine is used to set the port flowcontrol configuration
 * Parameters:
 *    Flow Control parameters include an enable, 4 XOFF/Drop thresholds,
 *    and a hysteresis (XON) threshold
 *    unit          (IN) Unit number.
 *    portid        (IN)  Port Identifier
 *    fc_params     (IN)  Flow Control parameters
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */

int
drv_avgr_custom_cosq_port_flowcontrol_set(
    int unit,
    bcm_gport_t portid,
    bcm_custom_flowcontrol_params_t fc_params) {

    int rv = SOC_E_NONE;
    cbx_flowcontrol_params_t cbx_fc_params;
    cbx_fc_params.thresh0 = fc_params.thresh0;
    cbx_fc_params.thresh1 = fc_params.thresh1;
    cbx_fc_params.thresh2 = fc_params.thresh2;
    cbx_fc_params.thresh3 = fc_params.thresh3;
    cbx_fc_params.hysteresis = fc_params.hysteresis;
    cbx_fc_params.enable = fc_params.enable;
    rv = (cbx_cosq_port_flowcontrol_set((cbx_portid_t)portid,
                                                       cbx_fc_params));
    return rv;

}

/**
 * Function:
 *    drv_avgr_custom_cosq_pfc_flowcontrol_get
 * Purpose:
 *    This routine is used to get the PFC flowcontrol configuration
 *
 *    Flow Control parameters include an enable, 4 XOFF/Drop thresholds,
 *    and a hysteresis (XON) threshold
 * Parameters:
 *    unit          (IN) Unit number.
 *    int_pri       (IN)  Internal traffic class priority
 *    fc_params     (IN)  Flow Control parameters
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */
int
drv_avgr_custom_cosq_pfc_flowcontrol_get(
    int unit,
    int int_pri,
    bcm_custom_flowcontrol_params_t *fc_params) {

    int rv = SOC_E_NONE;
    rv = (cbx_cosq_pfc_flowcontrol_get(int_pri, (cbx_flowcontrol_params_t *)fc_params));
    return rv;

}

/**
 * Function:
 *    drv_avgr_custom_cosq_pfc_flowcontrol_set
 * Purpose:
 *    This routine is used to set the PFC flowcontrol configuration
 *
 * Parameters:
 *    Flow Control parameters include an enable, 4 XOFF/Drop thresholds,
 *    and a hysteresis (XON) threshold
 *    unit          (IN) Unit number.
 *    int_pri       (IN)  Internal traffic class priority
 *    fc_params     (IN)  Flow Control parameters
 *
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */
int
drv_avgr_custom_cosq_pfc_flowcontrol_set(
    int unit,
    int int_pri,
    bcm_custom_flowcontrol_params_t fc_params) {

    int rv = SOC_E_NONE;
    cbx_flowcontrol_params_t cbx_fc_params;
    cbx_fc_params.thresh0 = fc_params.thresh0;
    cbx_fc_params.thresh1 = fc_params.thresh1;
    cbx_fc_params.thresh2 = fc_params.thresh2;
    cbx_fc_params.thresh3 = fc_params.thresh3;
    cbx_fc_params.hysteresis = fc_params.hysteresis;
    cbx_fc_params.enable = fc_params.enable;
    rv = (cbx_cosq_pfc_flowcontrol_set(int_pri, cbx_fc_params));
    return rv;

}

/**
 * Function:
 *    drv_avgr_custom_cosq_port_shaper_get
 * Purpose:
 *    This routine is used to get an egress shaper on a port.
 *
 * Parameters:
 *    unit           (IN) Unit number.
 *    portid         (IN)  PORT Identifier
 *    shaper_params  (OUT) Shaper parameters.
 *
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */
int
drv_avgr_custom_cosq_port_shaper_get(
    int unit,
    bcm_gport_t portid,
    bcm_custom_cosq_shaper_params_t *shaper_params) {

    int rv = SOC_E_NONE;
    rv = (cbx_cosq_port_shaper_get((cbx_portid_t)portid,
                                                 (cbx_cosq_shaper_params_t *)shaper_params));
    return rv;

}

/**
 * Function:
 *    drv_avgr_custom_cosq_port_shaper_set
 * Purpose:
 *    This routine is used to set an egress shaper on a port.
 *
 * Parameters:
 *    unit           (IN) Unit number.
 *    portid         (IN)  PORT Identifier
 *    shaper_params  (IN)  Shaper parameters.
 *
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */
int
drv_avgr_custom_cosq_port_shaper_set(
    int unit,
    bcm_gport_t portid,
    bcm_custom_cosq_shaper_params_t *shaper_params) {

    int rv = SOC_E_NONE;
    cbx_cosq_shaper_params_t cbx_shaper_params;
    cbx_shaper_params.flags = shaper_params->flags;
    cbx_shaper_params.bits_sec = shaper_params->bits_sec;
    cbx_shaper_params.max_bits_sec = shaper_params->max_bits_sec;
    cbx_shaper_params.bits_burst = shaper_params->bits_burst;
    cbx_shaper_params.bits_burst = shaper_params->bits_burst;

    rv = (cbx_cosq_port_shaper_set((cbx_portid_t)portid,
                                    &cbx_shaper_params));
    return rv;

}

/**
 * Function:
 *    drv_avgr_custom_cosq_queue_shaper_get
 * Purpose:
 *    This routine is used to get an egress shaper on a qos queue.
 *
 * Parameters:
 *    unit           (IN) Unit number.
 *    portid         (IN)  PORT Identifier
 *    int_pri        (IN)  Internal traffic class priority
 *    shaper_params  (OUT) Shaper parameters.
 *
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */

int
drv_avgr_custom_cosq_queue_shaper_get(
    int unit,
    bcm_gport_t portid,
    int int_pri,
    bcm_custom_cosq_shaper_params_t *shaper_params) {

    int rv = SOC_E_NONE;
    rv = (cbx_cosq_queue_shaper_get((cbx_portid_t)portid,
                                                  int_pri,
                                                  (cbx_cosq_shaper_params_t *)shaper_params));
    return rv;
}

/**
 * Function:
 *    drv_avgr_custom_cosq_queue_shaper_set
 * Purpose:
 *    This routine is used to set an egress shaper on a qos queue.
 *
 * Parameters:
 *    unit           (IN) Unit number.
 *    portid         (IN)  PORT Identifier
 *    int_pri        (IN)  Internal traffic class priority
 *    shaper_params  (IN)  Shaper parameters.
 *
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */

int
drv_avgr_custom_cosq_queue_shaper_set(
    int unit,
    bcm_gport_t portid,
    int int_pri,
    bcm_custom_cosq_shaper_params_t *shaper_params) {

    int rv = SOC_E_NONE;
    cbx_cosq_shaper_params_t cbx_shaper_params;
    cbx_shaper_params.flags = shaper_params->flags;
    cbx_shaper_params.bits_sec = shaper_params->bits_sec;
    cbx_shaper_params.max_bits_sec = shaper_params->max_bits_sec;
    cbx_shaper_params.bits_burst = shaper_params->bits_burst;
    cbx_shaper_params.bits_burst = shaper_params->bits_burst;

    rv = (cbx_cosq_queue_shaper_set((cbx_portid_t)portid,
                                     int_pri,
                                     &cbx_shaper_params));
    return rv;
}

/**
 * Function:
 *      drv_avgr_custom_cosq_tsn_params_t_init
 * Purpose:
 *      Initialize TSN parameters.
 * Parameters:
 *      unit         (IN) Unit number.
 *      tsn_params - (OUT) Parameter object
 * Returns:
 *      SOC_E_XXX
 */
int
drv_avgr_custom_cosq_tsn_params_t_init(
    int unit,
    bcm_custom_cosq_tsn_params_t *tsn_params) {
#ifdef CONFIG_TIMESYNC
    cbx_cosq_tsn_params_t_init((cbx_cosq_tsn_params_t *)tsn_params);
#endif
    return SOC_E_NONE;
}

/**
 * Function:
 *    drv_avgr_custom_cosq_queue_tsn_get
 * Purpose:
 *    This routine is used to get an TSN parameters on a qos queue.
 *
 * Parameters:
 *    unit           (IN)  Unit number.
 *    portid         (IN)  PORT Identifier
 *    int_pri        (IN)  Internal traffic class priority
 *    tsn_params     (OUT) TSN parameters.
 *
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */

int
drv_avgr_custom_cosq_queue_tsn_get(
    int unit,
    bcm_gport_t portid,
    int int_pri,
    bcm_custom_cosq_tsn_params_t *tsn_params) {

    int rv = SOC_E_NONE;
#ifdef CONFIG_TIMESYNC
    rv = (cbx_cosq_queue_tsn_config_get((cbx_portid_t)portid, int_pri,
                                 (cbx_cosq_tsn_params_t *)tsn_params));
#endif
    return rv;
}

/**
 * Function:
 *    drv_avgr_custom_cosq_queue_tsn_set
 * Purpose:
 *    This routine is used to set an TSN parameters on a qos queue.
 *
 * Parameters:
 *    unit           (IN) Unit number.
 *    portid         (IN)  PORT Identifier
 *    int_pri        (IN)  Internal traffic class priority
 *    tsn_params     (IN)  TSN parameters.
 *
 * Returns:
 *    SOC_E_NONE Success
 *    SOC_E_XXXX Failure
 */

int
drv_avgr_custom_cosq_queue_tsn_set(
    int unit,
    bcm_gport_t portid,
    int int_pri,
    bcm_custom_cosq_tsn_params_t *tsn_params) {

    int rv = SOC_E_NONE;
#ifdef CONFIG_TIMESYNC
    cbx_cosq_tsn_params_t cbx_tsn_params;

    cbx_tsn_params.cfg_id = tsn_params->cfg_id;
    cbx_tsn_params.flags = tsn_params->flags;
    cbx_tsn_params.sync_source = tsn_params->sync_source;
    cbx_tsn_params.tick_period = tsn_params->tick_period;
    cbx_tsn_params.start_state = tsn_params->start_state;
    cbx_tsn_params.start_duration = tsn_params->start_duration;
    cbx_tsn_params.gb_duration = tsn_params->gb_duration;
    cbx_tsn_params.hp_duration = tsn_params->hp_duration;
    cbx_tsn_params.lp_duration = tsn_params->lp_duration;

    rv = (cbx_cosq_queue_tsn_config_set((cbx_portid_t)portid,
                                     int_pri,
                                     &cbx_tsn_params));
#endif
    return rv;
}

