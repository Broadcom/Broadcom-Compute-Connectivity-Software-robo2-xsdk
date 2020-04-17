/*
 * $Id: stat.c $
 *
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <soc/drv.h>
#include <soc/error.h>
#include <soc/robo_stat.h>
#include <fsal_int/shared_types.h>
#include <fsal/vlan.h>
#include <fsal/error.h>
#include <bcm/vlan.h>
#include <avenger_service.h>
#include <shared/bsl.h>

extern int cbxi_vlan_init(void);

int drv_avgr_vlan_init(int unit) {
    int rv = CBX_E_NONE;
    rv = cbxi_vlan_init();
    if(rv < 0) {
       LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,"%s failed with err:%d\n"),__func__, rv));
    }
    return rv;
}
int drv_avgr_vlan_create(int unit, uint32 vlanid) {

    int rv;
    cbx_vlanid_t vsiid;
    cbx_vlan_params_t params;

    cbx_vlan_params_t_init(&params);
    params.vlan = vlanid;
    rv = cbx_vlan_create(&params, &vsiid);

    return rv;
}

int drv_avgr_vlan_destroy(int unit, uint32 vid) {

    return cbx_vlan_destroy(vid);
}

int drv_avgr_vlan_destroy_all(int unit) {

    return cbx_vlan_destroy_all();
}

int drv_avgr_vlan_port_add(int unit, uint32 vid, int port, int flags) {

   uint32 mode = 0;
   if(flags & BCM_VLAN_GPORT_ADD_UNTAGGED)
      mode = CBX_VLAN_PORT_MODE_UNTAGGED;
   else
      mode = CBX_VLAN_PORT_MODE_TAGGED;

   return cbx_vlan_port_add(vid, port, mode);

}
int drv_avgr_vlan_port_remove(int unit, uint32 vid, int port) {

   return cbx_vlan_port_remove(vid, port);
}

int drv_avgr_vlan_gport_get_all(int unit, uint32 vlan, int array_max,
                           int *gport_array, int *flags_array,
                           int* array_size)
{
   return cbx_vlan_port_get(vlan, array_max, (cbx_portid_t *)gport_array, (uint32_t *)flags_array, array_size);
}

int drv_avgr_vlan_stg_get(int unit, uint32 vid, int *stg_id) {

   int rv;

   cbx_vlan_params_t vlan_params;
   vlan_params.stgid = 0;

   rv = cbx_vlan_get ((cbx_vlanid_t)vid, &vlan_params);
   *stg_id = (uint32)vlan_params.stgid;

   return rv;
}


int drv_avgr_vlan_stg_set(int unit, uint32 vid, uint32 stg_state) {

   int rv;

   cbx_vlan_params_t vlan_params;
   vlan_params.stgid = 0;

   rv = cbx_vlan_get ((cbx_vlanid_t)vid, &vlan_params);
   vlan_params.stgid = stg_state;
   rv = cbx_vlan_set ((cbx_vlanid_t)vid, vlan_params);

   return rv;
}

int drv_avgr_vlan_control_vlan_set(int unit, uint32 vid, uint32 vlan_index, uint32 flags,
                                   uint32 learnmode, uint32 mcastid, uint32 ucastid) {

   int rv;

   cbx_vlan_params_t vlan_params;
   memset(&vlan_params, 0, sizeof(vlan_params));

   rv = cbx_vlan_get ((cbx_vlanid_t)vid, &vlan_params);

   if (flags & BCM_VLAN_UNKNOWN_UCAST_DROP)
      vlan_params.flags |= CBX_VLAN_DROP_DLF;
   if (flags & BCM_VLAN_UNKNOWN_SMAC_DROP)
      vlan_params.flags |= CBX_VLAN_DROP_SLF;
   if (flags & BCM_VLAN_NON_UCAST_DROP)
      vlan_params.flags |= CBX_VLAN_DROP_BCAST;

   vlan_params.vlan = vlan_index;
   /* At present we are supporting only LearnModeMarkReady */
   if(learnmode & BCM_VLAN_LEARN_CONTROL_PENDING)
      vlan_params.learn_mode |= cbxVlanLearnModeMarkReady;
   else
      vlan_params.learn_mode = cbxVlanLearnModeMarkReady;

   /* Mapping of mcastid & ucastid b/w FSAL XGS5 is NA */
   vlan_params.mcastid = 0;
   vlan_params.ucastid = 0;
   rv = cbx_vlan_set ((cbx_vlanid_t)vid, vlan_params);

   return rv;
}

int drv_avgr_vlan_control_vlan_get(int unit, uint32 vid, uint32 *vlan_index, uint32 *flags,
                                   uint32 *learnmode, uint32 *mcastid, uint32 *ucastid) {

   int rv;

   cbx_vlan_params_t vlan_params;
   memset(&vlan_params, 0, sizeof(vlan_params));

   rv = cbx_vlan_get ((cbx_vlanid_t)vid, &vlan_params);

   *flags = 0;
   if (vlan_params.flags & CBX_VLAN_DROP_DLF)
      *flags |= BCM_VLAN_UNKNOWN_UCAST_DROP;
   if (vlan_params.flags & CBX_VLAN_DROP_SLF)
      *flags |= BCM_VLAN_UNKNOWN_SMAC_DROP;
   if (vlan_params.flags & CBX_VLAN_DROP_BCAST)
      *flags |= BCM_VLAN_NON_UCAST_DROP;

   *vlan_index = vlan_params.vlan;
   *learnmode = 0;
   if(vlan_params.learn_mode & cbxVlanLearnModeMarkReady)
      *learnmode |= BCM_VLAN_LEARN_CONTROL_PENDING;

   return rv;
}
