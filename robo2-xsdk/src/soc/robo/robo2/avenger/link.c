/*
 * $Id: link.c $
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
#include <fsal/link.h>
#include <fsal/error.h>
#include <bcm/link.h>
#include <avenger_service.h>
#include <shared/bsl.h>

int drv_avgr_linkscan_init(int unit) {
    int rv = CBX_E_NONE;
    cbx_link_scan_params_t link_scan_params;

    link_scan_params.flags = 0;
    link_scan_params.usec = 100000; /* 100 ms */

    rv = cbx_link_scan_init(&link_scan_params);
    if(rv < 0) {
       LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,"%s failed with err:%d\n"),__func__, rv));
    }
    return rv;
}

int drv_avgr_linkscan_detach(int unit) {
    int rv = CBX_E_NONE;
    cbx_link_scan_params_t link_scan_params;

    link_scan_params.flags = 0;
    link_scan_params.usec = 0;

    rv = cbx_link_scan_detach(&link_scan_params);
    if(rv < 0) {
       LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,"%s failed with err:%d\n"),__func__, rv));
    }
    return rv;
}

int drv_avgr_linkscan_register(int unit, drv_link_scan_callback_t cb) {
    int rv = CBX_E_NONE;
    cbx_link_scan_info_t info;

    info.name = "BCM_Link_scan";
    rv = cbx_link_scan_register(info, (cbx_link_scan_callback_t) cb);

    if(rv < 0) {
       LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,"%s failed with err:%d\n"),__func__, rv));
    }
    return rv;
}

int drv_avgr_linkscan_unregister(int unit, drv_link_scan_callback_t cb) {
    int rv = CBX_E_NONE;

    rv = cbx_link_scan_unregister((cbx_link_scan_callback_t)cb);

    if(rv < 0) {
       LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,"%s failed with err:%d\n"),__func__, rv));
    }
    return rv;
}

int drv_avgr_linkscan_enable_get(int unit, int *us) {
    int rv = CBX_E_NONE;
    cbx_link_scan_params_t link_scan_params;

    rv = cbx_link_scan_enable_get(&link_scan_params);
    if(rv < 0) {
       LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,"%s failed with err:%d\n"),__func__, rv));
       *us = 0;
    }

    *us = link_scan_params.usec;
    return rv;
}

int drv_avgr_linkscan_enable_set(int unit, int us) {
    int rv = CBX_E_NONE;
    cbx_link_scan_params_t link_scan_params;

    link_scan_params.flags = 0;
    link_scan_params.usec = us;
    rv = cbx_link_scan_enable_set(&link_scan_params);

    if(rv < 0) {
       LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,"%s failed with err:%d\n"),__func__, rv));
    }
    return rv;
}

int drv_avgr_linkscan_dump(int unit) {
    int rv = CBX_E_NONE;

    rv = cbx_link_scan_dump();
    if(rv < 0) {
       LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,"%s failed with err:%d\n"),__func__, rv));
    }
    return rv;
}
