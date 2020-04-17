/* *
 * * $ID: $
 * *
 * * 
 * * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 * * 
* * $Copyright: (c) 2020 Broadcom Inc.
* * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef CBX_INT_INIT_H
#define CBX_INT_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * CBX Fsal init
 *
 * @param  None
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
extern int cbx_fsal_init(void);

/**
 * CBX Fsal uninit
 *
 * @param  None
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
extern int cbx_fsal_uninit(void);

#ifdef __cplusplus
}
#endif


#endif /* CBX_INT_INIT_H */
