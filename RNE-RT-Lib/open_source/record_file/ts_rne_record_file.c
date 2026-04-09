#include "ts_rne_record_file.h"

#include <stdio.h>
#include <string.h>
#include <ts_rne_log.h>
#include <ts_rne_nn_output.h>

static TS_CHAR s_temp_array[1024];

static TS_S32 DataAt(TS_U8 *addr, TS_SIZE_T index, RNE_BLOB_S *blob)
{
    TS_S32 u_size = blob->s32BitNum / CHAR_BIT;
    TS_S32 *d = (TS_S32 *)(addr + index * u_size);
    TS_S32 data = *d & MASK_BITS(blob->s32BitNum);
    TS_S32 bit_num = (blob->s32BitNum - 1);
    TS_S32 bit_value = MASK_BITS(bit_num);
    if (!blob->bIsUnsigned && data > bit_value) {
        bit_num = (blob->s32BitNum);
        bit_value = MASK_BITS(bit_num);
        data = data - (bit_value + 1);
    }
    return data;
}



#ifndef DISABLE_PRINT_DEBUG_LAYER
#define PRINT_DATA(file, ...)  printf(__VA_ARGS__); fprintf(file, __VA_ARGS__);
#else
#define PRINT_DATA(file, ...) fprintf(file, __VA_ARGS__);
#endif

static TS_VOID PrintHexData(FILE *file, RNE_BLOB_S *blob, TS_U8 *addr,
                            TS_SIZE_T index, TS_RNE_BOOL outMultByCoeff)
{
    if (*blob->s32pQ >= 0) {
        TS_S32 data = DataAt(addr, index, blob);
        if (!outMultByCoeff) {
            TS_S32 bit = (blob->s32BitNum + 3) / 4;
            TS_S32 t_data = data & MASK_BITS(blob->s32BitNum);
            PRINT_DATA(file, "%0*x,", bit, t_data);
        } else {
            TS_FLOAT f_data = data * (*blob->fCoeff);
            TS_U32 *t_data = (TS_U32 *)&f_data;
            PRINT_DATA(file, "%08x,", *t_data);
        }
    } else {
        TS_FLOAT f_data = *((TS_FLOAT *)addr + index);
        TS_U32 *t_data = (TS_U32 *)&f_data;
        PRINT_DATA(file, "%08x,", *t_data);
    }
}

static TS_VOID PrintDecData(FILE *file, RNE_BLOB_S *blob, TS_U8 *addr,
                            TS_SIZE_T index, TS_RNE_BOOL outMultByCoeff)
{
    if (*blob->s32pQ >= 0) {
        TS_S32 data = DataAt(addr, index, blob);
        if (!outMultByCoeff) {
            PRINT_DATA(file, "%0d,", data);
        } else {
            TS_FLOAT f_data = data * (*blob->fCoeff);
            PRINT_DATA(file, "%f,", f_data);
        }
    } else {
        TS_FLOAT f_data = *((TS_FLOAT *)addr + index);
        PRINT_DATA(file, "%f,", f_data);
    }
}

typedef TS_VOID (*PrintData)(FILE *, RNE_BLOB_S *, TS_U8 *, TS_SIZE_T,
                             TS_RNE_BOOL);

static TS_VOID DataToFile(FILE *file, RNE_BLOB_S *blob, TS_U8 *addr,
                          TS_SIZE_T index, TS_RNE_BOOL outMultByCoeff,
                          TS_RNE_BOOL is_hex_print)
{
    PrintData print_data_func[2] = {PrintDecData, PrintHexData};
    print_data_func[(TS_S32)is_hex_print](file, blob, addr, index, outMultByCoeff);
}

static TS_VOID ReplaceCToC(TS_CHAR *str, TS_CHAR frontC, TS_CHAR afterC)
{
    TS_CHAR *ptr = str;
    while (*ptr != '\0') {
        if (*ptr == frontC) *ptr = afterC;
        ptr++;
    }
}

static const TS_CHAR * GetRealBlobName(const TS_CHAR *name)
{
    if (!memcmp(name, "in@", 3)) {
        return &name[3];
    } else if (!memcmp(name, "cache@", 6)) {
        return &name[6];
    } else if (!memcmp(name, "out@", 4)) {
        return &name[4];
    }
    return name;
}

static TS_VOID MakeSureFileName(const TS_CHAR *outputPath, const TS_CHAR *name,
                                const TS_CHAR *hwc_name)
{
    memset(s_temp_array, 0, sizeof(s_temp_array));
    sprintf(s_temp_array, "%sresult-%s%s.txt", outputPath, name, hwc_name);
    ReplaceCToC(s_temp_array + strlen(outputPath), '/', '_');
}

TS_S32 BlobsToFileHwc(RNE_BLOBS_S *blobs, const TS_CHAR *outputPath,
                      const TS_CHAR *debugLayerName,
                      TS_RNE_BOOL outMultByCoeff, TS_S32 frameIndex, TS_S32 frameCount,
                      TS_RNE_BOOL is_debug_mode, TS_RNE_BOOL is_hex_print)
{
    TS_RNE_BOOL is_once_times = true;

    for (TS_U32 idx = 0; idx < blobs->u32NBlob; ++idx) {
        if (is_debug_mode && idx > 0 && blobs->stpBlob[idx].s32N > 1) {
            if (!is_debug_mode && (frameIndex == (frameCount - 1))) {
                TS_MPI_TRP_RNE_Info("Write file success: %s\n", s_temp_array);
            } else if ((idx + 1) == blobs->u32NBlob) {
                TS_MPI_TRP_RNE_Info("Write file success: %s\n", s_temp_array);
            }
            continue;
        }

        TS_S32 u_size = blobs->stpBlob[idx].s32BitNum / CHAR_BIT;
        TS_S32 c = blobs->stpBlob[idx].s32C;
        TS_S32 c_stride = TS_MPI_TRP_RNE_GetBlobCStride(&blobs->stpBlob[idx]);
        TS_S32 h = blobs->stpBlob[idx].s32H;
        TS_S32 w = blobs->stpBlob[idx].s32W;
        TS_S32 num = blobs->stpBlob[idx].s32N;
        const TS_CHAR *blob_name = blobs->stpBlob[idx].cpName;
        TS_U8 *out = (TS_U8 *)(blobs->stpBlob[idx].vpAddr);
        FILE *file = NULL;

        const TS_CHAR *name = is_debug_mode ? debugLayerName : GetRealBlobName(
                                  blob_name);
        const TS_CHAR *hwc_name = is_hex_print ? "_hwc" : "_hwc_p";
        MakeSureFileName(outputPath, name, hwc_name);

        if ((is_debug_mode && idx == 0) || (!is_debug_mode && frameIndex == 0)) {
            file = fopen(s_temp_array, "w");
        } else {
            file = fopen(s_temp_array, "a+");
        }
        if (!file) {
            TS_MPI_TRP_RNE_Error("can not open file %s.\n", s_temp_array);
            continue;
        }

        if (is_debug_mode && is_once_times) {
            TS_S32 merge_num = 0;
            for (TS_U32 i = 0; i < blobs->u32NBlob; ++i) {
                if (i > 0 && blobs->stpBlob[i].s32N > 1) {
                    continue;
                }
                merge_num += blobs->stpBlob[i].s32N;
            }
            PRINT_DATA(file, "SHAPE:(N:%d, H:%d, W:%d, C:%d)\n", merge_num, h, w, c);
            is_once_times = false;
        } else if (!is_debug_mode) {
            PRINT_DATA(file, "SHAPE:(N:%d, H:%d, W:%d, C:%d)\n", num, h, w, c);
        }

        TS_S32 c_cal = blobs->stpBlob[idx].eFormat == RNE_BLOB_N_H_W_C ? c : c_stride;

        for (TS_S32 n = 0; n < num; ++n) {
            for (TS_S32 i = 0; i < h; ++i) {
                for (TS_S32 j = 0; j < w; ++j) {
                    PRINT_DATA(file, "HW[%d,%d]:\n", i, j);
                    for (TS_S32 k = 0; k < c; k++) {
                        TS_SIZE_T out_idx = ((i * w + j) * c_cal + k);
                        DataToFile(file, &blobs->stpBlob[idx], out, out_idx, outMultByCoeff,
                                   is_hex_print);
                    }
                    PRINT_DATA(file, "\n");
                }
            }
            out += h * w * c_stride * u_size;
        }

        fclose(file);
        if (!is_debug_mode && (frameIndex == (frameCount - 1))) {
            TS_MPI_TRP_RNE_Info("Write file success: %s\n", s_temp_array);
        } else if ((idx + 1) == blobs->u32NBlob) {
            TS_MPI_TRP_RNE_Info("Write file success: %s\n", s_temp_array);
        }
    }
    return 0;
}

TS_S32 BlobsToFileChw(RNE_BLOBS_S *blobs, const TS_CHAR *outputPath,
                      const TS_CHAR *debugLayerName,
                      TS_RNE_BOOL outMultByCoeff, TS_S32 frameIndex, TS_S32 frameCount,
                      TS_RNE_BOOL is_debug_mode, TS_RNE_BOOL is_hex_print)
{
    TS_RNE_BOOL is_once_times = true;

    for (TS_U32 idx = 0; idx < blobs->u32NBlob; ++idx) {
        if (is_debug_mode && idx > 0 && blobs->stpBlob[idx].s32N > 1) {
            if (!is_debug_mode && (frameIndex == (frameCount - 1))) {
                TS_MPI_TRP_RNE_Info("Write file success: %s\n", s_temp_array);
            } else if ((idx + 1) == blobs->u32NBlob) {
                TS_MPI_TRP_RNE_Info("Write file success: %s\n", s_temp_array);
            }
            continue;
        }

        TS_S32 u_size = blobs->stpBlob[idx].s32BitNum / CHAR_BIT;
        TS_S32 c = blobs->stpBlob[idx].s32C;
        TS_S32 c_stride = TS_MPI_TRP_RNE_GetBlobCStride(&blobs->stpBlob[idx]);
        TS_S32 h = blobs->stpBlob[idx].s32H;
        TS_S32 w = blobs->stpBlob[idx].s32W;
        TS_S32 num = blobs->stpBlob[idx].s32N;
        const TS_CHAR *blob_name = blobs->stpBlob[idx].cpName;
        TS_U8 *out = (TS_U8 *)(blobs->stpBlob[idx].vpAddr);
        FILE *file = NULL;

        const TS_CHAR *name = is_debug_mode ? debugLayerName : GetRealBlobName(
                                  blob_name);
        const TS_CHAR *hwc_name = is_hex_print ? "" : "_p";
        MakeSureFileName(outputPath, name, hwc_name);

        if ((is_debug_mode && idx == 0) || (!is_debug_mode && frameIndex == 0)) {
            file = fopen(s_temp_array, "w");
        } else {
            file = fopen(s_temp_array, "a+");
        }
        if (!file) {
            TS_MPI_TRP_RNE_Error("can not open file %s.\n", s_temp_array);
            continue;
        }

        if (is_debug_mode && is_once_times) {
            TS_S32 merge_num = 0;
            for (TS_U32 i = 0; i < blobs->u32NBlob; ++i) {
                if (i > 0 && blobs->stpBlob[idx].s32N > 1) {
                    continue;
                }
                merge_num += blobs->stpBlob[i].s32N;
            }
            PRINT_DATA(file, "SHAPE:(N:%d, C:%d, H:%d, W:%d)\n", merge_num, c, h, w);
            is_once_times = false;
        } else if (!is_debug_mode) {
            PRINT_DATA(file, "SHAPE:(N:%d, C:%d, H:%d, W:%d)\n", num, c, h, w);
        }

        TS_S32 c_cal = blobs->stpBlob[idx].eFormat == RNE_BLOB_N_H_W_C ? c : c_stride;

        TS_FLOAT *out_f = (TS_FLOAT *)out;
        for (TS_S32 n = 0; n < num; ++n) {
            for (TS_S32 k = 0; k < c; k++) {
                PRINT_DATA(file, "C[%d]:\n", k);
                for (TS_S32 i = 0; i < h; ++i) {
                    for (TS_S32 j = 0; j < w; ++j) {
                        TS_SIZE_T out_idx = ((j + i * w) * c_cal + k);
                        DataToFile(file, &blobs->stpBlob[idx], out, out_idx, outMultByCoeff,
                                   is_hex_print);
                    }
                    PRINT_DATA(file, "\n");
                }
            }
            out += h * w * c_stride * u_size;
            out_f += h * w * c_stride;
        }
        fclose(file);

        if (!is_debug_mode && (frameIndex == (frameCount - 1))) {
            TS_MPI_TRP_RNE_Info("Write file success: %s\n", s_temp_array);
        } else if ((idx + 1) == blobs->u32NBlob) {
            TS_MPI_TRP_RNE_Info("Write file success: %s\n", s_temp_array);
        }
    }
    return 0;
}

static TS_S32 UpdateBlobs(RNE_BLOBS_S *blobs)
{
    RNE_BLOBS_S tmp_blobs;
    memset(&tmp_blobs, 0, sizeof(RNE_BLOBS_S));
    tmp_blobs.u32NBlob = blobs->u32NBlob;
    tmp_blobs.stpBlob = (RNE_BLOB_S *)TS_MPI_TRP_RNE_Alloc(sizeof(
                            RNE_BLOB_S) * blobs->u32NBlob);
    if (tmp_blobs.stpBlob == NULL) {
        TS_MPI_TRP_RNE_Error("insufficient memory!\n");
        return -1;
    }
    memset(tmp_blobs.stpBlob, 0, sizeof(RNE_BLOB_S) * blobs->u32NBlob);
    memcpy(tmp_blobs.stpBlob, blobs->stpBlob, sizeof(RNE_BLOB_S) * blobs->u32NBlob);
    for (TS_S32 i = 0; i < blobs->u32NBlob; ++i) {
        TS_S32 c_stride = TS_MPI_TRP_RNE_GetBlobCStride(&blobs->stpBlob[i]);
        TS_U32 u_size = blobs->stpBlob[i].s32BitNum / CHAR_BIT;
        TS_S32 blob_size = blobs->stpBlob[i].s32N * blobs->stpBlob[i].s32H *
                           blobs->stpBlob[i].s32W * c_stride * u_size;
        tmp_blobs.stpBlob[i].vpAddr = TS_MPI_TRP_RNE_Alloc(blob_size);
        if (tmp_blobs.stpBlob == NULL) {
            TS_MPI_TRP_RNE_Error("insufficient memory!\n");
            return -1;
        }
        memset(tmp_blobs.stpBlob[i].vpAddr, 0, blob_size);
        memcpy(tmp_blobs.stpBlob[i].vpAddr, blobs->stpBlob[i].vpAddr, blob_size);
    }

    if (0 != TS_MPI_TRP_RNE_FormatConversion(&tmp_blobs, blobs)) {
        return -1;
    }

    if (tmp_blobs.stpBlob != NULL) {
        for (TS_S32 i = 0; i < blobs->u32NBlob; ++i) {
            if (tmp_blobs.stpBlob[i].vpAddr != NULL) {
                TS_MPI_TRP_RNE_Free(tmp_blobs.stpBlob[i].vpAddr);
                tmp_blobs.stpBlob[i].vpAddr = NULL;
            }
        }
        TS_MPI_TRP_RNE_Free(tmp_blobs.stpBlob);
        tmp_blobs.stpBlob = NULL;
    }
    return 0;
}

TS_S32 RNE_RecordFile(RNE_BLOBS_S *blobs, TS_CHAR *outputPath,
                      TS_CHAR *debugLayerName, TS_S32 frameIndex,
                      TS_S32 frameCount)
{
    if (blobs == NULL) {
        return -1;
    }

    if (blobs->stpBlob->eFormat == RNE_BLOB_N_Cn_N_H_W_Cx) {
        // RNE_BLOB_N_Cn_N_H_W_Cx => RNE_BLOB_N_H_W_Cstride
        if (UpdateBlobs(blobs)) {
            return -1;
        }
    }

#if defined(RESULT_ALL) || defined(RESULT_HWC_P) || defined(RESULT_HWC) || defined(RESULT_CHW_P) || defined(RESULT_CHW)
    TS_RNE_BOOL is_debug_mode = false;
    if (debugLayerName != NULL) {
        is_debug_mode = true;
    }
#endif
    TS_S32 ret = 0;
#if defined(RESULT_ALL) || defined(RESULT_HWC_P)
    ret |= BlobsToFileHwc(blobs, outputPath, debugLayerName, false, frameIndex,
                          frameCount, is_debug_mode, true);
#endif
#if defined(RESULT_ALL) || defined(RESULT_HWC)
    ret |= BlobsToFileHwc(blobs, outputPath, debugLayerName, false, frameIndex,
                          frameCount, is_debug_mode, false);
#endif
#if defined(RESULT_ALL) || defined(RESULT_CHW_P)
    ret |= BlobsToFileChw(blobs, outputPath, debugLayerName, false, frameIndex,
                          frameCount, is_debug_mode, true);
#endif
#if defined(RESULT_ALL) || defined(RESULT_CHW)
    ret |= BlobsToFileChw(blobs, outputPath, debugLayerName, false, frameIndex,
                          frameCount, is_debug_mode, false);
#endif
    return ret;
}
