/*
 * $Id: storm.c $
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
#include <gex/robo_gex.h>
#include "robo_sf3.h"


#define _DRV_SF3_PKT_TYPE_UCAST_HIT     (0x1)
#define _DRV_SF3_PKT_TYPE_MCAST_HIT     (0x2)
#define _DRV_SF3_PKT_TYPE_RSV_MCAST     (0x4)
#define _DRV_SF3_PKT_TYPE_BCAST         (0x8)
#define _DRV_SF3_PKT_TYPE_MLF           (0x10)
#define _DRV_SF3_PKT_TYPE_DLF           (0x20)




/*
 *  Function : _drv_sf3_storm_control_type_enable_set
 *
 *  Purpose :
 *      Set the status and types of storm control.
 *
 *  Parameters :
 *      unit       :  RoboSwitch unit number.
 *      port       :  port number.
 *      type       :  storm control types.
 *      enable     :  status of the storm control types.
 *
 *  Return :
 *      SOC_E_NONE
 *
 *  Note :
 */
static int
_drv_sf3_storm_control_type_enable_set(int unit, uint32 port,
    uint32 type, uint32 enable)
{
    uint32  temp = 0;
    uint32  reg_value;

    SOC_IF_ERROR_RETURN(REG_READ_BC_SUP_RATECTRL_1_Pr
            (unit, port, &reg_value));

    soc_BC_SUP_RATECTRL_1_Pr_field_get(unit, &reg_value,
        PKT_MSK1f, &temp);

    /* Packet types */
    if (enable) {
        if (type & DRV_STORM_CONTROL_BCAST) {
            temp |= _DRV_SF3_PKT_TYPE_BCAST;
        }
        if (type & DRV_STORM_CONTROL_MCAST) {
            temp |= (_DRV_SF3_PKT_TYPE_MCAST_HIT | _DRV_SF3_PKT_TYPE_MLF);
        }
        if (type & DRV_STORM_CONTROL_DLF) {
            temp |= _DRV_SF3_PKT_TYPE_DLF;
        }
        if (type & DRV_STORM_CONTROL_RSV_MCAST) {
            temp |= _DRV_SF3_PKT_TYPE_RSV_MCAST;
        }
        if (type & DRV_STORM_CONTROL_UCAST) {
            temp |= (_DRV_SF3_PKT_TYPE_DLF | _DRV_SF3_PKT_TYPE_UCAST_HIT);
        }
    } else {
        if (type & DRV_STORM_CONTROL_BCAST) {
            temp &= ~(_DRV_SF3_PKT_TYPE_BCAST);
        }
        if (type & DRV_STORM_CONTROL_MCAST) {
            temp &= ~(_DRV_SF3_PKT_TYPE_MCAST_HIT | _DRV_SF3_PKT_TYPE_MLF);
        }
        if (type & DRV_STORM_CONTROL_DLF) {
            temp &= ~(_DRV_SF3_PKT_TYPE_DLF);
        }
        if (type & DRV_STORM_CONTROL_RSV_MCAST) {
            temp &= ~(_DRV_SF3_PKT_TYPE_RSV_MCAST);
        }
        if (type & DRV_STORM_CONTROL_UCAST) {
        /* _DRV_SF3_PKT_TYPE_DLF is cleared only if DRV_STORM_CONTROL_DLF not
	 * set */
            temp &= ~(_DRV_SF3_PKT_TYPE_UCAST_HIT);
        }
    }

    soc_BC_SUP_RATECTRL_1_Pr_field_set(unit, &reg_value,
        PKT_MSK1f, &temp);

    SOC_IF_ERROR_RETURN(REG_WRITE_BC_SUP_RATECTRL_1_Pr
            (unit, port, &reg_value));

    return SOC_E_NONE;
}

/*
 *  Function : _drv_sf3_storm_control_type_enable_get
 *
 *  Purpose :
 *      Get the status and types of storm control.
 *
 *  Parameters :
 *      unit       :  RoboSwitch unit number.
 *      port       :  port number.
 *      type       :  (OUT) storm control types.
 *
 *  Return :
 *      SOC_E_NONE
 *
 *  Note :
 */
static int
_drv_sf3_storm_control_type_enable_get(int unit, uint32 port,
    uint32 *type)
{
    uint32  temp = 0;
    uint32  reg_value;

    SOC_IF_ERROR_RETURN(REG_READ_BC_SUP_RATECTRL_1_Pr
            (unit, port, &reg_value));

    *type = 0;
    SOC_IF_ERROR_RETURN(soc_BC_SUP_RATECTRL_1_Pr_field_get
        (unit, &reg_value, PKT_MSK1f, &temp));
    if (temp & _DRV_SF3_PKT_TYPE_BCAST) {
        *type |= DRV_STORM_CONTROL_BCAST;
    }
    if ((temp & _DRV_SF3_PKT_TYPE_MCAST_HIT) &&
        (temp & _DRV_SF3_PKT_TYPE_MLF)) {
        *type |= DRV_STORM_CONTROL_MCAST;
    }
    if (temp & _DRV_SF3_PKT_TYPE_DLF) {
        *type |= DRV_STORM_CONTROL_DLF;
    }
    if (temp & _DRV_SF3_PKT_TYPE_RSV_MCAST) {
        *type |= DRV_STORM_CONTROL_RSV_MCAST;
    }
    if ((temp & _DRV_SF3_PKT_TYPE_DLF) &&
        (temp & _DRV_SF3_PKT_TYPE_UCAST_HIT)) {
        *type |= DRV_STORM_CONTROL_UCAST;
    }

    return SOC_E_NONE;
}

/*
 *  Function : drv_sf3_storm_control_enable_set
 *
 *  Purpose :
 *      Set the status for global storm control of selected port.
 *
 *  Parameters :
 *      unit       :  RoboSwitch unit number.
 *      port       :  port number.
 *      enable     :  status of the storm control (global).
 *
 *  Return :
 *      SOC_E_NONE
 *
 *  Note :
 */
int
drv_sf3_storm_control_enable_set(int unit, uint32 port, uint8 enable)
{
    uint32  temp;
    uint32  reg_value;

    LOG_INFO(BSL_LS_SOC_PORT, \
             (BSL_META_U(unit, \
                         "drv_sf3_storm_control_enable_set: \
                         unit = %d, port = %d, %sable\n"), unit, port, (enable) ? "en" : "dis"));

    /* Enable Ingress rate control bit */
    SOC_IF_ERROR_RETURN(REG_READ_BC_SUP_RATECTRL_Pr
            (unit, port, &reg_value));

    temp = enable;
    SOC_IF_ERROR_RETURN(soc_BC_SUP_RATECTRL_Pr_field_set
        (unit, &reg_value, EN_BUCKET1f, &temp));

    SOC_IF_ERROR_RETURN(REG_WRITE_BC_SUP_RATECTRL_Pr
            (unit, port, &reg_value));

    return SOC_E_NONE;
}

/*
 *  Function : drv_sf3_storm_control_enable_get
 *
 *  Purpose :
 *      Get the status for global storm control of selected port.
 *
 *  Parameters :
 *      unit       :  RoboSwitch unit number.
 *      port       :  port number.
 *      enable     :  (OUT) status of the storm control (global).
 *
 *  Return :
 *      SOC_E_NONE
 *
 *  Note :
 */
int
drv_sf3_storm_control_enable_get(int unit, uint32 port, uint8 *enable)
{
    uint32  temp = 0;
    uint32  reg_value;

    SOC_IF_ERROR_RETURN(REG_READ_BC_SUP_RATECTRL_Pr
            (unit, port, &reg_value));

    SOC_IF_ERROR_RETURN(soc_BC_SUP_RATECTRL_Pr_field_get
        (unit, &reg_value, EN_BUCKET1f, &temp));
    *enable = temp;

    LOG_INFO(BSL_LS_SOC_PORT, \
             (BSL_META_U(unit, \
                         "drv_sf3_storm_control_enable_get: \
                         unit = %d, port = %d, %sable\n"), unit, port, (*enable) ? "en" : "dis"));

    return SOC_E_NONE;
}

/*
 *  Function : drv_sf3_storm_control_set
 *
 *  Purpose :
 *      Set the types and limit value for storm control of selected port.
 *
 *  Parameters :
 *      unit       :  RoboSwitch unit number.
 *      bmp        :  port bitmap.
 *      type       :  types of strom control.
 *      limit      :  limit value of storm control (Kbits).
 *      burst_size :  burst bucket size (Kbits).
 *
 *  Return :
 *      SOC_E_NONE
 *
 *  Note :
 */
int
drv_sf3_storm_control_set(int unit, soc_pbmp_t bmp, uint32 type,
    uint32 limit, uint32 burst_size)
{
    uint32  reg_value, temp;
    uint32  port;
    uint32  disable_type = 0, burst_kbyte;

    LOG_INFO(BSL_LS_SOC_PORT, \
             (BSL_META_U(unit, \
                         "drv_sf3_storm_control_set: \
                         unit = %d, bmp = 0x%x, type = 0x%x, limit = %dK\n"),
              unit, SOC_PBMP_WORD_GET(bmp, 0), type, limit));


    PBMP_ITER(bmp, port) {

        if (limit == 0) {  /* Disable storm suppression type */
            SOC_IF_ERROR_RETURN(_drv_sf3_storm_control_type_enable_set
                (unit, port, type, FALSE));
        } else {
            disable_type = SF3_STORM_CONTROL_PKT_MASK;

            /* Set storm suppression type */
            SOC_IF_ERROR_RETURN(_drv_sf3_storm_control_type_enable_set
                (unit, port, type, TRUE));

            disable_type ^= type;
            if (disable_type != 0) {
                SOC_IF_ERROR_RETURN(_drv_sf3_storm_control_type_enable_set
                    (unit, port, disable_type, FALSE));
            }

            /* Set bucket 1 refresh count */
            SOC_IF_ERROR_RETURN(REG_READ_BC_SUP_RATECTRL_Pr
                    (unit, port, &reg_value));

            /* Burst size */
            if (burst_size) {
                if (burst_size > (500 * 8)) {  /* 500 KB */
                    return SOC_E_PARAM;
                }
                burst_kbyte = (burst_size / 8);
                if (burst_kbyte <= 16) {  /* 16KB */
                    temp = 0;
                } else if (burst_kbyte <= 20) {  /* 20KB */
                    temp = 1;
                } else if (burst_kbyte <= 28) {  /* 28KB */
                    temp = 2;
                } else if (burst_kbyte <= 44) {  /* 44KB */
                    temp = 3;
                } else if (burst_kbyte <= 76) {  /* 76KB */
                    temp = 4;
                } else {  /* else burst_kbyte <= 500 for 500KB */
                    temp = 7;
                }

                SOC_IF_ERROR_RETURN(soc_BC_SUP_RATECTRL_Pr_field_set
                    (unit, &reg_value, BUCKET1_SIZEf, &temp));
            }

            /* Refresh count  (fixed type) */
            if (limit <= 1792) {  /* 64KB ~ 1.792MB */
                temp = ((limit - 1) / 64) + 1;
            } else if (limit <= 100000) {  /* 2MB ~ 100MB */
                temp = (limit / 1000 ) + 27;
            } else if (limit <= 1000000) {  /* 104MB ~ 1000MB */
                temp = (limit / 8000) + 115;
            } else {
                return SOC_E_PARAM;
            }
            SOC_IF_ERROR_RETURN(soc_BC_SUP_RATECTRL_Pr_field_set
                (unit, &reg_value, BUCKET1_REF_CNTf, &temp));
            SOC_IF_ERROR_RETURN(REG_WRITE_BC_SUP_RATECTRL_Pr
                    (unit, port, &reg_value));
        }
    }

    return SOC_E_NONE;
}

/*
 *  Function : drv_sf3_storm_control_get
 *
 *  Purpose :
 *      Get the types and limit value for storm control of selected port.
 *
 *  Parameters :
 *      unit       :  RoboSwitch unit number.
 *      port       :  port number.
 *      type       :  (OUT) types of strom control.
 *      limit      :  (OUT) limit value of storm control (Kbits).
 *      burst_size :  (OUT) burst bucket size (Kbits).
 *
 *  Return :
 *      SOC_E_NONE
 *
 *  Note :
 */
int
drv_sf3_storm_control_get(int unit, uint32 port, uint32 *type,
    uint32 *limit, uint32 *burst_size)
{
    uint32  reg_value, temp = 0;


    SOC_IF_ERROR_RETURN(_drv_sf3_storm_control_type_enable_get
        (unit, port, type));
    if (!(*type)) {
        *limit = 0;
    } else {
        SOC_IF_ERROR_RETURN(REG_READ_BC_SUP_RATECTRL_Pr
                (unit, port, &reg_value));

        SOC_IF_ERROR_RETURN(soc_BC_SUP_RATECTRL_Pr_field_get
            (unit, &reg_value, BUCKET1_REF_CNTf, &temp));
        if (temp <= 28) {
            *limit = temp * 64;
        } else if (temp <= 127) {
            *limit = (temp - 27) * 1000;
        } else if (temp <= 240) {
            *limit = (temp - 115) * 1000 * 8;
        } else {
            return SOC_E_INTERNAL;
        }

        /* Burst size */
        SOC_IF_ERROR_RETURN(soc_BC_SUP_RATECTRL_Pr_field_get
            (unit, &reg_value, BUCKET1_SIZEf, &temp));

        switch (temp) {
            case 0:
                *burst_size = 16 * 8;  /* 16KB */
                break;
            case 1:
                *burst_size = 20 * 8;  /* 20KB */
                break;
            case 2:
                *burst_size = 28 * 8;  /* 28KB */
                break;
            case 3:
                *burst_size = 44 * 8;  /* 44KB */
                break;
            case 4:
                *burst_size = 76 * 8;  /* 76KB */
                break;
            case 6:
            case 7:
                *burst_size = 500 * 8;  /* 500KB */
                break;
            default:
                return SOC_E_INTERNAL;
        }
    }

    LOG_INFO(BSL_LS_SOC_PORT, \
             (BSL_META_U(unit, \
                         "drv_sf3_storm_control_get: \
                         unit = %d, port = %d, type = 0x%x, limit = %dK\n"),
              unit, port, *type, *limit));

    return SOC_E_NONE;
}