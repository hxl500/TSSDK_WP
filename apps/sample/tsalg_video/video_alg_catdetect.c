#include "video_alg_catdetect.h"
#include "video_alg_catdetect-api.h"
#include "soft_line.h"
#include <stdio.h>



TS_S32 VIDEO_ALG_CatDetect_Init(TS_VOID **pHandle)
{
	SAMPLE_ALG_INSTANCE_S *pInst = *((SAMPLE_ALG_INSTANCE_S **)pHandle);
	TS_S32 s32Ret = TS_SUCCESS;
	ALG_CAT_MODEL_INIT_S param;
	//param.yolo_model_cfg = (TS_U8 *)strdup("/mnt/sda0/model/yolov5_7_quantize_r.cfg");
	//param.yolo_model_weight = (TS_U8 *)strdup("/mnt/sda0/model/yolov5_7_quantize_r.weight");

	param.yolo_model_cfg = (TS_U8 *)strdup("/model/yolov5_7_quantize_r.cfg");
	param.yolo_model_weight = (TS_U8 *)strdup("/model/yolov5_7_quantize_r.weight");
	//param.yolo_model_cfg = (TS_U8 *)strdup("");
	//param.yolo_model_weight = (TS_U8 *)strdup("");
	
	//param.yolo_model_cfg = (TS_U8*)strdup("/model/yolov5.cfg");
	//param.yolo_model_weight = (TS_U8*)strdup("/model/yolov5.weight");
	param.embedding_model_cfg = (TS_U8 *)strdup("");	/// model/resnet.cfg
	param.embedding_model_weight = (TS_U8 *)strdup(""); /// model/resnet.weight
	// param.food_model_cfg = (TS_U8*)strdup("/tmp/a.cfg");
	// param.food_model_weight = (TS_U8*)strdup("/tmp/a.weight");
	param.food_model_cfg = NULL;
	param.food_model_weight = NULL;
	param.model_version_file = (TS_U8 *)strdup("");
	param.cat_callback = NULL;
	param.userdata = NULL;
	s32Ret = TS_ALG_CatDetect_Init(pHandle, &param);
	if (s32Ret < TS_SUCCESS)
	{
		return s32Ret;
	}
	ALG_CatDetect_DET_PARAM_S config;
	config.DetectionConfThres = 0.65;//0.65;
	config.SimilarityThres_Day = 0.6; // 0.825
	config.SimilarityThres_Night = 0.65;
	config.EAT_Thres = 0.25;
	config.OUT_times = 3;
	config.EAT_OUT_times = 10;
	TS_ALG_CatDetect_SetParam(&config);
	CatSetPicDir("/tmp");
	//	CatConfigRenew("http://veepai-dev.eye4.cn:31110/api/v1/pets/getBySn","VE5411849XJKW");
	CatConfigRenew("http://veepai-dev.eye4.cn:31110/api/v1/pets/getBySn", "OKB0505202RYRN");
	s32Ret = TS_SUCCESS;
	pInst->pf_Exit = VIDEO_ALG_CatDetect_Exit;
	pInst->pf_Result_Process = VIDEO_ALG_CatDetect_ResultProc;
	pInst->pf_Process = VIDEO_ALG_CatDetect_Proc_user;
	return s32Ret;
}

 
TS_S32 VIDEO_ALG_CatDetect_Proc_user(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_IMAGE_S *pImageDet, TS_VOID *pResult, TS_U8 cam_id)
{
	SAMPLE_ALG_RESULT_S *pCurResult = (SAMPLE_ALG_RESULT_S *)pResult;
	ALG_CatDetect_DET_RESULT_S *pTmpResult = &(pCurResult->gstAlgCatdetResult);
	 
	return VIDEO_ALG_CatDetect_Proc(pHandle, pImage, pImageDet, pTmpResult, cam_id);
	// TS_S32 VIDEO_ALG_CatDetect_Proc(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_IMAGE_S *pImageDet, ALG_CatDetect_DET_RESULT_S *pResult);
	//	return 0;
}



TS_VOID  VIDEO_ALG_CatDetect_ResultProc(TS_U8 *pYuvBuf,  TS_U32 width, TS_U32 height, TS_U32 u32ImageRatio, TS_VOID *pAlgResult)
{
        SAMPLE_ALG_RESULT_S *pCurResult = (SAMPLE_ALG_RESULT_S *)pAlgResult;
        ALG_CatDetect_DET_RESULT_S *pResult = &(pCurResult->gstAlgCatdetResult);
        RECT rect;
	int test_conf;
	for(TS_U32 i = 0; i < pResult->u32ObjNum; i++) {
		if(pResult->stBox[i].act != ALG_CAT_ACT_INT && pResult->stBox[i].act != ALG_CAT_ACT_EAT){
			continue;
		}
			rect.left = u32ImageRatio * pResult->stBox[i].f32Xmin  * width;
        	rect.top = u32ImageRatio * pResult->stBox[i].f32Ymin *  height;
        	rect.right = u32ImageRatio * pResult->stBox[i].f32Xmax * width;
        	rect.bottom = u32ImageRatio *pResult->stBox[i].f32Ymax * height;
		if( 0 == pResult->stBox[i].cam_id&&pResult->stBox[i].class_id == ALG_CAT_CLASS_ID_FOOD)
		{
			YUV_Draw_Rect(pYuvBuf, width, height, 12, &rect, YUV_YELLOW, 5);
		}
		else if( 0 == pResult->stBox[i].cam_id && pResult->stBox[i].class_id == ALG_CAT_CLASS_ID_FACE)
		{
			YUV_Draw_Rect(pYuvBuf, width, height, 12, &rect, YUV_GREEN, 5);
		}

// 		// YUY_Draw_num(pYuvBuf, width, height, 80,rect.right+2+10+48, rect.top+120, test_conf%10, YUV_BLUE);

// 		YUY_Draw_num(pYuvBuf, width, height, 80, rect.right + 30, rect.top, pResult->stBox[i].class_id, YUV_BLUE);

// 		test_conf = pResult->stBox[i].DetectionConf * 100;
// 		test_conf = test_conf % 100;
// 		YUY_Draw_num(pYuvBuf, width, height, 80, rect.right + 30, rect.top + 30, test_conf / 10, YUV_BLUE);
// 		YUY_Draw_num(pYuvBuf, width, height, 80, rect.right + 30 + 30, rect.top + 30, test_conf % 10, YUV_BLUE);

// 		test_conf = pResult->stBox[i].MaxSimilarity * 100;
// 		test_conf = test_conf % 100;
// 		YUY_Draw_num(pYuvBuf, width, height, 80, rect.right + 30, rect.top + 60, test_conf / 10, YUV_BLUE);
// 		YUY_Draw_num(pYuvBuf, width, height, 80, rect.right + 30 + 30, rect.top + 60, test_conf % 10, YUV_BLUE);
// #if 1
// 		int idlen;
// 		char ids[64];
// 		sprintf(ids, "%s", pResult->stBox[i].nameid);
// 		idlen = strlen(ids);
// 		int j;
// 		for (j = 0; j < idlen; j++)
// 		{
// 			YUY_Draw_num(pYuvBuf, width, height, 80, rect.right + 30 + j * 30, rect.top + 90, ids[j] - '0', YUV_BLUE);
// 		}
// #endif
// 		YUY_Draw_num(pYuvBuf, width, height, 80, rect.right + 30, rect.top + 120, pResult->stBox[i].act, YUV_BLUE);

// 		// printf("body:%d --score:%f xmin:%f ymin:%f xmax:%f ymax:%f\n", j, pResult->stBox[j].f32Score,
// 		// pResult->stBox[j].f32Xmin * imageox.s32W, pResult->stBox[j].f32Ymin * image.s32H,
// 		// pResult->stBox[j].f32Xmax * image.s32W, pResult->stBox[j].f32Ymax * image.s32H);
	}
	return;
}
