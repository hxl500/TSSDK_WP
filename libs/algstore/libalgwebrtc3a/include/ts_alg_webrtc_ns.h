/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_AUDIO_PROCESSING_NS_INCLUDE_NOISE_SUPPRESSION_X_H_
#define WEBRTC_MODULES_AUDIO_PROCESSING_NS_INCLUDE_NOISE_SUPPRESSION_X_H_

//#include "typedefs.h"
#include "ts_alg_type.h"

typedef struct{
    /**
     * @brief sampling ratio
     */
    TS_U32 u32Fs;
    TS_U32 u32FrameLen;
}ALG_WEBRTC_NSX_INIT_S;

typedef struct{
    /**
     * @brief Pointer to speech frame buffer for L band
     */
    TS_S16* ps16SpeechFrame;
    /**
     * @brief Pointer to speech frame buffer for H band
     */
    TS_S16* ps16SpeechFrameHB;
    TS_U32 u32InLen;
}ALG_WEBRTC_NSX_RROC_S;

typedef struct{
    /**
     * @brief Pointer to output frame for L band
     */
	TS_S16* ps16OutFrame;
    /**
     * @brief Pointer to output frame for H band
     */
    TS_S16* ps16OutFrameHB;
}ALG_WEBRTC_NSX_RESULT_S;

typedef struct{
    /**
     * @brief 0: Mild, 1: Medium , 2: Aggressive, 3:severe
     */
	TS_S32  s32Mode;
}ALG_WEBRTC_NSX_PARAM_S;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This function frees the dynamic memory of a specified Noise Suppression
 * instance.
 *
 * Input:
 *      - nsxInst       : Pointer to NS instance that should be freed
 *
 * Return value         :  0 - Ok
 *                        -1 - Error
 */
TS_S32 TS_ALG_WebRTCNSX_Exit(TS_VOID* nsxInst);


/*
 * This function initializes a NS instance
 *
 * Input:
 *      - nsxInst       : Instance that should be initialized
 *      - fs            : sampling frequency
 *
 * Output:
 *      - nsxInst       : Initialized instance
 *
 * Return value         :  0 - Ok
 *                        -1 - Error
 */
TS_S32 TS_ALG_WebRTCNSX_Init(TS_VOID** nsxInst, ALG_WEBRTC_NSX_INIT_S *pParam);

/*
 * This changes the aggressiveness of the noise suppression method.
 *
 * Input:
 *      - nsxInst       : Instance that should be initialized
 *      - mode          : 0: Mild, 1: Medium , 2: Aggressive, 3:
 *
 * Output:
 *      - nsxInst       : Initialized instance
 *
 * Return value         :  0 - Ok
 *                        -1 - Error
 */
TS_S32 TS_ALG_WebRTCNSX_SetParam(TS_VOID* nsxInst, ALG_WEBRTC_NSX_PARAM_S *pParam);

TS_S32 TS_ALG_WebRTCNSX_GetParam(TS_VOID* nsxInst, ALG_WEBRTC_NSX_PARAM_S *pParam);

/*
 * This functions does noise suppression for the inserted speech frame. The
 * input and output signals should always be 10ms (80 or 160 samples).
 *
 * Input
 *      - nsxInst       : NSx instance. Needs to be initiated before call.
 *      - speechFrame   : Pointer to speech frame buffer for L band
 *      - speechFrameHB : Pointer to speech frame buffer for H band
 *      - fs            : sampling frequency
 *
 * Output:
 *      - nsxInst       : Updated NSx instance
 *      - outFrame      : Pointer to output frame for L band
 *      - outFrameHB    : Pointer to output frame for H band
 *
 * Return value         :  0 - OK
 *                        -1 - Error
 */
TS_S32 TS_ALG_WebRTCNSX_Process(TS_VOID* nsxInst, ALG_WEBRTC_NSX_RROC_S *pParam, ALG_WEBRTC_NSX_RESULT_S *pRslt);

const TS_CHAR *TS_ALG_WebRTCNSX_GetVersion(TS_VOID);

#ifdef __cplusplus
}
#endif

#endif  // WEBRTC_MODULES_AUDIO_PROCESSING_NS_INCLUDE_NOISE_SUPPRESSION_X_H_
