/*
 * $Id: cint_eval_asts.h,v 1.2 Broadcom SDK $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        cint_eval_asts.h
 * Purpose:     CINT AST evaluator interfaces
 *
 */

#ifndef __CINT_EVAL_ASTS_H__
#define __CINT_EVAL_ASTS_H__

#include "cint_config.h"
#include "cint_ast.h"
#include "cint_error.h"
#include "cint_variables.h"

/*
 * Evaluate a single AST
 */
extern cint_variable_t*
cint_eval_ast(cint_ast_t* ast); 

/*
 * Evaluate all linked ASTs and return the 
 * the value of the last one. 
 */

extern cint_variable_t*
cint_eval_asts(cint_ast_t* ast); 

#endif /* __CINT_EVAL_ASTS_H__ */
