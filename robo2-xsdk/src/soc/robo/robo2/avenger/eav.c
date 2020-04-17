/*
 * $Id: eav.c $
 *
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <soc/error.h>
#include <fsal_int/shared_types.h>
#include <fsal/ptp.h>
#include <fsal/port.h>
#include <fsal/cosq.h>
#include <fsal_int/types.h>
#include <fsal_int/cosq.h>
#include <fsal_int/slic.h>
#include <fsal_int/trap.h>
#include <sal/appl/io.h>

extern uint8 cbxi_ptp_eth_p2p_mac_addr[6];
extern int cbxi_slicid_map(int unit, cbxi_slic_rule_id_t rule_number,
                           cbxi_slicid_t slicid, cbxi_slic_trap_t slic_trap);
void cbxi_ptp_port_admin_get(uint8 unit, cbx_port_t port, uint8 *admin);
/*
 * Function:
 *     drv_avgr_eav_enable_set
 * Purpose:
 *     Enables/Disables the port PTP configuration.
 * Parameters:
 *     unit     (IN) Unit number.
 *     portid   (IN) Port Identifier
 *     enable   (IN) Admin status
 * Returns:
 *     SOC_E_XXX
 */
int drv_avgr_eav_enable_set(int unit, uint32 port, uint32 enable) {
    int rc;
    cbx_tc_t tc;
    int remapped_pcp = 0;
    int priority;

    rc = cbx_ptp_port_admin_set(port, (uint8)enable);

    if ((rc == SOC_E_NONE) && (!enable)) {
        tc.int_dp = 0;
        for (priority = 0; priority <= CBXI_COSQ_MAX_INT_PRI; priority++) {
            tc.int_pri = priority;
            rc = cbx_cosq_port_pcp2tc_set((cbx_portid_t)port, priority, 0, tc);
            if (rc != SOC_E_NONE) {
                return rc;
            }
            rc = cbx_cosq_port_tc2pcp_set((cbx_portid_t)port, tc,
                                           remapped_pcp, 0);
            if (rc != SOC_E_NONE) {
                return rc;
            }
        }
    }
    return rc;
}

/*
 * Function:
 *     drv_avgr_eav_enable_get
 * Purpose:
 *     Gets the admin state of port PTP configurtion.
 * Parameters:
 *     unit     (IN) Unit number.
 *     portid   (IN) Port Identifier
 *     enable   (IN/OUT) Admin status
 * Returns:
 *     SOC_E_XXX
 */
int drv_avgr_eav_enable_get(int unit, uint32 port, uint32 *enable) {
    uint8 admin;
    int rc;

    rc = cbx_ptp_port_admin_get(port, &admin);
    *enable = admin;
    return rc;
}

/*
 * Function:
 *     drv_avgr_eav_egress_timestamp_get
 * Purpose:
 *     Gets the timestamp of last egress PTP packet on the port.
 * Parameters:
 *     unit      (IN) Unit number.
 *     portid    (IN) Port Identifier
 *     timestamp (IN/OUT) Timestamp value
 * Returns:
 *     SOC_E_XXX
 */
int drv_avgr_eav_egress_timestamp_get(int unit, uint32 port, uint32 *timestamp) {
    uint32 seq_id;
    uint32 ts = 0;
    uint8 index;
    int rc;

    for (index = 0; index < CBX_TS_FIFO_DEPTH; index++) {
        *timestamp = ts;
        /* Read the time stamp for the last egress packet */
        rc = cbx_ptp_timestamp_get(port, &ts, &seq_id);
        if (rc == SOC_E_NONE) {
            if ((ts == 0) || (seq_id == 0)) {
               break;
            }
        }
    }
    return rc;
}

/*
 * Function:
 *     drv_avgr_eav_link_status_get
 * Purpose:
 *     Gets the link status of EAV port
 * Parameters:
 *     unit     (IN) Unit number.
 *     portid   (IN) Port Identifier
 *     link     (IN/OUT) Link status
 * Returns:
 *     SOC_E_XXX
 */
int drv_avgr_eav_link_status_get(int unit, uint32 port, uint32 *link) {
    uint32 status;
    int rc;

    rc = cbx_port_attribute_get(port, cbxPortAttrLinkStatus, &status);
    *link = status;
    return rc;
}

/*
 * Function:
 *     drv_avgr_eav_timesync_mac_set
 * Purpose:
 *     Set the destination MAC address of time sync protocol
 * Parameters:
 *     unit        (IN) Unit number.
 *     mac         (IN) MAC address
 *     ether_type  (IN) Ether type  (not used)
 * Returns:
 *     SOC_E_XXX
 */
int drv_avgr_eav_timesync_mac_set(int unit, uint8 *mac, uint16 ether_type) {

    uint8 rule_num;
    sal_memcpy(cbxi_ptp_eth_p2p_mac_addr, mac, 6);

    for (rule_num = SLIC_PTP_ETH_P2P_PORT_0;
         rule_num < SLIC_PTP_ETH_P2P_PORT_15; rule_num++) {
        CBX_IF_ERROR_RETURN(cbxi_slic_rule_dmac_set(unit, rule_num, mac));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *     drv_avgr_eav_timesync_mac_get
 * Purpose:
 *     Get the destination MAC address of time sync protocol
 * Parameters:
 *     unit        (IN) Unit number.
 *     mac         (IN) MAC address
 *     ether_type  (IN/OUT) Ether type (not used)
 * Returns:
 *     SOC_E_XXX
 */
int drv_avgr_eav_timesync_mac_get(int unit, uint8 *mac, uint16 *ether_type) {

    sal_memcpy(mac, cbxi_ptp_eth_p2p_mac_addr, 6);
    return SOC_E_NONE;
}

/*
 * Function:
 *     drv_avgr_eav_srp_mac_ethertype_set
 * Purpose:
 *     Set the MAC address and ethertype used to trap SRP protocol packets
 * Parameters:
 *     unit        (IN) Unit number.
 *     mac         (IN) MAC address
 *     ether_type  (IN) Ether type
 * Returns:
 *     SOC_E_XXX
 */
int drv_avgr_eav_srp_mac_ethertype_set(int unit, uint8 *mac, uint16 ether_type) {

    CBX_IF_ERROR_RETURN(cbxi_soft_tag_user_etype_set(unit, ether_type));
    CBX_IF_ERROR_RETURN(cbxi_slic_rule_dmac_set(unit,SLIC_SRP,mac));
    CBX_IF_ERROR_RETURN(cbxi_slicid_map(CBX_AVENGER_PRIMARY, SLIC_SRP,
                                         SLICID_SRP, SLIC_TRAP_GROUP2));
    return SOC_E_NONE;
}

/*
 * Function:
 *     drv_avgr_eav_srp_mac_ethertype_get
 * Purpose:
 *     Get the MAC address and ethertype used to trap SRP protocol packets
 * Parameters:
 *     unit        (IN) Unit number.
 *     mac         (IN) MAC address
 *     ether_type  (IN/OUT) Ether type
 * Returns:
 *     SOC_E_XXX
 */
int drv_avgr_eav_srp_mac_ethertype_get(int unit, uint8 *mac,
                                       uint16 *ether_type) {

    CBX_IF_ERROR_RETURN(cbxi_soft_tag_user_etype_get(unit, ether_type));
    CBX_IF_ERROR_RETURN(cbxi_slic_rule_dmac_get(unit,SLIC_SRP,mac));
    return SOC_E_NONE;
}

/*
 * Function:
 *     drv_avgr_eav_pcp_mapping_set
 * Purpose:
 *     Set the PCP(Priority) value mapping for class A or B type streams
 * Parameters:
 *     unit        (IN) Unit number.
 *     type        (IN) Stream class A or B
 *     pcp         (IN) Priority for the class
 *     remapped_pcp (IN) Remapped priority at the boundary port
 * Returns:
 *     SOC_E_XXX
 */
int drv_avgr_eav_pcp_mapping_set(int unit, uint32 type, int pcp,
                                 int remapped_pcp) {

    int rc = SOC_E_NONE;
    cbx_tc_t tc;
    cbx_tc_t tc_temp;
    uint32 port;
    uint8 admin;
    uint8 eav_enabled = 0;
    int priority;

    tc.int_dp = 0;
    if (type == DRV_EAV_STREAM_CLASSA) {
        tc.int_pri = CBXI_AVB_CLASS_A_COSQ;
    } else if (type == DRV_EAV_STREAM_CLASSB){
        tc.int_pri = CBXI_AVB_CLASS_B_COSQ;
    } else {
       sal_printf("Unsupported class type %d\n", type);
       return SOC_E_PARAM;
    }
    for (port = 0; port < CBX_MAX_PORT_PER_UNIT; port++) {
        cbxi_ptp_port_admin_get(unit, port, &admin);
        if(admin) {
            /* check if any of the existing pcp is mapped to given traffic
             * class. If there are any update the TC to zero for them */
            for (priority = 0; priority <= CBXI_COSQ_MAX_INT_PRI; priority++) {
                rc = cbx_cosq_port_pcp2tc_get((cbx_portid_t)port, priority,
                                               0, &tc_temp);
                if (rc != SOC_E_NONE) {
                    return rc;
                }
                if (tc.int_pri == tc_temp.int_pri) {
                    tc_temp.int_pri = 0;
                    tc_temp.int_dp = 0;
                    rc = cbx_cosq_port_pcp2tc_set((cbx_portid_t)port, priority,
                                                   0, tc_temp);
                    if (rc != SOC_E_NONE) {
                        return rc;
                    }
                }
            }
            rc = cbx_cosq_port_pcp2tc_set((cbx_portid_t)port, pcp, 0, tc);
            if (rc != SOC_E_NONE) {
                return rc;
            }
            rc = cbx_cosq_port_tc2pcp_set((cbx_portid_t)port, tc,
                                           remapped_pcp, 0);
            if (rc != SOC_E_NONE) {
                return rc;
            }
            eav_enabled = 1;
        }
    }
    if (!eav_enabled) {
       sal_printf("EAV needs to be enabled in atleast one of the port for\
                  this setting \n");
    }
    return rc;
}

/*
 * Function:
 *     drv_avgr_eav_pcp_mapping_get
 * Purpose:
 *     Get the PCP(Priority) value mapping for class A or B type streams
 * Parameters:
 *     unit        (IN) Unit number.
 *     portid      (IN) Port Identifier
 *     type        (IN) Stream class A or B
 *     pcp         (OUT) Priority for the class
 *     remapped_pcp (OUT) Remapped priority at the boundary port
 * Returns:
 *     SOC_E_XXX
 */
int drv_avgr_eav_pcp_mapping_get(int unit, uint32 type,
                                 int *pcp, int *remapped_pcp) {

    int rc = SOC_E_NONE;
    int priority;
    int remap_priority;
    int dei;
    uint32 port;
    uint8 admin;
    cbx_tc_t tc;
    cbx_tc_t tc_temp;
    uint8 eav_enabled = 0;

    if (type == DRV_EAV_STREAM_CLASSA) {
        tc.int_pri = CBXI_AVB_CLASS_A_COSQ;
    } else if (type == DRV_EAV_STREAM_CLASSB) {
        tc.int_pri = CBXI_AVB_CLASS_B_COSQ;
    } else {
       sal_printf("Unsupported class type %d\n", type);
       return SOC_E_PARAM;
    }
    tc.int_dp = 0;
    *pcp = 0;
    *remapped_pcp = 0;
    for (port = 0; port < CBX_MAX_PORT_PER_UNIT; port++) {
        cbxi_ptp_port_admin_get(unit, port, &admin);
        if (admin) {
            for (priority = 0; priority <= CBXI_COSQ_MAX_INT_PRI; priority++) {
                rc = cbx_cosq_port_pcp2tc_get((cbx_portid_t)port, priority,
                                               0, &tc_temp);
                if (rc != SOC_E_NONE) {
                    return rc;
                }
                if (tc.int_pri == tc_temp.int_pri) {
                    *pcp = priority;
                    break;
                }
            }

            rc = cbx_cosq_port_tc2pcp_get((cbx_portid_t)port, tc,
                                           &remap_priority, &dei);
            if (rc != SOC_E_NONE) {
                return rc;
            }
            *remapped_pcp = remap_priority;
            eav_enabled = 1;
            break;
        }
    }
    if (!eav_enabled) {
        sal_printf("EAV is not enabled in any of the ports \n");
    }

    return rc;
}

/*
 * Function:
 *     drv_avgr_eav_bandwidth_set
 * Purpose:
 *     Set the bandwidth parameters for class A or B type traffic streams
 * Parameters:
 *     unit        (IN) Unit number.
 *     portid      (IN) Port Identifier
 *     type        (IN) Stream class A or B
 *     bytes_sec   (IN) Reserved bandwidth in bytes per second
 *     bytes_burst (IN) Maximum burst size in bytes
 * Returns:
 *     SOC_E_XXX
 */
int drv_avgr_eav_bandwidth_set(int unit, uint32 port, uint32 type,
                               uint32 bytes_sec, uint32 bytes_burst) {

    int rc = SOC_E_NONE;
    cbx_cosq_shaper_params_t shaper_params;
    uint32 int_pri;

    if (type == DRV_EAV_STREAM_CLASSA) {
       int_pri = CBXI_AVB_CLASS_A_COSQ;
    } else if (type == DRV_EAV_STREAM_CLASSB) {
       int_pri = CBXI_AVB_CLASS_B_COSQ;
    } else {
       sal_printf("Unsupported class type %d\n", type);
       return SOC_E_PARAM;
    }
    rc = cbx_cosq_queue_shaper_get((cbx_portid_t)port,
                                   int_pri, &shaper_params);
    if (rc == SOC_E_NONE) {
        shaper_params.flags = CBX_COSQ_SHAPER_MODE_AVB;
        shaper_params.bits_sec = (bytes_sec * 8)/1000;
        shaper_params.bits_burst = (bytes_burst * 8)/1000;

        rc = (cbx_cosq_queue_shaper_set((cbx_portid_t)port,
                                         int_pri, &shaper_params));
    }
    return rc;
}

/*
 * Function:
 *     drv_avgr_eav_bandwidth_get
 * Purpose:
 *     Get the bandwidth parameters for class A or B type traffic streams
 * Parameters:
 *     unit        (IN) Unit number.
 *     portid      (IN) Port Identifier
 *     type        (IN) Stream class A or B
 *     bytes_sec   (OUT) Reserved bandwidth in bytes per second
 *     bytes_burst (OUT) Maximum burst size in bytes
 * Returns:
 *     SOC_E_XXX
 */
int drv_avgr_eav_bandwidth_get(int unit, uint32 port, uint32 type,
                               uint32 *bytes_sec, uint32 *bytes_burst) {
    int rc = SOC_E_NONE;
    cbx_cosq_shaper_params_t shaper_params;
    uint32 int_pri;

    if (type == DRV_EAV_STREAM_CLASSA) {
       int_pri = CBXI_AVB_CLASS_A_COSQ;
    } else if (type == DRV_EAV_STREAM_CLASSB) {
       int_pri = CBXI_AVB_CLASS_B_COSQ;
    } else {
       sal_printf("Unsupported class type %d\n", type);
       return SOC_E_PARAM;
    }
    rc = cbx_cosq_queue_shaper_get((cbx_portid_t)port,
                                   int_pri, &shaper_params);
    if (rc == SOC_E_NONE) {
        *bytes_sec = (shaper_params.bits_sec/8) * 1000;
        *bytes_burst = (shaper_params.bits_burst/8) * 1000;
    }
    return rc;
}
