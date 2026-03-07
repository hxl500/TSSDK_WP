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

#include "sample_comm_audio.h"
#include "sample_comm_sys.h"
#include "sample_comm_util.h"
#include "mpi_log.h"

#ifdef VQE_WEBRTC_NN
#include "fullbank_gru_8k_cfg.h"
#include "fullbank_gru_8k_weight.h"
#endif /* VQE_WEBRTC_NN */

static TS_BOOL gs_bUserGetMode  = TS_FALSE;
extern char g_ip[20];

#if 1
PAYLOAD_TYPE_E gs_enPayloadType = PT_G711A;
#else
#ifdef AAC_CODEC
static PAYLOAD_TYPE_E gs_enPayloadType = PT_AAC;
#elif OPUS_CODEC
static PAYLOAD_TYPE_E gs_enPayloadType = PT_OPUS;
#elif G726_CODEC
static PAYLOAD_TYPE_E gs_enPayloadType = PT_G726;
#elif ADPCM_CODEC
static PAYLOAD_TYPE_E gs_enPayloadType = PT_ADPCMA;
#elif G711_CODEC
static PAYLOAD_TYPE_E gs_enPayloadType = PT_G711A;
#else
static PAYLOAD_TYPE_E gs_enPayloadType = PT_G711A;
#endif
#endif

AUDIO_SAMPLE_RATE_E g_enSampleRate = AUDIO_SAMPLE_RATE_16000;

/* 0: close, 1: talk*/
static TS_U32 u32AiVqeType = 1;


/******************************************************************************
* function : PT Number to String
******************************************************************************/
static char *SAMPLE_AUDIO_Pt2Str(PAYLOAD_TYPE_E enType)
{
	if (PT_G711A == enType) {
		return "g711a";
	} else if (PT_G711U == enType) {
		return "g711u";
	} else if (PT_ADPCMA == enType) {
		return "adpcm";
	} else if (PT_G726 == enType) {
		return "g726";
	} else if (PT_LPCM == enType) {
		return "pcm";
	} else if (PT_AAC == enType) {
		return "aac";
	} else if (PT_MP3 == enType) {
		return "mp3";
	} else if (PT_OPUS == enType) {
		return "opus";
	} else {
		return "data";
	}
}

/******************************************************************************
* function : Open Aenc File
******************************************************************************/
static FILE *SAMPLE_AUDIO_OpenAencFile(AENC_CHN AeChn, PAYLOAD_TYPE_E enType)
{
	FILE *pfd;
	TS_CHAR aszFileName[FILE_NAME_LEN] = { 0 };

	/* create file for save stream*/
	snprintf(aszFileName, FILE_NAME_LEN, "audio_chn%d.%s", AeChn, SAMPLE_AUDIO_Pt2Str(enType));

	pfd = fopen(aszFileName, "w+");
	if (NULL == pfd) {
		printf("%s: open file %s failed\n", __FUNCTION__, aszFileName);
		return NULL;
	}
	printf("open stream file:\"%s\" for aenc ok\n", aszFileName);
	return pfd;
}

/******************************************************************************
* function : Open Adec File
******************************************************************************/
static FILE *SAMPLE_AUDIO_OpenAdecFile(ADEC_CHN AdChn, PAYLOAD_TYPE_E enType)
{
	FILE *pfd;
	TS_CHAR aszFileName[FILE_NAME_LEN] = { 0 };

	/* create file for save stream*/
	snprintf(aszFileName, FILE_NAME_LEN, "audio_chn%d.%s", AdChn, SAMPLE_AUDIO_Pt2Str(enType));

	pfd = fopen(aszFileName, "rb");
	if (NULL == pfd) {
		printf("%s: open file %s failed\n", __FUNCTION__, aszFileName);
		return NULL;
	}
	printf("open stream file:\"%s\" for adec ok\n", aszFileName);
	return pfd;
}

/******************************************************************************
 * function : show usage
 ******************************************************************************/
void SAMPLE_AUDIO_Usage(char *sPrgNm)
{
	printf("usage: %s <index>\n", sPrgNm);
	printf("usage: %s <1|2> [g711a|g711u|aac|opus|g726|adpcm|pcm] [8000|16000] [192.168.1.20]\n", sPrgNm);
	printf("index:\n");
	printf("\t 0)ai ao\n"
	       "\t 1)ai aenc\n"
	       "\t 2)adec ao\n"
	       "\t 3)ai aenc vpe adec ao\n"
	       "\t 4)ai vqe aenc up stream;  down strem adec vqe ao\n"
	       "Example: sample_audio 0\n");
}

/******************************************************************************
 * function : to process abnormal case
 ******************************************************************************/
void SAMPLE_AUDIO_HandleSig(TS_S32 signo)
{
	if (SIGINT == signo || SIGTERM == signo) {
		//SAMPLE_COMM_SYS_Exit();
		printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
	}
	exit(-1);
}

TS_S32 SAMPLE_AUDIO_SYS_Init(void)
{
	TS_S32 s32Ret;
	VB_CONFIG_S stVbConf;

	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);

	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("SAMPLE_COMM_SYS_GetPicSize failed!\n");
		return s32Ret;
	}

	return TS_SUCCESS;
}

/******************************************************************************
* function : Ai -> Ao
******************************************************************************/
TS_S32 SAMPLE_AUDIO_AIAO(void)
{
	TS_S32 s32Ret = TS_SUCCESS;
	AI_CHN AiChn = 0;
	AO_CHN AoChn = 0;
	TS_BOOL bEnableAiChn = TS_FALSE;

	if (gs_bUserGetMode)
		bEnableAiChn = TS_TRUE;

	s32Ret = SAMPLE_AUDIO_SYS_Init();
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("system init failed with %d!\n", s32Ret);
		return s32Ret;
	}

	AIO_ATTR_S stAioAttr = { 0 };
	stAioAttr.u8CardNum = 0;
	stAioAttr.aioMode = AIO_INNER_CODEC;
	stAioAttr.u32PtNumPerFrm = 320;
	stAioAttr.u8DeviceNum = 0;
	stAioAttr.enSamplerate = g_enSampleRate;
	stAioAttr.enBitwidth = AUDIO_BIT_WIDTH_16;
	stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	stAioAttr.u32FrmNum = 8;
	stAioAttr.aecEnable = TS_TRUE; //TS_FALSE;//TS_TRUE;
	if (AUDIO_SAMPLE_RATE_32000 == g_enSampleRate || AUDIO_SAMPLE_RATE_48000 == g_enSampleRate) {
		stAioAttr.u32PtNumPerFrm = 640;
	} else {
		stAioAttr.u32PtNumPerFrm = 320;
	}

	TS_S32 s32VqeFrmLen = 160;
#ifdef VQE_NN_MONO_16K
	stAioAttr.aioMode = AIO_INNER_CODEC;
	stAioAttr.u8CardNum = 0;
	stAioAttr.enSamplerate = AUDIO_SAMPLE_RATE_16000;
	stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	stAioAttr.u32PtNumPerFrm = 256;
	s32VqeFrmLen = 256;
#elif VQE_NN_STEREO_16K
	stAioAttr.aioMode = AIO_PDM_DUAL;
	stAioAttr.u8CardNum = 3;
	stAioAttr.enSamplerate = AUDIO_SAMPLE_RATE_16000;
	stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	stAioAttr.u32PtNumPerFrm = 256;
	s32VqeFrmLen = 256;
#elif VQE_NN_MONO_8K
	stAioAttr.u8CardNum = 0;
	stAioAttr.aioMode = AIO_INNER_CODEC;
	stAioAttr.enSamplerate = AUDIO_SAMPLE_RATE_8000;
	stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	stAioAttr.u32PtNumPerFrm = 256;
	s32VqeFrmLen = 128;
#elif VQE_NN_STEREO_8K
	stAioAttr.u8CardNum = 1;
	stAioAttr.aioMode = AIO_EXTERN_CODEC_DUAL;
	stAioAttr.enSamplerate = AUDIO_SAMPLE_RATE_8000;
	stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	stAioAttr.u32PtNumPerFrm = 256;
	s32VqeFrmLen = 128;
#endif

	AUDIO_VQE_CONFIG_S stVqeConfig;
	memset(&stVqeConfig, 0, sizeof(AUDIO_VQE_CONFIG_S));
#ifdef VQE_WEBRTC
	if (stAioAttr.enSamplerate == AUDIO_SAMPLE_RATE_8000)
		s32VqeFrmLen = 80;
	else if (stAioAttr.enSamplerate == AUDIO_SAMPLE_RATE_16000)
		s32VqeFrmLen = 160;
	ALG_VQE_SOL_WEBRTC3A_CONF_PARAM sol_conf;
	memset(&sol_conf, 0, sizeof(ALG_VQE_SOL_WEBRTC3A_CONF_PARAM));
	sol_conf.u32OpenMask = 0 | ALG_VQE_SOL_WEBRTC_AEC_MASK | ALG_VQE_SOL_WEBRTC_AGC_MASK |
			       ALG_VQE_SOL_WEBRTC_ANR_MASK | ALG_VQE_MMSENS_MASK | ALG_VQE_NOISEGATE_MASK;
	sol_conf.u32SmplRate = stAioAttr.enSamplerate;
	sol_conf.u16FrameLen = s32VqeFrmLen;
	sol_conf.u8ChanNum = 1;
	sol_conf.s16AgcMode = 3;
	sol_conf.s32NoiseSuppress = -15;
	sol_conf.fAttackTime = 0.005f;
	sol_conf.fReleaseTime = 0.005f;
	sol_conf.s32Threshold = -65;
	sol_conf.solVqeProc.s16MsInSndCardBuf = 5; // ms
	sol_conf.solVqeProc.s32Skew = 0;
	sol_conf.solVqeSetParam.algAecParams.s16NlpMode = 2;
	sol_conf.solVqeSetParam.algAecParams.s16SkewMode = 0;
	sol_conf.solVqeSetParam.algAecParams.s16MetricsMode = 0;
	sol_conf.solVqeSetParam.algAecParams.s32DelayLogging = 0;
	sol_conf.solVqeSetParam.algAnrParams.s32Mode = 2;
	sol_conf.solVqeSetParam.algAgcParams.s16TargetLevelDbfs = 3;
	sol_conf.solVqeSetParam.algAgcParams.s16CompressionGaindB = 9;
	sol_conf.solVqeSetParam.algAgcParams.u8LimiterEnable = 1;
	stVqeConfig.vqe_config = &sol_conf;
#elif VQE_NN
	SOL_VQE_NN_CONF_PARAM sol_conf;
	memset(&sol_conf, 0, sizeof(SOL_VQE_NN_CONF_PARAM));
	sol_conf.u32OpenMask = SOL_AGC_MASK | SOL_AEC_ANR_MASK;
	sol_conf.u32SmplRate = stAioAttr.enSamplerate;
	sol_conf.u16FrameLen = s32VqeFrmLen;
	if (stAioAttr.aioMode == AIO_INNER_CODEC_DUAL || stAioAttr.aioMode == AIO_EXTERN_CODEC_DUAL ||
	    stAioAttr.aioMode == AIO_PDM_DUAL )
		sol_conf.u8ChanNum = 2;
	else
		sol_conf.u8ChanNum = 1;
	sol_conf.solVqeInit.algAgcInit.s16AgcMode = 3;
	sol_conf.solVqeInit.algAgcInit.u32Fs = stAioAttr.enSamplerate;
	sol_conf.solVqeInit.algAgcInit.s16FrameLen = s32VqeFrmLen;
	sol_conf.solVqeInit.algAgcParam.s16TargetLevelDbfs = 3;
	sol_conf.solVqeInit.algAgcParam.s16CompressionGaindB = 20;
	sol_conf.solVqeInit.algAgcParam.u8LimiterEnable = 1;

	sol_conf.solVqeInit.algAnrAecInit.pGraph[0] = up_fullbank_cfg;
	sol_conf.solVqeInit.algAnrAecInit.pWeight[0] = up_fullbank_weight;
	sol_conf.solVqeInit.algAnrAecInit.u32GraphSize[0] = sizeof(up_fullbank_cfg);
	sol_conf.solVqeInit.algAnrAecInit.u32WeightSize[0] = sizeof(up_fullbank_weight);
	sol_conf.solVqeInit.algAnrAecInit.vpSelfBuf[0] = NULL;
	sol_conf.solVqeInit.algAnrAecInit.pGraph[1] = up_subbank_cfg;
	sol_conf.solVqeInit.algAnrAecInit.pWeight[1] = up_subbank_weight;
	sol_conf.solVqeInit.algAnrAecInit.u32GraphSize[1] = sizeof(up_subbank_cfg);
	sol_conf.solVqeInit.algAnrAecInit.u32WeightSize[1] = sizeof(up_subbank_weight);
	sol_conf.solVqeInit.algAnrAecInit.vpSelfBuf[1] = NULL;
	sol_conf.solVqeInit.algAnrAecInit.bRneOff = 0;
	sol_conf.solVqeInit.algAnrAecInit.s32SmplRatio = sol_conf.u32SmplRate;
	sol_conf.solVqeInit.algAnrAecInit.s16ChanNum = sol_conf.u8ChanNum;
	sol_conf.solVqeInit.algAnrAecInit.s16FrameLen = sol_conf.u16FrameLen;
	sol_conf.solVqeInit.algAnrAecInit.aecAlignMode = SKEW_NONE;

	sol_conf.solVqeProc.algAgcProcParam.s16Samples = s32VqeFrmLen;
	// sol_conf.solVqeProc.algAgcProcParam.s32InGainLevel = 8;
	// sol_conf.solVqeProc.algAgcProcParam.s32LimiterOnly = 0;

	sol_conf.solVqeProc.algAnrAecProcParam.s16MsInSndCardBuf = 0; // ms
#ifdef VQE_STEREO_MODEL
	SOL_VQE_NN_SET_PARAM_S mSolVqeSetParam;
	memset(&mSolVqeSetParam, 0, sizeof(SOL_VQE_NN_SET_PARAM_S));
	sol_conf.solVqeSetParam = &mSolVqeSetParam;
	TS_SOL_VQE_NN_GetParam(NULL, sol_conf.solVqeSetParam);
	SAMPLE_PRT("[0]:%d, [0]:%f, [0]:%d, [0]:%d\n", sol_conf.solVqeSetParam->algAnrAecSetParam.ps16HistNum,
		sol_conf.solVqeSetParam->algAnrAecSetParam.f32MaxCohxThreshold,
		sol_conf.solVqeSetParam->algAnrAecSetParam.laecMode,
		sol_conf.solVqeSetParam->algAnrAecSetParam.stereoAnrMode);

	sol_conf.solVqeSetParam->algAnrAecSetParam.stereoAnrMode = 0; // 0;NN;1:MVDR
#endif
	stVqeConfig.vqe_config = &sol_conf;
#endif

#if defined(VQE_NN_STEREO_16K) || defined(VQE_NN_STEREO_8K)
	stVqeConfig.in_len = s32VqeFrmLen * 2 * 2;
#else
	stVqeConfig.in_len = s32VqeFrmLen * 2;
#endif
	stVqeConfig.in_cnt = s32VqeFrmLen;
	stVqeConfig.out_len = s32VqeFrmLen * 2;
	stVqeConfig.out_cnt = s32VqeFrmLen;

	LOG_LEVEL_CONF_S stLogConf;
	stLogConf.enModId = TS_ID_AIO;
	stLogConf.s32Level = 7; // 7:DEBUG
	//stLogConf.cModName = MPP_MOD_AIO;
	TS_MPI_LOG_SetLevelConf(&stLogConf);

	/********************************************
	  step 1: start Ai
	********************************************/
	AUDIO_DEV AiDev = SAMPLE_AUDIO_INNER_AI_DEV;
	TS_S32 s32AiChnCnt = 1;
	s32Ret = SAMPLE_COMM_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, bEnableAiChn, TS_FALSE,
					   &stVqeConfig, u32AiVqeType);
	//s32Ret = SAMPLE_COMM_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, TS_FALSE, TS_FALSE, NULL, 0);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("SAMPLE_COMM_AUDIO_StartAi failed with %d!\n", s32Ret);
		return s32Ret;
	}

	/********************************************
	  step 2: start Ao
	********************************************/
	AUDIO_DEV AoDev = SAMPLE_AUDIO_INNER_AO_DEV;
	TS_S32 s32AoChnCnt = 1;
	stAioAttr.u8CardNum = 0;
	stAioAttr.aioMode = AIO_INNER_CODEC;
	s32Ret = SAMPLE_COMM_AUDIO_StartAo(AoDev, s32AoChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, TS_FALSE, TS_NULL,
					   TS_FALSE);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("SAMPLE_COMM_AUDIO_StartAo failed with %d!\n", s32Ret);
		return s32Ret;
	}

	/********************************************
	  step 3: ai bind ao
	********************************************/
	if (TS_TRUE == gs_bUserGetMode) {
		s32Ret = SAMPLE_COMM_AUDIO_CreatTrdAiAo(AiDev, AiChn, AoDev, AoChn);
		if (s32Ret != TS_SUCCESS) {
			SAMPLE_PRT("SAMPLE_COMM_AUDIO_CreatTrdAiAo failed with %d", s32Ret);
			return s32Ret;
		}
	} else {
		s32Ret = SAMPLE_COMM_AI_Bind_AO(AiDev, AoDev);
		if (s32Ret != TS_SUCCESS) {
			SAMPLE_PRT("SAMPLE_COMM_AI_Bind_AO failed with %d", s32Ret);
			return s32Ret;
		}
	}

	char c = 0;
	while (1) {
		printf("---------------Press enter 'q' to exit; ---------------\n");
		c = getchar();
		if (c == 'q')
			break;
		sleep(3);
	}

	if (TS_TRUE == gs_bUserGetMode) {
		SAMPLE_COMM_AUDIO_StopTrdAiAo(AiDev, AiChn, AoDev, AoChn);
		printf("---------------SAMPLE_COMM_AUDIO_StopTrdAiAo SUCCESS---------------\n");
	} else {
		SAMPLE_COMM_AI_UnBind_AO(AiChn, AoChn);
		printf("---------------SAMPLE_COMM_AUDIO_AoUnbindAi SUCCESS---------------\n");
	}

	SAMPLE_COMM_AUDIO_StopAi(AiDev, s32AiChnCnt, TS_FALSE, TS_FALSE, TS_FALSE);
	printf("---------------SAMPLE_COMM_AUDIO_StopAi SUCCESS---------------\n");

	SAMPLE_COMM_AUDIO_StopAo(AoDev, s32AoChnCnt, TS_FALSE);
	printf("---------------SAMPLE_COMM_AUDIO_StopAo SUCCESS---------------\n");

	SAMPLE_COMM_SYS_Exit();

	printf("---------------deinit SUCCESS---------------\n");

	return TS_SUCCESS;
}

/******************************************************************************
* function : Ai -> Aenc -> file
******************************************************************************/
TS_S32 SAMPLE_AUDIO_AIAENC(void)
{
	TS_S32 s32Ret = TS_SUCCESS;
	FILE *pfd = NULL;
	AI_CHN      AiChn = 0;
	AO_CHN      AoChn = 0;
	ADEC_CHN    AdChn = 0;
	AENC_CHN    AeChn = 0;
	AUDIO_DEV AiDev = SAMPLE_AUDIO_INNER_AI_DEV;
	AUDIO_DEV AoDev = SAMPLE_AUDIO_INNER_AO_DEV;
	TS_S32 s32AiChnCnt = 1;
	TS_BOOL bEnableAiChn = TS_FALSE;

	if (gs_bUserGetMode)
		bEnableAiChn = TS_TRUE;

	s32Ret = SAMPLE_AUDIO_SYS_Init();
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("system init failed with %d!\n", s32Ret);
		return s32Ret;
	}

	AIO_ATTR_S stAioAttr = { 0 };

	stAioAttr.u8CardNum = 0;
	stAioAttr.aioMode = AIO_INNER_CODEC;
	stAioAttr.u8DeviceNum = 0;
	stAioAttr.enSamplerate = g_enSampleRate;
	stAioAttr.enBitwidth = AUDIO_BIT_WIDTH_16;
	stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	stAioAttr.u32FrmNum = 4;
	stAioAttr.u32PtNumPerFrm = 320;
	stAioAttr.aecEnable = TS_FALSE;

#ifdef VQE_NN_MONO_16K
	stAioAttr.aioMode = AIO_INNER_CODEC;
	stAioAttr.u8CardNum = 0;
#elif VQE_NN_STEREO_16K
	stAioAttr.aioMode = AIO_PDM_DUAL;
	stAioAttr.u8CardNum = 3;
#elif VQE_NN_MONO_8K
	stAioAttr.u8CardNum = 0;
	stAioAttr.aioMode = AIO_INNER_CODEC;
#elif VQE_NN_STEREO_8K
	stAioAttr.u8CardNum = 1;
	stAioAttr.aioMode = AIO_EXTERN_CODEC_DUAL;
#endif

	/********************************************
      step 1: start Ai
    ********************************************/
	s32Ret = SAMPLE_COMM_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, bEnableAiChn, TS_FALSE,
					   NULL, 0);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("SAMPLE_COMM_AUDIO_StartAi failed with %d!\n", s32Ret);
		return s32Ret;
	}

	/********************************************
      step 2: start Aenc
    ********************************************/
	TS_S32 s32AencChnCnt = s32AiChnCnt >> stAioAttr.enSoundmode;
	s32Ret = SAMPLE_COMM_AUDIO_StartAencExtern(s32AencChnCnt, &stAioAttr, gs_enPayloadType);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("SAMPLE_COMM_AUDIO_StartAenc failed with %d!\n", s32Ret);
		return s32Ret;
	}

	pfd = SAMPLE_AUDIO_OpenAencFile(AeChn, gs_enPayloadType);
	if (!pfd) {
		SAMPLE_PRT("%d\n", TS_FAILURE);
		return TS_FAILURE;
	}

	s32Ret = SAMPLE_COMM_AUDIO_CreatTrdAencAdec(AeChn, AdChn, pfd);
	if (s32Ret != TS_SUCCESS) {
		SAMPLE_PRT("%d\n", s32Ret);
		return TS_FAILURE;
	}

    /********************************************
      step 3: Ai bind Aenc Chn
    ********************************************/
	if (TS_TRUE == gs_bUserGetMode) {
        s32Ret = SAMPLE_COMM_AUDIO_CreatTrdAiAenc(AiDev, AiChn, AeChn);
		if (s32Ret != TS_SUCCESS) {
			SAMPLE_PRT("%d\n", s32Ret);
			return TS_FAILURE;
		}
	} else {
		SAMPLE_COMM_AI_Bind_AENC(AiChn, AeChn);
	}

	char c = 0;
	while (1) {
		printf("---------------Press enter 'q' to exit; ---------------\n");
		c = getchar();
		if (c == 'q') {
			break;
		}
		sleep(3);
	}

	printf("---------------deinit ---------------\n");

	SAMPLE_COMM_AUDIO_StopTrdAencAdec(AeChn, AdChn, pfd);
	fclose(pfd);
	printf("---------------SAMPLE_COMM_AUDIO_StopTrdAencAdec SUCCESS---------------\n");

	if (TS_TRUE == gs_bUserGetMode) {
		SAMPLE_COMM_AUDIO_StopTrdAiAo(AiDev, AiChn, AoDev, AoChn);
		printf("---------------SAMPLE_COMM_AUDIO_StopTrdAiAo SUCCESS---------------\n");
	} else {
		SAMPLE_COMM_AI_UnBind_AENC(AiChn, AeChn);
		printf("---------------SAMPLE_COMM_AI_UnBind_AENC SUCCESS---------------\n");
	}

	SAMPLE_COMM_AUDIO_StopAenc(s32AencChnCnt);
	printf("---------------SAMPLE_COMM_AUDIO_StopAenc SUCCESS---------------\n");

	SAMPLE_COMM_AUDIO_StopAi(AiDev, s32AiChnCnt, TS_FALSE, TS_FALSE, TS_FALSE);
	printf("---------------SAMPLE_COMM_AUDIO_StopAi SUCCESS---------------\n");

	SAMPLE_COMM_SYS_Exit();

	printf("---------------deinit SUCCESS---------------\n");

	return TS_SUCCESS;
}

/******************************************************************************
* function : file -> Adec -> Ao
******************************************************************************/
TS_S32 SAMPLE_AUDIO_ADECAO(void)
{
	TS_S32 s32Ret = TS_SUCCESS;
	AO_CHN AoChn = 0;
	ADEC_CHN AdChn = 0;
	FILE *pfd = NULL;

	s32Ret = SAMPLE_AUDIO_SYS_Init();
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("system init failed with %d!\n", s32Ret);
		return s32Ret;
	}

	/********************************************
      step 1: start Adec
    ********************************************/
	s32Ret = SAMPLE_COMM_AUDIO_StartAdecExtern(AdChn, gs_enPayloadType);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_ADEC_CreateChn failed with %d!\n", s32Ret);
		return s32Ret;
	}

	AIO_ATTR_S stAioAttr = { 0 };
	stAioAttr.u8CardNum = 0;
	stAioAttr.aioMode = AIO_INNER_CODEC;
	stAioAttr.u8DeviceNum = 0;
	stAioAttr.enSamplerate = g_enSampleRate;
	stAioAttr.enBitwidth = AUDIO_BIT_WIDTH_16;
	stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	stAioAttr.u32FrmNum = 4;
	stAioAttr.aecEnable = TS_FALSE;
	if (AUDIO_SAMPLE_RATE_32000 == g_enSampleRate || AUDIO_SAMPLE_RATE_48000 == g_enSampleRate) {
		stAioAttr.u32PtNumPerFrm = 640;
	} else {
		stAioAttr.u32PtNumPerFrm = 320;
	}

	/********************************************
      step 2: start Ao
    ********************************************/
	AUDIO_DEV AoDev = SAMPLE_AUDIO_INNER_AO_DEV;
	TS_S32 s32AoChnCnt = 1;
	s32Ret = SAMPLE_COMM_AUDIO_StartAo(AoDev, s32AoChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, TS_FALSE, TS_NULL,
					   TS_FALSE);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("SAMPLE_COMM_AUDIO_StartAo failed with %d!\n", s32Ret);
		return s32Ret;
	}

	/********************************************
      step 3: Adec bind Ao
	********************************************/
	SAMPLE_COMM_ADEC_Bind_AO(AdChn, AoDev, AoChn);

	pfd = SAMPLE_AUDIO_OpenAdecFile(AdChn, gs_enPayloadType);
	if (!pfd) {
		SAMPLE_PRT("%d\n", TS_FAILURE);
		return TS_FAILURE;
	}
	s32Ret = SAMPLE_COMM_AUDIO_CreatTrdFileAdec(AdChn, pfd);
	if (s32Ret != TS_SUCCESS) {
		SAMPLE_PRT("%d\n", s32Ret);
		return TS_FAILURE;
	}

#if 0
	int cnt = 0;
	while (cnt++ < 30) {
		sleep(1);

		ADEC_CHN_STATE_S adec_chn_stat = { 0 };
		TS_MPI_ADEC_QueryChnStat(0, &adec_chn_stat);

		AO_CHN_STATE_S ao_chn_stat = { 0 };
		TS_MPI_AO_QueryChnStat(AoDev, 0, &ao_chn_stat);
	}
#endif

	char c = 0;
	while (1) {
		printf("---------------Press enter 'q' to exit; ---------------\n");
		c = getchar();
		if (c == 'q') {
			break;
		}
		sleep(3);
	}

	printf("---------------deinit ---------------\n");

	SAMPLE_COMM_ADEC_UnBind_AO(AdChn, AoDev, AoChn);

	printf("---------------SAMPLE_COMM_ADEC_UnBind_AO SUCCESS---------------\n");

	SAMPLE_COMM_AUDIO_StopTrdFileAdec(AdChn, pfd);

	printf("---------------SAMPLE_COMM_AUDIO_StopTrdFileAdec SUCCESS---------------\n");

	SAMPLE_COMM_AUDIO_StopAo(AoDev, s32AoChnCnt, TS_FALSE);
	printf("---------------SAMPLE_COMM_AUDIO_StopAo SUCCESS---------------\n");
	SAMPLE_COMM_AUDIO_StopAdec(AdChn);
	printf("---------------SAMPLE_COMM_AUDIO_StopAdec SUCCESS---------------\n");

	SAMPLE_COMM_SYS_Exit();

	printf("---------------deinit SUCCESS---------------\n");

	return TS_SUCCESS;
}

/******************************************************************************
* function : Ai -> Aenc -> file -> Adec -> Ao (volume adjust)
******************************************************************************/
TS_S32 SAMPLE_AUDIO_AiVqeAencProcessAdecAo(void)
{
	TS_S32 s32Ret = TS_SUCCESS;
	AI_CHN      AiChn = 0;
	AO_CHN      AoChn = 0;
	ADEC_CHN    AdChn = 0;
	AENC_CHN    AeChn = 0;
	FILE *pfd = NULL;

	s32Ret = SAMPLE_AUDIO_SYS_Init();
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("system init failed with %d!\n", s32Ret);
		return s32Ret;
	}

	AIO_ATTR_S stAioAttr = { 0 };
	stAioAttr.u8DeviceNum = 0;
	stAioAttr.enBitwidth = AUDIO_BIT_WIDTH_16;
	stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	stAioAttr.u32FrmNum = 4;
	stAioAttr.aecEnable = TS_TRUE;

	TS_S32 s32VqeFrmLen = 80;
	AUDIO_VQE_CONFIG_S stVqeConfig;
	memset(&stVqeConfig, 0, sizeof(AUDIO_VQE_CONFIG_S));

	stAioAttr.enSamplerate = g_enSampleRate;

#ifdef VQE_WEBRTC
	stAioAttr.u8CardNum = 0;
	stAioAttr.aioMode = AIO_INNER_CODEC;
	if (stAioAttr.enSamplerate == AUDIO_SAMPLE_RATE_8000) {
		stAioAttr.u32PtNumPerFrm = 160;
		s32VqeFrmLen = 80;
	} else if (stAioAttr.enSamplerate == AUDIO_SAMPLE_RATE_16000) {
		stAioAttr.u32PtNumPerFrm = 320;
		s32VqeFrmLen = 160;
	} else 	if (AUDIO_SAMPLE_RATE_32000 == g_enSampleRate || AUDIO_SAMPLE_RATE_48000 == g_enSampleRate) {
		stAioAttr.u32PtNumPerFrm = 640;
	}

	ALG_VQE_SOL_WEBRTC3A_CONF_PARAM sol_conf;
	memset(&sol_conf, 0, sizeof(ALG_VQE_SOL_WEBRTC3A_CONF_PARAM));
	sol_conf.u32OpenMask =
		0 | ALG_VQE_SOL_WEBRTC_AEC_MASK | ALG_VQE_SOL_WEBRTC_AGC_MASK | ALG_VQE_SOL_WEBRTC_ANR_MASK
		| ALG_VQE_MMSENS_MASK
		| ALG_VQE_NOISEGATE_MASK;
		;
	sol_conf.u32SmplRate = stAioAttr.enSamplerate;
	sol_conf.u16FrameLen = s32VqeFrmLen;
	sol_conf.u8ChanNum = 1;
	sol_conf.s16AgcMode = 3;
	sol_conf.s32NoiseSuppress = -15;
	sol_conf.fAttackTime = 0.005f;
	sol_conf.fReleaseTime = 0.005f;
	sol_conf.s32Threshold = -65;
	sol_conf.solVqeProc.s16MsInSndCardBuf = 5; // ms
	sol_conf.solVqeProc.s32Skew = 0;
	sol_conf.solVqeSetParam.algAecParams.s16NlpMode = 2;
	sol_conf.solVqeSetParam.algAecParams.s16SkewMode = 0;
	sol_conf.solVqeSetParam.algAecParams.s16MetricsMode = 0;
	sol_conf.solVqeSetParam.algAecParams.s32DelayLogging = 0;
	sol_conf.solVqeSetParam.algAnrParams.s32Mode = 2;
	sol_conf.solVqeSetParam.algAgcParams.s16TargetLevelDbfs = 3;
	sol_conf.solVqeSetParam.algAgcParams.s16CompressionGaindB = 9;
	sol_conf.solVqeSetParam.algAgcParams.u8LimiterEnable = 1;
	stVqeConfig.vqe_config = &sol_conf;

#ifdef VQE_WEBRTC_NN
	if (stAioAttr.enSamplerate == AUDIO_SAMPLE_RATE_8000) {
		stAioAttr.u32PtNumPerFrm = 256;
		s32VqeFrmLen = 128;
	} else {
		SAMPLE_PRT("VQE_WEBRTC_NN support only 8K samping ratio, tAioAttr.enSamplerate:%d\n", stAioAttr.enSamplerate);
		return s32Ret;
	}
	sol_conf.u32OpenMask = 0
		| ALG_VQE_SOL_WEBRTC_AEC_MASK
		| ALG_VQE_SOL_WEBRTC_AGC_MASK
		| ALG_VQE_NNANR_MASK
		| ALG_VQE_NOISEGATE_MASK
		;
	sol_conf.u16FrameLen = s32VqeFrmLen;
	sol_conf.pGraph = fullbank_gru_8k_cfg;
	sol_conf.pWeight = fullbank_gru_8k_weight;
	sol_conf.u32GraphSize = sizeof(fullbank_gru_8k_cfg);
	sol_conf.vpSelfBuf = NULL;
#endif /* VQE_WEBRTC_NN */

#elif VQE_NN
	TS_S32 s32ChanNum = 1;
#ifdef VQE_NN_MONO_16K
	stAioAttr.u8CardNum = 0;
	stAioAttr.aioMode = AIO_INNER_CODEC;
	stAioAttr.enSamplerate = AUDIO_SAMPLE_RATE_16000;
	stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	stAioAttr.u32PtNumPerFrm = 256;
	s32VqeFrmLen = 256;
	s32ChanNum = 1;
#elif VQE_NN_STEREO_16K
	stAioAttr.aioMode = AIO_PDM_DUAL;
	stAioAttr.u8CardNum = 3;
	stAioAttr.enSamplerate = AUDIO_SAMPLE_RATE_16000;
	stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	stAioAttr.u32PtNumPerFrm = 256;
	s32VqeFrmLen = 256;
	s32ChanNum = 2;
#elif VQE_NN_MONO_8K
	stAioAttr.u8CardNum = 0;
	stAioAttr.aioMode = AIO_INNER_CODEC;
	stAioAttr.enSamplerate = AUDIO_SAMPLE_RATE_8000;
	stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	stAioAttr.u32PtNumPerFrm = 256;
	s32VqeFrmLen = 128;
	s32ChanNum = 1;
#elif VQE_NN_STEREO_8K
	stAioAttr.u8CardNum = 1;
	stAioAttr.aioMode = AIO_EXTERN_CODEC_DUAL;
	stAioAttr.enSamplerate = AUDIO_SAMPLE_RATE_8000;
	stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	stAioAttr.u32PtNumPerFrm = 256;
	s32VqeFrmLen = 128;
	s32ChanNum = 2;
#endif
	// #define SOL_AI_MASK				0x8
	// #define SOL_AO_MASK				0x10
	SOL_VQE_NN_CONF_PARAM sol_conf;
	memset(&sol_conf, 0, sizeof(SOL_VQE_NN_CONF_PARAM));

	sol_conf.u32OpenMask = 0
				| SOL_AEC_ANR_MASK
				| SOL_AGC_MASK
				;
	sol_conf.u32SmplRate = stAioAttr.enSamplerate;
	sol_conf.u16FrameLen = s32VqeFrmLen;
	sol_conf.u8ChanNum = s32ChanNum;
	sol_conf.solVqeInit.algAgcInit.s16AgcMode = 3;
	sol_conf.solVqeInit.algAgcInit.u32Fs = stAioAttr.enSamplerate;
	sol_conf.solVqeInit.algAgcInit.s16FrameLen = s32VqeFrmLen;
	sol_conf.solVqeInit.algAgcParam.s16TargetLevelDbfs = 3;
	sol_conf.solVqeInit.algAgcParam.s16CompressionGaindB = 20;
	sol_conf.solVqeInit.algAgcParam.u8LimiterEnable = 1;

	sol_conf.solVqeInit.algAnrAecInit.pGraph[0] = up_fullbank_cfg;
	sol_conf.solVqeInit.algAnrAecInit.pWeight[0] = up_fullbank_weight;
	sol_conf.solVqeInit.algAnrAecInit.u32GraphSize[0] = sizeof(up_fullbank_cfg);
	sol_conf.solVqeInit.algAnrAecInit.u32WeightSize[0] = sizeof(up_fullbank_weight);
	sol_conf.solVqeInit.algAnrAecInit.vpSelfBuf[0] = NULL;
	sol_conf.solVqeInit.algAnrAecInit.pGraph[1] = up_subbank_cfg;
	sol_conf.solVqeInit.algAnrAecInit.pWeight[1] = up_subbank_weight;
	sol_conf.solVqeInit.algAnrAecInit.u32GraphSize[1] = sizeof(up_subbank_cfg);
	sol_conf.solVqeInit.algAnrAecInit.u32WeightSize[1] = sizeof(up_subbank_weight);
	sol_conf.solVqeInit.algAnrAecInit.vpSelfBuf[1] = NULL;
	sol_conf.solVqeInit.algAnrAecInit.bRneOff = 0;
	sol_conf.solVqeInit.algAnrAecInit.s32SmplRatio = sol_conf.u32SmplRate;
	sol_conf.solVqeInit.algAnrAecInit.s16ChanNum = sol_conf.u8ChanNum;
	sol_conf.solVqeInit.algAnrAecInit.s16FrameLen = sol_conf.u16FrameLen;
	sol_conf.solVqeInit.algAnrAecInit.aecAlignMode = SKEW_NONE;

	sol_conf.solVqeProc.algAgcProcParam.s16Samples = s32VqeFrmLen;
	// sol_conf.solVqeProc.algAgcProcParam.s32InGainLevel = 8;
	// sol_conf.solVqeProc.algAgcProcParam.s32LimiterOnly = 0;

	sol_conf.solVqeProc.algAnrAecProcParam.s16MsInSndCardBuf = 0; // ms

#ifdef VQE_STEREO_MODEL
	SOL_VQE_NN_SET_PARAM_S mSolVqeSetParam;
	memset(&mSolVqeSetParam, 0, sizeof(SOL_VQE_NN_SET_PARAM_S));
	sol_conf.solVqeSetParam = &mSolVqeSetParam;
	TS_SOL_VQE_NN_GetParam(NULL, sol_conf.solVqeSetParam);
	SAMPLE_PRT("[0]:%d, [0]:%f, [0]:%d, [0]:%d\n", sol_conf.solVqeSetParam->algAnrAecSetParam.ps16HistNum,
		sol_conf.solVqeSetParam->algAnrAecSetParam.f32MaxCohxThreshold,
		sol_conf.solVqeSetParam->algAnrAecSetParam.laecMode,
		sol_conf.solVqeSetParam->algAnrAecSetParam.stereoAnrMode);
	sol_conf.solVqeSetParam->algAnrAecSetParam.stereoAnrMode = 0; // 0;NN;1:MVDR
#endif

	stVqeConfig.vqe_config = &sol_conf;
#endif /* VQE_WEBRTC / VQE_NN */

#if defined(VQE_NN_STEREO_16K) || defined(VQE_NN_STEREO_8K)
	stVqeConfig.in_len = s32VqeFrmLen * 2 * 2;
#else
	stVqeConfig.in_len = s32VqeFrmLen * 2;
#endif
	stVqeConfig.in_cnt = s32VqeFrmLen;
	stVqeConfig.out_len = s32VqeFrmLen * 2;
	stVqeConfig.out_cnt = s32VqeFrmLen;

	/********************************************
	  step 1: start Ai
	********************************************/
	AUDIO_DEV AiDev = SAMPLE_AUDIO_INNER_AI_DEV;
	TS_S32 s32AiChnCnt = 1;
	s32Ret = SAMPLE_COMM_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, TS_FALSE, TS_FALSE,
					   (void *)(&stVqeConfig), u32AiVqeType); //u32AiVqeType
	// s32Ret = SAMPLE_COMM_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, TS_FALSE, TS_FALSE, NULL, TS_FALSE);//u32AiVqeType
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("SAMPLE_COMM_AUDIO_StartAi failed with %d!\n", s32Ret);
		return s32Ret;
	}

	/********************************************
	  step 2: start Aenc
	********************************************/
	TS_S32 s32AencChnCnt = s32AiChnCnt >> stAioAttr.enSoundmode;
	s32Ret = SAMPLE_COMM_AUDIO_StartAencExtern(s32AencChnCnt, &stAioAttr, gs_enPayloadType);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("SAMPLE_COMM_AUDIO_StartAenc failed with %d!\n", s32Ret);
		return s32Ret;
	}

	pfd = SAMPLE_AUDIO_OpenAencFile(AeChn, gs_enPayloadType);
	if (!pfd) {
		SAMPLE_PRT("%d\n", TS_FAILURE);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_AUDIO_CreatTrdAencAdec(AeChn, AdChn, pfd);
	if (s32Ret != TS_SUCCESS) {
		SAMPLE_PRT("%d\n", s32Ret);
		return s32Ret;
	}

	/********************************************
	  step 3: Ai bind Aenc
	********************************************/
	SAMPLE_COMM_AI_Bind_AENC(AiChn, AeChn);

	char c = 0;
	while (1) {
		printf("---------------Press enter 'q' to adec->ao ---------------\n");
		c = getchar();
		if (c == 'q')
			break;
		sleep(3);
	}

	AUDIO_DEV AoDev = SAMPLE_AUDIO_INNER_AO_DEV;
	TS_S32 s32AoChnCnt = 1;
	stAioAttr.u8CardNum = 0;
	stAioAttr.aioMode = AIO_INNER_CODEC;

	memset(&stVqeConfig, 0, sizeof(AUDIO_VQE_CONFIG_S));
#ifdef VQE_WEBRTC
	if (stAioAttr.enSamplerate == AUDIO_SAMPLE_RATE_8000)
		s32VqeFrmLen = 80;
	else if (stAioAttr.enSamplerate == AUDIO_SAMPLE_RATE_16000)
		s32VqeFrmLen = 160;
	ALG_VQE_SOL_WEBRTC3A_CONF_PARAM sol_conf_ao;
	memset(&sol_conf_ao, 0, sizeof(ALG_VQE_SOL_WEBRTC3A_CONF_PARAM));
	sol_conf_ao.u32OpenMask = 0
				 | ALG_VQE_SOL_WEBRTC_AGC_MASK
				 | ALG_VQE_SOL_WEBRTC_ANR_MASK
				 ;
	sol_conf_ao.u32SmplRate = stAioAttr.enSamplerate;
	sol_conf_ao.u16FrameLen = s32VqeFrmLen;
	sol_conf_ao.u8ChanNum = 1;
	sol_conf_ao.s16AgcMode = 3;
	sol_conf_ao.solVqeSetParam.algAnrParams.s32Mode = 2;
	sol_conf_ao.solVqeSetParam.algAgcParams.s16TargetLevelDbfs = 3;
	sol_conf_ao.solVqeSetParam.algAgcParams.s16CompressionGaindB = 30;
	sol_conf_ao.solVqeSetParam.algAgcParams.u8LimiterEnable = 1;
	stVqeConfig.vqe_config = &sol_conf_ao;
	printf("sol_conf_ao.u32OpenMask:%d, sol_conf_ao.u32SmplRate:%d\n", sol_conf_ao.u32OpenMask,
	       sol_conf_ao.u32SmplRate);
#elif VQE_NN
	stAioAttr.u32PtNumPerFrm = 256;
	if (stAioAttr.enSamplerate == AUDIO_SAMPLE_RATE_8000)
		s32VqeFrmLen = 128;
	else if (stAioAttr.enSamplerate == AUDIO_SAMPLE_RATE_16000)
		s32VqeFrmLen = 256;
	else
		printf("stAioAttr.enSamplerate err:%d!!\n", stAioAttr.enSamplerate);

	SOL_VQE_NN_CONF_PARAM sol_conf_ao;
	printf("sizeof(SOL_VQE_NN_CONF_PARAM):%ld, sizeof(AUDIO_VQE_CONFIG_S):%ld\n", sizeof(SOL_VQE_NN_CONF_PARAM),
	       sizeof(AUDIO_VQE_CONFIG_S));
	memset(&sol_conf_ao, 0, sizeof(SOL_VQE_NN_CONF_PARAM));

	sol_conf_ao.u32OpenMask = 0 | SOL_ANR_MASK | SOL_AGC_MASK;
	sol_conf_ao.u32SmplRate = stAioAttr.enSamplerate;
	printf("sol_conf_ao.u32OpenMask:%d, sol_conf_ao.u32SmplRate:%d\n", sol_conf_ao.u32OpenMask,
	       sol_conf_ao.u32SmplRate);
	sol_conf_ao.u16FrameLen = s32VqeFrmLen;
	sol_conf_ao.u8ChanNum = 1;
	sol_conf_ao.solVqeInit.algAgcInit.s16AgcMode = 3;
	sol_conf_ao.solVqeInit.algAgcInit.u32Fs = stAioAttr.enSamplerate;
	sol_conf_ao.solVqeInit.algAgcInit.s16FrameLen = s32VqeFrmLen;
	sol_conf_ao.solVqeInit.algAgcParam.s16TargetLevelDbfs = 3;
	sol_conf_ao.solVqeInit.algAgcParam.s16CompressionGaindB = 20;
	sol_conf_ao.solVqeInit.algAgcParam.u8LimiterEnable = 1;

	sol_conf_ao.solVqeInit.algAnrAecInit.pGraph[0] = dn_fullbank_cfg;
	sol_conf_ao.solVqeInit.algAnrAecInit.pWeight[0] = dn_fullbank_weight;
	sol_conf_ao.solVqeInit.algAnrAecInit.u32GraphSize[0] = sizeof(dn_fullbank_cfg);
	sol_conf_ao.solVqeInit.algAnrAecInit.u32WeightSize[0] = sizeof(dn_fullbank_weight);
	sol_conf_ao.solVqeInit.algAnrAecInit.vpSelfBuf[0] = NULL;
	sol_conf_ao.solVqeInit.algAnrAecInit.pGraph[1] = dn_subbank_cfg;
	sol_conf_ao.solVqeInit.algAnrAecInit.pWeight[1] = dn_subbank_weight;
	sol_conf_ao.solVqeInit.algAnrAecInit.u32GraphSize[1] = sizeof(dn_subbank_cfg);
	sol_conf_ao.solVqeInit.algAnrAecInit.u32WeightSize[1] = sizeof(dn_subbank_weight);
	sol_conf_ao.solVqeInit.algAnrAecInit.vpSelfBuf[1] = NULL;
	sol_conf_ao.solVqeInit.algAnrAecInit.bRneOff = 1;
	sol_conf_ao.solVqeInit.algAnrAecInit.s32SmplRatio = sol_conf_ao.u32SmplRate;
	sol_conf_ao.solVqeInit.algAnrAecInit.s16ChanNum = sol_conf_ao.u8ChanNum;
	sol_conf_ao.solVqeInit.algAnrAecInit.s16FrameLen = sol_conf_ao.u16FrameLen;

	sol_conf_ao.solVqeProc.algAgcProcParam.s16Samples = s32VqeFrmLen;
	// sol_conf_ao.solVqeProc.algAgcProcParam.s32InGainLevel = 8;
	// sol_conf_ao.solVqeProc.algAgcProcParam.s32LimiterOnly = 0;
	stVqeConfig.vqe_config = &sol_conf_ao;
	printf("sol_conf_ao.u32OpenMask:%d, sol_conf_ao.u32SmplRate:%d\n", sol_conf_ao.u32OpenMask,
	       sol_conf_ao.u32SmplRate);
#endif
	stVqeConfig.in_len = s32VqeFrmLen * 2;
	stVqeConfig.in_cnt = s32VqeFrmLen;
	stVqeConfig.out_len = s32VqeFrmLen * 2;
	stVqeConfig.out_cnt = s32VqeFrmLen;

	/********************************************
	step 4: start Ao
	********************************************/
	s32Ret = SAMPLE_COMM_AUDIO_StartAo(AoDev, s32AoChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, TS_FALSE,
					   &stVqeConfig, u32AiVqeType);
	// s32Ret = SAMPLE_COMM_AUDIO_StartAo(AoDev, s32AoChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, TS_FALSE, TS_NULL, TS_FALSE);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("SAMPLE_COMM_AUDIO_StartAo failed with %d!\n", s32Ret);
		return s32Ret;
	}

	pfd = SAMPLE_AUDIO_OpenAdecFile(AdChn, gs_enPayloadType);
	if (!pfd) {
		SAMPLE_PRT("%d\n", TS_FAILURE);
		return s32Ret;
	}

	/********************************************
	  step 5: start Adec
	********************************************/
	s32Ret = SAMPLE_COMM_AUDIO_StartAdecExtern(AdChn, gs_enPayloadType);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_ADEC_CreateChn failed with %d!\n", s32Ret);
		return s32Ret;
	}

	/********************************************
	  step 5: Adec bind Ao
	********************************************/
	SAMPLE_COMM_ADEC_Bind_AO(AdChn, AoDev, AoChn);

	s32Ret = SAMPLE_COMM_AUDIO_CreatTrdFileAdec(AdChn, pfd);
	if (s32Ret != TS_SUCCESS) {
		SAMPLE_PRT("%d\n", s32Ret);
		return s32Ret;
	}

	//ao volume test
	TS_S32 volume = -8;
	s32Ret = TS_MPI_AO_SetVolume(0, volume);

	//char c = 0;
	while (1) {
		printf("---------------Press enter 'q' to exit; ---------------\n");
		c = getchar();
		 if (c == 'q')
		{
			break;
		}

		sleep(3);
	}

	SAMPLE_PRT("---------------deinit ---------------\n");

	SAMPLE_COMM_ADEC_UnBind_AO(AdChn, AoDev, AoChn);
	SAMPLE_PRT("---------------SAMPLE_COMM_ADEC_UnBind_AO success---------------\n");

	SAMPLE_COMM_AUDIO_StopTrdFileAdec(AdChn, pfd);
	SAMPLE_PRT("---------------SAMPLE_COMM_AUDIO_StopTrdFileAdec success---------------\n");

	SAMPLE_COMM_AI_UnBind_AENC(AiChn, AeChn);
	SAMPLE_PRT("---------------SAMPLE_COMM_AI_UnBind_AENC success---------------\n");

	SAMPLE_COMM_AUDIO_StopAdec(AdChn);
	SAMPLE_PRT("---------------SAMPLE_COMM_AUDIO_StopAdec success---------------\n");

	SAMPLE_COMM_AUDIO_StopAo(AoDev, s32AoChnCnt, TS_FALSE);
	SAMPLE_PRT("---------------SAMPLE_COMM_AUDIO_StopAo success---------------\n");

	SAMPLE_COMM_AUDIO_StopAenc(s32AencChnCnt);
	SAMPLE_PRT("---------------SAMPLE_COMM_AUDIO_StopAenc success---------------\n");

	SAMPLE_COMM_AUDIO_StopAi(AiDev, s32AiChnCnt, TS_FALSE, TS_FALSE, TS_TRUE);
	SAMPLE_PRT("---------------SAMPLE_COMM_AUDIO_StopAi success---------------\n");

	SAMPLE_COMM_AUDIO_StopTrdAencAdec(AeChn, AdChn, pfd);
	SAMPLE_PRT("---------------SAMPLE_COMM_AUDIO_StopTrdAencAdec success---------------\n");

	SAMPLE_COMM_SYS_Exit();

	SAMPLE_PRT("---------------deinit success ---------------\n");

	return TS_SUCCESS;
}

/******************************************************************************
* function : Ai -> Aenc -> Up Stream; Down Stream -> Adec -> Ao
******************************************************************************/
TS_S32 SAMPLE_AUDIO_AiVqeAencAdecVqeAoStream(void)
{
	TS_S32 s32Ret = TS_SUCCESS;
	AI_CHN      AiChn = 0;
	AO_CHN      AoChn = 0;
	ADEC_CHN    AdChn = 0;
	AENC_CHN    AeChn = 0;

	s32Ret = SAMPLE_AUDIO_SYS_Init();
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("system init failed with %d!\n", s32Ret);
		return s32Ret;
	}

	AIO_ATTR_S stAioAttr = { 0 };
	stAioAttr.u8DeviceNum = 0;
	stAioAttr.enBitwidth = AUDIO_BIT_WIDTH_16;
	stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	stAioAttr.u32FrmNum = 4;
	stAioAttr.aecEnable = TS_TRUE;

	TS_S32 s32VqeFrmLen = 80;
	AUDIO_VQE_CONFIG_S stVqeConfig;
	memset(&stVqeConfig, 0, sizeof(AUDIO_VQE_CONFIG_S));

	stAioAttr.enSamplerate = g_enSampleRate;
#ifdef VQE_WEBRTC
	stAioAttr.u8CardNum = 0;
	stAioAttr.aioMode = AIO_INNER_CODEC;
	if (stAioAttr.enSamplerate == AUDIO_SAMPLE_RATE_8000) {
		stAioAttr.u32PtNumPerFrm = 160;
		s32VqeFrmLen = 80;
	} else if (stAioAttr.enSamplerate == AUDIO_SAMPLE_RATE_16000) {
		stAioAttr.u32PtNumPerFrm = 320;
		s32VqeFrmLen = 160;
	} else 	if (AUDIO_SAMPLE_RATE_32000 == g_enSampleRate || AUDIO_SAMPLE_RATE_48000 == g_enSampleRate) {
		stAioAttr.u32PtNumPerFrm = 640;
	}

	ALG_VQE_SOL_WEBRTC3A_CONF_PARAM sol_conf;
	memset(&sol_conf, 0, sizeof(ALG_VQE_SOL_WEBRTC3A_CONF_PARAM));
	sol_conf.u32OpenMask =
		0 | ALG_VQE_SOL_WEBRTC_AEC_MASK | ALG_VQE_SOL_WEBRTC_AGC_MASK | ALG_VQE_SOL_WEBRTC_ANR_MASK
		| ALG_VQE_MMSENS_MASK | ALG_VQE_NOISEGATE_MASK;
		;
	sol_conf.u32SmplRate = stAioAttr.enSamplerate;
	sol_conf.u16FrameLen = s32VqeFrmLen;
	sol_conf.u8ChanNum = 1;
	sol_conf.s16AgcMode = 3;
	sol_conf.s32NoiseSuppress = -15;
	sol_conf.fAttackTime = 0.005f;
	sol_conf.fReleaseTime = 0.005f;
	sol_conf.s32Threshold = -65;
	sol_conf.solVqeProc.s16MsInSndCardBuf = 5; // ms
	sol_conf.solVqeProc.s32Skew = 0;
	sol_conf.solVqeSetParam.algAecParams.s16NlpMode = 2;
	sol_conf.solVqeSetParam.algAecParams.s16SkewMode = 0;
	sol_conf.solVqeSetParam.algAecParams.s16MetricsMode = 0;
	sol_conf.solVqeSetParam.algAecParams.s32DelayLogging = 0;
	sol_conf.solVqeSetParam.algAnrParams.s32Mode = 2;
	sol_conf.solVqeSetParam.algAgcParams.s16TargetLevelDbfs = 3;
	sol_conf.solVqeSetParam.algAgcParams.s16CompressionGaindB = 9;
	sol_conf.solVqeSetParam.algAgcParams.u8LimiterEnable = 1;
	stVqeConfig.vqe_config = &sol_conf;

#elif VQE_NN
#ifdef VQE_NN_MONO_16K
	stAioAttr.u8CardNum = 0;
	stAioAttr.aioMode = AIO_INNER_CODEC;
	stAioAttr.enSamplerate = AUDIO_SAMPLE_RATE_16000;
	stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	stAioAttr.u32PtNumPerFrm = 256;
	s32VqeFrmLen = 256;
#elif VQE_NN_STEREO_16K
	stAioAttr.aioMode = AIO_PDM_DUAL;
	stAioAttr.u8CardNum = 3;
	stAioAttr.enSamplerate = AUDIO_SAMPLE_RATE_16000;
	stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	stAioAttr.u32PtNumPerFrm = 256;
	s32VqeFrmLen = 256;
#elif VQE_NN_MONO_8K
	stAioAttr.u8CardNum = 0;
	stAioAttr.aioMode = AIO_INNER_CODEC;
	stAioAttr.enSamplerate = AUDIO_SAMPLE_RATE_8000;
	stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	stAioAttr.u32PtNumPerFrm = 256;
	s32VqeFrmLen = 128;
#elif VQE_NN_STEREO_8K
	stAioAttr.u8CardNum = 1;
	stAioAttr.aioMode = AIO_EXTERN_CODEC_DUAL;
	stAioAttr.enSamplerate = AUDIO_SAMPLE_RATE_8000;
	stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	stAioAttr.u32PtNumPerFrm = 256;
	s32VqeFrmLen = 128;
#endif
	// #define SOL_AI_MASK				0x8
	// #define SOL_AO_MASK				0x10
	SOL_VQE_NN_CONF_PARAM sol_conf;
	memset(&sol_conf, 0, sizeof(SOL_VQE_NN_CONF_PARAM));

	sol_conf.u32OpenMask = 0 | SOL_AGC_MASK | SOL_AEC_ANR_MASK;
	sol_conf.u32SmplRate = stAioAttr.enSamplerate;
	sol_conf.u16FrameLen = s32VqeFrmLen;
	if (stAioAttr.aioMode == AIO_INNER_CODEC_DUAL || stAioAttr.aioMode == AIO_EXTERN_CODEC_DUAL ||
	    stAioAttr.aioMode == AIO_PDM_DUAL)
		sol_conf.u8ChanNum = 2;
	else
		sol_conf.u8ChanNum = 1;
	sol_conf.solVqeInit.algAgcInit.s16AgcMode = 3;
	sol_conf.solVqeInit.algAgcInit.u32Fs = stAioAttr.enSamplerate;
	sol_conf.solVqeInit.algAgcInit.s16FrameLen = s32VqeFrmLen;
	sol_conf.solVqeInit.algAgcParam.s16TargetLevelDbfs = 3;
	sol_conf.solVqeInit.algAgcParam.s16CompressionGaindB = 20;
	sol_conf.solVqeInit.algAgcParam.u8LimiterEnable = 1;

	sol_conf.solVqeInit.algAnrAecInit.pGraph[0] = up_fullbank_cfg;
	sol_conf.solVqeInit.algAnrAecInit.pWeight[0] = up_fullbank_weight;
	sol_conf.solVqeInit.algAnrAecInit.u32GraphSize[0] = sizeof(up_fullbank_cfg);
	sol_conf.solVqeInit.algAnrAecInit.u32WeightSize[0] = sizeof(up_fullbank_weight);
	sol_conf.solVqeInit.algAnrAecInit.vpSelfBuf[0] = NULL;
	sol_conf.solVqeInit.algAnrAecInit.pGraph[1] = up_subbank_cfg;
	sol_conf.solVqeInit.algAnrAecInit.pWeight[1] = up_subbank_weight;
	sol_conf.solVqeInit.algAnrAecInit.u32GraphSize[1] = sizeof(up_subbank_cfg);
	sol_conf.solVqeInit.algAnrAecInit.u32WeightSize[1] = sizeof(up_subbank_weight);
	sol_conf.solVqeInit.algAnrAecInit.vpSelfBuf[1] = NULL;
	sol_conf.solVqeInit.algAnrAecInit.bRneOff = 0;
	sol_conf.solVqeInit.algAnrAecInit.s32SmplRatio = sol_conf.u32SmplRate;
	sol_conf.solVqeInit.algAnrAecInit.s16ChanNum = sol_conf.u8ChanNum;
	sol_conf.solVqeInit.algAnrAecInit.s16FrameLen = sol_conf.u16FrameLen;
	sol_conf.solVqeInit.algAnrAecInit.aecAlignMode = SKEW_NONE;

	sol_conf.solVqeProc.algAgcProcParam.s16Samples = s32VqeFrmLen;
	// sol_conf.solVqeProc.algAgcProcParam.s32InGainLevel = 8;
	// sol_conf.solVqeProc.algAgcProcParam.s32LimiterOnly = 0;

	sol_conf.solVqeProc.algAnrAecProcParam.s16MsInSndCardBuf = 0; // ms

#ifdef VQE_STEREO_MODEL
	SOL_VQE_NN_SET_PARAM_S mSolVqeSetParam = { 0 };
	sol_conf.solVqeSetParam = &mSolVqeSetParam;
	if (sol_conf.solVqeSetParam == NULL) {
		SAMPLE_PRT("[%d]:sol_conf.solVqeSetParam calloc err!\n", __LINE__);
		return -1;
	}
	TS_SOL_VQE_NN_GetParam(NULL, sol_conf.solVqeSetParam);
	SAMPLE_PRT("[0]:%d, [0]:%f, [0]:%d, [0]:%d\n", sol_conf.solVqeSetParam->algAnrAecSetParam.ps16HistNum,
		sol_conf.solVqeSetParam->algAnrAecSetParam.f32MaxCohxThreshold,
		sol_conf.solVqeSetParam->algAnrAecSetParam.laecMode,
		sol_conf.solVqeSetParam->algAnrAecSetParam.stereoAnrMode);

	sol_conf.solVqeSetParam->algAnrAecSetParam.stereoAnrMode = 0; // 0;NN;1:MVDR
#endif /* VQE_STEREO_MODEL */

	stVqeConfig.vqe_config = &sol_conf;
#endif /* VQE_WEBRTC / VQE_NN */

#if defined(VQE_NN_STEREO_16K) || defined(VQE_NN_STEREO_8K)
	stVqeConfig.in_len = s32VqeFrmLen * 2 * 2;
#else
	stVqeConfig.in_len = s32VqeFrmLen * 2;
#endif
	stVqeConfig.in_cnt = s32VqeFrmLen;
	stVqeConfig.out_len = s32VqeFrmLen * 2;
	stVqeConfig.out_cnt = s32VqeFrmLen;

	AUDIO_DEV AiDev = SAMPLE_AUDIO_INNER_AI_DEV;
	TS_S32 s32AiChnCnt = 1;
	TS_S32 s32AencChnCnt = s32AiChnCnt >> stAioAttr.enSoundmode;

	if (PT_LPCM == gs_enPayloadType) {
		/********************************************
		step 1: start Ai
		********************************************/
		s32Ret = SAMPLE_COMM_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, TS_TRUE, TS_FALSE,
						(void *)(&stVqeConfig), u32AiVqeType); //u32AiVqeType
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("SAMPLE_COMM_AUDIO_StartAi failed with %d!\n", s32Ret);
			return s32Ret;
		}

		s32Ret =  SAMPLE_COMM_AUDIO_CreatTrdAiPushStream(AiDev, AiChn);
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("SAMPLE_COMM_AUDIO_CreatTrdAiPushStream failed with %d!\n", s32Ret);
			return s32Ret;
		}
	} else {
		/********************************************
		step 1: start Ai
		********************************************/
		s32Ret = SAMPLE_COMM_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, TS_FALSE, TS_FALSE,
						(void *)(&stVqeConfig), u32AiVqeType); //u32AiVqeType
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("SAMPLE_COMM_AUDIO_StartAi failed with %d!\n", s32Ret);
			return s32Ret;
		}
		/********************************************
		step 2: start Aenc
		********************************************/
		s32Ret = SAMPLE_COMM_AUDIO_StartAencExtern(s32AencChnCnt, &stAioAttr, gs_enPayloadType);
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("SAMPLE_COMM_AUDIO_StartAenc failed with %d!\n", s32Ret);
			return s32Ret;
		}

		/********************************************
		step 3: start Aenc Up Push Stream
		********************************************/
		s32Ret = SAMPLE_COMM_AUDIO_CreatTrdAencPushStream(AeChn);
		if (s32Ret != TS_SUCCESS) {
			SAMPLE_PRT("%d\n", s32Ret);
			return s32Ret;
		}

		/********************************************
		step 4: Ai bind Aenc
		********************************************/
		SAMPLE_COMM_AI_Bind_AENC(AiChn, AeChn);
	}

	AUDIO_DEV AoDev = SAMPLE_AUDIO_INNER_AO_DEV;
	TS_S32 s32AoChnCnt = 1;
	stAioAttr.u8CardNum = 0;
	stAioAttr.aioMode = AIO_INNER_CODEC;

	memset(&stVqeConfig, 0, sizeof(AUDIO_VQE_CONFIG_S));
#ifdef VQE_WEBRTC
	if (stAioAttr.enSamplerate == AUDIO_SAMPLE_RATE_8000)
		s32VqeFrmLen = 80;
	else if (stAioAttr.enSamplerate == AUDIO_SAMPLE_RATE_16000)
		s32VqeFrmLen = 160;
	ALG_VQE_SOL_WEBRTC3A_CONF_PARAM sol_conf_ao;
	memset(&sol_conf_ao, 0, sizeof(ALG_VQE_SOL_WEBRTC3A_CONF_PARAM));
	sol_conf_ao.u32OpenMask = 0 | ALG_VQE_SOL_WEBRTC_AGC_MASK | ALG_VQE_SOL_WEBRTC_ANR_MASK;
	sol_conf_ao.u32SmplRate = stAioAttr.enSamplerate;
	sol_conf_ao.u16FrameLen = s32VqeFrmLen;
	sol_conf_ao.u8ChanNum = 1;
	sol_conf_ao.s16AgcMode = 3;
	sol_conf_ao.solVqeSetParam.algAnrParams.s32Mode = 2;
	sol_conf_ao.solVqeSetParam.algAgcParams.s16TargetLevelDbfs = 3;
	sol_conf_ao.solVqeSetParam.algAgcParams.s16CompressionGaindB = 9;
	sol_conf_ao.solVqeSetParam.algAgcParams.u8LimiterEnable = 1;
	stVqeConfig.vqe_config = &sol_conf_ao;
	printf("sol_conf_ao.u32OpenMask:%d, sol_conf_ao.u32SmplRate:%d\n", sol_conf_ao.u32OpenMask,
	       sol_conf_ao.u32SmplRate);
#elif VQE_NN
	stAioAttr.u32PtNumPerFrm = 256;
	if (stAioAttr.enSamplerate == AUDIO_SAMPLE_RATE_8000)
		s32VqeFrmLen = 128;
	else if (stAioAttr.enSamplerate == AUDIO_SAMPLE_RATE_16000)
		s32VqeFrmLen = 256;
	else
		printf("stAioAttr.enSamplerate err:%d!!\n", stAioAttr.enSamplerate);

	SOL_VQE_NN_CONF_PARAM sol_conf_ao;
	printf("sizeof(SOL_VQE_NN_CONF_PARAM):%ld, sizeof(AUDIO_VQE_CONFIG_S):%ld\n", sizeof(SOL_VQE_NN_CONF_PARAM),
	       sizeof(AUDIO_VQE_CONFIG_S));
	memset(&sol_conf_ao, 0, sizeof(SOL_VQE_NN_CONF_PARAM));

	sol_conf_ao.u32OpenMask = 0 | SOL_ANR_MASK | SOL_AGC_MASK;
	sol_conf_ao.u32SmplRate = stAioAttr.enSamplerate;
	printf("sol_conf_ao.u32OpenMask:%d, sol_conf_ao.u32SmplRate:%d\n", sol_conf_ao.u32OpenMask,
	       sol_conf_ao.u32SmplRate);
	sol_conf_ao.u16FrameLen = s32VqeFrmLen;
	sol_conf_ao.u8ChanNum = 1;
	sol_conf_ao.solVqeInit.algAgcInit.s16AgcMode = 3;
	sol_conf_ao.solVqeInit.algAgcInit.u32Fs = stAioAttr.enSamplerate;
	sol_conf_ao.solVqeInit.algAgcInit.s16FrameLen = s32VqeFrmLen;

	sol_conf_ao.solVqeInit.algAnrAecInit.pGraph[0] = dn_fullbank_cfg;
	sol_conf_ao.solVqeInit.algAnrAecInit.pWeight[0] = dn_fullbank_weight;
	sol_conf_ao.solVqeInit.algAnrAecInit.u32GraphSize[0] = sizeof(dn_fullbank_cfg);
	sol_conf_ao.solVqeInit.algAnrAecInit.u32WeightSize[0] = sizeof(dn_fullbank_weight);
	sol_conf_ao.solVqeInit.algAnrAecInit.vpSelfBuf[0] = NULL;
	sol_conf_ao.solVqeInit.algAnrAecInit.pGraph[1] = dn_subbank_cfg;
	sol_conf_ao.solVqeInit.algAnrAecInit.pWeight[1] = dn_subbank_weight;
	sol_conf_ao.solVqeInit.algAnrAecInit.u32GraphSize[1] = sizeof(dn_subbank_cfg);
	sol_conf_ao.solVqeInit.algAnrAecInit.u32WeightSize[1] = sizeof(dn_subbank_weight);
	sol_conf_ao.solVqeInit.algAnrAecInit.vpSelfBuf[1] = NULL;
	sol_conf_ao.solVqeInit.algAnrAecInit.bRneOff = 1;
	sol_conf_ao.solVqeInit.algAnrAecInit.s32SmplRatio = sol_conf_ao.u32SmplRate;
	sol_conf_ao.solVqeInit.algAnrAecInit.s16ChanNum = sol_conf_ao.u8ChanNum;
	sol_conf_ao.solVqeInit.algAnrAecInit.s16FrameLen = sol_conf_ao.u16FrameLen;

	sol_conf_ao.solVqeProc.algAgcProcParam.s16Samples = s32VqeFrmLen;
	// sol_conf_ao.solVqeProc.algAgcProcParam.s32InGainLevel = 8;
	// sol_conf_ao.solVqeProc.algAgcProcParam.s32LimiterOnly = 0;
	stVqeConfig.vqe_config = &sol_conf_ao;
	printf("sol_conf_ao.u32OpenMask:%d, sol_conf_ao.u32SmplRate:%d\n", sol_conf_ao.u32OpenMask,
	       sol_conf_ao.u32SmplRate);
#endif
	stVqeConfig.in_len = s32VqeFrmLen * 2;
	stVqeConfig.in_cnt = s32VqeFrmLen;
	stVqeConfig.out_len = s32VqeFrmLen * 2;
	stVqeConfig.out_cnt = s32VqeFrmLen;

	/********************************************
	step 5: start Ao
	********************************************/
	s32Ret = SAMPLE_COMM_AUDIO_StartAo(AoDev, s32AoChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, TS_FALSE,
					   &stVqeConfig, u32AiVqeType);
	// s32Ret = SAMPLE_COMM_AUDIO_StartAo(AoDev, s32AoChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, TS_FALSE, TS_NULL, TS_FALSE);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("SAMPLE_COMM_AUDIO_StartAo failed with %d!\n", s32Ret);
		return s32Ret;
	}

	if (PT_LPCM == gs_enPayloadType) {
		s32Ret = SAMPLE_COMM_AUDIO_CreatTrdPullStreamAo(AoDev, AoChn);
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("SAMPLE_COMM_AUDIO_CreatTrdPullStreamAo failed with %d!\n", s32Ret);
			return s32Ret;
		}
	} else {
		/********************************************
		step 6: start Adec
		********************************************/
		s32Ret = SAMPLE_COMM_AUDIO_StartAdecExtern(AdChn, gs_enPayloadType);
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("TS_MPI_ADEC_CreateChn failed with %d!\n", s32Ret);
			return s32Ret;
		}

		/********************************************
		step 7: Adec bind Ao
		********************************************/
		SAMPLE_COMM_ADEC_Bind_AO(AdChn, AoDev, AoChn);

		/********************************************
		step 8: start Pull Stream to Adec
		********************************************/
		s32Ret = SAMPLE_COMM_AUDIO_CreatTrdPullStreamAdec(AdChn);
		if (s32Ret != TS_SUCCESS) {
			SAMPLE_PRT("%d\n", s32Ret);
			return s32Ret;
		}
	}

	//ao volume test
	TS_S32 volume = -8;
	s32Ret = TS_MPI_AO_SetVolume(0, volume);

	char c = 0;
	while (1) {
		printf("---------------Press enter 'q' to exit; ---------------\n");
		c = getchar();
		 if (c == 'q')
		{
			break;
		}

		sleep(3);
	}

	SAMPLE_PRT("---------------deinit ---------------\n");

	if (PT_LPCM == gs_enPayloadType) {
		SAMPLE_COMM_AUDIO_StopTrdAiAo(AiDev, AiChn, AoDev, AoChn);
		SAMPLE_PRT("---------------SAMPLE_COMM_AUDIO_StopTrdAiAo success---------------\n");
		SAMPLE_COMM_AUDIO_StopAi(AiDev, s32AiChnCnt, TS_FALSE, TS_FALSE, TS_TRUE);
		SAMPLE_PRT("---------------SAMPLE_COMM_AUDIO_StopAi success---------------\n");
		SAMPLE_COMM_AUDIO_StopTrdAo(AoDev, AoChn);
		SAMPLE_PRT("---------------SAMPLE_COMM_AUDIO_StopTrdAo success---------------\n");
		SAMPLE_COMM_AUDIO_StopAo(AoDev, s32AoChnCnt, TS_FALSE);
		SAMPLE_PRT("---------------SAMPLE_COMM_AUDIO_StopAo success---------------\n");
	} else {
		SAMPLE_COMM_ADEC_UnBind_AO(AdChn, AoDev, AoChn);
		SAMPLE_PRT("---------------SAMPLE_COMM_ADEC_UnBind_AO success---------------\n");

		SAMPLE_COMM_AUDIO_StopTrdFileAdec(AdChn, NULL);
		SAMPLE_PRT("---------------SAMPLE_COMM_AUDIO_StopTrdFileAdec success---------------\n");

		SAMPLE_COMM_AI_UnBind_AENC(AiChn, AeChn);
		SAMPLE_PRT("---------------SAMPLE_COMM_AI_UnBind_AENC success---------------\n");

		SAMPLE_COMM_AUDIO_StopAdec(AdChn);
		SAMPLE_PRT("---------------SAMPLE_COMM_AUDIO_StopAdec success---------------\n");

		SAMPLE_COMM_AUDIO_StopAo(AoDev, s32AoChnCnt, TS_FALSE);
		SAMPLE_PRT("---------------SAMPLE_COMM_AUDIO_StopAo success---------------\n");

		SAMPLE_COMM_AUDIO_StopAenc(s32AencChnCnt);
		SAMPLE_PRT("---------------SAMPLE_COMM_AUDIO_StopAenc success---------------\n");

		SAMPLE_COMM_AUDIO_StopAi(AiDev, s32AiChnCnt, TS_FALSE, TS_FALSE, TS_TRUE);
		SAMPLE_PRT("---------------SAMPLE_COMM_AUDIO_StopAi success---------------\n");

		SAMPLE_COMM_AUDIO_StopTrdAencAdec(AeChn, AdChn, NULL);
		SAMPLE_PRT("---------------SAMPLE_COMM_AUDIO_StopTrdAencAdec success---------------\n");
	}
	SAMPLE_COMM_SYS_Exit();

	SAMPLE_PRT("---------------deinit success ---------------\n");

	return TS_SUCCESS;
}

/******************************************************************************
 * function    : main()
 * Description : main
 ******************************************************************************/
int main(int argc, char *argv[])
{
	TS_S32 s32Ret = TS_FAILURE;
	TS_S32 s32Index;

	if (argc < 2 || argc > 5) {
		SAMPLE_AUDIO_Usage(argv[0]);
		return TS_FAILURE;
	}

	if (!strncmp(argv[1], "-h", 2)) {
		SAMPLE_AUDIO_Usage(argv[0]);
		return TS_SUCCESS;
	}

#ifndef __TsingLite__
	signal(SIGINT, SAMPLE_AUDIO_HandleSig);
	signal(SIGTERM, SAMPLE_AUDIO_HandleSig);
#endif

	s32Index = atoi(argv[1]);

	if (3 == argc || 4 == argc) {
		if (!strcmp(argv[2], "g711a"))
			gs_enPayloadType = PT_G711A;
		else if (!strcmp(argv[2], "g711u"))
			gs_enPayloadType = PT_G711U;
		else if (!strcmp(argv[2], "aac"))
			gs_enPayloadType = PT_AAC;
		else if (!strcmp(argv[2], "opus"))
			gs_enPayloadType = PT_OPUS;
		else if (!strcmp(argv[2], "g726"))
			gs_enPayloadType = PT_G726;
		else if (!strcmp(argv[2], "adpcm"))
			gs_enPayloadType = PT_ADPCMA;
		else if (!strcmp(argv[2], "pcm"))
			gs_enPayloadType = PT_LPCM;
		else
			printf("type:%s is invalid! use default g711a\n", argv[2]);

		if (4 == argc) {
			if (!strcmp(argv[3], "8000"))
				g_enSampleRate = AUDIO_SAMPLE_RATE_8000;
			else if (!strcmp(argv[3], "16000"))
				g_enSampleRate = AUDIO_SAMPLE_RATE_16000;
			else if (!strcmp(argv[3], "32000"))
				g_enSampleRate = AUDIO_SAMPLE_RATE_32000;
			else if (!strcmp(argv[3], "48000"))
				g_enSampleRate = AUDIO_SAMPLE_RATE_48000;

			else
				printf("sample_rate:%s is invalid! use default 16000\n", argv[3]);
		}
	}

	if (5 == argc) {
		memcpy(g_ip, argv[4], strlen(argv[4]) + 1);
		printf("pull stream ip:%s \n", argv[4]);
	}

	switch (s32Index) {
	case 0:
		s32Ret = SAMPLE_AUDIO_AIAO();
		break;
	case 1:
		s32Ret = SAMPLE_AUDIO_AIAENC();
		break;
	case 2:
		s32Ret = SAMPLE_AUDIO_ADECAO();
		break;
	case 3:
		s32Ret = SAMPLE_AUDIO_AiVqeAencProcessAdecAo();
		break;
	case 4:
		s32Ret = SAMPLE_AUDIO_AiVqeAencAdecVqeAoStream();
		break;
	default:
		SAMPLE_PRT("the index %d is invalid!\n", s32Index);
		SAMPLE_AUDIO_Usage(argv[0]);
		s32Ret = TS_FAILURE;
		break;
	}
	if (TS_SUCCESS == s32Ret)
		SAMPLE_PRT("program exit normally!\n");
	else
		SAMPLE_PRT("program exit abnormally!\n");

	return s32Ret;
}
