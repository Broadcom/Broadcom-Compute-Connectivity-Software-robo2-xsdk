# $Id: Makefile,v 1.2 Broadcom SDK $
# 
# This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
#
# $Copyright: (c) 2020 Broadcom Inc.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# Include standard Makefile configuration
#

include ${SDK}/make/Make.config
include ${SDK}/make/Make.depend

all::

install:: ${install_files}

${install_files}::
	${INSTALL} -d ${install_headers}/$(dir $@)
	${INSTALL} -m0644 -c $@ ${install_headers}/$(dir $@)

clean::

distclean::
	$(RM) ${install_headers}
