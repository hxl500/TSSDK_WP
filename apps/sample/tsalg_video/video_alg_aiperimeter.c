


#include "video_alg_aiperimeter.h"
#include "video_common_utils.h"
#include "soft_line.h"
#include "ts_rne_device.h"
#include "ts_alg_gp_layers.h"

#if ALG_AIPERIMETER

static char sPtsNum[2][10] = {0}; 

static RECT s_ai_perimeter_rect_line; // 基线
static RECT s_ai_perimeter_rect_out;  //感兴趣区域
static RECT s_ai_perimeter_rect_in;  //感兴趣区域

#define PTS_WIDTH  1920
#define PTS_HIGHT  1080

static TS_VOID Draw_PtsNum_AI(TS_U8 *YuvBuf,  TS_U32 width, TS_U32 height, int Color, int num, ALG_INT_POINT_S* basePos, int base_i)
{
	
	int i = num;
	int step = 16;
	int sum_index = 0;
	if(i == 0){
		YUY_Draw_num(YuvBuf, width, height, 12, basePos->s32X+ i*step, basePos->s32Y, 0, Color);
		return;
	}

	while (i != 0) {
		sPtsNum[base_i][sum_index] = i % 10;
		sum_index++;
		i = i / 10;
	}
	
	for (int j=0, i = sum_index; i > 0; i--, j++) {	
		YUY_Draw_num(YuvBuf, width, height, 12, basePos->s32X+ j*step, basePos->s32Y, sPtsNum[base_i][i-1], Color);
	}
	
}

TS_S32 VIDEO_ALG_AIPerimeter_Init(TS_VOID **handle)
{
	TS_S32 s32Ret = TS_SUCCESS;
	TS_ALG_SetLogLevel(ALG_LOG_INFO);
    SAMPLE_ALG_INSTANCE_S* pInst = *((SAMPLE_ALG_INSTANCE_S**)handle);
	
	FILE *pf_cfg = NULL;
	FILE *pf_weight = NULL;
 
    TS_U8 weightpath[] = "./pbcedetect_weight.weight";
	TS_U8 cfgpath[] = "./pbcedetect_cfg.cfg";

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
		ALG_LOGE("pbce model file fopen failed !!!\n");
        ALG_LOGE("model_cfg: %s, model_weight:%s\n", cfg_file,  weight_file);
		return TS_FAILURE;
	}

	TS_U32 u32WeightLen = SAMPLE_ALGO_GetFileSize(pf_weight);
    TS_U8 *pWeightBuf = (TS_U8 *)malloc(u32WeightLen);
	fread(pWeightBuf, u32WeightLen, 1, pf_weight);
	fclose(pf_weight);

	TS_U32 u32CfgLen = SAMPLE_ALGO_GetFileSize(pf_cfg);
	TS_U8 *pCfgBuf = (TS_U8 *)malloc(u32CfgLen);
	fread(pCfgBuf, u32CfgLen, 1, pf_cfg);
	fclose(pf_cfg);

	ALG_AI_PERIMETER_INIT_S param;
	memset(&param, 0, sizeof(ALG_AI_PERIMETER_INIT_S));	//这个地方必须要初始化

	// 加载人形检测模型
	param.stBodyDet.pGraph = pCfgBuf;
	param.stBodyDet.pWeight = pWeightBuf;
	param.stBodyDet.u32GraphSize = u32CfgLen;
	param.stBodyDet.u32WeightSize = u32WeightLen;
	param.stBodyDet.vpSelfBuf = NULL;
	param.stBodyDet.eImageType = ALG_IMAGE_TYPE_INT_HWC_RGB0;
	param.stBodyDet.bRneOff = 1; //open/close rne device in solution init/release

	// 加载PTS
	// 设置基线
	param.stPts.baseLine[0] = 10;
	param.stPts.baseLine[1] = 500;
	param.stPts.baseLine[2] = 1900;
	param.stPts.baseLine[3] = 500;
	// 设置感兴趣区域
	param.stPts.focuseRect[0] = 10;
	param.stPts.focuseRect[1] = 10;
	param.stPts.focuseRect[2] = 1900;
	param.stPts.focuseRect[3] = 1070;

	// 设置模式 区域入侵 越界侦测 围栏
	param.bRegionalInstrusion = 1;
	param.bCrossBorderDetection = 0;
	param.bEnclosure = 0;

	param.roiRect[0] = 300;
	param.roiRect[1] = 200;
	param.roiRect[2] = 900;
	param.roiRect[3] = 600;

	s_ai_perimeter_rect_line.left = param.stPts.baseLine[0];
	s_ai_perimeter_rect_line.top = param.stPts.baseLine[1];
	s_ai_perimeter_rect_line.right = param.stPts.baseLine[2];
	s_ai_perimeter_rect_line.bottom = param.stPts.baseLine[3];

	s_ai_perimeter_rect_out.left = param.stPts.focuseRect[0];
	s_ai_perimeter_rect_out.top = param.stPts.focuseRect[1];
	s_ai_perimeter_rect_out.right = param.stPts.focuseRect[2];
	s_ai_perimeter_rect_out.bottom = param.stPts.focuseRect[3];

	s_ai_perimeter_rect_in.left = param.roiRect[0];
	s_ai_perimeter_rect_in.top = param.roiRect[1];
	s_ai_perimeter_rect_in.right = param.roiRect[0] + param.roiRect[2];
	s_ai_perimeter_rect_in.bottom = param.roiRect[1] + param.roiRect[3];

	param.bRneOff = 0;

	s32Ret = TS_ALG_AiPerimeter_Init(&(pInst->pHandle), &param);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_AiPerimeter_Init error\n");
		goto init_exit;
	}

	SAMPLE_PRT("TS_ALG_AiPerimeter_GetVersion: %s\n", TS_ALG_AiPerimeter_GetVersion());
    s32Ret = TS_SUCCESS;
    pInst->pf_Exit = VIDEO_ALG_AIPerimeter_Exit;
    pInst->pf_Result_Process = VIDEO_ALG_AIPerimeter_ResultProc;
    pInst->pf_Process = VIDEO_ALG_AIPerimeter_Proc;

init_exit:	
	free(pCfgBuf);
	free(pWeightBuf);

    return s32Ret;
}

TS_S32 VIDEO_ALG_AIPerimeter_Proc(TS_VOID* pHandle, ALG_IMAGE_S *pImageOri, ALG_IMAGE_S *pImageDet, TS_VOID *pResult)
{
	TS_S32 s32Ret = TS_SUCCESS;
	SAMPLE_ALG_INSTANCE_S* pInst = (SAMPLE_ALG_INSTANCE_S*)pHandle;
	if (!pHandle|| !pImageDet || !pResult) {
		ALG_LOGE("VIDEO_ALG_AIPerimeter_Proc param is null\n");
		return TS_FAILURE;
	}

	SAMPLE_ALG_RESULT_S *pCurResult = (SAMPLE_ALG_RESULT_S *)pResult;
    ALG_AI_PERIMETER_RESULTS_S *pTmpResult = &(pCurResult->gstAlgAiPerimeterResult);
	s32Ret = TS_ALG_AiPerimeter_Process(pInst->pHandle, pImageOri, pImageDet, pTmpResult);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_AiPerimeter_Process error\n");
	}
    
    return TS_SUCCESS;
}

TS_VOID  VIDEO_ALG_AIPerimeter_ResultProc(TS_U8 *YuvBuf,  TS_U32 width, TS_U32 height, TS_U32 ImageRatio, TS_VOID *pAlg_result)
{

	RECT rect;
	SAMPLE_ALG_RESULT_S *pCurResult = (SAMPLE_ALG_RESULT_S *)pAlg_result;
	ALG_AI_PERIMETER_RESULTS_S *pResult = &(pCurResult->gstAlgAiPerimeterResult);
	
	for (int i = 0; i < pResult->alarm.u32TrackNum; i++)
	{
		rect.left = pResult->alarm.stBox[i].u32X;
		rect.top = pResult->alarm.stBox[i].u32Y;
		rect.right = pResult->alarm.stBox[i].u32X + pResult->alarm.stBox[i].u32Width;
		rect.bottom = pResult->alarm.stBox[i].u32Y + pResult->alarm.stBox[i].u32Height;
		YUV_Draw_Rect(YuvBuf, width, height, 12, &rect, YUV_RED, 5);
	}
	
	// 画基线和感兴趣区域
	YUY_Draw_Line(YuvBuf, width, height, 12, s_ai_perimeter_rect_line.left, 
			s_ai_perimeter_rect_line.top, s_ai_perimeter_rect_line.right, 
			s_ai_perimeter_rect_line.bottom, YUV_RED, 10);
	YUV_Draw_Rect(YuvBuf, width, height, 12, &s_ai_perimeter_rect_out, YUV_BLUE, 5);
	YUV_Draw_Rect(YuvBuf, width, height, 12, &s_ai_perimeter_rect_in, YUV_WHITE, 5);

	ALG_INT_POINT_S PosIn;
	PosIn.s32X = 100;
	PosIn.s32Y = 20;
	ALG_INT_POINT_S PosOut;
	PosOut.s32X = width/2 + 200;
	PosOut.s32Y = PosIn.s32Y;	
	//printf("in[x,y]:[%d,%d], out[x,y]:[%d,%d]\n", PosIn.s32X, PosIn.s32Y, PosOut.s32X, PosOut.s32Y);

	Draw_PtsNum_AI(YuvBuf,   width,  height, YUV_GREEN, pResult->stPts.u32InNum, &PosIn, 0);
	Draw_PtsNum_AI(YuvBuf,   width,  height, YUV_GREEN, pResult->stPts.u32OutNum, &PosOut, 1);
}

TS_S32 VIDEO_ALG_AIPerimeter_Exit(TS_VOID *handle)
{
    TS_S32 s32Ret = TS_SUCCESS;

    if (!handle){
		SAMPLE_PRT("VIDEO_ALG_AIPerimeter_Exit param is null\n");
		return TS_FAILURE;
	}

    SAMPLE_ALG_INSTANCE_S* pInst = (SAMPLE_ALG_INSTANCE_S*)handle;
	s32Ret = TS_ALG_AiPerimeter_Exit(pInst->pHandle);
	if (0 != s32Ret) {
        ALG_LOGE("TS_ALG_AiPerimeter_Exit error\n");
    }
    return s32Ret;
}

#endif

