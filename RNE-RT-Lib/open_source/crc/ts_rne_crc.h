#ifndef __TS_RNE_CRC_H__
#define __TS_RNE_CRC_H__

#include <stdint.h>
#include <ts_rne_device.h>
#include <ts_rne_type.h>

BEGIN_DECLS

TS_U16 RNE_CRC16CCITT(TS_U16 crc, const TS_U8 *ptr, TS_S32 len);
TS_U16 RNE_CRC16(const TS_U8 *ptr, TS_S32 len);
TS_U16 RNE_GetBlobsCRC16CCITT(RNE_BLOBS_S *blobs);

TS_U32 RNE_CRC32Part(TS_U32 crc32val, const TS_U8 *ptr, TS_SIZE_T len);
TS_U32 RNE_CRC32(const TS_U8 *ptr, TS_SIZE_T len);
TS_U32 RNE_GetBlobsCRC32(RNE_BLOBS_S *blobs);

END_DECLS

#endif
