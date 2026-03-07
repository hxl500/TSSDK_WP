#ifndef TS_ALG_G726_H
#define TS_ALG_G726_H

#include <stdint.h>
#include "ts_alg_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum {
	ALG_G726_16K = 0, /* G726 16kbps, see RFC3551.txt 4.5.4 G726-16 */
	ALG_G726_24K, /* G726 24kbps, see RFC3551.txt 4.5.4 G726-24 */
	ALG_G726_32K, /* G726 32kbps, see RFC3551.txt 4.5.4 G726-32 */
	ALG_G726_40K, /* G726 40kbps, see RFC3551.txt 4.5.4 G726-40 */
	ALG_MEDIA_G726_16K, /* G726 16kbps for ASF ... */
	ALG_MEDIA_G726_24K, /* G726 24kbps for ASF ... */
	ALG_MEDIA_G726_32K, /* G726 32kbps for ASF ... */
	ALG_MEDIA_G726_40K, /* G726 40kbps for ASF ... */
	ALG_G726_BUTT,
} ALG_G726_BPS_T;

typedef struct{
	ALG_G726_BPS_T bps;
}ALG_G726_INIT_S;

typedef struct{
	TS_S16 *pSmpl;
	TS_U32 u32SmplLen;
}ALG_G726_SMPL_S;

typedef struct{
	TS_S8 *pStream;
	TS_U32 u32ByteLen;
} ALG_G726_STREAM_S;

TS_S32 TS_ALG_G726_Init(TS_VOID **ppHandle, ALG_G726_INIT_S *pParam);

TS_VOID TS_ALG_G726_Exit(TS_VOID **ppHandle);

TS_VOID TS_ALG_G726_Encode(TS_VOID *pHandle, ALG_G726_SMPL_S *pInput, ALG_G726_STREAM_S *pOutput);

TS_VOID TS_ALG_G726_Decode(TS_VOID *pHandle, ALG_G726_STREAM_S *pInput, ALG_G726_SMPL_S *pOutput);

const TS_CHAR *TS_ALG_G726_GetVersion(TS_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
