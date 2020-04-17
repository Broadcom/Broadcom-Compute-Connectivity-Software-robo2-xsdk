# $Id: Makefile,v 1.2 Broadcom SDK $
# 
# This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
#
# $Copyright: (c) 2020 Broadcom Inc.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# Makefile for SOC driver Library

CFLAGS += -g -O -DBCM_53158_A0

LOCALDIR = src/soc/robo/robo2

include ${SDK}/make/Make.config
CFLAGS += -DCONFIG_EXTERNAL_HOST
CFLAGS += -DLE_HOST
CFLAGS += -DMANAGED_MODE
CFLAGS += -D_SHR_PBMP_WIDTH=0

ifeq ($(CONFIG_AVS),1)
    CFLAGS += -DINCLUDE_ROBO2_AVS
endif

lib = libsocrobo2
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
