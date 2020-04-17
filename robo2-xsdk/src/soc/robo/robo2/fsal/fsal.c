/*
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:
 *    fsal.c
 * Description:
 *    Forwarding Servics Abstraction Layer Init
 */

#ifdef MANAGED_MODE
#include <sal/types.h>
#include <sal/core/thread.h>
#include <sal/core/alloc.h>
#include <sal/core/libc.h>
#include <fsal_int/shared_types.h>
#include <shared/bsl.h>
#else
#include <sal_types.h>
#include <sal_util.h>
#include <sal_task.h>
#include <sal_alloc.h>
#include <bsl_log/bslenum.h>
#include <bsl_log/bsl.h>
#endif
#include <shared/types.h>
#include <soc/types.h>
#include <fsal/port.h>
#include <fsal_int/port.h>
#include <fsal/vlan.h>
#include <fsal_int/vlan.h>
#include <fsal_int/l2.h>
#include <fsal/stat.h>
#include <fsal/error.h>
#include <fsal/cosq.h>
#include <fsal_int/cosq.h>
#include <fsal/meter.h>
#include <fsal/link.h>
#include <fsal/auth.h>
#include <fsal/l2.h>
#include <fsal/extender.h>
#include <fsal_int/l2.h>
#include <fsal_int/meter.h>
#include <fsal/pktio.h>
#include <fsal_int/init.h>
#include <fsal_int/trap.h>
#include <fsal_int/slic.h>
#include <fsal/mirror.h>
#include <fsal_int/mirror.h>
#include <fsal_int/mcast.h>
#include <fsal_int/lin.h>
#include <fsal_int/stg.h>
#include <fsal_int/pkt.h>
#include <fsal/cfp.h>
#include <fsal_int/cfp.h>
#ifdef CONFIG_PORT_EXTENDER
#include <fsal/l2.h>
#include <fsal_int/l2.h>
#include <fsal/extender.h>
#include <fsal_int/extender.h>
#endif
#include <fsal/ptp.h>

extern int cbxi_port_admin_status_get(int unit, cbx_port_t port_out,
           int *value);

extern int soc_robo2_pkt_flow_init(int unit);
#if 0 /* Initialized as part of SDK modules init */
extern int bcm_robo_port_init(int);
#endif
extern int soc_robo2_arl_table_init(void);
/**
 *  @brief avg_port_create
 *  Temporary function to configure port on init
 *  Creates the physical ports and LAG ports. Registers the user ports
 *  with Port Database. The port registration code needs to be moved
 *  under board specific code.
 *
 *  Note:
 *      Ports need to be created before packet Tx/Rx
 */

int avg_port_create(void)
{
    int max_units = 1, unit, rv = 0;
    int pp, port, idx = 0;
    uint32_t lag, reg_val, chip_id;
    uint32_t lag_start;
    cbx_portid_t portid;
    cbx_port_params_t port_params;
#ifndef CONFIG_EXTERNAL_HOST
    cbx_portid_t lagid;
    cbx_lag_params_t  lag_params;
#endif
#ifdef CONFIG_CASCADED_MODE
    uint8_t i = 0, j = 0;
    uint8_t port_num, casc_port[2][2] = {{255, 255}, {255, 255}};
#endif
#ifndef CONFIG_EXTERNAL_HOST    
    portdb_t portdb[CBX_MAX_UNITS * (CBX_MAX_PORT_PER_UNIT + CBX_MAX_LAG_PER_UNIT)];


    sal_memset(portdb, 0, (CBX_MAX_UNITS * (CBX_MAX_PORT_PER_UNIT + CBX_MAX_LAG_PER_UNIT))*
               sizeof(portdb_t));
#endif    
    if (SOC_IS_CASCADED(0)) {
        max_units = 2;
    }
    for (unit = 0; unit < max_units; unit++) {
        sal_printf("Unit %d: PBMP_ALL(unit)=%x\n", unit, PBMP_ALL(unit));
        /* Read chip ID */
        REG_READ_CRU_CRU_CHIP_ID_REGr(unit, &reg_val);
        chip_id = reg_val & 0xFFFFFF;
#ifdef CONFIG_CASCADED_MODE
        /* Get the cascaded port numbers for primary and secondary */
        if (SOC_IS_CASCADED(0)) {
            if (unit == 0) {
                CBX_PBMP_ITER(CBX_CASCADE_PORTS_MASTER, port_num) {
                    casc_port[i][j] = port_num;
                    if (j == 1) {
                        break;
                    } else {
                        j++;
                    }
                }
            } else {
                i = 1;
                j = 0;
                CBX_PBMP_ITER(CBX_CASCADE_PORTS_SLAVE, port_num) {
                    casc_port[i][j] = port_num;
                    if (j == 1) {
                        break;
                    } else {
                        j++;
                    }
                }
            }
        }
#endif
        CBX_PBMP_ITER(PBMP_ALL(unit), pp) {
            port = pp + (unit * CBX_MAX_PORT_PER_UNIT);
            cbx_port_params_t_init(&port_params);
            port_params.port_index = port;
            port_params.default_tc = 0;
            port_params.default_dp = 0;
#ifdef CONFIG_TIMESYNC
            port_params.ac_policy = 1;
#else
            port_params.ac_policy = 2;
#endif
            /* if ((port == CBX_PORT_ICPU) || (port == CBX_PORT_ECPU)) {
                port_params.port_type = cbxPortTypeHost;
            } */
            rv = cbx_port_create(&port_params, &portid);
            if (rv < 0) {
                sal_printf("Unit %d: Port Create failed for Port %d rv(%d)\n", unit, port, rv);
                return rv;
            }
#ifdef CONFIG_CASCADED_MODE
            if (((unit == 0) && ((pp == casc_port[0][0]) || (pp == casc_port[0][1])))
                || ((unit == 1) && ((pp == casc_port[1][0]) || (pp == casc_port[1][1])))) {
                /* Do not register as user port */
                continue;
            }
#endif
            if ((pp == CBX_PORT_ICPU) || (pp == CBX_PORT_ECPU)) {
                /* Do not register as user port */
                continue;
            }
#ifndef CONFIG_EXTERNAL_HOST    
            portdb[idx].uport_idx = idx;
            portdb[idx].uport_type = USER_PORT_TYPE_PHYSICAL;
            portdb[idx].fport = portid;

            switch (chip_id) {
                case BCM53162_DEVICE_ID:
                    if ((pp >= 0) && (pp <= 7)) {
                        portdb[idx].port_capab =  PORT_CAPAB_GPHY |
                            PORT_CAPAB_SPEED_10M | PORT_CAPAB_SPEED_100M |
                            PORT_CAPAB_SPEED_1000M | PORT_CAPAB_AUTONEG |
                            PORT_CAPAB_FDX | PORT_CAPAB_HDX;
                    } else if ((pp >= 8) && (pp <= 11)) {
                        portdb[idx].port_capab = PORT_CAPAB_SGMII |
                            PORT_CAPAB_SPEED_100M | PORT_CAPAB_SPEED_1000M |
                            PORT_CAPAB_SPEED_2500M | PORT_CAPAB_AUTONEG |
                            PORT_CAPAB_FDX | PORT_CAPAB_HDX;
                    } else if ((pp >= 12) && (pp <=13)) {
                        portdb[idx].port_capab = PORT_CAPAB_XFI |
                            PORT_CAPAB_SPEED_10G | PORT_CAPAB_FDX |
                            PORT_CAPAB_HDX;
                    }
                    break;
                case BCM53158_DEVICE_ID:
                    if ((pp >= 0) && (pp <= 7)) {
                        portdb[idx].port_capab = PORT_CAPAB_GPHY |
                            PORT_CAPAB_SPEED_10M | PORT_CAPAB_SPEED_100M |
                            PORT_CAPAB_SPEED_1000M | PORT_CAPAB_AUTONEG |
                            PORT_CAPAB_FDX | PORT_CAPAB_HDX;
                    } else if ((pp >= 8) && (pp <= 11)) {
                        portdb[idx].port_capab = PORT_CAPAB_QSGMII |
                            PORT_CAPAB_SPEED_1000M | PORT_CAPAB_FDX |
                            PORT_CAPAB_HDX;
                    } else if ((pp >= 12) && (pp <=13)) {
                        portdb[idx].port_capab = PORT_CAPAB_XFI |
                            PORT_CAPAB_SPEED_10G | PORT_CAPAB_FDX |
                            PORT_CAPAB_HDX;
                    }
                    break;
                case BCM53112_DEVICE_ID:
                    if ((pp >= 0) && (pp <= 7)) {
                        portdb[idx].port_capab = PORT_CAPAB_GPHY |
                            PORT_CAPAB_SPEED_10M | PORT_CAPAB_SPEED_100M |
                            PORT_CAPAB_AUTONEG | PORT_CAPAB_FDX |
                            PORT_CAPAB_HDX;
                    } else if ((pp >= 8) && (pp <= 11)) {
                        portdb[idx].port_capab = PORT_CAPAB_SGMII |
                            PORT_CAPAB_SPEED_10M | PORT_CAPAB_SPEED_100M |
                            PORT_CAPAB_SPEED_1000M | PORT_CAPAB_AUTONEG |
                            PORT_CAPAB_FDX | PORT_CAPAB_HDX;
                    } else if ((pp >= 12) && (pp <=13)) {
                        portdb[idx].port_capab = PORT_CAPAB_SGMII |
                            PORT_CAPAB_SPEED_100M | PORT_CAPAB_SPEED_1000M |
                            PORT_CAPAB_SPEED_2500M | PORT_CAPAB_AUTONEG |
                            PORT_CAPAB_FDX | PORT_CAPAB_HDX;
                    }
                    break;
                case BCM53156_DEVICE_ID:
                    if ((pp >= 0) && (pp <= 7)) {
                        portdb[idx].port_capab = PORT_CAPAB_GPHY |
                            PORT_CAPAB_SPEED_10M | PORT_CAPAB_SPEED_100M |
                            PORT_CAPAB_SPEED_1000M | PORT_CAPAB_AUTONEG |
                            PORT_CAPAB_FDX | PORT_CAPAB_HDX;
                    } else if (pp == 12) {
                        portdb[idx].port_capab = PORT_CAPAB_XFI |
                            PORT_CAPAB_SPEED_10G | PORT_CAPAB_FDX |
                            PORT_CAPAB_HDX;
                    }
                    break;
                case BCM53161_DEVICE_ID:
                    if ((pp >= 8) && (pp <= 11)) {
                        portdb[idx].port_capab = PORT_CAPAB_SGMII |
                            PORT_CAPAB_SPEED_100M | PORT_CAPAB_SPEED_1000M |
                            PORT_CAPAB_SPEED_2500M | PORT_CAPAB_AUTONEG |
                            PORT_CAPAB_FDX | PORT_CAPAB_HDX;
                    } else if ((pp >= 12) && (pp <=13)) {
                        portdb[idx].port_capab = PORT_CAPAB_XFI |
                            PORT_CAPAB_SPEED_10G | PORT_CAPAB_FDX |
                            PORT_CAPAB_HDX;
                    }
                    break;
                case BCM53154_DEVICE_ID:
                    if ((pp >= 0) && (pp <= 3)) {
                        portdb[idx].port_capab =  PORT_CAPAB_GPHY |
                            PORT_CAPAB_SPEED_10M | PORT_CAPAB_SPEED_100M |
                            PORT_CAPAB_SPEED_1000M | PORT_CAPAB_AUTONEG |
                            PORT_CAPAB_FDX | PORT_CAPAB_HDX;
                    } else if (pp == 8) {
                        portdb[idx].port_capab = PORT_CAPAB_SGMII |
                            PORT_CAPAB_SPEED_100M | PORT_CAPAB_SPEED_1000M |
                            PORT_CAPAB_SPEED_2500M | PORT_CAPAB_AUTONEG |
                            PORT_CAPAB_FDX | PORT_CAPAB_HDX;
                    }
                    break;
                default:
                    sal_printf("Unit %d: Chip ID %d not supported\n", unit, chip_id);
                    return CBX_E_NOT_FOUND;
            }
#endif            
            idx++;
        }

        if (SOC_IS_CASCADED(0) == 0) {
            lag_start = CBX_MAX_PORT_PER_UNIT;
        } else {
            lag_start = CBX_MAX_PORT_PER_UNIT * CBX_MAX_UNITS;
        }

        for (lag = lag_start + (unit * CBX_MAX_LAG_PER_UNIT); lag < (lag_start + CBX_MAX_LAG_PER_UNIT); lag++) {
#ifndef CONFIG_EXTERNAL_HOST
            cbx_lag_params_t_init(&lag_params);
            rv = cbx_lag_create(&lag_params, &lagid);
            if (rv < 0) {
                sal_printf("Unit %d: LAG Create failed for LAG %d rv(%d)\n", unit, lag, rv);
                return rv;
            }
            portdb[lag + (CBX_MAX_LAG_PER_UNIT * unit)].fport = lagid;
            portdb[lag + (CBX_MAX_LAG_PER_UNIT * unit)].uport_idx = lag;
            portdb[lag + (CBX_MAX_LAG_PER_UNIT * unit)].uport_type = USER_PORT_TYPE_LAG;
#endif
        }
    }
#ifndef CONFIG_EXTERNAL_HOST
    if (SOC_IS_CASCADED(0) == 0) {
        portdb_register(portdb, CBX_MAX_PORT_PER_UNIT + CBX_MAX_LAG_PER_UNIT);
    } else {
        portdb_register(portdb,
                        2 * (CBX_MAX_PORT_PER_UNIT + CBX_MAX_LAG_PER_UNIT));
        portdb_register(portdb, 2 * CBX_MAX_PORT_PER_UNIT - 4);
    }
#endif    
    return rv;
}

#ifdef CONFIG_CASCADED_MODE
int
avg_cascade_init(uint32 pbmp_u0, uint32 pbmp_u1, int trap_dest0, int trap_dest1)
{
    int max_units = 1, unit = 0;
    int rv = CBX_E_NONE;
    int count, pp, portid;
    uint32 lagid;
    pbmp_t pbmp_cascade;
    cbx_port_params_t port_params;

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        max_units = 2;
    }
    for (unit = 0; unit < max_units; unit++) {
        CBX_PBMP_CLEAR(pbmp_cascade);
        if (unit == CBX_AVENGER_PRIMARY) {
            CBX_PBMP_ASSIGN(pbmp_cascade, pbmp_u0);
        } else {
            CBX_PBMP_ASSIGN(pbmp_cascade, pbmp_u1);
        }
        CBX_PBMP_COUNT(pbmp_cascade, count);
        if (count > 1) {
            cbx_lag_params_t lag_params;
            cbx_lag_params_t_init( &lag_params );
            lag_params.lag_mode = cbxLagModeCascade;
            rv = cbx_lag_create(&lag_params, &lagid);
            if (CBX_FAILURE(rv)) {
                sal_printf("Failed creating lag for cascade ports\n");
                return rv;
            }
            CBX_PBMP_ITER(pbmp_cascade, pp) {
                portid = pp + (unit * CBX_MAX_PORT_PER_UNIT);
                rv = cbx_lag_member_add(lagid, portid);
                if (CBX_FAILURE(rv)) {
                    sal_printf("Failed adding cascade ports to lag\n");
                    return rv;
                }
            }
            rv = cbx_port_info_get(lagid, &port_params);
            if (CBX_FAILURE(rv)) {
                sal_printf("cbx_port_info_get failed for lag port %d rv(%d)\n", lagid, rv);
                return rv;
            }
            port_params.port_type = cbxPortTypeCascade;
            rv = cbx_port_info_set(lagid, &port_params);
            if (CBX_FAILURE(rv)) {
                sal_printf("cbx_port_info_set failed for lag port %d rv(%d)\n", lagid, rv);
                return rv;
            }
        } else if (count == 1) {
            sal_printf("Cascade ports Unit=%d PBM=%x\n", unit, PBMP_CASCADE(unit));
            CBX_PBMP_ITER(pbmp_cascade, pp) {
                portid = pp + (unit * CBX_MAX_PORT_PER_UNIT);
                rv = cbx_port_info_get(portid, &port_params);
                if (CBX_FAILURE(rv)) {
                    sal_printf("cbx_port_info_get failed for cascade port %d rv(%d)\n", portid, rv);
                    return rv;
                }
                port_params.port_type = cbxPortTypeCascade;
                rv = cbx_port_info_set(portid, &port_params);
                if (CBX_FAILURE(rv)) {
                    sal_printf("cbx_port_info_set failed for cascade port %d rv(%d)\n", portid, rv);
                    return rv;
                }
            }
        }
    }
    cbxi_trap_dest_update(CBX_AVENGER_PRIMARY, trap_dest0);
    cbxi_trap_dest_update(CBX_AVENGER_SECONDARY, trap_dest1);
    return rv;
}
#endif  /* CONFIG_CASCADED_MODE */

#ifdef CONFIG_PORT_EXTENDER
/**
 *  @brief avg_ext_config
 *  Temporary function to configure port extender on init
 *
 *  Note:
 *      These cofiguration are optional
 */

int avg_ext_config(void)
{
    cbx_portid_t portid;
    cbx_port_params_t port_params;
    cbx_l2_entry_t l2entry;
    int rv = 0;
    cbx_mcast_params_t mcast_params;
    cbx_mcastid_t mcastid;

    sal_memset(&l2entry, 0, sizeof(cbx_l2_entry_t));

    portid = 12;
    rv = cbx_port_info_get(portid, &port_params);
    port_params.port_type = cbxPortTypeExtenderUpstream;
    rv = cbx_port_info_set(portid, &port_params);

    portid = 1;
    rv = cbx_port_info_get(portid, &port_params);
    port_params.port_type = cbxPortTypeExtenderCascade;
    rv = cbx_port_info_set(portid, &port_params);

    portid = 2;
    rv = cbx_port_info_get(portid, &port_params);
    port_params.port_type = cbxPortTypeExtender;
    rv = cbx_port_info_set(portid, &port_params);

    portid = 3;
    rv = cbx_port_info_get(portid, &port_params);
    port_params.port_type = cbxPortTypeExtender;
    rv = cbx_port_info_set(portid, &port_params);

    l2entry.params.flags = CBX_L2_ADDR_ECID;
    l2entry.params.portid = 1;
    l2entry.params.dest_type = cbxL2AddrDestDPG;
    l2entry.params.e_cid = 0x11;
    rv = cbx_l2_entry_add(&l2entry);

    l2entry.params.flags = CBX_L2_ADDR_ECID;
    l2entry.params.portid = 2;
    l2entry.params.dest_type = cbxL2AddrDestDPG;
    l2entry.params.e_cid = 0x123;
    rv = cbx_l2_entry_add(&l2entry);

    l2entry.params.flags = CBX_L2_ADDR_ECID;
    l2entry.params.portid = 3;
    l2entry.params.dest_type = cbxL2AddrDestDPG;
    l2entry.params.e_cid = 0x567;
    rv = cbx_l2_entry_add(&l2entry);

    mcast_params.lilt_mode = cbxMcastLiltModeArray;
    rv = cbx_mcast_create(&mcast_params, &mcastid);

    portid = 1;
    rv = cbx_mcast_member_add(mcastid, portid);

    portid = 2;
    rv = cbx_mcast_member_add(mcastid, portid);

    l2entry.params.flags = CBX_L2_ADDR_ECID;
    l2entry.params.portid = mcastid;
    l2entry.params.dest_type = cbxL2AddrDestMcast;
    l2entry.params.e_cid = 0x1579; /* GRP = 0x1; E_CID_base = 0x579 */
    rv = cbx_l2_entry_add(&l2entry);

    return rv;
}
#endif

/**
 *  @brief avg_vlan_config
 *  Temporary function to configure vlan on init
 *
 *  Note:
 *      These cofiguration are optional
 */

int avg_vlan_config(void)
{
    int rv = 0;
    cbx_vlan_params_t params;
    cbx_vlanid_t vsiid;
    cbx_vlanid_t vlanid;
    cbx_portid_t portid;
    uint32_t mode = CBX_VLAN_PORT_MODE_TAGGED;

/* API sequence to initialize required VLAN configuration */
    /* Create VLAN 3 */
    cbx_vlan_params_t_init(&params);

    params.vlan=3;
    rv = cbx_vlan_create(&params, &vsiid);
    if (rv < 0) {
        return rv;
    }

    /* Add port 2 to VLAN 3 */
    portid = 2;
    vlanid = 3;
    rv = cbx_vlan_port_add(vlanid, portid, mode);
    if (rv < 0) {
        return rv;
    }

    /* Add port 3 to VLAN 3 */
    portid = 3;
    vlanid = 3;
    rv = cbx_vlan_port_add(vlanid, portid, mode);
    if (rv < 0) {
        return rv;
    }
    return rv;
}

#ifdef CONFIG_VIRTUAL_PORT_SUPPORT
/**
 *  @brief avg_lif_config
 *  Temporary function to configure vlan ports on init
 *
 *  Note:
 *      These cofiguration are optional
 */

int avg_lif_config(void)
{
    int rv;
    cbx_port_params_t port_params;
    cbx_mcast_params_t mcast_params;
    cbx_mcastid_t mcastid;
    cbx_vlan_params_t params;
    cbx_vlanid_t vsiid;
    uint32_t mode = CBX_VLAN_PORT_MODE_UNTAGGED;
    cbx_vlan_lif_params_t lif_params;
    cbx_portid_t portid, vp1, vp2, vp3;


    /* Create a default MCAST group */
    mcast_params.lilt_mode = cbxMcastLiltModeArray;
    rv = cbx_mcast_create(&mcast_params, &mcastid);

    /* Create VLAN 10 */
    cbx_vlan_params_t_init(&params);

    params.vlan = 10;
    params.mcastid = mcastid;
    rv = cbx_vlan_create(&params, &vsiid);

    /* Configure the ports 1, 2, 3 for Virtual ports */
    portid = 1;
    rv = cbx_port_info_get(portid, &port_params);
    port_params.flags |= CBX_PORT_VIRTUAL_PORTS_ENABLE;
    rv = cbx_port_info_set(portid, &port_params);

    portid = 2;
    rv = cbx_port_info_get(portid, &port_params);
    port_params.flags |= CBX_PORT_VIRTUAL_PORTS_ENABLE;
    rv = cbx_port_info_set(portid, &port_params);

    portid = 3;
    rv = cbx_port_info_get(portid, &port_params);
    port_params.flags |= CBX_PORT_VIRTUAL_PORTS_ENABLE;
    rv = cbx_port_info_set(portid, &port_params);

    /* Create two VLAN ports VP1, VP2  */
    /* VP1 on port 1 : CTAG = 50 */
    cbx_vlan_lif_params_t_init(&lif_params);
    lif_params.match_inner_vlan = 50;
    lif_params.portid = 1;
    lif_params.vlan_action = cbxVlanLifTagActionAdd;
    lif_params.egress_vlan = 0x300;
    lif_params.inner_vlan_action = cbxVlanLifTagActionAdd;
    lif_params.egress_inner_vlan = 0x501;

    rv = cbx_vlan_lif_create(&lif_params, &vp1);

    /* VP2 on port 2 : CTAG = 51 */
    cbx_vlan_lif_params_t_init(&lif_params);
    lif_params.match_inner_vlan = 51;
    lif_params.portid = 2;
    lif_params.vlan_action = cbxVlanLifTagActionAdd;
    lif_params.egress_vlan = 0x300;
    lif_params.inner_vlan_action = cbxVlanLifTagActionAdd;
    lif_params.egress_inner_vlan = 0x510;

    rv =cbx_vlan_lif_create(&lif_params, &vp2);

    /* VP3 on port 3 : CTAG = 52 */
    cbx_vlan_lif_params_t_init(&lif_params);
    lif_params.match_inner_vlan = 52;
    lif_params.portid = 3;
    lif_params.inner_vlan_action = cbxVlanLifTagActionAdd;
    lif_params.egress_inner_vlan = 0x520;

    rv =cbx_vlan_lif_create(&lif_params, &vp3);

    /* Add VP1 and VP2 to MCASt group */
    rv = cbx_mcast_member_add(mcastid, vp1);
    rv = cbx_mcast_member_add(mcastid, vp2);
    rv = cbx_mcast_member_add(mcastid, vp3);

    /*  Add VP1 and VP2 to VLAN 10 */
    rv = cbx_vlan_port_add(vsiid, vp1, mode);
    rv = cbx_vlan_port_add(vsiid, vp2, mode);
    rv = cbx_vlan_port_add(vsiid, vp3, mode);

    cbx_l2_entry_t l2entry;
    sal_memset(&l2entry, 0, sizeof(cbx_l2_entry_t));

    l2entry.src_mac[0]=0x00;
    l2entry.src_mac[1]=0x00;
    l2entry.src_mac[2]=0x00;
    l2entry.src_mac[3]=0x00;
    l2entry.src_mac[4]=0x11;
    l2entry.src_mac[5]=0x00;
    l2entry.vlanid = 10;
    l2entry.params.flags = 1;
    l2entry.params.dest_type = cbxL2AddrDestDLI;
    l2entry.params.portid = vp1;
    rv =  cbx_l2_entry_add(&l2entry);

    return rv;
}
#endif




void cfp_action_add(cbx_cfpid_t cfpid) {
    int rv = 0;

    cbx_cfp_action_t action;
    cbx_cfp_action_t_init(&action);
    int i  = 0;

    action.flags = CBX_CFP_ACTION_IN_POLICY_UPDATE_PRI |
                   CBX_CFP_ACTION_OUT_POLICY_UPDATE_PRI;

    action.common_action.dp = 1;
    action.common_action.count_mode = cbxCFPCountPacketsBytes;
    action.in_policy_action.forward_mode = cbxCFPForwardNoChange;
    action.in_policy_action.trapid = CBX_CFP_ACTION_TRAP_TO_CPU_AND_FORWARD;
    action.out_policy_action.forward_mode = cbxCFPForwardNoChange;
    action.out_policy_action.trapid = CBX_CFP_ACTION_TRAP_TO_CPU_AND_FORWARD;
    for (i = 0; i < 12; i++) {
        rv = cbx_cfp_action_set(&cfpid, &action, i);
        if (rv !=0) {
            return;
        }
    }
    return;
}

void cfp_udf_dhcp_rule_add(cbx_cfpid_t cfpid, uint32 index) {
    cbx_cfp_rule_t rule;
    int rv = 0; int i  = 0;
    rule.key_type = cbxCfpKeyUDFACL;
    rule.attr_selector = sal_alloc((CBX_CFP_UDF_ATTR_MAX * sizeof(cbx_cfp_rule_attr_selector_t)), "CFP_RULE");
    for (i = 0; i < CBX_CFP_UDF_ATTR_MAX; i++) {
        rule.attr_selector[i].attr_val = sal_alloc(16,"CFP_RULE");
        rule.attr_selector[i].attr_mask = sal_alloc(16,"CFP_RULE");
        sal_memset(rule.attr_selector[i].attr_val, 0, 16 * sizeof(uint8_t));
        sal_memset(rule.attr_selector[i].attr_mask,0, 16 * sizeof(uint8_t));
    }
    rule.num_attributes = 3;
    rule.attr_selector[0].attr = cbxCfpRuleAttrUDF0;
    rule.attr_selector[0].attr_val[7] = 0x08;
    rule.attr_selector[0].attr_len = 8;
    rule.attr_selector[0].attr_mask[7] = 0xFF;
    rule.attr_selector[1].attr = cbxCfpRuleAttrUDF1;
    rule.attr_selector[1].attr_val[4] = 0x11;
    rule.attr_selector[1].attr_len = 8;
    rule.attr_selector[1].attr_mask[4] = 0xFF;
    rule.attr_selector[2].attr = cbxCfpRuleAttrUDF3;
    rule.attr_selector[2].attr_val[0] = 67;
    rule.attr_selector[2].attr_val[2] = 68;
    rule.attr_selector[2].attr_len = 4;
    rule.attr_selector[2].attr_mask[0] = 0xFF;
    rule.attr_selector[2].attr_mask[2] = 0xFF;
    rv = cbx_cfp_rule_set(&cfpid, &rule, index);
    if (rv != 0) {
        sal_printf(" Failed to add entry\n");
    }
    index = index + 2;
    rule.attr_selector[2].attr_val[0] = 68;
    rule.attr_selector[2].attr_val[2] = 67;
    rule.attr_selector[2].attr_len = 4;
    rule.attr_selector[2].attr_mask[0] = 0xFF;
    rule.attr_selector[2].attr_mask[2] = 0xFF;
    rv = cbx_cfp_rule_set(&cfpid, &rule, index);
    if (rv != 0) {
        sal_printf(" Failed to add entry\n");
    }
    /* Ctag */
    index = index + 2;
    rule.num_attributes = 4;
    for (i = 0; i < CBX_CFP_UDF_ATTR_MAX; i++) {
        sal_memset(rule.attr_selector[i].attr_val, 0, 16 * sizeof(uint8_t));
        sal_memset(rule.attr_selector[i].attr_mask,0, 16 * sizeof(uint8_t));
    }
    rule.attr_selector[0].attr = cbxCfpRuleAttrUDF0;
    rule.attr_selector[0].attr_val[3] = 0x08;
    rule.attr_selector[0].attr_len = 8;
    rule.attr_selector[0].attr_mask[3] = 0xFF;
    rule.attr_selector[1].attr = cbxCfpRuleAttrUDF1;
    rule.attr_selector[1].attr_val[0] = 0x11;
    rule.attr_selector[1].attr_len = 8;
    rule.attr_selector[1].attr_mask[0] = 0xFF;
    rule.attr_selector[2].attr = cbxCfpRuleAttrUDF4;
    rule.attr_selector[2].attr_val[0] = 67;
    rule.attr_selector[2].attr_len = 2;
    rule.attr_selector[2].attr_mask[0] = 0xFF;
    rule.attr_selector[3].attr = cbxCfpRuleAttrUDF5;
    rule.attr_selector[3].attr_val[0] = 68;
    rule.attr_selector[3].attr_len = 2;
    rule.attr_selector[3].attr_mask[0] = 0xFF;
    rv = cbx_cfp_rule_set(&cfpid, &rule, index);
    if (rv != 0) {
        sal_printf(" Failed to add entry\n");
    }
    index = index + 2;
    rule.attr_selector[0].attr = cbxCfpRuleAttrUDF0;
    rule.attr_selector[0].attr_val[3] = 0x08;
    rule.attr_selector[0].attr_len = 8;
    rule.attr_selector[0].attr_mask[3] = 0xFF;
    rule.attr_selector[1].attr = cbxCfpRuleAttrUDF1;
    rule.attr_selector[1].attr_val[0] = 0x11;
    rule.attr_selector[1].attr_len = 8;
    rule.attr_selector[1].attr_mask[0] = 0xFF;
    rule.attr_selector[2].attr = cbxCfpRuleAttrUDF4;
    rule.attr_selector[2].attr_val[0] = 68;
    rule.attr_selector[2].attr_len = 2;
    rule.attr_selector[2].attr_mask[0] = 0xFF;
    rule.attr_selector[3].attr = cbxCfpRuleAttrUDF5;
    rule.attr_selector[3].attr_val[0] = 67;
    rule.attr_selector[2].attr_len = 2;
    rule.attr_selector[3].attr_mask[0] = 0xFF;
    rv = cbx_cfp_rule_set(&cfpid, &rule, index);
    if (rv != 0) {
        sal_printf(" Failed to add entry\n");
    }
    /* Stag */
    index = index + 2;
    rule.num_attributes = 4;
    for (i = 0; i < CBX_CFP_UDF_ATTR_MAX; i++) {
        sal_memset(rule.attr_selector[i].attr_val, 0, 16 * sizeof(uint8_t));
        sal_memset(rule.attr_selector[i].attr_mask,0, 16 * sizeof(uint8_t));
    }
    rule.attr_selector[0].attr = cbxCfpRuleAttrUDF1;
    rule.attr_selector[0].attr_val[7] = 0x08;
    rule.attr_selector[0].attr_len = 8;
    rule.attr_selector[0].attr_mask[7] = 0xFF;
    rule.attr_selector[1].attr = cbxCfpRuleAttrUDF2;
    rule.attr_selector[1].attr_val[0] = 0x11;
    rule.attr_selector[1].attr_len = 4;
    rule.attr_selector[1].attr_mask[0] = 0xFF;
    rule.attr_selector[2].attr = cbxCfpRuleAttrUDF6;
    rule.attr_selector[2].attr_val[0] = 67;
    rule.attr_selector[2].attr_len = 2;
    rule.attr_selector[2].attr_mask[0] = 0xFF;
    rule.attr_selector[3].attr = cbxCfpRuleAttrUDF7;
    rule.attr_selector[3].attr_val[0] = 68;
    rule.attr_selector[3].attr_len = 2;
    rule.attr_selector[3].attr_mask[0] = 0xFF;
    rv = cbx_cfp_rule_set(&cfpid, &rule, index);
    if (rv != 0) {
        sal_printf(" Failed to add entry\n");
    }
    index = index + 2;
    rule.attr_selector[2].attr_val[0] = 68;
    rule.attr_selector[2].attr_mask[0] = 0xFF;
    rule.attr_selector[3].attr_val[0] = 67;
    rule.attr_selector[3].attr_mask[0] = 0xFF;
    rv = cbx_cfp_rule_set(&cfpid, &rule, index);
    if (rv != 0) {
        sal_printf(" Failed to add entry\n");
    }
    for (i = 0; i < CBX_CFP_UDF_ATTR_MAX; i++) {
        sal_free(rule.attr_selector[i].attr_val);
        sal_free(rule.attr_selector[i].attr_mask);
    }
    sal_free(rule.attr_selector);
}

#ifdef CONFIG_IGMP_SNOOPING
void cfp_udf_igmp_rule_add(cbx_cfpid_t cfpid, uint32 index) {
    cbx_cfp_rule_t rule;
    int rv = 0; int i  = 0;
    rule.key_type = cbxCfpKeyUDFACL;
    rule.attr_selector = sal_alloc((CBX_CFP_UDF_ATTR_MAX * sizeof(cbx_cfp_rule_attr_selector_t)), "CFP_RULE");
    for (i = 0; i < CBX_CFP_UDF_ATTR_MAX; i++) {
        rule.attr_selector[i].attr_val = sal_alloc(16,"CFP_RULE");
        rule.attr_selector[i].attr_mask = sal_alloc(16,"CFP_RULE");
        sal_memset(rule.attr_selector[i].attr_val, 0, 16 * sizeof(uint8_t));
        sal_memset(rule.attr_selector[i].attr_mask,0, 16 * sizeof(uint8_t));
    }
    rule.num_attributes = 2;
    rule.attr_selector[0].attr = cbxCfpRuleAttrUDF0;
    rule.attr_selector[0].attr_val[7] = 0x08;
    rule.attr_selector[0].attr_len = 8;
    rule.attr_selector[0].attr_mask[7] = 0xFF;
    rule.attr_selector[1].attr = cbxCfpRuleAttrUDF1;
    rule.attr_selector[1].attr_val[4] = 0x2;
    rule.attr_selector[1].attr_len = 8;
    rule.attr_selector[1].attr_mask[4] = 0xFF;
    rv = cbx_cfp_rule_set(&cfpid, &rule, index);
    if (rv != 0) {
        sal_printf(" Failed to add entry\n");
    }
    /* Ctag */
    index = index + 2;
    rule.num_attributes = 2;
    for (i = 0; i < CBX_CFP_UDF_ATTR_MAX; i++) {
        sal_memset(rule.attr_selector[i].attr_val, 0, 16 * sizeof(uint8_t));
        sal_memset(rule.attr_selector[i].attr_mask,0, 16 * sizeof(uint8_t));
    }
    rule.attr_selector[0].attr = cbxCfpRuleAttrUDF0;
    rule.attr_selector[0].attr_val[3] = 0x08;
    rule.attr_selector[0].attr_len = 8;
    rule.attr_selector[0].attr_mask[3] = 0xFF;
    rule.attr_selector[1].attr = cbxCfpRuleAttrUDF1;
    rule.attr_selector[1].attr_val[0] = 0x2;
    rule.attr_selector[1].attr_len = 8;
    rule.attr_selector[1].attr_mask[0] = 0xFF;
    rv = cbx_cfp_rule_set(&cfpid, &rule, index);
    if (rv != 0) {
        sal_printf(" Failed to add entry\n");
    }
    /* Stag */
    index = index + 2;
    rule.num_attributes = 2;
    for (i = 0; i < CBX_CFP_UDF_ATTR_MAX; i++) {
        sal_memset(rule.attr_selector[i].attr_val, 0, 16 * sizeof(uint8_t));
        sal_memset(rule.attr_selector[i].attr_mask,0, 16 * sizeof(uint8_t));
    }
    rule.attr_selector[0].attr = cbxCfpRuleAttrUDF1;
    rule.attr_selector[0].attr_val[7] = 0x08;
    rule.attr_selector[0].attr_len = 8;
    rule.attr_selector[0].attr_mask[7] = 0xFF;
    rule.attr_selector[1].attr = cbxCfpRuleAttrUDF2;
    rule.attr_selector[1].attr_val[0] = 0x2;
    rule.attr_selector[1].attr_len = 1;
    rule.attr_selector[1].attr_mask[0] = 0xFF;
    rv = cbx_cfp_rule_set(&cfpid, &rule, index);
    if (rv != 0) {
        sal_printf(" Failed to add entry\n");
    }

    for (i = 0; i < CBX_CFP_UDF_ATTR_MAX; i++) {
        sal_free(rule.attr_selector[i].attr_val);
        sal_free(rule.attr_selector[i].attr_mask);
    }
    sal_free(rule.attr_selector);
}

void cfp_udf_igmp_rule_clear(cbx_cfpid_t cfpid, uint32 index) {
    int rv = 0;

    rv = cbx_cfp_rule_clear(&cfpid, index);
    if (rv != 0) {
        sal_printf(" Failed to clear CFP rule %d\n", index);
    }
    /* Ctag */
    index = index + 2;
    rv = cbx_cfp_rule_clear(&cfpid, index);
    if (rv != 0) {
        sal_printf(" Failed to clear CFP rule %d\n", index);
    }
    /* Stag */
    index = index + 2;
    rv = cbx_cfp_rule_clear(&cfpid, index);
    if (rv != 0) {
        sal_printf(" Failed to clear CFP rule %d\n", index);
    }
}
#endif

/* CFP ID */
static cbx_cfpid_t  cfpid = 0;

/**
 * CBX Fsal Get cfp id
 *
 * @param  pointer to cfp id to be returned
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbx_fsal_get_cfp_id(cbx_cfpid_t *pcfpid)
{
    if (pcfpid == NULL)
        return CBX_E_PARAM;

    *pcfpid = cfpid;
    return CBX_E_NONE;
}

/**
 * CBX Fsal init
 *
 * @param  None
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int
cbx_fsal_init(void)
{
    int rv = CBX_E_NONE;

#ifdef CONFIG_LINK_SCAN
    cbx_link_scan_params_t link_scan_params;
#endif
    cbx_auth_params_t auth_params;

    if (SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) {
        soc_robo2_arl_table_init();
    }
#if 0 /* Initialized as part of trunk module. Trunk module is accessing
       * slictcam, to be initialized before trunk module init */
    /* Init modules */
    sal_printf("cbxi_slictcam_init()\n");
    rv = cbxi_slictcam_init();
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbxi_slictcam_init: rv = %d\n"), rv));
    }
#endif

    sal_printf("cbxi_lin_init()\n");
    rv = cbxi_lin_init();
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbxi_lin_init: rv = %d\n"), rv));
    }
#if 0 /* Initialized as part of SDK modules init */
    sal_printf("cbx_port_init()\n");
    rv = bcm_robo_port_init(0);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbx_port_init: rv = %d\n"), rv));
    }

    /* Initialized as part of SDK modules init */
    sal_printf("cbx_lag_init()\n");
    rv = cbx_lag_init();
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbx_lag_init: rv = %d\n"), rv));
    }
#endif
    

    sal_printf("cbxi_trap_init()\n");
    rv = cbxi_trap_init();
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbxi_trap_init: rv = %d\n"), rv));
    }

#if 0 /* it is initialized as part of SDK modules init */
    sal_printf("cbxi_stg_init()\n");
    rv = cbxi_stg_init();
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbxi_stg_init: rv = %d\n"), rv));
    }

    sal_printf("cbxi_vlan_init()\n");
    rv = cbxi_vlan_init();
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbxi_vlan_init: rv = %d\n"), rv));
    }
    sal_printf("cbxi_l2_init()\n");
    rv = cbxi_l2_init();
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbxi_l2_init: rv = %d\n"), rv));
    }


    sal_printf("cbxi_mcast_init()\n");
    rv = cbxi_mcast_init();
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbxi_mcast_init: rv = %d\n"), rv));
    }

#ifdef CONFIG_PTP
    sal_printf("cbx_ptp_init()\n");
    rv = cbx_ptp_init();
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbx_ptp_init: rv = %d\n"), rv));
    }
#endif
    sal_printf("cbx_mirror_init()\n");
    rv = cbx_mirror_init();
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbx_mirror_init: rv = %d\n"), rv));
    }

    sal_printf("cbx_meter_init()\n");
    rv = cbx_meter_init();
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbxi_meter_init: rv = %d\n"), rv));
    }

    sal_printf("cbx_cosq_init()\n");
    rv = cbx_cosq_init();
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbxi_cosq_init: rv = %d\n"), rv));
    }
#endif

#if 0 /* Enable to bringup switch in Port Extender mode */
    sal_printf("cbx_extender_init()\n");
    rv = cbx_extender_init(&ext_params);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbx_extender_init: rv = %d\n"), rv));
    }
#endif

#if 0 /* it is initialized as part of SDK modules init */
    sal_printf("cbx_stat_init()\n");
    rv = cbx_stat_init();
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbx_stat_init: rv = %d\n"), rv));
    }

    sal_printf("cbx_cfp_init()\n");
    cfp_params.stage = cbxCfpStageIngress;
    rv = cbx_cfp_init(&cfp_params, &cfpid);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbx_stat_init: rv = %d\n"), rv));
    }
#endif
#ifndef CONFIG_EXTERNAL_HOST
    if (SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_A0) {
        cfp_action_add(cfpid);
        cfp_udf_dhcp_rule_add(cfpid, 0);
    }
#endif

    sal_printf("cbx_auth_init()\n");
    cbx_auth_params_t_init(&auth_params);
    rv = cbx_auth_init(&auth_params);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbx_auth_init: rv = %d\n"), rv));
    }

#if 0
    rv = avg_vlan_config();
    if (rv < 0) {
        sal_printf("avg_vlan_config: rv = %d\n", rv);
    }

#endif
#ifdef CONFIG_VIRTUAL_PORT_SUPPORT
    sal_printf("cbxi_lif_init()\n");
    rv = cbxi_lif_init();
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbx_lif_init: rv = %d\n"), rv));
    }
#endif

#ifdef CONFIG_LINK_SCAN
    link_scan_params.flags = 0;
    link_scan_params.usec = 100000; /* 100 ms */
    sal_printf("cbx_link_scan_init()\n");
    rv = cbx_link_scan_init(&link_scan_params);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbx_link_scan_init: rv = %d\n"), rv));
    }
    sal_printf("cbx_link_scan_enable_set()\n");
    rv = cbx_link_scan_enable_set(&link_scan_params);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbx_link_scan_enable_set: rv = %d\n"), rv));
    }
#endif

#ifdef CONFIG_PORT_EXTENDER /* Enable to bringup switch in Port Extender mode */
/* Once extender init is done, normal switch functionalities will not work */
    sal_printf("cbx_extender_init()\n");
    rv = cbx_extender_init();
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbx_extender_init: rv = %d\n"), rv));
    }
#ifndef CONFIG_EXTERNAL_HOST
#if 0 /* Sample configuration */
    sal_printf("avg_ext_config()\n");
    rv = avg_ext_config();
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbx_extender_init: rv = %d\n"), rv));
    }
#endif
#endif /* !CONFIG_EXTERNAL_HOST */
#endif /* CONFIG_PORT_EXTENDER */


    sal_printf("cbx_pktio_init()\n");
    rv = cbx_pktio_init(NULL);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("cbx_pktio_init: rv = %d\n"), rv));
    }
    /* Complete all the module inits */
    rv = soc_robo2_pkt_flow_init(0);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_FSAL_COMMON,
            (BSL_META("soc_robo2_pkt_flow_init: rv = %d\n"), rv));
    }

#ifdef CONFIG_CASCADED_MODE
#ifndef CONFIG_EXTERNAL_HOST
    if (SOC_IS_CASCADED(0)) {
        sal_printf("Cascade Init(MasterBmp=%x SlaveBmp=%x)\n",
                   CBX_CASCADE_PORTS_MASTER, CBX_CASCADE_PORTS_SLAVE);
        rv = avg_cascade_init(CBX_CASCADE_PORTS_MASTER, CBX_CASCADE_PORTS_SLAVE,
                              CBX_PORT_ICPU, CBX_CASCADE_TRAP_DEST);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_FSAL_COMMON,
                (BSL_META("avg_cascade_init: rv = %d\n"), rv));
        }
    }
#endif
#endif
    return rv;
}

/**
 * CBX Fsal uninit
 *
 * @param  None
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int
cbx_fsal_uninit(void)
{
    return CBX_E_NONE;
}

#ifndef CONFIG_EXTERNAL_HOST
/*
 *
 * dump_pktio_stats
 *    Dump stats for pktio module
 * @param None
 * @return None
 *
 */
void
dump_pktio_stats(void)
{
    hpa_dump_stats();
    tx_dump_stats();
    rx_dump_stats();
}
#else
void
dump_pktio_stats(void)
{
    extern void packetio_dump_stats(void);
    tx_dump_stats();
    packetio_dump_stats();
}
#endif

void cutthru_enable(void)
{
    int i;
    uint32_t status;
    rpslict_t ent_rpslict;
    uint32 reg_val = 0;
    uint32 temp;
    for (i= 0; i < SLICID_MAX_INDEX; i++) {
        soc_robo2_rpslict_get(0, i, &ent_rpslict);
        ent_rpslict.early_ack = 1;
        soc_robo2_rpslict_set(0, i, &ent_rpslict, &status);
    }
    reg_val = 0;
    temp = 64;
    soc_CB_BMU_IPP_INTF_GLOBAL_EARLY_ACK_OFFSETr_field_set(0, &reg_val, EARLY_ACK_OFFSETf, &temp);
    REG_WRITE_CB_BMU_IPP_INTF_GLOBAL_EARLY_ACK_OFFSETr(0, &reg_val);

    reg_val = 0;

    temp = 64;
    soc_CB_PQM_CUTTHRULENr_field_set(0, &reg_val, LENf, &temp);
    REG_WRITE_CB_PQM_CUTTHRULENr(0, &reg_val);

}

void cutthru_disable(void)
{
    int i;
    uint32_t status;
    rpslict_t ent_rpslict;
    uint32 reg_val = 0;
    uint32 temp;
    for (i= 0; i < SLICID_MAX_INDEX; i++) {
        soc_robo2_rpslict_get(0, i, &ent_rpslict);
        ent_rpslict.early_ack = 0;
        soc_robo2_rpslict_set(0, i, &ent_rpslict, &status);
    }
    reg_val = 0;
    temp = 0x3fff;
    soc_CB_BMU_IPP_INTF_GLOBAL_EARLY_ACK_OFFSETr_field_set(0, &reg_val, EARLY_ACK_OFFSETf, &temp);
    REG_WRITE_CB_BMU_IPP_INTF_GLOBAL_EARLY_ACK_OFFSETr(0, &reg_val);

    reg_val = 0;
    temp = 0x0;
    soc_CB_PQM_CUTTHRULENr_field_set(0, &reg_val, LENf, &temp);
    REG_WRITE_CB_PQM_CUTTHRULENr(0, &reg_val);

}


