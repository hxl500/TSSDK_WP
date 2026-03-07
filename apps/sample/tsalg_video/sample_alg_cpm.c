
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
#include <semaphore.h>
#include <dirent.h>
#include <sys/prctl.h>

#include "sample_alg_cpm.h"

#include "ts_alg_imgproc.h"
#include "ts_alg_type.h"
#include "ts_alg_log.h"
#include "ts_rne_device.h"
#include "ts_alg_gp_layers.h"
#include "mpi_sys.h"
// #include "soft_line.h"

#include "video_alg_catdetect-api.h"

#define USE_CPM 1

#define SINGLE_WIDTH 640
#define SINGLE_HEIGHT 360
#define DST_WIDTH 640 // 640
#define DST_HEIGHT 640
#define ALG_RGBA_CHN 4
#define ALG_RGB_SIZE (DST_WIDTH * DST_HEIGHT * ALG_RGBA_CHN)
#define ALGO_YUV_SIZE (DST_WIDTH * DST_HEIGHT * 3 / 2)
#define CPM_ALG_CYCLE 100
#define VI_GET_FRAME_TIMEOUT 1000


#define VI_DEV_ID 1                              // 1                              // VI设备号
#define VI_CHN_ID 1                              // VI通道号
#define CPM_ALG_CYCLE 20                         // CPM算法周期(ms)
#define VI_GET_FRAME_TIMEOUT (CPM_ALG_CYCLE / 2) // 取帧超时时间
#define VI_FRAME_BUF_DEPTH 4                     // VI通道帧缓存深度

/*******************************************************
 * *
 * *    alg instance init table
 * *
 ********************************************************/
static SAMPLE_ALG_INIT_CFG_S gstAlgInitTbl[] = {
#if ALG_MOTIONDETECT
    {SAMPLE_ALG_MASK_MOTION, VIDEO_ALG_Motion_Init, "MotionDetect", VIDEO_CHN_MODE0},
#endif

#if ALG_PCPPDETECT
    {SAMPLE_ALG_MASK_PCPPDETECT, VIDEO_ALG_MultiObj_Init, "PcppDetect", VIDEO_CHN_MODE0},
#endif

#if ALG_AIPERIMETER
    {SAMPLE_ALG_MASK_AIPERIMETER, VIDEO_ALG_AIPerimeter_Init, "AIPerimeter", VIDEO_CHN_MODE0},
#endif

#if ALG_BODYTRACK
    {SAMPLE_ALG_MASK_BODYTRACK, VIDEO_ALG_BodyTrack_Init, "BodyTrack", VIDEO_CHN_MODE0},
#endif
#if ALG_CAT_DETECT
    {SAMPLE_ALG_CAT_DETECT, VIDEO_ALG_CatDetect_Init, "CatDetect"},
#endif
};

/*******************************************************
 * *
 * *    alg cpm model
 * *
 ********************************************************/

SAMPLE_VIDEO_ALG_CPM gstSampleVideoAlgCpm[2];
ALG_IMAGE_S AlgoFaceIn;
ALG_IMAGE_S AlgoSrcFaceIn;
pthread_mutex_t g_AlgoFaceInLock = PTHREAD_MUTEX_INITIALIZER;

TS_S32 SAMPLE_ALG_CPM_HANDLE_Init(TS_VOID **pHandle);
TS_VOID SAMPLE_ALG_CPM_HANDLE_Exit(TS_VOID *pHandle);
TS_S32 SAMPLE_ALG_CPM_HANDLE_Process(TS_VOID *pHandle, TS_VOID **in, TS_VOID **out);
TS_S32 SAMPLE_ALG_CPM_HANDLE_SetParam(TS_VOID *pHandle, TS_VOID *pParam);
TS_S32 SAMPLE_ALG_CPM_HANDLE_GetParam(TS_VOID *pHandle, TS_VOID *pParam);
TS_S32 SAMPLE_ALG_CPM_HANDLE_GetResult(TS_VOID *pHandle, TS_VOID *pResult);
TS_S32 SAMPLE_ALG_CPM_HANDLE_ReleaseResult(TS_VOID *pHandle, TS_VOID *pResult);

TS_VOID SAMPLE_CPM_GetSrcSize(TS_U32 *pw, TS_U32 *ph, TS_U32 CPMGrp)
{
    if (TS_NULL == pw || TS_NULL == ph)
    {
        ALG_LOGE("SAMPLE_CPM_GetSrcSize error, NULL ptr \n");
        return;
    }

    *pw = gstSampleVideoAlgCpm[CPMGrp].u32Width;
    *ph = gstSampleVideoAlgCpm[CPMGrp].u32Height;
}

static TS_S32 SAMPLE_ALG_Init_Instances(SAMPLE_ALG_INSTANCE_S **ppAlgoInst_tbl, SAMPLE_ALG_TYPE_E *penAlgMask, TS_U32 u32AlgNum)
{
    TS_U32 i, j, idx = 0;
    SAMPLE_ALG_INSTANCE_S *pInst;
    bool bCurFlag;
    TS_U32 u32CpmAlgNum = sizeof(gstAlgInitTbl) / sizeof(SAMPLE_ALG_INIT_CFG_S);

    for (i = 0; i < u32AlgNum; i++)
    {
        bCurFlag = false;
        for (j = 0; j < u32CpmAlgNum; ++j)
        {
            if (gstAlgInitTbl[j].eMask == penAlgMask[i])
            {
                pInst = (SAMPLE_ALG_INSTANCE_S *)malloc(sizeof(SAMPLE_ALG_INSTANCE_S));
                if (!pInst)
                    return idx;

                memset(pInst, 0, sizeof(SAMPLE_ALG_INSTANCE_S));

                if (gstAlgInitTbl[j].pInit((void **)&pInst) != TS_SUCCESS)
                    goto algo_init_instance_faild;

                pInst->enAlgType = gstAlgInitTbl[j].eMask;
                *(ppAlgoInst_tbl + idx) = pInst;
                printf("ALG_Init_Instances, alg_type=0x%x, index=%d\n", penAlgMask[i], idx);
                idx++;
                bCurFlag = true;
                break;
            }
        }
        if (!bCurFlag)
            ALG_LOGE("not support algMask=%d \n", penAlgMask[i]);
    }
    return idx;

algo_init_instance_faild:
    if (pInst)
        free(pInst);

    return idx;
}

void SAMPLE_CPM_GetChnMode(SAMPLE_VIDEO_CHN_MODE mode, SAMPLE_ALG_CHN_IMG_SIZE_S *pstAlgChnSize)
{
    switch (mode)
    {
    case VIDEO_CHN_MODE0:
        pstAlgChnSize->width = 640;
        pstAlgChnSize->hight = 360;
        pstAlgChnSize->hight_ex = 24;
        break;
    case VIDEO_CHN_MODE1:
        pstAlgChnSize->width = 1280;
        pstAlgChnSize->hight = 720;
        pstAlgChnSize->hight_ex = 16;
        break;
    default:
        pstAlgChnSize->width = 640;
        pstAlgChnSize->hight = 360;
        pstAlgChnSize->hight_ex = 24;
        break;
    }

    printf("SAMPLE_CPM_GetChnMode, [w, h, exp_h]->[%d, %d, %d] \n", pstAlgChnSize->width,
           pstAlgChnSize->hight, pstAlgChnSize->hight_ex);
}

#if 0
static TS_S32 SaveYuvImage(const TS_CHAR *filepath, TS_U8 *yuv_data, const TS_S32 width, const TS_S32 height)
{
    FILE *pFile = NULL;
	// 打开输出文件
	if ((pFile = fopen(filepath, "wb")) == NULL) {
		ALG_LOGD(" fopen %s failed !. \n", filepath);
		return TS_FAILURE;
	}

	if (NULL != yuv_data) {
		size_t bytesToWrite = width * height * 3 / 2; // YUV420 格式下，总字节数为图像宽高的 1.5 倍
		size_t bytesWritten = fwrite(yuv_data, 1, bytesToWrite, pFile);
		if (bytesWritten != bytesToWrite) {
			fprintf(stderr, "Failed to write all YUV data to file !.\n");
		}
	}

	// 释放资源
	fclose(pFile);
	pFile = NULL;

	//ALG_LOGD(" szInfo: %s done \n", filepath);

	return TS_SUCCESS;
}
#endif

#if USE_CPM
static TS_S32 SAMPLE_ALG_Yuv2Rgb(TS_U8 *y_image, TS_U8 *uv_image, TS_U8 *rgb_image, TS_U32 src_width, TS_U32 src_height, TS_U32 des_width, TS_U32 des_height, ALG_RGB_TYPE_E rgb_type)
{
#if 0
    FILE *fp0 = NULL;
	FILE *fp1 = NULL;
    fp0 = fopen("./420sp.yuv", "r");
	if (fp0 == NULL)
	{
		printf("open fp0 error\n");
		return;
	}
	fread(yuv_image, 1, image_width * image_height * 3 /2, fp0);
	fclose(fp0);
#endif
    if (!y_image || !uv_image || !rgb_image)
    {
        SAMPLE_PRT("buf buf buf is NULL!\n");
    }
    TS_ALG_YUV2RGB(y_image, uv_image, rgb_image, src_width, src_height, des_width, des_height, rgb_type);
#if 0
	fp1 = fopen("./out.rgb", "w+");
	fwrite(rgb_image, 1, image_width * image_height * 3, fp1);
	fclose(fp1);
#endif
    return TS_SUCCESS;
}
#endif

SAMPLE_ALG_INIT_CFG_S *SAMPLE_ALG_GetInitCfg(TS_U32 *pNum)
{
    if (NULL == pNum)
    {
        ALG_LOGE("SAMPLE_ALG_GetInitCfg param is NULL!\n");
        return NULL;
    }

    *pNum = sizeof(gstAlgInitTbl) / sizeof(SAMPLE_ALG_INIT_CFG_S);
    return gstAlgInitTbl;
}

TS_S32 SAMPLE_ALG_CPM_Config(AVS_GRP CPMGrp, int pipeNum, TS_U32 u32Width, TS_U32 u32Height, SAMPLE_ALG_TYPE_E *penAlgType, TS_U32 u32AlgNum, SAMPLE_ALG_CHN_IMG_SIZE_S *pstAlgChnSize)
{
    TS_S32 s32Ret = TS_SUCCESS;
    TS_S32 i = 0;
    CPM_Handle_S stCPMHandle;
    CPM_GRP_ATTR_S stCPMGrpAttr;

    if (u32AlgNum > SAMPLE_ALG_MAX_NUM)
    {
        ALG_LOGE("SAMPLE_ALG_CPM_Config error, AlgNum is bigger than CPM support, u32AlgNum=%d, MaxAlgNum=%d !\n", u32AlgNum, SAMPLE_ALG_MAX_NUM);
        return TS_FAILURE;
    }

    memset(&stCPMGrpAttr, 0, sizeof(CPM_GRP_ATTR_S));
    stCPMGrpAttr.u32GrpId = 0;
    stCPMGrpAttr.u32PipeNum = pipeNum;
    stCPMGrpAttr.u32ChnNum = 1;
    stCPMGrpAttr.u32Interval = 1;
    stCPMGrpAttr.bSyncPipe = TS_FALSE;
    stCPMGrpAttr.stChnAttr[0].u32Width = u32Width;
    stCPMGrpAttr.stChnAttr[0].u32Height = u32Height;
    stCPMGrpAttr.stChnAttr[0].enPixelFormat = PIXEL_FORMAT_NV_12;

    // stCPMGrpAttr.stChnAttr[0].u32Depth = 8;

    memset(&gstSampleVideoAlgCpm[CPMGrp], 0, sizeof(SAMPLE_VIDEO_ALG_CPM));
    gstSampleVideoAlgCpm[CPMGrp].u32AlgNum = u32AlgNum;
    memcpy(gstSampleVideoAlgCpm[CPMGrp].enAlgMask, penAlgType, u32AlgNum * sizeof(SAMPLE_ALG_TYPE_E));
    gstSampleVideoAlgCpm[CPMGrp].u32Width = u32Width;
    gstSampleVideoAlgCpm[CPMGrp].u32Height = u32Height;

    gstSampleVideoAlgCpm[CPMGrp].stALgChnSize.hight = pstAlgChnSize->hight;
    gstSampleVideoAlgCpm[CPMGrp].stALgChnSize.width = pstAlgChnSize->width;
    gstSampleVideoAlgCpm[CPMGrp].stALgChnSize.hight_ex = pstAlgChnSize->hight_ex;

    printf("SAMPLE_ALGO_CPM_Config AlgNum =%d, AlgChansize[w,h,h_ex]=[%d,%d,%d]\n", gstSampleVideoAlgCpm[CPMGrp].u32AlgNum,
           pstAlgChnSize->width, pstAlgChnSize->hight, pstAlgChnSize->hight_ex);

    /*s32Ret = TS_MPI_CPM_CreateGrp(CPMGrp, &stCPMGrpAttr);
   if (TS_SUCCESS != s32Ret)
   {
        SAMPLE_PRT("Creat grp failed with %#x!\n", s32Ret);
        goto exit;
    }*/

    /*register algorithm*/
    memset(&stCPMHandle, 0, sizeof(CPM_Handle_S));
    stCPMHandle.s32PipeNum = pipeNum;
    for (i = 0; i < pipeNum; i++)
    {
        stCPMHandle.stPipeAttr[i].u32Width = u32Width;
        stCPMHandle.stPipeAttr[i].u32Height = u32Height;
        stCPMHandle.stPipeAttr[i].enPixelFormat = PIXEL_FORMAT_NV_12;

        /*algo channel*/
        if (i == 1)
        {
            stCPMHandle.stPipeAttr[1].u32Width = pstAlgChnSize->width;  // 640;
            stCPMHandle.stPipeAttr[1].u32Height = pstAlgChnSize->hight; // 360;
            stCPMHandle.stPipeAttr[1].enPixelFormat = PIXEL_FORMAT_NV_12;
            // stCPMHandle.stPipeAttr[i].enPixelFormat = PIXEL_FORMAT_ARGB_8888;
        }
    }

    stCPMHandle.init = SAMPLE_ALG_CPM_HANDLE_Init;
    stCPMHandle.exit = SAMPLE_ALG_CPM_HANDLE_Exit;
    stCPMHandle.process = SAMPLE_ALG_CPM_HANDLE_Process;
    stCPMHandle.set_param = SAMPLE_ALG_CPM_HANDLE_SetParam;
    stCPMHandle.get_param = SAMPLE_ALG_CPM_HANDLE_GetParam;
    stCPMHandle.get_result = SAMPLE_ALG_CPM_HANDLE_GetResult;
    stCPMHandle.release_result = SAMPLE_ALG_CPM_HANDLE_ReleaseResult;

    /*start cpm*/
#if SDK_VERSON_030
    s32Ret = SAMPLE_COMM_CPM_StartNew(CPMGrp, &stCPMGrpAttr, &stCPMHandle);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_CPM_StartNew failed with %#x!\n", s32Ret);
        goto exit;
    }
#else
    s32Ret = SAMPLE_COMM_CPM_Start(CPMGrp, &stCPMGrpAttr, &stCPMHandle);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_CPM_Start failed with %#x!\n", s32Ret);
        goto exit;
    }
#endif

    return TS_SUCCESS;

exit:
    TS_MPI_CPM_DestroyGrp(CPMGrp);

    return TS_FAILURE;
}

#if USE_CPM
static TS_S32 SAMPLE_ALG_Result_Proc(TS_U8 *YuvBuf, SAMPLE_ALG_RESULT_S *pstAlgResult, TS_U32 CPMGrp)
{
    TS_U32 width = gstSampleVideoAlgCpm[CPMGrp].u32Width;
    TS_U32 height = gstSampleVideoAlgCpm[CPMGrp].u32Height;
    TS_U32 ImageRatio = 1;

    // unsigned char * YuvBuf = (unsigned char *)(TS_UL)(in->stVFrame.u64VirAddr[0]);
    if (NULL == YuvBuf || NULL == pstAlgResult)
    {
        printf(" SAMPLE_ALG_ResultDraw, Param Is Null !\n");
        return TS_FAILURE;
    }

    TS_S32 i;
    for (i = 0; i < gstSampleVideoAlgCpm[CPMGrp].u32ActualAlgNum; ++i)
    {
        if (gstSampleVideoAlgCpm[CPMGrp].pstAlgInstList[i]->pf_Result_Process != NULL)
        {
            gstSampleVideoAlgCpm[CPMGrp].pstAlgInstList[i]->pf_Result_Process(YuvBuf, width, height, ImageRatio, (TS_VOID *)pstAlgResult);
        }
    }

    return TS_SUCCESS;
}
#endif



unsigned long long getSystemTime()
{
    unsigned long long int pts = 0;
    struct timeval tv1;
    gettimeofday(&tv1, NULL);
    pts = (unsigned long long int)tv1.tv_sec * 1000 + (tv1.tv_usec / 1000);
    return (long)pts;
}

void getDetailedTime(char *time_str, int max_len)
{
    struct timeval tv;
    struct tm *tm_info;

    gettimeofday(&tv, NULL);
    tm_info = localtime(&tv.tv_sec);

    snprintf(time_str, max_len, "%04d-%02d-%02d %02d:%02d:%02d.%03ld",
             tm_info->tm_year + 1900,
             tm_info->tm_mon + 1,
             tm_info->tm_mday,
             tm_info->tm_hour,
             tm_info->tm_min,
             tm_info->tm_sec,
             tv.tv_usec / 1000);
}

static TS_VOID *SAMPLE_CPM_ALG_Process(void *p)
{
#if USE_CPM
    TS_S32 s32Ret = TS_SUCCESS;
    SAMPLE_VIDEO_ALG_CPM *pCpmParam = (SAMPLE_VIDEO_ALG_CPM *)p;
    SAMPLE_ALG_INSTANCE_S *pcurAlgInst;
    static TS_S32 Process_frame_count = 0;

    TS_S32 algo_yuv_size = 640 * 640 * 3 / 2; // alg_chn_w * alg_chn_h * 3 / 2; 拼接双目
    int width = 640;
    int height = 360;
    int single_frame_size = width * height * 3 / 2;
    int concat_frame_size = single_frame_size * 2; // 拼接后大小约为原来的两倍
    TS_U8 *YuvImage = NULL;
   
    TS_U8 *frame_concat = (TS_U8 *)malloc(concat_frame_size);
    YuvImage = (TS_U8 *)malloc(algo_yuv_size);
    static TS_BOOL buffers_initialized = TS_FALSE;

    VIDEO_FRAME_INFO_S stFrameInfo = {0};
    s32Ret = TS_MPI_VPSS_EnableChn(VI_DEV_ID, VI_CHN_ID);
    if (s32Ret != TS_SUCCESS)
    {
        printf("TS_MPI_VPSS_EnableChn failed! ret = %d\n", s32Ret);
    }

    prctl(PR_SET_NAME, (unsigned long)"tsalg_proc_thread", 0, 0, 0);
    SAMPLE_PRT("SAMPLE_COMM_ALG_FACE_Process run (optimized thread with timing)\n");

    long t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10;
    static long total_time = 0;
    static long max_time = 0;
    static long min_time = 999999;
    char time_str[32];

    while (pCpmParam->bAlgProcRunFlag)
    {
    
        if (pCpmParam->enAlgProcBufStatus != BUFFER_STATUS_ALG_PROCESS)
        {
            // goto algo_next_loop;
            usleep(CPM_ALG_CYCLE * 1000);
            continue;
        }

        s32Ret = TS_MPI_VPSS_GetChnFrame(VI_DEV_ID, VI_CHN_ID, &stFrameInfo, VI_GET_FRAME_TIMEOUT);
        if (s32Ret != TS_SUCCESS)
        {
            printf("TS_MPI_VPSS_GetChnFrame failed! ret = %d, skip this frame\n", s32Ret);
            usleep(CPM_ALG_CYCLE * 1000);
            continue;
        }
        
        int ret = TS_NV12_Vertical_Concat_Correct(pCpmParam->stAlgBuffer.stVFrame.u64VirAddr[0], stFrameInfo.stVFrame.u64VirAddr[0], (uintptr_t)frame_concat, SINGLE_WIDTH, SINGLE_HEIGHT, DST_WIDTH, DST_HEIGHT);
        // ret = TS_NV12_Scale_Ex((uintptr_t)frame_concat, SINGLE_WIDTH, SINGLE_HEIGHT * 2, (uintptr_t)YuvImage, DST_WIDTH, DST_HEIGHT, 1);
        

        ret = TS_NV12_Scale_Fit_Fast(frame_concat, SINGLE_WIDTH, SINGLE_HEIGHT * 2, SINGLE_WIDTH,
                                   YuvImage, DST_WIDTH, DST_HEIGHT);
        if(ret != TS_SUCCESS){
            printf("TS_NV12_Scale_Fit_Fast failed! ret = %d\n", ret);
        }
        
        s32Ret = TS_MPI_VPSS_ReleaseChnFrame(VI_DEV_ID, VI_CHN_ID, &stFrameInfo);
        if (s32Ret != TS_SUCCESS)
        {
            printf("TS_MPI_VPSS_ReleaseChnFrame failed! ret = %d\n", s32Ret);
        }

        SAMPLE_ALG_Yuv2Rgb((uintptr_t)YuvImage,
                           (uintptr_t)YuvImage + DST_WIDTH * DST_HEIGHT,
                           (uintptr_t)AlgoFaceIn.pData, DST_WIDTH, DST_HEIGHT,
                           DST_WIDTH, DST_HEIGHT, ALG_RGB_TYPE_RGBA32);
        

        s32Ret = TS_MPI_SYS_MmzFlushCache(AlgoFaceIn.pDataPhy, AlgoFaceIn.pData, ALG_RGB_SIZE);
        if (0 != s32Ret)
        {
            SAMPLE_PRT("###error SAMPLE_CPM_ALG_Process, TS_MPI_SYS_MmzFlushcache\n");
        }
        
        pthread_mutex_lock(&g_AlgoFaceInLock);
        Process_frame_count++;
        AlgoFaceIn.s32H = DST_HEIGHT;
        AlgoFaceIn.s32W = DST_WIDTH;
        AlgoFaceIn.s32C = ALG_RGBA_CHN;

        AlgoSrcFaceIn.s32H = DST_HEIGHT;
        AlgoSrcFaceIn.s32W = DST_WIDTH;
        AlgoSrcFaceIn.s32C = 1;

        AlgoSrcFaceIn.pData = (void *)AlgoFaceIn.pData;
        AlgoSrcFaceIn.pDataPhy = pCpmParam->stAlgSrcBuffer.stVFrame.u64PhyAddr[0];
        pthread_mutex_unlock(&g_AlgoFaceInLock);
        

        for (int i = 0; i < pCpmParam->u32ActualAlgNum; ++i)
        {
            pcurAlgInst = pCpmParam->pstAlgInstList[i];
            if (pcurAlgInst->pf_Process != NULL)
            {
                s32Ret = pcurAlgInst->pf_Process(pcurAlgInst, &AlgoSrcFaceIn, &AlgoFaceIn, &pCpmParam->stAlgResult);
                if (TS_SUCCESS != s32Ret)
                {
                    SAMPLE_PRT("TS_ALGO_Process error\n");
                }
            }
        }
        
        VB_BLK tmp_vb_blk = TS_MPI_VB_PhysAddr2Handle(pCpmParam->stAlgBuffer.stVFrame.u64PhyAddr[0]);
        s32Ret = TS_MPI_VB_ReleaseBlock(tmp_vb_blk);
        if (s32Ret != 0)
        {
            SAMPLE_PRT("TS_MPI_VB_ReleaseBlock error\n");
        }
        
        pthread_mutex_lock(&(pCpmParam->stAlgProcLock));
        // Mark result as updated for main CPM process to process
        pCpmParam->bResultUpdate = TS_TRUE;
        pthread_mutex_unlock(&(pCpmParam->stAlgProcLock));
       
        pCpmParam->enAlgProcBufStatus = BUFFER_STATUS_FILL;


        // algo_next_loop:
        // usleep(1000 * 30);
    }
    if (NULL != YuvImage)
    {
        free(YuvImage);
        YuvImage = NULL;
    }

    if (NULL != frame_concat)
    {
        free(frame_concat);
        frame_concat = NULL;
    }
#endif
    return TS_NULL;
}

TS_S32 SAMPLE_ALG_CPM_HANDLE_Init(TS_VOID **pHandle)
{
    SAMPLE_VIDEO_ALG_CPM *pCpmParam = &gstSampleVideoAlgCpm[0];

    SAMPLE_PRT("cpm init \n");

    gstSampleVideoAlgCpm[0].u32ActualAlgNum = SAMPLE_ALG_Init_Instances(pCpmParam->pstAlgInstList, pCpmParam->enAlgMask, pCpmParam->u32AlgNum);

    if (0 == gstSampleVideoAlgCpm[0].u32ActualAlgNum)
    {
        ALG_LOGE("no one alg register\n");
        // return TS_FAILURE;
    }

    memset((TS_VOID *)&(pCpmParam->stAlgSrcBuffer), 0, sizeof(VIDEO_FRAME_INFO_S));
    memset((TS_VOID *)&(pCpmParam->stAlgBuffer), 0, sizeof(VIDEO_FRAME_INFO_S));

    memset((TS_VOID *)&(pCpmParam->stAlgResult), 0, sizeof(SAMPLE_ALG_RESULT_S));
    // 不需要临时结果，直接使用 stAlgResult

    pCpmParam->bAlgProcRunFlag = TS_TRUE;
    pCpmParam->enAlgProcBufStatus = BUFFER_STATUS_INIT;
    pCpmParam->bResultUpdate = TS_FALSE;
    // pCpmParam->stAlgProcLock = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init(&(pCpmParam->stAlgProcLock), NULL);

    pthread_mutex_lock(&g_AlgoFaceInLock);
    memset(&AlgoFaceIn, 0, sizeof(ALG_IMAGE_S));
    memset(&AlgoSrcFaceIn, 0, sizeof(ALG_IMAGE_S));
    TS_S32 s32Ret = TS_MPI_SYS_MmzAlloc_Cached(&(AlgoFaceIn.pDataPhy), &(AlgoFaceIn.pData), NULL, NULL, ALG_RGB_SIZE);
    if (0 != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_SYS_MmzAlloc_Cached error\n");
        pthread_mutex_unlock(&g_AlgoFaceInLock);
        return TS_FAILURE;
    }
    memset(AlgoFaceIn.pData, 114, ALG_RGB_SIZE);
    pthread_mutex_unlock(&g_AlgoFaceInLock);

    pthread_create(&(pCpmParam->stAlgProcPid), 0, SAMPLE_CPM_ALG_Process, (TS_VOID *)pCpmParam);

    return TS_SUCCESS;
}

TS_VOID SAMPLE_ALG_CPM_HANDLE_Exit(TS_VOID *pHandle)
{
    SAMPLE_VIDEO_ALG_CPM *pCpmParam = &gstSampleVideoAlgCpm[0];
    TS_S32 i;

    pCpmParam->bAlgProcRunFlag = TS_FALSE;

    if (pCpmParam->stAlgProcPid > 0)
    {
        pthread_join(pCpmParam->stAlgProcPid, 0);
        pCpmParam->stAlgProcPid = 0;
    }

    // 没有 stAlgProcPid1 成员，删除相关代码

    for (i = 0; i < pCpmParam->u32AlgNum; i++)
    {
        if (pCpmParam->pstAlgInstList[i] != NULL)
        {
            if (pCpmParam->pstAlgInstList[i]->pf_Exit != NULL)
            {
                pCpmParam->pstAlgInstList[i]->pf_Exit(pCpmParam->pstAlgInstList[i]);
            }
        }
    }

    pthread_mutex_lock(&g_AlgoFaceInLock);
    TS_S32 s32Ret = TS_MPI_SYS_MmzFree(AlgoFaceIn.pDataPhy, AlgoFaceIn.pData);
    if (s32Ret != 0)
    {
        SAMPLE_PRT("##### TS_MPI_SYS_MmzFree error\n");
    }
    pthread_mutex_unlock(&g_AlgoFaceInLock);

    pthread_mutex_destroy(&g_AlgoFaceInLock);

    return;
}

TS_S32 SAMPLE_ALG_CPM_HANDLE_Process(TS_VOID *pHandle, TS_VOID **in, TS_VOID **out)
{

#if USE_CPM

    VIDEO_FRAME_INFO_S *inPipeFrameVenc = in[0];
    SAMPLE_VIDEO_ALG_CPM *pCpmParam;

    pCpmParam = &gstSampleVideoAlgCpm[0];
    CHECK_NULL_PTR(in);
    CHECK_NULL_PTR(*in);
    CHECK_NULL_PTR(out);
    CHECK_NULL_PTR(*out);
    VIDEO_FRAME_INFO_S *inPipesrcFrameAlgo = in[0];
    VIDEO_FRAME_INFO_S *inPipeFrameAlgo = in[1];
    VIDEO_FRAME_INFO_S *outChnFrameVenc = out[0];
    static int count = 0;
    static int default_cnt = 0;

    switch (pCpmParam->enAlgProcBufStatus)
    {
    case BUFFER_STATUS_INIT:
    {
        pCpmParam->enAlgProcBufStatus = BUFFER_STATUS_FILL;
    }
    break;
    case BUFFER_STATUS_FILL:
    {
        memcpy(&(pCpmParam->stAlgSrcBuffer), inPipesrcFrameAlgo, sizeof(VIDEO_FRAME_INFO_S));
        // TS_MPI_VB_DupBlock(inPipesrcFrameAlgo->u32PoolId, inPipesrcFrameAlgo->stVFrame.u64PhyAddr[0]);

        memcpy(&(pCpmParam->stAlgBuffer), inPipeFrameAlgo, sizeof(VIDEO_FRAME_INFO_S));
        TS_MPI_VB_DupBlock_UID(inPipeFrameAlgo->u32PoolId, inPipeFrameAlgo->stVFrame.u64PhyAddr[0], VB_UID_USER);

        count++;
        pCpmParam->enAlgProcBufStatus = BUFFER_STATUS_ALG_PROCESS;
    }
    break;
    case BUFFER_STATUS_ALG_BYPASS:
    {
        memcpy(outChnFrameVenc, inPipeFrameAlgo, sizeof(VIDEO_FRAME_INFO_S));
        return TS_SUCCESS;
    }
    default:
        default_cnt++;
        break;
    }

#if 0
    /* Save venc chn src file*/
    if (TS_TRUE == g_algo_save_enable){
        SavePicData2File(ALGO_PIC_SAVE_PATH, inPipeFrame, picName);
    }
#endif

    if (pCpmParam->bAlgProcRunFlag)
    {
        pthread_mutex_lock(&(pCpmParam->stAlgProcLock));
        // if(pCpmParam->bResultUpdate)
        {
            VIDEO_FRAME_INFO_S *inPipeFrameVenc = in[0];
            unsigned char *YuvBuf = (unsigned char *)(TS_UL)(inPipeFrameVenc->stVFrame.u64VirAddr[0]);

            // 根据 pCpmParam 确定 CPM Group ID
            // TS_U32 CPMGrp = (pCpmParam == &gstSampleVideoAlgCpm[0]) ? 0 : 1;
            SAMPLE_ALG_Result_Proc(YuvBuf, &pCpmParam->stAlgResult, 0);

            TS_MPI_SYS_MflushCache(inPipeFrameVenc->stVFrame.u64PhyAddr[0],
                                   (TS_VOID *)(uintptr_t)(inPipeFrameVenc->stVFrame.u64VirAddr[0]),
                                   inPipeFrameVenc->stVFrame.u32Width * inPipeFrameVenc->stVFrame.u32Height * 3 / 2);
            // SAMPLE_ALG_ResultSave(YuvBuf, pCpmParam->pstAlgResultRead);
        }
        pCpmParam->bResultUpdate = TS_FALSE;
        pthread_mutex_unlock(&(pCpmParam->stAlgProcLock));
    }

    if (count % 5000 == 0)
    {
        printf("SAMPLE_ALG_CPM_HANDLE_Process count=%d, default_cnt=%d\n", count, default_cnt);
    }
#endif

    memcpy(out[0], in[0], sizeof(VIDEO_FRAME_INFO_S));
    VIDEO_FRAME_INFO_S *bufinfo = (VIDEO_FRAME_INFO_S *)in[0];
#if SDK_VERSON_030
#else
    if (bufinfo)
        TS_MPI_VB_DupBlock_UID(bufinfo->u32PoolId, bufinfo->stVFrame.u64PhyAddr[0], VB_UID_CPM);
#endif

    return TS_SUCCESS;
}
TS_S32 SAMPLE_ALG_CPM_HANDLE_SetParam(TS_VOID *pHandle, TS_VOID *pParam)
{
    printf("cpm set param \n");
    return TS_SUCCESS;
}

TS_S32 SAMPLE_ALG_CPM_HANDLE_GetParam(TS_VOID *pHandle, TS_VOID *pParam)
{
    printf("cpm get param \n");
    return TS_SUCCESS;
}

TS_S32 SAMPLE_ALG_CPM_HANDLE_GetResult(TS_VOID *pHandle, TS_VOID *pResult)
{
    printf("cpm get result \n");
    return TS_SUCCESS;
}

TS_S32 SAMPLE_ALG_CPM_HANDLE_ReleaseResult(TS_VOID *pHandle, TS_VOID *pResult)
{

    printf("cpm release result \n");
    return TS_SUCCESS;
}
