/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: base type definitions of alg
 * Author: Tsing Micro solution-application group
 * Create: 2022/08/18
 */
#ifndef _TS_ALG_TYPE_H_
#define _TS_ALG_TYPE_H_

#ifdef __KERNEL__

#include <linux/types.h>
#else

#include <stdint.h>
#include <stdbool.h>

#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef unsigned char TS_U8;
typedef unsigned short TS_U16;
typedef unsigned int TS_U32;
typedef unsigned long TS_UL;
typedef uintptr_t TS_UINTPTR_T;

typedef char TS_CHAR;
typedef signed char TS_S8;
typedef short TS_S16;
typedef int TS_S32;
typedef long TS_SL;

typedef float TS_FLOAT;
typedef double TS_DOUBLE;

#ifndef _M_IX86
typedef unsigned long long TS_U64;
typedef long long TS_S64;
#else
typedef unsigned __int64 TS_U64;
typedef __int64 TS_S64;
#endif

typedef unsigned long TS_SIZE_T;
typedef unsigned long TS_LENGTH_T;
typedef unsigned long int TS_PHYS_ADDR_T;

typedef unsigned int TS_HANDLE;
//typedef bool TS_BOOL;

#if __has_include("ts_type.h")

#else

#ifndef TS_FALSE
#define TS_FALSE 0
#endif

#ifndef TS_TRUE
#define TS_TRUE 1
#endif

#endif

#ifndef NULL
#define NULL 0L
#endif

#ifndef TS_NULL
#define TS_NULL 0L
#endif

#ifndef TS_SUCCESS
#define TS_SUCCESS 0
#endif

#ifndef TS_FAILURE
#define TS_FAILURE (-1)
#endif

#ifndef TS_VOID
#define TS_VOID void
#endif

#ifndef TS_UNUSED
#define TS_UNUSED(x) ((x) = (x))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(a) ((a) >= (0) ? (a) : (-a))
#endif

#define ROUNDF(a) ((int)((a) + 0.5f))

#define ALG_FLT_EPSILON        1.192092896e-07F        // smallest such that 1.0+FLT_EPSILON != 1.0
#define ALG_INT_MIN            (-2147483647 - 1)
#define ALG_INT_MAX            2147483647
#define ALG_DBL_EPSILON        2.2204460492503131e-016
#define ALG_NAN                2.2204460492503131e+016
#define ALG_DBL_MIN            2.2250738585072014e-308 // min positive value
#define ALG_FLT_MIN            1.175494351e-38F        // min normalized positive value
#define ALG_PI                 3.14159265358979

#define RET_LEN 100

typedef struct tsALG_FLOAT_POINT_S{
    TS_FLOAT f32X;
    TS_FLOAT f32Y;
}ALG_FLOAT_POINT_S;

typedef struct tsALG_INT_POINT_S{
    TS_S32 s32X;
    TS_S32 s32Y;
}ALG_INT_POINT_S;

typedef struct tsALG_SIZE_S{
    TS_S32 s32Width;
    TS_S32 s32Height;
}ALG_SIZE_S;

typedef struct tsALG_IMAGE_S{
    TS_S32 s32H;
    TS_S32 s32W;
    TS_S32 s32C;
    TS_VOID *pData;
    TS_U64 pDataPhy;  
}ALG_IMAGE_S;

typedef struct tsALG_IMAGES_S{
    ALG_IMAGE_S images[2];
}ALG_IMAGES_S;

typedef struct tsALG_SUB_IMAGE_S{
    TS_S32 s32X;
    TS_S32 s32Y;
    TS_S32 s32W;
    TS_S32 s32H;
}ALG_SUB_IMAGE_S;

typedef enum tsALG_IMAGE_TYPE_E {
    ALG_IMAGE_TYPE_FLOAT_HWC = 0,
    ALG_IMAGE_TYPE_FLOAT_CHW = 1,
    ALG_IMAGE_TYPE_INT_HWC = 2,
    ALG_IMAGE_TYPE_INT_CHW = 3,
    ALG_IMAGE_TYPE_INT_HWC_BGR0 = 4,
    ALG_IMAGE_TYPE_INT_HWC_RGB0 = 5,
    ALG_IMAGE_TYPE_INT_HWC_GREY = 6,
    ALG_IMAGE_TYPE_INT_HWCSTRIDE = 7
} ALG_IMAGE_TYPE_E;


typedef struct tsALG_MODEL_INIT_S{
    TS_U8   *pGraph;                       // mode graph address
    TS_U8   *pWeight;                      // mode weight address

    TS_U64  pGraphPhy;                     // mode graph Physical address
    TS_U64  pWeightPhy;                    // mode weight Physical address

    TS_U32  u32GraphSize;				   // mode graph size
    TS_U32  u32WeightSize;				   // mode weight size
    ALG_IMAGE_TYPE_E eImageType;           // input image type (RGBA, gray, yuv ...)
    TS_VOID *vpSelfBuf;                    // blob mem
    TS_U8 bRneOff;					       // open or not open RNE device, revent multiple operations
} ALG_MODEL_INIT_S;

typedef struct tsALG_TEST_RET_S{
    TS_S32   s32Ret;         // return -1 fail 0 success
    TS_FLOAT u32TmElapsed;    // Performance time consumption
    TS_FLOAT u32TmLoadInput;
    TS_FLOAT u32TmRNEForward; 
    TS_FLOAT u32TmPostProcess;
    TS_FLOAT u32TmAlgProcess;
    TS_U32 u32RetNum;           // return result count
    TS_FLOAT CPU;           // %
    TS_U32 Mem;           // KB
    TS_U32 VMem;          // KB
    TS_CHAR pRetStr[RET_LEN];   // return string
}ALG_TEST_RET_S;



TS_U32 TS_ALG_GetSysTime();

TS_U32 TS_ALG_GetSysTimeUs(void);

#define TS_ALG_GET_TIME()    (TS_ALG_GetSysTime())        //ms
#define TS_ALG_GET_TIME_US()    (TS_ALG_GetSysTimeUs())   //us
#define TS_ALG_PRINT_TIME(msg, time)  printf(" %s: %d ms\r\n", (msg), time)


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
