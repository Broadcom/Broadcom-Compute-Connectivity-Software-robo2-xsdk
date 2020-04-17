/**************************************************************************************
 **************************************************************************************
 *                                                                                    *
 *  Revision      :  $Id: eagle_phy_2_dependencies.h 1466 2016-07-27 15:28:10Z cvazquez $ *
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

/** @file eagle_phy_2_dependencies.h
 * Dependencies to be provided by IP User
 */

#ifndef EAGLE_PHY_2_API_DEPENDENCIES_H
#define EAGLE_PHY_2_API_DEPENDENCIES_H

#include "eagle_phy_2_usr_includes.h"
#include "common/srds_api_err_code.h"
#include <phymod/phymod.h>

/* #include "eagle_phy_2_ipconfig.h" */

/** Read a register from the currently selected Serdes IP Lane.
 * @param address Address of register to be read
 * @param *val value read out from the register
 * @return Error code generated by read function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle_phy_2_pmd_rdt_reg(const phymod_access_t *pa,uint16_t address, uint16_t *val);

/** Write to a register from the currently selected Serdes IP Lane.
 * @param address Address of register to be written
 * @param val Value to be written to the register
 * @return Error code generated by write function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle_phy_2_pmd_wr_reg(const phymod_access_t *pa,uint16_t address, uint16_t val);

/** Masked Register Write to the currently selected Serdes IP core/lane.
 * If using Serdes MDIO controller to access the registers, implement this function using eagle_phy_2_pmd_mdio_mwr_reg(..)
 *
 * If NOT using a Serdes MDIO controller or the Serdes PMI Masked write feature, please use the following code to
 * implement this function
 *
 *    eagle_phy_2_pmd_wr_reg(const phymod_access_t *pa,addr, ((eagle_phy_2_pmd_rdt_reg(const phymod_access_t *pa,addr) & ~mask) | (mask & (val << lsb))));
 *
 * @param addr Address of register to be written
 * @param mask 16-bit mask indicating the position of the field with bits of 1s
 * @param lsb  LSB of the field
 * @param val  16bit value to be written
 * @return Error code generated by write function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle_phy_2_pmd_mwr_reg(const phymod_access_t *pa,uint16_t addr, uint16_t mask, uint8_t lsb, uint16_t val);


/** Read a register with a 32-bit address from the currently selected Serdes IP Lane.
 * @param address 32-bit address of register to be read
 * @param *val value read out from the register
 * @return Error code generated by read function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle_phy_2_pmd_rdt_reg_addr32(const phymod_access_t *pa,uint32_t address, uint16_t *val);

/** Write to a register with a 32-bit address from the currently selected Serdes IP Lane.
 * @param address 32-bit address of register to be written
 * @param val Value to be written to the register
 * @return Error code generated by write function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle_phy_2_pmd_wr_reg_addr32(const phymod_access_t *pa,uint32_t address, uint16_t val);

/** Masked Register Write to the currently selected Serdes IP core/lane.
 * If using Serdes MDIO controller to access the registers, implement this function using eagle_phy_2_pmd_mdio_mwr_reg(..)
 *
 * If NOT using a Serdes MDIO controller or the Serdes PMI Masked write feature, please use the following code to
 * implement this function
 *
 *    eagle_phy_2_pmd_wr_reg_addr32(addr, ((eagle_phy_2_pmd_rdt_reg_addr32(addr) & ~mask) | (mask & (val << lsb))));
 *
 * @param addr 32-bit address of register to be written
 * @param mask 16-bit mask indicating the position of the field with bits of 1s
 * @param lsb  LSB of the field
 * @param val  16bit value to be written
 * @return Error code generated by write function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle_phy_2_pmd_mwr_reg_addr32(const phymod_access_t *pa,uint32_t addr, uint16_t mask, uint8_t lsb, uint16_t val);

/** Write message to the logger with the designated verbose level.
 * Output is sent to stdout and a logfile
 * @param message_verbose_level   Verbose level for the current message
 * @param *format Format string as in printf
 * @param ... Additional variables used as in printf
 * @return Error code generated by function (returns ERR_CODE_NONE if no errors)
 */
int eagle_logger_write(int message_verbose_level, const char *format, ...);

/** Delay the execution of the code for atleast specified amount of time in nanoseconds.
 * This function is used ONLY for delays less than 1 microsecond, non-zero error code may be returned otherwise.
 * The user can implement this as an empty function if their register access latency exceeds 1 microsecond.
 * @param delay_ns Delay in nanoseconds
 * @return Error code generated by delay function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle_phy_2_delay_ns(uint16_t delay_ns);

/** Delay the execution of the code for atleast specified amount of time in microseconds.
 * For longer delays, accuracy is required. When requested delay is > 100ms, the implemented delay is assumed
 * to be < 10% bigger than requested.
 * This function is used ONLY for delays greater than or equal to 1 microsecond.
 * @param delay_us Delay in microseconds
 * @return Error code generated by delay function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle_phy_2_delay_us(uint32_t delay_us);

/** Delay the execution of the code for atleast specified amount of time in milliseconds.
 * For longer delays, accuracy is required. When requested delay is > 100ms, the implemented delay is assumed
 * to be < 10% bigger than requested.
 * This function is used ONLY for delays greater than or equal to 1 millisecond.
 * @param delay_ms Delay in milliseconds
 * @return Error code generated by delay function (returns ERR_CODE_NONE if no errors)
 */
err_code_t eagle_phy_2_delay_ms(uint32_t delay_ms);

/** Return the address of current selected Serdes IP Core.
 * @return the IP level address of the current core.
 */
uint8_t eagle_phy_2_get_core();

/** Return the address of current selected Serdes IP lane.
 * @return the IP level address of the current lane. 0 to N-1, for an N lane IP
 */
uint8_t eagle_phy_2_get_lane(const phymod_access_t *pa);



/** Convert uC lane index.
* Convert uC lane index to system ID string.
* @param string a 16-byte output buffer to receive system ID
* @param uc_lane_idx uC lane index
* @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
* The textual identifier is pre-filled with a default:  implementors may
* safely return without modifying it if the default text is sufficient.
*/
err_code_t eagle_phy_2_uc_lane_idx_to_system_id(char string[16], uint8_t uc_lane_idx);



#endif
