/*
 * $Id: bcm-knet-kcom.h,v 1.1 Broadcom SDK $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Provides a kcom interface using User/Kernel proxy services
 */

#ifndef __BCM_KNET_KCOM_H__
#define __BCM_KNET_KCOM_H__

extern void *
bcm_knet_kcom_open(char *name);

extern int
bcm_knet_kcom_close(void *handle);

extern int
bcm_knet_kcom_msg_send(void *handle, void *msg,
                       unsigned int len, unsigned int bufsz);

extern int
bcm_knet_kcom_msg_recv(void *handle, void *msg,
                       unsigned int bufsz);

#endif /* __BCM_KNET_KCOM_H__ */
