/*
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Filename:
 *     robo2drv_if.h
 * Purpose:
 *     Interface to the HAL 
 */

#ifndef _ROBO2DRV_IF_H
#define _ROBO2DRV_IF_H

#ifndef XGXS_SLEEP_US
#define XGXS_SLEEP_US 1000
#endif
#define SOC_MAX_ROBO2_COUNTER_NUM 47
/*array size=32*47(max 32 ports and  per port 47 sw mib counters) */
#define SW_COUNT_ARR        1504
#define SOC_ROBO2_NUM_SUPPORTED_CHIPS   SOC_ROBO_NUM_SUPPORTED_CHIPS

#define ROBO2_REG_APB2PBUS_WRITE_WITH_ID(u, r, p, i, v) \
    DRV_APB2PBUS_REG_WRITE(u, p, DRV_REG_ADDR(u, r, p, i), v, DRV_REG_LENGTH_GET(u, r))

#define ROBO2_REG_APB2PBUS_READ_WITH_ID(u, r, p, i, v) \
    DRV_APB2PBUS_REG_READ(u, p, DRV_REG_ADDR(u, r, p, i), v, DRV_REG_LENGTH_GET(u, r))

#define DRV_APB2PBUS_REG_READ(unit, port, addr, data, len)\
         drv_robo2_apb2pbus_reg_read(unit, port, addr, data, len)

#define DRV_APB2PBUS_REG_WRITE(unit, port, addr, data, len)\
         drv_robo2_apb2pbus_reg_write(unit, port, addr, data, len)

extern int drv_robo2_apb2pbus_reg_read(int unit, int port, uint32 addr, void *data, int len);

extern int drv_robo2_apb2pbus_reg_write(int unit, int port, uint32 addr, void *data, int len);

#define DRV_REG_FIELD_SET_VAL(unit, reg, regbuf, field, fldbuf)\
         drv_robo2_reg_field_set_val(unit, reg, regbuf, field, fldbuf)

#define DRV_REG32_FIELD_SET_VAL(unit, reg, regbuf, field, fldbuf)\
         drv_robo2_reg32_field_set_val(unit, reg, regbuf, field, fldbuf)
#define DRV_REG_FIELD_GET_VAL(unit, reg, regval, field, fldbuf)\
         drv_robo2_reg_field_get_val(unit, reg, regval, field, fldbuf)

#if 0
#define DRV_REG_READ(unit, addr, data, len)\
         drv_robo2_reg_read(unit, addr, data, len)

#define DRV_REG_WRITE(unit, addr, data, len)\
         drv_robo2_reg_write(unit, addr, data, len)


#define DRV_REG_LENGTH_GET(unit, reg)\
         drv_robo2_reg_length_get(unit, reg)

#define DRV_REG_ADDR(unit, reg, port, index)\
         drv_robo2_reg_addr_get(unit, reg, port, index)

#define DRV_REG_FIELD_SET(unit, reg, regbuf, field, fldbuf)\
         drv_robo2_reg_field_set(unit, reg, regbuf, field, fldbuf)

#define DRV_REG_FIELD_GET(unit, reg, regbuf, field, fldbuf)\
         drv_robo2_reg_field_get(unit, reg, regbuf, field, fldbuf)

#define DRV_MEM_LENGTH_GET(unit, mem, data)\
         drv_robo2_mem_length_get(unit, mem, data)

#define DRV_MEM_WIDTH_GET(unit, mem, data)\
         drv_robo2_mem_width_get(unit, mem, data)

#define DRV_MEM_READ(unit, mem, entry_id, count, entry)\
         drv_robo2_mem_read(unit, mem, entry_id, count, entry)

#define DRV_MEM_WRITE(unit, mem, entry_id, count, entry)\
         drv_robo2_mem_write(unit, mem, entry_id, count, entry)

#define DRV_MEM_FIELD_GET(unit, mem, field_index, entry, fld_data)\
         drv_robo2_mem_field_get(unit, mem, field_index, entry, fld_data)

#define DRV_MEM_FIELD_SET(unit, mem, field_index, entry, fld_data)\
         drv_robo2_mem_field_set(unit, mem, field_index, entry, fld_data)

#endif



extern int drv_robo2_apb2pbus_reg_read(int unit, int port, uint32 addr, void *data, int len);
extern int drv_robo2_apb2pbus_reg_write(int unit, int port, uint32 addr, void *data, int len);
extern int drv_robo2_reg_read(int unit, uint32 addr, void *data, int len);
extern int drv_robo2_reg_write(int unit, uint32 addr, void *data, int len);
extern int avgr_reg_read(int unit, uint32 addr);
extern int avgr_reg_write(int unit, uint32 addr, int value);
extern int xlreg_read(int unit, int port, uint32 addr);
extern int xlreg_write(int unit, int port, uint32 addr, uint32 valuelo, uint32 valuehi);
extern int drv_robo2_reg_length_get(int unit, uint32 reg);
extern uint32 drv_robo2_reg_addr_get(int unit, uint32 reg, int port, int index);
extern int drv_robo2_reg_field_set(int unit, uint32 reg, uint32 *regbuf, uint32 field, uint32 *fldbuf);
extern int drv_robo2_reg_field_set_val(int unit, uint32 reg, uint64 *regbuf, uint32 field, uint32 fldbuf);
extern int drv_robo2_reg32_field_set_val(int unit, uint32 reg, uint32 *regbuf, uint32 field, uint32 fldbuf);
extern int drv_robo2_reg_field_get_val(int unit, uint32 reg, uint64 regval, uint32 field, uint32 *fldbuf);
extern int drv_robo2_reg_field_get(int unit, uint32 reg, uint32 *regbuf, uint32 field, uint32 *fldbuf);
extern int drv_robo2_mem_length_get(int unit, uint32 mem, uint32 *data);
extern int drv_robo2_mem_width_get(int unit, uint32 mem, uint32 *data);
extern int drv_robo2_mem_read(int unit, uint32 mem, uint32 entry_id, uint32 count, uint32 *entry);
extern int drv_robo2_mem_write(int unit, uint32 mem, uint32 entry_id, uint32 count, uint32 *entry);
extern int drv_robo2_mem_field_get(int unit, uint32 mem, uint32 field_index, uint32 *entry, uint32 *fld_data);
extern int drv_robo2_mem_field_set(int unit, uint32 mem, uint32 field_index, uint32 *entry, uint32 *fld_data);
extern char* drv_robo2_reg_name_get(int unit, uint32 reg);
extern int reg_block_read(int unit, uint32 block);

#endif
