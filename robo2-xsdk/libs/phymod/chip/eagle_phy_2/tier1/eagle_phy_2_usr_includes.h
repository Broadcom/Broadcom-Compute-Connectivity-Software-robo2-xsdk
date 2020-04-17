/**************************************************************************************
 *  File Name     :  eagle_phy_2_usr_includes.h                                        *
 *  Created On    :  05/07/2014                                                       *
 *  Created By    :  Kiran Divakar                                                    *
 *  Description   :  Header file which includes all required std libraries and macros *
 *  Revision      :  $Id: eagle_phy_2_usr_includes.h 996 2015-03-30 22:39:03Z kirand $ *
 *                                                                                    *
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$                                                              *
 *  No portions of this material may be reproduced in any form without                *
 *  the written permission of:                                                        *
 *      Broadcom Corporation                                                          *
 *      5300 California Avenue                                                        *
 *      Irvine, CA  92617                                                             *
 *                                                                                    *
 *  All information contained in this document is Broadcom Corporation                *
 *  company private proprietary, and trade secret.                                    *
 *                                                                                    *
 **************************************************************************************
 **************************************************************************************/

/** @file eagle_phy_2_usr_includes.h
 * Header file which includes all required std libraries and macros
 */

/* The user is expected to replace the macro definitions with their required implementation */

#ifndef EAGLE_PHY_2_API_USR_INCLUDES_H
#define EAGLE_PHY_2_API_USR_INCLUDES_H

/* Standard libraries that can be replaced by your custom libraries */
#ifdef _MSC_VER
/* Enclose all standard headers in a pragma to remove warings for MS compiler */
#pragma warning( push, 0 )
#endif
#ifdef LINUX
#include <stdint.h>
#endif
#include <string.h>

#ifdef _MSC_VER
#pragma warning( pop )
#endif

/* Redefine macros according your compiler requirements */
//#define USR_PRINTF(paren_arg_list)    ((void)usr_logger_write paren_arg_list)
//#define USR_PRINTF    post_log
#define USR_PRINTF
#define USR_MEMSET(mem, val, num)     memset(mem, val, num)
#define USR_STRLEN(string)            strlen(string)
#define USR_STRNCAT(str1, str2)        strcat(str1, str2)
#define USR_STRCPY(str1, str2)        strcpy(str1, str2)
#define USR_STRCMP(str1, str2)        strcmp(str1, str2)
#define USR_STRNCMP(str1, str2, num)  strncmp(str1, str2, num)

#define SERDES_API_FLOATING_POINT
#ifdef SERDES_API_FLOATING_POINT
#define USR_DOUBLE                    double
#else
#define USR_DOUBLE       int
#define double       undefined
#define float        undefined
#endif

/* Implementation specific macros below */
#define usr_logger_write(...) logger_write(-1,__VA_ARGS__)

#endif
