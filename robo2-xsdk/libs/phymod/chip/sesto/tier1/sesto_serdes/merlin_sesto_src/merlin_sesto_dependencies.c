/*
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *  $Id$
 */


/****************************************************************************
****************************************************************************
*  File Name     :  merlin_sesto_dependencies.c                            *
*  Created On    :  05/29/2014                                             *
*  Description   :  Version of Dependency APIs used in Eval                *
*  Revision      :  $Id: $                                                 *
*                                                                          *
*  Copyright 2013  Broadcom Corporation                                    *
*  All Rights Reserved                                                     *
*  No portions of this material may be reproduced in any form without      *
*  the written permission of:                                              *
*      Broadcom Corporation                                                *
*      5300 California Avenue                                              *
*      Irvine, CA  92617                                                   *
*                                                                          *
*  All information contained in this document is Broadcom Corporation      *
*  company private proprietary, and trade secret.                          *
*                                                                          *
****************************************************************************
***************************************************************************/

#include "merlin_sesto_dependencies.h"
#include "../common/srds_api_err_code.h"
#include "../../sesto_reg_structs.h"
#include "../../sesto_cfg_seq.h"
#include "../../sesto_address_defines.h"

#define SES_M0ACCESS_DATA_slave_dram_rdata    0x841b 
#define SES_M0ACCESS_DATA_slave_dram_wdata    0x841a
#define SES_M0ACCESS_ADDR_slave_dram_mem_add  0x8403

static const uint16_t M0_DATA_RAM_LENGTH    = 0x1400;     /* fixed by hardware design */

err_code_t merlin_sesto_pmd_rdt_reg(const phymod_access_t *pa, uint16_t address, uint16_t *val) {
    uint32_t data = 0;
    int rv = ERR_CODE_NONE;

    READ_SESTO_PMA_PMD_REG(pa, (uint32_t)address, data);
    *val = (uint16_t)data;

ERR:
    return (rv);
}

err_code_t merlin_sesto_pmd_wr_reg(const phymod_access_t *pa, uint16_t address, uint16_t val) {
    uint32_t data = 0xffff & val;
    int rv = ERR_CODE_NONE;

    WRITE_SESTO_PMA_PMD_REG(pa, (uint32_t) address, data);

ERR:
    return(rv);
}

err_code_t merlin_sesto_pmd_mwr_reg(const phymod_access_t *pa, uint16_t addr, uint16_t mask, uint8_t lsb, uint16_t val) {
    uint16_t tmp, otmp;
    uint32_t reg_val;
    int rv = ERR_CODE_NONE;
    val = val << lsb;
    
    val = val & mask ;
    /* Use clause 45 access if supported */
    READ_SESTO_PMA_PMD_REG(pa, addr, reg_val);
    tmp = (uint16_t) (reg_val & 0xffff);
    otmp = tmp;
    tmp &= ~(mask);
    tmp |= val;

    if (otmp != tmp) {
        WRITE_SESTO_PMA_PMD_REG(pa, addr, tmp);
    }

ERR:
    return(rv);
}

err_code_t merlin_sesto_delay_ns(uint16_t delay_ns) {

  if (delay_ns > 1000) {
    return (ERR_CODE_SERDES_DELAY);
  }
  return (ERR_CODE_NONE);
} 


err_code_t merlin_sesto_delay_us(uint32_t delay_us) {
  PHYMOD_USLEEP(delay_us);
  return (ERR_CODE_NONE);
}

uint8_t merlin_sesto_get_lane(const phymod_access_t *pa) {
    uint8_t lane_idx; 
    int rv = ERR_CODE_NONE;
    DEV1_SLICE_SLICE_TYPE_T slice_reg;
    
    READ_SESTO_PMA_PMD_REG(pa, DEV1_SLICE_SLICE_ADR, slice_reg.data);
    lane_idx = slice_reg.fields.lane;

    return (lane_idx);
ERR:
    return (rv);
}
/*---------------------------------------------*/
/*  Serdes IP RAM access - Lane RAM Variables  */
/*---------------------------------------------*/
/*          rd - read; wr - write              */ 
/*          b  - byte; w  - word               */
/*---------------------------------------------*/
/** Unsigned Word Read of a uC RAM variable.
* Write access through Micro Register Interface for PMD IPs. Output as little-endian format the LSB is the lowest address.
* @param rd_val 16bit unsigned value to be read from RAM variable
* @param addr Address of RAM variable to be read
* @param size Size of RAM in word to be read
* @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
*/ 
err_code_t merlin_sesto_rdw_ram(const phymod_access_t *pa, uint16_t *rd_val, uint16_t addr, uint16_t size)
{
    uint16_t uC_DRAM_data_reg = 0;
    int rv = ERR_CODE_NONE;

    if (((addr+size) > M0_DATA_RAM_LENGTH)  || ((addr % 2) != 0)) {
        return ERR_CODE_INVALID_RAM_ADDR;
    }

    WRITE_SESTO_PMA_PMD_REG(pa, SES_M0ACCESS_ADDR_slave_dram_mem_add, (uint32_t)addr);
    uC_DRAM_data_reg = (uint16_t) SES_M0ACCESS_DATA_slave_dram_rdata;

    while (size--) {
        READ_SESTO_PMA_PMD_REG(pa, uC_DRAM_data_reg, *rd_val);
        rd_val++;
    }

ERR:
    return rv;
}
/** Unsigned Byte Read of a uC RAM variable.
* Write access through Micro Register Interface for PMD IPs. 
* @param rd_val 8bit unsigned value to be read from RAM variable
* @param addr Address of RAM variable to be read
* @param size Size of RAM in byte to be read
* @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
*/ 
err_code_t merlin_sesto_rdb_ram(const phymod_access_t *pa, uint8_t *rd_val, uint16_t addr, uint16_t size)
{
    uint16_t tmp_data = 0;
    int rv = ERR_CODE_NONE;
    uint16_t uC_DRAM_data_reg = 0;

    if (((addr+size) > M0_DATA_RAM_LENGTH)) {
        return ERR_CODE_INVALID_RAM_ADDR;
    }
    WRITE_SESTO_PMA_PMD_REG(pa, SES_M0ACCESS_ADDR_slave_dram_mem_add, (addr - (addr%2)));
    /* the provided address must be an even one */
    uC_DRAM_data_reg = (uint16_t)SES_M0ACCESS_DATA_slave_dram_rdata;

    if (addr % 2) {
        /* starting address is odd */
        READ_SESTO_PMA_PMD_REG(pa, (uint32_t)uC_DRAM_data_reg, tmp_data);
        *rd_val = (uint8_t) (tmp_data>>8);
        rd_val++;
        size--;
    }

    while (size >= 2) {
        READ_SESTO_PMA_PMD_REG(pa, (uint32_t)uC_DRAM_data_reg, tmp_data);
        *rd_val = (uint8_t) (tmp_data & 0xFF); rd_val++;
        *rd_val = (uint8_t) (tmp_data >> 8); rd_val++;
        size -= 2;
    }

    if (size) {
        READ_SESTO_PMA_PMD_REG(pa, (uint32_t)uC_DRAM_data_reg, tmp_data);
        *rd_val = (uint8_t) (tmp_data & 0xFF);
        rd_val++;
    }

ERR:
    return rv;
}
/** Unsigned Byte Write of a uC RAM variable.
* Write access through Micro Register Interface for PMD IPs
* @param addr Address of RAM variable to be written
* @param size Size of RAM in byte to be written
* @param wr_val 8bit unsigned value to be written to RAM variable
* @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
*/ 
err_code_t merlin_sesto_wrb_ram(const phymod_access_t *pa, uint16_t addr, uint16_t size, uint8_t *wr_val)
{
    uint16_t uC_DRAM_data_reg = 0;
    uint16_t end_addr;
    uint8_t front_buf[2], end_buf[2];
    uint16_t tmp_data = 0;
    int rv = ERR_CODE_NONE;

    if (((addr+size) > M0_DATA_RAM_LENGTH)) {
        return ERR_CODE_INVALID_RAM_ADDR;
    }

 /* The native DRAM access is at granularity of 16-bit entity. If the first/last byte is at
 * an odd address, we need to a read modify write.
 *
 * In addition, the read and write cannot be mixed without a write to the DRAM address
 * register.
 *
 * So we will read the words that have the first/last requested byte and modify the result
 * before doing the batch write.
 */
    end_addr = addr + size;
    if ((end_addr % 2) != 0) {
        merlin_sesto_rdb_ram(pa, end_buf, end_addr-1, 2);
        end_buf[0] = wr_val[size-1];
    }

    if ((addr % 2) != 0) {
        merlin_sesto_rdb_ram(pa, front_buf, addr-1, 2);
        front_buf[1] = wr_val[0];
    }

    WRITE_SESTO_PMA_PMD_REG(pa, SES_M0ACCESS_ADDR_slave_dram_mem_add, (addr - (addr%2)));
    /* the provided address must be an even one */
    uC_DRAM_data_reg = (uint16_t)SES_M0ACCESS_DATA_slave_dram_wdata;

    if (addr % 2) {
        tmp_data  = front_buf[1];
        tmp_data <<= 8;
        tmp_data += front_buf[0];
        WRITE_SESTO_PMA_PMD_REG(pa, uC_DRAM_data_reg, tmp_data);
        wr_val++;
        size--;
    }

    while (size >= 2) {
       tmp_data  = wr_val[1];
       tmp_data <<= 8;
       tmp_data += wr_val[0];
       WRITE_SESTO_PMA_PMD_REG(pa, uC_DRAM_data_reg, tmp_data);

        wr_val += 2;
        size -= 2;
    }

    if (end_addr % 2) {
        tmp_data = end_buf[1];
        tmp_data <<= 8;
        tmp_data += end_buf[0];
        WRITE_SESTO_PMA_PMD_REG(pa, uC_DRAM_data_reg, tmp_data);
    }

ERR:
    return rv;
}

/** Unsigned Word Write of a uC RAM variable.
* Write access through Micro Register Interface for PMD IPs
* @param addr Address of RAM variable to be written
* @param size Size of RAM in word to be written
* @param wr_val 16bit unsigned value to be written to RAM variable
* @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
*/ 
err_code_t merlin_sesto_wrw_ram(const phymod_access_t *pa, uint16_t addr, uint16_t size, uint16_t *wr_val)
{
    uint16_t uC_DRAM_data_reg = 0;
    int rv = ERR_CODE_NONE;

    if (((addr+size) > M0_DATA_RAM_LENGTH)  || ((addr % 2) != 0)) {
        return ERR_CODE_INVALID_RAM_ADDR;
    }

    WRITE_SESTO_PMA_PMD_REG(pa, SES_M0ACCESS_ADDR_slave_dram_mem_add, (uint32_t)addr);
    uC_DRAM_data_reg = (uint16_t)SES_M0ACCESS_DATA_slave_dram_wdata;

    while (size--) {
        WRITE_SESTO_PMA_PMD_REG(pa, uC_DRAM_data_reg, *wr_val);
        wr_val++;
    }

ERR:
    return rv;
}
/** Retrieve Lane Based Information.
* Retrieve current lane memory address information
* @param ext_uc_lane_info an output pointer pointed to uC lane info struct
* @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
*/ 
err_code_t merlin_sesto_get_uc_ln_info(const phymod_access_t *pa, merlin_sesto_uc_lane_info_st *lane_info)
{
    if (lane_info == NULL) {
        return ERR_CODE_BAD_PTR_OR_INVALID_INPUT;
    }

    /* fixed values based on a specific implementation */
    lane_info->is_direct_ram_access_avail = 1;
    lane_info->core_var_base_address = 0x4c0;
    lane_info->lane_ram_size = 160;
    lane_info->trace_mem_size = 960;
    lane_info->num_lanes = 10;

    lane_info->uc_lane_idx = merlin_sesto_get_lane(pa);
    lane_info->trace_mem_base_address = 0x100;
    lane_info->lane_var_base_address = 0x500
        + lane_info->lane_ram_size*lane_info->uc_lane_idx;
    lane_info->diag_base_address = lane_info->trace_mem_base_address
        + 96*lane_info->uc_lane_idx;

    return ERR_CODE_NONE;
}
uint8_t merlin_sesto_get_core(const phymod_access_t *pa)
{
 return 0;
}

err_code_t merlin_sesto_uc_lane_idx_to_system_id(const phymod_access_t *pa, const char *string , uint8_t uc_lane_idx)
{
  return(0);
}
