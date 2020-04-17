# $Id: Makefile,v 1.6 Broadcom SDK $
# 
# This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
#
# $Copyright: (c) 2020 Broadcom Inc.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# Makefile for BCM5 driver Library
#

LOCALDIR = src/bcm/ea

include ${SDK}/make/Make.config

ifdef BCM_TK371X_SUPPORT
subdirs = common tk371x
endif

include ${SDK}/make/Make.subdirs

lib = libbcm_ea
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
