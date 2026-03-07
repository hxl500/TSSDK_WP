/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_comm_adec.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */
#ifndef  __TS_COMM_ADEC_H__
#define  __TS_COMM_ADEC_H__


#include "ts_type.h"
#include "ts_common.h"
#include "ts_comm_aio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct tsADEC_ATTR_G711_S
{
    TS_U32 resv;
}ADEC_ATTR_G711_S;

typedef struct tsADEC_ATTR_G726_S
{
    G726_BPS_E enG726bps;
}ADEC_ATTR_G726_S;

typedef struct tsADEC_ATTR_ADPCM_S
{
    ADPCM_TYPE_E enADPCMType;
}ADEC_ATTR_ADPCM_S;

typedef struct tsADEC_ATTR_LPCM_S
{
    TS_U32 resv;
}ADEC_ATTR_LPCM_S;

typedef enum tsADEC_MODE_E
{
    ADEC_MODE_PACK = 0,/*require input is valid dec pack(a
                        complete frame encode result),
                        e.g.the stream get from AENC is a
                        valid dec pack, the stream know actually
                        pack len from file is also a dec pack.
                        this mode is high-performative*/
    ADEC_MODE_STREAM ,/*input is stream,low-performative,
                        if you couldn't find out whether a stream is
                        vaild dec pack,you could use
                        this mode*/
    ADEC_MODE_BUTT
}ADEC_MODE_E;

typedef struct tsADEC_CH_ATTR_S
{
    PAYLOAD_TYPE_E enType;
    TS_U32         u32BufSize;  /*buf size[2~MAX_AUDIO_FRAME_NUM]*/
    ADEC_MODE_E    enMode;      /*decode mode*/
    TS_VOID ATTRIBUTE      *pValue;
}ADEC_CHN_ATTR_S;

typedef struct tsADEC_CHN_STATE_S
{
	TS_BOOL bEndOfStream;
	TS_U32 u32BufferFrmNum;
	TS_U32 u32BufferFreeNum;
	TS_U32 u32BufferBusyNum;
} ADEC_CHN_STATE_S;

typedef struct tsADEC_DECODER_S
{
    PAYLOAD_TYPE_E  enType;
    TS_CHAR         aszName[17];
    TS_S32          (*pfnOpenDecoder)(TS_VOID *pDecoderAttr, TS_VOID **ppDecoder); /*struct ppDecoder is packed by user,user malloc and free memory for this struct */
    TS_S32          (*pfnDecodeFrm)(TS_VOID *pDecoder, TS_U8 **pu8Inbuf,TS_S32 *ps32LeftByte,
                        TS_U16 *pu16Outbuf,TS_U32 *pu32OutLen,TS_U32 *pu32Chns);
    TS_S32          (*pfnGetFrmInfo)(TS_VOID *pDecoder, TS_VOID *pInfo);
    TS_S32          (*pfnCloseDecoder)(TS_VOID *pDecoder);
    TS_S32          (*pfnResetDecoder)(TS_VOID *pDecoder);
} ADEC_DECODER_S;

typedef enum tsEN_ADEC_ERR_CODE_E
{
    ADEC_ERR_DECODER_ERR     = 64,
    ADEC_ERR_BUF_LACK,

} EN_ADEC_ERR_CODE_E;

typedef struct ADEC_CHN_REGISTERED_T{
	ts_s32 (*adecInit)(void **handle, void *attr);
	ts_s32 (*adecExit)(void *handle);
	ts_s32 (*adecProcess)(void *handle, ts_u8 *stream, ts_u32 *frame_byte, ts_u8 *out, ts_u32 *out_len, ts_u32 *chn);
	ts_s32 (*adecGetFrmInfo)(void *handle, void *info);
	ts_s32 (*adecReset)(void *handle);
} ADEC_CHN_REGISTERED_T;


/* invlalid device ID */
#define TS_ERR_ADEC_INVALID_DEVID     TS_DEF_ERR(TS_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define TS_ERR_ADEC_INVALID_CHNID     TS_DEF_ERR(TS_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define TS_ERR_ADEC_ILLEGAL_PARAM     TS_DEF_ERR(TS_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define TS_ERR_ADEC_EXIST             TS_DEF_ERR(TS_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* channel unexists */
#define TS_ERR_ADEC_UNEXIST           TS_DEF_ERR(TS_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define TS_ERR_ADEC_NULL_PTR          TS_DEF_ERR(TS_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define TS_ERR_ADEC_NOT_CONFIG        TS_DEF_ERR(TS_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define TS_ERR_ADEC_NOT_SUPPORT       TS_DEF_ERR(TS_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define TS_ERR_ADEC_NOT_PERM          TS_DEF_ERR(TS_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define TS_ERR_ADEC_NOMEM             TS_DEF_ERR(TS_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define TS_ERR_ADEC_NOBUF             TS_DEF_ERR(TS_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define TS_ERR_ADEC_BUF_EMPTY         TS_DEF_ERR(TS_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define TS_ERR_ADEC_BUF_FULL          TS_DEF_ERR(TS_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define TS_ERR_ADEC_SYS_NOTREADY      TS_DEF_ERR(TS_ID_ADEC, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/* decoder internal err */
#define TS_ERR_ADEC_DECODER_ERR       TS_DEF_ERR(TS_ID_ADEC, EN_ERR_LEVEL_ERROR, ADEC_ERR_DECODER_ERR)
/* input buffer not enough to decode one frame */
#define TS_ERR_ADEC_BUF_LACK          TS_DEF_ERR(TS_ID_ADEC, EN_ERR_LEVEL_ERROR, ADEC_ERR_BUF_LACK)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif/* End of #ifndef __TS_COMM_ADEC_H__*/

