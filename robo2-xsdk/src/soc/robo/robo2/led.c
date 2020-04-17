/*
 * $Id: $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:
 *    led.c
 * Description:
 *    This file contains initialization code for LED uprocessor
 */

#ifdef MANAGED_MODE
#include <shared/bsl.h>
#include <sal/types.h>
#include <tables.h>
#include <fsal_int/init.h>
#include <led.h>
#include <robo2drv_if.h>
#else
#include <bsl_log/bsl.h>
#include <sal_types.h>
#include <soc/robo2/common/tables.h>
#include <soc/robo2/bcm53158/init.h>
#include <soc/robo2/bcm53158/led.h>
#endif
#include <shared/types.h>
#include <soc/types.h>
#include <soc/drv.h>
#include <fsal/error.h>

#ifdef CONFIG_LEDMODE_ONINIT
extern int avgr_reg_write(int unit, uint32 addr, int value);
#endif


#define LED_PROGRAM_MAX_SIZE 256
#define LED_OPCODE_SEND      0x3A


#ifdef CONFIG_EXTERNAL_HOST
#define LED_DEBUG   sal_printf
#else
#define LED_DEBUG   sysprintf
#endif /* CONFIG_EXTERNAL_HOST */

void led_refresh_cycle_config(int unit)
{
    uint8 i;
    uint32 remap_data;
    uint32 address;
    uint32 reg_len;
    uint8 temp_data;

    remap_data = 40;
    REG_WRITE_LED_LEDUP0_CLK_DIVr(unit, &remap_data);
    address = DRV_REG_ADDR(unit, INDEX(LED_LEDUP0_PORT_ORDER_REMAP_0_3r),
                           REG_PORT_ANY, 0);
    reg_len = DRV_REG_LENGTH_GET(unit,INDEX(LED_LEDUP0_PORT_ORDER_REMAP_0_3r));

    for (i = 0; i < 16; i++) {
        /* idea is to configure remap[port] = port */
        temp_data = ( i * 4 ) + 3;
        /* size of remap_data in register is 6bit per port */
        remap_data = temp_data;
        temp_data = temp_data - 1;
        remap_data = (remap_data << 6) | temp_data;
        temp_data = temp_data - 1;
        remap_data = (remap_data << 6) | temp_data;
        temp_data = temp_data - 1;
        remap_data = (remap_data << 6) | temp_data;

        drv_robo2_reg_write(unit, address, &remap_data, reg_len);
        address = address + 4;
    }
}

void led_delay_config(int unit)
{
    uint32 val;
    /* Set GPHY intra port delay */
    val = 0x11111111;
    REG_WRITE_CRU_CRU_LED_CONTROLLER_CFG0r(unit, &val);

    /* Set QSGMII intra port delay */
    val = 0x1111;
    REG_WRITE_CRU_CRU_LED_CONTROLLER_CFG1r(unit, &val);

    /* Set PMACRO intra port delay */
    val = 0x1;
    REG_WRITE_XLPORT_LED_CHAIN_CONFIGr(unit, &val);

}

void led_load_strap_to_data_ram(int unit)
{
    uint32 strap_data;
    uint8  led_strap_data;
    uint32 scan_chain_start_addr;

    REG_READ_CRU_CRU_STRAP_STATUS_REGr(unit, &strap_data);
#if 0
    LED_DEBUG("Strap Data:0x%x\n", strap_data);
#endif

    /* led 7 : bit 3 in strap */
    led_strap_data = (strap_data & 0x8) << 4;
    /* led 6 : bit 0 in strap */
    led_strap_data = led_strap_data | ((strap_data & 0x1) << 6);
    /* led 5 : bit 1 in strap */
    led_strap_data = led_strap_data | ((strap_data & 0x2) << 4);
    /* led 4 : bit 13 in strap */
    led_strap_data = led_strap_data | ((strap_data & 0x2000) >> 9);
    /* led 3 : bit 2 in strap */
    led_strap_data = led_strap_data | ((strap_data & 0x4) << 1);
    /* led 2 : bit 4 in strap */
    led_strap_data = led_strap_data | ((strap_data & 0x10) >> 2);
    /* led 1 : bit 9 in strap */
    led_strap_data = led_strap_data | ((strap_data & 0x200) >> 8);
    /* led 0 : bit 10 in strap */
    led_strap_data = led_strap_data | ((strap_data & 0x400) >> 10);

    /* Write the strap_data into LED_DATA_RAM location 0xFE.
	 * Multiplying by 4 to match the avenger addressing mechanism for
     * LED data ram
  	 */
    REG_WRITE_LED_LEDUP0_DATA_RAMr(unit, 0xFE, &led_strap_data);


    /*  config SCAN CHAIN ASSEMBLY AREA
     *  start address = 128;
     *  depth = 32
     *  Bits 32 * 8 = 256 bits max
     */
    scan_chain_start_addr = 128;
    REG_WRITE_LED_LEDUP0_SCANCHAIN_ASSEMBLY_ST_ADDRr(unit,
                                                     &scan_chain_start_addr);

}

void led_firmware_load(int unit)
{
    uint16 count;
    uint32 firmware_data;

    LED_DEBUG("Loading LED Firmware\n");
    for (count = 0; count < LED_PROGRAM_MAX_SIZE; count = count + 1)
    {
        firmware_data = led_firmware[count];
        REG_WRITE_LED_LEDUP0_PROGRAM_RAMr(unit, count, &firmware_data);
        if ((count % 40) == 0) {
            LED_DEBUG(".");
        }
    }
    LED_DEBUG("\n");
}

void led_start(int unit)
{
    uint32 reg_data;

    /* Refresh cycle =  33.33 ms */
    reg_data = 0xCB7355;
    REG_WRITE_LED_LEDUP0_CLK_PARAMSr(unit, &reg_data);

    /* { led_scan_start_dly = 0, led_intra_port_dly = 0, ledup_enable = 1} */
    reg_data = 1;
    REG_WRITE_LED_LEDUP0_CTRLr(unit, &reg_data);
}

void bcm53158_initialize_led(int unit)
{

#ifndef CONFIG_LEDMODE_ONINIT
    LED_DEBUG("Unit %d: LED Refresh cycle config\n", unit);
    led_refresh_cycle_config(unit);

    LED_DEBUG("Unit %d: LED Delay config\n", unit);
    led_delay_config(unit);

    LED_DEBUG("Unit %d: LED Strap load\n", unit);
    led_load_strap_to_data_ram(unit);
#endif

    led_firmware_load(unit);

    LED_DEBUG("Unit %d: LED Start\n", unit);
    led_start(unit);

}

#ifdef CONFIG_LEDMODE_ONINIT
void bcm53158_initialize_led_fixall(int unit, int highlevel, int totalportnum, int ledperport)
{
    uint32 regdata_wr;
    uint32 scanchain_start_addr;

    LED_DEBUG("[bcm53158_initialize_led_fixall]Unit %d: LED Refresh cycle config\n", unit);
    led_refresh_cycle_config(unit);

    LED_DEBUG("[bcm53158_initialize_led_fixall]Unit %d: LED Delay config\n", unit);
    led_delay_config(unit);

    LED_DEBUG("[bcm53158_initialize_led_fixall]Unit %d: LED Strap load\n", unit);
    led_load_strap_to_data_ram(unit);

    //Write program ram of LED to send command only
    regdata_wr = LED_OPCODE_SEND;
    REG_WRITE_LED_LEDUP0_PROGRAM_RAMr(unit, 0x0, &regdata_wr);
    regdata_wr = totalportnum * ledperport;
    REG_WRITE_LED_LEDUP0_PROGRAM_RAMr(unit, 0x1, &regdata_wr);

    LED_DEBUG("[bcm53158_initialize_led_fixall]Unit %d: LED Start\n", unit);
    led_start(unit);

    REG_READ_LED_LEDUP0_SCANCHAIN_ASSEMBLY_ST_ADDRr(unit, &scanchain_start_addr);
    if(highlevel){
        regdata_wr = 0x0;
        REG_WRITE_LED_LEDUP0_DATA_RAMr(unit, scanchain_start_addr, &regdata_wr);
        REG_WRITE_LED_LEDUP0_DATA_RAMr(unit, scanchain_start_addr + 1, &regdata_wr);
        REG_WRITE_LED_LEDUP0_DATA_RAMr(unit, scanchain_start_addr + 2, &regdata_wr);
    } else {
        regdata_wr = 0xFF;
        REG_WRITE_LED_LEDUP0_DATA_RAMr(unit, scanchain_start_addr, &regdata_wr);
        REG_WRITE_LED_LEDUP0_DATA_RAMr(unit, scanchain_start_addr + 1, &regdata_wr);
        REG_WRITE_LED_LEDUP0_DATA_RAMr(unit, scanchain_start_addr + 2, &regdata_wr);
    }
}
#endif