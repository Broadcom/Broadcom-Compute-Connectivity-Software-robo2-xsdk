/*
 * $Id: mcast.c $
 *
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <soc/drv.h>
#include <soc/error.h>
#include <fsal_int/shared_types.h>
#include <fsal/error.h>
#include <bcm/error.h>
#include <fsal/stg.h>
#include <bcm/types.h>
#include <bcm/stg.h>
#include <avenger_service.h>
#include <shared/gport.h>

extern int cbxi_stg_init(void);

int drv_avgr_stg_init(int unit)
{
   return cbxi_stg_init();
}

int drv_avgr_stg_create(int unit, int *stgid)
{
   int rc;
   cbx_stg_params_t stg_params;

   stg_params.stg_mode = cbxStgModeNormal;
   rc                  = cbx_stg_create(&stg_params, (cbx_stgid_t *)stgid);

   return rc;
}

int drv_avgr_stg_destroy(int unit, int stgid)
{
   int rc;

   rc = cbx_stg_destroy((cbx_stgid_t)stgid);

   return rc;
}

int drv_avgr_stg_vlan_add(int unit, int stgid, uint32 vlanid)
{
   int rc;

   rc = cbx_stg_vlan_add((cbx_stgid_t)stgid, (cbx_vlanid_t)vlanid);

   return rc;
}

int drv_avgr_stg_vlan_remove(int unit, int stgid, uint32 vlanid)
{
   int rc;

   rc = cbx_stg_vlan_remove((cbx_stgid_t)stgid, (cbx_vlanid_t)vlanid);

   return rc;
}

int drv_avgr_stg_vlan_remove_all(int unit, int stgid)
{
   int rc;

   rc = cbx_stg_vlan_remove_all((cbx_stgid_t)stgid);

   return rc;
}


void drv_avgr_stg_fsal_to_bcm_stp_state_convert(cbx_stg_stp_state_t fsal_stp_state, int *stp_state)
{
   bcm_stg_stp_t bcm_stp_state;

   switch(fsal_stp_state) {
      case cbxStgStpStateDiscard:
          bcm_stp_state = BCM_STG_STP_BLOCK;
          break;
      case cbxStgStpStateLearn:
          bcm_stp_state = BCM_STG_STP_LEARN;
          break;
      case cbxStgStpStateForward:
          bcm_stp_state = BCM_STG_STP_FORWARD;
          break;
      default:
          *stp_state = BCM_STG_STP_DISABLE;
          return;
   }

   *stp_state = bcm_stp_state;
   return;
}

void drv_avgr_stg_bcm_to_fsal_stp_state_convert(int stp_state, cbx_stg_stp_state_t *fsal_stp_state)
{
   bcm_stg_stp_t bcm_stp_state = stp_state;

   switch(bcm_stp_state) {
      case BCM_STG_STP_DISABLE:
          *fsal_stp_state = cbxStgStpStateDiscard;
          break;
      case BCM_STG_STP_BLOCK:
          *fsal_stp_state = cbxStgStpStateDiscard;
          break;
      case BCM_STG_STP_LISTEN:
          *fsal_stp_state = cbxStgStpStateDiscard;/*cbxStgStpStateLearn*/
          break;
      case BCM_STG_STP_LEARN:
          *fsal_stp_state = cbxStgStpStateLearn;
          break;
      case BCM_STG_STP_FORWARD:
          *fsal_stp_state = cbxStgStpStateForward;
          break;
      default:
          *fsal_stp_state = BCM_STG_STP_DISABLE;
          return;
   }

   return;
}

int drv_avgr_stg_stp_get(int unit, int stgid, int port, int *stp_state)
{
   int rc;
   cbx_stg_stp_state_t fsal_stp_state;

   rc = cbx_stg_stp_get((cbx_stgid_t)stgid, port, &fsal_stp_state);

   drv_avgr_stg_fsal_to_bcm_stp_state_convert(fsal_stp_state, stp_state);

   return rc;
}

int drv_avgr_stg_stp_set(int unit, int stgid, int port, int stp_state)
{
   int rc;
   cbx_stg_stp_state_t fsal_stp_state;


   drv_avgr_stg_bcm_to_fsal_stp_state_convert(stp_state, &fsal_stp_state);

   rc = cbx_stg_stp_set((cbx_stgid_t)stgid, port, fsal_stp_state);

   return rc;
}
