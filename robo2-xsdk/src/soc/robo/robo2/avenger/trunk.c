/*
 * $Id: trunk.c $
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
#include <fsal/lag.h>
#include <avenger_service.h>

#include <sal/appl/io.h>

extern int cbxi_slictcam_init(void);

#define DRV_TRUNK_MEMBER_ADD         0x01
#define DRV_TRUNK_MEMBER_REMOVE      0x02

#define LAG_PSC_PKTTYPE_IPV4_PBMP  (CBX_LAG_PSC_SPG |\
                                    CBX_LAG_PSC_SRCIP | \
                                    CBX_LAG_PSC_DSTIP )
#define LAG_PSC_PKTTYPE_IPV6_PBMP  (CBX_LAG_PSC_SPG |\
                                    CBX_LAG_PSC_SRCIP | \
                                    CBX_LAG_PSC_SRCIPV6 | \
                                    CBX_LAG_PSC_DSTIP |\
                                    CBX_LAG_PSC_DSTIPV6 )
#define LAG_PSC_PKTTYPE_OTHER_PBMP (CBX_LAG_PSC_SPG |\
                                    CBX_LAG_PSC_VLAN | \
                                    CBX_LAG_PSC_DSTMAC |\
                                    CBX_LAG_PSC_SRCMAC )

extern int cbx_lag_init( void );

int
_drv_avgr_trunk_drv2fsal_psc_convert( int psc_in, int *psc_out )
{
    if (psc_in & DRV_TRUNK_HASH_FIELD_MACSA ) {
        *psc_out |= CBX_LAG_PSC_SRCMAC;
        *psc_out |= CBX_LAG_PSC_SPG;
    }
    if (psc_in & DRV_TRUNK_HASH_FIELD_MACDA ) {
        *psc_out |= CBX_LAG_PSC_DSTMAC;
        *psc_out |= CBX_LAG_PSC_SPG;
    }
    if (psc_in & DRV_TRUNK_HASH_FIELD_VLANID ) {
        *psc_out |= CBX_LAG_PSC_VLAN;
        *psc_out |= CBX_LAG_PSC_SPG;
    }
    if (psc_in & DRV_TRUNK_HASH_FIELD_IP_MACSA ) {
        *psc_out |= CBX_LAG_PSC_SPG;
        *psc_out |= CBX_LAG_PSC_SRCIP;
        *psc_out |= CBX_LAG_PSC_SRCIPV6;
    }
    if (psc_in & DRV_TRUNK_HASH_FIELD_IP_MACDA ) {
        *psc_out |= CBX_LAG_PSC_SPG;
        *psc_out |= CBX_LAG_PSC_DSTIP;
        *psc_out |= CBX_LAG_PSC_DSTIPV6;
    }

    return SOC_E_NONE;
}

int
_drv_avgr_trunk_fsal2drv_psc_convert( int psc_in, int *psc_out )
{
    if (psc_in & CBX_LAG_PSC_SRCMAC) {
        *psc_out  |=  DRV_TRUNK_HASH_FIELD_MACSA;
    }
    if (psc_in & CBX_LAG_PSC_DSTMAC) {
        *psc_out  |=  DRV_TRUNK_HASH_FIELD_MACDA;
    }
    if (psc_in & CBX_LAG_PSC_VLAN) {
        *psc_out  |=  DRV_TRUNK_HASH_FIELD_VLANID;
    }
    if ((psc_in & CBX_LAG_PSC_SRCIP) ||
       (psc_in & CBX_LAG_PSC_SRCIPV6)) {
        *psc_out  |=  DRV_TRUNK_HASH_FIELD_IP_MACSA;
    }
    if ((psc_in & CBX_LAG_PSC_DSTIP) ||
       (psc_in & CBX_LAG_PSC_DSTIPV6)) {
        *psc_out  |=  DRV_TRUNK_HASH_FIELD_IP_MACDA;
    }

    return SOC_E_NONE;
}

int
_drv_avgr_trunk_psc_parse( int psc, unsigned int pkt_type, uint32 *psc_flags )
{
    /* From the psc input parse flags relevant for given pkt_type */
    switch (pkt_type) {
    case cbxPacketIpv4:
        *psc_flags = (psc & LAG_PSC_PKTTYPE_IPV4_PBMP);
        break;
    case cbxPacketIpv6:
        *psc_flags = (psc & LAG_PSC_PKTTYPE_IPV6_PBMP);
        break;
    case cbxPacketOther:
        *psc_flags = (psc & LAG_PSC_PKTTYPE_OTHER_PBMP);
        break;
    default:
        *psc_flags = 0;
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}

/*
 *  Function : drv_avgr_trunk_init
 *
 *  Purpose :
 *      Initialize trunk module.
 *
 *  Parameters :
 *      unit    :  RoboSwitch unit number.
 *
 *  Return :
 *      SOC_E_NONE
 *
 *  Note :
 */

int
drv_avgr_trunk_init( int unit )
{
    /* SLICTCAM to be initialized before trunk */
    sal_printf("cbxi_slictcam_init()\n");
    SOC_IF_ERROR_RETURN(cbxi_slictcam_init());
    return(cbx_lag_init());
}

/*
 *  Function : drv_avgr_trunk_create
 *
 *  Purpose :
 *      Create a trunk group.
 *
 *  Parameters :
 *      unit    :  RoboSwitch unit number.
 *      flag    :  trunk flags.
 *      tid     :  trunk id.
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int
drv_avgr_trunk_create( int unit, uint32 flags, int *tid )
{
    cbx_lag_params_t  lag_params;

    cbx_lag_params_t_init(&lag_params);
    lag_params.lag_mode = cbxLagModeNormal;

    return (cbx_lag_create(&lag_params, (uint32_t *)tid));
}

/*
 *  Function : drv_avgr_trunk_destroy
 *
 *  Purpose :
 *      Destroy a trunk group.
 *
 *  Parameters :
 *      unit    :  RoboSwitch unit number.
 *      tid     :  trunk id.
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int
drv_avgr_trunk_destroy( int unit, int tid )
{
    return (cbx_lag_destroy(tid));
}

/*
 *  Function : drv_avgr_trunk_modify
 *
 *  Purpose :
 *      Add/Delete members from a trunk group.
 *
 *  Parameters :
 *      unit    :  RoboSwitch unit number.
 *      tid     :  trunk id.
 *      gport   :  member gport.
 *      op      :  ADD/DELETE operation.
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int
drv_avgr_trunk_modify( int unit, int tid, int gport, int op )
{
    if(op == DRV_TRUNK_MEMBER_ADD)
    {
      return (cbx_lag_member_add(tid, gport));
    } else if(op == DRV_TRUNK_MEMBER_REMOVE) {
      return (cbx_lag_member_remove(tid, gport));
    }
    return SOC_E_NONE;
}

/*
 *  Function : drv_avgr_trunk_get
 *
 *  Purpose :
 *      Get the member ports of a trunk group.
 *
 *  Parameters :
 *      unit    :  RoboSwitch unit number.
 *      tid     :  trunk id.
 *      bmp     :  trunk member port bitmap.
 *      flag    :  trunk flag.
 *      hash_op :  trunk hash seed.
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int
drv_avgr_trunk_get( int unit, int tid, soc_pbmp_t *bmp,
                   uint32 flag, uint32 *hash_op )
{
    cbx_lag_member_info_t   lag_info;
    cbx_portid_t            p_array[CBX_MAX_PORT_PER_UNIT];
    soc_pbmp_t              temp = 0;
    int                     rv = SOC_E_NONE;
    int                     pindex = 0;

    cbx_lag_params_t_init( &lag_info.lag_params);
    lag_info.max_portid = CBX_MAX_PORT_PER_UNIT;
    lag_info.portid_array = p_array;
    SOC_PBMP_CLEAR(*bmp);
    SOC_PBMP_CLEAR(*hash_op);

    rv = cbx_lag_member_get(tid, &lag_info);

    if (SOC_SUCCESS(rv))
    {
        for(pindex=0;pindex<lag_info.valid_portid;pindex++)
        {
            SOC_PBMP_PORT_ADD(temp,lag_info.portid_array[pindex]);
        }
        SOC_PBMP_WORD_SET(*bmp, 0, temp);
    }
    return rv;
}

/*
 *  Function : drv_avgr_trunk_psc_get
 *
 *  Purpose :
 *      Get the PSC flags of a trunk group.
 *
 *  Parameters :
 *      unit    :  RoboSwitch unit number.
 *      tid     :  trunk id.
 *      psc     :  psc flags.
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int
drv_avgr_trunk_psc_get( int unit, int tid, int *psc )
{
    cbx_lag_psc_t lag_psc;
    int   rv = SOC_E_NONE;
    unsigned int pkt_type;
    int  psc_out = 0;

    cbx_lag_psc_t_init (&lag_psc);

    /* Clear psc flags */
    *psc = 0;

    /* Retrieve PSC flags for all packet types */
    for(pkt_type=cbxPacketIpv4;pkt_type<=cbxPacketOther;pkt_type++) {
        lag_psc.pkt_type = pkt_type;
        rv =  cbx_lag_psc_get(tid, &lag_psc);
        if (!SOC_SUCCESS(rv)) {
            LOG_ERROR(BSL_LS_SOC_TRUNK,
                  (BSL_META("%s: PSC get for packet type=%d failed\n"),
                  FUNCTION_NAME(), pkt_type));
        }

        psc_out |= lag_psc.flags;

        /* Clear previous flags */
        lag_psc.flags = 0;
    }

    SOC_IF_ERROR_RETURN(_drv_avgr_trunk_fsal2drv_psc_convert(
                                             psc_out, psc));
    return rv;
}

/*
 *  Function : drv_avgr_trunk_psc_set
 *
 *  Purpose :
 *      Set the PSC flags for a trunk group.
 *
 *  Parameters :
 *      unit    :  RoboSwitch unit number.
 *      tid     :  trunk id.
 *      psc     :  psc flags.
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int
drv_avgr_trunk_psc_set( int unit, int tid, int psc )
{
    cbx_lag_psc_t lag_psc;
    int   rv = SOC_E_NONE;
    unsigned int pkt_type;
    int   psc_out = 0;

    cbx_lag_psc_t_init (&lag_psc);

    SOC_IF_ERROR_RETURN(_drv_avgr_trunk_drv2fsal_psc_convert(psc, &psc_out));

    /* Set PSC flags for all packet types */
    for(pkt_type=cbxPacketIpv4;pkt_type<=cbxPacketOther;pkt_type++) {
        lag_psc.pkt_type = pkt_type;
        rv = _drv_avgr_trunk_psc_parse(psc_out, pkt_type, &(lag_psc.flags));
        if (SOC_SUCCESS(rv)) {
            /* Set PSC flags for this pkt type */
            rv =  cbx_lag_psc_set(tid, &lag_psc);
            if (!SOC_SUCCESS(rv)) {
                LOG_ERROR(BSL_LS_SOC_TRUNK,
                      (BSL_META("%s: PSC set for packet type=%d failed\n"),
                      FUNCTION_NAME(), pkt_type));
            }
        }

        /* Clear previous flags */
        lag_psc.flags = 0;
    }

    return rv;
}
