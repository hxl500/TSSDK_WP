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

// 定义一个函数，用于从文件中读取数据并分配内存保存数据
int getFileData(const char *file, int **data)
{
    FILE *f = fopen(file, "rb");
    if (!f) {
        return -1;
    }
    fseek(f, 0, SEEK_END);
    int fileSize = ftell(f);
    rewind(f);
    *data = (int *)TS_MPI_TRP_RNE_Alloc(fileSize);
    if (!(*data)) {
        fclose(f);
        return -1;
    }
    if (fread(*data, sizeof(uint8_t), fileSize, f) != fileSize) {
        fclose(f);
        return -1;
    }
    fclose(f);
    return 0;
}

RNE_NET_INPUT_TYPE_E getInputType(const char *inputDataFormat)
{
    if (strcmp(inputDataFormat, "int_hwc") == 0) {
        return RNE_NET_INPUT_TYPE_INT_HWC;
    } else if (strcmp(inputDataFormat, "int_chw") == 0) {
        return RNE_NET_INPUT_TYPE_INT_CHW;
    } else if (strcmp(inputDataFormat, "float_chw") == 0) {
        return RNE_NET_INPUT_TYPE_FLOAT_CHW;
    } else if (strcmp(inputDataFormat, "float_hwc") == 0) {
        return RNE_NET_INPUT_TYPE_FLOAT_HWC;
    } else if (strcmp(inputDataFormat, "bgr0") == 0) {
        return RNE_NET_INPUT_TYPE_INT_HWC_BGR0;
    } else if (strcmp(inputDataFormat, "rgb0") == 0) {
        return RNE_NET_INPUT_TYPE_INT_HWC_RGB0;
    } else if (strcmp(inputDataFormat, "hwc_grey") == 0) {
        return RNE_NET_INPUT_TYPE_INT_HWC_GREY;
    } else if (strcmp(inputDataFormat, "hwc_stride") == 0) {
        return RNE_NET_INPUT_TYPE_INT_HWCSTRIDE;
    } else {
        return -1; // Invalid input data format
    }
}

TS_S32 main(TS_S32 argc, TS_CHAR **argv)
{
    /* 1 设置芯片型号
     */
    TS_MPI_TRP_RNE_InitResourceByType(RNE_TYPE_NAME_TX5215CV200);
    /* 2 设置log等级（可选，默认RNE_LOG_ERROR）
     */
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

    // 绑定时间状态
    if (0 != TS_MPI_TRP_RNE_NetBindTimeState(net[0], &timeState)) {
        ret = -1;
        goto cleanup_model;
    }

    // 填充输入数据
    if (0 != TS_MPI_TRP_RNE_FillInputBlobs(net[0], 0, inputData)) {
        ret = -1;
        goto cleanup_model;
    }

    // 重置时间状态
    TS_MPI_TRP_RNE_ResetTimeState(&timeState);

    // 前向传播并获取输出结果
    RNE_BLOBS_S *outputBlobs = TS_MPI_TRP_RNE_Forward(net[0]);
    if (outputBlobs == NULL) {
        ret = -1;
        goto cleanup_model;
    }

    TS_U32 sLen = 0;
    for (TS_S32 i = 0; i < outputBlobs->u32NBlob; ++i) {
        sLen += outputBlobs->stpBlob[i].s32N * outputBlobs->stpBlob[i].s32H *
                outputBlobs->stpBlob[i].s32W *
                outputBlobs->stpBlob[i].s32C * outputBlobs->stpBlob[i].s32BitNum / CHAR_BIT;
    }

    TS_VOID *cpAddr = TS_MPI_TRP_RNE_Alloc(sLen);
    if (NULL == cpAddr) {
        TS_MPI_TRP_RNE_Error("insufficient memory!\n");
        goto cleanup_model;
    }
    /*  8 dump输出数据
     */
    TS_MPI_TRP_RNE_DumpOutputBlobs(net[0], cpAddr);
    TS_MPI_TRP_RNE_Info("TS_MPI_TRP_RNE_GetTotalTime     = %llu us\n",
                        TS_MPI_TRP_RNE_GetTotalTime(&timeState));
    TS_MPI_TRP_RNE_Info("TS_MPI_TRP_RNE_GetTimeOfGpLayer = %llu us\n",
                        TS_MPI_TRP_RNE_GetTimeOfGpLayer(&timeState));
    TS_MPI_TRP_RNE_Info("TS_MPI_TRP_RNE_GetTimeOfHwLayer = %llu us\n",
                        TS_MPI_TRP_RNE_GetTimeOfHwLayer(&timeState));
    TS_MPI_TRP_RNE_Info("outputBlobs crc = %04x\n",
                        RNE_GetBlobsCRC16CCITT(outputBlobs));

    RNE_RecordFile(outputBlobs, "./", NULL, 0, 1);
    TS_MPI_TRP_RNE_Info("net %d forward done\n", 0);

    TS_MPI_TRP_RNE_Free(cpAddr);

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
