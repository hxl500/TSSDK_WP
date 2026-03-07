
#ifndef __DESICION_CFG_HEYSIRI_8K_H__
#define __DESICION_CFG_HEYSIRI_8K_H__

#include "ZkyDecoderData.h"
// #include "../../../api/ZkyNieDecoder.h"

/**********************************************************
 *  MACRO
 */

#define  NN_OUT_Q					12

#define  DECODER_MODE				6

#define  NEW_SIL_SCORE_THRESH       0.05f
#define  ASR_SCORE_THRESH			0.08f
#define  ASR_POST_SMOOTH_LENGTH		3
#define  AVERAGE_STATE_FRAMES	    7

#define  WAKEUP_ID					1
#define  WAKEUP_SCORE_THRESH		0.6f
#define  WakeUP_AVG_STATE_FRAMES	2

#define  CMD_ID						4
#define  CMD_SCORE_THRESH			0.35f
//#define  CMD_SCORE_THRESH			0.20f
#define  CMD_AVG_STATE_FRAMES		15

// START OF CMD THRES CONFIG
//
float asr_threshmin[] = {0.40};
float asr_threshmax[] = {0.35};
#define NN_OUT_NUM 67
// END OF CMD THRES CONFIG

static ZkyNieParam ZkyParam =
{
	.nn_out_size                 = NN_OUT_NUM,
	.nn_out_Q                    = NN_OUT_Q,

	.asr_score_thresh            = ASR_SCORE_THRESH,

    .sensitive_score_thresh      = 0.50f,        // wakeup
	.test_score_thresh           = 0.40f,        // wakeup

    .asr_post_smooth_length      = ASR_POST_SMOOTH_LENGTH,
    .average_state_frames        = AVERAGE_STATE_FRAMES,

    .wakeup_score_thresh         = WAKEUP_SCORE_THRESH,
    .average_wakeup_state_frames = WakeUP_AVG_STATE_FRAMES,

	.number_state                = NUMBER_STATE,
	.start_state                 = START_STATE,
	.decoder_mode                = DECODER_MODE,

    .wakeup_id                   = WAKEUP_ID,
	.Cmd_id                      = CMD_ID,
	.cmd_score_thresh            = CMD_SCORE_THRESH,
	.average_cmd_state_frames    = CMD_AVG_STATE_FRAMES,

    .frames_per_second           = 31,            // wakeup
    .sensitive_duration          = 10,            // wakeup
    .wakeup_duration             = 1,

	.sil_score_thresh            = NEW_SIL_SCORE_THRESH,
	.pAsrScoreThreshMin          = asr_threshmin,
	.pAsrScoreThreshMax          = asr_threshmax,

	.pfillerflag                 = gfillerflag,
	.pfinalflag                  = gfinalflag,
	.pArcOffset                  = gArcOffset,
	.pIlabel                     = gIlabel,
	.pOlabel                     = gOlabel,
	.pNextState                  = gNextState,
};


#endif
