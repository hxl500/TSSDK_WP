#ifndef __TS_ALG_RNE_INFO_GET_H__
#define __TS_ALG_RNE_INFO_GET_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include "ts_rne_c_api.h"
#include "ts_rne_device.h"

TS_S32 TS_ALG_RneGetBlobByeLenCalign(RNE_BLOB_S *blob);
TS_S32 TS_ALG_RneGetBlobDataLenCalign(RNE_BLOB_S *blob);
TS_S32 TS_ALG_RneGetBlobDataLen(RNE_BLOB_S *blob);
TS_S32 TS_ALG_RneGetBlobByteLen(RNE_BLOB_S *blob);
TS_S32 TS_ALG_RneGetBlobDataCalign(RNE_BLOB_S *blob, TS_U8 *out_dst);
TS_S32 TS_ALG_RneGetBlobsData(RNE_BLOBS_S * st, TS_S32 blob_no, TS_U8 *out_buff);
TS_S32 TS_ALG_RneGetBlobsDataOffset(RNE_BLOBS_S * st, TS_S32 blob_no, TS_S32 offset_data_n, TS_U8 *out_buff);
TS_S32 TS_ALG_RneGetBlobsDataLenOffset(RNE_BLOBS_S * st, TS_S32 blob_no, TS_S32 offset_data_n, TS_S32 out_data_length, TS_U8 *out_buff);
TS_S32 TS_ALG_RneFillBlobOffset(RNE_NET_S *net, RNE_BLOB_S *blob, void *input, int data_len, int data_offset);
TS_S32 TS_ALG_RneCleanBlobCache(RNE_BLOB_S *blob);
TS_S32 TS_ALG_RneInvalidBlobCache(RNE_BLOB_S *blob);

TS_S32 TS_ALG_GetCfgWgtByDir_Proc(const TS_CHAR *cfg_dir, const TS_CHAR *weight_dir, TS_U8 **cfg, TS_U8 **weight, TS_S32 *cfg_byte_n);
TS_S32 TS_ALG_GetCfgWgtByDir_Exit(TS_U8 *cfg, TS_U8 *weight);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TS_ALG_RNE_INFO_GET_H__ */