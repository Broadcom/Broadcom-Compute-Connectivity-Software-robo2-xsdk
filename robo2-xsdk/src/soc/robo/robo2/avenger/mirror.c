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
#include <fsal/mirror.h>
#include <avenger_service.h>

typedef struct drv_mirror_dest_info_s {
    char                  in_use;/* Mirror entry use/free flag */
    cbx_mirror_destid_t   mirror_destid;/* Mirror destination ID */
    cbx_mirrorid_t        port_ingrmid; /* Port ingress Mirror ID */
    cbx_mirrorid_t        port_egrmid; /* Port egress Mirror ID */
    cbx_mirrorid_t        vlan_ingrmid; /* VLAN ingress Mirror ID */
    cbx_mirrorid_t        vlan_egrmid; /* VLAN egress Mirror ID */
} drv_mirror_dest_info_t;

drv_mirror_dest_info_t     mirror_dest_info[CBX_MIRROR_DEST_MAX];

extern int cbx_mirror_init( void );

/*
 *  Function : _drv_avgr_mirror_create
 *
 *  Purpose :
 *      Create a mirror group.
 *
 *  Parameters :
 *      flags           :  Mirror group flags.
 *      dest_id         :  Mirror destination identifier.
 *      mirrorid        :  Pointer to mirror identifier.
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int
_drv_avgr_mirror_create(uint32 flags, int dest_id, cbx_mirrorid_t  *mirror_id)
{
    cbx_mirror_params_t      mirror_params;

    cbx_mirror_params_t_init( &mirror_params );
    mirror_params.flags = flags;
    return( cbx_mirror_create ( &mirror_params, dest_id, mirror_id));
}

/*
 *  Function : _drv_avgr_mirror_destroy
 *
 *  Purpose :
 *      Destroy a mirror group.
 *
 *  Parameters :
 *      mirrorid        :  Mirror identifier.
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int
_drv_avgr_mirror_destroy(int  mirrorid)
{
    return (cbx_mirror_destroy ( mirrorid ));
}

/*
 *  Function : drv_avgr_mirror_init
 *
 *  Purpose :
 *      Initialize mirror module.
 *
 *  Parameters :
 *      unit    :  RoboSwitch unit number.
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */

int
drv_avgr_mirror_init( int unit )
{
    drv_mirror_dest_info_t   *dest_info;
    int id = 0;

    for (id = 0;id<CBX_MIRROR_DEST_MAX;id++)
    {
        dest_info = &mirror_dest_info[id];
        dest_info->in_use = FALSE;
        dest_info->mirror_destid = CBX_MIRROR_DEST_INVALID;
        dest_info->port_ingrmid = CBX_MIRROR_INVALID;
        dest_info->port_egrmid  = CBX_MIRROR_INVALID;
        dest_info->vlan_ingrmid = CBX_MIRROR_INVALID;
        dest_info->vlan_egrmid  = CBX_MIRROR_INVALID;
    }
    return(cbx_mirror_init());
}

/*
 *  Function : drv_avgr_mirror_dest_create
 *
 *  Purpose :
 *      Create a mirror destination.
 *
 *  Parameters :
 *      unit          :  RoboSwitch unit number.
 *      mirror_dest   :  Pointer to mirror destination structure.
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int
drv_avgr_mirror_dest_create(int unit, drv_mirror_dest_t *mirror_dest)
{
    cbx_mirror_dest_t   dest;
    cbx_mirror_destid_t mirror_destid;
    drv_mirror_dest_info_t   *dest_info;
    int rv = SOC_E_NONE;

    cbx_mirror_dest_t_init( &dest );
    mirror_dest->mirror_destid = CBX_MIRROR_DEST_INVALID;

    dest.flags = CBX_MIRROR_DEST_ENCAP;
    dest.port = mirror_dest->port;

    rv = cbx_mirror_dest_create ( &dest, &mirror_destid );

    if (SOC_SUCCESS(rv))
    {
        dest_info = &mirror_dest_info[mirror_destid];
        dest_info->in_use = TRUE;
        dest_info->mirror_destid = mirror_destid;
        dest_info->port_ingrmid = CBX_MIRROR_INVALID;
        dest_info->port_egrmid  = CBX_MIRROR_INVALID;
        dest_info->vlan_ingrmid = CBX_MIRROR_INVALID;
        dest_info->vlan_egrmid  = CBX_MIRROR_INVALID;
        mirror_dest->mirror_destid = mirror_destid;
    }

    return rv;
}

/*
 *  Function : drv_avgr_mirror_dest_destroy
 *
 *  Purpose :
 *      Destroy a mirror destination.
 *
 *  Parameters :
 *      unit            :  RoboSwitch unit number.
 *      dest_id         :  Mirror destination identifier.
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int
drv_avgr_mirror_dest_destroy(int unit, int  destid)
{
    drv_mirror_dest_info_t   *dest_info;
    int rv = SOC_E_NONE;

    dest_info = &mirror_dest_info[destid];

    if ( dest_info->in_use == FALSE )
    {
        return SOC_E_PARAM;
    }

    if ( dest_info->port_ingrmid != CBX_MIRROR_INVALID )
    {
        rv = _drv_avgr_mirror_destroy(dest_info->port_ingrmid);
        if (!SOC_SUCCESS(rv))
        {
            LOG_ERROR(BSL_LS_SOC_MIRROR,
                (BSL_META("%s:Failed to destroy port ingress mirror group = %d\n"),
                      FUNCTION_NAME(), dest_info->port_ingrmid));
            return rv;
        }
    }

    if ( dest_info->port_egrmid != CBX_MIRROR_INVALID )
    {
        rv = _drv_avgr_mirror_destroy(dest_info->port_egrmid);
        if (!SOC_SUCCESS(rv))
        {
            LOG_ERROR(BSL_LS_SOC_MIRROR,
                (BSL_META("%s: Failed to destroy port egress mirror group = %d\n"),
                      FUNCTION_NAME(), dest_info->port_egrmid));
            return rv;
        }
    }

    if ( dest_info->vlan_ingrmid != CBX_MIRROR_INVALID )
    {
        rv = _drv_avgr_mirror_destroy(dest_info->vlan_ingrmid);
        if (!SOC_SUCCESS(rv))
        {
            LOG_ERROR(BSL_LS_SOC_MIRROR,
               (BSL_META("%s: Failed to destroy vlan ingress mirror group = %d\n"),
                      FUNCTION_NAME(), dest_info->vlan_ingrmid));
            return rv;
        }
    }

    if ( dest_info->vlan_egrmid != CBX_MIRROR_INVALID )
    {
        rv = _drv_avgr_mirror_destroy(dest_info->vlan_egrmid);
        if (!SOC_SUCCESS(rv))
        {
            LOG_ERROR(BSL_LS_SOC_MIRROR,
                (BSL_META("%s: Failed to destroy vlan egress mirror group = %d\n"),
                      FUNCTION_NAME(), dest_info->vlan_egrmid));
            return rv;
        }
    }

    dest_info->in_use = FALSE;
    dest_info->mirror_destid = CBX_MIRROR_DEST_INVALID;
    dest_info->port_ingrmid = CBX_MIRROR_INVALID;
    dest_info->port_egrmid  = CBX_MIRROR_INVALID;
    dest_info->vlan_ingrmid = CBX_MIRROR_INVALID;
    dest_info->vlan_egrmid  = CBX_MIRROR_INVALID;

    return (cbx_mirror_dest_destroy ( destid ));
}

/*
 *  Function : drv_avgr_mirror_port_add
 *
 *  Purpose :
 *      Add mirroring to a port.
 *
 *  Parameters :
 *      unit            :  RoboSwitch unit number.
 *      portid          :  Port identifier.
 *      flags           :  Port mirroring flags.
 *      dest_id         :  Mirror destination identifier.
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int
drv_avgr_mirror_port_add(int unit, uint32 portid, uint32 flags, int dest_id )
{
    cbx_mirrorid_t           mirrorid;
    cbx_mirror_port_params_t mirror_port_params;
    drv_mirror_dest_info_t   *dest_info;
    int rv = SOC_E_NONE;

    dest_info = &mirror_dest_info[dest_id];

    if ( dest_info->in_use == FALSE )
    {
        return SOC_E_PARAM;
    }

    if ( flags & DRV_PORT_MIRROR_INGRESS_ENABLE )
    {
        if ( dest_info->port_ingrmid == CBX_MIRROR_INVALID )
        {
            rv = _drv_avgr_mirror_create(CBX_MIRROR_MODE_PORT_INGRESS,
                                         dest_id, &mirrorid);
            if (!SOC_SUCCESS(rv))
            {
                LOG_ERROR(BSL_LS_SOC_MIRROR,
                         (BSL_META("%s: Failed to create mirror group for"
                          "ingress mirroring for port =%d\n"),
                          FUNCTION_NAME(), portid));
                return rv;
            }
            dest_info->port_ingrmid = mirrorid;
        } else {
            mirrorid = dest_info->port_ingrmid;
        }

        cbx_mirror_port_params_t_init(&mirror_port_params);
        mirror_port_params.mode = cbxMirrorModeIngress;
        rv = cbx_mirror_port_add ( mirrorid,
                                   portid,
                                   &mirror_port_params );
        if (!SOC_SUCCESS(rv))
        {
            LOG_ERROR(BSL_LS_SOC_MIRROR,
                     (BSL_META("%s: Failed to add port =%d to mirror group=%d"
                      "for ingress mirroring\n"),
                      FUNCTION_NAME(), portid, mirrorid));
        }
    }

    if ( flags & DRV_PORT_MIRROR_EGRESS_ENABLE )
    {
        if ( dest_info->port_egrmid == CBX_MIRROR_INVALID )
        {
            rv = _drv_avgr_mirror_create(CBX_MIRROR_MODE_PORT_EGRESS,
                                         dest_id, &mirrorid);
            if (!SOC_SUCCESS(rv))
            {
                LOG_ERROR(BSL_LS_SOC_MIRROR,
                         (BSL_META("%s: Failed to create mirror group for"
                          "egress mirroring for port =%d\n"),
                          FUNCTION_NAME(), portid ));
                return rv;
            }
            dest_info->port_egrmid = mirrorid;
        } else {
            mirrorid = dest_info->port_egrmid;
        }

        cbx_mirror_port_params_t_init(&mirror_port_params);
        mirror_port_params.mode = cbxMirrorModeEgress;
        rv = cbx_mirror_port_add ( mirrorid,
                                   portid,
                                   &mirror_port_params );
        if (!SOC_SUCCESS(rv))
        {
            LOG_ERROR(BSL_LS_SOC_MIRROR,
                     (BSL_META("%s: Failed to add port =%d to mirror group=%d"
                      "for egress mirroring\n"),
                      FUNCTION_NAME(), portid, mirrorid));
        }
    }

    return rv;
}

/*
 *  Function : drv_avgr_mirror_port_delete
 *
 *  Purpose :
 *      Delete mirroring to a port.
 *
 *  Parameters :
 *      unit            :  RoboSwitch unit number.
 *      portid          :  Port identifier.
 *      flags           :  Port mirroring flags.
 *      dest_id         :  Mirror destination identifier.
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int
drv_avgr_mirror_port_delete(int unit, uint32 portid, uint32 flags, int dest_id )
{
    cbx_mirrorid_t           mirrorid;
    drv_mirror_dest_info_t   *dest_info;
    int rv = SOC_E_NONE;

    dest_info = &mirror_dest_info[dest_id];

    if ( dest_info->in_use == FALSE )
    {
        return SOC_E_PARAM;
    }

    if ( flags & DRV_PORT_MIRROR_INGRESS_ENABLE )
    {
        if ( dest_info->port_ingrmid != CBX_MIRROR_INVALID )
        {
            mirrorid = dest_info->port_ingrmid;
        }

        rv = cbx_mirror_port_remove ( mirrorid, portid );

        if (!SOC_SUCCESS(rv))
        {
            LOG_ERROR(BSL_LS_SOC_MIRROR,
                     (BSL_META("%s: Failed to remove port =%d from mirror"
                      "group=%d for ingress mirroring\n"),
                      FUNCTION_NAME(), portid, mirrorid));
        }
    }

    if ( flags & DRV_PORT_MIRROR_EGRESS_ENABLE )
    {
        if ( dest_info->port_egrmid != CBX_MIRROR_INVALID )
        {
            mirrorid = dest_info->port_egrmid;
        }

        rv = cbx_mirror_port_remove ( mirrorid, portid );

        if (!SOC_SUCCESS(rv))
        {
            LOG_ERROR(BSL_LS_SOC_MIRROR,
                     (BSL_META("%s: Failed to remove port =%d from mirror"
                      "group=%d for egress mirroring\n"),
                      FUNCTION_NAME(), portid, mirrorid));
        }
    }

    return rv;
}

/*
 *  Function : drv_avgr_mirror_vlan_add
 *
 *  Purpose :
 *      Add mirroring to a vlan.
 *
 *  Parameters :
 *      unit            :  RoboSwitch unit number.
 *      vlanid          :  Vlan identifier.
 *      flags           :  Vlan mirroring flags.
 *      dest_id         :  Mirror destination identifier.
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int
drv_avgr_mirror_vlan_add(int unit, uint32 vlanid, uint32 flags, int dest_id )
{
    cbx_mirrorid_t           mirrorid;
    cbx_mirror_vlan_params_t mirror_vlan_params;
    drv_mirror_dest_info_t   *dest_info;
    int rv = SOC_E_NONE;

    dest_info = &mirror_dest_info[dest_id];

    if ( dest_info->in_use == FALSE )
    {
        return SOC_E_PARAM;
    }

    if ( flags & DRV_PORT_MIRROR_INGRESS_ENABLE )
    {
        if ( dest_info->vlan_ingrmid == CBX_MIRROR_INVALID )
        {
            rv = _drv_avgr_mirror_create(CBX_MIRROR_MODE_VLAN_INGRESS,
                                         dest_id, &mirrorid);
            if (!SOC_SUCCESS(rv))
            {
                LOG_ERROR(BSL_LS_SOC_MIRROR,
                         (BSL_META("%s: Failed to create mirror group for"
                          "ingress mirroring for port =%d\n"),
                          FUNCTION_NAME(), vlanid));
                return rv;
            }
            dest_info->vlan_ingrmid = mirrorid;
        } else {
            mirrorid = dest_info->vlan_ingrmid;
        }

        cbx_mirror_vlan_params_t_init(&mirror_vlan_params);
        mirror_vlan_params.mode = cbxMirrorModeIngress;
        rv = cbx_mirror_vlan_add ( mirrorid,
                                   vlanid,
                                   &mirror_vlan_params );
        if (!SOC_SUCCESS(rv))
        {
            LOG_ERROR(BSL_LS_SOC_MIRROR,
                     (BSL_META("%s: Failed to add vlan =%d to mirror group=%d"
                      "for ingress mirroring\n"),
                      FUNCTION_NAME(), vlanid, mirrorid));
        }
    }

    if ( flags & DRV_PORT_MIRROR_EGRESS_ENABLE )
    {
        if ( dest_info->vlan_egrmid == CBX_MIRROR_INVALID )
        {
            rv = _drv_avgr_mirror_create(CBX_MIRROR_MODE_VLAN_EGRESS,
                                         dest_id, &mirrorid);
            if (!SOC_SUCCESS(rv))
            {
                LOG_ERROR(BSL_LS_SOC_MIRROR,
                         (BSL_META("%s: Failed to create mirror group for"
                          "egress mirroring for vlan =%d\n"),
                          FUNCTION_NAME(), vlanid));
                return rv;
            }
            dest_info->vlan_egrmid = mirrorid;
        } else {
            mirrorid = dest_info->vlan_egrmid;
        }

        cbx_mirror_vlan_params_t_init(&mirror_vlan_params);
        mirror_vlan_params.mode = cbxMirrorModeEgress;
        rv = cbx_mirror_vlan_add ( mirrorid,
                                   vlanid,
                                   &mirror_vlan_params );
        if (!SOC_SUCCESS(rv))
        {
            LOG_ERROR(BSL_LS_SOC_MIRROR,
                     (BSL_META("%s: Failed to add vlan =%d to mirror group=%d"
                      "for egress mirroring\n"),
                      FUNCTION_NAME(), vlanid, mirrorid));
        }
    }

    return rv;
}

/*
 *  Function : drv_avgr_mirror_vlan_delete
 *
 *  Purpose :
 *      Delete mirroring to a vlan.
 *
 *  Parameters :
 *      unit            :  RoboSwitch unit number.
 *      vlanid          :  Vlan identifier.
 *      flags           :  Vlan mirroring flags.
 *      dest_id         :  Mirror destination identifier.
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int
drv_avgr_mirror_vlan_delete(int unit, uint32 vlanid, uint32 flags, int dest_id )
{
    cbx_mirrorid_t           mirrorid;
    drv_mirror_dest_info_t   *dest_info;
    int rv = SOC_E_NONE;

    dest_info = &mirror_dest_info[dest_id];

    if ( dest_info->in_use == FALSE )
    {
        return SOC_E_PARAM;
    }

    if ( flags & DRV_PORT_MIRROR_INGRESS_ENABLE )
    {
        if ( dest_info->vlan_ingrmid != CBX_MIRROR_INVALID )
        {
            mirrorid = dest_info->vlan_ingrmid;
        }

        rv = cbx_mirror_vlan_remove ( mirrorid, vlanid );

        if (!SOC_SUCCESS(rv))
        {
            LOG_ERROR(BSL_LS_SOC_MIRROR,
                     (BSL_META("%s: Failed to remove vlan =%d from mirror"
                      "group=%d for ingress mirroring\n"),
                      FUNCTION_NAME(), vlanid, mirrorid));
        }
    }

    if ( flags & DRV_PORT_MIRROR_EGRESS_ENABLE )
    {
        if ( dest_info->vlan_egrmid != CBX_MIRROR_INVALID )
        {
            mirrorid = dest_info->vlan_egrmid;
        }

        rv = cbx_mirror_vlan_remove ( mirrorid, vlanid );

        if (!SOC_SUCCESS(rv))
        {
            LOG_ERROR(BSL_LS_SOC_MIRROR,
                     (BSL_META("%s: Failed to remove vlan =%d from mirror"
                      "group=%d for egress mirroring\n"),
                      FUNCTION_NAME(), vlanid, mirrorid));
        }
    }

    return rv;
}

/*
 *  Function : drvi_avgr_mirror_get_mirror_id
 *
 *  Purpose :
 *      Get mirror id from destination id.
 *
 *  Parameters :
 *      unit            :  RoboSwitch unit number.
 *      dest_id         :  Mirror destination identifier.
 *      mirror_id       :  cbx mirror id to be returned
 *
 *  Return :
 *      SOC_E_NONE
 *      SOC_E_XXX
 *
 *  Note :
 */
int drvi_avgr_mirror_get_mirror_id(int unit, int destid, uint32 *mirror_id)
{
    return SOC_E_UNAVAIL;
}
