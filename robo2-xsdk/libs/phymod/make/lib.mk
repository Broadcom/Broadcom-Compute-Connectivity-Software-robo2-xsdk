# $Id$
# 
# This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
#
# $Copyright: (c) 2020 Broadcom Inc.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# PHYMOD make rules for libraries
#

include $(PHYMOD)/make/config.mk

include $(PHYMOD)/make/rules.mk

BLIBNAME = $(LIBDIR)/$(LIBNAME).$(LIBSUFFIX)

.SECONDARY:: $(BOBJS)

all:: $(BLDDIR)/.tree $(BLIBNAME)

clean::
	@$(ECHO) Cleaning objects for $(notdir $(BLIBNAME))
	$(Q)$(RM) $(BLDDIR)/.tree $(BOBJS) $(BLIBNAME)

include $(PHYMOD)/make/depend.mk
