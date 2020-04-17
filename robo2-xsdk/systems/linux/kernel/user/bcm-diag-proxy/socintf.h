/*
 * $Id: socintf.h,v 1.3 Broadcom SDK $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        socintf.h
 * Purpose:     
 */

#ifndef   _SOCINTF_H_
#define   _SOCINTF_H_

extern int setup_socket(int port);
extern int wait_for_cnxn(int sockfd);
extern int conn(char* servier, int port); 

#endif /* _SOCINTF_H_ */
