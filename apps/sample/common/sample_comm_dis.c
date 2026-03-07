
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

#include "sample_comm_util.h"
#include "sample_comm_vi.h"
#include "sample_comm_isp.h"
#include "sample_comm_sys.h"
#include "sample_comm_dis.h"
#include "sample_comm_vpss.h"

TS_S32 SAMPLE_COMM_DIS_SetParam(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    TS_S32              i;
    TS_S32              s32Ret;
    VI_PIPE             ViPipe;
	VI_VPSS_MODE_E		aeMode[VI_MAX_PIPE_NUM];
    VI_VPSS_MODE_S      stVIVPSSMode = {0, aeMode};
    SAMPLE_VI_INFO_S*   pstViInfo = TS_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("null ptr\n");
        return TS_FAILURE;
    }

	if(pstViConfig->s32WorkingViNum > VI_MAX_PIPE_NUM){
		SAMPLE_PRT("WorkingViNum[%d] is invalid, maxValid=%d\n", pstViConfig->s32WorkingViNum, VI_MAX_PIPE_NUM);
		return TS_FAILURE;
	}

    s32Ret = TS_MPI_SYS_GetVIVPSSMode(&stVIVPSSMode);

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Get VI-VPSS mode Param failed with %#x!\n", s32Ret);

        return TS_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        pstViInfo = &pstViConfig->astViInfo[i];
        ViPipe    = pstViInfo->stPipeInfo.aPipe;
		if(ViPipe >= VI_MAX_PIPE_NUM){
			SAMPLE_PRT("Vi[%d] : pipe[%d] is invalid, maxValid=%d\n", i, ViPipe, VI_MAX_PIPE_NUM);
			return TS_FAILURE;
		}
        stVIVPSSMode.aenMode[ViPipe] = pstViInfo->stPipeInfo.enMastPipeMode;
    }
	stVIVPSSMode.s32ModeCount = pstViConfig->s32WorkingViNum;

    s32Ret = TS_MPI_SYS_SetVIVPSSMode(&stVIVPSSMode);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Set VI-VPSS mode Param failed with %#x!\n", s32Ret);
        return TS_FAILURE;
    }

    return TS_SUCCESS;
}

static TS_S32 SAMPLE_COMM_DIS_StopSingleViPipe(VI_PIPE ViPipe)
{
    TS_S32  s32Ret;

    s32Ret = TS_MPI_VI_StopPipe(ViPipe);
    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("TS_MPI_VI_StopPipe failed with %#x!\n", s32Ret);
        return TS_FAILURE;
    }

    s32Ret = TS_MPI_VI_DestroyPipe(ViPipe);
    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("TS_MPI_VI_DestroyPipe failed with %#x!\n", s32Ret);
        return TS_FAILURE;
    }

    return s32Ret;
}

TS_S32 SAMPLE_COMM_DIS_StartViPipe(SAMPLE_VI_INFO_S* pstViInfo)
{
    TS_S32          s32Ret = TS_SUCCESS;
    VI_PIPE         ViPipe;
    VI_PIPE_ATTR_S  stPipeAttr = {0};

	ViPipe = pstViInfo->stPipeInfo.aPipe;
	
	stPipeAttr.u32MaxW = pstViInfo->stPipeInfo.width;
	stPipeAttr.u32MaxH = pstViInfo->stPipeInfo.height;
	stPipeAttr.enPixFmt	= pstViInfo->stPipeInfo.enPixFmt;
	stPipeAttr.enBitWidth = pstViInfo->stPipeInfo.enBitWid;
	stPipeAttr.enPipeBypassMode = VI_PIPE_BYPASS_NONE;
	stPipeAttr.stFrameRate.s32SrcFrameRate = pstViInfo->stPipeInfo.frameRate;
	stPipeAttr.stFrameRate.s32DstFrameRate = pstViInfo->stPipeInfo.frameRate;
    stPipeAttr.bIspBypass = pstViInfo->stPipeInfo.bIspByFly;

	SAMPLE_PRT("vipipe[%d]: wh=[%d,%d], ispBypass=%d, yuvSkip=%d, bitWid=%d, pix=%d, frm-[%d,%d]\n", ViPipe,
		stPipeAttr.u32MaxW, stPipeAttr.u32MaxH,
		stPipeAttr.bIspBypass,
		stPipeAttr.bYuvSkip,
		stPipeAttr.enBitWidth,
		stPipeAttr.enPixFmt,
		stPipeAttr.stFrameRate.s32SrcFrameRate, stPipeAttr.stFrameRate.s32DstFrameRate);
	
	s32Ret = TS_MPI_VI_CreatePipe(ViPipe, &stPipeAttr);
	if (s32Ret != TS_SUCCESS)
	{
		SAMPLE_PRT("TS_MPI_VI_CreatePipe[%d] failed with %#x!\n", ViPipe, s32Ret);
		return TS_FAILURE;
	}
	SAMPLE_PRT("TS_MPI_VI_CreatePipe[%d] success!\n", ViPipe);
	
	s32Ret = TS_MPI_VI_StartPipe(ViPipe);
	if (s32Ret != TS_SUCCESS)
	{
		TS_MPI_VI_DestroyPipe(ViPipe);
		SAMPLE_PRT("TS_MPI_VI_StartPipe[%d] failed with %#x!\n", ViPipe, s32Ret);
		goto EXIT;
	}
	SAMPLE_PRT("TS_MPI_VI_StartPipe[%d] success!\n", ViPipe);

    return s32Ret;

EXIT:
    SAMPLE_COMM_DIS_StopSingleViPipe(ViPipe);

    return s32Ret;
}

TS_S32 SAMPLE_COMM_DIS_StopViPipe(SAMPLE_VI_INFO_S* pstViInfo)
{
    if (pstViInfo->stPipeInfo.aPipe >= 0  && pstViInfo->stPipeInfo.aPipe < VI_MAX_PIPE_NUM)
    {
        VI_PIPE ViPipe = pstViInfo->stPipeInfo.aPipe;
        SAMPLE_COMM_DIS_StopSingleViPipe(ViPipe);
    }

    return TS_SUCCESS;
}
TS_S32 SAMPLE_COMM_DIS_StartViChn(SAMPLE_VI_INFO_S* pstViInfo, SAMPLE_DIS_CONFIG_S *stLdcInfo)
{
    TS_S32 i, s32Ret = TS_SUCCESS;
    VI_PIPE ViPipe = pstViInfo->stPipeInfo.aPipe;
	
    for(i = 0; i < pstViInfo->stChnInfo.validChnlNum; i ++){
        VI_CHN  ViChn = pstViInfo->stChnInfo.ViChn[i];
        VI_CHN_ATTR_S stChnAttr = {0};
       if(ViChn >= 0 && ViChn < VI_MAX_PHY_CHN_NUM){
            stChnAttr.bFlip = TS_FALSE;
            stChnAttr.bMirror = TS_FALSE;
			stChnAttr.u32Depth = 5;
            stChnAttr.enPixelFormat = PIXEL_FORMAT_NV_12;
            stChnAttr.stSize.u32Width = pstViInfo->stChnInfo.width[ViChn];
            stChnAttr.stSize.u32Height = pstViInfo->stChnInfo.height[ViChn];
			stChnAttr.stFrameRate.s32SrcFrameRate = -1;
			stChnAttr.stFrameRate.s32DstFrameRate = -1;			

            s32Ret = TS_MPI_VI_SetChnAttr(ViPipe, ViChn, &stChnAttr);
            if (s32Ret != TS_SUCCESS)
            {
                SAMPLE_PRT("TS_MPI_VI_SetChnAttr failed with %#x!\n", s32Ret);
                return TS_FAILURE;
            }

			if(i < stLdcInfo->s32WorkingViNum)
			{
				s32Ret = TS_MPI_VI_SetChnLDCAttr(ViPipe, ViChn, &stLdcInfo->pstLdcInfo[i]);
				if (s32Ret != TS_SUCCESS)
				{
					SAMPLE_PRT("TS_MPI_VI_SetChnLDCAttr failed with %#x!\n", s32Ret);
					return TS_FAILURE;
				}
			}
            s32Ret = TS_MPI_VI_EnableChn(ViPipe, ViChn);

            if (s32Ret != TS_SUCCESS)
            {
                SAMPLE_PRT("TS_MPI_VI_EnableChn failed with %#x!\n", s32Ret);
                return TS_FAILURE;
				SAMPLE_PRT("TS_MPI_VI_EnableChn [%d] success!\n", ViChn);
            }            
        }
    }

    return s32Ret;
}

TS_S32 SAMPLE_COMM_DIS_StopViChn(SAMPLE_VI_INFO_S* pstViInfo)
{
    TS_S32  i, s32Ret = TS_SUCCESS;
    VI_PIPE ViPipe = pstViInfo->stPipeInfo.aPipe;

    for(i = 0; i < pstViInfo->stChnInfo.validChnlNum; i ++){
        VI_CHN  ViChn = pstViInfo->stChnInfo.ViChn[i];
        if(ViChn >= 0 && ViChn < VI_MAX_PHY_CHN_NUM){
            s32Ret = TS_MPI_VI_DisableChn(ViPipe, ViChn);

            if(s32Ret == TS_ERR_VI_NOT_PERM)
                s32Ret = TS_SUCCESS;

            if (s32Ret != TS_SUCCESS){
                SAMPLE_PRT("TS_MPI_VI_DisableChn failed with %#x!\n", s32Ret);
                return TS_FAILURE;
            }
        }    
    }

    return s32Ret;
}

static TS_S32  SAMPLE_COMM_DIS_CreateSingleVi(SAMPLE_VI_INFO_S* pstViInfo, SAMPLE_DIS_CONFIG_S *stLdcInfo)
{
    TS_S32 s32Ret = TS_SUCCESS;

    s32Ret = SAMPLE_COMM_DIS_StartViPipe(pstViInfo);

    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_DIS_StartViPipe failed !\n");
        goto EXIT2;
    }
    SAMPLE_PRT("SAMPLE_COMM_DIS_StartViPipe success !\n");

	if(VI_OFFLINE_VPSS_OFFLINE == pstViInfo->stPipeInfo.enMastPipeMode
		|| VI_ONLINE_VPSS_OFFLINE == pstViInfo->stPipeInfo.enMastPipeMode){
	    s32Ret = SAMPLE_COMM_DIS_StartViChn(pstViInfo, stLdcInfo);

	    if (s32Ret != TS_SUCCESS)
	    {
	        SAMPLE_PRT("SAMPLE_COMM_DIS_StartViChn failed !\n");
	        goto EXIT2;
	    }
	    SAMPLE_PRT("SAMPLE_COMM_DIS_StartViChn success !\n");
	}



    return TS_SUCCESS;

EXIT2:
    SAMPLE_COMM_DIS_StopViPipe(pstViInfo);

    return s32Ret;
}

static TS_S32 SAMPLE_COMM_DIS_DestroySingleVi(SAMPLE_VI_INFO_S* pstViInfo)
{
    TS_S32 s32Ret = TS_SUCCESS;
	
    s32Ret = SAMPLE_COMM_DIS_StopViChn(pstViInfo);
    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_DIS_StopViChn failed !\n");
		return TS_FAILURE;
    }

    s32Ret = SAMPLE_COMM_DIS_StopViPipe(pstViInfo);
    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_DIS_StopViPipe failed !\n");
		return TS_FAILURE;
    }

    return TS_SUCCESS;
}



TS_S32 SAMPLE_COMM_DIS_CreateVi(SAMPLE_VI_CONFIG_S* pstViConfig, SAMPLE_DIS_CONFIG_S *stLdcInfo)
{
    TS_S32              i;
    TS_S32              s32Ret = TS_SUCCESS;
    SAMPLE_VI_INFO_S*   pstViInfo = TS_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("null ptr\n");
        return TS_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        pstViInfo = &pstViConfig->astViInfo[i];

		if(pstViInfo->stPipeInfo.aPipe >= 0 && pstViInfo->stPipeInfo.aPipe < VI_MAX_PIPE_NUM){
	        s32Ret = SAMPLE_COMM_DIS_CreateSingleVi(pstViInfo, stLdcInfo);

	        if (s32Ret != TS_SUCCESS)
	        {
	            SAMPLE_PRT("SAMPLE_COMM_DIS_CreateSingleVi failed !\n");
	            goto EXIT;
	        }
		}
    }

    return TS_SUCCESS;
EXIT:

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        pstViInfo = &pstViConfig->astViInfo[i];

        SAMPLE_COMM_DIS_DestroySingleVi(pstViInfo);
    }

    return s32Ret;
}

TS_S32 SAMPLE_COMM_DIS_DestroyVi(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    TS_S32            i;
    SAMPLE_VI_INFO_S* pstViInfo = TS_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("null ptr\n");
        return TS_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        pstViInfo = &pstViConfig->astViInfo[i];

        SAMPLE_COMM_DIS_DestroySingleVi(pstViInfo);
    }

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_DIS_StartIsp(SAMPLE_VI_INFO_S* pstViInfo)
{
    TS_S32              s32Ret = TS_SUCCESS;
    VI_PIPE             ViPipe = pstViInfo->stPipeInfo.aPipe;
    ISP_PUB_ATTR_S      stPubAttr;

    if (ViPipe >= 0  && ViPipe < VI_MAX_PIPE_NUM){
        stPubAttr.enWDRMode				= pstViInfo->stPipeInfo.enWdrMode;
		stPubAttr.enBayer				= pstViInfo->stPipeInfo.enBayer;
		stPubAttr.stWndRect.u32Width	= pstViInfo->stPipeInfo.width;
		stPubAttr.stWndRect.u32Height	= pstViInfo->stPipeInfo.height;
		stPubAttr.stWndRect.s32X		= 0;
		stPubAttr.stWndRect.s32Y		= 0;
        stPubAttr.stSnsSize.u32Width    = pstViInfo->stPipeInfo.width;
        stPubAttr.stSnsSize.u32Height	= pstViInfo->stPipeInfo.height;
		stPubAttr.f32FrameRate			= pstViInfo->stPipeInfo.frameRate;
        stPubAttr.bDynFpsSync = pstViInfo->stPipeInfo.bDynFpsSync;
        stPubAttr.stOnlineParam.bOnline = pstViInfo->stPipeInfo.bIspByFly;
        stPubAttr.stOnlineParam.u32VinoutHtotalMargin = 0;
        stPubAttr.stOnlineParam.u32VinoutVtotalMargin = 0;

        s32Ret = TS_MPI_ISP_MemInit(ViPipe);

        if (s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("Init Ext memory failed with %#x!\n", s32Ret);
            SAMPLE_COMM_ISP_Stop(ViPipe);
            return TS_FAILURE;
        }

        s32Ret = TS_MPI_ISP_SetPubAttr(ViPipe, &stPubAttr);

        if (s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("SetPubAttr failed with %#x!\n", s32Ret);
            SAMPLE_COMM_ISP_Stop(ViPipe);
            return TS_FAILURE;
        }

        s32Ret = TS_MPI_ISP_Init(ViPipe);

        if (s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("ISP Init failed with %#x!\n", s32Ret);
            SAMPLE_COMM_ISP_Stop(ViPipe);
            return TS_FAILURE;
        }

        s32Ret = SAMPLE_COMM_ISP_Run(ViPipe);

        if (s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("ISP Run failed with %#x!\n", s32Ret);
            SAMPLE_COMM_ISP_Stop(ViPipe);
            return TS_FAILURE;
        }
    }

    return s32Ret;
}

TS_S32 SAMPLE_COMM_DIS_StopIsp(SAMPLE_VI_INFO_S* pstViInfo)
{
    VI_PIPE ViPipe = pstViInfo->stPipeInfo.aPipe;

    if (ViPipe >= 0  && ViPipe < VI_MAX_PIPE_NUM)
    {
        SAMPLE_COMM_ISP_Stop(ViPipe);
    }

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_DIS_CreateIsp(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    TS_S32              i;
    TS_S32              s32Ret = TS_SUCCESS;

    SAMPLE_VI_INFO_S*   pstViInfo = TS_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("null ptr\n");
        return TS_FAILURE;
    }

	SAMPLE_PRT("working ViNum=%d\n", pstViConfig->s32WorkingViNum);
    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        pstViInfo = &pstViConfig->astViInfo[i];

		SAMPLE_PRT("is going to start isp of vipipe[%d]\n", pstViInfo->stPipeInfo.aPipe);

        s32Ret = SAMPLE_COMM_DIS_StartIsp(pstViInfo);

        if (s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_COMM_DIS_StartIsp failed !\n");
            return TS_FAILURE;
        }
		SAMPLE_PRT("SAMPLE_COMM_DIS_StartIsp of vipipe[%d] success !\n", pstViInfo->stPipeInfo.aPipe);
    }

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_DIS_DestroyIsp(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    TS_S32              i;
    TS_S32              s32Ret = TS_SUCCESS;
    SAMPLE_VI_INFO_S*   pstViInfo = TS_NULL;

    if (!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return TS_FAILURE;
    }

    for (i = 0; i < pstViConfig->s32WorkingViNum; i++)
    {
        pstViInfo = &pstViConfig->astViInfo[i];



        s32Ret = SAMPLE_COMM_DIS_StopIsp(pstViInfo);

        if (s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_COMM_DIS_StopIsp failed !\n");
            return TS_FAILURE;
        }
    }

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_DIS_StartVi(SAMPLE_VI_CONFIG_S* pstViConfig, SAMPLE_DIS_CONFIG_S *stLdcInfo)
{
    TS_S32 s32Ret = TS_SUCCESS;

    if (!pstViConfig)
    {
        SAMPLE_PRT("null ptr\n");
        return TS_FAILURE;
    }

    s32Ret = SAMPLE_COMM_DIS_SetParam(pstViConfig);
    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_DIS_SetParam failed!\n");
        return TS_FAILURE;
    }

    s32Ret = SAMPLE_COMM_DIS_CreateVi(pstViConfig, stLdcInfo);
    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_DIS_CreateVi failed!\n");
        return TS_FAILURE;
    }

    s32Ret = SAMPLE_COMM_DIS_CreateIsp(pstViConfig);
    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_COMM_DIS_DestroyVi(pstViConfig);
        SAMPLE_PRT("SAMPLE_COMM_DIS_CreateIsp failed!\n");
        return TS_FAILURE;
    }
	
    return s32Ret;
}

TS_S32 SAMPLE_COMM_DIS_StopVi(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    TS_S32 s32Ret = TS_SUCCESS;

    s32Ret = SAMPLE_COMM_DIS_DestroyIsp(pstViConfig);

    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_DIS_DestroyIsp failed !\n");
        return TS_FAILURE;
    }

	SAMPLE_PRT("SAMPLE_COMM_DIS_DestroyIsp success!\n");

    s32Ret = SAMPLE_COMM_DIS_DestroyVi(pstViConfig);
    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_COMM_DIS_DestroyVi failed !\n");
        return TS_FAILURE;
    }

	SAMPLE_PRT("SAMPLE_COMM_DIS_DestroyVi success!\n");

    return s32Ret;
}

TS_S32 SAMPLE_COMM_DIS_StartVpss(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S* pstVpssGrpAttr, VPSS_CHN_ATTR_S* pastVpssChnAttr, TS_S32 chnlNum, SAMPLE_DIS_CONFIG_S *stLdcInfo)
{
    VPSS_CHN VpssChn;
    TS_S32 s32Ret;
    TS_S32 j;

    s32Ret = TS_MPI_VPSS_CreateGrp(VpssGrp, pstVpssGrpAttr);

    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("TS_MPI_VPSS_CreateGrp(grp:%d) failed with %#x!\n", VpssGrp, s32Ret);
        return TS_FAILURE;
    }

	SAMPLE_PRT("vpss grp[%d] : chnlNum=%d\n", VpssGrp, chnlNum);

    for (j = 0; j < chnlNum; j++)
    {
		VpssChn = j;
		SAMPLE_PRT("aaaaaaaaaaa, vpss grp[%d], chnl[%d]\n", VpssGrp, VpssChn);
		s32Ret = TS_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &pastVpssChnAttr[VpssChn]);
		
		if (s32Ret != TS_SUCCESS)
		{
			SAMPLE_PRT("TS_MPI_VPSS_SetChnAttr(grp:%d, chn:%d) failed with %#x\n", VpssGrp, j , s32Ret);
			return TS_FAILURE;
		}
		SAMPLE_PRT("TS_MPI_VPSS_SetChnAttrr(grp:%d, chn:%d) success\n", VpssGrp, j);

		if(VpssChn < stLdcInfo->s32WorkingViNum)
		{
			TS_MPI_VPSS_SetChnLDCAttr(VpssGrp, VpssChn, &stLdcInfo->pstLdcInfo[VpssChn]);
		}
		s32Ret = TS_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
		
		if (s32Ret != TS_SUCCESS)
		{
			SAMPLE_PRT("TS_MPI_VPSS_EnableChn(grp:%d, chn:%d) success\n", VpssGrp, j);
			return TS_FAILURE;
		}
		SAMPLE_PRT("TS_MPI_VPSS_EnableChn(grp:%d, chn:%d) success\n", VpssGrp, j);
    }

	SAMPLE_PRT("is going to TS_MPI_VPSS_StartGrp [%d]\n", VpssGrp);
    s32Ret = TS_MPI_VPSS_StartGrp(VpssGrp);

    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("TS_MPI_VPSS_StartGrp failed with %#x\n", s32Ret);
        return TS_FAILURE;
    }

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_DIS_StopVpss(VPSS_GRP VpssGrp, TS_S32 chnlNum)
{
    TS_S32 j;
    TS_S32 s32Ret = TS_SUCCESS;
	GDC_CONFIG_S pstLdcInfo;

    for (j = 0; j < chnlNum; j++)
    {
		TS_MPI_VPSS_GetChnLDCAttr(VpssGrp, j, &pstLdcInfo);
		printf("pstLdcInfo.enable = %d\n", pstLdcInfo.enable);
		if(pstLdcInfo.enable == TS_TRUE)
		{
			memset((void *)&pstLdcInfo, 0, sizeof(pstLdcInfo));
			pstLdcInfo.cfgTypeE = TS_CFG_FILE_TYPE;
			pstLdcInfo.enable = TS_FALSE;
			TS_MPI_VPSS_SetChnLDCAttr(VpssGrp, j, &pstLdcInfo);
		}

		VPSS_CHN VpssChn = j;
		s32Ret = TS_MPI_VPSS_DisableChn(VpssGrp, VpssChn);
		
		if (s32Ret != TS_SUCCESS)
		{
			SAMPLE_PRT("failed with %#x!\n", s32Ret);
			return TS_FAILURE;
		}
		SAMPLE_PRT("TS_MPI_VPSS_DisableChn [%d:%d] success\n", VpssGrp, VpssChn);
    }

    s32Ret = TS_MPI_VPSS_StopGrp(VpssGrp);

    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return TS_FAILURE;
    }
	SAMPLE_PRT("TS_MPI_VPSS_StopGrp [%d] success\n", VpssGrp);

    s32Ret = TS_MPI_VPSS_DestroyGrp(VpssGrp);

    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return TS_FAILURE;
    }
	SAMPLE_PRT("TS_MPI_VPSS_DestroyGrp [%d] success\n", VpssGrp);

    return TS_SUCCESS;
}

