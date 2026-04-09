
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

//static TS_CHAR **gppcArgv = NULL;
static TS_S32 g_sample_venc_terminal_all = 0;

/******************************************************************************
* function : show usage
******************************************************************************/
void SAMPLE_VENC_Usage(char* sPrgNm)
{

	printf("\nUsage: %s [index] [sub-option]\n\n", sPrgNm);

	printf("Tips:\n");
	printf("  Press Ctrl+C to exit.\n");

	printf("Index:\n");
	printf("\t  0) H.265 encoding with RTSP stream.\n");
	printf("\t  1) MJPEG and JPEG. Press \"Enter\" to capture.\n");
	printf("\t  2) Encode 200 frames from YUV input.\n");

    return;
}

TS_S32 SAMPLE_VENC_CheckArgs(ALL_OPT_t * pOpts)
{
	TS_S32 max_choise = 3;
	TS_S32 s32Ret = TS_FAILURE;
	
	if(!pOpts){
		return TS_FAILURE;
	}

	if(pOpts->choise >= max_choise){
		return TS_FAILURE;
	}

	do{
		if(0 == pOpts->choise){
			if(pOpts->vi.wdr == TS_TRUE){
				printf("case not support!!!\n");
				s32Ret = TS_FAILURE;
				return s32Ret;
			}
		}
		else if(1 == pOpts->choise){
			if(pOpts->camNum > 1) {
				printf("case not support!!!\n");
				s32Ret = TS_FAILURE;
				return s32Ret;
			}
			if(pOpts->vi.wdr == TS_TRUE){
				printf("case not support!!!\n");
				s32Ret = TS_FAILURE;
				return s32Ret;
			}
		}
		else if(2 == pOpts->choise){
			if(pOpts->camNum > 1) {
				printf("case not support!!!\n");
				s32Ret = TS_FAILURE;
				return s32Ret;
			}
			if(pOpts->vi.wdr == TS_TRUE){
				printf("case not support!!!\n");
				s32Ret = TS_FAILURE;
				return s32Ret;
			}
			if(strlen(pOpts->infilePath) == 0) {
				printf("Need yuv file input path!!!\n");
				s32Ret = TS_FAILURE;
				return s32Ret;
			}
			if(strlen(pOpts->outfilePath)  == 0) {
				printf("Need yuv file output path!!!\n");
				s32Ret = TS_FAILURE;
				return s32Ret;
			}
			if((pOpts->venc.chnl[0].height_yuv == 0) || (pOpts->venc.chnl[0].width_yuv == 0) ){
				printf("Need yuv file encoding size!!!\n");
				s32Ret = TS_FAILURE;
				return s32Ret;
			}
		}
		else if(3 == pOpts->choise){
		}
		else if(4 == pOpts->choise){
			if(pOpts->camNum > 1){
				SAMPLE_PRT("wdr do NOT support more than one camera\n");
				break;
			}
		}
		
		s32Ret = TS_SUCCESS;
	}while(0);	
	
	return s32Ret;
}

/******************************************************************************
* function : to process abnormal case
******************************************************************************/
void SAMPLE_VENC_HandleSig(TS_S32 signo)
{
	SAMPLE_PRT("\n\n\033[0;31mReceive signal %s, exit...!\033[0;39m\n\n", strsignal(signo));

    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
	g_sample_venc_terminal_all = 1;
}


static void SAMPLE_WAIT_Q_EXIT(ALL_OPT_t *pOpts)
{
	while(!g_sample_venc_terminal_all){
		if(pOpts->vi.userpic){
			if(access("/tmp/userpic_stop", F_OK) == 0)
			{
				remove("/tmp/userpic_stop");
				SAMPLE_PRT("vi userpic stop test\n");
				for(int i = 0; i < pOpts->camNum; i++){
					TS_MPI_VI_DisableUserPic(i);
				}
			}
			if(access("/tmp/userpic_start", F_OK) == 0)
			{
				remove("/tmp/userpic_start");
				SAMPLE_PRT("vi userpic start test\n");
				for(int i = 0; i < pOpts->camNum; i++){
					TS_MPI_VI_EnableUserPic(i);
				}
			}
		}
		usleep(100);
	}
}


TS_S32 SAMPLE_VENC_VI_Init( SAMPLE_VI_CONFIG_S *pstViConfig)
{
    TS_S32              s32Ret;
    VB_CONFIG_S 		stVbConf;

	stVbConf.astCommPool[0].u64BlkSize   = 1024;
	stVbConf.astCommPool[0].u32BlkCnt	  = 1;
	stVbConf.u32MaxPoolCnt = 1;

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_Init failed!\n");
        return s32Ret;
    }
	SAMPLE_PRT("SAMPLE_COMM_SYS_Init success!\n");

    s32Ret = SAMPLE_COMM_VI_StartVi(pstViConfig);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_COMM_SYS_Exit();
        SAMPLE_PRT("SAMPLE_COMM_VI_StartVi failed with %d!\n", s32Ret);
        return s32Ret;
    }

    return TS_SUCCESS;
}

TS_S32 SAMPLE_VENC_VPSS_Init(VPSS_GRP VpssGrp, SIZE_S * pPipeSize, SIZE_S * pChnlSize, TS_S32 * pMemCount, TS_S32 chnl_num)
{
    TS_S32 s32Ret = TS_FAILURE;
    VPSS_GRP_ATTR_S stVpssGrpAttr;
    VPSS_CHN_ATTR_S stVpssChnAttr[4];

	stVpssGrpAttr.enGrpMode = VPSS_GRP_MODE_STREAM;
    stVpssGrpAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
    stVpssGrpAttr.enPixelFormat  = PIXEL_FORMAT_NV_12;
    stVpssGrpAttr.u32MaxW        = pPipeSize->u32Width;
    stVpssGrpAttr.u32MaxH        = pPipeSize->u32Height;
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;

    for(TS_S32 i = 0; i < chnl_num; i ++)
    {
    	memset(&stVpssChnAttr[i], 0, sizeof(VPSS_CHN_ATTR_S));
		stVpssChnAttr[i].u32Width					  = pChnlSize[i].u32Width;
		stVpssChnAttr[i].u32Height					  = pChnlSize[i].u32Height;
		stVpssChnAttr[i].enChnMode					  = VPSS_CHN_MODE_USER;
		stVpssChnAttr[i].enCompressMode 			  = COMPRESS_MODE_NONE;
		stVpssChnAttr[i].enDynamicRange 			  = DYNAMIC_RANGE_SDR8;
		stVpssChnAttr[i].enVideoFormat				  = VIDEO_FORMAT_LINEAR;
		stVpssChnAttr[i].enPixelFormat				  = PIXEL_FORMAT_NV_12;
		stVpssChnAttr[i].stFrameRate.s32SrcFrameRate  = -1;
		stVpssChnAttr[i].stFrameRate.s32DstFrameRate  = -1;
		stVpssChnAttr[i].u32Depth					  = 3;
		stVpssChnAttr[i].bMirror						= TS_FALSE;
		stVpssChnAttr[i].bFlip							= TS_FALSE;
		stVpssChnAttr[i].enVideoFormat					= VIDEO_FORMAT_LINEAR;
		stVpssChnAttr[i].stAspectRatio.enMode			= ASPECT_RATIO_NONE;
		stVpssChnAttr[i].u32MaxW						= pChnlSize[i].u32Width;
		stVpssChnAttr[i].u32MaxH						= pChnlSize[i].u32Height;
		stVpssChnAttr[i].u32MemCount 					= pMemCount ? pMemCount[i] : 1;
		stVpssChnAttr[i].bVpuLoop						= TS_FALSE;
    }

    s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, &stVpssGrpAttr,  stVpssChnAttr, chnl_num);
    if(s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("start VPSS fail for %#x!\n", s32Ret);
    }
    return s32Ret;
}
#if 0
#define	SAMPLE_VENC_CAM_NUM	2

TS_S32 SAMPLE_VENC_H265_H264(TS_BOOL 	bLowDelay, TS_BOOL bSaveStream)
{
	TS_S32			s32Ret, i;
	MODE_SETTING_S	sns_mode_set;

	const TS_BOOL	bIspByfly = TS_TRUE;
	VI_PIPE 		ViPipe[SAMPLE_VENC_CAM_NUM]		= {0, 1};
	SAMPLE_VI_CONFIG_S stViConfig = {0};
	SIZE_S			stSize_vipipe;

	VPSS_GRP		VpssGrp[SAMPLE_VENC_CAM_NUM]	= {0, 1};
	PIXEL_FORMAT_E	enPixelFmt = PIXEL_FORMAT_NV_12;
	SIZE_S			stSize_vpssChnl[VPSS_MAX_PHY_CHN_NUM];
	TS_S32			VpssChnlNum = 1;

	VENC_CHN		VencChn[SAMPLE_VENC_CAM_NUM]	= {0,1};
	TS_U32			u32Profile[SAMPLE_VENC_CAM_NUM] = {0,2};
	PAYLOAD_TYPE_E	enPayLoad[SAMPLE_VENC_CAM_NUM]  = {PT_H265, PT_H265};
	VENC_GOP_MODE_E enGopMode;
	VENC_GOP_ATTR_S stGopAttr;
	SAMPLE_RC_E 	enRcMode = SAMPLE_RC_CBR;

	if(TS_SUCCESS != SAMPLE_COMM_SYS_SelectMode(&sns_mode_set)){
		SAMPLE_PRT("sample select mode faild\n");
		return TS_FAILURE;
	}
	SAMPLE_COMM_SYS_GetPicSize(sns_mode_set.enSize, &stSize_vipipe);

	stSize_vpssChnl[0].u32Width 	= stSize_vipipe.u32Width;
	stSize_vpssChnl[0].u32Height	= stSize_vipipe.u32Height;

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
		system("echo dsync_ms 30 >/proc/isp_stat");
	}

	s32Ret = SAMPLE_VENC_VI_Init(&stViConfig);
	if(s32Ret != TS_SUCCESS)
	{
		SAMPLE_PRT("Init VI err for %#x!\n", s32Ret);
		return TS_FAILURE;
	}

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
	SAMPLE_VENC_CONSUME_TYPE type = bSaveStream ? SAMPLE_VENC_BOTH : SAMPLE_VENC_RTSP;
	s32Ret = SAMPLE_COMM_VENC_StartGetStreamV2(VencChn, sns_mode_set.numCam, type);
	if (TS_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("Start Venc failed!\n");
		goto EXIT_VENC_H265_STOP;
	}

	g_sample_venc_terminal_all = 0;

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
#else
TS_S32 SAMPLE_VENC_H265_H264(ALL_OPT_t * pOpts)
{
	TS_S32			s32Ret, i, j;
	TS_BOOL 		bVivpssOnline = pOpts->bValid_VivpssOnlie ? pOpts->vivpss_bOnline : TS_FALSE;

	TS_BOOL			bIspByfly = TS_FALSE;
	VI_PIPE 		ViPipe;
	SAMPLE_VI_CONFIG_S stViConfig = {0};
	SIZE_S			stSize_vipipe = {pOpts->vi.width_pipe, pOpts->vi.height_pipe};
	TS_S32			camNum = pOpts->camNum;

	VPSS_GRP		VpssGrp;
	SIZE_S			stSize_vpssChnl[VPSS_MAX_PHY_CHN_NUM];
	TS_S32			memCount_tbl[VPSS_MAX_PHY_CHN_NUM];
	TS_S32			VpssChnlNum = pOpts->vpss.chnl_count;
	
	VENC_CHN		VencChn[VENC_MAX_CHN_NUM];
	VENC_GOP_MODE_E enGopMode;
	VENC_GOP_ATTR_S stGopAttr;
	TS_S32			vencNum = camNum * VpssChnlNum;
	SAMPLE_VENC_CHNL_ATTR_S vencAttrs[VENC_MAX_CHN_NUM];

	SAMPLE_PRT("camNum=%d, vpssChnlNum=%d, vipipeSize=[%d,%d]. ispByFly=%d, vivpssOnline=%d\n", camNum,	VpssChnlNum,
		stSize_vipipe.u32Width, stSize_vipipe.u32Height,
		bIspByfly, bVivpssOnline);

	for(i = 0; i < VpssChnlNum; i ++){
		stSize_vpssChnl[i].u32Width		= pOpts->vpss.chnl[i].width_out;
		stSize_vpssChnl[i].u32Height	= pOpts->vpss.chnl[i].height_out;
		memCount_tbl[i] = pOpts->vpss.chnl[i].bMemCountInit ? pOpts->vpss.chnl[i].memCount : 1;
		SAMPLE_PRT("vpssChnl[%d:%d] : wh=[%d,%d], memCount=%d\n", VpssChnlNum, i,
			stSize_vpssChnl[i].u32Width, stSize_vpssChnl[i].u32Height,
			memCount_tbl[i]);
	}

	for(i = 0; i < camNum; i ++){
		for(j = 0; j < VpssChnlNum; j ++){
			TS_U32 venc_idx = VpssChnlNum * i + j;
			SAMPLE_VENC_CHNL_ATTR_S * pVencAttr = &vencAttrs[venc_idx];
			
			VencChn[venc_idx] = venc_idx;
			pVencAttr->enType = pOpts->venc.chnl[venc_idx].pt_type;		
			pVencAttr->u32FrameRate = pOpts->venc.chnl[venc_idx].framerate;
			pVencAttr->enRcMode = pOpts->venc.chnl[venc_idx].rc_type;
			pVencAttr->u32Profile = 0;
			pVencAttr->u32Bitrate = pOpts->venc.chnl[venc_idx].bitrate;
			pVencAttr->u32Gop = pOpts->venc.chnl[venc_idx].gop;
			memcpy(&pVencAttr->stGopAttr, &stGopAttr, sizeof(stGopAttr));
			
			SAMPLE_COMM_SYS_GetPicSizeE(stSize_vpssChnl[j].u32Width, stSize_vpssChnl[j].u32Height, &pVencAttr->enSize);

			SAMPLE_PRT("venc[%d:%d] vpss[%d]: payload=%s@%d, rcMode=%s@%d, fps=%d, gop=%d, wh=[%d,%d], sizeE=%d\n", vencNum, venc_idx, j,
				pVencAttr->enType == PT_H265 ? "h265" : "otherPT", pVencAttr->enType,
				pVencAttr->enRcMode == SAMPLE_RC_CBR ? "cbr" : "otherRC", pVencAttr->enRcMode,
				pVencAttr->u32FrameRate,
				pVencAttr->u32Gop,
				stSize_vpssChnl[j].u32Width, stSize_vpssChnl[j].u32Height,
				pVencAttr->enSize);
		}
	}

	stViConfig.s32WorkingViNum		 = camNum;
	for(i = 0; i < camNum; i ++){
		SAMPLE_PIPE_INFO_S * pstPipeInfo = &stViConfig.astViInfo[i].stPipeInfo;
		SAMPLE_CHN_INFO_S * pstChnInfo = &stViConfig.astViInfo[i].stChnInfo;

		pstPipeInfo->aPipe		= i;
		pstPipeInfo->enBitWid	= pOpts->vi.bitwidth;
		pstPipeInfo->enBayer	= pOpts->vi.bayerformat;
		pstPipeInfo->width		= stSize_vipipe.u32Width;
		pstPipeInfo->height		= stSize_vipipe.u32Height;
		pstPipeInfo->frameRate	= pOpts->vi.fps;
		pstPipeInfo->enWdrMode	= pOpts->vi.wdr ? WDR_MODE_2To1_FRAME_FULL_RATE : WDR_MODE_NONE;
		pstPipeInfo->bIspByFly	= pOpts->vi.ispbyfly;
		if( TS_TRUE == pstPipeInfo->bIspByFly)
		{
			pstPipeInfo->bDynFpsSync = TS_TRUE;
		}
		else
		{
			pstPipeInfo->bDynFpsSync = TS_FALSE;
		}
		if(TS_TRUE == bVivpssOnline){
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
		system("echo dsync_ms 30 >/proc/isp_stat");
	}

	s32Ret = SAMPLE_VENC_VI_Init(&stViConfig);
	if(s32Ret != TS_SUCCESS)
	{
		SAMPLE_PRT("Init VI err for %#x!\n", s32Ret);
		return TS_FAILURE;
	}

	for(i = 0; i < camNum; i ++){
		VpssGrp = i;
		s32Ret = SAMPLE_VENC_VPSS_Init(VpssGrp, &stSize_vipipe, stSize_vpssChnl, memCount_tbl, VpssChnlNum);
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("Init VPSS group[%d] err for %#x!\n", VpssGrp, s32Ret);
			goto EXIT_VI_STOP;
		}
		SAMPLE_PRT("Init VPSS group[%d] success!\n", VpssGrp);

		if(pOpts->vpss.grp_crop.u32Width > 0 && pOpts->vpss.grp_crop.u32Height > 0){
			s32Ret = SAMPLE_COMM_VPSS_GrpCrop(VpssGrp, 0, 0, pOpts->vpss.width_crop, pOpts->vpss.height_crop);
		}
	}

	if(TS_FALSE == bVivpssOnline){
		for(i = 0; i < camNum; i ++){
			ViPipe = i;
			VpssGrp = i;
			s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, 0, VpssGrp);
			if(s32Ret != TS_SUCCESS)
			{
				SAMPLE_PRT("VI Bind VPSS err for %#x!\n", s32Ret);
				goto EXIT_VI_STOP;
			}
		}
	}
	else {
		SAMPLE_PRT("do NOT need VI Bind VPSS \n");
	}

   if (TS_SUCCESS != TS_MPI_VPSS_Start_Camera()) {
	   SAMPLE_PRT("TS_MPI_VPSS_Start_Camera fail !\n");
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

	for(i = 0; i < camNum; i ++){
		for(j = 0; j < VpssChnlNum; j ++){
			VENC_CHN vencIdx = VpssChnlNum * i + j;
			s32Ret = SAMPLE_COMM_VENC_StartEx(VencChn[vencIdx], &vencAttrs[vencIdx]);
			if (TS_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
				goto EXIT_VI_VPSS_UNBIND;
			}

			s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(i, j, VencChn[vencIdx]);
			if (TS_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("Venc Get GopAttr failed for %#x!\n", s32Ret);
				goto EXIT_VENC_H265_STOP;
			}
		}
		
	}

	/******************************************
	 stream save process
	******************************************/
	SAMPLE_VENC_CONSUME_TYPE type = SAMPLE_VENC_RTSP;
	if(pOpts->bOutfileValid){
		SAMPLE_COMM_VENC_SetTestSavePath(pOpts->outfilePath);
		type = SAMPLE_VENC_BOTH;
	}
	s32Ret = SAMPLE_COMM_VENC_StartGetStreamV2(VencChn, vencNum, type);
	if (TS_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("Start Venc failed!\n");
		goto EXIT_VENC_H265_STOP;
	}

	/******************************************
	 set vi userpic
	******************************************/
	if(pOpts->vi.userpic){
		for(i = 0; i < camNum; i++){
			ViPipe = i;
			VI_USERPIC_ATTR_S viUserpicInfo = {0};
			viUserpicInfo.enUsrPicMode = VI_USERPIC_MODE_BGC;
			viUserpicInfo.unUsrPic.stUsrPicBg.u32BgColor = 0xFF0080;
			s32Ret = TS_MPI_VI_SetUserPic(ViPipe, &viUserpicInfo);
			if(s32Ret != TS_SUCCESS)
			{
				SAMPLE_PRT("TS_MPI_VI_SetUserPic camid %d err for %#x!\n", i, s32Ret);
				continue;
			}
		}
	}

	g_sample_venc_terminal_all = 0;

	// 退出
	SAMPLE_WAIT_Q_EXIT(pOpts);


	/******************************************
	 exit process
	******************************************/
	SAMPLE_COMM_VENC_StopGetStream();
	SAMPLE_PRT("\nStart Exiting\n");

EXIT_VENC_H265_STOP:
	for(i = 0; i <  camNum; i ++){
		for(j = 0; j < VpssChnlNum; j ++){
			VENC_CHN vencIdx = VpssChnlNum * i + j;
			SAMPLE_COMM_VPSS_UnBind_VENC(i, j, VencChn[vencIdx]);
			SAMPLE_COMM_VENC_Stop(VencChn[vencIdx]);
		}
	}
EXIT_VI_VPSS_UNBIND:

	if(TS_FALSE == bVivpssOnline){
		for(i = 0; i <	camNum; i ++){
			SAMPLE_COMM_VI_UnBind_VPSS(i, 0, i);
		}
	}
	else{
		SAMPLE_PRT("do NOT need unbind vi and vpss\n");
	}
	
EXIT_VI_STOP:
	if(TS_TRUE == bVivpssOnline){
		SAMPLE_COMM_VI_StopVi(&stViConfig);
		for(i = 0; i <  camNum; i ++){
			SAMPLE_COMM_VPSS_Stop(i, VpssChnlNum);
		}
	} else {
		for(i = 0; i <	camNum; i ++){
			SAMPLE_COMM_VPSS_Stop(i, VpssChnlNum);
		}
		SAMPLE_COMM_VI_StopVi(&stViConfig);
	}
	SAMPLE_COMM_SYS_Exit();

	return s32Ret;
}
#endif

#define SAMPLE_VPSS_GRP_NUM_ONE 1
#define SAMPLE_VPSS_CHN_NUM_ONE 1
#define SAMPLE_VPSS_CHN_NUM_TWO 2
#define SAMPLE_VENC_CHN_NUM_ONE 1
#define SAMPLE_VENC_CHN_NUM_TWO 2

/******************************************************************************
* function: Mjpeg + Jpeg, Channel resolution adaptable with sensor
*
* Note:
*		1. default save path: /tmp. If you want to save to other path,
* 		   please modify /tmp/save_path.txt with new path.
*
* 		2. the mjpeg will save for 3 seconds.
*
******************************************************************************/
TS_S32 SAMPLE_VENC_MJPEG_JPEG(ALL_OPT_t * pOpts)
{
	TS_S32			s32Ret, i;

	const TS_BOOL	bIspByfly = TS_TRUE;
	VI_PIPE 		ViPipe[SAMPLE_VPSS_GRP_NUM_ONE]		= {0};
	SAMPLE_VI_CONFIG_S stViConfig = {0};
	SIZE_S			stSize_vipipe = {pOpts->vi.width_pipe, pOpts->vi.height_pipe};;

	VPSS_GRP		VpssGrp[SAMPLE_VPSS_GRP_NUM_ONE]	= {0};
	SIZE_S			stSize_vpssChnl[VPSS_MAX_PHY_CHN_NUM];
	TS_S32			VpssChnlNum = SAMPLE_VPSS_CHN_NUM_TWO;

	VENC_CHN		VencChn[SAMPLE_VENC_CHN_NUM_TWO]	= {0, 1};
	PAYLOAD_TYPE_E	enPayLoad[SAMPLE_VENC_CHN_NUM_TWO]  = {PT_MJPEG, PT_JPEG};
	TS_U32			u32Profile[SAMPLE_VENC_CHN_NUM_TWO] = {0};
	VENC_GOP_MODE_E enGopMode;
	VENC_GOP_ATTR_S stGopAttr;
	SAMPLE_RC_E 	enRcMode = SAMPLE_RC_CBR;
	TS_S32			bSupportDcf = 0;
	TS_CHAR			ch;
	TS_U32			u32SaveCount = 5;
	PIC_SIZE_E		enSize;
	
	SAMPLE_COMM_SYS_GetPicSizeE(stSize_vipipe.u32Width, stSize_vipipe.u32Height, &enSize);

	stViConfig.s32WorkingViNum		 = 1;

	for(i = 0; i < stViConfig.s32WorkingViNum; i ++){

		SAMPLE_PIPE_INFO_S * pstPipeInfo = &stViConfig.astViInfo[i].stPipeInfo;
		SAMPLE_CHN_INFO_S * pstChnInfo = &stViConfig.astViInfo[i].stChnInfo;

		pstPipeInfo->aPipe		= i;
		pstPipeInfo->enBitWid	= pOpts->vi.bitwidth;
		pstPipeInfo->enBayer	= pOpts->vi.bayerformat;
		pstPipeInfo->width		= stSize_vipipe.u32Width;
		pstPipeInfo->height		= stSize_vipipe.u32Height;
		pstPipeInfo->frameRate	= pOpts->vi.fps;
		pstPipeInfo->enWdrMode	= TS_FALSE;
		pstPipeInfo->bIspByFly	= TS_FALSE;
		pstPipeInfo->bDynFpsSync= TS_FALSE;
		if(TS_TRUE == pOpts->vivpss_bOnline){
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

	s32Ret = SAMPLE_VENC_VI_Init(&stViConfig);
	if(s32Ret != TS_SUCCESS)
	{
		SAMPLE_PRT("Init VI err for %#x!\n", s32Ret);
		return TS_FAILURE;
	}

	for (i = 0; i < SAMPLE_VPSS_CHN_NUM_TWO; i++)
	{
		stSize_vpssChnl[i].u32Width 	= stSize_vipipe.u32Width;
		stSize_vpssChnl[i].u32Height	= stSize_vipipe.u32Height;
	}

	for(i = 0; i < SAMPLE_VPSS_GRP_NUM_ONE; i ++){
		s32Ret = SAMPLE_VENC_VPSS_Init(VpssGrp[i], &stSize_vipipe, stSize_vpssChnl, TS_NULL, VpssChnlNum);
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

	if(TS_FALSE == pOpts->vivpss_bOnline){
		for(i = 0; i < SAMPLE_VPSS_GRP_NUM_ONE; i ++){
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


   /******************************************
     start stream venc
    ******************************************/

    enRcMode = SAMPLE_RC_CBR;
	enGopMode = VENC_GOPMODE_NORMALP;

    s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode,&stGopAttr);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }


   /***encode Mjpege **/
    SAMPLE_COMM_VENC_Start_By_Num(VencChn[0], enPayLoad[0],enSize, pOpts->venc.chnl[0].framerate, enRcMode, u32Profile[0], &stGopAttr, u32SaveCount);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp[0], 0, VencChn[0]);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr failed for %#x!\n", s32Ret);
        goto EXIT_VENC_MJPEGE_STOP;
    }

    /***encode Jpege **/
    s32Ret = SAMPLE_COMM_VENC_SnapStart(VencChn[1],&stSize_vipipe, bSupportDcf);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VENC_MJPEGE_UnBind;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(VpssGrp[0], 1, VencChn[1]);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc bind Vpss failed for %#x!\n", s32Ret);
        goto EXIT_VENC_JPEGE_STOP;
    }

	printf("\n\n############# save mjpeg start! #############\n\n");
    /******************************************
     *stream save process
    ******************************************/
	SAMPLE_VENC_CONSUME_TYPE type = SAMPLE_VENC_FILE;
	if(pOpts->bOutfileValid){
		SAMPLE_COMM_VENC_SetTestSavePath(pOpts->outfilePath);
	}
	s32Ret = SAMPLE_COMM_VENC_StartGetStreamV2(VencChn, 1, type); // receive mjpeg
	if (TS_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("Start Venc failed!\n");
		goto EXIT_VENC_JPEGE_UnBind;
	}

	while(SAMPLE_COMM_VENC_GetRecvNum(VencChn[0]) < 5){
		usleep(1000*100);
	}

	/******************************************
	* save mjpeg for 2s
    ******************************************/
	SAMPLE_COMM_VENC_StopGetStream();
    s32Ret = TS_MPI_VENC_StopRecvFrame(VencChn[0]);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VENC_StopRecvFrame faild with%#x! \n", s32Ret);
        return TS_FAILURE;
    }
	printf("\n\n############# save mjpeg finish! #############\n\n");

    /******************************************
     stream venc process -- get jpeg stream, then save it to file.
    ******************************************/

	printf("\n\n############# jpeg snap start! #############\n\n");
	printf("	press 'q' to exit snap!\n");
	printf("	press ENTER to capture one picture to file\n");

    i = 0;
    while ((ch = (char)getchar()) != 'q')
    {
	    if (ch != '\n')
			continue;

        s32Ret = SAMPLE_COMM_VENC_SnapProcess(VencChn[1], 1, TS_TRUE, TS_TRUE);
        if (TS_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("%s: sanp process failed!\n", __FUNCTION__);
            break;
        }
        SAMPLE_PRT("snap %d success!\n\n\n", i);

		printf("	press 'q' to exit snap!\n");
		printf("	press ENTER to capture one picture to file\n");

        i++;
    }


    /******************************************
     exit process
    ******************************************/

EXIT_VENC_JPEGE_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp[0], 1, VencChn[1]);
EXIT_VENC_JPEGE_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[1]);
EXIT_VENC_MJPEGE_UnBind:
    SAMPLE_COMM_VPSS_UnBind_VENC(VpssGrp[0], 0, VencChn[0]);
EXIT_VENC_MJPEGE_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[0]);
EXIT_VI_VPSS_UNBIND:
	if(TS_FALSE == pOpts->vivpss_bOnline)
	    SAMPLE_COMM_VI_UnBind_VPSS(ViPipe[i], 0, VpssGrp[0]);
EXIT_VI_STOP:
    SAMPLE_COMM_VI_StopVi(&stViConfig);
EXIT_VPSS_STOP:
    SAMPLE_COMM_VPSS_Stop(VpssGrp[0], VpssChnlNum);
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

static TS_S32 SAMPLE_SYS_Init()
{
    TS_S32 s32Ret;
    VB_CONFIG_S stVbConf = {0};

	stVbConf.astCommPool[0].u64BlkSize   = 640*360*3/2;
	stVbConf.astCommPool[0].u32BlkCnt	  = 1;
	stVbConf.u32MaxPoolCnt = 1;
	SAMPLE_PRT("blk size %llu, blk cnt %u, max pool cnt %u\n", stVbConf.astCommPool[0].u64BlkSize ,
		stVbConf.astCommPool[0].u32BlkCnt, stVbConf.u32MaxPoolCnt);

   s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
        return s32Ret;
    }

    return TS_SUCCESS;
}


static TS_S32 SAMPLE_VENC_INIT_FRAME_INFO(VIDEO_FRAME_INFO_S *pFrame, SIZE_S *pstSize, VB_BLK VbBlk, VB_POOL pool){

	void *viraddr = NULL;
	TS_U64 phyaddr = TS_MPI_VB_Handle2PhysAddr(VbBlk);

	TS_MPI_VB_GetBlockVirAddr(pool, phyaddr, &viraddr);

	pFrame->u32PoolId = TS_MPI_VB_Handle2PoolId(VbBlk);
	pFrame->stVFrame.enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420;//PIXEL_FORMAT_YVU_PLANAR_420;
	pFrame->stVFrame.enField = VIDEO_FIELD_FRAME;
	pFrame->stVFrame.enCompressMode = COMPRESS_MODE_NONE;
	pFrame->stVFrame.u64PhyAddr[0] = phyaddr;
	pFrame->stVFrame.u64PhyAddr[1] = pFrame->stVFrame.u64PhyAddr[0] + pstSize->u32Width * pstSize->u32Height;
	pFrame->stVFrame.u64PhyAddr[2] = pFrame->stVFrame.u64PhyAddr[1] + pstSize->u32Width * pstSize->u32Height / 2;

	pFrame->stVFrame.u64VirAddr[0] = (TS_U64)(unsigned long)viraddr;
	pFrame->stVFrame.u64VirAddr[1] = pFrame->stVFrame.u64VirAddr[0] + pstSize->u32Width * pstSize->u32Height;
	pFrame->stVFrame.u64VirAddr[2] = pFrame->stVFrame.u64VirAddr[0] + pstSize->u32Width * pstSize->u32Height * 3 / 2;

	pFrame->stVFrame.u32Width  = pstSize->u32Width;
	pFrame->stVFrame.u32Height = pstSize->u32Height;
	pFrame->stVFrame.u32Stride[0] = pstSize->u32Width;
	pFrame->stVFrame.u32Stride[1] = pstSize->u32Width;
	pFrame->stVFrame.u32Stride[2] = pstSize->u32Width;

	pFrame->stVFrame.size = pFrame->stVFrame.u32Width * pFrame->stVFrame.u32Height * 3 / 2;

	// PTS
	struct timeval tv;
	gettimeofday( &tv, NULL );

	pFrame->stVFrame.u64PTS	= (tv.tv_sec * 1000 + tv.tv_usec / 1000);

	return TS_SUCCESS;
}

static TS_S32 SAMPLE_VENC_READ_YUV(FILE * pFile, TS_U8 *pBuf, SIZE_S  stSize){

	TS_S32 u32ReadLen = 0;

	TS_S32 u32Nv12Len = stSize.u32Width * stSize.u32Height * 3 / 2;

	u32ReadLen = fread(pBuf, 1, u32Nv12Len, pFile);
	if(u32ReadLen < u32Nv12Len){
		fseek(pFile, 0, SEEK_SET);
		u32ReadLen = fread(pBuf, 1, u32Nv12Len, pFile);
	}

	SAMPLE_PRT("u32ReadLen = %u u32Nv12Len = %u\n", u32ReadLen, u32Nv12Len);

	return u32ReadLen == u32Nv12Len ? TS_SUCCESS : TS_FAILURE;
}

/**
 * @brief 根据整理发送数据
 * @param u32Chn
 * @param pYuvPath
 * @param stSize
 * @author Tsingmicro
 * @date 2024-12-30
 * @return
 */
static TS_S32 PDT_SAMPLE_VENC_SendFrameByFps(TS_U32 u32Chn, TS_CHAR *pYuvPath, SIZE_S  stSize, TS_S32 s32Num)
{
	if (NULL == pYuvPath) {
		SAMPLE_PRT("Param Is Null !. \n");
		return TS_FAILURE;
	}

	FILE *pFile = NULL;
	pFile = fopen(pYuvPath, "rb");
	if(!pFile){
		SAMPLE_PRT("open file [%s] faild\n", pYuvPath);
		// goto FAILED;
		return TS_FAILURE;
	}

    TS_U64 u64BlkSize = 0;
	TS_S32 u32SendNum = 0;
	TS_U32 u32SendNumTotal = 0;
	TS_S32 s32Ret = TS_FAILURE;
	VB_POOL hPool  = VB_INVALID_POOLID;
	VB_POOL_CONFIG_S stVbPoolCfg = {0};

	u32SendNumTotal = s32Num; //15 * 20 frames

    u64BlkSize = COMMON_GetPicBufferSize(stSize.u32Width,
										 stSize.u32Height,
										 PIXEL_FORMAT_YVU_SEMIPLANAR_420,
										 DATA_BITWIDTH_8,
										 COMPRESS_MODE_SEG,DEFAULT_ALIGN);

	// VB init
	stVbPoolCfg.u64BlkSize = u64BlkSize;
	stVbPoolCfg.u32BlkCnt = 2;

	hPool   = TS_MPI_VB_CreatePool(&stVbPoolCfg);
	if (hPool == VB_INVALID_POOLID)
	{
	    SAMPLE_PRT("TS_MPI_VB_CreatePool failed, blkSize=%lld, count=%d\n", stVbPoolCfg.u64BlkSize, stVbPoolCfg.u32BlkCnt);
		goto FAILED;
	}

	SAMPLE_PRT("TS_MPI_VB_CreatePool success, blkSize=%lld, u32BlkCnt=%d\n", u64BlkSize, stVbPoolCfg.u32BlkCnt);

	TS_MPI_VB_MmapPool(hPool);

	// send frame
	while(u32SendNum++ < u32SendNumTotal){
		VB_BLK VbBlk = VB_INVALID_HANDLE;
		VIDEO_FRAME_INFO_S vFrmInfo = {0};

		VbBlk = TS_MPI_VB_GetBlock(hPool, u64BlkSize, NULL);
		if (VB_INVALID_HANDLE == VbBlk)
		{
			SAMPLE_PRT("TS_MPI_VB_GetBlock err! size:%lld\n", u64BlkSize);
			break;
		}

		SAMPLE_VENC_INIT_FRAME_INFO(&vFrmInfo, &stSize, VbBlk, hPool);

		// read yuv file
		s32Ret = SAMPLE_VENC_READ_YUV(pFile, (TS_U8 *)(TS_UL)(vFrmInfo.stVFrame.u64VirAddr[0]), stSize);
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_VENC_READ_YUV failed!\n");
			goto FAILED;
		}

		// send frame
		s32Ret = TS_MPI_VENC_SendFrame(u32Chn, &vFrmInfo, -1);
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("TS_MPI_VENC_SendFrame failed!\n");
			goto FAILED;
		}

		SAMPLE_PRT("send frame success, num: %d!\n", u32SendNum);

		TS_MPI_VB_ReleaseBlock(VbBlk);
	}

	return TS_SUCCESS;

FAILED:
	return TS_FAILURE;
}


TS_S32 SAMPLE_VENC_FEED_YUV(ALL_OPT_t * pOpts)
{
    TS_S32 			s32Ret;
	//TS_CHAR 		pYuvFile[128] = {0};
	VENC_CHN		VencChn[SAMPLE_VENC_CHN_NUM_ONE]	= {0};
	PAYLOAD_TYPE_E	enPayLoad[SAMPLE_VENC_CHN_NUM_ONE]  = {pOpts->venc.chnl[0].pt_type};
	TS_U32			u32Profile[SAMPLE_VENC_CHN_NUM_ONE] = {0};
	VENC_GOP_MODE_E enGopMode;
	VENC_GOP_ATTR_S stGopAttr;
	SAMPLE_RC_E 	enRcMode;
	SIZE_S			stSize_venc = {pOpts->venc.chnl[0].width_yuv, pOpts->venc.chnl[0].height_yuv};
	PIC_SIZE_E		enSize;
	
	SAMPLE_COMM_SYS_GetPicSizeE(stSize_venc.u32Width, stSize_venc.u32Height, &enSize);
	//strcpy(pYuvFile, gppcArgv[2]);

   /******************************************
    * sys init
    ******************************************/
	s32Ret = SAMPLE_SYS_Init();
	if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr for %#x!\n", s32Ret);
        goto EXIT_SYS_EXIT;
    }

   /******************************************
    * start stream venc
    ******************************************/

    enRcMode = SAMPLE_RC_CBR;
	enGopMode = VENC_GOPMODE_NORMALP;
    s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode,&stGopAttr);

	if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Get GopAttr for %#x!\n", s32Ret);
        goto EXIT_SYS_EXIT;
    }

    SAMPLE_COMM_VENC_Start(VencChn[0], enPayLoad[0],enSize, pOpts->venc.chnl[0].framerate, enRcMode, u32Profile[0], &stGopAttr);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_SYS_EXIT;
    }

    /******************************************
     stream save process
    ******************************************/
    if(pOpts->bOutfileValid){
		SAMPLE_COMM_VENC_SetTestSavePath(pOpts->outfilePath);
	}
    s32Ret = SAMPLE_COMM_VENC_StartGetStreamV2(VencChn, 1, SAMPLE_VENC_FILE);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Start Venc failed!\n");
        goto EXIT_VENC_STOP;
    }

	s32Ret = PDT_SAMPLE_VENC_SendFrameByFps(VencChn[0], pOpts->infilePath, stSize_venc, pOpts->venc.chnl[0].pt_type == PT_H265 ? 200 : 5);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("send yuv failed!\n");
        goto EXIT_VENC_STOP;
    }

    printf("exit now ...\n");
	sleep(2);


    /******************************************
     exit process
    ******************************************/
    SAMPLE_COMM_VENC_StopGetStream();

EXIT_VENC_STOP:
    SAMPLE_COMM_VENC_Stop(VencChn[0]);

EXIT_SYS_EXIT:
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
	ALL_OPT_t all_opt = {0};

	if(TS_SUCCESS != sample_comm_parse_args(argc, argv, 3, SAMPLE_VENC_CheckArgs, &all_opt)){
		SAMPLE_VENC_Usage(argv[0]);
		return TS_FAILURE;
	}

	signal(SIGINT, SAMPLE_VENC_HandleSig);
	signal(SIGTERM, SAMPLE_VENC_HandleSig);

	switch (all_opt.choise)
	{
		case 0:
			s32Ret = SAMPLE_VENC_H265_H264(&all_opt);
			break;
		case 1:
			s32Ret = SAMPLE_VENC_MJPEG_JPEG(&all_opt);
			break;
		case 2:
			s32Ret = SAMPLE_VENC_FEED_YUV(&all_opt);
			break;
		default:
			printf("the index is invaild!\n");
			SAMPLE_VENC_Usage(argv[0]);
			return TS_FAILURE;
	}

	if (TS_SUCCESS == s32Ret)
	{ printf("program exit normally!\n"); }
	else
	{ printf("program exit abnormally!\n"); }

	exit(s32Ret);
}

