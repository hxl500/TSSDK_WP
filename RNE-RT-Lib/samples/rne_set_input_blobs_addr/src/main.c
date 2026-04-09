#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <ts_rne_c_api.h>
#include <ts_rne_log.h>
#include <ts_rne_record_file.h>
#include <ts_rne_nn_input.h>
#include <ts_rne_nn_output.h>
#include <ts_rne_time.h>
#include <ts_rne_version.h>
#include <ts_rne_crc.h>
#include <ts_rne_gp_layers.h>
#include "ts_rne_get_file_data.h"
#include "ts_rne_get_input_type.h"

static TS_BOOL IsInputBlobFloat(RNE_BLOB_S *blob)
{
    if ((blob->fCoeff != NULL && (((*blob->fCoeff - 1) < (-EPSILON))
                                  || ((*blob->fCoeff - 1) > EPSILON))) ||
        (*blob->s32pQ == -1)) {
        return true;
    } else {
        return false;
    }
}

static inline TS_FLOAT TS_MPI_TRP_RNE_GetUnalignedAccess(TS_FLOAT *src)
{
    float data = 0;
    memcpy(&data, src, sizeof(TS_FLOAT));
    return data;
}

static inline TS_FLOAT TS_MPI_TRP_RNE_GetAlignedAccess(TS_FLOAT *src)
{
    return *src;
}

union RNE_NN_INPUT_U {
    TS_FLOAT fdata;
    TS_S32 data;
};
TS_FLOAT (*get_float_fuc[2])(TS_FLOAT *) = {TS_MPI_TRP_RNE_GetAlignedAccess, TS_MPI_TRP_RNE_GetUnalignedAccess};

TS_S32 CHW_TO_HWCSTRIDE(RNE_NET_S *net, TS_VOID *in, TS_S32 coreIdx,
                        TS_VOID *input)
{
    TS_U8 *src_ptr = (TS_U8 *)(input);
    RNE_BLOBS_S *blobs = TS_MPI_TRP_RNE_GetInputBlobs(net);
    if (NULL == blobs) {
        TS_MPI_TRP_RNE_Error("input blobs is NULL!\n");
        return -1;
    }
    for (uint32_t idx_blob = 0; idx_blob < blobs->u32NBlob; ++idx_blob) {
        RNE_BLOB_S *blob = &blobs->stpBlob[idx_blob];
        TS_S32 n = blob->s32N;
        TS_S32 w = blob->s32W;
        TS_S32 h = blob->s32H;
        TS_S32 c = blob->s32C;
        TS_S32 q_scale = *blob->s32pQ;
        TS_S32 scale_int = *blob->fCoeff * (1LL << q_scale);
        TS_S32 c_stride = TS_MPI_TRP_RNE_GetBlobCStride(blob);
        TS_S32 unit_size = blob->s32BitNum / CHAR_BIT;
        TS_S32 bits = blob->bIsUnsigned ? blob->s32BitNum + 1 : blob->s32BitNum;
        TS_S64 data_max = ~MIN_BIT(bits - 1);
        TS_S32 sign_bit = sizeof(TS_S64) * CHAR_BIT - blob->s32BitNum;
        TS_S64 data_min = ((TS_S64)(MIN_BIT(bits - 1) & MASK_BITS(
                                        blob->s32BitNum)) << sign_bit) >> sign_bit;
        TS_S64 scale_max = ~MIN_BIT(16 - 1);
        TS_S64 scale_min = MIN_BIT(16 - 1);
        TS_S64 scale64_max = ~MIN_BIT(64 - 1);
        TS_S64 scale64_min = MIN_BIT(64 - 1);
        scale_int = scale_int > scale_max ? scale_max : scale_int;
        scale_int = scale_int < scale_min ? scale_min : scale_int;

        TS_U8 *dst = (TS_U8 *)(in);
        TS_S32 in_type = RNE_NET_INPUT_TYPE_INT_CHW;

        if (in_type == RNE_NET_INPUT_TYPE_FLOAT_HWC && !IsInputBlobFloat(blob)) {
            in_type = RNE_NET_INPUT_TYPE_INT_HWC;
        } else if (in_type == RNE_NET_INPUT_TYPE_FLOAT_CHW && !IsInputBlobFloat(blob)) {
            in_type = RNE_NET_INPUT_TYPE_INT_CHW;
        }

        switch (in_type) {
        case RNE_NET_INPUT_TYPE_FLOAT_HWC: {
            TS_FLOAT *src = (TS_FLOAT *)src_ptr;
            if (q_scale == -1) {
                memcpy(dst, src, n * h * w * c * unit_size);
            } else {
                TS_S32 access_index = (((size_t)src) / sizeof(float) == 0) ? 0 : 1;
                for (TS_S32 bat = 0; bat < n; bat++) {
                    for (TS_S32 i = 0; i < h * w; ++i) {
                        for (TS_S32 k = 0; k < c; ++k) {
                            union RNE_NN_INPUT_U tmp1;
                            tmp1.fdata = get_float_fuc[access_index](src++);
                            TS_DOUBLE tmp = (TS_DOUBLE)(tmp1.fdata) * scale_int;
                            tmp = tmp > scale64_max ? scale64_max : tmp;
                            tmp = tmp < scale64_min ? scale64_min : tmp;
                            TS_S64 data = 0;
                            if (blob->s32BitNum == 8) {
                                tmp /= (1 << q_scale);
                                data = ((tmp1.data | (1 << 30)) >> 30) * 0.5f + tmp;
                            } else {
                                data = tmp / (1 << q_scale);
                            }
                            data = data > data_max ? data_max : data;
                            data = data < data_min ? data_min : data;

                            for (TS_S32 u = 0; u < unit_size; ++u) {
                                dst[k * unit_size + u] = data >> (u * CHAR_BIT);
                            }
                        }
                        dst += c_stride * unit_size;
                    }
                    dst = (TS_U8 *)(((TS_SIZE_T)dst + BLOB_ALIGN_BYTES - 1) & (~((
                                        TS_SIZE_T)BLOB_ALIGN_BYTES - 1)));
                }
            }
            src_ptr += n * h * w * c * sizeof(TS_FLOAT);
            break;
        }
        case RNE_NET_INPUT_TYPE_FLOAT_CHW: {
            TS_FLOAT *src = (TS_FLOAT *)src_ptr;
            if (q_scale == -1) {
                TS_FLOAT *dst_float = (TS_FLOAT *)dst;
                TS_S32 idx = 0;
                for (TS_S32 bat = 0; bat < n; ++bat) {
                    for (TS_S32 i = 0; i < h; ++i) {
                        for (TS_S32 j = 0; j < w; ++j) {
                            for (TS_S32 k = 0; k < c; ++k) {
                                dst_float[idx++] = src[(k * h + i) * w + j];
                            }
                        }
                    }
                    src += h * w * c;
                }
            } else {
                TS_S32 access_index = (((size_t)src) / sizeof(float) == 0) ? 0 : 1;
                for (TS_S32 bat = 0; bat < n; bat++) {
                    for (TS_S32 i = 0; i < h; ++i) {
                        for (TS_S32 j = 0; j < w; ++j) {
                            for (TS_S32 k = 0; k < c; ++k) {
                                union RNE_NN_INPUT_U tmp1;
                                tmp1.fdata = get_float_fuc[access_index](src++);
                                TS_DOUBLE tmp = (TS_DOUBLE)(tmp1.fdata) * scale_int;
                                tmp = tmp > scale64_max ? scale64_max : tmp;
                                tmp = tmp < scale64_min ? scale64_min : tmp;
                                TS_S64 data = 0;
                                if (blob->s32BitNum == 8) {
                                    tmp /= (1 << q_scale);
                                    data = ((tmp1.data | (1 << 30)) >> 30) * 0.5f + tmp;
                                } else {
                                    data = tmp / (1 << q_scale);
                                }
                                data = data > data_max ? data_max : data;
                                data = data < data_min ? data_min : data;

                                for (TS_S32 u = 0; u < unit_size; ++u) {
                                    dst[k * unit_size + u] = data >> (u * CHAR_BIT);
                                }
                            }
                            dst += c_stride * unit_size;
                        }
                    }
                    dst = (TS_U8 *)(((TS_SIZE_T)dst + BLOB_ALIGN_BYTES - 1) & (~((
                                        TS_SIZE_T)BLOB_ALIGN_BYTES - 1)));
                    src += h * w * c;
                }
            }
            src_ptr += n * h * w * c * sizeof(TS_FLOAT);
            break;
        }

        case RNE_NET_INPUT_TYPE_INT_HWC: {
            TS_U8 *src = (TS_U8 *)src_ptr;
            for (TS_S32 bat = 0; bat < n; bat++) {
                for (TS_S32 i = 0; i < h * w; ++i) {
                    memcpy(dst, src, c * unit_size);
                    src += c * unit_size;
                    dst += c_stride * unit_size;
                }
                dst = (TS_U8 *)(((TS_SIZE_T)dst + BLOB_ALIGN_BYTES - 1) & (~((
                                    TS_SIZE_T)BLOB_ALIGN_BYTES - 1)));
            }
            src_ptr += n * h * w * c * unit_size;
            break;
        }
        case RNE_NET_INPUT_TYPE_INT_CHW: {
            TS_U8 *src = (TS_U8 *)src_ptr;
            for (TS_S32 bat = 0; bat < n; bat++) {
                for (TS_S32 i = 0; i < h; ++i) {
                    for (TS_S32 j = 0; j < w; ++j) {
                        for (TS_S32 k = 0; k < c; ++k) {
                            for (TS_S32 u = 0; u < unit_size; ++u) {
                                TS_S32 src_idx = ((k * h + i) * w + j) * unit_size + u;
                                dst[k * unit_size + u] = src[src_idx];
                            }
                        }
                        dst += c_stride * unit_size;
                    }
                }
                dst = (TS_U8 *)(((TS_SIZE_T)dst + BLOB_ALIGN_BYTES - 1) & (~((
                                    TS_SIZE_T)BLOB_ALIGN_BYTES - 1)));
                src += h * w * c * unit_size;
            }
            src_ptr += n * h * w * c * unit_size;
            break;
        }
        case RNE_NET_INPUT_TYPE_INT_HWC_BGR0:
        case RNE_NET_INPUT_TYPE_INT_HWC_RGB0:
        case RNE_NET_INPUT_TYPE_INT_HWC_GREY:
        case RNE_NET_INPUT_TYPE_INT_HWCSTRIDE: {
            TS_U8 *src = (TS_U8 *)src_ptr;
            memcpy(dst, src, n * h * w * c_stride * unit_size);
            src_ptr += n * h * w * c_stride * unit_size;
            break;
        }
        default:
            TS_MPI_TRP_RNE_Error("unsupport inputype:%d\n", net->eInputType);
            return -1;
            break;
        }
    }
    return 0;
}

TS_S32 main(TS_S32 argc, TS_CHAR **argv)
{
    TS_MPI_TRP_RNE_InitResourceByType(RNE_TYPE_NAME_TX5215CV200);

    TS_MPI_TRP_RNE_SetLogLevel(RNE_LOG_INFO);

    TS_MPI_TRP_RNE_Info("current log level : %d\n", TS_MPI_TRP_RNE_GetLogLevel());
    TS_MPI_TRP_RNE_Info("current lib version : %s\n",
                        TS_MPI_TRP_RNE_GetSdkVersion());
    TS_MPI_TRP_RNE_Info("main start...\n");

    if (argc < 5) {
        printf("Usage: %s <cfgFile> <weightFile> <inputDataFile> <inputDataFormat>\n",
               argv[0]);
        return 0;
    }

    int ret = 0;
    char *cfgFile = argv[1];
    char *weightFile = argv[2];
    char *inputDataFile = argv[3];
    char *inputDataFormat = argv[4];

    int *cfgData = NULL;
    int *weightData = NULL;
    int *inputData = NULL;

    // 读取配置文件数据
    if ((ret = getFileData(cfgFile, &cfgData)) == -1) {
        goto cleanup;
    }

    // 读取权重文件数据
    if ((ret = getFileData(weightFile, &weightData)) == -1) {
        goto cleanup;
    }

    // 读取输入数据文件
    if ((ret = getFileData(inputDataFile, &inputData)) == -1) {
        goto cleanup;
    }

    // 初始化 RNE 资源和日志级别
    TS_MPI_TRP_RNE_SetLogLevel(RNE_LOG_INFO);

    // 初始化时间状态
    RNE_TIME_STATES_S timeState;
    memset(&timeState, 0, sizeof(RNE_TIME_STATES_S));
    TS_MPI_TRP_RNE_InitTimeState(RNE_LAYER_TYPE_MAX_LAYER_TYPE -
                                 RNE_LAYER_TYPE_START_LAYER, &timeState);

    // 设置神经网络模型
    RNE_NET_S nnModel;
    memset(&nnModel, 0, sizeof(RNE_NET_S));
    nnModel.u8pGraph = (TS_U8 *)cfgData;
    nnModel.u8pParams = (TS_U8 *)weightData;
    nnModel.eInputType = getInputType(inputDataFormat);
    nnModel.eOutputType = RNE_NET_OUTPUT_TYPE_FORMAT_NONE;
    RNE_NET_S *net[] = {&nnModel};

    // 打开设备并注册 GP 层
    if ((ret = TS_MPI_TRP_RNE_OpenDevice(NULL,
                                         TS_MPI_TRP_RNE_RegisterGpLayers)) != 0) {
        goto cleanup_device;
    }

    // 加载模型
    if ((ret = TS_MPI_TRP_RNE_LoadModel(net[0])) != 0) {
        goto cleanup_model;
    }

    // 加载模型数据
    if ((ret = TS_MPI_TRP_RNE_OnceLoad(net[0])) != 0) {
        goto cleanup_model;
    }

    //获取inputblobsize
    RNE_BLOBS_S *inputBlobs = TS_MPI_TRP_RNE_GetInputBlobs(net[0]);
    if (NULL == inputBlobs) {
        TS_MPI_TRP_RNE_Error("inputBlobs is NULL!\n");
        goto cleanup_model;
    }
    TS_U32 inputLen = 0;
    for (TS_S32 j = 0; j < inputBlobs->u32NBlob; ++j) {
        inputLen += inputBlobs->stpBlob[j].u32Size;
    }
    // PC端申请为虚拟地址，只有在板端才有意义
    TS_VOID *in = TS_MPI_TRP_RNE_AllocLinearMemCached(inputLen, 1);
    if (in == NULL) {
        TS_MPI_TRP_RNE_Error("not enough memory!\n");
        goto cleanup_in;
    }

    // 绑定时间状态
    if (0 != TS_MPI_TRP_RNE_NetBindTimeState(net[0], &timeState)) {
        ret = -1;
        goto cleanup_in;
    }
    CHW_TO_HWCSTRIDE(net[0], in, 0, inputData);

    TS_MPI_TRP_RNE_ResetTimeState(&timeState);

    if (0 != TS_MPI_TRP_RNE_SetInputBlobsAddr(net[0], in,
            (TS_VOID *)TS_MPI_TRP_RNE_VirtualToPhysicalAddress((TS_SIZE_T)in))) {
        TS_MPI_TRP_RNE_Error("set input blobs addr error!\n");
        goto cleanup_in;
    }

    // 前向传播并获取输出结果
    RNE_BLOBS_S *outputBlobs = TS_MPI_TRP_RNE_Forward(net[0]);
    if (outputBlobs == NULL) {
        ret = -1;
        goto cleanup_in;
    }

    TS_SIZE_T sLen = 0;
    for (TS_S32 i = 0; i < outputBlobs->u32NBlob; ++i) {
        sLen += outputBlobs->stpBlob[i].u32Size;
    }

    TS_MPI_TRP_RNE_Info("TS_MPI_TRP_RNE_GetTotalTime     = %llu us\n",
                        TS_MPI_TRP_RNE_GetTotalTime(&timeState));
    TS_MPI_TRP_RNE_Info("TS_MPI_TRP_RNE_GetTimeOfGpLayer = %llu us\n",
                        TS_MPI_TRP_RNE_GetTimeOfGpLayer(&timeState));
    TS_MPI_TRP_RNE_Info("TS_MPI_TRP_RNE_GetTimeOfHwLayer = %llu us\n",
                        TS_MPI_TRP_RNE_GetTimeOfHwLayer(&timeState));
    TS_MPI_TRP_RNE_Info("outputBlobs crc = %04x\n",
                        RNE_GetBlobsCRC16CCITT(outputBlobs));

    TS_MPI_TRP_RNE_Info("net %d forward done\n", 0);

cleanup_in:
    TS_MPI_TRP_RNE_FreeLinearMemCached(in);

cleanup_model:
    // 卸载模型
    TS_MPI_TRP_RNE_UnloadModel(net[0]);
cleanup_device:
    // 关闭设备
    TS_MPI_TRP_RNE_CloseDevice();
    // 释放时间状态
    TS_MPI_TRP_RNE_ReleaseTimeState(&timeState);
cleanup:
    // 释放内存
    TS_MPI_TRP_RNE_Free(cfgData);
    TS_MPI_TRP_RNE_Free(weightData);
    TS_MPI_TRP_RNE_Free(inputData);

    TS_MPI_TRP_RNE_Info("program finished...\n");
    return 0;
}
