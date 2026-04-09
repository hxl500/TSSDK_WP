#if ALG_ICD
#include <stdlib.h>
#include <string.h>
//#include "ts_rne_device.h"
#include "audio_alg_icd.h"

#define ICD_MDL_CFG_8K "./icd_8k.cfg"
#define ICD_MDL_WGT_8K "./icd_8k.weight"
static TS_U32 SAMPLE_ALG_GetFileSize(FILE *pf)
{
	TS_U32 fileSize = 0;
	if (NULL != pf) {
		if (0 == fseek(pf, 0, SEEK_END)) {
			fileSize = ftell(pf);
		}
		rewind(pf);
	}
	return fileSize;
}

static TS_S32 SAMPLE_ALG_LOAD_MODEL(const TS_CHAR *fileName, TS_U32 *pModelSize, TS_U8 **ppAddr)
{
	FILE *pf_File = fopen(fileName, "rb");
	if (!pf_File) {
		return TS_FAILURE;
	}
	*pModelSize = SAMPLE_ALG_GetFileSize(pf_File);
	TS_U8 *pAddress = (TS_U8 *)malloc(*pModelSize * sizeof(TS_U8));

	fread(pAddress, *pModelSize, 1, pf_File);
	fclose(pf_File);
	*ppAddr = pAddress;
	return TS_SUCCESS;
}

TS_S32 AUDIO_ALG_ICD_Init(TS_VOID **pHandle)
{
	TS_S32 s32Ret = TS_SUCCESS;
	// TS_ALG_SetLogLevel(ALG_LOG_DEBUG);
	TS_ALG_SetLogLevel(ALG_LOG_ERROR);

	SAMPLE_AUDIO_ALG_INSTANCE_S *pInst = *((SAMPLE_AUDIO_ALG_INSTANCE_S **)pHandle);

	AUDIO_SAMPLE_RATE_E smpl_rate = 0;
	TS_S32 frame_len = 0;
	TS_U32 cfg_num;
	SAMPLE_AUDIO_ALG_INIT_CFG_S *alg_init_cfg = SAMPLE_AUDIO_ALG_GetInitCfg(&cfg_num);
	for (int i = 0; i < cfg_num; i++) {
		if (AUDIO_ALG_MASK_ICD_8K == alg_init_cfg[i].eMask) {
			smpl_rate = alg_init_cfg[i].eAudioFs;
			frame_len = alg_init_cfg[i].s32FrameLen;
			break;
		}
	}

	TS_U8 *weightpath, *cfgpath;
	if ((AUDIO_SAMPLE_RATE_8000 == smpl_rate)) {
		cfgpath = (TS_U8 *)malloc(sizeof(ICD_MDL_CFG_8K));
		weightpath = (TS_U8 *)malloc(sizeof(ICD_MDL_WGT_8K));
		memcpy(cfgpath, ICD_MDL_CFG_8K, sizeof(ICD_MDL_CFG_8K));
		memcpy(weightpath, ICD_MDL_WGT_8K, sizeof(ICD_MDL_WGT_8K));
	} else {
		printf("[%s](%d):smpl_rate error:%d(should 16k/8k)\n", __FUNCTION__, __LINE__, smpl_rate);
		return TS_FAILURE;
	}
	printf("cfgpath:%s, size:%zu\n", cfgpath, sizeof(ICD_MDL_CFG_8K));
	printf("weightpath:%s, size:%zu\n", weightpath, sizeof(ICD_MDL_WGT_8K));

	TS_U32 u32WeightLen;
	TS_U8 *pWeightBuf = NULL;
	TS_U32 u32CfgLen;
	TS_U8 *pCfgBuf = NULL;
	s32Ret = SAMPLE_ALG_LOAD_MODEL((const TS_CHAR *)cfgpath, &u32CfgLen, &pCfgBuf);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("load Kws cfg file error, cfg path =%s\n", cfgpath);
		goto init_exit2;
	}

	s32Ret = SAMPLE_ALG_LOAD_MODEL((const TS_CHAR *)weightpath, &u32WeightLen, &pWeightBuf);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("load Kws weight file error, weight path =%s\n", weightpath);
		goto init_exit1;
	}

	ALG_ICD_INIT_S alg_init_param;
	memset(&alg_init_param, 0, sizeof(ALG_ICD_INIT_S));

	alg_init_param.pCfg = pCfgBuf;
	alg_init_param.s32GraphSize = u32CfgLen;
	alg_init_param.pWeight = pWeightBuf;
	alg_init_param.pSelfBuf = NULL;
	alg_init_param.s16FrameLen = frame_len;

	s32Ret = TS_ALG_ICD_Init(&(pInst->pHandle), &alg_init_param);
	if (0 != s32Ret) {
		SAMPLE_PRT("TS_ALG_ICD_Init error\n");
		goto init_exit1;
	}

	SAMPLE_PRT("TS_ALG_ICD_GetVersion:%s\n", TS_ALG_ICD_GetVersion());

	ALG_ICD_PARAM_S alg_set_param;
	alg_set_param.u32SmoothFrameNum = 5;
	alg_set_param.f32SmoothThreshold = 0.55f;
	alg_set_param.u32JumpTrigger = 1;
	alg_set_param.u32AlarmInterval = 226;
	TS_ALG_ICD_SetParam(pInst->pHandle, &alg_set_param);

	s32Ret = TS_SUCCESS;
	pInst->pfExit = AUDIO_ALG_ICD_Exit;
	pInst->pfProcess = AUDIO_ALG_ICD_Proc;

init_exit1:
	if (NULL != pWeightBuf)
		free(pWeightBuf);
init_exit2:
	if (NULL != pCfgBuf)
		free(pCfgBuf);

	return s32Ret;
}

TS_S32 AUDIO_ALG_ICD_Proc(TS_VOID *pHandle, AUDIO_FRAME_S *pIn)
{
	static unsigned int ai_frame_id = 0;

	TS_S32 s32Ret = TS_SUCCESS;

	TS_S32 i;
	SAMPLE_AUDIO_ALG_INSTANCE_S *pInst = (SAMPLE_AUDIO_ALG_INSTANCE_S *)pHandle;
	if (!pHandle || !pIn) {
		SAMPLE_PRT("AUDIO_ALG_ICD_Proc param is null\n");
		return TS_FAILURE;
	}

	TS_U32 cfg_num;
	TS_S32 frame_len = 0;
	SAMPLE_AUDIO_ALG_INIT_CFG_S *alg_init_cfg = SAMPLE_AUDIO_ALG_GetInitCfg(&cfg_num);
	for (int i = 0; i < cfg_num; i++) {
		if (AUDIO_ALG_MASK_ICD_8K == alg_init_cfg[i].eMask) {
			frame_len = alg_init_cfg[i].s32FrameLen;
			break;
		}
	}
	if (0 == frame_len)
		return TS_FAILURE;

	TS_S32 loop = pIn->u32Len / (frame_len * sizeof(TS_S16));
	//printf("in-len:%d, fr_len:%d\n", pIn->u32Len, frame_len);

	TS_S16 *input_addr = pIn->u64VirAddr[0];
	int icd_input_data_len = frame_len;
	for (i = 0; i < loop; ++i) {
		// proc_param.pIn = input_addr;
		// proc_param.u32InLen = frame_len;

		ALG_ICD_PROC_S inparam;
		inparam.pIn = input_addr;
		inparam.u32InLen = icd_input_data_len;

		ALG_ICD_RSLT_S stAlgICDResult;
		s32Ret = TS_ALG_ICD_Process(pInst->pHandle, &inparam, &stAlgICDResult);
		if (TS_SUCCESS != s32Ret)
			SAMPLE_PRT("error, TS_ALG_ICD_Process ret=%d\n", s32Ret);

		if (255 == stAlgICDResult.u8CryFlg) {
			;
		} else if (stAlgICDResult.u8CryFlg == 1) {
			SAMPLE_PRT("[loop, frame, cryflag, alarmflg, score]: [%4d, %4d, %d, %d, %f]\n", i, ai_frame_id,
				   stAlgICDResult.u8CryFlg, stAlgICDResult.u8AlarmFlg, stAlgICDResult.fscore);
		} else if (ai_frame_id % 10 == 0) {
			SAMPLE_PRT("[loop, frame, cryflag, alarmflg, score]: [%4d, %4d, %d, %d, %f]\n", i, ai_frame_id,
				   stAlgICDResult.u8CryFlg, stAlgICDResult.u8AlarmFlg, stAlgICDResult.fscore);
		}

		ai_frame_id++;
		input_addr += frame_len;
	}

	return TS_SUCCESS;
}

TS_S32 AUDIO_ALG_ICD_Exit(TS_VOID *pHandle)
{
	TS_S32 s32Ret = TS_SUCCESS;
	if (!pHandle) {
		SAMPLE_PRT("AUDIO_ALG_ICD_Exit param is null\n");
		return TS_FAILURE;
	}

	SAMPLE_AUDIO_ALG_INSTANCE_S *pInst = (SAMPLE_AUDIO_ALG_INSTANCE_S *)pHandle;
	s32Ret = TS_ALG_ICD_Exit(pInst->pHandle);
	SAMPLE_PRT("TS_ALG_ICD_Exit.\n");

	return s32Ret;
}

#endif
