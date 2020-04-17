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
#include <fsal/error.h>
#include <bcm/error.h>
#include <fsal/l2.h>
#include <bcm/l2.h>
#include <avenger_service.h>
#include <shared/gport.h>
#include <bcm_int/common/multicast.h>

extern int cbxi_l2_init(void);

void drv_avgr_l2_bcm_to_fsal_convert(bcm_l2_addr_t *l2addr,
                      cbx_l2_entry_t *cbx_l2entry)
{
   if((!l2addr) || (!cbx_l2entry))
       return;

   sal_memset(cbx_l2entry, 0, sizeof(cbx_l2_entry_t));
   sal_memcpy(&cbx_l2entry->src_mac, l2addr->mac, sizeof(bcm_mac_t));
   cbx_l2entry->vlanid = l2addr->vid;

   cbx_l2entry->params.flags = 0;
   if (l2addr->flags & BCM_L2_STATIC) {
      cbx_l2entry->params.flags = CBX_L2_ADDR_STATIC;
   }

   if (l2addr->flags & BCM_L2_MCAST) {
       if ((l2addr->l2mc_group != -1) &&
                (_BCM_MULTICAST_IS_SET(l2addr->l2mc_group) &&
                                   _BCM_MULTICAST_IS_L2(l2addr->l2mc_group))) {
           cbx_l2entry->params.portid    = l2addr->l2mc_group;
           cbx_l2entry->params.dest_type =  cbxL2AddrDestMcast;
           cbx_l2entry->params.flags    |= CBX_L2_ADDR_STATIC;
       } else {
           cbx_l2entry->params.portid    = 0;
           cbx_l2entry->params.dest_type =  cbxL2AddrDestMcast;
       }
   } else {
       if(l2addr->flags & BCM_L2_TRUNK_MEMBER) {
          _SHR_GPORT_TRUNK_SET(cbx_l2entry->params.portid, l2addr->tgid);
       } else {
          cbx_l2entry->params.portid = _SHR_GPORT_LOCAL_GET(l2addr->port);
       }
       cbx_l2entry->params.dest_type =  cbxL2AddrDestDPG;
   }

   if (l2addr->flags & BCM_L2_DISCARD_SRC) {
      cbx_l2entry->params.fwd_ctrl = cbxL2AddrCtrlSrcDrop;
      cbx_l2entry->params.upd_ctrl = cbxL2AddrMovementUpdate;
   }
   if (l2addr->flags & BCM_L2_DISCARD_DST) {
      cbx_l2entry->params.dest_type =  cbxL2AddrDestDrop;
      cbx_l2entry->params.fwd_ctrl = cbxL2AddrCtrlNormal;
      cbx_l2entry->params.upd_ctrl = cbxL2AddrMovementUpdate;
   }

   if (cbx_l2entry->params.flags & CBX_L2_ADDR_ECID)
       cbx_l2entry->params.e_cid = 1;
   else
       cbx_l2entry->params.e_cid = 0;

   return;
}

void drv_avgr_l2_fsal_to_bcm_convert(bcm_l2_addr_t *l2addr,
                     cbx_l2_entry_t *cbx_l2entry)
{
   if((!l2addr) || (!cbx_l2entry))
       return;

   sal_memset(l2addr, 0, sizeof(bcm_l2_addr_t));
   sal_memcpy(l2addr->mac, &cbx_l2entry->src_mac, sizeof(bcm_mac_t));
   l2addr->vid = cbx_l2entry->vlanid;

   if (cbx_l2entry->params.flags & CBX_L2_ADDR_STATIC) {
       l2addr->flags |= BCM_L2_STATIC;
   }

   if(cbx_l2entry->params.dest_type ==  cbxL2AddrDestMcast) {
       
       _SHR_MULTICAST_L2_SET(l2addr->l2mc_group, cbx_l2entry->params.portid);
       l2addr->flags |= BCM_L2_MCAST;
       l2addr->flags |= BCM_L2_STATIC;
   } else {
       /*cbx_l2entry->params.dest_type = cbxL2AddrDestDPG;*/
       if(_SHR_GPORT_IS_TRUNK(cbx_l2entry->params.portid)) {
           _SHR_GPORT_TRUNK_SET(l2addr->tgid, cbx_l2entry->params.portid);
           l2addr->flags |= BCM_L2_TRUNK_MEMBER;
       } else {
           _SHR_GPORT_LOCAL_SET(l2addr->port, cbx_l2entry->params.portid);
       }
   }

   if(cbx_l2entry->params.dest_type == cbxL2AddrDestDrop)
       l2addr->flags |= BCM_L2_DISCARD_DST;

   if(cbx_l2entry->params.fwd_ctrl == cbxL2AddrCtrlSrcDrop)
       l2addr->flags |= BCM_L2_DISCARD_SRC;

    return;
}

int drv_avgr_l2_init(int unit)
{
   return cbxi_l2_init();
}

int drv_avgr_l2_add(int unit, bcm_l2_addr_t *l2addr)
{
   cbx_l2_entry_t l2entry;

   drv_avgr_l2_bcm_to_fsal_convert(l2addr, &l2entry);

   return cbx_l2_entry_add(&l2entry);
}

int drv_avgr_l2_remove(int unit, bcm_l2_addr_t *l2addr)
{
   cbx_l2_entry_t l2entry;
   int rc;

   drv_avgr_l2_bcm_to_fsal_convert(l2addr, &l2entry);

   rc = cbx_l2_lookup(&l2entry);
   if(rc)
       return rc;

   return cbx_l2_addr_remove(&l2entry);
}

int drv_avgr_l2_lookup(int unit, bcm_l2_addr_t *l2addr)
{
   cbx_l2_entry_t l2entry;
   int rc;

   drv_avgr_l2_bcm_to_fsal_convert(l2addr, &l2entry);

   rc = cbx_l2_lookup(&l2entry);
   if(rc == CBX_E_NOT_FOUND) {
      rc = BCM_E_NOT_FOUND;
   } else if(rc == CBX_E_NONE) {
      rc = BCM_E_NONE;
      drv_avgr_l2_fsal_to_bcm_convert(l2addr, &l2entry);
   }

   return rc;
}

int drv_avgr_l2_lookup_next(int unit, bcm_l2_addr_t *l2addr,
                       bcm_l2_addr_t *l2addr_next)
{
   cbx_l2_entry_t l2entry, l2entry_next, *l2_ptr = NULL;
   int rc;
   
   if(l2addr)
   l2_ptr = &l2entry;

   drv_avgr_l2_bcm_to_fsal_convert(l2addr, l2_ptr);
   rc = cbx_l2_lookup_next(l2_ptr, &l2entry_next);
   drv_avgr_l2_fsal_to_bcm_convert(l2addr_next, &l2entry_next);

   return rc;
}

int drv_avgr_l2_age_timer_get(int unit, uint32 *age_seconds)
{
   return cbx_l2_age_timer_get((int *)age_seconds);
}

int drv_avgr_l2_age_timer_set(int unit, uint32 age_seconds)
{
   return cbx_l2_age_timer_set(age_seconds);
}

int drv_avgr_l2_flush_by_vlan(int unit, uint32 vlan_id)
{
   return cbx_l2_flush(vlan_id);
}

int drv_avgr_l2_flush_by_port(int unit, uint32 port_id)
{
   return cbx_l2_port_flush(port_id);
}

int drv_avgr_l2_reseed(int unit)
{
   return cbx_l2_reseed();
}
