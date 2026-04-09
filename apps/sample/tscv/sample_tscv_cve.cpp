#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <unistd.h>
#include "mpi_sys.h"
#include "tscv.hpp"
#include "mpi_vb.h"

using namespace cv;

void log_message(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	printf("Process [%d]: ", getpid());
	vprintf(format, args);
	printf("\n");

	va_end(args);
}

#define SAMPLE_PRT(fmt...)                                                                                             \
	do {                                                                                                           \
		printf("[%s]-%d: ", __func__, __LINE__);                                                               \
		printf(fmt);                                                                                           \
	} while (0)

static long SAMPLE_TSCV_GetTime()
{
	struct timeval tv1;
	gettimeofday(&tv1, NULL);
	return (long)(tv1.tv_sec * 1000000) + (tv1.tv_usec);
}

/******************************************************************************
* function : vb init & MPI system init
******************************************************************************/
TS_S32 SAMPLE_SYS_Init()
{
	TS_S32 s32Ret = TS_FAILURE;

	TS_MPI_SYS_Exit();
	TS_MPI_VB_Exit();

	VB_CONFIG_S stVbConf;
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt = 1;
	TS_S32 blk_size = 100*100;
	stVbConf.astCommPool[0].u64BlkSize = blk_size;
	stVbConf.astCommPool[0].u32BlkCnt = 2;

	s32Ret = TS_MPI_VB_SetConfig(&stVbConf);

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

	return s32Ret;
}

/******************************************************************************
* function : vb exit & MPI system exit
******************************************************************************/
TS_VOID SAMPLE_SYS_Exit()
{
    /*SAMPLE_COMM_VO_Exit();*/	/* only for debug */
    TS_MPI_SYS_Exit();
//    TS_MPI_VB_ExitModCommPool(VB_UID_VDEC);
    TS_MPI_VB_Exit();
    return;
}

static TS_S32 CallCoreFunction(TS_S32 (*coreFunc)())
{
	TS_S32 ret = coreFunc();
	if (ret != TS_SUCCESS) {
		SAMPLE_PRT("Core function failed!\n");
	}
	return ret;
}


static TS_S32 CalcHistCore()
{
	TS_S32 ret = 0;
	Mat matIn(480, 640, CV_8UC1);
	Mat matOut(1, 256, CV_32SC1);
	TS_S32 insize = 480*640;
    TS_S32 outsize = 1*256*4;
    ret = TS_MPI_SYS_MmzAlloc(&matIn.mpi_phyaddr, (void**)&matIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

    ret = TS_MPI_SYS_MmzAlloc(&matOut.mpi_phyaddr, (void**)&matOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

	matIn.data = (uchar*)matIn.mpi_viraddr;
    matOut.data = (uchar*)matOut.mpi_viraddr;

	void *pHandleCve = tscv::createCve();
	if(pHandleCve == NULL) {
		SAMPLE_PRT("error, create cve handle failed!\n");
		return -1;
	}

	matIn.setTo(cv::Scalar(3));

	for(int i=0; i<10; i++) {
		long t0 = SAMPLE_TSCV_GetTime();
		int channels = 1;
		int histSize = 1;
		tscv::calcHistCve(pHandleCve, &matIn, 1, &channels, cv::Mat(), matOut, 1, &histSize, 0);
		long t1 = SAMPLE_TSCV_GetTime();
		SAMPLE_PRT("================calcHist time:%ldus\n", t1 - t0);
	}
	
	tscv::exitCve(pHandleCve);
	TS_U32 size = sizeof(int);
	TS_U32 counts = 256;
	TS_CHAR szOutFileNameWithPid[128];
	sprintf(szOutFileNameWithPid, "%s_%d.bin", "out/HistResult", getpid());
	FILE *txt_fd = fopen(szOutFileNameWithPid, "w");
	fwrite(matOut.data, size, counts, txt_fd);
	fflush(txt_fd);
	fclose(txt_fd);

	TS_MPI_SYS_MmzFree(matIn.mpi_phyaddr,(TS_VOID *)matIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(matOut.mpi_phyaddr,(TS_VOID *)matOut.mpi_viraddr);
	return 0;
}

static TS_S32 AddCore()
{
	TS_S32 ret = 0;
	Mat matIn(480, 640, CV_8UC1);
	Mat matIn2(480, 640, CV_8UC1);
	Mat matOut(480, 640, CV_8UC1);
	TS_S32 insize = 480*640;
    TS_S32 outsize = 480*640;
    ret = TS_MPI_SYS_MmzAlloc(&matIn.mpi_phyaddr, (void**)&matIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

	ret = TS_MPI_SYS_MmzAlloc(&matIn2.mpi_phyaddr, (void**)&matIn2.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

    ret = TS_MPI_SYS_MmzAlloc(&matOut.mpi_phyaddr, (void**)&matOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

	matIn.data = (uchar*)matIn.mpi_viraddr;
	matIn2.data = (uchar*)matIn2.mpi_viraddr;
    matOut.data = (uchar*)matOut.mpi_viraddr;
	
	matIn.setTo(cv::Scalar(10));
	matIn2.setTo(cv::Scalar(50));

	void *pHandleCve = tscv::createCve();
	if(pHandleCve == NULL) {
		SAMPLE_PRT("error, create cve handle failed!\n");
		return -1;
	}
	for(int i=0; i<10; i++) {
		long t0 = SAMPLE_TSCV_GetTime();
		tscv::addCve(pHandleCve, matIn, matIn2, matOut, cv::Mat());
		long t1 = SAMPLE_TSCV_GetTime();
		SAMPLE_PRT("================add time:%ldus\n", t1 - t0);
	}
	tscv::exitCve(pHandleCve);
	TS_CHAR szOutFileNameWithPid[128];
	sprintf(szOutFileNameWithPid, "%s_%d.bin", "out/AddResult", getpid());
	FILE *txt_fd = fopen(szOutFileNameWithPid, "w");
	fwrite(matOut.data, 1, outsize, txt_fd);
	fflush(txt_fd);
	fclose(txt_fd);

	TS_MPI_SYS_MmzFree(matIn.mpi_phyaddr,(TS_VOID *)matIn.mpi_viraddr);
	TS_MPI_SYS_MmzFree(matIn2.mpi_phyaddr,(TS_VOID *)matIn2.mpi_viraddr);
    TS_MPI_SYS_MmzFree(matOut.mpi_phyaddr,(TS_VOID *)matOut.mpi_viraddr);

	return 0;
}

static TS_S32 SubtractCore()
{
	TS_S32 ret = 0;
	Mat matIn(480, 640, CV_8UC1);
	Mat matIn2(480, 640, CV_8UC1);
	Mat matOut(480, 640, CV_8UC1);
	TS_S32 insize = 480*640;
    TS_S32 outsize = 480*640;
    ret = TS_MPI_SYS_MmzAlloc(&matIn.mpi_phyaddr, (void**)&matIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

	ret = TS_MPI_SYS_MmzAlloc(&matIn2.mpi_phyaddr, (void**)&matIn2.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

    ret = TS_MPI_SYS_MmzAlloc(&matOut.mpi_phyaddr, (void**)&matOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

	matIn.data = (uchar*)matIn.mpi_viraddr;
	matIn2.data = (uchar*)matIn2.mpi_viraddr;
    matOut.data = (uchar*)matOut.mpi_viraddr;
	
	matIn.setTo(cv::Scalar(50));
	matIn2.setTo(cv::Scalar(10));

	void *pHandleCve = tscv::createCve();
	if(pHandleCve == NULL) {
		SAMPLE_PRT("error, create cve handle failed!\n");
		return -1;
	}
	for(int i=0; i<10; i++) {
		long t0 = SAMPLE_TSCV_GetTime();
		tscv::subtractCve(pHandleCve, matIn, matIn2, matOut, cv::Mat());
		long t1 = SAMPLE_TSCV_GetTime();
		SAMPLE_PRT("================subtract time:%ldus\n", t1 - t0);
	}
	tscv::exitCve(pHandleCve);

	TS_CHAR szOutFileNameWithPid[128];
	sprintf(szOutFileNameWithPid, "%s_%d.bin", "out/SubResult", getpid());
	FILE *txt_fd = fopen(szOutFileNameWithPid, "w");
	fwrite(matOut.data, 1, outsize, txt_fd);
	fflush(txt_fd);
	fclose(txt_fd);

	TS_MPI_SYS_MmzFree(matIn.mpi_phyaddr,(TS_VOID *)matIn.mpi_viraddr);
	TS_MPI_SYS_MmzFree(matIn2.mpi_phyaddr,(TS_VOID *)matIn2.mpi_viraddr);
    TS_MPI_SYS_MmzFree(matOut.mpi_phyaddr,(TS_VOID *)matOut.mpi_viraddr);
	return 0;
}

static TS_S32 BitwiseAndCore()
{
	TS_S32 ret = 0;
	Mat matIn(480, 640, CV_8UC1);
	Mat matIn2(480, 640, CV_8UC1);
	Mat matOut(480, 640, CV_8UC1);
	TS_S32 insize = 480*640;
    TS_S32 outsize = 480*640;
    ret = TS_MPI_SYS_MmzAlloc(&matIn.mpi_phyaddr, (void**)&matIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

	ret = TS_MPI_SYS_MmzAlloc(&matIn2.mpi_phyaddr, (void**)&matIn2.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

    ret = TS_MPI_SYS_MmzAlloc(&matOut.mpi_phyaddr, (void**)&matOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

	matIn.data = (uchar*)matIn.mpi_viraddr;
	matIn2.data = (uchar*)matIn2.mpi_viraddr;
    matOut.data = (uchar*)matOut.mpi_viraddr;

	matIn.setTo(cv::Scalar(23));
	matIn2.setTo(cv::Scalar(185));

	void *pHandleCve = tscv::createCve();
	if(pHandleCve == NULL) {
		SAMPLE_PRT("error, create cve handle failed!\n");
		return -1;
	}

	for(int i=0; i<10; i++) {
		long t0 = SAMPLE_TSCV_GetTime();
		tscv::bitwiseAndCve(pHandleCve, matIn, matIn2, matOut, cv::Mat());
		long t1 = SAMPLE_TSCV_GetTime();
		SAMPLE_PRT("================bitwise_and time:%ldus\n", t1 - t0);
	}
	tscv::exitCve(pHandleCve);

	TS_CHAR szOutFileNameWithPid[128];
	sprintf(szOutFileNameWithPid, "%s_%d.bin", "out/bitwiseAnd", getpid());
	FILE *txt_fd = fopen(szOutFileNameWithPid, "w");
	fwrite(matOut.data, 1, outsize, txt_fd);
	fflush(txt_fd);
	fclose(txt_fd);

	TS_MPI_SYS_MmzFree(matIn.mpi_phyaddr,(TS_VOID *)matIn.mpi_viraddr);
	TS_MPI_SYS_MmzFree(matIn2.mpi_phyaddr,(TS_VOID *)matIn2.mpi_viraddr);
    TS_MPI_SYS_MmzFree(matOut.mpi_phyaddr,(TS_VOID *)matOut.mpi_viraddr);

	return 0;
}

static TS_S32 BitwiseOrCore()
{
	TS_S32 ret = 0;
	Mat matIn(480, 640, CV_8UC1);
	Mat matIn2(480, 640, CV_8UC1);
	Mat matOut(480, 640, CV_8UC1);
	TS_S32 insize = 480*640;
    TS_S32 outsize = 480*640;
    ret = TS_MPI_SYS_MmzAlloc(&matIn.mpi_phyaddr, (void**)&matIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

	ret = TS_MPI_SYS_MmzAlloc(&matIn2.mpi_phyaddr, (void**)&matIn2.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

    ret = TS_MPI_SYS_MmzAlloc(&matOut.mpi_phyaddr, (void**)&matOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

	matIn.data = (uchar*)matIn.mpi_viraddr;
	matIn2.data = (uchar*)matIn2.mpi_viraddr;
    matOut.data = (uchar*)matOut.mpi_viraddr;

	matIn.setTo(cv::Scalar(23));
	matIn2.setTo(cv::Scalar(185));

	void *pHandleCve = tscv::createCve();
	if(pHandleCve == NULL) {
		SAMPLE_PRT("error, create cve handle failed!\n");
		return -1;
	}
	for(int i=0; i<10; i++) {
		long t0 = SAMPLE_TSCV_GetTime();
		tscv::bitwiseOrCve(pHandleCve, matIn, matIn2, matOut, cv::Mat());
		long t1 = SAMPLE_TSCV_GetTime();
		SAMPLE_PRT("================bitwise_or time:%ldus\n", t1 - t0);
	}
	tscv::exitCve(pHandleCve);

	TS_CHAR szOutFileNameWithPid[128];
	sprintf(szOutFileNameWithPid, "%s_%d.bin", "out/bitwiseOr", getpid());
	FILE *txt_fd = fopen(szOutFileNameWithPid, "w");
	fwrite(matOut.data, 1, outsize, txt_fd);
	fflush(txt_fd);
	fclose(txt_fd);

	TS_MPI_SYS_MmzFree(matIn.mpi_phyaddr,(TS_VOID *)matIn.mpi_viraddr);
	TS_MPI_SYS_MmzFree(matIn2.mpi_phyaddr,(TS_VOID *)matIn2.mpi_viraddr);
    TS_MPI_SYS_MmzFree(matOut.mpi_phyaddr,(TS_VOID *)matOut.mpi_viraddr);

	return 0;
}

static TS_S32 BitwiseXorCore()
{
	TS_S32 ret = 0;
	Mat matIn(480, 640, CV_8UC1);
	Mat matIn2(480, 640, CV_8UC1);
	Mat matOut(480, 640, CV_8UC1);
	TS_S32 insize = 480*640;
    TS_S32 outsize = 480*640;
    ret = TS_MPI_SYS_MmzAlloc(&matIn.mpi_phyaddr, (void**)&matIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

	ret = TS_MPI_SYS_MmzAlloc(&matIn2.mpi_phyaddr, (void**)&matIn2.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

    ret = TS_MPI_SYS_MmzAlloc(&matOut.mpi_phyaddr, (void**)&matOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

	matIn.data = (uchar*)matIn.mpi_viraddr;
	matIn2.data = (uchar*)matIn2.mpi_viraddr;
    matOut.data = (uchar*)matOut.mpi_viraddr;

	matIn.setTo(cv::Scalar(23));
	matIn2.setTo(cv::Scalar(185));

	void *pHandleCve = tscv::createCve();
	if(pHandleCve == NULL) {
		SAMPLE_PRT("error, create cve handle failed!\n");
		return -1;
	}
	for(int i=0; i<10; i++) {
		long t0 = SAMPLE_TSCV_GetTime();
		tscv::bitwiseXorCve(pHandleCve, matIn, matIn2, matOut, cv::Mat());
		long t1 = SAMPLE_TSCV_GetTime();
		SAMPLE_PRT("================bitwise_xor time:%ldus\n", t1 - t0);
	}
	tscv::exitCve(pHandleCve);

	TS_CHAR szOutFileNameWithPid[128];
	sprintf(szOutFileNameWithPid, "%s_%d.bin", "out/bitwiseXor", getpid());
	FILE *txt_fd = fopen(szOutFileNameWithPid, "w");
	fwrite(matOut.data, 1, outsize, txt_fd);
	fflush(txt_fd);
	fclose(txt_fd);

	TS_MPI_SYS_MmzFree(matIn.mpi_phyaddr,(TS_VOID *)matIn.mpi_viraddr);
	TS_MPI_SYS_MmzFree(matIn2.mpi_phyaddr,(TS_VOID *)matIn2.mpi_viraddr);
    TS_MPI_SYS_MmzFree(matOut.mpi_phyaddr,(TS_VOID *)matOut.mpi_viraddr);
	return 0;
}

static TS_S32 ThresholdCore()
{
	TS_S32 ret = 0;
	Mat matIn(480, 640, CV_8UC1);
	Mat matOut(480, 640, CV_8UC1);
	TS_S32 insize = 480*640;
    TS_S32 outsize = 480*640;
    ret = TS_MPI_SYS_MmzAlloc(&matIn.mpi_phyaddr, (void**)&matIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

    ret = TS_MPI_SYS_MmzAlloc(&matOut.mpi_phyaddr, (void**)&matOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

	matIn.data = (uchar*)matIn.mpi_viraddr;
    matOut.data = (uchar*)matOut.mpi_viraddr;

	if (access("data/480p_gray_0.raw", F_OK) != 0) {
		SAMPLE_PRT("error, Test bin files not found!\n");
		return -1;
	}

	FILE *txt_fd = fopen("data/480p_gray_0.raw", "r");
	fread(matIn.data, 1, insize, txt_fd);
	fflush(txt_fd);
	fclose(txt_fd);

	void *pHandleCve = tscv::createCve();
	if(pHandleCve == NULL) {
		SAMPLE_PRT("error, create cve handle failed!\n");
		return -1;
	}
	for(int i=0; i<10; i++) {
		long t0 = SAMPLE_TSCV_GetTime();
		tscv::thresholdCve(pHandleCve, matIn, matOut, 100.0, 200.0, THRESH_BINARY);
		long t1 = SAMPLE_TSCV_GetTime();
		SAMPLE_PRT("================threshold time:%ldus\n", t1 - t0);
	}
	tscv::exitCve(pHandleCve);

	TS_CHAR szOutFileNameWithPid[128];
	sprintf(szOutFileNameWithPid, "%s_%d.bin", "out/ThreshResult", getpid());
	txt_fd = fopen(szOutFileNameWithPid, "w");
	fwrite(matOut.data, 1, outsize, txt_fd);
	fflush(txt_fd);
	fclose(txt_fd);

	TS_MPI_SYS_MmzFree(matIn.mpi_phyaddr,(TS_VOID *)matIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(matOut.mpi_phyaddr,(TS_VOID *)matOut.mpi_viraddr);
	return 0;
}

static TS_S32 ConvertToCore()
{
	printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
	TS_S32 ret = 0;
	Mat matIn(480, 640, CV_16SC1);
	Mat matOut(480, 640, CV_8UC1);
	TS_S32 insize = 480*640*2;
    TS_S32 outsize = 480*640;
    ret = TS_MPI_SYS_MmzAlloc(&matIn.mpi_phyaddr, (void**)&matIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

    ret = TS_MPI_SYS_MmzAlloc(&matOut.mpi_phyaddr, (void**)&matOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

	matIn.data = (uchar*)matIn.mpi_viraddr;
    matOut.data = (uchar*)matOut.mpi_viraddr;

	matIn.setTo(cv::Scalar(10));

	void *pHandleCve = tscv::createCve();
	if(pHandleCve == NULL) {
		SAMPLE_PRT("error, create cve handle failed!\n");
		return -1;
	}
	for(int i=0; i<10; i++) {
		long t0 = SAMPLE_TSCV_GetTime();
		tscv::convertToCve(pHandleCve, matIn, matOut, CV_8UC1, 0.32, 2);
		long t1 = SAMPLE_TSCV_GetTime();
		SAMPLE_PRT("================convertTo time:%ldus\n", t1 - t0);
	}
	tscv::exitCve(pHandleCve);

	TS_CHAR szOutFileNameWithPid[128];
	sprintf(szOutFileNameWithPid, "%s_%d.bin", "out/ConvertToResult", getpid());
	FILE *txt_fd = fopen(szOutFileNameWithPid, "w");
	fwrite(matOut.data, 1, outsize, txt_fd);
	fflush(txt_fd);
	fclose(txt_fd);
	printf("BBBBBBBBBBBBBBBBBBBBBBBB\n");
	TS_MPI_SYS_MmzFree(matIn.mpi_phyaddr,(TS_VOID *)matIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(matOut.mpi_phyaddr,(TS_VOID *)matOut.mpi_viraddr);
	printf("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC\n");
	return 0;
}

static TS_S32 MapCore()
{
	TS_U16 mapTscv[256] = {
		0x06, 0xb5, 0xbe, 0x24, 0xf6, 0xac, 0x7b, 0xca, 0xc8, 0xf0, 0xbb, 0x98, 0x5e, 0xb9, 0x7c, 0x59, 0xd0, 0x41,
		0x33, 0x9d, 0x03, 0xdd, 0x59, 0x6a, 0x27, 0x28, 0xa6, 0x8f, 0x16, 0xb8, 0xb2, 0xe8, 0x03, 0xad, 0xf5, 0xd3,
		0x7b, 0xd7, 0xcf, 0x74, 0x10, 0xc6, 0xe3, 0x8f, 0x75, 0x45, 0xd5, 0x87, 0xd2, 0x65, 0xab, 0xd5, 0x32, 0x6e,
		0xcd, 0xa4, 0x5e, 0x20, 0xc9, 0x84, 0x81, 0xc0, 0x94, 0x36, 0x6c, 0x9f, 0x69, 0xe4, 0xb2, 0x5d, 0xf6, 0x9f,
		0xdc, 0xc2, 0x2a, 0x89, 0xaa, 0x6e, 0x88, 0x69, 0x89, 0x32, 0x8a, 0xdf, 0x2e, 0x64, 0xe2, 0x9c, 0xec, 0x47,
		0x88, 0xae, 0xf0, 0x93, 0x16, 0xc0, 0x95, 0xd4, 0xc3, 0xc0, 0x00, 0x46, 0x74, 0x7c, 0x3f, 0x5d, 0x64, 0xa0,
		0x04, 0xc9, 0x70, 0xf9, 0x56, 0xe8, 0x80, 0xff, 0x59, 0x98, 0x43, 0x30, 0xe6, 0x24, 0x4b, 0xb5, 0x7b, 0x1b,
		0xf8, 0xa0, 0xb4, 0xe6, 0x27, 0xb1, 0x95, 0x6a, 0x50, 0x96, 0x55, 0x64, 0x19, 0xeb, 0xba, 0xea, 0xab, 0x4a,
		0x53, 0xe5, 0xf6, 0xd8, 0xf0, 0x2f, 0x5a, 0xa7, 0x5a, 0x0a, 0xa1, 0xc8, 0xb5, 0x18, 0x6c, 0x22, 0xd2, 0xe4,
		0x5b, 0x44, 0x77, 0xdd, 0x7a, 0xd6, 0xed, 0x2e, 0x1b, 0x27, 0x9f, 0xfb, 0xb8, 0x7f, 0xdc, 0x68, 0x97, 0x0f,
		0xfc, 0x5a, 0xdb, 0x94, 0x0b, 0x7b, 0x21, 0xfa, 0x89, 0x45, 0xaf, 0x3d, 0x2e, 0x29, 0xd9, 0x41, 0x35, 0xd9,
		0x0f, 0x67, 0x7f, 0x3b, 0x74, 0xb1, 0x68, 0x7b, 0x9f, 0x50, 0x62, 0x74, 0x01, 0x60, 0xb7, 0xd0, 0x37, 0xf0,
		0x5f, 0x66, 0x1a, 0x24, 0xdc, 0x7d, 0x06, 0xe7, 0x0e, 0xeb, 0xc6, 0x85, 0xf1, 0x9d, 0xad, 0x0b, 0xd0, 0xd1,
		0xc6, 0x72, 0xa3, 0x70, 0xdd, 0xcf, 0x49, 0x71, 0xf3, 0x9d, 0x0f, 0x99, 0x4f, 0xa6, 0x71, 0x25, 0x3d, 0xac,
		0x81, 0xab, 0xa6, 0x0a,
	};
	
	TS_S32 ret = 0;
	Mat matIn(480, 640, CV_8UC1);
	Mat matOut(480, 640, CV_8UC1);
	TS_S32 insize = 480*640;
    TS_S32 outsize = 480*640;
    ret = TS_MPI_SYS_MmzAlloc(&matIn.mpi_phyaddr, (void**)&matIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

    ret = TS_MPI_SYS_MmzAlloc(&matOut.mpi_phyaddr, (void**)&matOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

	matIn.data = (uchar*)matIn.mpi_viraddr;
    matOut.data = (uchar*)matOut.mpi_viraddr;

	matIn.setTo(cv::Scalar(0));

	void *pHandleCve = tscv::createCve();
	if(pHandleCve == NULL) {
		SAMPLE_PRT("error, create cve handle failed!\n");
		return -1;
	}
	for(int i=0; i<10; i++) {
		long t0 = SAMPLE_TSCV_GetTime();
		tscv::mapCve(pHandleCve, matIn, matOut, mapTscv, 256);
		long t1 = SAMPLE_TSCV_GetTime();
		SAMPLE_PRT("================map time:%ldus\n", t1 - t0);
	}
	tscv::exitCve(pHandleCve);

	TS_CHAR szOutFileNameWithPid[128];
	sprintf(szOutFileNameWithPid, "%s_%d.bin", "out/MapResult", getpid());
	FILE *txt_fd = fopen(szOutFileNameWithPid, "w");
	fwrite(matOut.data, 1, outsize, txt_fd);
	fflush(txt_fd);
	fclose(txt_fd);

	TS_MPI_SYS_MmzFree(matIn.mpi_phyaddr,(TS_VOID *)matIn.mpi_viraddr);
	TS_MPI_SYS_MmzFree(matOut.mpi_phyaddr,(TS_VOID *)matOut.mpi_viraddr);
	return 0;
}



static TS_VOID SAMPLE_TSCV_Usage(TS_CHAR *sPrgNm)
{
	printf("Usage : %s <index>\n", sPrgNm);
	printf("index:\n");

	printf("\t 0)calcHist\n");
	printf("\t 1)Add\n");
	printf("\t 2)Subtract\n");
	printf("\t 3)bitwiseAnd\n");
	printf("\t 4)bitwiseOr\n");
	printf("\t 5)bitwiseXor\n");
	printf("\t 6)Map\n");
	printf("\t 7)Threshold\n");
	printf("\t 8)converTo\n");
}

static TS_VOID SAMPLE_TSCV_HandleSig(TS_S32 signo)
{
	if (SIGINT == signo || SIGTERM == signo) {
		printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
	}

	exit(-1);
}

TS_S32 simple_task_core(int argc, char *argv[])
{
	TS_S32 s32Ret = TS_FAILURE;
	TS_S32 s32Index;

	if (argc < 2 || argc > 2) {
		SAMPLE_TSCV_Usage(argv[0]);
		return TS_FAILURE;
	}

	if (!strncmp(argv[1], "-h", 2)) {
		SAMPLE_TSCV_Usage(argv[0]);
		return TS_SUCCESS;
	}

#ifndef __TsingLite__
	signal(SIGINT, SAMPLE_TSCV_HandleSig);
	signal(SIGTERM, SAMPLE_TSCV_HandleSig);
#endif
	
	s32Ret = SAMPLE_SYS_Init();
	if (s32Ret != TS_SUCCESS) {
		SAMPLE_PRT("SAMPLE_SYS_Init failed, s32Ret:%d\n", s32Ret);
		return -1;
	}

	s32Index = atoi(argv[1]);
	SAMPLE_PRT("cve ------------------s32Index<%d>--------\n", s32Index);
	switch (s32Index) {
	case 0:
		s32Ret = CalcHistCore();
		break;
	case 1:
		s32Ret = AddCore();
		break;
	case 2:
		s32Ret = SubtractCore();
		break;
	case 3:
		s32Ret = BitwiseAndCore();
		break;
	case 4:
		s32Ret = BitwiseOrCore();
		break;
	case 5:
		s32Ret = BitwiseXorCore();
		break;
	case 6:
		s32Ret = MapCore();
		break;
	case 7:
		s32Ret = ThresholdCore();
		break;
	case 8:
		s32Ret = ConvertToCore();
		break;
		
	default:
		SAMPLE_PRT("the index %d is invaild!\n", s32Index);
		SAMPLE_TSCV_Usage(argv[0]);
		s32Ret = TS_FAILURE;
		break;
	}

	if (TS_SUCCESS == s32Ret)
		SAMPLE_PRT("program exit normally!\n");
	else
		SAMPLE_PRT("program exit abnormally!\n");

	SAMPLE_SYS_Exit();
	return s32Ret;
}

TS_S32 multi_task_core(int argc, char *argv[], int init_flag, int exit_flag)
{
	TS_S32 s32Ret = TS_FAILURE;
	TS_S32 s32Index;

	if (argc < 2 || argc > 2) {
		SAMPLE_TSCV_Usage(argv[0]);
		return TS_FAILURE;
	}

	if (!strncmp(argv[1], "-h", 2)) {
		SAMPLE_TSCV_Usage(argv[0]);
		return TS_SUCCESS;
	}

#ifndef __TsingLite__
	signal(SIGINT, SAMPLE_TSCV_HandleSig);
	signal(SIGTERM, SAMPLE_TSCV_HandleSig);
#endif

	if (init_flag) {
		s32Ret = SAMPLE_SYS_Init();
		if (s32Ret != TS_SUCCESS) {
			SAMPLE_PRT("SAMPLE_SYS_Init failed, s32Ret:%d\n", s32Ret);
			return -1;
		}
	}

	s32Index = atoi(argv[1]);
	SAMPLE_PRT("cve ------------------s32Index<%d>--------\n", s32Index);
	switch (s32Index) {
	case 0:
		s32Ret = CallCoreFunction(CalcHistCore);
		break;
	case 1:
		s32Ret = CallCoreFunction(AddCore);
		break;
	case 2:
		s32Ret = CallCoreFunction(SubtractCore);
		break;
	case 3:
		s32Ret = CallCoreFunction(BitwiseAndCore);
		break;
	case 4:
		s32Ret = CallCoreFunction(BitwiseOrCore);
		break;
	case 5:
		s32Ret = CallCoreFunction(BitwiseXorCore);
		break;
	case 6:
		s32Ret = CallCoreFunction(MapCore);
		break;
	case 7:
		s32Ret = CallCoreFunction(ThresholdCore);
		break;
	case 8:
		s32Ret = CallCoreFunction(ConvertToCore);
		break;
	default:
		SAMPLE_PRT("the index %d is invaild!\n", s32Index);
		SAMPLE_TSCV_Usage(argv[0]);
		s32Ret = TS_FAILURE;
		break;
	}

	if (TS_SUCCESS == s32Ret)
		SAMPLE_PRT("program exit normally!\n");
	else
		SAMPLE_PRT("program exit abnormally!\n");

	if (exit_flag)
		SAMPLE_SYS_Exit();

	return s32Ret;
}

int main(int argc, char *argv[])
{
	const char *new_argv[3];
	char *parm_argv[3];
	char str_i[10];
	//TS_U32 u32Pid = 0;

	// TS_MPI_SYS_GetPid(&u32Pid);
	// if (argc < 3) {
	// 	if(TX5368_AV200 == u32Pid)
	// 		SAMPLE_PRT("Usage: %s <mode> [ Use 'simple' or 'multi']  [num_children] \n", argv[0]);
	// 	else
	// 		SAMPLE_PRT("Usage: %s <mode> [ Use 'simple']  [num_children] \n", argv[0]);
	// 	return 1;
	// }
	if (argc < 3) {
		SAMPLE_PRT("Usage: %s <mode> [ Use 'simple' or 'multi']  [num_children] \n", argv[0]);
		return 1;
	}

	if (strcmp(argv[1], "simple") == 0) {
		// If "simple", execute simple_task_core

		int num_index = atoi(argv[2]);
		sprintf(str_i, "%d", num_index);
		new_argv[0] = "simple_task_core";
		new_argv[1] = str_i;
		new_argv[2] = NULL;

		parm_argv[0] = const_cast<char *>(new_argv[0]);
		parm_argv[1] = const_cast<char *>(new_argv[1]);
		parm_argv[2] = NULL;

		return simple_task_core(2, parm_argv);
	} else if (strcmp(argv[1], "multi") == 0 ) {	// && TX5368_AV200 == u32Pid
		// If "multi", execute multi_task_core
		int num_children = 3; // Default number of child processes
		int num_childrn_count = 0;
		pid_t child_pids[20];
		if (argc >= 3) {
			num_children = atoi(argv[2]); // Get number of child processes from command line argument
			SAMPLE_PRT(" num_children  %d \n", num_children);
			if (num_children <= 0) {
				SAMPLE_PRT("Invalid number of children. Please provide a positive integer.\n");
				return 1;
			}
		}

		for (int i = 0; i < num_children; i++) {
			pid_t pid = fork();
			if (pid == 0) { // Child process
				for (int j = 0; j < 9; j++) {
					sprintf(str_i, "%d", j);
					new_argv[0] = "multi_task_core";
					new_argv[1] = str_i;
					new_argv[2] = NULL;

					parm_argv[0] = const_cast<char *>(new_argv[0]);
					parm_argv[1] = const_cast<char *>(new_argv[1]);
					parm_argv[2] = NULL;

					if (j == 0)
						multi_task_core(2, parm_argv, 1, 0);
					else if (j >= (21 - 1))
						multi_task_core(2, parm_argv, 0, 1);
					else
						multi_task_core(2, parm_argv, 0, 0);
				}
				_exit(0);
			} else if (pid > 0) {
				child_pids[i] =
					pid; // wait(NULL); //Parent process waits for each child process to finish
				num_childrn_count = num_childrn_count + 1;
				log_message("+++++++++++++++++ create  child_pids[%d] : +++++++++++++++++\n", i,
					    child_pids[i]);
			} else {
				perror("fork");
				return 1;
			}
		}
		sleep(15);
		for (int i = 0; i < num_childrn_count; i++) {
			log_message(" +++++++++++++++++ wait child out all %d +++++++++++++++++ \n ",
				    num_childrn_count);
			pid_t result = waitpid(child_pids[i], NULL, 0);
			if (result == -1) {
				// 错误处理
			} else {
				log_message(" +++++++++++++++++ child [%d]  out   +++++++++++++++++ \n ", result);
			}
		}
	}else {
		// if(TX5368_AV200 == u32Pid)
		// 	SAMPLE_PRT("Invalid mode. Use 'simple' or 'multi'.\n");
		// else
		// 	SAMPLE_PRT("Invalid mode. Use 'simple'.\n");
		SAMPLE_PRT("Invalid mode. Use 'simple' or 'multi'.\n");
		return 1;
	}

	return 0;
}