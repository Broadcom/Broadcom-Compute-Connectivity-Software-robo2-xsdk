#
# $Id: Makefile,v 1.1 Broadcom SDK $
# 
# This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
#
# $Copyright: (c) 2020 Broadcom Inc.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# Makefile for phy/chip/koi/merlin_koi_src subdirectory
#
LOCALDIR = src/soc/phy/chip/koi/merlin_koi_src

include ${SDK}/make/Make.config

CFGFLAGS+=  -UCOMPILER_HAS_DOUBLE

LSRCS    =\
            merlin_koi_functions.c merlin_koi_dependencies.c\

OBJS := $(LSRCS:.c=.o)

lib = libsoc_koi
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
