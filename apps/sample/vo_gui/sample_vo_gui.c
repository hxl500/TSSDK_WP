#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <errno.h>
#include <sys/mman.h>
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

#include "lvgl_demo.h"

static int  g_should_exit = 0;
static      SAMPLE_VO_CONFIG_S stVoConfig;
static      TS_S32 g_s32Index = 0;

#define	SAMPLE_VENC_CAM_NUM	1

#define SAMPLE_STREAM_PATH   				"./"

#define SAMPLE_USE_GUI_LVGL                 1

#define  BUFFER_240P_GBRA8888_SIZE       (240*320*4)
#define  BUFFER_1080P_GBRA8888_SIZE      (1080*1920*4)

static pthread_t   g_VdecThread[2];
static TS_U32 g_u32VdecChnNum = 1;
static TS_S32 SAMPLE_VO_GUI_Exit(void);

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

static TS_S32 SAMPLE_COMM_VO_EXIT()
{
    TS_MPI_VO_DisableVideoLayer(SAMPLE_VO_LAYER_VHD0);
    TS_MPI_VO_Disable(SAMPLE_VO_DEV_UHD);

    return TS_SUCCESS;
}

static void SAMPLE_VO_GUI_HandleSig(TS_S32 signo)
{
    VPSS_GRP VpssGrp = 0;
    VPSS_CHN VpssChn = 0;
    VO_LAYER VoLayer = 0;
    VO_CHN VoChn = 0;

    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    if (SIGINT == signo || SIGTERM == signo || SIGTSTP == signo)
    {     
        if (g_VdecThread[0] != 0)    
        {
            g_should_exit = 1;
            usleep(10000);  // wait 10ms for thread exit

            SAMPLE_PRT("SAMPLE_VO_GUI_Exit start ...\n");
            SAMPLE_VO_GUI_Exit();
        }
        else            
        {
            if(g_s32Index != 0)
            {
                SAMPLE_COMM_VPSS_UnBind_VO(VpssGrp, VpssChn, VoLayer, VoChn);  
                SAMPLE_COMM_VO_EXIT();
                SAMPLE_COMM_VPSS_Stop(0, 1);
                SAMPLE_COMM_SYS_Exit();
            }
        }
        
        SAMPLE_PRT("\033[0;31mCtrl+C exit now!\033[0;39m\n"); 
        // os_system_linux("killall -9 sample_vo_gui");
        exit(0);
    }
}
TS_S32 SAMPLE_VO_System_Init()
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

static TS_S32 SAMPLE_COMM_VO_StartVO_MIPI(VO_SETTING_INFO info)
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
        SAMPLE_PRT("vo set layer:SAMPLE_VO_LAYER_VHD0 attr failed. s32Ret: 0x%x !\n", s32Ret);
        return TS_FAILURE;
    }
    SAMPLE_PRT("TS_MPI_VO_SetVideoLayerAttr for layer0 %dx%d success!\n", info.vo_size.u32Width, info.vo_size.u32Height);

    TS_MPI_VO_GetVideoLayerAttr(SAMPLE_VO_LAYER_VHD1, &pstVoLayerPubAttr);
    pstVoLayerPubAttr.stDispRect.s32X = 0;
    pstVoLayerPubAttr.stDispRect.s32Y = 0;
    pstVoLayerPubAttr.stDispRect.u32Width = info.vo_size.u32Width;
    pstVoLayerPubAttr.stDispRect.u32Height = info.vo_size.u32Height;
    pstVoLayerPubAttr.enPixFormat = info.output_pix_format;

    pstVoLayerPubAttr.stImageSize.u32Width = info.vo_size.u32Width;
    pstVoLayerPubAttr.stImageSize.u32Height = info.vo_size.u32Height;
    s32Ret = TS_MPI_VO_SetVideoLayerAttr(SAMPLE_VO_LAYER_VHD1, &pstVoLayerPubAttr);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo set layer:SAMPLE_VO_LAYER_VHD1 attr failed. s32Ret: 0x%x !\n", s32Ret);
        return TS_FAILURE;
    }
    SAMPLE_PRT("TS_MPI_VO_SetVideoLayerAttr for layer1 %dx%d success!\n", info.vo_size.u32Width, info.vo_size.u32Height);

    s32Ret = TS_MPI_VO_EnableVideoLayer(SAMPLE_VO_LAYER_VHD0);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VO_EnableVideoLayer:layer0 failed. s32Ret: 0x%x !\n", s32Ret);
        return TS_FAILURE;
    }
    
    s32Ret = TS_MPI_VO_EnableVideoLayer(SAMPLE_VO_LAYER_VHD1);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vTS_MPI_VO_EnableVideoLayer:layer1 attr failed. s32Ret: 0x%x !\n", s32Ret);
        return TS_FAILURE;
    }

	VO_CHN_ATTR_S pstChnAttr;
	memset(&pstChnAttr, 0, sizeof(VO_CHN_ATTR_S));

	pstChnAttr.u32Priority = 100;
	TS_MPI_VO_SetChnAttr(SAMPLE_VO_LAYER_VHD0, 0, &pstChnAttr);
	TS_MPI_VO_SetChnAttr(SAMPLE_VO_LAYER_VHD1, 0, &pstChnAttr);

	memset(&pstChnAttr, 0, sizeof(VO_CHN_ATTR_S));

	TS_MPI_VO_GetChnAttr(SAMPLE_VO_LAYER_VHD0, 0, &pstChnAttr);
    TS_MPI_VO_GetChnAttr(SAMPLE_VO_LAYER_VHD1, 0, &pstChnAttr);
	
    return TS_SUCCESS;
}
 
static TS_S32 SAMPLE_COMM_VO_StartVO_RGB(VO_SETTING_INFO info)
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
        SAMPLE_PRT("vo set layer:SAMPLE_VO_LAYER_VHD0 attr failed. s32Ret: 0x%x !\n", s32Ret);
        return TS_FAILURE;
    }

    TS_MPI_VO_GetVideoLayerAttr(SAMPLE_VO_LAYER_VHD1, &pstVoLayerPubAttr);
    pstVoLayerPubAttr.stDispRect.s32X = 0;
    pstVoLayerPubAttr.stDispRect.s32Y = 0;
    pstVoLayerPubAttr.stDispRect.u32Width = info.vo_size.u32Width;
    pstVoLayerPubAttr.stDispRect.u32Height = info.vo_size.u32Height;
    pstVoLayerPubAttr.enPixFormat = info.output_pix_format;

    pstVoLayerPubAttr.stImageSize.u32Width = info.vo_size.u32Width;
    pstVoLayerPubAttr.stImageSize.u32Height = info.vo_size.u32Height;
    s32Ret = TS_MPI_VO_SetVideoLayerAttr(SAMPLE_VO_LAYER_VHD1, &pstVoLayerPubAttr);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo set layer:SAMPLE_VO_LAYER_VHD1 attr failed. s32Ret: 0x%x !\n", s32Ret);
        return TS_FAILURE;
    }

    s32Ret = TS_MPI_VO_EnableVideoLayer(SAMPLE_VO_LAYER_VHD0);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo enable layer:SAMPLE_VO_LAYER_VHD0 attr failed. s32Ret: 0x%x !\n", s32Ret);
        return TS_FAILURE;
    }
    
    s32Ret = TS_MPI_VO_EnableVideoLayer(SAMPLE_VO_LAYER_VHD1);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo enable layer:SAMPLE_VO_LAYER_VHD1 attr failed. s32Ret: 0x%x !\n", s32Ret);
        return TS_FAILURE;
    }

	VO_CHN_ATTR_S pstChnAttr;
	memset(&pstChnAttr, 0, sizeof(VO_CHN_ATTR_S));

	pstChnAttr.u32Priority = 100;
	TS_MPI_VO_SetChnAttr(SAMPLE_VO_LAYER_VHD0, 0, &pstChnAttr);
	TS_MPI_VO_SetChnAttr(SAMPLE_VO_LAYER_VHD1, 0, &pstChnAttr);

	memset(&pstChnAttr, 0, sizeof(VO_CHN_ATTR_S));

	TS_MPI_VO_GetChnAttr(SAMPLE_VO_LAYER_VHD0, 0, &pstChnAttr);
    TS_MPI_VO_GetChnAttr(SAMPLE_VO_LAYER_VHD1, 0, &pstChnAttr);
	
    return TS_SUCCESS;
}
 
static TS_S32 SAMPLE_COMM_VO_StartVO_GUI(VO_SETTING_INFO info)
{
    VO_LAYER VoLayer = SAMPLE_VO_LAYER_VHD1;
    TS_S32			s32Ret;
    VO_VIDEO_LAYER_ATTR_S pstVoLayerPubAttr;
    TS_MPI_VO_GetVideoLayerAttr(VoLayer, &pstVoLayerPubAttr);
    pstVoLayerPubAttr.stDispRect.s32X = 0;
    pstVoLayerPubAttr.stDispRect.s32Y = 0;
    pstVoLayerPubAttr.stDispRect.u32Width = info.vo_size.u32Width;
    pstVoLayerPubAttr.stDispRect.u32Height = info.vo_size.u32Height;
    pstVoLayerPubAttr.enPixFormat = info.output_pix_format;

    pstVoLayerPubAttr.stImageSize.u32Width = info.vo_size.u32Width;
    pstVoLayerPubAttr.stImageSize.u32Height = info.vo_size.u32Height;
    s32Ret = TS_MPI_VO_SetVideoLayerAttr(VoLayer, &pstVoLayerPubAttr);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo set layer:SAMPLE_VO_LAYER_VHD1 attr failed. s32Ret: 0x%x !\n", s32Ret);
        return TS_FAILURE;
    }

    s32Ret = TS_MPI_VO_EnableVideoLayer(VoLayer);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo enable layer:SAMPLE_VO_LAYER_VHD1 attr failed. s32Ret: 0x%x !\n", s32Ret);
        return TS_FAILURE;
    }

    return TS_SUCCESS;
}

static TS_S32 SAMPLE_VO_VPSS_MIPI_1080P(TS_U32 fps) 
{
    TS_S32          s32Ret, i;
    VO_SETTING_INFO screen_set = g_vo_screen_setting_list[VO_SCREEN_MIPI_1080P_FORMAT_RGBA8888];

    SAMPLE_VI_CONFIG_S stViConfig = {0};

    VPSS_GRP        VpssGrp[SAMPLE_VENC_CAM_NUM]    = {0};
    TS_S32          VpssChnlNum = 1;

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
    s32Ret = SAMPLE_COMM_VO_StartVO_MIPI(screen_set);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_StartVO_MIPI failed, s32Ret: 0x%x !\n", s32Ret);
        goto EXIT_VO_STOP;
    }

    /*vpss bind vo*/
    s32Ret = SAMPLE_COMM_VPSS_Bind_VO(VpssGrp[0], 0, SAMPLE_VO_LAYER_VHD0, 0);    
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo bind vpss to layer0 failed, s32Ret: 0x%x !\n", s32Ret);
        goto EXIT_VO_STOP;
    }

    VIDEO_FRAME_INFO_S pstVideoFrame = {0};
    memset(&pstVideoFrame, 0, sizeof(VIDEO_FRAME_INFO_S));
    pstVideoFrame.stVFrame.enPixelFormat = screen_set.input_pix_format;
    pstVideoFrame.stVFrame.u32Width = screen_set.vpss_group_max_size.u32Width;
    pstVideoFrame.stVFrame.u32Height = screen_set.vpss_group_max_size.u32Height;
    pstVideoFrame.stVFrame.size = pstVideoFrame.stVFrame.u32Width * pstVideoFrame.stVFrame.u32Height * SAMPLE_VO_GetBitSizeByFmt(pstVideoFrame.stVFrame.enPixelFormat);
    SAMPLE_PRT("pstVideoFrame.stVFrame.u32Width=%d, pstVideoFrame.stVFrame.u32Height=%d\n", pstVideoFrame.stVFrame.u32Width, pstVideoFrame.stVFrame.u32Height);

    s32Ret = SAMPLE_Vo_CreateVb(&pstVideoFrame);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT_VO_STOP;
    }
    VB_BLK blk_id = TS_MPI_VB_PhysAddr2Handle(pstVideoFrame.stVFrame.u64PhyAddr[0]);
    pstVideoFrame.u32PoolId = TS_MPI_VB_Handle2PoolId(blk_id);

#if 1 // show yuv pic on video layer
    FILE        *pFileHandle = NULL;
    pFileHandle = fopen("./1080x1920_nv12_02.yuv", "rb"); 
    if (TS_NULL == pFileHandle)
    {
        SAMPLE_PRT("can't open file\n");
        return -1;
    }
    
    uintptr_t addr = (uintptr_t)pstVideoFrame.stVFrame.u64VirAddr[0];
    fread((char*)addr, 1, pstVideoFrame.stVFrame.size, pFileHandle);
    fclose(pFileHandle);

    s32Ret = TS_MPI_VPSS_SendFrame(VpssGrp[0], 0, &pstVideoFrame, 100); 
    if(TS_SUCCESS != s32Ret) {
        SAMPLE_PRT("TS_MPI_VPSS_SendFrame,failed, s32Ret: 0x%x !\n", s32Ret); 
    } else {
        SAMPLE_PRT("TS_MPI_VPSS_SendFrame to plane0 successful!\n"); 
    }
    usleep(100 * 1000);
#endif

#if 1
    lvgl_start_demo_slideshow(fps);     
#else    
    /*LVGL 适配mpi 接口测试程序*/ 
    int             index = 0;  
    char            buffer[BUFFER_1080P_GBRA8888_SIZE];
    FILE            *pFileRead = NULL;

    while(1)
    {
        index++;

        if (index == 1) {
            pFileRead = fopen("./1080x1920_bgra8888_01.bgra", "rb"); 
        } else if (index == 2) {
            pFileRead = fopen("./1080x1920_bgra8888_02.bgra", "rb");
        } else if (index == 3) {
            pFileRead = fopen("./1080x1920_nv12_01.yuv", "rb");
        } else if (index == 4) {
            pFileRead = fopen("./1080x1920_nv12_02.yuv", "rb");
        } 

        if(TS_NULL != pFileRead)
        {
            if (index == 3 || index == 4 ) // yuv
            {                   
                fread((char*)(pstVideoFrame.stVFrame.u64VirAddr[0]), 1, pstVideoFrame.stVFrame.size, pFileRead);       
                s32Ret = TS_MPI_VPSS_SendFrame(VpssGrp[0], 0, &pstVideoFrame, 100); 
                if(TS_SUCCESS != s32Ret) 
                {
                    SAMPLE_PRT("TS_MPI_VPSS_SendFrame,failed, s32Ret: 0x%x !\n", s32Ret); 
                } 
                else 
                {                    
                    SAMPLE_PRT("TS_MPI_VPSS_SendFrame to plane0 successful, index=%d!\n\n", index); 
                }

                if (index == 4)
                    index = 0;
            }
            else   // bgra8888
            {
                fread(buffer, 1, BUFFER_1080P_GBRA8888_SIZE, pFileRead);
                s32Ret = TS_MPI_VO_GUI_SendFrame(1, 1080, 1920, BUFFER_1080P_GBRA8888_SIZE, buffer);
                if(TS_SUCCESS != s32Ret) {
                    SAMPLE_PRT("TS_MPI_VO_GUI_SendFrame,failed, s32Ret: 0x%x !\n", s32Ret); 
                } else {
                    // SAMPLE_PRT("TS_MPI_VO_GUI_SendFrame to plane1, successful, index=%d!\n\n", index);
                }

                if (index == 2)
                    index = 0;
            }
        }
        else 
        {
            SAMPLE_PRT("can't open file!\n");
            break;
        }

        fclose(pFileRead);
        usleep(30 * 1000);
    }
#endif

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

static TS_S32 SAMPLE_VO_VPSS_RGB_240P(TS_U32 fps) 
{
    TS_S32          s32Ret, i;
    FILE            *pFileRead = NULL;
    VO_SETTING_INFO screen_set = g_vo_screen_setting_list[VO_SCREEN_RGB_240P_FORMAT_RGBA8888];

    SAMPLE_VI_CONFIG_S stViConfig = {0};

    VPSS_GRP        VpssGrp[SAMPLE_VENC_CAM_NUM]    = {0};
    TS_S32          VpssChnlNum = 1;

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
        SAMPLE_PRT("SAMPLE_COMM_VO_SetSceenParam for VO_SCREEN_TYPE_RGB failed, s32Ret: 0x%x !\n", s32Ret);
        goto EXIT_VPSS_STOP;
    }

    /*start vo*/
    s32Ret = SAMPLE_COMM_VO_StartVO_RGB(screen_set);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_StartVO_RGB failed, s32Ret: 0x%x !\n", s32Ret);
        goto EXIT_VO_STOP;
    }

    /*vpss bind vo*/
    s32Ret = SAMPLE_COMM_VPSS_Bind_VO(VpssGrp[0], 0, SAMPLE_VO_LAYER_VHD0, 0);    
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo bind vpss to layer0 failed, s32Ret: 0x%x !\n", s32Ret);
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

    pFileRead = fopen("./240x320_nv12_01.yuv", "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file\n");
        return -1;
    }

    uintptr_t addr = (uintptr_t)pstVideoFrame.stVFrame.u64VirAddr[0]; 
    fread((char*)addr, 1, pstVideoFrame.stVFrame.size, pFileRead);
    fclose(pFileRead);

    s32Ret = TS_MPI_VPSS_SendFrame(VpssGrp[0], 0, &pstVideoFrame, 100); 
    if(TS_SUCCESS != s32Ret) {
        SAMPLE_PRT("TS_MPI_VPSS_SendFrame,failed, s32Ret: 0x%x !\n", s32Ret); 
    } else {
        SAMPLE_PRT("TS_MPI_VPSS_SendFrame to plane0 successful!\n");
    }
    usleep(1000 * 1000);

#if 1
    lvgl_start_demo_slideshow(fps);       
 #else       
    /*LVGL 适配mpi 接口测试程序*/ 
    int             index = 0;  
    char            buffer[BUFFER_240P_GBRA8888_SIZE];
    while(1)
    {
        index++;

        if (index == 1) {
            pFileRead = fopen("./240x320_bgra8888_01.bgra", "rb"); 
        } else if (index == 2) {
            pFileRead = fopen("./240x320_bgra8888_02.bgra", "rb");
        } else if (index == 3) {
            pFileRead = fopen("./240x320_nv12_01.yuv", "rb");
        } else if (index == 4) {
            pFileRead = fopen("./240x320_nv12_02.yuv", "rb");
        } 

        if(TS_NULL != pFileRead)
        {
            if (index == 3 || index == 4)  // yuv
            {                   
                addr = (uintptr_t)pstVideoFrame.stVFrame.u64VirAddr[0]; 
                fread((char*)addr, 1, pstVideoFrame.stVFrame.size, pFileRead);       
                s32Ret = TS_MPI_VPSS_SendFrame(VpssGrp[0], 0, &pstVideoFrame, 100); 
                if(TS_SUCCESS != s32Ret) 
                {
                    SAMPLE_PRT("TS_MPI_VPSS_SendFrame,failed, s32Ret: 0x%x !\n", s32Ret); 
                } 
                else 
                {                    
                    SAMPLE_PRT("TS_MPI_VPSS_SendFrame to plane0 successful, index=%d!\n\n", index); 
                }

                if (index == 4)
                    index = 0;
            }
            else   // bgra8888
            {
                fread(buffer, 1, BUFFER_240P_GBRA8888_SIZE, pFileRead);
                s32Ret = TS_MPI_VO_GUI_SendFrame(1, 240, 320, BUFFER_240P_GBRA8888_SIZE, buffer);
                if(TS_SUCCESS != s32Ret) {
                    SAMPLE_PRT("TS_MPI_VO_GUI_SendFrame,failed, s32Ret: 0x%x !\n", s32Ret); 
                } else {
                    SAMPLE_PRT("TS_MPI_VO_GUI_SendFrame to plane1, successful, index=%d!\n\n", index);
                }

                if (index == 2)
                    index = 0;
            }
        }
        else 
        {
            SAMPLE_PRT("can't open file!\n");
            break;
        }

        fclose(pFileRead);
        usleep(30 * 1000);
    }
#endif


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

static TS_VOID * SAMPLE_VO_VDEC_Thread_SendStream(TS_VOID *pArgs)
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
        if(g_should_exit == 1)
        {
            SAMPLE_PRT("thread SAMPLE_VO_VDEC_SendStream exit now!\n");
            break;
        }

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
        // printf("send %d\n", stStream.u32Len);
        s32Ret=TS_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, pstVdecThreadParam->s32MilliSec);
        // printf("send %d ok!\n", stStream.u32Len);
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

    SAMPLE_PRT("\033[0;31m chn %d send steam thread return ... \033[0;39m \n", pstVdecThreadParam->s32ChnId);
    fflush(stdout);
    if (pu8Buf != TS_NULL)
    {
        free(pu8Buf);
    }
    fclose(fpStrm);

    return (TS_VOID *)TS_SUCCESS;
}

static TS_VOID SAMPLE_VO_VDEC_StartSendStream(TS_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread)
{
    TS_S32  i;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    for(i=0; i<s32ChnNum; i++)
    {
        pVdecThread[i] = 0;
#if 0 
        struct sched_param sched;
        pthread_attr_setschedpolicy(&attr, SCHED_RR);
        sched.sched_priority = 90;
        pthread_attr_setschedparam(&attr, &sched);
#else 
        pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
#endif
        pthread_attr_setstacksize(&attr, 0x20000);   
        pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE);  
        pthread_create(&pVdecThread[i], &attr, &SAMPLE_VO_VDEC_Thread_SendStream, (TS_VOID *)&pstVdecSend[i]);
        // pthread_setname_np(pVdecThread[i], "SAMPLE_VO_VDEC_Thread_SendStream");
        pthread_attr_destroy(&attr);
    }
}

static TS_VOID SAMPLE_VO_VDEC_StopSendStream(TS_S32 s32ChnNum, pthread_t *pVdecThread) 
{
    TS_S32  i;

    for(i=0; i<s32ChnNum; i++)
    {
        if(0 != pVdecThread[i])
        {
			pthread_join(pVdecThread[i], TS_NULL);
            SAMPLE_PRT("pthread_join pVdecThread[%d]=%lu success \n", i, pVdecThread[i]);
            pVdecThread[i] = 0;
        }
    }
}

static TS_S32 SAMPLE_VO_GUI_Exit(void)
{
#if 0
    TS_S32 i, s32Ret = TS_SUCCESS;
    TS_U32 u32VdecChnNum = g_u32VdecChnNum;
    TS_U32 VpssGrpNum = g_u32VdecChnNum;
    VO_LAYER VoLayer = 0;

    VoLayer = stVoConfig.VoDev;
    if (0 != g_VdecThread[0]) {
        SAMPLE_PRT("stop the SendStream thrad ....\n");
        SAMPLE_VO_VDEC_StopSendStream(u32VdecChnNum,  &g_VdecThread[0]);
    }

    for(i=0; i<VpssGrpNum; i++)
    {
        s32Ret = SAMPLE_COMM_VPSS_UnBind_VO(i, 0, VoLayer, i);
        if(s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("vpss unbind vo fail for %#x, VoLayer=%d!\n", s32Ret, VoLayer);
        }
    }

    SAMPLE_PRT("SAMPLE_COMM_VO_StopVO ...\n");
    SAMPLE_COMM_VO_StopVO(&stVoConfig);

    SAMPLE_PRT("SAMPLE_COMM_VPSS_UnBind_VO ...\n");
    for(i=0; i<u32VdecChnNum; i++)
    {
        s32Ret = SAMPLE_COMM_VDEC_UnBind_VPSS(i, i);
        if(s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("vdec unbind vpss fail for %#x!\n", s32Ret);
        }
    }

    SAMPLE_PRT("SAMPLE_COMM_VPSS_Stop ...\n");
    SAMPLE_COMM_VPSS_Stop(0, 1);

    SAMPLE_PRT("SAMPLE_COMM_VDEC_Stop, u32VdecChnNum=%d ...\n", u32VdecChnNum);
    SAMPLE_COMM_VDEC_Stop(u32VdecChnNum);

    SAMPLE_PRT("SAMPLE_COMM_VDEC_ExitVBPool ...\n");
    SAMPLE_COMM_VDEC_ExitVBPool();

    SAMPLE_PRT("SAMPLE_COMM_SYS_Exit ...\n");
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
#else
    return TS_SUCCESS;
#endif
}

static TS_S32 SAMPLE_VDEC_VO_MIPI_BY_TYPE(TS_U32 fps, TS_U8 VdecType, TS_S32 s32X, TS_S32 s32Y, TS_U32 u32Width, TS_U32 u32Height)
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
    // SAMPLE_VO_CONFIG_S stVoConfig;
    VPSS_GRP_ATTR_S stVpssGrpAttr;

    u32VdecChnNum = g_u32VdecChnNum;
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
			astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum = 3;
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
	step7:	start vo layer GUI
	*************************************************/
    VO_SETTING_INFO screen_set = g_vo_screen_setting_list[VO_SCREEN_MIPI_1080P_FORMAT_RGBA8888];
    s32Ret = SAMPLE_COMM_VO_StartVO_GUI(screen_set);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_StartVO_GUI failed, s32Ret: 0x%x !\n", s32Ret);
		goto END6;
    }

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
        stVdecSend[i].s32IntervalTime = 1000 * 100;  // mipi 10fps = 1000 / 10

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
    
    SAMPLE_VO_VDEC_StartSendStream(u32VdecChnNum, &stVdecSend[0], &g_VdecThread[0]);
    SAMPLE_PRT("step9>>>: SAMPLE_VO_VDEC_StartSendStream, g_VdecThread[0]=%lu!\n", g_VdecThread[0]);

    
    /************************************************
    lvgl_start_demo_slideshow
    *************************************************/
#if SAMPLE_USE_GUI_LVGL
    lvgl_start_demo_slideshow(fps);   
    SAMPLE_PRT(">>>: lvgl_start_demo_slideshow ...");
#endif

    SAMPLE_VO_VDEC_StopSendStream(u32VdecChnNum,  &g_VdecThread[0]);
    
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


/* VdecType: 0 H265; 1 H264; 2 JPEG; */
static TS_S32 SAMPLE_VDEC_VO_RGB_BY_TYPE(TS_U32 fps, TS_U8 VdecType, TS_S32 s32X, TS_S32 s32Y, TS_U32 u32Width, TS_U32 u32Height)
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
    // SAMPLE_VO_CONFIG_S stVoConfig;
    VPSS_GRP_ATTR_S stVpssGrpAttr;

    u32VdecChnNum = g_u32VdecChnNum;
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
			astSampleVdec[i].stSapmleVdecVideo.u32RefFrameNum = 3;
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
	step7:	VDEC bind VPSS
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
	step5:	start VO
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
	stVoConfig.enPixFormat = PIXEL_FORMAT_YUV_PLANAR_420;;

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
	SAMPLE_PRT("step6>>>: SAMPLE_COMM_VO_StartVO success !\n");

	/************************************************
	step7:	start vo layer GUI
	*************************************************/
    VO_SETTING_INFO screen_set = g_vo_screen_setting_list[VO_SCREEN_RGB_240P_FORMAT_RGBA8888];
    s32Ret = SAMPLE_COMM_VO_StartVO_GUI(screen_set);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_StartVO_GUI failed, s32Ret: 0x%x !\n", s32Ret);
		goto END6;
    }

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
    
    SAMPLE_VO_VDEC_StartSendStream(u32VdecChnNum, &stVdecSend[0], &g_VdecThread[0]);
    SAMPLE_PRT("step9>>>: SAMPLE_VO_VDEC_StartSendStream, g_VdecThread[0]=%lu!\n", g_VdecThread[0]);

    
    /************************************************
    step10:  lvgl_start_demo_slideshow
    *************************************************/
#if SAMPLE_USE_GUI_LVGL
    lvgl_start_demo_slideshow(fps);   
    SAMPLE_PRT("step10>>>: lvgl_start_demo_slideshow ...");
#endif

    SAMPLE_VO_VDEC_StopSendStream(u32VdecChnNum,  &g_VdecThread[0]);
    
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

static TS_S32 SAMPLE_GUI_DEMO(TS_U32 fps, char * info[], int size)
{
    SAMPLE_PRT("lvgl_start_demos start... \n"); 
    lvgl_start_demos(info, size, fps);

    return TS_SUCCESS;
}

/******************************************************************************
 * function : show usage
 ******************************************************************************/
static void SAMPLE_VO_GUI_Usage(char *sPrgNm)
{
    printf("Usage : %s <index> <fps>\n", sPrgNm);
    printf("index and fps :\n");
    printf("\t 0 30) ./sample_vo_gui 0 25 slideshow (default)\n");
    printf("\t       ./sample_vo_gui 0 30 widgets \n");
    printf("\t       ./sample_vo_gui 0 20 stress \n");
    printf("\t       ./sample_vo_gui 0 15 benchmark (not supported mipi)\n");
    printf("\t 1 15) MIPI screen: SHOW 1080x1920_nv12_02.yuv On layer0  + GUI slideshow On layer1.\n");
    printf("\t 2 10) MIPI screen: Play 1080P.h265 On layer0:(0,1344,720,576) + GUI slideshow On layer1.\n");
    printf("\t 3 20) RGB screen: SHOW 240x320_nv12_01.yuv On layer0  + GUI slideshow On layer1.\n");
    printf("\t 4 15) RGB screen: Play 1080P.h265 On layer0:(0, 0, 240,160) + GUI slideshow On layer1.\n");
    printf("\t 5 15) RGB screen: Play 1080P.h265 On layer0:(0,160,240,160) + GUI slideshow On layer1.\n");
    printf("\t 6 15) RGB screen: Play 1080P.h265 On layer0:(0, 0, 240,320) + GUI slideshow On layer1.\n");

    printf("\t If you have any questions, please look at readme.txt!\n");
}

int main(int argc, char *argv[]) 
{
    TS_S32 s32Ret = TS_FAILURE;
    TS_U32  fps = 0;

    g_VdecThread[0] = 0;

    if (argc < 3)
    {
        SAMPLE_PRT("param invalid, show help info ");
		SAMPLE_VO_GUI_Usage(argv[0]);
		return TS_FAILURE;
	}

    if (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0)
    {
		SAMPLE_VO_GUI_Usage(argv[0]);
		return TS_FAILURE;
	}

    setbuf(stdout, NULL);
    signal(SIGINT, SAMPLE_VO_GUI_HandleSig);
    signal(SIGTERM, SAMPLE_VO_GUI_HandleSig);
    signal(SIGTSTP , SAMPLE_VO_GUI_HandleSig);

    g_s32Index = atoi(argv[1]);
    SAMPLE_PRT("g_s32Index=%d!\n", g_s32Index);

    fps = atoi(argv[2]);
    if( fps > 30 || fps <= 0)
    {
        SAMPLE_PRT("fps should be greater than 0 and less than 30!\n");
		SAMPLE_VO_GUI_Usage(argv[0]);
        return TS_FAILURE;
    }
   
#if defined(TX5336)
    s32Ret = SAMPLE_COMM_VO_LCD_IOMUX(); 
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_RGBLCD_IOMUX, s32Ret=%#x!\n", s32Ret);
        return TS_FAILURE;
    }
#endif

    switch (g_s32Index)
    {
        case 0:   // run lvgl demos: slideshow(default), stress, benchmark
            s32Ret = SAMPLE_GUI_DEMO(fps, &argv[3], argc-3);
            break;

        case 1:   // for mipi: show picture on plane1 and lvgl on plane2 
            s32Ret = SAMPLE_VO_VPSS_MIPI_1080P(fps);
            break;

        case 2:   // for mipi: play video on plane1 and lvgl on plane2
            s32Ret = SAMPLE_VDEC_VO_MIPI_BY_TYPE(fps, 0, 0, 1344, 720, 576);
            break;

        case 3:   // for RGB: show picture on plane1 and lvgl on plane2 
            s32Ret = SAMPLE_VO_VPSS_RGB_240P(fps);
            break;

        case 4:   // for RGB: play video(0, 0, 240,160) on plane1 and lvgl on plane2
            s32Ret = SAMPLE_VDEC_VO_RGB_BY_TYPE(fps, 0, 0, 0, 240, 160);
            break;

        case 5:   // for RGB: play video(0,160,240,160) on plane1 and lvgl on plane2
            s32Ret = SAMPLE_VDEC_VO_RGB_BY_TYPE(fps, 0, 0, 160, 240, 160); 
            break;

        case 6:   // for RGB: play video(0,0,240,320) on plane1 and lvgl on plane2
            s32Ret = SAMPLE_VDEC_VO_RGB_BY_TYPE(fps, 0, 0, 0, 240, 320);  
            break;

        default:
            SAMPLE_PRT("the index %d is invaild!\n", g_s32Index);
            SAMPLE_VO_GUI_Usage(argv[0]);
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

    return s32Ret;
}
