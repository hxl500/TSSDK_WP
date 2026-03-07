#ifndef __TS_ALG_SP_COMMON_H__
#define __TS_ALG_SP_COMMON_H__

#ifdef __cplusplus
	extern "C" {
#endif /* __cplusplus */

#include "ts_alg_type.h"

#define TS_ALG_SP_VER "tsalg_utilsp_v05"

typedef struct{
	TS_FLOAT f32Real;
	TS_FLOAT f32Imag;
}TS_COMPLEX_F32;

#ifdef UNADK
#define TS_ALG_SETLOGLEVEL log_set_level
#define TS_ALG_GETLOGLEVEL log_get_level
#else
#define TS_ALG_SETLOGLEVEL TS_ALG_SetLogLevel
#define TS_ALG_GETLOGLEVEL TS_ALG_GetLogLevel
#endif

#ifdef UNADK
#define TS_ALG_GETTIMEUS getSysTimeUs
#else
#define TS_ALG_GETTIMEUS TS_ALG_GetSysTimeUs
#endif

#ifdef UNADK
#include "crc32.h"
#define TS_ALG_CRCPART crc32part
#else
#include "ts_alg_sp_check.h"
#define TS_ALG_CRCPART TS_ALG_Crc32Part
#endif

#ifdef __cplusplus
	}
#endif /* __cplusplus */

#endif /* __TS_ALG_SP_COMMON_H__ */