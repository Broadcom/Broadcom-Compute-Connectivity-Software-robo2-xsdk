#
# $Id: Makefile,v 1.6 Broadcom SDK $
# 
# This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
#
# $Copyright: (c) 2020 Broadcom Inc.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# Makefile for phy subdirectory
#
LOCALDIR = src/soc/phy

include ${SDK}/make/Make.config

-include wcmod/src/subdir.mk
-include tscmod/src/subdir.mk
-include phy8806x/src/subdir.mk
ifneq (,$(findstring -DINCLUDE_PLP_IMACSEC,$(CFGFLAGS)))
-include imacsec/subdir.mk
endif
include ${SDK}/make/Make.subdirs

lib = libsoc_phy

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
