/**************************************************************************************
**************************************************************************************
*                                                                                    *
*  Revision      :  $Id: eagle2_tsc2pll_dependencies.h 1466 2016-07-27 15:28:10Z cvazquez $ *
*                                                                                    *
*  Description   :  API Dependencies to be provided by IP user                       *
*                                                                                    *
*
* This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
*
* $Copyright: (c) 2020 Broadcom Inc.
* Broadcom Proprietary and Confidential. All rights reserved.$                                                              *
*  No portions of this material may be reproduced in any form without                *
*  the written permission of:                                                        *
*      Broadcom Corporation                                                          *
*      5300 California Avenue                                                        *
*      Irvine, CA  92617                                                             *
*                                                                                    *
*  All information contained in this document is Broadcom Corporation                *
*  company private proprietary, and trade secret.                                    *
*                                                                                    *
**************************************************************************************
**************************************************************************************/

/** @file eagle2_tsc2pll_dependencies.h
 * Dependencies to be provided by IP User
 */

#ifndef EAGLE2_TSC2PLL_API_DEPENDENCIES_H
#define EAGLE2_TSC2PLL_API_DEPENDENCIES_H
#include <phymod/phymod.h>

#include "eagle2_tsc2pll_usr_includes.h"
#include "common/srds_api_err_code.h"
#include <phymod/phymod.h>
#include "eagle2_tsc2pll_ipconfig.h"

/** Read a register from the currently selected Serdes IP Lane.
 * @param pa phymod_access_t struct
 * @param address Address of register to be read
 * @param *val value read out from the register
 * @return Error code generated by read function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle2_tsc2pll_pmd_rdt_reg(const phymod_access_t *pa, uint16_t address, uint16_t *val);

/** Write to a register from the currently selected Serdes IP Lane.
 * @param pa phymod_access_t struct
 * @param address Address of register to be written
 * @param val Value to be written to the register
 * @return Error code generated by write function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle2_tsc2pll_pmd_wr_reg(const phymod_access_t *pa, uint16_t address, uint16_t val);

/** Masked Register Write to the currently selected Serdes IP core/lane.
 * If using Serdes MDIO controller to access the registers, implement this function using eagle2_tsc2pll_pmd_mdio_mwr_reg(..)
 *
 * If NOT using a Serdes MDIO controller or the Serdes PMI Masked write feature, please use the following code to
 * implement this function
 *
 *    eagle2_tsc2pll_pmd_wr_reg(addr, ((eagle2_tsc2pll_pmd_rdt_reg(addr) & ~mask) | (mask & (val << lsb))));
 *
 * @param pa phymod_access_t struct
 * @param addr Address of register to be written
 * @param mask 16-bit mask indicating the position of the field with bits of 1s
 * @param lsb  LSB of the field
 * @param val  16bit value to be written
 * @return Error code generated by write function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle2_tsc2pll_pmd_mwr_reg(const phymod_access_t *pa, uint16_t addr, uint16_t mask, uint8_t lsb, uint16_t val);


/** Write message to the logger with the designated verbose level.
 * Output is sent to stdout and a logfile
 * @param pa phymod_access_t struct
 * @param message_verbose_level   Verbose level for the current message
 * @param *format Format string as in printf
 * @param ... Additional variables used as in printf
 * @return Error code generated by function (returns ERR_CODE_NONE if no errors)
 */
int logger_write(int message_verbose_level, const char *format, ...);

/** Delay the execution of the code for atleast specified amount of time in nanoseconds.
 * This function is used ONLY for delays less than 1 microsecond, non-zero error code may be returned otherwise.
 * The user can implement this as an empty function if their register access latency exceeds 1 microsecond.
 * @param pa phymod_access_t struct
 * @param delay_ns Delay in nanoseconds
 * @return Error code generated by delay function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle2_tsc2pll_delay_ns(uint16_t delay_ns);

/** Delay the execution of the code for atleast specified amount of time in microseconds.
 * For longer delays, accuracy is required. When requested delay is > 100ms, the implemented delay is assumed
 * to be < 10% bigger than requested.
 * This function is used ONLY for delays greater than or equal to 1 microsecond.
 * @param pa phymod_access_t struct
 * @param delay_us Delay in microseconds
 * @return Error code generated by delay function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle2_tsc2pll_delay_us(uint32_t delay_us);

/** Delay the execution of the code for atleast specified amount of time in milliseconds.
 * For longer delays, accuracy is required. When requested delay is > 100ms, the implemented delay is assumed
 * to be < 10% bigger than requested.
 * This function is used ONLY for delays greater than or equal to 1 millisecond.
 * @param pa phymod_access_t struct
 * @param delay_ms Delay in milliseconds
 * @return Error code generated by delay function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle2_tsc2pll_delay_ms(uint32_t delay_ms);

/** Return the address of current selected Serdes IP Core.
 * @param pa phymod_access_t struct
 * @return the IP level address of the current core.
 */
uint8_t eagle2_tsc2pll_get_core(void);

/** Return the address of current selected Serdes IP lane.
 * @param pa phymod_access_t struct
 * @return the IP level address of the current lane. 0 to N-1, for an N lane IP
 */
uint8_t eagle2_tsc2pll_get_lane(const phymod_access_t *pa);


/** Polls for register field "uc_dsc_ready_for_cmd" to be 1 within the time interval specified by timeout_ms.
 * Function returns 0 if polling passes, else it returns error code.
 * Define macro CUSTOM_REG_POLLING to replace the default implementation provided in eagle_tsc_functions.c.
 * @param pa phymod_access_t struct
 * @param timeout_ms Maximum time interval in milliseconds for which the polling is done
 * @return Error code generated by polling function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle2_tsc2pll_poll_uc_dsc_ready_for_cmd_equals_1( const phymod_access_t *pa, uint32_t timeout_ms);
/** Return the address of current selected Serdes IP PLL.
 * @param pa phymod_access_t struct
 * @return the IP level address of the current PLL. 0 to N-1, for an N-PLL IP
 */
uint8_t eagle2_tsc2pll_get_pll(const phymod_access_t *pa);

/** Set the address of current selected Serdes IP PLL.  Used in disgnoztic
 * and core-level management functions.
 * @param pa phymod_access_t struct
 * @return Any error code generated during execution; ERR_CODE NONE otherwise.
 */
err_code_t eagle2_tsc2pll_set_pll(phymod_access_t *pa, uint8_t pll_index);

/** Set the address of current selected Serdes IP lane.  Used in disgnoztic
 * and core-level management functions.
 * @param pa phymod_access_t struct
 * @return Any error code generated during execution; ERR_CODE NONE otherwise.
 */
err_code_t eagle2_tsc2pll_set_lane(phymod_access_t *pa, uint8_t lane_index);

/** Convert uC lane index.
 * Convert uC lane index to system ID string.
 * @param pa phymod_access_t struct
 * @param string a 16-byte output buffer to receive system ID
 * @param uc_lane_idx uC lane index
 * @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
 * The textual identifier is pre-filled with a default:  implementors may
 * safely return without modifying it if the default text is sufficient.
 */
err_code_t eagle2_tsc2pll_uc_lane_idx_to_system_id(char string[16], uint8_t uc_lane_idx);



#endif
