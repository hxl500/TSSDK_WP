/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_SHAREMSG_UAPI_H__
#define __TS_SHAREMSG_UAPI_H__

#include <linux/ioctl.h>
#include "ts_alg.h"

/* command for ioctl() */
#define SHAREMSG_IOC_MAGIC 's'

#define SHAREMSG_IOC_GET_STATUS		    _IOR(SHAREMSG_IOC_MAGIC, 0, struct sharemsg_status)
#define SHAREMSG_IOC_FREE_MEM		    _IO(SHAREMSG_IOC_MAGIC, 1)

#define VPE_MAIN_YUV_COMBINE
#ifdef VPE_MAIN_YUV_COMBINE
#define MAX_MAIN_INST_NUM 		(1)
#else
#define MAX_MAIN_INST_NUM 		(2)
#endif
#define MAX_SUB_INST_NUM 		(2)


typedef struct {
	u32 chn;
	u32 width;
	u32 height;

	u32 rne_run_cnt;
	u32 rne_ok_cnt;
	u32 rne_fail_cnt;
	u32 rne_err_code;
	u32 reserved;

	u32 curr_perdestrain_det;
	u32 curr_car_det;
	u32 curr_pet_det;
	u32 curr_pkg_det;

	u64 perdestrain_det_cnt;
	u64 car_det_cnt;
	u64 pet_det_cnt;
	u64 pkg_det_cnt;

	ALG_PCPP_DET_RESULT_S curr_alg_res;
} rne_alg_para_t;

typedef struct {
	u32 chn;
	u32 width;
	u32 height;
	u32 enc_run_cnt;
	u32 enc_ok_cnt;
	u32 enc_fail_cnt;
	u32 enc_err_code;

	void *fbs_base;
	u32 fbs_len;
	u32 fbs_max_size;
	u32 fbs_eof;
} venc_para_t;

typedef struct {
	u32 magic;
	u32 crc;
	rne_alg_para_t alg_paras[MAX_SUB_INST_NUM];
	venc_para_t venc_paras[MAX_MAIN_INST_NUM];
} bare_paras_t;

struct sharemsg_status {
	uint32_t mem_len;
	uint32_t phy_addr;
};

#endif
