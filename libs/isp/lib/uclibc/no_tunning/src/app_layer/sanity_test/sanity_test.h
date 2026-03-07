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

#ifndef __SANITY_TEST_H__
#define __SANITY_TEST_H__

#include "metadata.h"
#include "isp_dma_if.h"
#include "fw_api.h"
#include "isp_hw_if.h"
#ifdef HW_BUILD_FOR_TITIAN
#include "stest_evtname_titian.h"
#include "titian_hw_reg.h"

#define TOP_MD5_REG_OFFSET              TOP_TOP_CTRL_88_ADDR
#else
#include "stest_evtname_octob.h"
#include "octob_hw_reg.h"

#define TOP_MD5_REG_OFFSET              TOP_TOP_CTRL_18_ADDR
#endif

//#define    SANITY_TEST_ENABLE
//#define  STEST_RESULT_PATH                "/run/media/mmcblk0p1/stest_res/"
//#define  STEST_RESULT_NEW_PATH            "/mnt/stest_res/"
#define  RAW16_1080P_DSIZE                0x3F4800    //(1920*1080*16/8) Bytes

/// define sanity test event struct
typedef metadata_event_t stest_event_t;

typedef enum
{
    STEST_ISP_MD5_R_L   = 0,
    STEST_ISP_MD5_R_H,
    STEST_ISP_MD5_G_L,
    STEST_ISP_MD5_G_H,
    STEST_ISP_MD5_B_L,
    STEST_ISP_MD5_B_H,

    STEST_ISP_MD5_NUM_MAX,
} stest_md5_module;

/// sanity test result file type
typedef enum
{
    STFILE_YUVMD5_RES    = 0,
    STFILE_APICHK_RES,
    STFILE_IMAGE_RAW16,
    STFILE_ISPREG_LIST,

    STFILE_NUM_MAX,
} stest_file_type_t;

/// sanity test parameters ID
typedef enum
{
    STEST_CASE_INVALID          = -1,
    STEST_CASE_DDR_PRO_START    = 0x00,
    STEST_CASE_DDRP_LD_SDRAW,
    STEST_CASE_DDRP_REGLIST_START,
    STEST_CASE_DDRP_GEN_MD5,
    //STEST_CASE_DDRP_SAVE_YUV,
    STEST_CASE_DDRP_REGLIST_END,
    STEST_CASE_DDRP_DUMP_REGLIST,
    STEST_CASE_DDR_PRO_END,

    STEST_CASE_APIC_START       = 0x10,   //16
    STEST_CASE_APIC_END,

    STEST_CASE_NUM_MAX,
} stest_case_id_t;

typedef struct __stest_ddrp_params
{
    BOOL                      sDDRPInit;
    u32                       sResCnt;
    struct mem_block          sMemBlock;
    pthread_mutex_t           sCma_mutex;
} stest_ddrp_params;

typedef struct __stest_event_data
{
    BOOL        sDataInit;
    u32         sEvtNewId;
    u32         sEvtVal;
} stest_event_data;

typedef struct __stest_event_params
{
    u32               sEvtCnt;
    stest_event_data  sEvtPrms[STEST_EVTNAME_ARRAY_NUM];
} stest_event_params;

/// define Sanity Test params struct
typedef struct __sanity_test_params
{
    u32                   sCamId;
    stest_case_id_t       sTCaseId;
    stest_event_params    sEvtPrms[API_TYPE_TOTAL][META_EVT_DIRT_MAX];
    stest_ddrp_params     sDDRParams;
    FileHandle           *pFileHdl;
} sanity_test_params_t;


///Parse sanity test update api event params
isp_ret_t Stest_Api_Evt_Params(void*  pApiData);

isp_ret_t Stest_Res_File_Save(u32                camId,
                              stest_file_type_t  resType);

isp_ret_t Stest_Update_Glb_Params(
                sanity_test_params_t*  pStestParams);

void Isp_V4l_Hw_Debug_Print(isp_hw_req_t*  pHwReq,
                            drv_direction  direct);

extern char8* const g_paramStestToken[];
#endif

