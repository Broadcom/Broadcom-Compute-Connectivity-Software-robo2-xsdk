/*
 * $Id: README.TXT,v 1.2 Broadcom SDK $
 *
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

Diagnostic Tests

Notes
-----

Diag tests are not expected to clean up after themselves.
However, each test is expected to initialize the chip properly before
performing its work.  Many of the tests, such as port loopbacks, do this
by calling soc_reset() or by running rc.soc prior to the test.
