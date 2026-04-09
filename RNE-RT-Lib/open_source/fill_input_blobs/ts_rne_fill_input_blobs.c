#include <string.h>

#include "ts_rne_log.h"
#include "ts_rne_c_api.h"

static TS_BOOL TS_MPI_TRP_RNE_InputBlobIsFloat(RNE_BLOB_S *blob)
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

TS_S32 TS_MPI_TRP_RNE_FillInputBlobs(RNE_NET_S *net, TS_S32 coreIdx,
                                     TS_VOID *input)
{
    TS_MPI_TRP_RNE_Info("TS_MPI_TRP_RNE_FillInputBlobs is open!\n");
    TS_U8 *src_ptr = (TS_U8 *)(input);
    RNE_BLOBS_S* blobs = TS_MPI_TRP_RNE_GetInputBlobs(net);
    if (NULL == blobs) {
        TS_MPI_TRP_RNE_Error("input blobs is NULL!\n");
        return -1;
    }

    TS_FLOAT (*get_float_fuc[2])(TS_FLOAT *) = {TS_MPI_TRP_RNE_GetAlignedAccess, TS_MPI_TRP_RNE_GetUnalignedAccess};
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

        TS_U8 *dst = (TS_U8 *)(blob->vpAddr);
        TS_S32 in_type = net->eInputType;

        if (in_type == RNE_NET_INPUT_TYPE_FLOAT_HWC
            && !TS_MPI_TRP_RNE_InputBlobIsFloat(blob)) {
            in_type = RNE_NET_INPUT_TYPE_INT_HWC;
        } else if (in_type == RNE_NET_INPUT_TYPE_FLOAT_CHW
                   && !TS_MPI_TRP_RNE_InputBlobIsFloat(blob)) {
            in_type = RNE_NET_INPUT_TYPE_INT_CHW;
        }

        switch (in_type) {
        case RNE_NET_INPUT_TYPE_FLOAT_HWC: {
            TS_FLOAT *src = (TS_FLOAT *)src_ptr;
            if (q_scale == -1) {
                memcpy(dst, src, n * h * w * c * unit_size);
            } else {
                TS_S32 access_index = (((size_t)src) / sizeof(float) == 0)? 0 : 1;
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
            int access_index = (((size_t)src) / sizeof(float) == 0)? 0 : 1;
            if (q_scale == -1) {
                TS_FLOAT *dst_float = (TS_FLOAT *)dst;
                TS_S32 idx = 0;
                for (TS_S32 bat = 0; bat < n; ++bat) {
                    for (TS_S32 i = 0; i < h; ++i) {
                        for (TS_S32 j = 0; j < w; ++j) {
                            for (TS_S32 k = 0; k < c; ++k) {
                                dst_float[idx++] = get_float_fuc[access_index](&src[(k * h + i) * w + j]);
                            }
                        }
                    }
                    src += h * w * c;
                }
            } else {
                for (TS_S32 bat = 0; bat < n; bat++) {
                    for (TS_S32 i = 0; i < h; ++i) {
                        for (TS_S32 j = 0; j < w; ++j) {
                            for (TS_S32 k = 0; k < c; ++k) {
                                TS_S64 data = 0;
                                union RNE_NN_INPUT_U tmp1;
                                tmp1.fdata = get_float_fuc[access_index](&src[(k * h + i) * w + j]);
                                TS_DOUBLE tmp = (TS_DOUBLE)(tmp1.fdata) * scale_int;
                                tmp = tmp > scale64_max ? scale64_max : tmp;
                                tmp = tmp < scale64_min ? scale64_min : tmp;
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
