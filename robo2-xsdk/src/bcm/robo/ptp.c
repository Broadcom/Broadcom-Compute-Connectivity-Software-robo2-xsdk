/*
 * $Id: $
 *
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * PTP Module Implementation
 */

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/ptp.h>
#include <appl/dot1as/dot1as.h>

#ifdef INCLUDE_DOT1AS

/*
 * Function: bcm_robo_ptp_clock_port_enable
 * Purpose:
 *     This routine enable PTP Clock Port
 * Parameters:
 *     unit     (IN) BCM device number
 *     ptp_id   (IN) PTP stack id
 *     clk_num  (IN) PTP clock number
 *     port     (IN) Port number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_ptp_clock_port_enable(int unit, bcm_ptp_stack_id_t ptp_id,
                               int clk_num, uint32 port)
{
    return dot1asIntfModeSet(port, 1);
}

/*
 * Function: bcm_robo_ptp_clock_port_disable
 * Purpose:
 *     This routine disable PTP Clock Port
 * Parameters:
 *     unit     (IN) BCM device number
 *     ptp_id   (IN) PTP stack id
 *     clk_num  (IN) PTP clock number
 *     port     (IN) Port number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_ptp_clock_port_disable(int unit, bcm_ptp_stack_id_t ptp_id,
                                int clk_num, uint32 port)
{
    return dot1asIntfModeSet(port, 0);
}

/*
 * Function: bcm_robo_ptp_clock_priority1_set
 * Purpose:
 *     Set PTP Clock priority 1.
 * Parameters:
 *     unit     (IN) BCM device number
 *     ptp_id   (IN) PTP stack id
 *     clk_num  (IN) PTP clock number
 *     value    (IN) priority value
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_ptp_clock_priority1_set(int unit,bcm_ptp_stack_id_t ptp_id,
                                 int clk_num, uint32 value)
{
    return dot1asBridgePrioritySet(DOT1AS_CLOCK_PRIORITY1, value);
}

/*
 * Function: bcm_robo_ptp_clock_priority1_get
 * Purpose:
 *     Get PTP Clock priority 1.
 * Parameters:
 *     unit     (IN) BCM device number
 *     ptp_id   (IN) PTP stack id
 *     clk_num  (IN) PTP clock number
 *     value    (IN/OUT) priority value
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_ptp_clock_priority1_get(int unit,bcm_ptp_stack_id_t ptp_id,
                                     int clk_num, uint32 *value)
{
    return dot1asBridgePriorityGet(DOT1AS_CLOCK_PRIORITY1, value);
}

/*
 * Function: bcm_robo_ptp_clock_priority2_set
 * Purpose:
 *     Set PTP Clock priority 2.
 * Parameters:
 *     unit     (IN) BCM device number
 *     ptp_id   (IN) PTP stack id
 *     clk_num  (IN) PTP clock number
 *     value    (IN) priority value
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_ptp_clock_priority2_set(int unit,bcm_ptp_stack_id_t ptp_id,
                                     int clk_num, uint32 value)
{
    return dot1asBridgePrioritySet(DOT1AS_CLOCK_PRIORITY2, value);
}

/*
 * Function: bcm_robo_ptp_clock_priority2_get
 * Purpose:
 *     Get PTP Clock priority 2.
 * Parameters:
 *     unit     (IN) BCM device number
 *     ptp_id   (IN) PTP stack id
 *     clk_num  (IN) PTP clock number
 *     value    (IN/OUT) priority value
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_ptp_clock_priority2_get(int unit,bcm_ptp_stack_id_t ptp_id,
                                     int clk_num, uint32 *value)
{
    return dot1asBridgePriorityGet(DOT1AS_CLOCK_PRIORITY2, value);
}

/*
 * Function: bcm_robo_ptp_clock_port_log_min_pdelay_req_interval_set
 * Purpose:
 *     Set PTP clock port minimum log Pdelay Request Interval.
 * Parameters:
 *     unit     (IN) BCM device number
 *     ptp_id   (IN) PTP stack id
 *     clk_num  (IN) PTP clock number
 *     port     (IN) Port number
 *     value    (IN) pdelay interval value
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_ptp_clock_port_log_min_pdelay_req_interval_set(int unit,
             bcm_ptp_stack_id_t ptp_id, int clk_num, uint32 port, int value)
{
   return dot1asIntfIntervalSet(DOT1AS_INITIAL_PDELAY_TX_INTERVAL, port, value);
}

/*
 * Function: bcm_robo_ptp_clock_port_log_min_pdelay_req_interval_get
 * Purpose:
 *     Get PTP clock port minimum log Pdelay Request Interval.
 * Parameters:
 *     unit     (IN) BCM device number
 *     ptp_id   (IN) PTP stack id
 *     clk_num  (IN) PTP clock number
 *     port     (IN) Port number
 *     value    (IN/OUT) pdelay interval value
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_ptp_clock_port_log_min_pdelay_req_interval_get(int unit,
           bcm_ptp_stack_id_t ptp_id, int clk_num, uint32 port, int *value)
{
   return dot1asIntfIntervalGet(DOT1AS_INITIAL_PDELAY_TX_INTERVAL, port, value);
}

/*
 * Function: bcm_robo_ptp_clock_port_log_announce_interval_set
 * Purpose:
 *     Set PTP Clock Port Log Announce Interval.
 * Parameters:
 *     unit     (IN) BCM device number
 *     ptp_id   (IN) PTP stack id
 *     clk_num  (IN) PTP clock number
 *     port     (IN) Port number
 *     value    (IN) announce interval value
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_ptp_clock_port_log_announce_interval_set(int unit,
            bcm_ptp_stack_id_t ptp_id, int clk_num, uint32 port, int value)
{
   return dot1asIntfIntervalSet(DOT1AS_INITIAL_ANNOUNCE_TX_INTERVAL, port, value);
}

/*
 * Function: bcm_robo_ptp_clock_port_log_announce_interval_get
 * Purpose:
 *     Get PTP Clock Port Log Announce Interval.
 * Parameters:
 *     unit     (IN) BCM device number
 *     ptp_id   (IN) PTP stack id
 *     clk_num  (IN) PTP clock number
 *     port     (IN) Port number
 *     value    (IN/OUT) announce interval value
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_ptp_clock_port_log_announce_interval_get(int unit,
           bcm_ptp_stack_id_t ptp_id, int clk_num, uint32 port, int *value)
{
   return dot1asIntfIntervalGet(DOT1AS_INITIAL_ANNOUNCE_TX_INTERVAL, port, value);
}

/*
 * Function: bcm_robo_ptp_clock_port_log_sync_interval_set
 * Purpose:
 *     Set PTP Clock Port Log Sync Interval.
 * Parameters:
 *     unit     (IN) BCM device number
 *     ptp_id   (IN) PTP stack id
 *     clk_num  (IN) PTP clock number
 *     port     (IN) Port number
 *     value    (IN) sync interval value
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_ptp_clock_port_log_sync_interval_set(int unit,
            bcm_ptp_stack_id_t ptp_id, int clk_num, uint32 port, int value)
{
   return dot1asIntfIntervalSet(DOT1AS_INITIAL_SYNC_TX_INTERVAL, port, value);
}

/*
 * Function: bcm_robo_ptp_clock_port_log_sync_interval_get
 * Purpose:
 *     Get PTP Clock Port Log Sync Interval.
 * Parameters:
 *     unit     (IN) BCM device number
 *     ptp_id   (IN) PTP stack id
 *     clk_num  (IN) PTP clock number
 *     port     (IN) Port number
 *     value    (IN/OUT) sync interval value
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_ptp_clock_port_log_sync_interval_get(int unit,
           bcm_ptp_stack_id_t ptp_id, int clk_num, uint32 port, int *value)
{
   return dot1asIntfIntervalGet(DOT1AS_INITIAL_SYNC_TX_INTERVAL, port, value);
}

/*
 * Function: bcm_robo_ptp_clock_port_announce_receipt_timeout_set
 * Purpose:
 *     Set PTP Clock Port Announce Receipt Timeout.
 * Parameters:
 *     unit     (IN) BCM device number
 *     ptp_id   (IN) PTP stack id
 *     clk_num  (IN) PTP clock number
 *     port     (IN) Port number
 *     value    (IN) time out value
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_ptp_clock_port_announce_receipt_timeout_set(int unit,
         bcm_ptp_stack_id_t ptp_id, int clk_num, uint32 port, uint32 value)
{
   int rc, timeout_val;

   timeout_val = value;
   rc = dot1asIntfIntervalSet(DOT1AS_ANNOUNCE_RX_TIMEOUT, port, timeout_val);
   return rc;
}

/*
 * Function: bcm_robo_ptp_clock_port_announce_receipt_timeout_get
 * Purpose:
 *     Get PTP Clock Port Announce Receipt Timeout.
 * Parameters:
 *     unit     (IN) BCM device number
 *     ptp_id   (IN) PTP stack id
 *     clk_num  (IN) PTP clock number
 *     port     (IN) Port number
 *     value    (IN/OUT) time out value
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_ptp_clock_port_announce_receipt_timeout_get(int unit,
        bcm_ptp_stack_id_t ptp_id, int clk_num, uint32 port, uint32 *value)
{
   int rc, timeout_val;

   rc = dot1asIntfIntervalGet(DOT1AS_ANNOUNCE_RX_TIMEOUT, port, &timeout_val);
   *value = (uint32)timeout_val;
   return rc;
}

/*
 * Function: bcm_robo_custom_dot1as_debug_bmca_show
 * Purpose:
 *     Shows the 802.1as BMCA information
 * Parameters:
 *     unit  (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_custom_dot1as_debug_bmca_show(int unit)
{
    dot1asDebugBmcaShow();
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_dot1as_debug_pdelay_show
 * Purpose:
 *     Shows the 802.1as peer delay information.
 * Parameters:
 *     unit  (IN) BCM device number
 *     intf  (IN) Interface number
 *     showlog  (IN) Display log
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_custom_dot1as_debug_pdelay_show(int unit, uint32 intf, uint8 showlog)
{
    dot1asDebugPdelayShow(intf, showlog);
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_dot1as_debug_timesync_show
 * Purpose:
 *     Shows the 802.1as timesync information.
 * Parameters:
 *     unit    (IN) BCM device number
 *     verbose (IN) TRUE to dump log
 *     num     (IN) number of log entries
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_custom_dot1as_debug_timesync_show(int unit, uint8 verbose, uint32 num)
{
    dot1asDebugTimesyncShow(verbose, num);
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_dot1as_debug_intf_stats_show
 * Purpose:
 *     shows the 802.1as interface statistics
 * Parameters:
 *     unit   (IN) BCM device number
 *     intf   (IN) Interface number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_custom_dot1as_debug_intf_stats_show(int unit, uint32 intf)
{
    dot1asDebugIntfStatsShow(intf);
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_dot1as_debug_intf_status_show
 * Purpose:
 *     Shows the 802.1as interface status
 * Parameters:
 *     unit   (IN) BCM device number
 *     intf   (IN) Interface number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_custom_dot1as_debug_intf_status_show(int unit, uint32 intf)
{
    dot1asDebugIntfStatusShow(intf);
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_dot1as_debug_intf_stats_reset
 * Purpose:
 *     Resets the 802.1as interface statistics
 * Parameters:
 *     unit - (IN) BCM device number
 *     intf - (IN) Interface number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_custom_dot1as_debug_intf_stats_reset(int unit, uint32 intf)
{
    return dot1asIntfStatsReset(intf);
}

/*
 * Function: bcm_robo_custom_dot1as_debug_intl_show
 * Purpose:
 *     Shows the 802.1as timer intervals information
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_custom_dot1as_debug_intl_show(int unit)
{
    return dot1asDebugIntlShow();
}

/*
 * Function: bcm_robo_custom_dot1as_debug_flags_set
 * Purpose:
 *     Sets the 802.1as debug flags
 * Parameters:
 *     unit - (IN) BCM device number
 *     flags - (IN) Debug mask
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_robo_custom_dot1as_debug_flags_set(int unit, uint32 flags)
{
    dot1asDebugFlagsSet(flags);
    return BCM_E_NONE;
}

#endif /* INCLUDE_DOT1AS */
