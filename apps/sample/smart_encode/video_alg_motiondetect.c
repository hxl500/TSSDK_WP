#include "video_alg_motiondetect.h"
#include "soft_line.h"

#if ALG_MOTIONDETECT

#define ALG_MOTION_DETECT_IMAGE_W	640
#define ALG_MOTION_DETECT_IMAGE_H	360
#define ALG_MOTION_DETECT_IMG_C  	4
TS_S32 VIDEO_ALG_Motion_Init(TS_VOID **pHandle)
{
   	TS_S32 s32Ret = TS_SUCCESS;
    ALG_MOTION_DETECT_INIT_S param1;
    SAMPLE_ALG_INSTANCE_S* pInst = *((SAMPLE_ALG_INSTANCE_S**)pHandle);

    memset(&param1, 0, sizeof(ALG_MOTION_DETECT_INIT_S));
    param1.u32ImgW = ALG_MOTION_DETECT_IMAGE_W;
    param1.u32ImgH = ALG_MOTION_DETECT_IMAGE_H + 24;
    param1.u32ImgC = ALG_MOTION_DETECT_IMG_C;
    TS_ALG_SetLogLevel(ALG_LOG_ERROR);
    SAMPLE_PRT("motionDetect version:%s\n", TS_ALG_MotionDetect_GetVersion());
    s32Ret = TS_ALG_MotionDetect_Init(&(pInst->pHandle), &param1);
    if (0 != s32Ret) {
        SAMPLE_PRT("TS_ALG_MotionDetectInit error\n");
    }

    s32Ret = TS_SUCCESS;
    pInst->pf_Exit = VIDEO_ALG_Motion_Exit;
    pInst->pf_Result_Process = VIDEO_ALG_Motion_ResultProc;
    pInst->pf_Process = VIDEO_ALG_Motion_Proc;

    return s32Ret;
}

extern TS_S32 g_smart_encode_type;

TS_S32 VIDEO_ALG_Motion_Proc(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_IMAGE_S *pImageDet, TS_VOID *pResult)
{
	TS_S32 s32Ret = TS_SUCCESS;

    SAMPLE_ALG_INSTANCE_S* pInst = (SAMPLE_ALG_INSTANCE_S*)pHandle;
	if (!pHandle|| !pImageDet || !pResult) {
		SAMPLE_PRT("VIDEO_ALG_MotionProc param is null\n");
		return TS_FAILURE;
	}

    SAMPLE_ALG_RESULT_S *pCurResult = (SAMPLE_ALG_RESULT_S *)pResult;
	ALG_MOTION_DETECT_RESULT_S *ptmp_result = &(pCurResult->gstAlgMotionResult);
	s32Ret = TS_ALG_MotionDetect_Process(pInst->pHandle, pImageDet, ptmp_result);
	if (0 != s32Ret) {
		SAMPLE_PRT("TS_ALG_MotionDetect_Process error\n");
	}
    //printf("VIDEO_ALG_MotionProc\n");

    // if(g_smart_encode_type && ptmp_result->u32ObjNum > 0){
    if (g_smart_encode_type) {  // 没有目标也要告知驱动
        ALG_MOTION_DETECT_RESULT_S result = *ptmp_result;
        if(TS_MPI_VENC_SetMotionDetectParam(0, (VENC_MOTION_DETECT_RESULT_S *)&result) != TS_SUCCESS){
            SAMPLE_PRT("TS_MPI_VENC_SetMotionDetectParam error\n");
        }
    }

    return s32Ret;
}

TS_VOID  VIDEO_ALG_Motion_ResultProc(TS_U8 *pYuvBuf,  TS_U32 width, TS_U32 height, TS_U32 u32ImageRatio, TS_VOID *pAlgResult)
{
    RECT rect;
    SAMPLE_ALG_RESULT_S *pCurResult = (SAMPLE_ALG_RESULT_S *)pAlgResult;
	ALG_MOTION_DETECT_RESULT_S *pmotion_result = &(pCurResult->gstAlgMotionResult);
	for (int i = 0; i < pmotion_result->u32ObjNum; i++)
	{
        rect.left = u32ImageRatio * pmotion_result->stBox[i].f32Xmin  * width;
        rect.top = u32ImageRatio * pmotion_result->stBox[i].f32Ymin *  height;
        rect.right = u32ImageRatio * pmotion_result->stBox[i].f32Xmax * width;
        rect.bottom = u32ImageRatio *pmotion_result->stBox[i].f32Ymax * height;
        YUV_Draw_Rect(pYuvBuf, width, height, 12, &rect, YUV_BLUE, 5);
	}
    //printf("VIDEO_ALG_MotionResultProc\n");
}


TS_S32 VIDEO_ALG_Motion_Exit(TS_VOID *pHandle)
{
    TS_S32 s32Ret = TS_SUCCESS;

    if (!pHandle) {
		SAMPLE_PRT("VIDEO_ALG_MotionExit param is null\n");
		return TS_FAILURE;
	}

    SAMPLE_ALG_INSTANCE_S* pInst = (SAMPLE_ALG_INSTANCE_S*)pHandle;
    s32Ret = TS_ALG_MotionDetect_Exit(pInst->pHandle);
    printf("TS_ALG_MotionDetect_Exit \n");

    return s32Ret;
}

#endif
