
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>
#include <linux/gpio.h>

#include "sample_audio_alg_proxy.h"
//#include "ts_alg_type.h"
//#include "ts_alg_log.h"

/*******************************************************
 * *
 * *	alg instance init table
 * *
********************************************************/
static SAMPLE_AUDIO_ALG_INIT_CFG_S gstAlgInitTbl[] = {
#if ALG_ICD
	{ AUDIO_ALG_MASK_ICD_8K, AUDIO_SAMPLE_RATE_8000, AUDIO_ALG_ICD_Init, "ICD_8K", 256 },
#endif

#if ALG_CRYDETECT
	{ AUDIO_ALG_MASK_CRY_DET, AUDIO_SAMPLE_RATE_16000, AUDIO_ALG_CryDetect_Init, "CryDetect", 512 },
#endif

};

//reason： 规避sample_comm_audio.c
PAYLOAD_TYPE_E gs_enPayloadType = PT_G711A;
AUDIO_SAMPLE_RATE_E g_enSampleRate = AUDIO_SAMPLE_RATE_16000;

static SAMPLE_AUDIO_ALG_PROXY gstAudioAlgProxy;

static TS_S32 SAMPLE_AUDIO_ALG_Init_Instances(SAMPLE_AUDIO_ALG_INSTANCE_S **ppAlgoInst_tbl,
					      SAMPLE_AUDIO_ALG_TYPE_E *penAlgMask, TS_U32 u32AlgNum)
{
	TS_U32 i, j, idx = 0;
	SAMPLE_AUDIO_ALG_INSTANCE_S *pInst;
	bool bCurFlag;
	TS_U32 u32CpmAlgNum = sizeof(gstAlgInitTbl) / sizeof(SAMPLE_AUDIO_ALG_INIT_CFG_S);

	for (i = 0; i < u32AlgNum; i++) {
		bCurFlag = false;
		for (j = 0; j < u32CpmAlgNum; ++j) {
			if (gstAlgInitTbl[j].eMask == penAlgMask[i]) {
				pInst = (SAMPLE_AUDIO_ALG_INSTANCE_S *)malloc(sizeof(SAMPLE_AUDIO_ALG_INSTANCE_S));
				if (!pInst)
					return idx;

				memset(pInst, 0, sizeof(SAMPLE_AUDIO_ALG_INSTANCE_S));

				if (gstAlgInitTbl[j].pInit((void **)&pInst) != TS_SUCCESS)
					goto algo_init_instance_faild;

				pInst->enAlgType = gstAlgInitTbl[j].eMask;
				*(ppAlgoInst_tbl + idx) = pInst;
				printf("ALG_Init_Instances, alg_type=0x%x, index=%d\n", penAlgMask[i], idx);
				idx++;
				bCurFlag = true;
				break;
			}
		}
		if (!bCurFlag)
			ALG_LOGE("not support algMask=%d \n", penAlgMask[i]);
	}
	return idx;

algo_init_instance_faild:
	if (pInst)
		free(pInst);

	return idx;
}

static TS_VOID SAMPLE_AUDIO_ALG_Exit(TS_VOID *pHandle)
{
	SAMPLE_AUDIO_ALG_PROXY *pCpmParam = (SAMPLE_AUDIO_ALG_PROXY *)pHandle;
	TS_S32 i;

	pCpmParam->bAlgProcRunFlag = TS_FALSE;
	for (i = 0; i < pCpmParam->u32AlgNum; i++) {
		if (pCpmParam->pstAlgInstList[i] != NULL) {
			if (pCpmParam->pstAlgInstList[i]->pfExit != NULL) {
				pCpmParam->pstAlgInstList[i]->pfExit(pCpmParam->pstAlgInstList[i]);
			}
		}
	}

	return;
}

static ts_char *ai_raw_cap_store = "./aiao_raw_test.pcm";
static FILE *file_raw;
static ts_u32 bytes_per_frame_raw;
static int test_raw_flag = 1;
static ts_u32 pcm_raw_total_size;

static ts_bool is_raw_pcm_file_open(void)
{
	if (file_raw)
		return TS_TRUE;
	return TS_FALSE;
}

static ts_void create_raw_pcm_file(ts_char *file_path, ts_u8 bit_width, ts_u8 channel_num, ts_u32 sample_rate)
{
	file_raw = fopen(file_path, "wb");
	bytes_per_frame_raw = channel_num * bit_width / 8;
	printf("\n\n %s channels=%d, rate=%d, bit_width=%d, bytes_per_frame_raw=%d\n", __func__, channel_num, sample_rate,
	       bit_width, bytes_per_frame_raw);
}

static ts_void close_raw_pcm_file(void)
{
	if (!file_raw)
		return;

	fclose(file_raw);
	file_raw = NULL;
}

static ts_void write_raw_pcm_file(ts_char *buffer, ts_u32 size)
{
	fwrite(buffer, 1, size, file_raw);
	//fflush(file_raw);
}

static void test_save_raw_pcm(AUDIO_FRAME_S *pFrame, AUDIO_SAMPLE_RATE_E efs)
{
	if (0 == test_raw_flag)
		return;

	//printf("%s:u32Len = %d, pFrame->u64VirAddr[0]= %p\n", __func__, pFrame->u32Len, pFrame->u64VirAddr[0]);
	if (TS_FALSE == is_raw_pcm_file_open())
		create_raw_pcm_file(ai_raw_cap_store, AUDIO_BIT_WIDTH_16, (AUDIO_SOUND_MODE_MONO + 1), efs);
	if (pFrame && pFrame->u32Len) {
		write_raw_pcm_file(pFrame->u64VirAddr[0], pFrame->u32Len);
		pcm_raw_total_size += pFrame->u32Len;
	} else {
		printf("length is error:[%d]\n", pFrame->u32Len);
	}

	if (pcm_raw_total_size > 512 * 1024) {
		test_raw_flag = 0;
		close_raw_pcm_file();
	}

	return;
}

static TS_VOID *SAMPLE_AUDIO_ALG_Process(TS_VOID *pHandle)
{
	TS_S32 s32Ret;
	TS_S32 AiFd;
	fd_set read_fds;
	struct timeval TimeoutVal;
	TS_S32 i;
	SAMPLE_AUDIO_ALG_PROXY *pCpmParam = (SAMPLE_AUDIO_ALG_PROXY *)pHandle;
	SAMPLE_AUDIO_ALG_INSTANCE_S *pcurAlgInst;

	AUDIO_FRAME_S stFrame;

	FD_ZERO(&read_fds);
	AiFd = TS_MPI_AI_GetFd(pCpmParam->AiDev, pCpmParam->AiChn);
	FD_SET(AiFd, &read_fds);

	printf("\n\n *********** [ai_dev, ai_chn, ao_dev, ao_chn] = [%d, %d, %d, %d]\n", pCpmParam->AiDev,
	       pCpmParam->AiChn, pCpmParam->AoDev, pCpmParam->AoChn);

	while (pCpmParam->bAlgProcRunFlag) {
		TimeoutVal.tv_sec = 15;
		TimeoutVal.tv_usec = 0;

		FD_ZERO(&read_fds);
		FD_SET(AiFd, &read_fds);
		s32Ret = TS_MPI_AI_SELECT(AiFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
		if (s32Ret < 0) {
			printf("%s: get ai frame select failed\n", __FUNCTION__);
			break;
		} else if (0 == s32Ret) {
			printf("%s: get ai frame select time out\n", __FUNCTION__);
			break;
		}

		if (FD_ISSET(AiFd, &read_fds)) {
			/* get frame from ai chn */
			memset(&stFrame, 0, sizeof(AUDIO_FRAME_S));
			//s32Ret = TS_MPI_AI_GetRawFrame(pCpmParam->AiDev, pCpmParam->AiChn, &stFrame, NULL, 0);
			s32Ret = TS_MPI_AI_GetFrame(pCpmParam->AiDev, pCpmParam->AiChn, &stFrame, NULL,  0);  //get up-vqe pcm
			if (TS_SUCCESS != s32Ret) {
				printf("%s: TS_MPI_AI_GetFrame(%d, %d), failed with %#x!\n", __FUNCTION__,
				       pCpmParam->AiDev, pCpmParam->AiChn, s32Ret);
				//pCpmParam->bAlgProcRunFlag = TS_FALSE;
				//return NULL;
				continue;
			}

			//printf("%s:u32Len = %d, pFrame->u64VirAddr[0]= %p\n", __func__, stVqeFrame.u32Len, stVqeFrame.u64VirAddr[0]);

			test_save_raw_pcm(&stFrame, pCpmParam->eAudioFs);
			for (i = 0; i < pCpmParam->u32ActualAlgNum; ++i) {
				pcurAlgInst = pCpmParam->pstAlgInstList[i];
				if (pcurAlgInst->pfProcess != NULL) {
#if 1
					s32Ret = pcurAlgInst->pfProcess(pcurAlgInst, &stFrame);
					if (TS_SUCCESS != s32Ret) {
						printf("TS_ALGO_Process error\n");
						goto exit_process;
					}
					//printf("TS_ALGO_Process success\n");
#endif
				}
			}

#if 1
			s32Ret = TS_MPI_AO_SendFrame(pCpmParam->AoDev, pCpmParam->AoChn, &stFrame, 1000);
			if (TS_SUCCESS != s32Ret) {
				printf("%s: TS_MPI_AO_SendFrame(%d, %d), failed with %#x!\n", __FUNCTION__,
				       pCpmParam->AoDev, pCpmParam->AoChn, s32Ret);
				//pCpmParam->bAlgProcRunFlag = TS_FALSE;
				//return NULL;
			}
#endif

			/* finally you must release the stream */
			//s32Ret = TS_MPI_AI_ReleaseRawFrame(pCpmParam->AiDev, pCpmParam->AiChn, &stFrame, NULL);
			s32Ret = TS_MPI_AI_ReleaseFrame(pCpmParam->AiDev, pCpmParam->AiChn, &stFrame, NULL);
			if (TS_SUCCESS != s32Ret) {
				printf("%s: TS_MPI_AI_ReleaseFrame(%d, %d), failed with %#x!\n", __FUNCTION__,
				       pCpmParam->AiDev, pCpmParam->AiChn, s32Ret);
				goto exit_process;
			}
		}
	}

exit_process:
	pCpmParam->bAlgProcRunFlag = TS_FALSE;

	SAMPLE_AUDIO_ALG_Exit(pHandle);
	printf("---------------alg_process_thread exit\n");
	return NULL;
}

TS_S32 SAMPLE_AUDIO_ALG_Config(AUDIO_SAMPLE_RATE_E efs, AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn,
			       SAMPLE_AUDIO_ALG_TYPE_E *penAlgType, TS_U32 u32AlgNum)
{
	memset(&gstAudioAlgProxy, 0, sizeof(SAMPLE_AUDIO_ALG_PROXY));
	gstAudioAlgProxy.eAudioFs = efs;
	gstAudioAlgProxy.AiDev = AiDev;
	gstAudioAlgProxy.AiChn = AiChn;
	gstAudioAlgProxy.AoDev = AoDev;
	gstAudioAlgProxy.AoChn = AoChn;
	gstAudioAlgProxy.u32AlgNum = u32AlgNum;
	memcpy(gstAudioAlgProxy.enAlgMask, penAlgType, u32AlgNum * sizeof(SAMPLE_AUDIO_ALG_TYPE_E));

	gstAudioAlgProxy.u32ActualAlgNum =
		SAMPLE_AUDIO_ALG_Init_Instances(gstAudioAlgProxy.pstAlgInstList, penAlgType, u32AlgNum);

	if (0 == gstAudioAlgProxy.u32ActualAlgNum) {
		ALG_LOGE("no one alg register\n");
		//return TS_FAILURE;
	}

	gstAudioAlgProxy.bAlgProcRunFlag = TS_TRUE;

	pthread_create(&(gstAudioAlgProxy.stAlgProcPid), 0, SAMPLE_AUDIO_ALG_Process, (TS_VOID *)&gstAudioAlgProxy);

	return TS_SUCCESS;
}

SAMPLE_AUDIO_ALG_INIT_CFG_S *SAMPLE_AUDIO_ALG_GetInitCfg(TS_U32 *pNum)
{
	if (NULL == pNum) {
		ALG_LOGE("SAMPLE_AUDIO_ALG_GetInitCfg param is NULL!\n");
		return NULL;
	}

	*pNum = sizeof(gstAlgInitTbl) / sizeof(SAMPLE_AUDIO_ALG_INIT_CFG_S);
	return gstAlgInitTbl;
}

TS_VOID SAMPLE_AUDIO_ALG_Stop()
{
	gstAudioAlgProxy.bAlgProcRunFlag = TS_FALSE;
}
