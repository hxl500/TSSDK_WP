#ifndef __VIDEO_ALG_CONFIG_H__
#define __VIDEO_ALG_CONFIG_H__



/*******************************************************
 * *
 * *    alg configure
 * *
********************************************************/
	#define  ALG_MOTIONDETECT          1 //移动侦测


/*******************************************************
 * *
 * *    alg mask
 * *
********************************************************/
typedef enum {
	SAMPLE_ALG_MASK_MOTION = 0,           //移动侦测
	SAMPLE_ALG_MAX_NUM
}SAMPLE_ALG_TYPE_E;


/*******************************************************
 * *
 * *    alg head and result 
 * *
********************************************************/
#if ALG_MOTIONDETECT
#include "ts_alg_motiondetect.h"
#include "video_alg_motiondetect.h"
#endif


typedef struct {
#if ALG_MOTIONDETECT
	ALG_MOTION_DETECT_RESULT_S         gstAlgMotionResult;
#endif

}SAMPLE_ALG_RESULT_S;

/*******************************************************
 * *
 * *    SDK version
 * *
********************************************************/
#define SDK_VERSON_030       0

/*******************************************************
 * *
 * *    other video macro
 * *
********************************************************/

//#define ALG_RGB_CHN 	   3
#define ALG_RGBA_CHN 	   4

#endif
