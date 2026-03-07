#ifndef _TS_ALG_CHECK_H_
#define _TS_ALG_CHECK_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "ts_alg_type.h"

/**
 * @brief crc32part
 * @note  Continue CRC calculation on a part of the buffer.
 */
uint32_t TS_ALG_Crc32Part(const uint8_t *src, size_t len, uint32_t crc32val);


/**
 * @brief crc32
 * @return  return a 32-bit CRC of the contents of the 'src' buffer, length 'len'
 */
uint32_t TS_ALG_Crc32(const uint8_t *src, size_t len);

/**
 * @brief  lll...llrrr...rr <--> lrlrlr...lrlr
 * @note   mode: 0- interleave --> planar; 1 - planar --> interleave
 * @return  out_smple_num = floor(in_smpl_num / in_chn_num) * in_chn_num
 */
int TS_ALG_InterleavePlanarConvert(TS_S16 *in, TS_S32 chn_num, TS_S32 in_smpl_num, TS_S16 *out, TS_S32 *out_smpl_num, TS_S32 mode);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
