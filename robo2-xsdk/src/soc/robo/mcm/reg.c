/*
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Filename:
 *     Reg.c
 * Purpose:
 *     This file contains the register access routines
 */

#include <soc/types.h>
#include <soc/register.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/chip.h>
#include <soc/spi.h>
#include <sal/types.h>
#include <shared/bsl.h>
#include <fsal_int/shared_types.h>
#include <soc/robo/mcm/robo_types.h>
#include <soc/robo/mcm/allenum.h>
#include <soc/robo/mcm/memregs.h>
#include <soc/robo/mcm/regacc.h>


#define APB2PBUS_ADDR_GEN(type, port, regid)  \
                       ((((type) & 1) << 25) | (((regid) & 0x1FFFF) << 8) | ((port) & 0xff))

#define APB2PBUS_REG_SET(unit, addr, value) drv_robo2_apb2pbus_reg_access(unit, addr, value, 0)
#define APB2PBUS_REG_GET(unit, addr, value) drv_robo2_apb2pbus_reg_access(unit, addr, value, 1)
#define APB2PBUS_TIMEOUT  2000

static uint32
get_block_base(int unit, int *typeslist, int index)
{
    int blk, idx;
    soc_block_info_t *blkinfo;
    if (!SOC_UNIT_NUM_VALID(unit)) {
        LOG_ERROR(BSL_LS_SOC_REG,
             (BSL_META("Invalid unit %d"), unit));
        return SOC_REG_BAD_ADDR;
    }
    if ((!typeslist) || (index > _soc_robo_max_blocks_per_entry_get())) {
        LOG_ERROR(BSL_LS_SOC_REG,
             (BSL_META("Invalid index %d"), index));
        return SOC_REG_BAD_ADDR;
    } else {
        blk = typeslist[index];
        if (blk < 0) {
            LOG_ERROR(BSL_LS_SOC_REG,
                 (BSL_META_U(unit, "Internal error getting reg block")));
            return SOC_REG_BAD_ADDR;
        }
        for (idx = 0; idx < SOC_ROBO_MAX_NUM_BLKS; idx++) {
            blkinfo = &SOC_BLOCK_INFO(unit, idx);
            if ((blkinfo->type > 0) && (blkinfo->type == blk)) {
                return SOC_BLOCK_BASE_ADDR(blkinfo);
            }
        }
    }
    LOG_ERROR(BSL_LS_SOC_REG,
        (BSL_META_U(unit, "Internal error unknown reg block %d"), blk));
    return SOC_REG_BAD_ADDR;
}

static uint32
get_port_block_base(int unit, int port)
{
    soc_port_info_t *port_info;
    soc_block_info_t *blkinfo;

    if (!SOC_UNIT_NUM_VALID(unit)) {
        LOG_ERROR(BSL_LS_SOC_REG,
             (BSL_META("Invalid unit %d"), unit));
        return SOC_REG_BAD_ADDR;
    }
#if 0
    if (unit == 1) {
        port -= SOC_MAX_NUM_PORTS;
    }
#endif
    if (port > SOC_MAX_NUM_PORTS) {
        LOG_ERROR(BSL_LS_SOC_REG,
             (BSL_META_U(unit, "Invalid port %d"),
                 (unit == 0) ? port : port + SOC_MAX_NUM_PORTS));
        return SOC_REG_BAD_ADDR;
    }

    port_info = &SOC_PORT_INFO(unit, port);
    if (port_info->blk <= 0) {
        LOG_ERROR(BSL_LS_SOC_REG,
             (BSL_META_U(unit, "Internal error getting port %d base"),
                      (unit == 0) ? port : port + SOC_MAX_NUM_PORTS));
        return SOC_REG_BAD_ADDR;
    }
    blkinfo = &(SOC_BLOCK_INFO(unit, port_info->blk));
    return SOC_BLOCK_BASE_ADDR(blkinfo);
}


/**
 * @par Function : drv_robo2_reg_access_check
 *
 * @par Purpose :
 *      Check if we have access to the register
 *
 * @param [in]  unit      :   Switch id
 * @param [in]  addr      :   Address of the register to access
 * @param [out] data_buf  :   Pointer to data output
 * @param [in]  len       :   Number of bytes to be read
 *
 * @return SOC_E_NONE   :   success
 * @return SOC_E_PARAM  :   parameter error
 *
 */
int
drv_robo2_reg_access_check(int unit, uint32 addr, int len)
{
    return SOC_E_NONE;
}

/**
 * @par Function : drv_robo2_reg_read
 *
 * @par Purpose :
 *      Read an given register (8, 16, 32 and 64 bit register access only)
 *
 * @param [in]  unit      :   Switch id
 * @param [in]  addr      :   Address of the register to access
 * @param [out] data_buf  :   Pointer to data output
 * @param [in]  len       :   Number of bytes to be read
 *
 * @return SOC_E_NONE   :   success
 * @return SOC_E_PARAM  :   parameter error
 *
 */
int
drv_robo2_reg_read(int unit, uint32 addr, void *data_buf, int len)
{
    int       rv = SOC_E_NONE;
#ifndef CONFIG_EXTERNAL_HOST
    uint32    data = 0;
    uint32   *addrp = 0;
#ifdef BE_HOST
    uint8     *p0, *p1;
    uint8     tmp;
#endif
#endif
    uint32   u_addr = 0;

    LOG_VERBOSE(BSL_LS_SOC_REG,
                 (BSL_META_U(unit, "addr = %x, length = %d\n"), addr, len));

    if (drv_robo2_reg_access_check(unit, addr, len) < 0) {
        /* Access check failed, may be invalid register or parameter */
        return SOC_E_PARAM;
    }

    /* Map to correct device if cascading */
    u_addr = drv_robo2_map_to_device_addr(unit, addr);

#ifdef CONFIG_EXTERNAL_HOST

#ifdef CONFIG_CASCADED_MODE
    /* Fetch the value using the SPI interface */
    rv = soc_spi_read(0, u_addr, (uint8*)data_buf, len);
#else
    /* Fetch the value using the SPI interface */
    rv = soc_spi_read(unit, u_addr, (uint8*)data_buf, len);
#endif /* CONFIG_CASCADE_MODE */
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_REG,
                 (BSL_META_U(unit, "error reading addr=0x%x\n"), addr));
        return rv;
    }

#else /* CONFIG_EXTERNAL_HOST */

    addrp = (uint32*)u_addr;
    /* Fetch the value directly */
    if (len < 4) {
       data = (len > 1) ?  (*addrp & 0xFFFF) : (*addrp & 0xFF);
       *(uint32*)data_buf = data;
    } else {
       *((uint32*)data_buf) = *addrp;
       if (len > 4) {
           *((uint32*)data_buf+1) = *(addrp+1);
       }
    }


#ifdef BE_HOST
    /* endian translation */
    for ( p0 = (uint8*)data_buf, p1 = p0 + len - 1;
          p0 < p1;
          p0++, p1--)
    {
        tmp = *p0;
        *p0 = *p1;
        *p1 = tmp;
    }
#endif
#endif /* CONFIG_EXTERNAL_HOST */
    if (len > 4) {
        LOG_VERBOSE(BSL_LS_SOC_REG,
            (BSL_META_U(unit, "addr = %x,  value= %x%x\n"),
                addr, *(uint32*)data_buf, *((uint32*)data_buf+1)));
    } else {
        LOG_VERBOSE(BSL_LS_SOC_REG,
            (BSL_META_U(unit, "addr = %x,  value= %x\n"),
                addr, *(uint32*)data_buf));
    }

    return rv;
}

/**
 * @par Function : drv_robo2_reg_write
 *
 * @par Purpose :
 *      Write to an given register
 *
 * @param [in]  unit      :   Switch id
 * @param [in]  addr      :   Address of the register to access
 * @param [in] data_buf  :   Pointer to data output
 * @param [in]  len       :   Number of bytes to be read
 *
 * @return SOC_E_NONE    :   success
 * @return SOC_E_PARAM   :   parameter error
 *
 */
int
drv_robo2_reg_write(int unit, uint32 addr, void *data_buf, int len)
{
    int    rv = SOC_E_NONE;
    uint32 u_addr = 0;

#ifdef CONFIG_EXTERNAL_HOST
#ifdef BE_HOST
    uint8  *p0;
    uint8  *p1;
    uint8  tmp;
#endif
#else  /* CONFIG_EXTERNAL_HOST */
    uint32 *addrp = 0;
    uint32 data = 0;
#endif /* !CONFIG_EXTERNAL_HOST */
    LOG_VERBOSE(BSL_LS_SOC_REG,
        (BSL_META_U(unit, "addr = %x, length = %d\n"),
            addr, len));
    if (drv_robo2_reg_access_check(unit, addr, len) < 0) {
        /* Access check failed, may be invalid register */
        return SOC_E_PARAM;
    }

    /* Map to correct device if cascading */
    u_addr = drv_robo2_map_to_device_addr(unit, addr);


#ifdef CONFIG_EXTERNAL_HOST
#ifdef BE_HOST
    /* endian translation */
    for ( p0 = (uint8*)data_buf, p1 = p0 + len - 1;
          p0 < p1;
          p0++, p1--)
    {
        tmp = *p0;
        *p0 = *p1;
        *p1 = tmp;
    }
#endif

#ifdef CONFIG_CASCADED_MODE
    /* Set the value using SPI interface*/
    rv = soc_spi_write(0, u_addr, (uint8*)data_buf, len);
#else
    /* Set the value using SPI interface*/
    rv = soc_spi_write(unit, u_addr, (uint8*)data_buf, len);
#endif /* CONFIG_CASCADE_MODE */
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_REG,
            (BSL_META_U(unit, "error reading addr=0x%x\n"), addr));
        return rv;
    }


#else /* CONFIG_EXTERNAL_HOST */

    addrp = (uint32*)u_addr;
    /* set the value directly if 4B or more, else do RMW */
    if (len < 4) {
       data = (len > 1) ?  (*addrp & ~0xFFFF) : (*addrp & ~0xFF);
       data |= (len > 1) ?  *((uint16*)data_buf) : *((uint8*)data_buf);
       *(uint32*)addrp = data;
    } else {
       *addrp = *((uint32*)data_buf);
       if (len > 4) {
           *(addrp+1) = *(((uint32*)data_buf)+1);
       } else {
       }
    }
#endif /* CONFIG_EXTERNAL_HOST */

    if (len > 4) {
        LOG_VERBOSE(BSL_LS_SOC_REG,
            (BSL_META_U(unit, "addr = %x,  value= %x%x\n"),
                addr, *(uint32*)data_buf, *((uint32*)data_buf+1)));
    } else {
        LOG_VERBOSE(BSL_LS_SOC_REG,
            (BSL_META_U(unit, "addr = %x,  value= %x\n"),
                addr, *(uint32*)data_buf));
    }
    return rv;
}


/**
 * @par Function : drv_robo2_reg_field_get
 *
 * @par Purpose :
 *      Extract the value of a field from register.
 *
 * @param [in]  unit    :   switch number.
 * @param [in]  reg     :   register index.
 * @param [in]  regbuf  :   data pointer of register's current value.
 * @param [in]  field   :   field index.
 * @param [out] fldbuf  :   data pointer of field value
 *
 * @return SOC_E_NONE
 *
 */
int
drv_robo2_reg_field_get(int unit, uint32 reg, uint32 *regbuf,
                        uint32 field, uint32 *fldbuf)
{
    soc_field_info_t    *finfop;
    int         i, wp, bp, len;
#ifdef BE_HOST
    uint32      val32;
#endif

    LOG_VERBOSE(BSL_LS_SOC_REG,
        (BSL_META_U(unit, "reg index= 0x%x, field index = %x\n"), reg, field));
    SAL_ASSERT(SOC_REG_IS_VALID(unit, (int)reg));

    SOC_FIND_FIELD(field,
        SOC_REG_INFO(unit, reg).fields,
        SOC_REG_INFO(unit, reg).nFields,
        finfop);
    SAL_ASSERT(finfop);
#ifdef BE_HOST
    if (SOC_REG_IS_64(unit, reg)) {
         val32 = regbuf[0];
         regbuf[0] = regbuf[1];
         regbuf[1] = val32;
    }
    if (finfop->len > 32) {
        val32 = fldbuf[0];
        fldbuf[0] = fldbuf[1];
        fldbuf[1] = val32;
    }
#endif
    bp = finfop->bp;

    wp = bp / 32;
    bp = bp & (32 - 1);
    i = 0;

    for (len = finfop->len; len > 0; len -= 32) {
        if (bp) {
            fldbuf[i] = regbuf[wp++] >> bp & ((1 << (32 - bp)) - 1);
            fldbuf[i] |= regbuf[wp] << (32 - bp);
        } else {
            fldbuf[i] = regbuf[wp++];
        }
        if (len < 32) {
            fldbuf[i] &= ((1 << len) - 1);
        }
        i++;
    }
#ifdef BE_HOST
    if (SOC_REG_IS_64(unit, reg)) {
        val32 = regbuf[0];
        regbuf[0] = regbuf[1];
        regbuf[1] = val32;
    }
    if (finfop->len > 32) {
        val32 = fldbuf[0];
        fldbuf[0] = fldbuf[1];
        fldbuf[1] = val32;
    }
#endif
    return SOC_E_NONE;
}

/**
 * @par Function : drv_robo2_reg_field_set
 *
 * @par Purpose :
 *      Set value of a field in register.
 *
 * @param [in] unit        :   RoboSwitch unit number.
 * @param [in] reg   :   register index.
 * @param [in] regbuf  :   data pointer of register's current value.
 * @param [in] field   :   field index.
 * @param [in] fldbuf  :   data pointer of field value
 *
 * @return SOC_E_NONE
 *
 */
int
drv_robo2_reg_field_set(int unit, uint32 reg, uint32 *regbuf,
                        uint32 field,  uint32 *fldbuf)
{
    soc_field_info_t    *finfop;
    uint32              mask;
    int         i, wp, bp, len;
#ifdef BE_HOST
    uint32      val32;
#endif
    LOG_VERBOSE(BSL_LS_SOC_REG,
        (BSL_META_U(unit, "reg index= 0x%x, field index = %x\n"), reg, field));
    mask = field;

    SAL_ASSERT(SOC_REG_IS_VALID(unit, (int)reg));

    SOC_FIND_FIELD(field,
    SOC_REG_INFO(unit, reg).fields,
    SOC_REG_INFO(unit, reg).nFields,
    finfop);
    SAL_ASSERT(finfop);
#ifdef BE_HOST
    if (SOC_REG_IS_64(unit, reg)) {
        val32 = regbuf[0];
        regbuf[0] = regbuf[1];
        regbuf[1] = val32;
    }
    if (finfop->len > 32) {
        val32 = fldbuf[0];
        fldbuf[0] = fldbuf[1];
        fldbuf[1] = val32;
    }
#endif

    bp = finfop->bp;

    wp = bp / 32;
    bp = bp & (32 - 1);
    i = 0;

    for (len = finfop->len; len > 0; len -= 32) {
        if (bp) {
            if (len < 32) {
                mask = (1 << len) - 1;
            } else {
                mask = -1;
            }
            regbuf[wp] &= ~(mask << bp);
            regbuf[wp++] |= fldbuf[i] << bp;
            regbuf[wp] &= ~(mask >> (32 - bp));
            regbuf[wp] |=
            fldbuf[i] >> (32 - bp) & ((1 << bp) - 1);
        } else {
            if (len < 32) {
                mask = (1 << len) - 1;
                regbuf[wp] &= ~mask;
                regbuf[wp++] |= fldbuf[i] << bp;
            } else {
                regbuf[wp++] = fldbuf[i];
            }
        }
        i++;
    }
#ifdef BE_HOST
    if (SOC_REG_IS_64(unit, reg)) {
        val32 = regbuf[0];
        regbuf[0] = regbuf[1];
        regbuf[1] = val32;
    }
    if (finfop->len > 32) {
        val32 = fldbuf[0];
        fldbuf[0] = fldbuf[1];
        fldbuf[1] = val32;
    }
#endif
    return SOC_E_NONE;
}

/**
 * @par Function : drv_robo2_reg_addr_get
 *
 * @par Purpose :
 *      calculate the address of a register.
 *
 * @param [in] unit        :   RoboSwitch unit number.
 * @param [in] reg   :   register index.
 * @param [in] port  :   port index.
 * @param [in] index   :   index value.
 *
 * @return register address
 *
 */
uint32
drv_robo2_reg_addr_get(int unit, uint32 reg, int port, int index)
{
    uint32               base = 0;       /* base address from reg_info */
    soc_regtype_t        regtype;
    uint32               offset;
    soc_block_types_t    regblktypes;

    SAL_ASSERT(SOC_REG_IS_VALID(unit, (int)reg));
    regblktypes = SOC_REG_INFO(unit, reg).block;
    regtype = SOC_REG_INFO(unit, reg).regtype;
    offset = SOC_REG_INFO(unit, reg).offset;

    if (port < 0) {
        if (SOC_REG_INFO(unit, reg).flags & SOC_REG_FLAG_INDIRECT) {
            port = 13; /* Any of 12,13 */
        } else {
            port = 0;
        }
    }

    if (index < 0) {
        index = 0;
    }

    switch (regtype) {
        case soc_genreg:
            /* general case */
            base = get_block_base(unit, regblktypes, 0);
            SAL_ASSERT (base != SOC_REG_BAD_ADDR);
            base += offset + (index * 4);
            break;
        case soc_spi_reg:
            if (port > 0) {
                SAL_ASSERT(0);
            }
            break;
        case soc_phy_reg:
        case soc_portreg:
            if (index > 0) {
                SAL_ASSERT(0);
            }
            base = get_port_block_base(unit, port);
            SAL_ASSERT (base != SOC_REG_BAD_ADDR);
            base += offset;
            break;
        default:
            SAL_ASSERT(0);
        break;
    }

    SAL_ASSERT (base != SOC_REG_BAD_ADDR);

    LOG_VERBOSE(BSL_LS_SOC_REG,
        (BSL_META_U(unit, "regid= 0x%x, port = %d, index = %d, addr = 0x%x\n"),
            reg, port, index, base));
    return base;
}


/**
 * @par Function : drv_robo2_reg_length_get
 *
 * @par Purpose :
 *      Get the length of the specific register.
 *
 * @param [in] unit  :   switch number.
 * @param [in] reg   :   register index.
 *
 *
 * @return register length (bytes)
 *
 */
int
drv_robo2_reg_length_get(int unit, uint32 reg)
{
    if (SOC_REG_IS_64(unit, reg)) {
        return 8;
    } else {
        return 4;
    }
}

/**
 * @par Function : drv_robo2_reg_name_get
 *
 * @par Purpose :
 *      Get the name of the specific register.
 *
 * @param [in] unit  :   switch number.
 * @param [in] reg   :   register index.
 *
 *
 * @return pointer to register name
 *
 */
char*
drv_robo2_reg_name_get(int unit, uint32 reg)
{
    if (reg < NUM_SOC_REG) {
        return (soc_robo_reg_name[reg]);
    } else {
        return NULL;
    }
}

uint32
drv_robo2_apb2pbus_reg_addr(int unit, int regid, int index)
{
    uint32 regaddr = 0;
    regaddr = drv_robo2_reg_addr_get(unit, regid, REG_PORT_ANY, index);
    return drv_robo2_map_to_device_addr(unit, regaddr);
}

int
drv_robo2_apb2pbus_reg_access(int unit, uint32 addr, uint32 *value, int type)
{
    int rv = SOC_E_NONE;

#ifdef CONFIG_EXTERNAL_HOST
    if (type == 0) {
#ifdef CONFIG_CASCADED_MODE
       rv = soc_spi_write(0, addr, (uint8*)value, 4);
#else
       rv = soc_spi_write(unit, addr, (uint8*)value, 4);
#endif /* CONFIG_CASCADE_MODE */
       if (SOC_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_SOC_REG,
               (BSL_META_U(unit, "error reading addr=0x%x\n"), addr));
           return rv;
       }
    } else {
#ifdef CONFIG_CASCADED_MODE
        rv = soc_spi_read(0, addr, (uint8*)value, 4);
#else
        rv = soc_spi_read(unit, addr, (uint8*)value, 4);
#endif /* CONFIG_CASCADE_MODE */
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_REG,
                     (BSL_META_U(unit, "error reading addr=0x%x\n"), addr));
            return rv;
        }
    }
#else /* CONFIG_EXTERNAL_HOST */
    if (type == 0) {
        /* Set the value directly */
        *(uint32*)addr = *value;
    } else {
        /* Fetch the value directly */
        *value = *(uint32*)addr;
    }

#endif /* CONFIG_EXTERNAL_HOST */
    if (type == 0) {
        LOG_VERBOSE(BSL_LS_SOC_REG,
               (BSL_META_U(unit, "APB2PUS WRITE Addr=0x%x Value=%08x\n"), addr, *value));
    } else {
        LOG_VERBOSE(BSL_LS_SOC_REG,
               (BSL_META_U(unit, "APB2PUS READ Addr=0x%x Value=%08x\n"), addr, *value));
    }
    return rv;
}


/**
 * @par Function : drv_robo2_apb2pbus_reg_read_hw
 *
 * @par Purpose :
 *      Read a given register via APB2PBUS bridge
 *
 * @param [in]  unit      :   Switch id
 * @param [in]  port      :   Port number
 * @param [in]  addr      :   Address of the register to access
 * @param [out] data_buf  :   Pointer to data output
 * @param [in]  len       :   Number of bytes to be read
 *
 * @return SOC_E_NONE   :   success
 * @return SOC_E_PARAM  :   parameter error
 *
 */
int
drv_robo2_apb2pbus_reg_read_hw(int unit, int port, uint32 addr, void *data_buf, int len)
{
    uint32  tmp, done = 0, err = 0, data = 0;
    uint32  rd_addr = 0, rd_ctrl = 0, rd_go = 0, rd_data = 0, rd_status = 0;
    int i, timeout, type;

    LOG_VERBOSE(BSL_LS_SOC_REG,
                 (BSL_META_U(unit, "APB2PBUS Read Port %d addr = %x\n"), port, addr));

    timeout = APB2PBUS_TIMEOUT;
    if (port == REG_PORT_ANY) {
        type  = 1;   /* Generic = 1, PerPort = 0 */
        port  = 0;
    } else {
        type  = 0;   /* Generic = 1, PerPort = 0 */
        port  = SOC_PORT_INFO(unit, port).bindex;
    }
    data = APB2PBUS_ADDR_GEN(type, port, addr);
    rd_addr = drv_robo2_apb2pbus_reg_addr(unit, PBUSBRIDGE_APB2PBUS_RADDRr_ROBO, 0);
    APB2PBUS_REG_SET(unit, rd_addr, &data);

    data = 0;
    soc_PBUSBRIDGE_APB2PBUS_RCTRLr_field_set(unit, &data, RD_BYTE_CNTf, (uint32*)&len);
    rd_ctrl = drv_robo2_apb2pbus_reg_addr(unit, PBUSBRIDGE_APB2PBUS_RCTRLr_ROBO, 0);
    APB2PBUS_REG_SET(unit, rd_ctrl, &data);

    data = 0;
    tmp = 1;
    soc_PBUSBRIDGE_APB2PBUS_RD_GOr_field_set(unit, &data, RD_GOf, &tmp);
    rd_go = drv_robo2_apb2pbus_reg_addr(unit, PBUSBRIDGE_APB2PBUS_RD_GOr_ROBO, 0);
    APB2PBUS_REG_SET(unit, rd_go, &data);

    rd_status = drv_robo2_apb2pbus_reg_addr(unit, PBUSBRIDGE_APB2PBUS_RD_WR_STATUSr_ROBO, 0);
    do {
        data = 0;
        APB2PBUS_REG_GET(unit, rd_status, &data);
        soc_PBUSBRIDGE_APB2PBUS_RD_WR_STATUSr_field_get(unit, &data, RD_DONEf, &done);
        soc_PBUSBRIDGE_APB2PBUS_RD_WR_STATUSr_field_get(unit, &data, RD_ERRORf, &err);
    } while((done == 0) && (err == 0) && (timeout-- > 0));

    /* Clear status */
    APB2PBUS_REG_SET(unit, rd_status, &data);

    if (timeout <= 0) {
        LOG_ERROR(BSL_LS_SOC_REG,
            (BSL_META_U(unit, "APB2PBBUS Read Timeout addr = %x\n"), addr));
        return SOC_E_TIMEOUT;
    } else if (err == 1) {
        LOG_ERROR(BSL_LS_SOC_REG,
            (BSL_META_U(unit, "APB2PBBUS Read Failed addr = %x\n"), addr));
        return SOC_E_INTERNAL;
    }

    for (i = 0; i < (len >> 2); i++) {
        rd_data = drv_robo2_apb2pbus_reg_addr(unit, PBUSBRIDGE_APB2PBUS_RDATAr_ROBO, i);
        APB2PBUS_REG_GET(unit, rd_data, &data);
        *((uint32*)data_buf + i) = data;
    }

    LOG_VERBOSE(BSL_LS_SOC_REG,
            (BSL_META_U(unit, "APB2BUS Read addr = %x ok\n"), addr));

    return SOC_E_NONE;
}

/**
 * @par Function : drv_robo2_apb2pbus_reg_read
 *
 * @par Purpose :
 *      Read a given register via APB2PBUS bridge
 *
 * @param [in]  unit      :   Switch id
 * @param [in]  port      :   Port number
 * @param [in]  addr      :   Address of the register to access
 * @param [out] data_buf  :   Pointer to data output
 * @param [in]  len       :   Number of bytes to be read
 *
 * @return SOC_E_NONE   :   success
 * @return SOC_E_PARAM  :   parameter error
 *
 */
int
drv_robo2_apb2pbus_reg_read(int unit, int port, uint32 addr, void *data_buf, int len)
{
    int rv = 0;

    SOC_APB2PBUS_ACCESS_LOCK(unit);
    rv = drv_robo2_apb2pbus_reg_read_hw(unit, port, addr, data_buf, len);
    SOC_APB2PBUS_ACCESS_UNLOCK(unit);
    return rv;

}


/**
 * @par Function : drv_robo2_apb2pbus_reg_write_hw
 *
 * @par Purpose :
 *      Write to an given register via APB2PBUS bridge
 *
 * @param [in]  unit      :   Switch id
 * @param [in]  port      :   Port number
 * @param [in]  addr      :   Address of the register to access
 * @param [in] data_buf  :   Pointer to data output
 * @param [in]  len       :   Number of bytes to be read
 *
 * @return SOC_E_NONE    :   success
 * @return SOC_E_PARAM   :   parameter error
 *
 */
int
drv_robo2_apb2pbus_reg_write_hw(int unit, int port, uint32 addr, void *data_buf, int len)
{
    uint32  tmp, done = 0, err = 0, data = 0;
    uint32  wr_addr = 0, wr_ctrl = 0, wr_go = 0, wr_data = 0, wr_status = 0;
    int i, timeout, type;

    LOG_VERBOSE(BSL_LS_SOC_REG,
                 (BSL_META_U(unit, "APB2PBUS Write Port %d addr = %x\n"), port, addr));

    timeout = APB2PBUS_TIMEOUT;
    if (port == REG_PORT_ANY) {
        type  = 1;   /* Generic = 1, PerPort = 0 */
        port  = 0;
    } else {
        type  = 0;   /* Generic = 1, PerPort = 0 */
        port  = SOC_PORT_INFO(unit, port).bindex;
    }
    data = APB2PBUS_ADDR_GEN(type, port, addr);
    wr_addr = drv_robo2_apb2pbus_reg_addr(unit, PBUSBRIDGE_APB2PBUS_WADDRr_ROBO, 0);
    APB2PBUS_REG_SET(unit, wr_addr, &data);

    for (i = 0; i < (len >> 2); i++) {
        wr_data = drv_robo2_apb2pbus_reg_addr(unit, PBUSBRIDGE_APB2PBUS_WDATAr_ROBO, i);
        data = *((uint32*)data_buf + i);
        APB2PBUS_REG_SET(unit, wr_data, &data);
    }

    data = 0;
    soc_PBUSBRIDGE_APB2PBUS_WCTRLr_field_set(unit, &data, WR_BYTE_CNTf, (uint32*)&len);
    wr_ctrl = drv_robo2_apb2pbus_reg_addr(unit, PBUSBRIDGE_APB2PBUS_WCTRLr_ROBO, 0);
    APB2PBUS_REG_SET(unit, wr_ctrl, &data);

    data = 0; tmp = 1;
    soc_PBUSBRIDGE_APB2PBUS_WR_GOr_field_set(unit, &data, WR_GOf, &tmp);
    wr_go = drv_robo2_apb2pbus_reg_addr(unit, PBUSBRIDGE_APB2PBUS_WR_GOr_ROBO, 0);
    APB2PBUS_REG_SET(unit, wr_go, &data);

    wr_status = drv_robo2_apb2pbus_reg_addr(unit, PBUSBRIDGE_APB2PBUS_RD_WR_STATUSr_ROBO, 0);
    do {
        data = 0;
        APB2PBUS_REG_GET(unit, wr_status, &data);
        soc_PBUSBRIDGE_APB2PBUS_RD_WR_STATUSr_field_get(unit, &data, WR_DONEf, &done);
        soc_PBUSBRIDGE_APB2PBUS_RD_WR_STATUSr_field_get(unit, &data, WR_ERRORf, &err);
    } while((done == 0) && (err == 0) && (timeout-- > 0));

    /* Clear status */
    APB2PBUS_REG_SET(unit, wr_status, &data);

    if (timeout <= 0) {
        LOG_ERROR(BSL_LS_SOC_REG,
            (BSL_META_U(unit, "APB2PBBUS Write Timeout addr = %x\n"), addr));
        return SOC_E_TIMEOUT;
    } else if (err == 1) {
        LOG_ERROR(BSL_LS_SOC_REG,
            (BSL_META_U(unit, "APB2PBBUS Write Failed addr = %x\n"), addr));
        return SOC_E_INTERNAL;
    }


    LOG_VERBOSE(BSL_LS_SOC_REG,
            (BSL_META_U(unit, "APB2BUS Write addr = %x ok\n"), addr));

    return SOC_E_NONE;
}

/**
 * @par Function : drv_robo2_apb2pbus_reg_write
 *
 * @par Purpose :
 *      Write to an given register via APB2PBUS bridge
 *
 * @param [in]  unit      :   Switch id
 * @param [in]  port      :   Port number
 * @param [in]  addr      :   Address of the register to access
 * @param [in] data_buf  :   Pointer to data output
 * @param [in]  len       :   Number of bytes to be read
 *
 * @return SOC_E_NONE    :   success
 * @return SOC_E_PARAM   :   parameter error
 *
 */
int
drv_robo2_apb2pbus_reg_write(int unit, int port, uint32 addr, void *data_buf, int len)
{
    int rv;

    SOC_APB2PBUS_ACCESS_LOCK(unit);
    rv = drv_robo2_apb2pbus_reg_write_hw(unit, port, addr, data_buf, len);
    SOC_APB2PBUS_ACCESS_UNLOCK(unit);
    return rv;
}
