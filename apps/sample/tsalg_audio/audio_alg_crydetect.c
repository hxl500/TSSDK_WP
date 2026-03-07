#if ALG_CRYDETECT
#include "audio_alg_crydetect.h"
#include "ts_rne_device.h"
#include "ts_alg_gp_layers.h"
#include "sample_audio_alg_proxy.h"
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

TS_S32 AUDIO_ALG_CryDetect_Init(TS_VOID **pHandle)
{
	TS_S32 s32Ret = TS_SUCCESS;
	ALG_MODEL_INIT_S param;
	TS_ALG_SetLogLevel(ALG_LOG_ERROR);
	//log_set_level(ALG_LOG_DEBUG);

	SAMPLE_AUDIO_ALG_INSTANCE_S *pInst = *((SAMPLE_AUDIO_ALG_INSTANCE_S **)pHandle);

	TS_U8 weightpath[] = "./crydetect_16k_weight.weight";
	TS_U8 cfgpath[] = "./crydetect_16k_cfg.cfg";

	TS_U32 u32WeightLen;
	TS_U8 *pWeightBuf;
	TS_U32 u32CfgLen;
	TS_U8 *pCfgBuf;

	s32Ret = SAMPLE_ALG_LOAD_MODEL((const TS_CHAR *)cfgpath, &u32CfgLen, &pCfgBuf);
	if (TS_SUCCESS != s32Ret) {
		ALG_LOGE("load CryDetect cfg file error, cfg path =%s\n", cfgpath);
		goto init_exit2;
	}

	s32Ret = SAMPLE_ALG_LOAD_MODEL((const TS_CHAR *)weightpath, &u32WeightLen, &pWeightBuf);
	if (TS_SUCCESS != s32Ret) {
		ALG_LOGE("load CryDetect weight file error, weight path =%s\n", weightpath);
		goto init_exit1;
	}

	memset(&param, 0, sizeof(ALG_MODEL_INIT_S));
	param.pGraph = pCfgBuf;
	param.pWeight = pWeightBuf;
	param.u32GraphSize = u32CfgLen;
	param.u32WeightSize = u32WeightLen;
	param.eImageType = ALG_IMAGE_TYPE_INT_HWC;
	param.bRneOff = 0;

	s32Ret = TS_ALG_CryDetect_Init(&(pInst->pHandle), &param);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_CryDetect_Init error\n");
		goto init_exit1;
	}

	SAMPLE_PRT("TS_ALG_CryDetect_GetVersion:%s\n", TS_ALG_CryDetect_GetVersion());
	s32Ret = TS_SUCCESS;
	pInst->pfExit = AUDIO_ALG_CryDetect_Exit;
	pInst->pfProcess = AUDIO_ALG_CryDetect_Proc;

init_exit1:
	free(pWeightBuf);
init_exit2:
	free(pCfgBuf);

	return s32Ret;
}

TS_S32 AUDIO_ALG_CryDetect_Proc(TS_VOID *pHandle, AUDIO_FRAME_S *pIn)
{
	static unsigned int ai_frame_id = 0;
	//static FILE *file_data;

	TS_S32 s32Ret = TS_SUCCESS;
	TS_S32 i;
	SAMPLE_AUDIO_ALG_INSTANCE_S *pInst = (SAMPLE_AUDIO_ALG_INSTANCE_S *)pHandle;
	if (!pHandle || !pIn) {
		SAMPLE_PRT("AUDIO_ALG_CryDetect_Proc param is null\n");
		return TS_FAILURE;
	}

	ALG_CRY_RESULT_S stResult;

	ALG_IMAGE_S image;
	image.s32H = 1;
	image.s32W = 1;
	image.s32C = 512; //byte
	TS_S32 loop = pIn->u32Len / image.s32C;
	//if (0 == ai_frame_id)
	//	file_data = fopen("./cdy_data.log", "wb");

	for (i = 0; i < loop; ++i) {
		image.pData = pIn->u64VirAddr[0] + image.s32C * i;
		//fwrite(image.pData, 1, image.s32C, file_data);
		//image.pData = BB0591 + image.s32C*ai_frame_id;
		s32Ret = TS_ALG_CryDetect_Process(pInst->pHandle, &image, &stResult);
		if (TS_SUCCESS == s32Ret && 1 == stResult.s32CryRes)
			SAMPLE_PRT("[loop, frame id, result]: [%2d, %4d, %d]\n", loop, ai_frame_id,
				   stResult.s32CryRes); //0:no-cry; 1:cry
		else if (TS_SUCCESS == s32Ret && ai_frame_id % 20 == 0)
			SAMPLE_PRT("[loop, frame id, result]: [%2d, %4d, %d]\n", loop, ai_frame_id,
				   stResult.s32CryRes); //0:no-cry; 1:cry
		ai_frame_id++;
	}

	return TS_SUCCESS;
}

TS_S32 AUDIO_ALG_CryDetect_Exit(TS_VOID *pHandle)
{
	TS_S32 s32Ret = TS_SUCCESS;

	if (!pHandle) {
		SAMPLE_PRT("AUDIO_ALG_CryDetect_Exit param is null\n");
		return TS_FAILURE;
	}

	SAMPLE_AUDIO_ALG_INSTANCE_S *pInst = (SAMPLE_AUDIO_ALG_INSTANCE_S *)pHandle;
	s32Ret = TS_ALG_CryDetect_Exit(pInst->pHandle);
	SAMPLE_PRT("TS_ALG_CryDetect_Exit.\n");

	return s32Ret;
}

#endif
