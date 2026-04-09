#include <ts_rne_c_api.h>
#include <ts_rne_log.h>

void TS_MPI_TRP_RNE_GpLayerDeQuantization(RNE_BLOBS_S *input,
        RNE_BLOBS_S *output,
        RNE_BIN_DATA_S *extra_data,
        void *user_data)
{
    // 此软件层只支持作为最后输出层
    // 当前不支持64bit输出转float
    RNE_BLOB_S *inputBlob = input->stpBlob;
    RNE_BLOB_S *outputBlob = output->stpBlob;

    if (inputBlob->s32N != outputBlob->s32N || inputBlob->s32H != outputBlob->s32H
        ||
        inputBlob->s32W != outputBlob->s32W || inputBlob->s32C != outputBlob->s32C ||
        outputBlob->s32BitNum != TS_FLOAT_BIT_NUM) {
        TS_MPI_TRP_RNE_Error(
            "the input and output need to be the same [N:H:W:C:bitNum]:[%d:%d:%d:%d:%d][%d:%d:%d:%d:%d].\n",
            inputBlob->s32N, inputBlob->s32H, inputBlob->s32W, inputBlob->s32C,
            inputBlob->s32BitNum, outputBlob->s32N,
            outputBlob->s32H, outputBlob->s32W, outputBlob->s32C, outputBlob->s32BitNum);
    }

    if (inputBlob->eFormat == RNE_BLOB_N_Cn_N_H_W_Cx
        || outputBlob->eFormat == RNE_BLOB_N_Cn_N_H_W_Cx) {
        TS_MPI_TRP_RNE_Error("the format RNE_BLOB_N_Cn_N_H_W_Cx of gp layer is unsupport.");
    }

    TS_S32 cInStride = TS_MPI_TRP_RNE_GetBlobCStride(inputBlob);
    TS_S32 cOutStride = TS_MPI_TRP_RNE_GetBlobCStride(outputBlob);
    TS_S32 cIn = inputBlob->eFormat == RNE_BLOB_N_H_W_Cstride ? cInStride :
                 inputBlob->s32C;
    TS_S32 cOut = outputBlob->eFormat == RNE_BLOB_N_H_W_Cstride ? cOutStride :
                  outputBlob->s32C;
    TS_S32 uSize = inputBlob->s32BitNum / CHAR_BIT;
    TS_S32 c = inputBlob->s32C;
    TS_S32 h = inputBlob->s32H;
    TS_S32 w = inputBlob->s32W;
    TS_S32 num = inputBlob->s32N;
    TS_RNE_BOOL is_unsign = inputBlob->bIsUnsigned;
    TS_FLOAT coeff = *(outputBlob->fCoeff);
    TS_U8 *in = (TS_U8 *)(inputBlob->vpAddr);
    TS_FLOAT *out = (TS_FLOAT *)(outputBlob->vpAddr);

    for (TS_S32 n = 0; n < num; n++) {
        for (TS_S32 i = 0; i < h; i++) {
            for (TS_S32 j = 0; j < w; j++) {
                for (TS_S32 k = 0; k < c; k++) {
                    TS_S32 index = ((j + i * w) * cIn + k);
                    TS_S32 *d = (TS_S32 *)(in + (index * uSize));
                    TS_S32 data = *d & MASK_BITS(inputBlob->s32BitNum);
                    TS_S32 bitNum = (inputBlob->s32BitNum - 1);
                    TS_S32 bitValue = MASK_BITS(bitNum);
                    if (!is_unsign && data > bitValue) {
                        bitNum = inputBlob->s32BitNum;
                        bitValue = MASK_BITS(bitNum);
                        data = data - (bitValue + 1);
                    }
                    TS_S32 outdex = ((j + i * w) * cOut + k);
                    out[outdex] = data * coeff;
                }
            }
        }
        in += h * w * cInStride * uSize;
        out += h * w * cOutStride;
    }
    // 输出为浮点，q值必须为-1
    *(outputBlob->s32pQ) = -1;
}
