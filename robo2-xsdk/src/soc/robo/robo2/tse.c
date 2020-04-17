/*
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:
 *    tse.c
 * Description:
 *    TimeSync platform function for  BCM53158_A0
 */
#ifdef MANAGED_MODE
#include <sal/types.h>
#include <fsal_int/shared_types.h>
#include <sal/appl/io.h>
#else
#include "sal_types.h"
#include "sal_alloc.h"
#include "sal_time.h"
#include "bsl_log/bsl.h"
#include "fsal_int/types.h"
#include "soc/cpu_m7.h"
#endif
#include "fsal_int/extender.h"
#include "fsal_int/tse.h"
#include "fsal/ts.h"
#include "soc/drv.h"

#ifndef CONFIG_BOOTLOADER
/* #ifdef CORTEX_M7 */

#ifndef CORTEX_M7
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <errno.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <poll.h>

    /****************************************************************
    *  * Constants
    *   ****************************************************************/
    #define SYSFS_GPIO_DIR "/sys/class/gpio"
    #if 0
        #define POLL_TIMEOUT (3 * 1000) /* 3 seconds */
    #else
        #define POLL_TIMEOUT (-1) /* indefinite */
    #endif

    #define MAX_BUF             64
#ifdef MANAGED_MODE
    #define GPIO3               3
#else
    #define GPIO3               477
#endif
    #define TIMESYNC_THREAD_PRIO 10

    static void timesync_thread(void *params);
#endif

/* Checklist
   1. define handler for timesync
   2.

*/


/* Intr defs */
#define TIMESYNC_INTR       55
#define TSE_INTR_INTERRUPT_MASK (1<<19)
#define TSE_INTR_INTERRUPT_STATUS_MASK (0x3FFFF)


static int volatile intr=0;
static int volatile init=0;
static int volatile valid_events=0;
static int volatile cb_events=0;
static int volatile last_depth=0;
static int volatile last_capture_status=0;
static int volatile last_intsts=0;
static int volatile intsts_reads=0;
static int volatile intsts_reads_nz=0;
static int volatile last_val=0;
static int volatile last_val2=0;

static cbx_ts_time_t tod={0}; /* actual time of day Epoch, 1970-01-01 00:00:00 +0000 */
static uint64 tod_timestamp=0; /* value of timestamper at tod */


static uint64 recent_timestamp; /* 48bit value */
static uint32 recent_event_id;
extern ts_context_t ts_context;
extern void ts_event_task_wake(void);
extern void ts_event_enqueue(uint32_t event_id, cbx_ts_time_t *tod);

#ifndef CORTEX_M7

static sal_thread_t timesync_thread_id;

/****************************************************************
 *  * gpio_export
 *   ****************************************************************/
int gpio_export(unsigned int gpio)
{
        int fd, len;
        char buf[MAX_BUF];

        fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
        if (fd < 0) {
                perror("gpio/export");
                return fd;
        }

        len = snprintf(buf, sizeof(buf), "%d", gpio);
        write(fd, buf, len);
        close(fd);

        return 0;
}

/****************************************************************
 *  * gpio_unexport
 *   ****************************************************************/
int gpio_unexport(unsigned int gpio)
{
        int fd, len;
        char buf[MAX_BUF];

        fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
        if (fd < 0) {
                perror("gpio/export");
                return fd;
        }

        len = snprintf(buf, sizeof(buf), "%d", gpio);
        write(fd, buf, len);
        close(fd);
        return 0;
}

/****************************************************************
 *  * gpio_set_dir
 *   ****************************************************************/
int gpio_set_dir(unsigned int gpio, unsigned int out_flag)
{
        int fd, len;
        char buf[MAX_BUF];

        len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio);
        if (len < 0) { return len; }

        fd = open(buf, O_WRONLY);
        if (fd < 0) {
                perror("gpio/direction");
                return fd;
        }

        if (out_flag)
                write(fd, "out", 4);
        else
                write(fd, "in", 3);

        close(fd);
        return 0;
}

/****************************************************************
 *  * gpio_set_value
 *   ****************************************************************/
int gpio_set_value(unsigned int gpio, unsigned int value)
{
        int fd, len;
        char buf[MAX_BUF];

        len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
        if (len < 0) { return len; }

        fd = open(buf, O_WRONLY);
        if (fd < 0) {
                perror("gpio/set-value");
                return fd;
        }

        if (value)
                write(fd, "1", 2);
        else
                write(fd, "0", 2);

        close(fd);
        return 0;
}

/****************************************************************
 *  * gpio_get_value
 *   ****************************************************************/
int gpio_get_value(unsigned int gpio, unsigned int *value)
{
        int fd, len;
        char buf[MAX_BUF];
        char ch;

        len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
        if (len < 0) { return len; }

        fd = open(buf, O_RDONLY);
        if (fd < 0) {
                perror("gpio/get-value");
                return fd;
        }

        read(fd, &ch, 1);

        if (ch != '0') {
                *value = 1;
        } else {
                *value = 0;
        }

        close(fd);
        return 0;
}


/****************************************************************
 *  * gpio_set_edge
 *   ****************************************************************/

int gpio_set_edge(unsigned int gpio, char *edge)
{
        int fd, len;
        char buf[MAX_BUF];

        len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/edge", gpio);
        if (len < 0) { return len; }

        fd = open(buf, O_WRONLY);
        if (fd < 0) {
                perror("gpio/set-edge");
                return fd;
        }

        write(fd, edge, strlen(edge) + 1);
        close(fd);
        return 0;
}

/****************************************************************
 *  * gpio_fd_open
 *   ****************************************************************/

int gpio_fd_open(unsigned int gpio)
{
        int fd, len;
        char buf[MAX_BUF];

        len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
        if (len < 0) { return len; }

        fd = open(buf, O_RDONLY | O_NONBLOCK );
        if (fd < 0) {
                perror("gpio/fd_open");
        }
        return fd;
}

/****************************************************************
 *  * gpio_fd_close
 *   ****************************************************************/

int gpio_fd_close(int fd)
{
        return close(fd);
}

#endif /* Linux RSDK */


int
tse_init(void){
    int rv = SOC_E_NONE;
    uint32_t addr;
    uint32_t value;
#ifdef CORTEX_M7
    m7_disable_intr();
    m7_enable_irq(TIMESYNC_INTR);
    m7_enable_intr();
#else

    timesync_thread_id = sal_thread_create("TIMESYNC", 1024,
                                  TIMESYNC_THREAD_PRIO, timesync_thread, NULL);
    if (timesync_thread_id == SAL_THREAD_ERROR) {
        sal_printf("Failed Creating TIMESYNC Task\n");
    } else {
        sal_printf("Created TIMESYNC Task\n");
    }

#endif

    /* Turn off generator 0 (SyncA) which is on by default */
    addr = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_OUTPUT_ENABLEr_ROBO2, 0, 0);
    value = 0;
    DRV_REG_WRITE(0, addr, &value,4);
    TS_LOG_TMP("addr=0x%08x value=0x%08x\n", addr, value);

    addr = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_CTRLr_ROBO2, 0, 0);
    value=0;
    DRV_REG_WRITE(0, addr, &value,4);
    TS_LOG_TMP("addr=0x%08x value=0x%08x\n", addr, value);

    /* Turn off generator 1 (SyncB) which is on by default */
    addr = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_1_OUTPUT_ENABLEr_ROBO2, 0, 0);
    value = 0;
    DRV_REG_WRITE(0, addr, &value,4);
    TS_LOG_TMP("addr=0x%08x value=0x%08x\n", addr, value);

    addr = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_1_CTRLr_ROBO2, 0, 0);
    value=0;
    DRV_REG_WRITE(0, addr, &value,4);
    TS_LOG_TMP("addr=0x%08x value=0x%08x\n", addr, value);

    init=1;

    return rv;
}

int
tse_uninit(void){
    int rv = SOC_E_NONE;
#ifdef CORTEX_M7
    m7_disable_intr();
    m7_disable_irq(TIMESYNC_INTR);
    m7_enable_intr();
#else
    /* extern_intr_disable(0, TIMESYNC_INTR)  TRG-CHK Do I need to do this? */
#endif
    init=0;
    return rv;
}

int
tse_dump_stats(void)
{
    sal_printf("TSE init=%d intr=%d last_capture_status=0x%08x last_depth=%d last_intsts=0x%08x\n",
           init, intr, last_capture_status, last_depth, last_intsts);
    sal_printf("    last_val2=0x%08x last_val=0x%08x intsts_reads=%d intsts_reads_nz=%d\n",
           last_val2,
           last_val, intsts_reads, intsts_reads_nz);
    sal_printf("    valid_events=%d\n", valid_events);
    sal_printf("Recent time 0x%08x 0x%08x ID: %d\n",
           (uint32)(recent_timestamp>>32),
           (uint32)(recent_timestamp&0xffffffff),
           recent_event_id);
#ifdef LONGS_ARE_64BITS
    sal_printf("TOD 0x%lu 0x%08x \n", tod.sec, tod.nsec);
#else
    sal_printf("TOD 0x%llu 0x%08x \n", tod.sec, tod.nsec);
#endif
    /* clear out some of these */
    valid_events=0;
    last_depth=0;
    last_capture_status=0;
    last_intsts=0;
    intsts_reads=0;
    intsts_reads_nz=0;
    last_val=0;
    last_val2=0;

    return 0;
}

static int cpu_event_init=0;

/*
 * Function:
 *   tse_cpu
 * Description:
 *   Used to test the interrupt service routine, and event registration
 *
 */
int
tse_cpu(int num_events) {
    uint32_t val;
    int i;
    if (!cpu_event_init) {
        /* enable the interrupt*/
        REG_READ_CRU_TS_CORE_TIMESYNC_INTERRUPT_ENABLEr(CBX_AVENGER_PRIMARY, &val);
        val |= 0x1; /* set the CPU bit */
        REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERRUPT_ENABLEr(CBX_AVENGER_PRIMARY, &val);
    }
    for (i=0;i<num_events;i++) {
        REG_READ_CRU_TS_CORE_TIMESYNC_TIME_CAPTURE_MODEr(CBX_AVENGER_PRIMARY, &val);
        /* set the bit 0 (CPU) in the capture mode register */
        val |= 0x1;
        REG_WRITE_CRU_TS_CORE_TIMESYNC_TIME_CAPTURE_MODEr(CBX_AVENGER_PRIMARY, &val);
    }
    return 0;
}



/*
 * Function:
 *    tse_tod_update
 * Description:
 *    Called anytime a timestamp is gathered from the event FIFO
 */

inline void
tse_tod_update(uint32_t event_id, uint64_t timestamp){
    /* compare the upper/lower 48bit TS with the tod_timestamp,
       and add the difference in nsec to the ToD.
       This function called from the interrupt context - no locking
    */
    uint64_t diff;
    uint64_t diff_sec, diff_nsec;
    uint64_t nsec;

    if (timestamp >= tod_timestamp) {
        diff = timestamp-tod_timestamp;
        nsec = (uint64_t)tod.nsec + diff;
        tod.nsec = nsec % (uint64_t)1e9;
        tod.sec += nsec/(uint64_t)1e9;
    } else {
        diff = tod_timestamp-timestamp;
        diff_nsec = diff % (uint64_t)1e9;
        diff_sec = diff / (uint64_t)1e9;
        if (tod.nsec < diff_nsec) {
            tod.sec--;
            nsec = (uint64_t)tod.nsec + (uint64_t)1e9 - diff_nsec;
            tod.nsec = (uint32_t) nsec;
        } else {
            tod.nsec -= (uint32_t) diff_nsec;
        }
        tod.sec -= diff_sec;
    }
    tod_timestamp = timestamp;
#ifdef DEBUG_TS_TOD_UPDATE
#ifdef LONGS_ARE_64BITS
    sal_printf("Update ToD to: %lu.%09lu ts: 0x%lx (%lu)\n",
               tod.sec, tod.nsec, tod_timestamp, tod_timestamp);
#else
    sal_printf("Update ToD to: %llu.%09lu ts: 0x%llx (%llu)\n",
               tod.sec, tod.nsec, tod_timestamp, tod_timestamp);
#endif
#endif

    /* debug stuff update*/
    recent_event_id=event_id;
    recent_timestamp = timestamp;
}


/*
 * Gets the latest ToD and the exact timestamp when that ToD occurred
 */
int tse_tod_get(cbx_ts_time_t* time, uint64_t* timestamp) {
    uint32_t val, val2, val3, fval;

    if (!cpu_event_init) {
        /* enable the interrupt*/
        REG_READ_CRU_TS_CORE_TIMESYNC_INTERRUPT_ENABLEr(CBX_AVENGER_PRIMARY, &val);
        val |= 0x1; /* set the CPU bit */
        REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERRUPT_ENABLEr(CBX_AVENGER_PRIMARY, &val);
    }
    REG_READ_CRU_TS_CORE_TIMESYNC_TIME_CAPTURE_CONTROLr(CBX_AVENGER_PRIMARY, &val);
    val |= 0x1;
    REG_WRITE_CRU_TS_CORE_TIMESYNC_TIME_CAPTURE_CONTROLr(CBX_AVENGER_PRIMARY, &val);

    REG_READ_CRU_TS_CORE_TIMESYNC_TIME_CAPTURE_MODEr(CBX_AVENGER_PRIMARY, &val);
    /* set the bit 0 (CPU) in the capture mode register */
    val |= 0x1;
    REG_WRITE_CRU_TS_CORE_TIMESYNC_TIME_CAPTURE_MODEr(CBX_AVENGER_PRIMARY, &val);

    REG_READ_CRU_TS_CORE_TIMESYNC_INPUT_TIME_FIFO_TS_LOWERr(CBX_AVENGER_PRIMARY, &val);
    REG_READ_CRU_TS_CORE_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr(CBX_AVENGER_PRIMARY, &val2);

    soc_CRU_TS_CORE_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr_field_get(0, &val2, VALIDf, &fval);
    val3=fval;

    if (val3) {
        soc_CRU_TS_CORE_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr_field_get(0, &val2, EVENT_IDf, &val3);
        soc_CRU_TS_CORE_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr_field_get(0, &val2, TIMESTAMPf, &fval);
        val2=fval;
        tse_tod_update(val3, (((uint64)val2)<<32)|(uint64)val);
    }

#ifdef CORTEX_M7
    m7_disable_intr();
#else
    /* TRG-CHK : What is needed here? Close fd, disable timesync intr, other? */
#endif
    time->sec = tod.sec;
    time->nsec = tod.nsec;
    *timestamp = tod_timestamp;
#ifdef LONGS_ARE_64BITS
    TS_LOG_TMP("TOD: %lusec %unsec timestamp=0x%lx\n", tod.sec, tod.nsec, tod_timestamp);
#else
    TS_LOG_TMP("TOD: %llusec %unsec timestamp=0x%llx\n", tod.sec, tod.nsec, tod_timestamp);
#endif
#ifdef CORTEX_M7
    m7_enable_intr();
#else
#endif
    return 0;
}

/*
    Convert a time of day to it's corresponding 48bit timestamp value
*/
uint64_t tse_tod_to_ts(cbx_ts_time_t *tod) {
    uint64_t ts;

    ts = (((uint64_t)tod->nsec + tod->sec*1000000000) & 0x0000FFFFFFFFFFFFULL);
    /* sal_printf("ToD 0x%llx.0x%08lx [%llu.%09lu] >> ts 0x%llx\n", tod.sec, tod.nsec, tod.sec, tod.nsec, ts);*/ 
    return ts;
}

int tse_phase_adjust(uint64_t val, uint8_t neg) {
    uint32_t upper;
    uint32_t lower;
#ifdef DEBUG_TS_PHASE_ADJ
    uint64_t adj = val;
#endif

    if (val >= 0x1000000000000ULL) {
#ifdef LONGS_ARE_64BITS
        sal_printf("ERROR: Invalid phase adjust value 0x%lx exceeds 48bit max\n", val);
#else
        sal_printf("ERROR: Invalid phase adjust value 0x%llx exceeds 48bit max\n", val);
#endif
        return SOC_E_PARAM;
    }

    if (neg) {
        val = 0x1000000000000ULL - val;
    }

    upper = (uint32_t) (val >> 32);
    lower = (uint32_t) (val & 0x0FFFFFFFFULL);
    REG_WRITE_CRU_TS_CORE_TIMESYNC_TS0_PHASE_ADJUST_UPPERr(CBX_AVENGER_PRIMARY, &upper);
    REG_WRITE_CRU_TS_CORE_TIMESYNC_TS0_PHASE_ADJUST_LOWERr(CBX_AVENGER_PRIMARY, &lower);
#ifdef DEBUG_TS_PHASE_ADJ
#ifdef LONGS_ARE_64BITS
    sal_printf("Phase_adjust %c0x%lx [%c%lu], as %x:%x\n",
                neg? '-' : ' ', adj, neg? '-' : ' ', adj, upper, lower);
#else
    sal_printf("Phase_adjust %c0x%llx [%c%llu], as %lx:%lx\n",
                neg? '-' : ' ', adj, neg? '-' : ' ', adj, upper, lower);
#endif
#endif
    return 0;
}


/*
 * Sets the ToD at the specified _past_ timestamp. Please note that the
 * timestamper has likely advanced since this timestamp, so we
 * include the tod_timestamp-timestamp delta into the actual ToD which
 * is being set.
 *
 */
int tse_tod_set(cbx_ts_time_t *t, uint64_t ts) {
    uint8_t         neg = 0;
    uint64_t        ts2, ts_delta, diff, nsec;
    uint32_t        upper, lower;

#ifdef CORTEX_M7
    m7_disable_intr();
#else
#endif

    /* The timestamp value corresponding to the new time */
    ts2 = tse_tod_to_ts(t);

#ifdef DEBUG_TS_TOD_SET
    sal_printf("New ToD: %llu.%09lu, ts: 0x%llu  ToD: %llu.%09lu, tod_ts: 0x%llu\n",
        t->sec, t->nsec, ts2, tod.sec, tod.nsec, tod_timestamp);
#endif

    /* Update the current ToD
       Please note that the timestamper has likely advanced since this
       timestamp, so we include the tod_timestamp-timestamp delta into the
       actual ToD which is being set.
    */
    tod.sec = t->sec;
    tod.nsec = t->nsec;
    if (tod_timestamp >= ts2) {
        diff = tod_timestamp-ts2;
    } else {
        diff = ts2-tod_timestamp;
    }
    nsec = (uint64_t)tod.nsec + diff;
    tod.nsec = nsec % (uint64_t)1e9;
    tod.sec += tod.nsec/(uint64_t)1e9;
    tod_timestamp = tse_tod_to_ts(&tod);

    /* The delta between the new time and associated timestamp is the phase adj amount*/
    if (ts2 >= ts) {
        ts_delta = ts2 - ts;
    } else {
        ts_delta = ts - ts2;
        neg = 1;
    }

#ifdef TS_PHASE_ADJ_LIMITATION
    if (ts_delta < TS_PHASE_ADJ_INCREMENT) {
        /* Can't do any better than this */
        return 0;
    } else {
        uint64_t ns = ts_delta % TS_PHASE_ADJ_INCREMENT;
        if (ns < TS_PHASE_ADJ_INCREMENT/2) {
            ts_delta -= ns;
        }
    }
#endif

    /* Make the phase adjustment */
    if (ts_delta > 0) {

        if (ts_delta >= TS_PHASE_ADJ_COARSE) {
            /* If greater than 1 second, write the timestamp regs directly */
            upper = (uint32_t) (ts2 >> 32);
            lower = (uint32_t) (ts2 & 0x0FFFFFFFFULL);
#ifdef DEBUG_TS_PHASE_ADJ
#ifdef LONGS_ARE_64BITS
            sal_printf("Delta: %c0x%lx Writing timestamp regs with 0x%lx as 0x%x:0x%x\n",
                neg? '-' : ' ', ts_delta, ts2, upper, lower);
#else
            sal_printf("Delta: %c0x%llx Writing timestamp regs with 0x%llx as 0x%lx:0x%lx\n",
                neg? '-' : ' ', ts_delta, ts2, upper, lower);
#endif
#endif
            REG_WRITE_CRU_TS_CORE_TIMESYNC_TS0_FREQ_CTRL_UPPERr(CBX_AVENGER_PRIMARY, &upper);
            REG_WRITE_CRU_TS_CORE_TIMESYNC_TS0_FREQ_CTRL_LOWERr(CBX_AVENGER_PRIMARY, &lower);
        } else {
            /* perform the phase adjustment */
            (void) tse_phase_adjust(ts_delta, neg);
        }
    }

#ifdef CORTEX_M7
    m7_enable_intr();
#else
#endif
    return 0;
}


/*
 * Function:
 *    ts_intr
 * Description:
 *    Intr handler
 */

void
rimesync_handler(void)
{
    uint32 intsts;
    uint32 val,val2, val3;
    uint32 depth=0;
    uint32 fval;
#if 00
    uint32 address=0;
#endif
    int i;
    intr++;
#ifdef CORTEX_M7
    m7_disable_intr();
#else
#endif

    cb_events = 0;
    do {
        REG_READ_CRU_TS_CORE_TIMESYNC_INTERRUPT_STATUSr(CBX_AVENGER_PRIMARY, &intsts);
        intsts_reads++;
        /* force reread of the FIFO */
        if (1 || intsts & TSE_INTR_INTERRUPT_MASK) {
            if (intsts){
                intsts_reads_nz++;
                last_intsts = intsts;
            }
            /* AB501 TEMP */
            val=0xFFFFFFFF;
            REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERRUPT_CLRr(CBX_AVENGER_PRIMARY, &val);

            REG_READ_CRU_TS_CORE_TIMESYNC_CAPTURE_STATUSr(CBX_AVENGER_PRIMARY, &val);
            soc_CRU_TS_CORE_TIMESYNC_CAPTURE_STATUSr_field_get(0, &val, FIFO_DEPTHf, &fval);
            depth = fval;

            last_capture_status = val;
            last_depth = depth;
            for (i=0;i<depth;i++) {

                REG_READ_CRU_TS_CORE_TIMESYNC_INPUT_TIME_FIFO_TS_LOWERr(CBX_AVENGER_PRIMARY, &val);
                REG_READ_CRU_TS_CORE_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr(CBX_AVENGER_PRIMARY, &val2);
                last_val2=val2;
                last_val=val;

                soc_CRU_TS_CORE_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr_field_get(0, &val2, VALIDf, &fval);
                val3=fval;

                if (val3) {
                    valid_events++;
                    soc_CRU_TS_CORE_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr_field_get(0, &val2, EVENT_IDf, &val3);
                    soc_CRU_TS_CORE_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr_field_get(0, &val2, TIMESTAMPf, &fval);
                    val2=fval;
                    tse_tod_update(val3, (((uint64)val2)<<32)|(uint64)val);
                    if (ts_context.event_id_mask & 1<<val3) {
                        cb_events++;
                        ts_event_enqueue(val3, &tod);
                    }
                    #ifdef TS_DEBUG_INTR /* Enable to see the timestamps logged on the console */
#ifdef LONGS_ARE_64BITS
                        sal_printf("EventID-%u TOD=%lus 0x%uns ts=0x%lx\n", val3, tod.sec, tod.nsec, tod_timestamp);
#else
                        sal_printf("EventID-%u TOD=%llus 0x%uns ts=0x%llx\n", val3, tod.sec, tod.nsec, tod_timestamp);
#endif
                    #endif
                }
            }
        } else {
            /* AB501 what do we do here ? */
            sal_printf("*** What happened?\n");
        }
        if (intsts == 0) {
            break;
        }
    } while(1);
#ifdef CORTEX_M7
    m7_enable_intr();
#else
#endif
}


#ifndef CORTEX_M7
void timesync_thread(void *params) {
    struct pollfd fdset[1];
    int gpio_fd, timeout, interval, rc;
    char *buf[MAX_BUF];

    timeout = 0; /* POLL_TIMEOUT; */
    interval = 1000;  /* 1ms - Do we need to tweak this? TRGTRGTRG */

    extern_intr_enable(0, TIMESYNC_INTR);

    gpio_export(GPIO3);
    gpio_set_dir(GPIO3, 0);
    /**
     * rising: Trigger on rising edge
     * falling: Trigger on falling edge
     * both: Trigger on both edges
     * none: Disable interrupt on both edges
     */
    gpio_set_edge(GPIO3, "falling");
    gpio_fd = gpio_fd_open(GPIO3);

    while (1) {
        memset((void*)fdset, 0, sizeof(fdset));

        fdset[0].fd = gpio_fd;
        fdset[0].events = POLLPRI;

        rc = poll(fdset, 1, timeout);

        if (rc < 0) { sal_printf("\npoll() failed!\n"); }

        if (rc == 0) { /* printf("."); */ }

        if (fdset[0].revents & POLLPRI) {
            lseek(fdset[0].fd, 0, SEEK_SET);
            read(fdset[0].fd, buf, MAX_BUF);
            #if 0 /* Trace Interrupt Occurrance */
            sal_printf("\npoll() GPIO %d interrupt occurred\n", GPIO3);
            #endif
            rimesync_handler();
            extern_intr_ack(0, TIMESYNC_INTR);
        }
        fflush(stdout);
        if (cb_events > 0) {
            ts_event_task_wake();
        }
        sal_usleep(interval);
    }
    sal_printf("poll() exit\n");
}
#endif /* !CORTEX_M7 */

#endif /* !CONFIG_BOOTLOADER */
