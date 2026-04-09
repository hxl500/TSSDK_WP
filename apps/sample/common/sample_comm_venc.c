
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>

#include "sample_comm_util.h"
#include "sample_comm_venc.h"
#include "sample_comm_sys.h"
#include "RtspServerApp.h"

typedef struct tsSAMPLE_VENC_GETSTREAM_PARA_S
{
    TS_BOOL bThreadStart;
    VENC_CHN VeChn[VENC_MAX_CHN_NUM];
    TS_S32  s32Cnt;
    TS_U32  u32GetStreamCnt[VENC_MAX_CHN_NUM];
	SAMPLE_VENC_CONSUME_TYPE consume_type;
} SAMPLE_VENC_GETSTREAM_PARA_S;

static pthread_t gs_VencPid;
static pthread_t gs_VencUpdatePid;

static SAMPLE_VENC_GETSTREAM_PARA_S gs_stPara;
static SAMPLE_VENC_UPDAETFPS_S  gs_stUpdateFpsPara;

static TS_S32 gs_s32SnapCnt = 0;
static char g_venc_save_path[128] = {0};
int wrFd = 0;
int g_fifoflow = 0, g_fifoflow_doing = 0, g_isp_fifoflow = 0, g_isp_fifoflow_doing = 0;
////////////////// 打印流信息 - 开始 //////////////

static TS_S8 isNeedShowStreamInfo[VENC_MAX_CHN_NUM] = {0};

static TS_U64 SAMPLE_GetSysTimeUs(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
    return (TS_U64)tv.tv_sec * 1e6 + tv.tv_usec;
}

static TS_U64 SAMPLE_GetBootTimeUs(void)
{
#if defined(CLOCK_MONOTONIC)
	{
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC_COARSE, &ts);
	    return (TS_U64)ts.tv_sec * 1e6 + ts.tv_nsec/1000;
	}
#endif
	return 0;
}

static TS_VOID SAMPLE_ShowStreamInfo(TS_S32 s32Chn, VENC_STREAM_S *pstVencStream){

    if(!pstVencStream || isNeedShowStreamInfo[s32Chn] <= 0)
        return;

    SAMPLE_PRT("stream: CHN[%d] PTS: %llu BOOTUP_TIME: %llu SYS_TIME: %llu\n",
            s32Chn,
            pstVencStream->pstPack[0].u64PTS,
            SAMPLE_GetBootTimeUs(),
            SAMPLE_GetSysTimeUs());

    isNeedShowStreamInfo[s32Chn]--;
}

// 输出5次码流的信息
TS_S32 TS_SAMPLE_SetShowStreamInfo10Time(){

    for (TS_S32 i = 0; i < VENC_MAX_CHN_NUM; i++)
    {
        isNeedShowStreamInfo[i] = 5;
    }

    return TS_SUCCESS;
}
////////////////// 打印流信息 - 结束 //////////////


TS_S32 TS_SAMPLE_VencPackageStream(VENC_STREAM_S *pstVencStream, TS_S32 type, TS_S32 session_idx, int wrFd)
{
#ifdef OPEN_RTSP_DEBUG_LOG
    printf("pstVencStream:%p, i: %d wrFd：%d\n", pstVencStream, i, wrFd);
#endif

    if (NULL == pstVencStream) {
        printf(" %s %d Param Is Null !.. \n", __func__, __LINE__);
        return TS_FAILURE;
    }

    unsigned char *buffer;
    unsigned int size;
    unsigned long long timestamp = 0;
    buffer = pstVencStream->pstPack[0].pu8Addr + pstVencStream->pstPack[0].u32Offset;
    size = pstVencStream->pstPack[0].u32Len - pstVencStream->pstPack[0].u32Offset;

	 //VIDEO_FRAME_I = 0X01，VIDEO_FRAME_P = 0X02.
	if(pstVencStream->pstPack[0].DataType.enH265EType == H265E_NALU_IDRSLICE
	|| pstVencStream->pstPack[0].DataType.enH265EType == H265E_NALU_ISLICE
	|| pstVencStream->pstPack[0].DataType.enH264EType == H264E_NALU_IDRSLICE
	|| pstVencStream->pstPack[0].DataType.enH264EType == H264E_NALU_ISLICE){
		type = 1;
	}else{
		type = 2;
	}

	SAMPLE_ShowStreamInfo(session_idx, pstVencStream);
    sendToRtspVencFrame(buffer, size, type, session_idx, timestamp);

    return TS_SUCCESS;
}
void isp_reset_flag()
{
    g_isp_fifoflow = 1;
    printf("~~~~~~~~~~~~~~ isp fifoflow ~~~~~~~~~~~~~ %s\n",__func__);
}
int get_fifoflow()
{
    return g_fifoflow;
}

int get_fifoflow_doing()
{
    return g_fifoflow_doing;
}

int get_isp_fifoflow_doing()
{
    return g_isp_fifoflow_doing;
}


/******************************************************************************
* funciton : get file postfix according palyload_type.
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_GetFilePostfix(PAYLOAD_TYPE_E enPayload, char* szFilePostfix)
{
    if (PT_H264 == enPayload)
    {
        strcpy(szFilePostfix, ".h264");
    }
    else if (PT_H265 == enPayload)
    {
        strcpy(szFilePostfix, ".h265");
    }
    else if (PT_JPEG == enPayload)
    {
        strcpy(szFilePostfix, ".jpg");
    }
    else if (PT_MJPEG == enPayload)
    {
        strcpy(szFilePostfix, ".mjp");
    }
    else
    {
        SAMPLE_PRT("payload type err!\n");
        return TS_FAILURE;
    }
    return TS_SUCCESS;
}


TS_S32 SAMPLE_COMM_VENC_GetGopAttr(VENC_GOP_MODE_E enGopMode,VENC_GOP_ATTR_S *pstGopAttr)
{
    switch(enGopMode)
    {
        case VENC_GOPMODE_NORMALP:
            pstGopAttr->enGopMode  = VENC_GOPMODE_NORMALP;
            pstGopAttr->stNormalP.s32IPQpDelta = 2;
        break;
        case VENC_GOPMODE_SMARTP:
            pstGopAttr->enGopMode  = VENC_GOPMODE_SMARTP;
            pstGopAttr->stSmartP.s32BgQpDelta  = 4;
            pstGopAttr->stSmartP.s32ViQpDelta  = 2;
            pstGopAttr->stSmartP.u32BgInterval =  90;
        break;

        case VENC_GOPMODE_DUALP:
            pstGopAttr->enGopMode  = VENC_GOPMODE_DUALP;
            pstGopAttr->stDualP.s32IPQpDelta  = 4;
            pstGopAttr->stDualP.s32SPQpDelta  = 2;
            pstGopAttr->stDualP.u32SPInterval = 3;
        break;

        case VENC_GOPMODE_BIPREDB:
            pstGopAttr->enGopMode  = VENC_GOPMODE_BIPREDB;
            pstGopAttr->stBipredB.s32BQpDelta  = -2;
            pstGopAttr->stBipredB.s32IPQpDelta = 3;
            pstGopAttr->stBipredB.u32BFrmNum   = 2;
        break;

        default:
            SAMPLE_PRT("not support the gop mode !\n");
            return TS_FAILURE;
        break;
    }
    return TS_SUCCESS;
}


/******************************************************************************
* funciton : save stream
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_SaveStream(FILE* pFd, VENC_STREAM_S* pstStream)
{
    TS_S32 i;
	int ret;
	size_t nmemb;

    for (i = 0; i < pstStream->u32PackCount; i++)
    {
#ifndef IMPR_PRINT
		printf("once write stream[%p:%d]\n", pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset,
			pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset);
#endif
        nmemb = fwrite(pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset,
               pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset, 1, pFd);
		if (nmemb != 1)
			SAMPLE_PRT("nmemb 1 should be written, but %zd actually, error %d\n", nmemb, errno);

        ret = fflush(pFd);
		if (ret)
			SAMPLE_PRT("fflush ret %d, error %d\n", ret, errno);
    }

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VENC_CloseReEncode(VENC_CHN VencChn)
{
    TS_S32 s32Ret;
    //VENC_RC_PARAM_S stRcParam;
    VENC_CHN_ATTR_S stChnAttr;

    s32Ret = TS_MPI_VENC_GetChnAttr(VencChn,&stChnAttr);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("GetChnAttr failed!\n");
        return TS_FAILURE;
    }
	#if 0
    s32Ret = TS_MPI_VENC_GetRcParam(VencChn,&stRcParam);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("GetRcParam failed!\n");
        return TS_FAILURE;
    }

    if(VENC_RC_MODE_H264CBR == stChnAttr.stRcAttr.enRcMode)
    {
        stRcParam.stParamH264Cbr.s32MaxReEncodeTimes = 0;
    }
    else if(VENC_RC_MODE_H264VBR == stChnAttr.stRcAttr.enRcMode)
    {
        stRcParam.stParamH264Vbr.s32MaxReEncodeTimes = 0;
    }
    else if(VENC_RC_MODE_H265CBR == stChnAttr.stRcAttr.enRcMode)
    {
        stRcParam.stParamH264Cbr.s32MaxReEncodeTimes = 0;
    }
    else if(VENC_RC_MODE_H265VBR == stChnAttr.stRcAttr.enRcMode)
    {
        stRcParam.stParamH264Vbr.s32MaxReEncodeTimes = 0;
    }
    else
    {
        return TS_SUCCESS;
    }
    s32Ret = TS_MPI_VENC_SetRcParam(VencChn,&stRcParam);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SetRcParam failed!\n");
        return TS_FAILURE;
    }
	#endif
    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VENC_Creat(VENC_CHN VencChn, PAYLOAD_TYPE_E enType,  PIC_SIZE_E enSize, TS_U32 u32FrameRate, SAMPLE_RC_E enRcMode, TS_U32  u32Profile, VENC_GOP_ATTR_S *pstGopAttr)
{
    TS_S32 s32Ret;
    SIZE_S stPicSize;
    VENC_CHN_ATTR_S        stVencChnAttr = {0};
    VENC_ATTR_JPEG_S       stJpegAttr = {0};
    TS_U32                 u32StatTime;
    TS_U32                 u32Gop = 0;//100;

    SAMPLE_COMM_SYS_GetPicSize(enSize, &stPicSize);
	u32Gop = u32FrameRate * 3;  // 5->3
	SAMPLE_PRT("********************u32FrameRate=%d, GOP = %d\n",u32FrameRate, u32Gop);
    /******************************************
     step 1:  Create Venc Channel
    ******************************************/
    stVencChnAttr.stVencAttr.enType          = enType;
    stVencChnAttr.stVencAttr.u32MaxPicWidth  = stPicSize.u32Width;
    stVencChnAttr.stVencAttr.u32MaxPicHeight = stPicSize.u32Height;
    stVencChnAttr.stVencAttr.u32PicWidth     = stPicSize.u32Width;/*the picture width*/
    stVencChnAttr.stVencAttr.u32PicHeight    = stPicSize.u32Height;/*the picture height*/
    if((stPicSize.u32Width == 2688 && stPicSize.u32Height == 1520) || (stPicSize.u32Width == 2560 && stPicSize.u32Height == 1440))
		stVencChnAttr.stVencAttr.u32BufSize  = ALIGN_UP(stPicSize.u32Width, 32) * ALIGN_UP(stPicSize.u32Height, 32) * 3 / 5;
    else
		stVencChnAttr.stVencAttr.u32BufSize  = ALIGN_UP(stPicSize.u32Width, 32) * ALIGN_UP(stPicSize.u32Height, 32) * 3 / 5;
    stVencChnAttr.stVencAttr.u32Profile      = u32Profile;
    stVencChnAttr.stVencAttr.bByFrame        = TS_TRUE;/*get stream mode is slice mode or frame mode?*/

    if(VENC_GOPMODE_SMARTP == pstGopAttr->enGopMode)
    {
        u32StatTime = pstGopAttr->stSmartP.u32BgInterval/u32Gop;
    }
    else
    {
        u32StatTime = 1;
    }

    switch (enType)
    {
        case PT_H265:
        {
            if (SAMPLE_RC_CBR == enRcMode)
            {
                VENC_H265_CBR_S    stH265Cbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
                stH265Cbr.u32Gop            = u32Gop;

                stH265Cbr.u32StatTime       = u32StatTime; /* stream rate statics time(s) */
                stH265Cbr.u32SrcFrameRate   = u32FrameRate; /* input (vi) frame rate */
                stH265Cbr.fr32DstFrameRate  = u32FrameRate; /* target frame rate */
                switch (enSize)
                {
                    case PIC_176x144:
                        stH265Cbr.u32BitRate = 100;//1024 * 1 + 1024*u32FrameRate/30;
                        break;
                    case PIC_360P:
                        stH265Cbr.u32BitRate = 160;//1024 * 1 + 1024*u32FrameRate/30;
                        break;
                    case PIC_480x270:
                        stH265Cbr.u32BitRate = 200;//1024 * 1 + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                    case PIC_1280x720:
                        stH265Cbr.u32BitRate = 384;//1024 * 2 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH265Cbr.u32BitRate = 800;//1024 * 2 + 2048*u32FrameRate/30;
                        break;
					case PIC_2304x1296:
						stH265Cbr.u32BitRate = 2048;
						break;
					case PIC_576P:
						stH265Cbr.u32BitRate = 512;
						break;
                    case PIC_2560x1440:
                        stH265Cbr.u32BitRate = 3072;//1024 * 2 + 3072*u32FrameRate/30;
                        break;
					case PIC_1440x2560:
                        stH265Cbr.u32BitRate = 3072;//1024 * 2 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH265Cbr.u32BitRate = 2560;//1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stH265Cbr.u32BitRate = 2560;//1024 * 3 + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH265Cbr.u32BitRate = 4096;//8192;//change from 2048
                        break;
					case PIC_2880x1624:
                        stH265Cbr.u32BitRate = 2560;//1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH265Cbr.u32BitRate = 4096;//1024 * 3  + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH265Cbr.u32BitRate = 1024 * 10 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH265Cbr.u32BitRate = 1024 * 20 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH265Cbr.u32BitRate = 1024 * 15 + 2048*u32FrameRate/30;
                        break;
                }
				stH265Cbr.u32MaxBitRate = stH265Cbr.u32BitRate * 3 / 2;
                memcpy(&stVencChnAttr.stRcAttr.stH265Cbr, &stH265Cbr, sizeof(VENC_H265_CBR_S));
            }
            else if (SAMPLE_RC_FIXQP == enRcMode)
            {
                VENC_H265_FIXQP_S    stH265FixQp;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265FIXQP;
                stH265FixQp.u32Gop              = 30;
                stH265FixQp.u32SrcFrameRate     = u32FrameRate;
                stH265FixQp.fr32DstFrameRate    = u32FrameRate;
                stH265FixQp.u32IQp              = 25;
                stH265FixQp.u32PQp              = 30;
                stH265FixQp.u32BQp              = 32;
                memcpy(&stVencChnAttr.stRcAttr.stH265FixQp, &stH265FixQp, sizeof(VENC_H265_FIXQP_S));
            }
            else if (SAMPLE_RC_VBR == enRcMode)
            {
                VENC_H265_VBR_S    stH265Vbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;
                stH265Vbr.u32Gop           = u32Gop;
                stH265Vbr.u32StatTime      = u32StatTime;
                stH265Vbr.u32SrcFrameRate  = u32FrameRate;
                stH265Vbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_176x144:
                        stH265Vbr.u32MaxBitRate = 100;//1024 * 1 + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stH265Vbr.u32MaxBitRate = 1024 * 2 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH265Vbr.u32MaxBitRate = 1024 * 2 + 2048*u32FrameRate/30;
                        break;
					case PIC_576P:
                        stH265Vbr.u32MaxBitRate = 512;
                        break;
					case PIC_2560x1440:
						stH265Vbr.u32MaxBitRate = 3072; //1024 * 3 + 3072*u32FrameRate/30;
						break;
                    case PIC_2688x1860:
                        stH265Vbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stH265Vbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH265Vbr.u32MaxBitRate = 4096;//8192;//change from 2048
                        break;
					case PIC_2880x1624:
                        stH265Vbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH265Vbr.u32MaxBitRate = 1024 * 5  + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH265Vbr.u32MaxBitRate = 1024 * 10 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH265Vbr.u32MaxBitRate = 1024 * 20 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH265Vbr.u32MaxBitRate    = 1024 * 15 + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH265Vbr, &stH265Vbr, sizeof(VENC_H265_VBR_S));
            }
            else if(SAMPLE_RC_AVBR == enRcMode)
            {
                VENC_H265_AVBR_S    stH265AVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265AVBR;
                stH265AVbr.u32Gop         = u32Gop;
                stH265AVbr.u32StatTime    = u32StatTime;
                stH265AVbr.u32SrcFrameRate  = u32FrameRate;
                stH265AVbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_720P:
                        stH265AVbr.u32MaxBitRate = 1024 * 2 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH265AVbr.u32MaxBitRate = 1024 * 2 + 2048*u32FrameRate/30;
                        break;
					 case PIC_576P:
                        stH265AVbr.u32MaxBitRate = 1024 * 2 + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH265AVbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stH265AVbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH265AVbr.u32MaxBitRate = 4096;//8192;//change from 2048
                        break;
					case PIC_2880x1624:
                        stH265AVbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH265AVbr.u32MaxBitRate = 1024 * 5  + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH265AVbr.u32MaxBitRate = 1024 * 10 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH265AVbr.u32MaxBitRate = 1024 * 20 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH265AVbr.u32MaxBitRate    = 1024 * 15 + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH265AVbr, &stH265AVbr, sizeof(VENC_H265_AVBR_S));
            }
            else if(SAMPLE_RC_QVBR == enRcMode)
            {
                VENC_H265_QVBR_S    stH265QVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265QVBR;
                stH265QVbr.u32Gop         = u32Gop;
                stH265QVbr.u32StatTime    = u32StatTime;
                stH265QVbr.u32SrcFrameRate  = u32FrameRate;
                stH265QVbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_720P:
                        stH265QVbr.u32TargetBitRate= 1024 * 2 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH265QVbr.u32TargetBitRate = 1024 * 2 + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH265QVbr.u32TargetBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH265QVbr.u32TargetBitRate = 4096;//8192;//change from 2048
                        break;
                    case PIC_2592x1944:
                        stH265QVbr.u32TargetBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
					case PIC_2880x1624:
                        stH265QVbr.u32TargetBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH265QVbr.u32TargetBitRate = 1024 * 5  + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH265QVbr.u32TargetBitRate = 1024 * 10 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH265QVbr.u32TargetBitRate = 1024 * 20 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH265QVbr.u32TargetBitRate    = 1024 * 15 + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH265QVbr, &stH265QVbr, sizeof(VENC_H265_QVBR_S));
            }
            else if(SAMPLE_RC_CVBR == enRcMode)
            {
                VENC_H265_CVBR_S    stH265CVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CVBR;
                stH265CVbr.u32Gop         = u32Gop;
                stH265CVbr.u32StatTime    = u32StatTime;
                stH265CVbr.u32SrcFrameRate  = u32FrameRate;
                stH265CVbr.fr32DstFrameRate = u32FrameRate;
                stH265CVbr.u32LongTermStatTime  = 1;
                stH265CVbr.u32ShortTermStatTime = u32StatTime;
                switch (enSize)
                {
                    case PIC_720P:
                        stH265CVbr.u32MaxBitRate         = 1024 * 3 + 1024*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 2 + 1024*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 512;
                        break;
                    case PIC_1080P:
                        stH265CVbr.u32MaxBitRate         = 1024 * 2 + 2048*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 2 + 2048*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024;
                        break;
                    case PIC_2688x1860:
                        stH265CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
                    case PIC_2592x1944:
                        stH265CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
					case PIC_2688x1520:
                        stH265CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
					case PIC_2880x1624:
                        stH265CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
                    case PIC_3840x2160:
                        stH265CVbr.u32MaxBitRate         = 1024 * 8  + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 5  + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*3;
                        break;
                    case PIC_4000x3000:
                        stH265CVbr.u32MaxBitRate         = 1024 * 12  + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 10 + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*4;
                        break;
                    case PIC_7680x4320:
                        stH265CVbr.u32MaxBitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 20 + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*6;
                        break;
                    default :
                        stH265CVbr.u32MaxBitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 15 + 2048*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*5;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH265CVbr, &stH265CVbr, sizeof(VENC_H265_CVBR_S));
            }
            else if(SAMPLE_RC_QPMAP == enRcMode)
            {
                VENC_H265_QPMAP_S    stH265QpMap;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265QPMAP;
                stH265QpMap.u32Gop           = u32Gop;
                stH265QpMap.u32StatTime      = u32StatTime;
                stH265QpMap.u32SrcFrameRate  = u32FrameRate;
                stH265QpMap.fr32DstFrameRate = u32FrameRate;
                stH265QpMap.enQpMapMode      = VENC_RC_QPMAP_MODE_MEANQP;
                memcpy(&stVencChnAttr.stRcAttr.stH265QpMap, &stH265QpMap, sizeof(VENC_H265_QPMAP_S));
            }
            else
            {
                SAMPLE_PRT("%s,%d,enRcMode(%d) not support\n",__FUNCTION__,__LINE__,enRcMode);
                return TS_FAILURE;
            }
        }
		stVencChnAttr.stVencAttr.stAttrH265e.bRcnRefShareBuf = TS_FALSE;
        break;
        case PT_H264:
        {
            if (SAMPLE_RC_CBR == enRcMode)
            {
                VENC_H264_CBR_S    stH264Cbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
                stH264Cbr.u32Gop                = u32Gop; /*the interval of IFrame*/
                stH264Cbr.u32StatTime           = u32StatTime; /* stream rate statics time(s) */
                stH264Cbr.u32SrcFrameRate       = u32FrameRate; /* input (vi) frame rate */
                stH264Cbr.fr32DstFrameRate      = u32FrameRate; /* target frame rate */
                switch (enSize)
                {
                    case PIC_176x144:
                        stH264Cbr.u32BitRate = 100;
                        break;
                    case PIC_360P:
                        stH264Cbr.u32BitRate = 160;
                        break;
                    case PIC_480x270:
                        stH264Cbr.u32BitRate = 200;
                        break;
                    case PIC_720P:
                        stH264Cbr.u32BitRate         = 1024 * 3 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH264Cbr.u32BitRate         = 1024 * 2 + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH264Cbr.u32BitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH264Cbr.u32BitRate         = 4096;//8192;//change from 2048
                        break;
                    case PIC_2592x1944:
                        stH264Cbr.u32BitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        break;
					case PIC_2880x1624:
                        stH264Cbr.u32BitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH264Cbr.u32BitRate         = 1024 * 8  + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH264Cbr.u32BitRate         = 1024 * 12  + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH264Cbr.u32BitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH264Cbr.u32BitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        break;
                }
				stH264Cbr.u32MaxBitRate = stH264Cbr.u32BitRate * 3 /2;
                memcpy(&stVencChnAttr.stRcAttr.stH264Cbr, &stH264Cbr, sizeof(VENC_H264_CBR_S));
            }
            else if (SAMPLE_RC_FIXQP == enRcMode)
            {
                VENC_H264_FIXQP_S    stH264FixQp;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264FIXQP;
                stH264FixQp.u32Gop           = 30;
                stH264FixQp.u32SrcFrameRate  = u32FrameRate;
                stH264FixQp.fr32DstFrameRate = u32FrameRate;
                stH264FixQp.u32IQp           = 25;
                stH264FixQp.u32PQp           = 30;
                stH264FixQp.u32BQp           = 32;
                memcpy(&stVencChnAttr.stRcAttr.stH264FixQp, &stH264FixQp, sizeof(VENC_H264_FIXQP_S));
            }
            else if (SAMPLE_RC_VBR == enRcMode)
            {
                VENC_H264_VBR_S    stH264Vbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
                stH264Vbr.u32Gop           = u32Gop;
                stH264Vbr.u32StatTime      = u32StatTime;
                stH264Vbr.u32SrcFrameRate  = u32FrameRate;
                stH264Vbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                	case PIC_176x144:
                        stH264Vbr.u32MaxBitRate = 100;
                        break;
                    case PIC_360P:
                        stH264Vbr.u32MaxBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stH264Vbr.u32MaxBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH264Vbr.u32MaxBitRate = 1024 * 2   + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH264Vbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH264Vbr.u32MaxBitRate = 4096;//8192;//change from 2048
                        break;
                    case PIC_2592x1944:
                        stH264Vbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
					case PIC_2880x1624:
                        stH264Vbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH264Vbr.u32MaxBitRate = 1024 * 5   + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH264Vbr.u32MaxBitRate = 1024 * 10  + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH264Vbr.u32MaxBitRate = 1024 * 20  + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH264Vbr.u32MaxBitRate = 1024 * 15  + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH264Vbr, &stH264Vbr, sizeof(VENC_H264_VBR_S));
            }
            else if (SAMPLE_RC_AVBR == enRcMode)
            {
                VENC_H264_VBR_S    stH264AVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264AVBR;
                stH264AVbr.u32Gop           = u32Gop;
                stH264AVbr.u32StatTime      = u32StatTime;
                stH264AVbr.u32SrcFrameRate  = u32FrameRate;
                stH264AVbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_360P:
                        stH264AVbr.u32MaxBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stH264AVbr.u32MaxBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH264AVbr.u32MaxBitRate = 1024 * 2   + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH264AVbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH264AVbr.u32MaxBitRate = 4096;//8192;//change from 2048
                        break;
                    case PIC_2592x1944:
                        stH264AVbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
					case PIC_2880x1624:
                        stH264AVbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH264AVbr.u32MaxBitRate = 1024 * 5   + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH264AVbr.u32MaxBitRate = 1024 * 10  + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH264AVbr.u32MaxBitRate = 1024 * 20  + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH264AVbr.u32MaxBitRate = 1024 * 15  + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH264AVbr, &stH264AVbr, sizeof(VENC_H264_AVBR_S));
            }
            else if (SAMPLE_RC_QVBR == enRcMode)
            {
                VENC_H264_QVBR_S    stH264QVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264QVBR;
                stH264QVbr.u32Gop           = u32Gop;
                stH264QVbr.u32StatTime      = u32StatTime;
                stH264QVbr.u32SrcFrameRate  = u32FrameRate;
                stH264QVbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_360P:
                        stH264QVbr.u32TargetBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stH264QVbr.u32TargetBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH264QVbr.u32TargetBitRate = 1024 * 2   + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH264QVbr.u32TargetBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH264QVbr.u32TargetBitRate = 4096;//8192;//change from 2048
                        break;
                    case PIC_2592x1944:
                        stH264QVbr.u32TargetBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
					 case PIC_2880x1624:
                        stH264QVbr.u32TargetBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH264QVbr.u32TargetBitRate = 1024 * 5   + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH264QVbr.u32TargetBitRate = 1024 * 10  + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH264QVbr.u32TargetBitRate = 1024 * 20  + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH264QVbr.u32TargetBitRate = 1024 * 15  + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH264QVbr, &stH264QVbr, sizeof(VENC_H264_QVBR_S));
            }
            else if(SAMPLE_RC_CVBR == enRcMode)
            {
                VENC_H264_CVBR_S    stH264CVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CVBR;
                stH264CVbr.u32Gop         = u32Gop;
                stH264CVbr.u32StatTime    = u32StatTime;
                stH264CVbr.u32SrcFrameRate  = u32FrameRate;
                stH264CVbr.fr32DstFrameRate = u32FrameRate;
                stH264CVbr.u32LongTermStatTime  = 1;
                stH264CVbr.u32ShortTermStatTime = u32StatTime;
                switch (enSize)
                {
                    case PIC_720P:
                        stH264CVbr.u32MaxBitRate         = 1024 * 3 + 1024*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 2 + 1024*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 512;
                        break;
                    case PIC_1080P:
                        stH264CVbr.u32MaxBitRate         = 1024 * 2 + 2048*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 2 + 2048*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024;
                        break;
                    case PIC_2688x1860:
                        stH264CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
                    case PIC_2592x1944:
                        stH264CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
					case PIC_2688x1520:
                        stH264CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
					case PIC_2880x1624:
                        stH264CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
                    case PIC_3840x2160:
                        stH264CVbr.u32MaxBitRate         = 1024 * 8  + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 5  + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*3;
                        break;
                    case PIC_4000x3000:
                        stH264CVbr.u32MaxBitRate         = 1024 * 12  + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 10 + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*4;
                        break;
                    case PIC_7680x4320:
                        stH264CVbr.u32MaxBitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 20 + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*6;
                        break;
                    default :
                        stH264CVbr.u32MaxBitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 15 + 2048*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*5;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH264CVbr, &stH264CVbr, sizeof(VENC_H264_CVBR_S));
            }
            else if(SAMPLE_RC_QPMAP == enRcMode)
            {
                VENC_H264_QPMAP_S    stH264QpMap;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264QPMAP;
                stH264QpMap.u32Gop           = u32Gop;
                stH264QpMap.u32StatTime      = u32StatTime;
                stH264QpMap.u32SrcFrameRate  = u32FrameRate;
                stH264QpMap.fr32DstFrameRate = u32FrameRate;
                memcpy(&stVencChnAttr.stRcAttr.stH264QpMap, &stH264QpMap, sizeof(VENC_H264_QPMAP_S));
            }
            else
            {
                SAMPLE_PRT("%s,%d,enRcMode(%d) not support\n",__FUNCTION__,__LINE__,enRcMode);
                return TS_FAILURE;
            }
        }
		stVencChnAttr.stVencAttr.stAttrH264e.bRcnRefShareBuf = TS_FALSE;
        break;
        case PT_MJPEG:
        {
            if (SAMPLE_RC_FIXQP == enRcMode)
            {
                VENC_MJPEG_FIXQP_S stMjpegeFixQp;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGFIXQP;
                stMjpegeFixQp.u32Qfactor        = 95;
                stMjpegeFixQp.u32SrcFrameRate    = u32FrameRate;
                stMjpegeFixQp.fr32DstFrameRate   = u32FrameRate;

                memcpy(&stVencChnAttr.stRcAttr.stMjpegFixQp, &stMjpegeFixQp,sizeof(VENC_MJPEG_FIXQP_S));
            }
            else if (SAMPLE_RC_CBR == enRcMode)
            {
                VENC_MJPEG_CBR_S stMjpegeCbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGCBR;
                stMjpegeCbr.u32StatTime         = u32StatTime;
                stMjpegeCbr.u32SrcFrameRate     = u32FrameRate;
                stMjpegeCbr.fr32DstFrameRate    = u32FrameRate;
                switch (enSize)
                {
                    case PIC_360P:
                        stMjpegeCbr.u32BitRate = 1024 * 3  + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stMjpegeCbr.u32BitRate = 1024 * 5  + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stMjpegeCbr.u32BitRate = 1024 * 8  + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stMjpegeCbr.u32BitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stMjpegeCbr.u32BitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stMjpegeCbr.u32BitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
					case PIC_2880x1624:
                        stMjpegeCbr.u32BitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stMjpegeCbr.u32BitRate = 1024 * 25 + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stMjpegeCbr.u32BitRate = 1024 * 30 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stMjpegeCbr.u32BitRate = 1024 * 40 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stMjpegeCbr.u32BitRate = 1024 * 20 + 2048*u32FrameRate/30;
                        break;
                }

                memcpy(&stVencChnAttr.stRcAttr.stMjpegCbr, &stMjpegeCbr,sizeof(VENC_MJPEG_CBR_S));
            }
            else if ((SAMPLE_RC_VBR == enRcMode) ||(SAMPLE_RC_AVBR == enRcMode)||
                     (SAMPLE_RC_QVBR == enRcMode)||(SAMPLE_RC_CVBR == enRcMode))
            {
                VENC_MJPEG_VBR_S   stMjpegVbr;

                if(SAMPLE_RC_AVBR == enRcMode)
                {
                    SAMPLE_PRT("Mjpege not support AVBR, so change rcmode to VBR!\n");
                }

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGVBR;
                stMjpegVbr.u32StatTime      = u32StatTime;
                stMjpegVbr.u32SrcFrameRate  = u32FrameRate;
                stMjpegVbr.fr32DstFrameRate = 5;

                switch (enSize)
                {
                    case PIC_360P:
                        stMjpegVbr.u32MaxBitRate = 1024 * 3 + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stMjpegVbr.u32MaxBitRate = 1024 * 5 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stMjpegVbr.u32MaxBitRate = 1024 * 8 + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stMjpegVbr.u32MaxBitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stMjpegVbr.u32MaxBitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stMjpegVbr.u32MaxBitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
					case PIC_2880x1624:
                        stMjpegVbr.u32MaxBitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stMjpegVbr.u32MaxBitRate = 1024 * 25 + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stMjpegVbr.u32MaxBitRate    = 1024 * 30 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stMjpegVbr.u32MaxBitRate = 1024 * 40 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stMjpegVbr.u32MaxBitRate = 1024 * 20 + 2048*u32FrameRate/30;
                        break;
                }

                memcpy(&stVencChnAttr.stRcAttr.stMjpegVbr, &stMjpegVbr,sizeof(VENC_MJPEG_VBR_S));
            }
            else
            {
                SAMPLE_PRT("cann't support other mode(%d) in this version!\n",enRcMode);
                return TS_FAILURE;
            }
        }
        break;

        case PT_JPEG:
            stJpegAttr.bSupportDCF     = TS_FALSE;
            stJpegAttr.stMPFCfg.u8LargeThumbNailNum = 0;
            stJpegAttr.enReceiveMode                = VENC_PIC_RECEIVE_SINGLE;
            memcpy(&stVencChnAttr.stVencAttr.stAttrJpege, &stJpegAttr, sizeof(VENC_ATTR_JPEG_S));
            break;
        default:
            SAMPLE_PRT("cann't support this enType (%d) in this version!\n",enType);
            return TS_ERR_VENC_NOT_SUPPORT;
    }

    if(PT_MJPEG == enType || PT_JPEG == enType )
    {
        stVencChnAttr.stGopAttr.enGopMode  = VENC_GOPMODE_NORMALP;
        stVencChnAttr.stGopAttr.stNormalP.s32IPQpDelta = 0;
    }
    else
    {
        memcpy(&stVencChnAttr.stGopAttr,pstGopAttr,sizeof(VENC_GOP_ATTR_S));
        if((VENC_GOPMODE_BIPREDB == pstGopAttr->enGopMode)&&(PT_H264 == enType))
        {
            if(0 == stVencChnAttr.stVencAttr.u32Profile)
            {
                stVencChnAttr.stVencAttr.u32Profile = 1;

                SAMPLE_PRT("H.264 base profile not support BIPREDB, so change profile to main profile!\n");
            }
        }

        if((VENC_RC_MODE_H264QPMAP == stVencChnAttr.stRcAttr.enRcMode)||(VENC_RC_MODE_H265QPMAP == stVencChnAttr.stRcAttr.enRcMode))
        {
            if(VENC_GOPMODE_ADVSMARTP == pstGopAttr->enGopMode)
            {
                stVencChnAttr.stGopAttr.enGopMode = VENC_GOPMODE_SMARTP;

                SAMPLE_PRT("advsmartp not support QPMAP, so change gopmode to smartp!\n");
            }
        }
    }

	//SAMPLE_COMM_VENC_GetChnAttrByTpye(enType, enSize, &stVencChnAttr);

	SAMPLE_PRT("create venChn[%d], %s, MaxWH=[%d,%d], w=h[%d,%d], bufSize=%d\n", VencChn,
		PT_H264 == stVencChnAttr.stVencAttr.enType ? "H264" : "Other",
		stVencChnAttr.stVencAttr.u32MaxPicWidth, stVencChnAttr.stVencAttr.u32MaxPicHeight,
		stVencChnAttr.stVencAttr.u32PicWidth, stVencChnAttr.stVencAttr.u32PicHeight,
		stVencChnAttr.stVencAttr.u32BufSize);

    s32Ret = TS_MPI_VENC_CreateChn(VencChn, &stVencChnAttr);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_CreateChn [%d] faild with %#x! ===\n", \
                   VencChn, s32Ret);
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_VENC_CloseReEncode(VencChn);
    if (TS_SUCCESS != s32Ret)
    {
        TS_MPI_VENC_DestroyChn(VencChn);
        return s32Ret;
    }

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VENC_CreatAllocBufOnce(VENC_CHN VencChn, PAYLOAD_TYPE_E enType,  PIC_SIZE_E enSize, TS_U32 u32FrameRate, SAMPLE_RC_E enRcMode, TS_U32  u32Profile, VENC_GOP_ATTR_S *pstGopAttr)
{
    TS_S32 s32Ret;
    SIZE_S stPicSize;
    VENC_CHN_ATTR_S        stVencChnAttr = {0};
    VENC_ATTR_JPEG_S       stJpegAttr = {0};
    TS_U32                 u32StatTime;
    TS_U32                 u32Gop = 0;//100;

    SAMPLE_COMM_SYS_GetPicSize(enSize, &stPicSize);

	u32Gop = u32FrameRate * 3;  // 5->3
	SAMPLE_PRT("********************u32FrameRate=%d, GOP = %d\n",u32FrameRate, u32Gop);
    /******************************************
     step 1:  Create Venc Channel
    ******************************************/
    stVencChnAttr.stVencAttr.enType          = enType;
    stVencChnAttr.stVencAttr.u32MaxPicWidth  = stPicSize.u32Width;
    stVencChnAttr.stVencAttr.u32MaxPicHeight = stPicSize.u32Height;
    stVencChnAttr.stVencAttr.u32PicWidth     = stPicSize.u32Width;/*the picture width*/
    stVencChnAttr.stVencAttr.u32PicHeight    = stPicSize.u32Height;/*the picture height*/
    stVencChnAttr.stVencAttr.stAttrPrivate.bAllocBufOnce   = TS_TRUE;

    if((stPicSize.u32Width == 2688 && stPicSize.u32Height == 1520) || (stPicSize.u32Width == 2560 && stPicSize.u32Height == 1440))
		stVencChnAttr.stVencAttr.u32BufSize  = ALIGN_UP(stPicSize.u32Width, 32) * ALIGN_UP(stPicSize.u32Height, 32) * 3 / 5;
    else
		stVencChnAttr.stVencAttr.u32BufSize  = ALIGN_UP(stPicSize.u32Width, 32) * ALIGN_UP(stPicSize.u32Height, 32) * 3 / 5;
    stVencChnAttr.stVencAttr.u32Profile      = u32Profile;
    stVencChnAttr.stVencAttr.bByFrame        = TS_TRUE;/*get stream mode is slice mode or frame mode?*/

    if(VENC_GOPMODE_SMARTP == pstGopAttr->enGopMode)
    {
        u32StatTime = pstGopAttr->stSmartP.u32BgInterval/u32Gop;
    }
    else
    {
        u32StatTime = 1;
    }

    switch (enType)
    {
        case PT_H265:
        {
            if (SAMPLE_RC_CBR == enRcMode)
            {
                VENC_H265_CBR_S    stH265Cbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
                stH265Cbr.u32Gop            = u32Gop;

                stH265Cbr.u32StatTime       = u32StatTime; /* stream rate statics time(s) */
                stH265Cbr.u32SrcFrameRate   = u32FrameRate; /* input (vi) frame rate */
                stH265Cbr.fr32DstFrameRate  = u32FrameRate; /* target frame rate */
                switch (enSize)
                {
                	case PIC_176x144:
                        stH265Cbr.u32BitRate = 100;//1024 * 1 + 1024*u32FrameRate/30;
                        break;
                    case PIC_360P:
                        stH265Cbr.u32BitRate = 160;//1024 * 1 + 1024*u32FrameRate/30;
                        break;
                    case PIC_480x270:
                        stH265Cbr.u32BitRate = 200;//1024 * 1 + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                    case PIC_1280x720:
                        stH265Cbr.u32BitRate = 384;//1024 * 2 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH265Cbr.u32BitRate = 800;//1024 * 2 + 2048*u32FrameRate/30;
                        break;
					case PIC_2304x1296:
						stH265Cbr.u32BitRate = 2048;
						break;
					case PIC_576P:
						stH265Cbr.u32BitRate = 512;
						break;
                    case PIC_2560x1440:
                        stH265Cbr.u32BitRate = 3072;//1024 * 2 + 3072*u32FrameRate/30;
                        break;
					case PIC_1440x2560:
                        stH265Cbr.u32BitRate = 3072;//1024 * 2 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH265Cbr.u32BitRate = 2560;//1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stH265Cbr.u32BitRate = 2560;//1024 * 3 + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH265Cbr.u32BitRate = 4096;//8192;//change from 2048
                        break;
					case PIC_2880x1624:
                        stH265Cbr.u32BitRate = 2560;//1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH265Cbr.u32BitRate = 4096;//1024 * 3  + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH265Cbr.u32BitRate = 1024 * 10 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH265Cbr.u32BitRate = 1024 * 20 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH265Cbr.u32BitRate = 1024 * 15 + 2048*u32FrameRate/30;
                        break;
                }
				stH265Cbr.u32MaxBitRate = stH265Cbr.u32BitRate * 3 / 2;
                memcpy(&stVencChnAttr.stRcAttr.stH265Cbr, &stH265Cbr, sizeof(VENC_H265_CBR_S));
            }
            else if (SAMPLE_RC_FIXQP == enRcMode)
            {
                VENC_H265_FIXQP_S    stH265FixQp;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265FIXQP;
                stH265FixQp.u32Gop              = 30;
                stH265FixQp.u32SrcFrameRate     = u32FrameRate;
                stH265FixQp.fr32DstFrameRate    = u32FrameRate;
                stH265FixQp.u32IQp              = 25;
                stH265FixQp.u32PQp              = 30;
                stH265FixQp.u32BQp              = 32;
                memcpy(&stVencChnAttr.stRcAttr.stH265FixQp, &stH265FixQp, sizeof(VENC_H265_FIXQP_S));
            }
            else if (SAMPLE_RC_VBR == enRcMode)
            {
                VENC_H265_VBR_S    stH265Vbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;
                stH265Vbr.u32Gop           = u32Gop;
                stH265Vbr.u32StatTime      = u32StatTime;
                stH265Vbr.u32SrcFrameRate  = u32FrameRate;
                stH265Vbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                	case PIC_176x144:
                        stH265Vbr.u32MaxBitRate = 100;//1024 * 1 + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stH265Vbr.u32MaxBitRate = 1024 * 2 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH265Vbr.u32MaxBitRate = 1024 * 2 + 2048*u32FrameRate/30;
                        break;
					case PIC_576P:
                        stH265Vbr.u32MaxBitRate = 512;
                        break;
					case PIC_2560x1440:
						stH265Vbr.u32MaxBitRate = 3072; //1024 * 3 + 3072*u32FrameRate/30;
						break;
                    case PIC_2688x1860:
                        stH265Vbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stH265Vbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH265Vbr.u32MaxBitRate = 4096;//8192;//change from 2048
                        break;
					case PIC_2880x1624:
                        stH265Vbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH265Vbr.u32MaxBitRate = 1024 * 5  + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH265Vbr.u32MaxBitRate = 1024 * 10 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH265Vbr.u32MaxBitRate = 1024 * 20 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH265Vbr.u32MaxBitRate    = 1024 * 15 + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH265Vbr, &stH265Vbr, sizeof(VENC_H265_VBR_S));
            }
            else if(SAMPLE_RC_AVBR == enRcMode)
            {
                VENC_H265_AVBR_S    stH265AVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265AVBR;
                stH265AVbr.u32Gop         = u32Gop;
                stH265AVbr.u32StatTime    = u32StatTime;
                stH265AVbr.u32SrcFrameRate  = u32FrameRate;
                stH265AVbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_720P:
                        stH265AVbr.u32MaxBitRate = 1024 * 2 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH265AVbr.u32MaxBitRate = 1024 * 2 + 2048*u32FrameRate/30;
                        break;
					 case PIC_576P:
                        stH265AVbr.u32MaxBitRate = 1024 * 2 + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH265AVbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stH265AVbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH265AVbr.u32MaxBitRate = 4096;//8192;//change from 2048
                        break;
					case PIC_2880x1624:
                        stH265AVbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH265AVbr.u32MaxBitRate = 1024 * 5  + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH265AVbr.u32MaxBitRate = 1024 * 10 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH265AVbr.u32MaxBitRate = 1024 * 20 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH265AVbr.u32MaxBitRate    = 1024 * 15 + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH265AVbr, &stH265AVbr, sizeof(VENC_H265_AVBR_S));
            }
            else if(SAMPLE_RC_QVBR == enRcMode)
            {
                VENC_H265_QVBR_S    stH265QVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265QVBR;
                stH265QVbr.u32Gop         = u32Gop;
                stH265QVbr.u32StatTime    = u32StatTime;
                stH265QVbr.u32SrcFrameRate  = u32FrameRate;
                stH265QVbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_720P:
                        stH265QVbr.u32TargetBitRate= 1024 * 2 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH265QVbr.u32TargetBitRate = 1024 * 2 + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH265QVbr.u32TargetBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH265QVbr.u32TargetBitRate = 4096;//8192;//change from 2048
                        break;
                    case PIC_2592x1944:
                        stH265QVbr.u32TargetBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
					case PIC_2880x1624:
                        stH265QVbr.u32TargetBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH265QVbr.u32TargetBitRate = 1024 * 5  + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH265QVbr.u32TargetBitRate = 1024 * 10 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH265QVbr.u32TargetBitRate = 1024 * 20 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH265QVbr.u32TargetBitRate    = 1024 * 15 + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH265QVbr, &stH265QVbr, sizeof(VENC_H265_QVBR_S));
            }
            else if(SAMPLE_RC_CVBR == enRcMode)
            {
                VENC_H265_CVBR_S    stH265CVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CVBR;
                stH265CVbr.u32Gop         = u32Gop;
                stH265CVbr.u32StatTime    = u32StatTime;
                stH265CVbr.u32SrcFrameRate  = u32FrameRate;
                stH265CVbr.fr32DstFrameRate = u32FrameRate;
                stH265CVbr.u32LongTermStatTime  = 1;
                stH265CVbr.u32ShortTermStatTime = u32StatTime;
                switch (enSize)
                {
                    case PIC_720P:
                        stH265CVbr.u32MaxBitRate         = 1024 * 3 + 1024*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 2 + 1024*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 512;
                        break;
                    case PIC_1080P:
                        stH265CVbr.u32MaxBitRate         = 1024 * 2 + 2048*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 2 + 2048*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024;
                        break;
                    case PIC_2688x1860:
                        stH265CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
                    case PIC_2592x1944:
                        stH265CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
					case PIC_2688x1520:
                        stH265CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
					case PIC_2880x1624:
                        stH265CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
                    case PIC_3840x2160:
                        stH265CVbr.u32MaxBitRate         = 1024 * 8  + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 5  + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*3;
                        break;
                    case PIC_4000x3000:
                        stH265CVbr.u32MaxBitRate         = 1024 * 12  + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 10 + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*4;
                        break;
                    case PIC_7680x4320:
                        stH265CVbr.u32MaxBitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 20 + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*6;
                        break;
                    default :
                        stH265CVbr.u32MaxBitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 15 + 2048*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*5;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH265CVbr, &stH265CVbr, sizeof(VENC_H265_CVBR_S));
            }
            else if(SAMPLE_RC_QPMAP == enRcMode)
            {
                VENC_H265_QPMAP_S    stH265QpMap;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265QPMAP;
                stH265QpMap.u32Gop           = u32Gop;
                stH265QpMap.u32StatTime      = u32StatTime;
                stH265QpMap.u32SrcFrameRate  = u32FrameRate;
                stH265QpMap.fr32DstFrameRate = u32FrameRate;
                stH265QpMap.enQpMapMode      = VENC_RC_QPMAP_MODE_MEANQP;
                memcpy(&stVencChnAttr.stRcAttr.stH265QpMap, &stH265QpMap, sizeof(VENC_H265_QPMAP_S));
            }
            else
            {
                SAMPLE_PRT("%s,%d,enRcMode(%d) not support\n",__FUNCTION__,__LINE__,enRcMode);
                return TS_FAILURE;
            }
        }
		stVencChnAttr.stVencAttr.stAttrH265e.bRcnRefShareBuf = TS_FALSE;
        break;
        case PT_H264:
        {
            if (SAMPLE_RC_CBR == enRcMode)
            {
                VENC_H264_CBR_S    stH264Cbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
                stH264Cbr.u32Gop                = u32Gop; /*the interval of IFrame*/
                stH264Cbr.u32StatTime           = u32StatTime; /* stream rate statics time(s) */
                stH264Cbr.u32SrcFrameRate       = u32FrameRate; /* input (vi) frame rate */
                stH264Cbr.fr32DstFrameRate      = u32FrameRate; /* target frame rate */
                switch (enSize)
                {
                	 case PIC_176x144:
                        stH264Cbr.u32BitRate = 100;
                        break;
                    case PIC_360P:
                        stH264Cbr.u32BitRate = 160;
                        break;
                    case PIC_480x270:
                        stH264Cbr.u32BitRate = 200;
                        break;
                    case PIC_720P:
                        stH264Cbr.u32BitRate         = 1024 * 3 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH264Cbr.u32BitRate         = 1024 * 2 + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH264Cbr.u32BitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH264Cbr.u32BitRate         = 4096;//8192;//change from 2048
                        break;
                    case PIC_2592x1944:
                        stH264Cbr.u32BitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        break;
					case PIC_2880x1624:
                        stH264Cbr.u32BitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH264Cbr.u32BitRate         = 1024 * 8  + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH264Cbr.u32BitRate         = 1024 * 12  + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH264Cbr.u32BitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH264Cbr.u32BitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        break;
                }
				stH264Cbr.u32MaxBitRate = stH264Cbr.u32BitRate * 3 /2;
                memcpy(&stVencChnAttr.stRcAttr.stH264Cbr, &stH264Cbr, sizeof(VENC_H264_CBR_S));
            }
            else if (SAMPLE_RC_FIXQP == enRcMode)
            {
                VENC_H264_FIXQP_S    stH264FixQp;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264FIXQP;
                stH264FixQp.u32Gop           = 30;
                stH264FixQp.u32SrcFrameRate  = u32FrameRate;
                stH264FixQp.fr32DstFrameRate = u32FrameRate;
                stH264FixQp.u32IQp           = 25;
                stH264FixQp.u32PQp           = 30;
                stH264FixQp.u32BQp           = 32;
                memcpy(&stVencChnAttr.stRcAttr.stH264FixQp, &stH264FixQp, sizeof(VENC_H264_FIXQP_S));
            }
            else if (SAMPLE_RC_VBR == enRcMode)
            {
                VENC_H264_VBR_S    stH264Vbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
                stH264Vbr.u32Gop           = u32Gop;
                stH264Vbr.u32StatTime      = u32StatTime;
                stH264Vbr.u32SrcFrameRate  = u32FrameRate;
                stH264Vbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                	case PIC_176x144:
                        stH264Vbr.u32MaxBitRate = 100;
                        break;
                    case PIC_360P:
                        stH264Vbr.u32MaxBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stH264Vbr.u32MaxBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH264Vbr.u32MaxBitRate = 1024 * 2   + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH264Vbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH264Vbr.u32MaxBitRate = 4096;//8192;//change from 2048
                        break;
                    case PIC_2592x1944:
                        stH264Vbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
					case PIC_2880x1624:
                        stH264Vbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH264Vbr.u32MaxBitRate = 1024 * 5   + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH264Vbr.u32MaxBitRate = 1024 * 10  + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH264Vbr.u32MaxBitRate = 1024 * 20  + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH264Vbr.u32MaxBitRate = 1024 * 15  + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH264Vbr, &stH264Vbr, sizeof(VENC_H264_VBR_S));
            }
            else if (SAMPLE_RC_AVBR == enRcMode)
            {
                VENC_H264_VBR_S    stH264AVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264AVBR;
                stH264AVbr.u32Gop           = u32Gop;
                stH264AVbr.u32StatTime      = u32StatTime;
                stH264AVbr.u32SrcFrameRate  = u32FrameRate;
                stH264AVbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_360P:
                        stH264AVbr.u32MaxBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stH264AVbr.u32MaxBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH264AVbr.u32MaxBitRate = 1024 * 2   + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH264AVbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH264AVbr.u32MaxBitRate = 4096;//8192;//change from 2048
                        break;
                    case PIC_2592x1944:
                        stH264AVbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
					case PIC_2880x1624:
                        stH264AVbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH264AVbr.u32MaxBitRate = 1024 * 5   + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH264AVbr.u32MaxBitRate = 1024 * 10  + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH264AVbr.u32MaxBitRate = 1024 * 20  + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH264AVbr.u32MaxBitRate = 1024 * 15  + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH264AVbr, &stH264AVbr, sizeof(VENC_H264_AVBR_S));
            }
            else if (SAMPLE_RC_QVBR == enRcMode)
            {
                VENC_H264_QVBR_S    stH264QVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264QVBR;
                stH264QVbr.u32Gop           = u32Gop;
                stH264QVbr.u32StatTime      = u32StatTime;
                stH264QVbr.u32SrcFrameRate  = u32FrameRate;
                stH264QVbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_360P:
                        stH264QVbr.u32TargetBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stH264QVbr.u32TargetBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH264QVbr.u32TargetBitRate = 1024 * 2   + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH264QVbr.u32TargetBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH264QVbr.u32TargetBitRate = 4096;//8192;//change from 2048
                        break;
                    case PIC_2592x1944:
                        stH264QVbr.u32TargetBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
					 case PIC_2880x1624:
                        stH264QVbr.u32TargetBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH264QVbr.u32TargetBitRate = 1024 * 5   + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH264QVbr.u32TargetBitRate = 1024 * 10  + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH264QVbr.u32TargetBitRate = 1024 * 20  + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH264QVbr.u32TargetBitRate = 1024 * 15  + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH264QVbr, &stH264QVbr, sizeof(VENC_H264_QVBR_S));
            }
            else if(SAMPLE_RC_CVBR == enRcMode)
            {
                VENC_H264_CVBR_S    stH264CVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CVBR;
                stH264CVbr.u32Gop         = u32Gop;
                stH264CVbr.u32StatTime    = u32StatTime;
                stH264CVbr.u32SrcFrameRate  = u32FrameRate;
                stH264CVbr.fr32DstFrameRate = u32FrameRate;
                stH264CVbr.u32LongTermStatTime  = 1;
                stH264CVbr.u32ShortTermStatTime = u32StatTime;
                switch (enSize)
                {
                    case PIC_720P:
                        stH264CVbr.u32MaxBitRate         = 1024 * 3 + 1024*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 2 + 1024*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 512;
                        break;
                    case PIC_1080P:
                        stH264CVbr.u32MaxBitRate         = 1024 * 2 + 2048*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 2 + 2048*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024;
                        break;
                    case PIC_2688x1860:
                        stH264CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
                    case PIC_2592x1944:
                        stH264CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
					case PIC_2688x1520:
                        stH264CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
					case PIC_2880x1624:
                        stH264CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
                    case PIC_3840x2160:
                        stH264CVbr.u32MaxBitRate         = 1024 * 8  + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 5  + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*3;
                        break;
                    case PIC_4000x3000:
                        stH264CVbr.u32MaxBitRate         = 1024 * 12  + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 10 + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*4;
                        break;
                    case PIC_7680x4320:
                        stH264CVbr.u32MaxBitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 20 + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*6;
                        break;
                    default :
                        stH264CVbr.u32MaxBitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 15 + 2048*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*5;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stH264CVbr, &stH264CVbr, sizeof(VENC_H264_CVBR_S));
            }
            else if(SAMPLE_RC_QPMAP == enRcMode)
            {
                VENC_H264_QPMAP_S    stH264QpMap;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264QPMAP;
                stH264QpMap.u32Gop           = u32Gop;
                stH264QpMap.u32StatTime      = u32StatTime;
                stH264QpMap.u32SrcFrameRate  = u32FrameRate;
                stH264QpMap.fr32DstFrameRate = u32FrameRate;
                memcpy(&stVencChnAttr.stRcAttr.stH264QpMap, &stH264QpMap, sizeof(VENC_H264_QPMAP_S));
            }
            else
            {
                SAMPLE_PRT("%s,%d,enRcMode(%d) not support\n",__FUNCTION__,__LINE__,enRcMode);
                return TS_FAILURE;
            }
        }
		stVencChnAttr.stVencAttr.stAttrH264e.bRcnRefShareBuf = TS_FALSE;
        break;
        case PT_MJPEG:
        {
            if (SAMPLE_RC_FIXQP == enRcMode)
            {
                VENC_MJPEG_FIXQP_S stMjpegeFixQp;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGFIXQP;
                stMjpegeFixQp.u32Qfactor        = 95;
                stMjpegeFixQp.u32SrcFrameRate    = u32FrameRate;
                stMjpegeFixQp.fr32DstFrameRate   = u32FrameRate;

                memcpy(&stVencChnAttr.stRcAttr.stMjpegFixQp, &stMjpegeFixQp,sizeof(VENC_MJPEG_FIXQP_S));
            }
            else if (SAMPLE_RC_CBR == enRcMode)
            {
                VENC_MJPEG_CBR_S stMjpegeCbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGCBR;
                stMjpegeCbr.u32StatTime         = u32StatTime;
                stMjpegeCbr.u32SrcFrameRate     = u32FrameRate;
                stMjpegeCbr.fr32DstFrameRate    = u32FrameRate;
                switch (enSize)
                {
                    case PIC_360P:
                        stMjpegeCbr.u32BitRate = 1024 * 3  + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stMjpegeCbr.u32BitRate = 1024 * 5  + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stMjpegeCbr.u32BitRate = 1024 * 8  + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stMjpegeCbr.u32BitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stMjpegeCbr.u32BitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stMjpegeCbr.u32BitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
					case PIC_2880x1624:
                        stMjpegeCbr.u32BitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stMjpegeCbr.u32BitRate = 1024 * 25 + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stMjpegeCbr.u32BitRate = 1024 * 30 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stMjpegeCbr.u32BitRate = 1024 * 40 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stMjpegeCbr.u32BitRate = 1024 * 20 + 2048*u32FrameRate/30;
                        break;
                }

                memcpy(&stVencChnAttr.stRcAttr.stMjpegCbr, &stMjpegeCbr,sizeof(VENC_MJPEG_CBR_S));
            }
            else if ((SAMPLE_RC_VBR == enRcMode) ||(SAMPLE_RC_AVBR == enRcMode)||
                     (SAMPLE_RC_QVBR == enRcMode)||(SAMPLE_RC_CVBR == enRcMode))
            {
                VENC_MJPEG_VBR_S   stMjpegVbr;

                if(SAMPLE_RC_AVBR == enRcMode)
                {
                    SAMPLE_PRT("Mjpege not support AVBR, so change rcmode to VBR!\n");
                }

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGVBR;
                stMjpegVbr.u32StatTime      = u32StatTime;
                stMjpegVbr.u32SrcFrameRate  = u32FrameRate;
                stMjpegVbr.fr32DstFrameRate = 5;

                switch (enSize)
                {
                    case PIC_360P:
                        stMjpegVbr.u32MaxBitRate = 1024 * 3 + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stMjpegVbr.u32MaxBitRate = 1024 * 5 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stMjpegVbr.u32MaxBitRate = 1024 * 8 + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stMjpegVbr.u32MaxBitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stMjpegVbr.u32MaxBitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stMjpegVbr.u32MaxBitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
					case PIC_2880x1624:
                        stMjpegVbr.u32MaxBitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stMjpegVbr.u32MaxBitRate = 1024 * 25 + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stMjpegVbr.u32MaxBitRate    = 1024 * 30 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stMjpegVbr.u32MaxBitRate = 1024 * 40 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stMjpegVbr.u32MaxBitRate = 1024 * 20 + 2048*u32FrameRate/30;
                        break;
                }

                memcpy(&stVencChnAttr.stRcAttr.stMjpegVbr, &stMjpegVbr,sizeof(VENC_MJPEG_VBR_S));
            }
            else
            {
                SAMPLE_PRT("cann't support other mode(%d) in this version!\n",enRcMode);
                return TS_FAILURE;
            }
        }
        break;

        case PT_JPEG:
            stJpegAttr.bSupportDCF     = TS_FALSE;
            stJpegAttr.stMPFCfg.u8LargeThumbNailNum = 0;
            stJpegAttr.enReceiveMode                = VENC_PIC_RECEIVE_SINGLE;
            memcpy(&stVencChnAttr.stVencAttr.stAttrJpege, &stJpegAttr, sizeof(VENC_ATTR_JPEG_S));
            break;
        default:
            SAMPLE_PRT("cann't support this enType (%d) in this version!\n",enType);
            return TS_ERR_VENC_NOT_SUPPORT;
    }

    if(PT_MJPEG == enType || PT_JPEG == enType )
    {
        stVencChnAttr.stGopAttr.enGopMode  = VENC_GOPMODE_NORMALP;
        stVencChnAttr.stGopAttr.stNormalP.s32IPQpDelta = 0;
    }
    else
    {
        memcpy(&stVencChnAttr.stGopAttr,pstGopAttr,sizeof(VENC_GOP_ATTR_S));
        if((VENC_GOPMODE_BIPREDB == pstGopAttr->enGopMode)&&(PT_H264 == enType))
        {
            if(0 == stVencChnAttr.stVencAttr.u32Profile)
            {
                stVencChnAttr.stVencAttr.u32Profile = 1;

                SAMPLE_PRT("H.264 base profile not support BIPREDB, so change profile to main profile!\n");
            }
        }

        if((VENC_RC_MODE_H264QPMAP == stVencChnAttr.stRcAttr.enRcMode)||(VENC_RC_MODE_H265QPMAP == stVencChnAttr.stRcAttr.enRcMode))
        {
            if(VENC_GOPMODE_ADVSMARTP == pstGopAttr->enGopMode)
            {
                stVencChnAttr.stGopAttr.enGopMode = VENC_GOPMODE_SMARTP;

                SAMPLE_PRT("advsmartp not support QPMAP, so change gopmode to smartp!\n");
            }
        }
    }

	//SAMPLE_COMM_VENC_GetChnAttrByTpye(enType, enSize, &stVencChnAttr);

	SAMPLE_PRT("create venChn[%d], %s, MaxWH=[%d,%d], w=h[%d,%d], bufSize=%d\n", VencChn,
		PT_H264 == stVencChnAttr.stVencAttr.enType ? "H264" : "Other",
		stVencChnAttr.stVencAttr.u32MaxPicWidth, stVencChnAttr.stVencAttr.u32MaxPicHeight,
		stVencChnAttr.stVencAttr.u32PicWidth, stVencChnAttr.stVencAttr.u32PicHeight,
		stVencChnAttr.stVencAttr.u32BufSize);

    s32Ret = TS_MPI_VENC_CreateChn(VencChn, &stVencChnAttr);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_CreateChn [%d] faild with %#x! ===\n", \
                   VencChn, s32Ret);
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_VENC_CloseReEncode(VencChn);
    if (TS_SUCCESS != s32Ret)
    {
        TS_MPI_VENC_DestroyChn(VencChn);
        return s32Ret;
    }

    return TS_SUCCESS;
}


TS_S32 SAMPLE_COMM_VENC_CreatEx(VENC_CHN VencChn, SAMPLE_VENC_CHNL_ATTR_S * pChnlAttr)
{
    TS_S32 s32Ret;
    SIZE_S stPicSize;
    VENC_CHN_ATTR_S        stVencChnAttr = {0};
    VENC_ATTR_JPEG_S       stJpegAttr = {0};
    TS_U32                 u32StatTime;
    TS_U32                 u32Gop = 0;//100;
    PAYLOAD_TYPE_E enType;
	PIC_SIZE_E enSize;
	TS_U32 u32FrameRate;
	SAMPLE_RC_E enRcMode;
	TS_U32  u32Profile;
	VENC_GOP_ATTR_S *pstGopAttr;

	if(!pChnlAttr){
		SAMPLE_PRT("null ptr\n");
		return TS_FAILURE;
	}

	u32Gop = pChnlAttr->u32Gop;
	enType = pChnlAttr->enType;
	enSize = pChnlAttr->enSize;
	u32FrameRate = pChnlAttr->u32FrameRate;
	enRcMode = pChnlAttr->enRcMode;
	u32Profile = pChnlAttr->u32Profile;
	pstGopAttr = &pChnlAttr->stGopAttr;

    SAMPLE_COMM_SYS_GetPicSize(enSize, &stPicSize);

    /******************************************
     step 1:  Create Venc Channel
    ******************************************/
    stVencChnAttr.stVencAttr.enType          = enType;
    stVencChnAttr.stVencAttr.u32MaxPicWidth  = stPicSize.u32Width;
    stVencChnAttr.stVencAttr.u32MaxPicHeight = stPicSize.u32Height;
    stVencChnAttr.stVencAttr.u32PicWidth     = stPicSize.u32Width;/*the picture width*/
    stVencChnAttr.stVencAttr.u32PicHeight    = stPicSize.u32Height;/*the picture height*/
    if((stPicSize.u32Width == 2688 && stPicSize.u32Height == 1520) || (stPicSize.u32Width == 2560 && stPicSize.u32Height == 1440))
        stVencChnAttr.stVencAttr.u32BufSize  = ALIGN_UP(stPicSize.u32Width, 32) * ALIGN_UP(stPicSize.u32Height, 32) * 3 / 2;
    else
        stVencChnAttr.stVencAttr.u32BufSize  = ALIGN_UP(stPicSize.u32Width, 32) * ALIGN_UP(stPicSize.u32Height, 32) * 3 / 2;
    stVencChnAttr.stVencAttr.u32Profile      = u32Profile;
    stVencChnAttr.stVencAttr.bByFrame        = TS_TRUE;/*get stream mode is slice mode or frame mode?*/

    if(VENC_GOPMODE_SMARTP == pstGopAttr->enGopMode)
    {
        u32StatTime = pstGopAttr->stSmartP.u32BgInterval/u32Gop;
    }
    else
    {
        u32StatTime = 1;
    }

    switch (enType)
    {
        case PT_H265:
        {
            if (SAMPLE_RC_CBR == enRcMode)
            {
                VENC_H265_CBR_S    stH265Cbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
                stH265Cbr.u32Gop            = u32Gop;

                stH265Cbr.u32StatTime       = u32StatTime; /* stream rate statics time(s) */
                stH265Cbr.u32SrcFrameRate   = u32FrameRate; /* input (vi) frame rate */
                stH265Cbr.fr32DstFrameRate  = u32FrameRate; /* target frame rate */
                switch (enSize)
                {
                	case PIC_176x144:
                        stH265Cbr.u32BitRate = 100;//1024 * 1 + 1024*u32FrameRate/30;
                        break;
                    case PIC_360P:
                        stH265Cbr.u32BitRate = 160;//1024 * 1 + 1024*u32FrameRate/30;
                        break;
                    case PIC_480x270:
                        stH265Cbr.u32BitRate = 200;//1024 * 1 + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                    case PIC_1280x720:
                        stH265Cbr.u32BitRate = 384;//1024 * 2 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH265Cbr.u32BitRate = 800;//1024 * 2 + 2048*u32FrameRate/30;
                        break;
					case PIC_2304x1296:
						stH265Cbr.u32BitRate = 2048;
						break;
					case PIC_576P:
						stH265Cbr.u32BitRate = 512;
						break;
                    case PIC_2560x1440:
                        stH265Cbr.u32BitRate = 3072;//1024 * 2 + 3072*u32FrameRate/30;
                        break;
					case PIC_1440x2560:
                        stH265Cbr.u32BitRate = 3072;//1024 * 2 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH265Cbr.u32BitRate = 2560;//1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stH265Cbr.u32BitRate = 2560;//1024 * 3 + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH265Cbr.u32BitRate = 4096;//8192;//change from 2048
                        break;
					case PIC_2880x1624:
                        stH265Cbr.u32BitRate = 2560;//1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH265Cbr.u32BitRate = 4096;//1024 * 3  + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH265Cbr.u32BitRate = 1024 * 10 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH265Cbr.u32BitRate = 1024 * 20 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH265Cbr.u32BitRate = 1024 * 15 + 2048*u32FrameRate/30;
                        break;
                }
				stH265Cbr.u32MaxBitRate = stH265Cbr.u32BitRate * 3 / 2;
				if(pChnlAttr->u32Bitrate > 0){
					stH265Cbr.u32BitRate = pChnlAttr->u32Bitrate;
					stH265Cbr.u32MaxBitRate = stH265Cbr.u32BitRate * 3 / 2;
				}

                if (pChnlAttr->u32MaxBitrate > 0) {
                    stH265Cbr.u32MaxBitRate = pChnlAttr->u32MaxBitrate;
                }

                memcpy(&stVencChnAttr.stRcAttr.stH265Cbr, &stH265Cbr, sizeof(VENC_H265_CBR_S));
            }
            else if (SAMPLE_RC_FIXQP == enRcMode)
            {
                VENC_H265_FIXQP_S    stH265FixQp;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265FIXQP;
                stH265FixQp.u32Gop              = 30;
                stH265FixQp.u32SrcFrameRate     = u32FrameRate;
                stH265FixQp.fr32DstFrameRate    = u32FrameRate;
                stH265FixQp.u32IQp              = 25;
                stH265FixQp.u32PQp              = 30;
                stH265FixQp.u32BQp              = 32;
                memcpy(&stVencChnAttr.stRcAttr.stH265FixQp, &stH265FixQp, sizeof(VENC_H265_FIXQP_S));
            }
            else if (SAMPLE_RC_VBR == enRcMode)
            {
                VENC_H265_VBR_S    stH265Vbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;
                stH265Vbr.u32Gop           = u32Gop;
                stH265Vbr.u32StatTime      = u32StatTime;
                stH265Vbr.u32SrcFrameRate  = u32FrameRate;
                stH265Vbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                	case PIC_176x144:
                        stH265Vbr.u32MaxBitRate = 100;//1024 * 1 + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stH265Vbr.u32MaxBitRate = 1024 * 2 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH265Vbr.u32MaxBitRate = 1024 * 2 + 2048*u32FrameRate/30;
                        break;
					case PIC_576P:
                        stH265Vbr.u32MaxBitRate = 512;
                        break;
					case PIC_2560x1440:
						stH265Vbr.u32MaxBitRate = 3072; //1024 * 3 + 3072*u32FrameRate/30;
						break;
                    case PIC_2688x1860:
                        stH265Vbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stH265Vbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH265Vbr.u32MaxBitRate = 4096;//8192;//change from 2048
                        break;
					case PIC_2880x1624:
                        stH265Vbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH265Vbr.u32MaxBitRate = 1024 * 5  + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH265Vbr.u32MaxBitRate = 1024 * 10 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH265Vbr.u32MaxBitRate = 1024 * 20 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH265Vbr.u32MaxBitRate    = 1024 * 15 + 2048*u32FrameRate/30;
                        break;
                }

				if (pChnlAttr->u32MaxBitrate > 0) {
					stH265Vbr.u32MaxBitRate = pChnlAttr->u32MaxBitrate;
				}

				if (pChnlAttr->u32AvgBitRate > 0) {
					stH265Vbr.u32AvgBitRate = pChnlAttr->u32AvgBitRate;
				}

                memcpy(&stVencChnAttr.stRcAttr.stH265Vbr, &stH265Vbr, sizeof(VENC_H265_VBR_S));
            }
            else if(SAMPLE_RC_AVBR == enRcMode)
            {
                VENC_H265_AVBR_S    stH265AVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265AVBR;
                stH265AVbr.u32Gop         = u32Gop;
                stH265AVbr.u32StatTime    = u32StatTime;
                stH265AVbr.u32SrcFrameRate  = u32FrameRate;
                stH265AVbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_720P:
                        stH265AVbr.u32MaxBitRate = 1024 * 2 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH265AVbr.u32MaxBitRate = 1024 * 2 + 2048*u32FrameRate/30;
                        break;
					 case PIC_576P:
                        stH265AVbr.u32MaxBitRate = 1024 * 2 + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH265AVbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stH265AVbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH265AVbr.u32MaxBitRate = 4096;//8192;//change from 2048
                        break;
					case PIC_2880x1624:
                        stH265AVbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH265AVbr.u32MaxBitRate = 1024 * 5  + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH265AVbr.u32MaxBitRate = 1024 * 10 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH265AVbr.u32MaxBitRate = 1024 * 20 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH265AVbr.u32MaxBitRate    = 1024 * 15 + 2048*u32FrameRate/30;
                        break;
                }
				if(pChnlAttr->u32Bitrate > 0){
					stH265AVbr.u32MaxBitRate = pChnlAttr->u32Bitrate;
				}
                memcpy(&stVencChnAttr.stRcAttr.stH265AVbr, &stH265AVbr, sizeof(VENC_H265_AVBR_S));
            }
            else if(SAMPLE_RC_QVBR == enRcMode)
            {
                VENC_H265_QVBR_S    stH265QVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265QVBR;
                stH265QVbr.u32Gop         = u32Gop;
                stH265QVbr.u32StatTime    = u32StatTime;
                stH265QVbr.u32SrcFrameRate  = u32FrameRate;
                stH265QVbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_720P:
                        stH265QVbr.u32TargetBitRate= 1024 * 2 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH265QVbr.u32TargetBitRate = 1024 * 2 + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH265QVbr.u32TargetBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH265QVbr.u32TargetBitRate = 4096;//8192;//change from 2048
                        break;
                    case PIC_2592x1944:
                        stH265QVbr.u32TargetBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
					case PIC_2880x1624:
                        stH265QVbr.u32TargetBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH265QVbr.u32TargetBitRate = 1024 * 5  + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH265QVbr.u32TargetBitRate = 1024 * 10 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH265QVbr.u32TargetBitRate = 1024 * 20 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH265QVbr.u32TargetBitRate    = 1024 * 15 + 2048*u32FrameRate/30;
                        break;
                }
				if(pChnlAttr->u32Bitrate > 0){
					stH265QVbr.u32TargetBitRate = pChnlAttr->u32Bitrate;
				}
                memcpy(&stVencChnAttr.stRcAttr.stH265QVbr, &stH265QVbr, sizeof(VENC_H265_QVBR_S));
            }
            else if(SAMPLE_RC_CVBR == enRcMode)
            {
                VENC_H265_CVBR_S    stH265CVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CVBR;
                stH265CVbr.u32Gop         = u32Gop;
                stH265CVbr.u32StatTime    = u32StatTime;
                stH265CVbr.u32SrcFrameRate  = u32FrameRate;
                stH265CVbr.fr32DstFrameRate = u32FrameRate;
                stH265CVbr.u32LongTermStatTime  = 1;
                stH265CVbr.u32ShortTermStatTime = u32StatTime;
                switch (enSize)
                {
                    case PIC_720P:
                        stH265CVbr.u32MaxBitRate         = 1024 * 3 + 1024*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 2 + 1024*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 512;
                        break;
                    case PIC_1080P:
                        stH265CVbr.u32MaxBitRate         = 1024 * 2 + 2048*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 2 + 2048*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024;
                        break;
                    case PIC_2688x1860:
                        stH265CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
                    case PIC_2592x1944:
                        stH265CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
					case PIC_2688x1520:
                        stH265CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
					case PIC_2880x1624:
                        stH265CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
                    case PIC_3840x2160:
                        stH265CVbr.u32MaxBitRate         = 1024 * 8  + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 5  + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*3;
                        break;
                    case PIC_4000x3000:
                        stH265CVbr.u32MaxBitRate         = 1024 * 12  + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 10 + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*4;
                        break;
                    case PIC_7680x4320:
                        stH265CVbr.u32MaxBitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 20 + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*6;
                        break;
                    default :
                        stH265CVbr.u32MaxBitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        stH265CVbr.u32LongTermMaxBitrate = 1024 * 15 + 2048*u32FrameRate/30;
                        stH265CVbr.u32LongTermMinBitrate = 1024*5;
                        break;
                }
				if(pChnlAttr->u32Bitrate > 0){
					stH265CVbr.u32LongTermMaxBitrate = pChnlAttr->u32Bitrate;
				}
                memcpy(&stVencChnAttr.stRcAttr.stH265CVbr, &stH265CVbr, sizeof(VENC_H265_CVBR_S));
            }
            else if(SAMPLE_RC_QPMAP == enRcMode)
            {
                VENC_H265_QPMAP_S    stH265QpMap;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265QPMAP;
                stH265QpMap.u32Gop           = u32Gop;
                stH265QpMap.u32StatTime      = u32StatTime;
                stH265QpMap.u32SrcFrameRate  = u32FrameRate;
                stH265QpMap.fr32DstFrameRate = u32FrameRate;
                stH265QpMap.enQpMapMode      = VENC_RC_QPMAP_MODE_MEANQP;
                memcpy(&stVencChnAttr.stRcAttr.stH265QpMap, &stH265QpMap, sizeof(VENC_H265_QPMAP_S));
            }
            else
            {
                SAMPLE_PRT("%s,%d,enRcMode(%d) not support\n",__FUNCTION__,__LINE__,enRcMode);
                return TS_FAILURE;
            }
        }
		stVencChnAttr.stVencAttr.stAttrH265e.bRcnRefShareBuf = TS_FALSE;
        break;
        case PT_H264:
        {
            if (SAMPLE_RC_CBR == enRcMode)
            {
                VENC_H264_CBR_S    stH264Cbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
                stH264Cbr.u32Gop                = u32Gop; /*the interval of IFrame*/
                stH264Cbr.u32StatTime           = u32StatTime; /* stream rate statics time(s) */
                stH264Cbr.u32SrcFrameRate       = u32FrameRate; /* input (vi) frame rate */
                stH264Cbr.fr32DstFrameRate      = u32FrameRate; /* target frame rate */
                switch (enSize)
                {
                	 case PIC_176x144:
                        stH264Cbr.u32BitRate = 100;
                        break;
                    case PIC_360P:
                        stH264Cbr.u32BitRate = 160;
                        break;
                    case PIC_480x270:
                        stH264Cbr.u32BitRate = 200;
                        break;
                    case PIC_720P:
                        stH264Cbr.u32BitRate         = 1024 * 3 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH264Cbr.u32BitRate         = 1024 * 2 + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH264Cbr.u32BitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH264Cbr.u32BitRate         = 4096;//8192;//change from 2048
                        break;
                    case PIC_2592x1944:
                        stH264Cbr.u32BitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        break;
					case PIC_2880x1624:
                        stH264Cbr.u32BitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH264Cbr.u32BitRate         = 1024 * 8  + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH264Cbr.u32BitRate         = 1024 * 12  + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH264Cbr.u32BitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH264Cbr.u32BitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        break;
                }
				stH264Cbr.u32MaxBitRate = stH264Cbr.u32BitRate * 3 /2;
				if(pChnlAttr->u32Bitrate > 0){
					stH264Cbr.u32BitRate = pChnlAttr->u32Bitrate;
					stH264Cbr.u32MaxBitRate = stH264Cbr.u32BitRate * 3 / 2;
				}

                if (pChnlAttr->u32MaxBitrate > 0) {
                    stH264Cbr.u32MaxBitRate = pChnlAttr->u32MaxBitrate;
                }

                memcpy(&stVencChnAttr.stRcAttr.stH264Cbr, &stH264Cbr, sizeof(VENC_H264_CBR_S));
            }
            else if (SAMPLE_RC_FIXQP == enRcMode)
            {
                VENC_H264_FIXQP_S    stH264FixQp;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264FIXQP;
                stH264FixQp.u32Gop           = 30;
                stH264FixQp.u32SrcFrameRate  = u32FrameRate;
                stH264FixQp.fr32DstFrameRate = u32FrameRate;
                stH264FixQp.u32IQp           = 25;
                stH264FixQp.u32PQp           = 30;
                stH264FixQp.u32BQp           = 32;
                memcpy(&stVencChnAttr.stRcAttr.stH264FixQp, &stH264FixQp, sizeof(VENC_H264_FIXQP_S));
            }
            else if (SAMPLE_RC_VBR == enRcMode)
            {
                VENC_H264_VBR_S    stH264Vbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
                stH264Vbr.u32Gop           = u32Gop;
                stH264Vbr.u32StatTime      = u32StatTime;
                stH264Vbr.u32SrcFrameRate  = u32FrameRate;
                stH264Vbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                	case PIC_176x144:
                        stH264Vbr.u32MaxBitRate = 100;
                        break;
                    case PIC_360P:
                        stH264Vbr.u32MaxBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stH264Vbr.u32MaxBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH264Vbr.u32MaxBitRate = 1024 * 2   + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH264Vbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH264Vbr.u32MaxBitRate = 4096;//8192;//change from 2048
                        break;
                    case PIC_2592x1944:
                        stH264Vbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
					case PIC_2880x1624:
                        stH264Vbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH264Vbr.u32MaxBitRate = 1024 * 5   + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH264Vbr.u32MaxBitRate = 1024 * 10  + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH264Vbr.u32MaxBitRate = 1024 * 20  + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH264Vbr.u32MaxBitRate = 1024 * 15  + 2048*u32FrameRate/30;
                        break;
                }
				if(pChnlAttr->u32Bitrate > 0){
					stH264Vbr.u32MaxBitRate = pChnlAttr->u32Bitrate;
				}

				if (pChnlAttr->u32AvgBitRate > 0) {
					stH264Vbr.u32AvgBitRate = pChnlAttr->u32AvgBitRate;
				}

                memcpy(&stVencChnAttr.stRcAttr.stH264Vbr, &stH264Vbr, sizeof(VENC_H264_VBR_S));
            }
            else if (SAMPLE_RC_AVBR == enRcMode)
            {
                VENC_H264_VBR_S    stH264AVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264AVBR;
                stH264AVbr.u32Gop           = u32Gop;
                stH264AVbr.u32StatTime      = u32StatTime;
                stH264AVbr.u32SrcFrameRate  = u32FrameRate;
                stH264AVbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_360P:
                        stH264AVbr.u32MaxBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stH264AVbr.u32MaxBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH264AVbr.u32MaxBitRate = 1024 * 2   + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH264AVbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH264AVbr.u32MaxBitRate = 4096;//8192;//change from 2048
                        break;
                    case PIC_2592x1944:
                        stH264AVbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
					case PIC_2880x1624:
                        stH264AVbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH264AVbr.u32MaxBitRate = 1024 * 5   + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH264AVbr.u32MaxBitRate = 1024 * 10  + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH264AVbr.u32MaxBitRate = 1024 * 20  + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH264AVbr.u32MaxBitRate = 1024 * 15  + 2048*u32FrameRate/30;
                        break;
                }
				if(pChnlAttr->u32Bitrate > 0){
					stH264AVbr.u32MaxBitRate = pChnlAttr->u32Bitrate;
				}
                memcpy(&stVencChnAttr.stRcAttr.stH264AVbr, &stH264AVbr, sizeof(VENC_H264_AVBR_S));
            }
            else if (SAMPLE_RC_QVBR == enRcMode)
            {
                VENC_H264_QVBR_S    stH264QVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264QVBR;
                stH264QVbr.u32Gop           = u32Gop;
                stH264QVbr.u32StatTime      = u32StatTime;
                stH264QVbr.u32SrcFrameRate  = u32FrameRate;
                stH264QVbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_360P:
                        stH264QVbr.u32TargetBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stH264QVbr.u32TargetBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH264QVbr.u32TargetBitRate = 1024 * 2   + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stH264QVbr.u32TargetBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stH264QVbr.u32TargetBitRate = 4096;//8192;//change from 2048
                        break;
                    case PIC_2592x1944:
                        stH264QVbr.u32TargetBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
					 case PIC_2880x1624:
                        stH264QVbr.u32TargetBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH264QVbr.u32TargetBitRate = 1024 * 5   + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH264QVbr.u32TargetBitRate = 1024 * 10  + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH264QVbr.u32TargetBitRate = 1024 * 20  + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH264QVbr.u32TargetBitRate = 1024 * 15  + 2048*u32FrameRate/30;
                        break;
                }
				if(pChnlAttr->u32Bitrate > 0){
					stH264QVbr.u32TargetBitRate = pChnlAttr->u32Bitrate;
				}
                memcpy(&stVencChnAttr.stRcAttr.stH264QVbr, &stH264QVbr, sizeof(VENC_H264_QVBR_S));
            }
            else if(SAMPLE_RC_CVBR == enRcMode)
            {
                VENC_H264_CVBR_S    stH264CVbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CVBR;
                stH264CVbr.u32Gop         = u32Gop;
                stH264CVbr.u32StatTime    = u32StatTime;
                stH264CVbr.u32SrcFrameRate  = u32FrameRate;
                stH264CVbr.fr32DstFrameRate = u32FrameRate;
                stH264CVbr.u32LongTermStatTime  = 1;
                stH264CVbr.u32ShortTermStatTime = u32StatTime;
                switch (enSize)
                {
                    case PIC_720P:
                        stH264CVbr.u32MaxBitRate         = 1024 * 3 + 1024*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 2 + 1024*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 512;
                        break;
                    case PIC_1080P:
                        stH264CVbr.u32MaxBitRate         = 1024 * 2 + 2048*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 2 + 2048*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024;
                        break;
                    case PIC_2688x1860:
                        stH264CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
                    case PIC_2592x1944:
                        stH264CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
					case PIC_2688x1520:
                        stH264CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
					case PIC_2880x1624:
                        stH264CVbr.u32MaxBitRate         = 1024 * 4 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 3 + 3072*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*2;
                        break;
                    case PIC_3840x2160:
                        stH264CVbr.u32MaxBitRate         = 1024 * 8  + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 5  + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*3;
                        break;
                    case PIC_4000x3000:
                        stH264CVbr.u32MaxBitRate         = 1024 * 12  + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 10 + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*4;
                        break;
                    case PIC_7680x4320:
                        stH264CVbr.u32MaxBitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 20 + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*6;
                        break;
                    default :
                        stH264CVbr.u32MaxBitRate         = 1024 * 24  + 5120*u32FrameRate/30;
                        stH264CVbr.u32LongTermMaxBitrate = 1024 * 15 + 2048*u32FrameRate/30;
                        stH264CVbr.u32LongTermMinBitrate = 1024*5;
                        break;
                }
				if(pChnlAttr->u32Bitrate > 0){
					stH264CVbr.u32MaxBitRate = pChnlAttr->u32Bitrate;
				}
                memcpy(&stVencChnAttr.stRcAttr.stH264CVbr, &stH264CVbr, sizeof(VENC_H264_CVBR_S));
            }
            else if(SAMPLE_RC_QPMAP == enRcMode)
            {
                VENC_H264_QPMAP_S    stH264QpMap;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264QPMAP;
                stH264QpMap.u32Gop           = u32Gop;
                stH264QpMap.u32StatTime      = u32StatTime;
                stH264QpMap.u32SrcFrameRate  = u32FrameRate;
                stH264QpMap.fr32DstFrameRate = u32FrameRate;
                memcpy(&stVencChnAttr.stRcAttr.stH264QpMap, &stH264QpMap, sizeof(VENC_H264_QPMAP_S));
            }
            else
            {
                SAMPLE_PRT("%s,%d,enRcMode(%d) not support\n",__FUNCTION__,__LINE__,enRcMode);
                return TS_FAILURE;
            }
        }
		stVencChnAttr.stVencAttr.stAttrH264e.bRcnRefShareBuf = TS_FALSE;
        break;
        case PT_MJPEG:
        {
            if (SAMPLE_RC_FIXQP == enRcMode)
            {
                VENC_MJPEG_FIXQP_S stMjpegeFixQp;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGFIXQP;
                stMjpegeFixQp.u32Qfactor        = 95;
                stMjpegeFixQp.u32SrcFrameRate    = u32FrameRate;
                stMjpegeFixQp.fr32DstFrameRate   = u32FrameRate;

                memcpy(&stVencChnAttr.stRcAttr.stMjpegFixQp, &stMjpegeFixQp,sizeof(VENC_MJPEG_FIXQP_S));
            }
            else if (SAMPLE_RC_CBR == enRcMode)
            {
                VENC_MJPEG_CBR_S stMjpegeCbr;

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGCBR;
                stMjpegeCbr.u32StatTime         = u32StatTime;
                stMjpegeCbr.u32SrcFrameRate     = u32FrameRate;
                stMjpegeCbr.fr32DstFrameRate    = u32FrameRate;
                switch (enSize)
                {
                    case PIC_360P:
                        stMjpegeCbr.u32BitRate = 1024 * 3  + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stMjpegeCbr.u32BitRate = 1024 * 5  + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stMjpegeCbr.u32BitRate = 1024 * 8  + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stMjpegeCbr.u32BitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stMjpegeCbr.u32BitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stMjpegeCbr.u32BitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
					case PIC_2880x1624:
                        stMjpegeCbr.u32BitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stMjpegeCbr.u32BitRate = 1024 * 25 + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stMjpegeCbr.u32BitRate = 1024 * 30 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stMjpegeCbr.u32BitRate = 1024 * 40 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stMjpegeCbr.u32BitRate = 1024 * 20 + 2048*u32FrameRate/30;
                        break;
                }

				if(pChnlAttr->u32Bitrate > 0){
					stMjpegeCbr.u32BitRate = pChnlAttr->u32Bitrate;
				}
                memcpy(&stVencChnAttr.stRcAttr.stMjpegCbr, &stMjpegeCbr,sizeof(VENC_MJPEG_CBR_S));
            }
            else if ((SAMPLE_RC_VBR == enRcMode) ||(SAMPLE_RC_AVBR == enRcMode)||
                     (SAMPLE_RC_QVBR == enRcMode)||(SAMPLE_RC_CVBR == enRcMode))
            {
                VENC_MJPEG_VBR_S   stMjpegVbr;

                if(SAMPLE_RC_AVBR == enRcMode)
                {
                    SAMPLE_PRT("Mjpege not support AVBR, so change rcmode to VBR!\n");
                }

                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGVBR;
                stMjpegVbr.u32StatTime      = u32StatTime;
                stMjpegVbr.u32SrcFrameRate  = u32FrameRate;
                stMjpegVbr.fr32DstFrameRate = 5;

                switch (enSize)
                {
                    case PIC_360P:
                        stMjpegVbr.u32MaxBitRate = 1024 * 3 + 1024*u32FrameRate/30;
                        break;
                    case PIC_720P:
                        stMjpegVbr.u32MaxBitRate = 1024 * 5 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stMjpegVbr.u32MaxBitRate = 1024 * 8 + 2048*u32FrameRate/30;
                        break;
                    case PIC_2688x1860:
                        stMjpegVbr.u32MaxBitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
					case PIC_2688x1520:
                        stMjpegVbr.u32MaxBitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stMjpegVbr.u32MaxBitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
					case PIC_2880x1624:
                        stMjpegVbr.u32MaxBitRate = 1024 * 20 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stMjpegVbr.u32MaxBitRate = 1024 * 25 + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stMjpegVbr.u32MaxBitRate    = 1024 * 30 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stMjpegVbr.u32MaxBitRate = 1024 * 40 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stMjpegVbr.u32MaxBitRate = 1024 * 20 + 2048*u32FrameRate/30;
                        break;
                }

				if(pChnlAttr->u32Bitrate > 0){
					stMjpegVbr.u32MaxBitRate = pChnlAttr->u32Bitrate;
				}
                memcpy(&stVencChnAttr.stRcAttr.stMjpegVbr, &stMjpegVbr,sizeof(VENC_MJPEG_VBR_S));
            }
            else
            {
                SAMPLE_PRT("cann't support other mode(%d) in this version!\n",enRcMode);
                return TS_FAILURE;
            }
        }
        break;

        case PT_JPEG:
            stJpegAttr.bSupportDCF     = TS_FALSE;
            stJpegAttr.stMPFCfg.u8LargeThumbNailNum = 0;
            stJpegAttr.enReceiveMode                = VENC_PIC_RECEIVE_SINGLE;
            memcpy(&stVencChnAttr.stVencAttr.stAttrJpege, &stJpegAttr, sizeof(VENC_ATTR_JPEG_S));
            break;
        default:
            SAMPLE_PRT("cann't support this enType (%d) in this version!\n",enType);
            return TS_ERR_VENC_NOT_SUPPORT;
    }

    if(PT_MJPEG == enType || PT_JPEG == enType )
    {
        stVencChnAttr.stGopAttr.enGopMode  = VENC_GOPMODE_NORMALP;
        stVencChnAttr.stGopAttr.stNormalP.s32IPQpDelta = 0;
    }
    else
    {
        memcpy(&stVencChnAttr.stGopAttr,pstGopAttr,sizeof(VENC_GOP_ATTR_S));
        if((VENC_GOPMODE_BIPREDB == pstGopAttr->enGopMode)&&(PT_H264 == enType))
        {
            if(0 == stVencChnAttr.stVencAttr.u32Profile)
            {
                stVencChnAttr.stVencAttr.u32Profile = 1;

                SAMPLE_PRT("H.264 base profile not support BIPREDB, so change profile to main profile!\n");
            }
        }

        if((VENC_RC_MODE_H264QPMAP == stVencChnAttr.stRcAttr.enRcMode)||(VENC_RC_MODE_H265QPMAP == stVencChnAttr.stRcAttr.enRcMode))
        {
            if(VENC_GOPMODE_ADVSMARTP == pstGopAttr->enGopMode)
            {
                stVencChnAttr.stGopAttr.enGopMode = VENC_GOPMODE_SMARTP;

                SAMPLE_PRT("advsmartp not support QPMAP, so change gopmode to smartp!\n");
            }
        }
    }

	//SAMPLE_COMM_VENC_GetChnAttrByTpye(enType, enSize, &stVencChnAttr);

	SAMPLE_PRT("create venChn[%d], %s, MaxWH=[%d,%d], w=h[%d,%d], bufSize=%d\n", VencChn,
		PT_H264 == stVencChnAttr.stVencAttr.enType ? "H264" : "Other",
		stVencChnAttr.stVencAttr.u32MaxPicWidth, stVencChnAttr.stVencAttr.u32MaxPicHeight,
		stVencChnAttr.stVencAttr.u32PicWidth, stVencChnAttr.stVencAttr.u32PicHeight,
		stVencChnAttr.stVencAttr.u32BufSize);

    s32Ret = TS_MPI_VENC_CreateChn(VencChn, &stVencChnAttr);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_CreateChn [%d] faild with %#x! ===\n", \
                   VencChn, s32Ret);
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_VENC_CloseReEncode(VencChn);
    if (TS_SUCCESS != s32Ret)
    {
        TS_MPI_VENC_DestroyChn(VencChn);
        return s32Ret;
    }

    return TS_SUCCESS;
}

/******************************************************************************
* funciton : Start venc stream mode
* note      : rate control parameter need adjust, according your case.
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_Start_By_Num(VENC_CHN VencChn, PAYLOAD_TYPE_E enType,  PIC_SIZE_E enSize, TS_U32 u32FrameRate, SAMPLE_RC_E enRcMode,
        TS_U32  u32Profile, VENC_GOP_ATTR_S *pstGopAttr, TS_S32 s32Num)
{
    TS_S32 s32Ret;
    VENC_RECV_PIC_PARAM_S  stRecvParam;

    /******************************************
     step 1:  Creat Encode Chnl
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_Creat(VencChn,enType,enSize,u32FrameRate, enRcMode,u32Profile,pstGopAttr);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VENC_Creat faild with: %#x! \n", s32Ret);
        return TS_FAILURE;
    }
    /******************************************
     step 2:  Start Recv Venc Pictures
    ******************************************/
    stRecvParam.s32RecvPicNum = s32Num;
    s32Ret = TS_MPI_VENC_StartRecvFrame(VencChn,&stRecvParam);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_StartRecvPic faild with%#x! \n", s32Ret);
        return TS_FAILURE;
    }
    return TS_SUCCESS;
}


/******************************************************************************
* funciton : Start venc stream mode
* note      : rate control parameter need adjust, according your case.
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_Start_AllocBufOnce(VENC_CHN VencChn, PAYLOAD_TYPE_E enType,  PIC_SIZE_E enSize, TS_U32 u32FrameRate, SAMPLE_RC_E enRcMode,
        TS_U32  u32Profile, VENC_GOP_ATTR_S *pstGopAttr)
{
    TS_S32 s32Ret;
    VENC_RECV_PIC_PARAM_S  stRecvParam;

    /******************************************
     step 1:  Creat Encode Chnl
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_CreatAllocBufOnce(VencChn,enType,enSize,u32FrameRate, enRcMode,u32Profile,pstGopAttr);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VENC_Creat faild with: %#x! \n", s32Ret);
        return TS_FAILURE;
    }
    /******************************************
     step 2:  Start Recv Venc Pictures
    ******************************************/
    stRecvParam.s32RecvPicNum = -1;
    s32Ret = TS_MPI_VENC_StartRecvFrame(VencChn,&stRecvParam);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_StartRecvPic faild with%#x! \n", s32Ret);
        return TS_FAILURE;
    }
    return TS_SUCCESS;
}





/******************************************************************************
* funciton : Start venc stream mode
* note      : rate control parameter need adjust, according your case.
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_StartEx(VENC_CHN VencChn, SAMPLE_VENC_CHNL_ATTR_S * pChnlAttr)

{
    TS_S32 s32Ret;
    VENC_RECV_PIC_PARAM_S  stRecvParam;

    /******************************************
     step 1:  Creat Encode Chnl
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_CreatEx(VencChn, pChnlAttr);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VENC_Creat faild with: %#x! \n", s32Ret);
        return TS_FAILURE;
    }
    /******************************************
     step 2:  Start Recv Venc Pictures
    ******************************************/
    stRecvParam.s32RecvPicNum = -1;
    s32Ret = TS_MPI_VENC_StartRecvFrame(VencChn,&stRecvParam);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_StartRecvPic faild with%#x! \n", s32Ret);
        return TS_FAILURE;
    }
    return TS_SUCCESS;
}

/******************************************************************************
* funciton : Start venc stream mode
* note      : rate control parameter need adjust, according your case.
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_Start(VENC_CHN VencChn, PAYLOAD_TYPE_E enType,  PIC_SIZE_E enSize, TS_U32 u32FrameRate, SAMPLE_RC_E enRcMode, TS_U32  u32Profile, VENC_GOP_ATTR_S *pstGopAttr)
{
    TS_S32 s32Ret;
    VENC_RECV_PIC_PARAM_S  stRecvParam;

    /******************************************
     step 1:  Creat Encode Chnl
    ******************************************/
    s32Ret = SAMPLE_COMM_VENC_Creat(VencChn,enType,enSize,u32FrameRate, enRcMode,u32Profile,pstGopAttr);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VENC_Creat faild with: %#x! \n", s32Ret);
        return TS_FAILURE;
    }
    /******************************************
     step 2:  Start Recv Venc Pictures
    ******************************************/
    stRecvParam.s32RecvPicNum = -1;
    s32Ret = TS_MPI_VENC_StartRecvFrame(VencChn,&stRecvParam);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_StartRecvPic faild with%#x! \n", s32Ret);
        return TS_FAILURE;
    }
    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VENC_SetSmartEnable(VENC_CHN VencChn, TS_U8 u8Enable)
{
    TS_S32 s32Ret;
    VENC_RECV_PIC_PARAM_S  stRecvParam;

	s32Ret = TS_MPI_VENC_StopRecvFrame(VencChn);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_StopRecvFrame faild with%#x! \n", s32Ret);
        return TS_FAILURE;
    }

	s32Ret = TS_MPI_VENC_SetSmartEnable(VencChn,u8Enable);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_SetSmartEnable faild with%#x! \n", s32Ret);
        return TS_FAILURE;
    }

	s32Ret = TS_MPI_VENC_GetSmartEnable(VencChn,&u8Enable);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_GetSmartEnable faild with%#x! \n", s32Ret);
        return TS_FAILURE;
    }
	else
	{
		SAMPLE_PRT("TS_MPI_VENC_GetSmartEnable enable=%d! \n", u8Enable);
	}

    /******************************************
     step 2:  Start Recv Venc Pictures
    ******************************************/
    stRecvParam.s32RecvPicNum = -1;
    s32Ret = TS_MPI_VENC_StartRecvFrame(VencChn,&stRecvParam);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_StartRecvPic faild with%#x! \n", s32Ret);
        return TS_FAILURE;
    }
    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VENC_GetSmartEnable(VENC_CHN VencChn, TS_U8 *u8Enable)
{
    TS_S32 s32Ret;

	s32Ret = TS_MPI_VENC_GetSmartEnable(VencChn,u8Enable);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_GetSmartEnable faild with%#x! \n", s32Ret);
        return TS_FAILURE;
    }
	else
	{
		SAMPLE_PRT("TS_MPI_VENC_GetSmartEnable enable=%d! \n", *u8Enable);
	}

    return TS_SUCCESS;
}


TS_S32 SAMPLE_COMM_VENC_GetRecvNum(VENC_CHN VencChn){
    return gs_stPara.u32GetStreamCnt[VencChn];
}


// 根据属性获取GOP和帧率
// 函数返回值：0 表示成功，-1 表示失败
static int SAMPLE_GetGopFrameRateAndBitrate(const VENC_CHN_ATTR_S *chnAttr, TS_U32 *gop, TS_FR32 *dstFrameRate, TS_U32 *bitrate) {

    if (chnAttr == NULL || gop == NULL || dstFrameRate == NULL || bitrate == NULL) {
        printf("Error: Invalid input parameters\n");
        return -1;
    }

    // 提取 GOP 值
    switch (chnAttr->stRcAttr.enRcMode) {
        case VENC_RC_MODE_H264CBR:
            *gop = chnAttr->stRcAttr.stH264Cbr.u32Gop;
            *dstFrameRate = chnAttr->stRcAttr.stH264Cbr.fr32DstFrameRate;
            *bitrate = chnAttr->stRcAttr.stH264Cbr.u32BitRate; // 目标码率
            break;
        case VENC_RC_MODE_H264VBR:
            *gop = chnAttr->stRcAttr.stH264Vbr.u32Gop;
            *dstFrameRate = chnAttr->stRcAttr.stH264Vbr.fr32DstFrameRate;
            *bitrate = chnAttr->stRcAttr.stH264Vbr.u32MaxBitRate; // 最大码率
            break;
        case VENC_RC_MODE_H264AVBR:
            *gop = chnAttr->stRcAttr.stH264AVbr.u32Gop;
            *dstFrameRate = chnAttr->stRcAttr.stH264AVbr.fr32DstFrameRate;
            *bitrate = chnAttr->stRcAttr.stH264AVbr.u32MaxBitRate; // 最大码率
            break;
        case VENC_RC_MODE_H264QVBR:
            *gop = chnAttr->stRcAttr.stH264QVbr.u32Gop;
            *dstFrameRate = chnAttr->stRcAttr.stH264QVbr.fr32DstFrameRate;
            *bitrate = chnAttr->stRcAttr.stH264QVbr.u32TargetBitRate; // 目标码率
            break;
        case VENC_RC_MODE_H264CVBR:
            *gop = chnAttr->stRcAttr.stH264CVbr.u32Gop;
            *dstFrameRate = chnAttr->stRcAttr.stH264CVbr.fr32DstFrameRate;
            *bitrate = chnAttr->stRcAttr.stH264CVbr.u32MaxBitRate; // 最大码率
            break;
        case VENC_RC_MODE_H264FIXQP:
            *gop = chnAttr->stRcAttr.stH264FixQp.u32Gop;
            *dstFrameRate = chnAttr->stRcAttr.stH264FixQp.fr32DstFrameRate;
            *bitrate = 0; // FixQP 模式没有码率控制
            break;
        default:
            printf("Error: Unsupported RC mode\n");
            return -1;
    }

    return 0; // 成功
}



TS_S32 SAMPLE_COMM_VENC_SetSmartEncodeType(VENC_CHN VencChn, TS_S32 s32Type)
{
    TS_S32 s32IsNeedSet = 1;
    TS_U32 S32Gop = 0;
    TS_U32 S32Bitrate = 0;
    TS_FR32 fr32DstFrameRate = 0;

    VENC_SMART_ENC_PARA_S param = {0};
    VENC_CHN_ATTR_S chnAttr = {0};

    CHECK_RET_GOTO(TS_MPI_VENC_GetSmartEncodeParam(VencChn, &param), bail);
    CHECK_RET_GOTO(TS_MPI_VENC_GetChnAttr(VencChn, &chnAttr), bail);

    CHECK_RET_GOTO(SAMPLE_GetGopFrameRateAndBitrate(&chnAttr, &S32Gop, &fr32DstFrameRate, &S32Bitrate), bail);

    switch (s32Type)
    {
    case SMART_ENCODE_TYPE_DIANXIN:
        param.u32Valid = 1;
        param.u32SmartGopSize = 3 * fr32DstFrameRate;
        param.stRateCfg.u32Rate4Complex = S32Bitrate * 0.7;
        param.stRateCfg.u32Rate4Medium = S32Bitrate * 0.6;
        param.stRateCfg.u32Rate4Simple = S32Bitrate * 0.3;
        break;
    case SMART_ENCODE_TYPE_YIDONG:
        param.u32Valid = 1;
        param.u32SmartGopSize = 3 * fr32DstFrameRate;
        break;
    case SMART_ENCODE_TYPE_DEFAULT: // 不需要设置
    default:
        s32IsNeedSet = 0;
        break;
    }

    if(s32IsNeedSet){
        CHECK_RET_GOTO(TS_MPI_VENC_SetSmartEncodeParam(VencChn, &param), bail);
    }

    return TS_SUCCESS;
bail:
    return TS_FAILURE;
}

/******************************************************************************
* funciton : Stop venc ( stream mode -- H264, MJPEG )
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_Stop(VENC_CHN VencChn)
{
    TS_S32 s32Ret;
    /******************************************
     step 1:  Stop Recv Pictures
    ******************************************/
    s32Ret = TS_MPI_VENC_StopRecvFrame(VencChn);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_StopRecvFrame VencChn[%d] failed with %#x!\n", VencChn, s32Ret);
        return TS_FAILURE;
    }
	SAMPLE_PRT("TS_MPI_VENC_StopRecvFrame VencChn[%d] success!\n", VencChn);

	/******************************************
     step 2:  Distroy Venc Channel
    ******************************************/
    s32Ret = TS_MPI_VENC_DestroyChn(VencChn);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_DestroyChn VencChn[%d] failed with %#x!\n", VencChn, s32Ret);
        return TS_FAILURE;
    }
	SAMPLE_PRT("TS_MPI_VENC_DestroyChn VencChn[%d] success!\n", VencChn);
    return TS_SUCCESS;
}

/******************************************************************************
* funciton : Start snap
******************************************************************************/

TS_S32 SAMPLE_COMM_VENC_SnapStart(VENC_CHN VencChn, SIZE_S* pstSize, TS_BOOL bSupportDCF)
{
    TS_S32 s32Ret;
    VENC_CHN_ATTR_S stVencChnAttr = {0};
    /******************************************
     step 1:  Create Venc Channel
    ******************************************/
    stVencChnAttr.stVencAttr.enType = PT_JPEG;
    stVencChnAttr.stVencAttr.u32Profile         = 0;
    stVencChnAttr.stVencAttr.u32MaxPicWidth     = pstSize->u32Width;
    stVencChnAttr.stVencAttr.u32MaxPicHeight    = pstSize->u32Height;
    stVencChnAttr.stVencAttr.u32PicWidth        = pstSize->u32Width;
    stVencChnAttr.stVencAttr.u32PicHeight       = pstSize->u32Height;
    stVencChnAttr.stVencAttr.u32BufSize         = pstSize->u32Width * pstSize->u32Height * 2;
    stVencChnAttr.stVencAttr.bByFrame           = TS_TRUE;/*get stream mode is field mode  or frame mode*/
    stVencChnAttr.stVencAttr.stAttrJpege.bSupportDCF = bSupportDCF;
    //stVencChnAttr.stVencAttr.stAttrJpege.bSupportXMP = TS_FALSE;
    stVencChnAttr.stVencAttr.stAttrJpege.stMPFCfg.u8LargeThumbNailNum = 0;
    stVencChnAttr.stVencAttr.stAttrJpege.enReceiveMode                = VENC_PIC_RECEIVE_SINGLE;

    s32Ret = TS_MPI_VENC_CreateChn(VencChn, &stVencChnAttr);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_CreateChn [%d] faild with %#x!\n", \
                   VencChn, s32Ret);
        return s32Ret;
    }
    return TS_SUCCESS;
}


/******************************************************************************
* funciton : Stop snap
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_SnapStop(VENC_CHN VencChn)
{
    TS_S32 s32Ret;
    s32Ret = TS_MPI_VENC_StopRecvFrame(VencChn);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_StopRecvPic vechn[%d] failed with %#x!\n", VencChn, s32Ret);
        return TS_FAILURE;
    }
    s32Ret = TS_MPI_VENC_DestroyChn(VencChn);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_DestroyChn vechn[%d] failed with %#x!\n", VencChn, s32Ret);
        return TS_FAILURE;
    }
    return TS_SUCCESS;
}


TS_S32 SAMPLE_COMM_VENC_SnapProcess(VENC_CHN VencChn, TS_U32 SnapCnt, TS_BOOL bSaveJpg, TS_BOOL bSaveThm)
{
    VENC_STREAM_S stStream;
    TS_S32 s32Ret;
    VENC_RECV_PIC_PARAM_S  stRecvParam;
    TS_U32 i;

    /******************************************
     step 1:  Start Recv Venc Pictures
    ******************************************/
    stRecvParam.s32RecvPicNum = SnapCnt;
    s32Ret = TS_MPI_VENC_StartRecvFrame(VencChn, &stRecvParam);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_StartRecvPic faild with%#x!\n", s32Ret);
        return TS_FAILURE;
    }


    /******************************************
     step 2:  recv picture
    ******************************************/
    for(i=0; i<SnapCnt; i++)
    {

        stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S));
        if (NULL == stStream.pstPack)
        {
            SAMPLE_PRT("malloc memory failed!\n");
            return TS_FAILURE;
        }

        stStream.u32PackCount = 1;
        s32Ret = TS_MPI_VENC_GetStream(VencChn, &stStream, -1);
        if (TS_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("TS_MPI_VENC_GetStream failed with %#x!\n", s32Ret);

            free(stStream.pstPack);
            stStream.pstPack = NULL;
            return TS_FAILURE;
        }
        if(bSaveJpg || bSaveThm)
        {
            char acFile[256]    = {0};
            FILE* pFile;

            snprintf(acFile, 256, "%s/snap_%d.jpg",g_venc_save_path, gs_s32SnapCnt);
            pFile = fopen(acFile, "wb");
            if (pFile == NULL)
            {
                SAMPLE_PRT("open file err\n");

                free(stStream.pstPack);
                stStream.pstPack = NULL;
                return TS_FAILURE;
            }

            s32Ret = SAMPLE_COMM_VENC_SaveStream(pFile, &stStream);
            if (TS_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("save snap picture failed!\n");

                free(stStream.pstPack);
                stStream.pstPack = NULL;

                fclose(pFile);
                return TS_FAILURE;
            }

            fclose(pFile);
            gs_s32SnapCnt++;
        }

        s32Ret = TS_MPI_VENC_ReleaseStream(VencChn, &stStream);
        if (TS_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("TS_MPI_VENC_ReleaseStream failed with %#x!\n", s32Ret);

            free(stStream.pstPack);
            stStream.pstPack = NULL;

            return TS_FAILURE;
        }

        free(stStream.pstPack);
        stStream.pstPack = NULL;

    }
    /******************************************
     step 4:  stop recv picture
    ******************************************/
    s32Ret = TS_MPI_VENC_StopRecvFrame(VencChn);
    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("TS_MPI_VENC_StopRecvPic failed with %#x!\n",  s32Ret);
        return TS_FAILURE;
    }
    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VENC_SnapProcessUnBind(VENC_CHN VencChn, TS_U32 SnapCnt, TS_BOOL bSaveJpg, TS_BOOL bSaveThm)
{
    VENC_STREAM_S stStream;
    TS_S32 s32Ret;
    TS_U32 i;

    /******************************************
     step 1:  recv picture
    ******************************************/
    for(i=0; i<SnapCnt; i++)
    {

        stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S));
        if (NULL == stStream.pstPack)
        {
            SAMPLE_PRT("malloc memory failed!\n");
            return TS_FAILURE;
        }

        stStream.u32PackCount = 1;
        s32Ret = TS_MPI_VENC_GetStream(VencChn, &stStream, -1);
        if (TS_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("TS_MPI_VENC_GetStream failed with %#x!\n", s32Ret);

            free(stStream.pstPack);
            stStream.pstPack = NULL;
            return TS_FAILURE;
        }
        if(bSaveJpg || bSaveThm)
        {
            char acFile[FILE_NAME_LEN]    = {0};
            FILE* pFile;

            snprintf(acFile, FILE_NAME_LEN, "snap_%d.jpg", gs_s32SnapCnt);
            pFile = fopen(acFile, "wb");
            if (pFile == NULL)
            {
                SAMPLE_PRT("open file err\n");

                free(stStream.pstPack);
                stStream.pstPack = NULL;
                return TS_FAILURE;
            }

            s32Ret = SAMPLE_COMM_VENC_SaveStream(pFile, &stStream);
            if (TS_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("save snap picture failed!\n");

                free(stStream.pstPack);
                stStream.pstPack = NULL;

                fclose(pFile);
                return TS_FAILURE;
            }

            fclose(pFile);
            gs_s32SnapCnt++;
        }

        s32Ret = TS_MPI_VENC_ReleaseStream(VencChn, &stStream);
        if (TS_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("TS_MPI_VENC_ReleaseStream failed with %#x!\n", s32Ret);

            free(stStream.pstPack);
            stStream.pstPack = NULL;

            return TS_FAILURE;
        }

        free(stStream.pstPack);
        stStream.pstPack = NULL;

    }
    /******************************************
     step 2:  stop recv picture
    ******************************************/
    s32Ret = TS_MPI_VENC_StopRecvFrame(VencChn);
    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("TS_MPI_VENC_StopRecvPic failed with %#x!\n",  s32Ret);
        return TS_FAILURE;
    }
    return TS_SUCCESS;
}


#define CAL_INTERVAL_MS(tv,tv1) ((tv1.tv_sec-tv.tv_sec)*1000+(tv1.tv_usec- tv.tv_usec)/1000)
#define CAL_INTERVAL_US(tv,tv1) ((tv1.tv_sec-tv.tv_sec)*1000000+(tv1.tv_usec- tv.tv_usec)/1)

TS_S32 SAMPLE_COMM_VENC_Manual_AREAJPEG(
	VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn,
	TS_U32 x, TS_U32 y, TS_U32 w, TS_U32 h)
{
	TS_S32 s32Ret = TS_FAILURE;
	VIDEO_FRAME_INFO_S stVideoFrame = {0};
	VIDEO_FRAME_INFO_S *pstVideoFrame = &stVideoFrame;
    VENC_STREAM_S stStream = {0};
	int pack_cnt = 1;
    TS_CHAR aszFileName[256];
	static int call_cnt = 0;
    FILE *pFile = NULL;
	struct timeval tv,tv1;
	struct timezone tz,tz1;
	VENC_CHN_PARAM_S chn_param = {0};

	s32Ret = TS_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, pstVideoFrame, 1000);
	if(TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_VPSS_GetChnFrame err:0x%x\n",s32Ret);
		return s32Ret;
	}

	do {

		memset(&chn_param, 0, sizeof(chn_param));
		chn_param.stCropCfg.bEnable = 1;
		chn_param.stCropCfg.stRect.s32X = x;
		chn_param.stCropCfg.stRect.s32Y = y;
		chn_param.stCropCfg.stRect.u32Width = w;
		chn_param.stCropCfg.stRect.u32Height = h;
		s32Ret = TS_MPI_VENC_SetChnParam(VencChn, &chn_param);
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("TS_MPI_VENC_SetChnParam error:%x\n", s32Ret);
		} else {
			SAMPLE_PRT("TS_MPI_VENC_SetChnParam OK.\n");
		}

		memset(&stStream, 0, sizeof(stStream));
		stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * pack_cnt);
		if (NULL == stStream.pstPack) {
			SAMPLE_PRT("malloc stream pack failed!\n");
			s32Ret = TS_FAILURE;
			break;
		}

		SAMPLE_PRT("BF TS_MPI_VENC_SendFrame\n");
		gettimeofday(&tv, &tz);
		s32Ret = TS_MPI_VENC_SendFrame(VencChn, pstVideoFrame,-1);
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("TS_MPI_VENC_SendFrame err:0x%x\n",s32Ret);
		} else {
			SAMPLE_PRT("TS_MPI_VENC_SendFrame finish\n");
		}

		stStream.u32PackCount = pack_cnt;
		s32Ret = TS_MPI_VENC_GetStream(VencChn, &stStream, 500);
		if (TS_SUCCESS != s32Ret) {
			free(stStream.pstPack);
			stStream.pstPack = NULL;
			SAMPLE_PRT("TS_MPI_VENC_GetStream failed with %#x!\n", s32Ret);
			break;
		} else {
			gettimeofday(&tv1, &tz1);
			SAMPLE_PRT("TS_MPI_VENC_GetStream OK\n");
			SAMPLE_PRT("\nArea Jpeg %dx%d\nTS_MPI_VENC_SendFrame --> TS_MPI_VENC_GetStream:\nConsume %ld ms (%ld us)\n\n",
				w, h, CAL_INTERVAL_MS(tv,tv1), CAL_INTERVAL_US(tv,tv1));

			SAMPLE_PRT("viraddr %p, offset %d, len %d\n", stStream.pstPack[0].pu8Addr,
				stStream.pstPack[0].u32Offset, stStream.pstPack[0].u32Len);

			memset(aszFileName, 0 , sizeof(aszFileName));
			snprintf(aszFileName, sizeof(aszFileName), "./areajpeg_%d.jpg", call_cnt++);
			pFile = fopen(aszFileName, "wb");
			if (!pFile) {
				free(stStream.pstPack);
				stStream.pstPack = NULL;
				SAMPLE_PRT("open file err!\n");
				s32Ret = TS_FAILURE;
				break;
			}

			s32Ret = SAMPLE_COMM_VENC_SaveStream(pFile, &stStream);
			if (TS_SUCCESS != s32Ret) {
				SAMPLE_PRT("save stream failed!\n");
			} else {
				SAMPLE_PRT("save stream OK.\n");
			}

			s32Ret = TS_MPI_VENC_ReleaseStream(VencChn, &stStream);
			if (TS_SUCCESS != s32Ret) {
				SAMPLE_PRT("TS_MPI_VENC_ReleaseStream failed!\n");
			}

			free(stStream.pstPack);
			stStream.pstPack = NULL;

			if(fclose(pFile)) {
				perror("");
			}
		}
	}while(0);

	s32Ret = TS_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, pstVideoFrame);
	if(TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_VPSS_ReleaseChnFrame err:0x%x\n",s32Ret);
	}

	return s32Ret;
}


TS_S32 SAMPLE_COMM_VENC_SaveJpeg(VENC_CHN VencChn, TS_U32 SnapCnt)
{
    struct timeval TimeoutVal;
    fd_set read_fds;
    TS_S32 s32VencFd;
    VENC_CHN_STATUS_S stStat;
    VENC_STREAM_S stStream;
    TS_S32 s32Ret;
    TS_U32 i;

    s32VencFd = TS_MPI_VENC_GetFd(VencChn);
    if (s32VencFd < 0)
    {
        SAMPLE_PRT("TS_MPI_VENC_GetFd faild with%#x!\n", s32VencFd);
        return TS_FAILURE;
    }

    for(i=0; i<SnapCnt; i++)
    {
        FD_ZERO(&read_fds);
        FD_SET(s32VencFd, &read_fds);
        TimeoutVal.tv_sec  = 10;
        TimeoutVal.tv_usec = 0;
        s32Ret = select(s32VencFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("snap select failed!\n");
            return TS_FAILURE;
        }
        else if (0 == s32Ret)
        {
            SAMPLE_PRT("snap time out!\n");
            return TS_FAILURE;
        }
        else
        {
            if (FD_ISSET(s32VencFd, &read_fds))
            {
                s32Ret = TS_MPI_VENC_QueryStatus(VencChn, &stStat);
                if (s32Ret != TS_SUCCESS)
                {
                    SAMPLE_PRT("TS_MPI_VENC_QueryStatus failed with %#x!\n", s32Ret);
                    return TS_FAILURE;
                }
                /*******************************************************
                suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
                 if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
                 {                SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                    return TS_SUCCESS;
                 }
                 *******************************************************/
                if (0 == stStat.u32CurPacks)
                {
                    SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                    return TS_SUCCESS;
                }
                stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
                if (NULL == stStream.pstPack)
                {
                    SAMPLE_PRT("malloc memory failed!\n");
                    return TS_FAILURE;
                }
                stStream.u32PackCount = stStat.u32CurPacks;
                s32Ret = TS_MPI_VENC_GetStream(VencChn, &stStream, -1);
                if (TS_SUCCESS != s32Ret)
                {
                    SAMPLE_PRT("TS_MPI_VENC_GetStream failed with %#x!\n", s32Ret);

                    free(stStream.pstPack);
                    stStream.pstPack = NULL;
                    return TS_FAILURE;
                }
                if(1)
                {
                    char acFile[FILE_NAME_LEN]    = {0};
                    FILE* pFile;

                    snprintf(acFile, FILE_NAME_LEN, "snap_%d.jpg", gs_s32SnapCnt);
                    pFile = fopen(acFile, "wb");
                    if (pFile == NULL)
                    {
                        SAMPLE_PRT("open file err\n");

                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        return TS_FAILURE;
                    }

                    s32Ret = SAMPLE_COMM_VENC_SaveStream(pFile, &stStream);
                    if (TS_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("save snap picture failed!\n");

                        free(stStream.pstPack);
                        stStream.pstPack = NULL;

                        fclose(pFile);
                        return TS_FAILURE;
                    }

                    fclose(pFile);
                    gs_s32SnapCnt++;
                }

                s32Ret = TS_MPI_VENC_ReleaseStream(VencChn, &stStream);
                if (TS_SUCCESS != s32Ret)
                {
                    SAMPLE_PRT("TS_MPI_VENC_ReleaseStream failed with %#x!\n", s32Ret);

                    free(stStream.pstPack);
                    stStream.pstPack = NULL;

                    return TS_FAILURE;
                }

                free(stStream.pstPack);
                stStream.pstPack = NULL;
            }
        }
    }

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VENC_SendFrame(VENC_CHN VencChn, VIDEO_FRAME_INFO_S * pFrame, TS_S32 s32MilliSec)
{
	return TS_MPI_VENC_SendFrame(VencChn, pFrame, s32MilliSec);
}

TS_VOID SAMPLE_COMM_VENC_GetTestSavePath(TS_VOID)
{
	if(g_venc_save_path[0]){
		return;
	}

	FILE *fp = fopen("/tmp/save_path.txt", "r");
	if (NULL == fp)
	{
		goto exit;
	}
	fgets(g_venc_save_path, sizeof(g_venc_save_path) - 1, fp);
	g_venc_save_path[strlen(g_venc_save_path)-1] = 0;
	fclose(fp);
	return;
exit:
	sprintf(g_venc_save_path, "/tmp");
    return;
}

TS_VOID SAMPLE_COMM_VENC_SetTestSavePath(TS_CHAR * pFilePath)
{
	sprintf(g_venc_save_path, pFilePath);
    return;
}

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr)[0])//without check __must_be_array(arr)
#endif

#define UPDATE_COUNT_INTERVAL (30*10)
#define REFRESH_TARGET_RATE (4.0f) //per second
#define DIFF_RATE (0.5f)

static char angle_loop[] = {'|','/','-','\\','|','/','-','\\'};

void update_refresh_interval(int *interval)
{
	static unsigned int count = 0;
	static time_t last = 0;
	static time_t now;
	float real_call_rate;

	if (0 == count % UPDATE_COUNT_INTERVAL) {
		now = time(NULL);
		real_call_rate = ((float)UPDATE_COUNT_INTERVAL) / ((float)(now - last));

		(*interval) = (int)(real_call_rate / REFRESH_TARGET_RATE);

		*interval = (*interval <= 0) ? 1 : (*interval);

//		printf("now=%ld, last=%ld, real_call_rate=%0.2f, interval=%d\n", now, last, real_call_rate,*interval);

		last = now;
	}
	count++;

	return;
}

void show_running_fan(void)
{
	static int REFRESH_INTERVAL = 1;
	static int tick = 0;
	static int idx = 0;
	if (0 == tick % REFRESH_INTERVAL) {
		printf("\r Running <%c>", angle_loop[idx]);
		fflush(stdout);
		idx++;
		idx = (idx >= ARRAY_SIZE(angle_loop)) ? 0 : idx;
	}
	tick++;
	update_refresh_interval(&REFRESH_INTERVAL);
	return;
}

typedef void *(*on_stream_callback_t)(TS_S32 s32Chn, VENC_STREAM_S *pstStream);
on_stream_callback_t g_on_stream_callback = NULL;

/******************************************************************************
* funciton : get stream from each channels and save them
******************************************************************************/
TS_VOID* SAMPLE_COMM_VENC_GetVencStreamProcV2(TS_VOID* p)
{
    TS_S32 i;
    TS_S32 s32ChnTotal;
    VENC_CHN_ATTR_S stVencChnAttr;
    SAMPLE_VENC_GETSTREAM_PARA_S* pstPara;
    TS_S32 maxfd = 0;
    struct timeval TimeoutVal;
    fd_set read_fds;
    TS_U32 u32PictureCnt[VENC_MAX_CHN_NUM]={0};
    TS_S32 VencFd[VENC_MAX_CHN_NUM];
    TS_CHAR aszFileName[VENC_MAX_CHN_NUM][256];
    FILE* pFile[VENC_MAX_CHN_NUM];
    char szFilePostfix[10];
    VENC_CHN_STATUS_S stStat;
    VENC_STREAM_S stStream;
    TS_S32 s32Ret;
    VENC_CHN VencChn;
    PAYLOAD_TYPE_E enPayLoadType[VENC_MAX_CHN_NUM];
	RTSP_STREAM_TYPE_E enRtspStreamType[VENC_MAX_CHN_NUM];
	TS_S32 rtsp_session_num = 0;
	SAMPLE_VENC_CONSUME_TYPE consume_type;
	//VENC_CHN_ATTR_S chn_attr = {0};
    int streamGetCount[4] = {0,0,0,0};
#ifdef TARGET_SDK_MODE_FASTBOOT
    static TS_U32 first_frame = 0;
#endif

    prctl(PR_SET_NAME, "GetVencStream", 0,0,0);

    pstPara = (SAMPLE_VENC_GETSTREAM_PARA_S*)p;
	if (!pstPara)
		return NULL;
	consume_type = pstPara->consume_type;
    s32ChnTotal = pstPara->s32Cnt;

	SAMPLE_COMM_VENC_GetTestSavePath();
	SAMPLE_PRT("total chnl=%d, venc save path:[%s]\n", s32ChnTotal, g_venc_save_path);

    /******************************************
     step 1:  check & prepare save-file & venc-fd
    ******************************************/
    if (s32ChnTotal >= VENC_MAX_CHN_NUM)
    {
        SAMPLE_PRT("input count invaild\n");
        return NULL;
    }
    for (i = 0; i < s32ChnTotal; i++)
    {
        /* decide the stream file name, and open file to save stream */
        VencChn = pstPara->VeChn[i];
        s32Ret = TS_MPI_VENC_GetChnAttr(VencChn, &stVencChnAttr);
        if (s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("TS_MPI_VENC_GetChnAttr chn[%d] failed with %#x!\n", \
                       VencChn, s32Ret);
            return NULL;
        }
        enPayLoadType[i] = stVencChnAttr.stVencAttr.enType;
		if(PT_H264 == stVencChnAttr.stVencAttr.enType){
			enRtspStreamType[i] = RTSP_STREAM_TYPE_H264;
			rtsp_session_num ++;
		}
		else if(PT_H265 == stVencChnAttr.stVencAttr.enType){
			enRtspStreamType[i] = RTSP_STREAM_TYPE_H265;
			rtsp_session_num ++;
		}

        s32Ret = SAMPLE_COMM_VENC_GetFilePostfix(enPayLoadType[i], szFilePostfix);
        if (s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_COMM_VENC_GetFilePostfix [%d] failed with %#x!\n", \
                       stVencChnAttr.stVencAttr.enType, s32Ret);
            return NULL;
        }
        if(!(PT_JPEG == enPayLoadType[i] || PT_MJPEG == enPayLoadType[i]))
        {
			if (consume_type == SAMPLE_VENC_FILE || consume_type == SAMPLE_VENC_BOTH) {
				snprintf(aszFileName[i], 256, "%s/stream_chn%d%s", g_venc_save_path, i, szFilePostfix);
	            pFile[i] = fopen(aszFileName[i], "wb");
	            if (!pFile[i])
	            {
	                SAMPLE_PRT("open file[%s] failed!\n", aszFileName[i]);
	                return NULL;
	            }
			}
        }

        /* Set Venc Fd. */
        VencFd[i] = TS_MPI_VENC_GetFd(i);
        if (VencFd[i] < 0)
        {
            SAMPLE_PRT("TS_MPI_VENC_GetFd failed with %#x!\n", VencFd[i]);
            return NULL;
        }

        if (maxfd <= VencFd[i])
        {
            maxfd = VencFd[i];
        }
    }
	if (consume_type == SAMPLE_VENC_RTSP || consume_type == SAMPLE_VENC_BOTH){
		if(rtsp_session_num > 0){
			gb_rtsp_server_start(rtsp_session_num, enRtspStreamType);
		}
	}

	pstPara->bThreadStart = TS_TRUE;

    /******************************************
     step 2:  Start to get streams of each channel.
    ******************************************/
    while (TS_TRUE == pstPara->bThreadStart)
    {
#ifdef IMPR_PRINT
		show_running_fan();
#endif
        FD_ZERO(&read_fds);
        for (i = 0; i < s32ChnTotal; i++)
        {
            FD_SET(VencFd[i], &read_fds);
        }

        TimeoutVal.tv_sec  = 0;
        TimeoutVal.tv_usec = 4000 * 1000;//15fps, 66.7ms per frame

        s32Ret = TS_MPI_VENC_SELECT(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("select failed!\n");
            break;
        }
        else if (s32Ret == 0)
        {
            SAMPLE_PRT("get venc stream time out, try again\n");
            //check_fifoflow_and_reset();
            continue;
        }
        else
        {
            if (s32ChnTotal > 2)
            {
                if (abs(streamGetCount[0] - streamGetCount[2]) > 100)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        streamGetCount[j] = 0;
                    }
                }
                if (streamGetCount[0] > 1000 || streamGetCount[2] > 1000)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        streamGetCount[j] = 0;
                    }
                }
            }
            for (i = 0; i < s32ChnTotal; i++)
            {
                if (FD_ISSET(VencFd[i], &read_fds))
                {
                    /*******************************************************
                     step 2.1 : query how many packs in one-frame stream.
                    *******************************************************/
                    memset(&stStream, 0, sizeof(stStream));

                    s32Ret = TS_MPI_VENC_QueryStatus(i, &stStat);
                    if (TS_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("TS_MPI_VENC_QueryStatus chn[%d] failed with %#x!\n", i, s32Ret);
                        break;
                    }

                    /*******************************************************
                    step 2.2 :suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
                     if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
                     {
                        SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                        continue;
                     }
                    *******************************************************/
                    if(0 == stStat.u32CurPacks)
                    {
                          SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                          continue;
                    }
                    /*******************************************************
                     step 2.3 : malloc corresponding number of pack nodes.
                    *******************************************************/
                    stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
                    if (NULL == stStream.pstPack)
                    {
                        SAMPLE_PRT("malloc stream pack failed!\n");
                        break;
                    }

                    /*******************************************************
                     step 2.4 : call mpi to get one-frame stream
                    *******************************************************/
                    stStream.u32PackCount = stStat.u32CurPacks;
                    s32Ret = TS_MPI_VENC_GetStream(i, &stStream, TS_TRUE);
                    if (TS_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        SAMPLE_PRT("TS_MPI_VENC_GetStream failed with %#x!\n", \
                                   s32Ret);
                        break;
                    }
#ifdef TARGET_SDK_MODE_FASTBOOT
                    if (0 == first_frame) {
                        printf("^^^^^^^^^^^^^^^^^^got first frame!^^^^^^^^^^^^^^^^^^\n");
                        first_frame = 1;
                    }
#endif
					//SAMPLE_PRT("packCount=%d, type=%d, wh=[%d,%d]\n", stStream.u32PackCount,
					//	stStream.pstPack->DataType.enH265EType,
					//	stStream.u32Width, stStream.u32Height);

                     //static int cunt=1;
                    // char time_start[40];
                    // get_time_ms(time_start, sizeof(time_start));
                   // printf("TS_MPI_VENC_GetStream cunt=%d,%s\n", cunt++, time_start);

                    streamGetCount[i]++;
                    gs_stPara.u32GetStreamCnt[i]++;
                    /*******************************************************
                     step 2.5 : save frame to file
                    *******************************************************/
                    if (PT_JPEG == enPayLoadType[i] || PT_MJPEG == enPayLoadType[i])
                    {
						if (consume_type == SAMPLE_VENC_FILE || consume_type == SAMPLE_VENC_BOTH) {
							snprintf(aszFileName[i], 256, "%s/stream_chn%d_%d%s", g_venc_save_path, i, u32PictureCnt[i],szFilePostfix);

	                        pFile[i] = fopen(aszFileName[i], "wb");
	                        if (!pFile[i])
	                        {
	                            SAMPLE_PRT("open file err!\n");
	                            return NULL;
	                        }
                            SAMPLE_PRT("open file %s success!\n", aszFileName[i]);
						}
                    }

					if (consume_type == SAMPLE_VENC_FILE || consume_type == SAMPLE_VENC_BOTH) {
						s32Ret = SAMPLE_COMM_VENC_SaveStream(pFile[i], &stStream);
					}

					if (consume_type == SAMPLE_VENC_RTSP || consume_type == SAMPLE_VENC_BOTH) {
						ts_s32 frame_type = -1;

						if(RTSP_STREAM_TYPE_H264 == enRtspStreamType[i]){
							if(stStream.pstPack[0].DataType.enH264EType == H264E_NALU_IDRSLICE
								|| stStream.pstPack[0].DataType.enH264EType == H264E_NALU_ISLICE){
								frame_type = 1;
							}
							else{
								frame_type = 2;
							}
						}
						else if(RTSP_STREAM_TYPE_H265 == enRtspStreamType[i]){
							if(stStream.pstPack[0].DataType.enH265EType == H265E_NALU_IDRSLICE
								|| stStream.pstPack[0].DataType.enH265EType == H265E_NALU_ISLICE){
								frame_type = 1;
							}
							else{
								frame_type = 2;
							}
						}
						TS_SAMPLE_VencPackageStream(&stStream, frame_type, i, wrFd);
					}

					if(g_on_stream_callback){
                        g_on_stream_callback(i, &stStream);
                    }

                    if (TS_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        SAMPLE_PRT("save stream failed!\n");
                        break;
                    }
                    /*******************************************************
                     step 2.6 : release stream
                     *******************************************************/
                    s32Ret = TS_MPI_VENC_ReleaseStream(i, &stStream);
                    if (TS_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("TS_MPI_VENC_ReleaseStream failed!\n");
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        break;
                    }

                    /*******************************************************
                     step 2.7 : free pack nodes
                    *******************************************************/
                    free(stStream.pstPack);
                    stStream.pstPack = NULL;
                    u32PictureCnt[i]++;
					if(u32PictureCnt[i] % 100 == 0){
						SAMPLE_PRT("venc stream[%d] recv %d frames\n", i, u32PictureCnt[i]);
					}
                    if(PT_JPEG == enPayLoadType[i] || PT_MJPEG == enPayLoadType[i])
                    {
						if (consume_type == SAMPLE_VENC_FILE || consume_type == SAMPLE_VENC_BOTH) {
                        	fclose(pFile[i]);
						}
                    }
                }
            }
        }
    }
    /*******************************************************
    * step 3 : close save-file
    *******************************************************/
    for (i = 0; i < s32ChnTotal; i++)
    {
        if(PT_JPEG != enPayLoadType[i] && PT_MJPEG != enPayLoadType[i])
        {
			if (consume_type == SAMPLE_VENC_FILE || consume_type == SAMPLE_VENC_BOTH) {
				if(pFile[i] != NULL) {
					 fclose(pFile[i]);
                     pFile[i] = NULL;
				}
			}
        }
    }
	if (consume_type == SAMPLE_VENC_RTSP || consume_type == SAMPLE_VENC_BOTH) {
//		ProconVideoClose(wrFd);
		wrFd = 0;
		gb_rtsp_server_stop();
		SAMPLE_PRT("ProconVideoClose close!\n");
	}
    return NULL;
}

/******************************************************************************
* funciton : start get venc stream process thread
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_StartGetStreamV2(VENC_CHN VeChn[],TS_S32 s32Cnt, SAMPLE_VENC_CONSUME_TYPE type)
{
    TS_U32 i;

    gs_stPara.bThreadStart = TS_TRUE;
    gs_stPara.s32Cnt = s32Cnt;
    for(i=0; i<s32Cnt; i++)
    {
        gs_stPara.VeChn[i] = VeChn[i];
    }
	gs_stPara.consume_type = type;
    return pthread_create(&gs_VencPid, 0, SAMPLE_COMM_VENC_GetVencStreamProcV2, (TS_VOID*)&gs_stPara);
}

TS_VOID* SAMPLE_COMM_VENC_UpdateChnFps(TS_VOID* p)
{
	TS_S32 s32Ret = TS_FAILURE;
	VI_PIPE_STATUS_S pstStatus = {0};
	VENC_CHN_ATTR_S  chnAttr   = {0};
	SAMPLE_VENC_UPDAETFPS_S *stUpdateFps;
	TS_S32 s32Cnt = 0;
	VENC_CHN VeChn[VENC_MAX_CHN_NUM];
	prctl(PR_SET_NAME, "UpdateFps", 0,0,0);

	stUpdateFps = (SAMPLE_VENC_UPDAETFPS_S *)p;
	s32Cnt = stUpdateFps->s32Cnt;

	for(int i = 0; i < s32Cnt; i++){
		VeChn[i] = stUpdateFps->VeChn[i];
	}

	while(stUpdateFps->bThreadStart == TS_TRUE){

		for(int i = 0; i < s32Cnt; i++){
			s32Ret = TS_MPI_VI_QueryPipeStatus(VeChn[i],&pstStatus);
			if(TS_SUCCESS != s32Ret){
				SAMPLE_PRT("TS_MPI_VI_QueryPipeStatus faild with %#x\n", s32Ret);
			}

			s32Ret = TS_MPI_VENC_GetChnAttr(i, &chnAttr);
			if(TS_SUCCESS != s32Ret){
				SAMPLE_PRT("TS_MPI_VENC_GetChnAttr faild with %#x\n", s32Ret);
			}
			//sleep(10);
			//pstStatus.u32FrameRate = 10;
			SAMPLE_PRT("bEnable=%d,u32FrameRate=%f\n",pstStatus.bEnable,pstStatus.fFrameRate);

			if((pstStatus.bEnable) && (fabs(pstStatus.fFrameRate) > 0.01)){
				//waring:仅仅只是使用示例，具体使用应视情况调整
				if(chnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H265CBR){
					if(chnAttr.stRcAttr.stH265Cbr.fr32DstFrameRate != (TS_FR32)pstStatus.fFrameRate){
						if(((TS_FR32)pstStatus.fFrameRate > chnAttr.stRcAttr.stH265Cbr.fr32DstFrameRate + 1)||
							(TS_FR32)(pstStatus.fFrameRate < chnAttr.stRcAttr.stH265Cbr.fr32DstFrameRate - 1)){
							chnAttr.stRcAttr.stH265Cbr.fr32DstFrameRate = (TS_FR32)pstStatus.fFrameRate;
	                        s32Ret = TS_MPI_VENC_SetChnAttr(VeChn[i],&chnAttr);
	                        if(TS_SUCCESS != s32Ret){
	                            SAMPLE_PRT("TS_MPI_VENC_SetChnAttr faild with %#x\n", s32Ret);
	                        }
						}

					}
				}else if(chnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H264CBR){
					if(chnAttr.stRcAttr.stH264Cbr.fr32DstFrameRate != (TS_FR32)pstStatus.fFrameRate){
						if(((TS_FR32)pstStatus.fFrameRate > chnAttr.stRcAttr.stH264Cbr.fr32DstFrameRate + 1)||
							((TS_FR32)pstStatus.fFrameRate < chnAttr.stRcAttr.stH264Cbr.fr32DstFrameRate - 1)){
							chnAttr.stRcAttr.stH264Cbr.fr32DstFrameRate = (TS_FR32)pstStatus.fFrameRate;
	                        s32Ret = TS_MPI_VENC_SetChnAttr(VeChn[i],&chnAttr);
	                        if(TS_SUCCESS != s32Ret){
	                            SAMPLE_PRT("TS_MPI_VENC_SetChnAttr faild with %#x\n", s32Ret);
	                        }
						}

					}
				}


			}

		}

		sleep(3);
	}

	return NULL;

}


TS_S32 SAMPLE_COMM_VENC_UpdateFps(VENC_CHN VeChn[],TS_S32 s32Cnt)
{
    TS_U32 i;

    gs_stUpdateFpsPara.bThreadStart = TS_TRUE;
    gs_stUpdateFpsPara.s32Cnt = s32Cnt;
    for(i=0; i<s32Cnt; i++)
    {
        gs_stUpdateFpsPara.VeChn[i] = VeChn[i];
    }

    return pthread_create(&gs_VencUpdatePid, 0, SAMPLE_COMM_VENC_UpdateChnFps, (TS_VOID*)&gs_stUpdateFpsPara);
}

TS_S32 SAMPLE_COMM_VENC_StopUpdateFps(void)
{
    if (TS_TRUE == gs_stUpdateFpsPara.bThreadStart)
    {
        gs_stUpdateFpsPara.bThreadStart = TS_FALSE;
        pthread_join(gs_VencUpdatePid, 0);
    }
    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VENC_SwitchResolution(VENC_CHN VeChn, TS_U32 width, TS_U32 height)
{
	TS_S32 s32Ret = TS_FAILURE;
	VENC_CHN_ATTR_S chn_attr;
    VENC_RECV_PIC_PARAM_S  stRecvParam = {-1};

	s32Ret = TS_MPI_VENC_StopRecvFrame(VeChn);
	if(TS_SUCCESS != s32Ret){
		SAMPLE_PRT("TS_MPI_VENC_StopRecvFrame faild with %#x\n", s32Ret);
		return TS_FAILURE;
	}

	s32Ret = TS_MPI_VENC_GetChnAttr(VeChn, &chn_attr);
	if(TS_SUCCESS != s32Ret){
		SAMPLE_PRT("TS_MPI_VENC_GetChnAttr faild with %#x\n", s32Ret);
		return TS_FAILURE;
	}

	chn_attr.stVencAttr.u32PicWidth = width;
	chn_attr.stVencAttr.u32PicHeight = height;

	s32Ret = TS_MPI_VENC_SetChnAttr(VeChn, &chn_attr);
	if(TS_SUCCESS != s32Ret){
		SAMPLE_PRT("TS_MPI_VENC_SetChnAttr faild with %#x\n", s32Ret);
		return TS_FAILURE;
	}
	SAMPLE_PRT("TS_MPI_VENC_SetChnAttr success, new WH=[%d,%d]\n",
		width, height);

	s32Ret = TS_MPI_VENC_StartRecvFrame(VeChn, &stRecvParam);
	if(TS_SUCCESS != s32Ret){
		SAMPLE_PRT("TS_MPI_VENC_StartRecvFrame faild with %#x\n", s32Ret);
		return TS_FAILURE;
	}

	return s32Ret;
}

TS_S32 SAMPLE_COMM_VENC_SwitchEncoder(VENC_CHN VeChn, PAYLOAD_TYPE_E type,VENC_RC_MODE_E enRcMode)
{
	TS_S32 s32Ret = TS_FAILURE;
	VENC_CHN_ATTR_S chn_attr;

	s32Ret = TS_MPI_VENC_GetChnAttr(VeChn, &chn_attr);
	if(TS_SUCCESS != s32Ret){
		SAMPLE_PRT("TS_MPI_VENC_GetChnAttr faild with %#x\n", s32Ret);
		return TS_FAILURE;
	}

	chn_attr.stVencAttr.enType = type;
	chn_attr.stRcAttr.enRcMode = enRcMode;

	s32Ret = TS_MPI_VENC_SetChnAttr(VeChn, &chn_attr);
	if(TS_SUCCESS != s32Ret){
		SAMPLE_PRT("TS_MPI_VENC_SetChnAttr faild with %#x\n", s32Ret);
		return TS_FAILURE;
	}
	SAMPLE_PRT("TS_MPI_VENC_SetChnAttr success, new encoder=[%d]\n", type);

	return s32Ret;
}

/******************************************************************************
* funciton : stop get venc stream process.
******************************************************************************/
TS_S32 SAMPLE_COMM_VENC_StopGetStream(void)
{

    if (TS_TRUE == gs_stPara.bThreadStart)
    {
        gs_stPara.bThreadStart = TS_FALSE;
        pthread_join(gs_VencPid, 0);
    }
    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VENC_StopSendFrame(void)
{
    return TS_SUCCESS;
}


