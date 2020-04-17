# -*- Makefile -*-
# $Id: Makefile,v 0.1  Exp $
# 
# This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
#
# $Copyright: (c) 2020 Broadcom Inc.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# Makefile for avenger FSAL Library
#
CFLAGS += -g -O -DBCM_53158_A0

LOCALDIR = src/soc/robo/robo2/fsal

include ${SDK}/make/Make.config
CFLAGS += -DCONFIG_EXTERNAL_HOST
CFLAGS += -DLE_HOST
CFLAGS += -DMANAGED_MODE
CFLAGS += -D_SHR_PBMP_WIDTH=0

lib = librobo2_fsal
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend

