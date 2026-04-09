#include <string.h>
#include <fstream>
#include <time.h>

#include "ts_rne_c_api.h"
#include "ts_rne_log.h"
#include "ts_rne_nn_input.h"
#include "ts_rne_time.h"
#include "ts_rne_version.h"
#include "ts_rne_gp_layers.h"

#include "../include/pre_post_process.h"

TS_S32 FillInputBlobs_BGR_OR_RGB_UINT8(RNE_NET_S *net,
                                       TS_S32 coreIdx, TS_VOID *input);
TS_S32 InitNetsGraphAndParams(RNE_NET_S **net,
                              TS_U8 **paramStride,
                              TS_S32 *paramSize,
                              TS_S32 nNet)
{
    TS_S32 ret = TS_MPI_TRP_RNE_OpenDevice(NULL, TS_MPI_TRP_RNE_RegisterGpLayers);
    if (ret) {
        TS_MPI_TRP_RNE_Error("open device error!\n");
        return ret;
    }
    for (TS_S32 i = 0; i < nNet; ++i) {
        ret = TS_MPI_TRP_RNE_LoadModel(net[i]);
        if (ret) {
            TS_MPI_TRP_RNE_Error("load model error!\n");
            return ret;
        }
        ret = TS_MPI_TRP_RNE_OnceLoad(net[i]);
        if (ret) {
            TS_MPI_TRP_RNE_Error("once load error!\n");
            return ret;
        }
    }

    return ret;
}


#include <ts_rne_record_file.h>


TS_S32 main(int argc, char* argv[])
{
    const char *cfg_ptr = argv[1];
    const char *weight_ptr = argv[2];
    const char *src_img_ptr = argv[3];
    char *dst_dir = argv[4];
    char dst_img_ptr[200];
    RNE_TYPE_NAME_E chip_type_e;
    chip_type_e = RNE_TYPE_NAME_TX5368AV200;
    TS_MPI_TRP_RNE_InitResourceByType(chip_type_e);
    TS_MPI_TRP_RNE_SetLogLevel(RNE_LOG_INFO);

    TS_MPI_TRP_RNE_Info("current log level: %d\n", TS_MPI_TRP_RNE_GetLogLevel());
    TS_MPI_TRP_RNE_Info("current lib version: %s\n",
                        TS_MPI_TRP_RNE_GetSdkVersion());
    TS_MPI_TRP_RNE_Info("main start...\n");

    memset(dst_img_ptr,0,sizeof(dst_img_ptr));
    sprintf(dst_img_ptr,"%syolov5_simlib_result.jpg",dst_dir);

    if (argc != 5) {
        TS_MPI_TRP_RNE_Info("Need 5 input Args, but get %d\n",argc);
        for(int i=0; i<argc; i++)
            printf("--->argc(%d) %d:%s\n",argc,i,argv[i]);
        return -1;
    }
    // 读取原始图片
    cv::Mat src_img = cv::imread(src_img_ptr);
    if (src_img.empty()) {
        TS_MPI_TRP_RNE_Info("input is error\n");
        return -1;
    }
    TS_MPI_TRP_RNE_Info("load input success\n");

    // 前处理
#ifdef LOG_TIME
    TS_U64 start = TS_MPI_TRP_RNE_GetSysTimeMs();
#endif
    cv::Mat pre_img;
    cv::Mat end_img;
    cv::Size padded_size = PreProcess(src_img, pre_img, end_img);


#ifdef LOG_TIME
    TS_U64 end = TS_MPI_TRP_RNE_GetSysTimeMs();
    TS_U64 time_diff = end - start;
#endif

    // 加载部署资源
    FILE *f_cfg = fopen(cfg_ptr, "rb");
    if (NULL == f_cfg) {
        TS_MPI_TRP_RNE_Error("Failed to open model_cfg\n");
        return false;
    }

    FILE *f_weight = fopen(weight_ptr, "rb");
    if (NULL == f_weight) {
        TS_MPI_TRP_RNE_Error("Failed to open model_weight\n");
        return false;
    }

    int u32CfgSize = GetFileSize(f_cfg);
    int u32WeightSize = GetFileSize(f_weight);
    TS_U8 *p_cfgBuffer = (TS_U8 *)calloc(u32CfgSize + 1, 1);
    if (p_cfgBuffer == NULL) {
        TS_MPI_TRP_RNE_Error("Failed to alloc model_cfg\n");
        fclose(f_cfg);
        return false;
    }
    TS_MPI_TRP_RNE_Info("load cfg of net success\n");

    TS_U8 *p_weightBuffer = (TS_U8 *)calloc(u32WeightSize + 1, 1);
    if (p_weightBuffer == NULL) {
        TS_MPI_TRP_RNE_Error("Failed to alloc model_weight\n");
        fclose(f_cfg);
        fclose(f_weight);
        return false;
    }
    TS_MPI_TRP_RNE_Info("load weight of net success\n");

    fread(p_cfgBuffer, 1, u32CfgSize, f_cfg);
    fread(p_weightBuffer, 1, u32WeightSize, f_weight);

    if (NULL != f_cfg) {
        fclose(f_cfg);
    }
    if (NULL != f_weight) {
        fclose(f_weight);
    }
    // 推理
    RNE_NET_S nnModel;
    memset(&nnModel, 0, sizeof(RNE_NET_S));
    nnModel.u8pGraph = p_cfgBuffer;
    nnModel.s32GraphLen = u32CfgSize;
    nnModel.u8pParams = (TS_U8 *)p_weightBuffer;
    nnModel.eInputType = RNE_NET_INPUT_TYPE_INT_HWC;
    RNE_NET_S *net[] = {&nnModel};
    const TS_S32 num = sizeof(net) / sizeof(net[0]);
    TS_S32 paramSize[] = {u32WeightSize};
    TS_U8 *paramStride[num];
    memset(paramStride, 0, sizeof(paramStride));
    if (0 !=
        InitNetsGraphAndParams(net, paramStride, paramSize, num)) {
        TS_MPI_TRP_RNE_Error("nets init error!\n");
        goto releaseResult;
    }
    for (TS_S32 n = 0; n < num; ++n) {
#ifdef LOG_TIME
        start = TS_MPI_TRP_RNE_GetSysTimeMs();
#endif
        RNE_BLOBS_S *inputBlobs = TS_MPI_TRP_RNE_GetInputBlobs(net[n]);
        if (NULL == inputBlobs) {
            TS_MPI_TRP_RNE_Error("inputBlobs is NULL!\n");
            goto releaseResult;
        }

        // use_int8_hwc
        if (0 != FillInputBlobs_BGR_OR_RGB_UINT8(net[n], 0,
                pre_img.data)) {
            TS_MPI_TRP_RNE_Error("fill inputBlobs error!\n");
            goto releaseResult;
        }

        RNE_BLOBS_S *outputBlobs = TS_MPI_TRP_RNE_Forward(net[n]);
        if (outputBlobs == NULL) {
            TS_MPI_TRP_RNE_Error("net forward error!\n");
            goto releaseResult;
        }
        RNE_RecordFile(outputBlobs,dst_dir,NULL,0,1);

#ifdef LOG_TIME
        end = TS_MPI_TRP_RNE_GetSysTimeMs();
        time_diff = end - start;
        TS_MPI_TRP_RNE_Info("The inference time: %llu ms\n", time_diff);
#endif

        // 后处理
#ifdef LOG_TIME
        start = TS_MPI_TRP_RNE_GetSysTimeMs();
#endif
        PostProcess(outputBlobs, src_img, std::string(dst_img_ptr),padded_size);

#ifdef LOG_TIME
        end = TS_MPI_TRP_RNE_GetSysTimeMs();
        time_diff = end - start;
        TS_MPI_TRP_RNE_Info("The post process time: %llu ms\n", time_diff);
#endif
    }

releaseResult:
    for (TS_S32 n = 0; n < num; ++n) {
        TS_MPI_TRP_RNE_Free(paramStride[n]);
        TS_MPI_TRP_RNE_UnloadModel(net[n]);
    }
    TS_MPI_TRP_RNE_CloseDevice();
    if (p_cfgBuffer != NULL) {
        free(p_cfgBuffer);
        p_cfgBuffer = NULL;
    }
    if (p_weightBuffer != NULL) {
        free(p_weightBuffer);
        p_weightBuffer = NULL;
    }
    TS_MPI_TRP_RNE_Info("program finished...\n");
    return 0;
}

#ifndef BLOB_ALIGN_BYTES
#define BLOB_ALIGN_BYTES (32 / 8)
#endif

TS_S32 FillInputBlobs_BGR_OR_RGB_UINT8(RNE_NET_S *net,
                                       TS_S32 coreIdx, TS_VOID *input)
{
    RNE_BLOBS_S *blobs = TS_MPI_TRP_RNE_GetInputBlobs(net);
    TS_U8 *src_ptr = (TS_U8 *)(input);
    for (uint32_t idx_blob = 0; idx_blob < blobs->u32NBlob; ++idx_blob) {
        RNE_BLOB_S *blob = &blobs->stpBlob[idx_blob];
        TS_S32 n = blob->s32N;
        TS_S32 w = blob->s32W;
        TS_S32 h = blob->s32H;
        TS_S32 c = blob->s32C;
        TS_S32 c_stride = TS_MPI_TRP_RNE_CStride(blob->s32C, blob->s32BitNum,
                          blob->bIsJoined);
        TS_U8 *dst = (TS_U8 *)(blob->vpAddr);
        TS_U8 *src = (TS_U8 *)src_ptr;
        for (TS_S32 bat = 0; bat < n; bat++) {
            for (TS_S32 i = 0; i < h * w; ++i) {
                memcpy(dst, src, c );
                *(dst + 0) = *(src + 2);
                *(dst + 1) = *(src + 1);
                *(dst + 2) = *(src + 0);
                src += c;
                dst += c_stride;
            }
            dst = (TS_U8 *)(((TS_SIZE_T)dst + BLOB_ALIGN_BYTES - 1) & (~((
                                TS_SIZE_T)BLOB_ALIGN_BYTES - 1)));
        }
        src_ptr += n * h * w * c;
    }
    return 0;
}

