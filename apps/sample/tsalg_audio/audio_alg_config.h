/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: the definitions of common functions
 * Author: Tsing Micro solution-application group
 * Create: 2024/01/22
 */

#ifndef __AUDIO_ALG_CONFIG_H__
#define __AUDIO_ALG_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "sample_comm_sys.h"
#include "sample_comm_audio.h"
#include "sample_comm_util.h"
#include "ts_math.h"
#include "ts_alg_type.h"

/*******************************************************
 * *
 * *    audio alg mask
 * *
********************************************************/
typedef enum {
	AUDIO_ALG_MASK_ICD_8K = 0, // infant cry detection
	AUDIO_ALG_MASK_CRY_DET,
	AUDIO_ALG_MAX_NUM
} SAMPLE_AUDIO_ALG_TYPE_E;

/*******************************************************
 * *
 * *    audio alg head and result
 * *
********************************************************/
#if ALG_ICD
#include "ts_alg_icd.h"
#include "audio_alg_icd.h"
#endif

#if ALG_CRYDETECT
#include "ts_alg_crydetect.h"
#include "audio_alg_crydetect.h"
#endif

/*******************************************************
 * *
 * *	other audio macro
 * *
********************************************************/
#define ALG_AUDIO_FRM_NUM 8
#define ALG_AUDIO_PTNUM_PERFRM 512

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif
