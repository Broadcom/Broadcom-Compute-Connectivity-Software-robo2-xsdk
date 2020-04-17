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
*  File Name     :  merlin_quadra28_dependencies.c                            *
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

#include "merlin_quadra28_dependencies.h"
#include "common/srds_api_err_code.h"
#include "../quadra28_cfg_seq.h"
#include "../bcmi_quadra28_defs.h"
#include "../quadra28_reg_access.h"

#define SES_M0ACCESS_DATA_slave_dram_rdata    0x841b 
#define SES_M0ACCESS_DATA_slave_dram_wdata    0x841a
#define SES_M0ACCESS_ADDR_slave_dram_mem_add  0x8403

static const uint16_t M0_DATA_RAM_LENGTH    = 0x1400;     /* fixed by hardware design */

err_code_t merlin_quadra28_pmd_rdt_reg(const phymod_access_t *pa, uint16_t address, uint16_t *val) {
    uint32_t data = 0;
    PHYMOD_IF_ERR_RETURN(
        phymod_raw_iblk_read(pa, (uint32_t)address, &data));
    *val = (uint16_t)data;
    return ( ERR_CODE_NONE );
}

err_code_t merlin_quadra28_pmd_wr_reg(const phymod_access_t *pa, uint16_t address, uint16_t val) {
    uint32_t data = 0xffff & val;

    PHYMOD_IF_ERR_RETURN(
        phymod_raw_iblk_write(pa, (uint32_t) address, data));
    return(ERR_CODE_NONE);
}

err_code_t merlin_quadra28_pmd_mwr_reg(const phymod_access_t *pa, uint16_t addr, uint16_t mask, uint8_t lsb, uint16_t val) {
    uint16_t tmp, otmp;
    uint32_t reg_val;
    val = val << lsb;
    
    val = val & mask ;
    /* Use clause 45 access if supported */
    PHYMOD_IF_ERR_RETURN(
        phymod_raw_iblk_read(pa, addr, &reg_val));
    tmp = (uint16_t) (reg_val & 0xffff);
    otmp = tmp;
    tmp &= ~(mask);
    tmp |= val;

    if (otmp != tmp) {
        PHYMOD_IF_ERR_RETURN(
            phymod_raw_iblk_write(pa, addr, tmp));
    }
    return(ERR_CODE_NONE);
}

err_code_t merlin_quadra28_delay_ns(uint16_t delay_ns) {

  if (delay_ns > 1000) {
    return (ERR_CODE_SERDES_DELAY);
  }
  return (ERR_CODE_NONE);
} 


err_code_t merlin_quadra28_delay_us(uint32_t delay_us) {
  PHYMOD_USLEEP(delay_us);
  return (ERR_CODE_NONE);
}

uint8_t merlin_quadra28_get_lane(const phymod_access_t *pa) {
    uint8_t lane_idx; 
    ADDRESS_EXTENSIONr_t single_pmd_ch_sel;

    PHYMOD_MEMSET(&single_pmd_ch_sel, 0 , sizeof(ADDRESS_EXTENSIONr_t));
    PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_ADDRESS_EXTENSIONr(pa, &single_pmd_ch_sel));
    
    lane_idx = BCMI_QUADRA28_ADDRESS_EXTENSIONr_AER_CHf_GET(single_pmd_ch_sel);
    
    return (lane_idx);
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
err_code_t merlin_quadra28_rdw_ram(const phymod_access_t *pa, uint16_t *rd_val, uint16_t addr, uint16_t size)
{
    uint16_t uC_DRAM_data_reg = 0;
    uint32_t data;

    if (((addr+size) > M0_DATA_RAM_LENGTH)  || ((addr % 2) != 0)) {
        return ERR_CODE_INVALID_RAM_ADDR;
    }

    PHYMOD_IF_ERR_RETURN(
        phymod_raw_iblk_write(pa, SES_M0ACCESS_ADDR_slave_dram_mem_add, (uint32_t)addr));
    uC_DRAM_data_reg = (uint16_t) SES_M0ACCESS_DATA_slave_dram_rdata;

    while (size--) {
        PHYMOD_IF_ERR_RETURN(
            phymod_raw_iblk_read(pa, uC_DRAM_data_reg, &data));
        *rd_val = data & 0xFFFF;
        rd_val++;
    }

    return ERR_CODE_NONE;
}
/** Unsigned Byte Read of a uC RAM variable.
* Write access through Micro Register Interface for PMD IPs. 
* @param rd_val 8bit unsigned value to be read from RAM variable
* @param addr Address of RAM variable to be read
* @param size Size of RAM in byte to be read
* @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
*/ 
err_code_t merlin_quadra28_rdb_ram(const phymod_access_t *pa, uint8_t *rd_val, uint16_t addr, uint16_t size)
{
    uint32_t tmp_data = 0;
    uint16_t uC_DRAM_data_reg = 0;

    if (((addr+size) > M0_DATA_RAM_LENGTH)) {
        return ERR_CODE_INVALID_RAM_ADDR;
    }
    PHYMOD_IF_ERR_RETURN(
        phymod_raw_iblk_write(pa, SES_M0ACCESS_ADDR_slave_dram_mem_add, (addr - (addr%2))));
    /* the provided address must be an even one */
    uC_DRAM_data_reg = (uint16_t)SES_M0ACCESS_DATA_slave_dram_rdata;

    if (addr % 2) {
        /* starting address is odd */
        PHYMOD_IF_ERR_RETURN(
            phymod_raw_iblk_read(pa, (uint32_t)uC_DRAM_data_reg, &tmp_data));
        *rd_val = (uint8_t) (tmp_data>>8);
        rd_val++;
        size--;
    }

    while (size >= 2) {
        PHYMOD_IF_ERR_RETURN(
            phymod_raw_iblk_read(pa, (uint32_t)uC_DRAM_data_reg, &tmp_data));
        *rd_val = (uint8_t) (tmp_data & 0xFF); rd_val++;
        *rd_val = (uint8_t) (tmp_data >> 8); rd_val++;
        size -= 2;
    }

    if (size) {
        PHYMOD_IF_ERR_RETURN(
            phymod_raw_iblk_read(pa, (uint32_t)uC_DRAM_data_reg, &tmp_data));
        *rd_val = (uint8_t) (tmp_data & 0xFF);
        rd_val++;
    }

    return ERR_CODE_NONE;
}
/** Unsigned Byte Write of a uC RAM variable.
* Write access through Micro Register Interface for PMD IPs
* @param addr Address of RAM variable to be written
* @param size Size of RAM in byte to be written
* @param wr_val 8bit unsigned value to be written to RAM variable
* @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
*/ 
err_code_t merlin_quadra28_wrb_ram(const phymod_access_t *pa, uint16_t addr, uint16_t size, uint8_t *wr_val)
{
    uint16_t uC_DRAM_data_reg = 0;
    uint16_t end_addr;
    uint8_t front_buf[2], end_buf[2];
    uint16_t tmp_data = 0;

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
        merlin_quadra28_rdb_ram(pa, end_buf, end_addr-1, 2);
        end_buf[0] = wr_val[size-1];
    }

    if ((addr % 2) != 0) {
        merlin_quadra28_rdb_ram(pa, front_buf, addr-1, 2);
        front_buf[1] = wr_val[0];
    }

    PHYMOD_IF_ERR_RETURN(
        phymod_raw_iblk_write(pa, SES_M0ACCESS_ADDR_slave_dram_mem_add, (addr - (addr%2))));
    /* the provided address must be an even one */
    uC_DRAM_data_reg = (uint16_t)SES_M0ACCESS_DATA_slave_dram_wdata;

    if (addr % 2) {
        tmp_data  = front_buf[1];
        tmp_data <<= 8;
        tmp_data += front_buf[0];
        PHYMOD_IF_ERR_RETURN(
            phymod_raw_iblk_write(pa, uC_DRAM_data_reg, tmp_data));
        wr_val++;
        size--;
    }

    while (size >= 2) {
       tmp_data  = wr_val[1];
       tmp_data <<= 8;
       tmp_data += wr_val[0];
       PHYMOD_IF_ERR_RETURN(
           phymod_raw_iblk_write(pa, uC_DRAM_data_reg, tmp_data));

        wr_val += 2;
        size -= 2;
    }

    if (end_addr % 2) {
        tmp_data = end_buf[1];
        tmp_data <<= 8;
        tmp_data += end_buf[0];
        PHYMOD_IF_ERR_RETURN(
            phymod_raw_iblk_write(pa, uC_DRAM_data_reg, tmp_data));
    }

    return ERR_CODE_NONE;
}

/** Unsigned Word Write of a uC RAM variable.
* Write access through Micro Register Interface for PMD IPs
* @param addr Address of RAM variable to be written
* @param size Size of RAM in word to be written
* @param wr_val 16bit unsigned value to be written to RAM variable
* @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
*/ 
err_code_t merlin_quadra28_wrw_ram(const phymod_access_t *pa, uint16_t addr, uint16_t size, uint16_t *wr_val)
{
    uint16_t uC_DRAM_data_reg = 0;

    if (((addr+size) > M0_DATA_RAM_LENGTH)  || ((addr % 2) != 0)) {
        return ERR_CODE_INVALID_RAM_ADDR;
    }

    PHYMOD_IF_ERR_RETURN(
        phymod_raw_iblk_write(pa, SES_M0ACCESS_ADDR_slave_dram_mem_add, (uint32_t)addr));
    uC_DRAM_data_reg = (uint16_t)SES_M0ACCESS_DATA_slave_dram_wdata;

    while (size--) {
        PHYMOD_IF_ERR_RETURN(
            phymod_raw_iblk_write(pa, uC_DRAM_data_reg, *wr_val));
        wr_val++;
    }

    return ERR_CODE_NONE;
}
/** Retrieve Lane Based Information.
* Retrieve current lane memory address information
* @param ext_uc_lane_info an output pointer pointed to uC lane info struct
* @return Error Code generated by API (returns ERR_CODE_NONE if no errors)
*/ 
err_code_t merlin_quadra28_get_uc_ln_info(const phymod_access_t *pa, merlin_quadra28_uc_lane_info_st *lane_info)
{
    if (lane_info == NULL) {
        return ERR_CODE_BAD_PTR_OR_INVALID_INPUT;
    }

    lane_info->is_direct_ram_access_avail = 0;
    lane_info->core_var_base_address = 0x0;
    lane_info->lane_ram_size = 208;
    lane_info->trace_mem_size = 400;
    lane_info->num_lanes = 10;

    lane_info->uc_lane_idx = merlin_quadra28_get_lane(pa);
    lane_info->trace_mem_base_address = 0;
    lane_info->lane_var_base_address = 0;
    lane_info->diag_base_address = 0; 

    return ERR_CODE_NONE;
}
uint8_t merlin_quadra28_get_core(void)
{
 return 0;
}

err_code_t merlin_quadra28_uc_lane_idx_to_system_id(char *string , uint8_t uc_lane_idx)
{
    switch(uc_lane_idx)
    {
        case 0:
            string = "ZERO";   
        break; 
        case 1:
            string = "ONE";   
        break; 
        case 2:
            string = "TWO";   
        break; 
        case 3:
            string = "THREE";   
        break; 
        case 4:
            string = "FOUR";   
        break; 
        case 5:
            string = "FIVE";   
        break; 
        case 6:
            string = "SIX";   
        break; 
        case 7:
            string = "SEVEN";   
        break; 
        case 8:
            string = "EIGHT";   
        break; 
        case 9:
            string = "NINE";   
        break; 
    }
    Q28_UNUSED_PARAM(string);
    return ERR_CODE_NONE;
}
