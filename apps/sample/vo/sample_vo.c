#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdbool.h>

#include "ts_buffer.h"
#include "ts_comm_vo.h"
#include "mpi_vo.h"
#include "sample_comm_base.h"
#include "sample_comm_util.h"
#include "sample_comm_sys.h"
#include "sample_comm_vi.h"
#include "sample_comm_isp.h"
#include "sample_comm_vpss.h"
#include "sample_comm_venc.h"
#include "sample_comm_vdec.h"
#include "sample_comm_vo.h"

#define SAMPLE_STREAM_PATH   				"./"

static TS_S32 SAMPLE_COMM_VO_EXIT()
{
    TS_MPI_VO_DisableVideoLayer(SAMPLE_VO_LAYER_VHD0);
    TS_MPI_VO_Disable(SAMPLE_VO_DEV_UHD);

    return TS_SUCCESS;
}

static TS_VOID SAMPLE_VIO_MsgInit(TS_VOID)
{
}

static TS_VOID SAMPLE_VIO_MsgExit(TS_VOID)
{
}

static void SAMPLE_VIO_HandleSig(TS_S32 signo)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    if (SIGINT == signo || SIGTERM == signo)
    {
		VPSS_GRP VpssGrp = 0;
		VPSS_CHN VpssChn = 0;
		VO_LAYER VoLayer = 0;
		VO_CHN VoChn = 0;

		SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssChn, VoLayer, VoChn);
        SAMPLE_COMM_VO_EXIT();
        SAMPLE_COMM_VPSS_Stop(0, 1);
        SAMPLE_COMM_SYS_Exit();
        SAMPLE_PRT("\033[0;31mprogram termination abnormally!\033[0;39m\n");
    }
	exit(-1);
}

typedef enum tsVO_SCREEN_TYPE_E
{
    VO_SCREEN_RGB_240P_FORMAT_RGBA8888 = 0,
	VO_SCREEN_RGB_240P_FORMAT_YUV420,
	VO_SCREEN_MIPI_1080P_FORMAT_RGBA8888,
    VO_SCREEN_MIPI_1080P_FORMAT_YUV420,
    VO_SCREEN_NUM
}VO_SCREEN_TYPE_E;

typedef struct tsVO_SETTING_INFO
{
	SIZE_S  vpss_group_max_size;
	SIZE_S  vpss_chn_max_size;
	SIZE_S  vpss_chn_size;
    SIZE_S  vo_size;
    PIXEL_FORMAT_E input_pix_format;
    PIXEL_FORMAT_E output_pix_format;
}VO_SETTING_INFO;

static VO_SETTING_INFO g_vo_screen_setting_list[] = {
    {{240, 320}, {240, 320}, {240, 320}, {240, 320}, PIXEL_FORMAT_NV_12, PIXEL_FORMAT_BGRA_8888},
	{{1920, 1080}, {256, 320}, {240, 320}, {240, 320}, PIXEL_FORMAT_NV_12, PIXEL_FORMAT_YUV_PLANAR_420},
    {{1080, 1920}, {1080, 1920}, {1080, 1920}, {1080, 1920}, PIXEL_FORMAT_NV_12, PIXEL_FORMAT_BGRA_8888},
	{{1080, 1920}, {1080, 1920}, {1080, 1920}, {1080, 1920}, PIXEL_FORMAT_NV_12, PIXEL_FORMAT_YUV_PLANAR_420}
};

static TS_S32 SAMPLE_VO_System_Init()
{
    TS_S32              s32Ret;
    VB_CONFIG_S 		stVbConf;

    MPP_SYS_CONFIG_S pstSysConfig;
    pstSysConfig.u32Align = 8;
    TS_MPI_SYS_SetConfig(&pstSysConfig);

    stVbConf.astCommPool[0].u64BlkSize   = 1024;
    stVbConf.astCommPool[0].u32BlkCnt	 = 1;
    stVbConf.u32MaxPoolCnt = 1;

    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        return s32Ret;
    }
    

    return TS_SUCCESS;
}

static TS_S32 SAMPLE_VO_VPSS_Init(VPSS_GRP VpssGrp, VO_SETTING_INFO pPipeInfo, TS_S32 chnl_num)
{
    TS_S32 i;
    TS_S32 s32Ret;
    VPSS_GRP_ATTR_S stVpssGrpAttr;
    VPSS_CHN_ATTR_S stVpssChnAttr[4];

    stVpssGrpAttr.enGrpMode = VPSS_GRP_MODE_STREAM;
    stVpssGrpAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stVpssGrpAttr.enPixelFormat  = pPipeInfo.input_pix_format;
    stVpssGrpAttr.u32MaxW        = pPipeInfo.vpss_group_max_size.u32Width;
    stVpssGrpAttr.u32MaxH        = pPipeInfo.vpss_group_max_size.u32Height;
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;
    for(i = 0; i < chnl_num; i ++)
    {
        memset(&stVpssChnAttr[i], 0, sizeof(VPSS_CHN_ATTR_S));
        stVpssChnAttr[i].u32Width					  = pPipeInfo.vpss_chn_size.u32Width;
        stVpssChnAttr[i].u32Height					  = pPipeInfo.vpss_chn_size.u32Height;
        stVpssChnAttr[i].enChnMode					  = VPSS_CHN_MODE_USER;
        stVpssChnAttr[i].enCompressMode 			  = COMPRESS_MODE_NONE;
        stVpssChnAttr[i].enDynamicRange 			  = DYNAMIC_RANGE_SDR8;
        stVpssChnAttr[i].enVideoFormat				  = VIDEO_FORMAT_LINEAR;
        stVpssChnAttr[i].enPixelFormat				  = pPipeInfo.output_pix_format;
        stVpssChnAttr[i].stFrameRate.s32SrcFrameRate  = -1;
        stVpssChnAttr[i].stFrameRate.s32DstFrameRate  = -1;
        stVpssChnAttr[i].u32Depth					  = 3;
        stVpssChnAttr[i].bMirror						= TS_FALSE;
        stVpssChnAttr[i].bFlip							= TS_FALSE;
        stVpssChnAttr[i].enVideoFormat					= VIDEO_FORMAT_LINEAR;
        stVpssChnAttr[i].stAspectRatio.enMode			= ASPECT_RATIO_NONE;
        stVpssChnAttr[i].u32MaxW						= pPipeInfo.vpss_chn_max_size.u32Width;
        stVpssChnAttr[i].u32MaxH						= pPipeInfo.vpss_chn_max_size.u32Height;
        stVpssChnAttr[i].u32MemCount 					= 3;
        stVpssChnAttr[i].bVpuLoop						= TS_FALSE;
    }

    s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, &stVpssGrpAttr,  stVpssChnAttr, chnl_num);
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("start VPSS fail for %#x!\n", s32Ret);
    }
    return s32Ret;
}

static TS_FLOAT SAMPLE_VO_GetBitSizeByFmt(PIXEL_FORMAT_E pixel_fmt)
{
    TS_FLOAT fVal = 0;
    switch (pixel_fmt)
    {
        case PIXEL_FORMAT_BGRA_8888:
            fVal = 4;
        break;
        case PIXEL_FORMAT_RGB_565:
        case PIXEL_FORMAT_YUV_PLANAR_422:
            fVal = 2;
        break;

        case PIXEL_FORMAT_NV_12:
        case PIXEL_FORMAT_NV_21:
        case PIXEL_FORMAT_YUV_PLANAR_420:
            fVal = 1.5;
        break;
        default:
        break;
    }
	return fVal;
}

static TS_S32 SAMPLE_ReleaseVb(TS_U64 phy_addr)
{
    VB_BLK blk_id =  TS_MPI_VB_PhysAddr2Handle(phy_addr);
    VB_POOL pool_id = TS_MPI_VB_Handle2PoolId(blk_id);
    TS_S32 ret = TS_MPI_VB_MunmapPool(pool_id);
    ret = TS_MPI_VB_DestroyPool(pool_id);
    return ret;
}

static TS_S32 SAMPLE_Vo_CreateVb(VIDEO_FRAME_INFO_S *pstVideoFrame)
{
    VB_POOL_CONFIG_S stVbPoolCfg;
    uint32_t vbPool = 0;
    void *viraddr = NULL;

    TS_S32 nlinesize = pstVideoFrame->stVFrame.u32Width * pstVideoFrame->stVFrame.u32Height;
    TS_S32 nsize = nlinesize * SAMPLE_VO_GetBitSizeByFmt(pstVideoFrame->stVFrame.enPixelFormat);
    stVbPoolCfg.u64BlkSize = nsize;
    stVbPoolCfg.u32BlkCnt = 2;
    vbPool = TS_MPI_VB_CreatePool(&stVbPoolCfg);
    VB_BLK VbHandle = TS_MPI_VB_GetBlock(vbPool, nsize, TS_NULL);
    if (VB_INVALID_HANDLE == VbHandle)
    {
        TS_MPI_VB_DestroyPool(vbPool);
        printf("TS_MPI_VB_GetBlock failed!\n");
        return -2;
    }
    pstVideoFrame->stVFrame.u64PhyAddr[0] = TS_MPI_VB_Handle2PhysAddr(VbHandle);
    if(pstVideoFrame->stVFrame.u64PhyAddr[0] == 0)
    {
        TS_MPI_VB_ReleaseBlock(VbHandle);
        TS_MPI_VB_DestroyPool(vbPool);
        return -3;
    }
    TS_S32 ret = TS_MPI_VB_MmapPool(vbPool);
    if(ret != 0)
    {
        TS_MPI_VB_ReleaseBlock(VbHandle);
        TS_MPI_VB_DestroyPool(vbPool);
        return -4;
    }

    ret = TS_MPI_VB_GetBlockVirAddr(vbPool, pstVideoFrame->stVFrame.u64PhyAddr[0], &viraddr);
    if(ret != 0)
    {
        SAMPLE_ReleaseVb(pstVideoFrame->stVFrame.u64PhyAddr[0]);
        return -5;
    }
    pstVideoFrame->stVFrame.u64VirAddr[0] = (TS_U64)(uintptr_t)viraddr;
    printf("SAMPLE_Vo_CreateVb phyaddr 0x%llx viraddr 0x%llx\n", pstVideoFrame->stVFrame.u64PhyAddr[0], pstVideoFrame->stVFrame.u64VirAddr[0]);
    return 0;
}

static TS_S32 SAMPLE_COMM_VO_StartVO_MIPI_1080P(VO_SETTING_INFO info)
{
    TS_S32			s32Ret;

    s32Ret = TS_MPI_VO_Enable(SAMPLE_VO_DEV_UHD);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo dev enable failed. s32Ret: 0x%x !\n", s32Ret);
        return TS_FAILURE;
    }

    VO_VIDEO_LAYER_ATTR_S pstVoLayerPubAttr;
    TS_MPI_VO_GetVideoLayerAttr(SAMPLE_VO_LAYER_VHD0, &pstVoLayerPubAttr);
    pstVoLayerPubAttr.stDispRect.s32X = 0;
    pstVoLayerPubAttr.stDispRect.s32Y = 0;
    pstVoLayerPubAttr.stDispRect.u32Width = info.vo_size.u32Width;
    pstVoLayerPubAttr.stDispRect.u32Height = info.vo_size.u32Height;
    pstVoLayerPubAttr.enPixFormat = info.output_pix_format;

    pstVoLayerPubAttr.stImageSize.u32Width = info.vo_size.u32Width;
    pstVoLayerPubAttr.stImageSize.u32Height = info.vo_size.u32Height;
    s32Ret = TS_MPI_VO_SetVideoLayerAttr(SAMPLE_VO_LAYER_VHD0, &pstVoLayerPubAttr);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo set layer attr failed. s32Ret: 0x%x !\n", s32Ret);
        return TS_FAILURE;
    }

    s32Ret = TS_MPI_VO_EnableVideoLayer(SAMPLE_VO_LAYER_VHD0);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo enable layer attr failed. s32Ret: 0x%x !\n", s32Ret);
        return TS_FAILURE;
    }

    return TS_SUCCESS;
}

static TS_S32 SAMPLE_COMM_VO_StartVO_RGB_240P(  VO_SETTING_INFO info)
{
    TS_S32			s32Ret;

    s32Ret = TS_MPI_VO_Enable(SAMPLE_VO_DEV_UHD);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo dev enable failed. s32Ret: 0x%x !\n", s32Ret);
        return TS_FAILURE;
    }

    VO_VIDEO_LAYER_ATTR_S pstVoLayerPubAttr;
    TS_MPI_VO_GetVideoLayerAttr(SAMPLE_VO_LAYER_VHD0, &pstVoLayerPubAttr);
    pstVoLayerPubAttr.stDispRect.s32X = 0;
    pstVoLayerPubAttr.stDispRect.s32Y = 0;
    pstVoLayerPubAttr.stDispRect.u32Width = info.vo_size.u32Width;
    pstVoLayerPubAttr.stDispRect.u32Height = info.vo_size.u32Height;
    pstVoLayerPubAttr.enPixFormat = info.output_pix_format;

    pstVoLayerPubAttr.stImageSize.u32Width = info.vo_size.u32Width;
    pstVoLayerPubAttr.stImageSize.u32Height = info.vo_size.u32Height;
    s32Ret = TS_MPI_VO_SetVideoLayerAttr(SAMPLE_VO_LAYER_VHD0, &pstVoLayerPubAttr);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo set layer attr failed. s32Ret: 0x%x !\n", s32Ret);
        return TS_FAILURE;
    }

    s32Ret = TS_MPI_VO_EnableVideoLayer(SAMPLE_VO_LAYER_VHD0);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo enable layer attr failed. s32Ret: 0x%x !\n", s32Ret);
        return TS_FAILURE;
    }

	VO_CHN_ATTR_S pstChnAttr;
	memset(&pstChnAttr, 0, sizeof(VO_CHN_ATTR_S));

	pstChnAttr.u32Priority = 100;
	TS_MPI_VO_SetChnAttr(SAMPLE_VO_LAYER_VHD0, 0, &pstChnAttr);

	memset(&pstChnAttr, 0, sizeof(VO_CHN_ATTR_S));

	TS_MPI_VO_GetChnAttr(SAMPLE_VO_LAYER_VHD0, 0, &pstChnAttr);
	

    return TS_SUCCESS;
}
#if 0
TS_S32 SAMPLE_H265_VDEC_VPSS_VO(TS_VOID)
{
    VB_CONFIG_S stVbConfig;
    TS_S32 i, s32Ret = TS_SUCCESS;
    VDEC_THREAD_PARAM_S stVdecSend[VDEC_MAX_CHN_NUM];
    SIZE_S stDispSize;
    VO_LAYER VoLayer;
    TS_U32 u32VdecChnNum, VpssGrpNum;
    VPSS_GRP VpssGrp;
    pthread_t   VdecThread[2*VDEC_MAX_CHN_NUM];
    PIC_SIZE_E enDispPicSize;
    SAMPLE_VDEC_ATTR astSampleVdec[VDEC_MAX_CHN_NUM] = {0};
    VPSS_CHN_ATTR_S astVpssChnAttr[VPSS_MAX_CHN_NUM] = {0};
    SAMPLE_VO_CONFIG_S stVoConfig;
    VPSS_GRP_ATTR_S stVpssGrpAttr;
    TS_BOOL abChnEnable[VPSS_MAX_CHN_NUM];
    VO_INTF_SYNC_E enIntfSync;
    VO_INTF_TYPE_E enVoIntfType;
	VO_SETTING_INFO screen_set = g_vo_screen_setting_list[VO_SCREEN_RGB_240P_FORMAT_RGBA8888];

    u32VdecChnNum = 1;
    VpssGrpNum    = u32VdecChnNum;
    /************************************************
    step1:  init SYS, init common VB(for VPSS and VO)
    *************************************************/

    enDispPicSize = PIC_1080P;
    enIntfSync    = VO_OUTPUT_1080P60;
    enVoIntfType  = VO_INTF_HDMI;

    s32Ret =  SAMPLE_COMM_SYS_GetPicSize(enDispPicSize, &stDispSize);
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("sys get pic size fail for %#x!\n", s32Ret);
        goto END1;
    }

    memset(&stVbConfig, 0, sizeof(VB_CONFIG_S));
    stVbConfig.u32MaxPoolCnt             = 1;
    stVbConfig.astCommPool[0].u32BlkCnt  = 10*u32VdecChnNum;
    stVbConfig.astCommPool[0].u64BlkSize = COMMON_GetPicBufferSize(stDispSize.u32Width, stDispSize.u32Height,
                                                PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_SEG, 0);
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConfig);
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("init sys fail for %#x!\n", s32Ret);
        goto END1;
    }

    /************************************************
    step2:  init module VB or user VB(for VDEC)
    *************************************************/
    for(i=0; i<u32VdecChnNum; i++)
    {
        astSampleVdec[i].enType                           = PT_H265;
        astSampleVdec[i].u32Width                         = 1920;
        astSampleVdec[i].u32Height                        = 1080;
        astSampleVdec[i].enMode                           = VIDEO_MODE_FRAME;
        astSampleVdec[i].stSapmleVdecVideo.enDecMode      = VIDEO_DEC_MODE_IP;
        astSampleVdec[i].stSapmleVdecVideo.enBitWidth     = DATA_BITWIDTH_8;
        astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum = 2;
        astSampleVdec[i].u32DisplayFrameNum               = 2;
        astSampleVdec[i].u32FrameBufCnt = astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum + astSampleVdec[i].u32DisplayFrameNum + 1;
    }
    s32Ret = SAMPLE_COMM_VDEC_InitVBPool(u32VdecChnNum, &astSampleVdec[0]);
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("init mod common vb fail for %#x!\n", s32Ret);
        goto END2;
    }

    /************************************************
    step3:  start VDEC
    *************************************************/
    s32Ret = SAMPLE_COMM_VDEC_Start(u32VdecChnNum, &astSampleVdec[0]);
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("start VDEC fail for %#x!\n", s32Ret);
        goto END3;
    }
#if 0
    /************************************************
    step4:  start VPSS
    *************************************************/
    stVpssGrpAttr.enGrpMode = VPSS_GRP_MODE_STREAM;
    stVpssGrpAttr.u32MaxW = 2688;
    stVpssGrpAttr.u32MaxH = 2160;
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;
    stVpssGrpAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stVpssGrpAttr.enPixelFormat  = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    memset(abChnEnable, 0, sizeof(abChnEnable));
    abChnEnable[1] = TS_TRUE;
    astVpssChnAttr[1].u32Width                    = stDispSize.u32Width;
    astVpssChnAttr[1].u32Height                   = stDispSize.u32Height;
    astVpssChnAttr[1].enChnMode                   = VPSS_CHN_MODE_USER;
    astVpssChnAttr[1].enCompressMode              = COMPRESS_MODE_NONE;
    astVpssChnAttr[1].enDynamicRange              = DYNAMIC_RANGE_SDR8;
    astVpssChnAttr[1].enPixelFormat               = PIXEL_FORMAT_BGRA_8888;
    astVpssChnAttr[1].stFrameRate.s32SrcFrameRate = -1;
    astVpssChnAttr[1].stFrameRate.s32DstFrameRate = -1;
    astVpssChnAttr[1].u32Depth                    = 0;
	astVpssChnAttr[1].u32MemCount 					= 3;
    astVpssChnAttr[1].bMirror                     = TS_FALSE;
    astVpssChnAttr[1].bFlip                       = TS_FALSE;
    astVpssChnAttr[1].stAspectRatio.enMode        = ASPECT_RATIO_NONE;
    astVpssChnAttr[1].enVideoFormat               = VIDEO_FORMAT_LINEAR;
    for(i=0; i<u32VdecChnNum; i++)
    {
        VpssGrp = i;
	    s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, &abChnEnable[0], &stVpssGrpAttr, &astVpssChnAttr[0]);
	    if(s32Ret != TS_SUCCESS)
	    {
	        SAMPLE_PRT("start VPSS fail for %#x!\n", s32Ret);
	        goto END4;
	    }
    }

    /************************************************
    step6:  VDEC bind VPSS
    *************************************************/
    for(i=0; i<u32VdecChnNum; i++)
    {
        s32Ret = SAMPLE_COMM_VDEC_Bind_VPSS(i, i);
        if(s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("vdec bind vpss fail for %#x!\n", s32Ret);
            goto END6;
        }
    }
#endif
	/************************************************
	step5:	start VO
	*************************************************/
	s32Ret = SAMPLE_COMM_VO_StartVO_RGB_240P(screen_set);
	if(s32Ret != TS_SUCCESS)
	{
		SAMPLE_PRT("start VO fail for %#x!\n", s32Ret);
		goto END5;
	}

    /************************************************
    step7:  VPSS bind VO
    *************************************************/
    VoLayer = stVoConfig.VoDev;
    for(i=0; i<VpssGrpNum; i++)
    {
        s32Ret = SAMPLE_COMM_VDEC_Bind_VO(i, 1, VoLayer, i);
        if(s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("vpss bind vo fail for %#x!\n", s32Ret);
            goto END7;
        }
    }

    /************************************************
    step8:  send stream to VDEC
    *************************************************/
    for(i=0; i<u32VdecChnNum; i++)
    {
        snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), "1080P.h265");
        snprintf(stVdecSend[i].cFilePath, sizeof(stVdecSend[i].cFilePath), "%s", SAMPLE_STREAM_PATH);
        stVdecSend[i].enType          = astSampleVdec[i].enType;
        stVdecSend[i].s32StreamMode   = astSampleVdec[i].enMode;
        stVdecSend[i].s32ChnId        = i;
        stVdecSend[i].s32IntervalTime = 1000;
        stVdecSend[i].u64PtsInit      = 0;
        stVdecSend[i].u64PtsIncrease  = 0;
        stVdecSend[i].eThreadCtrl     = THREAD_CTRL_START;
        stVdecSend[i].bCircleSend     = TS_TRUE;
        stVdecSend[i].s32MilliSec     = 0;
        stVdecSend[i].s32MinBufSize   = (astSampleVdec[i].u32Width * astSampleVdec[i].u32Height * 3)>>1;
    }
    SAMPLE_COMM_VDEC_StartSendStream(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);

    SAMPLE_COMM_VDEC_CmdCtrl(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);

    SAMPLE_COMM_VDEC_StopSendStream(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);

END7:
    for(i=0; i<VpssGrpNum; i++)
    {
        s32Ret = SAMPLE_COMM_VPSS_UnBind_VO(i, 1, VoLayer, i);
        if(s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("vpss unbind vo fail for %#x!\n", s32Ret);
        }
    }

END6:
    for(i=0; i<u32VdecChnNum; i++)
    {
        s32Ret = SAMPLE_COMM_VDEC_UnBind_VPSS(i, i);
        if(s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("vdec unbind vpss fail for %#x!\n", s32Ret);
        }
    }

END5:
    SAMPLE_COMM_VO_StopVO(&stVoConfig);

END4:
    for(i = VpssGrp; i >= 0; i--)
    {
        VpssGrp = i;
        SAMPLE_COMM_VPSS_Stop(VpssGrp, &abChnEnable[0]);
    }
END3:
    SAMPLE_COMM_VDEC_Stop(u32VdecChnNum);

END2:
    SAMPLE_COMM_VDEC_ExitVBPool();

END1:
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}
#endif

#define	SAMPLE_VENC_CAM_NUM	1

static TS_S32 SAMPLE_VO_VPSS_MIPI_1080P(VO_SCREEN_TYPE_E type)
{
    TS_S32          s32Ret, i;
    VO_SETTING_INFO screen_set = g_vo_screen_setting_list[type];

    SAMPLE_VI_CONFIG_S stViConfig = {0};

    VPSS_GRP        VpssGrp[SAMPLE_VENC_CAM_NUM]    = {0};
    TS_S32          VpssChnlNum = 1;
    int             index = 0;

    s32Ret = SAMPLE_VO_System_Init(); 
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("Init System err for %#x!\n", s32Ret);
        return TS_FAILURE;
    }

    s32Ret = SAMPLE_VO_VPSS_Init(VpssGrp[0], screen_set, VpssChnlNum);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Init VPSS group[%d] err for %#x!\n", VpssGrp[0], s32Ret);
        goto EXIT_VPSS_STOP;
    }
    SAMPLE_PRT("Init VPSS group[%d] success!\n", VpssGrp[0]);

    /*mipi param setting */
    s32Ret = SAMPLE_COMM_VO_SetSceenParam(VO_SCREEN_TYPE_MIPI);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_SetSceenParam for mipi failed, s32Ret: 0x%x !\n", s32Ret);
        goto EXIT_VO_STOP;
    }

    /*start vo*/
    s32Ret = SAMPLE_COMM_VO_StartVO_MIPI_1080P(screen_set);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT_VO_STOP;
    }

    /*vpss bind vo*/
    s32Ret = SAMPLE_COMM_VPSS_Bind_VO(VpssGrp[0], 0, SAMPLE_VO_LAYER_VHD0, 0);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT_VO_STOP;
    }

    VIDEO_FRAME_INFO_S pstVideoFrame = {0};
    memset(&pstVideoFrame, 0, sizeof(VIDEO_FRAME_INFO_S));
    pstVideoFrame.stVFrame.enPixelFormat = screen_set.input_pix_format;
    pstVideoFrame.stVFrame.u32Width = screen_set.vpss_group_max_size.u32Width;
    pstVideoFrame.stVFrame.u32Height = screen_set.vpss_group_max_size.u32Height;
    pstVideoFrame.stVFrame.size = pstVideoFrame.stVFrame.u32Width * pstVideoFrame.stVFrame.u32Height * SAMPLE_VO_GetBitSizeByFmt(pstVideoFrame.stVFrame.enPixelFormat);

    s32Ret = SAMPLE_Vo_CreateVb(&pstVideoFrame);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo create vb fail. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT_VO_STOP;
    }
    VB_BLK blk_id = TS_MPI_VB_PhysAddr2Handle(pstVideoFrame.stVFrame.u64PhyAddr[0]);
    pstVideoFrame.u32PoolId = TS_MPI_VB_Handle2PoolId(blk_id);

    FILE *pFileRead = fopen("./1080x1920_nv12_01.yuv", "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file\n");
        return -1;
    }

    uintptr_t addr = (uintptr_t)pstVideoFrame.stVFrame.u64VirAddr[0];
    fread((char*)addr, 1, pstVideoFrame.stVFrame.size, pFileRead);
    fclose(pFileRead);

    VIDEO_FRAME_INFO_S pstVideoFrame1 = {0};
    memset(&pstVideoFrame1, 0, sizeof(VIDEO_FRAME_INFO_S));
    pstVideoFrame1.stVFrame.enPixelFormat = screen_set.input_pix_format;
    pstVideoFrame1.stVFrame.u32Width = screen_set.vpss_group_max_size.u32Width;
    pstVideoFrame1.stVFrame.u32Height = screen_set.vpss_group_max_size.u32Height;
    pstVideoFrame1.stVFrame.size = pstVideoFrame1.stVFrame.u32Width * pstVideoFrame1.stVFrame.u32Height * SAMPLE_VO_GetBitSizeByFmt(pstVideoFrame1.stVFrame.enPixelFormat);

    s32Ret = SAMPLE_Vo_CreateVb(&pstVideoFrame1);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo create vb fail. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT_VO_STOP;
    }
    blk_id = TS_MPI_VB_PhysAddr2Handle(pstVideoFrame1.stVFrame.u64PhyAddr[0]);
    pstVideoFrame1.u32PoolId = TS_MPI_VB_Handle2PoolId(blk_id);

    FILE *pFileRead1 = fopen("./1080x1920_nv12_02.yuv", "rb");
    if (TS_NULL == pFileRead1)
    {
        SAMPLE_PRT("can't open file\n");
        return -1;
    }

    addr = (uintptr_t)pstVideoFrame1.stVFrame.u64VirAddr[0];
    fread((char*)addr, 1, pstVideoFrame1.stVFrame.size, pFileRead1);
    fclose(pFileRead1);

    while(1)
    {
        index ++;
        if(index % 2 == 0)
        {
            TS_MPI_VPSS_SendFrame(VpssGrp[0], 0, &pstVideoFrame, 100);
        }
        else
        {
            TS_MPI_VPSS_SendFrame(VpssGrp[0], 0, &pstVideoFrame1, 100);
        }
        usleep(1000 * 1000);
    }
EXIT_VO_STOP:
    SAMPLE_COMM_VO_EXIT();

EXIT_VPSS_STOP:
    for(i = 0; i <  stViConfig.s32WorkingViNum; i ++)
    {
        SAMPLE_COMM_VPSS_Stop(VpssGrp[i], VpssChnlNum);
    }
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}


static TS_S32 SAMPLE_VO_VPSS_RGB_240P(VO_SCREEN_TYPE_E type)
{
    TS_S32          s32Ret, i;
    VO_SETTING_INFO screen_set = g_vo_screen_setting_list[type];

    SAMPLE_VI_CONFIG_S stViConfig = {0};

    VPSS_GRP        VpssGrp[SAMPLE_VENC_CAM_NUM]    = {0};
    TS_S32          VpssChnlNum = 1;
    int             index = 0;

    s32Ret = SAMPLE_VO_System_Init(); 
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("Init System err for %#x!\n", s32Ret);
        return TS_FAILURE;
    }

    s32Ret = SAMPLE_VO_VPSS_Init(VpssGrp[0], screen_set, VpssChnlNum);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Init VPSS group[%d] err for %#x!\n", VpssGrp[0], s32Ret);
        goto EXIT_VPSS_STOP;
    }
    SAMPLE_PRT("Init VPSS group[%d] success!\n", VpssGrp[0]);

    /*rgb param setting */
    s32Ret = SAMPLE_COMM_VO_SetSceenParam(VO_SCREEN_TYPE_RGB);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_SetSceenParam for rgb failed, s32Ret: 0x%x !\n", s32Ret);
        goto EXIT_VPSS_STOP;
    }

    /*start vo*/
    s32Ret = SAMPLE_COMM_VO_StartVO_RGB_240P(screen_set);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT_VO_STOP;
    }

    /*vpss bind vo*/
    s32Ret = SAMPLE_COMM_VPSS_Bind_VO(VpssGrp[0], 0, SAMPLE_VO_LAYER_VHD0, 0);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT_VO_STOP;
    }

    VIDEO_FRAME_INFO_S pstVideoFrame = {0};
    memset(&pstVideoFrame, 0, sizeof(VIDEO_FRAME_INFO_S));
    pstVideoFrame.stVFrame.enPixelFormat = screen_set.input_pix_format;
    pstVideoFrame.stVFrame.u32Width = screen_set.vpss_group_max_size.u32Width;
    pstVideoFrame.stVFrame.u32Height = screen_set.vpss_group_max_size.u32Height;
    pstVideoFrame.stVFrame.size = pstVideoFrame.stVFrame.u32Width * pstVideoFrame.stVFrame.u32Height * SAMPLE_VO_GetBitSizeByFmt(pstVideoFrame.stVFrame.enPixelFormat);

    s32Ret = SAMPLE_Vo_CreateVb(&pstVideoFrame);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT_VO_STOP;
    }
    VB_BLK blk_id = TS_MPI_VB_PhysAddr2Handle(pstVideoFrame.stVFrame.u64PhyAddr[0]);
    pstVideoFrame.u32PoolId = TS_MPI_VB_Handle2PoolId(blk_id);

    FILE *pFileRead = fopen("./240x320_nv12_01.yuv", "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file\n");
        return -1;
    }

    uintptr_t addr = (uintptr_t)pstVideoFrame.stVFrame.u64VirAddr[0];
    fread((char*)addr, 1, pstVideoFrame.stVFrame.size, pFileRead);
    fclose(pFileRead);

    VIDEO_FRAME_INFO_S pstVideoFrame1 = {0};
    memset(&pstVideoFrame1, 0, sizeof(VIDEO_FRAME_INFO_S));
    pstVideoFrame1.stVFrame.enPixelFormat = screen_set.input_pix_format;
    pstVideoFrame1.stVFrame.u32Width = screen_set.vpss_group_max_size.u32Width;
    pstVideoFrame1.stVFrame.u32Height = screen_set.vpss_group_max_size.u32Height;
    pstVideoFrame1.stVFrame.size = pstVideoFrame1.stVFrame.u32Width * pstVideoFrame1.stVFrame.u32Height * SAMPLE_VO_GetBitSizeByFmt(pstVideoFrame1.stVFrame.enPixelFormat);

    s32Ret = SAMPLE_Vo_CreateVb(&pstVideoFrame1);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT_VO_STOP;
    }
    blk_id = TS_MPI_VB_PhysAddr2Handle(pstVideoFrame1.stVFrame.u64PhyAddr[0]);
    pstVideoFrame1.u32PoolId = TS_MPI_VB_Handle2PoolId(blk_id);

    FILE *pFileRead1 = fopen("./240x320_nv12_02.yuv", "rb"); 
    if (TS_NULL == pFileRead1)
    {
        SAMPLE_PRT("can't open file\n");  
        return -1;
    }

    addr = (uintptr_t)pstVideoFrame1.stVFrame.u64VirAddr[0];
    fread((char*)addr, 1, pstVideoFrame1.stVFrame.size, pFileRead1);
    fclose(pFileRead1);

    while(1)
    {
        index ++;
        if(index % 2 == 0)
        {
            TS_MPI_VPSS_SendFrame(VpssGrp[0], 0, &pstVideoFrame, 100);
        }
        else
        {
            TS_MPI_VPSS_SendFrame(VpssGrp[0], 0, &pstVideoFrame1, 100);
        }
        usleep(1000 * 1000);
    }

	return s32Ret;
EXIT_VO_STOP:
    SAMPLE_COMM_VO_EXIT();

EXIT_VPSS_STOP:
	for(i = 0; i <  stViConfig.s32WorkingViNum; i ++)
    {
        SAMPLE_COMM_VPSS_Stop(VpssGrp[i], VpssChnlNum);
    }
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

static TS_S32 SAMPLE_H264_H265_VDEC_VO(TS_U8 VdecType)
{
    TS_S32 i, s32Ret = TS_SUCCESS;
    VDEC_THREAD_PARAM_S stVdecSend[VDEC_MAX_CHN_NUM];
	SIZE_S stVdecSize;
    SIZE_S stDispSize;
    VO_LAYER VoLayer;
    TS_U32 u32VdecChnNum, VpssGrpNum, VpssChnNum;
    VPSS_GRP VpssGrp;
    pthread_t   VdecThread[2*VDEC_MAX_CHN_NUM];
    SAMPLE_VDEC_ATTR astSampleVdec[VDEC_MAX_CHN_NUM] = {0};
    VPSS_CHN_ATTR_S astVpssChnAttr[VPSS_MAX_CHN_NUM] = {0};
    SAMPLE_VO_CONFIG_S stVoConfig;
    VPSS_GRP_ATTR_S stVpssGrpAttr;

    u32VdecChnNum = 1;
    VpssGrpNum    = u32VdecChnNum;
	VpssChnNum    = 1;

    /************************************************
    step1:  init SYS, init common VB(for VPSS and VO)
    *************************************************/
    PIC_SIZE_E enDispPicSize;
	enDispPicSize = PIC_240x320;
	//enVoIntfType  = VO_INTF_HDMI;

    s32Ret =  SAMPLE_COMM_SYS_GetPicSize(enDispPicSize, &stDispSize);
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("sys get pic size fail for %#x!\n", s32Ret);
        goto END1;
    }

    PIC_SIZE_E enVdecPicSize;
    enVdecPicSize = PIC_1080P;
	s32Ret =  SAMPLE_COMM_SYS_GetPicSize(enVdecPicSize, &stVdecSize);
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("sys get pic size fail for %#x!\n", s32Ret);
        goto END1;
    }

    VB_CONFIG_S stVbConfig;
    memset(&stVbConfig, 0, sizeof(VB_CONFIG_S));
    stVbConfig.u32MaxPoolCnt             = 2;
    stVbConfig.astCommPool[0].u32BlkCnt  = 20 * u32VdecChnNum;
    stVbConfig.astCommPool[0].u64BlkSize = COMMON_GetPicBufferSize(stVdecSize.u32Width, stVdecSize.u32Height,
                                                PIXEL_FORMAT_NV_12, DATA_BITWIDTH_8, COMPRESS_MODE_SEG, 0);

    stVbConfig.astCommPool[1].u32BlkCnt  = 15;
    stVbConfig.astCommPool[1].u64BlkSize = stVbConfig.astCommPool[0].u64BlkSize;

    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConfig);
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("init sys fail for %#x!\n", s32Ret);
        goto END1;
    }    
	SAMPLE_PRT("step1>>>: SAMPLE_COMM_SYS_Init success !\n");

    /************************************************
    step2:  init module VB or user VB(for VDEC)
    *************************************************/
    for(i=0; i<u32VdecChnNum; i++)
    {
        if (0 ==VdecType) {
            astSampleVdec[i].enType                           = PT_H265;
			astSampleVdec[i].stSapmleVdecVideo.enDecMode	  = VIDEO_DEC_MODE_IP;
			astSampleVdec[i].stSapmleVdecVideo.enBitWidth	  = DATA_BITWIDTH_8;
			astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum = 5;
        } else if (1 == VdecType) {
            astSampleVdec[i].enType                           = PT_H264;
			astSampleVdec[i].stSapmleVdecVideo.enDecMode	  = VIDEO_DEC_MODE_IP;
			astSampleVdec[i].stSapmleVdecVideo.enBitWidth	  = DATA_BITWIDTH_8;
			astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum = 5;
        } else if (2 == VdecType) {
            astSampleVdec[i].enType                           = PT_JPEG;
			astSampleVdec[i].stSapmleVdecPicture.enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
			astSampleVdec[i].stSapmleVdecPicture.u32Alpha = 255;
        }

        astSampleVdec[i].u32Width                         = stVdecSize.u32Width;
        astSampleVdec[i].u32Height                        = stVdecSize.u32Height;
        astSampleVdec[i].enMode                           = VIDEO_MODE_FRAME;
        astSampleVdec[i].u32DisplayFrameNum               = 5;
        astSampleVdec[i].u32FrameBufCnt = astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum + astSampleVdec[i].u32DisplayFrameNum + 1;
    }
    s32Ret = SAMPLE_COMM_VDEC_InitVBPool(u32VdecChnNum, &astSampleVdec[0]);
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("init mod common vb fail for %#x!\n", s32Ret);
        goto END2;
    }

    /************************************************
    step3:  start VDEC
    *************************************************/
    s32Ret = SAMPLE_COMM_VDEC_Start(u32VdecChnNum, &astSampleVdec[0]);
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("start VDEC fail for %#x!\n", s32Ret);
        goto END3;
    }

	/************************************************
	step4:	start VPSS
	*************************************************/
	memset(&astVpssChnAttr[i], 0, sizeof(VPSS_CHN_ATTR_S));
	stVpssGrpAttr.enGrpMode = VPSS_GRP_MODE_STREAM;
	stVpssGrpAttr.u32MaxW = 1920;
	stVpssGrpAttr.u32MaxH = 1080;
	stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;
	stVpssGrpAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
	stVpssGrpAttr.enPixelFormat  = PIXEL_FORMAT_NV_12;

	for(i = 0; i < VpssChnNum; i ++)
	{
		astVpssChnAttr[i].u32Width					  = stDispSize.u32Width;
		astVpssChnAttr[i].u32Height 				  = stDispSize.u32Height;
		astVpssChnAttr[i].enChnMode 				  = VPSS_CHN_MODE_USER;
		astVpssChnAttr[i].enCompressMode			  = COMPRESS_MODE_NONE;
		astVpssChnAttr[i].enDynamicRange			  = DYNAMIC_RANGE_SDR8;
		astVpssChnAttr[i].enVideoFormat 			  = VIDEO_FORMAT_LINEAR;
		astVpssChnAttr[i].enPixelFormat 			  = PIXEL_FORMAT_YUV_PLANAR_420;
		astVpssChnAttr[i].stFrameRate.s32SrcFrameRate  = -1;
		astVpssChnAttr[i].stFrameRate.s32DstFrameRate  = -1;
		astVpssChnAttr[i].u32Depth					  = 3;
		astVpssChnAttr[i].bMirror						= TS_FALSE;
		astVpssChnAttr[i].bFlip 						= TS_FALSE;
		astVpssChnAttr[i].enVideoFormat 				= VIDEO_FORMAT_LINEAR;
		astVpssChnAttr[i].stAspectRatio.enMode			= ASPECT_RATIO_NONE;
		astVpssChnAttr[i].u32MaxW						= stDispSize.u32Width + 32;
		astVpssChnAttr[i].u32MaxH						= stDispSize.u32Height;
		astVpssChnAttr[i].u32MemCount					= 3;
		astVpssChnAttr[i].bVpuLoop						= TS_FALSE;
	}
	for(i=0; i<u32VdecChnNum; i++)
	{
		VpssGrp = i;
		s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, &stVpssGrpAttr, &astVpssChnAttr[0], VpssChnNum);
		if(s32Ret != TS_SUCCESS)
		{
			SAMPLE_PRT("start VPSS fail for %#x!\n", s32Ret);
			goto END4;
		}
	}

    
	/************************************************
	step6:	VDEC bind VPSS
	*************************************************/
	for(i=0; i<u32VdecChnNum; i++)
	{
		s32Ret = SAMPLE_COMM_VDEC_Bind_VPSS(i, i);
		if(s32Ret != TS_SUCCESS)
		{
			SAMPLE_PRT("vdec bind vpss fail for %#x!\n", s32Ret);
			goto END5;
		}
	}

	/************************************************
	step7:	start VO
	*************************************************/
	stVoConfig.VoDev = SAMPLE_VO_DEV_UHD;
	stVoConfig.VoLayer = SAMPLE_VO_LAYER_VHD0;
	stVoConfig.enDstDynamicRange = DYNAMIC_RANGE_SDR8;

    stVoConfig.stDispRect.s32X       = 0;
    stVoConfig.stDispRect.s32Y       = 0;
    stVoConfig.stDispRect.u32Width   = stDispSize.u32Width;
    stVoConfig.stDispRect.u32Height  = stDispSize.u32Height;
    stVoConfig.stImageSize.u32Width  = stDispSize.u32Width;
    stVoConfig.stImageSize.u32Height = stDispSize.u32Height;
	stVoConfig.enPixFormat = PIXEL_FORMAT_YUV_PLANAR_420;
    
    /*rgb param setting */
    s32Ret = SAMPLE_COMM_VO_SetSceenParam(VO_SCREEN_TYPE_RGB);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_SetSceenParam for rgb failed, s32Ret: 0x%x !\n", s32Ret);
        goto END5;
    }

	/*start vo*/
	s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("start vo failed. s32Ret: 0x%x !\n", s32Ret);
		goto END6;
	}
	SAMPLE_PRT("start vo success1 !\n");

	/************************************************
	step8:	VPSS bind VO
	*************************************************/
	VoLayer = stVoConfig.VoDev;
	for(i=0; i<VpssGrpNum; i++)
	{
		s32Ret = SAMPLE_COMM_VPSS_Bind_VO(i, 0, stVoConfig.VoLayer, 0);
		if(s32Ret != TS_SUCCESS)
		{
			SAMPLE_PRT("vpss bind vo fail for %#x!\n", s32Ret);
			goto END7;
		}
	}

	// TS_MPI_SYS_ACTIVE();
    // TS_MPI_VDEC_ACTIVE();

    /************************************************
    step8:  send stream to VDEC
    *************************************************/
    for(i=0; i<u32VdecChnNum; i++)
    {
        stVdecSend[i].s32IntervalTime = 1000 * 30;

        if (0 ==VdecType) {
            snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), "1080P.h265");
        } else if (1 == VdecType) {
            snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), "1080P.h264");
        } else if (2 == VdecType) {
            snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), "1080P.jpg");
            stVdecSend[i].s32IntervalTime = 1000 * 1000;
        }

        snprintf(stVdecSend[i].cFilePath, sizeof(stVdecSend[i].cFilePath), "%s", SAMPLE_STREAM_PATH);
        stVdecSend[i].enType          = astSampleVdec[i].enType;
        stVdecSend[i].s32StreamMode   = astSampleVdec[i].enMode;
        stVdecSend[i].s32ChnId        = i;
        stVdecSend[i].u64PtsInit      = 0;
        stVdecSend[i].u64PtsIncrease  = 0;
        stVdecSend[i].eThreadCtrl     = THREAD_CTRL_START;
        stVdecSend[i].bCircleSend     = TS_TRUE;
        stVdecSend[i].s32MilliSec     = -1;
        stVdecSend[i].s32MinBufSize   = (astSampleVdec[i].u32Width * astSampleVdec[i].u32Height * 3)>>1;
    }
    SAMPLE_COMM_VDEC_StartSendStream(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);

    SAMPLE_COMM_VDEC_CmdCtrl(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);

    SAMPLE_COMM_VDEC_StopSendStream(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);

END7:
    for(i=0; i<VpssGrpNum; i++)
    {
        s32Ret = SAMPLE_COMM_VPSS_UnBind_VO(i, 1, VoLayer, i);
        if(s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("vpss unbind vo fail for %#x!\n", s32Ret);
        }
    }

END6:
    SAMPLE_COMM_VO_StopVO(&stVoConfig);

END5:
    for(i=0; i<u32VdecChnNum; i++)
    {
        s32Ret = SAMPLE_COMM_VDEC_UnBind_VPSS(i, i);
        if(s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("vdec unbind vpss fail for %#x!\n", s32Ret);
        }
    }

END4:
    for(i = VpssGrp; i >= 0; i--)
    {
        VpssGrp = i;
        SAMPLE_COMM_VPSS_Stop(VpssGrp, VpssChnNum);
    }
END3:
    SAMPLE_COMM_VDEC_Stop(u32VdecChnNum);

END2:
    SAMPLE_COMM_VDEC_ExitVBPool();

END1:
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

static TS_S32 SAMPLE_VDEC_VO_MIPI_BY_TYPE(TS_U8 VdecType, TS_S32 s32X, TS_S32 s32Y, TS_U32 u32Width, TS_U32 u32Height)
{
    TS_S32 i=0, s32Ret = TS_SUCCESS;
    VDEC_THREAD_PARAM_S stVdecSend[VDEC_MAX_CHN_NUM];
	SIZE_S stVdecSize;
    SIZE_S stDispSize;
    VO_LAYER VoLayer;
    TS_U32 u32VdecChnNum, VpssGrpNum, VpssChnNum;
    VPSS_GRP VpssGrp;
    SAMPLE_VDEC_ATTR astSampleVdec[VDEC_MAX_CHN_NUM] = {0};
    VPSS_CHN_ATTR_S astVpssChnAttr[VPSS_MAX_CHN_NUM] = {0};
    SAMPLE_VO_CONFIG_S stVoConfig;
    VPSS_GRP_ATTR_S stVpssGrpAttr;
    pthread_t   VdecThread[2*VDEC_MAX_CHN_NUM];    

    u32VdecChnNum = 1;
    VpssGrpNum    = u32VdecChnNum;
	VpssChnNum    = 1;

    /************************************************
    step1:  init SYS, init common VB(for VPSS and VO)
    *************************************************/
    stDispSize.u32Width = u32Width;
    stDispSize.u32Height = u32Height;

    PIC_SIZE_E enVdecPicSize;
    enVdecPicSize = PIC_1080P;
	s32Ret =  SAMPLE_COMM_SYS_GetPicSize(enVdecPicSize, &stVdecSize);
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("sys get pic size fail for %#x!\n", s32Ret);
        goto END1;
    }

    VB_CONFIG_S stVbConfig;
    memset(&stVbConfig, 0, sizeof(VB_CONFIG_S));
    stVbConfig.u32MaxPoolCnt             = 2;
    stVbConfig.astCommPool[0].u32BlkCnt  = 20 * u32VdecChnNum;
    stVbConfig.astCommPool[0].u64BlkSize = COMMON_GetPicBufferSize(stVdecSize.u32Width, stVdecSize.u32Height,
                                                PIXEL_FORMAT_NV_12, DATA_BITWIDTH_8, COMPRESS_MODE_SEG, 0);

    stVbConfig.astCommPool[1].u32BlkCnt  = 15;
    stVbConfig.astCommPool[1].u64BlkSize = stVbConfig.astCommPool[0].u64BlkSize;

    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConfig);
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("init sys fail for %#x!\n", s32Ret);
        goto END1;
    }    
	SAMPLE_PRT("step1>>>: SAMPLE_COMM_SYS_Init success !\n");

    /************************************************
    step2:  init module VB or user VB(for VDEC)
    *************************************************/
    for(i=0; i<u32VdecChnNum; i++)
    {
        if (0 == VdecType) {
            astSampleVdec[i].enType                           = PT_H265;
			astSampleVdec[i].stSapmleVdecVideo.enDecMode	  = VIDEO_DEC_MODE_IP;
			astSampleVdec[i].stSapmleVdecVideo.enBitWidth	  = DATA_BITWIDTH_8;
			astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum = 5;
        } else if (1 == VdecType) {
            astSampleVdec[i].enType                           = PT_H264;
			astSampleVdec[i].stSapmleVdecVideo.enDecMode	  = VIDEO_DEC_MODE_IP;
			astSampleVdec[i].stSapmleVdecVideo.enBitWidth	  = DATA_BITWIDTH_8;
			astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum = 5;
        } else if (2 == VdecType) {
            astSampleVdec[i].enType                           = PT_JPEG;
			astSampleVdec[i].stSapmleVdecPicture.enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
			astSampleVdec[i].stSapmleVdecPicture.u32Alpha = 255;
        }

        astSampleVdec[i].u32Width                         = stVdecSize.u32Width;
        astSampleVdec[i].u32Height                        = stVdecSize.u32Height;
        astSampleVdec[i].enMode                           = VIDEO_MODE_FRAME;
        astSampleVdec[i].u32DisplayFrameNum               = 5;
        astSampleVdec[i].u32FrameBufCnt = astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum + astSampleVdec[i].u32DisplayFrameNum + 1;
    }
    s32Ret = SAMPLE_COMM_VDEC_InitVBPool(u32VdecChnNum, &astSampleVdec[0]);
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("init mod common vb fail for %#x!\n", s32Ret);
        goto END2;
    }    
	SAMPLE_PRT("step2>>>: SAMPLE_COMM_VDEC_InitVBPool success !\n");


    /************************************************
    step3:  start VDEC
    *************************************************/
    s32Ret = SAMPLE_COMM_VDEC_Start(u32VdecChnNum, &astSampleVdec[0]);
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("start VDEC fail for %#x!\n", s32Ret);
        goto END3;
    }
	SAMPLE_PRT("step3>>>: SAMPLE_COMM_VDEC_Start success !\n");


	/************************************************
	step4:	start VPSS
	*************************************************/
	memset(&astVpssChnAttr[i], 0, sizeof(VPSS_CHN_ATTR_S));
	stVpssGrpAttr.enGrpMode = VPSS_GRP_MODE_STREAM;
	stVpssGrpAttr.u32MaxW = 1920;
	stVpssGrpAttr.u32MaxH = 1080;
	stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;
	stVpssGrpAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
	stVpssGrpAttr.enPixelFormat  = PIXEL_FORMAT_NV_12;

	for(i = 0; i < VpssChnNum; i ++)
	{
		astVpssChnAttr[i].u32Width					  = stDispSize.u32Width;
		astVpssChnAttr[i].u32Height 				  = stDispSize.u32Height;
		astVpssChnAttr[i].enChnMode 				  = VPSS_CHN_MODE_USER;
		astVpssChnAttr[i].enCompressMode			  = COMPRESS_MODE_NONE;
		astVpssChnAttr[i].enDynamicRange			  = DYNAMIC_RANGE_SDR8;
		astVpssChnAttr[i].enVideoFormat 			  = VIDEO_FORMAT_LINEAR;
		astVpssChnAttr[i].enPixelFormat 			  = PIXEL_FORMAT_YUV_PLANAR_420;
		astVpssChnAttr[i].stFrameRate.s32SrcFrameRate  = -1;
		astVpssChnAttr[i].stFrameRate.s32DstFrameRate  = -1;
		astVpssChnAttr[i].u32Depth					  = 3;
		astVpssChnAttr[i].bMirror						= TS_FALSE;
		astVpssChnAttr[i].bFlip 						= TS_FALSE;
		astVpssChnAttr[i].enVideoFormat 				= VIDEO_FORMAT_LINEAR;
		astVpssChnAttr[i].stAspectRatio.enMode			= ASPECT_RATIO_NONE;
		astVpssChnAttr[i].u32MaxW						= stDispSize.u32Width + 32;
		astVpssChnAttr[i].u32MaxH						= stDispSize.u32Height;
		astVpssChnAttr[i].u32MemCount					= 3;
		astVpssChnAttr[i].bVpuLoop						= TS_FALSE;
	}
	for(i=0; i<u32VdecChnNum; i++)
	{
		VpssGrp = i;
		s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, &stVpssGrpAttr, &astVpssChnAttr[0], VpssChnNum);
		if(s32Ret != TS_SUCCESS)
		{
			SAMPLE_PRT("start VPSS fail for %#x!\n", s32Ret);
			goto END4;
		}
	}
	SAMPLE_PRT("step4>>>: SAMPLE_COMM_VPSS_Start success !\n");

	/************************************************
	step5:	VDEC bind VPSS
	*************************************************/
	for(i=0; i<u32VdecChnNum; i++)
	{
        SAMPLE_PRT("SAMPLE_COMM_VDEC_Bind_VPSS start ...\n");
		s32Ret = SAMPLE_COMM_VDEC_Bind_VPSS(i, i);
        SAMPLE_PRT("SAMPLE_COMM_VDEC_Bind_VPSS end \n");
		if(s32Ret != TS_SUCCESS)
		{
			SAMPLE_PRT("vdec bind vpss fail for %#x!\n", s32Ret);
			goto END5; 
		}
	}
	SAMPLE_PRT("step5>>>: SAMPLE_COMM_VDEC_Bind_VPSS success !\n"); 

	/************************************************
	step6:	start VO
	*************************************************/
	stVoConfig.VoDev = SAMPLE_VO_DEV_UHD; 
    stVoConfig.VoLayer = SAMPLE_VO_LAYER_VHD0;
	stVoConfig.enDstDynamicRange = DYNAMIC_RANGE_SDR8;

    stVoConfig.stDispRect.s32X       = s32X;
    stVoConfig.stDispRect.s32Y       = s32Y;
    stVoConfig.stDispRect.u32Width   = stDispSize.u32Width;
    stVoConfig.stDispRect.u32Height  = stDispSize.u32Height;
    stVoConfig.stImageSize.u32Width  = stDispSize.u32Width;
    stVoConfig.stImageSize.u32Height = stDispSize.u32Height;
	stVoConfig.enPixFormat = PIXEL_FORMAT_YUV_PLANAR_420;
    
    /*mipi param setting */
    s32Ret = SAMPLE_COMM_VO_SetSceenParam(VO_SCREEN_TYPE_MIPI);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_SetSceenParam for mipi failed, s32Ret: 0x%x !\n", s32Ret);
        goto END5;
    }

	/*start vo*/
	s32Ret = SAMPLE_COMM_VO_StartVO(&stVoConfig);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("start vo failed. s32Ret: 0x%x !\n", s32Ret);
		goto END6;
	}
	SAMPLE_PRT("step6>>>: SAMPLE_COMM_VO_StartVO success !\n");

	/************************************************
	step8:	VPSS bind VO
	*************************************************/
	VoLayer = stVoConfig.VoDev;
	for(i=0; i<VpssGrpNum; i++)
	{
		s32Ret = SAMPLE_COMM_VPSS_Bind_VO(i, 0, stVoConfig.VoLayer, 0);
		if(s32Ret != TS_SUCCESS)
		{
			SAMPLE_PRT("vpss bind vo fail for %#x!\n", s32Ret);
			goto END7;
		}
	}
	SAMPLE_PRT("step8>>>: SAMPLE_COMM_VPSS_Bind_VO success !\n");

	// TS_MPI_SYS_ACTIVE();
    // TS_MPI_VDEC_ACTIVE();

    /************************************************
    step9:  send stream to VDEC
    *************************************************/
    for(i=0; i<u32VdecChnNum; i++)
    {
        stVdecSend[i].s32IntervalTime = 1000 * 30;  

        if (0 ==VdecType) {
            snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), "1080P.h265");
        } else if (1 == VdecType) {
            snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), "1080P.h264");
        } else if (2 == VdecType) {
            snprintf(stVdecSend[i].cFileName, sizeof(stVdecSend[i].cFileName), "1080P.jpg");
            stVdecSend[i].s32IntervalTime = 1000 * 1000;
        }

        snprintf(stVdecSend[i].cFilePath, sizeof(stVdecSend[i].cFilePath), "%s", SAMPLE_STREAM_PATH);
        stVdecSend[i].enType          = astSampleVdec[i].enType;
        stVdecSend[i].s32StreamMode   = astSampleVdec[i].enMode;
        stVdecSend[i].s32ChnId        = i;
        stVdecSend[i].u64PtsInit      = 0;
        stVdecSend[i].u64PtsIncrease  = 0;
        stVdecSend[i].eThreadCtrl     = THREAD_CTRL_START;
        stVdecSend[i].bCircleSend     = TS_TRUE;
        stVdecSend[i].s32MilliSec     = -1;
        stVdecSend[i].s32MinBufSize   = (astSampleVdec[i].u32Width * astSampleVdec[i].u32Height * 3)>>1;
    }
    
    SAMPLE_COMM_VDEC_StartSendStream(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);

    SAMPLE_COMM_VDEC_CmdCtrl(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);

    SAMPLE_COMM_VDEC_StopSendStream(u32VdecChnNum, &stVdecSend[0], &VdecThread[0]);
    
END7:
    for(i=0; i<VpssGrpNum; i++)
    {
        s32Ret = SAMPLE_COMM_VPSS_UnBind_VO(i, 1, VoLayer, i);
        if(s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("vpss unbind vo fail for %#x!\n", s32Ret);
        }
    }

END6:
    SAMPLE_COMM_VO_StopVO(&stVoConfig);

END5:
    for(i=0; i<u32VdecChnNum; i++)
    {
        s32Ret = SAMPLE_COMM_VDEC_UnBind_VPSS(i, i);
        if(s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("vdec unbind vpss fail for %#x!\n", s32Ret);
        }
    }

END4:
    for(i = VpssGrp; i >= 0; i--)
    {
        VpssGrp = i;
        SAMPLE_COMM_VPSS_Stop(VpssGrp, VpssChnNum);
    }
END3:
    SAMPLE_COMM_VDEC_Stop(u32VdecChnNum);

END2:
    SAMPLE_COMM_VDEC_ExitVBPool();

END1:
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

/******************************************************************************
 * function : show usage
 ******************************************************************************/
static void SAMPLE_VIO_Usage(char *sPrgNm)
{
    printf("Usage : %s <index>\n", sPrgNm);
    printf("index:\n");
    printf("\t 0)not support! VI - VO.\n");
    printf("\t 1)not support! VO .\n");
    printf("\t 2)not support! VpssOffline Vo.\n");
    printf("\t 3)MIPI sceen:play 1080P.h265, resize 720x576 .\n");
    printf("\t 4)RGB sceen: play 1080P.h265, resize 240x320 .\n");
    printf("\t 5)MIPI sceen: show yuv pic 1080x1920_nv12_01.yuv and 1080x1920_nv12_02.yuv \n");
    printf("\t 6)RGB sceen: show yuv pic 240x320_nv12_01.yuv and 240x320_nv12_02.yuv \n");

    printf("\t If you have any questions, please look at readme.txt!\n");
}

int main(int argc, char *argv[])
{
    TS_S32 s32Index;
    TS_S32 s32Ret = TS_FAILURE;

    if (argc < 2)
    {
		SAMPLE_VIO_Usage(argv[0]);
		return TS_FAILURE;
	}

    if (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0)
    {
		SAMPLE_VIO_Usage(argv[0]);
		return TS_FAILURE;
	}

#if defined(TX5336)
    s32Ret = SAMPLE_COMM_VO_LCD_IOMUX(); 
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_VO_RGBLCD_IOMUX, s32Ret=%#x!\n", s32Ret);
        return TS_FAILURE;
    }
#endif

    setbuf(stdout, NULL);
    signal(SIGINT, SAMPLE_VIO_HandleSig);
    signal(SIGTERM, SAMPLE_VIO_HandleSig);

    //SAMPLE_COMM_SYS_ReturnClass(&u32Pid);

    SAMPLE_VIO_MsgInit();

    s32Index = atoi(argv[1]);
    switch (s32Index)
    {
        case 0:
            //s32Ret = SAMPLE_VIO_Vi_Vo(u32VoIntfType);
        break;

        case 1:
            //s32Ret = SAMPLE_VO(u32VoIntfType);
            break;

        case 2:
            //s32Ret = SAMPLE_VIO_VpssOffline_Vo(u32VoIntfType);
            break;

        case 3:
            s32Ret = SAMPLE_VDEC_VO_MIPI_BY_TYPE(0, 0, 0, 720, 576);
            break;

        case 4:  // for rgb lcd h265 display
            s32Ret = SAMPLE_H264_H265_VDEC_VO(0);
            break;

        case 5:  // for mipi lcd yuv pic display
            s32Ret = SAMPLE_VO_VPSS_MIPI_1080P(VO_SCREEN_MIPI_1080P_FORMAT_RGBA8888);            
            // s32Ret = SAMPLE_VO_VPSS_MIPI_1080P(VO_SCREEN_MIPI_1080P_FORMAT_YUV420); 
            break;

        case 6: // for rgb lcd yuv pic display
            s32Ret = SAMPLE_VO_VPSS_RGB_240P(VO_SCREEN_RGB_240P_FORMAT_RGBA8888);
            break;

        default:
            SAMPLE_PRT("the index %d is invaild!\n", s32Index);
            SAMPLE_VIO_Usage(argv[0]);
            SAMPLE_VIO_MsgExit();
            return TS_FAILURE;
    }

    if (TS_SUCCESS == s32Ret)
    {
        SAMPLE_PRT("sample_vio exit success!\n");
    }
    else
    {
        SAMPLE_PRT("sample_vio exit abnormally!\n");
    }

    SAMPLE_VIO_MsgExit();

    return s32Ret;
}
