
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <getopt.h>//getopt_long,optarg...

#include "sample_comm_base.h"
#include "sample_comm_util.h"
#include "sample_comm_sys.h"
#include "sample_comm_vi.h"
#include "sample_comm_isp.h"
#include "sample_comm_vpss.h"
#include "sample_comm_venc.h"

#define echo_sysfs(x, y) system("echo \"" y "\" > \"" x "\"")
#define	SAMPLE_VENC_CAM_NUM	2

/*
	1. query from isp drv
	2. get from UI
	3. get from config file
	4. initial
*/

static TS_S32 bS32State = 0;
/******************************************************************************
* function : show usage
******************************************************************************/
void SAMPLE_VENC_Usage(char* sPrgNm)
{

	printf("Tips:\n");
	printf("  Press Ctrl+C to exit.\n");
	printf("\t Quick start, post load driver.\n");
    return;
}

/******************************************************************************
* function : to process abnormal case
******************************************************************************/
void SAMPLE_VENC_HandleSig(TS_S32 signo)
{
	SAMPLE_PRT("\n\n\033[0;31mReceive signal %s, exit...!\033[0;39m\n\n", strsignal(signo));

    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
	bS32State = 0;
}

static void SAMPLE_WAIT_Q_EXIT(void)
{
	while(1){
		if (bS32State == 0)
			break;
		else
			usleep(100);
	}
}

TS_S32 SAMPLE_VENC_VI_Init( SAMPLE_VI_CONFIG_S *pstViConfig, TS_BOOL bLowDelay)
{
    TS_S32              s32Ret;
    VB_CONFIG_S 		stVbConf;

	stVbConf.astCommPool[0].u64BlkSize   = 1024;
	stVbConf.astCommPool[0].u32BlkCnt	  = 1;
	stVbConf.u32MaxPoolCnt = 1;

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_VI_StartVi(pstViConfig);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_COMM_SYS_Exit();
        SAMPLE_PRT("SAMPLE_COMM_VI_StartVi failed with %d!\n", s32Ret);
        return s32Ret;
    }

    return TS_SUCCESS;
}

TS_S32 SAMPLE_VENC_VPSS_Init(VPSS_GRP VpssGrp, PIXEL_FORMAT_E enPixelFormat, SIZE_S * pPipeSize, SIZE_S * pChnlSize, TS_S32 chnl_num)
{
    TS_S32 i;
    TS_S32 s32Ret;
    VPSS_GRP_ATTR_S stVpssGrpAttr;
    VPSS_CHN_ATTR_S stVpssChnAttr[4];

	stVpssGrpAttr.enGrpMode = VPSS_GRP_MODE_STREAM;
    stVpssGrpAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stVpssGrpAttr.enPixelFormat  = enPixelFormat;
    stVpssGrpAttr.u32MaxW        = pPipeSize->u32Width;
    stVpssGrpAttr.u32MaxH        = pPipeSize->u32Height;
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;

    for(i = 0; i < chnl_num; i ++)
    {
    	memset(&stVpssChnAttr[i], 0, sizeof(VPSS_CHN_ATTR_S));
		stVpssChnAttr[i].u32Width					  = pChnlSize[i].u32Width;
		stVpssChnAttr[i].u32Height					  = pChnlSize[i].u32Height;
		stVpssChnAttr[i].enChnMode					  = VPSS_CHN_MODE_USER;
		stVpssChnAttr[i].enCompressMode 			  = COMPRESS_MODE_NONE;
		stVpssChnAttr[i].enDynamicRange 			  = DYNAMIC_RANGE_SDR8;
		stVpssChnAttr[i].enVideoFormat				  = VIDEO_FORMAT_LINEAR;
		stVpssChnAttr[i].enPixelFormat				  = enPixelFormat;
		stVpssChnAttr[i].stFrameRate.s32SrcFrameRate  = -1;
		stVpssChnAttr[i].stFrameRate.s32DstFrameRate  = -1;
		stVpssChnAttr[i].u32Depth					  = 3;
		stVpssChnAttr[i].bMirror						= TS_FALSE;
		stVpssChnAttr[i].bFlip							= TS_FALSE;
		stVpssChnAttr[i].enVideoFormat					= VIDEO_FORMAT_LINEAR;
		stVpssChnAttr[i].stAspectRatio.enMode			= ASPECT_RATIO_NONE;
		stVpssChnAttr[i].u32MaxW						= pChnlSize[i].u32Width;
		stVpssChnAttr[i].u32MaxH						= pChnlSize[i].u32Height;
		stVpssChnAttr[i].u32MemCount 					= 1;
		stVpssChnAttr[i].bVpuLoop						= TS_FALSE;
    }

    s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, &stVpssGrpAttr,  stVpssChnAttr, chnl_num);
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("start VPSS fail for %#x!\n", s32Ret);
    }
    return s32Ret;
}

ts_void fastboot_insmod(){
	system("insmod /usr/lib/modules/ts_codec.ko");
	system("insmod /usr/lib/modules/hx280enc.ko");

		SAMPLE_PRT("insmod /usr/lib/modules/vpu_h265_v1.ko");
		system("insmod /usr/lib/modules/vpu_h265_v1.ko");
	
	while (0 != access("/dev/vpu_mem", F_OK)) {
		SAMPLE_PRT("VPU driver is not ready, waiting...\n");
		usleep(1000);
	}

	system("insmod /usr/lib/modules/mpp_proc.ko");

	SAMPLE_PRT("insmod for fastboot finish\n");
}

TS_S32 SAMPLE_VENC_FASTBOOT(TS_BOOL 	bLowDelay)
{
	TS_S32			s32Ret, i;
	MODE_SETTING_S	sns_mode_set;

	const TS_BOOL	bIspByfly = TS_TRUE;
	VI_PIPE 		ViPipe[SAMPLE_VENC_CAM_NUM]		= {0, 1};
	//VI_PIPE 		ViPipe[SAMPLE_VENC_CAM_NUM]		= {0};
	SAMPLE_VI_CONFIG_S stViConfig = {0};
	SIZE_S			stSize_vipipe;

	VPSS_GRP		VpssGrp[SAMPLE_VENC_CAM_NUM]	= {0, 1};
	//VPSS_GRP		VpssGrp[SAMPLE_VENC_CAM_NUM]	= {0};
	PIXEL_FORMAT_E	enPixelFmt = PIXEL_FORMAT_NV_12;
	SIZE_S			stSize_vpssChnl[VPSS_MAX_PHY_CHN_NUM];
	TS_S32			VpssChnlNum = 1;

	VENC_CHN		VencChn[SAMPLE_VENC_CAM_NUM]	= {0,1};
	//VENC_CHN		VencChn[SAMPLE_VENC_CAM_NUM]	= {0};
	TS_U32			u32Profile[SAMPLE_VENC_CAM_NUM] = {0,2};
	//TS_U32			u32Profile[SAMPLE_VENC_CAM_NUM] = {0};
	PAYLOAD_TYPE_E	enPayLoad[SAMPLE_VENC_CAM_NUM]  = {PT_H265, PT_H265};
	//PAYLOAD_TYPE_E	enPayLoad[SAMPLE_VENC_CAM_NUM]  = {PT_H265};
	VENC_GOP_MODE_E enGopMode;
	VENC_GOP_ATTR_S stGopAttr;
	SAMPLE_RC_E 	enRcMode = SAMPLE_RC_CBR;

	sns_mode_set.numCam = 1;
	sns_mode_set.isOnLine = TS_FALSE;
	sns_mode_set.enSize = PIC_2560x1440;
	sns_mode_set.minFps = 15;
	sns_mode_set.maxFps = 15;
	sns_mode_set.wdr_mode = WDR_MODE_NONE;
	sns_mode_set.bit_width = DATA_BITWIDTH_8;
	sns_mode_set.enBayer = BAYER_GRBG;
	SAMPLE_COMM_SYS_GetPicSize(sns_mode_set.enSize, &stSize_vipipe);

	stViConfig.s32WorkingViNum		 = sns_mode_set.numCam;
	for(i = 0; i < stViConfig.s32WorkingViNum; i ++){
		SAMPLE_PIPE_INFO_S * pstPipeInfo = &stViConfig.astViInfo[i].stPipeInfo;
		SAMPLE_CHN_INFO_S * pstChnInfo = &stViConfig.astViInfo[i].stChnInfo;

		pstPipeInfo->aPipe		= i;
		pstPipeInfo->enBitWid	= sns_mode_set.bit_width;
		pstPipeInfo->enBayer	= sns_mode_set.enBayer;
		pstPipeInfo->width		= stSize_vipipe.u32Width;
		pstPipeInfo->height		= stSize_vipipe.u32Height;
		pstPipeInfo->frameRate	= sns_mode_set.maxFps;
		pstPipeInfo->enWdrMode	= sns_mode_set.wdr_mode;
		pstPipeInfo->bIspByFly	= sns_mode_set.isOnLine;
		if( TS_TRUE == pstPipeInfo->bIspByFly)
		{
			pstPipeInfo->bDynFpsSync = TS_TRUE;
		}
		else
		{
			pstPipeInfo->bDynFpsSync = TS_FALSE;
		}
		if(TS_TRUE == bLowDelay){
			pstPipeInfo->enMastPipeMode = VI_ONLINE_VPSS_ONLINE;
			pstPipeInfo->enPixFmt	= PIXEL_FORMAT_RGB_888;
		}
		else{
			pstPipeInfo->enMastPipeMode = VI_ONLINE_VPSS_OFFLINE;
			pstPipeInfo->enPixFmt	= PIXEL_FORMAT_NV_12;
		}

		pstChnInfo->ViChn[0]	= 0;
		pstChnInfo->width[0]	= stSize_vipipe.u32Width;
		pstChnInfo->height[0]	= stSize_vipipe.u32Height;
		pstChnInfo->validChnlNum= 1;
		pstChnInfo->enPixFormat = PIXEL_FORMAT_NV_12;
	}

	if(TS_TRUE == bIspByfly){
		SAMPLE_PRT("do something when ispByFly\n");
	}

	s32Ret = SAMPLE_VENC_VI_Init(&stViConfig, bLowDelay);
	if(s32Ret != TS_SUCCESS)
	{
		SAMPLE_PRT("Init VI err for %#x!\n", s32Ret);
		return TS_FAILURE;
	}

	stSize_vpssChnl[0].u32Width 	= stSize_vipipe.u32Width;
	stSize_vpssChnl[0].u32Height	= stSize_vipipe.u32Height;
	for(i = 0; i < sns_mode_set.numCam; i ++){
		s32Ret = SAMPLE_VENC_VPSS_Init(VpssGrp[i], enPixelFmt, &stSize_vipipe, stSize_vpssChnl, VpssChnlNum);
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("Init VPSS group[%d] err for %#x!\n", VpssGrp[i], s32Ret);
			goto EXIT_VI_STOP;
		}
		SAMPLE_PRT("Init VPSS group[%d] success!\n", VpssGrp[i]);
	}

	if (TS_SUCCESS != TS_MPI_VPSS_Start_Camera()) {
		SAMPLE_PRT("TS_MPI_VPSS_Start_Camera fail !\n");
	}

	if(TS_FALSE == bLowDelay){
		for(i = 0; i < sns_mode_set.numCam; i ++){
			s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe[i], 0, VpssGrp[i]);
			if(s32Ret != TS_SUCCESS)
			{
				SAMPLE_PRT("VI Bind VPSS err for %#x!\n", s32Ret);
				goto EXIT_VPSS_STOP;
			}
		}
	}
	else {
		SAMPLE_PRT("do NOT need VI Bind VPSS \n");
	}

	if (TS_TRUE == bLowDelay) {
		 /* 快启模式下，在这里加载venc和codec的ko,只需加载一次				 */
		 SAMPLE_COMM_ISP_GetVDTimeout(ViPipe[0], ISP_VD_FIRST_FRAME, 0);
		 SAMPLE_PRT("get first frame success !!\n");
		 fastboot_insmod();
	}

   /******************************************
	start stream venc
	******************************************/
	enGopMode = VENC_GOPMODE_NORMALP;

	s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode,&stGopAttr);
	if (TS_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("Venc Get GopAttr for %#x!\n", s32Ret);
		goto EXIT_VI_VPSS_UNBIND;
	}

	for(i = 0; i < sns_mode_set.numCam; i ++){
		s32Ret = SAMPLE_COMM_VENC_Start(VencChn[i], enPayLoad[i],sns_mode_set.enSize, sns_mode_set.maxFps, enRcMode,u32Profile[i], &stGopAttr);
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
			goto EXIT_VI_VPSS_UNBIND;
		}

		s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp[i], 0, VencChn[i]);
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("Venc Get GopAttr failed for %#x!\n", s32Ret);
			goto EXIT_VENC_H265_STOP;
		}
	}

	/******************************************
	 stream save process
	******************************************/
	SAMPLE_VENC_CONSUME_TYPE type = SAMPLE_VENC_RTSP;
	s32Ret = SAMPLE_COMM_VENC_StartGetStreamV2(VencChn, sns_mode_set.numCam, type);
	if (TS_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("Start Venc failed!\n");
		goto EXIT_VENC_H265_STOP;
	}

	// 退出
	SAMPLE_WAIT_Q_EXIT();


	/******************************************
	 exit process
	******************************************/
	SAMPLE_COMM_VENC_StopGetStream();
	SAMPLE_PRT("\nStart Exiting\n");

EXIT_VENC_H265_STOP:
	for(i = 0; i <  sns_mode_set.numCam; i ++){
		SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp[i], 0, VencChn[i]);
		SAMPLE_COMM_VENC_Stop(VencChn[i]);
	}
EXIT_VI_VPSS_UNBIND:

	if(TS_FALSE == bLowDelay){
		for(i = 0; i <	sns_mode_set.numCam; i ++){
			SAMPLE_COMM_VI_UnBind_VPSS(ViPipe[i], 0, VpssGrp[i]);
		}
	}
	else{
		SAMPLE_PRT("do NOT need unbind vi and vpss\n");
	}
EXIT_VI_STOP:
	SAMPLE_COMM_VI_StopVi(&stViConfig);
EXIT_VPSS_STOP:
	for(i = 0; i <  sns_mode_set.numCam; i ++){
		SAMPLE_COMM_VPSS_Stop(VpssGrp[i], VpssChnlNum);
	}
	SAMPLE_COMM_SYS_Exit();

	return s32Ret;
}


/******************************************************************************
* function	  : main()
* Description : video venc sample
******************************************************************************/
int main(int argc, char *argv[])
{
	
	TS_S32 s32Ret = 0;

	bS32State = 1;
	signal(SIGINT, SAMPLE_VENC_HandleSig);
	signal(SIGTERM, SAMPLE_VENC_HandleSig);

	s32Ret = SAMPLE_VENC_FASTBOOT(TS_TRUE);
	if (TS_SUCCESS == s32Ret)
	{ printf("program exit normally!\n"); }
	else
	{ printf("program exit abnormally!\n"); }

	exit(s32Ret);
}


