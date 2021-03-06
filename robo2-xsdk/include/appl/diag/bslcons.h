/*
 * $Id: $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Broadcom System Log State
 */

#ifndef _DIAG_BSLCONS_H
#define _DIAG_BSLCONS_H

int
bslcons_init(void);

int
bslcons_is_enabled(void);

int
bslcons_enable(int enable);

int
bslcons_thread_check_is_enabled(void);

int
bslcons_thread_check_enable(int enable);
#endif /* !_DIAG_BSLCONS_H */
