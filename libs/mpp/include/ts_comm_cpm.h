/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_comm_cpm.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */
#ifndef __TS_COMM_CPM_H__
#define __TS_COMM_CPM_H__

#include "ts_type.h"
#include "ts_common.h"
#include "ts_errno.h"
#include "ts_comm_video.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* failure caused by malloc buffer */
#define TS_ERR_CPM_NOBUF         TS_DEF_ERR(TS_ID_CPM, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define TS_ERR_CPM_BUF_EMPTY     TS_DEF_ERR(TS_ID_CPM, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
#define TS_ERR_CPM_NULL_PTR      TS_DEF_ERR(TS_ID_CPM, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define TS_ERR_CPM_ILLEGAL_PARAM TS_DEF_ERR(TS_ID_CPM, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define TS_ERR_CPM_BUF_FULL      TS_DEF_ERR(TS_ID_CPM, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
#define TS_ERR_CPM_SYS_NOTREADY  TS_DEF_ERR(TS_ID_CPM, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define TS_ERR_CPM_NOT_SUPPORT   TS_DEF_ERR(TS_ID_CPM, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define TS_ERR_CPM_NOT_PERMITTED TS_DEF_ERR(TS_ID_CPM, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define TS_ERR_CPM_BUSY          TS_DEF_ERR(TS_ID_CPM, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define TS_ERR_CPM_INVALID_DEVID TS_DEF_ERR(TS_ID_CPM, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define TS_ERR_CPM_INVALID_CHNID TS_DEF_ERR(TS_ID_CPM, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define TS_ERR_CPM_CHN_UNEXIST   TS_DEF_ERR(TS_ID_CPM, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define TS_ERR_CPM_COMP_INTERNAL    TS_DEF_ERR(TS_ID_CPM, EN_ERR_LEVEL_ERROR, EN_ERR_COMP_INTERNAL)
#define TS_ERR_CPM_NOMEM          TS_DEF_ERR(TS_ID_CPM, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define TS_ERR_CPM_COMP_INIT      TS_DEF_ERR(TS_ID_CPM, EN_ERR_LEVEL_ERROR, EN_ERR_COMP_INIT)
#define TS_ERR_CPM_COMP_GET       TS_DEF_ERR(TS_ID_CPM, EN_ERR_LEVEL_ERROR, EN_ERR_COMP_GET)

#define CPM_MAX_GRP_NUM          8
#define CPM_MAX_PIPE_NUM          4
#define CPM_MAX_CHN_NUM          2

//typedef TS_S32 CPM_GRP;


/* 0: stop; 1：start; 2.bypass;*/
typedef enum tsCPM_WORK_STATUS_E {
    TS_CPM_STATUS_STOP    = 0,
    TS_CPM_STATUS_START   = 1,
    TS_CPM_STATUS_BYPASS  = 2,
} CPM_WORK_STATUS_E;

/* type of CPM regions */
typedef enum tsCPM_DATA_TYPE_E {
    CPM_FMT_VIDEO = 0, /* video Format */
    CPM_FMT_AUDIO, /* Audio Format */
    CPM_FMT_BUTT
} CPM_DATA_TYPE_E;

typedef enum tsCPM_SOUND_MODE_E {
    CPM_AUDIO_SOUND_MODE_MONO   = 0, /* mono */
    CPM_AUDIO_SOUND_MODE_STEREO = 1, /* stereo */
    CPM_AUDIO_SOUND_MODE_BUTT
} CPM_AUDIO_SOUND_MODE_E;

typedef union tsCPM_DATA_FORMAT_U {
	PIXEL_FORMAT_E enPixelFormat;     /* cpm video pixel format */
	CPM_AUDIO_SOUND_MODE_E enAudioMode; /* cpm audio pixel format  */
} CPM_DATA_FORMAT_U;

typedef struct _tsCPM_FMT_T {
	CPM_DATA_TYPE_E enType; /* cpm type */
	CPM_DATA_FORMAT_U unFormat; /* region attribute */
}CPM_FMT_T;

typedef struct tsCPM_CHN_ATTR_S
{
    TS_U32              u32Width;
    TS_U32              u32Height;
	PIXEL_FORMAT_E       enPixelFormat;     /* cpm video pixel format */
    TS_U32              u32Depth;           /* RW; Range: [0, 8]; Chn user list depth. */
}CPM_CHN_ATTR_S;

typedef struct tsCPM_PIPE_ATTR_S
{
    TS_U32              u32Width;
    TS_U32              u32Height;
	PIXEL_FORMAT_E       enPixelFormat;     /* cpm video pixel format */
}CPM_PIPE_ATTR_S;


typedef struct _tsCPM_GRP_ATTR_S {
    TS_U32 u32GrpId;         //group_id
    TS_U32 u32PipeNum;   //input port: pipe_num
    TS_U32 u32ChnNum; //output port: channel_num
    TS_U32 u32Interval; //run cpm algorithm interval, 0: every frame. N: every N frame run one time.
    CPM_CHN_ATTR_S  stChnAttr[CPM_MAX_CHN_NUM];
    TS_BOOL bSyncPipe;
    TS_U32 u32Tolerance;
}CPM_GRP_ATTR_S;


typedef struct _tsCPM_Handle_S {
    TS_S32  s32PipeNum;
    CPM_PIPE_ATTR_S  stPipeAttr[CPM_MAX_PIPE_NUM];
    TS_VOID *pHandle;
    TS_S32  (*init)(TS_VOID **pHandle);
    TS_VOID (*exit)(TS_VOID *pHandle);
    TS_S32  (*process)(TS_VOID *pHandle, TS_VOID **in, TS_VOID **out);
    TS_S32  (*set_param)(TS_VOID *pHandle, TS_VOID *pParam); //设置处理函数配置参数
    TS_S32  (*get_param)(TS_VOID *pHandle, TS_VOID *pParam); //获取处理函数配置参数
    TS_S32  (*get_result)(TS_VOID *pHandle, TS_VOID *pResult);      //获取计算结果
    TS_S32  (*release_result)(TS_VOID *pHandle, TS_VOID *pResult);   //释放计算结果资源
	TS_S32 (*config_cpm_attr)(TS_VOID * pHandle, CPM_GRP_ATTR_S * pCpmAttr);
}CPM_Handle_S;

typedef struct tsCPM_SET_PARAM_T {
	TS_S32 s32Id;
	TS_VOID *pParam;
}CPM_SET_PARAM_T;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_COMM_CPM_H__ */
