/*
 * $Id: field.c,v 1.418 Broadcom SDK $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Module: Field Processor APIs
 *
 * Purpose:
 *     API for Field Processor (FP) for ROBO family and later.
 *
 *
 */


#include <shared/bsl.h>

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/field.h>
#include <sal/types.h>
#include <bcm_int/robo/field.h>
#include <soc/drv.h>
#include <bcm_int/robo_dispatch.h>

/*
 * Function: bcm_robo_custom_field_action_t_init
 *
 * Purpose:
 *     Initialize Field processor Action object
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     action - (OUT) Pointer to action object
 * Returns:
 *     BCM_E_XXX
 *
 */
int
bcm_robo_custom_field_action_t_init(int unit, bcm_custom_field_action_t *action)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_FIELD_ACTION_T_INIT(unit, action));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_field_init
 *
 * Purpose:
 *     This routine is used to initialize a Field processor
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     field_params - (IN) Field parameters structure.
 *     field_id - (OUT) Handle to field processor
 *
 * Returns:
 *     BCM_E_XXX
 *
 */
int bcm_robo_custom_field_init(int unit, bcm_custom_field_params_t *field_params, bcm_custom_field_id_t *field_id)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_FIELD_INIT(unit, field_params, field_id));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_field_detach
 *
 * Purpose:
 *     Detach a field processor previously initialized by bcm_robo_custom_field_init()
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     field_id - (IN) Handle of the field processor to be destroyed
 *
 * Returns:
 *     BCM_E_XXX
 *
 */
int
bcm_robo_custom_field_detach(int unit, bcm_custom_field_id_t field_id)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_FIELD_DETACH(unit, field_id));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_field_rule_t_init
 *
 * Purpose:
 *     Prefill attribute array in the Rule with initialized values and masks
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     pkt_type - (IN) L3/L4 packet type
 *     rule - (OUT) rule object
 *
 * Returns:
 *     BCM_E_XXX
 *
 */
int
bcm_robo_custom_field_rule_t_init(int unit, bcm_custom_field_l3_l4_pkt_type_t pkt_type, bcm_custom_field_rule_t *rule)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_FIELD_RULE_T_INIT(unit, pkt_type, rule));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_field_rule_set
 *
 * Purpose:
 *     This routine is used to set a Field processor rule
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     field_id - (IN) Field processor Identifier
 *     rule - (IN) Rule object
 *     index - Rule Index
 *
 * Returns:
 *     BCM_E_XXX
 *
 */
int
bcm_robo_custom_field_rule_set(int unit, bcm_custom_field_id_t *field_id, bcm_custom_field_rule_t *rule, uint32 index)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_FIELD_RULE_SET(unit, field_id, rule, index));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_field_rule_clear
 *
 * Purpose:
 *     This routine is used to clear a Field processor rule
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     field_id - (IN) Field processor identifier
 *     index - (IN) Rule Index
 *
 * Returns:
 *     BCM_E_XXX
 *
 */
int
bcm_robo_custom_field_rule_clear(int unit, bcm_custom_field_id_t *field_id, uint32 index)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_FIELD_RULE_CLEAR(unit, field_id, index));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_field_action_set
 *
 * Purpose:
 *     This routine is used to set a Field processor action
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     field_id - (IN) Field processor Identifier
 *     action - (IN) Action object
 *     index - (IN) Action Index
 *
 * Returns:
 *     BCM_E_XXX
 *
 */
int
bcm_robo_custom_field_action_set(int unit, bcm_custom_field_id_t *field_id, bcm_custom_field_action_t *action, uint32 index)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_FIELD_ACTION_SET(unit, field_id, action, index));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_field_action_clear
 *
 * Purpose:
 *     This routine is used to clear a Field processor action
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     field_id - (IN) Field processor Identifier
 *     index - (IN) Action Index
 *
 * Returns:
 *     BCM_E_XXX
 *
 */
int
bcm_robo_custom_field_action_clear(int unit, bcm_custom_field_id_t *field_id, uint32 index)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_FIELD_ACTION_CLEAR(unit, field_id, index));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_field_rule_dump
 *
 * Purpose:
 *     This routine is used to dump a Field processor rule
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     field_id - (IN) Field processor Identifier
 *     index - (IN) Action Index
 *
 * Returns:
 *     BCM_E_XXX
 *
 */
int
bcm_robo_custom_field_rule_dump(int unit, bcm_custom_field_id_t *field_id, uint32 index)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_FIELD_RULE_DUMP(unit, field_id, index));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_field_action_dump
 *
 * Purpose:
 *     This routine is used to dump a Field processor action
 *
 * Parameters:
 *     unit - (IN) BCM device number
 *     field_id - (IN) Field processor Identifier
 *     index - (IN) Action Index
 *
 * Returns:
 *     BCM_E_XXX
 *
 */
int
bcm_robo_custom_field_action_dump(int unit, bcm_custom_field_id_t *field_id, uint32 index)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_FIELD_ACTION_DUMP(unit, field_id, index));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_field_stat_multi_get
 *
 * Purpose:
 *     Get counter values for multiple statistics type
 *
 * Parameters:
 *     unit     - (IN) BCM device number
 *     field_id - (IN) Field processor Identifier
 *     nstat    - (IN) Number of elements in stat array
 *     stat_arr - (IN) Statistics type descriptor array
 *     value_arr - (OUT) Collected stat values
 *
 * Returns:
 *     BCM_E_XXX
 *
 */

int
bcm_robo_custom_field_stat_multi_get(int unit,
                                     bcm_custom_field_id_t *field_id,
                                     uint32 rule_id,
                                     int nstat,
                                     bcm_field_stat_t *stat_arr,
                                     uint64 *value_arr)
{
    BCM_IF_ERROR_RETURN(DRV_CUSTOM_FIELD_STAT_MULTI_GET(unit, field_id,
                                       rule_id, nstat, stat_arr, value_arr));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_field_stat_multi_set
 *
 * Purpose:
 *     Set the counter values for statistics of a field entry
 *
 * Parameters:
 *     unit     - (IN) BCM device number
 *     field_id - (IN) Field processor Identifier
 *     nstat    - (IN) Number of elements in stat array
 *     stat_arr - (IN) Statistics type descriptor array
 *     value_arr - (IN) Statistics value to set
 *
 * Returns:
 *     BCM_E_XXX
 *
 */
int
bcm_robo_custom_field_stat_multi_set(int unit,
                                     bcm_custom_field_id_t *field_id,
                                     uint32 rule_id,
                                     int nstat,
                                     bcm_field_stat_t *stat_arr,
                                     uint64 *value_arr)
{

    BCM_IF_ERROR_RETURN(DRV_CUSTOM_FIELD_STAT_MULTI_SET(unit, field_id,
                                      rule_id, nstat, stat_arr, value_arr));
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_field_stat_multi_get32
 *
 * Purpose:
 *     Get 32 bit counter values for multiple statistics type
 *
 * Parameters:
 *     unit     - (IN) BCM device number
 *     field_id - (IN) Field processor Identifier
 *     nstat    - (IN) Number of elements in stat array
 *     stat_arr - (IN) Statistics type descriptor array
 *     value_arr - (OUT) Collected stat values
 *
 * Returns:
 *     BCM_E_XXX
 *
 */

int
bcm_robo_custom_field_stat_multi_get32(int unit,
                                       bcm_custom_field_id_t *field_id,
                                       uint32 rule_id,
                                       int nstat,
                                       bcm_field_stat_t *stat_arr,
                                       uint32 *value_arr)
{
    int rv;
    uint64 value_arr64[2];
    int idx;
    if (nstat > 2) {
        return BCM_E_PARAM;
    }
    rv = bcm_custom_field_stat_multi_get(unit, field_id, rule_id, nstat, stat_arr, value_arr64);
    if (rv) {
        return rv;
    }
    for (idx = 0; idx < nstat; idx++) {
        value_arr[idx] = COMPILER_64_LO(value_arr64[idx]);
    }
    return BCM_E_NONE;
}

/*
 * Function: bcm_robo_custom_field_stat_multi_set32
 *
 * Purpose:
 *     Set 32 bit counter values for statistics of a field entry
 *     The higher 32 bit will be set to 0.
 *
 * Parameters:
 *     unit     - (IN) BCM device number
 *     field_id - (IN) Field processor Identifier
 *     nstat    - (IN) Number of elements in stat array
 *     stat_arr - (IN) Statistics type descriptor array
 *     value_arr - (IN) Statistics value to set
 *
 * Returns:
 *     BCM_E_XXX
 *
 */
int
bcm_robo_custom_field_stat_multi_set32(int unit,
                                       bcm_custom_field_id_t *field_id,
                                       uint32 rule_id,
                                       int nstat,
                                       bcm_field_stat_t *stat_arr,
                                       uint32 *value_arr)
{
    int rv;
    uint64 value_arr64[2];
    int idx;
    if (nstat > 2) {
        return BCM_E_PARAM;
    }

    for (idx = 0; idx < nstat; idx++) {
        COMPILER_64_SET(value_arr64[idx], 0, value_arr[idx]);
    }

    rv = bcm_custom_field_stat_multi_set(unit, field_id, rule_id, nstat, stat_arr, value_arr64);
    return rv;
}


