/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_AUDIO_PROCESSING_AGC_INCLUDE_GAIN_CONTROL_H_
#define WEBRTC_MODULES_AUDIO_PROCESSING_AGC_INCLUDE_GAIN_CONTROL_H_

//#include "typedefs.h"
#include "ts_alg_type.h"

// Errors
#define AGC_UNSPECIFIED_ERROR           18000
#define AGC_UNSUPPORTED_FUNCTION_ERROR  18001
#define AGC_UNINITIALIZED_ERROR         18002
#define AGC_NULL_POINTER_ERROR          18003
#define AGC_BAD_PARAMETER_ERROR         18004

// Warnings
#define AGC_BAD_PARAMETER_WARNING       18050

/* mode     = 0 - Only saturation protection
 *            1 - Analog Automatic Gain Control [-targetLevelDbfs (default -3 dBOv)]
 *            2 - Digital Automatic Gain Control [-targetLevelDbfs (default -3 dBOv)]
 *            3 - Fixed Digital Gain [compressionGaindB (default 8 dB)]
 */
enum{
    WEBRTC_AGC_MODE_UNCHANGED,
    WEBRTC_AGC_MODE_ADAPTIVE_ANALOG,
    WEBRTC_AGC_MODE_ADAPTIVE_DIGITAL,
    WEBRTC_AGC_MODE_FIXED_DIGITAL
};

enum{
    WEBRTC_AGC_FALSE = 0,
    WEBRTC_AGC_TRUE
};

typedef struct{
    /**
     * @brief WEBRTC_AGC_MODE_UNCHANGED, 
     * WEBRTC_AGC_MODE_ADAPTIVE_ANALOG: Adaptive Analog Automatic Gain Control [-targetLevelDbfs (default -3 dBOv)]
     * WEBRTC_AGC_MODE_ADAPTIVE_DIGITAL: Adaptive Digital Automatic Gain Control [-targetLevelDbfs (default -3 dBOv)]
     * WEBRTC_AGC_MODE_FIXED_DIGITAL: Fixed Digital Gain 0dB [compressionGaindB (default 8 dB)]
     */
    TS_S16 s16AgcMode;

    TS_S16 s16FrameLen;
    /**
     * @brief Sampling ratio
     */
    TS_U32 u32Fs;
}ALG_WEBRTC_AGC_INIT_S;

typedef struct{
    /**
     * @brief Near-end input speech vector (10 or 20 ms) for L band
     */
    const TS_S16* ps16In;

    /**
     * @brief Number of samples in input/output vector
     */
    TS_S16 s16Samples;

    TS_S32 s32InGainLevel;

    TS_S32 s32LimiterOnly;

}ALG_WEBRTC_AGC_PROC_S;

typedef struct{
    /**
     * @brief Gain-adjusted near-end speech vector (L band)
     * May be the same vector as the input.
     */
    TS_S16* ps16Out;
}ALG_WEBRTC_AGC_RESULT_S;

typedef struct{
    /**
     * @brief default 3 (-3 dBOv)
     */
    TS_S16 s16TargetLevelDbfs;
    /**
     * @brief default 9 dB
     */
    TS_S16 s16CompressionGaindB;
    /**
     * @brief default kAgcTrue (on)
     */
    // limite first several values of gainTable, the number of values depends s16CompressionGaindB
    TS_U8 u8LimiterEnable;
} ALG_WEBRTC_AGC_PARAM_S;

typedef struct{
    /**
     * @brief Far-end input speech vector (10 or 20ms)
     */
    const TS_S16* ps16InFar;
    TS_S16 s16Samples;
}ALG_WEBRTC_AGC_ADDFAREND_PARAM_S;

typedef struct{
    /**
     * @brief  Microphone input speech vector (10 or 20 ms) for L band
     */
    TS_S16* ps16InMic;
    /**
     * @brief Microphone input speech vector (10 or 20 ms) for H band
     */
    TS_S16* ps16InMicH;
    /**
     * @brief Number of samples in input vector
     */
    TS_S16 s16Samples;
}ALG_WEBRTC_AGC_ADDMIC_PARAM_S;

typedef struct{
    /**
     * @brief Microphone input speech vector (10 or 20 ms) for L band, input & output
     */
    TS_S16* ps16InMic;
    /**
     * @brief Microphone input speech vector (10 or 20 ms) for H band, input & output
     */
    TS_S16* ps16InMicH;
    /**
     * @brief Number of samples in input vector, input
     */
    TS_S16 s16Samples;
    /**
     * @brief Input level of microphone (static), input
     */
    TS_S32 s32MicLevelIn;
    /**
     * @brief Adjusted microphone level after processing, output
     */
    TS_S32* ps32MicLevelOut;
}ALG_WEBRTC_AGC_VIRTUALMIC_PARAM_S;



#if defined(__cplusplus)
extern "C"
{
#endif

/*
 * This function processes a 10/20/16ms/32ms frame and adjusts (normalizes) the gain
 * both analog and digitally. The gain adjustments are done only during
 * active periods of speech. The input speech length can be either 10ms or
 * 20ms or 16ms or 32ms and the output is of the same length. The length of the speech
 * vectors must be given in samples (80/160/128/256 when FS=8000, and 160/320/256/512 when
 * FS=16000). The echo parameter can be used to ensure the AGC will
 * not adjust upward in the presence of echo.
 *
 * This function should be called after processing the near-end microphone
 * signal, in any case after any echo cancellation.
 *
 * Input:
 *      - agcInst           : AGC instance
 *      - inNear            : Near-end input speech vector (10 or 20 ms) for
 *                            L band
 *      - inNear_H          : Near-end input speech vector (10 or 20 ms) for
 *                            H band
 *      - samples           : Number of samples in input/output vector
 *      - inMicLevel        : Current microphone volume level
 *      - echo              : Set to 0 if the signal passed to add_mic is
 *                            almost certainly free of echo; otherwise set
 *                            to 1. If you have no information regarding echo
 *                            set to 0.
 *
 * Output:
 *      - outMicLevel       : Adjusted microphone volume level
 *      - out               : Gain-adjusted near-end speech vector (L band)
 *                          : May be the same vector as the input.
 *      - out_H             : Gain-adjusted near-end speech vector (H band)
 *      - saturationWarning : A returned value of 1 indicates a saturation event
 *                            has occurred and the volume cannot be further
 *                            reduced. Otherwise will be set to 0.
 *
 * Return value:
 *                          :  0 - Normal operation.
 *                          : -1 - Error
 */
TS_S32 TS_ALG_WebRTCAGC_Process(TS_VOID* pHandle, ALG_WEBRTC_AGC_PROC_S *pInParam, ALG_WEBRTC_AGC_RESULT_S *pRslt);

/*
 * This function sets the config parameters (targetLevelDbfs,
 * compressionGaindB and limiterEnable).
 *
 * Input:
 *      - agcInst           : AGC instance
 *      - config            : config struct
 *
 * Output:
 *
 * Return value:
 *                          :  0 - Normal operation.
 *                          : -1 - Error
 */
TS_S32 TS_ALG_WebRTCAGC_SetParam(TS_VOID* pHandle, ALG_WEBRTC_AGC_PARAM_S *pConfig);

/*
 * This function returns the config parameters (targetLevelDbfs,
 * compressionGaindB and limiterEnable).
 *
 * Input:
 *      - agcInst           : AGC instance
 *
 * Output:
 *      - config            : config struct
 *
 * Return value:
 *                          :  0 - Normal operation.
 *                          : -1 - Error
 */
TS_S32 TS_ALG_WebRTCAGC_GetParam(TS_VOID* pHandle, ALG_WEBRTC_AGC_PARAM_S* pConfig);

/*
 * This function creates an AGC instance, which will contain the state
 * information for one (duplex) channel.
 *
 * Return value             : AGC instance if successful
 *                          : 0 (i.e., a NULL pointer) if unsuccessful
 */
//TS_S32 WebRtcAgc_Create(TS_VOID **agcInst);

/*
 * This function frees the AGC instance created at the beginning.
 *
 * Input:
 *      - agcInst           : AGC instance.
 *
 * Return value             :  0 - Ok
 *                            -1 - Error
 */
TS_S32 TS_ALG_WebRTCAGC_Exit(TS_VOID *pHandle);

/*
 * This function initializes an AGC instance.
 *
 * Input:
 *      - agcInst           : AGC instance.
 *      - minLevel          : Minimum possible mic level
 *      - maxLevel          : Maximum possible mic level
 *      - agcMode           : 0 - Unchanged
 *                          : 1 - Adaptive Analog Automatic Gain Control -3dBOv
 *                          : 2 - Adaptive Digital Automatic Gain Control -3dBOv
 *                          : 3 - Fixed Digital Gain 0dB
 *      - fs                : Sampling frequency
 *
 * Return value             :  0 - Ok
 *                            -1 - Error
 */
TS_S32 TS_ALG_WebRTCAGC_Init(TS_VOID **ppHandle, ALG_WEBRTC_AGC_INIT_S *pParam);

const TS_CHAR *TS_ALG_WebRTCAGC_GetVersion(TS_VOID);


#if defined(__cplusplus)
}
#endif

#endif  // WEBRTC_MODULES_AUDIO_PROCESSING_AGC_INCLUDE_GAIN_CONTROL_H_
