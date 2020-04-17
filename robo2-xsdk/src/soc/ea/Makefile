# $Id: Makefile,v 1.8 Broadcom SDK $
# 
# This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
#
# $Copyright: (c) 2020 Broadcom Inc.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# Makefile for SOC EA driver Library
#
LOCALDIR = src/soc/ea

subdirs = 
include ${SDK}/make/Make.config

ifdef BCM_TK371X_SUPPORT
subdirs += tk371x  
endif

include ${SDK}/make/Make.subdirs

