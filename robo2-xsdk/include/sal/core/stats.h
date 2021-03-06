/*
 * $Id: stats.h, Exp $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:     stats.h
 * Purpose:    
 *
 * cpu occupation stats routines
 *
 */

#ifndef _SAL_STATS_H
#define _SAL_STATS_H

#include <sal/types.h>

typedef struct {
	uint64 user;
	uint64 kernel;
	uint64 idle;
	uint64 total;
} sal_cpu_stats_t;

extern int sal_cpu_stats_get(sal_cpu_stats_t* cpu_stats);

#endif    /* !_SAL_STATS_H */
