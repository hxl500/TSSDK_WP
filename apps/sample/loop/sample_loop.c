/*********************************************************************************************
 * Copyright (C) 2021-2025 Tsing Micro Intelligent Tech. Co., Ltd.
 *
 * @File Name: sample_loop.c
 * @Created Time: Tue 24 Dec 2024 06:59:49 PM CST
 * @Author: Tsingmicro .
 * @Description: .
 * @History: created .
 *
 ********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <getopt.h>  //getopt_long,optarg...

#include "sample_comm_base.h"
#include "sample_comm_util.h"
#include "sample_comm_sys.h"
#include "sample_comm_vi.h"
#include "sample_comm_isp.h"
#include "sample_comm_vpss.h"
#include "sample_comm_venc.h"

static TS_BOOL s_bIsRunning = TS_FALSE;

/*
    1. query from isp drv
    2. get from UI
    3. get from config file
    4. initial
*/
static MODE_SETTING_S g_sample_venc_modeSetting_list[] = {
    // {1,TS_TRUE,PIC_1080P, 8, 15, 0, DATA_BITWIDTH_8, BAYER_GRBG},
    // {1,TS_TRUE,PIC_2560x1440, 8, 15, 0, DATA_BITWIDTH_8, BAYER_GRBG},
    // {1,TS_TRUE,PIC_2688x1520, 10, 25, 0, DATA_BITWIDTH_10, BAYER_RGGB},
    // {2,TS_TRUE,PIC_2560x1440, 8, 15, 0, DATA_BITWIDTH_8, BAYER_GRBG},
    { 2, TS_TRUE, PIC_1080P, 8, 15, 0, DATA_BITWIDTH_8, BAYER_GRBG },
    { 2, TS_TRUE, PIC_2304x1296, 8, 13, 0, DATA_BITWIDTH_8, BAYER_GRBG },
};

/**
 * @brief
 * @param pArgs
 * @author Tsingmicro
 * @date 2024-12-24
 * @return
 */
static TS_VOID SAMPLE_VENC_Usage(const TS_CHAR *pArgs)
{
    printf("\nUsage: %s [index] [sub-option]\n\n", pArgs);

    printf("option list:\n");
    printf("\t 0) The supported resolution is 1920x1080 (Dual on-line) !.\n");
    printf("\t 1) The supported resolution is 2304x1296 (Dual on-line) !.\n");

    return;
}

/**
 * @brief
 * @param u32Index
 * @author Tsingmicro
 * @date 2024-12-23
 */
static TS_VOID SAMPLE_VENC_ShowModeInfo(const TS_U32 u32Index)
{
    printf("\n\n Tips: If you choose dual online, you will need to set the sensor timing in advance !. \n\n");

    printf("Index:\n");

    TS_S32 arraySize = sizeof(g_sample_venc_modeSetting_list) / sizeof(MODE_SETTING_S);
    for (TS_S32 i = 0; i < arraySize; i++) {
        SIZE_S          stSize  = { 0 };
        MODE_SETTING_S *setting = &(g_sample_venc_modeSetting_list[i]);

        SAMPLE_COMM_SYS_GetPicSize(setting->enSize, &stSize);
        printf("\t %d) numSensor: %d, on-line: %s, enSize: %dx%d, minFps: %d, maxFps: %d, wdr_mode: %d, bit_width: %d, enBayer: %d\n", i, setting->numCam, setting->isOnLine ? "true" : "false",
               stSize.u32Width, stSize.u32Height, setting->minFps, setting->maxFps, setting->wdr_mode, setting->bit_width, setting->enBayer);
    }

    printf("\n");

    return;
}

/**
 * @brief
 * @param signo
 * @author Tsingmicro
 * @date 2024-12-24
 * @return
 */
static TS_VOID SAMPLE_VENC_HandleSig(TS_S32 signo)
{
    SAMPLE_PRT("\n\n\033[0;31mReceive signal %s, exit...!\033[0;39m\n\n", strsignal(signo));

    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    // ungetc('q', stdin);

    s_bIsRunning = TS_FALSE;
}

/**
 * @brief
 * @author Tsingmicro
 * @date 2024-12-24
 * @return
 */
static TS_VOID SAMPLE_WAIT_Q_EXIT(TS_VOID)
{
    while (s_bIsRunning) {
        ts_usleep(1000 * 50);
    }
}

/**
 * @brief
 * @param pstViConfig
 * @author Tsingmicro
 * @date 2024-12-19
 * @return
 */
static TS_S32 SAMPLE_VENC_VI_Init(SAMPLE_VI_CONFIG_S *pstViConfig)
{
    TS_S32      s32Ret = TS_FAILURE;
    VB_CONFIG_S stVbConf;

    stVbConf.astCommPool[0].u64BlkSize = 640 * 360 * 3 / 2;  // u64BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt  = 1;                  // SAMPLE_COMM_VENC_Get_int_fromKeyboard("blk cnt");
    stVbConf.u32MaxPoolCnt             = 1;
    printf("u64BlkSize: %llu, u32BlkCnt: %u, u32MaxPoolCnt: %u\n", stVbConf.astCommPool[0].u64BlkSize, stVbConf.astCommPool[0].u32BlkCnt, stVbConf.u32MaxPoolCnt);
    // stVbConf.astCommPool[0].u64BlkSize   = 1024;
    // stVbConf.astCommPool[0].u32BlkCnt	  = 1;
    // stVbConf.u32MaxPoolCnt = 1;

    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (TS_SUCCESS != s32Ret) {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_VI_StartVi(pstViConfig);
    if (TS_SUCCESS != s32Ret) {
        SAMPLE_COMM_SYS_Exit();
        SAMPLE_PRT("SAMPLE_COMM_VI_StartVi failed with %d!\n", s32Ret);
        return s32Ret;
    }

    return TS_SUCCESS;
}

/**
 * @brief
 * @param VpssGrp
 * @param enPixelFormat
 * @param pVpssGrpSize
 * @param pVpssChnSize
 * @param chnNum
 * @author Tsingmicro
 * @date 2024-12-23
 * @return
 */
static TS_S32 SAMPLE_VENC_VPSS_Init(VPSS_GRP VpssGrp, PIXEL_FORMAT_E enPixelFormat, SIZE_S *pVpssGrpSize, SIZE_S *pVpssChnSize, const TS_U32 chnNum)
{
    TS_S32          s32Ret           = TS_FAILURE;
    VPSS_GRP_ATTR_S stVpssGrpAttr    = { 0 };
    VPSS_CHN_ATTR_S stVpssChnAttr[4] = { 0 };

    stVpssGrpAttr.enPixelFormat               = enPixelFormat;
    stVpssGrpAttr.enGrpMode                   = VPSS_GRP_MODE_STREAM;
    stVpssGrpAttr.enDynamicRange              = DYNAMIC_RANGE_SDR8;
    stVpssGrpAttr.u32MaxW                     = pVpssGrpSize->u32Width;
    stVpssGrpAttr.u32MaxH                     = pVpssGrpSize->u32Height;
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;

    for (TS_S32 i = 0; i < chnNum; i++) {
        memset(&stVpssChnAttr[i], 0, sizeof(VPSS_CHN_ATTR_S));
        SAMPLE_PRT("pVpssChnSize[%d].u32Width: %d, pVpssChnSize[%d].u32Height: %d !\n", i, pVpssChnSize[i].u32Width, i, pVpssChnSize[i].u32Height);

        stVpssChnAttr[i].u32Width                    = pVpssChnSize[i].u32Width;
        stVpssChnAttr[i].u32Height                   = pVpssChnSize[i].u32Height;
        stVpssChnAttr[i].enChnMode                   = VPSS_CHN_MODE_USER;
        stVpssChnAttr[i].enCompressMode              = COMPRESS_MODE_NONE;  // COMPRESS_MODE_SEG;
        stVpssChnAttr[i].enDynamicRange              = DYNAMIC_RANGE_SDR8;
        stVpssChnAttr[i].enPixelFormat               = enPixelFormat;
        stVpssChnAttr[i].enVideoFormat               = VIDEO_FORMAT_LINEAR;
        stVpssChnAttr[i].stFrameRate.s32SrcFrameRate = -1;
        stVpssChnAttr[i].stFrameRate.s32DstFrameRate = -1;
        stVpssChnAttr[i].u32Depth                    = 3;
        stVpssChnAttr[i].bMirror                     = TS_FALSE;
        stVpssChnAttr[i].bFlip                       = TS_FALSE;
        stVpssChnAttr[i].stAspectRatio.enMode        = ASPECT_RATIO_NONE;
        stVpssChnAttr[i].u32MaxW                     = pVpssChnSize[i].u32Width;
        stVpssChnAttr[i].u32MaxH                     = pVpssChnSize[i].u32Height;
        stVpssChnAttr[i].u32MemCount                 = 1;
        stVpssChnAttr[i].bVpuLoop                    = TS_FALSE;

#if 1
#define ISP_VALID_FPS 13
#define ISP_VALID_FPS_TIME ( int )(1000 / ISP_VALID_FPS)
#define ISP_VALID_WIDTH 20
#define ISP_VALID_MID_WIDTH 10  // 30(dsync_ms) - ISP_VALID_WIDTH
#define ISP_VALID_DELTA 8       // related to encode time joggle

        LOOP_ENC_CFG_S stLoopEncCfg[4] = {
            { 0, 1, 1 },  // inst_idx 0 sensorId 是否是主码流 下一帧编码通道
            { 0, 0, 2 },  // inst_idx 1
            { 1, 1, 3 },  // inst_idx 2
            { 1, 0, 0 }   // inst_idx 3
        };

        int idx                                  = VpssGrp * 2 + i;
        stVpssChnAttr[i].stFastFlow.stLoopEncCfg = stLoopEncCfg[idx];
        if (VpssGrp == 0) {
            if (i == 0) {
                stVpssChnAttr[i].stFastFlow.s8Index       = 0;                     // all_opt->rsz0.index;
                stVpssChnAttr[i].stFastFlow.enFastFlow    = FAST_FLOW_MODE_LOOP1;  // FAST_FLOW_MODE_STITCH;//all_opt->rsz0.mode;
                stVpssChnAttr[i].stFastFlow.u32Depth      = 32;
                stVpssChnAttr[i].stFastFlow.s8SyncChannel = 2;
                stVpssChnAttr[i].stFastFlow.u32MinTiming  = ISP_VALID_WIDTH + ISP_VALID_MID_WIDTH;
                stVpssChnAttr[i].stFastFlow.u32MaxTiming  = ISP_VALID_FPS_TIME - ISP_VALID_WIDTH - ISP_VALID_MID_WIDTH + ISP_VALID_DELTA;
            } else {
                stVpssChnAttr[i].stFastFlow.s8Index       = -1;
                stVpssChnAttr[i].stFastFlow.enFastFlow    = FAST_FLOW_MODE_NORMAL;
                stVpssChnAttr[i].stFastFlow.u32Depth      = 0;
                stVpssChnAttr[i].stFastFlow.s8SyncChannel = 2;
            }
        } else {
            if (i == 0) {
                stVpssChnAttr[i].stFastFlow.s8Index       = 2;                     // all_opt->rsz1.index;
                stVpssChnAttr[i].stFastFlow.enFastFlow    = FAST_FLOW_MODE_LOOP1;  // FAST_FLOW_MODE_STITCH;//all_opt->rsz1.mode;
                stVpssChnAttr[i].stFastFlow.u32Depth      = 32;                    // all_opt->rsz1.depth; //?..............
                stVpssChnAttr[i].stFastFlow.s8SyncChannel = 2;
                stVpssChnAttr[i].stFastFlow.u32MinTiming  = 0;
                stVpssChnAttr[i].stFastFlow.u32MaxTiming  = ISP_VALID_FPS_TIME - ISP_VALID_WIDTH - ISP_VALID_MID_WIDTH + ISP_VALID_DELTA;
            } else {

                stVpssChnAttr[i].stFastFlow.s8Index       = -1;
                stVpssChnAttr[i].stFastFlow.enFastFlow    = FAST_FLOW_MODE_NORMAL;
                stVpssChnAttr[i].stFastFlow.u32Depth      = 0;
                stVpssChnAttr[i].stFastFlow.s8SyncChannel = 2;
            }
        }

        SAMPLE_PRT(" --> stVpssChnAttr[%d].stFastFlow.s8Index: %d, enFastFlow: %d, u32Depth: %d, s8SyncChannel: %d, u32MinTiming: %d, u32MaxTiming: %d !\n", i, stVpssChnAttr[i].stFastFlow.s8Index,
                   stVpssChnAttr[i].stFastFlow.enFastFlow, stVpssChnAttr[i].stFastFlow.u32Depth, stVpssChnAttr[i].stFastFlow.s8SyncChannel, stVpssChnAttr[i].stFastFlow.u32MinTiming,
                   stVpssChnAttr[i].stFastFlow.u32MaxTiming);
    }
#endif

    s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, &stVpssGrpAttr, stVpssChnAttr, chnNum);
    if (s32Ret != TS_SUCCESS) {
        SAMPLE_PRT("VPSS Start failed for %#x!\n", s32Ret);
    }

    return s32Ret;
}

/**
 * @brief loop
 * @param pstModeSetting
 * @author Tsingmicro
 * @date 2024-12-23
 * @return
 */
static TS_S32 PDT_SAMPLE_2SNS_4STREAM_LOOP(const MODE_SETTING_S *pstModeSetting)
{
#define TMP_VI_MAX_PHY_PIPE_NUM 2
    TS_S32 s32Ret = TS_SUCCESS;
    TS_S32 i      = 0;

    VI_CHN  ViChn      = 0;
    VI_PIPE ViPipe     = 0;
    TS_U32  u32Profile = 0;
    TS_BOOL bLowDelay  = TS_TRUE;

    PIXEL_FORMAT_E enPixFormat = PIXEL_FORMAT_NV_12;

    SAMPLE_RC_E        enRcMode;
    VENC_GOP_MODE_E    enGopMode;
    VENC_GOP_ATTR_S    stGopAttr;
    SAMPLE_VI_CONFIG_S stViConfig = {0};

    TS_U64 special_mode = 0;

    special_mode = SYS_SPECIAL_MASK_FASTBOOT | SYS_SPECIAL_MODE_STITCH;
    if (TS_SUCCESS != TS_MPI_SYS_SetSpecialMode(special_mode)) {
        SAMPLE_PRT("TS_MPI_SYS_SetSpecialMode failed !. \n");
        return TS_FAILURE;
    } else {
        SAMPLE_PRT("TS_MPI_SYS_SetSpecialMode OK.\n");
    }

    SIZE_S stViPipvSize = { .u32Width = 1920, .u32Height = 1080 };

    SAMPLE_COMM_SYS_GetPicSize(pstModeSetting->enSize, &stViPipvSize);

    stViConfig.s32WorkingViNum = TMP_VI_MAX_PHY_PIPE_NUM;

    for (i = 0; i < stViConfig.s32WorkingViNum; i++) {
        SAMPLE_CHN_INFO_S  *pstChnInfo  = &stViConfig.astViInfo[i].stChnInfo;
        SAMPLE_PIPE_INFO_S *pstPipeInfo = &stViConfig.astViInfo[i].stPipeInfo;

        pstPipeInfo->aPipe       = i;
        pstPipeInfo->enWdrMode   = WDR_MODE_NONE;
        pstPipeInfo->width       = stViPipvSize.u32Width;
        pstPipeInfo->height      = stViPipvSize.u32Height;
        pstPipeInfo->frameRate   = pstModeSetting->maxFps;
        pstPipeInfo->enBayer     = pstModeSetting->enBayer;
        pstPipeInfo->bIspByFly   = pstModeSetting->isOnLine;
        pstPipeInfo->enBitWid    = pstModeSetting->bit_width;
        pstPipeInfo->bDynFpsSync = TS_TRUE;
        if (TS_TRUE == bLowDelay) {
            pstPipeInfo->enMastPipeMode = VI_ONLINE_VPSS_ONLINE;
            pstPipeInfo->enPixFmt       = PIXEL_FORMAT_RGB_888;
        } else {
            pstPipeInfo->enMastPipeMode = VI_ONLINE_VPSS_OFFLINE;
            pstPipeInfo->enPixFmt       = PIXEL_FORMAT_NV_12;
        }

        pstChnInfo->ViChn[0]     = 0;
        pstChnInfo->width[0]     = stViPipvSize.u32Width;
        pstChnInfo->height[0]    = stViPipvSize.u32Height;
        pstChnInfo->validChnlNum = 1;
        pstChnInfo->enPixFormat  = PIXEL_FORMAT_NV_12;
    }

    // if(TS_TRUE == bIspByfly){
    // 	SAMPLE_PRT("do something when ispByFly\n");
    // }

    s32Ret = SAMPLE_VENC_VI_Init(&stViConfig);
    if (s32Ret != TS_SUCCESS) {
        SAMPLE_PRT("Init VI err for %#x!\n", s32Ret);
        return TS_FAILURE;
    }

    TS_S32 VpssChnlNum = pstModeSetting->numCam;

    VPSS_GRP VpssGrp[2]                             = { 0, 1 };
    SIZE_S   stVpssChnSize[TMP_VI_MAX_PHY_PIPE_NUM] = { { .u32Width = 1920, .u32Height = 1080 }, { .u32Width = 640, .u32Height = 360 } };

    for (i = 0; i < pstModeSetting->numCam; i++) {
        stVpssChnSize[0].u32Width  = stViPipvSize.u32Width;
        stVpssChnSize[0].u32Height = stViPipvSize.u32Height;

        s32Ret = SAMPLE_VENC_VPSS_Init(VpssGrp[i], enPixFormat, &stViPipvSize, stVpssChnSize, VpssChnlNum);
        if (TS_SUCCESS != s32Ret) {
            SAMPLE_PRT("VPSS Init Grp[%d] err for %#x!\n", VpssGrp[i], s32Ret);
            goto EXIT_VI_STOP;
        }

        SAMPLE_PRT("SAMPLE_VENC_VPSS_Init Grp[%d] success!\n", VpssGrp[i]);

        if (TS_FALSE != bLowDelay) {
            SAMPLE_PRT("do NOT need VI Bind VPSS \n");
            continue;
        }

        s32Ret = SAMPLE_COMM_VI_Bind_VPSS(i, 0, VpssGrp[i]);
        if (s32Ret != TS_SUCCESS) {
            SAMPLE_PRT("VI Bind VPSS err for %#x!\n", s32Ret);
            goto EXIT_VI_VPSS_UNBIND;
        }
    }

    if (TS_SUCCESS != TS_MPI_VPSS_Start_Camera()) {
        SAMPLE_PRT("TS_MPI_VPSS_Start_Camera fail !\n");
    }

    enRcMode  = SAMPLE_RC_CBR;
    enGopMode = VENC_GOPMODE_NORMALP;
    s32Ret    = SAMPLE_COMM_VENC_GetGopAttr(enGopMode, &stGopAttr);
    if (TS_SUCCESS != s32Ret) {
        SAMPLE_PRT("Venc Get GopAttr for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

    TS_U32         u32FrameRate     = 13;
	TS_U32         u32FrameRate_360p     = 13;
    VENC_CHN       VencChn[4]       = { 0, 1, 2, 3 };
    PAYLOAD_TYPE_E szPayLoadType[2] = { PT_H265, PT_H265 };
    PIC_SIZE_E     szPicSize[]      = { PIC_1080P, PIC_360P };

    if (2304 == stViPipvSize.u32Width || 1296 == stViPipvSize.u32Height) {
        szPicSize[0] = PIC_2304x1296;
    }

    // vpss grp 0, venc chn 0/1
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[0], szPayLoadType[0], szPicSize[0], u32FrameRate, enRcMode, u32Profile, &stGopAttr);
    if (TS_SUCCESS != s32Ret) {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(0, 0, 0);
    if (TS_SUCCESS != s32Ret) {
        SAMPLE_PRT("Venc Get GopAttr failed for %#x!\n", s32Ret);
        goto EXIT_VENC0_STOP;
    }

#if 1  // 小码流
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[1], szPayLoadType[1], szPicSize[1], u32FrameRate_360p, enRcMode, u32Profile, &stGopAttr);
    if (TS_SUCCESS != s32Ret) {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VENC0_UnBind;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(0, 1, 1);
    if (TS_SUCCESS != s32Ret) {
        SAMPLE_PRT("Venc bind Vpss failed for %#x!\n", s32Ret);
        goto EXIT_VENC1_STOP;
    }
#endif

    // vpss grp 1, venc chn 2/3
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[2], szPayLoadType[0], szPicSize[0], u32FrameRate, enRcMode, u32Profile, &stGopAttr);
    if (TS_SUCCESS != s32Ret) {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VENC1_UnBind;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(1, 0, 2);
    if (TS_SUCCESS != s32Ret) {
        SAMPLE_PRT("Venc Get GopAttr failed for %#x!\n", s32Ret);
        goto EXIT_VENC2_STOP;
    }

#if 1  // 小码流
    s32Ret = SAMPLE_COMM_VENC_Start(VencChn[3], szPayLoadType[1], szPicSize[1], u32FrameRate_360p, enRcMode, u32Profile, &stGopAttr);
    if (TS_SUCCESS != s32Ret) {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VENC2_UnBind;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(1, 1, 3);
    if (TS_SUCCESS != s32Ret) {
        SAMPLE_PRT("Venc bind Vpss failed for %#x!\n", s32Ret);
        goto EXIT_VENC3_STOP;
    }
#endif

    s32Ret = SAMPLE_COMM_VENC_StartGetStreamV2(VencChn, sizeof(VencChn) / sizeof(VencChn[0]), SAMPLE_VENC_RTSP);
    if (TS_SUCCESS != s32Ret) {
        SAMPLE_PRT("Start Get Stream failed !.\n");
        goto EXIT_VENC3_UnBind;
    }

    // 退出
    SAMPLE_WAIT_Q_EXIT();

    /** exit process */
    SAMPLE_COMM_VENC_StopGetStream();

EXIT_VENC3_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(1, 1, 3);

EXIT_VENC3_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[3]);

EXIT_VENC2_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(1, 0, 2);

EXIT_VENC2_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[2]);

EXIT_VENC1_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(0, 1, 1);

EXIT_VENC1_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[1]);

EXIT_VENC0_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(0, 0, 0);

EXIT_VENC0_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[0]);

EXIT_VI_VPSS_UNBIND:
    if (TS_FALSE == bLowDelay) {
        SAMPLE_PRT("unbind vi and vpss\n");
        SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp[1]);
    }

EXIT_VI_STOP:
    SAMPLE_COMM_VI_StopVi(&stViConfig);
    SAMPLE_PRT("stop vi finish\n");

    for (TS_U8 i = 0; i < pstModeSetting->numCam; i++) {
        SAMPLE_COMM_VPSS_Stop(VpssGrp[i], VpssChnlNum);
        // SAMPLE_COMM_VPSS_Stop(VpssGrp[1], 1);
    }

    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

/**
 * @brief
 * @param argc
 * @param argv
 * @author Tsingmicro
 * @date 2024-12-18
 * @return
 */
int main(int argc, char *argv[])
{
    TS_S32 s32Ret = 0;
    TS_U32 u32Opt = 0;

    printf("Usage: %s [index] \n", argv[0]);
    SAMPLE_VENC_ShowModeInfo(0);

    char line_buf[1000];
    printf("\r\n=>");

    while (1) {
        memset(line_buf, 0, sizeof(line_buf));
        setbuf(stdin, NULL);
        if (fgets(line_buf, sizeof(line_buf), stdin) != NULL) {
            break;
        }

        if (strcmp(line_buf, "\n") == 0) {
            printf("\n=> ");
        }

        if (strstr(line_buf, "q") != NULL) {
            // break;
            exit(s32Ret);
        }
    }

    line_buf[strlen(line_buf) - 1] = '\0';

    u32Opt = atoi(line_buf);

    if (u32Opt > sizeof(g_sample_venc_modeSetting_list) / sizeof(MODE_SETTING_S) - 1) {
        SAMPLE_VENC_Usage(argv[0]);
        exit(TS_FAILURE);
    }

    printf("\nTips:\n");
    printf("  Press Ctrl+C to exit !.\n\n");

    signal(SIGINT, SAMPLE_VENC_HandleSig);
    signal(SIGTERM, SAMPLE_VENC_HandleSig);

    s_bIsRunning = TS_TRUE;

    MODE_SETTING_S stModeSetting = g_sample_venc_modeSetting_list[u32Opt];

    s32Ret = PDT_SAMPLE_2SNS_4STREAM_LOOP(&stModeSetting);
    if (TS_SUCCESS == s32Ret) {
        printf("program exit normally!\n");
    } else {
        printf("program exit abnormally!\n");
    }

    exit(s32Ret);
}
