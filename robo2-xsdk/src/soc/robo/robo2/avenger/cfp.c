/*
 * $Id: cp.c $
 *
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <fsal_int/shared_types.h>
#include <fsal_int/types.h>
#include <fsal/cfp.h>
#include <avenger_service.h>
#include <sal/appl/io.h>

#define DRV_ERROR_RETURN(A)     \
    do {                        \
        int rv = A;             \
        SOC_IF_ERROR_RETURN(rv);\
    } while (0)


#define DROP_MLF_CFP_RULE_START_INDEX 18
/* -------------- CFP rule to disable drop_mlf for  Broadcast --------------- */
void cfp_bcast_action_create(cbx_cfp_action_t* action) {

    cbx_cfp_action_t_init(action);

    action->flags |= CBX_CFP_ACTION_DLF;

    return;
}

void cfp_bcast_rule_add(cbx_cfpid_t cfpid, uint32 index) {
    cbx_cfp_rule_t rule;
    int rv = 0; int i  = 0;
    rule.key_type = cbxCfpKeyL2EthernetACL;
    rule.attr_selector = sal_alloc((CBX_CFP_L2_ETHERNET_ATTR_MAX *
                    sizeof(cbx_cfp_rule_attr_selector_t)), "CFP_BCAST_RULE");
    for (i = 0; i < CBX_CFP_L2_ETHERNET_ATTR_MAX; i++) {
        rule.attr_selector[i].attr_val = sal_alloc(16,"CFP_BCAST_RULE");
        rule.attr_selector[i].attr_mask = sal_alloc(16,"CFP_BCAST_RULE");
        sal_memset(rule.attr_selector[i].attr_val, 0, 16 * sizeof(uint8_t));
        sal_memset(rule.attr_selector[i].attr_mask,0, 16 * sizeof(uint8_t));
    }
    rule.num_attributes = 1;
    rule.attr_selector[0].attr = cbxCfpRuleAttrDestMac;
    rule.attr_selector[0].attr_val[5] = 0xFF;
    rule.attr_selector[0].attr_val[4] = 0xFF;
    rule.attr_selector[0].attr_val[3] = 0xFF;
    rule.attr_selector[0].attr_val[2] = 0xFF;
    rule.attr_selector[0].attr_val[1] = 0xFF;
    rule.attr_selector[0].attr_val[0] = 0xFF;
    rule.attr_selector[0].attr_len = 6;
    rule.attr_selector[0].attr_mask[5] = 0xFF;
    rule.attr_selector[0].attr_mask[4] = 0xFF;
    rule.attr_selector[0].attr_mask[3] = 0xFF;
    rule.attr_selector[0].attr_mask[2] = 0xFF;
    rule.attr_selector[0].attr_mask[1] = 0xFF;
    rule.attr_selector[0].attr_mask[0] = 0xFF;

    rv = cbx_cfp_rule_set(&cfpid, &rule, index);
    if (rv != 0) {
        sal_printf(" Failed to add entry\n");
    }
    for (i = 0; i < CBX_CFP_L2_ETHERNET_ATTR_MAX; i++) {
        sal_free(rule.attr_selector[i].attr_val);
        sal_free(rule.attr_selector[i].attr_mask);
    }
    sal_free(rule.attr_selector);
}
/*
 *  Function : drv_avgr_custom_field_action_t_init
 *
 *  Purpose :
 *      Initialize Field processor Action object
 *
 *  Parameters :
 *      unit            :  RoboSwitch unit number.
 *      action          :  Pointer to action object
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int drv_avgr_custom_field_action_t_init(int unit, bcm_custom_field_action_t *action)
{
    cbx_cfp_action_t cbx_action;
    
    if (action == NULL)
        return SOC_E_PARAM;
    
    cbx_cfp_action_t_init(&cbx_action);
    
    /* Convert cbx action to bcm field action */
    action->flags = cbx_action.flags;
    action->common_action.meterid = 0;
    action->common_action.dp         = cbx_action.common_action.dp;
    action->common_action.count_mode = cbx_action.common_action.count_mode;
    
    action->in_policy_action.forward_mode  = cbx_action.in_policy_action.forward_mode;
    action->in_policy_action.ucastid       = cbx_action.in_policy_action.ucastid;
    action->in_policy_action.mcast_addr    = NULL;
    action->in_policy_action.sourceid      = cbx_action.in_policy_action.sourceid;
    action->in_policy_action.pri           = cbx_action.in_policy_action.pri;
    action->in_policy_action.trap_group    = cbx_action.in_policy_action.trapid;
    action->in_policy_action.mirror_dest   = NULL;
    
    action->out_policy_action.forward_mode = cbx_action.out_policy_action.forward_mode;
    action->out_policy_action.ucastid      = cbx_action.out_policy_action.ucastid;
    action->out_policy_action.mcast_addr   = NULL;
    action->out_policy_action.sourceid     = cbx_action.out_policy_action.sourceid;
    action->out_policy_action.pri          = cbx_action.out_policy_action.pri;
    action->out_policy_action.trap_group   = cbx_action.out_policy_action.trapid;
    action->out_policy_action.mirror_dest  = NULL;
    
    return SOC_E_NONE;
}

/*
 *  Function : drv_avgr_custom_field_init
 *
 *  Purpose :
 *      This routine is used to initialize a Field processor
 *
 *  Parameters :
 *      unit            :  RoboSwitch unit number.
 *      field_params    :  Field parameters structure.
 *      field_id        :  Handle to field processor
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int drv_avgr_custom_field_init(int unit, bcm_custom_field_params_t *field_params, bcm_custom_field_id_t *field_id)
{
    cbx_cfp_params_t cbx_cfp_params;
    cbx_cfpid_t cfpid;
    cbx_cfp_action_t action;
    
    if (field_params == NULL)
        return SOC_E_PARAM;
    
    if (field_id == NULL)
        return SOC_E_PARAM;
    
    /* Copy In params */
    cbx_cfp_params.stage = field_params->stage;
    
    DRV_ERROR_RETURN(cbx_cfp_init(&cbx_cfp_params, &cfpid));
    
    /* Copy out params */
    *field_id = cfpid;

    /* Set action to disable drop_mlf for Broadcast packets */
    cfp_bcast_action_create(&action);
    cfp_bcast_rule_add(cfpid, DROP_MLF_CFP_RULE_START_INDEX);
    cbx_cfp_action_set(&cfpid, &action, DROP_MLF_CFP_RULE_START_INDEX);
    sal_printf("Broadcast CFP rule added at idx = %d\n", DROP_MLF_CFP_RULE_START_INDEX);

    return SOC_E_NONE;
}

/*
 *  Function : drv_avgr_custom_field_detach
 *
 *  Purpose :
 *      Detach a field processor previously initialized by drv_avgr_custom_field_init()
 *
 *  Parameters :
 *      unit            :  RoboSwitch unit number.
 *      field_id        :  Handle of the field processor to be destroyed
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int drv_avgr_custom_field_detach(int unit, bcm_custom_field_id_t field_id)
{
    cbx_cfpid_t cfpid;

    cfpid = field_id;
    
    DRV_ERROR_RETURN(cbx_cfp_detach(cfpid));
    
    return SOC_E_NONE;
}

/*
 *  Function : drv_avgr_custom_field_rule_t_init
 *
 *  Purpose :
 *      Prefill attribute array in the Rule with initialized values and masks
 *
 *  Parameters :
 *      unit            :  RoboSwitch unit number.
 *      pkt_type        :  L3/L4 packet type
 *      rule            :  rule object
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int drv_avgr_custom_field_rule_t_init(int unit, bcm_custom_field_l3_l4_pkt_type_t pkt_type, bcm_custom_field_rule_t *rule)
{
    int i;
    cbx_cfp_rule_t cbx_rule;
    cbx_cfp_l3_l4_pkt_type_t cbx_pkt_type;
    
    if (rule == NULL)
        return SOC_E_PARAM;
    
    if (pkt_type > bcmFieldL3L4PktTypeIgmp)
        return SOC_E_PARAM;
    
    cbx_pkt_type = pkt_type;
    
    /* Copy in params */
    cbx_rule.key_type       = rule->key_type;
    cbx_rule.num_attributes = rule->num_attributes;
    cbx_rule.attr_selector  = (cbx_cfp_rule_attr_selector_t *)rule->attr_selector;
    
    for (i = 0; i < rule->num_attributes; i++) {
        cbx_rule.attr_selector[i].attr      = rule->attr_selector[i].attr;
        cbx_rule.attr_selector[i].attr_len  = rule->attr_selector[i].attr_len;
        cbx_rule.attr_selector[i].attr_val  = rule->attr_selector[i].attr_val;
        cbx_rule.attr_selector[i].attr_mask = rule->attr_selector[i].attr_mask;
    }
    
    cbx_cfp_rule_t_init(cbx_pkt_type, &cbx_rule);
    
    /* Copy out params */
    rule->key_type       = cbx_rule.key_type;
    rule->num_attributes = cbx_rule.num_attributes;
    for (i = 0; i < rule->num_attributes; i++) {
        rule->attr_selector[i].attr      = cbx_rule.attr_selector[i].attr;
        rule->attr_selector[i].attr_len  = cbx_rule.attr_selector[i].attr_len;
    }
    
    return SOC_E_NONE;
}

/*
 *  Function : drv_avgr_custom_field_rule_set
 *
 *  Purpose :
 *      This routine is used to set a Field processor rule
 *
 *  Parameters :
 *      unit            :  RoboSwitch unit number.
 *      field_id        :  Field processor Identifier
 *      rule            :  Rule object
 *      index           :  Rule Index
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int drv_avgr_custom_field_rule_set(int unit, bcm_custom_field_id_t *field_id, bcm_custom_field_rule_t *rule, uint32 index)
{
    int i;
    cbx_cfpid_t cfpid;
    cbx_cfp_rule_t cbx_rule;
    
    if (field_id == NULL)
        return SOC_E_PARAM;
    
    if ((rule == NULL) || (rule->attr_selector == NULL))
        return SOC_E_PARAM;
    
    /* Copy in params */
    cfpid = *field_id;

    cbx_rule.key_type       = rule->key_type;
    cbx_rule.num_attributes = rule->num_attributes;
    cbx_rule.attr_selector  = (cbx_cfp_rule_attr_selector_t *)rule->attr_selector;
    
    for (i = 0; i < rule->num_attributes; i++) {
        cbx_rule.attr_selector[i].attr      = rule->attr_selector[i].attr;
        cbx_rule.attr_selector[i].attr_len  = rule->attr_selector[i].attr_len;
        cbx_rule.attr_selector[i].attr_val  = rule->attr_selector[i].attr_val;
        cbx_rule.attr_selector[i].attr_mask = rule->attr_selector[i].attr_mask;
    }
    
    DRV_ERROR_RETURN(cbx_cfp_rule_set(&cfpid, &cbx_rule, index));

    return SOC_E_NONE;
}

/*
 *  Function : drv_avgr_custom_field_rule_clear
 *
 *  Purpose :
 *      This routine is used to clear a Field processor rule
 *
 *  Parameters :
 *      unit            :  RoboSwitch unit number.
 *      field_id        :  Field processor identifier
 *      index           :  Rule Index
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int drv_avgr_custom_field_rule_clear(int unit, bcm_custom_field_id_t *field_id, uint32 index)
{
    cbx_cfpid_t cfpid;

    if (field_id == NULL)
        return SOC_E_PARAM;

    /* Copy in params */
    cfpid = *field_id;
    
    DRV_ERROR_RETURN(cbx_cfp_rule_clear(&cfpid, index));

    return SOC_E_NONE;
}

/*
 *  Function : drv_avgr_custom_field_action_set
 *
 *  Purpose :
 *      This routine is used to set a Field processor action
 *
 *  Parameters :
 *      unit            :  RoboSwitch unit number.
 *      field_id        :  Field processor Identifier
 *      action          :  Action object
 *      index           :  Action Index
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int drv_avgr_custom_field_action_set(int unit, bcm_custom_field_id_t *field_id, bcm_custom_field_action_t *action, uint32 index)
{
    cbx_mcastid_t mcastid;
    cbx_cfpid_t cfpid;
    cbx_cfp_action_t cbx_action;

    if (field_id == NULL)
        return SOC_E_PARAM;
    
    if (action == NULL)
        return SOC_E_PARAM;

    /* Copy in params */
    cfpid = *field_id;
    
    /* Convert cbx action to bcm field action */
    sal_memset(&cbx_action, 0x0, sizeof(cbx_action));
    cbx_action.flags = action->flags;
    cbx_action.common_action.meterid = action->common_action.meterid;
    cbx_action.common_action.dp         = action->common_action.dp;
    cbx_action.common_action.count_mode = action->common_action.count_mode;

    cbx_action.in_policy_action.forward_mode  = action->in_policy_action.forward_mode;
    cbx_action.in_policy_action.ucastid       = action->in_policy_action.ucastid;
    if (action->in_policy_action.forward_mode == bcmFieldForwardMulticast) {
        if (action->in_policy_action.mcast_addr == NULL) {
            return SOC_E_PARAM;
        }
        mcastid = drvi_avgr_mcast_get_mcastid_by_mac_vid(
                            action->in_policy_action.mcast_addr->mac,
                            action->in_policy_action.mcast_addr->vid);
        if (mcastid == 0) {
            return SOC_E_PARAM;
        }
        cbx_action.in_policy_action.mcastid = mcastid;
    }
    cbx_action.in_policy_action.sourceid      = action->in_policy_action.sourceid;
    cbx_action.in_policy_action.pri           = action->in_policy_action.pri;
    cbx_action.in_policy_action.trapid        = action->in_policy_action.trap_group;
    if (action->in_policy_action.mirror_dest != NULL) {
        DRV_ERROR_RETURN(drvi_avgr_mirror_get_mirror_id(
                                unit,
                                action->in_policy_action.mirror_dest->mirror_dest_id,
                                &cbx_action.in_policy_action.mirrorid));
    }

    cbx_action.out_policy_action.forward_mode = action->out_policy_action.forward_mode;
    cbx_action.out_policy_action.ucastid      = action->out_policy_action.ucastid;
    if (action->out_policy_action.forward_mode == bcmFieldForwardMulticast) {
        if (action->out_policy_action.mcast_addr == NULL) {
            return SOC_E_PARAM;
        }
        mcastid = drvi_avgr_mcast_get_mcastid_by_mac_vid(
                            action->out_policy_action.mcast_addr->mac,
                            action->out_policy_action.mcast_addr->vid);
        if (mcastid == 0) {
            return SOC_E_PARAM;
        }
        cbx_action.out_policy_action.mcastid = mcastid;
    }
    cbx_action.out_policy_action.sourceid     = action->out_policy_action.sourceid;
    cbx_action.out_policy_action.pri          = action->out_policy_action.pri;
    cbx_action.out_policy_action.trapid       = action->out_policy_action.trap_group;
    if (action->out_policy_action.mirror_dest != NULL) {
        DRV_ERROR_RETURN(drvi_avgr_mirror_get_mirror_id(
                                unit,
                                action->out_policy_action.mirror_dest->mirror_dest_id,
                                &cbx_action.out_policy_action.mirrorid));
    }
   
    DRV_ERROR_RETURN(cbx_cfp_action_set(&cfpid, &cbx_action, index));

    return SOC_E_NONE;
}

/*
 *  Function : drv_avgr_custom_field_action_clear
 *
 *  Purpose :
 *      This routine is used to clear a Field processor action
 *
 *  Parameters :
 *      unit            :  RoboSwitch unit number.
 *      field_id        :  Field processor Identifier
 *      index           :  Action Index
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int drv_avgr_custom_field_action_clear(int unit, bcm_custom_field_id_t *field_id, uint32 index)
{
    cbx_cfpid_t cfpid;

    if (field_id == NULL)
        return SOC_E_PARAM;

    /* Copy in params */
    cfpid = *field_id;
    
    DRV_ERROR_RETURN(cbx_cfp_action_clear(&cfpid, index));

    return SOC_E_NONE;
}

/*
 *  Function : drv_avgr_custom_field_rule_dump
 *
 *  Purpose :
 *      This routine is used to dump a Field processor rule
 *
 *  Parameters :
 *      unit            :  RoboSwitch unit number.
 *      field_id        :  Field processor Identifier
 *      index           :  Action Index
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int drv_avgr_custom_field_rule_dump(int unit, bcm_custom_field_id_t *field_id, uint32 index)
{
    cbx_cfpid_t cfpid;

    if (field_id == NULL)
        return SOC_E_PARAM;

    /* Copy in params */
    cfpid = *field_id;
    
    cbx_cfp_rule_dump(&cfpid, index);

    return SOC_E_NONE;
}

/*
 *  Function : drv_avgr_custom_field_action_dump
 *
 *  Purpose :
 *      This routine is used to dump a Field processor action
 *
 *  Parameters :
 *      unit            :  RoboSwitch unit number.
 *      field_id        :  Field processor Identifier
 *      index           :  Action Index
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int drv_avgr_custom_field_action_dump(int unit, bcm_custom_field_id_t *field_id, uint32 index)
{
    cbx_cfpid_t cfpid;

    if (field_id == NULL)
        return SOC_E_PARAM;

    /* Copy in params */
    cfpid = *field_id;
    
    cbx_cfp_action_dump(&cfpid, index);

    return SOC_E_NONE;
}

/*
 *  Function : drv_avgr_custom_field_stat_multi_get
 *
 *  Purpose :
 *      This routine is used to get the statistics of a field entry
 *
 *  Parameters :
 *      unit            :  RoboSwitch unit number.
 *      field_id        :  Field processor Identifier
 *      index           :  Action Index
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int drv_avgr_custom_field_stat_multi_get(int unit,
                                         bcm_custom_field_id_t *field_id,
                                         uint32 index,
                                         int nstat,
                                         bcm_field_stat_t *stat_arr,
                                         uint64 *value_arr)
{
    cbx_cfp_stat_t cbx_cfp_stat_arr[2];
    cbx_cfpid_t cfpid;
    int stat_index;

    if (nstat > 2) {
        return SOC_E_PARAM;
    }

    if (field_id == NULL) {
        return SOC_E_PARAM;
    }

    /* Copy in params */
    cfpid = *field_id;

    for(stat_index = 0; stat_index < nstat; stat_index++) {
        switch (stat_arr[stat_index]) {
        case bcmFieldStatBytes:
            cbx_cfp_stat_arr[stat_index] = cbxCFPStatBytes;
            break;
        case bcmFieldStatPackets:
            cbx_cfp_stat_arr[stat_index] = cbxCFPStatPackets;
            break;
        case bcmFieldStatGreenPackets:
            cbx_cfp_stat_arr[stat_index] = cbxCFPStatPacketsInProfile;
            break;
        case bcmFieldStatNotGreenPackets:
            cbx_cfp_stat_arr[stat_index] = cbxCFPStatPacketsOutProfile;
            break;
        default:
           return SOC_E_PARAM;
        }
    }

    return cbx_cfp_stat_multi_get(&cfpid, index, nstat, cbx_cfp_stat_arr, value_arr);
}

/*
 *  Function : drv_avgr_custom_field_stat_multi_set
 *
 *  Purpose :
 *      This routine is used to get the statistics of a field entry
 *
 *  Parameters :
 *      unit            :  RoboSwitch unit number.
 *      field_id        :  Field processor Identifier
 *      index           :  Action Index
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int drv_avgr_custom_field_stat_multi_set(int unit,
                                         bcm_custom_field_id_t *field_id,
                                         uint32 index,
                                         int nstat,
                                         bcm_field_stat_t *stat_arr,
                                         uint64 *value_arr)
{
    cbx_cfp_stat_t cbx_cfp_stat_arr[2];
    cbx_cfpid_t cfpid;
    int stat_index;

    if (nstat > 2) {
        return SOC_E_PARAM;
    }

    if (field_id == NULL) {
        return SOC_E_PARAM;
    }

    /* Copy in params */
    cfpid = *field_id;

    for(stat_index = 0; stat_index < nstat; stat_index++) {
        switch (stat_arr[stat_index]) {
        case bcmFieldStatBytes:
            cbx_cfp_stat_arr[stat_index] = cbxCFPStatBytes;
            break;
        case bcmFieldStatPackets:
            cbx_cfp_stat_arr[stat_index] = cbxCFPStatPackets;
            break;
        case bcmFieldStatGreenPackets:
            cbx_cfp_stat_arr[stat_index] = cbxCFPStatPacketsInProfile;
            break;
        case bcmFieldStatNotGreenPackets:
            cbx_cfp_stat_arr[stat_index] = cbxCFPStatPacketsOutProfile;
            break;
        default:
           return SOC_E_PARAM;
        }
    }

    return cbx_cfp_stat_multi_set(&cfpid, index, nstat, cbx_cfp_stat_arr, value_arr);
}
