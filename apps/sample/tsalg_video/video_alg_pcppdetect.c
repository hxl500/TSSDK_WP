#include "video_alg_pcppdetect.h"
#include "soft_line.h"
#include "soft_alphabet_overlay.h"

#if ALG_PCPPDETECT

static TS_U32 getFileSize(FILE *pf)
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

TS_S32 VIDEO_ALG_MultiObj_Init(TS_VOID **pHandle)
{
	TS_S32 s32Ret = TS_SUCCESS;

	ALG_MODEL_INIT_S param;
	TS_ALG_SetLogLevel(ALG_LOG_ERROR);

    SAMPLE_ALG_INSTANCE_S* pInst = *((SAMPLE_ALG_INSTANCE_S**)pHandle);
    
    FILE *pf_cfg = NULL;
	FILE *pf_weight = NULL;

    TS_U8 weightpath[] = "./pcppdetect_weight.weight";
	TS_U8 cfgpath[] = "./pcppdetect_cfg.cfg";

	char cfg_file[SAMPLE_MODEL_FILE_LEN] = {0};
	char weight_file[SAMPLE_MODEL_FILE_LEN] = {0};
	if (TS_NULL == g_pcModelRoot){
		sprintf(cfg_file, "%s", cfgpath);
		sprintf(weight_file, "%s", weightpath);
	}else {
		sprintf(cfg_file, "%s/%s", g_pcModelRoot, cfgpath);
		sprintf(weight_file, "%s/%s", g_pcModelRoot, weightpath);
	}

    pf_cfg = fopen((const char *)cfg_file, "rb");
    pf_weight = fopen((const char *)weight_file, "rb");
	if ((NULL == pf_cfg) || (NULL == pf_weight)) {
		ALG_LOGE("pcpp model file fopen failed !!!\n");
        ALG_LOGE("model_cfg: %s, model_weight:%s\n", cfg_file,  weight_file);
		return TS_FAILURE;
	}

	TS_U32 u32WeightLen = getFileSize(pf_weight);
    TS_U8 *pWeightBuf = (TS_U8 *)malloc(u32WeightLen);
	fread(pWeightBuf, u32WeightLen, 1, pf_weight);
	fclose(pf_weight);

	TS_U32 u32CfgLen = getFileSize(pf_cfg);
	TS_U8 *pCfgBuf = (TS_U8 *)malloc(u32CfgLen);
	fread(pCfgBuf, u32CfgLen, 1, pf_cfg);
	fclose(pf_cfg);

    memset(&param, 0, sizeof(ALG_MODEL_INIT_S));
    param.pGraph = pCfgBuf;
	param.pWeight = pWeightBuf;
	param.u32GraphSize = u32CfgLen;
	param.u32WeightSize = u32WeightLen;
	param.eImageType = ALG_IMAGE_TYPE_INT_HWC_RGB0;
	param.bRneOff = 0;

	s32Ret = TS_ALG_PcppDet_Init(&(pInst->pHandle), &param);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_PcppDet_Init error\n");
		goto init_exit;
	}

	SAMPLE_PRT("TS_ALG_PcppDet_GetVersion:%s\n", TS_ALG_PcppDet_GetVersion());
    s32Ret = TS_SUCCESS;
    pInst->pf_Exit = VIDEO_ALG_MultiObj_Exit;
    pInst->pf_Result_Process = VIDEO_ALG_MultiObj_ResultProc;
    pInst->pf_Process = VIDEO_ALG_MultiObj_Proc;

init_exit:	
	free(pCfgBuf);
	free(pWeightBuf);

    return s32Ret;
}

TS_S32 VIDEO_ALG_MultiObj_Proc(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_IMAGE_S *pImageDet, TS_VOID *pResult)
{
	TS_S32 s32Ret = TS_SUCCESS;

    SAMPLE_ALG_INSTANCE_S* pInst = (SAMPLE_ALG_INSTANCE_S*)pHandle;
	if (!pHandle|| !pImageDet || !pResult) {
		ALG_LOGE("VIDEO_ALG_MultiObj_Proc param is null\n");
		return TS_FAILURE;
	}

	SAMPLE_ALG_RESULT_S *pCurResult = (SAMPLE_ALG_RESULT_S *)pResult;
    ALG_PCPP_DET_RESULT_S *pTmpResult = &(pCurResult->gstAlgPcppResult);
	s32Ret = TS_ALG_PcppDet_Process(pInst->pHandle, pImageDet, pTmpResult);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_PcppDetect_Process error\n");
	}

	//printf("VIDEO_ALG_MultiObjProc %d object\n", pTmpResult->u32ObjNum);

    return s32Ret;
}

TS_VOID  VIDEO_ALG_MultiObj_ResultProc(TS_U8 *pYuvBuf,  TS_U32 width, TS_U32 height, TS_U32 u32ImageRatio, TS_VOID *pAlgResult, TS_U32 u32Chn)
{
    RECT rect;
    TS_U32 i;
    float fThres = 0.0001f;
    enYuvColorIdx enColor = YUV_RED;

    SAMPLE_ALG_RESULT_S *pCurResult = (SAMPLE_ALG_RESULT_S *)pAlgResult;
	ALG_PCPP_DET_RESULT_S *pResult = &(pCurResult->gstAlgPcppResult);
	char *label;
	for (i = 0; i < pResult->u32ObjNum; i ++) {
	    if (pResult->stBox[i].f32Score > fThres) {
            if (pResult->stBox[i].f32Xmin < 0 || pResult->stBox[i].f32Xmax > 1
			   || pResult->stBox[i].f32Ymin < 0 || pResult->stBox[i].f32Ymax > 1) { 
                SAMPLE_PRT("pcppdetect get invalid box xy=[%f,%f][%f,%f]. skip this one\n",
				pResult->stBox[i].f32Xmin, pResult->stBox[i].f32Xmax,
				pResult->stBox[i].f32Ymin, pResult->stBox[i].f32Ymax);
			    continue;
		    }

		    switch (pResult->u8ObjNo[i]) {
				case 0:{
					//SAMPLE_ALGO_OBJ_TYPE_PEDESTRIAN;
					enColor = YUV_RED;
					label = "person";
					break;
				}
				case 1:{
					//SAMPLE_ALGO_OBJ_TYPE_CAR;
					enColor = YUV_YELLOW;
					label = "car";
					break;
				}
				case 2:{
					//SAMPLE_ALGO_OBJ_TYPE_PET;
					enColor = YUV_DARK_GREEN;
					label = "pet";
					break;
				}
				case 3:{
					//SAMPLE_ALGO_OBJ_TYPE_PACKAGE;
					enColor = YUV_PURPLE;
					label = "package";
					break;
				}
				default:{
					//pResult->type = SAMPLE_ALGO_OBJ_TYPE_PEDESTRIAN;
					label = "";
					break;
				}
			}

			rect.left = u32ImageRatio * pResult->stBox[i].f32Xmin  * width;
            rect.top = u32ImageRatio * pResult->stBox[i].f32Ymin *  height;
            rect.right = u32ImageRatio * pResult->stBox[i].f32Xmax * width;
            rect.bottom = u32ImageRatio *pResult->stBox[i].f32Ymax * height;
            YUV_Draw_Rect(pYuvBuf, width, height, 12, &rect, enColor, 5);
			overlay_letter(label, pYuvBuf, width, height, rect.left, rect.top);
		}		
	}

}


TS_S32 VIDEO_ALG_MultiObj_Exit(TS_VOID *pHandle)
{
	TS_S32 s32Ret = TS_SUCCESS;
	if (!pHandle) {
		SAMPLE_PRT("VIDEO_ALG_MultiObj_Exit param is null\n");
		return TS_FAILURE;
	}

	SAMPLE_ALG_INSTANCE_S* pInst = (SAMPLE_ALG_INSTANCE_S*)pHandle;
	s32Ret = TS_ALG_PcppDet_Exit(pInst->pHandle);
	return s32Ret;
}

#endif
