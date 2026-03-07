#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>

#include "ts_rne_c_api.h"
#include "ts_rne_log.h"
#include "ts_rne_version.h"
#include "ts_rne_time.h"

#include "ts_alg_log.h"
#include "ts_alg_body_detect_demo.h"
#include "process_demo.h"

#include "ts_rne_nn_input.h"

//#define TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM (4)	//56 de sdk内部会对齐
// #define TIME_CONSUME_LAYER

static TS_BOOL gRneOff = TS_FALSE;
static TS_FLOAT gThreshold = 0.45;
static TS_FLOAT *gPostProcBuf = NULL;

#ifdef TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM
	static TS_U8 *gParamStride = NULL;
#endif

static TS_S32 TS_ALG_BodyDetectFilter(ALG_BODY_RESULT_S *modelResult, TS_U32 w, TS_U32 h,
												ALG_BODY_RESULT_S *result)
{
	if(TS_NULL == modelResult || TS_NULL == result) {
		ALG_LOGE("error, Invalid parameter!\n");
		return -1;
	}

	TS_U32 i = 0, j = 0;
	TS_U32 num = modelResult->u32BodyNum;

	memset(result, 0, sizeof(ALG_BODY_RESULT_S));

	for(i = 0; i < num; i++) {

		// Filter threshold
		if(modelResult->stBox[i].f32Score < gThreshold)
			continue;

		// Filter Incomplete
		//TS_U32 xmin = modelResult->stBox[i].f32Xmin;
		//TS_U32 ymin = modelResult->stBox[i].f32Ymin;
		//TS_U32 xmax = modelResult->stBox[i].f32Xmax;
		//TS_U32 ymax = modelResult->stBox[i].f32Ymax;
		//if (xmin <= 2 || ymin <= 2 || xmax >= (w-2) || ymax >= (h-2))
		//	continue;

		memcpy(&result->stBox[j], &modelResult->stBox[i], sizeof(ALG_BODY_BOX_S));
		result->stBox[j].f32Xmin /= w;
		result->stBox[j].f32Ymin = MAX(0, (result->stBox[j].f32Ymin-12.0) / 360.0);	//送给模型的数据上面补了12行的边，还原会原始位置
		result->stBox[j].f32Xmax /= w;
		result->stBox[j].f32Ymax = MAX(0, (result->stBox[j].f32Ymax-12.0) / 360.0);	//送给模型的数据上面补了12行的边，还原会原始位置

		j++;
		result->u32BodyNum = j;
	}

	return 0;
}

TS_S32 TS_ALG_BodyDetect_Init(TS_VOID **handle, ALG_MODEL_INIT_S *param)
{
	TS_MPI_TRP_RNE_SetLogLevel(RNE_LOG_INFO);
    ALG_LOGI("rne log level : %d\n", TS_MPI_TRP_RNE_GetLogLevel());
    ALG_LOGI("rne lib version :%s\n", TS_MPI_TRP_RNE_GetSdkVersion());
	TS_S32 ret = 0;

	gRneOff = param->bRneOff;
	if(TS_TRUE != gRneOff) {
	    // ret = TS_MPI_TRP_RNE_OpenDevice(NULL, rne_register_gp_layers);
	    ret = TS_MPI_TRP_RNE_OpenDevice(NULL, NULL);
	    if (0 != ret) {
	        ALG_LOGE("open device error!\n");
	        return -1;
	    }
		ALG_LOGD("open device success! \n");
	}

    RNE_NET_S *nModel = (RNE_NET_S *)malloc(sizeof(RNE_NET_S));
    if(NULL == nModel)
    {
		ALG_LOGE("malloc error!\n");
        return -1;
    }

	memset(nModel, 0, sizeof(RNE_NET_S));
    nModel->u8pGraph = param->pGraph;
	nModel->s32GraphLen = param->u32GraphSize;
	nModel->u8pParams = param->pWeight;
    nModel->eInputType = param->eImageType;
    /* 初始化多网络模型，并在每次初始化网络配置后，进行网络OnceLoad
     */
    /* 量化和权重数据需要4byte对齐
     * 如果未在头文件4byte对齐，可执行W_ALIGN_BYTES_NUM内代码，进行4字节对齐
     */

#ifdef TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM

	if(TS_NULL != gParamStride) {
		ALG_LOGE("Multi threading is not supported!\n");
        return -1;
	}

#ifdef LINUX_PAL
    gParamStride = (TS_U8 *)TS_MPI_TRP_RNE_AllocLinearMem(param->u32WeightSize + TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM);
#else
	gParamStride = (TS_U8 *)TS_MPI_TRP_RNE_Alloc(param->u32WeightSize + TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM);
#endif

    if (NULL == gParamStride) {
        ALG_LOGE("insufficient memory!\n");
        return -1;
    }
    TS_SIZE_T addr = (TS_SIZE_T)gParamStride;
    addr += TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM - 1;
    addr &= ~(TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM - 1);
    memcpy((TS_VOID *)addr, nModel->u8pParams, param->u32WeightSize);
    nModel->u8pParams = (TS_U8 *)addr;
#endif

    /* 初始化单个网路
        */
    ret = TS_MPI_TRP_RNE_LoadModel(nModel);
    if (ret) {
        ALG_LOGE("load model error!\n");
        return ret;
    }
    /* net once load
        * 仅有网络模型配置为once load情况下，内部才真正执行once load
        */
    ret = TS_MPI_TRP_RNE_OnceLoad(nModel);
    if (ret) {
        ALG_LOGE("once load error!\n");
        return ret;
    }

#ifdef TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM
	if(TS_NULL != gParamStride) {
#ifdef LINUX_PAL
		TS_MPI_TRP_RNE_FreeLinearMem(gParamStride);
#else
		TS_MPI_TRP_RNE_Free(gParamStride);
#endif
		gParamStride = TS_NULL;
	}
#endif

	TS_U32 heap_size = TS_MPI_TRP_RNE_GetBlobsBufSize(nModel);
	if(heap_size > 0){
		ALG_LOGI("model heap size is %d\n", heap_size);
	}
	else{
		ALG_LOGE("error, TS_MPI_TRP_RNE_GetBlobsBufSize error!\n");
		return -1;
	}

    int shapeVec[] = { 1, 3 * 48 * 80 + 3 * 24 * 40 + 3 * 12 * 20, 6};//0,1,2
    gPostProcBuf = (TS_FLOAT *)malloc(shapeVec[0] * shapeVec[1] * shapeVec[2] * sizeof(float));    //362,880
	if(NULL == gPostProcBuf){
		ALG_LOGE("malloc error !!!\n");
		return -1;
	}
	ALG_LOGI("algo heap size is %d\n", shapeVec[0] * shapeVec[1] * shapeVec[2] * sizeof(float));

    *handle = nModel;
    return 0;
}

TS_S32 TS_ALG_BodyDetect_Exit(TS_VOID *handle)
{
    RNE_NET_S *nModel = (RNE_NET_S *)handle;

	TS_MPI_TRP_RNE_UnloadModel(nModel);

	if(TS_NULL != nModel) {
		free(nModel);
		nModel = TS_NULL;
	}

	if(TS_TRUE != gRneOff) {
		TS_MPI_TRP_RNE_CloseDevice();
	}

	if(TS_NULL != gPostProcBuf) {
		free(gPostProcBuf);
		gPostProcBuf = TS_NULL;
	}

    return 0;
}

TS_S32 TS_ALG_BodyDetect_Process(TS_VOID *handle, ALG_IMAGE_S *image, ALG_BODY_RESULT_S *result)
{
	TS_U32 cstride[16];
	TS_FLOAT fcoeff[16];

    if(TS_NULL == image->pData || TS_NULL == result) {
		ALG_LOGE("error, Invalid parameter!\n");
		return -1;
	}

	RNE_NET_S *nModel = (RNE_NET_S *)handle;

	RNE_BLOBS_S *inputBlobs = TS_MPI_TRP_RNE_GetInputBlobs(nModel);
	if(TS_NULL == inputBlobs) {
		ALG_LOGE("error, inputBlobs is NULL!\n");
		return -1;
	}

#if 1 //input image is 640*384
	if(image->s32H != inputBlobs->stpBlob->s32H || image->s32W != inputBlobs->stpBlob->s32W) {
		ALG_LOGE("error, Invalid image size need resize!\n");
		ALG_LOGE("image h:%d w:%d, blob h:%d w:%d\n", image->s32H, image->s32W,
								inputBlobs->stpBlob->s32H, inputBlobs->stpBlob->s32W);
		return -1;
	}
	if (0 != TS_MPI_TRP_RNE_FillInputBlobs(nModel, 0, image->pData)) {
      	ALG_LOGE("fill inputBlobs error!\n");
      	return -1;
    }
#else //input image is 640*360
	if(image->s32H != 360 || image->s32W != 640) {
		ALG_LOGE("error, Invalid image size need resize!\n");
		ALG_LOGE("image h:%d w:%d, blob h:%d w:%d\n", image->s32H, image->s32W,
								inputBlobs->stpBlob->s32H, inputBlobs->stpBlob->s32W);
		return -1;
	}

	//image pretreatment: 640*360->640*384, fill with 114(0x72) before image and end image
	TS_U32 inputSize = image->s32C*image->s32H*image->s32W;
	TS_U32 imageSize = image->s32C*640*384;

	memset(imageBuf, 114, imageSize);
	memcpy(&imageBuf[image->s32C*640*12], image->pData, inputSize);

	nModel->vpInput = imageBuf;
#endif

#ifdef TIME_CONSUME_LAYER
	 TS_MPI_TRP_RNE_StartSysTimer();
	 RNE_TIME_STATES_S time;
	 memset(&time, 0, sizeof(RNE_TIME_STATES_S));
	 TS_MPI_TRP_RNE_InitTimeState(DETECT_OUTPUT_GP_LAYER, MAX_LAYER_TYPE - SLICE_GP_LAYER, &time);
	 TS_MPI_TRP_RNE_NetBindTimeState(nModel, &time);
#endif

	TS_U32 time0 = TS_ALG_GET_TIME();
	//TS_ALG_GET_TIME();
	RNE_BLOBS_S *outputBlobs = TS_MPI_TRP_RNE_Forward(nModel);
	if (outputBlobs == TS_NULL) {
		ALG_LOGE("net forward error!\n");
		#ifdef TIME_CONSUME_LAYER
		TS_MPI_TRP_RNE_ReleaseTimeState(&time);
		#endif
		return -1;
	}

	TS_U32 time1 = TS_ALG_GET_TIME();
	ALG_LOGD("rne forward time:%d\n",time1-time0);

#ifdef TIME_CONSUME_LAYER
	ALG_LOGD("total time:%lld us\n", TS_MPI_TRP_RNE_GetTotalTime(&time));
	ALG_LOGD("forward time:%lld us\n", TS_MPI_TRP_RNE_GetTimeOfForward(&time));
	ALG_LOGD("hw layer time:%lld us\n", TS_MPI_TRP_RNE_GetTimeOfHwLayer(&time));
	ALG_LOGD("gp layer time:%lld us\n", TS_MPI_TRP_RNE_GetTimeOfGpLayer(&time));
	TS_MPI_TRP_RNE_ReleaseTimeState(&time);
#endif

	//ALG_LOGI("net body detect forward done\n");
	//ALG_LOGI("outputBlobs->u32NBlob:%d\n", outputBlobs->u32NBlob);

	if(outputBlobs->u32NBlob <= 0 || TS_NULL == outputBlobs->stpBlob) {
		ALG_LOGE("net forward no result!\n");
		return -1;
	}

	//ALG_LOGD("totol u32NBlob:%d\n", outputBlobs->u32NBlob);
	TS_U8 *resultAddr[3] = {TS_NULL, TS_NULL, TS_NULL};

	for(TS_U32 i = 0; i < outputBlobs->u32NBlob; i++) {

		const int c = outputBlobs->stpBlob[i].s32C;
		const int s32BitNum = outputBlobs->stpBlob[i].s32BitNum;
		const int c_align = TS_MPI_TRP_RNE_CStride(c, s32BitNum, outputBlobs->stpBlob[i].bIsJoined);

		cstride[i] = c_align;
		fcoeff[i] = outputBlobs->stpBlob[i].fCoeff[0];
		// ALG_LOGD("u32NBlob:%d H: %d, W:%d, C:%d, Cstride:%d, coeff: %f\n", i, outputBlobs->stpBlob[i].s32H, outputBlobs->stpBlob[i].s32W, outputBlobs->stpBlob[i].s32C, c_align, outputBlobs->stpBlob[i].fCoeff[0]);
	}

	//exchange outblob1 outblob2 and outblob3
	resultAddr[0] = (TS_U8 *)outputBlobs->stpBlob[0].vpAddr;
	resultAddr[1] = (TS_U8 *)outputBlobs->stpBlob[1].vpAddr;
	resultAddr[2] = (TS_U8 *)outputBlobs->stpBlob[2].vpAddr;

	ALG_BODY_RESULT_S modelResult;
	ts_bodydetect_post_process(resultAddr, &modelResult, cstride, fcoeff, gThreshold, gPostProcBuf);

	if(0 != TS_ALG_BodyDetectFilter(&modelResult, image->s32W, image->s32H, result)) {
		ALG_LOGE("TS_ALG_BodyDetectFilter error!\n");
		return -1;
	}

	return 0;
}

TS_S32 TS_ALG_BodyDetect_SetParam(TS_VOID *handle, ALG_BODY_PARAM_S *param)
{
	if(TS_NULL == param) {
		ALG_LOGE("error, Invalid parameter!\n");
		return -1;
	}

	gThreshold = param->f32Thresh;
	ALG_LOGD("body detect thresh:%f\n", gThreshold);
	return 0;
}

TS_S32 TS_ALG_BodyDetect_GetParam(TS_VOID *handle, ALG_BODY_PARAM_S *param)
{
	if(TS_NULL == param) {
		ALG_LOGE("error, Invalid parameter!\n");
		return -1;
	}

	param->f32Thresh = gThreshold;
	return 0;
}

const TS_CHAR* TS_ALG_BodyDetect_GetVersion(TS_VOID)
{
    static const TS_CHAR *ver = "bodyDetect_v1.5_model_0712";

    return (const TS_CHAR *)ver;
}