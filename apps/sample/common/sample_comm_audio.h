#ifndef __SAMPLE_COMM__AUDIO_H__
#define __SAMPLE_COMM__AUDIO_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <pthread.h>

#include "ts_common.h"
#include "ts_buffer.h"
#include "ts_comm_sys.h"
#include "ts_comm_vb.h"
#include "ts_comm_aio.h"
#include "ts_comm_adec.h"
#include "ts_comm_aenc.h"
#include "ts_comm_ai.h"
#include "ts_comm_ao.h"
#include "ts_defines.h"

#ifdef VQE_NN
#include "ts_sol_vqe_nn.h"
#ifdef VQE_NN_MONO_16K
#include "fullbank_mono_16k_cfg.h"
#include "fullbank_mono_16k_weight.h"
#include "subbank_mono_16k_cfg.h"
#include "subbank_mono_16k_weight.h"
#elif VQE_NN_STEREO_16K
	#ifdef VQE_STEREO_MODEL
	#include "fullbank_stereo_16k_cfg.h"
	#include "fullbank_stereo_16k_weight.h"
	#include "subbank_stereo_16k_cfg.h"
	#include "subbank_stereo_16k_weight.h"
	#else // VQE_STEREO_MVDR
	#include "fullbank_mono_16k_cfg.h"
	#include "fullbank_mono_16k_weight.h"
	#include "subbank_mono_16k_cfg.h"
	#include "subbank_mono_16k_weight.h"
	#endif
#elif VQE_NN_MONO_8K
#include "fullbank_mono_8k_cfg.h"
#include "fullbank_mono_8k_weight.h"
#include "subbank_mono_8k_cfg.h"
#include "subbank_mono_8k_weight.h"
#elif VQE_NN_STEREO_8K
	#ifdef VQE_STEREO_MODEL
	#include "fullbank_stereo_8_cfgk.h"
	#include "fullbank_stereo_8k_weight.h"
	#include "subbank_stereo_8k_cfg.h"
	#include "subbank_stereo_8k_weight.h"
	#else // VQE_STEREO_MVDR
	#include "fullbank_mono_8k_cfg.h"
	#include "fullbank_mono_8k_weight.h"
	#include "subbank_mono_8k_cfg.h"
	#include "subbank_mono_8k_weight.h"
	#endif
#endif
#elif VQE_WEBRTC
#include "ts_alg_vqe_sol_webrtc3a.h"
#endif

#include "mpi_sys.h"
#include "mpi_audio.h"

/*******************************************************
 *    macro define
 *******************************************************/
#define FILE_NAME_LEN               128

#define SAMPLE_AUDIO_EXTERN_AI_DEV 0
#define SAMPLE_AUDIO_EXTERN_AO_DEV 0
#define SAMPLE_AUDIO_INNER_AI_DEV 0
#define SAMPLE_AUDIO_INNER_AO_DEV 0
#define SAMPLE_AUDIO_INNER_HDMI_AO_DEV 1

#define SAMPLE_AUDIO_PTNUMPERFRM   480

//The algorithm returns the revised result.
#ifdef VQE_NN_MONO_16K
	#define up_fullbank_cfg fullbank_mono_16k_cfg
	#define up_fullbank_weight fullbank_mono_16k_weight
	#define up_subbank_cfg subbank_mono_16k_cfg
	#define up_subbank_weight subbank_mono_16k_weight
	#define dn_fullbank_cfg up_fullbank_cfg
	#define dn_fullbank_weight up_fullbank_weight
	#define dn_subbank_cfg up_subbank_cfg
	#define dn_subbank_weight up_subbank_weight
#elif VQE_NN_STEREO_16K
	#ifdef VQE_STEREO_MODEL
		#define up_fullbank_cfg fullbank_stereo_16k_cfg
		#define up_fullbank_weight fullbank_stereo_16k_weight
		#define up_subbank_cfg subbank_stereo_16k_cfg
		#define up_subbank_weight subbank_stereo_16k_weight
	#else // VQE_STEREO_MVDR
		#define up_fullbank_cfg fullbank_mono_16k_cfg
		#define up_fullbank_weight fullbank_mono_16k_weight
		#define up_subbank_cfg subbank_mono_16k_cfg
		#define up_subbank_weight subbank_mono_16k_weight
	#endif
	#define dn_fullbank_cfg fullbank_mono_16k_cfg
	#define dn_fullbank_weight fullbank_mono_16k_weight
	#define dn_subbank_cfg subbank_mono_16k_cfg
	#define dn_subbank_weight subbank_mono_16k_weight
#elif VQE_NN_MONO_8K
	#define up_fullbank_cfg fullbank_mono_8k_cfg
	#define up_fullbank_weight fullbank_mono_8k_weight
	#define up_subbank_cfg subbank_mono_8k_cfg
	#define up_subbank_weight subbank_mono_8k_weight

	#define dn_fullbank_cfg up_fullbank_cfg
	#define dn_fullbank_weight up_fullbank_weight
	#define dn_subbank_cfg up_subbank_cfg
	#define dn_subbank_weight up_subbank_weight
#elif VQE_NN_STEREO_8K
	#ifdef VQE_STEREO_MODEL
		#define up_fullbank_cfg fullbank_stereo_8k_cfg
		#define up_fullbank_weight fullbank_stereo_8k_weight
		#define up_subbank_cfg subbank_stereo_8k_cfg
		#define up_subbank_weight subbank_stereo_8k_weight
	#else  // VQE_STEREO_MVDR
		#define up_fullbank_cfg fullbank_mono_8k_cfg
		#define up_fullbank_weight fullbank_mono_8k_weight
		#define up_subbank_cfg subbank_mono_8k_cfg
		#define up_subbank_weight subbank_mono_8k_weight
	#endif
	#define dn_fullbank_cfg fullbank_mono_8k_cfg
	#define dn_fullbank_weight fullbank_mono_8k_weight
	#define dn_subbank_cfg subbank_mono_8k_cfg
	#define dn_subbank_weight subbank_mono_8k_weight
#endif

#define SAMPLE_PRT(fmt...)   \
	do {\
		printf("[%s]-%d: ", __func__, __LINE__);\
		printf(fmt);\
	} while (0)


/*******************************************************
 *   function announce
 *******************************************************/



TS_S32 SAMPLE_COMM_SYS_Init(VB_CONFIG_S *pstVbConfig);
TS_VOID SAMPLE_COMM_SYS_Exit(void);

TS_S32 SAMPLE_COMM_AUDIO_StartAi(AUDIO_DEV AiDevId, TS_S32 s32AiChnCnt,
	AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enOutSampleRate, TS_BOOL bEnableChn, TS_BOOL bResampleEn, TS_VOID* pstAiVqeAttr, TS_U32 u32AiVqeType);
TS_S32 SAMPLE_COMM_AUDIO_StartAo(AUDIO_DEV AoDevId, TS_S32 s32AoChnCnt,
								 AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enInSampleRate, TS_BOOL bResampleEn, TS_VOID* pstAoVqeAttr, TS_U32 u32AoVqeType);
TS_S32 SAMPLE_COMM_AUDIO_StartAdec(ADEC_CHN AdChn, PAYLOAD_TYPE_E enType);
TS_S32 SAMPLE_COMM_AUDIO_StartAenc(TS_S32 s32AencChnCnt, AIO_ATTR_S *pstAioAttr, PAYLOAD_TYPE_E enType);
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAo(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
TS_S32 SAMPLE_COMM_AUDIO_StopTrdAiAo(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAiTul(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
TS_S32 SAMPLE_COMM_AUDIO_StopTrdAiTul(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAiRaw(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
TS_S32 SAMPLE_COMM_AUDIO_StopTrdAiRaw(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAiRef(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
TS_S32 SAMPLE_COMM_AUDIO_StopTrdAiRef(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAenc(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);

TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAo(AUDIO_DEV AoDev, AO_CHN AoChn);
TS_S32 SAMPLE_COMM_AUDIO_StopTrdAo(AUDIO_DEV AoDev, AO_CHN AoChn);
TS_S32 SAMPLE_COMM_AUDIO_StartAencExtern(TS_S32 s32AencChnCnt, AIO_ATTR_S *pstAioAttr, PAYLOAD_TYPE_E enType);
TS_S32 SAMPLE_COMM_AUDIO_StartAdecExtern(ADEC_CHN AdChn, PAYLOAD_TYPE_E enType);
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAencAdec(AENC_CHN AeChn, ADEC_CHN AdChn, FILE* pAecFd);
TS_S32 SAMPLE_COMM_AUDIO_StopTrdAencAdec(AENC_CHN AeChn, ADEC_CHN AdChn, FILE* pAecFd);
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdFileAdec(ADEC_CHN AdChn, FILE* pAdcFd);
TS_S32 SAMPLE_COMM_AUDIO_StopTrdFileAdec(ADEC_CHN AdChn, FILE* pAdcFd);
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdGetAdec(ADEC_CHN AdChn);
TS_S32 SAMPLE_COMM_AUDIO_StopTrdGetAdec(ADEC_CHN AdChn);
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdSendAenc(AENC_CHN AeChn);
TS_S32 SAMPLE_COMM_AUDIO_StopTrdSendAenc(AENC_CHN AeChn);
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAencPushStream(AENC_CHN AeChn);
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdPullStreamAdec(ADEC_CHN AdChn);
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAiPushStream(AUDIO_DEV AiDev, AI_CHN AiChn);
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdPullStreamAo(AUDIO_DEV AoDev, AO_CHN AoChn);

TS_S32 SAMPLE_COMM_AI_Bind_AO(AI_CHN AiChn, AO_CHN AoChn);
TS_S32 SAMPLE_COMM_AI_UnBind_AO(AI_CHN AiChn, AO_CHN AoChn);

TS_S32 SAMPLE_COMM_AI_Bind_AENC(AI_CHN AiChn, AENC_CHN AencChn);
TS_S32 SAMPLE_COMM_AI_UnBind_AENC(AI_CHN AiChn, AENC_CHN AencChn);

TS_S32 SAMPLE_COMM_ADEC_Bind_AO(ADEC_CHN AdChn, AUDIO_DEV AoDev, AO_CHN AoChn);
TS_S32 SAMPLE_COMM_ADEC_UnBind_AO(ADEC_CHN AdChn, AUDIO_DEV AoDev, AO_CHN AoChn);

TS_S32 SAMPLE_COMM_AUDIO_StopAi(AUDIO_DEV AiDevId, TS_S32 s32AiChnCnt,  TS_BOOL bDisableAiChn, TS_BOOL bResampleEn, TS_BOOL bVqeEn);
TS_S32 SAMPLE_COMM_AUDIO_StopAo(AUDIO_DEV AoDevId, TS_S32 s32AoChnCnt, TS_BOOL bResampleEn);
TS_S32 SAMPLE_COMM_AUDIO_StopAenc(TS_S32 s32AencChnCnt);
TS_S32 SAMPLE_COMM_AUDIO_StopAdec(ADEC_CHN AdChn);

ts_s32 ai_vqe_open(ts_void **handle, ts_void *attr);
ts_s32 ai_vqe_set_proc_addr(ts_void *handle, ts_void *addr);
ts_s32 ai_vqe_process(ts_void *handle, ts_u8 *in_buf, ts_u8 *ref_buf, ts_u32 *in_len, ts_u8 *out_buf, ts_u32 *out_len);
ts_s32 ai_vqe_close(ts_void *handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* End of #ifndef __SAMPLE_COMM__AUDIO_H__ */
