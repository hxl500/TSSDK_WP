#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#include "video_alg_catdetect.h"
#include "soft_line.h"
#include "tsalg_alg_lib.h"
#include "ts_alg_body_detect_v2.h"
#include "cJSON.h"
#include "arrr_diff.h"
#include "file_sync.h"

#define MAX_MODEL_PATH_LEN 128
#define MIN_COORD 0.0f
#define MAX_COORD 1.0f

static int is_food_on = 0;
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
#ifdef OVERLAY_RGN
static TS_OVERLAY_RGN_PARAM_S overlay_rgn_param[TS_RNG_HANDLE_BUTT];
#endif
#define TS_MAX_OVERLAY_NUM	20
//static int nArraySize[TS_MAX_OVERLAY_NUM] = {0};
//static TS_BITMAP_CHAT_S *bitmapArray[TS_MAX_OVERLAY_NUM] = {NULL};

//libs add
static ALG_CatDetect_DET_PARAM_S all_det_param;
#define CATVERSION "1.0.2"
TS_CHAR cat_version_buf[64] = {0};
TS_CHAR yolo_version_buf[64] = {0};
TS_CHAR emb_version_buf[64] = {0};
ALG_CAT_MODEL_INIT_S *model_info = NULL;

extern TS_VOID  VIDEO_ALG_CatDetect_ResultProc(TS_U8 *pYuvBuf,  TS_U32 width, TS_U32 height, TS_U32 u32ImageRatio, TS_VOID *pAlgResult);

#define CLAMP(x, min_val, max_val) ((x) < (min_val) ? (min_val) : ((x) > (max_val) ? (max_val) : (x)))

static int safe_strncpy(char *dst, const char *src, size_t dst_size)
{
	if (!dst || !src || dst_size == 0) {
		return -1;
	}
	size_t src_len = strlen(src);
	if (src_len >= dst_size) {
		ALG_LOGE("string too long: %zu >= %zu\n", src_len, dst_size);
		return -1;
	}
	memcpy(dst, src, src_len + 1);
	return 0;
}

TS_S32 TS_ALG_CatDetect_Init(TS_VOID **pHandle, ALG_CAT_MODEL_INIT_S *param)
{
	TS_S32 s32Ret = TS_SUCCESS;
	ALG_MODEL_INIT_S alg_param;
	ALG_CAT_MODEL_INIT_S *mode_param = param;
	FILE *pf_cfg = NULL;
	FILE *pf_weight = NULL;
	TS_U8 *pWeightBuf = NULL;
	TS_U8 *pCfgBuf = NULL;
	SAMPLE_ALG_INSTANCE_S* pInst = NULL;

	if (!pHandle || !param) {
		ALG_LOGE("TS_ALG_CatDetect_Init param is null\n");
		return TS_FAILURE;
	}

	if (!model_info) {
		model_info = malloc(sizeof(ALG_CAT_MODEL_INIT_S));
		if (!model_info) {
			ALG_LOGE("malloc model_info failed\n");
			return TS_FAILURE;
		}
		memset(model_info, 0, sizeof(ALG_CAT_MODEL_INIT_S));

		model_info->embedding_model_cfg = malloc(MAX_MODEL_PATH_LEN);
		model_info->embedding_model_weight = malloc(MAX_MODEL_PATH_LEN);
		model_info->food_model_cfg = malloc(MAX_MODEL_PATH_LEN);
		model_info->food_model_weight = malloc(MAX_MODEL_PATH_LEN);
		model_info->yolo_model_cfg = malloc(MAX_MODEL_PATH_LEN);
		model_info->yolo_model_weight = malloc(MAX_MODEL_PATH_LEN);
		model_info->model_version_file = malloc(MAX_MODEL_PATH_LEN);

		if (!model_info->embedding_model_cfg || !model_info->embedding_model_weight ||
		    !model_info->food_model_cfg || !model_info->food_model_weight ||
		    !model_info->yolo_model_cfg || !model_info->yolo_model_weight ||
		    !model_info->model_version_file) {
			ALG_LOGE("malloc model path buffers failed\n");
			if (model_info->embedding_model_cfg) free(model_info->embedding_model_cfg);
			if (model_info->embedding_model_weight) free(model_info->embedding_model_weight);
			if (model_info->food_model_cfg) free(model_info->food_model_cfg);
			if (model_info->food_model_weight) free(model_info->food_model_weight);
			if (model_info->yolo_model_cfg) free(model_info->yolo_model_cfg);
			if (model_info->yolo_model_weight) free(model_info->yolo_model_weight);
			if (model_info->model_version_file) free(model_info->model_version_file);
			free(model_info);
			model_info = NULL;
			return TS_FAILURE;
		}
	}

	memset(model_info->embedding_model_cfg, 0, MAX_MODEL_PATH_LEN);
	memset(model_info->embedding_model_weight, 0, MAX_MODEL_PATH_LEN);
	memset(model_info->food_model_cfg, 0, MAX_MODEL_PATH_LEN);
	memset(model_info->food_model_weight, 0, MAX_MODEL_PATH_LEN);
	memset(model_info->yolo_model_cfg, 0, MAX_MODEL_PATH_LEN);
	memset(model_info->yolo_model_weight, 0, MAX_MODEL_PATH_LEN);
	memset(model_info->model_version_file, 0, MAX_MODEL_PATH_LEN);

	if (mode_param->food_model_cfg != NULL && mode_param->food_model_weight != NULL) {
		if (safe_strncpy((char*)model_info->food_model_cfg, (char*)mode_param->food_model_cfg, MAX_MODEL_PATH_LEN) == 0 &&
		    safe_strncpy((char*)model_info->food_model_weight, (char*)mode_param->food_model_weight, MAX_MODEL_PATH_LEN) == 0) {
			is_food_on = 1;
		}
	}

	if (safe_strncpy((char*)model_info->embedding_model_cfg, (char*)mode_param->embedding_model_cfg, MAX_MODEL_PATH_LEN) != 0 ||
	    safe_strncpy((char*)model_info->embedding_model_weight, (char*)mode_param->embedding_model_weight, MAX_MODEL_PATH_LEN) != 0 ||
	    safe_strncpy((char*)model_info->yolo_model_cfg, (char*)mode_param->yolo_model_cfg, MAX_MODEL_PATH_LEN) != 0 ||
	    safe_strncpy((char*)model_info->yolo_model_weight, (char*)mode_param->yolo_model_weight, MAX_MODEL_PATH_LEN) != 0 ||
	    safe_strncpy((char*)model_info->model_version_file, (char*)mode_param->model_version_file, MAX_MODEL_PATH_LEN) != 0) {
		s32Ret = TS_FAILURE;
		goto init_exit;
	}

	model_info->cat_callback = mode_param->cat_callback;
	model_info->userdata = mode_param->userdata;

	TS_U8 *pu8_model_cfg = model_info->yolo_model_cfg;
	TS_U8 *pu8_model_weight = model_info->yolo_model_weight;

	SAMPLE_PRT("pu8_model_cfg %s !!!\n", pu8_model_cfg);
	SAMPLE_PRT("pu8_model_weight %s !!!\n", pu8_model_weight);

	pf_cfg = fopen((const char*)pu8_model_cfg, "rb");
	pf_weight = fopen((const char*)pu8_model_weight, "rb");

	if ((NULL == pf_cfg) || (NULL == pf_weight)) {
		SAMPLE_PRT("cat model file fopen failed !!!\n");
		SAMPLE_PRT("model_cfg %s, model_weight:%s\n", pu8_model_cfg, pu8_model_weight);
		s32Ret = TS_FAILURE;
		goto init_exit;
	}

	TS_U32 u32WeightLen = getFileSize(pf_weight);
	if (u32WeightLen == 0) {
		ALG_LOGE("weight file is empty\n");
		s32Ret = TS_FAILURE;
		goto init_exit;
	}

	pWeightBuf = (TS_U8 *)malloc(u32WeightLen);
	if (!pWeightBuf) {
		ALG_LOGE("malloc pWeightBuf failed\n");
		s32Ret = TS_FAILURE;
		goto init_exit;
	}

	size_t read_count = fread(pWeightBuf, u32WeightLen, 1, pf_weight);
	if (read_count != 1) {
		ALG_LOGE("fread weight failed\n");
		s32Ret = TS_FAILURE;
		goto init_exit;
	}
	fclose(pf_weight);
	pf_weight = NULL;

	TS_U32 u32CfgLen = getFileSize(pf_cfg);
	if (u32CfgLen == 0) {
		ALG_LOGE("cfg file is empty\n");
		s32Ret = TS_FAILURE;
		goto init_exit;
	}

	pCfgBuf = (TS_U8 *)malloc(u32CfgLen);
	if (!pCfgBuf) {
		ALG_LOGE("malloc pCfgBuf failed\n");
		s32Ret = TS_FAILURE;
		goto init_exit;
	}

	read_count = fread(pCfgBuf, u32CfgLen, 1, pf_cfg);
	if (read_count != 1) {
		ALG_LOGE("fread cfg failed\n");
		s32Ret = TS_FAILURE;
		goto init_exit;
	}
	fclose(pf_cfg);
	pf_cfg = NULL;

	memset(&alg_param, 0, sizeof(ALG_MODEL_INIT_S));
	alg_param.pGraph = pCfgBuf;
	alg_param.pWeight = pWeightBuf;
	alg_param.u32GraphSize = u32CfgLen;
	alg_param.u32WeightSize = u32WeightLen;
	alg_param.eImageType = ALG_IMAGE_TYPE_INT_HWC_RGB0;
	alg_param.bRneOff = 0;

	// for (int i = 0; i < MAX_CAM_NUM; i++) {
	// 	shake_window_init(&g_shake_windows[i]);
	// 	shake_window_init(&g_eat_shake_windows[i]);
	// }

	pInst = (SAMPLE_ALG_INSTANCE_S*)malloc(sizeof(SAMPLE_ALG_INSTANCE_S));
	if (!pInst) {
		ALG_LOGE("malloc SAMPLE_ALG_INSTANCE_S failed\n");
		s32Ret = TS_FAILURE;
		goto init_exit;
	}
	memset(pInst, 0, sizeof(SAMPLE_ALG_INSTANCE_S));

	SAMPLE_PRT("TS_ALG_BodyDetect_Init begin\n");
	s32Ret = TS_ALG_BodyDetect_Init(&(pInst->pHandle), &alg_param);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_BodyDetect_Init error\n");
		free(pInst);
		pInst = NULL;
		goto init_exit;
	}

	*pHandle = pInst;

	SAMPLE_PRT("TS_ALG_BodyDetect_GetVersion:%s\n", TS_ALG_BodyDetect_GetVersion());
	s32Ret = TS_SUCCESS;

	if (is_food_on) {
	}
	SAMPLE_PRT("ALG_RSN_Detect_Init sucess!\n");

init_exit:
	if (pf_cfg) fclose(pf_cfg);
	if (pf_weight) fclose(pf_weight);
	if (pCfgBuf) free(pCfgBuf);
	if (pWeightBuf) free(pWeightBuf);
	return s32Ret;
}
extern void set_result(ALG_CatDetect_DET_RESULT_S *data);
// TS_S32 VIDEO_ALG_CatDetect_Proc(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_IMAGE_S *pImageDet, ALG_CatDetect_DET_RESULT_S *pResult, TS_U8 cam_id)
// {
// 	TS_S32 s32Ret = TS_SUCCESS;
	

// 	SAMPLE_ALG_INSTANCE_S* pInst = (SAMPLE_ALG_INSTANCE_S*)pHandle;
// 	if (!pHandle|| !pImageDet || !pResult) {
// 		ALG_LOGE("VIDEO_ALG_CatDetect_Proc param is null\n");
// 		return TS_FAILURE;
// 	}

// 	ALG_CatDetect_DET_RESULT_S *pTmpResult = (ALG_CatDetect_DET_RESULT_S *)pResult;
// 	s32Ret = TS_ALG_BodyDetect_Process(pInst->pHandle, pImageDet, pTmpResult);
// 	if (0 != s32Ret) {
// 		ALG_LOGE("TS_ALG_BodyDetect_Process error\n");
// 	}

// 	RECT rect;
// 	const float img_w = (float)pImage->s32W;
// 	const float img_h = (float)pImage->s32H;

// 	for (int i = 0; i < pTmpResult->u32ObjNum; i++)
// 	{
// 		if(pTmpResult->stBox[i].f32Xmin > 1.0){
// 			pTmpResult->stBox[i].f32Xmin = 1.0;
// 		}else if(pTmpResult->stBox[i].f32Xmin < 0){
// 			pTmpResult->stBox[i].f32Xmin = 0;
// 		}
// 		if(pTmpResult->stBox[i].f32Ymin > 1.0){
// 			pTmpResult->stBox[i].f32Ymin = 1.0;
// 		}else if(pTmpResult->stBox[i].f32Ymin < 0){
// 			pTmpResult->stBox[i].f32Ymin = 0;
// 		}
// 		if(pTmpResult->stBox[i].f32Xmax > 1.0){
// 			pTmpResult->stBox[i].f32Xmax = 1.0;
// 		}else if(pTmpResult->stBox[i].f32Xmax < 0){
// 			pTmpResult->stBox[i].f32Xmax = 0;
// 		}
// 		if(pTmpResult->stBox[i].f32Ymax > 1.0){
// 			pTmpResult->stBox[i].f32Ymax = 1.0;
// 		}else if(pTmpResult->stBox[i].f32Ymax < 0){
// 			pTmpResult->stBox[i].f32Ymax = 0;
// 		}

// 		rect.left = pTmpResult->stBox[i].f32Xmin * img_w;
// 		rect.top = pTmpResult->stBox[i].f32Ymin * img_h;
// 		rect.right = pTmpResult->stBox[i].f32Xmax * img_w;
// 		rect.bottom = pTmpResult->stBox[i].f32Ymax * img_h;
// 		pTmpResult->stBox[i].cam_id = cam_id;
// 	}

// 	set_result(pTmpResult);

// 	return s32Ret;
// }
TS_S32 VIDEO_ALG_CatDetect_Proc(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_IMAGE_S *pImageDet, ALG_CatDetect_DET_RESULT_S *pResult, TS_U8 cam_id)
{
    TS_S32 s32Ret = TS_SUCCESS;

    SAMPLE_ALG_INSTANCE_S *pInst = (SAMPLE_ALG_INSTANCE_S *)pHandle;
    if (!pHandle || !pImageDet || !pResult) {
        ALG_LOGE("VIDEO_ALG_CatDetect_Proc param is null\\n");
        return TS_FAILURE;
    }

    memset(pResult, 0, sizeof(ALG_CatDetect_DET_RESULT_S));
    ALG_CatDetect_DET_RESULT_S *pTmpResult = pResult;

    s32Ret = TS_ALG_BodyDetect_Process(pInst->pHandle, pImageDet, pTmpResult);
    if (s32Ret != 0) {
        ALG_LOGE("TS_ALG_BodyDetect_Process error\\n");
        pTmpResult->u32ObjNum = 0;
        return s32Ret;
    }

    if (pTmpResult->u32ObjNum > MAX_CAT_DET_NUM || pTmpResult->u32ObjNum < 0) {
        ALG_LOGE("invalid u32ObjNum: %d, clear\\n", pTmpResult->u32ObjNum);
        pTmpResult->u32ObjNum = 0;
        return TS_FAILURE;
    }

    RECT rect;
    const float img_w = (float)pImage->s32W;
    const float img_h = (float)pImage->s32H;

    for (int i = 0; i < pTmpResult->u32ObjNum; i++) {
        if (pTmpResult->stBox[i].f32Xmin < 0.0f) pTmpResult->stBox[i].f32Xmin = 0.0f;
        if (pTmpResult->stBox[i].f32Xmin > 1.0f) pTmpResult->stBox[i].f32Xmin = 1.0f;

        if (pTmpResult->stBox[i].f32Ymin < 0.0f) pTmpResult->stBox[i].f32Ymin = 0.0f;
        if (pTmpResult->stBox[i].f32Ymin > 1.0f) pTmpResult->stBox[i].f32Ymin = 1.0f;

        if (pTmpResult->stBox[i].f32Xmax < 0.0f) pTmpResult->stBox[i].f32Xmax = 0.0f;
        if (pTmpResult->stBox[i].f32Xmax > 1.0f) pTmpResult->stBox[i].f32Xmax = 1.0f;

        if (pTmpResult->stBox[i].f32Ymax < 0.0f) pTmpResult->stBox[i].f32Ymax = 0.0f;
        if (pTmpResult->stBox[i].f32Ymax > 1.0f) pTmpResult->stBox[i].f32Ymax = 1.0f;

        rect.left = pTmpResult->stBox[i].f32Xmin * img_w;
        rect.top = pTmpResult->stBox[i].f32Ymin * img_h;
        rect.right = pTmpResult->stBox[i].f32Xmax * img_w;
        rect.bottom = pTmpResult->stBox[i].f32Ymax * img_h;

        pTmpResult->stBox[i].cam_id = cam_id;
    }

    set_result(pTmpResult);
    return s32Ret;
}


void overlay_bitmap_on_nv12(Uint8 *nv12_y, int width, int height, TS_BITMAP_CHAT_S *bitmapArray, int arraySize, int posX, int posY)
{
	if (NULL == bitmapArray){
		printf("bitmapArray is null!\n");
		return;
	}
	if (0== arraySize){
		printf("arraySize is 0!\n");
		return;
	}
    for (int i = 0; i < arraySize; i++) {
        int x = posX + bitmapArray[i].x;
        int y = posY + bitmapArray[i].y;
        if (x >= 0 && x < width && y >= 0 && y < height) {
            Uint8 *pixelY = nv12_y + y * width + x;
            Uint8 alpha = bitmapArray[i].alpha;
            *pixelY = (*pixelY * (255 - alpha) + 255 * alpha) / 255; // 混合Y平面亮度
        }
    }
}

void overlay_with_shadow_bitmap_on_nv12(Uint8 *nv12_y, int width, int height, TS_BITMAP_CHAT_S *bitmapArray, int arraySize, int posX, int posY)
{
    int shadow_offset = 2;			// offset
    Uint8 shadow_intensity = 50;	// 阴影亮度较低

    // shadow
    for (int i = 0; i < arraySize; i++) {
        int x = posX + bitmapArray[i].x + shadow_offset;
        int y = posY + bitmapArray[i].y + shadow_offset;
        if (x >= 0 && x < width && y >= 0 && y < height) {
            Uint8 *pixelY = nv12_y + y * width + x;
            Uint8 alpha = bitmapArray[i].alpha;
            *pixelY = (*pixelY * (255 - alpha) + shadow_intensity * alpha) / 255;
        }
    }

    // text
    for (int i = 0; i < arraySize; i++) {
        int x = posX + bitmapArray[i].x;
        int y = posY + bitmapArray[i].y;
        if (x >= 0 && x < width && y >= 0 && y < height) {
            Uint8 *pixelY = nv12_y + y * width + x;
            Uint8 alpha = bitmapArray[i].alpha;
            *pixelY = (*pixelY * (255 - alpha) + 255 * alpha) / 255;
        }
    }
}

TS_S32 VIDEO_ALG_CatDetect_Exit(TS_VOID *pHandle)
{
	TS_S32 s32Ret = TS_SUCCESS;
	if (!pHandle) {
		SAMPLE_PRT("VIDEO_ALG_CatDetect_Exit param is null\n");
		s32Ret = TS_FAILURE;
		return s32Ret;
	}

	SAMPLE_ALG_INSTANCE_S* pInst = (SAMPLE_ALG_INSTANCE_S*)pHandle;
	if (pInst->pHandle) {
		s32Ret = TS_ALG_BodyDetect_Exit(pInst->pHandle);
	}

	free(pInst);

	if (model_info) {
		if (model_info->embedding_model_cfg) {
			free(model_info->embedding_model_cfg);
		}
		if (model_info->embedding_model_weight) {
			free(model_info->embedding_model_weight);
		}
		if (model_info->food_model_cfg) {
			free(model_info->food_model_cfg);
		}
		if (model_info->food_model_weight) {
			free(model_info->food_model_weight);
		}
		if (model_info->yolo_model_cfg) {
			free(model_info->yolo_model_cfg);
		}
		if (model_info->yolo_model_weight) {
			free(model_info->yolo_model_weight);
		}
		if (model_info->model_version_file) {
			free(model_info->model_version_file);
		}
		free(model_info);
		model_info = NULL;
	}

	return s32Ret;
}


TS_S32 TS_ALG_CatDetect_SetParam(ALG_CatDetect_DET_PARAM_S *pParam)
{
	TS_S32 s32Ret = TS_SUCCESS;
	if(!pParam)
	{
		SAMPLE_PRT("ALG_CatDetect_DET_PARAM_S param is null\n");
		return TS_FAILURE;
	}
	ALG_CatDetect_DET_PARAM_S *det_param = pParam;
	if(det_param->DetectionConfThres<0||det_param->DetectionConfThres>1)
	{
		SAMPLE_PRT("DetectionConfThres is overload Please set 0.0 - 1.0\n");
		return TS_FAILURE;
	}
	if(det_param->SimilarityThres_Day<0||det_param->SimilarityThres_Day>1)
	{
		SAMPLE_PRT("SimilarityThres_Day is overload Please set 0.0 - 1.0\n");
		return TS_FAILURE;
	}
	if(det_param->SimilarityThres_Night<0||det_param->SimilarityThres_Night>1)
	{
		SAMPLE_PRT("SimilarityThres_Night is overload Please set 0.0 - 1.0\n");
		return TS_FAILURE;
	}
	all_det_param.DetectionConfThres = det_param->DetectionConfThres;
	all_det_param.SimilarityThres_Day = det_param->SimilarityThres_Day;
	all_det_param.SimilarityThres_Night = det_param->SimilarityThres_Night;
	set_eta_thres(det_param->EAT_Thres);
	set_out_times(det_param->OUT_times);
	set_eat_out_times(det_param->EAT_OUT_times);
//	set_eat_shake_out_times(det_param->EAT_OUT_SHAKE_times);

	return s32Ret;
}

TS_S32 TS_ALG_CatDetect_GetParam(ALG_CatDetect_DET_PARAM_S *pParam)
{
	TS_S32 s32Ret = TS_SUCCESS;
	if(!pParam)
	{
		SAMPLE_PRT("TS_ALG_CatDetect_GetParam param is null\n");
		return TS_FAILURE;
	}
	ALG_CatDetect_DET_PARAM_S *det_param = pParam;
	det_param->DetectionConfThres = all_det_param.DetectionConfThres;
	det_param->SimilarityThres_Day = all_det_param.SimilarityThres_Day;
	det_param->SimilarityThres_Night = all_det_param.SimilarityThres_Night;
	
	return s32Ret;
}

TS_CHAR* CatLibVerGet(void)
{
	//SAMPLE_PRT("VER %s %s %s\n",__DATE__,__TIME__,CATVERSION);
	sprintf(cat_version_buf,"%s%s%s",__DATE__,__TIME__,CATVERSION);
	return cat_version_buf;
}

TS_CHAR* CatYoloModelVerGet(void)
{
	sprintf(yolo_version_buf,"%s%s%s",__DATE__,__TIME__,CATVERSION);
	return yolo_version_buf;
}

TS_CHAR* CatEmbeddingModelVerGet(void)
{
	sprintf(emb_version_buf,"%s%s%s",__DATE__,__TIME__,CATVERSION);
	return emb_version_buf;
}

char* extract_filepath(const char* full_path) {
	char* last_slash = strrchr(full_path, '/');
	if (last_slash == NULL) return strdup("");

	size_t path_len = last_slash - full_path + 1;
	char* path = malloc(path_len + 1);
	if (!path) return NULL;

	strncpy(path, full_path, path_len);
	path[path_len] = '\0';
	return path;
}

TS_S32 CatConfigRenew(TS_CHAR*url,TS_CHAR*sn)
{
	pic_renew(url,sn);
	return 0;
}

TS_S32 CatSetPicDir(TS_CHAR*path)
{
	SAMPLE_PRT("CatSetPicDir\n");
	set_pic_path(path);
	return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
