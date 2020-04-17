/*
 * * $ID: $
 * *
 * * 
 * * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 * * 
* * $Copyright: (c) 2020 Broadcom Inc.
* * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef MANAGED_MODE
#include <shared/bsl.h>
#include <sal/types.h>
#include <fsal_int/shared_types.h>
#include <sal/core/alloc.h>
#include <sal/core/libc.h>
#include <tables.h>
#include <regacc.h>
#else
#include <bsl_log/bsl.h>
#include <sal_types.h>
#include <soc/types.h>
#include <soc/robo2/common/tables.h>
#include <sal_alloc.h>
#endif
#include <fsal/error.h>
#include <fsal/l2.h>
#include <fsal/extender.h>
#include <fsal/vlan.h>
#include <fsal/lag.h>
#include <fsal_int/port.h>
#include <fsal_int/slic.h>
#include <fsal_int/lin.h>
#include <fsal_int/encap.h>
#include <fsal_int/mcast.h>
#include <fsal_int/vlan.h>
#include <fsal_int/trap.h>
#include <fsal_int/extender.h>
#ifndef CONFIG_EXTERNAL_HOST
#include <fsal_int/stg.h>
#endif
#include <fsal_int/lag.h>


#define EPMT_PMT_DATA_DEI_0             0x02468ACE
#define EPMT_PMT_DATA_DEI_1             0x13579BDF

uint16_t  lb_port = CBX_PE_LB_PORT;
uint16_t  lb_lpg =  CBX_PE_LB_PORT;
uint16_t  avg_csd_lpg[CBX_MAX_UNITS];

int cbxi_pe_port_lb_encap_set(cbx_portid_t portid, uint8_t enable);

extern int cbxi_slicid_map(int unit, cbxi_slic_rule_id_t rule_number,
        cbxi_slicid_t slicid, cbxi_slic_trap_t slic_trap);

cbx_extender_sw_info_t cbx_extender_info;

/* Loopback port 12G bandwidth Calendar values
 * Only calendar values that are different from defaults are configured */
static cbxi_pqs_cal_info_t cbxi_pqs_cal_val[] = {
    /*IDX,  en , port */
    { 0x03, 0x1, 0x0E},
    { 0x11, 0x1, 0x0E},
    { 0x1F, 0x1, 0x0E},
    { 0x21, 0x1, 0x00},
    { 0x2D, 0x1, 0x0E},
    { 0x2F, 0x1, 0x02},
    { 0x3B, 0x1, 0x0E},
    { 0x3D, 0x1, 0x04},
    { 0x49, 0x1, 0x0E},
    { 0x50, 0x1, 0x08},
    { 0x57, 0x1, 0x0E},
    { 0x5E, 0x1, 0x0A},
    { 0x65, 0x1, 0x0E},
    { 0x67, 0x1, 0x06},
    { 0x81, 0x1, 0x0E},
    { -1  , 0x0, 0x00} /* Dummy entry to indicate end of array */
};
static cbxi_epp_cal_info_t cbxi_epp_cal_val[] = {
    /*IDX,, port */
    { 0x01, 0x0E},
    { 0x05, 0x0E},
    { 0x09, 0x0E},
    { 0x0D, 0x0A},
    { 0x11, 0x0E},
    { 0x15, 0x0E},
    { 0x19, 0x0E},
    { 0x1D, 0x0B},
    { 0x21, 0x0E},
    { 0x25, 0x0E},
    { 0x29, 0x0E},
    { 0x2D, 0x0F},
    { 0x31, 0x0E},
    { 0x35, 0x0E},
    { 0x39, 0x0E},
    { 0x3D, 0x0A},
    { 0x41, 0x0E},
    { 0x45, 0x0E},
    { 0x49, 0x0E},
    { 0x4B, 0x08},
    { 0x4D, 0x0B},
    { 0x4F, 0x09},
    { -1  , 0x00} /* Dummy entry to indicate end of array */
};

/*
 * Function:
 *  cbxi_pe_lb_12G_bw
 * Purpose:
 *  Configure Loopback port to handle 12G bandwidth
 *
 */
STATIC int
cbxi_pe_lb_12G_bw(int unit)
{
    epp_calendar_t ent_epp_cal;
    calendar_t     ent_pqs_cal;
    uint32_t       status = 0;
    int            id;

    CBX_IF_ERROR_RETURN(soc_robo2_calendar_enable(unit));
    CBX_IF_ERROR_RETURN(soc_robo2_calendar_reset(unit));

    CBX_IF_ERROR_RETURN(soc_robo2_epp_calendar_enable(unit));
    CBX_IF_ERROR_RETURN(soc_robo2_epp_calendar_reset(unit));

    for (id = 0; cbxi_pqs_cal_val[id].idx >= 0; id++) {
        CBX_IF_ERROR_RETURN(soc_robo2_calendar_get(
                unit, cbxi_pqs_cal_val[id].idx, &ent_pqs_cal));
        ent_pqs_cal.slot_port_sel_en = cbxi_pqs_cal_val[id].slot_en;
        ent_pqs_cal.slot_port        = cbxi_pqs_cal_val[id].slot_port;
        CBX_IF_ERROR_RETURN(soc_robo2_calendar_set(
                 unit, cbxi_pqs_cal_val[id].idx, &ent_pqs_cal, &status));
    }
    for (id = 0; cbxi_epp_cal_val[id].idx >= 0; id++) {
        CBX_IF_ERROR_RETURN(soc_robo2_epp_calendar_get(
                unit, cbxi_epp_cal_val[id].idx, &ent_epp_cal));
        ent_epp_cal.slot_port        = cbxi_epp_cal_val[id].slot_port;
        CBX_IF_ERROR_RETURN(soc_robo2_epp_calendar_set(
                 unit, cbxi_epp_cal_val[id].idx, &ent_epp_cal, &status));
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_port_epmt_set
 * Purpose:
 *  Configure default PCP,DEI map for given port group
 */
STATIC int
cbxi_pe_port_epmt_set(int unit, cbxi_pgid_t lpg)
{
    epmt_t   ent_epmt;
    uint32_t status = 0;

    /* Set pri_map index for offset = lpg */
    /* update EPMT entry */
    ent_epmt.pmt_data = EPMT_PMT_DATA_DEI_0; /* DEI = 0 ; GREEN */
    CBX_IF_ERROR_RETURN(
              soc_robo2_epmt_set(unit, (lpg * 3), &ent_epmt, &status));

    /* update EPMT entry */
    ent_epmt.pmt_data = EPMT_PMT_DATA_DEI_1; /* DEI = 1 ; YELLOW, RED */
    CBX_IF_ERROR_RETURN(
              soc_robo2_epmt_set(unit, ((lpg * 3) + 1), &ent_epmt, &status));
    CBX_IF_ERROR_RETURN(
              soc_robo2_epmt_set(unit, ((lpg * 3) + 2), &ent_epmt, &status));
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_check_port_extender_mode
 * Purpose:
 *  Check if system is in Port extender mode
 *  return CBX_E_NONE if in PE mode
 */
int
cbxi_check_port_extender_mode(void)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    CHECK_EXTENDER_INIT(ext_info);
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_avg_csd_config
 * Purpose:
 *  Configure local variables for Avg cascade ports
 */
int
cbxi_pe_avg_csd_config(void)
{
    uint32_t regval = 0;
    uint32_t fval = 0;
    uint8    num_units = 1;
    int      unit = 0;

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    } else {
        return CBX_E_NONE;
    }

    for (unit = 0; unit < num_units; unit++) {
        CBX_IF_ERROR_RETURN(REG_READ_CB_PQM_CASCADEr(unit, &regval));
        soc_CB_PQM_CASCADEr_field_get(unit, &regval, ENABLEf, &fval);
        if (fval == 0) {
            return CBX_E_NONE;
        }
        soc_CB_PQM_CASCADEr_field_get(unit, &regval, LSPGIDf, &fval);
        avg_csd_lpg[unit] = fval;
    }


    return CBX_E_NONE;
}


/*
 * Function:
 *  cbxi_pe_mode_get
 * Purpose:
 *  Get PE modes Access/Transit
 */
int
cbxi_pe_mode_get(cbxi_extender_mode_t *mode)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    CHECK_EXTENDER_INIT(ext_info);

    *mode = ext_info->mode;
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_mode_set
 * Purpose:
 *  Set PE modes Access/Transit
 */
int
cbxi_pe_mode_set(cbxi_extender_mode_t mode)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    fpslict_t    ent_fpslict;
    pbmp_t       pg_map = 0;
    int          slic_idx;
    int          unit = 0;
    uint32_t     status = 0;
    uint8        num_units = 1;


    CHECK_EXTENDER_INIT(ext_info);

    ext_info->mode = mode;
    if (mode != cbxiPEModeTransit) {
        return CBX_E_NONE;
    }

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    for (unit = 0; unit < num_units; unit++) {
        for (slic_idx = SLIC_PE_1BR_IN_ECID_U0_P0;
                    slic_idx < SLIC_PE_1BR_DECAP; slic_idx++) {
            /* Enable Uplink port for all MCAST Ingress ECID rules + DECAP rule*/
            /* Access PE : Enable these rules on Uplink port
             * Transit PE: Enable these rules on Port 14*/

            CBX_PBMP_PORT_ADD(pg_map, lb_lpg);
            CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit, slic_idx, pg_map));
        }

        CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit, SLIC_PE_1BR_DECAP, pg_map));

        CBX_IF_ERROR_RETURN(soc_robo2_fpslict_get(
                            unit, SLICID_PE_ETAG_DECAP, &ent_fpslict));
        ent_fpslict.default_sli &= 0x3F000;
        ent_fpslict.default_sli = (0x20000 | ((lb_lpg | (0x10 * unit)) << 12) |
                                    ext_info->downlink_vsi_ext_mc[unit]);
        CBX_IF_ERROR_RETURN(soc_robo2_fpslict_set(
                        unit, SLICID_PE_ETAG_DECAP, &ent_fpslict, &status));
    }

    return CBX_E_NONE;

}

/*
 * Function:
 *  cbxi_pe_ext_port_init
 * Purpose:
 *  Add PE extender port to pgmap for PE ext VSI
 */
int
cbxi_pe_ext_port_init(cbx_portid_t portid)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    cbxi_encap_record_t  encap_record;
    cbx_lag_info_t       lag_info;
    cbxi_slic_rule_t     slic_rule;
    cbxi_slic_rule_id_t  rule_idx, uc_idx;
    vsit_t         ent_vsit;
    pgt_t          ent_pgt;
    dliet_t        ent_dliet;
    dli2ldpg_t     ent_dli2ldpg;
    cbxi_pgid_t    up_port_lpg, ex_port_lpg;
    cbxi_pgid_t    lpg[CBX_MAX_UNITS];
    uint8_t        num_units = 1;
    cbx_port_t     port;
    pbmp_t         pg_map = 0;
    int            unit = 0;
    int            partner_unit = 0;
    int            i, lin, vsit_idx;
    uint32_t       status = 0;

    CHECK_EXTENDER_INIT(ext_info);

    /* **************** Downstream settings *********************** */
    CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                                portid, &port, &ex_port_lpg, &unit));

    if (ex_port_lpg == CBXI_PGID_INVALID) {
        return CBX_E_NONE;
    }

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
        partner_unit = CBXI_CASC_PARTNER_UNIT(unit);
    }

    lpg[CBX_AVENGER_PRIMARY]   = CBXI_PGID_INVALID;
    lpg[CBX_AVENGER_SECONDARY] = CBXI_PGID_INVALID;

    if (CBX_PORTID_IS_TRUNK(portid)) {
        /* Configure UCAST DLI to given LPG */
        CBX_IF_ERROR_RETURN(cbxi_robo2_lag_get(portid, &lag_info));
        CBX_IF_ERROR_RETURN(cbxi_pe_lag_dest_set(&lag_info));
        lpg[CBX_AVENGER_PRIMARY]   = lag_info.lpgid;
        lpg[CBX_AVENGER_SECONDARY] = lag_info.rlpgid;
    } else {
        /* UCAST DLI of Ext Physical ports are always set to LB port 14 */
        /* Enable CB tag encap */
        CBX_IF_ERROR_RETURN(cbxi_pe_port_lb_encap_set(portid, TRUE));

        /* Configure MCAST DLI to given LPG */
        CBX_IF_ERROR_RETURN(cbxi_pe_port_sli_get(portid, &lin));
        CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(unit, lin, ex_port_lpg));
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(
                    partner_unit, lin, avg_csd_lpg[partner_unit]));
        }

        lpg[unit] = ex_port_lpg;
    }

    /* Enable UC & MC SLIC rules, if previously configured with ECIDs */
    if (CBX_PORTID_IS_TRUNK(portid)) {
        port = CBX_PORTID_TRUNK_GET(portid);
        uc_idx   = (SLIC_PE_1BR_UC_ETAG_LAG0 + port);
        rule_idx = (SLIC_PE_1BR_IN_ECID_LAG0 + port);
    } else {
        uc_idx   = (SLIC_PE_1BR_UC_ETAG_U0_P0 + CBXI_GLOBAL_PORT(port, unit));
        rule_idx = (SLIC_PE_1BR_IN_ECID_U0_P0 + CBXI_GLOBAL_PORT(port, unit));
    }
    CBX_IF_ERROR_RETURN(cbxi_slicmap_rule_get(unit, uc_idx, &slic_rule));
    if ((slic_rule.tci3 & 0x3FFF) > 0) {
        /* ECID configured for UC rule. Enable SLIC rule */
        CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(unit, uc_idx, TRUE));
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(
                                                partner_unit, uc_idx, TRUE));
        }
    }
    CBX_IF_ERROR_RETURN(cbxi_slicmap_rule_get(unit, rule_idx, &slic_rule));
    if ((slic_rule.tci2 & 0xFFF) > 0) {
        CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(unit, rule_idx, TRUE));
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(
                                                partner_unit, rule_idx, TRUE));
        }
    }

    for (i = 0; i < num_units; i++) {
        if (lpg[i] == CBXI_PGID_INVALID) {
            continue;
        }
        /* Add to Downlink VSI ext */
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(i,
                        ext_info->downlink_vsi_ext_uc, &ent_vsit));
        ent_vsit.pg_map |= (1 << lpg[i]);
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(i,
                        ext_info->downlink_vsi_ext_uc, &ent_vsit, &status));

        CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(i,
                    ext_info->downlink_vsi_ext_mc[unit], &ent_vsit));
        ent_vsit.pg_map |= (1 << lpg[i]);
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(i,
                    ext_info->downlink_vsi_ext_mc[unit], &ent_vsit, &status));
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            vsit_idx = ext_info->downlink_vsi_ext_mc[CBXI_CASC_PARTNER_UNIT(unit)];
            CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(i, vsit_idx, &ent_vsit));
            ent_vsit.pg_map |= (1 << lpg[i]);
            CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(i,
                                                vsit_idx, &ent_vsit, &status));
        }

        /* Remove from  Downlink VSI csd */
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(i,
                    ext_info->downlink_vsi_csd_uc, &ent_vsit));
        ent_vsit.pg_map &= ~(1 << lpg[i]);
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(i,
                    ext_info->downlink_vsi_csd_uc, &ent_vsit, &status));

        CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(i,
                    ext_info->downlink_vsi_csd_mc[unit], &ent_vsit));
        ent_vsit.pg_map &= ~(1 << lpg[i]);
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(i,
                    ext_info->downlink_vsi_csd_mc[unit], &ent_vsit, &status));
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            vsit_idx = ext_info->downlink_vsi_csd_mc[CBXI_CASC_PARTNER_UNIT(unit)];
            CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(i, vsit_idx, &ent_vsit));
            ent_vsit.pg_map &= ~(1 << lpg[i]);
            CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(i,
                                                vsit_idx, &ent_vsit, &status));
        }
    }

    /* ****************** Upstream settings *********************** */
    CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                                        portid, &port, &ex_port_lpg, &unit));

    /* Add to extender port SLIC rules ETAG drop, Non ETAG fwd */
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_get(unit, SLIC_PE_1BR_DROP, &pg_map));
    CBX_PBMP_PORT_ADD(pg_map, ex_port_lpg);
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit, SLIC_PE_1BR_DROP, pg_map));

    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_get(unit, SLIC_PE_UNTAG_FWD, &pg_map));
    CBX_PBMP_PORT_ADD(pg_map, ex_port_lpg);
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit, SLIC_PE_UNTAG_FWD, pg_map));


    /* Remove extender port from SLIC rules ETAG fwd, Non ETAG drop */
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_get(unit, SLIC_PE_1BR_FWD_UPSTR, &pg_map));
    CBX_PBMP_PORT_REMOVE(pg_map, ex_port_lpg);
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit, SLIC_PE_1BR_FWD_UPSTR, pg_map));

    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_get(unit, SLIC_PE_UNTAG_DROP, &pg_map));
    CBX_PBMP_PORT_REMOVE(pg_map, ex_port_lpg);
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit, SLIC_PE_UNTAG_DROP, pg_map));


    /* SLI --> VSI --> DLI mapping */
    /* Note: Since N value in DLI is unique, we can use same value for VSIT
     * index */

    CBX_IF_ERROR_RETURN(soc_robo2_pgt_get(unit, ex_port_lpg, &ent_pgt));
    /* Check if Extender port is already set.
     * Reuse same VSI which is same as N */
    if (PV_SLI_2_VSI(ent_pgt.default_sli) < 2) {
        /* PG is pointing to default VSI of 0 or 1.
         * Extender port set is getting called for first time for this port */
        CBX_IF_ERROR_RETURN(cbxi_lin_slot_get(&lin));

        vsit_idx = lin;
    } else {
        vsit_idx = PV_SLI_2_VSI(ent_pgt.default_sli);
        lin   = vsit_idx;
    }

    CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(unit, vsit_idx, &ent_vsit));
    ent_vsit.learn                = cbxVlanLearnModeDisable;
    ent_vsit.default_is_multicast = 0;
    ent_vsit.dlliid_dgid          = lin; /* Assign same LIN as destination */
    ent_vsit.pg_map               = PBMP_GE_ALL(unit) | PBMP_XL_ALL(unit);
    ent_vsit.fid                  = vsit_idx;
    CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(
                                    unit, vsit_idx, &ent_vsit, &status));
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        /* Set same VSIT in Partner Avenger */
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(
            CBXI_CASC_PARTNER_UNIT(unit),vsit_idx, &ent_vsit, &status));
    }

    ent_pgt.etag_en               = 1;
    ent_pgt.default_sli           = (ent_pgt.default_sli & 0x3F000);
    ent_pgt.default_sli           = (ent_pgt.default_sli | vsit_idx);
    CBX_IF_ERROR_RETURN(soc_robo2_pgt_set(
                                        unit, ex_port_lpg, &ent_pgt, &status));


    /* Create DLI to add E-TAG at uplink port */
    /* Map port to lpgid and set it as destination */
    if (ext_info->uplink_port[unit] != CBX_PORT_INVALID) {
        CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                    ext_info->uplink_port[unit], &port, &up_port_lpg, &i));
    } else if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        up_port_lpg = avg_csd_lpg[partner_unit];
    }

    /* Preserve any previous E-TAG encapsulation at egress of uplink port */
    /* Change only the Opcode at encap0 */
    CBX_IF_ERROR_RETURN(soc_robo2_dliet_get(unit, lin, &ent_dliet));


    encap_record.data[0] = CBX_EGR_DIRECTIVE_INSERT_8B_TAG; /* opcode */
    encap_record.data[1] = ent_dliet.encap1; /* TPID from TET | primap idx */
    encap_record.data[2] = ent_dliet.encap2; /* Hardcoded PCP, DEI */
    encap_record.data[3] = ent_dliet.encap3;
    encap_record.data[4] = ent_dliet.encap4; /* GRP | E-CID */
    encap_record.data[5] = ent_dliet.encap5; /* E-CID */
    encap_record.data[6] = ent_dliet.encap6;
    encap_record.data[7] = ent_dliet.encap7;
    encap_record.len     = 8;

    CBX_EPP_INSERT_EOR(&encap_record);

    /* Update LIN encap only on Avenger where Ext port is present */
    /* The behavior depends on CB_ETM_Cascade_Port_Enable reg value.
     * If Cascade port is set in the above reg, then put same encap directive in
     * both Avengers, else configure DLIET of only one Avenger */
    REG_READ_CB_ETM_CASCADE_PORT_ENABLEr(unit, &status);
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY) && (status == 0)) {
        if (unit) {
            CBX_IF_ERROR_RETURN(cbxi_lin_encap_set(
                        portid, lin, 0, &encap_record, CBXI_ENCAP_AVG_SEC));
        } else {
            CBX_IF_ERROR_RETURN(cbxi_lin_encap_set(
                        portid, lin, 0, &encap_record, CBXI_ENCAP_AVG_PRI));
        }
    } else {
        CBX_IF_ERROR_RETURN(cbxi_lin_encap_set(
                        portid, lin, 0, &encap_record, CBXI_ENCAP_AVG_SYS));
    }

    /* Configure the destination LPG for LIN
     * Tx enable for LIN is done while adding L2 entries, hence preserve
     * previous Tx state */
    ent_dli2ldpg.ldpgid = up_port_lpg;
    CBX_IF_ERROR_RETURN(soc_robo2_dli2ldpg_set(
                                unit, lin, &ent_dli2ldpg, &status));

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        /* Configure cascade port for the lin on partner Avenger
         * This is to handle Uplink failover case */
        unit = CBXI_CASC_PARTNER_UNIT(unit);
        if (ext_info->uplink_port[unit] != CBX_PORT_INVALID) {
            CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                    ext_info->uplink_port[unit], &port, &up_port_lpg, &unit));
            ent_dli2ldpg.ldpgid = up_port_lpg;;
            CBX_IF_ERROR_RETURN(soc_robo2_dli2ldpg_set(
                                unit, lin, &ent_dli2ldpg, &status));
        }
    }

    /* Clear from PE cascade port map */
    SHR_BITCLR(ext_info->pe_cascade_bitmap, CBXI_GLOBAL_PORT(ex_port_lpg, unit));

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_csd_port_init
 * Purpose:
 *  Add PE extender cascade port to pgmap for PE csd VSI
 */
int
cbxi_pe_csd_port_init(cbx_portid_t portid)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    cbx_lag_info_t      lag_info;
    cbxi_slic_rule_id_t rule_idx, uc_idx;
    vsit_t         ent_vsit;
    pgt_t          ent_pgt;
    dliet_t        ent_dliet;
    cbxi_pgid_t    cs_port_lpg;
    cbxi_pgid_t    lpg[CBX_MAX_UNITS];
    uint8_t        num_units = 1;
    cbx_port_t     port;
    pbmp_t         pg_map = 0;
    int            unit = 0;
    int            partner_unit = 0;
    int            i, lin, upstr_lin, vsit_idx;
    uint32_t       status = 0;

    CHECK_EXTENDER_INIT(ext_info);

    /* **************** Downstream settings *********************** */
    CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                                portid, &port, &cs_port_lpg, &unit));

    if (cs_port_lpg == CBXI_PGID_INVALID) {
        return CBX_E_NONE;
    }

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
        partner_unit = CBXI_CASC_PARTNER_UNIT(unit);
    }

    lpg[CBX_AVENGER_PRIMARY]   = CBXI_PGID_INVALID;
    lpg[CBX_AVENGER_SECONDARY] = CBXI_PGID_INVALID;

    if (CBX_PORTID_IS_TRUNK(portid)) {
        /* Configure UCAST DLI to given LPG */
        CBX_IF_ERROR_RETURN(cbxi_robo2_lag_get(portid, &lag_info));
        CBX_IF_ERROR_RETURN(cbxi_pe_lag_dest_set(&lag_info));
        lpg[CBX_AVENGER_PRIMARY]   = lag_info.lpgid;
        lpg[CBX_AVENGER_SECONDARY] = lag_info.rlpgid;
    } else {
        /* Enable CB tag encap */
        CBX_IF_ERROR_RETURN(cbxi_pe_port_lb_encap_set(portid, TRUE));

         /* Configure UCAST DLI to loopback  LPG */
        CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(unit,
            cbx_extender_info.dwn_str_dpp_dli[CBXI_GLOBAL_PORT(port, unit)],
                                                                lb_lpg));

        /* Configure MCAST DLI to given LPG */
        CBX_IF_ERROR_RETURN(cbxi_pe_port_sli_get(portid, &lin));
        CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(unit, lin, cs_port_lpg));
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(
                    partner_unit, lin, avg_csd_lpg[partner_unit]));
        }
        lpg[unit] = cs_port_lpg;
    }

    /* Disable UC & MC SLIC rules
     * (If same port has been used  as PE ext port previously) */
    if (CBX_PORTID_IS_TRUNK(portid)) {
        port = CBX_PORTID_TRUNK_GET(portid);
        uc_idx   = (SLIC_PE_1BR_UC_ETAG_LAG0 + port);
        rule_idx = (SLIC_PE_1BR_IN_ECID_LAG0 + port);
    } else {
        uc_idx   = (SLIC_PE_1BR_UC_ETAG_U0_P0 + CBXI_GLOBAL_PORT(port, unit));
        rule_idx = (SLIC_PE_1BR_IN_ECID_U0_P0 + CBXI_GLOBAL_PORT(port, unit));
    }
    CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(unit, uc_idx, FALSE));
    CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(unit, rule_idx, FALSE));
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(partner_unit, uc_idx, FALSE));
        CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(partner_unit, rule_idx, FALSE));

    }


    for (i = 0; i < num_units; i++) {
        if (lpg[i] == CBXI_PGID_INVALID) {
            continue;
        }
        /* Add to Downlink VSI csd */
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(i,
                        ext_info->downlink_vsi_csd_uc, &ent_vsit));
        ent_vsit.pg_map |= (1 << lpg[i]);
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(i,
                        ext_info->downlink_vsi_csd_uc, &ent_vsit, &status));

        CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(i,
                        ext_info->downlink_vsi_csd_mc[unit], &ent_vsit));
        ent_vsit.pg_map |= (1 << lpg[i]);
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(i,
                        ext_info->downlink_vsi_csd_mc[unit], &ent_vsit, &status));
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            vsit_idx = ext_info->downlink_vsi_csd_mc[CBXI_CASC_PARTNER_UNIT(unit)];
            CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(i, vsit_idx, &ent_vsit));
            ent_vsit.pg_map |= (1 << lpg[i]);
            CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(i,
                                                vsit_idx, &ent_vsit, &status));
        }

        /* Remove from  Downlink VSI ext */
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(i,
                        ext_info->downlink_vsi_ext_uc, &ent_vsit));
        ent_vsit.pg_map &= ~(1 << lpg[i]);
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(i,
                        ext_info->downlink_vsi_ext_uc, &ent_vsit, &status));

        CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(i,
                ext_info->downlink_vsi_ext_mc[unit], &ent_vsit));
        ent_vsit.pg_map &= ~(1 << lpg[i]);
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(i,
                ext_info->downlink_vsi_ext_mc[unit], &ent_vsit, &status));
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            vsit_idx = ext_info->downlink_vsi_ext_mc[CBXI_CASC_PARTNER_UNIT(unit)];
            CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(i, vsit_idx, &ent_vsit));
            ent_vsit.pg_map &= ~(1 << lpg[i]);
            CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(i,
                                                vsit_idx, &ent_vsit, &status));
        }
    }

    /* ****************** Upstream settings *********************** */
    /* Add to PE cascade port to SLIC rules ETAG fwd, Non ETAG drop */
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_get(unit, SLIC_PE_1BR_FWD_UPSTR, &pg_map));
    CBX_PBMP_PORT_ADD(pg_map, cs_port_lpg);
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit, SLIC_PE_1BR_FWD_UPSTR, pg_map));

    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_get(unit, SLIC_PE_UNTAG_DROP, &pg_map));
    CBX_PBMP_PORT_ADD(pg_map, cs_port_lpg);
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit, SLIC_PE_UNTAG_DROP, pg_map));

    /* Remove PE cascade port from SLIC rules ETAG drop, Non ETAG fwd */
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_get(unit, SLIC_PE_1BR_DROP, &pg_map));
    CBX_PBMP_PORT_REMOVE(pg_map, cs_port_lpg);
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit, SLIC_PE_1BR_DROP, pg_map));

    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_get(unit, SLIC_PE_UNTAG_FWD, &pg_map));
    CBX_PBMP_PORT_REMOVE(pg_map, cs_port_lpg);
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit, SLIC_PE_UNTAG_FWD, pg_map));


    /* SLI --> VSI --> DLI mapping */
    /* Note: Since N value in DLI is unique, we can use same value for VSIT
     * index */

    CBX_IF_ERROR_RETURN(soc_robo2_pgt_get(unit, cs_port_lpg, &ent_pgt));
    /* Check if Extender port is already set.
     * Reuse same VSI which is same as N */
    if (PV_SLI_2_VSI(ent_pgt.default_sli) < 2) {
        /* PG is pointing to default VSI of 0 or 1.
         * Extender port set is getting called for first time for this port */
        CBX_IF_ERROR_RETURN(cbxi_lin_slot_get(&lin));

        vsit_idx = lin;
    } else {
        vsit_idx = PV_SLI_2_VSI(ent_pgt.default_sli);
        lin   = vsit_idx;
    }

    /* Upstream VSI has destination LIN same as VSIT idx
     * Destination for this LIN is upstream port without any encap */
    upstr_lin = ext_info->uplink_vsi;

    CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(unit, vsit_idx, &ent_vsit));
    ent_vsit.learn                = cbxVlanLearnModeDisable;
    ent_vsit.default_is_multicast = 0;
    ent_vsit.dlliid_dgid          = upstr_lin; /* Assign upstream LIN as destination */
    ent_vsit.pg_map               = PBMP_GE_ALL(unit) | PBMP_XL_ALL(unit);
    ent_vsit.fid                  = vsit_idx;
    CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(
                                    unit, vsit_idx, &ent_vsit, &status));
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        /* Set same VSIT in Partner Avenger */
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(
            CBXI_CASC_PARTNER_UNIT(unit),vsit_idx, &ent_vsit, &status));
    }

    ent_pgt.etag_en               = 1;
    ent_pgt.default_sli           = (ent_pgt.default_sli & 0x3F000);
    ent_pgt.default_sli           = (ent_pgt.default_sli | vsit_idx);
    CBX_IF_ERROR_RETURN(soc_robo2_pgt_set(
                                        unit, cs_port_lpg, &ent_pgt, &status));

    /* Preserve any previous E-TAG encapsulation at egress of uplink port */
    /* Disable the encap by putting EOR on encap0 */
    CBX_IF_ERROR_RETURN(soc_robo2_dliet_get(unit, lin, &ent_dliet));
    ent_dliet.encap0 = 0xFF;
    CBX_IF_ERROR_RETURN(soc_robo2_dliet_set(unit, lin, &ent_dliet, &status));
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        CBX_IF_ERROR_RETURN(soc_robo2_dliet_get(
                CBXI_CASC_PARTNER_UNIT(unit), lin, &ent_dliet));
        ent_dliet.encap0 = 0xFF;
        CBX_IF_ERROR_RETURN(soc_robo2_dliet_set(
                CBXI_CASC_PARTNER_UNIT(unit), lin, &ent_dliet, &status));
    }

    SHR_BITSET(ext_info->pe_cascade_bitmap,CBXI_GLOBAL_PORT(cs_port_lpg, unit));


    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_shared_lag_get
 * Purpose:
 *  Check if given lagid is shared LAG
 *  Return CBX_E_NONE if its a shared LAG
 */
int
cbxi_pe_shared_lag_get(cbx_portid_t lagid)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    int            rv;
    cbx_port_t     lag_out;

    CHECK_EXTENDER_INIT(ext_info);

    rv = cbxi_robo2_portid_validate(lagid, &lag_out);
    if (CBX_FAILURE(rv)) {
        return CBX_E_PORT;
    }

    if(CBX_PBMP_MEMBER(ext_info->shared_lag_map, lag_out)) {
        return CBX_E_NONE;
    }

    return CBX_E_PARAM;
}

/*
 * Function:
 *  cbxi_pe_shared_lag_set
 * Purpose:
 *  Set given LAGid in Shared LAGid bit map
 */
int
cbxi_pe_shared_lag_set(cbx_portid_t lagid, int enable)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    int            rv;
    cbx_port_t     lag_out;

    CHECK_EXTENDER_INIT(ext_info);

    rv = cbxi_robo2_portid_validate(lagid, &lag_out);
    if (CBX_FAILURE(rv)) {
        return CBX_E_PORT;
    }

    if (enable) {
        CBX_PBMP_PORT_ADD(ext_info->shared_lag_map, lag_out);
    } else {
        CBX_PBMP_PORT_REMOVE(ext_info->shared_lag_map, lag_out);
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_shared_lag_port_get
 * Purpose:
 *  Check if given port is in shared LAG
 *  Return CBX_E_NONE if port is in shared LAG
 */
int
cbxi_pe_shared_lag_port_get(int unit, cbx_port_t port)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;

    CHECK_EXTENDER_INIT(ext_info);

    port = CBXI_GLOBAL_PORT(port, unit);

    if(CBX_PBMP_MEMBER(ext_info->shared_lag_ports, port)) {
        return CBX_E_NONE;
    }

    return CBX_E_PARAM;
}

/*
 * Function:
 *  cbxi_pe_shared_lag_port_set
 * Purpose:
 *  Set given port in Shared LAG ports bit map
 */
int
cbxi_pe_shared_lag_port_set(int unit,
                cbx_portid_t lag_out, cbx_port_t port, int enable)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    fpslict_t ent_fpslict;
    litet_t  ent_litet;
    int      slicid;
    int      mc_lin;
    uint32_t status = 0;

    CHECK_EXTENDER_INIT(ext_info);

    port = CBXI_GLOBAL_PORT(port, unit);

    slicid = (SLICID_PE_IN_ECID_U0_P0 + port);

    CBX_IF_ERROR_RETURN(soc_robo2_fpslict_get(unit, slicid, &ent_fpslict));

    if (enable) {
        CBX_PBMP_PORT_ADD(ext_info->shared_lag_ports, port);
        /* MCAST group is formed on Shared LAG but not on individual ports of
         * shared LAG
         * Disable MC DLI tx */
        CBX_IF_ERROR_RETURN(cbxi_pe_port_sli_get(port, &mc_lin));
        ent_litet.tx_ena = FALSE;
        /* Change default_sli to common (Shared LAG) */
        ent_fpslict.default_sli = cbx_extender_info.in_ecid_lag_sli[lag_out];
    } else {
        CBX_PBMP_PORT_REMOVE(ext_info->shared_lag_ports, port);
        /* Enable MC DLI tx */
        CBX_IF_ERROR_RETURN(cbxi_pe_port_sli_get(port, &mc_lin));
        ent_litet.tx_ena = TRUE;
        /* Change default_sli to mc_lin */
        ent_fpslict.default_sli = cbx_extender_info.in_ecid_port_sli[port];
    }


    CBX_IF_ERROR_RETURN(soc_robo2_litet_set(
                                        unit, mc_lin, &ent_litet, &status));
    /* Change default_sli of IN_ECID rules for MCAST ingress ECID filtering */
    CBX_IF_ERROR_RETURN(soc_robo2_fpslict_set(
                        unit, slicid, &ent_fpslict, &status));
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        CBX_IF_ERROR_RETURN(soc_robo2_fpslict_set(
                CBXI_CASC_PARTNER_UNIT(unit), slicid, &ent_fpslict, &status));
        CBX_IF_ERROR_RETURN(soc_robo2_litet_set(
                CBXI_CASC_PARTNER_UNIT(unit), mc_lin, &ent_litet, &status));
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_lag_dest_get
 * Purpose:
 *  Get destination LPG for a given LAGid
 */
int
cbxi_pe_lag_dest_get(int unit, cbx_port_t lag_out, cbxi_pgid_t *lpg)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    dli2ldpg_t     ent_dli2ldpg;
    litet_t        ent_litet;

    CHECK_EXTENDER_INIT(ext_info);

    CBX_IF_ERROR_RETURN(soc_robo2_litet_get(unit,
            cbx_extender_info.dwn_str_lag_dli[lag_out], &ent_litet));

    if (ent_litet.tx_ena == FALSE) {
        *lpg = CBXI_PGID_INVALID;
        return CBX_E_NONE;
    }

    CBX_IF_ERROR_RETURN(soc_robo2_dli2ldpg_get(
        unit, cbx_extender_info.dwn_str_lag_dli[lag_out], &ent_dli2ldpg));

    *lpg = ent_dli2ldpg.ldpgid;

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_lag_dest_set
 * Purpose:
 *  Set destination LPG for a given LAGid
 */
int
cbxi_pe_lag_dest_set(cbx_lag_info_t  *lag_info)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    litet_t        ent_litet;
    cbx_portid_t   lagid = 0;
    int            lin;
    uint32_t       status = 0;
    int            unit = 0;
    uint8_t        num_units = 1;
    uint16_t       lpg = 0;

    CHECK_EXTENDER_INIT(ext_info);

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    if ((lag_info->lpgid  == CBXI_PGID_INVALID) &&
                (lag_info->rlpgid  == CBXI_PGID_INVALID)) {
        ent_litet.tx_ena = FALSE;
        for (unit = 0; unit < num_units; unit++) {
            CBX_IF_ERROR_RETURN(soc_robo2_litet_set(unit,
                cbx_extender_info.dwn_str_lag_dli[lag_info->lagid],
                                                &ent_litet, &status));
        }
        return CBX_E_NONE;
    }

    /* LAGID */
    CBX_PORTID_TRUNK_SET(lagid, lag_info->lagid);
    CBX_IF_ERROR_RETURN(cbxi_pe_port_sli_get(lagid, &lin));

    for (unit = 0; unit < num_units; unit++) {
        if (unit == 0) {
            if (lag_info->lpgid  == CBXI_PGID_INVALID) {
                lpg = avg_csd_lpg[unit];
            } else {
                lpg = lag_info->lpgid;
            }
        } else {
            if (lag_info->rlpgid  == CBXI_PGID_INVALID) {
                lpg = avg_csd_lpg[unit];
            } else {
                lpg = lag_info->rlpgid;
            }
        }
        /* Configure UCAST DLI to given LPG */
        CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(unit,
                cbx_extender_info.dwn_str_lag_dli[lag_info->lagid], lpg));

        /* Configure MCAST DLI to given LPG */
        CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(unit, lin, lpg));
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_lin_port_resolve
 * Purpose:
 *  Get the actual destination port / LAG for a given LIN
 */
int
cbxi_pe_lin_port_resolve(int lin, cbx_portid_t *portid)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    int    idx;

    for (idx = 0; idx < CBX_PORT_MAX; idx++) {
        if (ext_info->dwn_str_dpp_dli[idx] == lin) {
            CBX_PORTID_LOCAL_SET(*portid, idx);
            return CBX_E_NONE;
        }
    }

    for (idx = 0; idx < CBX_LAG_MAX; idx++) {
        if (ext_info->dwn_str_lag_dli[idx] == lin) {
            CBX_PORTID_TRUNK_SET(*portid, idx);
            return CBX_E_NONE;
        }
    }

    return CBX_E_NOT_FOUND;
}

/*
 * Function:
 *  cbxi_pe_port_lin_get
 * Purpose:
 *  Get the actual LIN for a given portid
 */
int
cbxi_pe_port_lin_get(cbx_portid_t portid, int *lin)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    cbxi_pgid_t    port_lpg;
    cbx_port_t     port;
    int            unit = 0;

    if (CBX_PORTID_IS_TRUNK(portid)) {
        CBX_IF_ERROR_RETURN(cbxi_robo2_portid_validate(portid, &port));
        *lin = ext_info->dwn_str_lag_dli[port];
    } else {
        CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                                        portid, &port, &port_lpg, &unit));
        *lin = ext_info->dwn_str_dpp_dli[CBXI_GLOBAL_PORT(port, unit)];
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_vsi_init
 * Purpose:
 *  Initialize Downlink VSIs
 */
int
cbxi_pe_vsi_init(void)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    lin2vsi_t    ent_lin2vsi;
    vsit_t       ent_vsit;
    uint32_t     status = 0;
    int          unit = 0;
    int          csd_unit = 1;
    uint8        num_units = 1;
    int          id, vsit_idx = 0;

    CHECK_EXTENDER_INIT(ext_info);

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    /* Allocate separate unique VSIs using LIN slot get */
    if (!ext_info->downlink_vsi_ext_uc) {
        CBX_IF_ERROR_RETURN(cbxi_lin_slot_get(&id));
        ext_info->downlink_vsi_ext_uc = id;
    }
    if (!ext_info->downlink_vsi_csd_uc) {
        CBX_IF_ERROR_RETURN(cbxi_lin_slot_get(&id));
        ext_info->downlink_vsi_csd_uc = id;
    }

    if (!ext_info->downlink_vsi_ext_mc[unit]) {
        CBX_IF_ERROR_RETURN(cbxi_lin_slot_get(&id));
        ext_info->downlink_vsi_ext_mc[unit] = id;
    }
    if (!ext_info->downlink_vsi_csd_mc[unit]) {
        CBX_IF_ERROR_RETURN(cbxi_lin_slot_get(&id));
        ext_info->downlink_vsi_csd_mc[unit] = id;
    }


    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        if (!ext_info->downlink_vsi_ext_mc[csd_unit]) {
            CBX_IF_ERROR_RETURN(cbxi_lin_slot_get(&id));
            ext_info->downlink_vsi_ext_mc[csd_unit] = id;
        }
        if (!ext_info->downlink_vsi_csd_mc[csd_unit]) {
            CBX_IF_ERROR_RETURN(cbxi_lin_slot_get(&id));
            ext_info->downlink_vsi_csd_mc[csd_unit] = id;
        }
    }
    for (unit = 0; unit < num_units; unit++) {
        vsit_idx = ext_info->downlink_vsi_ext_uc;

        CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(unit, vsit_idx, &ent_vsit));
        ent_vsit.learn                = cbxVlanLearnModeMarkReady;
        ent_vsit.drop_ulf             = 1;
        ent_vsit.drop_mlf             = 1;
        ent_vsit.default_is_multicast = 0;
        ent_vsit.pg_map               = 0;
        ent_vsit.fid                  = ext_info->downlink_vsi_ext_uc;  /* Use same FID */

        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            ent_vsit.pg_map = 0;
            /* Configure pgmap = 0 on Given units for VSI of patner units */
            csd_unit = CBXI_CASC_PARTNER_UNIT(unit);

            vsit_idx = ext_info->downlink_vsi_ext_mc[csd_unit];
            CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(
                                        unit, vsit_idx, &ent_vsit, &status));
            vsit_idx = ext_info->downlink_vsi_csd_mc[csd_unit];
            CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(
                                        unit, vsit_idx, &ent_vsit, &status));
        }

        /* Add cascade ports for PE ext MCAST VSI */
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
             ent_vsit.pg_map = (1 << avg_csd_lpg[unit]);
        }
        vsit_idx = ext_info->downlink_vsi_ext_mc[unit];
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(
                                        unit, vsit_idx, &ent_vsit, &status));
        /* Add Cascade port + Loopback ports for PE csd and PE ext UCAST VSI */
        ent_vsit.pg_map  |= (1 << lb_lpg);

        vsit_idx = ext_info->downlink_vsi_csd_mc[unit];
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(
                                        unit, vsit_idx, &ent_vsit, &status));
        vsit_idx = ext_info->downlink_vsi_ext_uc;
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(
                                        unit, vsit_idx, &ent_vsit, &status));
        vsit_idx = ext_info->downlink_vsi_csd_uc;
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(
                                        unit, vsit_idx, &ent_vsit, &status));


        /* Configure LIN2VSI mapping for Ingress ECID filtering on PE ext ports */
        sal_memset(&ent_lin2vsi, 0, sizeof(lin2vsi_t));
        ent_lin2vsi.vsi = ext_info->downlink_vsi_ext_mc[unit]; /* Exclude LB port */

        for (id = 0; id < CBX_LAG_MAX; id++) {
            if (cbx_extender_info.in_ecid_lag_sli[id] != 0) {
                CBX_IF_ERROR_RETURN(soc_robo2_lin2vsi_set(unit,
                cbx_extender_info.in_ecid_lag_sli[id], &ent_lin2vsi, &status));
            }
        }
        for (id = 0; id < CBX_PORT_MAX; id++) {
            if (cbx_extender_info.in_ecid_port_sli[id] != 0) {
                CBX_IF_ERROR_RETURN(soc_robo2_lin2vsi_set(unit,
                cbx_extender_info.in_ecid_port_sli[id], &ent_lin2vsi, &status));
            }
        }
    }


    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_mcast_lb_check
 * Purpose:
 *  Check if LB port needs to be added/removed from a mcast group
 *  flags = CBXI_MCAST_MEMBER_ADD --> To add LB port to MCAST group
 *  flags = CBXI_MCAST_MEMBER_REMOVE --> To remove LB port
 *  flags = 0  --> No action required
 *
 */
int
cbxi_pe_mcast_lb_check(int unit, uint32_t mcast, uint32_t *flags)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    vsit_t         ent_vsit;
    dgt_t          ent_dgt;
    pbmp_t         vsit_pgmap = 0;
    pbmp_t         dgt_pgmap = 0;

    CHECK_EXTENDER_INIT(ext_info);

    *flags = 0;
    if (ext_info->mode != cbxiPEModeTransit) {
        return CBX_E_NONE;
    }


    CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(
                        unit, ext_info->downlink_vsi_ext_mc[unit], &ent_vsit));
    vsit_pgmap = ent_vsit.pg_map;

    CBX_IF_ERROR_RETURN(soc_robo2_dgt_get(unit, mcast, &ent_dgt));
    dgt_pgmap = ent_dgt.pgfov;


    CBX_PBMP_AND(vsit_pgmap, dgt_pgmap);

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        /* Remove Avenger Cascade port LPGs from pgmap*/
        CBX_PBMP_PORT_REMOVE(dgt_pgmap, avg_csd_lpg[unit]);
        CBX_PBMP_PORT_REMOVE(vsit_pgmap, avg_csd_lpg[unit]);
    }

    if (vsit_pgmap != 0) {
        /* PE Extender ports are present in MCAST entry */
        if (CBX_PBMP_MEMBER(dgt_pgmap, lb_lpg)) {
            /* LB port is already present */
            *flags = 0;
        } else {
            /* LB port is not present. Need to add */
            *flags = CBXI_MCAST_MEMBER_ADD;
        }
    } else {
        /* None of PE Extender ports are present in MCAST entry */
        if (CBX_PBMP_MEMBER(dgt_pgmap, lb_lpg)) {
            /* LB port is present. Remove */
            *flags = CBXI_MCAST_MEMBER_REMOVE;
        } else {
            /* LB port is not present */
            *flags = 0;
        }
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_port_lb_encap_set
 * Purpose:
 *  Configure CB tag on given physical port for loopback if enable = 1
 *  Else clear the encap settings
 *  return CBX_E_NONE
 */
int
cbxi_pe_port_lb_encap_set(cbx_portid_t portid, uint8_t enable)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    cbxi_encap_record_t  encap_record;
    cbx_port_t     port;
    cbxi_pgid_t    lpg;
    uint32         flags = 0;
    int            unit = 0;
    int            lin;

    /* Enable loopback from init for all UCAST to single PP*/

    if (CBX_PORTID_IS_TRUNK(portid)) {
        /* Not configured on LAG */
        return CBX_E_NONE;
    }
    CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(portid, &port, &lpg, &unit));

    lin = ext_info->dwn_str_dpp_dli[CBXI_GLOBAL_PORT(port, unit)];

    /* Set Loopback encapsulation at egress of downstream port */
    sal_memset(&encap_record, 0, sizeof(cbxi_encap_record_t));

    if (enable == 0) {
        /* Reset DLIET */
        CBX_EPP_INSERT_EOR(&encap_record);
    } else {
        /* Configure CB tag */
        encap_record.data[0] = CBX_EGR_DIRECTIVE_INSERT_8B_TAG; /* opcode */
        encap_record.data[1] = ((cbxiTagReserved2 << 5));
        encap_record.data[2] = 0x00;  /* FWD_OP = 0 */
        encap_record.data[3] = port;  /* DPP */
        encap_record.data[4] = 0x00;
        encap_record.data[5] = 0x00;
        encap_record.data[6] = 0x00;
        encap_record.data[7] = 0x00;
        encap_record.len     = 8;
        CBX_EPP_INSERT_EOR(&encap_record);
    }

    if (unit == 0) {
        flags = CBXI_ENCAP_AVG_PRI;
    } else {
        flags = CBXI_ENCAP_AVG_SEC;
    }
    CBX_IF_ERROR_RETURN(cbxi_lin_encap_set(port, lin, 0, &encap_record, flags));

    if (enable == 0) {
        /* Configure given port LPG as destination */
        CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(unit, lin, lpg));
    } else {
        /* Configure Loopback LPG as destination */
        CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(unit, lin, lb_lpg));
    }

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(CBXI_CASC_PARTNER_UNIT(unit), lin,
                                avg_csd_lpg[CBXI_CASC_PARTNER_UNIT(unit)]));
    }

    return CBX_E_NONE;
}


/*
 * Function:
 *  cbxi_internal_loopback_port_set
 * Purpose:
 *  Configure internal Port 14 loopback
 */
int
cbxi_internal_loopback_port_set(void)
{
/*    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;*/
    ipp_pp2lpg_t ent_pp2lpg;
    pgt_t ent_pgt;
    uint32_t status = 0;
    int lin;
    int unit = 0;
    uint8 num_units = 1;

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }
    CBX_IF_ERROR_RETURN(soc_robo2_ipp_pp2lpg_get(unit, lb_port, &ent_pp2lpg));
    lb_lpg = ent_pp2lpg.pgid;

    CBX_IF_ERROR_RETURN(soc_robo2_pgt_get(
                        CBX_AVENGER_PRIMARY, CBX_PORT_ICPU, &ent_pgt));

    for (unit = 0; unit < num_units; unit++) {
        /* Configure default_sli of port 14 while configuring uplink port*/
        ent_pgt.default_sli = 0x20000 | ((lb_lpg | (0x10 * unit)) << 12) ;
        ent_pgt.spgid       = (lb_lpg | (0x10 * unit));
        ent_pgt.soft_tag_en |= 0x1; /* Enable CB tag parsing */
        ent_pgt.src2    = 3; /* ETAG_PCP_DEI */
        ent_pgt.etag_en = 1;
        ent_pgt.slic_map_en = 1;
        CBX_IF_ERROR_RETURN(soc_robo2_pgt_set(
                                unit, lb_lpg, &ent_pgt, &status));

        /* Configure loopback on port 14 NPA_npa_loopback_control*/
        status = 0;
        REG_READ_NPA_NPA_LOOPBACK_CONTROLr(unit, &status);
        status |= (1 << lb_port);
        REG_WRITE_NPA_NPA_LOOPBACK_CONTROLr(unit, &status);

        CBX_IF_ERROR_RETURN(REG_READ_NPA_NPA_TLV_CONTROLr(unit, &status));
        status |= (1 << lb_port);
        CBX_IF_ERROR_RETURN(REG_WRITE_NPA_NPA_TLV_CONTROLr(unit, &status));

        REG_READ_CB_EPP_EGRESS_SOURCE_PORT_GROUP_TABLEr(unit, &status);
        status &= ~(0x3 << (lb_lpg * 2));/* Clear the md_size field for given port */
        status |= ((ent_pgt.metadata_len & 0x3) << (lb_lpg * 2));
        REG_WRITE_CB_EPP_EGRESS_SOURCE_PORT_GROUP_TABLEr(unit, &status);

        CBX_IF_ERROR_RETURN(cbxi_pe_port_sli_get(
                                        CBXI_GLOBAL_PORT(lb_port, unit), &lin));
        CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(unit, lin, lb_lpg));
        CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(unit,
            cbx_extender_info.dwn_str_dpp_dli[CBXI_GLOBAL_PORT(lb_port, unit)],
                                                                    lb_lpg));

        /* Configure Loopback port to handle 12G bandwidth */
        CBX_IF_ERROR_RETURN(cbxi_pe_lb_12G_bw(unit));
    }

    return CBX_E_NONE;
}

#ifdef CONFIG_PORT_EXTENDER
/*
 * Function:
 *  cbxi_pe_port_sli_init
 * Purpose:
 *  Allocate LIN to be used as extender SLI for each port
 *  return CBX_E_NONE
 */
STATIC int
cbxi_pe_port_sli_init(void)
{
    fpslict_t ent_fpslict;
    int idx, lin;
    uint32_t status = 0;

    /* If Avg cascade ports are not configured during INIT, then call to
     * LB encap set during port configuration will configure the LIN destination
     * correctly*/

    for (idx = SLICID_PE_IN_ECID_U0_P0;
        idx <= SLICID_PE_IN_ECID_LAG31; idx++) {
        lin = 0;
        CBX_IF_ERROR_RETURN(cbxi_lin_slot_get(&lin));
        CBX_IF_ERROR_RETURN(soc_robo2_fpslict_get(
                        CBX_AVENGER_PRIMARY, idx, &ent_fpslict));
        ent_fpslict.default_sli = lin;
        CBX_IF_ERROR_RETURN(soc_robo2_fpslict_set(
                        CBX_AVENGER_PRIMARY, idx, &ent_fpslict, &status));
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            CBX_IF_ERROR_RETURN(soc_robo2_fpslict_set(
                        CBX_AVENGER_SECONDARY, idx, &ent_fpslict, &status));
        }
        if (idx < SLICID_PE_IN_ECID_LAG0) {
            cbx_extender_info.in_ecid_port_sli[idx - SLICID_PE_IN_ECID_U0_P0] = lin;
        } else {
            cbx_extender_info.in_ecid_lag_sli[idx - SLICID_PE_IN_ECID_LAG0] = lin;
        }

    }

    /* PE DLIs for UCAST streams to encap CB tag for loopback */
    for (idx = 0; idx < (CBX_PORT_MAX - 2); idx++) {
        if ((!SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) &&
                                (idx >= CBX_MAX_PORT_PER_UNIT )) {
            break;
        } else if ((idx == CBX_PE_LB_PORT) || (idx == CBX_PORT_ICPU)){
            continue;
        }
        lin = 0;
        CBX_IF_ERROR_RETURN(cbxi_lin_slot_get(&lin));

        /* Loopback encapsulation is configured during LAG update */

        /* Add to Avenger Cascade port for 2 Avenger setup
         * Ports 0 - 15 on UNIT 1 and Ports 16 - 31 on UNIT 0 */
        CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(CBX_AVENGER_PRIMARY, lin, idx));
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            if (idx < CBX_MAX_PORT_PER_UNIT) {
                CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(CBX_AVENGER_SECONDARY,
                                lin, avg_csd_lpg[CBX_AVENGER_SECONDARY]));
            } else {
                CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(CBX_AVENGER_PRIMARY,
                                        lin, avg_csd_lpg[CBX_AVENGER_PRIMARY]));
                CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(
                                CBX_AVENGER_SECONDARY, lin, idx));
            }
        }

        cbx_extender_info.dwn_str_dpp_dli[idx] = lin;

        /* As per requirement, all UCAST traffic to single physical port is
         * looped back */
        cbxi_pe_port_lb_encap_set(idx, TRUE);

    }

    /* PE DLIs for UCAST streams on LAG.
     * These DLIs are used to dynamically change DLI2LDPG mappings */
    for (idx = 0; idx < CBX_LAG_MAX; idx++) {
        lin = 0;
        CBX_IF_ERROR_RETURN(cbxi_lin_slot_get(&lin));

        cbx_extender_info.dwn_str_lag_dli[idx] = lin;
    }


    return CBX_E_NONE;
}
#endif /* CONFIG_PORT_EXTENDER */

/*
 * Function:
 *  cbxi_pe_lag_dli_get
 * Purpose:
 *  Get LIN type DLI to use for a LAG (Shared LAG & Traditional LAG)
 *  return CBX_E_NONE
 */
int
cbxi_pe_lag_dli_get(cbx_portid_t portid, int *lin)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    int            rv;
    cbx_port_t     lag_out;

    CHECK_EXTENDER_INIT(ext_info);

    rv = cbxi_robo2_portid_validate(portid, &lag_out);
    if (CBX_FAILURE(rv)) {
        return CBX_E_PORT;
    }

    *lin = cbx_extender_info.dwn_str_lag_dli[lag_out];

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_port_sli_get
 * Purpose:
 *  Get LIN type SLI on uplink port for a given extender LPG
 *  return CBX_E_NONE
 */
int
cbxi_pe_port_sli_get(cbx_portid_t ex_port, int *s_lin)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    cbxi_pgid_t lpg;
    cbx_port_t port;
    int unit = 0;
    int idx = 0;

    CHECK_EXTENDER_INIT(ext_info);

    if (CBX_PORTID_IS_TRUNK(ex_port)) {
        idx = CBX_PORTID_TRUNK_GET(ex_port);
        *s_lin = cbx_extender_info.in_ecid_lag_sli[idx];
    } else {
        CBX_IF_ERROR_RETURN(
                        cbxi_robo2_port_validate(ex_port, &port, &lpg, &unit));
        idx = CBXI_GLOBAL_PORT(port, unit);
        *s_lin = cbx_extender_info.in_ecid_port_sli[idx];
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_port_sli_ecid_set
 * Purpose:
 *  Set Ingress E-CID in SLIC rule and enable it
 *  return CBX_E_NONE
 */
int
cbxi_pe_port_sli_ecid_set(cbx_portid_t ex_port, uint32_t e_cid)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    cbxi_slic_rule_t slic_rule;
    cbxi_slic_rule_id_t rule_idx, uc_idx;
    cbxi_pgid_t lpg;
    cbx_port_t port;
    int unit = 0;
    int s_lin = 0;

    CHECK_EXTENDER_INIT(ext_info);

    /* Configure SLI */
    CBX_IF_ERROR_RETURN(cbxi_pe_port_sli_get(ex_port, &s_lin));

    CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(ex_port, &port, &lpg, &unit));

    /* Configure E_CID in SLIC rule */
    if (CBX_PORTID_IS_TRUNK(ex_port)) {
        uc_idx   = (SLIC_PE_1BR_UC_ETAG_LAG0 + CBX_PORTID_TRUNK_GET(ex_port));
        rule_idx = (SLIC_PE_1BR_IN_ECID_LAG0 + CBX_PORTID_TRUNK_GET(ex_port));
    } else {
        uc_idx   = (SLIC_PE_1BR_UC_ETAG_U0_P0 + CBXI_GLOBAL_PORT(port, unit));
        rule_idx = (SLIC_PE_1BR_IN_ECID_U0_P0 + CBXI_GLOBAL_PORT(port, unit));
    }

    /* Enable & Configure UCAST rule */
    CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(unit,uc_idx, TRUE));
    CBX_IF_ERROR_RETURN(cbxi_slicmap_rule_get(unit, uc_idx, &slic_rule));
    slic_rule.tci3      = (e_cid & 0x3FFF);
    slic_rule.tci3_mask = 0x3FFF;
    slic_rule.mask_flags |= ROBO2_SLIC_MASK_TCI;
    CBX_IF_ERROR_RETURN(cbxi_slicmap_rule_set(unit, uc_idx, slic_rule));

    /* Enable & Configure Ingress ECID filtering rule */
    CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(unit,rule_idx, TRUE));
    CBX_IF_ERROR_RETURN(cbxi_slicmap_rule_get(unit, rule_idx, &slic_rule));
    slic_rule.tci2      = (e_cid & 0x0FFF);
    slic_rule.tci2_mask = 0x0FFF;
    slic_rule.mask_flags |= ROBO2_SLIC_MASK_TCI;
    CBX_IF_ERROR_RETURN(cbxi_slicmap_rule_set(unit, rule_idx, slic_rule));

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        /* Configure same LI on partner Avenger with the
         * Avg cascade port as destination */
        unit = CBXI_CASC_PARTNER_UNIT(unit);

        /* Enable & Configure UCAST rule */
        CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(unit,uc_idx, TRUE));
        CBX_IF_ERROR_RETURN(cbxi_slicmap_rule_get(unit, uc_idx, &slic_rule));
        slic_rule.tci3      = (e_cid & 0x3FFF);
        slic_rule.tci3_mask = 0x3FFF;
        slic_rule.mask_flags |= ROBO2_SLIC_MASK_TCI;
        CBX_IF_ERROR_RETURN(cbxi_slicmap_rule_set(unit, uc_idx, slic_rule));


        /* Preserve the pgmap */
        CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(unit,rule_idx, TRUE));
        CBX_IF_ERROR_RETURN(cbxi_slicmap_rule_get(unit, rule_idx, &slic_rule));
        slic_rule.tci2      = (e_cid & 0x0FFF);
        slic_rule.tci2_mask = 0x0FFF;
        slic_rule.mask_flags |= ROBO2_SLIC_MASK_TCI;
        CBX_IF_ERROR_RETURN(cbxi_slicmap_rule_set(unit, rule_idx, slic_rule));

        CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(unit, s_lin, avg_csd_lpg[unit]));
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_port_sli_ecid_clear
 * Purpose:
 *  Clear Ingress E-CID in SLIC rule and disable it
 *  return CBX_E_NONE
 */
int
cbxi_pe_port_sli_ecid_clear(cbx_portid_t ex_port)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    cbxi_slic_rule_t slic_rule;
    cbxi_slic_rule_id_t rule_idx, uc_idx;
    cbxi_pgid_t lpg;
    cbx_port_t port;
    int unit = 0;
    uint8_t num_units = 1;

    CHECK_EXTENDER_INIT(ext_info);

    CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(ex_port, &port, &lpg, &unit));

    /* Configure E_CID in SLIC rule */
    if (CBX_PORTID_IS_TRUNK(ex_port)) {
        uc_idx   = (SLIC_PE_1BR_UC_ETAG_LAG0 + CBX_PORTID_TRUNK_GET(ex_port));
        rule_idx = (SLIC_PE_1BR_IN_ECID_LAG0 + CBX_PORTID_TRUNK_GET(ex_port));
    } else {
        uc_idx   = (SLIC_PE_1BR_UC_ETAG_U0_P0 + CBXI_GLOBAL_PORT(port, unit));
        rule_idx = (SLIC_PE_1BR_IN_ECID_U0_P0 + CBXI_GLOBAL_PORT(port, unit));
    }

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    for (unit = 0; unit < num_units; unit++) {
        /* Configure E_CID in SLIC rule to zero */
        CBX_IF_ERROR_RETURN(cbxi_slicmap_rule_get(unit, uc_idx, &slic_rule));
        slic_rule.tci3      = 0x0000;
        slic_rule.tci3_mask = 0x3FFF;
        slic_rule.mask_flags |= ROBO2_SLIC_MASK_TCI;
        CBX_IF_ERROR_RETURN(cbxi_slicmap_rule_set(unit, uc_idx, slic_rule));
        CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(unit,uc_idx, FALSE));

        CBX_IF_ERROR_RETURN(cbxi_slicmap_rule_get(unit, rule_idx, &slic_rule));
        slic_rule.tci2      = 0x0000;
        slic_rule.tci2_mask = 0x0FFF;
        slic_rule.mask_flags |= ROBO2_SLIC_MASK_TCI;
        CBX_IF_ERROR_RETURN(cbxi_slicmap_rule_set(unit, rule_idx, slic_rule));
        CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(unit,rule_idx, FALSE));
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_uplink_failover_set
 * Purpose:
 *  Set failover port for uplink
 *  return CBX_E_NONE failover changed PPFOV
 *  return CBX_E_EXISTS if PPFOV remains unchanged
 */
int
cbxi_pe_uplink_failover_set(cbx_portid_t up_portid,int link_sts)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    cbx_port_params_t port_params;
    lpg2ppfov_t  ent_lpg2ppfov;
    cbx_port_t   up_port;
    cbx_port_t   csd_port = 13;
    cbxi_pgid_t  up_port_lpg;
    cbx_portid_t portid_fin;
#ifdef CONFIG_CASCADED_MODE
    cbxi_pgid_t  csd_port_lpg = 13;
    cbx_portid_t csd_portid;
#endif
    int          unit_csd = 1;
    pbmp_t       ppfov = 0;
    uint32_t     status = 0;
    int          unit = 0;

    CHECK_EXTENDER_INIT(ext_info);

    if (CBX_FAILURE(cbxi_robo2_lag_member_check(up_portid, &portid_fin))) {
        /* If the port is not part of LAG, use the port value */
        portid_fin = up_portid;
    }

    /* Check if the port is actually uplink port or not */
    CBX_IF_ERROR_RETURN(cbx_port_info_get(portid_fin, &port_params));
    if (port_params.port_type != cbxPortTypeExtenderUpstream) {
        return CBX_E_PARAM;
    }

    CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                            up_portid, &up_port, &up_port_lpg, &unit));

    /* Set failover port only when valid uplink port is present in partner Avg*/
    unit_csd = CBXI_CASC_PARTNER_UNIT(unit);
    if (ext_info->uplink_port[unit_csd] == CBX_PORT_INVALID) {
        return CBX_E_UNAVAIL;
    }

#ifndef CONFIG_EXTERNAL_HOST
    /* In case of PE in Robo-OS, link change on uplink port is handled by
     * intrrupt*/
    /* Disable traffic on uplink during table update */
    CBX_IF_ERROR_RETURN(cbxi_stg_stp_set(CBX_STG_DEFAULT,
                                        portid_fin, cbxStgStpStateDiscard));
#endif


#ifdef CONFIG_CASCADED_MODE
    CBX_IF_ERROR_RETURN(cbxi_cascade_port_get(unit, &csd_portid));
    CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                    csd_portid, &csd_port, &csd_port_lpg, &unit_csd));
#endif

    CBX_IF_ERROR_RETURN(soc_robo2_lpg2ppfov_get(
                        unit, up_port_lpg, &ent_lpg2ppfov));
    ppfov = ent_lpg2ppfov.ppfov;

    if (link_sts) {
        /* Uplink port is UP. Add it to PPFOV and remove csd port */
        if (CBX_PBMP_MEMBER(ppfov, up_port)) {
            /* Already exixts, no need to change */
            return CBX_E_EXISTS;
        }
        CBX_PBMP_PORT_ADD(ppfov, up_port);
        CBX_PBMP_PORT_REMOVE(ppfov, csd_port);
    } else {
        /* Uplink port is DOWN. Remove it from PPFOV and
         * add csd port if no other Physical port for uplink is present*/
        if (!CBX_PBMP_MEMBER(ppfov, up_port)) {
            /* Already removed from ppfov, no need to chenge */
            return CBX_E_EXISTS;
        }
        CBX_PBMP_PORT_REMOVE(ppfov, up_port);
        if (CBX_PBMP_IS_NULL(ppfov)) {
            CBX_PBMP_PORT_ADD(ppfov, csd_port);
        }
    }
    ent_lpg2ppfov.ppfov = ppfov;
    CBX_IF_ERROR_RETURN(soc_robo2_lpg2ppfov_set(
                 unit, up_port_lpg, &ent_lpg2ppfov, &status));
#ifndef CONFIG_EXTERNAL_HOST
    /* In case of PE in Robo-OS, link change on uplink port is handled by
     * intrrupt*/
    CBX_IF_ERROR_RETURN(cbxi_stg_stp_set(CBX_STG_DEFAULT,
                                        portid_fin, cbxStgStpStateForward));
#endif

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_mcast_ecid_get
 * Purpose:
 *  Obtain the E_CID assigned for a multicast group
 *
 *  It is assumed that mcastid is already checked.
 *
 *  Note: E_CID value is stored in hardware instead of software.
 *  Since there is no separate hardware table to store E_CID, it will to store
 *  in the unused entry of LILT for that particular MCAST id. In our case we are
 *  not using LILT entry for ports 14, 15 and hence last entry can be used for
 *  storing E_CID value.
 *  Each MCAST is assigned 8 LILT entries and E_CID can be stored at 8th entry.
 */
int
cbxi_pe_mcast_ecid_get(cbx_mcastid_t mcastid, uint32_t *e_cid)
{
    dgt_t ent_dgt;
    lilt_t ent_lilt;
    int mcast = 0;
    int unit = 0;

    if ((mcast = CBX_PORTID_MCAST_ID_GET(mcastid)) < 0) {
        return CBX_E_PARAM;
    }

    *e_cid = 0;

    /* Retrive DGT table entry */
    CBX_IF_ERROR_RETURN(soc_robo2_dgt_get(unit, mcast, &ent_dgt));
    /* Retrive the E_CID value stored at lilt entry at index
     * (ent_dgt.lilt_ndx + 7) */
    CBX_IF_ERROR_RETURN(soc_robo2_lilt_get(
                        unit, (ent_dgt.lilt_ndx + 7), &ent_lilt));
    *e_cid = ent_lilt.lilt_entry;

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_mcast_ecid_set
 * Purpose:
 *  Store the E_CID assigned for a multicast group
 *
 *  It is assumed that mcastid is already checked.
 *
 *  Note: E_CID value is stored in hardware instead of software.
 *  Since there is no separate hardware table to store E_CID, it will to store
 *  in the unused entry of LILT for that particular MCAST id. In our case we are
 *  not using LILT entry for ports 14, 15 and hence last entry can be used for
 *  storing E_CID value.
 *  Each MCAST is assigned 8 LILT entries and E_CID can be stored at 8th entry.
 */
int
cbxi_pe_mcast_ecid_set(cbx_mcastid_t mcastid, uint32_t e_cid)
{
    dgt_t ent_dgt;
    lilt_t ent_lilt;
    int mcast = 0;
    int unit = 0;
    uint32_t status = 0;

    if ((mcast = CBX_PORTID_MCAST_ID_GET(mcastid)) < 0) {
        return CBX_E_PARAM;
    }

    /* Retrive DGT table entry */
    CBX_IF_ERROR_RETURN(soc_robo2_dgt_get(unit, mcast, &ent_dgt));
    /* Store the E_CID value lilt entry index (ent_dgt.lilt_ndx + 7) */
    ent_lilt.lilt_entry = e_cid;
    CBX_IF_ERROR_RETURN(soc_robo2_lilt_set(
                unit, (ent_dgt.lilt_ndx + 7), &ent_lilt, &status));

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_release_lin_slot
 * Purpose:
 *  Release LIN which is not being by PE
 */
int
cbxi_pe_release_lin_slot(cbx_l2_entry_t *l2entry)
{
    int rv = 0;

    rv = cbx_l2_lookup(l2entry);
    if (rv == CBX_E_NOT_FOUND) {
        return CBX_E_NONE;
    } else if (rv < 0) {
        return rv;
    }
    l2entry->params.flags = CBX_L2_ADDR_ECID;

    if (l2entry->params.dest_type == cbxL2AddrDestDPG) {
        CBX_IF_ERROR_RETURN(cbxi_pe_extender_port_clear(
                                        l2entry->params.portid));
        return CBX_E_NONE;
    } else if (l2entry->params.dest_type == cbxL2AddrDestDLI) {
        /* Cascade ports */
        if ((rv = CBX_PORTID_VLAN_PORT_ID_GET(l2entry->params.portid)) < 0) {
            return CBX_E_PARAM;
        }
        cbxi_lin_slot_release(rv);
    } else if (l2entry->params.dest_type == cbxL2AddrDestMcast) {
#if 0 /* PE loopback solution: No MCAST specific LIN to release */
        /* Clear E_CID for this MCAST group */
        CBX_IF_ERROR_RETURN(cbxi_pe_mcast_ecid_set(l2entry->params.portid, 0));
#endif
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_uplink_port_set
 * Purpose:
 *  Configure given port as uplink port
 */
int
cbxi_pe_uplink_port_set(cbx_portid_t up_port)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    fpslict_t    ent_fpslict;
    vsit_t       ent_vsit;
    pgt_t        ent_pgt;
#if defined(CORTEX_M7) || defined(CONFIG_CASCADED_MODE)
    lpg2ppfov_t  ent_lpg2ppfov;
#endif
    cbxi_pgid_t  up_port_lpg;
    cbx_port_t   port;
    cbxi_pgid_t  csd_port_lpg;
#ifdef CONFIG_CASCADED_MODE
    cbx_portid_t csd_portid;
    int          unit_csd = 1;
#endif
    pbmp_t       pg_map = 0;
    uint32_t     status = 0;
    int          unit = 0;
    int          dlinid, slic_idx, vsit_idx = 0;

    CHECK_EXTENDER_INIT(ext_info);

    /* ********** Downstream settings ********** */
    CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                            up_port, &port, &up_port_lpg, &unit));

    if ((!SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) && (unit == 1)) {
        return CBX_E_UNIT;
    }

    /* Add to Uplink port SLIC rule */
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_get(unit, SLIC_PE_1BR_FWD_DWNSTR_UC, &pg_map));
    CBX_PBMP_PORT_ADD(pg_map, up_port_lpg);
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit, SLIC_PE_1BR_FWD_DWNSTR_UC, pg_map));
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_get(unit, SLIC_PE_1BR_FWD_DWNSTR_MC, &pg_map));
    CBX_PBMP_PORT_ADD(pg_map, up_port_lpg);
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit, SLIC_PE_1BR_FWD_DWNSTR_MC, pg_map));

    for (slic_idx = SLIC_PE_1BR_UC_ETAG_U0_P0;
                slic_idx <= SLIC_PE_1BR_UC_ETAG_LAG31; slic_idx++) {
        CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(unit, slic_idx, TRUE));
        /* Enable Uplink port for all UCAST ETAG rules */
        CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit, slic_idx, pg_map));
        CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(unit, slic_idx, FALSE));
    }

    for (slic_idx = SLIC_PE_1BR_IN_ECID_U0_P0;
                slic_idx <= SLIC_PE_1BR_IN_ECID_LAG31; slic_idx++) {
        /* Enable Uplink port for all MCAST Ingress ECID rules */
        
        CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(unit, slic_idx, TRUE));
        if (ext_info->mode == cbxiPEModeTransit) {
            CBX_PBMP_PORT_ADD(pg_map, lb_lpg);
            CBX_PBMP_PORT_REMOVE(pg_map, up_port_lpg);
            if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
                /* Configure LB port on Parnter Avenger also */
                CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(
                               CBXI_CASC_PARTNER_UNIT(unit), slic_idx, pg_map));
                CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(
                               CBXI_CASC_PARTNER_UNIT(unit), slic_idx, FALSE));
            }
        } else if (ext_info->mode == cbxiPEModeAccess) {
            CBX_PBMP_PORT_ADD(pg_map, up_port_lpg);
            CBX_PBMP_PORT_REMOVE(pg_map, lb_lpg);
            
        }
        CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit, slic_idx, pg_map));
        CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(unit, slic_idx, FALSE));
    }
   /* Enable Uplink port for Decap rules for remaining MCAST ECID */
   
    CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(unit, SLIC_PE_1BR_DECAP, TRUE));
    if (ext_info->mode == cbxiPEModeTransit) {
        CBX_PBMP_PORT_ADD(pg_map, lb_lpg);
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            /* Configure LB port on Parnter Avenger also */
            CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(
                   CBXI_CASC_PARTNER_UNIT(unit), SLIC_PE_1BR_DECAP, pg_map));
        }
    } else if (ext_info->mode == cbxiPEModeAccess) {
        CBX_PBMP_PORT_ADD(pg_map, up_port_lpg);
    }
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit, SLIC_PE_1BR_DECAP, pg_map));

    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_get(unit, SLIC_PE_UNTAG_DROP, &pg_map));
    CBX_PBMP_PORT_ADD(pg_map, up_port_lpg);
    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit, SLIC_PE_UNTAG_DROP, pg_map));

    /* Get a unique VSIT idx using lin slot get to use for ARL search */
    /* Use same VSI for downlink on both avengers in case of Avenger Cascade */
    CBX_IF_ERROR_RETURN(cbxi_pe_vsi_init());

    /* Update default SLI in fpslict for Decap rule */
    

    CBX_IF_ERROR_RETURN(soc_robo2_fpslict_get(
                        unit, SLICID_PE_ETAG_DECAP, &ent_fpslict));
    if (ext_info->mode == cbxiPEModeTransit) {
        ent_fpslict.default_sli = (0x20000 | (lb_lpg << 12) |
                                ext_info->downlink_vsi_ext_mc[unit]);
        CBX_IF_ERROR_RETURN(soc_robo2_fpslict_set(
                        unit, SLICID_PE_ETAG_DECAP, &ent_fpslict, &status));

        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            /* Configure LB port on Parnter Avenger also */
            ent_fpslict.default_sli = (0x20000 | (lb_lpg << 12) |
                ext_info->downlink_vsi_ext_mc[CBXI_CASC_PARTNER_UNIT(unit)]);
            CBX_IF_ERROR_RETURN(soc_robo2_fpslict_set(
                CBXI_CASC_PARTNER_UNIT(unit), SLICID_PE_ETAG_DECAP,
                                                &ent_fpslict, &status));
        }
    } else if (ext_info->mode == cbxiPEModeAccess) {
        ent_fpslict.default_sli = (0x20000 | (up_port_lpg << 12) |
                                        ext_info->downlink_vsi_ext_mc[unit]);
        CBX_IF_ERROR_RETURN(soc_robo2_fpslict_set(
                        unit, SLICID_PE_ETAG_DECAP, &ent_fpslict, &status));
    }

    /* UCAST slic rules requires LB port in VSI for Loopback operation */
    CBX_IF_ERROR_RETURN(soc_robo2_fpslict_get(
                        unit, SLICID_PE_ETAG_DECAP_LB, &ent_fpslict));
    ent_fpslict.default_sli =
                (0x20000 | (up_port_lpg << 12) | ext_info->downlink_vsi_ext_uc);
    CBX_IF_ERROR_RETURN(soc_robo2_fpslict_set(
                        unit, SLICID_PE_ETAG_DECAP_LB, &ent_fpslict, &status));
    /* Configure LB port on Parnter Avenger also */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        CBX_IF_ERROR_RETURN(soc_robo2_fpslict_set(CBXI_CASC_PARTNER_UNIT(unit),
                            SLICID_PE_ETAG_DECAP_LB, &ent_fpslict, &status));
    }

    CBX_IF_ERROR_RETURN(soc_robo2_fpslict_get(
                        unit, SLICID_PE_FWD_DWNSTR_UC, &ent_fpslict));
    ent_fpslict.default_sli = (0x20000 | (up_port_lpg << 12) |
                                ext_info->downlink_vsi_csd_uc);
    CBX_IF_ERROR_RETURN(soc_robo2_fpslict_set(
                        unit, SLICID_PE_FWD_DWNSTR_UC, &ent_fpslict, &status));


    CBX_IF_ERROR_RETURN(soc_robo2_fpslict_get(
                        unit, SLICID_PE_FWD_DWNSTR_MC, &ent_fpslict));
    ent_fpslict.default_sli = (0x20000 | (up_port_lpg << 12) |
                                ext_info->downlink_vsi_csd_mc[unit]);
    CBX_IF_ERROR_RETURN(soc_robo2_fpslict_set(
                        unit, SLICID_PE_FWD_DWNSTR_MC, &ent_fpslict, &status));


    /* Update PGT entry */
    CBX_IF_ERROR_RETURN(soc_robo2_pgt_get(unit, up_port_lpg, &ent_pgt));
    ent_pgt.src2    = 3; /* ETAG_PCP_DEI */
    ent_pgt.etag_en = 1;
    ent_pgt.default_sli           = (ent_pgt.default_sli & 0x3F000);
    ent_pgt.default_sli           = (ent_pgt.default_sli |
                                        ext_info->downlink_vsi_csd_mc[unit]);
    CBX_IF_ERROR_RETURN(soc_robo2_pgt_set(
                                unit, up_port_lpg, &ent_pgt, &status));

    /* Update PGT entry of Loopback port P14*/
    CBX_IF_ERROR_RETURN(soc_robo2_pgt_get(unit, lb_lpg, &ent_pgt));
    ent_pgt.default_sli           = (ent_pgt.default_sli & 0x3F000);
    ent_pgt.default_sli           = (ent_pgt.default_sli |
                                         ext_info->downlink_vsi_ext_mc[unit]);
    CBX_IF_ERROR_RETURN(soc_robo2_pgt_set(
                                unit, lb_lpg, &ent_pgt, &status));

    REG_READ_CB_EPP_EGRESS_SOURCE_PORT_GROUP_TABLEr(unit, &status);
    status &= ~(0x3 << (lb_lpg * 2));/* Clear the md_size field for given port */
    status |= ((ent_pgt.metadata_len & 0x3) << (lb_lpg * 2));
    REG_WRITE_CB_EPP_EGRESS_SOURCE_PORT_GROUP_TABLEr(unit, &status);

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        ent_pgt.default_sli = (ent_pgt.default_sli & 0x3F000);
        ent_pgt.default_sli = (ent_pgt.default_sli |
                ext_info->downlink_vsi_ext_mc[CBXI_CASC_PARTNER_UNIT(unit)]);
        CBX_IF_ERROR_RETURN(soc_robo2_pgt_set(
                CBXI_CASC_PARTNER_UNIT(unit), lb_lpg, &ent_pgt, &status));
        REG_READ_CB_EPP_EGRESS_SOURCE_PORT_GROUP_TABLEr(
                                        CBXI_CASC_PARTNER_UNIT(unit), &status);
        status &= ~(0x3 << (lb_lpg * 2));
        status |= ((ent_pgt.metadata_len & 0x3) << (lb_lpg * 2));
        REG_WRITE_CB_EPP_EGRESS_SOURCE_PORT_GROUP_TABLEr(
                                        CBXI_CASC_PARTNER_UNIT(unit), &status);
    }

#ifdef CORTEX_M7
    if (unit) {
        /* Enable external intr on secondary for NPA_IRQn = 24 */
        /* INTR_L pin of secondary is connected to GPIO 7 of primary */
        extern_intr_enable(unit, 24);
        gpio_intr_enable(CBX_AVENGER_PRIMARY, 7); /* Enable GPIO 7 on primary*/
    }
    CBX_IF_ERROR_RETURN(soc_robo2_lpg2ppfov_get(
                        unit, up_port_lpg, &ent_lpg2ppfov));
    CBX_PBMP_ITER(ent_lpg2ppfov.ppfov, port) {
        npa_intr_enable(unit, port);
    }
#endif /* CORTEX_M7 */

    /* ********** Upstream settings ********** */
    /* Get another unique VSIT idx to forward all traffic from Cascade ports to
     * uplink port. This will be only used by Cascade ports */
    /* Use same VSI for Uplink on both avengers in case of Avenger Cascade */
    if (!ext_info->uplink_vsi) {
        CBX_IF_ERROR_RETURN(cbxi_lin_slot_get(&dlinid));
        vsit_idx = dlinid;
    } else if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        vsit_idx = ext_info->uplink_vsi;
        dlinid   = vsit_idx;
    }

    CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(unit, vsit_idx, &ent_vsit));
    ent_vsit.learn                = cbxVlanLearnModeDisable;
    ent_vsit.default_is_multicast = 0;
    ent_vsit.dlliid_dgid          = dlinid;
    ent_vsit.pg_map               = PBMP_GE_ALL(unit) | PBMP_XL_ALL(unit);
    ent_vsit.fid                  = vsit_idx;
    CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(unit, vsit_idx, &ent_vsit, &status));
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY) && !ext_info->uplink_vsi) {
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(
                CBXI_CASC_PARTNER_UNIT(unit),vsit_idx, &ent_vsit, &status));
    }

    CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(unit, dlinid, up_port_lpg));

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY) && !ext_info->uplink_vsi) {
        csd_port_lpg = 13;
#ifdef CONFIG_CASCADED_MODE
        /* Set Uplink dest as Avg cascade port on partner Avg.
         * This covers the scenario in which only one uplink port is present in
         * either Primary or Secondary Avenger */
        CBX_IF_ERROR_RETURN(cbxi_cascade_port_get(
                                CBXI_CASC_PARTNER_UNIT(unit), &csd_portid));
        CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                    csd_portid, &port, &csd_port_lpg, &unit_csd));
#endif
        CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(
                CBXI_CASC_PARTNER_UNIT(unit), dlinid, csd_port_lpg));
    }

    /* Configure EPMT for default values. This idx is used in encap directive */
    CBX_IF_ERROR_RETURN(cbxi_pe_port_epmt_set(unit, up_port_lpg));
#ifdef CONFIG_CASCADED_MODE
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        CBX_IF_ERROR_RETURN(cbxi_pe_port_epmt_set(CBX_AVENGER_PRIMARY,
                                        avg_csd_lpg[CBX_AVENGER_PRIMARY]));
        CBX_IF_ERROR_RETURN(cbxi_pe_port_epmt_set(CBX_AVENGER_SECONDARY,
                                        avg_csd_lpg[CBX_AVENGER_SECONDARY]));
    }
#endif
    ext_info->uplink_port[unit] = up_port;
    ext_info->uplink_vsi  = vsit_idx;


#ifdef CONFIG_CASCADED_MODE
    /* Check if any of the uplink is already down, set failover accordingly */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY) &&
                (ext_info->uplink_port[0] != CBX_PORT_INVALID) &&
                        (ext_info->uplink_port[1] != CBX_PORT_INVALID)) {
        for (unit = 0; unit < 2; unit++) {
            CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                ext_info->uplink_port[unit], &port, &up_port_lpg, &unit_csd));
            CBX_IF_ERROR_RETURN(soc_robo2_lpg2ppfov_get(
                        unit, up_port_lpg, &ent_lpg2ppfov));
            CBX_PBMP_ITER(ent_lpg2ppfov.ppfov, port) {
                CBX_IF_ERROR_RETURN(cbx_port_attribute_get(
                        CBXI_GLOBAL_PORT(port, unit),
                                        cbxPortAttrLinkStatus, &status));
                if (!status) {
                /* If link down, set failover */
                    cbxi_pe_uplink_failover_set(
                             CBXI_GLOBAL_PORT(port, unit), status);
                }
            }
        }
    }
#endif

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_extender_port_set
 * Purpose:
 *  Configure given port as extender port
 *  Note: e_cid refers to 14 bit E-CID: 2 bit GRP + 12 bit E_CID base
 */
int
cbxi_pe_extender_port_set(
                cbx_portid_t ex_port, uint32_t e_cid, cbx_l2_entry_t *l2entry)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    cbxi_encap_record_t  encap_record;
    pgt_t          ent_pgt;
    cbxi_pgid_t    up_port_lpg, ex_port_lpg;
    cbx_port_t     port;
#ifdef CONFIG_CASCADED_MODE
    cbx_portid_t   csd_portid;
#endif
    int            unit = 0, i;
    int            dlinid;

    CHECK_EXTENDER_INIT(ext_info);

    /* ********** Downstream settings ********** */
    /* Add L2 ARL entry for the E-CID, port combination packets coming from
     * uplink port to PE ports*/
    l2entry->params.flags        = CBX_L2_ADDR_ECID;
    l2entry->vlanid              = ext_info->downlink_vsi_ext_uc;
    l2entry->params.e_cid        = e_cid;

    /* Destination is set as type LIN in L2 module while composing the hardware
     * L2 ARL structure */
    if (CHECK_ECID_IS_MCAST(e_cid)) {
         /* Nothing has to be done for individual extender ports in downstream
          * Upstream is always through unicast E-CIDs */
        return CBX_E_NONE;
    } else {
        CBX_IF_ERROR_RETURN(cbxi_pe_port_sli_ecid_set(ex_port, e_cid));
    }

    if (CBX_PORTID_IS_TRUNK(ex_port)) {
        if (cbxi_pe_shared_lag_get(ex_port) == CBX_E_NONE) {
            /* No upstream setting for Shared LAG */
            return CBX_E_NONE;
        }
    }

    /* ********** Upstream settings ********** */
    CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                                        ex_port, &port, &ex_port_lpg, &unit));

    CBX_IF_ERROR_RETURN(soc_robo2_pgt_get(unit, ex_port_lpg, &ent_pgt));
    /* Ext port init has already configured the LIN */
    dlinid = PV_SLI_2_VSI(ent_pgt.default_sli);


    /* Create DLI to add E-TAG at uplink port */
    /* Map port to lpgid and set it as destination */
    if (ext_info->uplink_port[unit] != CBX_PORT_INVALID) {
        CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                    ext_info->uplink_port[unit], &port, &up_port_lpg, &i));
    } else {
#ifdef CONFIG_CASCADED_MODE
        CBX_IF_ERROR_RETURN(cbxi_cascade_port_get(unit, &csd_portid));
        CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                    csd_portid, &port, &up_port_lpg, &i));
#endif
    }
    CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(unit, dlinid, up_port_lpg));

    /* Set E-TAG encapsulation at egress of uplink port */

    encap_record.data[0] = CBX_EGR_DIRECTIVE_INSERT_8B_TAG; /* opcode */
    encap_record.data[1] = ((cbxiTagReserved1 << 5) | (up_port_lpg + 1)); /* TPID from TET | primap idx */
    encap_record.data[2] = (0x0 << 4) ; /* Hardcoded PCP, DEI */
    encap_record.data[3] = 0x00;
    encap_record.data[4] = ((e_cid >> 8) & 0x3F); /* GRP | E-CID */
    encap_record.data[5] = (e_cid & 0xFF); /* E-CID */
    encap_record.data[6] = 0x00;
    encap_record.data[7] = 0x00;
    encap_record.len     = 8;

    CBX_EPP_INSERT_EOR(&encap_record);

    /* Update LIN encap only on Avenger where Ext port is present */
    /* The behavior depends on CB_ETM_Cascade_Port_Enable reg value.
     * If Cascade port is set in the above reg, then put same encap directive in
     * both Avengers, else configure DLIET of only one Avenger */
    uint32_t status = 0;
    REG_READ_CB_ETM_CASCADE_PORT_ENABLEr(unit, &status);
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY) && (status == 0)) {
        if (unit) {
            CBX_IF_ERROR_RETURN(cbxi_lin_encap_set(
                        ex_port, dlinid, 0, &encap_record, CBXI_ENCAP_AVG_SEC));
            encap_record.len = 0;
            CBX_EPP_INSERT_EOR(&encap_record);
            CBX_IF_ERROR_RETURN(cbxi_lin_encap_set(
                        ex_port, dlinid, 0, &encap_record, CBXI_ENCAP_AVG_PRI));

        } else {
            CBX_IF_ERROR_RETURN(cbxi_lin_encap_set(
                        ex_port, dlinid, 0, &encap_record, CBXI_ENCAP_AVG_PRI));
            encap_record.len = 0;
            CBX_EPP_INSERT_EOR(&encap_record);
            CBX_IF_ERROR_RETURN(cbxi_lin_encap_set(
                        ex_port, dlinid, 0, &encap_record, CBXI_ENCAP_AVG_SEC));

        }
    } else {
        CBX_IF_ERROR_RETURN(cbxi_lin_encap_set(
                        ex_port, dlinid, 0, &encap_record, CBXI_ENCAP_AVG_SYS));
    }

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        /* Enable Tx for the dlinid on partner Avenger
         * This is to handle Uplink failover case */
        unit = CBXI_CASC_PARTNER_UNIT(unit);
        if (ext_info->uplink_port[unit] != CBX_PORT_INVALID) {
            CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                    ext_info->uplink_port[unit], &port, &up_port_lpg, &unit));
            CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(unit, dlinid, up_port_lpg));
        }
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_extender_port_clear
 * Purpose:
 *  Clear extender port settings when L2 addr is removed
 */
int
cbxi_pe_extender_port_clear(cbx_portid_t ex_port)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    litet_t        ent_litet;
    pgt_t          ent_pgt;
    cbxi_pgid_t    ex_port_lpg;
    cbx_port_t    port;
    int            dlinid;
    uint32_t       status = 0;
    int            unit = 0;

    CHECK_EXTENDER_INIT(ext_info);

    CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                                        ex_port, &port, &ex_port_lpg, &unit));

    CBX_IF_ERROR_RETURN(soc_robo2_pgt_get(unit, ex_port_lpg, &ent_pgt));
    /* VSI used for uplink is same as the dlinind */
    dlinid = PV_SLI_2_VSI(ent_pgt.default_sli);

    ent_litet.tx_ena = FALSE;
    CBX_IF_ERROR_RETURN(soc_robo2_litet_set(unit, dlinid, &ent_litet, &status));
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        /* Clear on Secondary also */
        CBX_IF_ERROR_RETURN(soc_robo2_litet_set(
                        CBX_AVENGER_SECONDARY, dlinid, &ent_litet, &status));
    }
    CBX_IF_ERROR_RETURN(cbxi_pe_port_sli_ecid_clear(ex_port));

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_pe_cascade_port_set
 * Purpose:
 *  Configure given port as cascade port
 *  Note: e_cid refers to 14 bit E-CID: 2 bit GRP + 12 bit E_CID base
 */
int
cbxi_pe_cascade_port_set(
                cbx_portid_t cs_port, uint32_t e_cid, cbx_l2_entry_t *l2entry)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;

    CHECK_EXTENDER_INIT(ext_info);

    /* ********** Downstream settings ********** */
    /* Add L2 entry with key as E_CID  */
    l2entry->params.flags        = CBX_L2_ADDR_ECID;
    l2entry->vlanid              = ext_info->downlink_vsi_ext_uc;
    l2entry->params.e_cid        = e_cid;

    /* Destination is set as type LIN in L2 module while composing the hardware
     * L2 ARL structure */

    if (CBX_PORTID_IS_TRUNK(cs_port)) {
        if (cbxi_pe_shared_lag_get(cs_port) == CBX_E_NONE) {
            /* No upstream setting for Shared LAG */
            return CBX_E_NONE;
        }
    }


    /* ********** Upstream settings ********** */
    /* No separate upstream settings required. */

    return CBX_E_NONE;
}

/**************************************************************************
 *                 EXTENDER FSAL API IMPLEMENTATION                           *
 **************************************************************************/

/**
 * Initializes the Port Extender subsystem
 * - setup of Port Extender module
 *
 * @param void
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_extender_init(void)
{
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
#ifdef CONFIG_PORT_EXTENDER
    cbxi_slic_rule_id_t  idx;
    vsit_t   ent_vsit;
    tet_t    ent_tet;
    uint8_t  unit = 0;
    uint8_t  num_units = 1;
    uint32   temp, regval;
    uint32_t status = 0;

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    /* Populate Avg CSD LPGs.
     * In case Avg CSD ports are not configured start with some default values
     * It will changed once Avg CSD ports are configured*/
        avg_csd_lpg[0] = 13;
        avg_csd_lpg[1] = 13;
        CBX_IF_ERROR_RETURN(cbxi_pe_avg_csd_config());
    }


    sal_memset(ext_info, 0, sizeof(cbx_extender_info));

#if 0
        /* Requirement is that to change Port type dynamically
         * Hence if PE comes up in Access mode, then the configuration done till
         * the mode changes to Trasit(one of the port chaged to PE Cascade type)
         * may not work properly */
    ext_info->mode = cbxiPEModeAccess;
#endif
    ext_info->mode = cbxiPEModeTransit;

    if (ext_info->pe_cascade_bitmap == NULL) {
        ext_info->pe_cascade_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(CBX_PBMP_PORT_MAX), "CASCADE-BITMAP");
        sal_memset(ext_info->pe_cascade_bitmap, 0,
                        SHR_BITALLOCSIZE(CBX_PBMP_PORT_MAX));
    }
    if (ext_info->pe_cascade_bitmap == NULL) {
        return CBX_E_MEMORY;
    }

    ext_info->uplink_port[CBX_AVENGER_PRIMARY] = CBX_PORT_INVALID;
    ext_info->uplink_port[CBX_AVENGER_SECONDARY] = CBX_PORT_INVALID;

    /* It is assumed that COSQ init has been done */

    for (unit = 0; unit < num_units; unit++) {
       /* Disable learning */
        temp = 1;
        CBX_IF_ERROR_RETURN(REG_READ_CB_ITM_ARL_CONFIGr(unit, &regval));
        CBX_IF_ERROR_RETURN(soc_CB_ITM_ARL_CONFIGr_field_set(
                                    unit, &regval, DISABLE_LEARNINGf, &temp));
        CBX_IF_ERROR_RETURN(REG_WRITE_CB_ITM_ARL_CONFIGr(unit, &regval));

        CBX_IF_ERROR_RETURN(soc_robo2_tet_get(
                        unit, cbxiTagReserved1, &ent_tet));
        ent_tet.tag = CBX_TPID_ETAG; /* ETAG */
        CBX_IF_ERROR_RETURN(soc_robo2_tet_set(
                        unit, cbxiTagReserved1, &ent_tet, &status));
        CBX_IF_ERROR_RETURN(soc_robo2_tet_get(
                        unit, cbxiTagReserved2, &ent_tet));
        ent_tet.tag = 0xAAB1; /* CB tag */
        CBX_IF_ERROR_RETURN(soc_robo2_tet_set(
                        unit, cbxiTagReserved2, &ent_tet, &status));


        for (idx = 0; idx < SLIC_MAX_INDEX; idx++) {
            switch (idx)
            {
                case SLIC_CSD_DEV1_DROP:
                case SLIC_CSD_DEV0_FWD:
                case SLIC_ARP_BCAST:
                case SLIC_ARP_UCAST:
                case SLIC_LLDP:
                case SLIC_L2CP:
                    break;
                case SLIC_PE_1BR_DROP:
                case SLIC_PE_1BR_FWD_UPSTR:
                case SLIC_PE_1BR_DECAP:
                case SLIC_PE_1BR_FWD_DWNSTR_UC:
                case SLIC_PE_1BR_FWD_DWNSTR_MC:
                case SLIC_PE_UNTAG_DROP:
                case SLIC_PE_UNTAG_FWD:
                    CBX_IF_ERROR_RETURN(
                                cbxi_slic_rule_validate(unit, idx, TRUE));
                    break;
                default:
                    /* Invalidate all other rules */
                    CBX_IF_ERROR_RETURN(
                                cbxi_slic_rule_validate(unit, idx, FALSE));
                    break;
            }
        }

        for (idx = SLIC_PE_1BR_UC_ETAG_U0_P0;
                                    idx <= SLIC_PE_1BR_UC_ETAG_LAG31; idx++) {
            /* Enable per port rules only when they are configured with ECID */
            CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(unit, idx, FALSE));
        }
        for (idx = SLIC_PE_1BR_IN_ECID_U0_P0;
                                    idx <= SLIC_PE_1BR_IN_ECID_LAG31; idx++) {
            /* Enable per port rules only when they are configured with ECID */
            CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(unit, idx, FALSE));
        }

        /* Enable LLDP drop and trap */
        CBX_IF_ERROR_RETURN(cbxi_slicid_map(
                            unit, SLIC_LLDP, SLICID_LLDP, SLIC_TRAP_GROUP2));

        /* Remove all ports from flooding domain in VSI 1 so that protocol
         * packets are not flooded */
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(unit, 1, &ent_vsit));
        ent_vsit.pg_map = 0;
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(unit, 1, &ent_vsit, &status));
    }

    CBX_IF_ERROR_RETURN(cbxi_pe_port_sli_init());

#ifdef CORTEX_M7
    CBX_IF_ERROR_RETURN(npa_init());
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        CBX_IF_ERROR_RETURN(gpio_init());
    }
#endif /* CORTEX_M7 */

    ext_info->init = TRUE;

    CBX_IF_ERROR_RETURN(cbxi_internal_loopback_port_set());

    return CBX_E_NONE;
#else /* CONFIG_PORT_EXTENDER */
    ext_info->init = FALSE;
    return CBX_E_UNAVAIL;
#endif /* !CONFIG_PORT_EXTENDER */
}

/**
 * Detach Port Extender previously initialized by cbx_extender_init()
 * - shut down the Port Extender subsystem
 *
 * @param void
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_extender_detach(void)
{
#ifdef CONFIG_PORT_EXTENDER
    cbx_extender_sw_info_t *ext_info = &cbx_extender_info;
    cbxi_slic_rule_id_t  idx;
    vsit_t   ent_vsit;
    uint8_t  unit = 0;
    uint8_t  num_units = 1;
    uint32   temp, regval;
    uint32_t status = 0;

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    sal_free(ext_info->pe_cascade_bitmap);

    for (unit = 0; unit < num_units; unit++) {
       /* Enable learning */
        temp = 0;
        CBX_IF_ERROR_RETURN(REG_READ_CB_ITM_ARL_CONFIGr(unit, &regval));
        CBX_IF_ERROR_RETURN(soc_CB_ITM_ARL_CONFIGr_field_set(
                                    unit, &regval, DISABLE_LEARNINGf, &temp));
        CBX_IF_ERROR_RETURN(REG_WRITE_CB_ITM_ARL_CONFIGr(unit, &regval));

        for (idx = 0; idx < SLIC_MAX_INDEX; idx++) {
            switch (idx)
            {
                case SLIC_CSD_DEV1_DROP:
                case SLIC_CSD_DEV0_FWD:
                case SLIC_ARP_BCAST:
                case SLIC_ARP_UCAST:
                case SLIC_LLDP:
                case SLIC_L2CP:
                    break;
                case SLIC_PE_1BR_DROP:
                case SLIC_PE_1BR_FWD_UPSTR:
                case SLIC_PE_1BR_DECAP:
                case SLIC_PE_1BR_FWD_DWNSTR_UC:
                case SLIC_PE_1BR_FWD_DWNSTR_MC:
                case SLIC_PE_UNTAG_DROP:
                case SLIC_PE_UNTAG_FWD:
                    /* Disable all port extender rules */
                    CBX_IF_ERROR_RETURN(
                                cbxi_slic_rule_validate(unit, idx, FALSE));
                    break;
                default:
                    /* Enable all rules */
                    CBX_IF_ERROR_RETURN(
                                cbxi_slic_rule_validate(unit, idx, TRUE));
                    break;
            }
        }
        for (idx = SLIC_PE_1BR_UC_ETAG_U0_P0;
                                    idx <= SLIC_PE_1BR_UC_ETAG_LAG31; idx++) {
            /* Enable per port rules only when they are configured with ECID */
            CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(unit, idx, FALSE));
        }
        for (idx = SLIC_PE_1BR_IN_ECID_U0_P0;
                                    idx <= SLIC_PE_1BR_IN_ECID_LAG31; idx++) {
            /* Enable per port rules only when they are configured with ECID */
            CBX_IF_ERROR_RETURN(cbxi_slic_rule_validate(unit, idx, FALSE));
        }

        /* Enable all ports for flooding domain in VSI 1 */
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_get(unit, 1, &ent_vsit));
        ent_vsit.pg_map = PBMP_GE_ALL(unit) | PBMP_XL_ALL(unit);
        CBX_IF_ERROR_RETURN(soc_robo2_vsit_set(unit, 1, &ent_vsit, &status));

    }

#ifdef CORTEX_M7
    npa_uninit();
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        gpio_uninit();
    }
#endif /* CORTEX_M7 */

    ext_info->init = FALSE;

    return CBX_E_NONE;
#else /* CONFIG_PORT_EXTENDER */
    return CBX_E_UNAVAIL;
#endif /* !CONFIG_PORT_EXTENDER */
}
