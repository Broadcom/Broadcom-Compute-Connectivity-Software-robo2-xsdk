/*
 * $Id: port.c,v 1.4 Broadcom SDK $
 *
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <soc/robo.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/robo_stat.h>
#include <fsal_int/shared_types.h>
#include <fsal/port.h>
#include <fsal/cosq.h>
#include <fsal/cable_diag.h>
#include <avenger_service.h>
#include <shared/bsl.h>
#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phyreg.h>
#include <fsal_int/init.h>
#include <fsal_int/port.h>
#include <fsal_int/trap.h>
#include "../../common/robo_common.h"


extern int soc_robo2_pkt_flow_enable();

#define FSAL_CABLE_DIAG

#ifdef FSAL_CABLE_DIAG
int drv_avgr_cable_diag_fsal2bcm_convert(cbx_port_cable_diag_t port_result, bcm_port_cable_diag_t *bcm_port_result)
{
    uint8 pair;
    uint8 faultOpen = 0;
    uint8 faultShort = 0;

    if (port_result.port_cdiag_result == cbxCdiagResultNill ) {
    } else if (port_result.port_cdiag_result == cbxCdiagResultBusy ) {
        bcm_port_result->state = _SHR_PORT_CABLE_STATE_CROSSTALK;
    } else if (port_result.port_cdiag_result == cbxCdiagResultInvalid ) {
        bcm_port_result->state = _SHR_PORT_CABLE_STATE_UNKNOWN;
    } else if (port_result.port_cdiag_result == cbxCdiagResultPass ) {
        bcm_port_result->state = _SHR_PORT_CABLE_STATE_OK;
    } else if (port_result.port_cdiag_result == cbxCdiagResultFail ) {
        bcm_port_result->state = _SHR_PORT_CABLE_STATE_UNKNOWN;
    }
/*
        bcm_port_result->state = _SHR_PORT_CABLE_STATE_OPEN;
        bcm_port_result->state = _SHR_PORT_CABLE_STATE_SHORT;
        bcm_port_result->state = _SHR_PORT_CABLE_STATE_OPENSHORT;
*/
    bcm_port_result->npairs = 4;
    for (pair = 0; pair < 4; pair++) {
        if (port_result.pair_state[pair] == cbxCableStateInvalid) {
            bcm_port_result->pair_state[pair] = _SHR_PORT_CABLE_STATE_UNKNOWN;
        } else if (port_result.pair_state[pair] == cbxCableStateOK)  {
            bcm_port_result->pair_state[pair] = _SHR_PORT_CABLE_STATE_OK;
        } else if (port_result.pair_state[pair] == cbxCableStateOpen) {
            faultOpen = 1;
            bcm_port_result->pair_state[pair] = _SHR_PORT_CABLE_STATE_OPEN;
        } else if (port_result.pair_state[pair] == cbxCableStateIntraShort) {
            faultShort = 1;
            bcm_port_result->pair_state[pair] = _SHR_PORT_CABLE_STATE_SHORT;
        } else if (port_result.pair_state[pair] == cbxCableStateInterShort) {
            bcm_port_result->pair_state[pair] = _SHR_PORT_CABLE_STATE_OPENSHORT;
        } else if (port_result.pair_state[pair] == cbxCableStateBusy) {
            bcm_port_result->pair_state[pair] = _SHR_PORT_CABLE_STATE_CROSSTALK;
        }
        bcm_port_result->pair_len[pair] = (int8) port_result.pair_len[pair];
    }

    if (faultOpen && faultShort) {
        bcm_port_result->state = _SHR_PORT_CABLE_STATE_OPENSHORT;
    } else if (faultOpen) {
        bcm_port_result->state = _SHR_PORT_CABLE_STATE_OPEN;
    } else if (faultShort) {
        bcm_port_result->state = _SHR_PORT_CABLE_STATE_SHORT;
    }

    return 0;
}
#endif

int drv_avgr_fsal_init(int unit)
{
    int        rv = SOC_E_NONE;
    rv = cbx_fsal_init();
    if(rv < 0) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,"FSAL Init failed!\n")));
    }
    return rv;
}

/*
 *  Function : drv_avgr_port_get
 *
 *  Purpose :
 *      Initialise Port Module
 *
 *  Parameters :
 *      unit        :   unit id
 *
 *  Return :
 *      SOC_E_XXX
 *
 */

int drv_avgr_port_init(int unit)
{
    int rv = SOC_E_NONE;
    if(unit == 1) {
        return rv;
    }
    sal_printf("cbx_port_init()\n");
    rv = cbx_port_init();
    if(rv < 0) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,"Port Init failed!\n")));
    }
    return rv;
}

/*
 *  Function : drv_avgr_port_get
 *
 *  Purpose :
 *      Get the property to the specific ports.
 *
 *  Parameters :
 *      unit        :   unit id
 *      port   :   port id.
 *      prop_type  :   port property type.
 *      prop_val    :   port property value.
 *
 *  Return :
 *      SOC_E_XXX
 *
 *  Note :
 *      
 *
 */
int 
drv_avgr_port_get(int unit, int port, 
                uint32 prop_type, uint32 *prop_val)
{
    int        rv = SOC_E_NONE;
    int        loopback = 0;
    uint32     temp = 0;
    cbx_port_params_t port_params;
    mac_driver_t *p_mac = NULL;
    uint32     mac_ability = 0, phy_ability = 0;
    /* remarked for linkscan called each time
     *  (too many messages will be printed out)
    LOG_INFO(BSL_LS_SOC_PORT,
             (BSL_META_U(unit,
                         "%s unit=%d port=%d command:%x\n"),__func__, unit, port, prop_type)); */

    SOC_ROBO_PORT_INIT(unit);
    p_mac = SOC_ROBO_PORT_MAC_DRIVER(unit, port);
 
    switch (prop_type) {
        case DRV_PORT_PROP_SPEED:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: Speed\n")));
            temp = 0;
            rv = cbx_port_attribute_get(port, cbxPortAttrSpeed, &temp);
            if (rv != 0){
                return rv;
            }
            switch(temp) {
                case 10:
                    *prop_val = DRV_PORT_STATUS_SPEED_10M;
                    break;
                case 100:
                    *prop_val = DRV_PORT_STATUS_SPEED_100M;
                    break;
                case 1000:
                    *prop_val = DRV_PORT_STATUS_SPEED_1G;
                    break;
                case 2500:
                    *prop_val = DRV_PORT_STATUS_SPEED_2500M;
                    break;
                case 10312:
                case 10000:
                    *prop_val = DRV_PORT_STATUS_SPEED_10G;
                    break;
                default:
                    *prop_val = 0;
                    break;
            }
            break;
        case DRV_PORT_PROP_DUPLEX:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: Duplex\n")));
            temp = 0;
            rv = cbx_port_attribute_get(port, cbxPortAttrDuplex, &temp);
            if (rv != 0){
                return rv;
            }

            switch(temp) {
                case 0:
                    *prop_val = DRV_PORT_STATUS_DUPLEX_HALF;
                    break;
                case 1:
                    *prop_val = DRV_PORT_STATUS_DUPLEX_FULL;
                    break;
                default:
                    break;
            }
            break;
        case DRV_PORT_PROP_AUTONEG:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: Autoneg\n")));
            temp = 0;
            rv = cbx_port_attribute_get(port, cbxPortAttrAutoNegotiate, &temp);
            if (rv != 0){
                return rv;
            }
            *prop_val = temp;
            break;
        case DRV_PORT_PROP_TX_PAUSE:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: TX Pause\n")));
            if (p_mac != NULL) {
                int  pause_rx;
                if (p_mac->md_pause_get != NULL) {
                    rv = MAC_PAUSE_GET(p_mac,
                        unit, port, (int *)prop_val, &pause_rx);
                } else {
                    rv = SOC_E_UNAVAIL;
                }
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case DRV_PORT_PROP_RX_PAUSE:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: RX Pause\n")));
            if (p_mac != NULL) {
                int  pause_tx;
                if (p_mac->md_pause_get != NULL) {
                    rv = MAC_PAUSE_GET(p_mac,
                        unit, port, &pause_tx, (int *)prop_val);
                } else {
                    rv = SOC_E_UNAVAIL;
                }
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case DRV_PORT_PROP_LOCAL_ADVERTISE:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: Local Advertise\n")));
            rv = soc_phyctrl_adv_local_get(unit, port, prop_val);
            break;
        case DRV_PORT_PROP_REMOTE_ADVERTISE:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: Remote Advertise\n")));
            /* if auto-negotiation is ON and negotiation is completed */
            /*   get remote advertisement from PHY */
            rv = soc_phyctrl_adv_remote_get(unit, port, prop_val);
            break;
        case DRV_PORT_PROP_PORT_ABILITY:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: Port Ability\n")));
            rv = soc_phyctrl_ability_get(unit, port, &phy_ability);
            SOC_IF_ERROR_RETURN(rv);
            if (p_mac != NULL) {
                if (p_mac->md_ability_get != NULL) {
                    rv = MAC_ABILITY_GET(
                        p_mac, unit, port, &mac_ability);
                } else {
                    rv = SOC_E_UNAVAIL;
                }
            } else {
                rv = SOC_E_PARAM;
            }

            *prop_val  = mac_ability & phy_ability;
            *prop_val |= phy_ability & SOC_PM_ABILITY_PHY;
            break;
        case DRV_PORT_PROP_MAC_ABILITY:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: MAC Ability\n")));
            if (p_mac != NULL) {
                if (p_mac->md_ability_get != NULL) {
                    rv = MAC_ABILITY_GET(
                        p_mac, unit, port, &mac_ability);
                } else {
                    rv = SOC_E_UNAVAIL;
                }
            } else {
                rv = SOC_E_PARAM;
            }
            *prop_val = mac_ability;
            break;
        case DRV_PORT_PROP_PHY_ABILITY:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: PHY Ability\n")));
            rv = soc_phyctrl_ability_get(unit, port, prop_val);
            break;
        case DRV_PORT_PROP_INTERFACE:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: Interface\n")));
            if (p_mac != NULL) {
                if (p_mac->md_interface_get != NULL) {
                    rv = MAC_INTERFACE_GET(
                        p_mac, unit, port, prop_val); 
                } else {
                    rv = SOC_E_UNAVAIL;
                }
            } else {
                rv = SOC_E_PARAM;
            } 
            break;
        case DRV_PORT_PROP_ENABLE:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: Enable\n")));
            if (port != CBX_PORT_RGMII) {
                rv = soc_phyctrl_enable_get(unit, port, (int *) prop_val);
            } else {
                *prop_val = 1;
            }
            break;
        case DRV_PORT_PROP_ENABLE_DROP_1Q:
        case DRV_PORT_PROP_ENABLE_DROP_NON1Q:
        case DRV_PORT_PROP_ENABLE_RX:
        case DRV_PORT_PROP_ENABLE_TX:
        case DRV_PORT_PROP_ENABLE_TXRX:
        case DRV_PORT_PROP_EGRESS_PCP_REMARK:
        case DRV_PORT_PROP_EGRESS_CFI_REMARK:
        case DRV_PORT_PROP_EGRESS_C_PCP_REMARK:
        case DRV_PORT_PROP_EGRESS_S_PCP_REMARK:
        case DRV_PORT_PROP_EGRESS_DSCP_REMARK:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "%s: Enable Get\n"), FUNCTION_NAME()));
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_IPG_FE:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: IPG FE\n")));
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_IPG_GE:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: IPG GE\n")));
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_JAM:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: JAM\n")));
                rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_BPDU_RX:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: BPDU RX\n")));
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_RESTART_AUTONEG:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: PROP_RESTART_AUTONEG not support\n")));
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_MAC_LOOPBACK:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: PROP_MAC_LOOPBACK\n")));
            rv = cbxi_port_loopback_get(unit, port, &loopback);
            if (loopback == 1)
               *prop_val = loopback;
            break;
        case DRV_PORT_PROP_PHY_LOOPBACK:
            rv = cbxi_port_loopback_get(unit, port, &loopback);
            if (loopback == 2)
               *prop_val = loopback;
            break;
        case DRV_PORT_PROP_PHY_MEDIUM:
            rv = soc_phyctrl_medium_get(unit, port, prop_val);
            break;
        case DRV_PORT_PROP_PHY_MDIX:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: PROP_PHY_MDIX\n")));
            rv = soc_phyctrl_mdix_get(unit, port, prop_val);
            break;
        case DRV_PORT_PROP_PHY_MDIX_STATUS:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: PROP_PHY_MDIX_STATUS\n")));
            rv = soc_phyctrl_mdix_status_get(unit, port, prop_val);
            break;
        case DRV_PORT_PROP_MS:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: PROP_MS\n")));
            rv = soc_phyctrl_master_get(unit, port, (int *) prop_val);
            break;
        case DRV_PORT_PROP_SEC_MAC_MODE_NONE:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: PROP_SEC_MODE_NONE\n")));
            return SOC_E_UNAVAIL;
            break;

        case DRV_PORT_PROP_SEC_MAC_MODE_EXTEND:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: PROP_SEC_MAC_MODE_EXTEND\n")));
            return SOC_E_UNAVAIL;
            break;

        case DRV_PORT_PROP_SEC_MAC_MODE_SIMPLIFY:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: DRV_PORT_PROP_SEC_MAC_MODE_SIMPLIFY\n")));
            return SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_SEC_MAC_MODE_STATIC_ACCEPT:
        case DRV_PORT_PROP_SEC_MAC_MODE_STATIC_REJECT:
        case DRV_PORT_PROP_SEC_MAC_MODE_DYNAMIC_SA_MATCH:
        case DRV_PORT_PROP_SEC_MAC_MODE_DYNAMIC_SA_NUM:
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_SA_UNKNOWN_DROP:
            rv = cbx_port_info_get(port, &port_params);
            if (rv != 0){
                return rv;
            }

            if (port_params.flags & CBX_PORT_DROP_SLF) {
                *prop_val = 1;
            } else {
                *prop_val = 0;
            }
            rv = SOC_E_NONE;
            break;
        case DRV_PORT_PROP_SA_UNKNOWN_CPUCOPY:
            rv = cbx_port_info_get(port, &port_params);
            if (rv != 0){
                return rv;
            }

            if (port_params.flags & CBX_PORT_SA_UNKNOWN_CPUCOPY) {
                *prop_val = 1;
            } else {
                *prop_val = 0;
            }
            rv = SOC_E_NONE;
            break;
        case DRV_PORT_PROP_PHY_CABLE_DIAG:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: PROP_PHY_CABLE_DIAG\n")));
#ifdef FSAL_CABLE_DIAG
            cbx_port_cable_diag_t port_result;
            rv = cbx_port_cable_diag(port, &port_result);
            rv = drv_avgr_cable_diag_fsal2bcm_convert(port_result, (bcm_port_cable_diag_t *)prop_val);
#else
            rv = soc_phyctrl_cable_diag(unit, port,
                                        (soc_port_cable_diag_t *)prop_val);
#endif
            break;
        case DRV_PORT_PROP_PHY_LINK_CHANGE:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: PROP_PHY_LINKCHANGE\n")));
            rv = soc_phyctrl_link_change(unit, port, (int *) prop_val);
            break;
        case DRV_PORT_PROP_PAUSE_FRAME_BYPASS_RX:
        case DRV_PORT_PROP_PAUSE_FRAME_BYPASS_TX:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: PROP_PAUSE_FRAME_BYPASS\n")));
            return SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_DTAG_MODE:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: PROP_DTAG_MODE\n")));
            *prop_val = 0;
            rv = SOC_E_NONE;
            break;
        case DRV_PORT_PROP_DTAG_ISP_PORT:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: PROP_DTAG_ISP_PORT\n")));
            return SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_DTAG_TPID:            
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: PROP_DTAG_TPID\n")));
            return SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_MAC_BASE_VLAN:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: PROP_MAC_BASE_VLAN\n")));
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_MAX_FRAME_SZ:
             rv = cbxi_port_frame_max_get(unit, port, prop_val);
            break;
        case DRV_PORT_PROP_INGRESS_VLAN_CHK:
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_EEE_ENABLE:
        case DRV_PORT_PROP_EEE_SLEEP_DELAY_TIMER_G:
        case DRV_PORT_PROP_EEE_SLEEP_DELAY_TIMER_H:
        case DRV_PORT_PROP_EEE_MIN_LPI_TIMER_G:
        case DRV_PORT_PROP_EEE_MIN_LPI_TIMER_H:
        case DRV_PORT_PROP_EEE_WAKE_TRANS_TIMER_G:
        case DRV_PORT_PROP_EEE_WAKE_TRANS_TIMER_H:
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_DEFAULT_TC_PRIO:
                LOG_INFO(BSL_LS_SOC_PORT,
                         (BSL_META_U(unit,
                                     "drv_port_get: PORT_PROP_DEFAULT_TC_PRIO\n")));
            rv = cbx_port_info_get(port, &port_params);
            *prop_val = port_params.default_tc;
            if (rv != 0){
                return rv;
            }
            rv = SOC_E_NONE;
            break;
        case DRV_PORT_PROP_UNTAG_DEFAULT_TC:
            rv = SOC_E_UNAVAIL;
            break;

        case DRV_PORT_PROP_PPPOE_PARSE_EN:
            rv = SOC_E_UNAVAIL;
            break;

        case DRV_PORT_PROP_RAW_TC_MAP_MODE_SELECT:
            rv = cbx_port_info_get(port, &port_params);
            if (rv != 0){
                return rv;
            }
            rv = SOC_E_NONE;
            if (port_params.flags & CBX_PORT_TRUST_MODE_UNTRUSTED) {
                *prop_val = DRV_PORT_PROP_TRUST_MODE_UNTRUSTED;
            } else if (port_params.flags & CBX_PORT_TRUST_MODE_DOT1P) {
                *prop_val = DRV_PORT_PROP_TRUST_MODE_DOT1P;
            } else if (port_params.flags & CBX_PORT_TRUST_MODE_IPDSCP) {
                *prop_val = DRV_PORT_PROP_TRUST_MODE_IP_DSCP;
            } else {
                rv = SOC_E_CONFIG;
            }
            break;

        case DRV_PORT_PROP_L2_LEARN_LIMIT_PORT_ACTION:
            rv = SOC_E_UNAVAIL;
            break;

        case DRV_PORT_PROP_L2_LEARN_LIMIT_PORT_ACTION_DROP:
        case DRV_PORT_PROP_L2_LEARN_LIMIT_PORT_ACTION_CPU:
        case DRV_PORT_PROP_L2_LEARN_LIMIT_PORT_ACTION_NONE:
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_DISABLE_LEARN:
            rv = cbx_port_info_get(port, &port_params);
            if (rv != 0){
                return rv;
            }
            rv = SOC_E_NONE;

            if (port_params.learn_limit) {
                *prop_val = 0;
            } else {
                *prop_val = 1;
            }
            break;

        /* not supported list */
        case DRV_PORT_PROP_SFLOW_INGRESS_RATE:
        case DRV_PORT_PROP_SFLOW_EGRESS_RATE:
        case DRV_PORT_PROP_SFLOW_INGRESS_PRIO:
        case DRV_PORT_PROP_SFLOW_EGRESS_PRIO:
        case DRV_PORT_PROP_DEFAULT_DROP_PRECEDENCE:
        case DRV_PORT_PROP_EGRESS_ECN_REMARK:
        case DRV_PORT_PROP_ROAMING_OPT:
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_MCAST_PFM:
            rv = cbx_port_info_get(port, &port_params);
            if (rv != 0){
                return rv;
            }
            if (port_params.flags & CBX_PORT_DROP_MLF) {
                *prop_val = DRV_PORT_PROP_MCAST_FLOOD_NONE;
            } else {
                *prop_val = DRV_PORT_PROP_MCAST_FLOOD_UNKNOWN;
            }
            rv = SOC_E_NONE;
            break;
            
        default: 
            return SOC_E_PARAM;
    }

    return rv;
}

/*
 *  Function : drv_avgr_port_status_get
 *
 *  Purpose :
 *      Get the status of the port for selected status type.
 *
 *  Parameters :
 *      unit        :   unit id
 *      port   :   port number.
 *      status_type  :   port status type.
 *      vla     :   status value.
 *
 *  Return :
 *      SOC_E_XXX
 *
 *  Note :
 *      
 *
 */
int 
drv_avgr_port_status_get(int unit, uint32 port, uint32 status_type, uint32 *val)
{
    int     rv = SOC_E_NONE;
    int     okay = 0;
    uint32 up = 0, speed = 0, duplex = 0;

    /* int_pd and pd used to prevent the runpacket issue on the GE port.
     * (with Internal SerDes bounded)
     */
    phy_ctrl_t *ext_pc = NULL;
    
    /* special process to detach port driver */
    if (status_type == DRV_PORT_STATUS_DETACH){
        *val = TRUE;
        rv = drv_robo_port_sw_detach(unit);
        if (rv < 0) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "Port detach failed!\n")));
            *val = FALSE;
        }
        LOG_INFO(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "drv_avgr_port_status_get: DETACH %s\n"),
                  *val ? "OK" : "FAIL"));
        return SOC_E_NONE;
    }
    
    /* To prevent the runpacket issue on the GE port which bounded 
     *  with internal SerDes and connected to an external PHY through SGMII.
     *  The linked information report will be designed to retrieved from   
     *  internal SerDes instead of Ext_PHY.
     *  (original design check external PHY only) 
     */
    if (IS_GE_PORT(unit, port)){
        ext_pc = EXT_PHY_SW_STATE(unit, port);
    }
     

    /* remarked for performance issue when debugging LinkScan
    LOG_INFO(BSL_LS_SOC_PORT,
             (BSL_META_U(unit,
                         "%s unit = %d, port = %d command:%x \n"),
                         __func__, unit, port, status_type)); */

    switch (status_type)
    {
        case DRV_PORT_STATUS_LINK_UP:
            /* get the port loopback status (for Robo Chip)
             */
            up = 0;
            cbx_port_attribute_get(port, cbxPortAttrLinkStatus, &up);

            if (rv < 0){
                *val = FALSE;
            } else {
                *val = (up) ? TRUE : FALSE;
            }
            break;
        case DRV_PORT_STATUS_LINK_SPEED:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_get: Speed\n")));
            speed = 0;
            rv = cbx_port_attribute_get(port, cbxPortAttrSpeed, &speed);
            if (rv != 0){
                return rv;
            }
            *val = (speed == 10312) ? DRV_PORT_STATUS_SPEED_10G :
                    (speed == 2500) ? DRV_PORT_STATUS_SPEED_2500M :
                    (speed == 2000) ? DRV_PORT_STATUS_SPEED_2G :
                    (speed == 1000) ? DRV_PORT_STATUS_SPEED_1G :
                    (speed == 100) ? DRV_PORT_STATUS_SPEED_100M :
                                    DRV_PORT_STATUS_SPEED_10M;

            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_status_get: SPEED = %d\n"),
                      *val));
            break;
        case DRV_PORT_STATUS_LINK_DUPLEX:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_status_get: DUPLEX\n")));
            duplex = 0;
            rv = cbx_port_attribute_get(port, cbxPortAttrDuplex, &duplex);
            if (rv){
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "%s, Can't get the PHY duplex!\n"), FUNCTION_NAME())); 
                return rv;
            }
            *val = (duplex == TRUE) ? DRV_PORT_STATUS_DUPLEX_FULL :
                                    DRV_PORT_STATUS_DUPLEX_HALF;
            break;
        case DRV_PORT_STATUS_PROBE:
            *val = 0;
            rv = drv_robo_port_probe(unit, port, &okay);
            *val = okay;
            if (rv < 0) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "Port probe failed on port %s\n"),
                          SOC_PORT_NAME(unit, port)));
            }
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_status_get: PROBE %s\n"),
                      *val ? "OK" : "FAIL"));
            break;
        case DRV_PORT_STATUS_INIT:
            rv = drv_robo_port_sw_init_status_get(unit);
            if (rv == SOC_E_INIT) {
                *val = FALSE;
            } else {
                *val = TRUE;
            }
            break;
        case DRV_PORT_STATUS_PHY_DRV_NAME:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_status_get: PHY_DRV_NAME\n")));
            SOC_ROBO_PORT_INIT(unit);
            *val = PTR_TO_INT(soc_phyctrl_drv_name(unit, port));
            break;
        default:
            rv = SOC_E_PARAM;
    }
    return rv;
}

/*
 *  Function : drv_avgr_port_set
 *
 *  Purpose :
 *      Set the property to the specific ports.
 *
 *  Parameters :
 *      unit        :   unit id
 *      bmp   :   port bitmap.
 *      prop_type  :   port property type.
 *      prop_val    :   port property value.
 *
 *  Return :
 *      SOC_E_XXX
 *
 *  Note :
 *      
 *
 */
int 
drv_avgr_port_set(int unit, soc_pbmp_t bmp, 
                uint32 prop_type, uint32 prop_val)
{
    uint32 temp = 0;
    int port = CBXI_PGID_INVALID;
    int rv = SOC_E_NONE;
    mac_driver_t *p_mac = NULL;
    cbx_port_params_t port_params;
    int global_port = CBXI_PGID_INVALID;
    cbxi_trap_ctrl_t  trap_ctl;

    LOG_INFO(BSL_LS_SOC_PORT,
             (BSL_META_U(unit,
                         "drv_avgr_port_set: unit=%d bmp=%x\n"),
              unit, SOC_PBMP_WORD_GET(bmp, 0)));

    SOC_ROBO_PORT_INIT(unit);
    p_mac = SOC_ROBO_PORT_MAC_DRIVER(unit, port);
    switch (prop_type) {
        case DRV_PORT_PROP_SPEED:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_SPEED\n")));
            temp = 0;
            switch (prop_val) {
                case DRV_PORT_STATUS_SPEED_10M:
                    temp = 10;
                    break;
                case DRV_PORT_STATUS_SPEED_100M:
                    temp =100;
                    break;
                case DRV_PORT_STATUS_SPEED_1G:
                    temp = 1000;
                    break;
                case DRV_PORT_STATUS_SPEED_2500M:
                    temp = 2500;
                    break;
                case DRV_PORT_STATUS_SPEED_10G:
                    temp = 10000;
                    break;
                default:
                    return SOC_E_PARAM;
            }
            PBMP_ITER(bmp, port) {
                rv = cbx_port_attribute_set(port, cbxPortAttrSpeed, temp);
            }
            break;

        case DRV_PORT_PROP_DUPLEX:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_DUPLEX\n")));
            switch (prop_val) {
                case DRV_PORT_STATUS_DUPLEX_HALF:
                    temp = FALSE;
                    break;
                case DRV_PORT_STATUS_DUPLEX_FULL:
                    temp =TRUE;
                    break;
                default:
                    return SOC_E_PARAM;
            }
            PBMP_ITER(bmp, port) {
                rv = cbx_port_attribute_set(port, cbxPortAttrDuplex, temp);
            }
            break;
        case DRV_PORT_PROP_RESTART_AUTONEG:
        case DRV_PORT_PROP_AUTONEG:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_AUTONEG\n")));
            PBMP_ITER(bmp, port) {
                rv = cbx_port_attribute_set(port, cbxPortAttrAutoNegotiate, prop_val);
            }
            break;
        case DRV_PORT_PROP_TX_PAUSE:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_TX_PAUSE\n")));
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_RX_PAUSE:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_RX_PAUSE\n")));
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_LOCAL_ADVERTISE:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: LOCAL_ADVER\n")));
            PBMP_ITER(bmp, port) {
                /* set advertise to PHY accordingly */
                rv = soc_phyctrl_adv_local_set(unit, port, prop_val);
            }
            break;
        case DRV_PORT_PROP_REMOTE_ADVERTISE:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: REMOTE_ADVER not support\n")));
            /* can not set remote advert */
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_PORT_ABILITY:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_PORT_ABILITY not support\n")));
            /* can not be set */
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_MAC_ABILITY:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_MAC_ABILITY not support\n")));
            /* can not be set */
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_PHY_ABILITY:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_PHY_ABILITY not support\n")));
            /* can not be set */
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_INTERFACE:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_INTERFACE %x %x\n"), 
            SOC_PBMP_WORD_GET(bmp, 0), prop_val));
            PBMP_ITER(bmp, port) {
                rv = soc_phyctrl_interface_set(unit, port, prop_val);
            }
            break;
        case DRV_PORT_PROP_MAC_ENABLE:
            /* This case is called for _bcm_robo_port_update() only. */
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_MAC_ENABLE\n")));
            PBMP_ITER(bmp, port) {
                /* MAC register(s) should be set also */
                p_mac = SOC_ROBO_PORT_MAC_DRIVER(unit, port);
                if (p_mac != NULL) {
                    if (p_mac->md_enable_set != NULL) {
                        rv = MAC_ENABLE_SET(
                            p_mac, unit, port, prop_val);
                    } else {
                        rv = SOC_E_UNAVAIL;
                    }
                } else {
                    rv = SOC_E_PARAM;
                }
            }
            break;
        case DRV_PORT_PROP_ENABLE:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_ENABLE\n")));
            {
               PBMP_ITER(bmp, port) {
                  /* Set PHY registers anyway. */
                  if (port != CBX_PORT_RGMII) {
                      rv = soc_phyctrl_enable_set(unit, port, prop_val);
                  }
                  global_port = port;
#ifdef CONFIG_CASCADED_MODE
                  if ((SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) &&
                      (global_port < CBX_MAX_PORT_PER_UNIT)) {
                      global_port  += (unit * CBX_MAX_PORT_PER_UNIT);
                  }
#endif /* CONFIG_CASCADED_MODE */

                  cbx_port_params_t_init(&port_params);
                  port_params.port_index = global_port;
#ifdef CONFIG_TIMESYNC
                  port_params.ac_policy = 1;
#else
                  port_params.ac_policy = 2;
#endif
                  rv =  cbx_port_create(&port_params,
                                       (cbx_portid_t *)&global_port);
                  if (rv == SOC_E_EXISTS) {
                      rv = SOC_E_NONE;
                  }
               }
            }
            break;
        case DRV_PORT_PROP_ENABLE_DROP_1Q:
        case DRV_PORT_PROP_ENABLE_DROP_NON1Q:
        case DRV_PORT_PROP_ENABLE_RX:
        case DRV_PORT_PROP_ENABLE_TX:
        case DRV_PORT_PROP_ENABLE_TXRX:
        case DRV_PORT_PROP_EGRESS_PCP_REMARK:
        case DRV_PORT_PROP_EGRESS_CFI_REMARK:
        case DRV_PORT_PROP_EGRESS_C_PCP_REMARK:
        case DRV_PORT_PROP_EGRESS_S_PCP_REMARK:
        case DRV_PORT_PROP_EGRESS_DSCP_REMARK:
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_IPG_FE:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_IPG_FE\n")));
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_IPG_GE:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_IPG_GE\n")));
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_JAM:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_JAM\n")));
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_BPDU_RX:
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_MAC_LOOPBACK:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_MAC_LOOPBACK\n")));
            PBMP_ITER(bmp, port) {
                rv = cbxi_port_loopback_set(unit, port, 1);
            }
            break;
        case DRV_PORT_PROP_PHY_LOOPBACK:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_PHY_LOOPBACK\n")));
            PBMP_ITER(bmp, port) {
                rv = cbxi_port_loopback_set(unit, port, 2);
            }
            break;
        case DRV_PORT_PROP_PHY_MEDIUM:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_ex_port_set: PROP_PHY_MEDIUM not support\n")));
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_PHY_MDIX:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_PHY_MDIX\n")));
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_PHY_MDIX_STATUS:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_PHY_MDIX_STATUS not support\n")));
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_MS:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_MS not support\n")));
            rv = SOC_E_UNAVAIL;
            break;
            
        case DRV_PORT_PROP_SEC_MAC_MODE_NONE:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_SEC_MODE_NONE\n")));
            return SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_SEC_MAC_MODE_EXTEND:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_SEC_MAC_MODE_EXTEND\n")));
            return SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_SEC_MAC_MODE_SIMPLIFY:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_SEC_MAC_MODE_SIMPLIFY\n")));
            return SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_SEC_MAC_MODE_STATIC_ACCEPT:
        case DRV_PORT_PROP_SEC_MAC_MODE_STATIC_REJECT:
        case DRV_PORT_PROP_SEC_MAC_MODE_DYNAMIC_SA_MATCH:
        case DRV_PORT_PROP_SEC_MAC_MODE_DYNAMIC_SA_NUM:
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_SA_UNKNOWN_DROP:
            PBMP_ITER(bmp, port) {
                rv = cbx_port_info_get(port, &port_params);
                if (rv != 0){
                    return rv;
                }
                if (prop_val) {
                    port_params.flags |= CBX_PORT_DROP_SLF;
                } else {
                    port_params.flags &= ~CBX_PORT_DROP_SLF;
                }
                rv = cbx_port_info_set(port, &port_params);
                if (rv != 0){
                    return rv;
                }
            }
            rv = SOC_E_NONE;
            break;
        case DRV_PORT_PROP_SA_UNKNOWN_CPUCOPY:
            PBMP_ITER(bmp, port) {
                rv = cbx_port_info_get(port, &port_params);
                if (rv != 0){
                    return rv;
                }
                if (prop_val) {
                    port_params.flags |= CBX_PORT_SA_UNKNOWN_CPUCOPY;
                } else {
                    port_params.flags &= ~CBX_PORT_SA_UNKNOWN_CPUCOPY;
                }
                rv = cbx_port_info_set(port, &port_params);
                if (rv != 0){
                    return rv;
                }
            }
            rv = SOC_E_NONE;
            break;

        case DRV_PORT_PROP_PHY_LINKUP_EVT:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_PHY_LINKUP_EVT\n")));
            return SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_PHY_LINKDN_EVT:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_PHY_LINKDN_EVT\n")));
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_PHY_RESET:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_PHY_RESET\n")));
            PBMP_ITER(bmp, port) {
                rv = soc_phy_reset(unit, port);
            }
            break;
        case DRV_PORT_PROP_PAUSE_FRAME_BYPASS_RX:
        case DRV_PORT_PROP_PAUSE_FRAME_BYPASS_TX:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_PAUSE_FRAME_BYPASS\n")));
            return SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_DTAG_MODE:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_DTAG_MODE\n")));
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_DTAG_ISP_PORT:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_DTAG_ISP_PORT\n")));
            return SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_DTAG_TPID:       
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_DTAG_TPID\n")));
            return SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_MAC_BASE_VLAN:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_MAC_BASE_VLAN\n")));
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_MAX_FRAME_SZ:
            PBMP_ITER(bmp, port) {
                rv = cbxi_port_frame_max_set(unit, port, prop_val);
            }
            break;
        case DRV_PORT_PROP_INGRESS_VLAN_CHK:
            LOG_INFO(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "drv_avgr_port_set: PROP_INGRESS_VLAN_CHK\n")));
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_EEE_ENABLE:
        case DRV_PORT_PROP_EEE_SLEEP_DELAY_TIMER_G:
        case DRV_PORT_PROP_EEE_SLEEP_DELAY_TIMER_H:
        case DRV_PORT_PROP_EEE_MIN_LPI_TIMER_G:
        case DRV_PORT_PROP_EEE_MIN_LPI_TIMER_H:
        case DRV_PORT_PROP_EEE_WAKE_TRANS_TIMER_G:
        case DRV_PORT_PROP_EEE_WAKE_TRANS_TIMER_H:
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_DEFAULT_TC_PRIO:
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_UNTAG_DEFAULT_TC:
            rv = SOC_E_UNAVAIL;
            break;

        case DRV_PORT_PROP_MIB_CLEAR:
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_PPPOE_PARSE_EN:
            rv = SOC_E_UNAVAIL;
            break;

        case DRV_PORT_PROP_RAW_TC_MAP_MODE_SELECT:
            PBMP_ITER(bmp, port) {
                rv = cbx_port_info_get(port, &port_params);
                if (rv != 0){
                    return rv;
                }
                switch (prop_val) {
                    case DRV_PORT_PROP_TRUST_MODE_UNTRUSTED:
                        /* Set priority to port default and clear the rest */
                        port_params.flags |=  CBX_PORT_TRUST_MODE_UNTRUSTED;
                        port_params.flags &= ~CBX_PORT_TRUST_MODE_DOT1P;
                        port_params.flags &= ~CBX_PORT_TRUST_MODE_IPDSCP;
                        break;
                    case DRV_PORT_PROP_TRUST_MODE_DOT1P:
                        /* Set priority to DOT1P default and clear the rest */
                        port_params.flags |=  CBX_PORT_TRUST_MODE_DOT1P;
                        port_params.flags &= ~CBX_PORT_TRUST_MODE_IPDSCP;
                        port_params.flags &= ~CBX_PORT_TRUST_MODE_UNTRUSTED;
                        break;
                    case DRV_PORT_PROP_TRUST_MODE_IP_DSCP:
                        /* Set priority to IPDSCP default and clear the rest */
                        port_params.flags |=  CBX_PORT_TRUST_MODE_IPDSCP;
                        port_params.flags &= ~CBX_PORT_TRUST_MODE_DOT1P;
                        port_params.flags &= ~CBX_PORT_TRUST_MODE_UNTRUSTED;
                        break;
                    default:
                        rv = SOC_E_PARAM;
                        break;
                }

                rv = cbx_port_info_set(port, &port_params);
                if (rv != 0){
                    return rv;
                }
            }
            rv = SOC_E_NONE;
            break;

        case DRV_PORT_PROP_L2_LEARN_LIMIT_PORT_ACTION:
            rv = SOC_E_UNAVAIL;
            break;

        case DRV_PORT_PROP_L2_LEARN_LIMIT_PORT_ACTION_DROP:
        case DRV_PORT_PROP_L2_LEARN_LIMIT_PORT_ACTION_CPU:
        case DRV_PORT_PROP_L2_LEARN_LIMIT_PORT_ACTION_NONE:
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_DISABLE_LEARN:
            PBMP_ITER(bmp, port) {
                rv = cbx_port_info_get(port, &port_params);
                if (rv != 0){
                    return rv;
                }
                if (prop_val) {
                    port_params.learn_limit = 0x0;
                } else {
                    port_params.learn_limit = 0x4000;
                }
                rv = cbx_port_info_set(port, &port_params);
                if (rv != 0){
                    return rv;
                }
            }
            rv = SOC_E_NONE;
            break;

        /* not supported list */
        case DRV_PORT_PROP_SFLOW_INGRESS_RATE:
        case DRV_PORT_PROP_SFLOW_EGRESS_RATE:
        case DRV_PORT_PROP_SFLOW_INGRESS_PRIO:
        case DRV_PORT_PROP_SFLOW_EGRESS_PRIO:
        case DRV_PORT_PROP_DEFAULT_DROP_PRECEDENCE:
        case DRV_PORT_PROP_EGRESS_ECN_REMARK:
            rv = SOC_E_UNAVAIL;
            break;
        case DRV_PORT_PROP_ROAMING_OPT:
            PBMP_ITER(bmp, port) {
                cbx_port_params_t port_params;
                rv = cbx_port_info_get(port, &port_params);
                if (rv == SOC_E_NONE) {
                    if (port_params.flags & CBX_PORT_SA_UNKNOWN_CPUCOPY) {
                        port_params.flags |= CBX_PORT_SA_MOVE_CPUCOPY;
                    } else if (port_params.flags & CBX_PORT_DROP_SLF) {
                        port_params.flags |= CBX_PORT_SA_MOVE_DROP;
                    } else {
                        port_params.flags |= CBX_PORT_SA_MOVE_DROP;
                    }
                    rv = cbx_port_info_set(port, &port_params);
                    if (rv != SOC_E_NONE) {
                        break;
                    }
                    if (port_params.flags & CBX_PORT_SA_MOVE_CPUCOPY) {
                        SOC_IF_ERROR_RETURN(cbxi_trap_control_get(
                                        unit,FM_COPY_CPU, &trap_ctl));
                        trap_ctl.drop = 1;
                        trap_ctl.term = 0;
                        SOC_IF_ERROR_RETURN(cbxi_trap_control_set(
                                        unit,FM_COPY_CPU, &trap_ctl));
                    } else {
                        SOC_IF_ERROR_RETURN(cbxi_trap_control_get(
                                        unit,FM_COPY_CPU, &trap_ctl));
                        trap_ctl.drop = 0;
                        trap_ctl.term = 0;
                        SOC_IF_ERROR_RETURN(cbxi_trap_control_set(
                                        unit, FM_COPY_CPU, &trap_ctl));
                    }
                } else {
                   break;
                }
            }
            break;

        /* discard mode */
        case DRV_PORT_PROP_ENABLE_DROP_UNTAG:
            PBMP_ITER(bmp, port) {
                cbx_port_params_t port_params;
                rv = cbx_port_info_get(port, &port_params);
                if (rv == SOC_E_NONE) {
                    if (prop_val == TRUE) {
                        port_params.flags |= CBX_PORT_DROP_UNTAGGED;
                    } else {
                        port_params.flags &= ~CBX_PORT_DROP_UNTAGGED;
                    }
                    rv = cbx_port_info_set(port, &port_params);
                    if (rv != SOC_E_NONE) {
                        break;
                    }
                } else {
                   break;
                }
            }
            break;

        case DRV_PORT_PROP_ENABLE_DROP_PRITAG:
            PBMP_ITER(bmp, port) {
                cbx_port_params_t port_params;
                rv = cbx_port_info_get(port, &port_params);
                if (rv == SOC_E_NONE) {
                    if (prop_val == TRUE) {
                        port_params.flags |= CBX_PORT_DROP_TAGGED;
                    } else {
                        port_params.flags &= ~CBX_PORT_DROP_TAGGED;
                    }
                    rv = cbx_port_info_set(port, &port_params);
                    if (rv != SOC_E_NONE) {
                        break;
                    }
                } else {
                    break;
                }
            }
            break;
        case DRV_PORT_PROP_MCAST_PFM:
            PBMP_ITER(bmp, port) {
                cbx_port_params_t port_params;
                rv = cbx_port_info_get(port, &port_params);
                if (rv == SOC_E_NONE) {
                   if (prop_val == DRV_PORT_PROP_MCAST_FLOOD_NONE ) {
                       port_params.flags |= CBX_PORT_DROP_MLF;
                   } else if (prop_val == DRV_PORT_PROP_MCAST_FLOOD_UNKNOWN ) {
                       port_params.flags &= ~CBX_PORT_DROP_MLF;
                   } else {
                       rv = SOC_E_PARAM;
                   }
                   rv = cbx_port_info_set(port, &port_params);
                   if (rv != SOC_E_NONE) {
                        break;
                   }
                } else {
                    break;
                }
            }
            break;

        default:
            rv = SOC_E_PARAM; 
        break;
    }

    LOG_INFO(BSL_LS_SOC_PORT,
             (BSL_META_U(unit,
                         "drv_avgr_port_set: Exit\n")));
    return rv;
}
/*
 *  Function : drv_avgr_port_pri_mapop_get
 *
 *  Purpose :
 *      Get the mapping of incomming port, packet priority,and cfi bit to
 *      switch internal priority and color.
 *
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      op_type      - (IN) operation type
 *      pkt_pri      - (IN) Packet priority
 *      cfi          - (IN) Packet CFI
 *      internal_pri - (OUT) Internal priority
 *      color        - (OUT) color

 *  Return :
 *      SOC_E_XXX
 *
 *  Note :
 *
 */

int
drv_avgr_port_pri_mapop_get(int unit, int port, int op_type, uint32 pkt_pri,
                       uint32 cfi, uint32 *internal_pri, uint32 *color)
{
    int rv = 0;
    cbx_tc_t tc;

    switch (op_type) {
        case DRV_PORT_OP_PCP2TC :
            rv = cbx_cosq_port_pcp2tc_get(port, pkt_pri, cfi, &tc);

            *internal_pri = tc.int_pri;
            *color = tc.int_dp;
            break;
        default:
            rv = SOC_E_UNAVAIL;
            break;
    }

    return rv;
}

/*
 *  Function : drv_avgr_port_pri_mapop_set
 *
 *  Purpose :
 *      Define the mapping of incomming port, packet priority,and cfi bit to
 *      switch internal priority and color.
 *
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      op_type      - (IN) operation type
 *      pkt_pri      - (IN) Packet priority
 *      cfi          - (IN) Packet CFI
 *      internal_pri - (IN) Internal priority
 *      color        - (IN) color

 *  Return :
 *      SOC_E_XXX
 *
 *  Note :
 *
 */

int
drv_avgr_port_pri_mapop_set(int unit, int port, int op_type, uint32 pkt_pri,
                       uint32 cfi, uint32 internal_pri, uint32 color)
{
    int rv = 0;
    cbx_tc_t tc;

    switch (op_type) {
        case DRV_PORT_OP_PCP2TC :
            tc.int_pri = internal_pri;
            if (color > 2) {
                tc.int_dp = cbxDpDrop;
            } else {
                tc.int_dp = color;
            }
            rv = cbx_cosq_port_pcp2tc_set(port, pkt_pri, cfi, tc);
            break;
        default:
            rv = SOC_E_UNAVAIL;
            break;
    }

    return rv;
}

int drv_avgr_port_vlan_pvid_set(int unit,uint32 port,uint32 outer_tag,uint32 inner_tag)
{
    int rv = SOC_E_NONE;
    cbx_port_params_t port_params;

    rv = cbx_port_info_get(port, &port_params);
    if (rv == SOC_E_NONE) {
        port_params.default_vid = outer_tag;
        rv = cbx_port_info_set(port, &port_params);
    }
    return rv;
}

int drv_avgr_port_vlan_pvid_get(int unit,uint32 port,uint32 *outer_tag,uint32 *inner_tag)
{
    int rv = SOC_E_NONE;
    cbx_port_params_t port_params;

    rv = cbx_port_info_get(port, &port_params);
    if (rv == SOC_E_NONE) {
        *outer_tag = port_params.default_vid;
        *inner_tag = 0;
    }
    return rv;
}

int drv_avgr_port_pkt_flow_enable(int unit)
{
    int rv;
    rv = soc_robo2_pkt_flow_enable();
    return rv;
}
