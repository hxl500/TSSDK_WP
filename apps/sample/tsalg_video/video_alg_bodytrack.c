
#include <pthread.h>
#include <semaphore.h>
#include <sys/prctl.h>

#include "video_alg_bodytrack.h"
#include "video_common_utils.h"
#include "soft_line.h"

#if ALG_BODYTRACK

static ALG_BODYDET_RESULT_S gstBodyDetResult;
//static sem_t      gstTraceProcSem; 
static int  gTrace_flag = 1;

//镜头水平、垂直视场角，和镜头有关系
#define TS_H_FOV  (91U) //水平视场角
#define TS_V_FOV  (46U) //垂直视场角

#define ALG_RGB_CHN 	   4
#define ALG_SKIP_INTERVAL 7

typedef struct
{
    float cx_;       // 目标左上角x坐标
    float cy_;       // 目标左上角y坐标
    float width_;    // 目标宽
    float height_;   // 目标高
}TS_RECT_FLOAT;


extern int ts_motor_run(int hor_val,int ver_val);
extern int ts_motor_reset_to_middle();
extern int ts_motor_init();

//将人形框中心点坐标转换为图像中的角度
static TS_S32 TS_calcObjectTraceAngle(TS_RECT_FLOAT *pRect, float *pAngleX, float *pAngleY)
{
    float center_x = 0.0;
    float center_y = 0.0;
    float angle_x = 0.0;
    float angle_y = 0.0;
    center_x = pRect->cx_+pRect->width_/2;
    center_y = pRect->cy_+pRect->height_/2;
    angle_x = center_x * TS_H_FOV;
    angle_y = center_y * TS_V_FOV;

    *pAngleX = angle_x - TS_H_FOV/2;
    *pAngleY = angle_y - TS_V_FOV/2;
    //printf("x:%.2f,y:%.2f\n",*pAngleX,*pAngleY);
    return TS_SUCCESS;
}

//跟踪目标选择
static void TS_chooseTrackObj(const TS_RECT_FLOAT *pObjs, const int num, TS_RECT_FLOAT *pChooseObj)
{
    // #TODO,处理一下det_objs为空的情况
    static int initFlag = 0;
    static TS_RECT_FLOAT lastTraceObj;

    if (!initFlag)  /* 第一次选人形框最大的 */
    {
        float maxArea = 0.0;
        int choosedIdx = 0;
        for (int i = 0; i < num; ++i)
        {
            float area = pObjs[i].width_ * pObjs[i].height_;

            if (area > maxArea)
            {
                maxArea = area;
                choosedIdx = i;
            }
        }
        *pChooseObj = pObjs[choosedIdx];
        lastTraceObj = pObjs[choosedIdx];

        initFlag = 1;
    }
    else  /* 后续选择目标是离上一帧跟踪目标最近的坐标 */
    {
        int choosedIdx = 0;
        float last_centerx = lastTraceObj.cx_+lastTraceObj.width_/2;
        float minDist = 1.0;
        for (int i = 1; i< num; ++i)  // 再遍历其它目标，最终的目的是选个距离上帧跟踪目标最近的
        {
            float dist = abs(last_centerx-(pObjs[i].cx_+pObjs[i].width_/2));
            if (dist < minDist)
            {
                minDist = dist;
                choosedIdx = i;
            }
        }

        *pChooseObj = pObjs[choosedIdx];
        lastTraceObj = pObjs[choosedIdx];
    }
}

static void TS_autoTraceMotorProc()
{
	int move_x = 0;
	int move_y = 0;
	int motor_reset_flag = 0;

    prctl(PR_SET_NAME, (unsigned long)__FUNCTION__, 0, 0, 0);

	while(1)
	{
        if (0 == gTrace_flag) break;

        //sem_wait(&gstTraceProcSem);

		if (gstBodyDetResult.u32BodyNum > 0)
		{
					// noPersonCnt = 0;
			TS_RECT_FLOAT rectF[25] = {0};
			TS_RECT_FLOAT rectFtmp = {0};
			float angleX = 0.0;
			float angleY = 0.0;
			for (int i = 0; i < gstBodyDetResult.u32BodyNum ; i++)
			{
				rectF[i].cx_ = gstBodyDetResult.stBox[i].f32Xmin;
				rectF[i].cy_ = gstBodyDetResult.stBox[i].f32Ymin;
				rectF[i].width_ = gstBodyDetResult.stBox[i].f32Xmax - gstBodyDetResult.stBox[i].f32Xmin;
				rectF[i].height_ = gstBodyDetResult.stBox[i].f32Ymax - gstBodyDetResult.stBox[i].f32Ymin;
			}

			TS_chooseTrackObj(rectF, gstBodyDetResult.u32BodyNum, &rectFtmp);
			//printf("chose: x:%.1f,y:%.1f,w:%.1f,h:%.1f\n",rectFtmp.cx_,rectFtmp.cy_,rectFtmp.width_,rectFtmp.height_);
			//TS_calcObjectTraceAngle(&rectFtmp, &angleX, &angleY);
			TS_calcObjectTraceAngle(&rectFtmp, &angleX, &angleY);
			if(abs(angleX)>15)
				move_x = 5;
			else
				move_x = 0;

			if(abs(angleY)>8)
				move_y = 5;
			else
				move_y = 0;

			if(angleX>0)
				move_x = -move_x;
			if(angleY<0)
				move_y = -move_y;

			ts_motor_run(move_x,move_y);

			//printf("==========x = %f %f %d %d ==========\r\n", angleX, angleY, stepx, -stepy);
			//printf("move_x:%d,move_y:%d, angleX:%f, angleY:%f\n",move_x,move_y, angleX, angleY);
			//motor_set_posion(stepx, stepy);
			usleep(70*1000);
		}
		else
		{
			motor_reset_flag++;
			usleep(200*1000);
			if(motor_reset_flag == 50)
			{
				motor_reset_flag = 0;
				ts_motor_reset_to_middle();
			}
		}

	}
}

static TS_S32 SAMPLE_AutoTrack()
{
    int ret;
    ts_motor_init();
    // if (sem_init(&gstTraceProcSem, 0, 0) == -1) {
    //      printf("[ERROR]: gstTraceProcSem init error\n");
    // }
    
    pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    ret = pthread_create(&thread, &attr,(void*) TS_autoTraceMotorProc, NULL);
    if (ret)
    {
        printf("[ERROR]: create md/cd thread failed, ret=%d\n", ret);
    }
	pthread_detach(thread);
    return ret;
}

TS_S32 VIDEO_ALG_BodyTrack_Init(TS_VOID **pHandle)
{
    TS_S32 s32Ret = TS_SUCCESS;

    memset(&gstBodyDetResult, 0, sizeof(ALG_BODYDET_RESULT_S));

	ALG_MODEL_INIT_S param;
	TS_ALG_SetLogLevel(ALG_LOG_ERROR);
    SAMPLE_ALG_INSTANCE_S* pInst = *((SAMPLE_ALG_INSTANCE_S**)pHandle);
    
    FILE *pf_cfg = NULL;
	FILE *pf_weight = NULL;
 
    TS_U8 weightpath[] = "./bodydetect_weight.weight";
	TS_U8 cfgpath[] = "./bodydetect_cfg.cfg";

	char cfg_file[SAMPLE_MODEL_FILE_LEN] = {0};
	char weight_file[SAMPLE_MODEL_FILE_LEN] = {0};
	if (TS_NULL == g_pcModelRoot){
		sprintf(cfg_file, "%s", cfgpath);
		sprintf(weight_file, "%s", weightpath);
	}else {
		sprintf(cfg_file, "%s/%s", g_pcModelRoot, cfgpath);
		sprintf(weight_file, "%s/%s", g_pcModelRoot, weightpath);
	}

    pf_cfg = fopen((const char *)cfg_file, "rb");
    pf_weight = fopen((const char *)weight_file, "rb");
	if ((NULL == pf_cfg) || (NULL == pf_weight)) {
		ALG_LOGE("bodytrack model file fopen failed !!!\n");
        ALG_LOGE("model_cfg: %s, model_weight:%s\n", cfg_file,  weight_file);
		return TS_FAILURE;
	}

	TS_U32 u32WeightLen = SAMPLE_ALGO_GetFileSize(pf_weight);
    TS_U8 *pWeightBuf = (TS_U8 *)malloc(u32WeightLen);
	fread(pWeightBuf, u32WeightLen, 1, pf_weight);
	fclose(pf_weight);

	TS_U32 u32CfgLen = SAMPLE_ALGO_GetFileSize(pf_cfg);
	TS_U8 *pCfgBuf = (TS_U8 *)malloc(u32CfgLen);
	fread(pCfgBuf, u32CfgLen, 1, pf_cfg);
	fclose(pf_cfg);

    memset(&param, 0, sizeof(ALG_MODEL_INIT_S));
    param.pGraph = pCfgBuf;
	param.pWeight = pWeightBuf;
	param.u32GraphSize = u32CfgLen;
	param.u32WeightSize = u32WeightLen;
	param.vpSelfBuf = NULL;
	param.eImageType = ALG_IMAGE_TYPE_INT_HWC_RGB0;
	param.bRneOff = 0;

	ALG_LOGD("vpSelfBuf: %p\n", param.vpSelfBuf);

	s32Ret = TS_ALG_BodyDet_Init(&(pInst->pHandle), &param);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_BodyDet_Init error\n");
		goto init_exit;
	}

    SAMPLE_PRT("TS_ALG_BodyDet_GetVersion:%s\n", TS_ALG_BodyDet_GetVersion());
    s32Ret = TS_SUCCESS;
    pInst->pf_Exit = VIDEO_ALG_BodyTrack_Exit;
    pInst->pf_Result_Process = VIDEO_ALG_BodyTrack_ResultProc;
    pInst->pf_Process = VIDEO_ALG_BodyTrack_Proc;

    SAMPLE_AutoTrack();

init_exit:	
	free(pCfgBuf);
	free(pWeightBuf);

    return s32Ret;
}

TS_S32 VIDEO_ALG_BodyTrack_Proc(TS_VOID *pHandle, ALG_IMAGE_S *pImageOri, ALG_IMAGE_S *pImageDet, TS_VOID *pResult)
{
    TS_S32 s32Ret = TS_SUCCESS;

    SAMPLE_ALG_INSTANCE_S* pInst = (SAMPLE_ALG_INSTANCE_S*)pHandle;
	if (!pHandle|| !pImageDet || !pResult) {
		ALG_LOGE("VIDEO_ALG_BodyDetect_Proc param is null\n");
		return TS_FAILURE;
	}

	SAMPLE_ALG_RESULT_S *pCurResult = (SAMPLE_ALG_RESULT_S *)pResult;
    ALG_BODYDET_RESULT_S *pTmpResult = &(pCurResult->gstAlgBodyResult);
	s32Ret = TS_ALG_BodyDet_Process(pInst->pHandle, pImageDet, pTmpResult);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_BodyDet_Process error\n");
	}

	//printf("TS_ALG_BodyDetect_Process %d body\n", pTmpResult->u32BodyNum);

    return s32Ret;
}

TS_VOID VIDEO_ALG_BodyTrack_ResultProc(TS_U8 *pYuvBuf,  TS_U32 width, TS_U32 height,TS_U32 u32ImageRatio, TS_VOID *pAlgResult)
{
    SAMPLE_ALG_RESULT_S *pCurResult = (SAMPLE_ALG_RESULT_S *)pAlgResult;
	ALG_BODYDET_RESULT_S *pResult = &(pCurResult->gstAlgBodyResult);

    float fThres = 0.001f; 
    RECT rect;
	for (int i = 0; i < pResult->u32BodyNum; i++) {
        if (pResult->stBox[i].f32Score > fThres) {
            if (pResult->stBox[i].f32Xmin < 0 || pResult->stBox[i].f32Xmax > 1
			   || pResult->stBox[i].f32Ymin < 0 || pResult->stBox[i].f32Ymax > 1) { 
                SAMPLE_PRT("bodydetect get invalid box xy=[%f,%f][%f,%f]. skip this one\n",
				pResult->stBox[i].f32Xmin, pResult->stBox[i].f32Xmax,
				pResult->stBox[i].f32Ymin, pResult->stBox[i].f32Ymax);
			    continue;
		    }

            rect.left = u32ImageRatio * pResult->stBox[i].f32Xmin  * width;
            rect.top = u32ImageRatio * pResult->stBox[i].f32Ymin *  height;
            rect.right = u32ImageRatio * pResult->stBox[i].f32Xmax * width;
            rect.bottom = u32ImageRatio *pResult->stBox[i].f32Ymax * height;
            YUV_Draw_Rect(pYuvBuf, width, height, 12, &rect, YUV_RED, 5);
		}
	}
    
    memcpy(&gstBodyDetResult, pResult, sizeof(ALG_BODYDET_RESULT_S));

    //sem_post(&gstTraceProcSem);
}

TS_S32 VIDEO_ALG_BodyTrack_Exit(TS_VOID *pHandle)
{

    gTrace_flag = 0;
    //sem_destroy(&gstTraceProcSem);

    TS_S32 s32Ret = TS_SUCCESS;
    if (!pHandle) {
		ALG_LOGE("VIDEO_ALG_BodyDetect_Exit param is null\n");
		return TS_FAILURE;
	}

    SAMPLE_ALG_INSTANCE_S* pInst = (SAMPLE_ALG_INSTANCE_S*)pHandle;
    s32Ret = TS_ALG_BodyDet_Exit(pInst->pHandle);
	if (0 != s32Ret) {
        ALG_LOGE("TS_ALG_BodyDet_Exit error\n");
    }
    return s32Ret;
}
#endif
