/*
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:
 *     drv.c
 * Purpose:
 *     Robo2 switch driver
 */


#include <sal/types.h>
#include <soc/types.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/robo/mcm/tables.h>
#include <soc/robo/mcm/memregs.h>
#include "driver.h"
#include "robo_drv.h"
#include "robo2drv_if.h"
#include "robo2.h"
#include <shared/bsl.h>
#include <assert.h>
#include <soc/robo/robo2/fsal_int/port.h>

#ifdef INCLUDE_ROBO2_AVS
#include <soc/robo/robo2/avs.h>
#include <soc/robo/robo2/crmu_avs.h>
#include <sal/appl/io.h>
#endif

#define SAL_ASSERT  assert
#define SOC_IS_CASCADED(unit) (SOC_INFO(unit).cascaded_mode == 1)

#define ROBO2_REG_READ                  ROBO_REG_READ
#define ROBO2_REG_WRITE                 ROBO_REG_WRITE
#define ROBO2_REG_READ_WITH_ID          ROBO_REG_READ_WITH_ID
#define ROBO2_REG_WRITE_WITH_ID         ROBO_REG_WRITE_WITH_ID
#define SOC_ROBO2_REG_NAME              SOC_ROBO_REG_NAME
#define SOC_ROBO2_MAX_NUM_BLKS          SOC_ROBO_MAX_NUM_BLKS
#define soc_robo2_base_driver_table     soc_robo_base_driver_table
#define soc_robo2_reg_name              soc_robo_reg_name
#define NUM_SOC_ROBO2_REG               NUM_SOC_ROBO_REG
#define XLMAC_CTRLr_ROBO2               XLMAC_CTRLr_ROBO
int soc_robo2_reset(int unit);
int soc_robo2_mgmt_init(int unit);
int _soc_robo2_device_created(int unit);
int soc_robo2_detach(int unit);

extern void bcm53158_initialize_led(int unit);
#ifdef CONFIG_LEDMODE_ONINIT
extern void bcm53158_initialize_led_fixall(int unit, int highlevel, int totalportnum, int ledperport);
#endif
extern int soc_robo2_arl_table_init(void);

#ifdef INCLUDE_ROBO2_AVS
/**
 * It was found that AVS (especially Dynamic AVS) takes a long time
 * to complete the convergence. So it was decided to:
 * (i) Execute the Dynamic AVS only once during the very first boot
 *     after the device is opened from the box.
 * (ii) Store the results of the above AVS in some persistent memory
 *     such as Flash / NVM etc. or a File.
 * (iii) In subsequent boots simply read the stored dac code, apply
 *       temperature compensation (if any) and apply the core voltage.
 *
 * AVS Status is a data structure that contains the necessary
 * information to be stored in the persistent memory.
 *
 * Currently it is decided to store this in a file.
 *
 */
#define AVS_STATUS_INITIALISED_PATTERN 0x55555555
avs_status_t avs_status_primary;
avs_status_t avs_status_secondary;
/* AVS File Pointer */
FILE *avsfp_primary;
FILE *avsfp_secondary;
/* Return Code for AVS / File Operation Routines*/
int ret_primary;
int ret_secondary;
#endif

/*
 * Driver global variables
 *
 *   soc_control: per-unit driver control structure
 *   soc_ndev: the number of units created
 *   soc_ndev_attached: the number of units attached
 */

extern int soc_eth_unit;
extern int soc_eth_ndev;

uint32_t otp_flags = 0;

/*
 * Function:
 *  soc_timesync_init
 * Purpose:
 *  Initialization of clock for modules.
 *
 */
int
soc_timesync_init(int unit) {
    int           i = unit;
    uint32        fval = 0;
    uint32        reg_data;

        /* Remove Reset for Timesync */
        (
          REG_READ_CRU_CRU_IP_RESET_REG_CRUr(i, &reg_data));
        reg_data = reg_data | (0x1 << 5);
        reg_data = reg_data | (0x1 << 6);
        (
          REG_WRITE_CRU_CRU_IP_RESET_REG_CRUr(i, &reg_data));

        /* Enable Common Controls for TS */
        reg_data = 0x0;
        fval = 1;
        soc_CRU_TS_CORE_TIMESYNC_COUNTER_CONFIG_SELECTr_field_set(i, &reg_data,
                                       ENABLE_COMMON_CONTROLf, &fval);
        (
           REG_WRITE_CRU_TS_CORE_TIMESYNC_COUNTER_CONFIG_SELECTr(i, &reg_data));


        reg_data = 0x0;
        fval = 1;
        soc_CRU_TS_CORE_TIMESYNC_TS0_COUNTER_ENABLEr_field_set(i, &reg_data,
                                       ENABLEf, &fval);
        (
          REG_WRITE_CRU_TS_CORE_TIMESYNC_TS0_COUNTER_ENABLEr(i, &reg_data));


        /* We use 500 Mhz in Avenger */
        reg_data = 0x0;
        fval = 0x40000000;
        soc_CRU_TS_CORE_TIMESYNC_TS0_FREQ_CTRL_FRACr_field_set(i, &reg_data,
                                           FRACf, &fval);
        (
              REG_WRITE_CRU_TS_CORE_TIMESYNC_TS0_FREQ_CTRL_FRACr(i, &reg_data));


        reg_data = 0x0;
        fval = 1;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_OUTPUT_ENABLEr_field_set(
                               i, &reg_data, OUTPUT_ENABLEf, &fval);
        (
            REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_OUTPUT_ENABLEr(
                               i, &reg_data));


        reg_data = 0x0;
        fval = 0xf2d;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_UP_EVENT_CTRLr_field_set(
                               i, &reg_data, INTERVAL_LENGTHf, &fval);
        (
           REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_UP_EVENT_CTRLr(
                               i, &reg_data));

        reg_data = 0x0;
        fval = 0xf2d;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_DOWN_EVENT_CTRLr_field_set(
                                         i, &reg_data, INTERVAL_LENGTHf, &fval);
        (
           REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_DOWN_EVENT_CTRLr(
                                                                 i, &reg_data));


        reg_data = 0x0;
        fval = 1;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_CTRLr_field_set(i,
                                                    &reg_data, ENABLEf, &fval);
        (
              REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_CTRLr(i,
                                                                &reg_data));


        reg_data = 0x0;
        fval = 1;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_1_OUTPUT_ENABLEr_field_set(
                                      i, &reg_data, OUTPUT_ENABLEf, &fval);
        (
           REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_1_OUTPUT_ENABLEr(i,
                                                                   &reg_data));


        reg_data = 0x0;
        fval = 8;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_1_UP_EVENT_CTRLr_field_set(
                                       i, &reg_data, INTERVAL_LENGTHf, &fval);
        (
           REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_1_UP_EVENT_CTRLr(i,
                                                                    &reg_data));

        reg_data = 0x0;
        fval = 8;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_1_DOWN_EVENT_CTRLr_field_set(
                                        i, &reg_data, INTERVAL_LENGTHf, &fval);
        (
          REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_1_DOWN_EVENT_CTRLr(
                                        i, &reg_data));


        reg_data = 0x0;
        fval = 1;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_1_CTRLr_field_set(i,
                                                  &reg_data, ENABLEf, &fval);
        (
          REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_1_CTRLr(i,
                                                             &reg_data));

        reg_data = 0x0;
        fval = 1;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_2_OUTPUT_ENABLEr_field_set(
                                      i, &reg_data, OUTPUT_ENABLEf, &fval);
        (
           REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_2_OUTPUT_ENABLEr(i,
                                                                    &reg_data));

        reg_data = 0x0;
        fval = 8;
        /* 0xf42 is 3906 Decimal. This is the half-period so 3906*2 = 7812 ns is the full period.
         * Shaper Clock rate as (1/(7812*10^-9) i.e. 1/0.000,007,812 or 128008.192 Hz, the clock
         * needed to be configured (128 kHz)
         */
        if (SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) {
            fval = 0xf42;
        }
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_2_UP_EVENT_CTRLr_field_set(
                                      i, &reg_data, INTERVAL_LENGTHf, &fval);
        (
           REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_2_UP_EVENT_CTRLr(i,
                                                                    &reg_data));

        reg_data = 0x0;
        fval = 8;
        /* 0xf42 is 3906 Decimal. This is the half-period so 3906*2 = 7812 ns is the full period.
         * Shaper Clock rate as (1/(7812*10^-9) i.e. 1/0.000,007,812 or 128008.192 Hz, the clock
         * needed to be configured (128 kHz)
         */
        if (SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) {
            fval = 0xf42;
        }
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_2_DOWN_EVENT_CTRLr_field_set(
                                     i, &reg_data, INTERVAL_LENGTHf, &fval);
        (
           REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_2_DOWN_EVENT_CTRLr(
                                                             i, &reg_data));


        reg_data = 0x0;
        fval = 1;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_2_CTRLr_field_set(i,
                                                &reg_data, ENABLEf, &fval);
        (
           REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_2_CTRLr(i,
                                                              &reg_data));

        reg_data = 0x0;
        fval = 1;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_3_OUTPUT_ENABLEr_field_set(
                                i, &reg_data, OUTPUT_ENABLEf, &fval);
        (
            REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_3_OUTPUT_ENABLEr(
                                i, &reg_data));


        reg_data = 0x0;
        fval = 8;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_3_UP_EVENT_CTRLr_field_set(
                                i, &reg_data, INTERVAL_LENGTHf, &fval);
        (
           REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_3_UP_EVENT_CTRLr(i,
                                                                    &reg_data));

        reg_data = 0x0;
        fval = 8;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_3_DOWN_EVENT_CTRLr_field_set(
                                   i, &reg_data, INTERVAL_LENGTHf, &fval);
        (
           REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_3_DOWN_EVENT_CTRLr(
                                                              i, &reg_data));

        reg_data = 0x0;
        fval = 1;
        soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_3_CTRLr_field_set(i,
                                      &reg_data, ENABLEf, &fval);
        (
          REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_3_CTRLr(
                                                      i, &reg_data));
    return SOC_E_NONE;
}


int
soc_lcpll2_init(int unit, int is_50mhz)
{

#define LCPLL_STATUS_TS_LCPLL_LOCK            (1 << 12)
#define LCPLL_CONTROL_TS_LCPLL_RESETB         (1 << 31)
#define LCPLL_CONTROL_TS_LCPLL_POSTDIV_RESETB (1 << 30)

    int  timeout_count;
    uint32 lcpll_control = 0x300;
    uint32 lcpll_status = 0;
    uint32 regval = 0;
    uint32 reg_data = 0x0;
    uint32 fval = 1;
    soc_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_CTRLr_field_set(unit,
                                                    &reg_data, ENABLEf, &fval);
    REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_CTRLr(unit, &reg_data);

    /* Program LCPLL Holdover bypass os that CRU controls LCPLL directly */
    regval = 1;
    REG_WRITE_CRU_TS_TOP_TIMESYNC_LCPLL_HOLDOVER_BYPASSr(unit, &regval);

    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL0r(unit, &lcpll_control);

    regval = 0x80000000;
    if (is_50mhz) {
        /* PDIV = 2 for 50Mhz (default) */
        regval |= 0x8000000;
    } else {
        /* Pdiv = 1 for 25Mhz */
        regval |= 0x4000000;
    }
    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL1r(unit, &regval);

    /* Program LCPLL FDIV Upper Bits */
    regval = 0x1F40;
    REG_WRITE_CRU_TS_TOP_TIMESYNC_LCPLL_HOLDOVER_FBDIV_CTRL_UPPERr(unit, &regval);

    /* Program LCPLL FDIV Lower Bits so that entire FDIV write takes effect */
    regval = 0;
    REG_WRITE_CRU_TS_TOP_TIMESYNC_LCPLL_HOLDOVER_FBDIV_CTRL_LOWERr(unit, &regval);

    /* Program CH1 MDIV and CH2 MDIV to get 25M */
    regval = 0x07d1f414;
    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL3r(unit, &regval);

    /* Program bit 62 of {ts_lcpll_pll_ctrl_h, ts_lcpll_pll_ctrl_l}
        which is bit 30 of ts_lcpll_pll_ctrl_h to 0
        to forward the refclk as this bit acts as mux select
     */
    regval = 0x0010047D;
    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL4r(unit, &regval);
    regval = 0x00805804;
    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL5r(unit, &regval);
    regval = 0x40000014;
    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL6r(unit, &regval);
    regval = 0xF1F0203D;
    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL7r(unit, &regval);

    /* ts_lcpll_msc_ctrl[8:7] should be enabled so that QSGMII get clocks.
       Hence reset value of 0x80 is changed to 0x180.
     */
    regval =  0x00260180;
    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL8r(unit, &regval);
    regval =  0x0;
    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL9r(unit, &regval);
    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL10r(unit, &regval);

    /* Out of reset */
    lcpll_control |= LCPLL_CONTROL_TS_LCPLL_RESETB;
    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL0r(unit, &lcpll_control);

    timeout_count = 100;
    /* Wait for PLL Lock */
    do {
        /* Read the LCPLL Status */
        REG_READ_CRU_TS_TOP_TS_LCPLL_STATUSr(unit, &lcpll_status);
        if (lcpll_status & LCPLL_STATUS_TS_LCPLL_LOCK) {
            break;
        }
        sal_usleep(100);
    } while(timeout_count--);

#ifndef EMULATION_BUILD
    if (timeout_count < 0) {
        return SOC_E_INIT;
    }
#endif

    lcpll_control |= LCPLL_CONTROL_TS_LCPLL_POSTDIV_RESETB;
    REG_WRITE_CRU_TS_TOP_TS_LCPLL_CONTROL0r(unit, &lcpll_control);
    return SOC_E_NONE;
}

/*
 * Function:
 *    soc_spi_master_enable
 * Purpose:
 *    Enable the SPI master
 * Parameters:
 *    None
 * Returns:
 *    None
 */
void
soc_spi_master_enable()
{
    uint32 regval = 0;
    uint32 fval   = 1;
    int    unit   = 0;

    /* Reset SPIM */
    REG_WRITE_M7SS_IDM_SPIM_IDM_RESET_CONTROLr(unit, &fval);
    fval = 0;
    REG_WRITE_M7SS_IDM_SPIM_IDM_RESET_CONTROLr(unit, &fval);

    if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_B0) {
    /* Default in B0 is selected as GPIO unlike in A0.
     * In order to use SPI as same way as A0, MFIO_control_register_2 has to be
     * programmed to select the SS coming from SPIM.
     * The selection is in MFIO_control_register_2 bit 0*/
        fval   = 1;
        REG_WRITE_CRU_CRU_MFIO_CONTROL_REGISTER_2r(unit, &fval);
    }
    /* Initialize SPI Master */
    REG_READ_CRU_CRU_PLL1_POST_DIVIDER_CH_CTRL2r(unit, &regval);
    soc_CRU_CRU_PLL1_POST_DIVIDER_CH_CTRL2r_field_get(unit, &regval, POST_DIV_FOR_CH1f, &fval);
    if (fval == 0xA) {
        fval = 0x887;
    } else {
        fval = 0x587;
    }
    /*dbg_printf("Cascade Fval=%x\n", fval);*/
    regval = 0;
    REG_WRITE_M7SS_M7SC_RACRr(unit, &regval);

    /* Spi clock pre-divisor */
    regval = 2;
    REG_WRITE_M7SS_SPI0_SSPCPSRr(unit, &regval);

    /* Spi clock divisor, Data size and Spi mode */
    if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_B0) {
    /* The cpol/cpha of avenger has changed from A0 to B0.
     * For SPI-S:
     *     A0 is CPOL=0; CPHA=1
     *     B0 is CPOL=1; CPHA=1
     * Change the bit 6 of SSPCR0 in order for SPI master to talk to the SPIS */
        fval |= 0x40;
    }
    REG_WRITE_M7SS_SPI0_SSPCR0r(unit, &fval);

    /* Spi SSE enable */
    regval = 2;
    REG_WRITE_M7SS_SPI0_SSPCR1r(unit, &regval);

    /* Remote access */
    regval = 7;
    REG_WRITE_M7SS_M7SC_RACRr(unit, &regval);
}

#if 0
/*
 * Function:
 *    soc_dev_check
 * Purpose:
 *    Check the Chip ID
 * Parameters:
 *    ChipID, RevID
 * Returns:
 *    SOC_E_NONE - Found valid device
 *    SOC_E_NOTFOUND - Not valid device
 */
static int
soc_dev_check(int unit, uint32 chipid, uint32 revid)
{
    /*dbg_printf("Unit %d: ChipID: %d Rev %d\n", unit, chipid, revid);*/
    switch (chipid) {
    case BCM53112_DEVICE_ID:
    case BCM53156_DEVICE_ID:
    case BCM53157_DEVICE_ID:
    case BCM53158_DEVICE_ID:
    case BCM53161_DEVICE_ID:
    case BCM53162_DEVICE_ID:
        return 0;
    }
    /*dbg_printf("Unit %d: ChipID verfication failed\n", unit);*/
    return 1;
}
#endif

int
soc_cb_init(int unit)
{
    uint32 regval = 0;
    uint32 fval = 0;
    uint32 done;
    uint8_t timeout = 200;

    /* Reset CB */
    REG_READ_CRU_CRU_IP_SYSTEM_RESET4r(unit, &regval);
    regval &=  ~(0x1 << 2);  /* CB Reset */
    regval &=  ~(0x1 << 7);  /* CB Reset */
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET4r(unit, &regval);
    regval = regval | (0x1 << 2);  /* CB Reset */
    regval = regval | (0x1 << 7);  /* CB Reset */
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET4r(unit, &regval);

    /* Initialize LLC free page list */
    regval = 0x0;
    fval = 1;
    soc_CB_BMU_LLC_SELF_INITr_field_set(unit, (uint32 *)&regval, SELF_INITf, &fval);
    soc_CB_BMU_LLC_SELF_INITr_field_set(unit, (uint32 *)&regval, START_PAGEf, &fval);
    fval = 4000;
    soc_CB_BMU_LLC_SELF_INITr_field_set(unit, (uint32 *)&regval, END_PAGEf, &fval);
    REG_WRITE_CB_BMU_LLC_SELF_INITr(unit, (uint32 *)&regval);

    /* Initialize LLC control */
    regval = 0x0;
    fval = 1;
    soc_CB_BMU_LLC_CONTROLr_field_set(unit, (uint32 *)&regval, LLC_ENABLEf, &fval);
    REG_WRITE_CB_BMU_LLC_CONTROLr(unit, (uint32 *)&regval);

    done = 0;
    while ( (done == 0) & (timeout > 0) ) {
        regval = 0x0;
        REG_READ_CB_BMU_LLC_STATUSr(unit, (uint32 *)&regval);
        soc_CB_BMU_LLC_STATUSr_field_get(unit , (uint32 *)&regval, SELF_INIT_DONEf, &done);
        timeout--;
    }
    /* Write back to clear self init done */
    REG_WRITE_CB_BMU_LLC_STATUSr(unit, (uint32 *)&regval);

    return SOC_E_NONE;
}

int
soc_pll1_init(int unit)
{
    uint32 clkmux, regval, fval;

    REG_READ_CRU_CRU_PLL1_POST_DIVIDER_CH_CTRL2r(unit, &regval);
    soc_CRU_CRU_PLL1_POST_DIVIDER_CH_CTRL2r_field_get(
            unit, &regval, POST_DIV_FOR_CH1f, &fval);
    /*dbg_printf("Unit %d: PLL1 CH1 POSTDIV %x\n", unit, fval);*/
    if (unit == 0) {
        /* Set NIC to 250Mhz on Master, Fval=16 */
        fval = 16;
        /*fval = 10; */
        soc_CRU_CRU_PLL1_POST_DIVIDER_CH_CTRL2r_field_set(
                unit, &regval, POST_DIV_FOR_CH1f, &fval);
        REG_READ_CRU_CRU_CLK_MUX_SELr(unit, &clkmux);
        /* Turn off Channel 1 */
        /* Workaround to fix ROBO-389 */
   /*     clkmux &= ~0x2; */
        SOC_IF_ERROR_RETURN(
            REG_WRITE_CRU_CRU_CLK_MUX_SELr(unit, &clkmux));
        SOC_IF_ERROR_RETURN(
            REG_WRITE_CRU_CRU_PLL1_POST_DIVIDER_CH_CTRL2r(unit, &regval));
        /* Turn on Channel 1 */
        clkmux |= 0x2; /* Channel 1 */
        SOC_IF_ERROR_RETURN(
            REG_WRITE_CRU_CRU_CLK_MUX_SELr(unit, &clkmux));
    }
    return SOC_E_NONE;
}

int soc_otp_status_read(int unit)
{
    uint32 regval = 0;

/* #define OTP_BCM53161 */
/* #define OTP_BCM53156 */
#ifdef OTP_BCM53161

    /* OTP override for software testing only */
    regval = 0x218;   /* Disable 0to3, 4to7 gigi ports. QSGMII_config=sgmii_mode (01) */
    REG_WRITE_CRU_CRU_OTP_OVERRIDE_VALUE0_REGISTERr(unit, &regval);
    REG_WRITE_CRU_CRU_OTP_OVERRIDE_EN0_REGISTERr(unit, &regval);

    regval = 0x80000000;
    REG_WRITE_CRU_CRU_OTP_OVERRIDE_VALUE1_REGISTERr(unit, &regval);
    REG_WRITE_CRU_CRU_OTP_OVERRIDE_EN1_REGISTERr(unit, &regval);

    regval = 0x000CFA9;
    REG_WRITE_CRU_CRU_OTP_OVERRIDE_VALUE3_REGISTERr(unit, &regval);
    REG_WRITE_CRU_CRU_OTP_OVERRIDE_EN3_REGISTERr(unit, &regval);
#endif

#ifdef OTP_BCM53156
    /* OTP override for software testing only */
    regval = 0x00000620;   /* Disable port12. QSGMII_config=disabled (11) */
    REG_WRITE_CRU_CRU_OTP_OVERRIDE_VALUE0_REGISTERr(unit, &regval);
    REG_WRITE_CRU_CRU_OTP_OVERRIDE_EN0_REGISTERr(unit, &regval);

    regval = 0x80000000;
    REG_WRITE_CRU_CRU_OTP_OVERRIDE_VALUE1_REGISTERr(unit, &regval);
    REG_WRITE_CRU_CRU_OTP_OVERRIDE_EN1_REGISTERr(unit, &regval);

    regval = 0x40000;
    REG_WRITE_CRU_CRU_OTP_OVERRIDE_VALUE3_REGISTERr(unit, &regval);
    REG_WRITE_CRU_CRU_OTP_OVERRIDE_EN3_REGISTERr(unit, &regval);

#endif

    /* ****  Read OTP values ***** */
    REG_READ_CRU_CRU_OTP_STATUS_VALUE_REGISTER0r(unit, &regval);
    otp_flags |= ((regval & 0x1) ? SOC_OTP_FLAG_GPHY_1G_DIS: 0);
    otp_flags |= ((regval & 0x4) ? SOC_OTP_FLAG_RGMII_DIS: 0);
    otp_flags |= ((regval & 0x8) ? SOC_OTP_FLAG_GPHY_0_3_DIS: 0);
    otp_flags |= ((regval & 0x10) ? SOC_OTP_FLAG_GPHY_4_7_DIS: 0);
    otp_flags |= ((regval & 0x20) ? SOC_OTP_FLAG_XFI_13_DIS: 0);
    otp_flags |= ((regval & 0x40) ? SOC_OTP_FLAG_XFI_12_DIS: 0);
    otp_flags |= (((regval & 0x600) == 0x000) ? SOC_OTP_FLAG_QSG_MODE_QSG: 0);
    otp_flags |= (((regval & 0x600) == 0x200) ? SOC_OTP_FLAG_QSG_MODE_SGM: 0);
    otp_flags |= (((regval & 0x600) == 0x600) ? SOC_OTP_FLAG_QSGMII_DIS: 0);
    otp_flags |= ((regval & 0x1000) ? SOC_OTP_FLAG_SGMII_2G5_DIS: 0);
    otp_flags |= ((regval & 0x2000) ? SOC_OTP_FLAG_SGMII_1G_DIS: 0);

    REG_READ_CRU_CRU_OTP_STATUS_VALUE_REGISTER1r(unit, &regval);
    otp_flags |= ((regval & 0x8) ? SOC_OTP_FLAG_ARL_SIZE_8K: 0);
    otp_flags |= ((regval & 0x10) ? SOC_OTP_FLAG_CFP_DIS: 0);

    REG_READ_CRU_CRU_OTP_STATUS_VALUE_REGISTER2r(unit, &regval);
    if (regval == 0x0C00CFA2) {
        otp_flags |= SOC_OTP_FLAG_PORT_9_DISABLE;
    }

    REG_READ_CRU_CRU_OTP_STATUS_VALUE_REGISTER3r(unit, &regval);
    otp_flags |= ((regval & 0x00400000) ? SOC_OTP_FLAG_XLPORT_DISABLE: 0);
    otp_flags |= ((regval & 0x00800000) ? SOC_OTP_FLAG_XFI_13_MAX_2G5: 0);
    otp_flags |= ((regval & 0x01000000) ? SOC_OTP_FLAG_XFI_12_MAX_2G5: 0);

    sal_printf("OTP_FLAGS = 0x%x\n", otp_flags);

    return SOC_E_NONE;
}

int
soc_bootstrap_init(int unit)
{
    uint32 regval = 0;
    uint32 straps = 0;
    uint32   fval = 1;
    int is_50mhz;
    uint32 reg_data[2];
    uint32 speed;
    soc_pbmp_t pbmp_valid = 0;

    speed  = soc_property_port_get(unit, 13, spn_PORT_INIT_SPEED, 10312);
    /*
     * pbmp_valid is a bitmap of all ports that exist on the unit.
     */
    pbmp_valid = soc_property_get_pbmp(unit, spn_PBMP_VALID, 0);


    REG_READ_CRU_CRU_CHIP_ID_REGr(unit, &regval);
    regval &= 0xFFFFFF; /* 24 bit chipid */
    REG_READ_CRU_CRU_REVISION_ID_REGr(unit, &fval);
    fval &= 0xFF; /* 8 bit revid */
   /*  SOC_IF_ERROR_RETURN(soc_dev_check(unit, regval, fval)); */

    REG_READ_CRU_CRU_STRAP_STATUS_REGr(unit, &straps);
    sal_printf("Unit %d: Straps: %08x\n", unit, straps);

    /* Set PLL1 */
    soc_pll1_init(unit);

    /* Enable lcpll2 */
    is_50mhz = (straps >> 4) & 1;
    SOC_IF_ERROR_RETURN(soc_lcpll2_init(unit, is_50mhz));

#ifdef CONFIG_EXTERNAL_HOST
    regval = 0x0;
#else
    /* Do not reset CB */
    regval = 0x84;
#endif
    /* Disable all peripherals */
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET4r(unit, &regval);
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET5r(unit, &regval);

#ifdef CONFIG_EXTERNAL_HOST
    /* External Host, init CB regardless of unit */
    soc_cb_init(unit);
#else
    /* Internal Host, init CB only on cascade */
    if ((unit == 1) && (SOC_IS_CASCADED(0))) {
        sal_printf("Unit 1 cb init\n");
        soc_cb_init(1);
    }
#endif

    /* set  CRU_CRU_MFIO_control_register */
    regval = 0x59996655;
    REG_WRITE_CRU_CRU_MFIO_CONTROL_REGISTERr(unit, &regval);

    REG_READ_CRU_CRU_PAD_CONTROL_PUP_REGISTER_4r(unit, (uint32 *)&regval);
    regval = regval | (0x1 << 15);
    REG_WRITE_CRU_CRU_PAD_CONTROL_PUP_REGISTER_4r(unit, (uint32 *)&regval);
    regval = 0x59992655;
    REG_WRITE_CRU_CRU_MFIO_CONTROL_REGISTERr(unit, &regval);

    /* Workaround for Avenger-A0 : Issue with Return FIFO LLC Error*/
    REG_READ_CB_BMU_LLC_CONTROLr(unit, (uint32 *)&regval);
    fval = 1;
    soc_CB_BMU_LLC_CONTROLr_field_set(unit , (uint32 *)&regval, LLC_MC_MODEf , &fval);
    REG_WRITE_CB_BMU_LLC_CONTROLr(unit, (uint32 *)&regval);

#ifdef CONFIG_EXTERNAL_HOST
    regval = 0x000C23FF;
#else
    regval = 0xFFFFFFFF;
#endif
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESETr(unit, &regval);
    regval = 0;
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET2r(unit, &regval);
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET3r(unit, &regval);
    regval = 0xFFFFFFFF;
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET2r(unit, &regval);
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET3r(unit, &regval);
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET4r(unit, &regval);
    REG_WRITE_CRU_CRU_IP_SYSTEM_RESET5r(unit, &regval);

    /* ARL table init is done here as a work around to avoid the
      port ARL count getting to max value immediately after the power on */
    soc_robo2_arl_table_init();
    SOC_IF_ERROR_RETURN(soc_timesync_init(unit));

    /* reset XLMAC */
    if (otp_flags & SOC_OTP_FLAG_XLPORT_DISABLE) {
        /* NO XLMAC */
    } else {
        uint32 xl_reg_data[2];
        xl_reg_data[0] = 0 ;
        xl_reg_data[1] = 0 ;
        fval = 1;
        SOC_IF_ERROR_RETURN(REG_READ_XLPORT_MAC_CONTROLr(unit, (uint32 *)&xl_reg_data));
        soc_XLPORT_MAC_CONTROLr_field_set(unit , (uint32 *)&xl_reg_data, XMAC0_RESETf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_MAC_CONTROLr(unit, &xl_reg_data));
        fval = 0;
        soc_XLPORT_MAC_CONTROLr_field_set(unit , (uint32 *)&xl_reg_data, XMAC0_RESETf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_MAC_CONTROLr(unit, &xl_reg_data));

        fval = 0;
        SOC_IF_ERROR_RETURN(REG_READ_XLPORT_XGXS0_CTRL_REGr(unit, &xl_reg_data));
        soc_XLPORT_XGXS0_CTRL_REGr_field_set(unit, (uint32 *)&xl_reg_data, RSTB_HWf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_XGXS0_CTRL_REGr(unit, &xl_reg_data));
        fval = 1;
        soc_XLPORT_XGXS0_CTRL_REGr_field_set(unit, (uint32 *)&xl_reg_data, RSTB_HWf, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_XGXS0_CTRL_REGr(unit, &xl_reg_data));
    }

    /* PVT mon */
    regval = 0x58;
    REG_WRITE_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr(unit, &regval);
    regval = 0x78;
    REG_WRITE_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr(unit, &regval);
    regval = 0x68;
    REG_WRITE_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr(unit, &regval);
    regval = 0x48;
    REG_WRITE_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr(unit, &regval);
    regval = 0x68;
    REG_WRITE_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr(unit, &regval);
    regval = 0xe8;
    REG_WRITE_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr(unit, &regval);
    regval = 0xa8;
    REG_WRITE_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr(unit, &regval);

    if (otp_flags & SOC_OTP_FLAG_XFI_13_DIS) {
        REG_READ_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr(unit, &regval);
        fval = 1;
        soc_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr_field_set(unit, &regval,
                                                     PM_TSC_CLK_MUX_SELECTf, &fval);
        REG_WRITE_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr(unit, &regval);
    }

    /* Use the PM core clock when ports 12 and 13 are disabled */
    if (!(pbmp_valid & CBX_PORT_PBMP_10G_PORTS)) {
        REG_READ_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr(unit, &regval);
        fval = 2;
        soc_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr_field_set(unit, &regval,
                                                     PM_TSC_CLK_MUX_SELECTf, &fval);
        REG_WRITE_CRU_CRU_QSGMII_PM_CONTROL_REGISTERr(unit, &regval);
    }

    if (otp_flags & SOC_OTP_FLAG_XLPORT_DISABLE) {
        /* NO XLMAC */
    } else {
        reg_data[0] = 0 ;
        reg_data[1] = 0 ;
        fval = 1;
        SOC_IF_ERROR_RETURN(REG_READ_XLPORT_SOFT_RESETr(unit, &regval));
        soc_XLPORT_SOFT_RESETr_field_set(unit, (uint32 *)&regval, PORT2f, &fval);
        soc_XLPORT_SOFT_RESETr_field_set(unit, (uint32 *)&regval, PORT3f, &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_SOFT_RESETr(unit, &regval));

        reg_data[0] = 0 ;
        fval = 1;
        SOC_IF_ERROR_RETURN(REG_READ_XLPORT_ENABLE_REGr(unit, (uint32 *)&reg_data[0]));
        soc_XLPORT_ENABLE_REGr_field_set(unit , (uint32 *)&reg_data, PORT0f , &fval);
        soc_XLPORT_ENABLE_REGr_field_set(unit , (uint32 *)&reg_data, PORT1f , &fval);
        soc_XLPORT_ENABLE_REGr_field_set(unit , (uint32 *)&reg_data, PORT2f , &fval);
        soc_XLPORT_ENABLE_REGr_field_set(unit , (uint32 *)&reg_data, PORT3f , &fval);
        SOC_IF_ERROR_RETURN(REG_WRITE_XLPORT_ENABLE_REGr(unit, (uint32 *)&reg_data[0]));
        reg_data[0] = 0 ;
        SOC_IF_ERROR_RETURN(REG_READ_XLPORT_ENABLE_REGr(unit,  (uint32 *)&reg_data[0]));

        REG_READ_AVR_PORTMACRO_PORT_MACRO_SPEED_CONFIGr(unit, &regval);
        /* Lane0 * [bit * 2:0] * = * 0x4, * lane1 * [bit * 6:4] * = * 4 * ;
        * configure * 10G 10M : 'b000 100M : 'b001 1G : 'b010 2.5G : 'b011 10G : 'b100 */
        if (speed == 1000) {
            regval = (regval & 0xFFFFFF00) | 0x22;
        } else if (speed == 2500) {
            regval = (regval & 0xFFFFFF00) | 0x33;
        } else {
            regval = (regval & 0xFFFFFF00) | 0x44;
        }
#if (CONFIG_XFI_SPEED == 1000)
        regval = (regval & 0xFFFFFF00) | 0x22;
#endif
#if (CONFIG_XFI_SPEED == 2500)
        regval = (regval & 0xFFFFFF00) | 0x33;
#endif
        REG_WRITE_AVR_PORTMACRO_PORT_MACRO_SPEED_CONFIGr(unit, &regval);

        regval = 0;
        if (straps & 1) {
            REG_READ_M7SS_MDIO_CMIC_MIIM_SCAN_CTRLr(unit, &regval);
            regval |= (1 << 28);
            REG_WRITE_M7SS_MDIO_CMIC_MIIM_SCAN_CTRLr(unit, &regval);
        }
    }

#ifdef CONFIG_CASCADED_MODE
    if (unit == 0) {
/*
#ifndef CONFIG_EXTERNAL_HOST
*/
#if 0
        /* M7 Bootfrom flash can process straps */
        if (1 == ((straps >> 7) & 0x3)) {
             /* Cascaded Primary device  */
             dbg_printf("Unit %d: Cascade Configuration: Primary device \n", unit);
             soc_spi_master_enable();
             /* Attempt to access the Slave before confirming cascade operation */
             if (SOC_E_NONE == REG_READ_CRU_CRU_CHIP_ID_REGr(unit, &regval)) {
                 SOC_INFO(unit).cascaded_mode = 1;
                 dbg_printf("Cascade Configuration Enabled\n");
                 SOC_INFO(unit).cascade_ports = CBX_CASCADE_PORTS_MASTER;
             } else {
                 dbg_printf("Cascade Config Not Enabled (Failed to detect Slave)\n");
             }
        }
#else
        /* External CPU mode cannot process straps */
        soc_spi_master_enable();
        /* Attempt to access the Slave before confirming cascade operation */
        if (SOC_E_NONE == REG_READ_CRU_CRU_CHIP_ID_REGr(unit, &regval)) {
            SOC_INFO(unit).cascaded_mode = 1;
            sal_printf("Cascade Configuration Enabled\n");
            SOC_INFO(unit).cascade_ports = CBX_CASCADE_PORTS_MASTER;
        } else {
            sal_printf("Cascade Configuration Not Enabled (Failed to detect Slave)\n");
        }
#endif
        regval = 0;
        REG_WRITE_CB_IPP_IPP_CONFIGr(unit, &regval);
    } else {
        regval = 1;
        REG_WRITE_CB_IPP_IPP_CONFIGr(unit, &regval);
        SOC_INFO(unit).cascade_ports = CBX_CASCADE_PORTS_SLAVE;
    }
#if 0

    /* Cannot Use Straps in A0: */

     if (1 == ((straps >> 7) & 0x3)) {
         /* Cascaded Primary device  */
         dbg_printf("Unit %d: Cascade Configuration: Primary device \n", unit);
         soc_spi_master_enable();
         /* Attempt to access the Slave before confirming cascade operation */
         if (SOC_E_NONE == REG_READ_CRU_CRU_CHIP_ID_REGr(unit, &regval)) {
              SOC_INFO(unit).cascaded_mode = 1;
              dbg_printf("Cascade Configuration Enabled\n");
              SOC_INFO(unit).cascade_ports = CBX_CASCADE_PORTS_MASTER;
         } else {
              dbg_printf("Cascade Configuration Not Enabled (Failed to detect Slave)\n");
         }
         regval = 0;
         REG_WRITE_CB_IPP_IPP_CONFIGr(unit, &regval);
         }
     }
     if (2 == ((straps >> 7) & 0x3)) {
         /* Cascaded Secondary device */
         dbg_printf("Unit %d: Cascade Configuration: Secondary device \n", unit);
         regval = 1;
         REG_WRITE_CB_IPP_IPP_CONFIGr(unit, &regval);
         SOC_INFO(unit).cascade_ports = CBX_CASCADE_PORTS_SLAVE;
     }

#endif
#endif /* CONFIG_CASCADE_MODE */
    return SOC_E_NONE;
}
/*
 * Function:
 *  soc_robo2_power_down_config
 * Purpose:
 *      Disable/Enable PORTX power down mode
 * Parameters:
 *  valid_pbmp - pbmp to disable power down mode (PHY is enable)
 */
int
soc_robo2_power_down_config(int unit, soc_pbmp_t valid_pbmp)
{
    return SOC_E_NONE;
}

static int
soc_robo2_board_led_init(int unit)
{
    bcm53158_initialize_led(unit);
    return SOC_E_NONE;
}

/*
 * Function:
 *  robo2_soc_chip_find
 * Purpose:
 *      Return the SOC_E_NONE for a chip if found.
 *      If not found, return SOC_E_NOT_FOUND.
 * Parameters:
 *  unit - RoboSwitch unit number.
 * Returns:
 *  Return the SOC_E_NONE for a chip if found, else SOC_E_NOT_FOUND.
 */
int
soc_robo2_chip_find(int unit)
{
    int         i;
    soc_driver_t    *d;
    uint16      dev_id;
    uint8       rev_id;
    uint16      driver_dev_id;
    uint8       driver_rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    if (soc_cm_get_id_driver(dev_id, rev_id,
                             &driver_dev_id, &driver_rev_id) < 0) {
        return SOC_E_NOT_FOUND;
    }

    /*
     * Find driver in table.  In theory any IDs returned by
     * soc_cm_id_to_driver_id() should have a driver in the table.
     */
    for (i = 0; i < SOC_ROBO2_NUM_SUPPORTED_CHIPS; i++) {
        d = soc_robo2_base_driver_table[i];
        if ((d != NULL) &&
                (d->block_info != NULL) &&
                (d->pci_device == driver_dev_id) &&
                /* An exact match for metal spin is not required. */
                ((d->pci_revision == driver_rev_id) ||
                ((d->pci_revision & 0xf0) == (driver_rev_id & 0xf0)))) {
            return SOC_E_NONE;
        }
    }

    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("soc_chip_find: driver in devid table "
                        "not in soc_robo2_base_driver_table\n")));

    return SOC_E_NOT_FOUND;
}

/*
 * Function:
 *  robo2_soc_chip_driver_find
 * Purpose:
 *      Return the soc_driver for a chip if found.
 *      If not found, return NULL.
 * Parameters:
 *  pci_dev_id - PCI dev ID to find (exact match)
 *  pci_rev_id - PCI dev ID to find (exact match)
 * Returns:
 *  Pointer to static driver structure
 */
static soc_driver_t *
soc_robo2_chip_driver_find(uint16 pci_dev_id, uint8 pci_rev_id)
{
    int         i;
    soc_driver_t    *d;
    uint16      driver_dev_id = 0;
    uint8       driver_rev_id = 0;

    if (soc_cm_get_id_driver(pci_dev_id, pci_rev_id,
                             &driver_dev_id, &driver_rev_id) < 0) {
        return NULL;
    }

    /*
     * Find driver in table.  In theory any IDs returned by
     * soc_cm_id_to_driver_id() should have a driver in the table.
     */
    for (i = 0; i < SOC_ROBO2_NUM_SUPPORTED_CHIPS; i++) {
        d = soc_robo2_base_driver_table[i];
        if ((d != NULL) &&
                (d->block_info != NULL) &&
                (d->pci_device == driver_dev_id) &&
                /* An exact match for metal spin is not required. */
                ((d->pci_revision == driver_rev_id) ||
                ((d->pci_revision & 0xf0) == (driver_rev_id & 0xf0)))) {
            return d;
        }
    }

    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META("soc_chip_driver_find: driver in devid table "
                        "not in soc_robo_base_driver_table\n")));

    return NULL;
}

/*
 * Function:
 *  soc_info_config
 * Parameters:
 *  unit - RoboSwitch unit number.
 *  soc  - soc_control_t associated with this unit
 * Purpose:
 *  Fill in soc_info structure for a newly attached unit.
 *  Generates bitmaps and various arrays based on block and
 *  ports that the hardware has enabled.
 */

static void
soc_robo2_info_config(int unit, soc_control_t *soc)
{
    soc_info_t      *si;
    soc_robo_control_t   *robo;
    soc_pbmp_t      pbmp_valid = 0;
    soc_pbmp_t      pbmp_xport_ge;  /* XL port 2.5 Gbps capable */
    uint16      dev_id;
    uint8       rev_id;
    uint16      drv_dev_id = 0;
    uint8       drv_rev_id = 0;
    int         port, blk = 0, bindex = 0, pno = 0;
    char        *bname;
    int         blktype;
    int         disabled_port;

    si = &soc->info;
    sal_memset((void *)si, 0, sizeof(soc_info_t));

    robo = SOC_ROBO_CONTROL(unit);

#ifdef CONFIG_CASCADED_MODE
    soc_cm_get_id(0, &dev_id, &rev_id);
#else
    soc_cm_get_id(unit, &dev_id, &rev_id);
#endif /* CONFIG_CASCADE_MODE */
    soc_cm_get_id_driver(dev_id, rev_id, &drv_dev_id, &drv_rev_id);

#ifdef CONFIG_CASCADED_MODE
    if (CMDEV(0).dev.info->dev_type & SOC_SPI_DEV_TYPE) {
#else
    if (CMDEV(unit).dev.info->dev_type & SOC_SPI_DEV_TYPE) {
#endif /* CONFIG_CASCADE_MODE */
        si->spi_device = TRUE;
    }

    si->driver_type = soc->chip_driver->type;
    si->driver_group = soc_chip_type_map[si->driver_type];
    si->num_cpu_cosq = 1;
    si->port_addr_max = 31;
    si->modid_count = 1;
    /* there is no Stacking solution for ROBO2 devices */
    si->modid_max = 1;    /* See SOC_MODID_MAX(unit) */

    SOC_PBMP_CLEAR(si->s_pbm);  /* 10/100/1000/2500 Mbps comboserdes */
    SOC_PBMP_CLEAR(si->gmii_pbm);
    /* Defaults for the xport settings */
    SOC_PBMP_ASSIGN(pbmp_xport_ge, 0xf00);

    /*
     * pbmp_valid is a bitmap of all ports that exist on the unit.
     */
    pbmp_valid = soc_property_get_pbmp(unit, spn_PBMP_VALID, 1);

#ifdef CONFIG_CASCADED_MODE
    if(unit  == 1)
    {
        pbmp_valid = 0x3fff;
    }
#endif /* CONFIG_CASCADE_MODE */

    /*
     * Used to implement the SOC_IS_*(unit) macros
     */
    switch (drv_dev_id) {
    case BCM53158_DEVICE_ID:
        robo->chip_type = SOC_ROBO_CHIP_53158;
        robo->arch_type = SOC_ROBO_ARCH_ROBO2;
        robo->rev_id = drv_rev_id;
        si->num_cpu_cosq = 8;
        /* Port 8,9,10,11 are 2.5G ports */
        SOC_PBMP_ASSIGN(pbmp_xport_ge, 0xf00);
        break;
    case BCM53154_DEVICE_ID:
        robo->chip_type = SOC_ROBO_CHIP_53154;
        robo->arch_type = SOC_ROBO_ARCH_ROBO2;
        robo->rev_id = drv_rev_id;
        si->num_cpu_cosq = 8;
        break;
    default:
        si->chip = 0;
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                  "soc_info_config: driver device %04x unexpected\n"),
                  drv_dev_id));
        break;
    }

#if 0 /* Enable this after 2.5G ports pbmp is provided through config.bcm */
    pbmp_xport_ge = soc_property_get_pbmp_default(unit, spn_PBMP_XPORT_GE,
                                                  pbmp_xport_ge);
#endif
    si->ipic_port = -1;
    si->ipic_block = -1;
    si->exp_port = -1;
    si->exp_block = -1;
    si->spi_port = -1;
    si->spi_block = -1;
    si->fe.min = si->fe.max = -1;
    si->ge.min = si->ge.max = -1;
    si->xe.min = si->xe.max = -1;
    si->ether.min = si->ether.max = -1;
    si->port.min = si->port.max = -1;
    si->all.min = si->all.max = -1;
    sal_memset(si->has_block, 0, sizeof(soc_block_t) * COUNTOF(si->has_block));

    LOG_WARN(BSL_LS_SOC_COMMON,
             (BSL_META_U(unit,
              "count of has_block: %d\n"),
              COUNTOF(si->has_block)));

    for (blk = 0; blk < SOC_ROBO2_MAX_NUM_BLKS; blk++) {
        si->block_port[blk] = REG_PORT_ANY;
    }

    for (port = 0; ; port++) {
        disabled_port = FALSE;
        blk = SOC_PORT_INFO(unit, port).blk;
        bindex = SOC_PORT_INFO(unit, port).bindex;
        if (blk < 0 && bindex < 0) {     /* end of list */
            break;
        }
        if (blk < 0) {                  /* empty slot */
            disabled_port = TRUE;
            blktype = 0;
        } else {
            blktype = SOC_BLOCK_INFO(unit, blk).type;
            if (!SOC_PBMP_MEMBER(pbmp_valid, port)) {   /* disabled port */
                if (SOC_BLOCK_IN_LIST(&blktype, SOC_BLK_CPU)) {
                    LOG_WARN(BSL_LS_SOC_COMMON,
                             (BSL_META_U(unit,
                                         "soc_info_config: "
                                         "cannot disable cpu port\n")));
                } else {
                    disabled_port = TRUE;
                }
            }
        }

        if (disabled_port) {
            sal_snprintf(si->port_name[port], sizeof(si->port_name[port]),
                         "?%d", port);
            si->port_offset[port] = port;
            continue;
        }


#define ADD_PORT(ptype, port) \
            si->ptype.port[si->ptype.num++] = port; \
            if (si->ptype.min < 0) { \
            si->ptype.min = port; \
            } \
            if (port > si->ptype.max) { \
            si->ptype.max = port; \
            } \
            SOC_PBMP_PORT_ADD(si->ptype.bitmap, port);

        bname = soc_block_port_name_lookup_ext(blktype, unit);
        switch (blktype) {
        case SOC_BLK_UNIMAC:
        case SOC_BLK_HPA:
            pno = si->ge.num;
            ADD_PORT(ge, port);
            ADD_PORT(ether, port);
            ADD_PORT(port, port);
            ADD_PORT(all, port);
            if (port == CBX_PORT_ECPU) {
                si->cmic_port = CBX_PORT_ECPU;
                si->cmic_bitmap = ( 1 << CBX_PORT_ECPU);
            }
            break;
        case SOC_BLK_XLPORT:
            if (SOC_PBMP_MEMBER(pbmp_xport_ge, port)) {
                si->port_init_speed[port] = 1000;
                si->port_speed_max[port] = 2500;
                pno = si->ge.num;
                ADD_PORT(ge, port);
                SOC_PBMP_PORT_ADD(si->s_pbm, port);
#ifndef INCLUDE_542XX_EXT_PHY
                SOC_PBMP_PORT_ADD(si->gmii_pbm, port);
#endif
            } else {
                pno = si->xl.num;
                ADD_PORT(xl, port);
            }
            si->port_num_lanes[port] = 1;
            ADD_PORT(ether, port);
            ADD_PORT(port, port);
            ADD_PORT(all, port);
            break;
        case SOC_BLK_SPI:
            pno = 0;
            si->spi_port = port;
            si->spi_block = blk;
            SOC_PBMP_PORT_ADD(si->spi_bitmap, port);
            ADD_PORT(all, port);
            break;
        default:
            pno = 0;
            break;
        }
#undef  ADD_PORT
        if (bname[0] == '?') {
            pno = port;
        }

        if(port >=8 && port <=11)
            si->port_num_lanes[port] = 1;

        sal_snprintf(si->port_name[port], sizeof(si->port_name[port]),
                     "%s%d", bname, pno);
        si->port_type[port] = blktype;
        si->port_offset[port] = pno;
        si->block_valid[blk] += 1;
        if (si->block_port[blk] < 0) {
            si->block_port[blk] = port;
        }
        SOC_PBMP_PORT_ADD(si->block_bitmap[blk], port);
    }
    si->port_num = port;

    /* some things need to be found in the block table */
    si->arl_block = -1;
    si->mmu_block = -1;
    si->mcu_block = -1;
    si->inter_block = -1;
    si->exter_block = -1;
    for (blk = 0; SOC_BLOCK_INFO(unit, blk).type >= 0; blk++) {
        blktype = SOC_BLOCK_INFO(unit, blk).type;
        si->has_block[blk] = blktype;
        sal_snprintf(si->block_name[blk], sizeof(si->block_name[blk]),
                     "%s%d",
                     soc_block_name_lookup_ext(blktype, unit),
                     SOC_BLOCK_INFO(unit, blk).number);
    }
    si->block_num = blk;
}



int
soc_robo2_chip_reset(int unit)
{
    int rv = SOC_E_NONE;

    /* This assumes that same OTP values on both Avengers in cascade setup*/
    rv = soc_otp_status_read(unit);
    if (rv != SOC_E_NONE) {
        return rv;
    }

    return(soc_bootstrap_init(unit));
}

#ifdef INCLUDE_ROBO2_AVS
int
soc_robo2_avs_init( void )
{
    sal_printf("Reading Primary Avenger OTP...\n");
    sal_printf("Starting Dynamic AVS Using ROs on Primary Avenger...\n");

    /*
     * 1. Read the Initialised Pattern to know if this area is erased and
     *    written with Pattern. Generally after erase, the memory is filled
     *    with 0xFFFFFFFF.
     * 2. If the Pattern is "Initialized" to the chosen pattern (here
     *    0x55555555) then this is NOT the first boot, so read values from
     *    Flash and apply the dac code.
     * 3. Otherwise, i.e. If this is the first boot, then invoke the AVS.
     * 4. Store the last set core voltage and dac code.
     */
    avsfp_primary = sal_fopen("avs_status_file_primary", "r");
    if (avsfp_primary == NULL) {
        sal_printf("Error opening Primary AVS file for reading!! Create One.\n");
        avsfp_primary = sal_fopen("avs_status_file_primary", "w");
        if (avsfp_primary == NULL) {
            sal_printf("Error creating Primary AVS file!!\n");
        }
        else {
            avs_init(CBX_AVENGER_PRIMARY);
            if(avs_start(CBX_AVENGER_PRIMARY) < 0) {
                sal_printf("Error!! Dynamic AVS Failed on Primary."
                           " Reverting to Static AVS.\n");
                avenger_static_avs(CBX_AVENGER_PRIMARY);
            }
            avs_status_primary.core_voltage = AvsReadVoltage(CBX_AVENGER_PRIMARY, 5);
            avs_status_primary.dac_code     = currentDAC; /* Last DAC Code */

            /* ROBO-992: Incorporating suggestions from AVENGER-628
             * 20mV needs to be added to final AVS voltage to leave enough
             * margin for aging */
            /* 20mV * SCALING_FACTOR = 200 */
            avs_status_primary.core_voltage += 200;
            currentDAC = SetAvsVoltage(CBX_AVENGER_PRIMARY,
                                        avs_status_primary.core_voltage,
                                                avs_status_primary.dac_code);
            avs_status_primary.core_voltage = AvsReadVoltage(CBX_AVENGER_PRIMARY, 3);
            avs_status_primary.dac_code     = currentDAC; /* Last DAC Code */

            sal_printf("AVS3 Successful!! Core Voltage After AVS on"
                       " Primary Avenger: %d\n", avs_status_primary.core_voltage);
            avs_status_primary.initialized = AVS_STATUS_INITIALISED_PATTERN;
            ret_primary = sal_fwrite(&avs_status_primary.initialized, 4, 4, avsfp_primary);
            if(ret_primary != 4) {
                sal_printf("Error Writing Back AVS info into File!!\n");
            }
            /* Close the file */
            ret_primary = sal_fclose(avsfp_primary);
            if(ret_primary != 0) {
                sal_printf("Error Closing File!!\n");
            }
        }
    }
    else {
        /*
         * Not the First Boot Out of the Box. So Just apply saved Core Voltage.
         */
        ret_primary = sal_fread(&avs_status_primary.initialized, 4, 4, avsfp_primary);
        if(ret_primary != 4) {
            sal_printf("Error Reading AVS info from File!!\n");
        }
        else {
            sal_printf("Applying Saved Core Voltage:%d with saved DAC Code:"
                       " %d.\n", avs_status_primary.core_voltage, avs_status_primary.dac_code);
            SetAvsVoltage(CBX_AVENGER_PRIMARY, avs_status_primary.core_voltage,
                          avs_status_primary.dac_code);
            sal_printf("Core Voltage After applying saved AVS results on"
                       " Primary Avenger: %d\n",
                       AvsReadVoltage(CBX_AVENGER_PRIMARY, 5));
            /* Close the file */
            ret_primary = sal_fclose(avsfp_primary);
            if(ret_primary != 0) {
                sal_printf("Error Closing File!!\n");
            }
        }
    }

    /* Invoke AVS on Secondary Avenger if the Primary Avenger is in cascaded Mode */
    if(SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        sal_printf("Reading Secondary Avenger OTP...\n");
        sal_printf("Starting Dynamic AVS Using ROs on Secondary Avenger...\n");

        /*
         * 1. Read the Initialised Pattern to know if this area is erased and
         *    written with Pattern. Generally after erase, the memory is filled
         *    with 0xFFFFFFFF.
         * 2. If the Pattern is "Initialized" to the chosen pattern (here
         *    0x55555555) then this is NOT the first boot, so read values from
         *    Flash and apply the dac code.
         * 3. Otherwise, i.e. If this is the first boot, then invoke the AVS.
         * 4. Store the last set core voltage and dac code.
         */
        avsfp_secondary = sal_fopen("avs_status_file_secondary", "r");
        if (avsfp_secondary == NULL) {
            sal_printf("Error opening Secondary AVS file for reading!! Create One.\n");
            avsfp_secondary = sal_fopen("avs_status_file_secondary", "w");
            if (avsfp_secondary == NULL) {
                sal_printf("Error creating Secondary AVS file!!\n");
            }
            else {
                avs_init(CBX_AVENGER_SECONDARY);
                if(avs_start(CBX_AVENGER_SECONDARY) < 0) {
                    sal_printf("Error!! Dynamic AVS Failed on Secondary."
                               " Reverting to Static AVS.\n");
                    avenger_static_avs(CBX_AVENGER_SECONDARY);
                }
                avs_status_secondary.core_voltage = AvsReadVoltage(CBX_AVENGER_SECONDARY, 5);
                avs_status_secondary.dac_code     = currentDAC; /* Last DAC Code */

                /* ROBO-992: Incorporating suggestions from AVENGER-628
                 * 20mV needs to be added to final AVS voltage to leave enough
                 * margin for aging */
                /* 20mV * SCALING_FACTOR = 200 */
                avs_status_secondary.core_voltage += 200;
                currentDAC = SetAvsVoltage(CBX_AVENGER_SECONDARY,
                                        avs_status_secondary.core_voltage,
                                              avs_status_secondary.dac_code);
                avs_status_secondary.core_voltage = AvsReadVoltage(CBX_AVENGER_SECONDARY, 3);
                avs_status_secondary.dac_code     = currentDAC; /* Last DAC Code */


                sal_printf("AVS3 Successful!! Core Voltage After AVS on"
                           " Secondary Avenger: %d\n", avs_status_secondary.core_voltage);
                avs_status_secondary.initialized = AVS_STATUS_INITIALISED_PATTERN;
                ret_secondary = sal_fwrite(&avs_status_secondary.initialized, 4, 4, avsfp_secondary);
                if(ret_secondary != 4) {
                    sal_printf("Error Writing Back AVS info into File!!\n");
                }
                /* Close the file */
                ret_secondary = sal_fclose(avsfp_secondary);
                if(ret_secondary != 0) {
                    sal_printf("Error Closing File!!\n");
                }
            }
        }
        else {
            /*
             * Not the First Boot Out of the Box. So Just apply saved Core Voltage.
             */
            ret_secondary = sal_fread(&avs_status_secondary.initialized, 4, 4, avsfp_secondary);
            if(ret_secondary != 4) {
                sal_printf("Error Reading AVS info from File!!\n");
            }
            else {
                sal_printf("Applying Saved Core Voltage:%d with saved DAC Code:"
                           " %d.\n", avs_status_secondary.core_voltage, avs_status_secondary.dac_code);
                SetAvsVoltage(CBX_AVENGER_SECONDARY, avs_status_secondary.core_voltage,
                              avs_status_secondary.dac_code);
                sal_printf("Core Voltage After applying saved AVS results on"
                           " Secondary Avenger: %d\n",
                           AvsReadVoltage(CBX_AVENGER_SECONDARY, 5));
                /* Close the file */
                ret_secondary = sal_fclose(avsfp_secondary);
                if(ret_secondary != 0) {
                    sal_printf("Error Closing File!!\n");
                }
            }
        }
    }

    return SOC_E_NONE;
}
#endif

/*
 * @par Function:
 *      soc_robo2_do_init
 * @par Purpose:
 *      Initialize a robo2 switch without resetting it.
 *
 * @params [in] unit - unit #
 * @return SOC_E_XXX
 */
int
soc_robo2_do_init(int unit, int reset)
{
    soc_control_t        *soc;

    SAL_ASSERT(SOC_UNIT_VALID(unit));
    if (!SOC_UNIT_VALID(unit)) {
        return SOC_E_UNIT;
    }

    soc = SOC_CONTROL(unit);

    if (!(soc->soc_flags & SOC_F_ATTACHED)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                  "soc_robo2_do_init: Unit %d not attached\n"), unit));
        return(SOC_E_UNIT);
    }


    /* Backwards compatible default */
    SOC_MCAST_ADD_ALL_ROUTER_PORTS(unit) = 1;

    if (soc->soc_flags & SOC_F_INITED) {
        /********************************************************************/
        /* If the device has already been initialized before, perform
         * some de-initialization to avoid stomping on existing activities.
         */
        /********************************************************************/

        soc->soc_flags &= ~SOC_F_INITED;
    }

    /* Set bitmaps according to which type of device it is */
    soc_robo2_info_config(unit, soc);

    /* Chip HW Reset */
    if (reset) {
        SOC_IF_ERROR_RETURN(soc_robo2_chip_reset(unit));
    }

#ifdef INCLUDE_ROBO2_AVS
    SOC_IF_ERROR_RETURN(soc_robo2_avs_init());
#endif

    /*
     * Configure DMA channels.
     */
    if (soc_robo2_mgmt_init(unit) != 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                  "Unit %d: DMA initialization failed, unavailable\n"),
                   unit));
        return(SOC_E_INTERNAL);
    }

    /*
     * Chip depended misc init.
     */
    if (soc_misc_init(unit) != 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                  "Unit %d: Chip Misc initialization failed, unavailable\n"),
                   unit));
        return(SOC_E_INTERNAL);
    }

    /* Set feature cache */
    soc_feature_init(unit);


    /***********************************************************************/
    /* Begin initialization from a known state (reset).                    */
    /***********************************************************************/
    /*
     * PHY drivers and ID map
     */
    SOC_IF_ERROR_RETURN(soc_phyctrl_software_init(unit));

    /*
     * Update saved chip state to reflect values after reset.
     */

    soc->soc_flags &= SOC_F_RESET;
    /*
    Will fill more needed fields in soc_control_t.
    Ex. soc->xxx = xxx;
    */

    /*
    Any SPI Register needed be set ?
    */

    if (reset) {
        SOC_IF_ERROR_RETURN(soc_robo2_reset(unit));
    }

    /* Configure board LED */
#ifdef CONFIG_LEDMODE_ONINIT
    bcm53158_initialize_led_fixall(unit, 1, 14, 2);
#else
    soc_robo2_board_led_init(unit);
#endif

    return(SOC_E_NONE);
}


/*
 * @par Function:
 *      soc_robo2_deinit
 * @par Purpose:
 *      DeInitialize a Device.
 *
 * @params unit - Device unit #
 * @return SOC_E_XXX
 */
int
soc_robo2_deinit(int unit)
{
    return SOC_E_NONE;
}

/*
 * @par Function:
 *    soc_robo2_reset
 * @par Purpose:
 *    Reset some registers in unit to default working values.
 *
 * @params [in] unit - unit number.
 */
int
soc_robo2_reset(int unit)
{
    /* It will be implemented when receive chip
       init default setting from DVT. */
    int rv = SOC_E_NONE;
    soc_port_t port;
    pbmp_t bm;

    /* Reset register */
    SOC_PBMP_ASSIGN(bm, PBMP_ALL(unit));
    PBMP_ITER(bm, port) {
        continue;
    }

    /* TBD:Enable Frame Forwarding, Set Managed Mode */
    /* power down the invalid port*/
    soc_robo2_power_down_config(unit, bm);

    return rv;
}

/*
 * @par Function:
 *    soc_robo2_reset_init
 * @par Purpose:
 *    Reset and Initialize the switch
 *
 * @params [in] unit - unit number.
 */
int
soc_robo2_reset_init(int unit)
{
    return(soc_robo2_do_init(unit, TRUE));
}

/*
 * Function:
 *    soc_robo2_init
 * Purpose:
 *    Initialize the device to a known state. Writes some minor initial
 *    operating settings to the internal registers (e.g. interrupt
 *    config, channel config). Initializes the soc_control [unit]
 *    structure to be consistent with the chip reset state.
 * Parameters:
 *    unit - Robo2Switch unit number.
 * Returns:
 *    SOC_E_XXX
 * Notes:
 *    soc_robo2_init provides a way for diagnostics to bring the chip
 *    to a known state.
 */
int
soc_robo2_init(int unit)
{
    return(soc_robo2_do_init(unit, FALSE));
}

/*
 * @par Function:
 *    soc_robo2_attach
 * @par Purpose:
 *    Initialize the soc_control_t structure for a device, allocating all memory
 *    and semaphores required.
 * @params [in] unit - switch unit #.
 * @params [in] detach - Callback function called on detach.
 * @returns  SOC_E_XXX
 */
int
soc_robo2_attach(int unit)
{
    soc_control_t        *soc;

    LOG_INFO(BSL_LS_SOC_PCI,
             (BSL_META_U(unit,
                         "soc_robo2_attach: unit=%d\n"), unit));

    _soc_robo2_device_created(unit);

    soc = SOC_CONTROL(unit);

    /*
     *  Robo control structure
     */
    if(SOC_ROBO_CONTROL(unit) == NULL) {
        SOC_CONTROL(unit)->drv = (soc_robo_control_t *)
                                 sal_alloc(sizeof(soc_robo_control_t),
                                           "soc_robo_control");

        if (SOC_ROBO_CONTROL(unit) == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(SOC_ROBO_CONTROL(unit), 0, sizeof (soc_robo_control_t));
    }

    /* Install the Interrupt Handler */
    /* Make sure interrupts are masked before connecting line. */

    /*
     * Attached flag must be true during initialization.
     * If initialization fails, the flag is cleared by soc_detach (below).
     */

    soc->soc_flags |= SOC_F_ATTACHED;

    if (soc_ndev_attached++ == 0) {
        int            chip;

        /* Work to be done before the first SOC device is attached. */
        for (chip = 0; chip < SOC_ROBO2_NUM_SUPPORTED_CHIPS; chip++) {

            /* Call each chip driver's init function */
            if (soc_robo2_base_driver_table[chip]->init) {

                (soc_robo2_base_driver_table[chip]->init)();
            }

        }
    }

    if (spiMutex == NULL) {
       if ((spiMutex = sal_mutex_create("SPI")) == NULL) {
            goto error;
       }
    }


    /*
     * Set up port bitmaps.  They are also set up on each soc_robo2_init so
     * they can be changed from a CLI without rebooting.
     */
    soc_robo2_info_config(unit, soc);

    /* Set supported feature cache */
    soc_feature_init(unit);

    if ((soc->miimMutex = sal_mutex_create("MIIM")) == NULL) {
        goto error;
    }

    soc->apb2pbus_access_mutex = sal_mutex_create("APB2PBUS");
    if (soc->apb2pbus_access_mutex == NULL) {
            goto error;
    }


/* Check if the mutexes are required for ROBO2 devices */
#if 0
    /* Allocate counter module resources */
    if (soc_robo_counter_attach(unit)) {
        goto error;
    }

    /*
     * Create mutices
     */
    for (mem = 0; mem < NUM_SOC_ROBO2_MEM; mem++) {
        /*
         * should only create mutexes for valid memories.
         */
        if ((soc->memState[mem].lock =
                    sal_mutex_create(SOC_ROBO2_MEM_NAME(unit, mem))) == NULL) {
            goto error;
        }
    }

    if ((soc->arl_table_Mutex = sal_mutex_create("ARL_SW_TABLE")) == NULL) {
        goto error;
    }

    if ((soc->arl_mem_search_Mutex = sal_mutex_create("ARL_MEM_SEARCH")) ==
            NULL) {
        goto error;
    }

    if ((soc->arl_notify =
                sal_sem_create("ARL interrupt", sal_sem_BINARY, 0)) == NULL) {
        goto error;
    }

    if ((soc->mac_low_power_mutex = sal_mutex_create("MAC_LOW_POWER")) ==
            NULL) {
        goto error;
    }

    if ((soc->arbiter_mutex = sal_mutex_create("ARBITER")) ==
            NULL) {
        goto error;
    }

    soc->arl_pid = SAL_THREAD_ERROR;
    soc->arl_exit = _ROBO_SEARCH_LOCK | _ROBO_SCAN_LOCK;

#endif

    /* Clear statistics */
    sal_memset(&soc->stat, 0, sizeof(soc->stat));


#ifdef CONFIG_CASCADED_MODE
    if (soc_cm_get_dev_type(0) & SOC_SPI_DEV_TYPE) {
        LOG_CLI((BSL_META_U(unit,
                            "SPI device %s attached as unit %d.\n"),
                 soc_dev_name(0), unit));
    }
#else
    if (soc_cm_get_dev_type(unit) & SOC_SPI_DEV_TYPE) {
        LOG_CLI((BSL_META_U(unit,
                            "SPI device %s attached as unit %d.\n"),
                 soc_dev_name(unit), unit));
    }
#endif /* CONFIG_CASCADE_MODE */

    return(SOC_E_NONE);

error:
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "soc_robo2_attach: unit %d failed\n"), unit));

    soc_robo2_detach(unit); /* Perform necessary clean-ups on error */
    return SOC_E_MEMORY;
}

/*
 * @par Function:
 *    soc_robo2_detach
 * @par Purpose:
 *    Clean up allocated memories and release sems
 *    Free up device access
 * @params [in] unit - switch unit #.
 * @params [in] detach - Callback function called on detach.
 * @returns  SOC_E_XXX
 */
int
soc_robo2_detach(int unit)
{
    soc_control_t   *soc;
    soc_robo_control_t *robo;

    LOG_INFO(BSL_LS_SOC_PCI,
             (BSL_META_U(unit,
                         "Detaching\n")));

    soc = SOC_CONTROL(unit);

    if (NULL == soc) {
        return SOC_E_NONE;
    }

    if (0 == (soc->soc_flags & SOC_F_ATTACHED)) {
        return SOC_E_NONE;
    }

    robo = SOC_ROBO_CONTROL(unit);

    /*
     * Call soc_robo_init to cancel link scan task, counter DMA task,
     * outstanding DMAs, interrupt generation, and anything else the
     * driver or chip may be doing.
     */

    SOC_IF_ERROR_RETURN(soc_robo2_reset_init(unit));

    /*
     * PHY drivers and ID map
     */
    SOC_IF_ERROR_RETURN(soc_phyctrl_software_deinit(unit));

    /* Terminate counter module; frees allocated space */
    if(soc_robo_counter_detach(unit)) {
        LOG_CLI((BSL_META_U(unit,
                            "soc_detach: could not detach counter thread!\n")));
        return SOC_E_INTERNAL;
    }

    if (soc->miimMutex) {
        sal_mutex_destroy(soc->miimMutex);
        soc->miimMutex = NULL;
    }

    if (soc->apb2pbus_access_mutex) {
        sal_mutex_destroy(soc->apb2pbus_access_mutex);
        soc->apb2pbus_access_mutex = NULL;
    }

    SOC_FLAGS_CLR(soc, SOC_F_ATTACHED);

/* Destroy the mutexes for ROBO2 devices if created in soc_robo2_attach */
#if 0

    if(soc_robo_arl_mode_set(unit,ARL_MODE_NONE)) {
        LOG_CLI((BSL_META_U(unit,
                            "soc_detach: could not detach arl thread!\n")));
        return SOC_E_INTERNAL;
    }

    if (soc->arl_table_Mutex) {
        sal_mutex_destroy(soc->arl_table_Mutex);
        soc->arl_table_Mutex = NULL;
    }

    if (soc->arl_mem_search_Mutex) {
        sal_mutex_destroy(soc->arl_mem_search_Mutex);
        soc->arl_mem_search_Mutex = NULL;
    }

    for (mem = 0; mem < NUM_SOC_ROBO2_MEM; mem++) {
        if (SOC_MEM_IS_VALID(unit, mem)) {

            if (soc->memState[mem].lock != NULL) {
                sal_mutex_destroy(soc->memState[mem].lock);
                soc->memState[mem].lock = NULL;
            }
        }
    }
#endif

    if (robo!= NULL) {
        sal_free(robo);
        robo = soc->drv = NULL;
    }

    sal_free(SOC_CONTROL(unit));
    SOC_CONTROL(unit) = NULL;

    if (--soc_ndev_attached == 0) {
        /* Work done after the last SOC device is detached. */
        /* (currently nothing) */
        if (spiMutex) {
            sal_mutex_destroy(spiMutex);
            spiMutex = NULL;
        }
    }

    return SOC_E_NONE;
}

int soc_robo2_is_1g_disabled(int unit, soc_port_t port)
{
    
    SAL_ASSERT((unit == 0));
    SAL_ASSERT((port < CBX_PORT_MAX_GPHY_PORTS));

    if ((port < CBX_PORT_MAX_GPHY_PORTS) && (otp_flags & SOC_OTP_FLAG_GPHY_1G_DIS)) {
        return 1;
    } else {
        return 0;
    }
}

/*
 * Function:
 *    _soc_functions_find
 * Purpose:
 *    Return function array corresponding to the driver
 * Returns:
 *    Pointer to static function array soc_functions_t
 */

static soc_functions_t *
_soc_functions_find(soc_driver_t *drv)
{
    switch (drv->type) {

#ifdef BCM_ROBO2_SUPPORT
    case SOC_CHIP_BCM53158_B0:
        return &soc_robo2_drv_funs;
    case SOC_CHIP_BCM53158_A0:
        return &soc_robo2_drv_funs;
#endif

    default:
        break;
    }

    LOG_CLI((BSL_META("_soc_functions_find: no functions for this chip\n")));
    SAL_ASSERT(0);
    return NULL;
}

/*
 * Function:
 *    _soc_device_created
 * Purpose:
 *    Called by soc_cm_device_create() to tell the driver a
 *    device is being created.
 * Notes:
 *    This function may perform management initialization only.
 *    It may not touch the chip, as its accessors will not
 *    have been setup yet.
 *
 *    This function is a result of the prior organization,
 *    and is here to make the transition easier.
 *    and this should all probably be restructured to match
 *    the new configuration code.
 */

int
_soc_robo2_device_created(int unit)
{
    soc_control_t        *soc;
    int         chip = 0;
    uint16        dev_id = 0;
    uint8        rev_id = 0;

    if (SOC_CONTROL(unit) != NULL) {
        return SOC_E_EXISTS;
    }
    soc = SOC_CONTROL(unit) = sal_alloc(sizeof(soc_control_t), "soc_control");
    if (soc == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(soc, 0, sizeof(soc_control_t));

#ifdef CONFIG_CASCADED_MODE
    if (soc_cm_get_dev_type(0) & SOC_ETHER_DEV_TYPE) {
#else
    if (soc_cm_get_dev_type(unit) & SOC_ETHER_DEV_TYPE) {
#endif /* CONFIG_CASCADE_MODE */
        if (soc_eth_unit == -1) {
            soc_eth_unit = unit;
        }
        soc_eth_ndev++;

#ifdef ETH_MII_DEBUG
        /*
         * Instantiate the driver -- Verify chip revision matches driver
         * compilation revision.
         */

#ifdef CONFIG_CASCADED_MODE
        soc_cm_get_id(0, &dev_id, &rev_id);
#else
        soc_cm_get_id(unit, &dev_id, &rev_id);
#endif /* CONFIG_CASCADE_MODE */

        soc->chip_driver = soc_robo2_chip_driver_find(dev_id, rev_id);
        soc->soc_functions = NULL;

#endif
        return 0;
    }

    /*
     * Instantiate the driver -- Verify chip revision matches driver
     * compilation revision.
     */

#ifdef CONFIG_CASCADED_MODE
    soc_cm_get_id(0, &dev_id, &rev_id);
#else
    soc_cm_get_id(unit, &dev_id, &rev_id);
#endif /* CONFIG_CASCADE_MODE */

    soc->chip_driver = soc_robo2_chip_driver_find(dev_id, rev_id);
    if (soc->chip_driver == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                   "_soc_robo2_device_created: unit %d has no driver "
                   "(device 0x%04x rev 0x%02x)\n"),
                   unit, dev_id, rev_id));
        return SOC_E_UNAVAIL;
    }
    soc->soc_functions = _soc_functions_find(soc->chip_driver);

    if (soc->chip_driver == NULL) {
#if 0 /* Need to check if this call is required */
        SOC_ROBO_FIRST_ACTIVE_DRIVER(chip);
#endif
        soc->chip_driver = soc_robo2_base_driver_table[chip];
        LOG_CLI((BSL_META_U(unit,
                            "#### \n")));
        LOG_CLI((BSL_META_U(unit,
                            "#### Chip not recognized. \n")));
        LOG_CLI((BSL_META_U(unit,
                            "#### Device id:  0x%04x.  Rev id:  0x%02x\n"),
                 dev_id, rev_id));
        LOG_CLI((BSL_META_U(unit,
                            "#### Installing default driver for device %d.\n"),
                 (soc->chip_driver->type)));
        /* SOC_CHIP_NAME(soc->chip_driver->type))); */
        LOG_CLI((BSL_META_U(unit,
                            "#### Unpredictable behavior may result.\n")));
        LOG_CLI((BSL_META_U(unit,
                            "#### \n")));
    }

    return 0;
}

/*
 * Function:
 *  soc_robo2_mgmt_init
 * Purpose:
 *  Initialize the management mode/IMP configuration.
 * Parameters:
 *  unit - SOC unit #
 * Returns:
 *  SOC_E_NONE - Success
 *  SOC_E_XXX
 */

int
soc_robo2_mgmt_init(int unit)
{
    uint32 regval = 0;
    REG_READ_CRU_CRU_SW_TOP_CONTROL_REGISTERr(unit, (uint32 *)&regval);
    /* set RGMII delayed mode
    The RGMII delay mode setting are through bit[16] and bit[4:3].
    -  Bit[17]: port 14 link
    -  Bit[16]: delay 2ns bypass
    -  Bit[4]: enable RGMII delay on RXC
    -  Bit[3]: enable RGMII delay on TXC */
    regval = regval | (0x3 << 3)| (0x1 << 17);
    REG_WRITE_CRU_CRU_SW_TOP_CONTROL_REGISTERr(unit, (uint32 *)&regval);
    return SOC_E_NONE;
}

/*
 *    xlreg_read
 * @par Purpose:
 *    read 4 byte value from registers for debug
 * @params [in] unit  unit number
 * @params [in] port number (-1 for control reg, others for port reg)
 * @params [in] addr - register address
 * @returns  SOC_E_XXX
 */
int
xlreg_read(int unit, int port, uint32 addr)
{
    int value[2], rv = 0;
    int len = 8;
    if ((unit < 0) || (unit > 2)) return SOC_E_PARAM;
    if ((unit == 1) && (!SOC_IS_CASCADED(0))) return SOC_E_PARAM;
    if (port < 0) {
        port = REG_PORT_ANY;
    }
    if ((addr >= 0x200) && (addr <= 0x230)) {
        len = 4;
    }
    rv = drv_robo2_apb2pbus_reg_read(unit, port, addr, &value[0], len);
    if( rv != 0)
        return rv;
    if (port < 0) {
        if (len == 4)
            sal_printf("XlReg Addr 0x%x = %d (0x%x)\n",
                                  addr, value[0], value[0]);
        else
            sal_printf("XlReg Addr 0x%x = %d%d (0x%x%08x)\n",
                                  addr, value[1], value[0], value[1], value[0]);
    } else {
        if (len == 4)
            sal_printf("XlReg Port %d Addr 0x%x = %d (0x%x)\n",
                                  port, addr, value[0], value[0]);
        else
            sal_printf("XlReg Port %d Addr 0x%x = %d%d (0x%x%08x)\n",
                            port, addr, value[1], value[0], value[1], value[0]);
    }
    return rv;
}

/*
 *     xlreg_write
 * @par Purpose:
 *     write  4 byte value into the register for debug
 * @params [in] unit  unit number
 * @params [in] port number (-1 for control reg, others for port reg)
 * @params [in] addr  register address
 * @params [in] valuelo  Lower 4 byte data to write
 * @params [in] valuehi  Higher 4 byte data to write
 * @returns  SOC_E_XXX
 */
int
xlreg_write(int unit, int port, uint32 addr, uint32 valuelo, uint32 valuehi)
{
    int rv = 0;
    int len = 8;
    uint32 value[2];
    if ((unit < 0) || (unit > 2)) return SOC_E_PARAM;
    if ((unit == 1) && (!SOC_IS_CASCADED(0))) return SOC_E_PARAM;
    if (port < 0) {
        port = REG_PORT_ANY;
    }
    if ((addr >= 0x200) && (addr <= 0x230)) {
        len = 4;
        value[0] = valuelo;
    } else {
        value[0] = valuelo;
        value[1] = valuehi;
    }
    rv = drv_robo2_apb2pbus_reg_write(unit, port, addr, &value, len);
    return rv;
}
/*
 *
 *    reg_read
 * @par Purpose:
 *    read 4 byte value from registers for debug
 * @params [in] unit  unit number
 * @params [in] addr - register address
 * @returns  SOC_E_XXX
 */
int
avgr_reg_read(int unit, uint32 addr)
{
    int value = 0, rv = 0;
    if ((unit < 0) || (unit > 2)) return SOC_E_PARAM;
    if ((unit == 1) && (!SOC_IS_CASCADED(0))) return SOC_E_PARAM;
    rv = drv_robo2_reg_read(unit, addr, &value, 4);
    if( rv != 0)
        return rv;
    sal_printf("Reg Addr 0x%x = %d (0x%x)\n", addr, value, value);
    return rv;
}
#if 0
typedef enum soc_block_e {
  ipp = 0,
  epp = 1,
  bmu = 2,
  pqm = 3,
  pqs = 4,
  itm = 5,
  etm = 6
} soc_block_t;
#endif
int
reg_block_read(int unit, uint32 block_name)
{
    int value = 0, rv = 0;
    int start = 0;
    int end = 0;
    int index = 0;
    uint32 addr = 0;
    int i = 0;
    switch (block_name) {
        case 0:
           start = CB_IPP_BA_QUEUE_CONFIGr_ROBO;
           end  =  CB_IPP_VSIT_IA_WDATA_PART3r_ROBO;
           break;
        case 1:
           start = CB_EPP_BIMC_CMD0r_ROBO;
           end  =  CB_EPP_TXREQ_CM_STATUSr;
           break;
        case 2:
           start = CB_BMU_BIMC_CMD0r_ROBO;
           end  =  CB_BMU_SPG_PP_BMP_CONFIG_IA_WDATA_PART0r_ROBO;
           break;
        case 3:
           start = CB_PQM_ADMCTRLCFGr_ROBO;
           end  =  CB_PQM_WPT_IA_WDATA_PART0r_ROBO;
           break;
        case 4:
           start = CB_PQS_BURST_SIZE_PORT_0_SHAPER_A0r_ROBO;
           end  =  CB_PQS_WEIGHT_LOW_QUEUE_PORT_A15r_ROBO;
           break;
        case 5:
           start = CB_ITM_ARLFM0_IA_CAPABILITYr_ROBO;
           end  =  CB_ITM_SPARE_OUTr_ROBO;
           break;
        case 6:
           start = CB_ETM_BIMC_CMD0r_ROBO;
           end  = CB_ETM_VTCT_IA_WDATA_PART1r_ROBO;
           break;
        default:
          sal_printf( " dumping for all blocks \n");
          start = CB_IPP_BA_QUEUE_CONFIGr_ROBO;
          end  =  (NUM_SOC_ROBO2_REG - 1);
          break;
    }
    for (i = start; i <= end; i++) {
        addr =  drv_robo2_reg_addr_get(unit, i, REG_PORT_ANY, index);
#if 0
        if ((i >= M7SS_SPI0_SSPCPSRr_ROBO2) &&
                (i <= M7SS_SPI0_SSPTDRr_ROBO2)) {
               if (SOC_IS_CASCADED(0)) {
                   /* Spi registers are locked cannot access them */
                   continue;
               }
        }
#endif
        if (i >= XLMAC_CTRLr_ROBO2) {
            rv = drv_robo2_apb2pbus_reg_read(unit, REG_PORT_ANY, addr, &value, 4);
        } else {
            rv = drv_robo2_reg_read(unit, addr, &value, 4);
        }
        if( rv != 0)
            return rv;
#if !defined(SOC_NO_NAMES)
        sal_printf(" %s Reg Addr 0x%x = %d (0x%x)\n", soc_robo2_reg_name[i],addr, value, value);
#else
        sal_printf("Reg Addr 0x%x = %d (0x%x)\n", addr, value, value);
#endif
    }
    return rv;
}

/*
 *
 *    reg_write
 * @par Purpose:
 *    write  4 byte value into the register for debug
 * @params [in] unit  unit number
 * @params [in] addr - register address
 * @params [in] value - 4 byte data to write
 * @returns  SOC_E_XXX
 */
int
avgr_reg_write(int unit, uint32 addr, int value)
{
    int rv = 0;
    if ((unit < 0) || (unit > 2)) return SOC_E_PARAM;
    if ((unit == 1) && (!SOC_IS_CASCADED(0))) return SOC_E_PARAM;
    rv = drv_robo2_reg_write(unit, addr, &value, 4);
    return rv;
}

/*
 *
 *    dump_table
 * @par Purpose:
 *    dump table tid from index idx till idx + len
 * @params [in] tid - table id
 * @params [in] idx - index of the table
 * @params [in] len - number of entries to dump
 * @returns  SOC_E_XXX
 */
int
#if 0
dump_table(soc_robo2_table_id_t tid, int idx, int len)
#endif
dump_table(int tid, int idx, int len)
{
    int rv = 0;
    int i = 0;
    int unit = 0;
    int num_units = 1;

    if (SOC_IS_CASCADED(0)) {
        num_units = 2;
    }

    for (i = idx; i < (idx + len); i++) {
        for (unit = 0; unit < num_units; unit++) {
            if (!unit) {
                sal_printf("\t\t PRIMARY AVENGER\n");
            } else {
                sal_printf("\t\tSECONDARY AVENGER\n");
            }

            rv = soc_robo2_table_dump(unit, tid, i);
            if(rv < 0) {
                if (rv == SOC_E_INIT) {
                    sal_printf("dump_table ERROR: Check if table is enabled\n");
                }
                return rv;
            }
        }
    }
    return rv;
}

char*
ipp_counter_name(int index)
{
    char *name = "UNKNWON";
    switch (index) {
    case 0:
        name = "FWD_OK";
        break;
    case 1:
        name = "DST_DENIED";
        break;
    case 2:
        name = "SRC_DENIED";
        break;
    case 3:
        name = "TRAP_DROP";
        break;
    case 4:
        name = "CFP_DROP";
        break;
    case 5:
        name = "STP_DROP";
        break;
    case 6:
        name = "SLF_DROP";
        break;
    case 7:
        name = "ULF_DROP";
        break;
    case 8:
        name = "MLF_DROP";
        break;
    case 9:
        name = "BC_DROP";
        break;
    case 10:
        name = "SLI_DROP";
        break;
    }
    return name;
}

void
dump_cb_counters(int unit)
{
    uint32 regval = 0;
    int i=0;
    if ((unit < 0) || (unit > 2)) return;
    if ((unit == 1) && (!SOC_IS_CASCADED(0))) return;
    sal_printf("\nCB Counter dump for Unit=%d\n",unit);
    sal_printf("---------------------------\n");
    for (i=0; i < 15; i++) {
        REG_READ_CB_SIA_NISI_DROP_COUNT_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
            sal_printf("CB_SIA_nisi_drop_count_count_a[%d] => %08x\n", i, regval);
        }
    }
    REG_READ_CB_SIA_HISI_DROP_COUNT_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_SIA_hisi_drop_count_count_a => %08x\n", regval);
    }
    for (i=0; i < 15; i++) {
        REG_READ_CB_SIA_MIRR_DROP_COUNT_COUNT_Ar(unit,i , &regval);
        if (regval != 0) {
             sal_printf("CB_SIA_mirr_drop_count_count_a[%d] => %08x\n", i, regval);
        }
    }
    REG_READ_CB_SIA_HISI_OCTET_COUNT_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_SIA_hisi_octet_count_count_a => %08x\n", regval);
    }
    for (i=0; i < 11; i++) {
        REG_READ_CB_IPP_FORWARD_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_IPP_forward_count_a[%s] => %08x\n", ipp_counter_name(i), regval);
        }
    }
    for (i =0; i < 2; i++)  {
        REG_READ_CB_IPP_SIA_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_IPP_sia_count_a[%d] => %08x\n", i, regval);
        }
    }
    REG_READ_CB_IPP_SBE_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_IPP_sbe_count_a => %08x\n", regval);
    }
    REG_READ_CB_IPP_MBE_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_IPP_mbe_count_a => %08x\n", regval);
    }
    for (i =0; i < 4; i++)  {
        REG_READ_CB_IPP_TST_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_IPP_tst_a[%d] => %08x\n", i, regval);
             REG_WRITE_CB_IPP_TST_Ar(unit, i, &regval);
        }
    }
    REG_READ_CB_CFP_SBE_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_CFP_sbe_count_a => %08x\n", regval);
    }
    REG_READ_CB_CFP_MBE_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_CFP_mbe_count_a => %08x\n", regval);
    }
    REG_READ_CB_BMU_BMU_QUAL_DROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_BMU_bmu_qual_drop_count_a => %08x\n", regval);
    }
    REG_READ_CB_BMU_BMU_CH_PKT_DROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_BMU_bmu_ch_pkt_drop_count_a => %08x\n", regval);
    }
    REG_READ_CB_BMU_BMU_STM_PKT_DROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_BMU_bmu_stm_pkt_drop_count_a => %08x\n", regval);
    }
    for (i=0; i<31; i++) {
        REG_READ_CB_BMU_METER_DROP_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_BMU_meter_drop_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_BMU_DOMAIN_ADMISSION_DROP_ALL_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_BMU_domain_admission_drop_all_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_BMU_DOMAIN_ADMISSION_DROP_YELLOW_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_BMU_domain_admission_drop_yellow_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_BMU_DOMAIN_ADMISSION_DROP_RED_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_BMU_domain_admission_drop_red_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_BMU_GLOBAL_ADMISSION_DROP_ALL_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_BMU_global_admission_drop_all_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_BMU_GLOBAL_ADMISSION_DROP_YELLOW_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_BMU_global_admission_drop_yellow_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_BMU_GLOBAL_ADMISSION_DROP_RED_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_BMU_global_admission_drop_red_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_BMU_IPP_DROP_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_BMU_ipp_drop_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_BMU_CNTXT_RESOURCE_DROP_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_BMU_cntxt_resource_drop_count_a[%d] => %08x\n", i, regval);
        }
    }

    for (i=0; i < 16; i++) {
        REG_READ_CB_PQS_PER_QUEUE_EN_QUEUE_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_PQS_per_queue_en_queue_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_PQS_PER_PORT_STATUS_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_PQS_per_port_status_a[%d] => %08x\n", i, regval);
        }
    }
    REG_READ_CB_PQM_PMAPDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_pmapdrop_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_DLIDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_dlidrop_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_DPDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_dpdrop_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_UCENQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_ucenq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_MCENQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_mcenq_count_a => %08x\n", regval);
    }

    REG_READ_CB_PQM_MRRENQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_mrrenq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_TGENQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_tgenq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_UCQENQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_ucqenq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_UCQDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_ucqdrop_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_MCQENQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_mcqenq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_MCQDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_mcqdrop_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_MCQADDENQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_mcqaddenq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_MRRQENQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_mrrqenq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_MRRQDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_mrrqdrop_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_TGQENQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_tgqenq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_TGQDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_tgqdrop_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_UC_DEQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_uc_deq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_MC_DEQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_mc_deq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_MRR_DEQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_mrr_deq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_TG_DEQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_tg_deq_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_MCREP_DEQ_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_mcrep_deq_count_a => %08x\n", regval);
    }

    REG_READ_CB_PQM_UCSLIDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_ucslidrop_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_UCSPDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_ucspdrop_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_UCSTGETDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_ucstgetdrop_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_UCPGMAPDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_ucpgmapdrop_count_a => %08x\n", regval);
    }
    REG_READ_CB_PQM_UCCASDROP_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_PQM_uccasdrop_count_a => %08x\n", regval);
    }

    for (i=0; i < 128; i++) {
        REG_READ_CB_PQM_ADM_ENQ_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_PQM_adm_enq_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_PQM_ADM_WRED_DROP_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_PQM_adm_wred_drop_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_PQM_ADM_THRSH_DROP_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_PQM_adm_thrsh_drop_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_PQM_ADM_DEQ_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_PQM_adm_deq_count_a[%d] => %08x\n", i, regval);
        }
    }

    for (i=0; i < 16; i++) {
        REG_READ_CB_EPP_COUNT_EPP_OK_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_EPP_count_epp_ok_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_EPP_COUNT_EPP_ERR_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_EPP_count_epp_err_count_a[%d] => %08x\n", i, regval);
        }
        REG_READ_CB_EPP_COUNT_EPP_DROP_COUNT_Ar(unit, i, &regval);
        if (regval != 0) {
             sal_printf("CB_EPP_count_epp_drop_count_a[%d] => %08x\n", i, regval);
        }
    }

    REG_READ_CB_SID_MIRR_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_SID_mirr_count_a => %08x\n", regval);
    }
    REG_READ_CB_SID_HESI_OCTET_CNT_COUNT_Ar(unit, &regval);
    if (regval != 0) {
         sal_printf("CB_SID_hesi_octet_cnt_count_a => %08x\n", regval);
    }
}
