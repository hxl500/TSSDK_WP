#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

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
static TS_S32 g_sample_zoom_status = 0;
static pthread_t g_sample_zoom_ctrl_thread;

static TS_S32 SAMPLE_ZOOM_VPSS_DumpYuv(VPSS_GRP VpssGrp, VPSS_CHN VpssChn)
{
    TS_S32 ret;
    VIDEO_FRAME_INFO_S stFrameInfo = {0};
    ret = TS_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &stFrameInfo, -1);
    if (TS_SUCCESS != ret) {
        SAMPLE_PRT("TS_MPI_VPSS_GetChnFrame [%d:%d] faild with %#x!\n", VpssGrp, VpssChn, ret);
        return TS_FAILURE;
    }
	
	// send frame
	ret = TS_MPI_VENC_SendFrame(0, &stFrameInfo, -1);
	if (TS_SUCCESS != ret)
	{
		SAMPLE_PRT("TS_MPI_VENC_SendFrame failed!\n");
	}
	
    ret = TS_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &stFrameInfo);
    if (TS_SUCCESS != ret)
    {
        SAMPLE_PRT("TS_MPI_VPSS_ReleaseChnFrame [%d:%d] faild with %#x!\n", VpssGrp, VpssChn, ret);
        return TS_FAILURE;
    }

    return TS_SUCCESS;
}


static void* SAMPLE_ZOOM_Start(void* arg)
{
	FILE *pFile1 = NULL,*pFile2 = NULL;
	VPSS_GRP VpssGrp = 0;
	VPSS_CHN VpssChn = 0;
	char     flag = 0;

	while(g_sample_zoom_status)
	{
		pFile1 = fopen("/tmp/zoom.0", "rb");

		if(pFile1 == NULL)
		{
			pFile1 = fopen("/tmp/zoom.1", "rb");
			
			if(pFile1 == NULL)
			{
				VpssGrp = 0;
				VpssChn = 0;
				if(flag)
				{
					flag = 0;
					SAMPLE_COMM_VPSS_GrpCrop(VpssGrp, 0, 0, 1920, 1080);
				}
			}
			else
			{
				fclose(pFile1);
				pFile1 = NULL;
				VpssGrp = 1;
				VpssChn = 0;
				pFile2 = fopen("/tmp/zoom.N", "rb");
				if(pFile2 != NULL)
				{
					fclose(pFile2);
					pFile2 = NULL;
					if(!flag)
						SAMPLE_COMM_VPSS_GrpCrop(VpssGrp, 0, 0, 800, 800);
					flag = 1;
				}
				else
				{
					if(flag)
						SAMPLE_COMM_VPSS_GrpCrop(VpssGrp, 0, 0, 1920, 1080);
					flag = 0;
				}
			}
		}
		else
		{
			fclose(pFile1);
			pFile1 = NULL;
			
			VpssGrp = 0;
			VpssChn = 0;
			
			pFile2 = fopen("/tmp/zoom.N", "rb");
			if(pFile2 != NULL)
			{
				fclose(pFile2);
				pFile2 = NULL;
				if(!flag)
					SAMPLE_COMM_VPSS_GrpCrop(VpssGrp, 0, 0, 800, 800);
				flag = 1;
			}
			else
			{
				if(flag)
					SAMPLE_COMM_VPSS_GrpCrop(VpssGrp, 0, 0, 1920, 1080);
				flag = 0;
			}
		}

		SAMPLE_ZOOM_VPSS_DumpYuv(VpssGrp,VpssChn);
	}
	return NULL;
}

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
	
	g_sample_zoom_status = 0;
	pthread_join(g_sample_zoom_ctrl_thread, NULL);
	
	g_sample_venc_terminal_all = 1;
}

static void SAMPLE_WAIT_Q_EXIT(void)
{
	while(!g_sample_venc_terminal_all){
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
	TS_S32			vencNum = 1;
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
			enGopMode = VENC_GOPMODE_NORMALP;
		    s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode,&stGopAttr);
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
	}

	if (TS_SUCCESS != TS_MPI_VPSS_Start_Camera()) {
		SAMPLE_PRT("TS_MPI_VPSS_Start_Camera fail !\n");
	}

	if(TS_FALSE == bVivpssOnline){
		for(i = 0; i < camNum; i ++){
			ViPipe = i;
			VpssGrp = i;
			s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, 0, VpssGrp);
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

	s32Ret = SAMPLE_COMM_VENC_StartEx(VencChn[0], &vencAttrs[0]);
	if (TS_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
		goto EXIT_VI_VPSS_UNBIND;
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

	g_sample_venc_terminal_all = 0;
	
	// create zoom thread
	if(!g_sample_zoom_status)
	{
		g_sample_zoom_status = 1;
		s32Ret = pthread_create(&g_sample_zoom_ctrl_thread, NULL, SAMPLE_ZOOM_Start, NULL);
		if (s32Ret != 0) {
			g_sample_zoom_status = 0;
		}
	}

	// 退出
	SAMPLE_WAIT_Q_EXIT();

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
	SAMPLE_COMM_VI_StopVi(&stViConfig);
EXIT_VPSS_STOP:
	for(i = 0; i <  camNum; i ++){
		SAMPLE_COMM_VPSS_Stop(i, VpssChnlNum);
	}
	SAMPLE_COMM_SYS_Exit();

	return s32Ret;
}

#define SAMPLE_VPSS_GRP_NUM_ONE 1
#define SAMPLE_VPSS_CHN_NUM_ONE 1
#define SAMPLE_VPSS_CHN_NUM_TWO 2
#define SAMPLE_VENC_CHN_NUM_ONE 1
#define SAMPLE_VENC_CHN_NUM_TWO 2

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

	all_opt.bValid_VivpssOnlie = TS_TRUE;
	all_opt.vivpss_bOnline = TS_TRUE;
	all_opt.vi.width_pipe = 1920;
	all_opt.vi.height_pipe = 1080;
	all_opt.camNum = 2;
	all_opt.vpss.chnl_count = 1;

	switch (all_opt.choise)
	{
		case 0:
			s32Ret = SAMPLE_VENC_H265_H264(&all_opt);
			break;
		default:
			printf("the index is invaild!\n");
			SAMPLE_VENC_Usage(argv[0]);
			return TS_FAILURE;
	}

	if (TS_SUCCESS == s32Ret)
	{ 
		printf("program exit normally!\n"); 
	}
	else
	{
		printf("program exit abnormally!\n"); 
	}

	exit(s32Ret);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

