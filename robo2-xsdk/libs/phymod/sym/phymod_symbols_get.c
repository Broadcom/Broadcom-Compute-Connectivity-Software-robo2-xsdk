/*
 * $Id$
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/*******************************************************************************
 *
 * PHYMOD Symbol Routines
 *
 *
 ******************************************************************************/

#include <phymod/phymod_symbols.h>

int 
phymod_symbols_get(const phymod_symbols_t* symbols, uint32_t sindex, phymod_symbol_t* rsym)
{
    if (symbols) {
	if (sindex < symbols->size) {
	    /* Index is within the symbol table */
	    *rsym = symbols->symbols[sindex]; 
	    return 0;
	}
    }
    return -1; 
}

