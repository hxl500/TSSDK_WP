
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>

#include "sample_comm_base.h"
#include "sample_comm_util.h"
#include "sample_comm_sys.h"
#include "sample_comm_vi.h"
#include "sample_comm_isp.h"
#include "sample_comm_vpss.h"
#include "sample_comm_venc.h"
#include "ts_alg_imgproc.h"
#include "ts_alg_type.h"
#include "ts_alg_log.h"

#include <semaphore.h>
#include  <dirent.h>

#include "trp.h"

#include "ts_alg_facedetect2.h"

#include "ts_alg_body_detect.h"

#include "ts_alg_motiondetect.h"
#include "ts_alg_pcpp_detect.h"


TS_U32 SAMPLE_COMM_ALGO_GetFileSize(FILE *pf)
{
	TS_U32 fileSize = 0;
	if(NULL != pf){
		if(0 == fseek(pf, 0, SEEK_END)){
			fileSize = ftell(pf);
		}
		rewind(pf);
	}
	return fileSize;
}

TS_S32 SAMPLE_COMM_ALGO_Process_FaceDetect(TS_VOID *pHandle, ALG_IMAGE_S *pImage, SAMPLE_ALGO_RESULT_TABLE_S * pResultTbl)
{
	TS_U32 i;
	ALG_FACEDET2_RESULT_S result = {0};



	for(i = 0; i < result.u32FaceNum; i ++){
		SAMPLE_ALGO_RESULT_S * pResult;

		if(pResultTbl->result_num >= SAMPLE_ALGO_RESULT_TABLE_SIZE_MAX){
			SAMPLE_PRT("face detect reach max of algo result table size[%d]\n", SAMPLE_ALGO_RESULT_TABLE_SIZE_MAX);
			return TS_SUCCESS;
		}

		if(result.stBox[i].f32Xmin < 0
			|| result.stBox[i].f32Xmax > 1
			|| result.stBox[i].f32Ymin < 0
			|| result.stBox[i].f32Ymax > 1){
			SAMPLE_PRT("face detect get invalid box xy=[%f,%f][%f,%f]. skip this one\n",
				result.stBox[i].f32Xmin, result.stBox[i].f32Xmax,
				result.stBox[i].f32Ymin, result.stBox[i].f32Ymax);
			continue;
		}

		pResult = &pResultTbl->result_tbl[pResultTbl->result_num];
		pResult->f32Xmin	= result.stBox[i].f32Xmin;
		pResult->f32Xmax	= result.stBox[i].f32Xmax;
		pResult->f32Ymin	= result.stBox[i].f32Ymin;
		pResult->f32Ymax	= result.stBox[i].f32Ymax;
		pResult->f32Score	= result.stBox[i].f32Score;
		pResult->type		= SAMPLE_ALGO_OBJ_TYPE_FACE;
		pResultTbl->result_num ++;
	}

	return TS_SUCCESS;
}


TS_S32 SAMPLE_COMM_ALGO_Process_BodyDetect(TS_VOID *pHandle, ALG_IMAGE_S *pImage, SAMPLE_ALGO_RESULT_TABLE_S * pResultTbl)
{
	TS_S32 s32Ret;
	TS_U32 i;
	ALG_BODYDET_RESULT_S result = {0};
	s32Ret = TS_ALG_BodyDet_Process(pHandle, pImage, &result);
	if(TS_SUCCESS != s32Ret){
		return TS_FAILURE;
	}

	for(i = 0; i < result.u32BodyNum; i ++){
		SAMPLE_ALGO_RESULT_S * pResult;

		if(pResultTbl->result_num >= SAMPLE_ALGO_RESULT_TABLE_SIZE_MAX){
			SAMPLE_PRT("body detect reach max of algo result table size[%d]\n", SAMPLE_ALGO_RESULT_TABLE_SIZE_MAX);
			return TS_SUCCESS;
		}

		if(result.stBox[i].f32Xmin < 0
			|| result.stBox[i].f32Xmax > 1
			|| result.stBox[i].f32Ymin < 0
			|| result.stBox[i].f32Ymax > 1){
			SAMPLE_PRT("face detect get invalid box xy=[%f,%f][%f,%f]. skip this one\n",
				result.stBox[i].f32Xmin, result.stBox[i].f32Xmax,
				result.stBox[i].f32Ymin, result.stBox[i].f32Ymax);
			continue;
		}

		pResult = &pResultTbl->result_tbl[pResultTbl->result_num];
		pResult->f32Xmin	= result.stBox[i].f32Xmin;
		pResult->f32Xmax	= result.stBox[i].f32Xmax;
		pResult->f32Ymin	= result.stBox[i].f32Ymin;
		pResult->f32Ymax	= result.stBox[i].f32Ymax;
		pResult->f32Score	= result.stBox[i].f32Score;
		pResult->type		= SAMPLE_ALGO_OBJ_TYPE_BODY;
		pResultTbl->result_num ++;
	}

	return TS_SUCCESS;
}


TS_S32 SAMPLE_COMM_ALGO_Process_MotionDetect(TS_VOID *pHandle, ALG_IMAGE_S *pImage, SAMPLE_ALGO_RESULT_TABLE_S * pResultTbl)
{
	TS_U32 i;
	ALG_MOTION_DETECT_RESULT_S result = {0};
	VENC_MOTION_DETECT_RESULT_S venc_motion_info = {0};
	TS_U32 u32Pid = 0;
	TS_S32 s32Ret = 0;

	s32Ret = TS_ALG_MotionDetect_Process(pHandle, pImage, &result);
	if(TS_SUCCESS != s32Ret){
		return TS_FAILURE;
	}

	memcpy(&venc_motion_info,&result,sizeof(VENC_MOTION_DETECT_RESULT_S));

	if(venc_motion_info.u32ObjNum >= 0){
		s32Ret = TS_MPI_VENC_SetMotionDetectParam(0, &venc_motion_info);
		if(TS_SUCCESS != s32Ret){
			SAMPLE_PRT("TS_MPI_VENC_SetMotionDetectParam faild, objNum=%d\n",
				venc_motion_info.u32ObjNum);
		}
		//SAMPLE_PRT("TS_MPI_VENC_SetMotionDetectParam objNum=%d\n", venc_motion_info.u32ObjNum);
	}

	for(i = 0; i < result.u32ObjNum; i ++){
		SAMPLE_ALGO_RESULT_S * pResult;

		if(pResultTbl->result_num >= SAMPLE_ALGO_RESULT_TABLE_SIZE_MAX){
			SAMPLE_PRT("reach max of algo result table size[%d]\n", SAMPLE_ALGO_RESULT_TABLE_SIZE_MAX);
			return TS_SUCCESS;
		}

		if(result.stBox[i].f32Xmin < 0
			|| result.stBox[i].f32Xmax > 1
			|| result.stBox[i].f32Ymin < 0
			|| result.stBox[i].f32Ymax > 1){
			SAMPLE_PRT("motion detect get invalid box xy=[%f,%f][%f,%f]. skip this one\n",
				result.stBox[i].f32Xmin, result.stBox[i].f32Xmax,
				result.stBox[i].f32Ymin, result.stBox[i].f32Ymax);
			continue;
		}

		pResult = &pResultTbl->result_tbl[pResultTbl->result_num];
		pResult->f32Xmin	= result.stBox[i].f32Xmin;
		pResult->f32Xmax	= result.stBox[i].f32Xmax;
		pResult->f32Ymin	= result.stBox[i].f32Ymin;
		pResult->f32Ymax	= result.stBox[i].f32Ymax;
		pResult->f32Score	= result.stBox[i].f32Score;
		pResult->type		= SAMPLE_ALGO_OBJ_TYPE_MOTION;
		pResultTbl->result_num ++;
	}

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_ALGO_Process_PcppDetect(TS_VOID *pHandle, ALG_IMAGE_S *pImage, SAMPLE_ALGO_RESULT_TABLE_S * pResultTbl)
{
	TS_U32 i;
	ALG_PCPP_DET_RESULT_S result = {0};


	TS_S32 s32Ret = TS_ALG_PcppDet_Process(pHandle, pImage, &result);
	if(TS_SUCCESS != s32Ret){
		return TS_FAILURE;
	}

	for(i = 0; i < result.u32ObjNum; i ++){
		SAMPLE_ALGO_RESULT_S * pResult;

		if(pResultTbl->result_num >= SAMPLE_ALGO_RESULT_TABLE_SIZE_MAX){
			SAMPLE_PRT("reach max of algo result table size[%d]\n", SAMPLE_ALGO_RESULT_TABLE_SIZE_MAX);
			return TS_SUCCESS;
		}

		if(result.stBox[i].f32Xmin < 0
			|| result.stBox[i].f32Xmax > 1
			|| result.stBox[i].f32Ymin < 0
			|| result.stBox[i].f32Ymax > 1){
			SAMPLE_PRT("motion detect get invalid box xy=[%f,%f][%f,%f]. skip this one\n",
				result.stBox[i].f32Xmin, result.stBox[i].f32Xmax,
				result.stBox[i].f32Ymin, result.stBox[i].f32Ymax);
			continue;
		}
		pResult = &pResultTbl->result_tbl[pResultTbl->result_num];
		pResult->f32Xmin	= result.stBox[i].f32Xmin;
		pResult->f32Xmax	= result.stBox[i].f32Xmax;
		pResult->f32Ymin	= result.stBox[i].f32Ymin;
		pResult->f32Ymax	= result.stBox[i].f32Ymax;
		pResult->f32Score	= result.stBox[i].f32Score;
		switch(result.u8ObjNo[i]){
			case 0:{
				pResult->type = SAMPLE_ALGO_OBJ_TYPE_PEDESTRIAN;
				break;
			}
			case 1:{
				pResult->type = SAMPLE_ALGO_OBJ_TYPE_CAR;
				break;
			}
			case 2:{
				pResult->type = SAMPLE_ALGO_OBJ_TYPE_PET;
				break;
			}
			case 3:{
				pResult->type = SAMPLE_ALGO_OBJ_TYPE_PACKAGE;
				break;
			}
			default:{
				pResult->type = SAMPLE_ALGO_OBJ_TYPE_PEDESTRIAN;
				break;
			}
		}
		pResultTbl->result_num ++;

//		printf("PCPP X min_max [%f,%f] Y min_max [%f,%f] type[%d]\n",
//			pResult->f32Xmin, pResult->f32Xmax,
//			pResult->f32Ymin, pResult->f32Ymax, result.u8ObjNo[i]);
	}

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_ALGO_Init_FaceDetect(SAMPLE_ALGO_INSTANCE_S * pInstance)
{

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_ALGO_Init_BodyDetect(SAMPLE_ALGO_INSTANCE_S * pInstance)
{
	TS_S32 s32Ret;
	ALG_MODEL_INIT_S param = {0};
	FILE *pf_cfg = NULL;
	FILE *pf_weight = NULL;

	pf_cfg = fopen("/usr/bin/bodydetect_cfg.cfg", "rb");
	pf_weight = fopen("/usr/bin/bodydetect_weight.weight", "rb");

	if((NULL == pf_cfg) || (NULL == pf_weight)){
		SAMPLE_PRT("model file fopen failed !!!\n");
		return TS_FAILURE;
	}

	TS_U32 u32CfgSize = SAMPLE_COMM_ALGO_GetFileSize(pf_cfg);
	TS_U32 u32WeightSize = SAMPLE_COMM_ALGO_GetFileSize(pf_weight);

	TS_U8 *pCfgAddr = (TS_U8 *)malloc(u32CfgSize*sizeof(TS_U8));
	TS_U8 *pWeightAddr = (TS_U8 *)malloc(u32WeightSize*sizeof(TS_U8));
	fread(pCfgAddr, u32CfgSize, 1, pf_cfg);
	fread(pWeightAddr, u32WeightSize, 1, pf_weight);

	param.pGraph = pCfgAddr;
	param.pWeight = pWeightAddr;
	param.u32GraphSize = u32CfgSize;
	param.u32WeightSize = u32WeightSize;
	param.eImageType = ALG_IMAGE_TYPE_INT_HWC_RGB0;
	param.bRneOff = TS_FALSE;
	s32Ret = TS_ALG_BodyDet_Init(&pInstance->pHandle, &param);
	TS_ALG_SetLogLevel(ALG_LOG_ERROR);
	if(TS_SUCCESS != s32Ret){
		return TS_FAILURE;
	}

	free(pWeightAddr);
	free(pCfgAddr);

	fclose(pf_cfg);
	fclose(pf_weight);

	pInstance->pf_process = SAMPLE_COMM_ALGO_Process_BodyDetect;
	pInstance->pf_exit = TS_ALG_BodyDet_Exit;
	SAMPLE_PRT("%s\n", TS_ALG_BodyDet_GetVersion());
	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_ALGO_Init_MotionDetect(SAMPLE_ALGO_INSTANCE_S * pInstance)
{
	ALG_MOTION_DETECT_INIT_S param = {0};
	ALG_MOTION_DETECT_PARAM_S MOTION_DETECT = {0};

	param.u32ImgW = ALG_DETECT_IMAGE_W;
	param.u32ImgH = ALG_DETECT_ENTIRE_H;
	param.u32ImgC = 4;

	TS_S32 s32Ret = TS_ALG_MotionDetect_Init(&pInstance->pHandle, &param);
	if(TS_SUCCESS != s32Ret){
		return TS_FAILURE;
	}
	s32Ret = TS_ALG_MotionDetect_GetParam(pInstance->pHandle, &MOTION_DETECT);
    if(TS_SUCCESS != s32Ret){
          return TS_FAILURE;
    }
      //printf("[u32DiffThres=%d,u32AreaThres=%d]\n",MOTION_DETECT.u32DiffThres,MOTION_DETECT.u32AreaThres);
      MOTION_DETECT.u32DiffThres = 100;
      MOTION_DETECT.u32AreaThres = 200;

      s32Ret = TS_ALG_MotionDetect_SetParam(pInstance->pHandle, &MOTION_DETECT);
      if(TS_SUCCESS != s32Ret){
              return TS_FAILURE;
      }

	SAMPLE_PRT("%s\n", TS_ALG_MotionDetect_GetVersion());
	TS_ALG_SetLogLevel(ALG_LOG_ERROR);
	pInstance->pf_process = SAMPLE_COMM_ALGO_Process_MotionDetect;
	pInstance->pf_exit = TS_ALG_MotionDetect_Exit;

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_ALGO_Init_ObjDetect(SAMPLE_ALGO_INSTANCE_S * pInstance)
{
	return TS_FAILURE;
}

TS_S32 SAMPLE_COMM_ALGO_Init_pcppDetect(SAMPLE_ALGO_INSTANCE_S * pInstance)
{
	static void *handle = 0;
	static int flag = 0;
	if (flag) {
		SAMPLE_PRT("Already inited\n");
		pInstance->pHandle = handle;
		return 0;
	} else {
		flag = 1;
	}//TODO: add mutex for the whole function
	//OR TS_ALG_PcppDetect_Init support multi-thread with different handle, delete this code

	TS_S32 s32Ret;
	FILE *pf_cfg = NULL;
	FILE *pf_weight = NULL;
	ALG_MODEL_INIT_S param = {0};

	pf_cfg = fopen("/usr/bin/pcppdetect_cfg.cfg", "rb");
	pf_weight = fopen("/usr/bin/pcppdetect_weight.weight", "rb");

	if((NULL == pf_cfg) || (NULL == pf_weight)){
		SAMPLE_PRT("model file fopen failed !!!\n");
		return TS_FAILURE;
	}

	TS_U32 u32CfgSize = SAMPLE_COMM_ALGO_GetFileSize(pf_cfg);
	TS_U32 u32WeightSize = SAMPLE_COMM_ALGO_GetFileSize(pf_weight);

	TS_U8 *pCfgAddr = (TS_U8 *)malloc(u32CfgSize*sizeof(TS_U8));
	TS_U8 *pWeightAddr = (TS_U8 *)malloc(u32WeightSize*sizeof(TS_U8));
	fread(pCfgAddr, u32CfgSize, 1, pf_cfg);
	fread(pWeightAddr, u32WeightSize, 1, pf_weight);

	param.pGraph = pCfgAddr;
	param.pWeight = pWeightAddr;
	param.u32GraphSize = u32CfgSize;
	param.u32WeightSize = u32WeightSize;
	param.eImageType = ALG_IMAGE_TYPE_INT_HWC_RGB0;
	param.bRneOff = TS_FALSE;

	s32Ret = TS_ALG_PcppDet_Init(&pInstance->pHandle, &param);
	if(TS_SUCCESS != s32Ret){
		return TS_FAILURE;
	}

	TS_ALG_SetLogLevel(ALG_LOG_ERROR);

	free(pWeightAddr);
	free(pCfgAddr);

	fclose(pf_cfg);
	fclose(pf_weight);

	pInstance->pf_process = SAMPLE_COMM_ALGO_Process_PcppDetect;
	pInstance->pf_exit = TS_ALG_PcppDet_Exit;
	handle = pInstance->pHandle;

	SAMPLE_PRT("%s\n", TS_ALG_PcppDet_GetVersion());

	return TS_SUCCESS;
}

SAMPLE_ALGO_INSTANCE_S * SAMPLE_COMM_ALGO_Init_Instance(SAMPLE_ALGO_TYPE_E algType)
{
	SAMPLE_ALGO_INSTANCE_S * pInst = (SAMPLE_ALGO_INSTANCE_S *)malloc(sizeof(SAMPLE_ALGO_INSTANCE_S));
	if(!pInst){
		return TS_NULL;
	}

	memset(pInst, 0,sizeof(SAMPLE_ALGO_INSTANCE_S));

	if(0 != TS_MPI_TRP_RNE_MutexInit()){
		printf("ai vae trp rne init failed !");
		return NULL;
	}else {
		printf("[SAMPLE_COMM_ALGO_Init_Instance] [TS_MPI_TRP_RNE_MutexInit] Sucess !!!");
	}

	if (0 != TS_MPI_TRP_RNE_MutexLockWithTimeout(1, 5000)) {
				printf("ai trp rne lock timeout failed !\n");
				return NULL;
	}else{
			printf("[SAMPLE_COMM_ALGO_Init_Instance] [TS_MPI_TRP_RNE_MutexLockWithTimeout success !!!\n");
	}

	switch(algType){
		case SAMPLE_ALGO_TYPE_FACE_DETECT:{
			if(SAMPLE_COMM_ALGO_Init_FaceDetect(pInst) != TS_SUCCESS){
				goto algo_init_instance_faild;
			}
			break;
		}
		case SAMPLE_ALGO_TYPE_BODY_DETECT:{
			if(SAMPLE_COMM_ALGO_Init_BodyDetect(pInst) != TS_SUCCESS){
				goto algo_init_instance_faild;
			}
			break;
		}
		case SAMPLE_ALGO_TYPE_OBJ_DETECT:{
			if(SAMPLE_COMM_ALGO_Init_ObjDetect(pInst) != TS_SUCCESS){
				goto algo_init_instance_faild;
			}
			break;
		}
		case SAMPLE_ALGO_TYPE_MOTION_DETECT:{
			if(SAMPLE_COMM_ALGO_Init_MotionDetect(pInst) != TS_SUCCESS){
				goto algo_init_instance_faild;
			}
			break;
		}
		case SAMPLE_ALGO_TYPE_PCPP_DETECT:{
			if(SAMPLE_COMM_ALGO_Init_pcppDetect(pInst) != TS_SUCCESS){
				goto algo_init_instance_faild;
			}
			break;
		}
		default:{
			if(SAMPLE_COMM_ALGO_Init_FaceDetect(pInst) != TS_SUCCESS){
				goto algo_init_instance_faild;
			}
			break;
		}
	}

	if (0 != TS_MPI_TRP_RNE_MutexUnlock()) {
		printf("ai trp rne unlock failed !\n");
		return TS_NULL;
	}

	pInst->enAlgoType = algType;

	return pInst;

algo_init_instance_faild:
	if(pInst){
		free(pInst);
	}

	return TS_NULL;
}

TS_S32 SAMPLE_COMM_ALGO_Init_Instances(SAMPLE_ALGO_INSTANCE_S ** ppAlgoInst_tbl, TS_U32 algMask)
{
	TS_U32 idx = 0;
	SAMPLE_ALGO_INSTANCE_S *tmp = TS_NULL;

	if(algMask & ALGO_BIT_FACE_DETECT){
		tmp = SAMPLE_COMM_ALGO_Init_Instance(SAMPLE_ALGO_TYPE_FACE_DETECT);
		if (!tmp) {
			return TS_FAILURE;
		} else {
			*(ppAlgoInst_tbl + idx ++) = tmp;
		}
	}

	if(algMask & ALGO_BIT_BODY_DETECT){
		tmp = SAMPLE_COMM_ALGO_Init_Instance(SAMPLE_ALGO_TYPE_BODY_DETECT);
		if (!tmp) {
			return TS_FAILURE;
		} else {
			*(ppAlgoInst_tbl + idx ++) = tmp;
		}
	}

	if(algMask & ALGO_BIT_MOTION_DETECT){
		tmp = SAMPLE_COMM_ALGO_Init_Instance(SAMPLE_ALGO_TYPE_MOTION_DETECT);
		if (!tmp) {
			return TS_FAILURE;
		} else {
			*(ppAlgoInst_tbl + idx ++) = tmp;
		}
	}

	if(algMask & ALGO_BIT_OBJ_DETECT){
		tmp = SAMPLE_COMM_ALGO_Init_Instance(SAMPLE_ALGO_TYPE_OBJ_DETECT);
		if (!tmp) {
			return TS_FAILURE;
		} else {
			*(ppAlgoInst_tbl + idx ++) = tmp;
		}
	}

	if(algMask & ALGO_BIT_PCPP_DETECT){
		tmp = SAMPLE_COMM_ALGO_Init_Instance(SAMPLE_ALGO_TYPE_PCPP_DETECT);
		if (!tmp) {
			return TS_FAILURE;
		} else {
			*(ppAlgoInst_tbl + idx ++) = tmp;
		}
	}

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_ALGO_Deinit_Instances(SAMPLE_ALGO_INSTANCE_S ** ppAlgoInst_tbl, TS_U32 algNum)
{
	TS_S32 i;

	for(i = 0; i < algNum; i ++){
		SAMPLE_ALGO_INSTANCE_S * pInst = *(ppAlgoInst_tbl + i);

		if(pInst->pf_exit && pInst->pHandle){
			pInst->pf_exit(pInst->pHandle);
		}
	}

	return TS_SUCCESS;
}
