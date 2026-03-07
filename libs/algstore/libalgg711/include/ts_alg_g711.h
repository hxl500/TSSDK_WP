/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: the definitions of body detection
 * Author: Tsing Micro solution-application group
 * Create: 2022/11/18
 */
#ifndef  _TS_ALG_G711_H_
#define _TS_ALG_G711_H_

#include "ts_alg_type.h"

#if defined(__cplusplus)
extern "C"
{
#endif

typedef enum tsALG_G711_MODE_E {
    G711_ALAW = 0,
    G711_ULAW
}ALG_G711_MODE_E;


/**
 * @brief G711 encoder
 * 
 * @param mode    : G711 encoder mode
 * @param pSrc    : input sample data address
 * @param pOut    : output g711 data address
 * @param in_size : input sample number in byte
 *
 * @return error code
 */ 
TS_S32 TS_ALG_G711_Encoder(TS_S32 mode, TS_U8 *pOut, const TS_U8 *pSrc, TS_S32 s32InSize);

/**
 * @brief G711 decoder
 * 
 * @param mode    : G711 encoder mode
 * @param pSrc    : input g711 data address
 * @param pOut    : output sample data address
 * @param in_size : code length(uint8_t)
 *
 * @return error code
 */ 
TS_S32 TS_ALG_G711_Decoder(TS_S32 mode, TS_U8 *pOut, const TS_U8 *pSrc, TS_S32 s32InSize);


/**
 * @brief G711 decoder
 * 
 * @param mode    : G711 encoder mode       mode=0 :alaw_to_ulaw   mode=other; ulaw_to_alaw
 * @param pSrc    : input g711 data address
 * @param pOut    : output g711 data address
 * @param in_size : code length(uint8_t)
 *
 * @return error code
 */ 
TS_S32 TS_ALG_G711_Transcode(TS_S32 mode, TS_U8 *pOut, const TS_U8 *pSrc, TS_S32 s32InSize);

/**
 * @brief g711 get version
 *
 * @param void
 *
 * @return version
 */
const TS_CHAR* TS_ALG_G711_GetVersion(TS_VOID);

#if defined(__cplusplus)
}
#endif

#endif
