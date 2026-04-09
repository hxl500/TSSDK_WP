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

#include "ts_type.h"
#include "sample_audio_alg_proxy.h"

extern TS_BOOL g_raw_enable;
static TS_S32 SAMPLE_AUDIO_AIAORAW(AUDIO_SAMPLE_RATE_E efs, SAMPLE_AUDIO_ALG_TYPE_E *peAlgType, TS_U32 u32AlgNum,
				   TS_S32 s32FrameLen)
{
	TS_S32 s32Ret = TS_SUCCESS;

	VB_CONFIG_S stVbConf;
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("SAMPLE_COMM_SYS_Init failed!\n");
		return s32Ret;
	}

	AIO_ATTR_S stAioAttr = { 0 };
	stAioAttr.u8CardNum = 0;
	stAioAttr.aioMode = AIO_INNER_CODEC;
	stAioAttr.u32PtNumPerFrm = s32FrameLen;

	stAioAttr.u8DeviceNum = 0;
	stAioAttr.enSamplerate = efs;
	stAioAttr.enBitwidth = AUDIO_BIT_WIDTH_16;
	stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	stAioAttr.u32FrmNum = ALG_AUDIO_FRM_NUM;
	stAioAttr.aecEnable = TS_TRUE;

	/********************************************
	  step 1: start Ai
	********************************************/
	AUDIO_DEV AiDev = SAMPLE_AUDIO_INNER_AI_DEV;
	TS_S32 s32AiChnCnt = 1;
	g_raw_enable = TS_FALSE;
	//s32Ret = SAMPLE_COMM_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, TS_FALSE, TS_FALSE, &stVqeConfig, 1);
	s32Ret = SAMPLE_COMM_AUDIO_StartAi(AiDev, s32AiChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, TS_TRUE, TS_FALSE,
					   NULL, 0);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("SAMPLE_COMM_AUDIO_StartAi failed with %d!\n", s32Ret);
		//g_raw_enable = TS_FALSE;
		return s32Ret;
	}
	//g_raw_enable = TS_FALSE;

	TS_S32 s32VolumeDb;
	s32Ret = TS_MPI_AI_GetVqeVolume(AiDev, s32AiChnCnt, &s32VolumeDb);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("#### error, TS_MPI_AI_GetVqeVolume failed with %d!\n", s32Ret);
	}

	SAMPLE_PRT("TS_MPI_AI_GetVqeVolume %d db!\n", s32VolumeDb);

	// s32VolumeDb = 26;
	// s32Ret = TS_MPI_AI_SetVqeVolume(AiDev, s32AiChnCnt, s32VolumeDb);
	// if (TS_SUCCESS != s32Ret) {
	// 	SAMPLE_PRT("#### error, TS_MPI_AI_SetVqeVolume failed with %d!\n", s32Ret);
	// }

	// SAMPLE_PRT("TS_MPI_AI_SetVqeVolume %d db!\n", s32VolumeDb);

	/********************************************
	  step 2: start Ao
	********************************************/
	AUDIO_DEV AoDev = SAMPLE_AUDIO_INNER_AO_DEV;
	TS_S32 s32AoChnCnt = 1;
	stAioAttr.u8CardNum = 0;
	stAioAttr.aioMode = AIO_INNER_CODEC;
	//AUDIO_VQE_CONFIG_S stVqeConfig;
	//memset(&stVqeConfig, 0, sizeof(AUDIO_VQE_CONFIG_S));
	s32Ret = SAMPLE_COMM_AUDIO_StartAo(AoDev, s32AoChnCnt, &stAioAttr, AUDIO_SAMPLE_RATE_BUTT, TS_FALSE, TS_NULL,
					   0); //close vqe
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("SAMPLE_COMM_AUDIO_StartAo failed with %d!\n", s32Ret);
		return s32Ret;
	}

	SAMPLE_PRT("########SAMPLE_COMM_AUDIO_StartAo success\n");

	SAMPLE_AUDIO_ALG_Config(efs, AiDev, 0, AoDev, 0, peAlgType, u32AlgNum);
	SAMPLE_PRT("########SAMPLE_AUDIO_ALG_Config success\n");

	//SAMPLE_COMM_AI_Bind_AO(0, 0);

	char c = 0;
	while (1) {
		printf("---------------Press enter 'q' to exit; ---------------\n");
		c = getchar();
		if (c == 'q')
			break;
		sleep(3);
	}

	printf("---------------deinit ---------------\n");

	SAMPLE_AUDIO_ALG_Stop();

	SAMPLE_COMM_AUDIO_StopAo(AoDev, s32AoChnCnt, TS_FALSE);
	printf("---------------SAMPLE_COMM_AUDIO_StopAo SUCCESS---------------\n");

	SAMPLE_COMM_AUDIO_StopAi(AiDev, s32AiChnCnt, TS_TRUE, TS_FALSE, TS_TRUE);
	printf("---------------SAMPLE_COMM_AUDIO_StopAi SUCCESS---------------\n");

	SAMPLE_COMM_SYS_Exit();
	printf("---------------deinit SUCCESS---------------\n");

	return TS_SUCCESS;
}

static TS_VOID SAMPLE_TSALG_AUDIO_Usage(TS_CHAR *sPrgNm)
{
	TS_U32 u32AlgNum = 0;
	SAMPLE_AUDIO_ALG_INIT_CFG_S *pAlgInitTbl = SAMPLE_AUDIO_ALG_GetInitCfg(&u32AlgNum);
	printf("Usage : %s <index>\n", sPrgNm);
	printf("index:\n");

	for (size_t i = 0; i < u32AlgNum; ++i) {
		printf("\t %d)%s\n", pAlgInitTbl[i].eMask, pAlgInitTbl[i].u8AlgName);
	}
}

static TS_S32 SAMPLE_TSALG_AUDIO_Match(TS_S32 s32Index, SAMPLE_AUDIO_ALG_TYPE_E *penMask, AUDIO_SAMPLE_RATE_E *pefs,
				       TS_S32 *s32FrameLen)
{
	TS_S32 s32Ret = TS_FAILURE;
	TS_U32 u32AlgNum = 0;
	SAMPLE_AUDIO_ALG_INIT_CFG_S *pAlgInitTbl = SAMPLE_AUDIO_ALG_GetInitCfg(&u32AlgNum);
	for (size_t i = 0; i < u32AlgNum; ++i) {
		if (s32Index == pAlgInitTbl[i].eMask) {
			*penMask = pAlgInitTbl[i].eMask;
			*pefs = pAlgInitTbl[i].eAudioFs;
			*s32FrameLen = pAlgInitTbl[i].s32FrameLen;
			s32Ret = TS_SUCCESS;
			break;
		}
	}
	return s32Ret;
}

/******************************************************************************
 * function : to process abnormal case
 ******************************************************************************/
static void SAMPLE_AUDIO_HandleSig(TS_S32 signo)
{
	if (SIGINT == signo || SIGTERM == signo) {
		SAMPLE_COMM_SYS_Exit();
		printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
	}
	exit(-1);
}

int main(int argc, char *argv[])
{
	TS_S32 s32Ret = TS_FAILURE;
	TS_S32 s32Index;
	SAMPLE_AUDIO_ALG_TYPE_E enCurMask;
	AUDIO_SAMPLE_RATE_E efs;
	TS_S32 s32FrameLen;

	if (argc < 2 || argc > 2) {
		SAMPLE_TSALG_AUDIO_Usage(argv[0]);
		return TS_FAILURE;
	}
	if (!strncmp(argv[1], "-h", 2)) {
		SAMPLE_TSALG_AUDIO_Usage(argv[0]);
		return TS_SUCCESS;
	}

	s32Index = atoi(argv[1]);
	s32Ret = SAMPLE_TSALG_AUDIO_Match(s32Index, &enCurMask, &efs, &s32FrameLen);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_TSALG_AUDIO_Usage(argv[0]);
		return TS_SUCCESS;
	}

	SAMPLE_COMM_SYS_ShowVersion(argv[0]);

	signal(SIGINT, SAMPLE_AUDIO_HandleSig);
	signal(SIGTERM, SAMPLE_AUDIO_HandleSig);

	//s32Ret = SAMPLE_AUDIO_AIRAW(&enCurMask, 1);
	s32Ret = SAMPLE_AUDIO_AIAORAW(efs, &enCurMask, 1, s32FrameLen);
	if (TS_SUCCESS == s32Ret)
		SAMPLE_PRT("program exit normally!\n");
	else
		printf("program exit abnormally, AlgType:%d!\n", enCurMask);
	return s32Ret;
}