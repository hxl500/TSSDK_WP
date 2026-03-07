/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_comm_aenc.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef  __TS_COMM_AENC_H__
#define  __TS_COMM_AENC_H__

#include "ts_type.h"
#include "ts_common.h"
#include "ts_comm_aio.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct tsAENC_ATTR_G711_S
{
    TS_U32 resv;            /*reserve item*/
}AENC_ATTR_G711_S;

typedef struct tsAENC_ATTR_G726_S
{
    G726_BPS_E enG726bps;
}AENC_ATTR_G726_S;

typedef struct tsAENC_ATTR_ADPCM_S
{
    ADPCM_TYPE_E enADPCMType;
}AENC_ATTR_ADPCM_S;

typedef struct tsAENC_ATTR_LPCM_S
{
    TS_U32 resv;            /*reserve item*/
}AENC_ATTR_LPCM_S;

typedef struct tsAENC_ENCODER_S
{
    PAYLOAD_TYPE_E  enType;
    TS_U32          u32MaxFrmLen;
    TS_CHAR         aszName[17];    /* encoder type,be used to print proc information */
    TS_S32          (*pfnOpenEncoder)(TS_VOID *pEncoderAttr, TS_VOID **ppEncoder); /* pEncoder is the handle to control the encoder */
    TS_S32          (*pfnEncodeFrm)(TS_VOID *pEncoder, const AUDIO_FRAME_S *pstData,
                        TS_U8 *pu8Outbuf,TS_U32 *pu32OutLen);
    TS_S32          (*pfnCloseEncoder)(TS_VOID *pEncoder);
} AENC_ENCODER_S;

typedef struct tsAENC_CHN_ATTR_S
{
    PAYLOAD_TYPE_E      enType;         /*payload type ()*/
	TS_U32 				u32PtNumPerFrm;
    TS_U32              u32BufSize;      /*buf size [2~MAX_AUDIO_FRAME_NUM]*/
    TS_VOID ATTRIBUTE   *pValue;  /*point to attribute of definite audio encoder*/
}AENC_CHN_ATTR_S;

typedef enum tsEN_AENC_ERR_CODE_E
{
    AENC_ERR_ENCODER_ERR     = 64 ,
    AENC_ERR_VQE_ERR        = 65 ,

} EN_AENC_ERR_CODE_E;

typedef struct AENC_CHN_REGISTERED_T {
ts_s32 (*aencInit)(void **handle, void *attr);
ts_s32 (*aencExit)(void *handle);
ts_s32 (*aencProcess)(void *handle, AUDIO_FRAME_S *frame, ts_u8 *out, ts_u32 *len);
} AENC_CHN_REGISTERED_T;

/* invlalid device ID */
#define TS_ERR_AENC_INVALID_DEVID     TS_DEF_ERR(TS_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define TS_ERR_AENC_INVALID_CHNID     TS_DEF_ERR(TS_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define TS_ERR_AENC_ILLEGAL_PARAM     TS_DEF_ERR(TS_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define TS_ERR_AENC_EXIST             TS_DEF_ERR(TS_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* channel unexists */
#define TS_ERR_AENC_UNEXIST           TS_DEF_ERR(TS_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define TS_ERR_AENC_NULL_PTR          TS_DEF_ERR(TS_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define TS_ERR_AENC_NOT_CONFIG        TS_DEF_ERR(TS_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define TS_ERR_AENC_NOT_SUPPORT       TS_DEF_ERR(TS_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change static attribute */
#define TS_ERR_AENC_NOT_PERM          TS_DEF_ERR(TS_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define TS_ERR_AENC_NOMEM             TS_DEF_ERR(TS_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define TS_ERR_AENC_NOBUF             TS_DEF_ERR(TS_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define TS_ERR_AENC_BUF_EMPTY         TS_DEF_ERR(TS_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define TS_ERR_AENC_BUF_FULL          TS_DEF_ERR(TS_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define TS_ERR_AENC_SYS_NOTREADY      TS_DEF_ERR(TS_ID_AENC, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/* encoder internal err */
#define TS_ERR_AENC_ENCODER_ERR       TS_DEF_ERR(TS_ID_AENC, EN_ERR_LEVEL_ERROR, AENC_ERR_ENCODER_ERR)
/* vqe internal err */
#define TS_ERR_AENC_VQE_ERR       TS_DEF_ERR(TS_ID_AENC, EN_ERR_LEVEL_ERROR, AENC_ERR_VQE_ERR)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif/* End of #ifndef __TS_COMM_AENC_H__*/

