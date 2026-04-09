#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ts_rne_c_api.h>
#include <ts_rne_device.h>
#include <ts_rne_log.h>

/*
 *
 */
void TS_MPI_TRP_RNE_GpLayerCustormOperator(RNE_BLOBS_S *input,
        RNE_BLOBS_S *output, RNE_BIN_DATA_S *extraData,
        void *userData)
{
    // userData test
    if (NULL != userData) {
        TS_MPI_TRP_RNE_Info("UserData get <== %s\n", (TS_CHAR *)userData);
    }
    // prototxt 里面rce_param的值，float类型
    typedef struct {
        TS_S32 num;
        TS_FLOAT param1;
        TS_FLOAT param2;
    } ParamsData;
    ParamsData *pd = (ParamsData *)extraData->u8pData;
    if (pd != NULL) {
        TS_MPI_TRP_RNE_Info("params num=%d\n", pd->num);
        TS_MPI_TRP_RNE_Info("params%d=%f\n", 0, pd->param1);
        TS_MPI_TRP_RNE_Info("params%d=%f\n", 1, pd->param2);
    }

    // 打印详细输入输出信息
    TS_MPI_TRP_RNE_Info(
        "GpLayerCustormOperator input blobs: N:H:W:C[%d:%d:%d:%d] isUnsigned: %d "
        "isJoined: %d bitNum: %d q: %d coffe: %f\n",
        input->u32NBlob, input->stpBlob->s32H, input->stpBlob->s32W,
        input->stpBlob->s32C, input->stpBlob->bIsJoined,
        input->stpBlob->bIsUnsigned, input->stpBlob->s32BitNum,
        *(input->stpBlob->s32pQ), *(input->stpBlob->fCoeff));
    switch (input->stpBlob->eFormat) {
    case RNE_BLOB_N_H_W_Cstride:
        TS_MPI_TRP_RNE_Info("GpLayerCustormOperator input blobs format: RNE_BLOB_N_H_W_Cstride\n");
        break;
    case RNE_BLOB_N_Cn_N_H_W_Cx:
        TS_MPI_TRP_RNE_Info("GpLayerCustormOperator input blobs format: RNE_BLOB_N_Cn_N_H_W_Cx\n");
        break;
    case RNE_BLOB_N_H_W_C:
        TS_MPI_TRP_RNE_Info("GpLayerCustormOperator input blobs format: RNE_BLOB_N_H_W_C\n");
        break;
    default:
        TS_MPI_TRP_RNE_Info("GpLayerCustormOperator input blobs format: unknow!\n");
        break;
    }

    TS_MPI_TRP_RNE_Info(
        "GpLayerCustormOperator output blobs: N:H:W:C[%d:%d:%d:%d] isUnsigned: %d "
        "isJoined: %d bitNum: %d q: %d coffe: %f\n",
        output->u32NBlob, output->stpBlob->s32H, output->stpBlob->s32W,
        output->stpBlob->s32C,
        output->stpBlob->bIsJoined, output->stpBlob->bIsUnsigned,
        output->stpBlob->s32BitNum, *(output->stpBlob->s32pQ),
        *(output->stpBlob->fCoeff));
    switch (output->stpBlob->eFormat) {
    case RNE_BLOB_N_H_W_Cstride:
        TS_MPI_TRP_RNE_Info("GpLayerCustormOperator output blobs format: RNE_BLOB_N_H_W_Cstride\n");
        break;
    case RNE_BLOB_N_Cn_N_H_W_Cx:
        TS_MPI_TRP_RNE_Info("GpLayerCustormOperator output blobs format: RNE_BLOB_N_Cn_N_H_W_Cx\n");
        break;
    case RNE_BLOB_N_H_W_C:
        TS_MPI_TRP_RNE_Info("GpLayerCustormOperator output blobs format: RNE_BLOB_N_H_W_C\n");
        break;
    default:
        TS_MPI_TRP_RNE_Info("GpLayerCustormOperator output blobs format: unknow!\n");
        break;
    }

    // 当前测试的自定义算子层 输入输出RNE_BLOB_N_H_W_Cstride 且为8bit有符号
    TS_S32 size = 0;
    for (TS_S32 idx = 0; idx < input->u32NBlob; idx++) {
        size +=
            input->stpBlob->s32H * input->stpBlob->s32W * input->stpBlob->s32C *
            (input->stpBlob->s32BitNum / CHAR_BIT);
    }
    memcpy(output->stpBlob->vpAddr, input->stpBlob->vpAddr, size);
}
