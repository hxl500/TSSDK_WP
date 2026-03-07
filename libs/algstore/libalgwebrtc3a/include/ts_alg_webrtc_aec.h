/*
 *  Copyright (c) 2022 Tsingmicro. All Rights Reserved.
 */

#ifndef _TS_ALG_WEBRTC_AEC_H_
#define _TS_ALG_WEBRTC_AEC_H_

#include <stdint.h>

#include "ts_alg_type.h"

enum {
  AEC_SUCCESS = 0,
  AEC_PARAM_ERR = -1,
  AEC_MALLOC_ERR = -2,
  AEC_UNINITED_ERR = -3,
};

enum {
  AEC_NLP_CONSERVATIVE = 0,
  AEC_NLP_MODERATE,
  AEC_NLP_AGGRESSIVE
};

enum {
  AEC_FALSE = 0,
  AEC_TRUE
};

typedef struct{
    /**
     * @brief sampling ratio: 8000, 16000, 32000
     */
    TS_S32 s32SampFreq;
    /**
     * @brief sampling ratio of sound card: [1, 96000]
     * used for calculate skew of player and recoder
     */
    TS_S32 s32ScSampFreq;

    TS_S32 s32FrameLen;
}ALG_WEBRTC_AEC_INIT_S;

typedef struct{
    /**
     * @brief address of near end input samples L band
     */
    const TS_S16* s16NearEnd;
    /**
     * @brief address of near end input samples H band, should config if s32SampFreq is 32000, otherwise can be NULL
     */
    const TS_S16* s16NearEndH;
    /**
     * @brief address of far end reference samples
     */
    const TS_S16* s16FarEnd; // useless, should delete
    /**
     * @brief samples per frame of near end
     * sampling ratio=8k: s16NRofSamples=80/160
     * sampling ratio=16k: s16NRofSamples=160, if s16NRofSamples=80
     */
    TS_S16 s16NRofSamples;
    /**
     * @brief Delay estimate for sound card and system buffers, [0, 500], unit: ms
     */
    TS_S16 s16MsInSndCardBuf;
    /**
     * @brief Difference between number of samples played and recorded at the soundcard (for clock skew compensation)
     * unsed when skewmod is enable
     */
    TS_S32 s32Skew;
}ALG_WEBRTC_AEC_PROC_S;

typedef struct{

    /**
     * @brief Out buffer, one frame of processed nearend for L band
     */
    TS_S16* ps16Out;
    /**
     * @brief Out buffer, one frame of processed nearend for H band, will be used if s32SampFreq is 32000
     */
    TS_S16* ps16OutH;
}ALG_WEBRTC_AEC_RESULT_S;

typedef struct{
    /**
     * @brief In buffer containing one frame of farend signal for L band
     */
    TS_S16 *ps16FarEnd;
    /**
     * @brief Number of samples in farend buffer
     */
    TS_S16  s16NRofSamples;
}ALG_WEBRTC_AEC_FAREND_PARAM_S;

typedef struct {
    /**
     * @brief AEC_NLP_CONSERVATIVE(default), AEC_NLP_MODERATE, AEC_NLP_AGGRESSIVE
     */
    TS_S16 s16NlpMode;
    /**
     * @brief AEC_FALSE(default)/AEC_TRUE
     * for skew correct of farend
     */
    TS_S16 s16SkewMode;
    /**
     * @brief AEC_FALSE(default)/AEC_TRUE
     */
    TS_S16 s16MetricsMode;
    /**
     * @brief AEC_FALSE(default)/AEC_TRUE
     */
    TS_S32 s32DelayLogging;
} ALG_WEBRTC_AEC_PARAM_S;

typedef struct {
  int instant;
  int average;
  int max;
  int min;
} ALG_WEBRTC_AEC_LEVEL_S;

typedef struct {
  ALG_WEBRTC_AEC_LEVEL_S rerl;
  ALG_WEBRTC_AEC_LEVEL_S erl;
  ALG_WEBRTC_AEC_LEVEL_S erle;
  ALG_WEBRTC_AEC_LEVEL_S aNlp;
} ALG_WEBRTC_AEC_METRICS_S;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This function releases the memory allocated by WebRtcAec_Create().
 *
 * Inputs                                   Description
 * -------------------------------------------------------------------
 * TS_VOID         *aecInst         Pointer to the AEC instance
 *
 * Outputs                          Description
 * -------------------------------------------------------------------
 * TS_S32           return           0: OK
 *                                  -1: error
 */
TS_S32 TS_ALG_WebRTCAEC_Exit(TS_VOID* pHandle);

/*
 * Initializes an AEC instance.
 *
 * Inputs                       Description
 * -------------------------------------------------------------------
 * TS_VOID       *aecInst      Pointer to the AEC instance
 * TS_S32        sampFreq      Sampling frequency of data
 * TS_S32        scSampFreq    Soundcard sampling frequency
 *
 * Outputs                      Description
 * -------------------------------------------------------------------
 * TS_S32        return        0: OK
 *                             -1: error
 */
TS_S32 TS_ALG_WebRTCAEC_Init(TS_VOID** ppHandle, ALG_WEBRTC_AEC_INIT_S* pParam);

/*
 * Runs the echo canceller on an 80 or 160 sample blocks of data.
 *
 * Inputs                                        Description
 * -------------------------------------------------------------------
 * TS_VOID      *aecInst                Pointer to the AEC instance
 * TS_S16       *s16NearEnd             In buffer containing one frame of
 *                                      nearend+echo signal for L band
 * TS_S16       *s16NearEndH            In buffer containing one frame of
 *                                      nearend+echo signal for H band
 * TS_S16       s16NRofSamples          Number of samples in nearend buffer
 * TS_S16       s16MsInSndCardBuf       Delay estimate for sound card and
 *                                      system buffers
 * TS_S16       s32Skew                 Difference between number of samples played
 *                                      and recorded at the soundcard (for clock skew
 *                                      compensation)
 *
 * Outputs                                  Description
 * -------------------------------------------------------------------
 * TS_S16       *ps16Out            Out buffer, one frame of processed nearend
 *                                  for L band
 * TS_S16       *ps16OutH           Out buffer, one frame of processed nearend
 *                                  for H band
 * TS_S32       return              0: OK
 *                                  -1: error
 *
 * attention:
 * extended mode has not opened, normal mode setted in init function, if want to change to extended mode, should use interface which has not opened.
 */
 TS_S32 TS_ALG_WebRTCAEC_Process(TS_VOID* pHandle, ALG_WEBRTC_AEC_PROC_S* pInParam, ALG_WEBRTC_AEC_RESULT_S* pRslt);

/*
 * Inserts an 80 or 160 sample block of data into the farend buffer.
 *
 * Inputs                       Description
 * -------------------------------------------------------------------
 * void           *aecInst      Pointer to the AEC instance
 * int16_t        *farend       In buffer containing one frame of
 *                              farend signal for L band
 * int16_t        nrOfSamples   Number of samples in farend buffer
 *
 * Outputs                      Description
 * -------------------------------------------------------------------
 * int32_t        return        0: OK
 *                             -1: error
 */
TS_S32 TS_ALG_WebRTCAEC_BufferFarend(TS_VOID* pHandle, ALG_WEBRTC_AEC_FAREND_PARAM_S* pFarParam);

/*
 * This function enables the user to set certain parameters on-the-fly.
 *
 * Inputs                                            Description
 * -------------------------------------------------------------------
 * TS_VOID                     *pHandle       Pointer to the AEC instance
 * ALG_WEBRTC_AEC_PARAM_S      pConfig        Config instance that contains all
 *                                            properties to be set
 *
 * Outputs                      Description
 * -------------------------------------------------------------------
 * int            return         0: OK
 *                              -1: error
 */
TS_S32 TS_ALG_WebRTCAEC_SetParam(TS_VOID* pHandle, ALG_WEBRTC_AEC_PARAM_S* pConfig);

TS_S32 TS_ALG_WebRTCAEC_GetParam(TS_VOID* pHandle, ALG_WEBRTC_AEC_PARAM_S* pConfig);

 const TS_CHAR* TS_ALG_WebRTCAEC_GetVersion(TS_VOID);

TS_S32 TS_ALG_WebRTCAEC_GetDelayMetrics(TS_VOID* handle, TS_S32* median, TS_S32* std);

TS_S32 TS_ALG_WebRTCAEC_GetMetrics(TS_VOID* handle, ALG_WEBRTC_AEC_METRICS_S* metrics);

#ifdef __cplusplus
}
#endif
#endif  // _TS_ALG_WEBRTC_AEC_H_
