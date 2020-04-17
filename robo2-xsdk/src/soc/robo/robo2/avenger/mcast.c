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
#include <fsal/l2.h>
#include <fsal/mcast.h>
#include <bcm/types.h>
#include <bcm/mcast.h>
#include <avenger_service.h>
#include <shared/gport.h>
#include <bcm_int/common/multicast.h>
#define MAX_ML2_ENTRIES 2048

static cbx_l2_entry_t mcast_l2_entries[MAX_ML2_ENTRIES];
static int total_mcast_l2_entries;

extern int cbxi_mcast_init(void);
extern int cbx_ptp_init(void);

int drv_avgr_mcast_init(int unit)
{
    int rc;

    rc = cbxi_mcast_init();
#ifdef CONFIG_PTP
    /* PTP has dependency on multicast, initialize after the multicast */
    if (rc == 0) {
        rc = cbx_ptp_init();
    }
#endif
    return rc;
}

int drv_avgr_mcast_find_l2_entry_by_mac_vid(bcm_mac_t mac, cbx_vlanid_t vid)
{
   int j;

   for(j = 0; j < MAX_ML2_ENTRIES; j++) {
      if(mcast_l2_entries[j].vlanid == vid && (!sal_memcmp(mac,
      		  mcast_l2_entries[j].src_mac, sizeof(bcm_mac_t))))
          return j;
   }

   return -1;
}

cbx_mcastid_t drv_avgr_mcast_get_mcastid_by_mac_vid(bcm_mac_t mac, cbx_vlanid_t vid)
{
   int j;
   j = drv_avgr_mcast_find_l2_entry_by_mac_vid(mac, vid);

   if(j >= 0)
      return mcast_l2_entries[j].params.portid;
   else
      return 0;
}

int drv_avgr_mcast_create(int unit, bcm_mcast_addr_t *mcaddr)
{
   int rc;
   cbx_l2_entry_t cbx_l2entry;

   cbx_mcastid_t mcastid;
   cbx_mcast_params_t mcast_type;

   if(total_mcast_l2_entries >= MAX_ML2_ENTRIES)
       return CBX_E_FULL;

   sal_memset(&cbx_l2entry, 0, sizeof(cbx_l2_entry_t));
   sal_memcpy(&cbx_l2entry.src_mac, mcaddr->mac, sizeof(bcm_mac_t));

   cbx_l2entry.vlanid   = mcaddr->vid;
   mcast_type.lilt_mode = cbxMcastLiltModeNone;
   rc = cbx_mcast_create(&mcast_type, &mcastid);
   if(rc)
       return rc;
   mcaddr->l2mc_index           = mcastid;
   cbx_l2entry.params.flags     = CBX_L2_ADDR_STATIC;
   cbx_l2entry.params.dest_type = cbxL2AddrDestMcast;
   cbx_l2entry.params.portid    = mcastid;
   rc = cbx_l2_entry_add(&cbx_l2entry);

   mcast_l2_entries[total_mcast_l2_entries++] = cbx_l2entry;


   return rc;
}

int drv_avgr_mcast_destroy(int unit, bcm_mcast_addr_t *mcaddr)
{
   int rc, j;
   cbx_l2_entry_t cbx_l2entry;

   sal_memset(&cbx_l2entry, 0, sizeof(cbx_l2_entry_t));
   sal_memcpy(&cbx_l2entry.src_mac, mcaddr->mac, sizeof(bcm_mac_t));

   cbx_l2entry.vlanid = mcaddr->vid;
   cbx_l2entry.params.dest_type =  cbxL2AddrDestMcast;
   j = drv_avgr_mcast_find_l2_entry_by_mac_vid(mcaddr->mac, mcaddr->vid);
   if(j < 0)
       return CBX_E_PARAM;
   cbx_l2entry.params.portid = mcast_l2_entries[j].params.portid;

   mcaddr->l2mc_index = cbx_l2entry.params.portid;
   mcast_l2_entries[j] = mcast_l2_entries[total_mcast_l2_entries--];

   rc = cbx_l2_addr_remove(&cbx_l2entry);
   if(rc < 0) {
       return rc;
   }
   rc = cbx_mcast_destroy(mcaddr->l2mc_index);

   return rc;
}

int drv_avgr_mcast_info_get(int unit, bcm_mcast_addr_t *mcaddr)
{
   int j, rc;
   cbx_mcast_member_info_t mcast_member_info;
   cbx_portid_t       portid_array[32];
   cbx_l2_entry_t cbx_l2entry;


   mcast_member_info.portid_array = portid_array;
   mcast_member_info.max_portid   = 32;

   sal_memset(&cbx_l2entry, 0, sizeof(cbx_l2_entry_t));
   sal_memcpy(&cbx_l2entry.src_mac, mcaddr->mac, sizeof(bcm_mac_t));

   cbx_l2entry.vlanid = mcaddr->vid;
   cbx_l2entry.params.dest_type =  cbxL2AddrDestMcast;
   cbx_l2entry.params.portid = drv_avgr_mcast_get_mcastid_by_mac_vid(mcaddr->mac, mcaddr->vid);
   if(!cbx_l2entry.params.portid)
       return CBX_E_PARAM;

   /* Ensure both mcast id/portid & L2 entry are valid */
   /*rc = cbx_l2_lookup(&cbx_l2entry);
   if(rc)
       return rc; */

   mcaddr->l2mc_index = cbx_l2entry.params.portid;
   rc = cbx_mcast_member_info_get(mcaddr->l2mc_index, &mcast_member_info);
   if(rc)
       return rc;

   BCM_PBMP_CLEAR(mcaddr->pbmp);
   for (j = 0; j < mcast_member_info.valid_portid; j++) {
      int port;
      port          = CBX_PORTID_LOCAL_GET(mcast_member_info.portid_array[j]);
      mcaddr->pbmp |= (1 << port);
   }

   return CBX_E_NONE;
}

int drv_avgr_mcast_add(int unit, bcm_mcast_addr_t *mcaddr, uint32 portid)
{
   cbx_mcastid_t mcastid;


   mcastid = drv_avgr_mcast_get_mcastid_by_mac_vid(mcaddr->mac, mcaddr->vid);
   if(!mcastid)
       return CBX_E_PARAM;

   return cbx_mcast_member_add(mcastid, portid);

}

int drv_avgr_mcast_remove(int unit, bcm_mcast_addr_t *mcaddr, uint32 portid)
{
   cbx_mcastid_t mcastid;


   mcastid = drv_avgr_mcast_get_mcastid_by_mac_vid(mcaddr->mac, mcaddr->vid);
   if(!mcastid)
       return CBX_E_PARAM;

   return cbx_mcast_member_remove(mcastid, portid);
}

uint32 drvi_avgr_mcast_get_mcastid_by_mac_vid(bcm_mac_t mac, uint32 vid)
{
    return drv_avgr_mcast_get_mcastid_by_mac_vid(mac, vid);
}
