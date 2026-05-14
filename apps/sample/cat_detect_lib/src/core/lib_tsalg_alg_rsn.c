#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

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
//#include "ts_alg_body_detect_v2.h"
//#include "process_demo.h"

#include "ts_rne_nn_input.h"

#include "soft_line.h"
#include "ts_alg_type.h"
//#include "layers_GP.h"

#include "video_alg_catdetect-api.h"
static TS_VOID *pRsnHandle = NULL;
TS_BOOL gRneOff = TS_FALSE;
//#define RSN_LOCAL "/media/mmcblk0p1/bin/resnet18_quantize_r.weight"
//#define RSN_LOCAL_CFG  "/media/mmcblk0p1/bin/resnet18_quantize_r.cfg"

#define RSN_LOCAL "/home/ipc_pro/bin/resnet18_quantize_r.weight"
#define RSN_LOCAL_CFG  "/home/ipc_pro/bin/resnet18_quantize_r.cfg"
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
#if 0
static int _PostProcess(RNE_BLOBS_S *outblobs, TS_S32 rsn_result[])
{
    FILE *file = NULL;
#if WIRTE_RESNET_RESULT
    static int i = 0;
    char filename[32];

    sprintf(filename, "resnet_result_%d.txt", i);
    file = fopen(filename, "wb");
    i++;
#endif

    for (TS_U32 idx = 0; idx < outblobs->u32NBlob; idx++) {
        TS_S32 h = outblobs->stpBlob[idx].s32H;
        TS_S32 w = outblobs->stpBlob[idx].s32W;
        TS_S32 c = outblobs->stpBlob[idx].s32C;
        bool is_unsign = outblobs->stpBlob[idx].bIsUnsigned;
        TS_S32 cStride = TS_MPI_TRP_RNE_CStride(c, outblobs->stpBlob[idx].s32BitNum,
                                                outblobs->stpBlob[idx].bIsJoined);
        TS_S32 c_out=(outblobs->stpBlob[idx].eFormat==RNE_BLOB_N_H_W_C) ? c : cStride;
        TS_S32 uSize = (outblobs->stpBlob[idx].s32BitNum / CHAR_BIT);
        TS_U8 *out = (TS_U8 *)(outblobs->stpBlob[idx].vpAddr);

        // float coeff = *(outblobs->stpBlob[idx].fCoeff);
        printf("u32NBlob:%d stpBlob[%d].s32N:%d n:%d, h:%d, w:%d, c:%d, cStride:%d, c_out:%d, uSize:%d\n",
                outblobs->u32NBlob, idx, outblobs->stpBlob[idx].s32N,
                outblobs->u32NBlob, h, w, c, cStride, c_out, uSize);
        for(TS_S32 n = 0; n < outblobs->stpBlob[idx].s32N; n++) {
            //int max_class_id = -1;
            TS_S32 max_class_score = INT_MIN;
            for (TS_S32 i = 0; i < h; i++) {
                for (TS_S32 j = 0; j < w; j++) {
                    for(TS_S32 l = 0; l < c; l++) {
                        TS_S32 *d = (TS_S32 *)(out + (((j + i * w) * c_out + l)*uSize));
                        // 解析结果数据类型(有无符号，位宽)
                        TS_S32 data = *d & MASK_BITS(outblobs->stpBlob[idx].s32BitNum);
                        TS_S32 bitNum = (outblobs->stpBlob[idx].s32BitNum - 1);
                        TS_S32 bitValue = MASK_BITS(bitNum);
                        if (!is_unsign && data > bitValue) {
                            bitNum = (outblobs->stpBlob[idx].s32BitNum);
                            bitValue = MASK_BITS(bitNum);
                            data = data - (bitValue + 1);
                        }
                        if (data >= max_class_score) {
                            max_class_score = data;
                            max_class_id = l;
                        }
                        //printf("rsn _PostProcess:[%d][%d][%d]:%d\n",
                        //        i, j, l, data);

#if 0
                        if(file) {
                            char content[128];
                            sprintf(content, "[%d][%d][%d]:%d\n",
                                    i, j, l, data);
                            //printf("content:%s\n", content);

                            fwrite((char*)content, 1, strlen(content), file);
                        }
#else
                        rsn_result[l] = data;
#endif
                    }
                }
            }

            //std::string outText = imagenet2012_class[max_class_id];
            //printf("<predict_%d  idx:%d\tclass:%s>\n",n,max_class_id,outText.c_str());
            //out += h * w * cStride * uSize;
        }
    }

    if(file) {
        fclose(file);
    }
    //printf("PostProcess >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    return 0;
}
#endif

TS_S32 ALG_RSN_Detect_Process(ALG_IMAGE_S *image, TS_FLOAT rsn_result[])
{
    TS_VOID *handle = pRsnHandle;
	//TS_U32 cstride[16];
	//TS_FLOAT fcoeff[16];

    if(TS_NULL == handle) {
		ALG_LOGE("handle is null\n");
		return -1;
	}

    if(TS_NULL == image->pData) {
		ALG_LOGE("error, Invalid parameter!\n");
		return -1;
	}

	RNE_NET_S *nModel = (RNE_NET_S *)handle;
#if 0
	RNE_BLOBS_S *inputBlobs = TS_MPI_TRP_RNE_GetInputBlobs(nModel);
	if(TS_NULL == inputBlobs) {
		ALG_LOGE("error, inputBlobs is NULL!\n");
		return -1;
	}

    //printf("%s(%d)inputBlobs->stpBlob->s32H:%d inputBlobs->stpBlob->s32W:%d\n",
    //        __func__, __LINE__, inputBlobs->stpBlob->s32H, inputBlobs->stpBlob->s32W);
	if(image->s32H != inputBlobs->stpBlob->s32H || image->s32W != inputBlobs->stpBlob->s32W) {
		ALG_LOGE("error, Invalid image size need resize!\n");
		ALG_LOGE("image h:%d w:%d, blob h:%d w:%d\n", image->s32H, image->s32W,
								inputBlobs->stpBlob->s32H, inputBlobs->stpBlob->s32W);
		return -1;
	}

    //printf("(%s)%d###########################\n", __func__, __LINE__);
	if (0 != TS_MPI_TRP_RNE_FillInputBlobs(nModel, 0, image->pData)) {
      	ALG_LOGE("fill inputBlobs error!\n");
      	return -1;
    }
#endif
    //printf("(%s)%d###########################\n", __func__, __LINE__);
    if (0 != TS_MPI_TRP_RNE_SetInputBlobsAddr(nModel, (void *)(image->pData), (void*)(intptr_t)(image->pDataPhy))) {
        ALG_LOGE("set inputBlobs error!\n");
        return -1;
    }


#ifdef TIME_CONSUME_LAYER
	 TS_MPI_TRP_RNE_StartSysTimer();
	 RNE_TIME_STATES_S time;
	 memset(&time, 0, sizeof(RNE_TIME_STATES_S));
	 TS_MPI_TRP_RNE_InitTimeState(DETECT_OUTPUT_GP_LAYER, MAX_LAYER_TYPE - SLICE_GP_LAYER, &time);
	 TS_MPI_TRP_RNE_NetBindTimeState(nModel, &time);
#endif

	//TS_U32 time0 = TIME_CACL_GET();
	//TIME_CACL_GET();
    //printf("(%s)%d###########################\n", __func__, __LINE__);
	RNE_BLOBS_S *outputBlobs = TS_MPI_TRP_RNE_Forward(nModel);
    //printf("(%s)%d###########################\n", __func__, __LINE__);
	if (outputBlobs == TS_NULL) {
		ALG_LOGE("net forward error!\n");
		#ifdef TIME_CONSUME_LAYER
		TS_MPI_TRP_RNE_ReleaseTimeState(&time);
		#endif
		return -1;
	}

	//printf("rne forward time:%f\n",TS_ALG_GetTimeMS()-time0);
    //printf("u32NBlob: %d\n", outputBlobs->u32NBlob);
    //printf("s32N: %d s32H: %d s32W: %d s32C: %d\n", outputBlobs->stpBlob[0].s32N,outputBlobs->stpBlob[0].s32H,outputBlobs->stpBlob[0].s32W,outputBlobs->stpBlob[0].s32C);
    //printf("u32Size: %d\n", outputBlobs->stpBlob[0].u32Size);
    //printf("s32BitNum: %d\n", outputBlobs->stpBlob[0].s32BitNum);
    printf("s32c: %d\n", outputBlobs->stpBlob[0].s32C);
    TS_S8 * data = (TS_S8*)outputBlobs->stpBlob[0].vpAddr;
    for(int i = 0; i < outputBlobs->stpBlob[0].s32C; i++)
    {
        rsn_result[i] = data[i]*outputBlobs->stpBlob[0].fCoeff[0];
        //printf("data[%d]: %f\n", i,data[i]*outputBlobs->stpBlob[0].fCoeff[0]);
    }
    //printf("(%s)%d###########################\n", __func__, __LINE__);
    return 0;
	//return _PostProcess(outputBlobs, rsn_result);
}

TS_S32 TS_ALG_RSN_Init(TS_VOID **handle, ALG_MODEL_INIT_S *param)
{
    ALG_LOGI("===========111111111111111===============rne log level : %d\n", TS_MPI_TRP_RNE_GetLogLevel());
    ALG_LOGI("============11111111111111=================rne lib version :%s\n", TS_MPI_TRP_RNE_GetSdkVersion());
	TS_S32 ret = 0;

	gRneOff = param->bRneOff;
	if(TS_TRUE != gRneOff) {
		printf("to open one\n");
	    ret = TS_MPI_TRP_RNE_OpenDevice(NULL, NULL);//rne_register_gp_layers
	    //ret = TS_MPI_TRP_RNE_OpenDevice(NULL, NULL);
	    if (0 != ret) {
	        ALG_LOGE("open device error!\n");
	        return -1;
	    }
		ALG_LOGD("open device success! \n");
	}

		printf("to open one22\n");
    RNE_NET_S *nModel = (RNE_NET_S *)malloc(sizeof(RNE_NET_S));
    if(NULL == nModel)
    {
		ALG_LOGE("malloc error!\n");
        return -1;
    }
		printf("to open one23\n");

	memset(nModel, 0, sizeof(RNE_NET_S));
    nModel->u8pGraph = param->pGraph;
    nModel->u8pParams = param->pWeight;
    nModel->s32GraphLen = param->u32GraphSize;
    nModel->eInputType = param->eImageType;
    /* 初始化多网络模型，并在每次初始化网络配置后，进行网络OnceLoad
     */
    /* 量化和权重数据需要4byte对齐
     * 如果未在头文件4byte对齐，可执行W_ALIGN_BYTES_NUM内代码，进行4字节对齐
     */

#ifdef TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM
		printf("to open one24\n");

	if(TS_NULL != gParamStride) {
		printf("to open one25\n");
		ALG_LOGE("Multi threading is not supported!\n");
        return -1;
	}

#ifdef LINUX_PAL
		printf("to open one26\n");
    gParamStride = (TS_U8 *)TS_MPI_TRP_RNE_AllocLinearMem(param->u32WeightSize + TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM);
#else
		printf("to open one27\n");
	gParamStride = (TS_U8 *)TS_MPI_TRP_RNE_Alloc(param->u32WeightSize + TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM);
#endif

		printf("to open one28\n");
    if (NULL == gParamStride) {
        ALG_LOGE("insufficient memory!\n");
        return -1;
    }
		printf("to open one29\n");
    TS_SIZE_T addr = (TS_SIZE_T)gParamStride;
    addr += TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM - 1;
    addr &= ~(TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM - 1);
    memcpy((TS_VOID *)addr, nModel->u8pParams, param->u32WeightSize);
    nModel->u8pParams = (TS_U8 *)addr;
#endif
		printf("to open one10\n");

    /* 初始化单个网路
        */
    ret = TS_MPI_TRP_RNE_LoadModel(nModel);
    if (ret) {
        ALG_LOGE("load model error!\n");
        return ret;
    }
		printf("to open one11\n");
    /* net once load
        * 仅有网络模型配置为once load情况下，内部才真正执行once load
        */
    ret = TS_MPI_TRP_RNE_OnceLoad(nModel);
    if (ret) {
        ALG_LOGE("once load error!\n");
        return ret;
    }
		printf("to open one12\n");

#ifdef TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM
		printf("to open one13\n");
	if(TS_NULL != gParamStride) {
#ifdef LINUX_PAL
		printf("to open one14\n");
		TS_MPI_TRP_RNE_FreeLinearMem(gParamStride);
#else
		printf("to open one15\n");
		TS_MPI_TRP_RNE_Free(gParamStride);
#endif
		printf("to open one16\n");
		gParamStride = TS_NULL;
	}
#endif
		printf("to open one17\n");

	TS_U32 heap_size = TS_MPI_TRP_RNE_GetBlobsBufSize(nModel);
	if(heap_size > 0){
		ALG_LOGI("model heap size is %d\n", heap_size);
	}
	else{
		ALG_LOGE("error, TS_MPI_TRP_RNE_GetBlobsBufSize error!\n");
		return -1;
	}

    *handle = nModel;
    return 0;
}
extern ALG_CAT_MODEL_INIT_S *model_info;
TS_S32 ALG_RSN_Detect_Init()
{
    TS_VOID	*pHandle = NULL;
	TS_S32 s32Ret = TS_SUCCESS;

	ALG_MODEL_INIT_S param;
	//log_set_level(ALG_LOG_ERROR);
	//log_set_level(ALG_LOG_DEBUG);

    FILE *pf_cfg = NULL;
	FILE *pf_weight = NULL;

    	//TS_U8 weightpath[] = RSN_LOCAL;//"./resnet18_quantize_r.weight"; //"./yolov5s_cat_v2_quantize_r.weight"; //"./pcpp_detect_weight.weight";
	//TS_U8 cfgpath[] = RSN_LOCAL_CFG;//"./resnet18_quantize_r.cfg"; //"./yolov5s_cat_v2_quantize_r.cfg"; //"./pcpp_detect_cfg.cfg";
	TS_U8 *weightpath = model_info->embedding_model_weight;
	TS_U8 *cfgpath = model_info->embedding_model_cfg;
	

    printf("(%s)%d###########################\n", __func__, __LINE__);

    printf("rsn weightpath=%s\n",weightpath);
    printf("rsn cfgpath=%s\n",cfgpath);
    pf_cfg = fopen((const char *)cfgpath, "rb");
    pf_weight = fopen((const char *)weightpath, "rb");
	if ((NULL == pf_cfg) || (NULL == pf_weight)) {
		ALG_LOGE("pcpp model file fopen failed !!!\n");
        ALG_LOGE("model_cfg: %s, model_weight:%s\n", cfgpath,  weightpath);
		return TS_FAILURE;
	}

    printf("(%s)%d###########################\n", __func__, __LINE__);
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
	param.eImageType = ALG_IMAGE_TYPE_INT_HWC_RGB0;//ALG_IMAGE_TYPE_FORMAT_0RGB;
	param.bRneOff = 0;

    printf("(%s)%d###########################\n", __func__, __LINE__);
	s32Ret = TS_ALG_RSN_Init(&pHandle, &param);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_BodyDetV16_Init error\n");
		goto init_exit;
	}

    printf("(%s)%d###########################\n", __func__, __LINE__);
    pRsnHandle = pHandle;

init_exit:
	free(pCfgBuf);
	free(pWeightBuf);
    return s32Ret;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
