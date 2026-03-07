#ifndef __VIDEO_ALG_CONFIG_H__
#define __VIDEO_ALG_CONFIG_H__



/*******************************************************
 * *
 * *    alg configure
 * *
********************************************************/
	#define  ALG_MOTIONDETECT          0 //移动侦测
	#define  ALG_PCPPDETECT             0 //多目标检测(行人、车辆、宠物、包裹)
	#define  ALG_AIPERIMETER			0 // AI周界（人车非检测+人车非跟踪 画个框，判断是否有东西在这个框里）
	#define  ALG_BODYTRACK              0 //人形跟踪
	#define ALG_CAT_DETECT 1

/*******************************************************
 * *
 * *    alg mask
 * *
********************************************************/
typedef enum {
	SAMPLE_ALG_MASK_MOTION = 0,           //移动侦测
	SAMPLE_ALG_MASK_PCPPDETECT,           //多目标检测(行人、车辆、宠物、包裹)	
	SAMPLE_ALG_MASK_AIPERIMETER,    	  //AI周界
	SAMPLE_ALG_MASK_BODYTRACK,    	      //人形跟踪
	SAMPLE_ALG_CAT_DETECT,
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

#if ALG_PCPPDETECT
	#include "ts_alg_pcpp_detect.h"
	#include "video_alg_pcppdetect.h"	
#endif

#if ALG_AIPERIMETER
	#include "ts_alg_aiperimeter.h"
	#include "video_alg_aiperimeter.h"
#endif

#if ALG_BODYTRACK
	#include "ts_alg_body_detect.h"
	#include "video_alg_bodytrack.h"
#endif
#if ALG_CAT_DETECT
        #include "video_alg_catdetect.h"
        #include "video_alg_catdetect-api.h"
#endif


typedef struct {
#if ALG_MOTIONDETECT
	ALG_MOTION_DETECT_RESULT_S         gstAlgMotionResult;
#endif

#if ALG_PCPPDETECT
	ALG_PCPP_DET_RESULT_S              gstAlgPcppResult;
#endif

#if ALG_AIPERIMETER
	ALG_AI_PERIMETER_RESULTS_S         gstAlgAiPerimeterResult;
#endif

#if ALG_BODYTRACK
	ALG_BODYDET_RESULT_S       gstAlgBodyResult;
#endif
#if ALG_CAT_DETECT
	//ALG_BODYDET_RESULT_S			gstAlgCatdetResult;
	ALG_CatDetect_DET_RESULT_S		gstAlgCatdetResult;
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
