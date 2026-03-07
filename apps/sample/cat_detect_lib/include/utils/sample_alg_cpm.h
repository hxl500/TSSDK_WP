#ifndef __SAMPLE_ALG_CPM_H__
#define __SAMPLE_ALG_CPM_H__

#include <pthread.h>
#include <semaphore.h>

#include "sample_comm_util.h"
#include "sample_comm_base.h"
#include "sample_comm_sys.h"
#include "sample_comm_isp.h"
#include "sample_comm_vi.h"
#include "sample_comm_vpss.h"
#include "sample_comm_venc.h"
#include "sample_comm_cpm.h"
#include "ts_alg_type.h"
#include "ts_alg_log.h"
//#include "video_alg_config.h"

extern char *g_pcModelRoot;

#define SAMPLE_MODEL_FILE_LEN  200

typedef TS_S32  (*pf_MyALG_Init)(TS_VOID **ppHdl, TS_VOID *pParam);
typedef TS_S32  (*pf_MyALG_Process)(TS_VOID *pHdl, ALG_IMAGE_S *pImageBig, ALG_IMAGE_S *pImageSmall, TS_VOID  *pResult);
typedef TS_VOID (*pf_MyALG_Result_Process)(TS_U8 *pYuvBuf,  TS_U32 width, TS_U32 height,TS_U32 u32ImageRatio, TS_VOID *pResult);
typedef TS_S32  (*pf_MyALG_Exit)(TS_VOID *pHdl);
typedef enum {
	SAMPLE_ALG_MASK_MOTION = 0,           //移动侦测
	SAMPLE_ALG_MASK_PCPPDETECT,           //多目标检测(行人、车辆、宠物、包裹)	
	SAMPLE_ALG_MASK_AIPERIMETER,    	  //AI周界
	SAMPLE_ALG_BODY_DETECT_V16,
	SAMPLE_ALG_CAT_DETECT,
	SAMPLE_ALG_FIRESMOKEDETECT,
	SAMPLE_ALG_MAX_NUM
}SAMPLE_ALG_TYPE_E;

typedef struct {
	SAMPLE_ALG_TYPE_E	        enAlgType;	

	pf_MyALG_Process		    pf_Process;
	pf_MyALG_Result_Process     pf_Result_Process;
	pf_MyALG_Exit			    pf_Exit;

	TS_VOID	*			        pHandle;
}SAMPLE_ALG_INSTANCE_S;

typedef enum {
    BUFFER_STATUS_INIT = 0,
	BUFFER_STATUS_FILL,
	BUFFER_STATUS_ALG_PROCESS,
    BUFFER_STATUS_ALG_BYPASS	
} ALG_BUFFER_STATUS;

//alg 通路图像大小
typedef struct ALG_CHN_IMG_SIZE_S {
    TS_U32    width;
	TS_U32    hight;
	TS_U32    hight_ex;  //扩充
}SAMPLE_ALG_CHN_IMG_SIZE_S;


typedef enum {
    VIDEO_CHN_MODE0 = 0,  //w=640, h=360, exp_h=24
	 VIDEO_CHN_MODE1      //w=1280, h=720, exp_h=16
} SAMPLE_VIDEO_CHN_MODE;



typedef TS_S32 (*pf_ALG_INSTANCE_Init)(TS_VOID **pHandle);
typedef struct ALG_INIT_S{
    SAMPLE_ALG_TYPE_E    eMask;
	pf_ALG_INSTANCE_Init pInit; 
	TS_U8                u8AlgName[30];
	SAMPLE_VIDEO_CHN_MODE eChnMode; //
}SAMPLE_ALG_INIT_CFG_S;


TS_S32 SAMPLE_ALG_CPM_Config(AVS_GRP CPMGrp, int pipeNum, TS_U32 u32Width, TS_U32 u32Height, SAMPLE_ALG_TYPE_E* penAlgType, TS_U32 u32AlgNum, SAMPLE_ALG_CHN_IMG_SIZE_S* stAlgChnSize);

SAMPLE_ALG_INIT_CFG_S *SAMPLE_ALG_GetInitCfg(TS_U32 *pNum);

void SAMPLE_CPM_GetChnMode(SAMPLE_VIDEO_CHN_MODE mode, SAMPLE_ALG_CHN_IMG_SIZE_S *pstAlgChnSize);


TS_VOID SAMPLE_CPM_GetSrcSize(TS_U32 *pw, TS_U32 *ph);

#endif
