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
#define TEST_MODE1 0 //打开这个 则保存640码流
#define TEST_MODE2 0 //打开这个 则读取输入文件为视频流
#define TEST_MODE3 0 //打开这个 则不允许猫脸检测 只保存640码流
static int is_food_on = 0;

#if TEST_MODE1
#define MODE1_NAME_PRE "/media/mmcblk0p1/testfile/test_mode1/test_mode1_%d.rgb"
char mode1_name[128] = {0};
#define MODE1_NAME_PRE_YUV "/media/mmcblk0p1/testfile/test_mode1/test_mode1_%d.yuv"
char mode1_name_yuv[128] = {0};

#endif

#if TEST_MODE2
char mode2_name[128] = {0};
#define MODE2_NAME_PRE "/media/mmcblk0p1/testfile/test_mode2/input_mode2.rgba"
char mode2_name_yuv[128] = {0};
#define MODE2_NAME_PRE_YUV "/media/mmcblk0p1/testfile/test_mode2/input_mode2.yuv"

#endif
#if 1
//#define LOCAL_MODERL "/media/mmcblk0p1/bin/yolov5_relu_quantize_r.cfg"
//#define LOCAL_MODERL_WEIGHT "/media/mmcblk0p1/bin/yolov5_relu_quantize_r.weight"

#define LOCAL_MODERL "/home/ipc_pro/bin/yolov5_relu_quantize_r.cfg"
#define LOCAL_MODERL_WEIGHT "/home/ipc_pro/bin/yolov5_relu_quantize_r.weight"

#define LOCAL_MODERL_TT_1 "/tmp/aicfg/yolov5_relu_quantize_r.cfg"
#define LOCAL_MODERL_WEIGHT_TT_1 "/tmp/aicfg/yolov5_relu_quantize_r.weight"

#define RSN_LOCAL_TT_1 "/tmp/aicfg/resnet18_quantize_r.weight"
#define RSN_LOCAL_CFG_TT_1  "/tmp/aicfg/resnet18_quantize_r.cfg"

#define MODEL_VERSION_TT_1  "/tmp/aicfg/version"

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

//
#if 0
static TS_OVERLAY_YUV_PARAM_S overlay_yuv_param[TS_MAX_OVERLAY_NUM];
static char overlay_yuv_param_set[TS_MAX_OVERLAY_NUM][128];
TS_OVERLAY_YUV_PARAM_S * overlay_yuv_param_get(const char*name){
	int i;
	printf("overlay_yuv_param_get name=%s\n",name);
	TS_S32 s32Ret;
	for(i=0;i<TS_MAX_OVERLAY_NUM;i++){
		if(strlen(overlay_yuv_param_set[i]) == 0){
			printf("to set===%d\n",i);
			strcpy(overlay_yuv_param_set[i],name);
			snprintf(overlay_yuv_param[i].show_content_buffer, sizeof(overlay_yuv_param[i].show_content_buffer), "%s", overlay_yuv_param_set[i]);
			s32Ret = SAMPLE_COMM_REGION_GetOverlayYuvData(&overlay_yuv_param[i]);
			printf("to set===ret=%d\n",s32Ret);
                	if (0 != s32Ret) {
                        	ALG_LOGE("SAMPLE_COMM_REGION_GetOverlayYuvData error\n");
				overlay_yuv_param[i].show_content_buffer[0] = 0;
                        	return NULL;
                	}
			return &overlay_yuv_param[i];

		}
		if(strcmp(overlay_yuv_param[i].show_content_buffer,name) == 0){
			return &overlay_yuv_param[i];

		}
	}
	return NULL;

}
#endif
TS_S32 TS_ALG_CatDetect_Init(TS_VOID **pHandle, ALG_CAT_MODEL_INIT_S *param)
{
	TS_S32 s32Ret = TS_SUCCESS;

	ALG_MODEL_INIT_S alg_param;
	//ts_alg_log_set_level(ALG_LOG_WARN);
	ALG_CAT_MODEL_INIT_S *mode_param = param;
    SAMPLE_ALG_INSTANCE_S* pInst = *((SAMPLE_ALG_INSTANCE_S**)pHandle);

    FILE *pf_cfg = NULL;
	FILE *pf_weight = NULL;
	if(!model_info)
	{
		model_info = malloc(sizeof(ALG_CAT_MODEL_INIT_S));
		model_info->embedding_model_cfg = malloc(128);
		model_info->embedding_model_weight = malloc(128);
		model_info->food_model_cfg = malloc(128);
		model_info->food_model_weight = malloc(128);
		model_info->yolo_model_cfg = malloc(128);
		model_info->yolo_model_weight = malloc(128);
		model_info->model_version_file = malloc(128);
	}
	memset(model_info->embedding_model_cfg,0,128);
	memset(model_info->embedding_model_weight,0,128);
	memset(model_info->food_model_cfg,0,128);
	memset(model_info->food_model_weight,0,128);
	memset(model_info->yolo_model_cfg,0,128);
	memset(model_info->yolo_model_weight,0,128);
	memset(model_info->model_version_file,0,128);
	if(mode_param->food_model_cfg != NULL && mode_param->food_model_weight != NULL){
		memcpy(model_info->food_model_cfg ,mode_param->food_model_cfg,strlen((char *)mode_param->food_model_cfg));
		memcpy(model_info->food_model_weight ,mode_param->food_model_weight,strlen((char *)mode_param->food_model_weight));
		is_food_on = 1;
	}	
	#if 1
	memcpy(model_info->embedding_model_cfg ,mode_param->embedding_model_cfg,strlen((char *)mode_param->embedding_model_cfg));
	memcpy(model_info->embedding_model_weight ,mode_param->embedding_model_weight,strlen((char *)mode_param->embedding_model_weight));

	memcpy(model_info->yolo_model_cfg ,mode_param->yolo_model_cfg,strlen((char *)mode_param->yolo_model_cfg));
	memcpy(model_info->yolo_model_weight ,mode_param->yolo_model_weight,strlen((char *)mode_param->yolo_model_weight));
	memcpy(model_info->model_version_file ,mode_param->model_version_file,strlen((char *)mode_param->model_version_file));
	#else
	memcpy(model_info->embedding_model_cfg ,RSN_LOCAL_CFG_TT_1,strlen(RSN_LOCAL_CFG_TT_1));
	memcpy(model_info->embedding_model_weight ,RSN_LOCAL_TT_1,strlen(RSN_LOCAL_TT_1));

	memcpy(model_info->yolo_model_cfg ,LOCAL_MODERL_TT_1,strlen(LOCAL_MODERL_TT_1));
	memcpy(model_info->yolo_model_weight ,LOCAL_MODERL_WEIGHT_TT_1,strlen(LOCAL_MODERL_WEIGHT_TT_1));
	memcpy(model_info->model_version_file ,MODEL_VERSION_TT_1,strlen(MODEL_VERSION_TT_1));
	#endif
	model_info->cat_callback = mode_param->cat_callback;
	model_info->userdata = mode_param->userdata;



	TS_U8 *pu8_model_cfg = model_info->yolo_model_cfg;
    TS_U8 *pu8_model_weight = model_info->yolo_model_weight;
    //TS_U8 pu8_model_cfg[] = model_info->embedding_model_cfg;
    //TS_U8 pu8_model_weight[] = model_info->embedding_model_weight;
    //TS_U8 pu8_model_cfg[] = "./firesmokedetect_cfg.cfg";
    //TS_U8 pu8_model_weight[] = "./firesmokedetect_weight.weight";
	SAMPLE_PRT("pu8_model_cfg %s !!!\n",pu8_model_cfg);
	SAMPLE_PRT("pu8_model_weight %s !!!\n",pu8_model_weight);
	//SAMPLE_PRT("FireSmokeDetect model file fopen failed !!!\n");
	//SAMPLE_PRT("FireSmokeDetect model file fopen failed !!!\n");
    pf_cfg = fopen((const char*)pu8_model_cfg, "rb");
	pf_weight = fopen((const char*)pu8_model_weight, "rb");
	if((NULL == pf_cfg) || (NULL == pf_weight)){
		SAMPLE_PRT("FireSmokeDetect model file fopen failed !!!\n");
        SAMPLE_PRT("model_cfg %s, model_weight:%s\n", pu8_model_cfg,  pu8_model_weight);
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

    memset(&alg_param, 0, sizeof(ALG_MODEL_INIT_S));
    alg_param.pGraph = pCfgBuf;
	alg_param.pWeight = pWeightBuf;
	alg_param.u32GraphSize = u32CfgLen;
	alg_param.u32WeightSize = u32WeightLen;
	alg_param.eImageType = ALG_IMAGE_TYPE_INT_HWC_RGB0;//ALG_IMAGE_TYPE_FORMAT_0RGB;
	alg_param.bRneOff = 0;

	//libs add
	//all_det_param.DetectionConfThres = 0.4;//0.7
	//all_det_param.SimilarityThres_Night = 0.4;//0.45
	//all_det_param.SimilarityThres = 0.4;//0.45
	//
	printf("TS_ALG_BodyDetect_Init begin\n");
	s32Ret = TS_ALG_BodyDetect_Init(&(pInst->pHandle), &alg_param);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_BodyDetect_Init error\n");
		goto init_exit;
	}

	SAMPLE_PRT("TS_ALG_BodyDetect_GetVersion:%s\n", TS_ALG_BodyDetect_GetVersion());
    s32Ret = TS_SUCCESS;
    ALG_RSN_Detect_Init();
	if(is_food_on){
		ALG_RSN_Detect_FOOD_Init();
	}
	SAMPLE_PRT("ALG_RSN_Detect_Init sucess!\n");
init_exit:
	free(pCfgBuf);
	free(pWeightBuf);
    return s32Ret;
}

TS_S32 VIDEO_ALG_CatDetect_Proc(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_IMAGE_S *pImageDet, ALG_CatDetect_DET_RESULT_S *pResult, TS_U8 cam_id)
{
	TS_S32 s32Ret = TS_SUCCESS;
	extern void set_result(ALG_CatDetect_DET_RESULT_S *data);

    SAMPLE_ALG_INSTANCE_S* pInst = (SAMPLE_ALG_INSTANCE_S*)pHandle;
	if (!pHandle|| !pImageDet || !pResult) {
		ALG_LOGE("VIDEO_ALG_CatDetect_Proc param is null\n");
		return TS_FAILURE;
	}

	ALG_CatDetect_DET_RESULT_S *pTmpResult = (ALG_CatDetect_DET_RESULT_S *)pResult;
	s32Ret = TS_ALG_BodyDetect_Process(pInst->pHandle, pImageDet, pTmpResult);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_BodyDetect_Process error\n");
	}

    RECT rect;
    const float img_w = (float)pImage->s32W;
    const float img_h = (float)pImage->s32H;
    
	for (int i = 0; i < pTmpResult->u32ObjNum; i++)
	{
        #define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
		pTmpResult->stBox[i].f32Xmin = CLAMP(pTmpResult->stBox[i].f32Xmin, 0.0f, 1.0f);
		pTmpResult->stBox[i].f32Ymin = CLAMP(pTmpResult->stBox[i].f32Ymin, 0.0f, 1.0f);
		pTmpResult->stBox[i].f32Xmax = CLAMP(pTmpResult->stBox[i].f32Xmax, 0.0f, 1.0f);
		pTmpResult->stBox[i].f32Ymax = CLAMP(pTmpResult->stBox[i].f32Ymax, 0.0f, 1.0f);
        #undef CLAMP

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
#if 0
TS_VOID  VIDEO_ALG_CatDetect_ResultProc(TS_U8 *pYuvBuf,  TS_U32 width, TS_U32 height, TS_U32 u32ImageRatio, TS_VOID *pAlgResult)
{
    RECT rect;
    //TS_U32 i;
    //float fThres = 0.0001f;
    //enYuvColorIdx enColor = YUV_RED;

    SAMPLE_ALG_RESULT_S *pCurResult = (SAMPLE_ALG_RESULT_S *)pAlgResult;
	ALG_BODYDET_RESULT_S *pResult = &(pCurResult->gstAlgCatdetResult);
	//测试时需要退出
	#if TEST_MODE3
		return;
	#endif
	//
    //printf("-algo body target num: %d w:%d h:%d\n",
    //        pResult->u32BodyNum, width, height);
#if 1
    for(TS_U32 j = 0; j < pResult->u32BodyNum; j++) {
        //printf("body:%d --score:%f xmin:%f ymin:%f xmax:%f ymax:%f\n", j, pResult->stBox[j].f32Score,
        //        pResult->stBox[j].f32Xmin * image.s32W, pResult->stBox[j].f32Ymin * image.s32H,
        //        pResult->stBox[j].f32Xmax * image.s32W, pResult->stBox[j].f32Ymax * image.s32H);
    }
	for (int i = 0; i < pResult->u32BodyNum; i++)
	{
        rect.left = u32ImageRatio * pResult->stBox[i].f32Xmin  * width;
        rect.top = u32ImageRatio * pResult->stBox[i].f32Ymin *  height;
        rect.right = u32ImageRatio * pResult->stBox[i].f32Xmax * width;
        rect.bottom = u32ImageRatio *pResult->stBox[i].f32Ymax * height;
		//lx test
		int test_conf = test_conf_get();
		YUY_Draw_num(pYuvBuf, width, height, 12,(rect.left+rect.right)/2, rect.top+i+20, test_conf/10, YUV_BLUE);
		YUY_Draw_num(pYuvBuf, width, height, 12,(rect.left+rect.right)/2+12, rect.top+i+20+12, test_conf%10, YUV_BLUE);
		//
		YUV_Draw_Rect(pYuvBuf, width, height, 12, &rect, YUV_BLUE, 5);
		TS_OVERLAY_YUV_PARAM_S * overlay_yuv_param_p;
		char name[128];
#if 1
		extern int get_catName_name(int id, char*name);
		if((pResult->stBox[i].id == 0) || get_catName_name(pResult->stBox[i].id-1, name) < 0){
			overlay_yuv_param_p =overlay_yuv_param_get(" ");//no id
		}else{
			printf("uname=%s\n",name);
			overlay_yuv_param_p =overlay_yuv_param_get(name);
		}
#endif
		#if 1
		//overlay_yuv_param_p =overlay_yuv_param_get(name);
		int newTmpLocaX;
		int newTmpLocaY;
		newTmpLocaX = abs((rect.left + (rect.right - rect.left)/2)-(overlay_yuv_param_p->sDis_w/2));
		newTmpLocaY = (rect.top - (overlay_yuv_param_p->sDis_h))<0?0:(rect.top - (overlay_yuv_param_p->sDis_h));
		overlay_yuv_param_p->sDis_x = newTmpLocaX;
		overlay_yuv_param_p->sDis_y = newTmpLocaY;
		#if 0
		overlay_bitmap_on_nv12(pYuvBuf, width, height, overlay_yuv_param[i].pBitmapByte, overlay_yuv_param[i].sArraySize, overlay_yuv_param[i].sDis_x, overlay_yuv_param[i].sDis_y);
		#else
		overlay_with_shadow_bitmap_on_nv12(pYuvBuf, width, height, overlay_yuv_param_p->pBitmapByte, overlay_yuv_param_p->sArraySize, overlay_yuv_param_p->sDis_x, overlay_yuv_param_p->sDis_y);
		#endif
		#endif
#ifdef OVERLAY_RGN
		//font osd
		int newTmpLocaX = abs((rect.left + (rect.right - rect.left)/2)-(overlay_rgn_param[i].sDis_w/2));
		int newTmpLocaY = (rect.top - (overlay_rgn_param[i].sDis_h))<0?0:(rect.top - (overlay_rgn_param[i].sDis_h));
		int LocaChX = abs(overlay_rgn_param[i].sDis_x - newTmpLocaX);//居中显示
		int LocaChY = abs(overlay_rgn_param[i].sDis_y - newTmpLocaY);//画框之上
		if (LocaChX>5 || LocaChY>5){
			overlay_rgn_param[i].sDis_x = newTmpLocaX;
			overlay_rgn_param[i].sDis_y = newTmpLocaY;
			overlay_rgn_param[i].rgnUpdate = 1;
		}
		afafafafa
#endif
	}

#ifdef OVERLAY_RGN
	if (pResult->u32BodyNum==0){	//侦测不到，撤回所有显示
		for (int i = 0; i < TS_RNG_HANDLE_BUTT; i++){
			if(overlay_rgn_param[i].AttachState){
				SAMPLE_COMM_REGION_Detach(&overlay_rgn_param[i]);
				overlay_rgn_param[i].AttachState = 0;
				overlay_rgn_param[i].sDis_x = 0;
				overlay_rgn_param[i].sDis_y = 0;
			}
		}
	}
	else{
		for (int i = 0; i < pResult->u32BodyNum; i++){
			if(overlay_rgn_param[i].AttachState && overlay_rgn_param[i].rgnUpdate){
				SAMPLE_COMM_REGION_Detach(&overlay_rgn_param[i]);
				overlay_rgn_param[i].AttachState = 0;
			}
			if(!overlay_rgn_param[i].AttachState&& overlay_rgn_param[i].rgnUpdate){
				SAMPLE_COMM_REGION_Attach(&overlay_rgn_param[i]);
				overlay_rgn_param[i].AttachState = 1;
				overlay_rgn_param[i].rgnUpdate = 0;
			}
		}
		for (int i = pResult->u32BodyNum; i < TS_RNG_HANDLE_BUTT; i++){
			if(overlay_rgn_param[i].AttachState){
				SAMPLE_COMM_REGION_Detach(&overlay_rgn_param[i]);
				overlay_rgn_param[i].AttachState = 0;
			}
		}
	}
#endif
#endif
}
#endif
TS_S32 VIDEO_ALG_CatDetect_Exit(TS_VOID *pHandle)
{
	TS_S32 s32Ret = TS_SUCCESS;
	if (!pHandle) {
		SAMPLE_PRT("VIDEO_ALG_CatDetect_Exit param is null\n");
		s32Ret = TS_FAILURE;
		return s32Ret;
	}

	SAMPLE_ALG_INSTANCE_S* pInst = (SAMPLE_ALG_INSTANCE_S*)pHandle;
	s32Ret = TS_ALG_BodyDetect_Exit(pInst->pHandle);
	
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

//json test


#if 0
static char *json_key_buf[] = {"pets","petId","images"};

// 读取文件内容到字符串
static char* read_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("无法打开文件");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *content = (char*)malloc(length + 1);
    if (!content) {
        perror("malloc error");
        fclose(file);
        return NULL;
    }

    fread(content, 1, length, file);
    content[length] = '\0';

    fclose(file);
    return content;
}

int catinfo_cjson_anylx(char * file_content)
{
	int cat_id = 0;
	// 解析JSON字符串
	cJSON *json = cJSON_Parse(file_content);
	if (!json) {
		printf("json anxlx error: %s\n", cJSON_GetErrorPtr());
		free(file_content);
		return 1;
	}

	// 示例：打印JSON对象的键值对
	cJSON *item = NULL;
	item = cJSON_GetObjectItem(json,json_key_buf[0]);
	if (!item) {
		printf("item anxlx error: %s\n", cJSON_GetErrorPtr());
		free(file_content);
		cJSON_Delete(json);
		return 1;
	}
	int num = cJSON_GetArraySize(item);
	cJSON *array_item = NULL;
	cJSON *id_item = NULL;
	cJSON *path_item = NULL;
	cJSON *test_item = NULL;
	int pic_num = 0;
	struct catPicInfo*pic_data = (struct catPicInfo*)malloc(sizeof(struct catPicInfo)*25);//MAX_PIC_NUM
	memset(pic_data,0,sizeof(struct catPicInfo)*25);
	for(int i=0 ;i < num ;i++)
	{
		array_item = cJSON_GetArrayItem(item,i);
		id_item = cJSON_GetObjectItem(array_item,json_key_buf[1]);
		cat_id = id_item->valueint;
		//printf("key: %s, value: %d\n", id_item->string, cat_id);
		path_item = cJSON_GetObjectItem(array_item,json_key_buf[2]);
		cJSON_ArrayForEach(test_item, path_item){
			//printf("key: %s, value[%d]: %s\n", test_item->string, strlen(cJSON_GetStringValue(test_item)),cJSON_GetStringValue(test_item));
			pic_data[pic_num].id = cat_id;
			pic_data[pic_num].size = strlen(cJSON_GetStringValue(test_item));
			memcpy(pic_data[pic_num].data,cJSON_GetStringValue(test_item),pic_data[pic_num].size);
			pic_data[pic_num].data[pic_data[pic_num].size+1] = '\0';
			pic_num++;
		}
	}
	change_catName_info2(pic_data,pic_num,NULL,0);
	free(pic_data);
	// 清理
	cJSON_Delete(json);
	free(file_content);
	return 0;
}
#endif
//
char* extract_filepath(const char* full_path) {
    char* last_slash = strrchr(full_path, '/');     // 查找最后一个'/'位置‌:ml-citation{ref="4" data="citationList"}
    if (last_slash == NULL) return strdup("");      // 无路径时返回空字符串‌:ml-citation{ref="1" data="citationList"}
    
    size_t path_len = last_slash - full_path + 1;   // 计算路径长度（包含末尾斜杠）
    char* path = malloc(path_len + 1);              // 分配动态内存‌:ml-citation{ref="3" data="citationList"}
    if (!path) return NULL;
    
    strncpy(path, full_path, path_len);             // 复制路径部分‌:ml-citation{ref="4" data="citationList"}
    path[path_len] = '\0';                          // 添加终止符
    return path;
}
#if 0
TS_S32 CatConfigRenew(TS_CHAR*path)
{
    const char *filename = path;
    char *file_content = read_file(filename);
    if (!file_content) {
        return 1;
    }
	int ret = catinfo_cjson_anylx(file_content);
	char *pic_path_head = extract_filepath(filename);
   set_picpath_head(pic_path_head);
    return ret;

}
#endif
TS_S32 CatConfigRenew(TS_CHAR*url,TS_CHAR*sn)

{
	pic_renew(url,sn);
    return 0;

}
TS_S32 CatSetPicDir(TS_CHAR*path)
{
	printf("CatSetPicDir\n");
	set_pic_path(path);
	return 0;
}

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
