# $Id: Makefile,v 1.1 Broadcom SDK $
# 
# This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
#
# $Copyright: (c) 2020 Broadcom Inc.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# Makefile for Shared Library
#

LOCALDIR = src/shared

include ${SDK}/make/Make.config

$(warning INCLUDE ${SDK}/MAKE/MAKE.CONFIG)


#
# For both DPP or DNX, we need to take library xml. However, it is
# NOT taken for 'kernel' compilation (Note that this is not relevant
# for DNX which does not support 'kernel')
#
# For DNX, we do NOT take 'swstate'. In the future, e shall use a
# specifically adapted 'swstate'
#
# Once we have more than 1 family we use SAND_CHIPS
#

ifdef SAND_CHIPS
# { 
subdirs = shrextend utilex dbx
ifdef DPP_CHIPS
# {
subdirs += swstate
# }
endif
ifdef DNX_CHIPS
# {
ifndef DPP_CHIPS
# {
subdirs += swstate
# }
endif
# }
endif
ifeq (,$(findstring -DNO_FILEIO,$(CFGFLAGS)))
# {
	subdirs += libxml
# }
endif
# }
endif

include ${SDK}/make/Make.subdirs

lib = libshared
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
