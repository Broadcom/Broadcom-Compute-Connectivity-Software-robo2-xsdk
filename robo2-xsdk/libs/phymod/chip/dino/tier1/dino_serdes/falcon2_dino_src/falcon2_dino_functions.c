/***********************************************************************************
 ***********************************************************************************
 *  File Name     :  falcon2_dino_functions.c                                        *
 *  Created On    :  29/04/2013                                                    *
 *  Created By    :  Kiran Divakar                                                 *
 *  Description   :  APIs for Serdes IPs                                           *
 *  Revision      :  $Id: falcon2_dino_functions.c 1404 2016-04-14 22:49:19Z cvazquez $ *
 *                                                                                 *
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$                                                           *
 *  No portions of this material may be reproduced in any form without             *
 *  the written permission of:                                                     *
 *      Broadcom Corporation                                                       *
 *      5300 California Avenue                                                     *
 *      Irvine, CA  92617                                                          *
 *                                                                                 *
 *  All information contained in this document is Broadcom Corporation             *
 *  company private proprietary, and trade secret.                                 *
 *                                                                                 *
 ***********************************************************************************
 ***********************************************************************************/

/** @file falcon2_dino_functions.c
 * Implementation of API functions
 */

#ifdef _MSC_VER
/* Enclose all standard headers in a pragma to remove warings for MS compiler */
#pragma warning( push, 0 )
#endif

#ifdef SERDES_API_FLOATING_POINT
#include <math.h>
#include <phymod/phymod.h>
#endif
#ifdef _MSC_VER
#pragma warning( pop )
#endif

#include "falcon2_dino_usr_includes.h"
#include "falcon2_dino_functions.h"
#include "falcon2_dino_field_access.c"
#include "falcon2_dino_internal.c"
#include "falcon2_dino_dv_functions.c"




#include "falcon2_dino_pll_config.c"




/************************************/
/*  Display Eye Scan                */
/************************************/

/* This is best method for terminal ASCII display */
err_code_t falcon2_dino_display_eye_scan( const phymod_access_t *pa ) {
    uint32_t   stripe[64];
    uint16_t   status = 0;
    int8_t     y;

    FALCON2_DINO_EFUN(falcon2_dino_display_eye_scan_header( pa, 1));
    /* start horizontal acquisition */
    {   err_code_t err_code = falcon2_dino_meas_eye_scan_start( pa, EYE_SCAN_HORIZ);
        if (err_code) {
            FALCON2_DINO_EFUN((falcon2_dino_meas_eye_scan_done( pa ), err_code));
        }
    }

    /* display stripes */
    for (y = 62;y>=-62;y=y-2)
    {
        {   err_code_t err_code = falcon2_dino_read_eye_scan_stripe( pa, &stripe[0], &status);
            if (err_code) {
                FALCON2_DINO_EFUN((falcon2_dino_meas_eye_scan_done( pa ), err_code));
            }
        }
        FALCON2_DINO_EFUN(falcon2_dino_display_eye_scan_stripe( pa, y,&stripe[0]));
        FALCON2_DINO_EFUN_PRINTF(("\n"));
    }
    /* stop acquisition */
    FALCON2_DINO_EFUN(falcon2_dino_meas_eye_scan_done( pa ));
    FALCON2_DINO_EFUN(falcon2_dino_display_eye_scan_footer( pa, 1));

    return(ERR_CODE_NONE);
}

/* This function is for Falcon and is configured for passive mode */
err_code_t falcon2_dino_meas_lowber_eye( const phymod_access_t *pa, const struct falcon2_dino_eyescan_options_st eyescan_options, uint32_t *buffer) {
  int8_t y,x;
  int16_t i;
  uint16_t status;
  uint32_t errors = 0;
  uint16_t timeout;
  uint8_t stopped_state;

  if(!buffer) {
      return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }
  i = 0;
  FALCON2_DINO_ESTM(stopped_state = falcon2_dino_rdv_usr_sts_micro_stopped());

  timeout = eyescan_options.timeout_in_milliseconds;
  FALCON2_DINO_EFUN(falcon2_dino_wrcv_diag_max_time_control((uint8_t)timeout));

  FALCON2_DINO_EFUN(falcon2_dino_wrv_usr_diag_mode(eyescan_options.mode));

  FALCON2_DINO_EFUN_PRINTF(("Calculating\n"));
  for (y = eyescan_options.vert_max;y>=eyescan_options.vert_min;y=y-eyescan_options.vstep) {
    for (x=eyescan_options.horz_min;x<=eyescan_options.horz_max;x=x+eyescan_options.hstep) {
      /* acquire sample */
        FALCON2_DINO_EFUN(falcon2_dino_pmd_uc_cmd_with_data( pa, CMD_DIAG_EN,CMD_UC_DIAG_GET_EYE_SAMPLE,((uint16_t)x)<<8 | (uint8_t)y,200));
        /* wait for sample complete */
#if 0
        do {
            FALCON2_DINO_EFUN(falcon2_dino_delay_us(1000));
            FALCON2_DINO_ESTM(status=falcon2_dino_rdv_usr_diag_status());
            FALCON2_DINO_EFUN_PRINTF(("status=%04x\n",status));
        } while((status & 0x8000) == 0);
#else
        FALCON2_DINO_EFUN(falcon2_dino_poll_diag_done( pa, &status, (((uint32_t)timeout)<<7)*10 + 20000));
#endif
        FALCON2_DINO_EFUN(falcon2_dino_prbs_err_count_ll( pa, &errors));
        /* FALCON2_DINO_EFUN_PRINTF(("(%d,%d) = %u\n",x,y,errors & 0x7FFFFFF));    */

        buffer[i] = errors & 0x7FFFFFFF;

        i++;
        FALCON2_DINO_EFUN_PRINTF(("."));
    }
    FALCON2_DINO_EFUN_PRINTF(("\n"));
  }
  FALCON2_DINO_EFUN_PRINTF(("\n"));
  FALCON2_DINO_EFUN(falcon2_dino_meas_eye_scan_done( pa ));
  FALCON2_DINO_EFUN(falcon2_dino_wrv_usr_sts_micro_stopped(stopped_state));
  return(ERR_CODE_NONE);
}


/* Display the LOW BER EyeScan */
err_code_t falcon2_dino_display_lowber_eye( const phymod_access_t *pa, const struct falcon2_dino_eyescan_options_st eyescan_options, uint32_t *buffer) {
    int8_t x,y,i,z;
    int16_t j; /* buffer pointer */
    uint32_t val;
    uint8_t overflow;
    uint32_t limits[13]; /* allows upto 400 sec test time per point (1e-13 BER @ 10G) */

    if(!buffer) {
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
     }

    /* Calculate initial total bitcount BER 1e-1 */
    limits[0] = falcon2_dino_mult_with_overflow_check(eyescan_options.linerate_in_khz/10, eyescan_options.timeout_in_milliseconds, &overflow);
    if(overflow > 0) {
        limits[0] = UINT32_MAX;
        FALCON2_DINO_EFUN_PRINTF(("Very long timout_in_milliseconds results in saturation of Err counter can cause in accurate results\n"));
    }

    for(i=1;i<13;i++) {            /* calculate thresholds */
        limits[i] = limits[i-1]/10;
    }

    FALCON2_DINO_EFUN(falcon2_dino_display_eye_scan_header( pa, 1));
    j = 0;
    for (y = eyescan_options.vert_max;y>=eyescan_options.vert_min;y=y-eyescan_options.vstep) {
      FALCON2_DINO_ESTM_PRINTF(("%6dmV : ", falcon2_dino_ladder_setting_to_mV(pa,y, 0                 )));
      for(z=-31;z<eyescan_options.horz_min;z++) {
              FALCON2_DINO_EFUN_PRINTF((" "));    /* add leading spaces if any */
      }
      for (x=eyescan_options.horz_min;x<=eyescan_options.horz_max;x=x+eyescan_options.hstep) {
/*        val = float8_to_int32(buffer[j); */
        val = buffer[j];

        for (i=0;i<13;i++) {
          if ((val != 0) & ((val>=limits[i]) | (limits[i] == 0))) {
            for(z=1;z<=eyescan_options.hstep;z++) {
              if(z==1) {
                if(i<=8) {
                  FALCON2_DINO_EFUN_PRINTF(("%c", '1'+i));
                } else {
                  FALCON2_DINO_EFUN_PRINTF(("%c", 'A'+i-9));
                }
              } else {
                  FALCON2_DINO_EFUN_PRINTF((" "));
              }
            }
            break;
          }
        }
        if (i==13) {
           for(z=1;z<=eyescan_options.hstep;z++) {
              if(z==1) {
                if ((x%5)==0 && (y%5)==0) {
                   FALCON2_DINO_EFUN_PRINTF(("+"));
                } else if ((x%5)!=0 && (y%5)==0) {
                   FALCON2_DINO_EFUN_PRINTF(("-"));
                } else if ((x%5)==0 && (y%5)!=0) {
                   FALCON2_DINO_EFUN_PRINTF((":"));
                } else {
                   FALCON2_DINO_EFUN_PRINTF((" "));
                }
              } else {
                   FALCON2_DINO_EFUN_PRINTF((" "));
              }
           }
        }
        j++;
     }
     FALCON2_DINO_EFUN_PRINTF(("\n"));
   }
   FALCON2_DINO_EFUN(falcon2_dino_display_eye_scan_footer( pa, 1));
   return(ERR_CODE_NONE);
}

err_code_t falcon2_dino_meas_eye_scan_start( const phymod_access_t *pa, uint8_t direction) {
    uint8_t lock;

    FALCON2_DINO_ESTM(lock = falcon2_dino_rd_pmd_rx_lock());
    if(lock == 0) {
          FALCON2_DINO_EFUN_PRINTF(("Error: No PMD_RX_LOCK on lane requesting 2D eye scan\n"));
          return(ERR_CODE_DIAG_SCAN_NOT_COMPLETE);
    }
    if(direction == EYE_SCAN_VERTICAL) {
        FALCON2_DINO_EFUN(falcon2_dino_pmd_uc_diag_cmd( pa, CMD_UC_DIAG_START_VSCAN_EYE,200));
    } else {
        FALCON2_DINO_EFUN(falcon2_dino_pmd_uc_diag_cmd( pa, CMD_UC_DIAG_START_HSCAN_EYE,200));
    }
    return(ERR_CODE_NONE);
}

err_code_t falcon2_dino_read_eye_scan_stripe( const phymod_access_t *pa, uint32_t *buffer,uint16_t *status){
    uint32_t val[2] = {0,0};
    uint16_t sts = 0;
    int8_t   i;

    if(!buffer || !status) {
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    *status = 0;
    for(i=0;i<32;i++) {
        {   err_code_t err_code = falcon2_dino_poll_diag_eye_data(pa,&val[0], &sts, 200);
            *status |= sts & 0xF000;
            if (err_code) {
                return (err_code);
            }
        }
        buffer[i*2]     = val[0];
        buffer[(i*2)+1] = val[1];
    }
    *status |= sts & 0x00FF;
    return(ERR_CODE_NONE);
}

err_code_t falcon2_dino_display_eye_scan_stripe( const phymod_access_t *pa, int8_t y,uint32_t *buffer) {

    const uint32_t limits[7] = {1835008, 183501, 18350, 1835, 184, 18, 2};

    int8_t x,i;
    int16_t level;

    /* FALCON2_DINO_ESTM(data_thresh = falcon2_dino_rd_rx_data_thresh_sel()); */
    level = falcon2_dino_ladder_setting_to_mV(pa,y,0);

    if(!buffer) {
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    FALCON2_DINO_EFUN_PRINTF(("%6dmV : ",level));

    for (x=-31;x<32;x++) {
      for (i=0;i<7;i++)
        if (buffer[x+31]>=limits[i]) {
            FALCON2_DINO_EFUN_PRINTF(("%c", '0'+i+1));
            break;
        }
        if (i==7) {
            if      ((x%5)==0 && (y%5)==0) {FALCON2_DINO_EFUN_PRINTF(("+"));}
            else if ((x%5)!=0 && (y%5)==0) {FALCON2_DINO_EFUN_PRINTF(("-"));}
            else if ((x%5)==0 && (y%5)!=0) {FALCON2_DINO_EFUN_PRINTF((":"));}
            else                           {FALCON2_DINO_EFUN_PRINTF((" "));}
        }
    }
    return(ERR_CODE_NONE);
}

err_code_t falcon2_dino_display_eye_scan_header( const phymod_access_t *pa, int8_t i) {
int8_t x;
    FALCON2_DINO_EFUN_PRINTF(("\n"));
    FALCON2_DINO_EFUN_PRINTF((" Each character N represents approximate error rate 1e-N at that location\n"));
    for(x=1;x<=i;x++) {
        FALCON2_DINO_EFUN_PRINTF(("  UI/64  : -30  -25  -20  -15  -10  -5    0    5    10   15   20   25   30"));
    }
    FALCON2_DINO_EFUN_PRINTF(("\n"));
    for(x=1;x<=i;x++) {
        FALCON2_DINO_EFUN_PRINTF(("         : -|----|----|----|----|----|----|----|----|----|----|----|----|-"));
    }
    FALCON2_DINO_EFUN_PRINTF(("\n"));
    return(ERR_CODE_NONE);
}

err_code_t falcon2_dino_display_eye_scan_footer( const phymod_access_t *pa, int8_t i) {
int8_t x;
    for(x=1;x<=i;x++) {
        FALCON2_DINO_EFUN_PRINTF(("         : -|----|----|----|----|----|----|----|----|----|----|----|----|-"));
    }
    FALCON2_DINO_EFUN_PRINTF(("\n"));
    for(x=1;x<=i;x++) {
        FALCON2_DINO_EFUN_PRINTF(("  UI/64  : -30  -25  -20  -15  -10  -5    0    5    10   15   20   25   30"));
    }
    FALCON2_DINO_EFUN_PRINTF(("\n"));
    return(ERR_CODE_NONE);
}


/*eye_scan_status_t falcon2_dino_read_eye_scan_status() */
err_code_t falcon2_dino_read_eye_scan_status( const phymod_access_t *pa, uint16_t *status) {

   if(!status) {
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

   FALCON2_DINO_ESTM(*status=falcon2_dino_rdv_usr_diag_status());

    return(ERR_CODE_NONE);
}


err_code_t falcon2_dino_meas_eye_scan_done( const phymod_access_t *pa ) {
  FALCON2_DINO_EFUN(falcon2_dino_pmd_uc_diag_cmd( pa, CMD_UC_DIAG_DISABLE,200));
  return(ERR_CODE_NONE);
}


err_code_t falcon2_dino_start_ber_scan_test( const phymod_access_t *pa, uint8_t ber_scan_mode, uint8_t timer_control, uint8_t max_error_control) {
    uint8_t lock,sts;
    FALCON2_DINO_ESTM(lock = falcon2_dino_rd_pmd_rx_lock());
    if(lock == 0) {
        FALCON2_DINO_EFUN_PRINTF(("Error: No PMD_RX_LOCK on lane requesting BER scan\n"));
        return(ERR_CODE_DIAG_SCAN_NOT_COMPLETE);
    }
    FALCON2_DINO_ESTM(sts =falcon2_dino_rdv_usr_sts_micro_stopped());
    if(sts > 1) {
       FALCON2_DINO_EFUN_PRINTF(("Error: Lane is busy (%d) requesting BER scan\n",sts));
        return(ERR_CODE_DIAG_SCAN_NOT_COMPLETE);
    }

    FALCON2_DINO_EFUN(falcon2_dino_wrcv_diag_max_time_control(timer_control));
    FALCON2_DINO_EFUN(falcon2_dino_wrcv_diag_max_err_control(max_error_control));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_uc_cmd( pa, CMD_CAPTURE_BER_START, ber_scan_mode,500));
    return(ERR_CODE_NONE);
}

err_code_t falcon2_dino_read_ber_scan_data( const phymod_access_t *pa, uint32_t *errors, uint32_t *timer_values, uint8_t *cnt, uint32_t timeout) {
    uint8_t i,prbs_byte,prbs_multi,time_byte,time_multi;
    uint16_t sts,dataword;

    if(!errors || !timer_values || !cnt) {
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    /* init data arrays */
    for(i=0;i< FALCON2_DINO_DIAG_MAX_SAMPLES;i++) {
        errors[i]=0;
        timer_values[i]=0;
    }
    /* Check for completion read ln.diag_status byte?*/
 FALCON2_DINO_ESTM(sts = falcon2_dino_rdv_usr_diag_status());
    if((sts & 0x8000) == 0) {
        return(falcon2_dino_error(ERR_CODE_DATA_NOTAVAIL));
    }
    *cnt = (sts & 0x00FF)/3;
    for(i=0;i < *cnt;i++) {
        /* Read 2 bytes of data */
        FALCON2_DINO_EFUN(falcon2_dino_pmd_uc_cmd( pa,  CMD_READ_DIAG_DATA_WORD, 0, timeout));
        FALCON2_DINO_ESTM(dataword = falcon2_dino_rd_uc_dsc_data());           /* LSB contains 2 -4bit nibbles */
        time_byte = (uint8_t)(dataword>>8);    /* MSB is time byte */
        prbs_multi = (uint8_t)dataword & 0x0F; /* split nibbles */
        time_multi = (uint8_t)dataword>>4;
        /* Read 1 bytes of data */
        FALCON2_DINO_EFUN(falcon2_dino_pmd_uc_cmd( pa,  CMD_READ_DIAG_DATA_BYTE, 0, timeout));
        FALCON2_DINO_ESTM(prbs_byte = (uint8_t)falcon2_dino_rd_uc_dsc_data());
        errors[i] = falcon2_dino_float12_to_uint32( pa, prbs_byte,prbs_multi); /* convert 12bits to uint32 */
        timer_values[i] = (falcon2_dino_float12_to_uint32( pa, time_byte,time_multi)<<3);
    /*  FALCON2_DINO_EFUN_PRINTF(("Err=%d (%02x<<%d); Time=%d (%02x<<%d)\n",errors[i],prbs_byte,prbs_multi,timer_values[i],time_byte,time_multi<<3)); */
        /*if(timer_values[i] == 0 && errors[i] == 0) break;*/
    }

  return(ERR_CODE_NONE);
}


/* This is good example function to do BER extrapolation */
err_code_t falcon2_dino_eye_margin_proj(const phymod_access_t *pa, USR_DOUBLE rate, uint8_t ber_scan_mode, uint8_t timer_control, uint8_t max_error_control) {
    uint32_t errs[FALCON2_DINO_DIAG_MAX_SAMPLES];
    uint32_t time[FALCON2_DINO_DIAG_MAX_SAMPLES];
    uint8_t i,cnt;
    uint16_t sts;
    int16_t offset_start;
    /* Below 'verbose' level is intended to be modified only within a debug */
    /* session immediately after a breakpoint, and to retain its state only */
    /* through function exit:  therefore it must be 'volatile' to prevent a */
    /* compiler from eliding code conditioned on it, but NOT 'static'.      */
    uint8_t volatile verbose = 0;


    for(i=0;i<FALCON2_DINO_DIAG_MAX_SAMPLES;i++) {
        errs[i]=0;
        time[i]=0;
    }
    /* start UC acquisition */
    if(verbose > 2) FALCON2_DINO_EFUN_PRINTF(("start begin\n"));
    FALCON2_DINO_EFUN(falcon2_dino_start_ber_scan_test( pa, ber_scan_mode, timer_control, max_error_control));
    FALCON2_DINO_ESTM(offset_start = falcon2_dino_rd_uc_dsc_data());
    if(verbose > 2) FALCON2_DINO_EFUN_PRINTF(("offset_start = %d:%dmV\n",offset_start,falcon2_dino_ladder_setting_to_mV(pa,(int8_t)offset_start,0)));
    if(verbose > 2) FALCON2_DINO_EFUN_PRINTF(("start done\n"));

    /* This wait is VERY LONG and should be replaced with interupt or something */
    if(verbose > 5) {
        do {
            FALCON2_DINO_EFUN(falcon2_dino_delay_us(2000000));
            FALCON2_DINO_ESTM(sts = falcon2_dino_rdv_usr_diag_status());
            FALCON2_DINO_EFUN_PRINTF(("sts=%04x\n",sts));

        } while ((sts & 0x8000) == 0);
    } else {
        FALCON2_DINO_EFUN_PRINTF(("Waiting for measurement time approx %d seconds",timer_control+(timer_control>>1)));
        FALCON2_DINO_EFUN(falcon2_dino_poll_diag_done( pa, &sts,timer_control*2000));
    }
    if(verbose > 2) FALCON2_DINO_EFUN_PRINTF(("delay done\n"));

    FALCON2_DINO_EFUN(falcon2_dino_read_ber_scan_data( pa,  &errs[0], &time[0], &cnt, 2000));

    if(verbose > 2) FALCON2_DINO_EFUN_PRINTF(("read done cnt=%d\n",cnt));

    FALCON2_DINO_EFUN(falcon2_dino_pmd_uc_cmd( pa, CMD_CAPTURE_BER_END,0x00,50));

    if(verbose > 2) FALCON2_DINO_EFUN_PRINTF(("end function done\n"));
 /* if(cnt == 1) {                                                     */
 /*     FALCON2_DINO_EFUN_PRINTF(("Not enough points found to extrapolate BER\n")); */
 /*     return(ERR_CODE_NONE);                                         */
 /* }                                                                  */

    FALCON2_DINO_EFUN(falcon2_dino_display_ber_scan_data(pa, rate, ber_scan_mode, &errs[0], &time[0],(uint8_t)falcon2_dino_abs(offset_start)));

    if(verbose > 2) FALCON2_DINO_EFUN_PRINTF(("display done\n"));

    return(ERR_CODE_NONE);
}



err_code_t falcon2_dino_display_ber_scan_data (const phymod_access_t *pa, USR_DOUBLE rate, uint8_t ber_scan_mode, uint32_t *total_errs, uint32_t *total_time, uint8_t max_offset) {

#ifdef SERDES_API_FLOATING_POINT
    /* 'margins_mv[]' vector maps the p1 threshold code with actual mV
        Only relevant when mode=0
        This is not totally linear: for code 0~25 step=6mV; code 25~30 step=18mV; code 30~31 step=12
        'margins_mv[]' is valid only for Merlin. This vector would need to be modified accordingly for different Serdes
    USR_DOUBLE margins_mv[] = {0,6,12,18,24,30,36,42,48,54,60,66,72,78,84,
                           90,96,102,108,114,120,126,132,138,144,150,168,186,204,222,240,252};
    const USR_DOUBLE narrow_margins_mv[] = {0,3.6,7.2,10.8,14.4,18,21.6,25.5,28.8,32.4,36,39.6,43.2,46.8,50.4,
                                        54,57.6,61.2,64.8,68.4,72,75.6,79.2,82.8,86.4,90,100.8,111.6,122.4,133.2,144,151.2}; */
    const unsigned int HI_CONFIDENCE_ERR_CNT = 100;      /* bit errors */
    const unsigned int HI_CONFIDENCE_MIN_ERR_CNT = 20;   /* bit errors */
    const unsigned int MAX_CLIPPED_ERR_CNT = 8355840;
    const USR_DOUBLE ARTIFICIAL_BER = 0.5;    /* used along ARTIFICIAL_MARGIN(_V/_H) when not enough points to extrapolate */
    const int ARTIFICIAL_MARGIN_V = 500;    /* Used along ARTIFICIAL_BER when not enough points to extrapolate. Unit: mV. Based on the concept of max Vpp of 1 Volt */
    const int ARTIFICIAL_MARGIN_H = 1;    /* Used along ARTIFICIAL_BER when not enough points to extrapolate. Unit: UI. Based on the concept of two consecutive scrambled bits (1 UI appart) being uncorrelated */
    const int MIN_BER_TO_REPORT = -24;   /* we clip the projected BER using this number */
    const USR_DOUBLE MIN_BER_FOR_FIT = -8.0;    /*    all points with BER <= 10^MIN_BER_FOR_FIT will be used for line fit (i.e used for extrapolation) */
    const int8_t verbose = 1;    /* set verbosity to 1 for normal API operation */

    /* BER confidence scale */
    const USR_DOUBLE ber_conf_scale[104] = { 
        2.9957,5.5717,3.6123,2.9224,2.5604,2.3337,2.1765,2.0604,1.9704,1.8983,
        1.8391,1.7893,1.7468,1.7100,1.6778,1.6494,1.6239,1.6011,1.5804,1.5616,
        1.5444,1.5286,1.5140,1.5005,1.4879,1.4762,1.4652,1.4550,1.4453,1.4362,
        1.4276,1.4194,1.4117,1.4044,1.3974,1.3908,1.3844,1.3784,1.3726,1.3670,
        1.3617,1.3566,1.3517,1.3470,1.3425,1.3381,1.3339,1.3298,1.3259,1.3221,
        1.3184,1.3148,1.3114,1.3080,1.3048,1.3016,1.2986,1.2956,1.2927,1.2899,
        1.2872,1.2845,1.2820,1.2794,1.2770,1.2746,1.2722,1.2700,1.2677,1.2656,
        1.2634,1.2614,1.2593,1.2573,1.2554,1.2535,1.2516,1.2498,1.2481,1.2463,
        1.2446,1.2429,1.2413,1.2397,1.2381,1.2365,1.2350,1.2335,1.2320,1.2306,
        1.2292,1.2278,1.2264,1.2251,1.2238,1.2225,1.2212,1.2199,1.2187,1.2175,    
        1.2163,    /* starts in index: 100 for #errors: 100,200,300,400 */
        1.1486,    /* 200 */
        1.1198,    /* 300 */
        1.1030};    /*400 */


    /* Define variables */
    USR_DOUBLE lbers[FALCON2_DINO_DIAG_MAX_SAMPLES];                /* Internal linear scale sqrt(-log(ber)) */
    USR_DOUBLE margins[FALCON2_DINO_DIAG_MAX_SAMPLES];                /* Eye margin @ each measurement */
    USR_DOUBLE bers[FALCON2_DINO_DIAG_MAX_SAMPLES];                /* computed bit error rate */
    uint32_t i;
    int8_t offset[FALCON2_DINO_DIAG_MAX_SAMPLES];
    int8_t mono_flags[FALCON2_DINO_DIAG_MAX_SAMPLES];
    
    int8_t direction;
    uint8_t heye;
    int8_t delta_n;
    USR_DOUBLE Exy = 0.0;
    USR_DOUBLE Eyy = 0.0;
    USR_DOUBLE Exx = 0.0;
    USR_DOUBLE Ey  = 0.0;
    USR_DOUBLE Ex  = 0.0;
    USR_DOUBLE alpha = 0.0;
    USR_DOUBLE gauss_noise = -1;
    USR_DOUBLE beta = 0.0;
    USR_DOUBLE sq_r = 0.0, alpha2 = 0.0;
    USR_DOUBLE proj_ber = 0.0, proj_ber_aux = 0.0;
    USR_DOUBLE proj_margin_12 = 0.0;
    USR_DOUBLE proj_margin_15 = 0.0;
    USR_DOUBLE proj_margin_18 = 0.0;
    USR_DOUBLE sq_err1 = 0.0, sq_err2 = 0.0;
    USR_DOUBLE ierr;
    uint8_t start_n;
    uint8_t stop_n;
    uint8_t low_confidence;
    uint8_t loop_index;
    uint8_t n_mono = 0;
    uint8_t eye_cnt = 1;
    uint8_t hi_confidence_cnt = 0;
    int8_t first_good_ber_idx = -1;
    int8_t first_small_errcnt_idx = -1;
    int8_t first_non_clipped_errcnt_idx = -1;
    uint8_t range250;
    uint8_t intrusive;
    uint8_t ber_clipped = 0;
    uint8_t last_point_discard;
    uint8_t fit_count;    
    int artificial_margin;
    int proj_case = 0;    /* this variable will be used to signal what particular extrapolation case has happened at the end (after discarding invalid points of all sorts). To avoid potential issues: NEVER RE-USE VALUES... new cases should receive brand-new integer value */
    USR_DOUBLE artificial_lber;
    char message[256] = "NO MESSAGE";
    char unit[5];
    
    if(!total_errs || !total_time ) {
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    /* Initialize BER array */
    for (i = 0; i < FALCON2_DINO_DIAG_MAX_SAMPLES; i++) {
        bers[i] = 0;
        mono_flags[i] = 0;
    }
    
    /* Decode mode/direction/etc. */
    heye = (ber_scan_mode & DIAG_BER_HORZ)>>1;
    direction = (ber_scan_mode & DIAG_BER_NEG) ? -1 : 1 ;
    range250 = (ber_scan_mode & DIAG_BER_P1_NARROW) ? 0 : 1;
    intrusive = (ber_scan_mode & DIAG_BER_INTR) ? 1 : 0;
        
    /* Prepare artificial points in case they are needed */
    if (heye == 1) {
        artificial_margin = direction*ARTIFICIAL_MARGIN_H;
    } else {
        artificial_margin = direction*ARTIFICIAL_MARGIN_V;
    }
    artificial_lber = (USR_DOUBLE)sqrt(-log10(ARTIFICIAL_BER));
    
    /* Printing on-screen message */
    if (heye == 1) {
         if (verbose > 0) {
            if (direction==-1) FALCON2_DINO_EFUN_PRINTF(("\n\n********** HORIZONTAL PROJECTION: LEFT SIDE ******************\n"));
            if (direction==1) FALCON2_DINO_EFUN_PRINTF(("\n\n********** HORIZONTAL PROJECTION: RIGHT SIDE *****************\n"));
        }
    } else {
        if (verbose > 0) {
            if (direction==-1) FALCON2_DINO_EFUN_PRINTF(("\n\n********** VERTICAL PROJECTION: BOTTOM ***********************\n"));
            if (direction==1) FALCON2_DINO_EFUN_PRINTF(("\n\n********** VERTICAL PROJECTION: TOP **************************\n"));
        }
    }

    /* *******************************************
        * Generate margins[]
        * Generate ber[]
        * Find first and last points for linear fit 
        ******************************************* */
    i=0;
    do {
        if(heye == 1) {
            FALCON2_DINO_ENULL_STRCPY(unit,"mUI");
            offset[i] = (int8_t)(max_offset-i);
#ifndef STANDALONE_EVENT
            margins[i] = direction*offset[i]*1000.0/64.0;
#else
            margins[i] = info_out->margins[i];
#endif
        } else {
            FALCON2_DINO_ENULL_STRCPY(unit,"mV");
            offset[i] = (int8_t)(max_offset-i);
#ifndef STANDALONE_EVENT
            if(intrusive) {
                margins[i] = direction*falcon2_dino_ladder_setting_to_mV(pa,offset[i], 0);
            } else {
                margins[i] = direction*falcon2_dino_ladder_setting_to_mV(pa,offset[i], range250);
            }
#else
            margins[i] = info_out->margins[i];
#endif
        }
        if (total_errs[i] == 0) {
            bers[i] = 1.0/(((USR_DOUBLE)total_time[i])*0.00001*rate);
        } else {
            bers[i] = (USR_DOUBLE)total_errs[i]/(((USR_DOUBLE)total_time[i])*0.00001*rate);
        }

        /* Find the first data point with good BER (BER <= 10^MIN_BER_FOR_FIT) 
        NOTE: no need for lower bound on BER, since correction factors will be applied for all total_errs>=0 */
        if ((log10(bers[i]) <= MIN_BER_FOR_FIT) && (first_good_ber_idx == -1)) {
            first_good_ber_idx = (int8_t)i;
        }

        /* Determine high-confidence iterations */
        if (total_errs[i] >= HI_CONFIDENCE_ERR_CNT) {
            hi_confidence_cnt++;
        } else if ((total_errs[i] < HI_CONFIDENCE_MIN_ERR_CNT) && (first_small_errcnt_idx == -1)) {
            /* find the first data point with small error count */
            first_small_errcnt_idx = (int8_t)i;
        }
        
        /* Determine first NON-clipped error count
            NOTE: Originally this limit was created for post processing of micro-generated data; however, this could be used for PC-generated data as well */
        if ((total_errs[i] < MAX_CLIPPED_ERR_CNT) && (first_non_clipped_errcnt_idx == -1) ) {
            first_non_clipped_errcnt_idx = (int8_t)i;
        } 
        
      i++;

    } while(((total_errs[i] != 0) || (total_time[i] != 0)) && (i<=max_offset));
        
    eye_cnt = (int8_t) i;
    
    
    /* *******************************************
    Setting up stop_n variable.
    Check if:
        - There is only one point in measurement vector (i.e. eye_cnt = 1)
        - The very last point's measurement time was "too short"
    ******************************************* */
    
    i = eye_cnt - 1;
    if (i>=1) {
        if ((total_time[i] >= 0.5*total_time[i-1]) || (total_errs[i] >= HI_CONFIDENCE_MIN_ERR_CNT) ){
            stop_n = eye_cnt;    /* last point will be included in linear fit */
            last_point_discard = 0;
        } else {
            stop_n = eye_cnt - 1;    /* discards the very last point */
            last_point_discard = 1;
        }
    } else {
        stop_n = 1; /* there is ONLY one measurement */
    }
    

    /* *******************************************
    Print on screen (prints RAW BER data. i.e. conf factors)
    ******************************************* */
    if (verbose > 0) {
        i = 0;
        do {
            if (total_errs[i] == 0) {
                FALCON2_DINO_EFUN_PRINTF(("BER @ %4.0f %s < 1e%-6.2f (%u errors in %0.2f sec)\n", margins[i], unit, log10(bers[i]), total_errs[i], ((USR_DOUBLE)total_time[i])*0.00001));
            } else if (total_errs[i] >= MAX_CLIPPED_ERR_CNT) {
                FALCON2_DINO_EFUN_PRINTF(("BER @ %4.0f %s > 1e%-6.2f (%u errors in %0.2f sec)\n", margins[i], unit, log10(bers[i]), total_errs[i], ((USR_DOUBLE)total_time[i])*0.00001));
            } else {
                FALCON2_DINO_EFUN_PRINTF(("BER @ %4.0f %s = 1e%-6.2f (%u errors in %0.2f sec)\n", margins[i], unit, log10(bers[i]), total_errs[i], ((USR_DOUBLE)total_time[i])*0.00001));
            }
            i++;
        } while (i<stop_n);
    }

    /* *******************************************
    Correcting *all* BER values using confidence factors in 'ber_conf_scale' vector
    This step is done for extrapolation purposes 
    ******************************************* */
    for (loop_index=0; loop_index < eye_cnt; loop_index++) {
        if (total_errs[loop_index] <= 100) {
            bers[loop_index] = ber_conf_scale[total_errs[loop_index]] * bers[loop_index];
        } else if (total_errs[loop_index] > 100 && total_errs[loop_index] < 200) {
            bers[loop_index] = ber_conf_scale[100] * bers[loop_index];
        } else if (total_errs[loop_index] >= 200 && total_errs[loop_index] < 300) {
            bers[loop_index] = ber_conf_scale[101] * bers[loop_index];
        } else if (total_errs[loop_index] >= 300 && total_errs[loop_index] < 400) {
            bers[loop_index] = ber_conf_scale[102] * bers[loop_index];
        } else if (total_errs[loop_index] >= 400) {
            bers[loop_index] = ber_conf_scale[103] * bers[loop_index];
        }
    }

    /* *******************************************
    Computes the "linearised" ber vector 
    ******************************************* */
    for (loop_index=0; loop_index<eye_cnt; loop_index++) {
        lbers[loop_index] = (USR_DOUBLE)sqrt(-log10(bers[loop_index]));
    }

    /* *******************************************
    Assign highest data point to use 
    ******************************************* */
    if (first_good_ber_idx == -1) {
        start_n = stop_n;
    } else {
        start_n = first_good_ber_idx;
    }


    
    /* ******************************************************
    ***********  EXTRAPOLATION (START) **********************
    *********************************************************
    Different data set profiles can be received by this code.
    Each case is processed accordingly here (IF-ELSE IF cases)
    ****************************************************** */

    /* ====> Errors encountered all the way to sampling point */
    if (start_n >= eye_cnt) {
        proj_case = 1;
        FALCON2_DINO_ENULL_STRCPY (message,"No low-BER point measured");
        
        /* confidence factor of 0.96 is applied in this case to set a LOWER bound and report accordingly.
            This factor corresponds to approximately 3000 errors @95% confidence
            For reference: factors for 900, 2000, 3000, 5000, 20000 and 50000 errors are: 0.96, 0.96, 0.97, 0.99, 0.99, respectively */
        proj_ber = 0.96*log10(bers[eye_cnt-1]);
        proj_ber_aux = proj_ber;
        FALCON2_DINO_EFUN_PRINTF(("BER *worse* than 1e%0.2f\n", proj_ber));
        FALCON2_DINO_EFUN_PRINTF(("No margin @ 1e-12, 1e-15 & 1e-18\n\n\n"));
        fit_count = 1;
    } 
    
    else {
    
        /* ====> Only ONE measured point. Typically when the eye is wide open.
            Artificial points will be used to make extrapolation possible */
        if (stop_n==1) {
            proj_case = 2;
            FALCON2_DINO_ENULL_STRCPY (message,"Not enough points (single measured point). Using artificial point");
            
            low_confidence = 1;
            delta_n = 1;    /* 'delta_n' and 'fit_count' variables were kept for future use in case a new approach to handle low confidence case is adopted */
            fit_count = 2;
            
            /* Compute covariances and means... but only for two points: artificial and the single measured point */
            Exy = ((margins[0]*lbers[0] + artificial_margin*artificial_lber)/2.0);
            Eyy = ((lbers[0]*lbers[0] + artificial_lber*artificial_lber)/2.0);
            Exx = ((margins[0]*margins[0] + artificial_margin*artificial_margin)/2.0);
            Ey  = ((lbers[0] + artificial_lber)/2.0);
            Ex  = ((margins[0] + artificial_margin)/2.0);
        }
    
        /* ====> "NORMAL" case (when there are more than 1 measurements) */
        else {
        
            /* Detect and record nonmonotonic data points */
            for (loop_index=0; loop_index < stop_n; loop_index++) {
                if ((loop_index > start_n) && (log10(bers[loop_index]) > log10(bers[loop_index-1]))) {
                    mono_flags[loop_index] = 1;
                    if (first_good_ber_idx != -1) {
                        n_mono++;
                    }
                }
            }
            
            /* Finds number of MEASURED points available for extrapolation */
            delta_n = (stop_n-start_n-n_mono);


            /*    HIGH CONFIDENCE case */
            
            if (delta_n >= 2) {    /* there are at least 2 points to trace a line */
                proj_case = 3;
                FALCON2_DINO_ENULL_STRCPY (message,"Normal case");
                low_confidence = 0;
                
                /* Compute covariances and means */
                fit_count = 0;
                for (loop_index=start_n; loop_index < stop_n; loop_index++) {
                    if (mono_flags[loop_index] == 0) {
                        Exy += (margins[loop_index]*lbers[loop_index]/(USR_DOUBLE)delta_n);
                        Eyy += (lbers[loop_index]*lbers[loop_index]/(USR_DOUBLE)delta_n);
                        Exx += (margins[loop_index]*margins[loop_index]/(USR_DOUBLE)delta_n);
                        Ey  += (lbers[loop_index]/(USR_DOUBLE)delta_n);
                        Ex  += (margins[loop_index]/(USR_DOUBLE)delta_n);
                        fit_count++;    
                    }
                }
            } 
                
            /*    LOW CONFIDENCE case */
            
            else {    /*    NEW APPROACH (08/28/2014): consider very first point (error count < MAX_CLIPPED_ERR_CNT) and very last point for linear fit. This will give pessimistic/conservative extrapolation */
                low_confidence = 1;
                if ( (first_non_clipped_errcnt_idx>=0) && (first_non_clipped_errcnt_idx < start_n)) {
                    proj_case = 4;
                    FALCON2_DINO_ENULL_STRCPY (message,"Not enough points. Using first measured point for conservative estimation");
                    fit_count = 2;
                    /* Compute covariances and means... but only for two points: first and last */
                    Exy = ((margins[stop_n-1]*lbers[stop_n-1] + margins[first_non_clipped_errcnt_idx]*lbers[first_non_clipped_errcnt_idx])/2.0);
                    Eyy = ((lbers[stop_n-1]*lbers[stop_n-1] + lbers[first_non_clipped_errcnt_idx]*lbers[first_non_clipped_errcnt_idx])/2.0);
                    Exx = ((margins[stop_n-1]*margins[stop_n-1] + margins[first_non_clipped_errcnt_idx]*margins[first_non_clipped_errcnt_idx])/2.0);
                    Ey  = ((lbers[stop_n-1] + lbers[first_non_clipped_errcnt_idx])/2.0);
                    Ex  = ((margins[stop_n-1] + margins[first_non_clipped_errcnt_idx])/2.0);
                } else {
                    proj_case = 5;
                    FALCON2_DINO_ENULL_STRCPY (message,"Not enough points (cannot use non-clipped ErrorCount point). Using artificial point");
                    /* Compute covariances and means... but only for two points: artificial and the single measured point */
                    Exy = (artificial_margin*artificial_lber)/2.0;
                    Eyy = (artificial_lber*artificial_lber)/2.0;
                    Exx = (artificial_margin*artificial_margin)/2.0;
                    Ey  = (artificial_lber)/2.0;
                    Ex  = (artificial_margin)/2.0;
                    fit_count = 1;
                    /* This FOR loop checks for monotonicity as well */
                    for (loop_index=start_n; loop_index < stop_n; loop_index++) {
                        if (mono_flags[loop_index] == 0) {
                            Exy += (margins[loop_index]*lbers[loop_index]/2.0);
                            Eyy += (lbers[loop_index]*lbers[loop_index]/2.0);
                            Exx += (margins[loop_index]*margins[loop_index]/2.0);
                            Ey  += (lbers[loop_index]/2.0);
                            Ex  += (margins[loop_index]/2.0);
                            fit_count++;
                        }
                    }
                }
            }
        }
        
        /* Compute fit slope and offset: ber = alpha*margin + beta */
        alpha = (Exy - Ey*Ex) / (Exx - Ex*Ex);
        beta = Ey - Ex*alpha;
        /* Compute alpha2: slope of regression: margin = alpha2*ber + beta2 */
        alpha2 = (Exy - Ey*Ex) / (Eyy - Ey*Ey);
        /* Compute correlation index sq_r */
        sq_r = alpha*alpha2;

        proj_ber = pow(10,(-beta*beta));
        proj_margin_12 = direction*(sqrt(-log10(1e-12))-beta)/alpha;
        proj_margin_15 = direction*(sqrt(-log10(1e-15))-beta)/alpha;
        proj_margin_18 = direction*(sqrt(-log10(1e-18))-beta)/alpha;

        /* Estimate modeled gaussian noise.
        
            The following is based on the Q-function model and the following table:
                Q    |    log10(BER)
            =======================
              7.033    |    -12
              7.941    |    -15
              8.757    |    -18
              
        Based on the above, we solve for sigma:
            7.033*sigma = u - proj_margin_12 , and
            7.941*sigma = u - proj_margin_15
        */
        gauss_noise = (proj_margin_12 - proj_margin_15)/0.908;
        
        sq_err1 = (Eyy + (beta*beta) + (Exx*alpha*alpha) - 
                   (2*Ey*beta) - (2*Exy*alpha) + (2*Ex*beta*alpha));
        sq_err2 = 0;
        for (loop_index=start_n; loop_index<stop_n; loop_index++) {
            ierr = (lbers[loop_index] - (alpha*margins[loop_index] + beta));
            sq_err2 += (ierr*ierr/(USR_DOUBLE)delta_n);
        }
        
        proj_ber = log10(proj_ber);
        proj_ber_aux = proj_ber;

        if (proj_ber < MIN_BER_TO_REPORT) {
            proj_ber = MIN_BER_TO_REPORT;
            ber_clipped = 1;
        }
        
        /* Extrapolated results, low confidence */
        if (low_confidence == 1) {

            FALCON2_DINO_EFUN_PRINTF(("BER(extrapolated) < 1e%0.2f\n", proj_ber));
            FALCON2_DINO_EFUN_PRINTF(("Margin @ 1e-12    > %0.2f %s\n", (proj_ber < -12)? falcon2_dino_abs(proj_margin_12) : 0, unit));
            FALCON2_DINO_EFUN_PRINTF(("Margin @ 1e-15    > %0.2f %s\n", (proj_ber < -15)? falcon2_dino_abs(proj_margin_15) : 0, unit));
            FALCON2_DINO_EFUN_PRINTF(("Margin @ 1e-18    > %0.2f %s\n", (proj_ber < -18)? falcon2_dino_abs(proj_margin_18) : 0, unit));
            
        /* Extrapolated results, HIGH confidence */            
        } else {
          
            if (ber_clipped == 1) {
                FALCON2_DINO_EFUN_PRINTF(("BER(extrapolated) < 1e%0.2f\n", proj_ber));
            } else {
                FALCON2_DINO_EFUN_PRINTF(("BER(extrapolated) = 1e%0.2f\n", proj_ber));
            }
            FALCON2_DINO_EFUN_PRINTF(("Margin @ 1e-12    = %0.2f %s\n", (proj_ber < -12)? falcon2_dino_abs(proj_margin_12) : 0, unit));
            FALCON2_DINO_EFUN_PRINTF(("Margin @ 1e-15    = %0.2f %s\n", (proj_ber < -15)? falcon2_dino_abs(proj_margin_15) : 0, unit));
            FALCON2_DINO_EFUN_PRINTF(("Margin @ 1e-18    = %0.2f %s\n", (proj_ber < -18)? falcon2_dino_abs(proj_margin_18) : 0, unit));
        }
        
        FALCON2_DINO_EFUN_PRINTF(("\n\n"));
        
        /* Print non-monotonic outliers */
        if (n_mono != 0) {
            FALCON2_DINO_EFUN_PRINTF(("Detected non-monotonicity at { "));
            for (loop_index = start_n; loop_index < stop_n; loop_index++) {
                if (mono_flags[loop_index] == 1) {
                    FALCON2_DINO_EFUN_PRINTF(("%0.2f ", margins[loop_index]));
                }
            } 
            FALCON2_DINO_EFUN_PRINTF(("} %s\n\n\n",unit));
        }        
        
    }
    /* *******************************************
    ***********  EXTRAPOLATION (END) *************
    ********************************************* */


    
    /* SUMMARY (for debugging purposes */
    if (verbose > 2) FALCON2_DINO_EFUN_PRINTF (("\t=====> DEBUG INFO (start)\n\n"));
    if (verbose > 2) {
        FALCON2_DINO_EFUN_PRINTF((" loop   Margin      total_errors   time(sec)  logBER       lber"));
        for (loop_index=0; loop_index < stop_n+last_point_discard; loop_index++) {
            FALCON2_DINO_EFUN_PRINTF(("\n%5d %11.0f %14d %10.3f %8.2f %10.3f", loop_index, margins[loop_index], total_errs[loop_index], ((USR_DOUBLE)total_time[loop_index])*0.00001, log10(bers[loop_index]), lbers[loop_index]));
        }
        FALCON2_DINO_EFUN_PRINTF(("\n\n"));
    }
    if (verbose > 2) FALCON2_DINO_EFUN_PRINTF(("Max Offset = %d\n",max_offset));    
    if (verbose > 2) FALCON2_DINO_EFUN_PRINTF(("ber_clipped: %d, Projected BER (proj_ber_aux) = %.2f\n", ber_clipped, proj_ber_aux));    
    if (verbose > 2) FALCON2_DINO_EFUN_PRINTF(("first good ber idx at %d, ber = 1e%f\n", first_good_ber_idx, ((first_good_ber_idx>=0) ? log10(bers[first_good_ber_idx]) : 0.0)));
    if (verbose > 2) FALCON2_DINO_EFUN_PRINTF(("first small errcnt idx at %d, errors = %d\n", first_small_errcnt_idx, ((first_small_errcnt_idx>=0) ? total_errs[first_small_errcnt_idx] : -1)));
    if (verbose > 2) FALCON2_DINO_EFUN_PRINTF(("last point discarded?: %d, low_confidence: %d, first_non_clipped_errcnt_idx: %d, start_n: %d, stop_n: %d, eye_cnt: %d, n_mono: %d, first_good_ber_idx = %d, first_small_errcnt_idx = %d, fit_count: %d, delta_n: %d\n", 
                last_point_discard, low_confidence, first_non_clipped_errcnt_idx, start_n, stop_n, eye_cnt, n_mono, first_good_ber_idx, first_small_errcnt_idx, fit_count, delta_n));
    if (verbose > 2) FALCON2_DINO_EFUN_PRINTF(("Exy=%.2f, Eyy=%.4f, Exx=%.2f, Ey=%.4f, Ex=%.2f, alpha=%.4f, beta=%.4f, alpha2=%.3f, sq_r=%.3f, sq_err1=%g, sq_err2=%g, gauss_noise=%.3f\n", Exy,Eyy,Exx,Ey,Ex,alpha,beta,alpha2,sq_r,sq_err1,sq_err2,gauss_noise));
    if (verbose > 2) FALCON2_DINO_EFUN_PRINTF(("%s\n", message));
    if (verbose > 2) FALCON2_DINO_EFUN_PRINTF(("proj_case %d\n",proj_case));
    if (verbose > 2) FALCON2_DINO_EFUN_PRINTF (("\n\t=====> DEBUG INFO (end)\n\n"));
    FALCON2_DINO_EFUN_PRINTF(("\n\n\n"));

    
    
#else
    (void)rate;
    (void)ber_scan_mode;
    (void)max_offset;

    FALCON2_DINO_EFUN_PRINTF(("This function needs SERDES_API_FLOATING_POINT define to operate \n"));

    if(!total_errs || !total_time ) {
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
#endif
    return(ERR_CODE_NONE);

}




/*****************************/
/*  Display Lane/Core State  */
/*****************************/

err_code_t falcon2_dino_display_lane_state_hdr( const phymod_access_t *pa ) {
  FALCON2_DINO_EFUN_PRINTF(("LN "));
  FALCON2_DINO_EFUN_PRINTF(("(CDRxN      , UC_CFG,RST,STP)  "));
  
  FALCON2_DINO_EFUN_PRINTF(("SD LCK RXPPM "));
  FALCON2_DINO_EFUN_PRINTF(("CLK90 CLKP1 "));
  FALCON2_DINO_EFUN_PRINTF(("PF(M,L)  "));
  FALCON2_DINO_EFUN_PRINTF(("VGA DCO "));
  FALCON2_DINO_EFUN_PRINTF(("P1mV "));
  FALCON2_DINO_EFUN_PRINTF(("M1mV "));
  FALCON2_DINO_EFUN_PRINTF((" DFE(1,2,3,4,5,6)        "));
    FALCON2_DINO_EFUN_PRINTF(("TXPPM "));
  FALCON2_DINO_EFUN_PRINTF(("TXEQ(n1,m,p1,2,3) "));
    FALCON2_DINO_EFUN_PRINTF(("TXAMP "));
  FALCON2_DINO_EFUN_PRINTF(("  EYE(L,R,U,D)  "));
  FALCON2_DINO_EFUN_PRINTF(("LINK_TIME"));
  FALCON2_DINO_EFUN_PRINTF(("\n"));
  return (ERR_CODE_NONE);
}

err_code_t falcon2_dino_display_lane_state_legend( const phymod_access_t *pa ) {
  FALCON2_DINO_EFUN_PRINTF(("\n"));
  FALCON2_DINO_EFUN_PRINTF(("**********************************************************************************************\n"));
  FALCON2_DINO_EFUN_PRINTF(("****                Legend of Entries in display_lane_state()                             ****\n"));
  FALCON2_DINO_EFUN_PRINTF(("**********************************************************************************************\n"));
  FALCON2_DINO_EFUN_PRINTF(("LN        : lane index within IP core\n"));
  FALCON2_DINO_EFUN_PRINTF(("CDRxN     : CDR type x OSR ratio\n"));
  FALCON2_DINO_EFUN_PRINTF(("UC_CFG    : micro lane configuration variable\n"));
  FALCON2_DINO_EFUN_PRINTF(("TX_RX_RST : TX and RX Reset State {reset_active, reset_occured, reset_held}\n"));
  FALCON2_DINO_EFUN_PRINTF(("STP       : uC Stopped State\n"));
  FALCON2_DINO_EFUN_PRINTF(("SD        : signal detect\n"));
  FALCON2_DINO_EFUN_PRINTF(("LCK       : pmd_rx_lock\n"));
  FALCON2_DINO_EFUN_PRINTF(("RXPPM     : Frequency offset of local reference clock with respect to RX data in ppm\n"));
  FALCON2_DINO_EFUN_PRINTF(("CLK90     : Delay of zero crossing slicer, m1, wrt to data in PI codes\n"));
  FALCON2_DINO_EFUN_PRINTF(("CLKP1     : Delay of diagnostic/lms slicer, p1, wrt to data in PI codes\n"));
  FALCON2_DINO_EFUN_PRINTF(("PF(M,L)   : Peaking Filter Main (0..15) and Low Frequency (0..7) settings\n"));
  FALCON2_DINO_EFUN_PRINTF(("VGA       : Variable Gain Amplifier settings (0..39)\n"));
  FALCON2_DINO_EFUN_PRINTF(("DCO       : DC offset DAC control value\n"));
  FALCON2_DINO_EFUN_PRINTF(("P1mV      : Vertical threshold voltage of p1 slicer\n"));
  FALCON2_DINO_EFUN_PRINTF(("M1mV      : Vertical threshold voltage of m1 slicer\n"));
  FALCON2_DINO_EFUN_PRINTF(("DFE taps  : ISI correction taps in units of 2.35mV\n"));
  FALCON2_DINO_EFUN_PRINTF(("TXPPM            : Frequency offset of local reference clock with respect to TX data in ppm\n"));
  FALCON2_DINO_EFUN_PRINTF(("TXEQ(n1,m,p1,p2,p3) : TX equalization FIR tap weights in units of 1Vpp/160 units\n"));
  FALCON2_DINO_EFUN_PRINTF(("TXAMP : TX amplitude, Drivermode\n"));
  FALCON2_DINO_EFUN_PRINTF(("EYE(L,R,U,D)     : Eye margin @ 1e-5 as seen by internal diagnostic slicer in mUI and mV\n"));
  FALCON2_DINO_EFUN_PRINTF(("LINK_TIME        : Link time in milliseconds\n"));
  FALCON2_DINO_EFUN_PRINTF(("**********************************************************************************************\n"));
  return (ERR_CODE_NONE);
}

err_code_t falcon2_dino_display_lane_state( const phymod_access_t *pa ) {
  err_code_t err_code = _falcon2_dino_display_lane_state_no_newline( pa );
  FALCON2_DINO_EFUN_PRINTF(("\n"));
  return (err_code);
}

err_code_t falcon2_dino_get_eye_margin_est( const phymod_access_t *pa, int *left_eye_mUI, int *right_eye_mUI, int *upper_eye_mV, int *lower_eye_mV) {
  uint8_t ladder_range = 0;


  FALCON2_DINO_ESTM(*left_eye_mUI = falcon2_dino_eye_to_mUI(falcon2_dino_rdv_usr_sts_heye_left()));
  FALCON2_DINO_ESTM(*right_eye_mUI = falcon2_dino_eye_to_mUI(falcon2_dino_rdv_usr_sts_heye_right()));
  FALCON2_DINO_ESTM(*upper_eye_mV = falcon2_dino_eye_to_mV(pa, falcon2_dino_rdv_usr_sts_veye_upper(), ladder_range));
  FALCON2_DINO_ESTM(*lower_eye_mV = falcon2_dino_eye_to_mV(pa, falcon2_dino_rdv_usr_sts_veye_lower(), ladder_range));
  return (ERR_CODE_NONE);
}

err_code_t falcon2_dino_display_core_state_hdr( const phymod_access_t *pa ) {
           
    FALCON2_DINO_EFUN_PRINTF(("SerDes type = %s\n","falcon2_dino"));

  FALCON2_DINO_EFUN_PRINTF(("CORE RST ST PLL_PWDN  UC_ATV   COM_CLK   UCODE_VER  AFE_VER   LIVE_TEMP   AVG_TMON   RESCAL   VCO_RATE  ANA_VCO_RANGE   PLL_DIV    PLL_LOCK\n"));
  return (ERR_CODE_NONE);
}

err_code_t falcon2_dino_display_core_state_line( const phymod_access_t *pa ) {
  err_code_t err_code = _falcon2_dino_display_core_state_no_newline( pa );
  FALCON2_DINO_EFUN_PRINTF(("\n"));
  return (err_code);
}

err_code_t falcon2_dino_display_core_state_legend( const phymod_access_t *pa ) {
  FALCON2_DINO_EFUN_PRINTF(("\n"));
  FALCON2_DINO_EFUN_PRINTF(("**************************************************************************************************************\n"));
  FALCON2_DINO_EFUN_PRINTF(("****                          Legend of Entries in display_core_state()                                   ****\n"));
  FALCON2_DINO_EFUN_PRINTF(("**************************************************************************************************************\n"));
  FALCON2_DINO_EFUN_PRINTF(("*  RST              : Core DP Reset State{reset_active, reset_occured, reset_held}                           *\n"));
  FALCON2_DINO_EFUN_PRINTF(("*  ST               : Core uC Status byte(hex)                                                               *\n"));
  FALCON2_DINO_EFUN_PRINTF(("*  PLL_PWDN         : PLL Powerdown Control Bit (active high)                                                *\n"));
  FALCON2_DINO_EFUN_PRINTF(("*  UC_ATV           : UC Active bit                                                                          *\n"));
  FALCON2_DINO_EFUN_PRINTF(("*  COM_CLK          : COM Clock frequency in MHz                                                             *\n"));
  FALCON2_DINO_EFUN_PRINTF(("*  UCODE_VER        : Microcode Version [majorversion_minorversion]                                          *\n"));
  FALCON2_DINO_EFUN_PRINTF(("*  AFE_VER          : AFE Hardware Vesrion                                                                   *\n"));
  FALCON2_DINO_EFUN_PRINTF(("*  LIVE_TEMP        : Live Die temperature in Celsius                                                        *\n"));
  FALCON2_DINO_EFUN_PRINTF(("*  AVG_TMON         : uC Temp_idx, Average temperature in Celsius                                            *\n"));
  FALCON2_DINO_EFUN_PRINTF(("*  RESCAL           : Analog Resistor Calibration value                                                      *\n"));
  FALCON2_DINO_EFUN_PRINTF(("*  VCO_RATE         : uC VCO Rate in GHz (approximate)                                                       *\n"));
  FALCON2_DINO_EFUN_PRINTF(("*  ANA_VCO_RANGE    : Analog VCO Range                                                                       *\n"));
  FALCON2_DINO_EFUN_PRINTF(("*  PLL_DIV          : (Register Value) Actual PLL Divider Value                                              *\n"));
  FALCON2_DINO_EFUN_PRINTF(("*  PLL_Lock         : PLL Lock                                                                               *\n"));

  FALCON2_DINO_EFUN_PRINTF(("**************************************************************************************************************\n"));
  return (ERR_CODE_NONE);
}

/**********************************************/
/*  Display Lane/Core Config and Debug Status */
/**********************************************/

err_code_t falcon2_dino_display_core_config( const phymod_access_t *pa ) {
    FALCON2_DINO_EFUN_PRINTF(("\n\n***********************************\n"  ));
    FALCON2_DINO_EFUN_PRINTF((    "**** SERDES CORE CONFIGURATION ****\n"  ));
    FALCON2_DINO_EFUN_PRINTF((    "***********************************\n\n"));
    {
        struct   falcon2_dino_uc_core_config_st core_cfg;
        FALCON2_DINO_ENULL_MEMSET(&core_cfg, 0, sizeof(core_cfg));
            FALCON2_DINO_EFUN(falcon2_dino_get_uc_core_config( pa, &core_cfg));
        {
            uint16_t  vco_mhz = (uint16_t)core_cfg.vco_rate_in_Mhz;
            USR_UNUSED_VAR(vco_mhz);
    FALCON2_DINO_EFUN_PRINTF((    "uC Config VCO Rate   = %d (~%d.%03dGHz)\n"                  , core_cfg.field.vco_rate
                                                                                  , (vco_mhz / 1000)
                                                                                  , (vco_mhz % 1000)                ));
    FALCON2_DINO_EFUN_PRINTF((    "Core Config from PCS = %d\n\n"                              , core_cfg.field.core_cfg_from_pcs));
        }
    }
    FALCON2_DINO_ESTM_PRINTF((    "Tx Lane Addr 0       = %d\n"                            , falcon2_dino_rdc_tx_lane_addr_0()));
    FALCON2_DINO_ESTM_PRINTF((    "Rx Lane Addr 0       = %d\n"                            , falcon2_dino_rdc_rx_lane_addr_0()));
    FALCON2_DINO_ESTM_PRINTF((    "Tx Lane Addr 1       = %d\n"                            , falcon2_dino_rdc_tx_lane_addr_1()));
    FALCON2_DINO_ESTM_PRINTF((    "Rx Lane Addr 1       = %d\n"                            , falcon2_dino_rdc_rx_lane_addr_1()));
    FALCON2_DINO_ESTM_PRINTF((    "Tx Lane Addr 2       = %d\n"                            , falcon2_dino_rdc_tx_lane_addr_2()));
    FALCON2_DINO_ESTM_PRINTF((    "Rx Lane Addr 2       = %d\n"                            , falcon2_dino_rdc_rx_lane_addr_2()));
    FALCON2_DINO_ESTM_PRINTF((    "Tx Lane Addr 3       = %d\n"                            , falcon2_dino_rdc_tx_lane_addr_3()));
    FALCON2_DINO_ESTM_PRINTF((    "Rx Lane Addr 3       = %d\n"                            , falcon2_dino_rdc_rx_lane_addr_3()));
    return(ERR_CODE_NONE);
}


err_code_t falcon2_dino_display_lane_config( const phymod_access_t *pa ) {
    struct falcon2_dino_uc_lane_config_st lane_cfg;

    FALCON2_DINO_ENULL_MEMSET(&lane_cfg, 0, sizeof(lane_cfg));

    FALCON2_DINO_EFUN_PRINTF(("\n\n*************************************\n"                                          ));
    FALCON2_DINO_ESTM_PRINTF((   "**** SERDES LANE %d CONFIGURATION ****\n"  , falcon2_dino_get_lane(pa)                     ));
    FALCON2_DINO_EFUN_PRINTF((    "*************************************\n\n"                                        ));
    FALCON2_DINO_EFUN(                                                         falcon2_dino_get_uc_lane_cfg( pa, &lane_cfg)      );
    FALCON2_DINO_EFUN_PRINTF((    "Auto-Neg Enabled            = %d\n"       , lane_cfg.field.an_enabled             ));
    FALCON2_DINO_EFUN_PRINTF((    "DFE on                      = %d\n"       , lane_cfg.field.dfe_on                 ));
    FALCON2_DINO_EFUN_PRINTF((    "DFE low power mode          = %d\n"       , lane_cfg.field.dfe_lp_mode            ));
    FALCON2_DINO_EFUN_PRINTF((    "Brdfe_on                    = %d\n"       , lane_cfg.field.force_brdfe_on         ));
    FALCON2_DINO_EFUN_PRINTF((    "Media Type                  = %d\n"       , lane_cfg.field.media_type             ));
    FALCON2_DINO_EFUN_PRINTF((    "Unreliable LOS              = %d\n"       , lane_cfg.field.unreliable_los         ));
    FALCON2_DINO_EFUN_PRINTF((    "Scrambling Disable          = %d\n"       , lane_cfg.field.scrambling_dis         ));
    FALCON2_DINO_EFUN_PRINTF((    "Lane Config from PCS        = %d\n\n"     , lane_cfg.field.lane_cfg_from_pcs      ));
    FALCON2_DINO_EFUN_PRINTF((    "CL72 Auto Polarity   Enable = %d\n"       , lane_cfg.field.cl72_auto_polarity_en  ));
    FALCON2_DINO_EFUN_PRINTF((    "CL72 Restart timeout Enable = %d\n"       , lane_cfg.field.cl72_restart_timeout_en));
    FALCON2_DINO_EFUN_PRINTF((    "Enable Short Channel OSx1   = %d\n"       , lane_cfg.field.en_short_chn_osx1      ));
    FALCON2_DINO_ESTM_PRINTF((    "CL93/72 Training Enable     = %d\n"       , falcon2_dino_rd_cl93n72_ieee_training_enable()     ));

    FALCON2_DINO_ESTM_PRINTF((    "EEE Mode Enable             = %d\n"       , falcon2_dino_rd_eee_mode_en()                      ));
    FALCON2_DINO_ESTM_PRINTF((    "OSR Mode Force              = %d\n"       , falcon2_dino_rd_osr_mode_frc()                     ));
    FALCON2_DINO_ESTM_PRINTF((    "OSR Mode Force Val          = %d\n"       , falcon2_dino_rd_osr_mode_frc_val()                 ));
    FALCON2_DINO_ESTM_PRINTF((    "TX Polarity Invert          = %d\n"       , falcon2_dino_rd_tx_pmd_dp_invert()                 ));
    FALCON2_DINO_ESTM_PRINTF((    "RX Polarity Invert          = %d\n\n"     , falcon2_dino_rd_rx_pmd_dp_invert()                 ));
    FALCON2_DINO_ESTM_PRINTF((    "TXFIR Post2                 = %d\n"       , falcon2_dino_rd_txfir_post2()                      ));
    FALCON2_DINO_ESTM_PRINTF((    "TXFIR Post3                 = %d\n"       , falcon2_dino_rd_txfir_post3()                      ));
    FALCON2_DINO_ESTM_PRINTF((    "TXFIR Main                  = %d\n"       , falcon2_dino_rd_cl93n72_txfir_main()               ));
    FALCON2_DINO_ESTM_PRINTF((    "TXFIR Pre                   = %d\n"       , falcon2_dino_rd_cl93n72_txfir_pre()                ));
    FALCON2_DINO_ESTM_PRINTF((    "TXFIR Post                  = %d\n"       , falcon2_dino_rd_cl93n72_txfir_post()               ));
    return(ERR_CODE_NONE);
}


err_code_t falcon2_dino_display_core_state( const phymod_access_t *pa ) {
    FALCON2_DINO_EFUN(falcon2_dino_display_core_state_hdr( pa ));
    FALCON2_DINO_EFUN(falcon2_dino_display_core_state_line( pa ));
    FALCON2_DINO_EFUN(falcon2_dino_display_core_state_legend( pa ));
    return ERR_CODE_NONE;
}



err_code_t falcon2_dino_display_lane_debug_status( const phymod_access_t *pa ) {
    /* startup */
    struct falcon2_dino_usr_ctrl_disable_functions_st     ds;
    struct falcon2_dino_usr_ctrl_disable_dfe_functions_st dsd;
    /* steady state */
    struct falcon2_dino_usr_ctrl_disable_functions_st     dss;
    struct falcon2_dino_usr_ctrl_disable_dfe_functions_st dssd;

    FALCON2_DINO_ENULL_MEMSET(&ds  , 0, sizeof(ds  ));
    FALCON2_DINO_ENULL_MEMSET(&dsd , 0, sizeof(dsd ));
    FALCON2_DINO_ENULL_MEMSET(&dss , 0, sizeof(dss ));
    FALCON2_DINO_ENULL_MEMSET(&dssd, 0, sizeof(dssd));

    FALCON2_DINO_EFUN_PRINTF(("\n\n************************************\n"                                                     ));
    FALCON2_DINO_ESTM_PRINTF((   "**** SERDES LANE %d DEBUG STATUS ****\n"               , falcon2_dino_get_lane(pa)                   ));
    FALCON2_DINO_EFUN_PRINTF((    "************************************\n\n"                                                   ));
    FALCON2_DINO_ESTM_PRINTF((    "Restart Count                                = %d\n"  , falcon2_dino_rdv_usr_sts_restart_counter()       ));
    FALCON2_DINO_ESTM_PRINTF((    "Reset Count                                  = %d\n"  , falcon2_dino_rdv_usr_sts_reset_counter()         ));
    FALCON2_DINO_ESTM_PRINTF((    "PMD Lock Count                               = %d\n\n", falcon2_dino_rdv_usr_sts_pmd_lock_counter()      ));
    FALCON2_DINO_EFUN(falcon2_dino_get_usr_ctrl_disable_startup( pa, &ds));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Startup PF Adaptation                = %d\n"  , ds.field.pf_adaptation              ));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Startup PF2 Adaptation               = %d\n"  , ds.field.pf2_adaptation             ));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Startup DC Adaptation                = %d\n"  , ds.field.dc_adaptation              ));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Startup VGA Adaptation               = %d\n"  , ds.field.vga_adaptation             ));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Startup Slicer vOffset Tuning        = %d\n"  , ds.field.slicer_voffset_tuning      ));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Startup Slicer hOffset Tuning        = %d\n"  , ds.field.slicer_hoffset_tuning      ));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Startup Phase offset Adaptation      = %d\n"  , ds.field.phase_offset_adaptation    ));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Startup Eye Adaptaion                = %d\n"  , ds.field.eye_adaptation             ));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Startup All Adaptaion                = %d\n\n", ds.field.all_adaptation             ));
    FALCON2_DINO_EFUN(                                                            falcon2_dino_get_usr_ctrl_disable_startup_dfe( pa, &dsd) );
    FALCON2_DINO_EFUN_PRINTF((    "Disable Startup DFE Tap1 Adaptation          = %d\n"  , dsd.field.dfe_tap1_adaptation       ));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Startup DFE FX Taps Adaptation       = %d\n"  , dsd.field.dfe_fx_taps_adaptation    ));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Startup DFE FL Taps Adaptation       = %d\n"  , dsd.field.dfe_fl_taps_adaptation    ));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Startup DFE Tap DCD                  = %d\n"  , dsd.field.dfe_dcd_adaptation        ));
    FALCON2_DINO_EFUN(                                                           falcon2_dino_get_usr_ctrl_disable_steady_state( pa, &dss) );
    FALCON2_DINO_EFUN_PRINTF((    "Disable Steady State PF Adaptation           = %d\n"  , dss.field.pf_adaptation             ));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Steady State PF2 Adaptation          = %d\n"  , dss.field.pf2_adaptation            ));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Steady State DC Adaptation           = %d\n"  , dss.field.dc_adaptation             ));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Steady State VGA Adaptation          = %d\n"  , dss.field.vga_adaptation            ));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Steady State Slicer vOffset Tuning   = %d\n"  , dss.field.slicer_voffset_tuning     ));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Steady State Slicer hOffset Tuning   = %d\n"  , dss.field.slicer_hoffset_tuning     ));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Steady State Phase offset Adaptation = %d\n"  , dss.field.phase_offset_adaptation   ));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Steady State Eye Adaptaion           = %d\n"  , dss.field.eye_adaptation            ));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Steady State All Adaptaion           = %d\n\n", dss.field.all_adaptation            ));
    FALCON2_DINO_EFUN(                                                      falcon2_dino_get_usr_ctrl_disable_steady_state_dfe( pa, &dssd) );
    FALCON2_DINO_EFUN_PRINTF((    "Disable Steady State DFE Tap1 Adaptation     = %d\n"  , dssd.field.dfe_tap1_adaptation      ));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Steady State DFE FX Taps Adaptation  = %d\n"  , dssd.field.dfe_fx_taps_adaptation   ));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Steady State DFE FL Taps Adaptation  = %d\n"  , dssd.field.dfe_fl_taps_adaptation   ));
    FALCON2_DINO_EFUN_PRINTF((    "Disable Steady State DFE Tap DCD             = %d\n"  , dssd.field.dfe_dcd_adaptation       ));
    FALCON2_DINO_ESTM_PRINTF((    "Retune after Reset                           = %d\n"  , falcon2_dino_rdv_usr_ctrl_retune_after_restart() ));
    FALCON2_DINO_ESTM_PRINTF((    "Clk90 offset Adjust                          = %d\n"  , falcon2_dino_rdv_usr_ctrl_clk90_offset_adjust()  ));
    FALCON2_DINO_ESTM_PRINTF((    "Clk90 offset Override                        = %d\n"  , falcon2_dino_rdv_usr_ctrl_clk90_offset_override()));
    FALCON2_DINO_ESTM_PRINTF((    "Lane Event Log Level                         = %d\n"  , falcon2_dino_rdv_usr_ctrl_lane_event_log_level() ));

    return(ERR_CODE_NONE);
}



/*************************/
/*  Stop/Resume uC Lane  */
/*************************/

err_code_t falcon2_dino_stop_uc_lane( const phymod_access_t *pa, uint8_t enable) {

  if (enable) {
    return(falcon2_dino_pmd_uc_control( pa, CMD_UC_CTRL_STOP_GRACEFULLY,100));
  }
  else {
    return(falcon2_dino_pmd_uc_control( pa, CMD_UC_CTRL_RESUME,50));
  }
}


err_code_t falcon2_dino_stop_uc_lane_status( const phymod_access_t *pa, uint8_t *uc_lane_stopped) {

  if(!uc_lane_stopped) {
      return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  FALCON2_DINO_ESTM(*uc_lane_stopped = falcon2_dino_rdv_usr_sts_micro_stopped());

  return (ERR_CODE_NONE);
}


/*******************************/
/*  Stop/Resume RX Adaptation  */
/*******************************/

err_code_t falcon2_dino_stop_rx_adaptation( const phymod_access_t *pa, uint8_t enable) {

  if (enable) {
    return(falcon2_dino_pmd_uc_control( pa, CMD_UC_CTRL_STOP_GRACEFULLY,100));
  }
  else {
    return(falcon2_dino_pmd_uc_control( pa, CMD_UC_CTRL_RESUME,50));
  }
}

err_code_t falcon2_dino_request_stop_rx_adaptation( const phymod_access_t *pa ) {

  return(falcon2_dino_pmd_uc_control_return_immediate( pa, CMD_UC_CTRL_STOP_GRACEFULLY));
}


/**********************/
/*  uCode CRC Verify  */
/**********************/

err_code_t falcon2_dino_ucode_crc_verify( const phymod_access_t *pa, uint16_t ucode_len,uint16_t expected_crc_value) {
    uint16_t calc_crc;

    FALCON2_DINO_EFUN(falcon2_dino_pmd_uc_cmd_with_data( pa, CMD_CALC_CRC,0,ucode_len,200));

    FALCON2_DINO_ESTM(calc_crc = falcon2_dino_rd_uc_dsc_data());
    if(calc_crc != expected_crc_value) {
        FALCON2_DINO_EFUN_PRINTF(("UC CRC did not match expected=%04x : calculated=%04x\n",expected_crc_value, calc_crc));
        return(falcon2_dino_error(ERR_CODE_UC_CRC_NOT_MATCH));
    }

    return(ERR_CODE_NONE);
}

err_code_t falcon2_dino_start_ucode_crc_calc( const phymod_access_t *pa, uint16_t ucode_len) {

    FALCON2_DINO_EFUN(falcon2_dino_pmd_uc_cmd_with_data_return_immediate( pa, CMD_CALC_CRC, 0, ucode_len)); /* Invoke Calculate CRC command and return control immediately */
    return(ERR_CODE_NONE);
}

err_code_t falcon2_dino_check_ucode_crc( const phymod_access_t *pa, uint16_t expected_crc_value, uint32_t timeout_ms) {
    uint16_t calc_crc;
    err_code_t err_code;

    err_code = falcon2_dino_poll_uc_dsc_ready_for_cmd_equals_1( pa, timeout_ms);     /* Poll for uc_dsc_ready_for_cmd = 1 to indicate falcon2_dino ready for command */
    if (err_code) {
      FALCON2_DINO_EFUN_PRINTF(("ERROR : DSC ready for command timed out. Previous uC command not finished yet\n"));
      return (err_code);
    }
    FALCON2_DINO_ESTM(calc_crc = falcon2_dino_rd_uc_dsc_data());
    if(calc_crc != expected_crc_value) {
        FALCON2_DINO_EFUN_PRINTF(("UC CRC did not match expected=%04x : calculated=%04x\n",expected_crc_value, calc_crc));
        return(falcon2_dino_error(ERR_CODE_UC_CRC_NOT_MATCH));
    }

    return(ERR_CODE_NONE);
}


/******************************************************/
/*  Commands through Serdes FW DSC Command Interface  */
/******************************************************/

err_code_t falcon2_dino_pmd_uc_cmd_return_immediate( const phymod_access_t *pa, enum srds_pmd_uc_cmd_enum cmd, uint8_t supp_info) {
  err_code_t err_code;
  uint16_t   cmddata;

  err_code = falcon2_dino_poll_uc_dsc_ready_for_cmd_equals_1( pa, 1); /* Poll for uc_dsc_ready_for_cmd = 1 to indicate falcon2_dino ready for command */
  if (err_code) {
    FALCON2_DINO_EFUN_PRINTF(("ERROR : DSC ready for command timed out (before cmd) cmd = %d, supp_info = x%02x err=%d !\n", cmd, supp_info, err_code));
    return (err_code);
  }
/*FALCON2_DINO_EFUN(wr_uc_dsc_supp_info(supp_info)); */                     /* Supplement info field */
/*FALCON2_DINO_EFUN(wr_uc_dsc_error_found(0x0)    ); */                     /* Clear error found field */
/*FALCON2_DINO_EFUN(wr_uc_dsc_gp_uc_req(cmd)      ); */                     /* Set command code */
/*FALCON2_DINO_EFUN(wr_uc_dsc_ready_for_cmd(0x0)  ); */                     /* Issue command, by clearing "ready for command" field */
  cmddata = (((uint16_t)supp_info)<<8) | (uint16_t)cmd;        /* Combine writes to single write instead of 4 RMW */

  FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,FALCON2_DINO_DSC_A_DSC_UC_CTRL, cmddata));         /* This address is same for Eagle, and all Merlin */

  return(ERR_CODE_NONE);
}

err_code_t falcon2_dino_pmd_uc_cmd( const phymod_access_t *pa, enum srds_pmd_uc_cmd_enum cmd, uint8_t supp_info, uint32_t timeout_ms) {

  uint8_t uc_dsc_error_found;

  FALCON2_DINO_EFUN(falcon2_dino_pmd_uc_cmd_return_immediate( pa, cmd, supp_info));    /* Invoke falcon2_dino_uc_cmd and return control immediately */

  FALCON2_DINO_EFUN(falcon2_dino_poll_uc_dsc_ready_for_cmd_equals_1( pa, timeout_ms)); /* Poll for uc_dsc_ready_for_cmd = 1 to indicate falcon2_dino ready for command */
  FALCON2_DINO_ESTM(uc_dsc_error_found = falcon2_dino_rd_uc_dsc_error_found());
  if(uc_dsc_error_found) {
      FALCON2_DINO_ESTM_PRINTF(("ERROR : DSC ready for command return error ( after cmd) cmd = %d, supp_info = x%02x !\n", cmd, falcon2_dino_rd_uc_dsc_supp_info()));
      return(falcon2_dino_error(ERR_CODE_UC_CMD_RETURN_ERROR));
  }
  return(ERR_CODE_NONE);
}


err_code_t falcon2_dino_pmd_uc_cmd_with_data_return_immediate( const phymod_access_t *pa, enum srds_pmd_uc_cmd_enum cmd, uint8_t supp_info, uint16_t data) {
  uint16_t   cmddata;
  err_code_t err_code;

  err_code = falcon2_dino_poll_uc_dsc_ready_for_cmd_equals_1( pa, 1); /* Poll for uc_dsc_ready_for_cmd = 1 to indicate falcon2_dino ready for command */
  if (err_code) {
     FALCON2_DINO_EFUN_PRINTF(("ERROR : DSC ready for command timed out (before cmd) cmd = %d, supp_info = x%02x, data = x%04x err=%d !\n", cmd, supp_info, data,err_code));
    return (err_code);
  }

  FALCON2_DINO_EFUN(falcon2_dino_wr_uc_dsc_data(data));                                  /* Write value written to uc_dsc_data field */
/*FALCON2_DINO_EFUN(wr_uc_dsc_supp_info(supp_info)); */                     /* Supplement info field */
/*FALCON2_DINO_EFUN(wr_uc_dsc_error_found(0x0));     */                     /* Clear error found field */
/*FALCON2_DINO_EFUN(wr_uc_dsc_gp_uc_req(cmd));       */                     /* Set command code */
/*FALCON2_DINO_EFUN(wr_uc_dsc_ready_for_cmd(0x0));   */                     /* Issue command, by clearing "ready for command" field */
  cmddata = (((uint16_t)supp_info)<<8) | (uint16_t)cmd;        /* Combine writes to single write instead of 4 RMW */

  FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,FALCON2_DINO_DSC_A_DSC_UC_CTRL, cmddata));         /* This address is same for Eagle, and all Merlin */

  return(ERR_CODE_NONE);
}


err_code_t falcon2_dino_pmd_uc_cmd_with_data( const phymod_access_t *pa, enum srds_pmd_uc_cmd_enum cmd, uint8_t supp_info, uint16_t data, uint32_t timeout_ms) {

    uint8_t uc_dsc_error_found;

    FALCON2_DINO_EFUN(falcon2_dino_pmd_uc_cmd_with_data_return_immediate( pa, cmd, supp_info, data)); /* Invoke falcon2_dino_uc_cmd and return control immediately */

    FALCON2_DINO_EFUN(falcon2_dino_poll_uc_dsc_ready_for_cmd_equals_1( pa, timeout_ms)); /* Poll for uc_dsc_ready_for_cmd = 1 to indicate falcon2_dino ready for command */
    FALCON2_DINO_ESTM(uc_dsc_error_found = falcon2_dino_rd_uc_dsc_error_found());
    if(uc_dsc_error_found) {
        FALCON2_DINO_ESTM_PRINTF(("ERROR : DSC ready for command return error ( after cmd) cmd = %d, supp_info = x%02x !\n", cmd, falcon2_dino_rd_uc_dsc_supp_info()));
        return(falcon2_dino_error(ERR_CODE_UC_CMD_RETURN_ERROR));
    }
    return(ERR_CODE_NONE);
}

err_code_t falcon2_dino_pmd_uc_control_return_immediate( const phymod_access_t *pa, enum srds_pmd_uc_ctrl_cmd_enum control) {
  return(falcon2_dino_pmd_uc_cmd_return_immediate( pa, CMD_UC_CTRL, (uint8_t) control));
}

err_code_t falcon2_dino_pmd_uc_control( const phymod_access_t *pa, enum srds_pmd_uc_ctrl_cmd_enum control, uint32_t timeout_ms) {
  return(falcon2_dino_pmd_uc_cmd( pa, CMD_UC_CTRL, (uint8_t) control, timeout_ms));
}

err_code_t falcon2_dino_pmd_uc_diag_cmd( const phymod_access_t *pa, enum srds_pmd_uc_diag_cmd_enum control, uint32_t timeout_ms) {
  return(falcon2_dino_pmd_uc_cmd( pa, CMD_DIAG_EN, (uint8_t) control, timeout_ms));
}


/************************************************************/
/*      Serdes IP RAM access - Lane RAM Variables           */
/*----------------------------------------------------------*/
/*   - through Micro Register Interface for PMD IPs         */
/*   - through Serdes FW DSC Command Interface for Gallardo */
/************************************************************/

/* Micro RAM Lane Byte Read */
uint8_t falcon2_dino_rdbl_uc_var( const phymod_access_t *pa, err_code_t *err_code_p, uint16_t addr) {
    uint8_t rddata;

    if(!err_code_p) {
        return(0);
    }

    FALCON2_DINO_EPSTM(rddata = falcon2_dino_rdb_uc_ram( pa, err_code_p, (FALCON2_DINO_LANE_VAR_RAM_BASE+addr+(falcon2_dino_get_lane(pa)*FALCON2_DINO_LANE_VAR_RAM_SIZE)))); /* Use Micro register interface for reading RAM */

    return (rddata);
}

/* Micro RAM Lane Byte Signed Read */
int8_t falcon2_dino_rdbls_uc_var( const phymod_access_t *pa, err_code_t *err_code_p, uint16_t addr) {
  return ((int8_t) falcon2_dino_rdbl_uc_var( pa, err_code_p, addr));
}

/* Micro RAM Lane Word Read */
uint16_t falcon2_dino_rdwl_uc_var( const phymod_access_t *pa, err_code_t *err_code_p, uint16_t addr) {
  uint16_t rddata;

  if(!err_code_p) {
      return(0);
  }

  if (addr%2 != 0) {                                                                /* Validate even address */
      *err_code_p = ERR_CODE_INVALID_RAM_ADDR;
      return (0);
  }

  FALCON2_DINO_EPSTM(rddata = falcon2_dino_rdw_uc_ram( pa, err_code_p, (FALCON2_DINO_LANE_VAR_RAM_BASE+addr+(falcon2_dino_get_lane(pa)*FALCON2_DINO_LANE_VAR_RAM_SIZE)))); /* Use Micro register interface for reading RAM */

  return (rddata);
}


/* Micro RAM Lane Word Signed Read */
int16_t falcon2_dino_rdwls_uc_var( const phymod_access_t *pa, err_code_t *err_code_p, uint16_t addr) {
  return ((int16_t) falcon2_dino_rdwl_uc_var( pa, err_code_p, addr));
}

/* Micro RAM Lane Byte Write */
err_code_t falcon2_dino_wrbl_uc_var( const phymod_access_t *pa, uint16_t addr, uint8_t wr_val) {


    if(addr < FALCON2_DINO_LANE_VAR_RAM_SIZE) {
        return (falcon2_dino_wrb_uc_ram( pa, (FALCON2_DINO_LANE_VAR_RAM_BASE+addr+(falcon2_dino_get_lane(pa)*FALCON2_DINO_LANE_VAR_RAM_SIZE)), wr_val));    /* Use Micro register interface for writing RAM */
    } else {
        return(falcon2_dino_error(ERR_CODE_INVALID_RAM_ADDR));
    }

}

/* Micro RAM Lane Byte Signed Write */
err_code_t falcon2_dino_wrbls_uc_var( const phymod_access_t *pa, uint16_t addr, int8_t wr_val) {
  return (falcon2_dino_wrbl_uc_var( pa, addr, wr_val));
}

/* Micro RAM Lane Word Write */
err_code_t falcon2_dino_wrwl_uc_var( const phymod_access_t *pa, uint16_t addr, uint16_t wr_val) {


    if (addr%2 != 0) {                                                                /* Validate even address */
        return (falcon2_dino_error(ERR_CODE_INVALID_RAM_ADDR));
    }
    if(addr < FALCON2_DINO_LANE_VAR_RAM_SIZE) {
        return (falcon2_dino_wrw_uc_ram( pa, (FALCON2_DINO_LANE_VAR_RAM_BASE+addr+(falcon2_dino_get_lane(pa)*FALCON2_DINO_LANE_VAR_RAM_SIZE)), wr_val));    /* Use Micro register interface for writing RAM */
    } else {
        return(falcon2_dino_error(ERR_CODE_INVALID_RAM_ADDR));
    }
}

/* Micro RAM Lane Word Signed Write */
err_code_t falcon2_dino_wrwls_uc_var( const phymod_access_t *pa, uint16_t addr, int16_t wr_val) {
  return (falcon2_dino_wrwl_uc_var( pa, addr,wr_val));
}


/************************************************************/
/*      Serdes IP RAM access - Core RAM Variables           */
/*----------------------------------------------------------*/
/*   - through Micro Register Interface for PMD IPs         */
/*   - through Serdes FW DSC Command Interface for Gallardo */
/************************************************************/

/* Micro RAM Core Byte Read */
uint8_t falcon2_dino_rdbc_uc_var( const phymod_access_t *pa, err_code_t *err_code_p, uint8_t addr) {

  uint8_t rddata;

  if(!err_code_p) {
      return(0);
  }

  FALCON2_DINO_EPSTM(rddata = falcon2_dino_rdb_uc_ram( pa, err_code_p, (FALCON2_DINO_CORE_VAR_RAM_BASE+addr)));                      /* Use Micro register interface for reading RAM */
  return (rddata);
}

/* Micro RAM Core Byte Signed Read */
int8_t falcon2_dino_rdbcs_uc_var( const phymod_access_t *pa, err_code_t *err_code_p, uint8_t addr) {
  return ((int8_t) falcon2_dino_rdbc_uc_var( pa, err_code_p, addr));
}

/* Micro RAM Core Word Read */
uint16_t falcon2_dino_rdwc_uc_var( const phymod_access_t *pa, err_code_t *err_code_p, uint8_t addr) {

  uint16_t rddata;

  if(!err_code_p) {
      return(0);
  }
  if (addr%2 != 0) {                                                                /* Validate even address */
      *err_code_p = ERR_CODE_INVALID_RAM_ADDR;
      return (0);
  }

  FALCON2_DINO_EPSTM(rddata = falcon2_dino_rdw_uc_ram( pa, err_code_p, (FALCON2_DINO_CORE_VAR_RAM_BASE+addr)));                  /* Use Micro register interface for reading RAM */
  return (rddata);
}

/* Micro RAM Core Word Signed Read */
int16_t falcon2_dino_rdwcs_uc_var( const phymod_access_t *pa, err_code_t *err_code_p, uint8_t addr) {
  return ((int16_t) falcon2_dino_rdwc_uc_var( pa, err_code_p, addr));
}

/* Micro RAM Core Byte Write  */
err_code_t falcon2_dino_wrbc_uc_var( const phymod_access_t *pa, uint8_t addr, uint8_t wr_val) {


    if(addr < FALCON2_DINO_CORE_VAR_RAM_SIZE) {
    return (falcon2_dino_wrb_uc_ram( pa, (FALCON2_DINO_CORE_VAR_RAM_BASE+addr), wr_val));                                /* Use Micro register interface for writing RAM */
    } else {
        return(falcon2_dino_error(ERR_CODE_INVALID_RAM_ADDR));
    }
}


/* Micro RAM Core Byte Signed Write */
err_code_t falcon2_dino_wrbcs_uc_var( const phymod_access_t *pa, uint8_t addr, int8_t wr_val) {
  return (falcon2_dino_wrbc_uc_var( pa, addr, wr_val));
}

/* Micro RAM Core Word Write  */
err_code_t falcon2_dino_wrwc_uc_var( const phymod_access_t *pa, uint8_t addr, uint16_t wr_val) {
    if (addr%2 != 0) {                                                                /* Validate even address */
        return (falcon2_dino_error(ERR_CODE_INVALID_RAM_ADDR));
    }
    if(addr < FALCON2_DINO_CORE_VAR_RAM_SIZE) {
        return (falcon2_dino_wrw_uc_ram( pa, (FALCON2_DINO_CORE_VAR_RAM_BASE+addr), wr_val));                                 /* Use Micro register interface for writing RAM */
    } else {
        return(falcon2_dino_error(ERR_CODE_INVALID_RAM_ADDR));
    }
}

/* Micro RAM Core Word Signed Write */
err_code_t falcon2_dino_wrwcs_uc_var( const phymod_access_t *pa, uint8_t addr, int16_t wr_val) {
  return(falcon2_dino_wrwc_uc_var( pa, addr,wr_val));
}



/*******************************************************************/
/*  APIs to Write Core/Lane Config and User variables into uC RAM  */
/*******************************************************************/

err_code_t falcon2_dino_set_uc_core_config( const phymod_access_t *pa, struct falcon2_dino_uc_core_config_st struct_val) {
    {   
#ifndef ATE_LOG
        uint8_t reset_state;
        FALCON2_DINO_ESTM(reset_state = falcon2_dino_rdc_core_dp_reset_state());
        if(reset_state < 7) {
            FALCON2_DINO_EFUN_PRINTF(("ERROR: falcon2_dino_set_uc_core_config( pa, ..) called without core_dp_s_rstb=0 Lane=%d reset_state=%d\n",falcon2_dino_get_lane(pa),reset_state));
            return (falcon2_dino_error(ERR_CODE_CORE_DP_NOT_RESET));
        }
#endif
    }
    if(struct_val.vco_rate_in_Mhz > 0) {
        struct_val.field.vco_rate = FALCON2_DINO_MHZ_TO_VCO_RATE(struct_val.vco_rate_in_Mhz);
    }
    FALCON2_DINO_EFUN(falcon2_dino_update_uc_core_config_word( pa, &struct_val));
        FALCON2_DINO_EFUN(falcon2_dino_wrcv_config_word(struct_val.word));
    return (ERR_CODE_NONE);
}

err_code_t falcon2_dino_set_usr_ctrl_core_event_log_level( const phymod_access_t *pa, uint8_t core_event_log_level) {
  return(falcon2_dino_wrcv_usr_ctrl_core_event_log_level(core_event_log_level));
}

err_code_t falcon2_dino_set_uc_lane_cfg( const phymod_access_t *pa, struct falcon2_dino_uc_lane_config_st struct_val) {
#ifndef ATE_LOG
    uint8_t reset_state;

    FALCON2_DINO_ESTM(reset_state = falcon2_dino_rd_lane_dp_reset_state());
    if(reset_state < 7) {
        FALCON2_DINO_EFUN_PRINTF(("ERROR: falcon2_dino_set_uc_lane_cfg( pa, ..) called without ln_dp_s_rstb=0 Lane=%d reset_state=%d\n",falcon2_dino_get_lane(pa),reset_state));
        return (falcon2_dino_error(ERR_CODE_LANE_DP_NOT_RESET));
    }
#endif
  FALCON2_DINO_EFUN(falcon2_dino_update_uc_lane_config_word( pa, &struct_val));
  return(falcon2_dino_wrv_config_word(struct_val.word));
}

err_code_t falcon2_dino_set_usr_ctrl_lane_event_log_level( const phymod_access_t *pa, uint8_t lane_event_log_level) {
  return(falcon2_dino_wrv_usr_ctrl_lane_event_log_level(lane_event_log_level));
}

err_code_t falcon2_dino_set_usr_ctrl_disable_startup( const phymod_access_t *pa, struct falcon2_dino_usr_ctrl_disable_functions_st set_val) {
  FALCON2_DINO_EFUN(falcon2_dino_update_usr_ctrl_disable_functions_byte( pa, &set_val));
  return(falcon2_dino_wrv_usr_ctrl_disable_startup_functions_word(set_val.word));
}

err_code_t falcon2_dino_set_usr_ctrl_disable_startup_dfe( const phymod_access_t *pa, struct falcon2_dino_usr_ctrl_disable_dfe_functions_st set_val) {
  FALCON2_DINO_EFUN(falcon2_dino_update_usr_ctrl_disable_dfe_functions_byte( pa, &set_val));
  return(falcon2_dino_wrv_usr_ctrl_disable_startup_dfe_functions_byte(set_val.byte));
}

err_code_t falcon2_dino_set_usr_ctrl_disable_steady_state( const phymod_access_t *pa, struct falcon2_dino_usr_ctrl_disable_functions_st set_val) {
  FALCON2_DINO_EFUN(falcon2_dino_update_usr_ctrl_disable_functions_byte( pa, &set_val));
  return(falcon2_dino_wrv_usr_ctrl_disable_steady_state_functions_word(set_val.word));
}

err_code_t falcon2_dino_set_usr_ctrl_disable_steady_state_dfe( const phymod_access_t *pa, struct falcon2_dino_usr_ctrl_disable_dfe_functions_st set_val) {
  FALCON2_DINO_EFUN(falcon2_dino_update_usr_ctrl_disable_dfe_functions_byte( pa, &set_val));
  return(falcon2_dino_wrv_usr_ctrl_disable_steady_state_dfe_functions_byte(set_val.byte));
}



/******************************************************************/
/*  APIs to Read Core/Lane Config and User variables from uC RAM  */
/******************************************************************/

err_code_t falcon2_dino_get_uc_core_config(const phymod_access_t *pa, struct falcon2_dino_uc_core_config_st *get_val)
{
    
    if(!get_val) {
        return falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
    }
        FALCON2_DINO_ESTM(get_val->word = falcon2_dino_rdcv_config_word());
    FALCON2_DINO_EFUN(falcon2_dino_update_uc_core_config_st( pa, get_val));
    return ERR_CODE_NONE;
}

err_code_t falcon2_dino_get_usr_ctrl_core_event_log_level( const phymod_access_t *pa, uint8_t *core_event_log_level) {

  if(!core_event_log_level) {
     return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  FALCON2_DINO_ESTM(*core_event_log_level = falcon2_dino_rdcv_usr_ctrl_core_event_log_level());

  return (ERR_CODE_NONE);
}

err_code_t falcon2_dino_get_uc_lane_cfg( const phymod_access_t *pa, struct falcon2_dino_uc_lane_config_st *get_val) {

  if(!get_val) {
     return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  FALCON2_DINO_ESTM(get_val->word = falcon2_dino_rdv_config_word());
  FALCON2_DINO_EFUN(falcon2_dino_update_uc_lane_config_st( pa, get_val));
  return (ERR_CODE_NONE);
}

err_code_t falcon2_dino_get_usr_ctrl_lane_event_log_level( const phymod_access_t *pa, uint8_t *lane_event_log_level) {

  if(!lane_event_log_level) {
     return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  FALCON2_DINO_ESTM(*lane_event_log_level = falcon2_dino_rdv_usr_ctrl_lane_event_log_level());
  return (ERR_CODE_NONE);
}

err_code_t falcon2_dino_get_usr_ctrl_disable_startup( const phymod_access_t *pa, struct falcon2_dino_usr_ctrl_disable_functions_st *get_val) {

  if(!get_val) {
     return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  FALCON2_DINO_ESTM(get_val->word = falcon2_dino_rdv_usr_ctrl_disable_startup_functions_word());
  FALCON2_DINO_EFUN(falcon2_dino_update_usr_ctrl_disable_functions_st( pa, get_val));
  return (ERR_CODE_NONE);
}

err_code_t falcon2_dino_get_usr_ctrl_disable_startup_dfe( const phymod_access_t *pa, struct falcon2_dino_usr_ctrl_disable_dfe_functions_st *get_val) {

  if(!get_val) {
     return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  FALCON2_DINO_ESTM(get_val->byte = falcon2_dino_rdv_usr_ctrl_disable_startup_dfe_functions_byte());
  FALCON2_DINO_EFUN(falcon2_dino_update_usr_ctrl_disable_dfe_functions_st( pa, get_val));
  return (ERR_CODE_NONE);
}

err_code_t falcon2_dino_get_usr_ctrl_disable_steady_state( const phymod_access_t *pa, struct falcon2_dino_usr_ctrl_disable_functions_st *get_val) {

  if(!get_val) {
     return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  FALCON2_DINO_ESTM(get_val->word = falcon2_dino_rdv_usr_ctrl_disable_steady_state_functions_word());
  FALCON2_DINO_EFUN(falcon2_dino_update_usr_ctrl_disable_functions_st( pa, get_val));
  return (ERR_CODE_NONE);
}

err_code_t falcon2_dino_get_usr_ctrl_disable_steady_state_dfe( const phymod_access_t *pa, struct falcon2_dino_usr_ctrl_disable_dfe_functions_st *get_val) {

  if(!get_val) {
     return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  FALCON2_DINO_ESTM(get_val->byte = falcon2_dino_rdv_usr_ctrl_disable_steady_state_dfe_functions_byte());
  FALCON2_DINO_EFUN(falcon2_dino_update_usr_ctrl_disable_dfe_functions_st( pa, get_val));
  return (ERR_CODE_NONE);
}

err_code_t falcon2_dino_get_clk90_offset_adjust( const phymod_access_t *pa, int8_t *adjust) {
  if (!adjust) {
    return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }
  FALCON2_DINO_ESTM(*adjust = (int8_t)falcon2_dino_rdv_usr_ctrl_clk90_offset_adjust());
  return (ERR_CODE_NONE);
}

err_code_t falcon2_dino_set_clk90_offset_adjust( const phymod_access_t *pa, int8_t adjust) {

  FALCON2_DINO_EFUN(falcon2_dino_check_uc_lane_stopped( pa ));                     /* make sure uC is stopped to avoid race conditions */

  if ((adjust > 16) || (adjust < -16)) {
    return(falcon2_dino_error(ERR_CODE_INVALID_CLK90_ADJUST));
  }
  FALCON2_DINO_ESTM(falcon2_dino_wrv_usr_ctrl_clk90_offset_adjust((uint8_t)(adjust)));
  return (ERR_CODE_NONE);
}

err_code_t falcon2_dino_get_clk90_offset_override( const phymod_access_t *pa, uint8_t *override) {
  if (!override) {
    return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }
  FALCON2_DINO_ESTM(*override = falcon2_dino_rdv_usr_ctrl_clk90_offset_override());
  return (ERR_CODE_NONE);
}

err_code_t falcon2_dino_set_clk90_offset_override( const phymod_access_t *pa, uint8_t enable, uint8_t override) {        

  FALCON2_DINO_EFUN(falcon2_dino_check_uc_lane_stopped( pa ));                     /* make sure uC is stopped to avoid race conditions */

  if ((override >= 52) || (override <= 24)) {
    return(falcon2_dino_error(ERR_CODE_INVALID_CLK90_OVERRIDE));
  }    
  FALCON2_DINO_ESTM(falcon2_dino_wrv_usr_ctrl_clk90_offset_override(((enable&1)<<7) | override));
  return (ERR_CODE_NONE);
}

err_code_t falcon2_dino_get_clk90_offset( const phymod_access_t *pa, int8_t *offset) {

  if (!offset) {
    return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  FALCON2_DINO_ESTM(*offset = falcon2_dino_rdv_usr_sts_phase_hoffset());
  
  return (ERR_CODE_NONE);
}


/******************************************/
/*  Serdes Register field Poll functions  */
/******************************************/

#ifndef CUSTOM_REG_POLLING

/* poll for microcontroller to populate the dsc_data register */
err_code_t falcon2_dino_poll_diag_done( const phymod_access_t *pa, uint16_t *status, uint32_t timeout_ms) {
 uint8_t loop;

 if(!status) {
     return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

 for(loop=0;loop < 100; loop++) {
     FALCON2_DINO_ESTM(*status=falcon2_dino_rdv_usr_diag_status());

     if((*status & 0x8000) > 0) {
        return(ERR_CODE_NONE);
     }
     if(loop>10) {
         FALCON2_DINO_EFUN(falcon2_dino_delay_us(10*timeout_ms));
     }
 }
 return(falcon2_dino_error(ERR_CODE_DIAG_TIMEOUT));
}

/* poll for microcontroller to populate the dsc_data register */
err_code_t falcon2_dino_poll_diag_eye_data(const phymod_access_t *pa,uint32_t *data,uint16_t *status, uint32_t timeout_ms) {
 uint8_t loop;
 uint16_t dscdata;
 if(!data || !status) {
     return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

 for(loop=0;loop < 100; loop++) {
     FALCON2_DINO_ESTM(*status=falcon2_dino_rdv_usr_diag_status());
     if(((*status & 0x00FF) > 2) || ((*status & 0x8000) > 0)) {
        FALCON2_DINO_EFUN(falcon2_dino_pmd_uc_cmd( pa,  CMD_READ_DIAG_DATA_WORD, 0, 200));
        FALCON2_DINO_ESTM(dscdata = falcon2_dino_rd_uc_dsc_data());
        data[0] = falcon2_dino_float8_to_int32((float8_t)(dscdata >>8));
        data[1] = falcon2_dino_float8_to_int32((float8_t)(dscdata & 0x00FF));
        return(ERR_CODE_NONE);
     }
     if(loop>10) {
         FALCON2_DINO_EFUN(falcon2_dino_delay_us(10*timeout_ms));
     }
 }
 return(falcon2_dino_error(ERR_CODE_DIAG_TIMEOUT));
}

/** Poll for field "uc_dsc_ready_for_cmd" = 1 [Return Val => Error_code (0 = Polling Pass)] */
err_code_t falcon2_dino_poll_uc_dsc_ready_for_cmd_equals_1( const phymod_access_t *pa, uint32_t timeout_ms) {
#ifndef ATE_LOG
  uint16_t loop;

  /* read quickly for 4 tries */
  for (loop = 0; loop < 100; loop++) {
    uint16_t rddata;
    FALCON2_DINO_EFUN(falcon2_dino_pmd_rdt_reg(pa,FALCON2_DINO_DSC_A_DSC_UC_CTRL, &rddata));
    if (rddata & 0x0080) {    /* bit 7 is uc_dsc_ready_for_cmd */
      if (rddata & 0x0040) {  /* bit 6 is uc_dsc_error_found   */
        FALCON2_DINO_ESTM_PRINTF(("ERROR : DSC command returned error (after cmd) cmd = 0x%x, supp_info = 0x%02x !\n", falcon2_dino_rd_uc_dsc_gp_uc_req(), falcon2_dino_rd_uc_dsc_supp_info()));
        return(falcon2_dino_error(ERR_CODE_UC_CMD_RETURN_ERROR));
      }
      return (ERR_CODE_NONE);
    }
    if(loop>10) {
        FALCON2_DINO_EFUN(falcon2_dino_delay_us(10*timeout_ms));
    }
  }

#else
  {   uint16_t rddata;
      FALCON2_DINO_EFUN_PRINTF(("// ATE_LOG falcon2_dino_poll_uc_dsc_ready_for_cmd_equals_1 begin\n"));
      if(timeout_ms < 100) {
          FALCON2_DINO_EFUN(falcon2_dino_delay_us(1000 * timeout_ms));
      } else {
          FALCON2_DINO_EFUN(falcon2_dino_delay_ms(timeout_ms));
      }
      FALCON2_DINO_EFUN(falcon2_dino_pmd_rdt_reg(pa,FALCON2_DINO_DSC_A_DSC_UC_CTRL, &rddata));
      if (rddata & 0x0080) {    /* bit 7 is uc_dsc_ready_for_cmd */
          if (rddata & 0x0040) {  /* bit 6 is uc_dsc_error_found   */
              FALCON2_DINO_ESTM_PRINTF(("ERROR : DSC command returned error (after cmd) cmd = 0x%x, supp_info = 0x%02x !\n", falcon2_dino_rd_uc_dsc_gp_uc_req(), falcon2_dino_rd_uc_dsc_supp_info()));
              return(falcon2_dino_error(ERR_CODE_UC_CMD_RETURN_ERROR));
          }
          FALCON2_DINO_EFUN_PRINTF(("// ATE_LOG falcon2_dino_poll_uc_dsc_ready_for_cmd_equals_1 end\n"));
          return (ERR_CODE_NONE);
      }
      FALCON2_DINO_EFUN_PRINTF(("// ATE_LOG falcon2_dino_poll_uc_dsc_ready_for_cmd_equals_1 timeout\n"));
  }
#endif
  FALCON2_DINO_EFUN_PRINTF(("ERROR : DSC ready for command is not working, applying workaround and getting debug info !\n"));
  FALCON2_DINO_DISP(falcon2_dino_rd_uc_dsc_ready_for_cmd());
  FALCON2_DINO_DISP(falcon2_dino_rd_uc_dsc_supp_info());
  FALCON2_DINO_DISP(falcon2_dino_rd_uc_dsc_gp_uc_req());
  FALCON2_DINO_DISP(falcon2_dino_rd_uc_dsc_data());
  FALCON2_DINO_DISP(falcon2_dino_rd_dsc_state());
  FALCON2_DINO_ESTM_PRINTF(("Uc Core Status Byte = 0x%x\n",falcon2_dino_rdcv_status_byte()));
  /* artifically terminate the command to re-enable the command interface */
  FALCON2_DINO_EFUN(falcon2_dino_wr_uc_dsc_ready_for_cmd(0x1));
  return (falcon2_dino_error(ERR_CODE_POLLING_TIMEOUT));          /* Error Code for polling timeout */
}

/* Poll for field "dsc_state" = DSC_STATE_UC_TUNE [Return Val => Error_code (0 = Polling Pass)] */
err_code_t falcon2_dino_poll_dsc_state_equals_uc_tune( const phymod_access_t *pa, uint32_t timeout_ms) {
  uint16_t loop;
  uint16_t dsc_state;
  /* poll 10 times to avoid longer delays later */
  for (loop = 0; loop < 100; loop++) {
    FALCON2_DINO_ESTM(dsc_state = falcon2_dino_rd_dsc_state());
    if (dsc_state == DSC_STATE_UC_TUNE) {
      return (ERR_CODE_NONE);
    }
    if(loop>10) {
        FALCON2_DINO_EFUN(falcon2_dino_delay_us(10*timeout_ms));
    }
  }
  FALCON2_DINO_ESTM_PRINTF(("FALCON2_DINO_DSC_STATE = %d\n", falcon2_dino_rd_dsc_state()));
  return (falcon2_dino_error(ERR_CODE_POLLING_TIMEOUT));          /* Error Code for polling timeout */
}


/* Poll for field "st_afe_tx_fifo_resetb" = 1 [Return Val => Error_code (0 = Polling Pass)] */
err_code_t falcon2_dino_poll_st_afe_tx_fifo_resetb_equals_1( const phymod_access_t *pa, uint32_t timeout_ms) {
  uint16_t loop;

  for (loop = 0; loop <= 100; loop++) {
    uint8_t st_afe_tx_fifo_resetb;
    FALCON2_DINO_ESTM(st_afe_tx_fifo_resetb = falcon2_dino_rd_st_afe_tx_fifo_resetb());
    if (st_afe_tx_fifo_resetb) {
      return (ERR_CODE_NONE);
    }
    FALCON2_DINO_EFUN(falcon2_dino_delay_us(10*timeout_ms));
  }
  return (falcon2_dino_error(ERR_CODE_POLLING_TIMEOUT));          /* Error Code for polling timeout */
}


/* Poll for field "micro_ra_initdone" = 1 [Return Val => Error_code (0 = Polling Pass)] */
err_code_t falcon2_dino_poll_micro_ra_initdone( const phymod_access_t *pa, uint32_t timeout_ms) {
  uint16_t loop;

  uint8_t result;
  for (loop = 0; loop <= 100; loop++) {
    FALCON2_DINO_ESTM(result = falcon2_dino_rdc_micro_ra_initdone());
    if (result) {
      return (ERR_CODE_NONE);
    }
    FALCON2_DINO_EFUN(falcon2_dino_delay_us(10*timeout_ms));
  }
  return (falcon2_dino_error(ERR_CODE_POLLING_TIMEOUT));          /* Error Code for polling timeout */
}
#endif /* CUSTOM_REG_POLLING */


/* Configure ULL mode for TX */
err_code_t falcon2_dino_tx_ull_config( const phymod_access_t *pa, uint8_t enable) {
    FALCON2_DINO_EFUN(falcon2_dino_tx_rptr_mode_timing( pa, enable)); /* Enable jitter filter enable */
    FALCON2_DINO_EFUN(falcon2_dino_ull_tx_mode_setup( pa, enable));
    return (ERR_CODE_NONE);
}

/* Configure ULL mode for RX */
err_code_t falcon2_dino_rx_ull_config( const phymod_access_t *pa, uint8_t enable) {
    FALCON2_DINO_EFUN(falcon2_dino_rx_rptr_mode_timing( pa, enable));
    FALCON2_DINO_EFUN(falcon2_dino_ull_rx_mode_setup( pa, enable));
    return (ERR_CODE_NONE);
}

/* TX PI setup for Repeater Mode */
err_code_t falcon2_dino_tx_rptr_mode_timing( const phymod_access_t *pa, uint8_t enable) {
        /* Enable TX PI and turn on 1st order loop in jitter filter */
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_repeater_mode_en(0x1));
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_en(enable));                               /* TX_PI enable: 0 = diabled, 1 = enabled */
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_jitter_filter_en(enable));                 /* Jitter filter enable to lock freq: 0 = diabled, 1 = enabled */
        FALCON2_DINO_EFUN(falcon2_dino_delay_us(25));                            /* Wait for TX_PI to settle */
    return (ERR_CODE_NONE);
}

/* RX setup for Repeater Mode timing */
err_code_t falcon2_dino_rx_rptr_mode_timing( const phymod_access_t *pa, uint8_t enable) {
    if(enable) {
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_loop_timing_src_sel(1));
        FALCON2_DINO_EFUN(falcon2_dino_wr_ams_tx_ticksel(3));
    } else {
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_loop_timing_src_sel(0));
    }

    return (ERR_CODE_NONE);
}


err_code_t falcon2_dino_init_pram_for_uc_load( const phymod_access_t *pa, uint16_t ucode_len) {

    if (ucode_len > FALCON2_DINO_UCODE_MAX_SIZE) {                     /* uCode size should be less than UCODE_MAX_SIZE  */
      return (falcon2_dino_error(ERR_CODE_INVALID_UCODE_LEN));
    }

    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_protect_fwcode(0x0));                    /* Remove Gate-off RAM chip select and other signals */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_master_clk_en(0x1));                     /* Enable clock to microcontroller subsystem */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_master_rstb(0x1));                       /* De-assert reset to microcontroller sybsystem */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_master_rstb(0x0));                       /* Assert reset to microcontroller sybsystem - Toggling reset*/
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_master_rstb(0x1));                       /* De-assert reset to microcontroller sybsystem */

    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_init(0x1));                           /* Set initialization command to initialize code RAM */
    FALCON2_DINO_EFUN(falcon2_dino_poll_micro_ra_initdone( pa, 250));               /* Poll for micro_ra_initdone = 1 to indicate initialization done */

    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_init(0x0));                           /* Clear initialization command */
    /* ucode_len_padded = ((ucode_len + 3) & 0xFFFC); */    /* make sure it is on 32bit boundary */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_pramif_ahb_wraddr_msw(0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_pramif_ahb_wraddr_lsw(0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_pram_if_rstb(1));
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_pramif_en(1));

    /* Wait 8 pram clocks */
    /* write ucode into pram */
    /* Wait 8 pram clocks */
    /* call falcon2_dino_finish_pram_load */
    return(ERR_CODE_NONE);
}

err_code_t falcon2_dino_finish_pram_load( const phymod_access_t *pa ) {
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_pramif_en(0));
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_core_clk_en(1));
   /* FALCON2_DINO_EFUN(wrc_micro_core_rstb(1)); */
    return(ERR_CODE_NONE);
}


/****************************************/
/*  Serdes Register/Variable Dump APIs  */
/****************************************/

err_code_t falcon2_dino_reg_dump( const phymod_access_t *pa ) {

  uint16_t addr, rddata;

  FALCON2_DINO_EFUN_PRINTF(("\n****  SERDES REGISTER DUMP    ****"));

  for (addr = 0x0; addr < 0x10; addr++) {
    if (!(addr % 16))  {
      FALCON2_DINO_EFUN_PRINTF(("\n%04x ",addr));
    }
    FALCON2_DINO_EFUN(falcon2_dino_pmd_rdt_reg(pa,addr,&rddata));
    FALCON2_DINO_EFUN_PRINTF(("%04x ",rddata));
  }

  for (addr = 0x90; addr < 0xA0; addr++) {
    if (!(addr % 16))  {
      FALCON2_DINO_EFUN_PRINTF(("\n%04x ",addr));
    }
    FALCON2_DINO_EFUN(falcon2_dino_pmd_rdt_reg(pa,addr,&rddata));
    FALCON2_DINO_EFUN_PRINTF(("%04x ",rddata));
  }

  for (addr = 0xD000; addr < 0xD180; addr++) {
    if (!(addr % 16))  {
      FALCON2_DINO_EFUN_PRINTF(("\n%04x ",addr));
    }
    FALCON2_DINO_EFUN(falcon2_dino_pmd_rdt_reg(pa,addr,&rddata));
    FALCON2_DINO_EFUN_PRINTF(("%04x ",rddata));
  }

  for (addr = 0xD180; addr < 0xD200; addr++) {
    if (!(addr % 16))  {
      FALCON2_DINO_EFUN_PRINTF(("\n%04x ",addr));
    }
    FALCON2_DINO_EFUN(falcon2_dino_pmd_rdt_reg(pa,addr,&rddata));
    FALCON2_DINO_EFUN_PRINTF(("%04x ",rddata));
  }


  for (addr = 0xD200; addr < 0xD230; addr++) {
    if (!(addr % 16))  {
      FALCON2_DINO_EFUN_PRINTF(("\n%04x ",addr));
    }
    FALCON2_DINO_EFUN(falcon2_dino_pmd_rdt_reg(pa,addr,&rddata));
    FALCON2_DINO_EFUN_PRINTF(("%04x ",rddata));
  }

  for (addr = 0xFFD0; addr < 0xFFE0; addr++) {
    if (!(addr % 16))  {
      FALCON2_DINO_EFUN_PRINTF(("\n%04x ",addr));
    }
    FALCON2_DINO_EFUN(falcon2_dino_pmd_rdt_reg(pa,addr,&rddata));
    FALCON2_DINO_EFUN_PRINTF(("%04x ",rddata));
  }
  return (ERR_CODE_NONE);
}


err_code_t falcon2_dino_uc_core_var_dump( const phymod_access_t *pa ) {
  uint8_t addr;

  FALCON2_DINO_EFUN_PRINTF(("\n**** SERDES UC CORE RAM VARIABLE DUMP ****"));

  for (addr = 0x0; addr < FALCON2_DINO_CORE_VAR_RAM_SIZE; addr++) {
    if (!(addr % 26))  {
      FALCON2_DINO_EFUN_PRINTF(("\n%04x ",addr));
    }
    FALCON2_DINO_ESTM_PRINTF(("%02x ", falcon2_dino_rdbc_uc_var( pa, FALCON2_DINO_ERR, addr)));
  }
  return (ERR_CODE_NONE);
}


err_code_t falcon2_dino_uc_lane_var_dump( const phymod_access_t *pa ) {
  uint8_t     rx_lock, uc_stopped = 0;
  uint16_t    addr;


  FALCON2_DINO_EFUN_PRINTF(("\n**** SERDES UC LANE %d RAM VARIABLE DUMP ****",falcon2_dino_get_lane(pa)));

  FALCON2_DINO_ESTM(rx_lock = falcon2_dino_rd_pmd_rx_lock());

  if (rx_lock == 1) {
      FALCON2_DINO_ESTM(uc_stopped = falcon2_dino_rdv_usr_sts_micro_stopped());
      if (!uc_stopped) {
          FALCON2_DINO_EFUN(falcon2_dino_stop_rx_adaptation( pa, 1));
      }
  } else {
      FALCON2_DINO_EFUN(falcon2_dino_pmd_uc_control( pa, CMD_UC_CTRL_STOP_IMMEDIATE,200));
  }

    for (addr = 0x0; addr < FALCON2_DINO_LANE_VAR_RAM_SIZE; addr++) {
      if (!(addr % 26))  {
        FALCON2_DINO_EFUN_PRINTF(("\n%04x ",addr));
      }
      FALCON2_DINO_ESTM_PRINTF(("%02x ", falcon2_dino_rdbl_uc_var( pa, FALCON2_DINO_ERR, addr)));
    }

  if (rx_lock == 1) {
      if (!uc_stopped) {
          FALCON2_DINO_EFUN(falcon2_dino_stop_rx_adaptation( pa, 0));
      }
  } else {
      FALCON2_DINO_EFUN(falcon2_dino_stop_rx_adaptation( pa, 0));
  }

  return (ERR_CODE_NONE);
}

/* Required Diagnostic Functions */
err_code_t falcon2_dino_display_diag_data( const phymod_access_t *pa, uint16_t  diag_level) {
    uint8_t rx_lock, micro_stop;
    uint32_t api_version;
    FALCON2_DINO_EFUN_PRINTF(("\n**** SERDES DISPLAY DIAG DATA ****\n"));
    FALCON2_DINO_ESTM_PRINTF(("Rev ID Letter  = %02X\n", falcon2_dino_rdc_revid_rev_letter()));
    FALCON2_DINO_ESTM_PRINTF(("Rev ID Process = %02X\n", falcon2_dino_rdc_revid_process()));
    FALCON2_DINO_ESTM_PRINTF(("Rev ID Model   = %02X\n", falcon2_dino_rdc_revid_model()));
    FALCON2_DINO_ESTM_PRINTF(("Rev ID Model   = %02X\n", falcon2_dino_rdc_revid2()));
    FALCON2_DINO_ESTM_PRINTF(("Rev ID # Lanes = %d\n"  ,falcon2_dino_rdc_revid_multiplicity()));
    FALCON2_DINO_ESTM_PRINTF(("Core = %d; LANE = %d\n",falcon2_dino_get_core(),falcon2_dino_get_lane(pa)));
    FALCON2_DINO_EFUN(falcon2_dino_version( pa, &api_version));
    FALCON2_DINO_EFUN_PRINTF(("SERDES API Version         = %06X\n",api_version));
    FALCON2_DINO_ESTM_PRINTF(("Common Ucode Version       = %04X", falcon2_dino_rdcv_common_ucode_version()));
    FALCON2_DINO_ESTM_PRINTF(("_%02X\n", falcon2_dino_rdcv_common_ucode_minor_version()));
    FALCON2_DINO_ESTM_PRINTF(("AFE Hardware Version       = 0x%X\n\n", falcon2_dino_rdcv_afe_hardware_version()));


    /* stop micro so all accesses are consistent */
    FALCON2_DINO_ESTM(rx_lock = falcon2_dino_rd_pmd_rx_lock());
    FALCON2_DINO_ESTM(micro_stop = falcon2_dino_rdv_usr_sts_micro_stopped());
    if (rx_lock == 1) {
        if (!micro_stop) {
            FALCON2_DINO_EFUN(falcon2_dino_stop_rx_adaptation( pa, 1));
        } 
    } else {
        FALCON2_DINO_EFUN(falcon2_dino_pmd_uc_control( pa, CMD_UC_CTRL_STOP_IMMEDIATE,200));
    }

    if(diag_level & SRDS_DIAG_LANE) {
        struct falcon2_dino_detailed_lane_status_st lane_st;
        USR_MEMSET(&lane_st, 0, sizeof(lane_st));
        FALCON2_DINO_EFUN(falcon2_dino_log_full_pmd_state( pa, &lane_st));
        FALCON2_DINO_EFUN(falcon2_dino_disp_full_pmd_state( pa, &lane_st,1));
    } else {
        FALCON2_DINO_EFUN(falcon2_dino_display_lane_state_hdr( pa ));
        FALCON2_DINO_EFUN(falcon2_dino_display_lane_state( pa ));
    }
    if(diag_level & SRDS_DIAG_CORE) {
        FALCON2_DINO_EFUN(falcon2_dino_display_core_state_hdr( pa ));
        FALCON2_DINO_EFUN(falcon2_dino_display_core_state_line( pa ));
    }
    if(diag_level & SRDS_DIAG_EVENT) {
        uint8_t trace_mem[1000]; 
        USR_MEMSET(trace_mem,0,sizeof(trace_mem));
        FALCON2_DINO_EFUN(falcon2_dino_read_event_log( pa, trace_mem,EVENT_LOG_HEX));
    }
    if(diag_level & SRDS_DIAG_EYE) {
        FALCON2_DINO_EFUN(falcon2_dino_display_eye_scan( pa ));
    }
    if(diag_level & SRDS_DIAG_REG_CORE) {
        FALCON2_DINO_EFUN(falcon2_dino_reg_dump( pa ));
    }
    /* currently REG_CORE and REG_LANE dump same data. */
    /*if(diag_level & SRDS_DIAG_REG_LANE) {*/
    /*    FALCON2_DINO_EFUN(falcon2_dino_reg_dump( pa ));*/
    /*}*/
    if(diag_level & SRDS_DIAG_UC_CORE) {
        FALCON2_DINO_EFUN(falcon2_dino_uc_core_var_dump( pa ));
    }
    if(diag_level & SRDS_DIAG_UC_LANE) {
        FALCON2_DINO_EFUN(falcon2_dino_uc_lane_var_dump( pa ));
    }
    if(diag_level & SRDS_DIAG_LANE_DEBUG) {
        FALCON2_DINO_EFUN(falcon2_dino_display_lane_debug_status( pa ));
    }
    if(diag_level & SRDS_DIAG_BER_VERT) {
        /* display ber projections for all channels */
        uint8_t ber_mode = DIAG_BER_VERT | DIAG_BER_POS;
        uint8_t timer_control = 23;       /* 30 seconds */
        uint8_t err_threshold = 100 / 16; /* 100 errors */ 
        FALCON2_DINO_EFUN(_falcon2_dino_display_ber_scan_data( pa, ber_mode, timer_control,  err_threshold));
        ber_mode = DIAG_BER_VERT | DIAG_BER_NEG;
        FALCON2_DINO_EFUN(_falcon2_dino_display_ber_scan_data( pa, ber_mode, timer_control,  err_threshold));
    }
    if(diag_level & SRDS_DIAG_BER_HORZ) {
        /* display ber projections for all channels */
        uint8_t ber_mode = DIAG_BER_HORZ | DIAG_BER_POS;
        uint8_t timer_control = 23;       /* 30 seconds */
        uint8_t err_threshold = 100 / 16; /* 100 errors */ 
        FALCON2_DINO_EFUN(_falcon2_dino_display_ber_scan_data( pa, ber_mode, timer_control,  err_threshold));
        ber_mode = DIAG_BER_HORZ | DIAG_BER_NEG;
        FALCON2_DINO_EFUN(_falcon2_dino_display_ber_scan_data( pa, ber_mode, timer_control,  err_threshold));
    }
    
    /* re enable micro */
    if (rx_lock == 1) {
        if (!micro_stop) {
            FALCON2_DINO_EFUN(falcon2_dino_stop_rx_adaptation( pa, 0));
        } 
    } else {
        FALCON2_DINO_EFUN(falcon2_dino_stop_rx_adaptation( pa, 0));
    }

    FALCON2_DINO_EFUN_PRINTF(("\n\n"));
    return (ERR_CODE_NONE);

}

/* Required Diagnostic Functions */
err_code_t falcon2_dino_diag_access( const phymod_access_t *pa, enum srds_diag_access_enum type, uint16_t addr, uint16_t data, uint16_t param) {

    switch(type) {
    case SRDS_REG_READ:      {
                             uint16_t tmp;
                             if(data > 1) {
                                 uint16_t i;
                                 FALCON2_DINO_EFUN_PRINTF(("\n****  SERDES BLK REGISTER READ    ****"));
                                 for (i = 0; i < data; i++) {
                                     if (!(i % 16))  {
                                         FALCON2_DINO_ESTM_PRINTF(("\n%04x ",i+addr));
                                     }
                                     FALCON2_DINO_EFUN(falcon2_dino_pmd_rdt_reg(pa,i+addr,&tmp));
                                     FALCON2_DINO_ESTM_PRINTF(("%04x ", tmp));
                                 }
                                 FALCON2_DINO_EFUN_PRINTF(("\n"));
                             } else {
                                 FALCON2_DINO_EFUN(falcon2_dino_pmd_rdt_reg(pa,addr,&tmp));
                                 FALCON2_DINO_EFUN_PRINTF(("Register Read: x%04x = x%04x\n",addr,tmp));
                             }
                             } break;
    case SRDS_REG_RMW:       {
                             uint16_t tmp, tmp2;
                             FALCON2_DINO_EFUN(falcon2_dino_pmd_rdt_reg(pa,addr,&tmp));
                             tmp2 = (tmp & ~param) | (param & data);
                             FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,addr,tmp2));
                             FALCON2_DINO_EFUN_PRINTF(("Register RMW: x%04x = x%04x -> x%04x\n",addr,tmp,tmp2));
                             } break;
    case SRDS_CORE_RAM_READ_BYTE: {
                             if(data > 1) {
                                 uint8_t i;
                                 FALCON2_DINO_EFUN_PRINTF(("\n****  SERDES BLK CORE RAM READ BYTE   ****"));
                                 for (i = 0; i < (uint8_t)data; i++) {
                                     if (!(i % 26))  {
                                         FALCON2_DINO_ESTM_PRINTF(("\n%04x ",i+(uint8_t)addr));
                                     }
                                     FALCON2_DINO_ESTM_PRINTF(("%02x ", falcon2_dino_rdbc_uc_var( pa, FALCON2_DINO_ERR, i+(uint8_t)addr)));
                                 }
                                 FALCON2_DINO_EFUN_PRINTF(("\n"));
                             } else {
                                 FALCON2_DINO_ESTM_PRINTF(("Core RAM Read byte: x%04x = x%02x\n",(uint8_t)addr,falcon2_dino_rdbc_uc_var( pa, FALCON2_DINO_ERR, (uint8_t)addr)));
                             }
                             } break;
    case SRDS_LANE_RAM_READ_BYTE: {
                             if(data > 1) {
                                 uint16_t i;
                                 FALCON2_DINO_EFUN_PRINTF(("\n****  SERDES BLK LANE RAM READ BYTE   ****"));
                                 for (i = 0; i < data; i++) {
                                     if (!(i % 26))  {
                                         FALCON2_DINO_ESTM_PRINTF(("\n%04x ",i+addr));
                                     }
                                     FALCON2_DINO_ESTM_PRINTF(("%02x ", falcon2_dino_rdbl_uc_var( pa, FALCON2_DINO_ERR,i+addr)));
                                 }
                                 FALCON2_DINO_EFUN_PRINTF(("\n"));
                             } else {
                                 FALCON2_DINO_ESTM_PRINTF(("Lane RAM Read byte: x%04x = x%02x\n",addr,falcon2_dino_rdbl_uc_var( pa, FALCON2_DINO_ERR,addr)));
                             }
                             } break;
    case SRDS_CORE_RAM_READ_WORD: {
                             if(data > 1) {
                                 uint8_t i;
                                 FALCON2_DINO_EFUN_PRINTF(("\n****  SERDES BLK CORE RAM READ WORD   ****"));
                                 for (i = 0; i < (uint8_t)data; i+=2) {
                                     if (!(i % 16))  {
                                         FALCON2_DINO_ESTM_PRINTF(("\n%04x ",i+(uint8_t)addr));
                                     }
                                     FALCON2_DINO_ESTM_PRINTF(("%04x ", falcon2_dino_rdwc_uc_var( pa, FALCON2_DINO_ERR, i+(uint8_t)addr)));
                                 }
                                 FALCON2_DINO_EFUN_PRINTF(("\n"));
                             } else {
                                 FALCON2_DINO_ESTM_PRINTF(("Core RAM Read word: x%04x = x%04x\n",addr,falcon2_dino_rdwc_uc_var( pa, FALCON2_DINO_ERR, (uint8_t)addr)));
                             }
                             } break;
    case SRDS_LANE_RAM_READ_WORD: {
                             if(data > 1) {
                                 uint16_t i;
                                 FALCON2_DINO_EFUN_PRINTF(("\n****  SERDES BLK LANE RAM READ WORD   ****"));
                                 for (i = 0; i < data; i+=2) {
                                     if (!(i % 16))  {
                                         FALCON2_DINO_ESTM_PRINTF(("\n%04x ",i=i+addr));
                                     }
                                     FALCON2_DINO_ESTM_PRINTF(("%04x ", falcon2_dino_rdwl_uc_var( pa, FALCON2_DINO_ERR,i+addr)));
                                 }
                                 FALCON2_DINO_EFUN_PRINTF(("\n"));
                             } else {
                                 FALCON2_DINO_ESTM_PRINTF(("Lane RAM Read word: x%04x = x%04x\n",addr,falcon2_dino_rdwl_uc_var( pa, FALCON2_DINO_ERR, addr)));
                             }
                             } break;
    case SRDS_CORE_RAM_RMW_BYTE:  {
                             uint8_t tmp, tmp2;
                             FALCON2_DINO_ESTM(tmp = falcon2_dino_rdbc_uc_var( pa, FALCON2_DINO_ERR, (uint8_t)addr));
                             tmp2 = (tmp & (uint8_t)~param) | ((uint8_t)param & data);
                             FALCON2_DINO_EFUN(falcon2_dino_wrbc_uc_var( pa, (uint8_t)addr,tmp2));
                             FALCON2_DINO_EFUN_PRINTF(("Core RAM RMW byte: x%04x = x%02x -> x%02x\n",addr,tmp,tmp2));
                             } break;
    case SRDS_LANE_RAM_RMW_BYTE:  {
                             uint8_t tmp, tmp2;
                             FALCON2_DINO_ESTM(tmp = falcon2_dino_rdbl_uc_var( pa, FALCON2_DINO_ERR,addr));
                             tmp2 = (tmp & (uint8_t)~param) | ((uint8_t)param & data);
                             FALCON2_DINO_EFUN(falcon2_dino_wrbl_uc_var( pa, addr,tmp2));
                             FALCON2_DINO_EFUN_PRINTF(("Lane RAM RMW byte: x%04x = x%02x -> x%02x\n",addr,tmp,tmp2));
                             } break;
    case SRDS_CORE_RAM_RMW_WORD:  {
                             uint16_t tmp, tmp2;
                             FALCON2_DINO_ESTM(tmp = falcon2_dino_rdwc_uc_var( pa, FALCON2_DINO_ERR, (uint8_t)addr));
                             tmp2 = (tmp & ~param) | (param & data);
                             FALCON2_DINO_EFUN(falcon2_dino_wrwc_uc_var( pa, (uint8_t)addr,tmp2));
                             FALCON2_DINO_EFUN_PRINTF(("Core RAM RMW word: x%04x = x%04x -> x%04x\n",addr,tmp,tmp2));
                             } break;
    case SRDS_LANE_RAM_RMW_WORD:  {
                             uint16_t tmp, tmp2;
                             FALCON2_DINO_ESTM(tmp = falcon2_dino_rdwl_uc_var( pa, FALCON2_DINO_ERR,addr));
                             tmp2 = (tmp & ~param) | (param & data);
                             FALCON2_DINO_EFUN(falcon2_dino_wrwl_uc_var( pa, addr,tmp2));
                             FALCON2_DINO_EFUN_PRINTF(("Lane RAM RMW word: x%04x = x%04x -> x%04x\n",addr,tmp,tmp2));
                             } break;
    case SRDS_GLOB_RAM_READ_BYTE: {
                             if(data > 1) {
                                 uint16_t i;
                                 FALCON2_DINO_EFUN_PRINTF(("\n****  SERDES BLK GLOB RAM READ BYTE   ****"));
                                 for (i = 0; i < data; i++) {
                                     if (!(i % 16))  {
                                         FALCON2_DINO_ESTM_PRINTF(("\n%04x ",i+addr));
                                     }
                                     FALCON2_DINO_ESTM_PRINTF(("%02x ", _falcon2_dino_rdb_uc_var( pa, FALCON2_DINO_ERR,i+addr)));
                                 }
                                 FALCON2_DINO_EFUN_PRINTF(("\n"));
                             } else {
                                 FALCON2_DINO_ESTM_PRINTF(("Glob RAM Read byte: x%04x = x%02x\n",addr,_falcon2_dino_rdb_uc_var( pa, FALCON2_DINO_ERR,addr)));
                             }
                             } break;
    case SRDS_GLOB_RAM_RMW_BYTE:  {
                             uint8_t tmp, tmp2;
                             FALCON2_DINO_ESTM(tmp = _falcon2_dino_rdb_uc_var( pa, FALCON2_DINO_ERR,addr));
                             tmp2 = (tmp & (uint8_t)~param) | ((uint8_t)param & data);
                             FALCON2_DINO_EFUN(_falcon2_dino_wrb_uc_var( pa, addr,tmp2));
                             FALCON2_DINO_EFUN_PRINTF(("Glob RAM RMW byte: x%04x = x%02x -> x%02x\n",addr,tmp,tmp2));
                             } break;
    case SRDS_GLOB_RAM_READ_WORD: {
                             if(data > 1) {
                                 uint16_t i;
                                 FALCON2_DINO_EFUN_PRINTF(("\n****  SERDES BLK GLOB RAM READ WORD   ****"));
                                 for (i = 0; i < data; i+=2) {
                                     if (!(i % 16))  {
                                         FALCON2_DINO_ESTM_PRINTF(("\n%04x ",i+addr));
                                     }
                                     FALCON2_DINO_ESTM_PRINTF(("%04x ", _falcon2_dino_rdw_uc_var( pa, FALCON2_DINO_ERR,i+addr)));
                                 }
                                 FALCON2_DINO_EFUN_PRINTF(("\n"));
                             } else {
                                 FALCON2_DINO_ESTM_PRINTF(("Glob RAM Read word: x%04x = x%04x\n",addr,_falcon2_dino_rdw_uc_var( pa, FALCON2_DINO_ERR,addr)));
                             }
                             } break;
    case SRDS_GLOB_RAM_RMW_WORD:  {
                             uint16_t tmp, tmp2;
                             FALCON2_DINO_ESTM(tmp = _falcon2_dino_rdw_uc_var( pa, FALCON2_DINO_ERR,addr));
                             tmp2 = (tmp & ~param) | (param & data);
                             FALCON2_DINO_EFUN(_falcon2_dino_wrw_uc_var( pa, addr,tmp2));
                             FALCON2_DINO_EFUN_PRINTF(("Glob RAM RMW word: x%04x = x%04x -> x%04x\n",addr,tmp,tmp2));
                             } break;
    case SRDS_UC_CMD:        {
                             uint16_t tmp;
                             FALCON2_DINO_EFUN(falcon2_dino_pmd_uc_cmd_with_data( pa, (enum srds_pmd_uc_cmd_enum)addr, (uint8_t)param, data, 100));
                             FALCON2_DINO_ESTM(tmp = falcon2_dino_rd_uc_dsc_data());
                             FALCON2_DINO_EFUN_PRINTF(("uC Command: cmd=x%02x supp=x%02x data=x%04x returned=x%04x\n",addr,param,data,tmp));
                             } break;
    case SRDS_BER_PROJ_DATA: {
                             /* display ber projections for all channels */
                             FALCON2_DINO_EFUN(_falcon2_dino_display_ber_scan_data( pa, (uint8_t)addr, (uint8_t)data,  (uint8_t)(param>>4)));
                             } break;
    case SRDS_EN_BREAKPOINT: 
    case SRDS_GOTO_BREAKPOINT:
    case SRDS_RD_BREAKPOINT:
    case SRDS_DIS_BREAKPOINT:
    default: FALCON2_DINO_EFUN_PRINTF(("Invalid request type falcon2_dino_diag_access\n"));
    }

    return(ERR_CODE_NONE);
}

/************************/
/*  Serdes API Version  */
/************************/

err_code_t falcon2_dino_version( const phymod_access_t *pa, uint32_t *api_version) {

    if(!api_version) {
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    *api_version = 0xA10214;
    return (ERR_CODE_NONE);
}


/***************************************/
/*  API Function to Read Event Logger  */
/***************************************/

err_code_t falcon2_dino_read_event_log( const phymod_access_t *pa, uint8_t *trace_mem,enum srds_event_log_display_mode_enum display_mode) {

    /* validate input arguments */
    if (trace_mem == NULL || (display_mode >= EVENT_LOG_MODE_MAX))
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

    /* stop writing event log */
    FALCON2_DINO_EFUN(falcon2_dino_event_log_stop( pa ));

    FALCON2_DINO_EFUN(falcon2_dino_event_log_readmem( pa, trace_mem));

    FALCON2_DINO_EFUN(falcon2_dino_event_log_display( pa, trace_mem, display_mode));

    return(ERR_CODE_NONE);
}

err_code_t falcon2_dino_event_log_stop( const phymod_access_t *pa ) {


    FALCON2_DINO_EFUN_PRINTF(("\n\n********************************************\n"));
    FALCON2_DINO_EFUN_PRINTF((    "**** SERDES UC TRACE MEMORY DUMP ***********\n"));
    FALCON2_DINO_EFUN_PRINTF((    "********************************************\n"));

    /* Start Read to stop uC logging and read the word at last event written by uC */
    FALCON2_DINO_EFUN(falcon2_dino_pmd_uc_cmd( pa, CMD_EVENT_LOG_READ, CMD_EVENT_LOG_READ_START, 10));

    return(ERR_CODE_NONE);
}

err_code_t falcon2_dino_event_log_readmem( const phymod_access_t *pa, uint8_t *trace_mem) {
    uint8_t uc_dsc_supp_info;
    uint16_t addr=0,trace_mem_size=0;
    /* validate input arguments */
    if (trace_mem == NULL)  return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

    FALCON2_DINO_ESTM_PRINTF(( "\n  DEBUG INFO: trace memory read index = 0x%04x\n", falcon2_dino_rdcv_trace_mem_rd_idx()));

    trace_mem_size = 768;
    FALCON2_DINO_EFUN_PRINTF(("  DEBUG INFO: trace memory size = 0x%04x\n\n", trace_mem_size));

    do {
        /* Read Next */
        FALCON2_DINO_EFUN(falcon2_dino_pmd_uc_cmd( pa, CMD_EVENT_LOG_READ, CMD_EVENT_LOG_READ_NEXT, 10));

        if (addr >= trace_mem_size) {
            return (ERR_CODE_INVALID_EVENT_LOG_READ);
        }
        else {
            addr++;
        }

        FALCON2_DINO_ESTM(*(trace_mem++) = (uint8_t) falcon2_dino_rd_uc_dsc_data());
        FALCON2_DINO_ESTM(uc_dsc_supp_info = falcon2_dino_rd_uc_dsc_supp_info());
    } while (uc_dsc_supp_info != 1);

    /* Read Done to resume logging  */
    FALCON2_DINO_EFUN(falcon2_dino_pmd_uc_cmd( pa, CMD_EVENT_LOG_READ, CMD_EVENT_LOG_READ_DONE, 10));

    return(ERR_CODE_NONE);
}

err_code_t falcon2_dino_event_log_display( const phymod_access_t *pa, uint8_t *trace_mem,enum srds_event_log_display_mode_enum display_mode) {
#define MAX_ENTRY_SIZE   (7)
#define NOR_ENTRY_SIZE   (4)
    uint16_t trace_mem_size=0;
    uint8_t num_lanes=0;
    uint8_t is_ref_time_found=0,is_last_event_timewrap=0;
    uint8_t post_cursor=0,curr_cursor=0,prev_cursor=0;
    uint16_t addr=0, rr=0, prev_rr=0;
    uint8_t cc=0;
    uint16_t time=0,ref_time=0,num_time_wraparound=0,this_num_time_wraparound=0;
    USR_DOUBLE prev_time=0,curr_time=0;
    uint8_t word_per_row=8;
    uint8_t prev_event=0;
    uint8_t supp_info[MAX_ENTRY_SIZE-NOR_ENTRY_SIZE];
    char uc_lane_id_str[256];
    uint8_t char_0='0';

    /* validate input arguments */
    if (trace_mem == NULL || (display_mode >= EVENT_LOG_MODE_MAX))
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

    trace_mem_size = 768;
    num_lanes = 4;

    /* output */
    if (display_mode == EVENT_LOG_HEX || display_mode == EVENT_LOG_HEX_AND_DECODED) {
        /* print the 16-bit words in Hex format to screen, 8 words per row */
        for (rr=0; rr<trace_mem_size; rr+=(2*word_per_row)) {
            for (cc=0; cc<2*word_per_row; cc+=2) {
                FALCON2_DINO_EFUN_PRINTF(("  0x%02x%02x", *(trace_mem+rr+cc), *(trace_mem+rr+cc+1)));
            }
            FALCON2_DINO_EFUN_PRINTF(("    %d\n",rr));
        }
    }
    if (display_mode == EVENT_LOG_DECODED || display_mode == EVENT_LOG_HEX_AND_DECODED) {
        /* decode for level 1 events */

        /* print a text log of the events going backwards in time, showing time as T-10.340ms, where T is when the Start Read happened. */
        addr = 0;
        while (addr < trace_mem_size-8) {                                                            /* last 8-byte reserved for trace memory wraparound */

            if (*(trace_mem+addr) == 0x0) {                                                          /* reach event log end */
                FALCON2_DINO_EFUN_PRINTF(("\n========== End of Event Log ==================\n"));
                break;
            }

            if (*(trace_mem+addr) == 0xff) {                                                         /* timestamp wraparound event handling */

                this_num_time_wraparound = *(trace_mem+(++addr));
                this_num_time_wraparound = ((this_num_time_wraparound<<8) | *(trace_mem+(++addr)));
                num_time_wraparound += this_num_time_wraparound;

                if (!is_last_event_timewrap) {                                                         /* display the rest of previous event info */
                    FALCON2_DINO_EFUN_PRINTF((","));
                    FALCON2_DINO_EFUN(_falcon_display_event( pa, prev_event,(uint8_t)prev_rr,prev_cursor,curr_cursor,post_cursor,supp_info));
                    is_last_event_timewrap = 1;
                }

                FALCON2_DINO_EFUN_PRINTF(("\n  %5d timestamp wraparound(s). \n\n", this_num_time_wraparound));
                this_num_time_wraparound = 0;

                addr++;
                continue;
            }
            else {
                cc = (trace_mem[addr] & 0x1f);                                                         /* lane id */
                if (cc >= num_lanes) {
                    FALCON2_DINO_EFUN_PRINTF(("\n\n  Incorrect lane ID. Terminating event log display for current core... \n\n"));
                    break;
                }

                rr = (uint16_t) ((trace_mem[addr]>>5) & 0x7);                                          /* determine event entry length */

                time = *(trace_mem+(++addr));                                                          /* timestamp */
                time = ((time<<8) | *(trace_mem+(++addr)));
                if (is_ref_time_found == 0) {                                                          /* determine the reference time origin */
                    is_ref_time_found = 1;
                    ref_time = time;
                }
                else {
#ifdef SERDES_API_FLOATING_POINT
                    curr_time = (time-ref_time-(num_time_wraparound*65536))/100.0;
#else
                    curr_time = (time-ref_time-(num_time_wraparound*65536))/100;
#endif
                    if (!is_last_event_timewrap) {
#ifdef SERDES_API_FLOATING_POINT
                        FALCON2_DINO_EFUN_PRINTF((" (+%.2f),", prev_time-curr_time));
#else
                        FALCON2_DINO_EFUN_PRINTF((" (+%d),", prev_time-curr_time));
#endif
                        FALCON2_DINO_EFUN(_falcon_display_event( pa, prev_event,(uint8_t)prev_rr,prev_cursor,curr_cursor,post_cursor,supp_info));
                    }
                    else {
                        is_last_event_timewrap = 0;
                    }
                }
                /* coverity[dead_error_condition] */
                if (cc < 10) {
                    uc_lane_id_str[0] = char_0 + cc;
                    uc_lane_id_str[1] = '\0';
                } 
                /* coverity[dead_error_condition] */
                else if (cc < 100) {
                    uc_lane_id_str[0] = char_0 + (cc/10);
                    uc_lane_id_str[1] = char_0 + (cc%10);
                    uc_lane_id_str[2] = '\0';
                } else {
                    uc_lane_id_str[0] = char_0 + (cc/100);
                    uc_lane_id_str[1] = char_0 + ((cc%100)-(cc%10))/10;
                    uc_lane_id_str[2] = char_0 + (cc%10);
                    uc_lane_id_str[3] = '\0';
                }

                FALCON2_DINO_EFUN(falcon2_dino_uc_lane_idx_to_system_id(uc_lane_id_str, cc));
                FALCON2_DINO_EFUN_PRINTF(("  Lane %s: ",uc_lane_id_str));
#ifdef SERDES_API_FLOATING_POINT
                FALCON2_DINO_EFUN_PRINTF(("  t= %.2f ms", curr_time));
#else
                FALCON2_DINO_EFUN_PRINTF(("  t= %d ms", curr_time));
#endif
                prev_time = curr_time;
                prev_rr = rr;

                prev_event = *(trace_mem+(++addr));
                switch (prev_event) {
                case EVENT_CODE_CL72_READY_FOR_COMMAND:
                case EVENT_CODE_EACH_WRITE_TO_CL72_TX_CHANGE_REQUEST:
                case EVENT_CODE_CL72_LOCAL_TX_CHANGED:
                    if (rr != NOR_ENTRY_SIZE) {
                        post_cursor = (*(trace_mem+(++addr))&0x30)>>4;
                        curr_cursor = (*(trace_mem+addr)&0x0C)>>2;
                        prev_cursor = *(trace_mem+addr)&0x03;
                        addr--;                                                         /* rewind to populate supplement info */
                    }
                    break;
                case EVENT_CODE_GENERAL_EVENT_0:
                case EVENT_CODE_GENERAL_EVENT_1:
                case EVENT_CODE_GENERAL_EVENT_2:
                    post_cursor = *(trace_mem+(++addr));
                    prev_cursor = *(trace_mem+(++addr));
                    addr -= 2;                                                         /* rewind to populate supplement info */
                    break;
                case EVENT_CODE_ERROR_EVENT:
                    post_cursor = *(trace_mem+(++addr));
                    addr--;
                    break;
                case EVENT_CODE_SM_STATUS_RESTART:
                    post_cursor = *(trace_mem+(++addr));
                    addr--;
                    break;
                default:
                    break;
                }

                /* retrieve supplement info, if any */
                for(cc=0; cc<rr-NOR_ENTRY_SIZE; cc++) {
                    supp_info[cc] = *(trace_mem+(++addr));
                }

                addr++;
            }
        }
    }

    return(ERR_CODE_NONE);
}


err_code_t falcon2_dino_display_state ( const phymod_access_t *pa ) {
    FALCON2_DINO_EFUN(falcon2_dino_display_core_state( pa ));
    FALCON2_DINO_EFUN(falcon2_dino_display_lane_state_hdr( pa ));
    FALCON2_DINO_EFUN(falcon2_dino_display_lane_state( pa ));
    FALCON2_DINO_EFUN(falcon2_dino_display_lane_state_legend( pa ));
    return(ERR_CODE_NONE);
}

err_code_t falcon2_dino_display_config ( const phymod_access_t *pa ) {
    FALCON2_DINO_EFUN(falcon2_dino_display_core_config( pa ));
    FALCON2_DINO_EFUN(falcon2_dino_display_lane_config( pa ));
    return(ERR_CODE_NONE);
}


/**********************************************/
/*  Temperature forcing and reading           */
/**********************************************/

err_code_t falcon2_dino_force_die_temperature ( const phymod_access_t *pa, int16_t die_temp) {
    /* Formula for PVTMON: T = 410.04-0.48705*reg10bit (from PVTMON Analog Module Specification v5.0 section 6.2) */
    int32_t die_temp_int;
    if (die_temp>130)
        die_temp = 130;
    if (die_temp<-45)
        die_temp = -45;

    die_temp_int = (int32_t)die_temp;
    FALCON2_DINO_EFUN(falcon2_dino_wrcv_temp_frc_val((uint16_t)((431045 - ((die_temp_int<<10) + (die_temp_int<<4) + (die_temp_int<<3) + (die_temp_int<<1) + die_temp_int))>>9)));

    return(ERR_CODE_NONE);
}

err_code_t falcon2_dino_read_die_temperature ( const phymod_access_t *pa, int16_t *die_temp) {

    uint16_t die_temp_sensor_reading;

    FALCON2_DINO_EFUN(falcon2_dino_pmd_uc_cmd( pa, CMD_UC_DBG,CMD_UC_DBG_DIE_TEMP,100));
    FALCON2_DINO_ESTM(die_temp_sensor_reading=falcon2_dino_rd_uc_dsc_data());
    *die_temp = falcon2_dino_bin_to_degC(die_temp_sensor_reading);

    return(ERR_CODE_NONE);
}


err_code_t falcon2_dino_read_die_temperature_double (const phymod_access_t *pa, USR_DOUBLE *die_temp) {
#ifdef SERDES_API_FLOATING_POINT
    /* Formula for PVTMON: T = 410.04-0.48705*reg10bit (from PVTMON Analog Module Specification v5.0 section 6.2) */
    uint16_t die_temp_sensor_reading;

    FALCON2_DINO_EFUN(falcon2_dino_pmd_uc_cmd( pa, CMD_UC_DBG,CMD_UC_DBG_DIE_TEMP,100));
    FALCON2_DINO_ESTM(die_temp_sensor_reading=falcon2_dino_rd_uc_dsc_data());

    *die_temp = 410.04-0.48705* ((USR_DOUBLE)die_temp_sensor_reading);

    return(ERR_CODE_NONE);
#else
    FALCON2_DINO_EFUN_PRINTF((" Function 'falcon2_dino_read_die_temperature_double' needs SERDES_API_FLOATING_POINT defined to operate \n"));
    return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
#endif
}




