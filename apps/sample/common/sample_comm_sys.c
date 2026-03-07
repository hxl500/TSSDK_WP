
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
#include <sys/mman.h>

#include "sample_comm_util.h"
#include "sample_comm_sys.h"

#include "mpi_log.h"

static TS_CHAR * g_sample_comm_name_vivpssMode[] = {
	[0]		= (ts_char* const)"offline",
    [1]     = (ts_char* const)"online"
};

static TS_CHAR * g_sample_comm_name_picSize[PIC_BUTT] = {
    [PIC_1080P]			= (ts_char* const)"1080P",
	[PIC_2560x1440]		= (ts_char* const)"2560x1440",
	[PIC_2688x1520]		= (ts_char* const)"2688x1520",
    [PIC_2880x1624]		= (ts_char* const)"2880x1624",
    [PIC_3840x2160]		= (ts_char* const)"3840x2160"
};
	
static TS_CHAR * g_sample_comm_name_wdrMode[WDR_MODE_BUTT] = {
    [WDR_MODE_NONE]			= (ts_char* const)"WDR_NONE",
	[WDR_MODE_2To1_LINE]	= (ts_char* const)"2To1_Line",
	[WDR_MODE_2To1_FRAME]	= (ts_char* const)"2To1_Frame"
};

static TS_CHAR * g_sample_comm_name_bitWid[DATA_BITWIDTH_BUTT] = {
    [DATA_BITWIDTH_8]		= (ts_char* const)"8bit",
	[DATA_BITWIDTH_10]		= (ts_char* const)"10bit",
	[DATA_BITWIDTH_12]		= (ts_char* const)"12bit"
};

static TS_CHAR * g_sample_comm_name_bayer[BAYER_BUTT] = {
    [BAYER_RGGB]		= (ts_char* const)"rggb",
	[BAYER_GRBG]		= (ts_char* const)"grbg",
	[BAYER_GBRG]		= (ts_char* const)"gbrg",
	[BAYER_BGGR]		= (ts_char* const)"bggr"
};

static MODE_SETTING_S g_sample_comm_modeSetting_list[] = {
	{1,TS_FALSE,PIC_1080P, 8, 15, WDR_MODE_NONE, DATA_BITWIDTH_8, BAYER_GRBG, "450ai & imx415 & 850sl"},
	{1,TS_FALSE,PIC_2560x1440, 8, 15, WDR_MODE_NONE, DATA_BITWIDTH_8, BAYER_GRBG,  "450ai & imx415"},
    {1,TS_TRUE,PIC_2880x1624, 8, 15, WDR_MODE_NONE, DATA_BITWIDTH_8, BAYER_GRBG,  "450ai & imx415"},



    {2,TS_TRUE,PIC_1080P, 8, 15, WDR_MODE_NONE, DATA_BITWIDTH_8, BAYER_GRBG,  "imx415 & gc4023"},
	{2,TS_TRUE,PIC_2560x1440, 8, 15, WDR_MODE_NONE, DATA_BITWIDTH_8, BAYER_GRBG,  "imx415"},
	{2,TS_FALSE,PIC_2560x1440, 8, 15, WDR_MODE_NONE, DATA_BITWIDTH_8, BAYER_GRBG,  "imx415"},

    
};

/******************************************************************************
* function : vb init & MPI system init
******************************************************************************/
TS_S32 SAMPLE_COMM_SYS_Init(VB_CONFIG_S* pstVbConfig)
{
    TS_S32 s32Ret = TS_FAILURE;

    TS_MPI_SYS_Exit();
    TS_MPI_VB_Exit();

    if (NULL == pstVbConfig)
    {
        SAMPLE_PRT("input parameter is null, it is invaild!\n");
        return TS_FAILURE;
    }

    s32Ret = TS_MPI_VB_SetConfig(pstVbConfig);

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VB_SetConf failed!\n");
        return TS_FAILURE;
    }

    s32Ret = TS_MPI_VB_Init();

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_VB_Init failed!\n");
        return TS_FAILURE;
    }

    s32Ret = TS_MPI_SYS_Init();
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_SYS_Init failed!\n");
        TS_MPI_VB_Exit();
        return TS_FAILURE;
    }

	if(0){
		LOG_LEVEL_CONF_S stConf = {0};

		stConf.enModId = TS_ID_VENC;

		TS_MPI_LOG_GetLevelConf(&stConf);
		stConf.s32Level = TS_DBG_DEBUG;
		TS_MPI_LOG_SetLevelConf(&stConf);
	}

    return s32Ret;
}

/******************************************************************************
* function : vb exit & MPI system exit
******************************************************************************/
TS_VOID SAMPLE_COMM_SYS_Exit(void)
{
    TS_S32 s32Ret = TS_FAILURE;

    /*SAMPLE_COMM_VO_Exit();*/	/* only for debug */
    s32Ret = TS_MPI_SYS_Exit();
	if(TS_SUCCESS != s32Ret){
		SAMPLE_PRT("TS_MPI_SYS_Exit faild with %#x\n", s32Ret);
	}
	else{
		SAMPLE_PRT("TS_MPI_SYS_Exit success\n");
	}

    //TS_MPI_VB_ExitModCommPool(VB_UID_VDEC);
    s32Ret = TS_MPI_VB_Exit();
	if(TS_SUCCESS != s32Ret){
		SAMPLE_PRT("TS_MPI_VB_Exit failed. ret %#x\n", s32Ret);
	}
	else{
		SAMPLE_PRT("TS_MPI_VB_Exit success\n");
	}

    return;
}

TS_S32 SAMPLE_COMM_SYS_ShowVersion(TS_CHAR * pAppName)
{
	MPP_VERSION_S stVersion = {0};

	TS_MPI_SYS_GetVersion(&stVersion);

	SAMPLE_PRT("%s %s: %s is working now\n", stVersion.aVersion, APP_VERSION_STR, pAppName);

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VB_Save2File(VIDEO_FRAME_INFO_S *pVideoFrame, TS_CHAR * pPath, TS_S32 fileIdx)
{
	TS_CHAR fileName[128];
    FILE * fd = NULL;
	int bytes = 0;

	if(!pVideoFrame){
		SAMPLE_PRT("NULL ptr \n");
		return TS_FAILURE;
	}

	SAMPLE_PRT("frame info : wh=[%d,%d], stride=[%d,%d], virAddr=[%llx,%llx], [%llx,%llx]\n",
		pVideoFrame->stVFrame.u32Width, pVideoFrame->stVFrame.u32Height,
		pVideoFrame->stVFrame.u32Stride[0], pVideoFrame->stVFrame.u32Stride[1],
		pVideoFrame->stVFrame.u64VirAddr[0], pVideoFrame->stVFrame.u64VirAddr[1],
		pVideoFrame->stVFrame.u64PhyAddr[0], pVideoFrame->stVFrame.u64PhyAddr[1]);

	if(fileIdx >= 0){
		if(pPath){
			sprintf(fileName, "%s/frame_%04d_%d_%d.yuv", pPath, fileIdx,
				pVideoFrame->stVFrame.u32Width, pVideoFrame->stVFrame.u32Height);
		}
		else{
			sprintf(fileName, "./frame_%04d_%d_%d.yuv", fileIdx,
				pVideoFrame->stVFrame.u32Width, pVideoFrame->stVFrame.u32Height);
		}
	}
	else{
		if(pPath){
			sprintf(fileName, "%s/frame_%d_%d.yuv", pPath,
				pVideoFrame->stVFrame.u32Width, pVideoFrame->stVFrame.u32Height);
		}
		else{
			sprintf(fileName, "./frame_%d_%d.yuv",
				pVideoFrame->stVFrame.u32Width, pVideoFrame->stVFrame.u32Height);
		}
	}

	#if 1
	fd = fopen(fileName, "w+");
	#else
	fd = fopen(fileName, O_WRONLY | O_CREAT | O_TRUNC,
			  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	#endif
	if (!fd){
		SAMPLE_PRT("unable to create debug file.");
		return TS_FAILURE;
	}

	#if 0
	if(pVideoFrame->stVFrame.u64VirAddr){
		#if 1
		bytes = fwrite(pVideoFrame->stVFrame.u64VirAddr[0], 1,
			pVideoFrame->stVFrame.u32Width * pVideoFrame->stVFrame.u32Height * 3 / 2, fd);
		#else
		bytes = fwrite(fd, pVideoFrame->stVFrame.u64VirAddr[0],
			pVideoFrame->stVFrame.u32Width * pVideoFrame->stVFrame.u32Height * 3 / 2);
		#endif
		if(bytes <= 0){
			SAMPLE_PRT("write faild with %d, errString is %s\n", bytes, strerror(errno));
		}
	}
	#else
    ts_char *dmaStr = mmap(NULL, pVideoFrame->stVFrame.size, PROT_READ, MAP_SHARED,
    	pVideoFrame->stVFrame.fd, 0);

    if (dmaStr && dmaStr != MAP_FAILED) {
        bytes = fwrite(dmaStr, pVideoFrame->stVFrame.size, 1, fd);
		if(bytes <= 0){
			SAMPLE_PRT("write faild with %d, errString is %s\n", bytes, strerror(errno));
		}
        munmap(dmaStr, pVideoFrame->stVFrame.size);
    }
	#endif

	fclose(fd);

	SAMPLE_PRT("save %s success, size=%d\n", fileName, bytes);

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_SYS_GetPicSize(PIC_SIZE_E enPicSize, SIZE_S * pstSize)
{
    switch (enPicSize)
    {
        case PIC_CIF:   /* 352 * 288 */
            pstSize->u32Width  = 352;
            pstSize->u32Height = 288;
            break;

        case PIC_D1_PAL:   /* 720 * 576 */
            pstSize->u32Width  = 720;
            pstSize->u32Height = 576;
            break;

        case PIC_D1_NTSC:   /* 720 * 480 */
            pstSize->u32Width  = 720;
            pstSize->u32Height = 480;
            break;

        case PIC_360P:   /* 640 * 360 */
            pstSize->u32Width  = 640;
            pstSize->u32Height = 360;
            break;
			
		case PIC_576P:   /* 704 * 576 */
            pstSize->u32Width  = 704;
            pstSize->u32Height = 576;
            break;
		case PIC_720P:   /* 1280 * 720 */
            pstSize->u32Width  = 1280;
            pstSize->u32Height = 720;
            break;

        case PIC_1080P:  /* 1920 * 1080 */
            pstSize->u32Width  = 1920;
            pstSize->u32Height = 1080;
            break;
        
        case PIC_1080x1920:  /* 1080 * 1920 */
            pstSize->u32Width  = 1080;
            pstSize->u32Height = 1920;
            break;

        case PIC_2560x1440:
            pstSize->u32Width  = 2560;
            pstSize->u32Height = 1440;
            break;

		case PIC_1440x2560:
			pstSize->u32Width  = 1440;
			pstSize->u32Height = 2560;
			break;

        case PIC_2592x1520:
            pstSize->u32Width  = 2592;
            pstSize->u32Height = 1520;
            break;

        case PIC_2688x1860:
            pstSize->u32Width  = 2688;
            pstSize->u32Height = 1860;
            break;

        case PIC_2592x1944:
            pstSize->u32Width  = 2592;
            pstSize->u32Height = 1944;
            break;

        case PIC_2592x1536:
            pstSize->u32Width  = 2592;
            pstSize->u32Height = 1536;
            break;
		case PIC_2688x1520:
            pstSize->u32Width  = 2688;
            pstSize->u32Height = 1520;
            break;
	    case PIC_2716x1524:
            pstSize->u32Width  = 2716;
            pstSize->u32Height = 1524;
            break;

		case PIC_2880x1624:
            pstSize->u32Width  = 2880;
            pstSize->u32Height = 1624;
            break;

        case PIC_3840x2160:
            pstSize->u32Width  = 3840;
            pstSize->u32Height = 2160;
            break;

        case PIC_3000x3000:
            pstSize->u32Width  = 3000;
            pstSize->u32Height = 3000;
            break;

        case PIC_4000x3000:
            pstSize->u32Width  = 4000;
            pstSize->u32Height = 3000;
            break;

        case PIC_4096x2160:
            pstSize->u32Width  = 4096;
            pstSize->u32Height = 2160;
            break;

        case PIC_7680x4320:
            pstSize->u32Width  = 7680;
            pstSize->u32Height = 4320;
            break;
		case PIC_8192x8192:
            pstSize->u32Width  = 8192;
            pstSize->u32Height = 8192;
            break;
        case PIC_3840x8640:
            pstSize->u32Width = 3840;
            pstSize->u32Height = 8640;
            break;
        case PIC_2304x1296:
            pstSize->u32Width  = 2304;
            pstSize->u32Height = 1296;
            break;
        case PIC_2880x1620:
            pstSize->u32Width  = 2880;
            pstSize->u32Height = 1620;
            break;
        case PIC_3200x1800:
            pstSize->u32Width  = 3200;
            pstSize->u32Height = 1800;
            break;
        case PIC_2560x2560:
            pstSize->u32Width  = 2560;
            pstSize->u32Height = 2560;
            break;
        case PIC_1280x720:
            pstSize->u32Width  = 1280;
            pstSize->u32Height = 720;
            break;
        case PIC_1366x768:
            pstSize->u32Width  = 1366;
            pstSize->u32Height = 768;
            break;
        case PIC_704x576:
            pstSize->u32Width  = 704;
            pstSize->u32Height = 576;
            break;
        case PIC_176x144:
            pstSize->u32Width  = 176;
            pstSize->u32Height = 144;
            break;
        case PIC_480x270:
            pstSize->u32Width  = 480;
            pstSize->u32Height = 270;
            break;
        case PIC_240x320:
            pstSize->u32Width  = 240;
            pstSize->u32Height = 320;
            break;
        case PIC_240x160:
            pstSize->u32Width  = 240;
            pstSize->u32Height = 160;
            break;
        case PIC_1080EX:
            pstSize->u32Width = 1920;
            pstSize->u32Height = 1080 * 2;
            break;
        case PIC_360EX:
            pstSize->u32Width = 640;
            pstSize->u32Height = 360 * 2;
            break;
        case PIC_PATTERN:
            pstSize->u32Width = 64;
            pstSize->u32Height = 64;
            break;
        default:
            return TS_FAILURE;
    }

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_SYS_GetPicSizeE(TS_U32 width, TS_U32 height, PIC_SIZE_E * enPicSize)
{
	if(352 == width && 288 == height){
		* enPicSize = PIC_CIF;
	}
	else if(720 == width && 576 == height){
		* enPicSize = PIC_D1_PAL;
	}
	else if(720 == width && 480 == height){
		* enPicSize = PIC_D1_NTSC;
	}
	else if(640 == width && 360 == height){
		* enPicSize = PIC_360P;
	}
	else if(704 == width && 576 == height){
		* enPicSize = PIC_576P;
	}
	else if(1280 == width && 720 == height){
		* enPicSize = PIC_720P;
	}
	else if(1920 == width && 1080 == height){
		* enPicSize = PIC_1080P;
		SAMPLE_PRT("enPicSize=%d\n", *enPicSize);
	}
	else if(2560 == width && 1440 == height){
		* enPicSize = PIC_2560x1440;
	}
	else if(1440 == width && 2560 == height){
		* enPicSize = PIC_1440x2560;
	}
	else if(2592 == width && 1520 == height){
		* enPicSize = PIC_2592x1520;
	}
	else if(2688 == width && 1860 == height){
		* enPicSize = PIC_2688x1860;
	}
	else if(2592 == width && 1944 == height){
		* enPicSize = PIC_2592x1944;
	}
	else if(2592 == width && 1536 == height){
		* enPicSize = PIC_2592x1536;
	}
	else if(2688 == width && 1520 == height){
		* enPicSize = PIC_2688x1520;
	}
	else if(2716 == width && 1524 == height){
		* enPicSize = PIC_2716x1524;
	}
	else if(2880 == width && 1624 == height){
		* enPicSize = PIC_2880x1624;
	}
	else if(3840 == width && 2160 == height){
		* enPicSize = PIC_3840x2160;
	}
	else if(3000 == width && 3000 == height){
		* enPicSize = PIC_3000x3000;
	}
	else if(4000 == width && 3000 == height){
		* enPicSize = PIC_4000x3000;
	}
	else if(4096 == width && 2160 == height){
		* enPicSize = PIC_4096x2160;
	}
	else if(7680 == width && 4320 == height){
		* enPicSize = PIC_7680x4320;
	}else if(8192 == width && 8192 == height){
		* enPicSize = PIC_8192x8192;
	}
	else if(3840 == width && 8640 == height){
		* enPicSize = PIC_3840x8640;
	}
	else if(2304 == width && 1296 == height){
		* enPicSize = PIC_2304x1296;
	}
	else if(2880 == width && 1620 == height){
		* enPicSize = PIC_2880x1620;
	}
	else if(3200 == width && 1800 == height){
		* enPicSize = PIC_3200x1800;
	}
	else if(2560 == width && 2560 == height){
		* enPicSize = PIC_2560x2560;
	}
	else if(1280 == width && 720 == height){
		* enPicSize = PIC_1280x720;
	}
	else if(1366 == width && 768 == height){
		* enPicSize = PIC_1366x768;
	}
	else if(704 == width && 576 == height){
		* enPicSize = PIC_704x576;
	}
	else if(176 == width && 144 == height){
		* enPicSize = PIC_176x144;
	}
	else if(480 == width && 270 == height){
		* enPicSize = PIC_480x270;
	}
	else if(1920 == width && 1080*2 == height){
		* enPicSize = PIC_1080EX;
	}
	else if(640 == width && 360*2 == height){
		* enPicSize = PIC_360EX;
	}
	else if(64 == width && 64 == height){
		* enPicSize = PIC_PATTERN;
	}
	else{
		return TS_FAILURE;
	}

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_SYS_SelectMode(MODE_SETTING_S * pModeSelect)
{
	TS_S32 arraySize = sizeof(g_sample_comm_modeSetting_list) / sizeof(MODE_SETTING_S);
	TS_CHAR inBuf[32] = {0};
	TS_S32 i, offset, nSelect = -1;
    
	printf("you can select the sns mode setting from the below %d :\n", arraySize);
	for(i = 0; i < arraySize; i ++){
		MODE_SETTING_S * pSetting = &g_sample_comm_modeSetting_list[i];
	
		printf("(%02d): Camera number: %d, %s,%s, fpsRange=[%d,%d], %s, %s, %s. used for %s\n", i,
			pSetting->numCam,
			g_sample_comm_name_vivpssMode[pSetting->isOnLine],
			g_sample_comm_name_picSize[pSetting->enSize],
			pSetting->minFps, pSetting->maxFps,
			g_sample_comm_name_wdrMode[pSetting->wdr_mode],
			g_sample_comm_name_bitWid[pSetting->bit_width],
			g_sample_comm_name_bayer[pSetting->enBayer],
			pSetting->alias);
	}

	while(1){
		TS_S32 bError = 0;

	    printf("please input, and press q to break : ");
		memset(inBuf,0x00,sizeof(inBuf));
	    setbuf(stdin, NULL);
		fgets(inBuf, sizeof(inBuf), stdin);
		offset = strcspn(inBuf, "\n");
		if(offset > 0 && offset <= 30){
            TS_S32 curIdx = 0;
			inBuf[offset] = 0;

            for(i = 0; i < offset; i ++){
                if(inBuf[i] == ' ') curIdx ++;
                else break;
            }

            if('q' == inBuf[curIdx] && 0 == inBuf[curIdx + 1]){
                printf("quit select\n");
                return TS_FAILURE;
            }

			for(i = curIdx; i < offset; i ++){
				if(inBuf[i] < '0' || inBuf[i] > '9' || inBuf[i] == ' '){
					bError = 1;
					break;
				}
			}

			if(!bError){
				nSelect = atoi(inBuf);
				if(nSelect < 0 || nSelect >= arraySize){
					bError = 1;
				}
			}

			if(bError){				
				printf("invalid input, try again\n");
				continue;
			}
			memcpy(pModeSelect, &g_sample_comm_modeSetting_list[nSelect], sizeof(MODE_SETTING_S));
			printf("get select[%d] : camNum=%d, %s, %s, fpsRange=[%d,%d], %s, %s, %s. \n", nSelect,
                pModeSelect->numCam,
                g_sample_comm_name_vivpssMode[pModeSelect->isOnLine],
                g_sample_comm_name_picSize[pModeSelect->enSize],
                pModeSelect->minFps, pModeSelect->maxFps,
                g_sample_comm_name_wdrMode[pModeSelect->wdr_mode],
                g_sample_comm_name_bitWid[pModeSelect->bit_width],
                g_sample_comm_name_bayer[pModeSelect->enBayer]);
			break;
		}
		else{
			continue;
		}
	}

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VI_Bind_VO(VI_PIPE ViPipe, VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = TS_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VI-VO)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VI_UnBind_VO(VI_PIPE ViPipe, VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = TS_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(VI-VO)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VI_Bind_VPSS(VI_PIPE ViPipe, VI_CHN ViChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = TS_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VI-VPSS)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VI_UnBind_VPSS(VI_PIPE ViPipe, VI_CHN ViChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = TS_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(VI-VPSS)");

	SAMPLE_PRT("TS_MPI_SYS_UnBind success! ViPipe:%d ViChn:%d VpssGrp:%d\n", ViPipe, ViChn, VpssGrp);

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VI_Bind_VENC(VI_PIPE ViPipe, VI_CHN ViChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = TS_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VI-VENC)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VI_UnBind_VENC(VI_PIPE ViPipe, VI_CHN ViChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = TS_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(VI-VENC)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VPSS_Bind_AVS(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP AvsGrp, AVS_PIPE AvsPipe)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = TS_ID_AVS;
    stDestChn.s32DevId = AvsGrp;
    stDestChn.s32ChnId = AvsPipe;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VPSS-AVS)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VPSS_UnBind_AVS(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP AvsGrp, AVS_PIPE AvsPipe)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = TS_ID_AVS;
    stDestChn.s32DevId = AvsGrp;
    stDestChn.s32ChnId = AvsPipe;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(VPSS-AVS)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VI_Bind_CPM(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, CPM_GRP CpmGrp, CPM_PIPE CpmPipe)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VI;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = TS_ID_CPM;
    stDestChn.s32DevId = CpmGrp;
    stDestChn.s32ChnId = CpmPipe;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VI-CPM)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VI_UnBind_CPM(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, CPM_GRP CpmGrp, CPM_PIPE CpmPipe)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VI;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = TS_ID_CPM;
    stDestChn.s32DevId = CpmGrp;
    stDestChn.s32ChnId = CpmPipe;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(VI-CPM)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VPSS_Bind_CPM(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, CPM_GRP CpmGrp, CPM_PIPE CpmPipe)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = TS_ID_CPM;
    stDestChn.s32DevId = CpmGrp;
    stDestChn.s32ChnId = CpmPipe;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VPSS-CPM)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VPSS_UnBind_CPM(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, CPM_GRP CpmGrp, CPM_PIPE CpmPipe)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = TS_ID_CPM;
    stDestChn.s32DevId = CpmGrp;
    stDestChn.s32ChnId = CpmPipe;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(VPSS-CPM)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_CPM_Bind_VENC(CPM_GRP CpmGrp, CPM_CHN CpmChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_CPM;
    stSrcChn.s32DevId  = CpmGrp;
    stSrcChn.s32ChnId  = CpmChn;

    stDestChn.enModId  = TS_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(CPM-VENC)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_CPM_UnBind_VENC(CPM_GRP CpmGrp, CPM_CHN CpmChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_CPM;
    stSrcChn.s32DevId  = CpmGrp;
    stSrcChn.s32ChnId  = CpmChn;

    stDestChn.enModId  = TS_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(CPM-VENC)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_CPM_Bind_VO(CPM_GRP CpmGrp, CPM_CHN CpmChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_CPM;
    stSrcChn.s32DevId  = CpmGrp;
    stSrcChn.s32ChnId  = CpmChn;

    stDestChn.enModId  = TS_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(CPM-VO)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_CPM_UnBind_VO(CPM_GRP CpmGrp, CPM_CHN CpmChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_CPM;
    stSrcChn.s32DevId  = CpmGrp;
    stSrcChn.s32ChnId  = CpmChn;

    stDestChn.enModId  = TS_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(CPM-VO)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VPSS_Bind_VO(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = TS_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VPSS-VO)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VPSS_Bind_VPSS(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_GRP VpssGrpDst)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = TS_ID_VPSS;
    stDestChn.s32DevId = VpssGrpDst;
    stDestChn.s32ChnId = 0;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VPSS-VPSS)");

    return TS_SUCCESS;
}


TS_S32 SAMPLE_COMM_VPSS_UnBind_VO(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = TS_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(VPSS-VO)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VPSS_Bind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = TS_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VPSS-VENC)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VPSS_UnBind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VPSS;
    stSrcChn.s32DevId  = VpssGrp;
    stSrcChn.s32ChnId  = VpssChn;

    stDestChn.enModId  = TS_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(VPSS-VENC)");

	SAMPLE_PRT("TS_MPI_SYS_UnBind success! VpssGrp:%d VpssChn:%d VencChn:%d\n", VpssGrp, VpssChn, VencChn);

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VDEC_Bind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VDEC;
    stSrcChn.s32DevId  = 0;
    stSrcChn.s32ChnId  = VdecChn;

    stDestChn.enModId  = TS_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VDEC-VPSS)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VDEC_UnBind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId  = TS_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VdecChn;

    stDestChn.enModId  = TS_ID_VPSS;
    stDestChn.s32DevId = VpssGrp;
    stDestChn.s32ChnId = 0;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(VDEC-VPSS)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VO_Bind_VO(VO_LAYER  SrcVoLayer, VO_CHN SrcVoChn, VO_LAYER DstVoLayer, VO_CHN DstVoChn)
{
    MPP_CHN_S stSrcChn, stDestChn;
    stSrcChn.enModId    = TS_ID_VO;
    stSrcChn.s32DevId   = SrcVoLayer;
    stSrcChn.s32ChnId   = SrcVoChn;

    stDestChn.enModId   = TS_ID_VO;
    stDestChn.s32DevId  = DstVoLayer;
    stDestChn.s32ChnId  = DstVoChn;

    return TS_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

TS_S32 SAMPLE_COMM_VO_UnBind_VO(VO_LAYER DstVoLayer, VO_CHN DstVoChn)
{
    MPP_CHN_S stDestChn;
    stDestChn.enModId   = TS_ID_VO;
    stDestChn.s32DevId  = DstVoLayer;
    stDestChn.s32ChnId  = DstVoChn;

    return TS_MPI_SYS_UnBind(NULL, &stDestChn);
}


TS_S32 SAMPLE_COMM_VDEC_Bind_VO(VDEC_CHN VdecChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = TS_ID_VDEC;
    stSrcChn.s32DevId  = 0;
    stSrcChn.s32ChnId  = VdecChn;

    stDestChn.enModId  = TS_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;


    CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VDEC-VO)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VDEC_UnBind_VO(VDEC_CHN VdecChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId  = TS_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VdecChn;

    stDestChn.enModId  = TS_ID_VO;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(VDEC-VO)");

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VDEC_Bind_CPM(VDEC_CHN VdecChn, CPM_GRP CpmGrp, CPM_PIPE CpmPipe)
{
	MPP_CHN_S stSrcChn;
	MPP_CHN_S stDestChn;

	stSrcChn.enModId   = TS_ID_VDEC;
	stSrcChn.s32DevId  = 0;
	stSrcChn.s32ChnId  = VdecChn;

	stDestChn.enModId  = TS_ID_CPM;
	stDestChn.s32DevId = CpmGrp;
	stDestChn.s32ChnId = CpmPipe;

	CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(VDEC-CPM)");

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_VDEC_UnBind_CPM(VDEC_CHN VdecChn, CPM_GRP CpmGrp, CPM_PIPE CpmPipe)
{
	MPP_CHN_S stSrcChn;
	MPP_CHN_S stDestChn;

	stSrcChn.enModId   = TS_ID_VDEC;
	stSrcChn.s32DevId  = 0;
	stSrcChn.s32ChnId  = VdecChn;

	stDestChn.enModId  = TS_ID_CPM;
	stDestChn.s32DevId = CpmGrp;
	stDestChn.s32ChnId = CpmPipe;

	CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(VDEC-CPM)");

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_AI_Bind_AO(AI_CHN AiChn, AO_CHN AoChn)
{
	MPP_CHN_S stSrcChn;
	MPP_CHN_S stDestChn;

	stSrcChn.enModId   = TS_ID_AI;
	stSrcChn.s32DevId  = 0;
	stSrcChn.s32ChnId  = AiChn;

	stDestChn.enModId  = TS_ID_AO;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = AoChn;

	CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(AI-AO)");

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_AI_UnBind_AO(AI_CHN AiChn, AO_CHN AoChn)
{
	MPP_CHN_S stSrcChn;
	MPP_CHN_S stDestChn;

	stSrcChn.enModId   = TS_ID_AI;
	stSrcChn.s32DevId  = 0;
	stSrcChn.s32ChnId  = AiChn;

	stDestChn.enModId  = TS_ID_AO;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = AoChn;

	CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(AI-AO)");

	return TS_SUCCESS;
}


TS_S32 SAMPLE_COMM_AI_Bind_AENC(AI_CHN AiChn, AENC_CHN AencChn)
{
	MPP_CHN_S stSrcChn;
	MPP_CHN_S stDestChn;

	stSrcChn.enModId   = TS_ID_AI;
	stSrcChn.s32DevId  = 0;
	stSrcChn.s32ChnId  = AiChn;

	stDestChn.enModId  = TS_ID_AENC;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = AencChn;

	CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(AI-AENC)");

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_AI_UnBind_AENC(AI_CHN AiChn, AENC_CHN AencChn)
{
	MPP_CHN_S stSrcChn;
	MPP_CHN_S stDestChn;

	stSrcChn.enModId   = TS_ID_AI;
	stSrcChn.s32DevId  = 0;
	stSrcChn.s32ChnId  = AiChn;

	stDestChn.enModId  = TS_ID_AENC;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = AencChn;

	CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(AI-AENC)");

	return TS_SUCCESS;
}


TS_S32 SAMPLE_COMM_ADEC_Bind_AO(ADEC_CHN AdChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
	MPP_CHN_S stSrcChn;
	MPP_CHN_S stDestChn;

	stSrcChn.enModId   = TS_ID_ADEC;
	stSrcChn.s32DevId  = 0;
	stSrcChn.s32ChnId  = AdChn;

	stDestChn.enModId  = TS_ID_AO;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = AoChn;

	CHECK_RET(TS_MPI_SYS_Bind(&stSrcChn, &stDestChn), "TS_MPI_SYS_Bind(ADEC-AO)");

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_ADEC_UnBind_AO(ADEC_CHN AdChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
	MPP_CHN_S stSrcChn;
	MPP_CHN_S stDestChn;

	stSrcChn.enModId   = TS_ID_ADEC;
	stSrcChn.s32DevId  = 0;
	stSrcChn.s32ChnId  = AdChn;

	stDestChn.enModId  = TS_ID_AO;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = AoChn;

	CHECK_RET(TS_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "TS_MPI_SYS_UnBind(ADEC-AO)");

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_EXECUTE_CMD_BY_SYSTEM(const char *cmd_line) 
{
	TS_S32 status = 0;
	__sighandler_t old_handler;

	if (NULL == cmd_line)
	{
		SAMPLE_PRT("input param error!\n");
		return TS_FAILURE;
	}

	/*note: resole call system failed when sig changed(not SIG_DFL)*/
	old_handler = signal(SIGCHLD, SIG_DFL);
	status = system((char*)cmd_line);
	signal(SIGCHLD, old_handler);

	if ( status < 0)
	{
		/*note: you best output errno info,remember #include <errno.h>*/
		SAMPLE_PRT("cmd: %s\t error: %s!\n",cmd_line, strerror(errno));
		return TS_FAILURE;
	}
	else
	{
		if (WIFEXITED(status))
		{
			/*excute command result*/
			if (0 == WEXITSTATUS(status))
			{
                SAMPLE_PRT("run system cmd: %s\t successfully \n",cmd_line);
				return TS_SUCCESS;
			}
			else
			{
                SAMPLE_PRT("cmd: %s!\n",cmd_line);
                SAMPLE_PRT("normal termination, run shell script fail, script exit code: %d, error: %s!\n", WEXITSTATUS(status), strerror(errno)); 
				return TS_FAILURE;
			}
		}
		else if (WIFSIGNALED(status))
		{
			/*cmd_line is interrupted by signal, get signal number*/
			SAMPLE_PRT("Abnormal termination, signal number = %d.\n", WTERMSIG(status));
			return TS_FAILURE;
		}
		else if (WIFSTOPPED(status))
		{
			/*cmd_line is stopped by child , get signal number*/
			SAMPLE_PRT("child stopped, signal number = %d\n",WSTOPSIG(status));
			return TS_FAILURE;
		}
	}

	SAMPLE_PRT("unknow error!\n");
	return TS_FAILURE;
}

TS_CHAR* SAMPLE_COMM_EXECUTE_CMD_BY_POPEN(const char *cmd_line) 
{
    FILE *fp;
    char buffer[256];

    static char output[256];
    output[0] = '\0';

    fp = popen(cmd_line, "r");
    if (fp == NULL) {
        perror("popen");
        return NULL;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        strcat(output, buffer);
    }

    pclose(fp);
    return output;
}
