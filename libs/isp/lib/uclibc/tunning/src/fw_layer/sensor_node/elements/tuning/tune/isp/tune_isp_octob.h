/************************************************************************************************
 * This source code is CONFIDENTIAL and any use by you is subject to the terms of the agreement
 * between you and Arm China or the terms of the agreement between you and the party authorised
 * by Arm China to disclose this document to you.
 *
 * The confidential and proprietary information contained in this file may only be used by a
 * person authorised under and to the extent permitted by a subsisting licensing agreement from
 * Arm China.
 *
 * This entire notice must be reproduced on all copies of this file and copies of this file may
 * only be made by a person if such person is permitted to do so under the terms of a subsisting
 * license agreement from Arm China.
 *
 * Release Information: Linlon-i5-r0p2-00eac0
 *
 * Copyright (C) 2021 Arm Technology (China) Co. Ltd. All rights reserved.
 *
 ***********************************************************************************************/

#ifndef __TUNING_ISP_OCTOB_H__
#define __TUNING_ISP_OCTOB_H__

#if (defined(HW_BUILD_FOR_I5)       ||\
     defined(HW_BUILD_FOR_I5_R1P0)  ||\
     defined(HW_BUILD_FOR_I3)       ||\
     defined(HW_BUILD_FOR_CUST0))

#include "tune_utils.h"

#include "tune_isp_gain_range.h"
#include "tune_isp_blc_v0.h"
#include "tune_isp_ccm_v0.h"
#include "tune_isp_cnr_v0.h"
#include "tune_isp_csp_v0.h"
#include "tune_isp_dpc_v0.h"
#include "tune_isp_dpf_v0.h"
#include "tune_isp_lsc_v0.h"
#include "tune_isp_np_v0.h"
#include "tune_isp_sde_v0.h"

#include "tune_isp_lsh_v0.h"
#include "tune_isp_ltm_v0.h"
#include "tune_isp_cmb_v0.h"
#include "tune_isp_dms_v0.h"
#include "tune_isp_ggc_v0.h"
#include "tune_isp_mvd_v0.h"
#include "tune_isp_rnr_v0.h"
#include "tune_isp_tnr_v0.h"
#include "tune_isp_rir_v0.h"
#include "tune_isp_cac_v0.h"

#include "isp_ttnr_tuning_tun_536.h"
/***************************************************
*                   ISP Params                      *
 ***************************************************/
typedef struct
{
    tune_version_t          ispTuneVersion;
    tune_isp_gain_range_t   gainRange;
    tune_blc_param_t        blc;
    tune_lsc_param_t        lsc;
    tune_ltm_param_t        ltm;
    tune_ccm_param_t        ccm;
    tune_gamma_param_t      ggc;
    tune_dpc_param_t        dpc;
    tune_rnr_param_t        rnr;
    tune_np_param_t         np;
    tune_dms_param_t        dms;
    tune_dpf_param_t        dpf;
    tune_csp_param_t        csp;
    tune_mvd_param_t        mvd;
    tune_tnr_param_t        tnr;
    tune_lsh_param_t        lsh;
    tune_cnr_param_t        cnr;
    tune_cmb_param_t        cmb;
    tune_sde_param_t        sde;
    tune_rir_param_t        rir;
    tune_cac_param_t        cac;
    tun_ttnr_param_t ttnr;
} tune_isp_params_t;

#endif

#endif
