/*
 * $Id: $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Broadcom System Log Trace
 */

#ifndef _DIAG_BSLTRACE_H
#define _DIAG_BSLTRACE_H

int
bsltrace_init(void);

int
bsltrace_clear(void);

int
bsltrace_config_set(int nentry, int size);

int
bsltrace_config_get(int *nentry, int *size);

int
bsltrace_print(int entries);

#endif /* !_DIAG_BSLTRACE_H */
