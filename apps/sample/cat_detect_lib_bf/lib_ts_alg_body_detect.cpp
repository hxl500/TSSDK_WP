#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>

#include <map>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>

#include "ts_rne_c_api.h"
#include "ts_rne_log.h"
#include "ts_rne_version.h"
#include "ts_rne_time.h"

#include "ts_alg_log.h"
#include "ts_alg_body_detect_v2.h"
#include "ts_rne_nn_input.h"
#include "arrr_diff.h"
//#define TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM (4)	//56 de sdk内部会对齐

// ===================== 性能优化配置 =====================
// #define TIME_CONSUME_LAYER          // 启用耗时统计（需要SDK支持）
#define MAX_DETECT_NUM 100          // 最大检测数量
// ========================================================

static TS_BOOL gRneOff = TS_FALSE;
static TS_FLOAT gThreshold = 0.25;

#ifdef TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM
	static TS_U8 *gParamStride = NULL;
#endif
ALG_CatDetect_DET_PARAM_S det_param_cpp;
static TS_FLOAT *gPostProcBuf = NULL;

static void quick_sort(int* idx, float* score, int left, int right) {
    if (left >= right) return;
    int i = left, j = right;
    float pivot = score[(left + right) / 2];
    while (i <= j) {
        while (score[i] > pivot) i++;
        while (score[j] < pivot) j--;
        if (i <= j) {
            int t = idx[i]; idx[i] = idx[j]; idx[j] = t;
            float tf = score[i]; score[i] = score[j]; score[j] = tf;
            i++; j--;
        }
    }
    quick_sort(idx, score, left, j);
    quick_sort(idx, score, i, right);
}

static int nms(float *torchCat, int len1, int len2, int *nmsOut, float thresh, int maxDet)
{
    if (len1 <= 0) return 0;
    if (maxDet > len1) maxDet = len1;

    float tmpX, tmpY, tmpW, tmpH;

    float* pScore = (float *)(nmsOut + len1);
    int* box_idx = (int *)(pScore + len1);
    float* areas = (float*)(box_idx + len1);
    
    for (int i = 0; i < len1; i++) {
        box_idx[i] = i;
        pScore[i] = torchCat[len2 * i + 4];
        tmpX = torchCat[len2 * i + 0];
        tmpY = torchCat[len2 * i + 1];
        tmpW = torchCat[len2 * i + 2];
        tmpH = torchCat[len2 * i + 3];
        areas[i] = tmpW * tmpH;
        torchCat[len2 * i + 0] = tmpX - tmpW / 2;
        torchCat[len2 * i + 1] = tmpY - tmpH / 2;
        torchCat[len2 * i + 2] = tmpX + tmpW / 2;
        torchCat[len2 * i + 3] = tmpY + tmpH / 2;
    }

    // 优化：使用快速排序替代冒泡排序
    quick_sort(box_idx, pScore, 0, len1 - 1);

    int cnt = 0;
    int sSize = len1;
    
    while (sSize > 0 && cnt < maxDet) {
        nmsOut[cnt++] = box_idx[0];
        
        if (sSize == 1) break;
        
        int keep_idx = box_idx[0];
        int new_size = 0;
        
        float keep_x1 = torchCat[len2 * keep_idx + 0];
        float keep_y1 = torchCat[len2 * keep_idx + 1];
        float keep_x2 = torchCat[len2 * keep_idx + 2];
        float keep_y2 = torchCat[len2 * keep_idx + 3];
        float keep_area = areas[keep_idx];
        
        for (int i = 1; i < sSize; i++) {
            int curr_idx = box_idx[i];
            
            float curr_x1 = torchCat[len2 * curr_idx + 0];
            float curr_y1 = torchCat[len2 * curr_idx + 1];
            float curr_x2 = torchCat[len2 * curr_idx + 2];
            float curr_y2 = torchCat[len2 * curr_idx + 3];
            
            float inter_x1 = keep_x1 > curr_x1 ? keep_x1 : curr_x1;
            float inter_y1 = keep_y1 > curr_y1 ? keep_y1 : curr_y1;
            float inter_x2 = keep_x2 < curr_x2 ? keep_x2 : curr_x2;
            float inter_y2 = keep_y2 < curr_y2 ? keep_y2 : curr_y2;
            
            float inter_w = inter_x2 - inter_x1;
            float inter_h = inter_y2 - inter_y1;
            
            if (inter_w > 0 && inter_h > 0) {
                float inter_area = inter_w * inter_h;
                float union_area = keep_area + areas[curr_idx] - inter_area;
                float iou = inter_area / union_area;
                
                if (iou < thresh) {
                    box_idx[new_size++] = curr_idx;
                }
            } else {
                box_idx[new_size++] = curr_idx;
            }
        }
        
        sSize = new_size;
    }
    
    return cnt;
}

//lx test
float test_conf = 0;
int test_conf_set(float conf)
{
	test_conf = conf;
	return 0;
}
int test_conf_get()
{
	int ret = test_conf*100;
	return ret;
}
//
int TS_ALG_PcppDetV12_PostProcess(unsigned char **blob, unsigned int *cstride, unsigned int *s32C, float *fcoeff, float* dataVec, TS_U16* dataidx)
{
	int nblob = 3;
	int shapeVec2[3][5] = {{1, 3, 48, 80, 6}, {1, 3, 24, 40, 6}, {1, 3, 12, 20, 6}};

    float scale_output[] = { fcoeff[0], fcoeff[1], fcoeff[2] };
    int stride[3] = { 8, 16, 32 };
    int anch[3][6] = {{10,13, 16,30, 33,23}, {30,61, 62,45, 59,119}, {116,90, 156,198, 373,326}};
    float value_det = 0.5f;

    int len = 0;
    int idxtmp = 0;
    float petThres = 0.7;
    TS_ALG_CatDetect_GetParam(&det_param_cpp);
    petThres = det_param_cpp.DetectionConfThres;
    int maxNms = 1024;

    // 优化：预计算stride和scale，减少循环内计算
    float stride_scale[3];
    for(int i = 0; i < 3; i++) {
        stride_scale[i] = stride[i] * scale_output[i] * 2.0f;
    }

    for (int i = 0; i < nblob; i++) {
        int no_yolo = s32C[i] / 3;
        shapeVec2[i][4] = no_yolo;
        float ss = stride_scale[i];
        
        for (int j = 0; j < shapeVec2[i][1]; j++) {
            int* anchorgrid = &(anch[i][j * 2]);
            float anchor_w = anchorgrid[0];
            float anchor_h = anchorgrid[1];
            
            for (int k = 0; k < shapeVec2[i][2]; k++) {
                float grid_y = (float)k - value_det;
                
                for (int m = 0; m < shapeVec2[i][3]; m++) {
                    unsigned char *dataf = blob[i] + j * no_yolo + k * shapeVec2[i][3] * cstride[i] + m * cstride[i];
                    
                    float conf = dataf[4] * scale_output[i];
                    
                    if (conf > petThres && len < maxNms) {
                        // 优化：查找最大类别时，一旦找到足够高的分数就提前退出
                        unsigned char max_data = 0;
                        unsigned char max_idx = 5;
                        for(int n = 5; n < no_yolo; n++){
                            if(max_data < dataf[n]){
                                max_data = dataf[n];
                                max_idx = n;
                                if(max_data > 200) break;
                            }
                        }

                        float score_tmp = dataf[max_idx] * scale_output[i] * conf;
                        
                        if(score_tmp > petThres){
                            float grid_x = (float)m - value_det;
                            
                            // 优化：减少乘法运算
                            float coord_x = dataf[0] * ss + grid_x * stride[i];
                            float coord_y = dataf[1] * ss + grid_y * stride[i];
                            
                            float w_val = dataf[2] * scale_output[i] * 2.0f;
                            float h_val = dataf[3] * scale_output[i] * 2.0f;
                            
                            dataVec[idxtmp++] = coord_x;
                            dataVec[idxtmp++] = coord_y;
                            dataVec[idxtmp++] = w_val * w_val * anchor_w;
                            dataVec[idxtmp++] = h_val * h_val * anchor_h;
                            dataVec[idxtmp++] = score_tmp;
                            
                            dataidx[len++] = max_idx;
                        }
                    }
                }
            }
        }
    }

    return len;
}

TS_S32 TS_ALG_BodyDetect_Init(TS_VOID **handle, ALG_MODEL_INIT_S *param)
{
	printf("22222==============\n");
	TS_MPI_TRP_RNE_SetLogLevel(RNE_LOG_INFO);
    ALG_LOGI("rne log level : %d\n", TS_MPI_TRP_RNE_GetLogLevel());
    ALG_LOGI("rne lib version :%s\n", TS_MPI_TRP_RNE_GetSdkVersion());
	TS_S32 ret = 0;

	gRneOff = (TS_BOOL)param->bRneOff;
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
	//ALG_LOGE("malloc error!\n");

	memset(nModel, 0, sizeof(RNE_NET_S));
    nModel->u8pGraph = param->pGraph;
    nModel->u8pParams = param->pWeight;
    nModel->eInputType = (RNE_NET_INPUT_TYPE_E)param->eImageType;
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
	//ALG_LOGE("malloc error!\n");
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
	//ALG_LOGE("malloc error!\n");
	TS_U32 heap_size = TS_MPI_TRP_RNE_GetBlobsBufSize(nModel);
	if(heap_size > 0){
		ALG_LOGI("model heap size is %d\n", heap_size);
	}
	else{
		ALG_LOGE("error, TS_MPI_TRP_RNE_GetBlobsBufSize error!\n");
		return -1;
	}
	//ALG_LOGE("malloc error!\n");
    int shapeVec[] = { 1, 3 * 48 * 80 + 3 * 24 * 40 + 3 * 12 * 20, 6};//0,1,2
    gPostProcBuf = (TS_FLOAT *)malloc(shapeVec[0] * shapeVec[1] * shapeVec[2] * sizeof(float));    //362,880
	if(NULL == gPostProcBuf){
		ALG_LOGE("malloc error !!!\n");
		return -1;
	}
	ALG_LOGI("algo heap size is %d\n", shapeVec[0] * shapeVec[1] * shapeVec[2] * sizeof(float));
	//ALG_LOGE("malloc error!\n");
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

TS_S32 strides[3] = {8, 16, 32};
std::unordered_map<std::string, std::vector<std::vector<TS_S32>>> anchors = {{"0", {{10, 13}, {16, 30}, {33, 23}}},
    {"1", {{30, 61}, {62, 45}, {59, 119}}},
    {"2", {{116, 90}, {156, 198}, {373, 326}}}
};

TS_S32 TS_ALG_BodyDetect_Process(TS_VOID *handle, ALG_IMAGE_S *image, ALG_CatDetect_DET_RESULT_S *result)
{
#if 1
	TS_U32 cstride[16];
	TS_FLOAT fcoeff[16];

    if(TS_NULL == image->pData || TS_NULL == result) {
		ALG_LOGE("error, Invalid parameter!\n");
		return -1;
	}
	//printf("%s,%d\n",__FUNCTION__,__LINE__);
	RNE_NET_S *nModel = (RNE_NET_S *)handle;

#if 1 //input image is 640*384
    if (0 != TS_MPI_TRP_RNE_SetInputBlobsAddr(nModel, (void *)(image->pData), (void *)(image->pDataPhy))) {
        ALG_LOGE("set inputBlobs error!\n");
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

	RNE_BLOBS_S *outputBlobs = TS_MPI_TRP_RNE_Forward(nModel);
	if (outputBlobs == TS_NULL) {
		ALG_LOGE("net forward error!\n");
		return -1;
	}

	if(outputBlobs->u32NBlob <= 0 || TS_NULL == outputBlobs->stpBlob) {
		ALG_LOGE("net forward no result!\n");
		return -1;
	}

	//ALG_LOGD("totol u32NBlob:%d\n", outputBlobs->u32NBlob);
	// 检查是否有足够的blob数量
	if(outputBlobs->u32NBlob < 3) {
		ALG_LOGE("insufficient blobs: expected at least 3, got %d\n", outputBlobs->u32NBlob);
		return -1;
	}
	
	TS_U8 *resultAddr[3] = {TS_NULL, TS_NULL, TS_NULL};
    unsigned int s32C[16];
    
	// 检查每个vpAddr是否为空
	for(int i = 0; i < 3; i++) {
		if(TS_NULL == outputBlobs->stpBlob[i].vpAddr) {
			ALG_LOGE("blob[%d] vpAddr is NULL\n", i);
			return -1;
		}
		resultAddr[i] = (TS_U8 *)outputBlobs->stpBlob[i].vpAddr;
	}
	for(TS_U32 i = 0; i < outputBlobs->u32NBlob; i++) {
		const int c = outputBlobs->stpBlob[i].s32C;
		const int s32BitNum = outputBlobs->stpBlob[i].s32BitNum;
		const int c_align = TS_MPI_TRP_RNE_CStride(c, s32BitNum, outputBlobs->stpBlob[i].bIsJoined);
		cstride[i] = c_align;
		s32C[i] = c;
		fcoeff[i] = *(outputBlobs->stpBlob[i].fCoeff);
		// ALG_LOGD("u32NBlob:%d H: %d, W:%d, C:%d, Cstride:%d, coeff: %f\n", i, outputBlobs->stpBlob[i].s32H, outputBlobs->stpBlob[i].s32W, outputBlobs->stpBlob[i].s32C, c_align, outputBlobs->stpBlob[i].fCoeff[0]);
	}

    float* dataVec = (float*)gPostProcBuf;    //362,880
    TS_U16* dataidx = (TS_U16 *)(gPostProcBuf + 15*1024);    //362,880
	int len = TS_ALG_PcppDetV12_PostProcess(resultAddr, cstride, s32C, fcoeff, dataVec, dataidx);
    
    int *nmsOut = (int *)(gPostProcBuf + len * 5);
    
    // 使用优化版NMS
#ifdef OPTIMIZED_NMS
    int outNum = nms_optimized(gPostProcBuf, len, 5, nmsOut, 0.45, MAX_DETECT_NUM);
#else
    int outNum = nms(gPostProcBuf, len, 5, nmsOut, 0.45, MAX_DETECT_NUM);
#endif
    
	result->u32ObjNum = outNum;
	
    TS_ALG_CatDetect_GetParam(&det_param_cpp);
	
	int j = 0;
	
	for (int i = 0; i < outNum; i++) {
		if(gPostProcBuf[nmsOut[i] * 5 + 4] <= det_param_cpp.DetectionConfThres){
			continue;
		}
		
		// NMS后坐标已经是角点坐标（x1, y1, x2, y2），直接归一化
		result->stBox[j].f32Xmin = gPostProcBuf[nmsOut[i] * 5 + 0] / 640.0f;
		result->stBox[j].f32Ymin = (gPostProcBuf[nmsOut[i] * 5 + 1] - 12.0f) / 360.0f; // 12是填充的偏移
		result->stBox[j].f32Xmax = gPostProcBuf[nmsOut[i] * 5 + 2] / 640.0f;
		result->stBox[j].f32Ymax = (gPostProcBuf[nmsOut[i] * 5 + 3] - 12.0f) / 360.0f;
		
		// 确保归一化值在0-1范围内
		#define CLAMP_01(x) ((x) < 0.0f ? 0.0f : ((x) > 1.0f ? 1.0f : (x)))
		result->stBox[j].f32Xmin = CLAMP_01(result->stBox[j].f32Xmin);
		result->stBox[j].f32Ymin = CLAMP_01(result->stBox[j].f32Ymin);
		result->stBox[j].f32Xmax = CLAMP_01(result->stBox[j].f32Xmax);
		result->stBox[j].f32Ymax = CLAMP_01(result->stBox[j].f32Ymax);
		#undef CLAMP_01
		
        result->stBox[j].DetectionConf = gPostProcBuf[nmsOut[i] * 5 + 4];
		result->stBox[j].class_id = dataidx[nmsOut[i]] - 5;
		j++;
	}
	result->u32ObjNum = j;

#endif

	return 0;
}

TS_S32 TS_ALG_BodyDetect_SetParam(TS_VOID *handle, ALG_BODYDET_PARAM_S *param)
{
	if(TS_NULL == param) {
		ALG_LOGE("error, Invalid parameter!\n");
		return -1;
	}

	gThreshold = param->f32Thresh;
	ALG_LOGD("body detect thresh:%f\n", gThreshold);
	return 0;
}

TS_S32 TS_ALG_BodyDetect_GetParam(TS_VOID *handle, ALG_BODYDET_PARAM_S *param)
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
