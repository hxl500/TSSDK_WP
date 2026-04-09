#include <string.h>

#include "ts_rne_c_api.h"
#include "ts_rne_log.h"

void *TS_MPI_TRP_RNE_DumpOutputBlob(RNE_NET_S *net, RNE_BLOB_S *blob,
                                    void *output)
{
    if (net == NULL || blob == NULL || output == NULL) {
        return NULL;
    }
    int c_stride = TS_MPI_TRP_RNE_GetBlobCStride(blob);
    int unit_size = blob->s32BitNum / CHAR_BIT;
    uint8_t *src = (uint8_t *)blob->vpAddr;
    uint8_t *dst = (uint8_t *)output;
    switch (net->eOutputType) {
    case RNE_NET_OUTPUT_TYPE_FORMAT_NHWC_TO_NCHW:
        for (int n = 0; n < blob->s32N; ++n) {
            for (int c = 0; c < blob->s32C; ++c) {
                for (int h = 0; h < blob->s32H; ++h) {
                    for (int w = 0; w < blob->s32W; ++w) {
                        int index_nhwc = unit_size * (n * blob->s32H * blob->s32W * c_stride + h *
                                                      blob->s32W * c_stride + w * c_stride + c);
                        int index_nchw = unit_size * (n * blob->s32C * blob->s32H * blob->s32W + c *
                                                      blob->s32H * blob->s32W + h * blob->s32W + w);
                        memcpy(&dst[index_nchw], &src[index_nhwc], unit_size);
                    }
                }
            }
        }
        break;
    case RNE_NET_OUTPUT_TYPE_FORMAT_NONE:
    default:
        for (int n = 0; n < blob->s32N; ++n) {
            for (int h = 0; h < blob->s32H; ++h) {
                for (int w = 0; w < blob->s32W; ++w) {
                    for (int c = 0; c < blob->s32C; ++c) {
                        int index_src = unit_size * (n * blob->s32H * blob->s32W * c_stride + h *
                                                     blob->s32W * c_stride + w * c_stride + c);
                        int index_dst = unit_size * (n * blob->s32H * blob->s32W * blob->s32C + h *
                                                     blob->s32W * blob->s32C + w * blob->s32C + c);
                        memcpy(&dst[index_dst], &src[index_src], unit_size);
                    }
                }
            }
        }
        break;
    }
    return output;
}

void *TS_MPI_TRP_RNE_DumpOutputBlobs(RNE_NET_S *net, void *output)
{
    if (net == NULL || output == NULL) {
        return NULL;
    }

    RNE_BLOBS_S *blobs = TS_MPI_TRP_RNE_GetResultBlobs(net);
    if (NULL == blobs) {
        TS_MPI_TRP_RNE_Error("result blobs is NULL!\n");
        return NULL;
    }

    uint8_t *dst = (uint8_t *)output;
    for (int index = 0; index < blobs->u32NBlob; ++index) {
        TS_MPI_TRP_RNE_DumpOutputBlob(net, &blobs->stpBlob[index], dst);
        int unit_size = blobs->stpBlob[index].s32BitNum / CHAR_BIT;
        int offset = blobs->stpBlob[index].s32N * blobs->stpBlob[index].s32H *
                     blobs->stpBlob[index].s32W * blobs->stpBlob[index].s32C * unit_size;
        dst += offset;
    }
    return output;
}
