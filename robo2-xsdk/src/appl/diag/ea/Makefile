# -*- Makefile -*-
# $Id: Makefile,v 1.1 Broadcom SDK $
# 
# This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
#
# $Copyright: (c) 2020 Broadcom Inc.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
LOCALDIR = src/appl/diag/ea

include ${SDK}/make/Make.config

ifdef IMP_PROTECT
CFLAGS += -DIMP_SW_PROTECT
endif

#ifdef COND_LIB_BCMX_DIAG
#subdirs := bcmx
#include ${SDK}/make/Make.subdirs
#endif

lib = libdiag_ea
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
