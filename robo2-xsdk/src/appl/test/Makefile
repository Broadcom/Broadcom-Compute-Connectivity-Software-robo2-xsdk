# $Id: Makefile,v 1.3 Broadcom SDK $
# 
# This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
#
# $Copyright: (c) 2020 Broadcom Inc.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# Makefile for Broadcom Command Monitor (BCM) diagnostic tests
#

LOCALDIR = src/appl/test

CFGFLAGS += -DSOURCE_CHECK_LIBAPPL

include ${SDK}/make/Make.config

ifdef BCM_88030_A0
subdirs += caladan3
include ${SDK}/make/Make.subdirs
endif

lib = libtest
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
