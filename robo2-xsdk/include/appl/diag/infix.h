/*
 * $Id: infix.h,v 1.4 Broadcom SDK $
 *
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef _DIAG_INFIX_H
#define _DIAG_INFIX_H

#include <sal/types.h>

#define INFIX_ASTK_SIZE		16
#define INFIX_OSTK_SIZE		6
#define INFIX_TYPE		uint32

int infix_eval(char *s, INFIX_TYPE *v);

#endif /* _DIAG_INFIX_H */
