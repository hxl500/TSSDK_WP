

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

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
#include <sys/prctl.h>
#include "sample_comm_base.h"
#include "sample_comm_util.h"
#include "sample_comm_sys.h"
#include "sample_comm_vi.h"
#include "sample_comm_isp.h"
#include "sample_comm_vpss.h"
#include "sample_comm_vdec.h"

VB_SOURCE_E  g_enVdecVBSource  = VB_SOURCE_MODULE;

VB_POOL g_ahPicVbPool[VB_MAX_POOLS] = {[0 ... (VB_MAX_POOLS-1)] = VB_INVALID_POOLID};
VB_POOL g_ahTmvVbPool[VB_MAX_POOLS] = {[0 ... (VB_MAX_POOLS-1)] = VB_INVALID_POOLID};

#define PRINTF_VDEC_CHN_STATUS(Chn, stStatus) \
do{\
    printf("\033[0;33m ---------------------------------------------------------------------------------------------------\033[0;39m\n");\
    printf("\033[0;33m chn:%d, Type:%d, bStart:%d, DecodeFrames:%d, LeftPics:%d, LeftBytes:%d, LeftFrames:%d, RecvFrames:%d  \033[0;39m\n",\
        Chn,\
        stStatus.enType,\
        stStatus.bStartRecvStream,\
        stStatus.u32DecodeStreamFrames,\
        stStatus.u32LeftPics,\
        stStatus.u32LeftStreamBytes,\
        stStatus.u32LeftStreamFrames,\
        stStatus.u32RecvStreamFrames);\
    printf("\033[0;33m FormatErr:%d,    s32PicSizeErrSet:%d,  s32StreamUnsprt:%d,  s32PackErr:%d,  u32PrtclNumErrSet:%d,  s32RefErrSet:%d,  s32PicBufSizeErrSet:%d  \033[0;39m\n",\
        stStatus.stVdecDecErr.s32FormatErr,\
        stStatus.stVdecDecErr.s32PicSizeErrSet,\
        stStatus.stVdecDecErr.s32StreamUnsprt,\
        stStatus.stVdecDecErr.s32PackErr,\
        stStatus.stVdecDecErr.s32PrtclNumErrSet,\
        stStatus.stVdecDecErr.s32RefErrSet,\
        stStatus.stVdecDecErr.s32PicBufSizeErrSet);\
    printf("\033[0;33m ---------------------------------------------------------------------------------------------------\033[0;39m\n");\
}while(0)



#define SAVE_FILE_NAME(aFileName, chn, s32Cnt, cStreamName, enPixelFormat)\
do{\
    TS_CHAR *Postfix;\
    if(enPixelFormat == PIXEL_FORMAT_RGB_565)\
    {\
        Postfix = "rgb565";\
    }\
    else if(enPixelFormat == PIXEL_FORMAT_BGR_565)\
    {\
        Postfix = "bgr565";\
    }\
    else if(enPixelFormat == PIXEL_FORMAT_RGB_888)\
    {\
        Postfix = "rgb888";\
    }\
    else if(enPixelFormat == PIXEL_FORMAT_BGR_888)\
    {\
        Postfix = "bgr888";\
    }\
    else if(enPixelFormat == PIXEL_FORMAT_ARGB_8888)\
    {\
        Postfix = "argb8888";\
    }\
    else if(enPixelFormat == PIXEL_FORMAT_ABGR_8888)\
    {\
        Postfix = "abgr8888";\
    }\
    else if(enPixelFormat == PIXEL_FORMAT_ARGB_1555)\
    {\
        Postfix = "argb1555";\
    }\
    else if(enPixelFormat == PIXEL_FORMAT_ABGR_1555)\
    {\
        Postfix = "abgr1555";\
    }\
    else if((enPixelFormat == PIXEL_FORMAT_YVU_SEMIPLANAR_420)||(enPixelFormat == PIXEL_FORMAT_YUV_400))\
    {\
        Postfix = "yuv";\
    }\
    else\
    {\
        printf("[%s]-%d: enPixelFormat type err", __FUNCTION__, __LINE__);\
        return (TS_VOID *)(TS_FAILURE);\
    }\
    snprintf(aFileName, sizeof(aFileName), "chn%d_%d_%s.%s", chn, s32Cnt, cStreamName, Postfix);\
}while(0)



TS_S32 SAMPLE_COMM_VDEC_InitVBPool(TS_U32 ChnNum, SAMPLE_VDEC_ATTR *pastSampleVdec)
{
    VB_CONFIG_S stVbConf;
    TS_S32 i, j, pos=0, s32Ret=0;
    TS_BOOL bFindFlag;
    SAMPLE_VDEC_BUF astSampleVdecBuf[VDEC_MAX_CHN_NUM];
    VB_POOL_CONFIG_S stVbPoolCfg;

    memset(astSampleVdecBuf, 0, sizeof(SAMPLE_VDEC_BUF)*VDEC_MAX_CHN_NUM);
    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));

    for(i=0; i<ChnNum; i++)
    {
        if(PT_H265 == pastSampleVdec[i].enType)
        {
            astSampleVdecBuf[i].u32PicBufSize = VDEC_GetPicBufferSize(pastSampleVdec[i].enType, pastSampleVdec[i].u32Width, pastSampleVdec[i].u32Height,
                                                    PIXEL_FORMAT_YVU_SEMIPLANAR_420, pastSampleVdec[i].stSapmleVdecVideo.enBitWidth, 0);
            astSampleVdecBuf[i].u32TmvBufSize = VDEC_GetTmvBufferSize(pastSampleVdec[i].enType, pastSampleVdec[i].u32Width, pastSampleVdec[i].u32Height);
        }
        else if(PT_H264 == pastSampleVdec[i].enType)
        {
            astSampleVdecBuf[i].u32PicBufSize = VDEC_GetPicBufferSize(pastSampleVdec[i].enType, pastSampleVdec[i].u32Width, pastSampleVdec[i].u32Height,
                                                    PIXEL_FORMAT_YVU_SEMIPLANAR_420, pastSampleVdec[i].stSapmleVdecVideo.enBitWidth, 0);
            if(VIDEO_DEC_MODE_IPB == pastSampleVdec[i].stSapmleVdecVideo.enDecMode)
            {
                astSampleVdecBuf[i].u32TmvBufSize = VDEC_GetTmvBufferSize(pastSampleVdec[i].enType, pastSampleVdec[i].u32Width, pastSampleVdec[i].u32Height);
            }
        }
        else
        {
            astSampleVdecBuf[i].u32PicBufSize = VDEC_GetPicBufferSize(pastSampleVdec[i].enType, pastSampleVdec[i].u32Width, pastSampleVdec[i].u32Height,
                                                    pastSampleVdec[i].stSapmleVdecPicture.enPixelFormat, DATA_BITWIDTH_8, 0);
        }
    }

    /* PicBuffer */
    for(j=0; j<VB_MAX_COMM_POOLS; j++)
    {
        bFindFlag = TS_FALSE;
        for(i=0; i<ChnNum; i++)
        {
            if((TS_FALSE == bFindFlag) && (0 != astSampleVdecBuf[i].u32PicBufSize) && (TS_FALSE == astSampleVdecBuf[i].bPicBufAlloc) )
            {
                stVbConf.astCommPool[j].u64BlkSize = astSampleVdecBuf[i].u32PicBufSize;
                stVbConf.astCommPool[j].u32BlkCnt  = pastSampleVdec[i].u32FrameBufCnt;
                astSampleVdecBuf[i].bPicBufAlloc   = TS_TRUE;
                bFindFlag                          = TS_TRUE;
                pos = j;
            }

            if((TS_TRUE == bFindFlag) && (TS_FALSE == astSampleVdecBuf[i].bPicBufAlloc)
                && (stVbConf.astCommPool[j].u64BlkSize == astSampleVdecBuf[i].u32PicBufSize) )
            {
                stVbConf.astCommPool[j].u32BlkCnt += pastSampleVdec[i].u32FrameBufCnt;
                astSampleVdecBuf[i].bPicBufAlloc   = TS_TRUE;
            }
        }
    }

    /* TmvBuffer */
    for(j=pos+1; j<VB_MAX_COMM_POOLS; j++)
    {
        bFindFlag = TS_FALSE;
        for(i=0; i<ChnNum; i++)
        {
            if((TS_FALSE == bFindFlag) && (0 != astSampleVdecBuf[i].u32TmvBufSize) && (TS_FALSE == astSampleVdecBuf[i].bTmvBufAlloc) )
            {
                stVbConf.astCommPool[j].u64BlkSize = astSampleVdecBuf[i].u32TmvBufSize;
                stVbConf.astCommPool[j].u32BlkCnt  = pastSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum+1;
                astSampleVdecBuf[i].bTmvBufAlloc   = TS_TRUE;
                bFindFlag                          = TS_TRUE;
                pos = j;
            }

            if((TS_TRUE == bFindFlag) && (TS_FALSE == astSampleVdecBuf[i].bTmvBufAlloc)
                && (stVbConf.astCommPool[j].u64BlkSize == astSampleVdecBuf[i].u32TmvBufSize) )
            {
                stVbConf.astCommPool[j].u32BlkCnt += pastSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum+1;
                astSampleVdecBuf[i].bTmvBufAlloc   = TS_TRUE;
            }
        }
    }
    stVbConf.u32MaxPoolCnt = pos + 1;

    if(VB_SOURCE_MODULE == g_enVdecVBSource)
    {
        //TS_MPI_VB_ExitModCommPool(VB_UID_VDEC);
        //CHECK_RET(TS_MPI_VB_SetModPoolConfig(VB_UID_VDEC, &stVbConf), "TS_MPI_VB_SetModPoolConfigig");
        //s32Ret = TS_MPI_VB_InitModCommPool(VB_UID_VDEC);
        if (TS_SUCCESS != s32Ret)
        {
            printf("TS_MPI_VB_InitModCommPool fail for 0x%x\n", s32Ret);
            //TS_MPI_VB_ExitModCommPool(VB_UID_VDEC);
            return TS_FAILURE;
        }
    }
    else if (VB_SOURCE_USER == g_enVdecVBSource)
    {
        for (i = 0; i < ChnNum; i++)
        {
            if ( (0 != astSampleVdecBuf[i].u32PicBufSize) && (0 != pastSampleVdec[i].u32FrameBufCnt))
            {
                memset(&stVbPoolCfg, 0, sizeof(VB_POOL_CONFIG_S));
                stVbPoolCfg.u64BlkSize  = astSampleVdecBuf[i].u32PicBufSize;
                stVbPoolCfg.u32BlkCnt   = pastSampleVdec[i].u32FrameBufCnt;
                // stVbPoolCfg.enRemapMode = VB_REMAP_MODE_NONE;
                g_ahPicVbPool[i] = TS_MPI_VB_CreatePool(&stVbPoolCfg);
                if (VB_INVALID_POOLID == g_ahPicVbPool[i])
                {
                    goto fail;
                }
            }
            if (0 != astSampleVdecBuf[i].u32TmvBufSize)
            {
                memset(&stVbPoolCfg, 0, sizeof(VB_POOL_CONFIG_S));
                stVbPoolCfg.u64BlkSize  = astSampleVdecBuf[i].u32TmvBufSize;
                stVbPoolCfg.u32BlkCnt   = pastSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum+1;
                // stVbPoolCfg.enRemapMode = VB_REMAP_MODE_NONE;
                g_ahTmvVbPool[i] = TS_MPI_VB_CreatePool(&stVbPoolCfg);
                if (VB_INVALID_POOLID == g_ahTmvVbPool[i])
                {
                    goto fail;
                }
            }
        }
    }

    return TS_SUCCESS;

    fail:
        for (;i>=0;i--)
        {
            if (VB_INVALID_POOLID != g_ahPicVbPool[i])
            {
                s32Ret = TS_MPI_VB_DestroyPool(g_ahPicVbPool[i]);
                if(TS_SUCCESS != s32Ret)
                {
                    printf("TS_MPI_VB_DestroyPool %d fail!\n",g_ahPicVbPool[i]);
                }
                g_ahPicVbPool[i] = VB_INVALID_POOLID;
            }
            if (VB_INVALID_POOLID != g_ahTmvVbPool[i])
            {
                s32Ret = TS_MPI_VB_DestroyPool(g_ahTmvVbPool[i]);
                if(TS_SUCCESS != s32Ret)
                {
                    printf("TS_MPI_VB_DestroyPool %d fail!\n",g_ahTmvVbPool[i]);
                }
                g_ahTmvVbPool[i] = VB_INVALID_POOLID;
            }
        }
        return TS_FAILURE;
}


TS_VOID SAMPLE_COMM_VDEC_ExitVBPool(TS_VOID)
{
    TS_S32 i, s32Ret;

    if(VB_SOURCE_MODULE == g_enVdecVBSource)
    {
        //TS_MPI_VB_ExitModCommPool(VB_UID_VDEC);
    }
    else if (VB_SOURCE_USER == g_enVdecVBSource)
    {
        for (i=VB_MAX_POOLS-1; i>=0; i--)
        {
            if (VB_INVALID_POOLID != g_ahPicVbPool[i])
            {
                s32Ret = TS_MPI_VB_DestroyPool(g_ahPicVbPool[i]);
                if(TS_SUCCESS != s32Ret)
                {
                    printf("TS_MPI_VB_DestroyPool %d fail!\n",g_ahPicVbPool[i]);
                }
                g_ahPicVbPool[i] = VB_INVALID_POOLID;
            }
            if (VB_INVALID_POOLID != g_ahTmvVbPool[i])
            {
                s32Ret = TS_MPI_VB_DestroyPool(g_ahTmvVbPool[i]);
                if(TS_SUCCESS != s32Ret)
                {
                    printf("TS_MPI_VB_DestroyPool %d fail!\n",g_ahTmvVbPool[i]);
                }
                g_ahTmvVbPool[i] = VB_INVALID_POOLID;
            }
        }
    }

    return;
}

TS_VOID * SAMPLE_COMM_VDEC_SendStream(TS_VOID *pArgs)
{
    VDEC_THREAD_PARAM_S *pstVdecThreadParam =(VDEC_THREAD_PARAM_S *)pArgs;
    TS_BOOL bEndOfStream = TS_FALSE;
    TS_S32 s32UsedBytes = 0, s32ReadLen = 0;
    FILE *fpStrm=NULL;
    TS_U8 *pu8Buf = NULL;
    VDEC_STREAM_S stStream;
    TS_BOOL bFindStart, bFindEnd;
    TS_U64 u64PTS = 0;
    TS_U32 u32Len, u32Start;
    TS_S32 s32Ret = TS_FALSE,  i = 0;
    TS_CHAR cStreamFile[256];

    prctl(PR_SET_NAME, "VideoSendStream", 0,0,0);
    snprintf(cStreamFile, sizeof(cStreamFile), "%s/%s", pstVdecThreadParam->cFilePath,pstVdecThreadParam->cFileName);
    if(cStreamFile != 0)
    {
        fpStrm = fopen(cStreamFile, "rb");
        if(fpStrm == NULL)
        {
            SAMPLE_PRT("chn %d can't open file %s in send stream thread!\n", pstVdecThreadParam->s32ChnId, cStreamFile);
            return (TS_VOID *)(TS_FAILURE);
        }
    }
    printf("\n \033[0;36m chn %d, stream file:%s, userbufsize: %d \033[0;39m\n", pstVdecThreadParam->s32ChnId,
        pstVdecThreadParam->cFileName, pstVdecThreadParam->s32MinBufSize);

    pu8Buf = malloc(pstVdecThreadParam->s32MinBufSize);
    if(pu8Buf == NULL)
    {
        SAMPLE_PRT("chn %d can't alloc %d in send stream thread!\n", pstVdecThreadParam->s32ChnId, pstVdecThreadParam->s32MinBufSize);
        fclose(fpStrm);
        return (TS_VOID *)(TS_FAILURE);
    }
    fflush(stdout);

    u64PTS = pstVdecThreadParam->u64PtsInit;
    while (1)
    {
        if (pstVdecThreadParam->eThreadCtrl == THREAD_CTRL_STOP)
        {
            break;
        }
        else if (pstVdecThreadParam->eThreadCtrl == THREAD_CTRL_PAUSE)
        {
            sleep(1);
            continue;
        }

        bEndOfStream = TS_FALSE;
        bFindStart   = TS_FALSE;
        bFindEnd     = TS_FALSE;
        u32Start     = 0;
        fseek(fpStrm, s32UsedBytes, SEEK_SET);
        s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
        // printf("## s32ReadLen = %d\n", s32ReadLen);

        if (s32ReadLen < pstVdecThreadParam->s32MinBufSize)
        {
            if (pstVdecThreadParam->bCircleSend == TS_TRUE)
            {
                memset(&stStream, 0, sizeof(VDEC_STREAM_S) );
                stStream.bEndOfStream = TS_TRUE;
                s32UsedBytes = 0;
                fseek(fpStrm, 0, SEEK_SET);
                s32ReadLen = fread(pu8Buf, 1, pstVdecThreadParam->s32MinBufSize, fpStrm);
		    }else{
                 printf("\n\n####### exit?????? size %d\n\n", stStream.u32Len);
                break;
            }
        }


        if (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME
            && (pstVdecThreadParam->enType == PT_H265 || pstVdecThreadParam->enType == PT_H264))
        {

			// 因为一次读取数据较多，所以肯定能找到！！！
			// find nalu header
			int headerBytes = 0;
			i = 0;

refind:
			for (; i < s32ReadLen - 4; i++) {
				if (pu8Buf[i + 0] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1) {
					headerBytes = 3;
					bFindStart = TS_TRUE;
					break;
				}
				if ((pu8Buf[i + 0] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 0 &&
				     pu8Buf[i + 3] == 1)) {
					headerBytes = 4;
					bFindStart = TS_TRUE;
					break;
				}
			}

			i += headerBytes;

			for (; i < s32ReadLen - 4; i++) {
				if (pu8Buf[i + 0] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 1) {
					bFindEnd = TS_TRUE;
					break;
				}
				if ((pu8Buf[i + 0] == 0 && pu8Buf[i + 1] == 0 && pu8Buf[i + 2] == 0 &&
				     pu8Buf[i + 3] == 1)) {
					bFindEnd = TS_TRUE;
					break;
				}
			}

			if(i < 1024){
				goto refind;
			}

			if (bFindStart && bFindEnd) {
				s32ReadLen = i + 1;
			} else {
				SAMPLE_PRT("EOS reached? chn[%d] can not find start code! s32ReadLen %d, s32UsedBytes %d.!\n",
					   pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
                s32UsedBytes = 0;
                continue;
			}

            if(i>0)s32ReadLen = i;

            if (bFindEnd == TS_FALSE)
            {
                s32ReadLen = i+6;
            }

        }
        else if (pstVdecThreadParam->enType == PT_MJPEG || pstVdecThreadParam->enType == PT_JPEG)
        {
            for (i=0; i<s32ReadLen-1; i++)
            {
                if (pu8Buf[i] == 0xFF && pu8Buf[i+1] == 0xD8)
                {
                    u32Start = i;
                    bFindStart = TS_TRUE;
                    i = i + 2;
                    break;
                }
            }

            for (; i<s32ReadLen-3; i++)
            {
                if ((pu8Buf[i] == 0xFF) && (pu8Buf[i+1]& 0xF0) == 0xE0)
                {
                     u32Len = (pu8Buf[i+2]<<8) + pu8Buf[i+3];
                     i += 1 + u32Len;
                }
                else
                {
                    break;
                }
            }

            for (; i<s32ReadLen-1; i++)
            {
                if (pu8Buf[i] == 0xFF && pu8Buf[i+1] == 0xD9)
                {
                    bFindEnd = TS_TRUE;
                    break;
                }
            }
            s32ReadLen = i+2;

            if (bFindStart == TS_FALSE)
            {
                SAMPLE_PRT("chn %d can not find JPEG start code!s32ReadLen %d, s32UsedBytes %d.!\n",
                    pstVdecThreadParam->s32ChnId, s32ReadLen, s32UsedBytes);
            }
        }
        else
        {
            if((s32ReadLen != 0) && (s32ReadLen < pstVdecThreadParam->s32MinBufSize))
            {
                bEndOfStream = TS_TRUE;
            }
            SAMPLE_PRT("not find frame end,i =%d\n",i);
        }

        stStream.u64PTS       = u64PTS;
        stStream.pu8Addr      = pu8Buf + u32Start;
        stStream.u32Len       = s32ReadLen;
        stStream.bEndOfFrame  = (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME)? TS_TRUE: TS_FALSE;
        stStream.bEndOfStream = bEndOfStream;
        stStream.bDisplay     = 1;
        // SAMPLE_PRT("send data[%d,%d,%d,%d,%d,%d,%d,%d]\n",stStream.pu8Addr[0],stStream.pu8Addr[1],stStream.pu8Addr[2],stStream.pu8Addr[3]
        // ,stStream.pu8Addr[4],stStream.pu8Addr[5],stStream.pu8Addr[6],stStream.pu8Addr[7]);

SendAgain:
        s32Ret=TS_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, pstVdecThreadParam->s32MilliSec);
        if( (TS_SUCCESS != s32Ret) && (THREAD_CTRL_START == pstVdecThreadParam->eThreadCtrl) )
        {
            usleep(pstVdecThreadParam->s32IntervalTime);
            goto SendAgain;
        }
        else
        {
            bEndOfStream = TS_FALSE;
            s32UsedBytes = s32UsedBytes +s32ReadLen + u32Start;
            u64PTS += pstVdecThreadParam->u64PtsIncrease;
        }
        usleep(pstVdecThreadParam->s32IntervalTime);
    }

    /* send the flag of stream end */
    memset(&stStream, 0, sizeof(VDEC_STREAM_S) );
    stStream.bEndOfStream = TS_TRUE;
//    TS_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, -1);

    printf("\033[0;35m chn %d send steam thread return ...  \033[0;39m\n", pstVdecThreadParam->s32ChnId);
    fflush(stdout);
    if (pu8Buf != TS_NULL)
    {
        free(pu8Buf);
    }
    fclose(fpStrm);

    return (TS_VOID *)TS_SUCCESS;
}

static void SAMPLE_COMM_VDEC_SaveYUVFile_Linear8Bit(FILE* pfd, VIDEO_FRAME_S* pVBuf)
{
    TS_U8* pY_map = NULL;
    TS_U8* pC_map = NULL;
    unsigned int w, h;
    TS_U8* pMemContent;
    TS_U8 *pTmpBuff=TS_NULL;
    TS_U64 phy_addr;
    TS_U32 s32Csize = 0;
    TS_U32 s32Ysize = 0;
    PIXEL_FORMAT_E  enPixelFormat = pVBuf->enPixelFormat;
    TS_U32 u32UvHeight;

    if (PIXEL_FORMAT_YVU_SEMIPLANAR_420 == enPixelFormat)
    {
        s32Ysize = pVBuf->u32Stride[0] * pVBuf->u32Height;
        u32UvHeight = pVBuf->u32Height / 2;
    }
    else if(PIXEL_FORMAT_YVU_SEMIPLANAR_422 == enPixelFormat)
    {
        s32Ysize = pVBuf->u32Stride[0] * pVBuf->u32Height;
        u32UvHeight = pVBuf->u32Height;
    }
    else if(PIXEL_FORMAT_YUV_400 == enPixelFormat)
    {
        s32Ysize = pVBuf->u32Stride[0] * pVBuf->u32Height;
        u32UvHeight = 0;
    }
    else
    {
        printf("%s %d: This YUV format is not support!\n",__func__, __LINE__);
        return;
    }

    pTmpBuff = (TS_U8 *)malloc(pVBuf->u32Width / 2);
    if(TS_NULL == pTmpBuff)
    {
        SAMPLE_PRT("malloc pTmpBuff (size=%d) fail!!!\n",pVBuf->u32Stride[0]);
        return;
    }

    phy_addr = pVBuf->u64PhyAddr[0];

    pY_map = (TS_U8*) TS_MPI_SYS_Mmap(phy_addr, s32Ysize);
    if (TS_NULL == pY_map)
    {
        SAMPLE_PRT("TS_MPI_SYS_Mmap for pY_map fail!!\n");
        free(pTmpBuff);
        pTmpBuff = TS_NULL;
        return;
    }
    if (PIXEL_FORMAT_YUV_400 != enPixelFormat) {
        s32Csize = u32UvHeight * pVBuf->u32Stride[1];
        pC_map = (TS_U8*)TS_MPI_SYS_Mmap(pVBuf->u64PhyAddr[1], s32Csize);
        if (TS_NULL == pC_map)
        {
            SAMPLE_PRT("TS_MPI_SYS_Mmap for pC_map fail!!\n");
            free(pTmpBuff);
            pTmpBuff = TS_NULL;
            TS_MPI_SYS_Munmap(pY_map, s32Ysize);
            pY_map = TS_NULL;
            return;
        }
    }

    fprintf(stderr, "saving......Y......");
    fflush(stderr);
    for (h = 0; h < pVBuf->u32Height; h++)
    {
        pMemContent = pY_map + h * pVBuf->u32Stride[0];
        fwrite(pMemContent, pVBuf->u32Width, 1, pfd);
    }

    if(PIXEL_FORMAT_YUV_400 != enPixelFormat)
    {
        fflush(pfd);
        fprintf(stderr, "U......");
        fflush(stderr);

        for (h = 0; h < u32UvHeight; h++)
        {
            pMemContent = pC_map + h * pVBuf->u32Stride[1];

            pMemContent += 1;

            for (w = 0; w < pVBuf->u32Width / 2; w++)
            {
                pTmpBuff[w] = *pMemContent;
                pMemContent += 2;
            }
            fwrite(pTmpBuff, pVBuf->u32Width / 2, 1, pfd);
        }
        fflush(pfd);

        fprintf(stderr, "V......");
        fflush(stderr);
        for (h = 0; h < u32UvHeight; h++)
        {
            pMemContent = pC_map + h * pVBuf->u32Stride[1];

            for (w = 0; w < pVBuf->u32Width / 2; w++)
            {
                pTmpBuff[w] = *pMemContent;
                pMemContent += 2;
            }
            fwrite(pTmpBuff, pVBuf->u32Width / 2, 1, pfd);
        }
    }
    fflush(pfd);

    fprintf(stderr, "done!\n");
    fflush(stderr);
    free(pTmpBuff);
    pTmpBuff = TS_NULL;

    if (pC_map != TS_NULL) {
        TS_MPI_SYS_Munmap(pC_map, s32Csize);
        pC_map = TS_NULL;
    }

    TS_MPI_SYS_Munmap(pY_map, s32Ysize);
    pY_map = TS_NULL;

    return;
}





void SAMPLE_COMM_VDEC_SaveRGBFile(FILE* fRgb, VIDEO_FRAME_S* pVB)
{
    TS_U32 i;
    TS_U32 u32Size;
    TS_U8 *pUserPageAddr, *pTmp;
    TS_U32 u32WidthInBytes;
    TS_U32 u32Stride;

    if( (PIXEL_FORMAT_ARGB_8888 == pVB->enPixelFormat) || (PIXEL_FORMAT_ABGR_8888 == pVB->enPixelFormat))
    {
        u32WidthInBytes = pVB->u32Width*4;
    }
    else if( (PIXEL_FORMAT_ARGB_1555 == pVB->enPixelFormat) || (PIXEL_FORMAT_ABGR_1555 == pVB->enPixelFormat))
    {
        u32WidthInBytes = pVB->u32Width*2;
    }
    else if( (PIXEL_FORMAT_RGB_888 == pVB->enPixelFormat) || (PIXEL_FORMAT_BGR_888 == pVB->enPixelFormat))
    {
        u32WidthInBytes = pVB->u32Width*3;
    }
    else if( (PIXEL_FORMAT_RGB_565 == pVB->enPixelFormat) || (PIXEL_FORMAT_BGR_565 == pVB->enPixelFormat))
    {
        u32WidthInBytes = pVB->u32Width*2;
    }
    else
    {
        printf("%s %d: This RGB format is not support!\n",__func__, __LINE__);
        return;
    }

    u32Stride = pVB->u32Stride[0];
    u32Size   = u32Stride * ALIGN_UP(pVB->u32Height, 16);

    pUserPageAddr = (TS_U8*) TS_MPI_SYS_Mmap(pVB->u64PhyAddr[0], u32Size);

    if (TS_NULL == pUserPageAddr)
    {
        printf("%s %d:TS_MPI_SYS_Mmap fail!!! u32Size=%d\n",__func__, __LINE__,u32Size);
        return;
    }

    fprintf(stderr, "saving......RGB..%d x %d......", pVB->u32Width, pVB->u32Height);
    fflush(stderr);

    pTmp = pUserPageAddr;
    for (i = 0; i < pVB->u32Height; i++, pTmp += u32Stride)
    {
        fwrite(pTmp, u32WidthInBytes, 1, fRgb) ;
    }
    fflush(fRgb);

    fprintf(stderr, "done!\n");
    fflush(stderr);

    TS_MPI_SYS_Munmap(pUserPageAddr, u32Size);
    pUserPageAddr = TS_NULL;

    return;
}

static char get_input_dbg(void)
{
	FILE *file;
	char c;
	size_t ret;
	file = fopen("./input_dbg", "r");
	if (!file) {
        // SAMPLE_PRT("fopen file ./input_dbg failed:%s", strerror(errno));
        return 0x0;
    }

	ret = fread(&c, 1 ,1, file);
	if (1 != ret) {
        // SAMPLE_PRT("fread file ./input_dbg failed:%s", strerror(errno));
		fclose(file);
		return 0x0;
	}

	fclose(file);
	remove("./input_dbg");
	return c;
}

static int exit_flag = 0;

TS_VOID SAMPLE_COMM_VDEC_CmdCtrl(TS_S32 s32ChnNum,VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread)
{
    TS_S32 i, s32Ret;
    VDEC_CHN_STATUS_S stStatus;
    char c=0;

    for(i=0; i<s32ChnNum; i++)
    {
        if(TS_TRUE == pstVdecSend[i].bCircleSend)
        {
            goto WHILE;
        }
    }

    printf("decoding..............");
    for(i=0; i<s32ChnNum; i++)
    {
        if(0 != pVdecThread[i])
        {
            s32Ret = pthread_join(pVdecThread[i], TS_NULL);
            if(0 == s32Ret)
            {
                pVdecThread[i] = 0;
            }
        }
        pVdecThread[i] = 0;
        // while(1)
        // {
        //     s32Ret = TS_MPI_VDEC_QueryStatus(pstVdecSend[i].s32ChnId, &stStatus);
        //     if(s32Ret != TS_SUCCESS)
        //     {
        //         printf("chn %d TS_MPI_VDEC_QueryStatus fail!!!\n",s32Ret);
        //         return;
        //     }
        //     if((0 == stStatus.u32LeftStreamBytes)&&(0 == stStatus.u32LeftPics))
        //     {
        //         PRINTF_VDEC_CHN_STATUS(pstVdecSend[i].s32ChnId, stStatus);
        //         break;
        //     }
        //     usleep(1000);
        // }
    }
    printf("end!\n");
    return;

WHILE:
	printf("\nSAMPLE_TEST:press 'e' to exit; 'q' to query status!\n");
    while(1)
    {
        c = get_input_dbg();//getchar();
        if (c == 'e')
            break;
        else if (c == 'q')
        {
            for (i=0; i<s32ChnNum; i++)
            {
                TS_MPI_VDEC_QueryStatus(pstVdecSend[i].s32ChnId, &stStatus);
                PRINTF_VDEC_CHN_STATUS(pstVdecSend[i].s32ChnId, stStatus);
            }
        }

		sleep(1);
    }

	exit_flag = 1;

    return;
}


#define VDEC_FILE_PATH "./vdec_file"
TS_VOID* SAMPLE_COMM_VDEC_FILE(TS_VOID* p)
{
    VIDEO_FRAME_INFO_S stVFrame;
	TS_S32 ret = 0;
    FILE* file;
	static int count = 0;
	file = fopen(VDEC_FILE_PATH, "wb");
	if (!file)
	{
		SAMPLE_PRT("open file[%s] failed!\n", VDEC_FILE_PATH);
		return NULL;
	}

	while (!exit_flag) {

        ret=TS_MPI_VDEC_GetFrame(0, &stVFrame, -1);

	    /*
	    SAMPLE_PRT("pixel_format %d, size %d, w %d, h %d\n",
			stVFrame.stVFrame.enPixelFormat, stVFrame.stVFrame.size,
			stVFrame.stVFrame.u32Width, stVFrame.stVFrame.u32Height);
		SAMPLE_PRT("phyaddr[0] %lld, phyaddr[1] %lld, phyaddr[2] %lld\n",
			stVFrame.stVFrame.u64PhyAddr[0], stVFrame.stVFrame.u64PhyAddr[1], stVFrame.stVFrame.u64PhyAddr[2]);

		SAMPLE_PRT("viraddr[0] %lld, viraddr[1] %lld, viraddr[2] %lld\n",
			stVFrame.stVFrame.u64VirAddr[0], stVFrame.stVFrame.u64VirAddr[1], stVFrame.stVFrame.u64VirAddr[2]);*/

        if (TS_SUCCESS == ret) {

            printf("Recv frame %6d, PixelFormat %d, Size %d, Resolution %dx%d\r",
                count, stVFrame.stVFrame.enPixelFormat, stVFrame.stVFrame.size,
                stVFrame.stVFrame.u32Width, stVFrame.stVFrame.u32Height);
            fflush(stdout); // 刷新输出缓冲区，确保立即显示


            if (count < 5){

                size_t nmemb;

                printf("saving frame %d to file\n\n", count);

                if (stVFrame.stVFrame.enPixelFormat == PIXEL_FORMAT_NV_12 ) {
                    nmemb = fwrite((void *)(uintptr_t)stVFrame.stVFrame.u64VirAddr[0],
                        stVFrame.stVFrame.u32Height * stVFrame.stVFrame.u32Width, 1, file);
                    if (nmemb != 1)
                        SAMPLE_PRT("nmemb 1 should be written, but %zu actually, error %d", nmemb, errno);

                    nmemb = fwrite((void *)(uintptr_t)stVFrame.stVFrame.u64VirAddr[1],
                        stVFrame.stVFrame.u32Height * stVFrame.stVFrame.u32Width / 2, 1, file);
                    if (nmemb != 1)
                        SAMPLE_PRT("nmemb 1 should be written, but %zu actually, error %d", nmemb, errno);
                } else if (stVFrame.stVFrame.enPixelFormat == PIXEL_FORMAT_YUV_PLANAR_420) {
                    nmemb = fwrite(&stVFrame.stVFrame.u64VirAddr[0],
                        stVFrame.stVFrame.u32Height * stVFrame.stVFrame.u32Width, 1, file);
                    if (nmemb != 1)
                        SAMPLE_PRT("nmemb 1 should be written, but %zu actually, error %d", nmemb, errno);

                    nmemb = fwrite(&stVFrame.stVFrame.u64VirAddr[1],
                        stVFrame.stVFrame.u32Height * stVFrame.stVFrame.u32Width / 4, 1, file);
                    if (nmemb != 1)
                        SAMPLE_PRT("nmemb 1 should be written, but %zu actually, error %d", nmemb, errno);

                    nmemb = fwrite(&stVFrame.stVFrame.u64VirAddr[2],
                        stVFrame.stVFrame.u32Height * stVFrame.stVFrame.u32Width / 4, 1, file);
                    if (nmemb != 1)
                        SAMPLE_PRT("nmemb 1 should be written, but %zu actually, error %d", nmemb, errno);
                } else {
                    SAMPLE_PRT("Not support pixel format %d !\n", stVFrame.stVFrame.enPixelFormat);
                }

                ret = fflush(file);
                if (ret)
                    SAMPLE_PRT("fflush ret %d, error %d", ret, errno);
            }


			ret=TS_MPI_VDEC_ReleaseFrame(0, &stVFrame);

			if (TS_SUCCESS != ret) {
				SAMPLE_PRT("TS_MPI_VDEC_ReleaseFrame Failed! %x\n", ret);

			}
			count++;

    	} else {
			SAMPLE_PRT("TS_MPI_VDEC_GetFrame Failed!\n");
            usleep(1000);
		}

	}

	fclose(file);

    return NULL;
}
TS_VOID SAMPLE_COMM_VDEC_StartSendStream(TS_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread)
{
    TS_S32  i;

    for(i=0; i<s32ChnNum; i++)
    {
        pVdecThread[i] = 0;
        pthread_create(&pVdecThread[i], 0, SAMPLE_COMM_VDEC_SendStream, (TS_VOID *)&pstVdecSend[i]);
    }
}

TS_VOID SAMPLE_COMM_VDEC_StopSendStream(TS_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread)
{
    TS_S32  i;

    for(i=0; i<s32ChnNum; i++)
    {
        pstVdecSend[i].eThreadCtrl = THREAD_CTRL_STOP;
        if(0 != pVdecThread[i])
        {
			pthread_detach(pVdecThread[i]);

            pVdecThread[i] = 0;
        }
//        TS_MPI_VDEC_StopRecvStream(i);
    }
}

TS_VOID * SAMPLE_COMM_VDEC_GetPic(TS_VOID *pArgs)
{
    VDEC_THREAD_PARAM_S *pstVdecThreadParam =(VDEC_THREAD_PARAM_S *)pArgs;
    FILE *fp = TS_NULL;
    TS_S32 s32Ret, s32Cnt = 0;
    VDEC_CHN_ATTR_S  stAttr;
    VIDEO_FRAME_INFO_S stVFrame;
    TS_CHAR cSaveFile[256];

    prctl(PR_SET_NAME, "VdecGetPic", 0,0,0);

    s32Ret = TS_MPI_VDEC_GetChnAttr(pstVdecThreadParam->s32ChnId, &stAttr);
    if(TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("chn %d get chn attr fail for %#x!\n", pstVdecThreadParam->s32ChnId, s32Ret);
        return (TS_VOID *)(TS_FAILURE);
    }
    if(PT_JPEG != stAttr.enType)
    {
        SAMPLE_PRT("chn %d enType %d do not suport save file!\n", pstVdecThreadParam->s32ChnId, stAttr.enType);
        return (TS_VOID *)(TS_FAILURE);
    }

    while(1)
    {
        if (pstVdecThreadParam->eThreadCtrl == THREAD_CTRL_STOP)
        {
            break;
        }

        s32Ret=TS_MPI_VDEC_GetFrame(pstVdecThreadParam->s32ChnId, &stVFrame, pstVdecThreadParam->s32MilliSec);
        if(TS_SUCCESS == s32Ret)
        {
            s32Cnt++;
            SAVE_FILE_NAME(cSaveFile, pstVdecThreadParam->s32ChnId, s32Cnt, pstVdecThreadParam->cFileName, stVFrame.stVFrame.enPixelFormat);
            if (cSaveFile != 0)
            {
                fp=fopen(cSaveFile,"wb");
                if(fp==NULL)
                {
                    SAMPLE_PRT("chn %d can't open file %s in get picture thread!\n", pstVdecThreadParam->s32ChnId, cSaveFile);
                    return (TS_VOID *)(TS_FAILURE);
                }
                printf("\033[0;34m chn %d saving yuv file:%s \033[0;39m \n",pstVdecThreadParam->s32ChnId, cSaveFile);
            }

            if ( (PIXEL_FORMAT_YVU_SEMIPLANAR_420 != stVFrame.stVFrame.enPixelFormat) && (PIXEL_FORMAT_YUV_400 != stVFrame.stVFrame.enPixelFormat))
               {
                SAMPLE_COMM_VDEC_SaveRGBFile(fp, &stVFrame.stVFrame);
               }
            else if( (stVFrame.stVFrame.enCompressMode == COMPRESS_MODE_NONE) &&
                     (stVFrame.stVFrame.enVideoFormat == VIDEO_FORMAT_LINEAR) )
            {
                SAMPLE_COMM_VDEC_SaveYUVFile_Linear8Bit(fp,&stVFrame.stVFrame);
            }
            else
            {
                SAMPLE_PRT("chn %d enPixelFormat %d do not suport save file!\n", pstVdecThreadParam->s32ChnId, stVFrame.stVFrame.enPixelFormat);
                s32Ret=TS_MPI_VDEC_ReleaseFrame(pstVdecThreadParam->s32ChnId, &stVFrame);
                if (TS_SUCCESS != s32Ret)
                {
                    SAMPLE_PRT("chn %d TS_MPI_VDEC_ReleaseFrame fail for s32Ret=0x%x!\n", pstVdecThreadParam->s32ChnId, s32Ret);
                }
                break;
            }
            s32Ret=TS_MPI_VDEC_ReleaseFrame(pstVdecThreadParam->s32ChnId, &stVFrame);
            if (TS_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("chn %d TS_MPI_VDEC_ReleaseFrame fail for s32Ret=0x%x!\n", pstVdecThreadParam->s32ChnId, s32Ret);
            }
            if (fp != NULL)
            {
                fclose(fp);
            }
        }
        else
        {
            usleep(1000);
        }
    }

    printf("\033[0;35m chn %d get pic thread return ...  \033[0;39m\n", pstVdecThreadParam->s32ChnId);

    return (TS_VOID *)TS_SUCCESS;
}


TS_VOID SAMPLE_COMM_VDEC_StartGetPic(TS_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecGet, pthread_t *pVdecThread)
{
    TS_S32  i;

    for(i=0; i<s32ChnNum; i++)
    {
        pVdecThread[i+VDEC_MAX_CHN_NUM] = 0;
        pthread_create(&pVdecThread[i+VDEC_MAX_CHN_NUM], 0, SAMPLE_COMM_VDEC_GetPic, (TS_VOID *)&pstVdecGet[i]);
    }
}

TS_VOID SAMPLE_COMM_VDEC_StopGetPic(TS_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecGet, pthread_t *pVdecThread)
{
    TS_S32  i;

    for(i=0; i<s32ChnNum; i++)
    {
        pstVdecGet[i].eThreadCtrl = THREAD_CTRL_STOP;
        if(0 != pVdecThread[i+VDEC_MAX_CHN_NUM])
        {
            pthread_join(pVdecThread[i+VDEC_MAX_CHN_NUM], TS_NULL);
            pVdecThread[i+VDEC_MAX_CHN_NUM] = 0;
        }
    }
}


TS_S32 SAMPLE_COMM_VDEC_Start(TS_S32 s32ChnNum, SAMPLE_VDEC_ATTR *pastSampleVdec)
{
    TS_S32  i;
    VDEC_CHN_ATTR_S stChnAttr[VDEC_MAX_CHN_NUM];
    //VDEC_CHN_POOL_S stPool;
    VDEC_CHN_PARAM_S stChnParam;
    //VDEC_MOD_PARAM_S stModParam;

    // CHECK_RET(TS_MPI_VDEC_GetModParam(&stModParam), "TS_MPI_VDEC_GetModParam");

    //stModParam.enVdecVBSource = g_enVdecVBSource;
    // CHECK_RET(TS_MPI_VDEC_SetModParam(&stModParam), "TS_MPI_VDEC_GetModParam");

    for(i=0; i<s32ChnNum; i++)
    {
        stChnAttr[i].enType           = pastSampleVdec[i].enType;
        stChnAttr[i].enMode           = pastSampleVdec[i].enMode;
        stChnAttr[i].u32PicWidth      = pastSampleVdec[i].u32Width;
        stChnAttr[i].u32PicHeight     = pastSampleVdec[i].u32Height;
        stChnAttr[i].u32StreamBufSize = pastSampleVdec[i].u32Width*pastSampleVdec[i].u32Height*100;
        stChnAttr[i].u32FrameBufCnt   = pastSampleVdec[i].u32FrameBufCnt;

        if (PT_H264 == pastSampleVdec[i].enType || PT_H265 == pastSampleVdec[i].enType)
        {
            stChnAttr[i].stVdecVideoAttr.u32RefFrameNum     = pastSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum;
            stChnAttr[i].stVdecVideoAttr.bTemporalMvpEnable = 1;
            if (PT_H264 == pastSampleVdec[i].enType)
            {
                stChnAttr[i].stVdecVideoAttr.bTemporalMvpEnable = 0;
            }
            stChnAttr[i].u32FrameBufSize  = VDEC_GetPicBufferSize(stChnAttr[i].enType, pastSampleVdec[i].u32Width, pastSampleVdec[i].u32Height,
                    PIXEL_FORMAT_YVU_SEMIPLANAR_420, pastSampleVdec[i].stSapmleVdecVideo.enBitWidth, 0);
        }
        else if (PT_JPEG == pastSampleVdec[i].enType || PT_MJPEG == pastSampleVdec[i].enType)
        {
            stChnAttr[i].enMode           = VIDEO_MODE_FRAME;
            stChnAttr[i].u32FrameBufSize  = VDEC_GetPicBufferSize(stChnAttr[i].enType, pastSampleVdec[i].u32Width, pastSampleVdec[i].u32Height,
                                                pastSampleVdec[i].stSapmleVdecPicture.enPixelFormat, DATA_BITWIDTH_8, 0);
        }

        CHECK_CHN_RET(TS_MPI_VDEC_CreateChn(i, &stChnAttr[i]), i, "TS_MPI_VDEC_CreateChn");

        if (VB_SOURCE_USER == g_enVdecVBSource)
        {
            //stPool.hPicVbPool = g_ahPicVbPool[i];
            //stPool.hTmvVbPool = g_ahTmvVbPool[i];
            // CHECK_CHN_RET(TS_MPI_VDEC_AttachVbPool(i, &stPool), i, "TS_MPI_VDEC_AttachVbPool");
        }

        // CHECK_CHN_RET(TS_MPI_VDEC_GetChnParam(i, &stChnParam), i, "TS_MPI_VDEC_GetChnParam");
        if (PT_H264 == pastSampleVdec[i].enType || PT_H265 == pastSampleVdec[i].enType)
        {
            stChnParam.stVdecVideoParam.enDecMode         = pastSampleVdec[i].stSapmleVdecVideo.enDecMode;
            stChnParam.stVdecVideoParam.enCompressMode    = COMPRESS_MODE_NONE;
            stChnParam.stVdecVideoParam.enVideoFormat     = VIDEO_FORMAT_TILE_64x16;
            if(VIDEO_DEC_MODE_IPB == stChnParam.stVdecVideoParam.enDecMode)
            {
                stChnParam.stVdecVideoParam.enOutputOrder = VIDEO_OUTPUT_ORDER_DISP;
            }
            else
            {
                stChnParam.stVdecVideoParam.enOutputOrder = VIDEO_OUTPUT_ORDER_DEC;
            }
        }
        else
        {
            stChnParam.stVdecPictureParam.enPixelFormat   = pastSampleVdec[i].stSapmleVdecPicture.enPixelFormat;
            stChnParam.stVdecPictureParam.u32Alpha        = pastSampleVdec[i].stSapmleVdecPicture.u32Alpha;
        }
        stChnParam.u32DisplayFrameNum                     = pastSampleVdec[i].u32DisplayFrameNum;
        // CHECK_CHN_RET(TS_MPI_VDEC_SetChnParam(i, &stChnParam), i, "TS_MPI_VDEC_GetChnParam");
        CHECK_CHN_RET(TS_MPI_VDEC_StartRecvStream(i), i, "TS_MPI_VDEC_StartRecvStream");

    }

    return TS_SUCCESS;
}


TS_S32 SAMPLE_COMM_VDEC_Stop(TS_S32 s32ChnNum)
{
    TS_S32 i;

	sleep(1);

    for(i=0; i<s32ChnNum; i++)
    {
        CHECK_CHN_RET(TS_MPI_VDEC_StopRecvStream(i), i, "TS_MPI_VDEC_StopRecvStream");
        CHECK_CHN_RET(TS_MPI_VDEC_DestroyChn(i), i, "TS_MPI_VDEC_DestroyChn");
    }

    return TS_SUCCESS;
}

