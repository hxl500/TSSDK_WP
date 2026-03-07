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
#include <sys/prctl.h>

#include "sample_comm_isp.h"
#include "sample_alg_cpm.h"
#include "soft_line.h"

extern SAMPLE_VIDEO_ALG_CPM gstSampleVideoAlgCpm[2];

typedef struct {
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	VENC_CHN VencChn;
	TS_U32 u32Width;
	TS_U32 u32Height;
	TS_BOOL bRunFlag;
	pthread_t tid;
	SAMPLE_ALG_RESULT_S stAlgResult;
	SAMPLE_ALG_INSTANCE_S *pstAlgInstList[SAMPLE_ALG_MAX_NUM];
	TS_U32 u32ActualAlgNum;
	pthread_mutex_t stResultLock;
	TS_BOOL bResultUpdate;
	pthread_t tidResultSync;
	TS_S32 s32CPMGrp;
} VPSS_VENC_DRAW_BOX_S;

static TS_VOID *VPSS_VENC_DrawBox_ResultSync_Thread(TS_VOID *pParam);
static TS_VOID *VPSS_VENC_DrawBox_Thread(TS_VOID *pParam);
TS_S32 SAMPLE_VPSS_VENC_DrawBox_Start(VPSS_VENC_DRAW_BOX_S *pDrawBox, 
									   VPSS_GRP VpssGrp, VPSS_CHN VpssChn, 
									   VENC_CHN VencChn, TS_U32 u32Width, TS_U32 u32Height,
									   SAMPLE_ALG_INSTANCE_S **pstAlgInstList, TS_U32 u32AlgNum,
									   TS_S32 s32CPMGrp);
TS_S32 SAMPLE_VPSS_VENC_DrawBox_Stop(VPSS_VENC_DRAW_BOX_S *pDrawBox);

char *g_pcModelRoot = TS_NULL;

static TS_U32 g_sample_algo_terminal_all = 0;

static TS_S32 grp_num = 2;//2; // 2;hxl 双目标识

static ts_void fastboot_insmod()
{
	system("insmod /usr/lib/modules/ts_codec.ko");
	system("insmod /usr/lib/modules/hx280enc.ko");
	system("insmod /usr/lib/modules/mpp_proc.ko");
	SAMPLE_PRT("insmod for fastboot finish\n");
}

char get_input_dbg(void)
{
	FILE *file;
	char c;
	size_t ret;
	file = fopen("./input_dbg", "r");
	if (!file)
		return 0x0;

	ret = fread(&c, 1, 1, file);
	if (1 != ret)
	{
		fclose(file);
		return 0x0;
	}

	fclose(file);
	system("rm -f ./input_dbg");
	return c;
}

void simple_algo_interact_loop(void)
{
	// TS_S32 i = 0;
	char c;
	int bail_flag = TS_FALSE;
	// VI_PIPE	ViPipe = 0;
	TS_S32 ret;
	VPSS_CHN_ATTR_S stVpssChnAttr = {0};

	printf("\n-----------------------------\n"
		   "e: exit, f: flip, m:mirror, p:snap\n"
		   "-----------------------------\n\n");

	system("rm ./input_dbg -rf");

	g_sample_algo_terminal_all = 0;

	for (;;)
	{
		if (g_sample_algo_terminal_all)
			break;
		c = get_input_dbg(); // getchar();
		switch (c)
		{
		case 'e':
			bail_flag = TS_TRUE;
			break;
		case 'f':
			SAMPLE_PRT("is going to flip\n");
			ret = TS_MPI_VPSS_GetChnAttr(0, 0, &stVpssChnAttr);
			if (TS_SUCCESS != ret)
			{
				SAMPLE_PRT("TS_MPI_VPSS_GetChnAttr [0:0] faild with %#x! ===\n", ret);
				break;
			}
			/*SAMPLE_PRT("get vpss chnl[0:0}, maxWH=[%d,%d], wh=[%d,%d], memCount=%d, mf=[%d,%d]\n",
				stVpssChnAttr.u32MaxW, stVpssChnAttr.u32MaxH,
				stVpssChnAttr.u32Width, stVpssChnAttr.u32Height,
				stVpssChnAttr.u32MemCount,
				stVpssChnAttr.bMirror, stVpssChnAttr.bFlip);
			*/
			stVpssChnAttr.bFlip = !stVpssChnAttr.bFlip;
			ret = TS_MPI_VPSS_SetChnAttr(0, 0, &stVpssChnAttr);
			if (TS_SUCCESS != ret)
			{
				SAMPLE_PRT("TS_MPI_VPSS_SetChnAttr [0:0] faild with %#x! ===\n", ret);
				break;
			}
			break;

		case 'm':
			SAMPLE_PRT("is going to mirror\n");
			ret = TS_MPI_VPSS_GetChnAttr(0, 0, &stVpssChnAttr);
			if (TS_SUCCESS != ret)
			{
				SAMPLE_PRT("TS_MPI_VPSS_GetChnAttr [0:0] faild with %#x! ===\n", ret);
				break;
			}
			/*SAMPLE_PRT("get vpss chnl[0:0}, maxWH=[%d,%d], wh=[%d,%d], memCount=%d, mf=[%d,%d]\n",
				stVpssChnAttr.u32MaxW, stVpssChnAttr.u32MaxH,
				stVpssChnAttr.u32Width, stVpssChnAttr.u32Height,
				stVpssChnAttr.u32MemCount,
				stVpssChnAttr.bMirror, stVpssChnAttr.bFlip);
			*/
			stVpssChnAttr.bMirror = !stVpssChnAttr.bMirror;
			ret = TS_MPI_VPSS_SetChnAttr(0, 0, &stVpssChnAttr);
			if (TS_SUCCESS != ret)
			{
				SAMPLE_PRT("TS_MPI_VPSS_SetChnAttr [0:0] faild with %#x! ===\n", ret);
				break;
			}
			break;
		/*case 'N':
			ret = SAMPLE_COMM_VI_SetDayNight(ViPipe, 1);
			if(TS_SUCCESS != ret){
				SAMPLE_PRT("SAMPLE_COMM_VI_SetDayNight to IR mode!\n");
			}
			else{
				SAMPLE_PRT("SAMPLE_COMM_VI_SetDayNight to IR mode success!\n");
			}
			break;
		case 'D':
			ret = SAMPLE_COMM_VI_SetDayNight(ViPipe, 0);
			if(TS_SUCCESS != ret){
				SAMPLE_PRT("SAMPLE_COMM_VI_SetDayNight to DAY mode!\n");
			}
			else{
				SAMPLE_PRT("SAMPLE_COMM_VI_SetDayNight to DAY mode!\n");
			}
			break;
		*/
		default:
			printf("Invalid input[%c]!\n", c);
			printf("\n-----------------------------\n"
				   "e: exit, f: flip, m:mirror, p:snap\n"
				   "-----------------------------\n\n");
			break;
		}

		if (bail_flag)
			break;

		sleep(1);
	}

	return;
}

static TS_S32 SAMPLE_ALG_GetSizeBySensor(SAMPLE_SNS_TYPE_E enMode, PIC_SIZE_E *penSize)
{
	TS_S32 s32Ret = TS_SUCCESS;

	if (!penSize)
	{
		return TS_FAILURE;
	}

	switch (enMode)
	{
	case X86_LOGITECH_HD1080:
		*penSize = PIC_1080P;
		break;

	case X86_LOGITECH_HD720:
		*penSize = PIC_720P;
		break;

	case HAPS_ARMISP:
		*penSize = PIC_720P;
		break;

	case EVB_SENSOR:
	case EVB_SENSOR_2MP:
		*penSize = PIC_1080P;
		break;

	case EVB_SENSOR_4K:
		*penSize = PIC_3840x2160;
		break;

	case EVB_SENSOR_4MP:
		*penSize = PIC_2560x1440;
		break;

	case EVB_SENSOR_5MP:
		*penSize = PIC_2688x1860;
		break;
	case EVB_SENSOR_1520P:
		*penSize = PIC_2688x1520;
		break;
	case EVB_SENSOR_5MP_2592x1944:
		*penSize = PIC_2592x1944;
		break;

	case EVB_SENSOR_5MP_2880x1624:
		*penSize = PIC_2880x1624;
		break;

	case EVB_SENSOR_2304x1296:
		*penSize = PIC_2304x1296;
		break;

	default:
		*penSize = PIC_1080P;
		break;
	}

	return s32Ret;
}

TS_S32 SAMPLE_VI_VPSS_MULTICPM_VENC(SAMPLE_SNS_TYPE_E eSensor0, SAMPLE_ALG_TYPE_E* peAlgType, TS_U32 u32AlgNum, SAMPLE_VIDEO_CHN_MODE eChnMode)
{
	TS_S32 s32Ret = TS_SUCCESS;
	TS_S32 i;

	// TS_S32             s32ViCnt       = 1;
	// VI_DEV             ViDev          = 0;
	// VI_PIPE            ViPipe         = 0;
	TS_S32 s32WorkSnsId = 0;
	SAMPLE_VI_CONFIG_S stViConfig = {0};

	SIZE_S stSize;
	VB_CONFIG_S stVbConf;
	PIC_SIZE_E enPicSize;
	TS_U32 u32BlkSize;
	TS_BOOL bLowDelay = TS_FALSE;//TS_TRUE;

	//    WDR_MODE_E         enWDRMode      = WDR_MODE_NONE;
	DYNAMIC_RANGE_E enDynamicRange = DYNAMIC_RANGE_SDR8;
	// PIXEL_FORMAT_E     enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
	VIDEO_FORMAT_E enVideoFormat = VIDEO_FORMAT_LINEAR;
	COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

	VPSS_GRP VpssGrp = 0;
	VPSS_GRP_ATTR_S stVpssGrpAttr;
	// VPSS_CHN           VpssChn        = VPSS_CHN0;
	// TS_BOOL            abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
	VPSS_CHN_ATTR_S astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {0};

	CPM_GRP CPMGrp = 0;
	TS_S32 u32CpmPipeNUm = 2; // 1, 2;
	TS_S32 s32ChnNum = 1;//1;	  // 1;//1; hxl
							  // TS_S32          s32VpssOutNum  = 2;
							  // CPM_GRP_ATTR_S stCpmGrpAttr = {0};
							  // CPM_Handle_S stCPMHandle = {0};

	// VENC_CHN VencChn[2] = {0, 1}; // hxl
	VENC_CHN VencChn[4] = {0, 1, 2, 3};
	TS_U32 u32Profile[2] = {0, 2};
	PAYLOAD_TYPE_E enPayLoad[2] = {PT_H265, PT_H264};
	VENC_GOP_MODE_E enGopMode;
	VENC_GOP_ATTR_S stGopAttr;
	SAMPLE_RC_E enRcMode;
	VI_PIPE ViPipe [2] = {0, 1};
	VPSS_CHN VpssChn = 0;

	VPSS_VENC_DRAW_BOX_S stDrawBox[2][2];
	TS_BOOL bUseVPSS_VENC_DrawBox[2][2] = {{TS_FALSE, TS_FALSE}, {TS_TRUE, TS_FALSE}};

	/*get picture size*/
	s32Ret = SAMPLE_ALG_GetSizeBySensor(eSensor0, &enPicSize);
	if (TS_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("get picture size by sensor failed!\n");
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
	if (TS_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("get picture size failed!\n");
		return s32Ret;
	}

    SAMPLE_PRT("#######stSize.u32Width=%d, stSize.u32Height=%d\n",stSize.u32Width, stSize.u32Height);
	memset(&stViConfig, 0, sizeof(SAMPLE_VI_CONFIG_S));
	
	SAMPLE_PIPE_INFO_S *pstPipeInfo = &stViConfig.astViInfo[s32WorkSnsId].stPipeInfo;
	SAMPLE_CHN_INFO_S *pstChnInfo = &stViConfig.astViInfo[s32WorkSnsId].stChnInfo;

	pstPipeInfo->aPipe = ViPipe[0];
	pstPipeInfo->width = stSize.u32Width;
	pstPipeInfo->height = stSize.u32Height;
#if 1
	pstPipeInfo->enBitWid = DATA_BITWIDTH_8;
	pstPipeInfo->enBayer = BAYER_GRBG;
	pstPipeInfo->frameRate = 20;
	pstPipeInfo->enWdrMode = TS_FALSE;
	pstPipeInfo->bIspByFly = TS_FALSE;
#else
	MODE_SETTING_S sns_mode_set;
	if (TS_SUCCESS != SAMPLE_COMM_SYS_SelectMode(&sns_mode_set))
	{
		SAMPLE_PRT("sample select mode faild\n");
		return TS_FAILURE;
	}

	SAMPLE_PRT("sns_set, [cam_num, bitwid, layer, fps, wdr, isp_fly] = [%d, %d, %d, %d, %d, %d]\n",
			   sns_mode_set.numCam, sns_mode_set.bit_width, sns_mode_set.enBayer,
			   sns_mode_set.maxFps, sns_mode_set.wdr_mode, sns_mode_set.isOnLine);

	pstPipeInfo->enBitWid = sns_mode_set.bit_width;
	pstPipeInfo->enBayer = sns_mode_set.enBayer;
	pstPipeInfo->frameRate = sns_mode_set.maxFps;
	pstPipeInfo->enWdrMode = sns_mode_set.wdr_mode;
	pstPipeInfo->bIspByFly = sns_mode_set.isOnLine;
#endif

	if (TS_TRUE == pstPipeInfo->bIspByFly)
	{
		pstPipeInfo->bDynFpsSync = TS_TRUE;
	}
	else
	{
		pstPipeInfo->bDynFpsSync = TS_FALSE;
	}

		if (TS_TRUE == bLowDelay)
		{
			pstPipeInfo->enMastPipeMode = VI_ONLINE_VPSS_OFFLINE;//VI_ONLINE_VPSS_ONLINE;
			pstPipeInfo->enPixFmt = PIXEL_FORMAT_RGB_888;
		}
		else
		{
			pstPipeInfo->enMastPipeMode = VI_ONLINE_VPSS_OFFLINE;
			pstPipeInfo->enPixFmt = PIXEL_FORMAT_NV_12;
		}

	if (0 == access("/root/enable_wdr.flag", 0))
	{
		SAMPLE_PRT("!!! get wdr flag !\n");
		pstPipeInfo->enWdrMode = WDR_MODE_BUILT_IN;
	}
	else
	{
		pstPipeInfo->enWdrMode = WDR_MODE_NONE;
	}

	pstChnInfo->ViChn[0] = 0;
	pstChnInfo->validChnlNum = s32ChnNum;
	pstChnInfo->enPixFormat = PIXEL_FORMAT_NV_12;
	pstChnInfo->width[0] = stSize.u32Width;
	pstChnInfo->height[0] = stSize.u32Height;

	// pstChnInfo->ViChn[1]        = 1;
	// pstChnInfo->validChnlNum = s32ChnNum;
	// pstChnInfo->enPixFormat     = PIXEL_FORMAT_NV_12;
	// pstChnInfo->width[1] = 640;
	// pstChnInfo->height[1] = 360;

	/*config vb*/
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt = 1;

	u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YVU_SEMIPLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_SEG, DEFAULT_ALIGN);
	stVbConf.astCommPool[0].u64BlkSize = 1000;//u32BlkSize;
	stVbConf.astCommPool[0].u32BlkCnt = 1;

	u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	stVbConf.astCommPool[1].u64BlkSize = 1*1024*1024;//u32BlkSize;
	stVbConf.astCommPool[1].u32BlkCnt = 1;

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (TS_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("system init failed with %d!\n", s32Ret);
		return s32Ret;
	}

	SAMPLE_PRT("####### SAMPLE_COMM_SYS_Init success!\n");

	// system("echo VI=7 > /proc/mpp/logmpp");
	// system("echo VPSS=7 > /proc/mpp/logmpp");
	// system("echo VB=7 > /proc/mpp/logmpp");
	// system("echo SYS=7 > /proc/mpp/logmpp");
	// system("echo CPM=7 > /proc/mpp/logmpp");
	stViConfig.s32WorkingViNum = 2;//1; // s32ViCnt;

	for (i = 0; i < 2; i ++) {
        pstPipeInfo = &stViConfig.astViInfo[i].stPipeInfo;
		pstChnInfo = &stViConfig.astViInfo[i].stChnInfo;

        pstPipeInfo->aPipe		= i;
        pstPipeInfo->enBitWid	= DATA_BITWIDTH_8;
        pstPipeInfo->enBayer    = BAYER_GRBG;
        pstPipeInfo->width      = 1920;
        pstPipeInfo->height     = 1080;
        pstPipeInfo->frameRate  = 20;
        pstPipeInfo->enWdrMode	= WDR_MODE_NONE;
        pstPipeInfo->bIspByFly  = TS_FALSE;
        pstPipeInfo->bDynFpsSync = TS_FALSE;
        pstPipeInfo->enMastPipeMode = VI_ONLINE_VPSS_ONLINE;
		pstPipeInfo->enPixFmt	= PIXEL_FORMAT_RGB_888;

        pstChnInfo->ViChn[0]	= 0;
		pstChnInfo->width[0]	= 1920;
		pstChnInfo->height[0]	= 1080;
		pstChnInfo->validChnlNum= 1;
		pstChnInfo->enPixFormat = PIXEL_FORMAT_NV_12;
    }

	s32Ret = SAMPLE_COMM_VI_SetParam(&stViConfig);
	if (TS_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("SAMPLE_COMM_VI_SetParam failed with %d!\n", s32Ret);
		return s32Ret;
	}

	/*start vi*/
	s32Ret = SAMPLE_COMM_VI_StartVi(&stViConfig);
	if (TS_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("SAMPLE_COMM_VI_StartVi failed with %d!\n", s32Ret);
		goto EXIT;
	}

	SAMPLE_PRT("####### SAMPLE_COMM_VI_StartVi success!\n");

	 

	 
#if SDK_VERSON_030
#else
	for (TS_S32 i = 0; i < 1; i++)
	{
		s32Ret = TS_MPI_ISP_SetLtm(ViPipe[i], 0); // enable LTM
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("TS_MPI_ISP_SetLtm failed for ViPipe[%d].s32Ret:0x%x !\n", i, s32Ret);
			goto EXIT1;
		}
		else
		{
			SAMPLE_PRT("enable LTM OK for ViPipe[%d].\n", i);
		}
	}

	SAMPLE_PRT("####### TS_MPI_ISP_SetLtm success!\n");
#endif
	/*config vpss*/
	memset(&stVpssGrpAttr, 0, sizeof(VPSS_GRP_ATTR_S));
	stVpssGrpAttr.enGrpMode = VPSS_GRP_MODE_STREAM;
	stVpssGrpAttr.stFrameRate.s32SrcFrameRate = -1;
	stVpssGrpAttr.stFrameRate.s32DstFrameRate = -1;
	stVpssGrpAttr.enDynamicRange = DYNAMIC_RANGE_SDR8;
	stVpssGrpAttr.enPixelFormat = PIXEL_FORMAT_NV_12; // enPixFormat;
	stVpssGrpAttr.u32MaxW = stSize.u32Width;
	stVpssGrpAttr.u32MaxH = stSize.u32Height;

	VpssChn = 0;
	 if (VpssChn == 0) {
	 stSize.u32Width = 1920;
	 stSize.u32Height = 1080;
	 
	}
 
	astVpssChnAttr[VpssChn].u32MaxW = stSize.u32Width;
	astVpssChnAttr[VpssChn].u32MaxH = stSize.u32Height;
	astVpssChnAttr[VpssChn].u32Width = stSize.u32Width;
	astVpssChnAttr[VpssChn].u32Height = stSize.u32Height;
	astVpssChnAttr[VpssChn].enChnMode = VPSS_CHN_MODE_USER;
	astVpssChnAttr[VpssChn].enCompressMode = enCompressMode;
	astVpssChnAttr[VpssChn].enDynamicRange = enDynamicRange;
	astVpssChnAttr[VpssChn].enVideoFormat = enVideoFormat;
	astVpssChnAttr[VpssChn].enPixelFormat = PIXEL_FORMAT_NV_12; // PIXEL_FORMAT_NV_12;//enPixFormat;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = -1;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = -1;
	astVpssChnAttr[VpssChn].u32Depth = 5;
	astVpssChnAttr[VpssChn].bMirror = TS_FALSE;
	astVpssChnAttr[VpssChn].bFlip = TS_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.enMode = ASPECT_RATIO_NONE;
#if SDK_VERSON_030
	astVpssChnAttr[VpssChn].bSmallMem = TS_TRUE;
#else
	astVpssChnAttr[VpssChn].u32MemCount = 2;
	astVpssChnAttr[VpssChn].bVpuLoop = TS_FALSE;
#endif

	/*start vpss*/
	// abChnEnable[0] = TS_TRUE;

	SAMPLE_ALG_CHN_IMG_SIZE_S stAlgChnSize;
	SAMPLE_CPM_GetChnMode(eChnMode, &stAlgChnSize);

	VpssChn = 1;

	 
	astVpssChnAttr[VpssChn].u32MaxW = stAlgChnSize.width;
	astVpssChnAttr[VpssChn].u32MaxH = stAlgChnSize.hight;
	astVpssChnAttr[VpssChn].u32Width = stAlgChnSize.width;
	astVpssChnAttr[VpssChn].u32Height = stAlgChnSize.hight;
	astVpssChnAttr[VpssChn].enChnMode = VPSS_CHN_MODE_USER;
	astVpssChnAttr[VpssChn].enCompressMode = enCompressMode;
	astVpssChnAttr[VpssChn].enDynamicRange = enDynamicRange;
	astVpssChnAttr[VpssChn].enVideoFormat = enVideoFormat;
	astVpssChnAttr[VpssChn].enPixelFormat = PIXEL_FORMAT_NV_12; // PIXEL_FORMAT_NV_12;//enPixFormat;
	astVpssChnAttr[VpssChn].stFrameRate.s32SrcFrameRate = -1;
	astVpssChnAttr[VpssChn].stFrameRate.s32DstFrameRate = -1;
	astVpssChnAttr[VpssChn].u32Depth = 5;
	astVpssChnAttr[VpssChn].bMirror = TS_FALSE;
	astVpssChnAttr[VpssChn].bFlip = TS_FALSE;
	astVpssChnAttr[VpssChn].stAspectRatio.enMode = ASPECT_RATIO_NONE;

#if SDK_VERSON_030
	astVpssChnAttr[VpssChn].bSmallMem = TS_TRUE;
#else
	astVpssChnAttr[VpssChn].u32MemCount = 3;
	astVpssChnAttr[VpssChn].bVpuLoop = TS_FALSE;
#endif

    /*start vpss*/
    //abChnEnable[1] = TS_TRUE;
    s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, &stVpssGrpAttr, astVpssChnAttr, VpssChn+1);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT1;
    }

 

	if (grp_num == 2) {
		s32Ret = SAMPLE_COMM_VPSS_Start(1, &stVpssGrpAttr, astVpssChnAttr, 2);
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
			goto EXIT1;
		}
	}

#if SDK_VERSON_030
#else
	if (TS_SUCCESS != TS_MPI_VPSS_Start_Camera())
	{
		SAMPLE_PRT("TS_MPI_VPSS_Start_Camera fail !\n");
	}
#endif

	/*bind*/
	if (TS_FALSE == bLowDelay)
	{
	}
#if SDK_VERSON_030
#else
	else
	{
		TS_U64 mode = SYS_SPECIAL_MODE_NONE;

		s32Ret = TS_MPI_SYS_GetSpecialMode(&mode);
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("MPP_SYS_SPECIAL_MODE_E faild\n");
		}

		SAMPLE_PRT("do NOT need VI Bind VPSS \n");

		if (SYS_SPECIAL_MODE_FASTBOOT == mode)
		{
			/* 快启模式下，在这里加载venc和codec的ko        			*/
			SAMPLE_COMM_ISP_GetVDTimeout(ViPipe, ISP_VD_FIRST_FRAME, 0);
			SAMPLE_PRT("get first frame success !!\n");
			fastboot_insmod();
		}
	}
#endif

	SAMPLE_PRT("###################SAMPLE_COMM_VI_Bind_VPSS  success!\n");
	/*start cpm*/
	s32Ret = SAMPLE_ALG_CPM_Config(CPMGrp, u32CpmPipeNUm, stSize.u32Width, stSize.u32Height, peAlgType, u32AlgNum, &stAlgChnSize);
	if (TS_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("SAMPLE_ALGO_CPM_Config failed for %#x!\n", s32Ret);
		goto EXIT2;
	}
	SAMPLE_PRT("###################SAMPLE_ALGO_CPM_Config[0]  success!\n");
	
	if (!bUseVPSS_VENC_DrawBox[0][0])
	{
		SAMPLE_PRT("###################SAMPLE_COMM_VPSS_Bind_CPM 0000 success!\n");
		s32Ret = SAMPLE_COMM_VPSS_Bind_CPM(0, 0, 0, 0);
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VPSS_Bind_CPM failed for %#x!\n", s32Ret);
			goto EXIT_VENC_H265_STOP;
		}
		SAMPLE_PRT("vpss bind cpm success, [%d,%d ---> %d,%d]\n",
				   0, 0, 0, 0);
	}
	else
	{
		SAMPLE_PRT("=============== Skip VPSS-CPM binding for camera 0 main stream (using VPSS_VENC_DrawBox mode) ================\n");
	}
	if (!bUseVPSS_VENC_DrawBox[0][1])
	{
		s32Ret = SAMPLE_COMM_VPSS_Bind_CPM(0, 1, 0, 1);
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VPSS_Bind_CPM failed for %#x!\n", s32Ret);
			goto EXIT_VENC_H265_STOP;
		}
		SAMPLE_PRT("vpss bind cpm success, [%d,%d ---> %d,%d]\n",
				   0, 1, 0, 1);
	}
	
	if (!bUseVPSS_VENC_DrawBox[1][0])
	{
		SAMPLE_PRT("###################SAMPLE_COMM_VPSS_Bind_CPM1001 success!\n");
		s32Ret = SAMPLE_COMM_VPSS_Bind_CPM(1, 0, 0, 0);
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VPSS_Bind_CPM failed for %#x!\n", s32Ret);
			//goto EXIT_VENC_H265_STOP;
		}
		SAMPLE_PRT("vpss bind cpm success, [%d,%d ---> %d,%d]\n",
				   1, 0, 0, 0);
		
	}
	else
	{
		SAMPLE_PRT("=============== Skip VPSS-CPM binding for camera 1 (using VPSS_VENC_DrawBox mode) ================\n");
	}
	
	if (!bUseVPSS_VENC_DrawBox[1][1])
	{
		SAMPLE_PRT("###################SAMPLE_COMM_VPSS_Bind_CPM1111 success!\n");

		s32Ret = SAMPLE_COMM_VPSS_Bind_CPM(1, 1, 0, 1);
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VPSS_Bind_CPM failed for %#x!\n", s32Ret);
			//goto EXIT_VENC_H265_STOP;
		}
		SAMPLE_PRT("vpss bind cpm success, [%d,%d ---> %d,%d]\n",
				   1, 1, 0, 1);
		SAMPLE_PRT("###################SAMPLE_COMM_VPSS_Bind_CPM1110 success!\n");
	}
	else
	{
		SAMPLE_PRT("=============== Skip VPSS-CPM binding for camera 1 (using VPSS_VENC_DrawBox mode) ================\n");
	}
	enRcMode = SAMPLE_RC_CBR;
	enGopMode = VENC_GOPMODE_NORMALP;

	s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode, &stGopAttr);
	if (TS_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("Venc Get GopAttr for %#x!\n", s32Ret);
	}

	SAMPLE_PRT("###################SAMPLE_COMM_VENC_GetGopAttr  success!\n");

	/***encode h.265 **/
	s32Ret = SAMPLE_COMM_VENC_Start(VencChn[0], enPayLoad[0], enPicSize, 20, enRcMode, u32Profile[0], &stGopAttr); // new
	// s32Ret = SAMPLE_COMM_VENC_Start(VencChn[0], enPayLoad[0],enPicSize, enRcMode,u32Profile[0],bRcnRefShareBuf,&stGopAttr); //old
	if (TS_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
	}

	/***encode h.265 **/
	s32Ret = SAMPLE_COMM_VENC_Start(VencChn[1], enPayLoad[0], enPicSize, 20, enRcMode, u32Profile[0], &stGopAttr); // new
	// s32Ret = SAMPLE_COMM_VENC_Start(VencChn[0], enPayLoad[0],enPicSize, enRcMode,u32Profile[0],bRcnRefShareBuf,&stGopAttr); //old
	if (TS_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
	}
	if (2 == grp_num)
	{
		/***encode h.265 **/
		s32Ret = SAMPLE_COMM_VENC_Start(VencChn[2], enPayLoad[0], enPicSize, 20, enRcMode, u32Profile[0], &stGopAttr); // new
		// s32Ret = SAMPLE_COMM_VENC_Start(VencChn[0], enPayLoad[0],enPicSize, enRcMode,u32Profile[0],bRcnRefShareBuf,&stGopAttr); //old
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
		}

		/***encode h.265 **/
		s32Ret = SAMPLE_COMM_VENC_Start(VencChn[3], enPayLoad[0], enPicSize, 20, enRcMode, u32Profile[0], &stGopAttr); // new
		// s32Ret = SAMPLE_COMM_VENC_Start(VencChn[0], enPayLoad[0],enPicSize, enRcMode,u32Profile[0],bRcnRefShareBuf,&stGopAttr); //old
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("Venc Start failed for %#x!\n", s32Ret);
		}
	}
	s32Ret = SAMPLE_COMM_CPM_Bind_VENC(CPMGrp, 0, VencChn[0]);
	SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VPSS 0 %#x!\n", s32Ret);
	if (TS_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("SAMPLE_COMM_VI_Bind_VPSS failed for %#x!\n", s32Ret);
		goto EXIT_VENC_H265_UnBind;
	}
	else
	{
		SAMPLE_PRT("=============== Skip CPM-VENC binding for camera 0 main stream (using VPSS_VENC_DrawBox mode) ================\n");
	}

	if (!bUseVPSS_VENC_DrawBox[0][1])
	{
		s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(CPMGrp, 1, VencChn[1]);
		SAMPLE_PRT("SAMPLE_COMM_VPSS_Bind_VENC 1 %d\n", s32Ret);
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_CPM_Bind_VENC failed for %#x!\n", s32Ret);
			goto EXIT_VENC_H265_UnBind;
		}
	}
	else
	{
		SAMPLE_PRT("=============== Skip CPM-VENC binding for camera 0 sub stream (using VPSS_VENC_DrawBox mode) ================\n");
	}

	if (2 == grp_num)
	{
		if (!bUseVPSS_VENC_DrawBox[1][0])
		{	
			printf("===============111==SAMPLE_COMM_VPSS_Bind_VENC========================\n");
			s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(1, 0, VencChn[2]);
			SAMPLE_PRT("SAMPLE_COMM_VPSS_Bind_VENC 3 %d\n", s32Ret);
			if (TS_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("SAMPLE_COMM_VPSS_Bind_VENC failed for %#x!\n", s32Ret);
				goto EXIT_VENC_H265_UnBind;
			}
		}
		else
		{
			SAMPLE_PRT("=============== Skip VPSS-VENC binding for camera 1 (using VPSS_VENC_DrawBox mode) ================\n");
		}

		if (!bUseVPSS_VENC_DrawBox[1][1])
		{
			 printf("===============111==SAMPLE_COMM_VPSS_Bind_VENC========================\n");
			 s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(1, 1, VencChn[3]);
			 SAMPLE_PRT("SAMPLE_COMM_VPSS_Bind_VENC 3 %d\n", s32Ret);
			 if (TS_SUCCESS != s32Ret)
			 {
			 	SAMPLE_PRT("SAMPLE_COMM_VPSS_Bind_VENC failed for %#x!\n", s32Ret);
			 	goto EXIT_VENC_H265_UnBind;
			 }
		}
		else
		{
			SAMPLE_PRT("=============== Skip VPSS-VENC binding for camera 1 (using VPSS_VENC_DrawBox mode) ================\n");
		}

	}

	

	if (bUseVPSS_VENC_DrawBox[0][1])
	{
		SAMPLE_PRT("=============== Using VPSS_VENC_DrawBox mode for camera 0 sub stream ================\n");
		
		SAMPLE_CPM_GetChnMode(eChnMode, &stAlgChnSize);
		
		SAMPLE_ALG_INSTANCE_S *pstAlgInstList[SAMPLE_ALG_MAX_NUM];
		u32AlgNum = 0;
		 
		for (TS_S32 i = 0; i < SAMPLE_ALG_MAX_NUM; i++)
		{
			if (gstSampleVideoAlgCpm[0].pstAlgInstList[i] != NULL)
			{
				pstAlgInstList[u32AlgNum++] = gstSampleVideoAlgCpm[0].pstAlgInstList[i];
			}
		}

		s32Ret = SAMPLE_VPSS_VENC_DrawBox_Start(&stDrawBox[0][1], 0, 1, VencChn[1], 
												  640, 360, 
												  pstAlgInstList, u32AlgNum, CPMGrp);
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_VPSS_VENC_DrawBox_Start[0][1] failed: %#x\n", s32Ret);
		}
	}

	if (2 == grp_num && bUseVPSS_VENC_DrawBox[1][0])
	{
		SAMPLE_PRT("=============== Using VPSS_VENC_DrawBox mode for camera 1 main stream ================\n");
		
		SAMPLE_CPM_GetChnMode(eChnMode, &stAlgChnSize);
		
		SAMPLE_ALG_INSTANCE_S *pstAlgInstList[SAMPLE_ALG_MAX_NUM];
		u32AlgNum = 0;
		 
		for (TS_S32 i = 0; i < SAMPLE_ALG_MAX_NUM; i++)
		{
			if (gstSampleVideoAlgCpm[1].pstAlgInstList[i] != NULL)
			{
				pstAlgInstList[u32AlgNum++] = gstSampleVideoAlgCpm[1].pstAlgInstList[i];
			}
		}

		s32Ret = SAMPLE_VPSS_VENC_DrawBox_Start(&stDrawBox[1][0], 1, 0, VencChn[2], 
												  1920, 1080, 
												  pstAlgInstList, u32AlgNum, CPMGrp+1);
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_VPSS_VENC_DrawBox_Start[1][0] failed: %#x\n", s32Ret);
		}
	}
	
	if (2 == grp_num && bUseVPSS_VENC_DrawBox[1][1])
	{
		SAMPLE_PRT("=============== Using VPSS_VENC_DrawBox mode for camera 1 sub stream ================\n");
		
		SAMPLE_CPM_GetChnMode(eChnMode, &stAlgChnSize);
		
		SAMPLE_ALG_INSTANCE_S *pstAlgInstList[SAMPLE_ALG_MAX_NUM];
		u32AlgNum = 0;
		 
		for (TS_S32 i = 0; i < SAMPLE_ALG_MAX_NUM; i++)
		{
			if (gstSampleVideoAlgCpm[1].pstAlgInstList[i] != NULL)
			{
				pstAlgInstList[u32AlgNum++] = gstSampleVideoAlgCpm[1].pstAlgInstList[i];
			}
		}

		s32Ret = SAMPLE_VPSS_VENC_DrawBox_Start(&stDrawBox[1][1], 1, 1, VencChn[3], 
												  640, 360, 
												  pstAlgInstList, u32AlgNum, CPMGrp+1);
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_VPSS_VENC_DrawBox_Start[1][1] failed: %#x\n", s32Ret);
		}
	}
	s32Ret = SAMPLE_COMM_VENC_StartGetStreamV2(VencChn, 4, SAMPLE_VENC_RTSP);
	 //s32Ret = SAMPLE_COMM_VENC_StartGetStreamV2(VencChn,3,SAMPLE_VENC_RTSP);
	if (TS_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("Start Venc failed!\n");
		goto EXIT4;
	}

	// s32Ret = SAMPLE_COMM_VENC_UpdateFps(VencChn, 1);
	// 	if (TS_SUCCESS != s32Ret)
	// 		SAMPLE_PRT("Update fps failed!\n");

	simple_algo_interact_loop();
	// SAMPLE_COMM_VENC_StopUpdateFps();
	SAMPLE_COMM_VENC_StopGetStream();

	if (bUseVPSS_VENC_DrawBox[0][1])
	{
		SAMPLE_PRT("=============== Stopping VPSS_VENC_DrawBox for camera 0 sub stream ================\n");
		SAMPLE_VPSS_VENC_DrawBox_Stop(&stDrawBox[0][1]);
	}
	if (2 == grp_num && bUseVPSS_VENC_DrawBox[1][0])
	{
		SAMPLE_PRT("=============== Stopping VPSS_VENC_DrawBox for camera 1 main stream ================\n");
		SAMPLE_VPSS_VENC_DrawBox_Stop(&stDrawBox[1][0]);
	}
	if (2 == grp_num && bUseVPSS_VENC_DrawBox[1][1])
	{
		SAMPLE_PRT("=============== Stopping VPSS_VENC_DrawBox for camera 1 sub stream ================\n");
		SAMPLE_VPSS_VENC_DrawBox_Stop(&stDrawBox[1][1]);
	}

EXIT4:
	if (!bUseVPSS_VENC_DrawBox[0][0])
	{
		SAMPLE_COMM_CPM_UnBind_VENC(CPMGrp, 0, VencChn[0]);
	}
	if (!bUseVPSS_VENC_DrawBox[0][1])
	{
		SAMPLE_COMM_CPM_UnBind_VENC(CPMGrp, 1, VencChn[1]);
	}
EXIT_VENC_H265_UnBind:
	if (TS_FALSE == bLowDelay)
	{
		SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, 0, VpssGrp);
	}
EXIT_VENC_H265_STOP:
	SAMPLE_COMM_VENC_Stop(VencChn[0]);
EXIT2:
	if (!bUseVPSS_VENC_DrawBox[0][0])
	{
		s32Ret = SAMPLE_COMM_VPSS_UnBind_CPM(0, 0, 0, 0);
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VPSS_UnBind_CPM failed for %#x!\n", s32Ret);
		}
		else
		{
			SAMPLE_PRT("vpss unBind CPM success! [%d,%d ---> %d,%d]\n",
					   0, 0, 0, 0);
		}
	}
	if (!bUseVPSS_VENC_DrawBox[0][1])
	{
		s32Ret = SAMPLE_COMM_VPSS_UnBind_CPM(0, 1, 0, 1);
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("SAMPLE_COMM_VPSS_UnBind_CPM failed for %#x!\n", s32Ret);
		}
		else
		{
			SAMPLE_PRT("vpss unBind CPM success! [%d,%d ---> %d,%d]\n",
					   0, 1, 0, 1);
		}
	}
	if (!bUseVPSS_VENC_DrawBox[0][0] || !bUseVPSS_VENC_DrawBox[0][1])
	{
		SAMPLE_PRT("=============== VPSS-CPM unbinding completed ================\n");
	}
	else
	{
		SAMPLE_PRT("=============== Skip VPSS-CPM unbinding (using VPSS_VENC_DrawBox mode) ================\n");
	}
	SAMPLE_COMM_CPM_Stop(CPMGrp);
	SAMPLE_COMM_VI_StopVi(&stViConfig);

EXIT1:
	// SAMPLE_COMM_VPSS_Stop(VpssGrp, s32VpssChnNum);
	SAMPLE_COMM_VPSS_Stop(VpssGrp, 2);
EXIT:
	SAMPLE_COMM_SYS_Exit();
	return s32Ret;
}

/******************************************************************************
 * function : to process abnormal case
 ******************************************************************************/
void SAMPLE_ALGO_HandleSig(TS_S32 signo)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
#if 1
	g_sample_algo_terminal_all = 1;
#else
	if (SIGINT == signo || SIGTERM == signo)
	{
		SAMPLE_COMM_All_ISP_Stop();
		// SAMPLE_COMM_VO_HdmiStop();
		SAMPLE_COMM_SYS_Exit();
		printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
	}
	exit(-1);
#endif
}

static TS_VOID SAMPLE_TSALG_VIDEO_Usage(TS_CHAR *sPrgNm)
{
	TS_U32 u32AlgNum = 0;
	SAMPLE_ALG_INIT_CFG_S *pAlgInitTbl = SAMPLE_ALG_GetInitCfg(&u32AlgNum);
	printf("Usage : %s <index> <model_file_directory>\n", sPrgNm);
	printf("index:\n");

	for (size_t i = 0; i < u32AlgNum; ++i)
	{
		printf("\t %d)%s\n", pAlgInitTbl[i].eMask, pAlgInitTbl[i].u8AlgName);
	}
}

static TS_S32 SAMPLE_TSALG_VIDEO_Match(TS_S32 s32Index, SAMPLE_ALG_TYPE_E *penMask, SAMPLE_VIDEO_CHN_MODE *penChnMode)
{
	TS_S32 s32Ret = TS_FAILURE;
	TS_U32 u32AlgNum = 0;
	SAMPLE_ALG_INIT_CFG_S *pAlgInitTbl = SAMPLE_ALG_GetInitCfg(&u32AlgNum);
	for (size_t i = 0; i < u32AlgNum; ++i)
	{
		if (s32Index == pAlgInitTbl[i].eMask)
		{
			*penMask = pAlgInitTbl[i].eMask;
			*penChnMode = pAlgInitTbl[i].eChnMode;
			s32Ret = TS_SUCCESS;
			break;
		}
	}
	return s32Ret;
}

static TS_VOID *VPSS_VENC_DrawBox_ResultSync_Thread(TS_VOID *pParam)
{
	VPSS_VENC_DRAW_BOX_S *pDrawBox = (VPSS_VENC_DRAW_BOX_S *)pParam;
	SAMPLE_VIDEO_ALG_CPM *pCpmParam = &gstSampleVideoAlgCpm[0];

	prctl(PR_SET_NAME, (unsigned long)"vpss_venc_sync", 0, 0, 0);
	SAMPLE_PRT("VPSS_VENC_DrawBox_ResultSync_Thread started for CPM Grp %d\n", pDrawBox->s32CPMGrp);

	while (pDrawBox->bRunFlag)
	{
		pthread_mutex_lock(&pCpmParam->stAlgProcLock);
		if (pCpmParam->bResultUpdate)
		{
			pthread_mutex_lock(&pDrawBox->stResultLock);
			memcpy(&pDrawBox->stAlgResult, &pCpmParam->stAlgResult, sizeof(SAMPLE_ALG_RESULT_S));
			pDrawBox->bResultUpdate = TS_TRUE;
			pthread_mutex_unlock(&pDrawBox->stResultLock);
			pCpmParam->bResultUpdate = TS_FALSE; // 重置更新标志
		}
		pthread_mutex_unlock(&pCpmParam->stAlgProcLock);

		usleep(25*1000);
	}

	SAMPLE_PRT("VPSS_VENC_DrawBox_ResultSync_Thread exited for CPM Grp %d\n", pDrawBox->s32CPMGrp);
	return TS_NULL;
}

static TS_VOID *VPSS_VENC_DrawBox_Thread(TS_VOID *pParam)
{
	VPSS_VENC_DRAW_BOX_S *pDrawBox = (VPSS_VENC_DRAW_BOX_S *)pParam;
	TS_S32 s32Ret;
	VIDEO_FRAME_INFO_S stFrameInfo;
	TS_U32 u32FrameCount = 0;

	prctl(PR_SET_NAME, (unsigned long)"vpss_venc_draw", 0, 0, 0);
	SAMPLE_PRT("VPSS_VENC_DrawBox_Thread started: VPSS[%d:%d] -> VENC[%d]\n", 
			   pDrawBox->VpssGrp, pDrawBox->VpssChn, pDrawBox->VencChn);

	while (pDrawBox->bRunFlag)
	{
		s32Ret = TS_MPI_VPSS_GetChnFrame(pDrawBox->VpssGrp, pDrawBox->VpssChn, &stFrameInfo, 1000);
		if (TS_SUCCESS != s32Ret)
		{
			usleep(10000);
			continue;
		}

		TS_U8 *pYuvBuf = (TS_U8 *)(uintptr_t)stFrameInfo.stVFrame.u64VirAddr[0];
		// SAMPLE_PRT("VPSS_VENC_DrawBox_Thread started: pDrawBox->u32Width:%d pDrawBox->u32Height:%d \n", 
		// 	  pDrawBox->u32Width, pDrawBox->u32Height);
		// RECT testRect;
		// testRect.left = pDrawBox->u32Width / 4;
		// testRect.top = pDrawBox->u32Height / 4;
		// testRect.right = pDrawBox->u32Width * 3 / 4;
		// testRect.bottom = pDrawBox->u32Height * 3 / 4;
		// YUV_Draw_Rect(pYuvBuf, pDrawBox->u32Width, pDrawBox->u32Height, 12, &testRect, YUV_RED, 5);
		
		//SAMPLE_ALG_RESULT_S *pCurResult = (SAMPLE_ALG_RESULT_S *)pAlgResult;
		ALG_CatDetect_DET_RESULT_S *pResult = &(pDrawBox->stAlgResult);
		RECT rect;
		for (TS_U32 i = 0; i < pResult->u32ObjNum; i++)
		{
		if (pResult->stBox[i].act != ALG_CAT_ACT_INT && pResult->stBox[i].act != ALG_CAT_ACT_EAT)
		{ // hxl 取消侧脸限制 2025.12.26
			continue;
		}
		printf("ssss pResult f32Xmin %f,f32Ymin %f,f32Xmax %f,f32Ymax %f\n",pResult->stBox[i].f32Xmin,
            pResult->stBox[i].f32Ymin,pResult->stBox[i].f32Xmax,pResult->stBox[i].f32Ymax);
		
		rect.left = 1 * (pResult->stBox[i].f32Xmin) * pDrawBox->u32Width;
		rect.top = 1 * (pResult->stBox[i].f32Ymin )* pDrawBox->u32Height;
		rect.right = 1 * (pResult->stBox[i].f32Xmax) * pDrawBox->u32Width ;
		rect.bottom = 1 * (pResult->stBox[i].f32Ymax) * pDrawBox->u32Height;//1.75
		//YUV_Draw_Rect(pYuvBuf, pDrawBox->u32Width, pDrawBox->u32Height, 12, &rect, YUV_GREEN, 5);
		if( 1 == pResult->stBox[i].cam_id&&pResult->stBox[i].class_id == ALG_CAT_CLASS_ID_FOOD)
		{
			//YUV_Draw_Rect(pYuvBuf, width, height, 12, &rect, YUV_YELLOW, 5);
			YUV_Draw_Rect(pYuvBuf, pDrawBox->u32Width, pDrawBox->u32Height, 12, &rect, YUV_YELLOW, 5);
		}
		else if( 1 == pResult->stBox[i].cam_id && pResult->stBox[i].class_id == ALG_CAT_CLASS_ID_FACE)
		{
			//YUV_Draw_Rect(pYuvBuf, width, height, 12, &rect, YUV_GREEN, 5);
			YUV_Draw_Rect(pYuvBuf, pDrawBox->u32Width, pDrawBox->u32Height, 12, &rect, YUV_GREEN, 5);
		}
		}
		pthread_mutex_lock(&pDrawBox->stResultLock);
		if (pDrawBox->bResultUpdate)
		{
			TS_S32 i;
			
			for (i = 0; i < pDrawBox->u32ActualAlgNum; ++i)
			{
				if (pDrawBox->pstAlgInstList[i] != NULL && 
					pDrawBox->pstAlgInstList[i]->pf_Result_Process != NULL)
				{
					pDrawBox->pstAlgInstList[i]->pf_Result_Process(pYuvBuf, 
						pDrawBox->u32Width, pDrawBox->u32Height, 1, 
						(TS_VOID *)&pDrawBox->stAlgResult);
				}
			}
			pDrawBox->bResultUpdate = TS_FALSE;
		}
		pthread_mutex_unlock(&pDrawBox->stResultLock);

		VIDEO_FRAME_INFO_S stFrameInfoCopy = stFrameInfo;
		s32Ret = TS_MPI_VENC_SendFrame(pDrawBox->VencChn, &stFrameInfoCopy, -1);
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("TS_MPI_VENC_SendFrame failed: 0x%x\n", s32Ret);
		}

		s32Ret = TS_MPI_VPSS_ReleaseChnFrame(pDrawBox->VpssGrp, pDrawBox->VpssChn, &stFrameInfo);
		if (TS_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("TS_MPI_VPSS_ReleaseChnFrame failed: 0x%x\n", s32Ret);
		}

		u32FrameCount++;
		if (u32FrameCount % 100 == 0)
		{
			SAMPLE_PRT("VPSS_VENC_DrawBox_Thread[%d:%d] processed %d frames\n", 
					   pDrawBox->VpssGrp, pDrawBox->VpssChn, u32FrameCount);
		}
		usleep(25*1000);
	}

	SAMPLE_PRT("VPSS_VENC_DrawBox_Thread exited: VPSS[%d:%d] -> VENC[%d]\n", 
			   pDrawBox->VpssGrp, pDrawBox->VpssChn, pDrawBox->VencChn);
	return TS_NULL;
}

TS_S32 SAMPLE_VPSS_VENC_DrawBox_Start(VPSS_VENC_DRAW_BOX_S *pDrawBox, 
											   VPSS_GRP VpssGrp, VPSS_CHN VpssChn, 
											   VENC_CHN VencChn, TS_U32 u32Width, TS_U32 u32Height,
											   SAMPLE_ALG_INSTANCE_S **pstAlgInstList, TS_U32 u32AlgNum,
											   TS_S32 s32CPMGrp)
{
	TS_S32 i;

	if (pDrawBox == TS_NULL)
	{
		SAMPLE_PRT("pDrawBox is NULL!\n");
		return TS_FAILURE;
	}

	memset(pDrawBox, 0, sizeof(VPSS_VENC_DRAW_BOX_S));
	pDrawBox->VpssGrp = VpssGrp;
	pDrawBox->VpssChn = VpssChn;
	pDrawBox->VencChn = VencChn;
	pDrawBox->u32Width = u32Width;
	pDrawBox->u32Height = u32Height;
	pDrawBox->bRunFlag = TS_TRUE;
	pDrawBox->u32ActualAlgNum = u32AlgNum;
	pDrawBox->bResultUpdate = TS_FALSE;
	pDrawBox->s32CPMGrp = s32CPMGrp;

	for (i = 0; i < u32AlgNum && i < SAMPLE_ALG_MAX_NUM; i++)
	{
		pDrawBox->pstAlgInstList[i] = pstAlgInstList[i];
	}

	pthread_mutex_init(&pDrawBox->stResultLock, TS_NULL);

	if (pthread_create(&pDrawBox->tidResultSync, TS_NULL, VPSS_VENC_DrawBox_ResultSync_Thread, pDrawBox) != 0)
	{
		SAMPLE_PRT("Failed to create VPSS_VENC_DrawBox_ResultSync_Thread!\n");
		pthread_mutex_destroy(&pDrawBox->stResultLock);
		return TS_FAILURE;
	}

	if (pthread_create(&pDrawBox->tid, TS_NULL, VPSS_VENC_DrawBox_Thread, pDrawBox) != 0)
	{
		SAMPLE_PRT("Failed to create VPSS_VENC_DrawBox_Thread!\n");
		pDrawBox->bRunFlag = TS_FALSE;
		pthread_join(pDrawBox->tidResultSync, TS_NULL);
		pthread_mutex_destroy(&pDrawBox->stResultLock);
		return TS_FAILURE;
	}

	SAMPLE_PRT("VPSS_VENC_DrawBox started: VPSS[%d:%d] -> VENC[%d]\n", 
			   VpssGrp, VpssChn, VencChn);
	return TS_SUCCESS;
}

TS_S32 SAMPLE_VPSS_VENC_DrawBox_Stop(VPSS_VENC_DRAW_BOX_S *pDrawBox)
{
	if (pDrawBox == TS_NULL)
	{
		return TS_SUCCESS;
	}

	pDrawBox->bRunFlag = TS_FALSE;
	pthread_join(pDrawBox->tidResultSync, TS_NULL);
	pthread_join(pDrawBox->tid, TS_NULL);
	pthread_mutex_destroy(&pDrawBox->stResultLock);

	SAMPLE_PRT("VPSS_VENC_DrawBox stopped\n");
	return TS_SUCCESS;
}

static TS_VOID SAMPLE_VPSS_VENC_DrawBox_UpdateResult(VPSS_VENC_DRAW_BOX_S *pDrawBox, SAMPLE_ALG_RESULT_S *pAlgResult)
{
	if (pDrawBox == TS_NULL || pAlgResult == TS_NULL)
	{
		return;
	}

	pthread_mutex_lock(&pDrawBox->stResultLock);
	memcpy(&pDrawBox->stAlgResult, pAlgResult, sizeof(SAMPLE_ALG_RESULT_S));
	pDrawBox->bResultUpdate = TS_TRUE;
	pthread_mutex_unlock(&pDrawBox->stResultLock);
}

int main(int argc, char *argv[])
{
	TS_S32 s32Ret = TS_FAILURE;
	TS_S32 s32Index;
	printf("================\n");
	printf("================2\n");
	SAMPLE_SNS_TYPE_E snsType = EVB_SENSOR_2MP;

	SAMPLE_ALG_TYPE_E enCurMask;

	if (argc < 2)
	{
		SAMPLE_TSALG_VIDEO_Usage(argv[0]);
		return TS_FAILURE;
	}

	if (!strncmp(argv[1], "-h", 2))
	{
		SAMPLE_TSALG_VIDEO_Usage(argv[0]);
		return TS_SUCCESS;
	}

	s32Index = atoi(argv[1]);
	if (argc > 2)
	{
		g_pcModelRoot = argv[2];
		SAMPLE_PRT("model_file_rootdir:%s!\n", g_pcModelRoot);
	}
	else
	{
		g_pcModelRoot = TS_NULL;
	}

	SAMPLE_VIDEO_CHN_MODE eChnMode;
	s32Ret = SAMPLE_TSALG_VIDEO_Match(s32Index, &enCurMask, &eChnMode);
	if (TS_SUCCESS != s32Ret)
	{
		SAMPLE_TSALG_VIDEO_Usage(argv[0]);
		return TS_SUCCESS;
	}

	SAMPLE_COMM_SYS_ShowVersion(argv[0]);

	signal(SIGINT, SAMPLE_ALGO_HandleSig);
	signal(SIGTERM, SAMPLE_ALGO_HandleSig);

#ifdef TARGET_SDK_MODE_FASTBOOT
	if (TS_SUCCESS != TS_MPI_SYS_SetSpecialMode(SYS_SPECIAL_MASK_FASTBOOT))
	{
		SAMPLE_PRT("TS_MPI_SYS_SetSpecialMode fastmode fail!\n");
	}
	else
	{
		SAMPLE_PRT("TS_MPI_SYS_SetSpecialMode fastmode OK.\n");
	}
#endif

    s32Ret = SAMPLE_VI_VPSS_MULTICPM_VENC(snsType,  &enCurMask, 1, eChnMode);
    if (TS_SUCCESS == s32Ret)
		SAMPLE_PRT("program exit normally!\n");
	else
		printf("program exit abnormally, AlgType:%d!\n", enCurMask);

	return s32Ret;
}