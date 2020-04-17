/*
 * * $ID: $
 * *
 * * 
 * * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 * * 
* * $Copyright: (c) 2020 Broadcom Inc.
* * Broadcom Proprietary and Confidential. All rights reserved.$
 * *
 * * File:
 * *     phy.c
 * * Purpose:
 * *     Robo2 phy module
 * *
 * */

#include <fsal_int/shared_types.h>
#include <soc/drv.h>
#include <soc/robo.h>
#include <fsal/error.h>
#include <soc/phy/phyctrl.h>

#define DEBUG_MIIM_READ
#undef DEBUG_MIIM_READ

#define CDK_DEBUG_MIIM(x)
#define CDK_CONFIG_MIIM_MAX_POLLS   100000
#define MIIM_PARAM_ID_OFFSET 		16
#define MIIM_PARAM_REG_ADDR_OFFSET	24
#define CDK_XGSM_MIIM_CLAUSE45   0x00000020 /* Select clause 45 access method */

/* CMIC_CMCx_MIIM_STAT(x) */
#define CMIC_MIIM_OPN_DONE              (0x00000001)

/*  CMIC_CMCx_MIIM_CTRL(x) */
#define CMIC_MIIM_WR_START              (0x00000001)
#define CMIC_MIIM_RD_START              (0x00000002)

int phy_debug = 0;
void phy_debug_set(int enable)
{
       phy_debug = enable;
}

int
soc_robo2_miim_int_read(int unit, uint32 phy_addr, uint32 reg, uint16 *val)
{
    uint32              phy_param;
    uint32              phy_miim_addr;
    uint32              internal = 0;
    uint32              regval=0;
    uint32 read_start;
    uint32_t polls;
    int rv = 0;
    int ioerr = 0;
    
    phy_miim_addr = 0;
    phy_param = 0;

    phy_addr &= 0x1F;
    if (phy_addr < 15) {
       internal = 1;
       phy_addr |= 0x200;
    }

    if (reg & 0x003f0000) {
        phy_addr |= CDK_XGSM_MIIM_CLAUSE45;
        reg &= 0x001fffff;
    }

    phy_param = (phy_addr << 16);
    ioerr += REG_WRITE_M7SS_MDIO_CMIC_MIIM_PARAMr(unit, (void *)&phy_param);
    ioerr += REG_WRITE_M7SS_MDIO_CMIC_MIIM_ADDRESSr(unit, (void *)&reg);

    /* Tell CMIC to start */
    read_start = CMIC_MIIM_RD_START;
    ioerr += REG_WRITE_M7SS_MDIO_CMIC_MIIM_CTRLr(unit, (void *)&read_start);

    /* Poll for completion */
    for (polls = 0; polls < CDK_CONFIG_MIIM_MAX_POLLS; polls++) {
        ioerr += REG_READ_M7SS_MDIO_CMIC_MIIM_STATr(unit, &regval);
        if (regval & CMIC_MIIM_OPN_DONE) {
            break;
	}
    }

    /* Check for timeout and error conditions */
    if (polls == CDK_CONFIG_MIIM_MAX_POLLS) {
        rv = -1;
    }

    if (rv >= 0) {
        ioerr = REG_READ_M7SS_MDIO_CMIC_MIIM_READ_DATAr(unit, (void *)&regval);
       *val = (uint16)regval;
    }
    read_start = 0;
    ioerr += REG_WRITE_M7SS_MDIO_CMIC_MIIM_CTRLr(unit, (void *)&read_start);
    if (phy_debug) {
         cli_out ("Reading from phy register : PhyId = 0x%x PhyRegAddr = 0x%x PhyDataRead = 0x%x \n",
                      phy_addr & 0x1F, reg, *val);

    }
    return (ioerr || rv);
}

int
soc_robo2_miim_int_write(int unit, uint32 phy_addr, uint32 reg, uint16 val)
{

    int rv = CBX_E_NONE;
    uint32 internal, regval, phy_param;
    int ioerr = 0;
    uint32 wr_start;
    uint32 polls;


    phy_addr &= 0x1F;
    internal = 0;
    if (phy_addr < 15) {
        internal = 1;
        phy_addr |= 0x200;
    }

    /*
     * Use clause 45 access if DEVAD specified.
     * Note that DEVAD 32 (0x20) can be used to access special DEVAD 0.
     */
    if (reg & 0x003f0000) {
        phy_addr |= CDK_XGSM_MIIM_CLAUSE45;
        reg &= 0x001fffff;
    }


    regval = 0;
    phy_param = 0;

    phy_param = (phy_addr << MIIM_PARAM_ID_OFFSET) | val;

    ioerr += REG_WRITE_M7SS_MDIO_CMIC_MIIM_PARAMr(unit, (void *)&phy_param);
    ioerr += REG_WRITE_M7SS_MDIO_CMIC_MIIM_ADDRESSr(unit, (void *)&reg);

    /* Tell CMIC to start */
    wr_start = CMIC_MIIM_WR_START;
    ioerr += REG_WRITE_M7SS_MDIO_CMIC_MIIM_CTRLr(unit, (void *)&wr_start);

    /* Poll for completion */
    for (polls = 0; polls < CDK_CONFIG_MIIM_MAX_POLLS; polls++) {
        ioerr += REG_READ_M7SS_MDIO_CMIC_MIIM_STATr(unit, &regval);
        if (regval & CMIC_MIIM_OPN_DONE) {
            break;
	}
    }

    /* Check for timeout and error conditions */
    if (polls == CDK_CONFIG_MIIM_MAX_POLLS) {
        rv = -1;
    }
    wr_start = 0;
    ioerr += REG_WRITE_M7SS_MDIO_CMIC_MIIM_CTRLr(unit, (void *)&wr_start);

    if (phy_debug) {
           cli_out("Writing to phy register   : PhyId = 0x%x PhyRegAddr = 0x%x PhyWrData = 0x%x\n",
                                             phy_addr & 0x3F, reg, val);
    }
    return (ioerr || rv);
}

int soc_robo2_pmi_int_write(int unit, uint32 phy_id, uint32 phy_reg_addr,
                       uint16 phy_wr_data)

{
    uint32 reg_data;
    uint32 pmi_phy_reg_addr;
    uint32 done;
    int timeout = 2000;
    uint32  fval, phy_wr_data_val;

    phy_wr_data_val = (phy_wr_data & 0xFFFF);

    if ((phy_id & 0xFF) == 9) {
       pmi_phy_reg_addr = 0x10000 | phy_reg_addr;
       fval = 0;
    } else {
       pmi_phy_reg_addr = phy_reg_addr & 0xFFFEFFFF;
       fval = 1;
    }
    reg_data = 0x0;
    soc_AVR_PORTMACRO_PORT_MACRO_TEMP_CONFIGr_field_set(unit, &reg_data, RESERVED_FOR_ECOf, &fval);
    REG_WRITE_AVR_PORTMACRO_PORT_MACRO_TEMP_CONFIGr(unit, &reg_data);

    reg_data = 0x0;
    soc_AVR_PORTMACRO_PMI_LP_WADDRr_field_set(unit, &reg_data, PMI_LP_WRITE_ADDRESSf, &pmi_phy_reg_addr);
    REG_WRITE_AVR_PORTMACRO_PMI_LP_WADDRr(unit, &reg_data);

    reg_data = 0x0;
    soc_AVR_PORTMACRO_PMI_LP_WDATAr_field_set(unit, &reg_data, PMI_LP_WRITE_DATAf, &phy_wr_data_val);
    REG_WRITE_AVR_PORTMACRO_PMI_LP_WDATAr(unit, &reg_data);

    reg_data = 0x0;
    fval = 1;
    soc_AVR_PORTMACRO_PMI_LP_WRGOr_field_set(unit, &reg_data, PMI_LP_WRITE_GOf, &fval);
    REG_WRITE_AVR_PORTMACRO_PMI_LP_WRGOr(unit, &reg_data);

    done = 0;
    while ( (done == 0) & (timeout > 0) ) {
        reg_data = 0x0;
        REG_READ_AVR_PORTMACRO_PMI_LP_RDWR_STATUSr(unit, &reg_data);
        soc_AVR_PORTMACRO_PMI_LP_RDWR_STATUSr_field_get(unit, &reg_data, PMI_LP_WRITE_DONEf, &done);
        if ( done == 1 ) {
            REG_WRITE_AVR_PORTMACRO_PMI_LP_RDWR_STATUSr(unit, &reg_data);
        }
        timeout--;
    }

    if (timeout == 0 ) {
        cli_out ("ERROR : Timeout while writing to phy register   : PhyId = 0x%x \
                     PhyRegAddr = 0x%x PhyWrData =  0x%x \n",
                     phy_id, pmi_phy_reg_addr, phy_wr_data);
        return -1;
    }
    if (phy_debug) {
        cli_out ("Writing to phy register   : PhyId = 0x%x PhyRegAddr = 0x%x PhyWrData = 0x%x \n",
                (phy_id & 0xFF), pmi_phy_reg_addr, phy_wr_data);
    }
    return 0;
}

int soc_robo2_pmi_int_read(int unit, uint32 phy_id, uint32 phy_reg_addr,
                                      uint16 *phy_rd_data)
{
    uint32 reg_data;
    uint32 pmi_phy_reg_addr;
    uint32 done;
    int timeout = 200;
    uint32  fval;
	
    if ((phy_id & 0xFF) == 9 ) {
        pmi_phy_reg_addr = 0x10000 | phy_reg_addr;
        fval = 0;
    } else {
        pmi_phy_reg_addr = phy_reg_addr & 0xFFFEFFFF;
        fval = 1;
    }
    reg_data = 0x0;
    soc_AVR_PORTMACRO_PORT_MACRO_TEMP_CONFIGr_field_set(unit, &reg_data, RESERVED_FOR_ECOf, &fval);
    REG_WRITE_AVR_PORTMACRO_PORT_MACRO_TEMP_CONFIGr(unit, &reg_data);

    reg_data = 0x0;
    soc_AVR_PORTMACRO_PMI_LP_RADDRr_field_set(unit, &reg_data, PMI_LP_READ_ADDRESSf, &pmi_phy_reg_addr);
    REG_WRITE_AVR_PORTMACRO_PMI_LP_RADDRr(unit, &reg_data);

    reg_data = 0x0;
    fval = 1;
    soc_AVR_PORTMACRO_PMI_LP_RDGOr_field_set(unit, &reg_data, PMI_LP_READ_GOf, &fval);
    REG_WRITE_AVR_PORTMACRO_PMI_LP_RDGOr(unit, &reg_data);

    done = 0;
    while ( (done == 0) & (timeout > 0) ) {
        reg_data = 0x0;
        REG_READ_AVR_PORTMACRO_PMI_LP_RDWR_STATUSr(unit, &reg_data);
        soc_AVR_PORTMACRO_PMI_LP_RDWR_STATUSr_field_get(unit, &reg_data, PMI_LP_READ_DONEf, &done);
        if ( done == 1 ) {
            REG_WRITE_AVR_PORTMACRO_PMI_LP_RDWR_STATUSr(unit, &reg_data);
        }
        timeout--;
    }

    if (timeout == 0 ) {
        cli_out ("ERROR : Timeout while reading from phy register : PhyId = 0x%x PhyRegAddr = 0x%x \n", phy_id, pmi_phy_reg_addr);
        return -1;
    }
    reg_data = 0x0;
    REG_READ_AVR_PORTMACRO_PMI_LP_RDATAr(unit, &reg_data);
    *phy_rd_data = (uint16)reg_data;
    if (phy_debug)
        cli_out ("Reading from phy register : PhyId = 0x%x PhyRegAddr = 0x%x PhyDataRead = 0x%x \n", (phy_id & 0xFF), pmi_phy_reg_addr, reg_data);
    return 0;

}
