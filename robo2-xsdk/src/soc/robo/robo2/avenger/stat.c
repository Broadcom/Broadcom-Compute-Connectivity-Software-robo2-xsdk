/*
 * $Id: stat.c $
 *
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/robo_stat.h>
#include <fsal_int/shared_types.h>
#include <fsal/stat.h>
#include <avenger_service.h>

#define MAX_CBX_STAT 110

int snmp_sdk_cbx_map[]={
0,
1,
2,
3,
4,
5,
9,
10,
11,
12,
13,
14,
42,
MAX_CBX_STAT,
MAX_CBX_STAT,
46,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
17,
18,
19,
20,
21,
22,
23,
24,
25,
26,
27,
32,
33,
34,
35,
36,
37,
38,
39,
40,
41,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
51,
MAX_CBX_STAT,
MAX_CBX_STAT,
56,
MAX_CBX_STAT,
61,
55,
60,
6,
7,
15,
16,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
70,
71,
72,
73,
74,
75,
76,
77,
78,
80,
81,
82,
83,
84,
85,
86,
87,
88,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT,
MAX_CBX_STAT
};


int drv_avgr_counter_thread(int unit,uint32 thread_op,uint32 flags,
                           int interval,soc_pbmp_t bmp) {
    int rv;
    rv = cbx_stat_init();
    return rv;
}

int drv_avgr_counter_set(int unit,soc_pbmp_t bmp,uint32 counter_type,
                        uint64 val) {
    int rv;
	cbx_portid_t  port;

    if (counter_type == DRV_SNMP_VAL_COUNT) {
        PBMP_ITER(bmp, port){
            rv = cbx_port_stat_clear(port);
            if (!SOC_SUCCESS(rv))
            {
                LOG_ERROR(BSL_LS_SOC_STAT,
                    (BSL_META("%s:Failed to clear stats for port = %d\n"),
                          FUNCTION_NAME(), port));
                return rv;
            }
        }
    } else {
	     rv = SOC_E_UNAVAIL;
    }
	return rv;
}

int drv_avgr_counter_get(int unit, uint32 port, uint32 counter_type,
                        int sync_hw, uint64 *val) {
    int rv, type = MAX_CBX_STAT;
    type = snmp_sdk_cbx_map[counter_type];
    rv = cbx_port_stat_get(port, type, val);
    return rv;
}

int drv_avgr_counter_reset(int unit) {
    int rv;
    rv = cbx_stat_reset();
    return rv;
}
