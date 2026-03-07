
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include "ts_type.h"
#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vgs.h"
#include "ts_math.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <getopt.h>  // getopt_long,optarg...
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h> 


#include "tscv.hpp"

#include "opencv2/opencv.hpp"

#define SAMPLE_PRT(fmt...)   \
	do {\
		printf("[%s]-%d: ", __func__, __LINE__);\
		printf(fmt);\
	} while (0)

typedef struct _VPSS_THREAD_PARA_S {
	TS_BOOL bThreadStart;
	void* vpssHandle;
} VPSS_THREAD_PARA_S;
#define MAX_FRAMES 1024

void printCallers()
{
    int layers = 0, i = 0;
    char ** symbols = NULL;
    
    void * frames[MAX_FRAMES];
    memset(frames, 0, sizeof(frames));
    layers = backtrace(frames, MAX_FRAMES);
    for (i=0; i<layers; i++) {
        printf("Layer %d: %p\n", i, frames[i]);
    }
    printf("------------------\n");
    
    symbols = backtrace_symbols(frames, layers);
    if (symbols) {
        for (i=0; i<layers; i++) {
            printf("SYMBOL layer %d: %s\n", i, symbols[i]);
        }
         free(symbols);
    }
    else {
        printf("Failed to parse function names\n");
    }
}

void signal_handler(int signo)  
{  
    printf("\n=========>>>catch signal %d <<<=========\n", signo);  
      
    printf("Dump stack start...\n");  
    printCallers();  
    printf("Dump stack end...\n"); 

    signal(signo, SIG_DFL); /* 恢复信号默认处理 */  
    raise(signo);           /* 重新发送信号 */ 
}


void log_message(const char* format, ...) {
    va_list args;
    va_start(args, format);

    printf("Process [%d]: ", getpid());
    vprintf(format, args);
    printf("\n");

    va_end(args);
}

TS_FLOAT SAMPLE_TSCV_GetBitSizeByFmt(tscv::tsImgType pixel_fmt)
{
	TS_FLOAT fVal = 0;
	switch (pixel_fmt)
	{
	case tscv::TS_IMGTYPE_ARGB8888:
	case tscv::TS_IMGTYPE_ABGR8888:
	case tscv::TS_IMGTYPE_BGRA8888:
	case tscv::TS_IMGTYPE_RGBA8888:
		fVal = 4.0;
		break;
    case tscv::TS_IMGTYPE_YUV422_YUYV:
	case tscv::TS_IMGTYPE_YUV422_YVYU:
	case tscv::TS_IMGTYPE_YUV422_UYVY:
	case tscv::TS_IMGTYPE_YUV422_VYUY:
    case tscv::TS_IMGTYPE_YUV422SP_NV12:
    case tscv::TS_IMGTYPE_YUV422SP_NV21:
		fVal = 2.0;
		break;
	case tscv::TS_IMGTYPE_YUV420SP_NV12:
	case tscv::TS_IMGTYPE_YUV420SP_NV21:
		fVal = 1.5;
		break;
	default:
		break;
	}
	return fVal;
}

/******************************************************************************
* function : vb init & MPI system init
******************************************************************************/
TS_S32 SAMPLE_SYS_Init(VB_CONFIG_S* pstVbConfig)
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

    return s32Ret;
}

/******************************************************************************
* function : vb exit & MPI system exit
******************************************************************************/
TS_VOID SAMPLE_SYS_Exit(void)
{
    /*SAMPLE_COMM_VO_Exit();*/	/* only for debug */
    TS_MPI_SYS_Exit();
//    TS_MPI_VB_ExitModCommPool(VB_UID_USER);
    TS_MPI_VB_Exit();
    return;
}

TS_S32 SAMPLE_TSCV_ResizeVpss(void* vpssHandle)
{
    TS_CHAR szInFileName[128] = "data/1080p_nv12.raw";
    TS_CHAR szOutFileName[128] = "out/out_file_cv_resize";
    TS_CHAR szOutFileNameWithPid[128];
    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = 640;
    TS_S32 dst_h = 360;
    cv::Size srcSize(src_w, src_h*3/2);
    cv::Size dstSize(dst_w, dst_h);
    cv::Mat yuvIn(srcSize, CV_8UC1);
    cv::Mat yuvOut(dst_h*3/2, dst_w, CV_8UC1);
    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_YUV420SP_NV12);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_YUV420SP_NV12);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

    yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
    yuvOut.data = (uchar*)yuvOut.mpi_viraddr;

    //加载测试数据
    FILE *pFileRead = fopen(szInFileName, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", szInFileName);
        return -1;
    }
    fread((char*)yuvIn.data, 1, insize, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;

    tscv::resizeVpss(vpssHandle, yuvIn, yuvOut, dstSize);/**参数与初始化冲突时，内部会重新初始化，时间消耗会变大**/
    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    for(int i=0; i<10; i++) {
        gettimeofday(&tmStart, NULL);
        tscv::resizeVpss(vpssHandle, yuvIn, yuvOut, dstSize);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::resizeVpss Nv12 1920x1080 to Nv12 640*360 time cost: %dus\n", nTmCost);
    }
    

    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d.yuv", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_YUV420SP_NV12);
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        TS_S32 ret = fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
        printf("success to save out file, ret=%d\n", ret);
    }

    // 释放vb
    TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);

	yuvIn.data = 0;
	yuvOut.data = 0;
	yuvIn.mpi_phyaddr = 0;
	yuvOut.mpi_phyaddr = 0;
	yuvIn.mpi_viraddr = 0;
	yuvOut.mpi_viraddr = 0;

	return 0;
}


static TS_VOID *pThreadDealCvtColorVpss(TS_VOID *ptemp)
{
	// TS_S32 s32Ret = TS_FAILURE;
	VPSS_THREAD_PARA_S *p_param = (VPSS_THREAD_PARA_S *)ptemp;
	if(!p_param || !p_param->vpssHandle)
		return NULL;

    TS_CHAR szInFileName[128] = "data/1080p_nv12.raw";
    TS_CHAR szOutFileName[128] = "out/out_file_cv_cvtColor_nv12_argb";
    TS_CHAR szOutFileNameWithPid[128];
    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = 1920;
    TS_S32 dst_h = 1080;
    cv::Size srcSize(src_w, src_h*3/2);
    cv::Size dstSize(dst_w, dst_h);
    cv::Mat yuvIn(srcSize, CV_8UC1);
    cv::Mat yuvOut(dst_h, dst_w, CV_8UC4);
    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_YUV420SP_NV12);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_ARGB8888);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return NULL;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return NULL;
    }


    yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
    yuvOut.data = (uchar*)yuvOut.mpi_viraddr;

    //加载测试数据
    FILE *pFileRead = fopen(szInFileName, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", szInFileName);
        return NULL;
    }
    fread((char*)yuvIn.data, 1, insize, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;

    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    tscv::cvtColorVpss(p_param->vpssHandle, yuvIn, yuvOut, tscv::TS_YUV420SP2ARGB);/**参数与初始化冲突时，内部会重新初始化，时间消耗会变大**/
	for(int i=0;i<10;i++)
	{
		tscv::cvtColorVpss(p_param->vpssHandle, yuvIn, yuvOut, tscv::TS_YUV420SP2ARGB);
	}
    gettimeofday(&tmStart, NULL);
    tscv::cvtColorVpss(p_param->vpssHandle, yuvIn, yuvOut, tscv::TS_YUV420SP2ARGB);
    gettimeofday(&tmEnd, NULL);
    nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
    printf("tscv::cvtColorVpss Nv12 1920x1080 to argb 1920*1080 time cost: %dus\n", nTmCost);

    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d.yuv", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_ARGB8888);
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        TS_S32 ret = fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
        printf("success to save out file, ret=%d\n", ret);
    }

    // 释放vb
    TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);
	yuvIn.data = 0;
	yuvOut.data = 0;
	yuvIn.mpi_phyaddr = 0;
	yuvOut.mpi_phyaddr = 0;
	yuvIn.mpi_viraddr = 0;
	yuvOut.mpi_viraddr = 0;


	p_param->bThreadStart = TS_FALSE;
	printf("thread exec over\n");

	return NULL;
}


TS_S32 SAMPLE_TSCV_MultiThread_ResizeAndCvtColorVpss(void* vpssResizeHandle)
{
    //sub thread run cvtColor
    pthread_t pthreadId = 0;
    cv::Size srcSize(1920, 1080);
    cv::Size dstSize(1920, 1080);
    tscv::tsImgType srcType = tscv::TS_IMGTYPE_YUV420SP_NV12, dstType = tscv::TS_IMGTYPE_ARGB8888;
    void* vpssCvtColorHandle = tscv::createVpss(srcSize, dstSize, srcType, dstType);
    if(vpssCvtColorHandle == NULL)
	{
	    SAMPLE_PRT("%s %d createVpss exec abnormally, no resource left!\n",__func__, __LINE__);
	    return -1;
	}
	//start cvtColor thread
	VPSS_THREAD_PARA_S pthreadPara;
	pthreadPara.bThreadStart = TS_TRUE;
	pthreadPara.vpssHandle = vpssCvtColorHandle;
	//thread run cvtColor 1000 counts
	pthread_create(&pthreadId, 0, pThreadDealCvtColorVpss, (TS_VOID *)&pthreadPara);

    //main thread run resize
    if(1)
    {
	    TS_CHAR szInFileName[128] = "data/1080p_nv12.raw";
	    TS_CHAR szOutFileName[128] = "out/out_file_cv_resize";
	    TS_CHAR szOutFileNameWithPid[128];
	    TS_S32 src_w = 1920;
	    TS_S32 src_h = 1080;
	    TS_S32 dst_w = 640;
	    TS_S32 dst_h = 360;
	    cv::Size srcSize(src_w, src_h*3/2);
	    cv::Size dstSize(dst_w, dst_h);
	    cv::Mat yuvIn(srcSize, CV_8UC1);
	    cv::Mat yuvOut(dst_h*3/2, dst_w, CV_8UC1);
	    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_YUV420SP_NV12);
        TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_YUV420SP_NV12);
		TS_S32 ret = 0;
		
        ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
		if(ret != 0){
			printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
			return -1;
		}
        ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
		if(ret != 0){
			printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
			return -1;
		}

	    yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
	    yuvOut.data = (uchar*)yuvOut.mpi_viraddr;

	    //加载测试数据
	    FILE *pFileRead = fopen(szInFileName, "rb");
	    if (TS_NULL == pFileRead)
	    {
	        SAMPLE_PRT("can't open file %s\n", szInFileName);
	        return -1;
	    }
	    fread((char*)yuvIn.data, 1, insize, pFileRead);
	    fclose(pFileRead);
	    pFileRead = NULL;

	    struct timeval tmStart, tmEnd;
        int nTmCost = 0;
	    tscv::resizeVpss(vpssResizeHandle, yuvIn, yuvOut, dstSize);/**参数与初始化冲突时，内部会重新初始化，时间消耗会变大**/
		for(int i=0;i<10;i++)
		{
			tscv::resizeVpss(vpssResizeHandle, yuvIn, yuvOut, dstSize);
		}
	    gettimeofday(&tmStart, NULL);
	    tscv::resizeVpss(vpssResizeHandle, yuvIn, yuvOut, dstSize);
	    gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::resizeVpss Nv12 1920x1080 to Nv12 640*360 time cost: %dus\n", nTmCost);

	    //保存测试数据
	    sprintf(szOutFileNameWithPid, "%s_%d.yuv", (TS_CHAR*)szOutFileName, getpid());
	    pFileRead = fopen(szOutFileNameWithPid, "wb");
	    if (TS_NULL != pFileRead) {
	        TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_YUV420SP_NV12);
	        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
	        TS_S32 ret = fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
	        fclose(pFileRead);
	        printf("success to save out file, ret=%d\n", ret);
	    }

	    // 释放vb
        TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
        TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);

		yuvIn.data = 0;
		yuvOut.data = 0;
		yuvIn.mpi_phyaddr = 0;
		yuvOut.mpi_phyaddr = 0;
		yuvIn.mpi_viraddr = 0;
		yuvOut.mpi_viraddr = 0;
    }

	//clean sub thread handle
	if (TS_TRUE == pthreadPara.bThreadStart) {
		pthread_join(pthreadId, 0);
	}
	tscv::exitVpss(vpssCvtColorHandle);
	return 0;
}


TS_S32 SAMPLE_TSCV_ResizeVpss_argb(void* vpssHandle)
{
    TS_CHAR szInFileName[128] = "data/1080p_argb.raw";
    TS_CHAR szOutFileName[128] = "out/out_file_cv_resize_argb";
    TS_CHAR szOutFileNameWithPid[128];
    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = 640;
    TS_S32 dst_h = 360;
    cv::Size srcSize(src_w, src_h);
    cv::Size dstSize(dst_w, dst_h);
    cv::Mat yuvIn(srcSize, CV_8UC4);
    cv::Mat yuvOut(dstSize, CV_8UC4);
    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_ARGB8888);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_ARGB8888);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

    yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
    yuvOut.data = (uchar*)yuvOut.mpi_viraddr;

    //加载测试数据
    FILE *pFileRead = fopen(szInFileName, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", szInFileName);
        return -1;
    }
    fread((char*)yuvIn.data, 1, insize, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;


    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    tscv::resizeVpss(vpssHandle, yuvIn, yuvOut, dstSize,tscv::TS_IMGTYPE_ARGB8888);/**参数与初始化冲突时，内部会重新初始化，时间消耗会变大**/
    // get_time_ms(time_start, sizeof(time_start));
    // tscv::resizeVpss(vpssHandle, yuvIn, yuvOut, dstSize,tscv::TS_IMGTYPE_ARGB8888);
    // get_time_ms(time_end, sizeof(time_end));
    // printf("tscv::resizeVpss argb 1920x1080 to argb 640*360\n");
    // printf("soft time start:%s\n", time_start);
    // printf("soft time   end:%s\n", time_end);

    for(int i=0; i<10; i++) {
        gettimeofday(&tmStart, NULL);
        tscv::resizeVpss(vpssHandle, yuvIn, yuvOut, dstSize,tscv::TS_IMGTYPE_ARGB8888);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::resizeVpss argb 1920x1080 to argb 640*360 time cost: %dus\n", nTmCost);
    }

    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d.yuv", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_ARGB8888);
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        TS_S32 ret = fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
        printf("success to save out file, ret=%d\n", ret);
    }

    // 释放vb
    TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);

	yuvIn.data = 0;
	yuvOut.data = 0;
	yuvIn.mpi_phyaddr = 0;
	yuvOut.mpi_phyaddr = 0;
	yuvIn.mpi_viraddr = 0;
	yuvOut.mpi_viraddr = 0;

    return 0;
}

TS_S32 SAMPLE_TSCV_CvtColorVpss_nv12_argb(void* vpssHandle)
{
    TS_CHAR szInFileName[128] = "data/1080p_nv12.raw";
    TS_CHAR szOutFileName[128] = "out/out_file_cv_cvtColor_nv12_argb";
    TS_CHAR szOutFileNameWithPid[128];
    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = 1920;
    TS_S32 dst_h = 1080;
    cv::Size srcSize(src_w, src_h*3/2);
    cv::Size dstSize(dst_w, dst_h);
    cv::Mat yuvIn(srcSize, CV_8UC1);
    cv::Mat yuvOut(dst_h, dst_w, CV_8UC4);
    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_YUV420SP_NV12);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_ARGB8888);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

    yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
    yuvOut.data = (uchar*)yuvOut.mpi_viraddr;

    //加载测试数据
    FILE *pFileRead = fopen(szInFileName, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", szInFileName);
        return -1;
    }
    fread((char*)yuvIn.data, 1, insize, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;

    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    tscv::cvtColorVpss(vpssHandle, yuvIn, yuvOut, tscv::TS_YUV420SP2ARGB);/**参数与初始化冲突时，内部会重新初始化，时间消耗会变大**/
    for(int i=0; i<10; i++) {
        gettimeofday(&tmStart, NULL);
        tscv::cvtColorVpss(vpssHandle, yuvIn, yuvOut, tscv::TS_YUV420SP2ARGB);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::cvtColorVpss Nv12 1920x1080 to argb 1920*1080 time cost: %dus\n", nTmCost);
    }

    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d.yuv", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_ARGB8888);
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        TS_S32 ret = fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
        printf("success to save out file, ret=%d\n", ret);
    }

    // 释放vb
    TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);

	yuvIn.data = 0;
	yuvOut.data = 0;
	yuvIn.mpi_phyaddr = 0;
	yuvOut.mpi_phyaddr = 0;
	yuvIn.mpi_viraddr = 0;
	yuvOut.mpi_viraddr = 0;


	return 0;
}


TS_S32 SAMPLE_TSCV_CvtColorVpss_nv12_rgba(void* vpssHandle)
{
    TS_CHAR szInFileName[128] = "data/1080p_nv12.raw";
    TS_CHAR szOutFileName[128] = "out/out_file_cv_cvtColor_nv12_rgba";
    TS_CHAR szOutFileNameWithPid[128];
    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = 1920;
    TS_S32 dst_h = 1080;
    cv::Size srcSize(src_w, src_h*3/2);
    cv::Size dstSize(dst_w, dst_h);
    cv::Mat yuvIn(srcSize, CV_8UC1);
    cv::Mat yuvOut(dst_h, dst_w, CV_8UC4);
    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_YUV420SP_NV12);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_RGBA8888);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

    yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
    yuvOut.data = (uchar*)yuvOut.mpi_viraddr;

    //加载测试数据
    FILE *pFileRead = fopen(szInFileName, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", szInFileName);
        return -1;
    }
    fread((char*)yuvIn.data, 1, insize, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;

    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    /**参数与初始化冲突时，内部会重新初始化，时间消耗会变大**/
    tscv::cvtColorVpss(vpssHandle, yuvIn, yuvOut, tscv::TS_YUV420SP2RGBA);
    for(int i=0; i<10; i++) {
        gettimeofday(&tmStart, NULL);
        tscv::cvtColorVpss(vpssHandle, yuvIn, yuvOut, tscv::TS_YUV420SP2RGBA);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::cvtColorVpss Nv12 1920x1080 to rgba 1920*1080 time cost: %dus\n", nTmCost);
    }

    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d.yuv", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_RGBA8888);
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        TS_S32 ret = fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
        printf("success to save out file, ret=%d\n", ret);
    }

    // 释放vb
    TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);

	yuvIn.data = 0;
	yuvOut.data = 0;
	yuvIn.mpi_phyaddr = 0;
	yuvOut.mpi_phyaddr = 0;
	yuvIn.mpi_viraddr = 0;
	yuvOut.mpi_viraddr = 0;


	return 0;
}

TS_S32 SAMPLE_TSCV_CvtColorVpss_argb_nv12(void* vpssHandle)
{
    TS_CHAR szInFileName[128] = "data/1080p_argb.raw";
    TS_CHAR szOutFileName[128] = "out/out_file_cv_cvtColor_argb_nv12";
    TS_CHAR szOutFileNameWithPid[128];
    TS_S32 nw = 1920;
    TS_S32 nh = 1080;
    cv::Mat yuvIn(nh, nw, CV_8UC4);
    cv::Mat yuvOut(nh*3/2, nw, CV_8UC1);

    TS_S32 insize = nw*nh*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_ARGB8888);
    TS_S32 outsize = nw*nh*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_YUV420SP_NV12);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

    yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
    yuvOut.data = (uchar*)yuvOut.mpi_viraddr;

    //加载测试数据
    FILE *pFileRead = fopen(szInFileName, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", szInFileName);
        return -1;
    }
    fread((char*)yuvIn.data, 1, insize, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;

    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    tscv::cvtColorVpss(vpssHandle, yuvIn, yuvOut, tscv::TS_ARGB2YUV420SP);/**参数与初始化冲突时，内部会重新初始化，时间消耗会变大**/
    for(int i=0; i<10; i++) {
        gettimeofday(&tmStart, NULL);
        tscv::cvtColorVpss(vpssHandle, yuvIn, yuvOut, tscv::TS_ARGB2YUV420SP);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::cvtColorVpss argb 1920x1080 to Nv12 1920*1080 time cost: %dus\n", nTmCost);
    }

    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d.yuv", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        TS_S32 outsize = nw*nh*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_YUV420SP_NV12);
        printf("out file w=%d h=%d, size=%d ptr=%p\n", nw, nh, outsize, (void*)yuvOut.data);
        TS_S32 ret = fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
        printf("success to save out file, ret=%d\n", ret);
    }

    // 释放vb
    TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);

	yuvIn.data = 0;
	yuvOut.data = 0;
	yuvIn.mpi_phyaddr = 0;
	yuvOut.mpi_phyaddr = 0;
	yuvIn.mpi_viraddr = 0;
	yuvOut.mpi_viraddr = 0;


	return 0;
}

TS_S32 SAMPLE_TSCV_ResizeCscVpss_nv12_argb(void* vpssHandle)
{
    TS_CHAR szInFileName[128] = "data/1080p_nv12.raw";
    TS_CHAR szOutFileName[128] = "out/out_file_cv_resize_csc_nv12_argb";
    TS_CHAR szOutFileNameWithPid[128];
    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = 640;
    TS_S32 dst_h = 360;
    cv::Size dstSize(640, 360);
    tscv::tsImgType src_fmt = tscv::TS_IMGTYPE_YUV420SP_NV12;
    tscv::tsImgType dst_fmt = tscv::TS_IMGTYPE_ARGB8888;

    cv::Mat yuvIn(src_h*3/2, src_w, CV_8UC1);
    cv::Mat yuvOut(dst_h, dst_w, CV_8UC4);

    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(src_fmt);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(dst_fmt);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

    yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
    yuvOut.data = (uchar*)yuvOut.mpi_viraddr;

    //加载测试数据
    FILE *pFileRead = fopen(szInFileName, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", szInFileName);
        return -1;
    }
    fread((char*)yuvIn.data, 1, insize, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;

    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    tscv::resizeCvtColorVpss(vpssHandle, yuvIn, yuvOut, dstSize, tscv::TS_YUV420SP2ARGB);/**参数与初始化冲突时，内部会重新初始化，时间消耗会变大**/
    
    for(int i=0; i<10; i++) {
        gettimeofday(&tmStart, NULL);
        tscv::resizeCvtColorVpss(vpssHandle, yuvIn, yuvOut, dstSize, tscv::TS_YUV420SP2ARGB);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::resizeCvtColorVpss Nv12 1920x1080 to argb 640*360 time cost: %dus\n", nTmCost);
    }

    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d.yuv", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(dst_fmt);
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        TS_S32 ret = fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
        printf("success to save out file, ret=%d\n", ret);
    }

    // 释放vb
    TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);

	yuvIn.data = 0;
	yuvOut.data = 0;
	yuvIn.mpi_phyaddr = 0;
	yuvOut.mpi_phyaddr = 0;
	yuvIn.mpi_viraddr = 0;
	yuvOut.mpi_viraddr = 0;


	return 0;
}

TS_S32 SAMPLE_TSCV_ResizeCscVpss_argb_nv12(void* vpssHandle)
{
    TS_CHAR szInFileName[128] = "data/1080p_argb.raw";
    TS_CHAR szOutFileName[128] = "out/out_file_cv_resize_csc_argb_nv12";
    TS_CHAR szOutFileNameWithPid[128];
    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = 640;
    TS_S32 dst_h = 360;
    cv::Size dstSize(640, 360);
    tscv::tsImgType src_fmt = tscv::TS_IMGTYPE_ARGB8888;
    tscv::tsImgType dst_fmt = tscv::TS_IMGTYPE_YUV420SP_NV12;

    cv::Mat yuvIn(src_h, src_w, CV_8UC4);
    cv::Mat yuvOut(dst_h*3/2, dst_w, CV_8UC1);

    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(src_fmt);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(dst_fmt);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

    yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
    yuvOut.data = (uchar*)yuvOut.mpi_viraddr;

    //加载测试数据
    FILE *pFileRead = fopen(szInFileName, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", szInFileName);
        return -1;
    }
    fread((char*)yuvIn.data, 1, insize, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;

    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    tscv::resizeCvtColorVpss(vpssHandle, yuvIn, yuvOut, dstSize, tscv::TS_ARGB2YUV420SP);/**参数与初始化冲突时，内部会重新初始化，时间消耗会变大**/
    
    for(int i=0; i<10; i++) {
        gettimeofday(&tmStart, NULL);
        tscv::resizeCvtColorVpss(vpssHandle, yuvIn, yuvOut, dstSize, tscv::TS_ARGB2YUV420SP);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::resizeCvtColorVpss argb 1920x1080 to Nv12 640*360 time cost: %dus\n", nTmCost);
    }

    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d.yuv", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(dst_fmt);
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        TS_S32 ret = fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
        printf("success to save out file, ret=%d\n", ret);
    }

    // 释放vb
    TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);

	yuvIn.data = 0;
	yuvOut.data = 0;
	yuvIn.mpi_phyaddr = 0;
	yuvOut.mpi_phyaddr = 0;
	yuvIn.mpi_viraddr = 0;
	yuvOut.mpi_viraddr = 0;


	return 0;
}

TS_S32 SAMPLE_TSCV_Padding_nv12(void* vpssHandle)
{
    TS_CHAR szInFileName[128] = "data/1080p_nv12.raw";
    TS_CHAR szOutFileName[128] = "out/out_file_cv_padding";
    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    cv::Size srcSize(src_w, src_h*3/2);	
    TS_S32 dst_w = 640;
    TS_S32 dst_h = 360;
	
	TS_S32 effective_w = 640;
    TS_S32 effective_h = 360;

	TS_S32 padding_num = 0;

	for(int i = 0;i < 2;i++)//
		for(int j = 0;j < 2;j++)
			for(int z = 0;z < 6;z++)
			{
				int padding_w = 0;
				int padding_h = 0;
				int padding_color = 0;
				
				if(i == 0)
				{
					padding_w = z*16;
					padding_h = 0;
					dst_w = 640 + padding_w*2;
					dst_h = 360;
					if(j == 0)
						padding_color = 0xffffff;
					else
						padding_color = 0;
				}
				else
				{
					padding_w = 0;
					padding_h = 16*z;
					dst_w = 640;
					dst_h = 360 + padding_h*2;
					if(j == 0)
						padding_color = 0xffffff;
					else
						padding_color = 0;
				}
				padding_num ++;

				TS_S32 ret = 0;
				cv::Size dstSize(dst_w, dst_h);	
				cv::Size effectiveSize(effective_w, effective_h);
			    cv::Mat yuvIn(srcSize, CV_8UC1);
			    cv::Mat yuvOut(dst_h*3/2, dst_w, CV_8UC1);
				TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_YUV420SP_NV12);
				TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_YUV420SP_NV12);
				
				ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
				if(ret != 0){
					printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
					return -1;
				}
				ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
				if(ret != 0){
					printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
					return -1;
				}
				
				yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
				yuvOut.data = (uchar*)yuvOut.mpi_viraddr;
				
				//加载测试数据
				FILE *pFileRead = fopen(szInFileName, "rb");
				if (TS_NULL == pFileRead)
				{
					SAMPLE_PRT("can't open file %s\n", szInFileName);
					return -1;
				}
				fread((char*)yuvIn.data, 1, insize, pFileRead);
				fclose(pFileRead);
				pFileRead = NULL;
				
			    tscv::paddingVpss(vpssHandle, yuvIn, yuvOut, dstSize,effectiveSize,padding_color);/**参数与初始化冲突时，内部会重新初始化，时间消耗会变大**/
				struct timeval tmStart, tmEnd;
				int nTmCost = 0;
				for(int i=0; i<10; i++) {
					gettimeofday(&tmStart, NULL);
					tscv::paddingVpss(vpssHandle, yuvIn, yuvOut, dstSize,effectiveSize,padding_color);/**参数与初始化冲突时，内部会重新初始化，时间消耗会变大**/
					gettimeofday(&tmEnd, NULL);
					nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
					printf("tscv::resizeVpss Nv12 1920x1080 to Nv12 %d*%d time cost: %dus\n", dst_w,dst_h,nTmCost);
				}
						
				//保存测试数据
				sprintf(szOutFileName, "out/out_file%d*%d_cv_padding_%d_%d.yuv", dst_w,dst_h,padding_num,getpid());
				pFileRead = fopen(szOutFileName, "wb");
				if (TS_NULL != pFileRead) {
					TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_YUV420SP_NV12);
					printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
					TS_S32 ret = fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
					fclose(pFileRead);
					printf("success to save out file, ret=%d\n", ret);
				}
				
				// 释放vb
				TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
				TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);
				
				yuvIn.data = 0;
				yuvOut.data = 0;
				yuvIn.mpi_phyaddr = 0;
				yuvOut.mpi_phyaddr = 0;
				yuvIn.mpi_viraddr = 0;
				yuvOut.mpi_viraddr = 0;
			}
    
	return 0;

}

TS_S32 SAMPLE_TSCV_Padding_argb(void* vpssHandle)
{
	return 0;
}

TS_S32 SAMPLE_TSCV_Padding_scale_argb(void* vpssHandle)
{
	return 0;
}

TS_S32 SAMPLE_TSCV_Crop_argb(void* vpssHandle)
{
    TS_CHAR szInFileName[128] = "data/1080p_argb.raw";
    TS_CHAR szOutFileName[128] = "out/out_file_cv_crop_argb";
    TS_CHAR szOutFileNameWithPid[128];

    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = 640;
    TS_S32 dst_h = 640;

    tscv::tsImgType src_fmt = tscv::TS_IMGTYPE_ARGB8888;
    tscv::tsImgType dst_fmt = tscv::TS_IMGTYPE_ARGB8888;

    cv::Mat yuvIn(src_h, src_w, CV_8UC4);
    cv::Mat yuvOut(dst_h, dst_w, CV_8UC4);
    cv::Rect rect(320, 320, 640, 640);

    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(src_fmt);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(dst_fmt);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

    yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
    yuvOut.data = (uchar*)yuvOut.mpi_viraddr;

    //加载测试数据
    FILE *pFileRead = fopen(szInFileName, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", szInFileName);
        return -1;
    }
    fread((char*)yuvIn.data, 1, insize, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;

    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    tscv::cropVpss(vpssHandle, yuvIn, yuvOut, rect, 0, 0, tscv::TS_IMGTYPE_ARGB8888);/**参数与初始化冲突时，内部会重新初始化，时间消耗会变大**/
    for(int i=0; i<10; i++) {
        gettimeofday(&tmStart, NULL);
        tscv::cropVpss(vpssHandle, yuvIn, yuvOut, rect, 0, 0, tscv::TS_IMGTYPE_ARGB8888);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::cropVpss argb 1920x1080 to argb 640*640 time cost: %dus\n", nTmCost);
    }

    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d.yuv", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(dst_fmt);
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        TS_S32 ret = fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
        printf("success to save out file, ret=%d\n", ret);
    }

    // 释放vb
    TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);

	yuvIn.data = 0;
	yuvOut.data = 0;
	yuvIn.mpi_phyaddr = 0;
	yuvOut.mpi_phyaddr = 0;
	yuvIn.mpi_viraddr = 0;
	yuvOut.mpi_viraddr = 0;


	return 0;
}

TS_S32 SAMPLE_TSCV_Flip_argb(void* vpssHandle)
{
    TS_CHAR szInFileName[128] = "data/1080p_argb.raw";
    TS_CHAR szOutFileName[128] = "out/out_file_cv_flip_argb";
    TS_CHAR szOutFileNameWithPid[128];
    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = 1920;
    TS_S32 dst_h = 1080;

    tscv::tsImgType src_fmt = tscv::TS_IMGTYPE_ARGB8888;
    tscv::tsImgType dst_fmt = tscv::TS_IMGTYPE_ARGB8888;

    cv::Mat yuvIn(src_h, src_w, CV_8UC4);
    cv::Mat yuvOut(dst_h, dst_w, CV_8UC4);

    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(src_fmt);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(dst_fmt);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }


    yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
    yuvOut.data = (uchar*)yuvOut.mpi_viraddr;

    //加载测试数据
    FILE *pFileRead = fopen(szInFileName, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", szInFileName);
        return -1;
    }
    fread((char*)yuvIn.data, 1, insize, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;
    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    tscv::flipVpss(vpssHandle, yuvIn, yuvOut, 0, tscv::TS_IMGTYPE_ARGB8888);/**参数与初始化冲突时，内部会重新初始化，时间消耗会变大**/
    for(int i=0; i<10; i++) {
        gettimeofday(&tmStart, NULL);
        tscv::flipVpss(vpssHandle, yuvIn, yuvOut, 0, tscv::TS_IMGTYPE_ARGB8888);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::flipVpss argb 1920x1080 time cost: %dus\n", nTmCost);
    }

    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d.yuv", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(dst_fmt);
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        TS_S32 ret = fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
        printf("success to save out file, ret=%d\n", ret);
    }

    // 释放vb
    TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);

	yuvIn.data = 0;
	yuvOut.data = 0;
	yuvIn.mpi_phyaddr = 0;
	yuvOut.mpi_phyaddr = 0;
	yuvIn.mpi_viraddr = 0;
	yuvOut.mpi_viraddr = 0;


	return 0;
}

TS_S32 SAMPLE_TSCV_Flip_nv12(void* vpssHandle)
{
    TS_CHAR szInFileName[128] = "data/1080p_nv12.raw";
    TS_CHAR szOutFileName[128] = "out/out_file_cv_flip_nv12";
    TS_CHAR szOutFileNameWithPid[128];

    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = 1920;
    TS_S32 dst_h = 1080;

    tscv::tsImgType src_fmt = tscv::TS_IMGTYPE_YUV420SP_NV12;
    tscv::tsImgType dst_fmt = tscv::TS_IMGTYPE_YUV420SP_NV12;

    cv::Mat yuvIn(src_h*3/2, src_w, CV_8UC1);
    cv::Mat yuvOut(dst_h*3/2, dst_w, CV_8UC1);

    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(src_fmt);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(dst_fmt);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }


    yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
    yuvOut.data = (uchar*)yuvOut.mpi_viraddr;

    //加载测试数据
    FILE *pFileRead = fopen(szInFileName, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", szInFileName);
        return -1;
    }
    fread((char*)yuvIn.data, 1, insize, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;
    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    tscv::flipVpss(vpssHandle, yuvIn, yuvOut, 0, tscv::TS_IMGTYPE_YUV420SP_NV12);/**参数与初始化冲突时，内部会重新初始化，时间消耗会变大**/
    
    for(int i=0; i<10; i++) {
        gettimeofday(&tmStart, NULL);
        tscv::flipVpss(vpssHandle, yuvIn, yuvOut, 0, tscv::TS_IMGTYPE_YUV420SP_NV12);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::flipVpss NV12 1920x1080 time cost: %dus\n", nTmCost);
    }

    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d.yuv", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(dst_fmt);
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        TS_S32 ret = fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
        printf("success to save out file, ret=%d\n", ret);
    }

    // 释放vb
    TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);

	yuvIn.data = 0;
	yuvOut.data = 0;
	yuvIn.mpi_phyaddr = 0;
	yuvOut.mpi_phyaddr = 0;
	yuvIn.mpi_viraddr = 0;
	yuvOut.mpi_viraddr = 0;


	return 0;
}

TS_S32 SAMPLE_TSCV_CropResize_argb(void* vpssHandle)
{
    TS_CHAR szInFileName[128] = "data/1080p_argb.raw";
    TS_CHAR szOutFileName[128] = "out/out_file_cv_cropResize_argb";
    TS_CHAR szOutFileNameWithPid[128];

    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = 320;
    TS_S32 dst_h = 320;

    tscv::tsImgType src_fmt = tscv::TS_IMGTYPE_ARGB8888;
    tscv::tsImgType dst_fmt = tscv::TS_IMGTYPE_ARGB8888;

    cv::Mat yuvIn(src_h, src_w, CV_8UC4);
    cv::Mat yuvOut(dst_h, dst_w, CV_8UC4);
    cv::Rect rect(320, 320, 640, 640);
    cv::Size dsize(320, 320);

    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(src_fmt);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(dst_fmt);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

    yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
    yuvOut.data = (uchar*)yuvOut.mpi_viraddr;

    //加载测试数据
    FILE *pFileRead = fopen(szInFileName, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", szInFileName);
        return -1;
    }
    fread((char*)yuvIn.data, 1, insize, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;

    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    /**参数与初始化冲突时，内部会重新初始化，时间消耗会变大**/
    tscv::cropResizeVpss(vpssHandle, yuvIn, yuvOut, rect, dsize, tscv::TS_IMGTYPE_ARGB8888, tscv::TS_IMGTYPE_ARGB8888);
    
    for(int i=0; i<10; i++) {
        gettimeofday(&tmStart, NULL);
        tscv::cropResizeVpss(vpssHandle, yuvIn, yuvOut, rect, dsize, tscv::TS_IMGTYPE_ARGB8888, tscv::TS_IMGTYPE_ARGB8888);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::cropResizeVpss argb 1920x1080 to argb 320*320 time cost: %dus\n", nTmCost);
    }

    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d.yuv", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(dst_fmt);
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        TS_S32 ret = fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
        printf("success to save out file, ret=%d\n", ret);
    }

    // 释放vb
    TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);

	yuvIn.data = 0;
	yuvOut.data = 0;
	yuvIn.mpi_phyaddr = 0;
	yuvOut.mpi_phyaddr = 0;
	yuvIn.mpi_viraddr = 0;
	yuvOut.mpi_viraddr = 0;


	return 0;
}

TS_S32 SAMPLE_TSCV_CropResize_argb_nv12(void* vpssHandle)
{
    TS_CHAR szInFileName[128] = "data/1080p_argb.raw";
    TS_CHAR szOutFileName[128] = "out/out_file_cv_cropResize_argb_nv12";
    TS_CHAR szOutFileNameWithPid[128];

    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = 320;
    TS_S32 dst_h = 320;

    tscv::tsImgType src_fmt = tscv::TS_IMGTYPE_ARGB8888;
    tscv::tsImgType dst_fmt = tscv::TS_IMGTYPE_YUV420SP_NV12;

    cv::Mat yuvIn(src_h, src_w, CV_8UC4);
    cv::Mat yuvOut(dst_h*3/2, dst_w, CV_8UC1);
    cv::Rect rect(320, 320, 640, 640);
    cv::Size dsize(320, 320);

    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(src_fmt);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(dst_fmt);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }

    yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
    yuvOut.data = (uchar*)yuvOut.mpi_viraddr;

    //加载测试数据
    FILE *pFileRead = fopen(szInFileName, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", szInFileName);
        return -1;
    }
    fread((char*)yuvIn.data, 1, insize, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;


    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    /**参数与初始化冲突时，内部会重新初始化，时间消耗会变大**/
    tscv::cropResizeVpss(vpssHandle, yuvIn, yuvOut, rect, dsize, tscv::TS_IMGTYPE_ARGB8888, tscv::TS_IMGTYPE_YUV420SP_NV12);
    
    for(int i=0; i<10; i++) {
        gettimeofday(&tmStart, NULL);
        tscv::cropResizeVpss(vpssHandle, yuvIn, yuvOut, rect, dsize, tscv::TS_IMGTYPE_ARGB8888, tscv::TS_IMGTYPE_YUV420SP_NV12);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::cropResizeVpss argb 1920x1080 to Nv12 320*320 time cost: %dus\n", nTmCost);
    }

    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d.yuv", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(dst_fmt);
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        TS_S32 ret = fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
        printf("success to save out file, ret=%d\n", ret);
    }

    // 释放vb
    TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);

	yuvIn.data = 0;
	yuvOut.data = 0;
	yuvIn.mpi_phyaddr = 0;
	yuvOut.mpi_phyaddr = 0;
	yuvIn.mpi_viraddr = 0;
	yuvOut.mpi_viraddr = 0;


	return 0;
}

TS_S32 SAMPLE_TSCV_CropResize_nv12_argb(void* vpssHandle)
{
    TS_CHAR szInFileName[128] = "data/1080p_nv12.raw";
    TS_CHAR szOutFileName[128] = "out/out_file_cv_cropResize_nv12_argb";
    TS_CHAR szOutFileNameWithPid[128];

    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = 320;
    TS_S32 dst_h = 320;

    tscv::tsImgType src_fmt = tscv::TS_IMGTYPE_YUV420SP_NV12;
    tscv::tsImgType dst_fmt = tscv::TS_IMGTYPE_ARGB8888;

    cv::Mat yuvIn(src_h*3/2, src_w, CV_8UC1);
    cv::Mat yuvOut(dst_h, dst_w, CV_8UC4);
    cv::Rect rect(320, 320, 640, 640);
    cv::Size dsize(320, 320);

    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(src_fmt);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(dst_fmt);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return -1;
    }


    yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
    yuvOut.data = (uchar*)yuvOut.mpi_viraddr;

    //加载测试数据
    FILE *pFileRead = fopen(szInFileName, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", szInFileName);
        return -1;
    }
    fread((char*)yuvIn.data, 1, insize, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;

    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    /**参数与初始化冲突时，内部会重新初始化，时间消耗会变大**/
    tscv::cropResizeVpss(vpssHandle, yuvIn, yuvOut, rect, dsize, tscv::TS_IMGTYPE_YUV420SP_NV12, tscv::TS_IMGTYPE_ARGB8888);
    
    for(int i=0; i<10; i++) {
        gettimeofday(&tmStart, NULL);
        tscv::cropResizeVpss(vpssHandle, yuvIn, yuvOut, rect, dsize, tscv::TS_IMGTYPE_YUV420SP_NV12, tscv::TS_IMGTYPE_ARGB8888);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::cropResizeVpss Nv12 1920x1080 to argb 320*320 time cost: %dus\n", nTmCost);
    }

    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d.yuv", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(dst_fmt);
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        TS_S32 ret = fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
        printf("success to save out file, ret=%d\n", ret);
    }

    // 释放vb
    TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);

	yuvIn.data = 0;
	yuvOut.data = 0;
	yuvIn.mpi_phyaddr = 0;
	yuvOut.mpi_phyaddr = 0;
	yuvIn.mpi_viraddr = 0;
	yuvOut.mpi_viraddr = 0;

	return 0;
}

TS_U32 SAMPLE_TSCV_OPENCV_TM()
{
    printf("hello\n");
    cv::Mat src_img = cv::imread("data/cars_1024_192.jpg");
    if (src_img.empty()) {
        printf("input is error\n");
        return -1; 
    }
    cv::Mat dest;
    cv::cvtColor(src_img, dest, CV_BGR2RGBA);
    cv::Mat resizeMat;
    cv::Size destsize(512,96);
    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    gettimeofday(&tmStart, NULL);
    cv::resize(dest, resizeMat, destsize);
    gettimeofday(&tmEnd, NULL);
    nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
    printf("opencv resize time cost: %dms\n", nTmCost);
    
    cv::imwrite("./dest.jpg",resizeMat);
    printf("dest size w=%d h=%d\n",dest.size().width, dest.size().height);
    return 0;
}

TS_VOID SAMPLE_TSCV_Usage(TS_CHAR *sPrgNm)
{
	printf("Usage : %s <index>\n", sPrgNm);
	printf("index:\n");
	printf("\t 0) resize   nv12 1080p->360p\n");
    printf("\t 1) resize   argb 1080p->360p\n");
	printf("\t 2) cvtColor nv12->argb\n");
	printf("\t 3) cvtColor argb->nv12\n");
    printf("\t 4) resizeCsc 1080p_nv12->360p_argb\n");
    printf("\t 5) resizeCsc 1080p_argb->360p_nv12\n");
//     printf("\t 6) padding nv12\n");
//     printf("\t 7) padding argb\n");
//     printf("\t 8) padding scale argb\n");
    printf("\t 9) crop argb\n");
    printf("\t 10)flip argb\n");
    printf("\t 11)flip nv12\n");
    printf("\t 12)crop resize argb\n");
    printf("\t 13)crop resizeCsc argb nv12\n");
    printf("\t 14)crop resizeCsc nv12 argb\n");
	printf("\t 15) cvtColor nv12->rgba\n");
	printf("\t 16) multi thread test\n");

}

TS_S32 simple_task_core(int argc, char *argv[])
{
    TS_S32 s32Ret = TS_FAILURE;
	TS_S32 s32Index;
	void* vpssHandle = NULL;

	if (argc < 2 || argc > 2) {
		SAMPLE_TSCV_Usage(argv[0]);
		return TS_FAILURE;
	}

	if (!strncmp(argv[1], "-h", 2)) {
		SAMPLE_TSCV_Usage(argv[0]);
		return TS_SUCCESS;
	}

    VB_CONFIG_S stVbConf;
    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
    stVbConf.u32MaxPoolCnt = 1;
    TS_S32 blk_size = 100*100;
    stVbConf.astCommPool[0].u64BlkSize = blk_size;
    stVbConf.astCommPool[0].u32BlkCnt = 2;

    s32Ret = TS_FAILURE;
    s32Ret = SAMPLE_SYS_Init(&stVbConf);
    if (s32Ret != TS_SUCCESS)
    {
        SAMPLE_PRT("SAMPLE_SYS_Init failed, s32Ret:%d\n", s32Ret);
        return -1;
    }
    
    s32Index = atoi(argv[1]);
     SAMPLE_PRT("vpss ------------------s32Index<%d>--------\n",s32Index);
    cv::Size srcSize(1920, 1080);
    cv::Size dstSize(640, 360);
    tscv::tsImgType srcType = tscv::TS_IMGTYPE_YUV420SP_NV12, dstType = tscv::TS_IMGTYPE_YUV420SP_NV12;
    switch (s32Index) {
    case 0:
        srcType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        dstType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        break;
    case 1:
        srcType = tscv::TS_IMGTYPE_ARGB8888;
        dstType = tscv::TS_IMGTYPE_ARGB8888;
        break;
    case 2:
        srcType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        dstType = tscv::TS_IMGTYPE_ARGB8888;
        break;
    case 3:
        srcType = tscv::TS_IMGTYPE_ARGB8888;
        dstType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        break;
    case 4:
        srcType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        dstType = tscv::TS_IMGTYPE_ARGB8888;
        break;
    case 5:
        srcType = tscv::TS_IMGTYPE_ARGB8888;
        dstType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        break;
    case 6:
        srcType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        dstType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        break;
    case 7:
        srcType = tscv::TS_IMGTYPE_ARGB8888;
        dstType = tscv::TS_IMGTYPE_ARGB8888;
        break;
    case 8:
        srcType = tscv::TS_IMGTYPE_ARGB8888;
        dstType = tscv::TS_IMGTYPE_ARGB8888;
        break;
    case 9:
        srcType = tscv::TS_IMGTYPE_ARGB8888;
        dstType = tscv::TS_IMGTYPE_ARGB8888;
        break;
    case 10:
        srcType = tscv::TS_IMGTYPE_ARGB8888;
        dstType = tscv::TS_IMGTYPE_ARGB8888;
        break;
    case 11:
        srcType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        dstType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        break;
    case 12:
        srcType = tscv::TS_IMGTYPE_ARGB8888;
        dstType = tscv::TS_IMGTYPE_ARGB8888;
        break;
    case 13:
        srcType = tscv::TS_IMGTYPE_ARGB8888;
        dstType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        break;
    case 14:
        srcType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        dstType = tscv::TS_IMGTYPE_ARGB8888;
        break;
    case 15:
        srcType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        dstType = tscv::TS_IMGTYPE_RGBA8888;
        break;
    default:
        break;
	}

    vpssHandle = tscv::createVpss(srcSize, dstSize, srcType, dstType);
    if(vpssHandle == NULL)
	{
	    SAMPLE_PRT("createVpss exec abnormally, no resource left!\n");
		SAMPLE_SYS_Exit();
		return -1;
	}

    switch (s32Index) {
    case 0:
        s32Ret = SAMPLE_TSCV_ResizeVpss(vpssHandle);
        break;
    case 1:
        s32Ret = SAMPLE_TSCV_ResizeVpss_argb(vpssHandle);
        break;
    case 2:
        s32Ret = SAMPLE_TSCV_CvtColorVpss_nv12_argb(vpssHandle);
        break;
    case 3:
        s32Ret = SAMPLE_TSCV_CvtColorVpss_argb_nv12(vpssHandle);
        break;
    case 4:
        s32Ret = SAMPLE_TSCV_ResizeCscVpss_nv12_argb(vpssHandle);
        break;
    case 5:
        s32Ret = SAMPLE_TSCV_ResizeCscVpss_argb_nv12(vpssHandle);
        break;
    case 6:
        s32Ret = SAMPLE_TSCV_Padding_nv12(vpssHandle);
        break;
    case 7:
        s32Ret = SAMPLE_TSCV_Padding_argb(vpssHandle);
        break;
    case 8:
        s32Ret = SAMPLE_TSCV_Padding_scale_argb(vpssHandle);
        break;
    case 9:
        s32Ret = SAMPLE_TSCV_Crop_argb(vpssHandle);
        break;
    case 10:
        s32Ret = SAMPLE_TSCV_Flip_argb(vpssHandle);
        break;
    case 11:
        s32Ret = SAMPLE_TSCV_Flip_nv12(vpssHandle);
        break;
    case 12:
        s32Ret =SAMPLE_TSCV_CropResize_argb(vpssHandle);
        break;
    case 13:
        s32Ret = SAMPLE_TSCV_CropResize_argb_nv12(vpssHandle);
        break;
    case 14:
        s32Ret = SAMPLE_TSCV_CropResize_nv12_argb(vpssHandle);
        break;
    case 15:
        s32Ret = SAMPLE_TSCV_CvtColorVpss_nv12_rgba(vpssHandle);
        break;
    case 16:
        s32Ret = SAMPLE_TSCV_MultiThread_ResizeAndCvtColorVpss(vpssHandle);
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
    tscv::exitVpss(vpssHandle);
    SAMPLE_SYS_Exit();
    return 1;
}


TS_S32 multi_task_core(int argc, char *argv[],int init_flag,int exit_flag)
{
    TS_S32 s32Ret = TS_FAILURE;
	TS_S32 s32Index;
	void* vpssHandle = NULL;

	if (argc < 2 || argc > 2) {
		SAMPLE_TSCV_Usage(argv[0]);
		return TS_FAILURE;
	}

	if (!strncmp(argv[1], "-h", 2)) {
		SAMPLE_TSCV_Usage(argv[0]);
		return TS_SUCCESS;
	}

    VB_CONFIG_S stVbConf;
    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
    stVbConf.u32MaxPoolCnt = 1;
    TS_S32 blk_size = 100*100;
    stVbConf.astCommPool[0].u64BlkSize = blk_size;
    stVbConf.astCommPool[0].u32BlkCnt = 2;

    

    s32Ret = TS_FAILURE;
     if(init_flag) {
        s32Ret = SAMPLE_SYS_Init(&stVbConf);
        if (s32Ret != TS_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_SYS_Init failed, s32Ret:%d\n", s32Ret);
            return -1;
        }
     }
    s32Index = atoi(argv[1]);
    log_message("==============%s=s32Index<%d>==================\n",__FUNCTION__,s32Index);
    cv::Size srcSize(1920, 1080);
    cv::Size dstSize(640, 360);
    tscv::tsImgType srcType = tscv::TS_IMGTYPE_YUV420SP_NV12, dstType = tscv::TS_IMGTYPE_YUV420SP_NV12;
    switch (s32Index) {
    case 0:
        srcType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        dstType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        break;
    case 1:
        srcType = tscv::TS_IMGTYPE_ARGB8888;
        dstType = tscv::TS_IMGTYPE_ARGB8888;
        break;
    case 2:
        srcType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        dstType = tscv::TS_IMGTYPE_ARGB8888;
        break;
    case 3:
        srcType = tscv::TS_IMGTYPE_ARGB8888;
        dstType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        break;
    case 4:
        srcType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        dstType = tscv::TS_IMGTYPE_ARGB8888;
        break;
    case 5:
        srcType = tscv::TS_IMGTYPE_ARGB8888;
        dstType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        break;
    case 6:
        srcType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        dstType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        break;
    case 7:
        srcType = tscv::TS_IMGTYPE_ARGB8888;
        dstType = tscv::TS_IMGTYPE_ARGB8888;
        break;
    case 8:
        srcType = tscv::TS_IMGTYPE_ARGB8888;
        dstType = tscv::TS_IMGTYPE_ARGB8888;
        break;
    case 9:
        srcType = tscv::TS_IMGTYPE_ARGB8888;
        dstType = tscv::TS_IMGTYPE_ARGB8888;
        break;
    case 10:
        srcType = tscv::TS_IMGTYPE_ARGB8888;
        dstType = tscv::TS_IMGTYPE_ARGB8888;
        break;
    case 11:
        srcType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        dstType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        break;
    case 12:
        srcType = tscv::TS_IMGTYPE_ARGB8888;
        dstType = tscv::TS_IMGTYPE_ARGB8888;
        break;
    case 13:
        srcType = tscv::TS_IMGTYPE_ARGB8888;
        dstType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        break;
    case 14:
        srcType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        dstType = tscv::TS_IMGTYPE_ARGB8888;
        break;
    case 15:
        srcType = tscv::TS_IMGTYPE_YUV420SP_NV12;
        dstType = tscv::TS_IMGTYPE_RGBA8888;
        break;
    default:
        break;
	}

    vpssHandle = tscv::createVpss(srcSize, dstSize, srcType, dstType);
    if(vpssHandle == NULL)
	{
	    SAMPLE_PRT("createVpss exec abnormally, no resource left!\n");
		if (exit_flag) {
			SAMPLE_SYS_Exit();
		}
		return -1;
	}

    switch (s32Index) {
    case 0:
        s32Ret = SAMPLE_TSCV_ResizeVpss(vpssHandle);
        break;
    case 1:
        s32Ret = SAMPLE_TSCV_ResizeVpss_argb(vpssHandle);
        break;
    case 2:
        s32Ret = SAMPLE_TSCV_CvtColorVpss_nv12_argb(vpssHandle);
        break;
    case 3:
        s32Ret = SAMPLE_TSCV_CvtColorVpss_argb_nv12(vpssHandle);
        break;
    case 4:
        s32Ret = SAMPLE_TSCV_ResizeCscVpss_nv12_argb(vpssHandle);
        break;
    case 5:
        s32Ret = SAMPLE_TSCV_ResizeCscVpss_argb_nv12(vpssHandle);
        break;
    case 6:
		usleep(200*1000);
        //s32Ret = SAMPLE_TSCV_Padding_nv12(vpssHandle);
        break;
    case 7:
		usleep(200*1000);
        s32Ret = SAMPLE_TSCV_Padding_argb(vpssHandle);
        break;
    case 8:
		usleep(200*1000);
        s32Ret = SAMPLE_TSCV_Padding_scale_argb(vpssHandle);
        break;
    case 9:
        s32Ret = SAMPLE_TSCV_Crop_argb(vpssHandle);
        break;
    case 10:
        s32Ret = SAMPLE_TSCV_Flip_argb(vpssHandle);
        break;
    case 11:
        s32Ret = SAMPLE_TSCV_Flip_nv12(vpssHandle);
        break;
    case 12:
        s32Ret =SAMPLE_TSCV_CropResize_argb(vpssHandle);
        break;
    case 13:
        s32Ret = SAMPLE_TSCV_CropResize_argb_nv12(vpssHandle);
        break;
    case 14:
        s32Ret = SAMPLE_TSCV_CropResize_nv12_argb(vpssHandle);
        break;
    case 15:
        s32Ret = SAMPLE_TSCV_CvtColorVpss_nv12_rgba(vpssHandle);
        break;
    case 16:
        s32Ret = SAMPLE_TSCV_MultiThread_ResizeAndCvtColorVpss(vpssHandle);
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
    
    tscv::exitVpss(vpssHandle);

    if (exit_flag) {
	    SAMPLE_SYS_Exit();
    }

    return 1;
}

int main(int argc, char *argv[])
{
    // 设置log等级
    system("echo MPI=4 > /proc/mpp/logmpp");

	const char *new_argv[3];
	char *parm_argv[3];
	char str_i[10];

	signal(SIGSEGV, signal_handler);
	//TS_U32 u32Pid = 0;
	// int run_case = 0;

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
	} else if (strcmp(argv[1], "multi") == 0 ) {    // && TX5368_AV200 == u32Pid
		// If "multi", execute multi_task_core
		int num_children = 3; // Default number of child processes
		if (argc >= 3) {
			num_children = atoi(argv[2]); // Get number of child processes from command line argument
			// run_case = atoi(argv[3]);
			SAMPLE_PRT(" num_children  %d \n", num_children);
			if (num_children <= 0) {
				SAMPLE_PRT("Invalid number of children. Please provide a positive integer.\n");
				return 1;
			}
		}
		int ppid = getpid();

		for (int i = 0; i < num_children; i++) {
			fork();
		}
		{
			if (ppid == getppid()) {
				for (int j = 0; j < 15; j++)
				{
					//int j = 0;
					sprintf(str_i, "%d", j);
					new_argv[0] = "multi_task_core";
					new_argv[1] = str_i;
					new_argv[2] = NULL;

					parm_argv[0] = const_cast<char *>(new_argv[0]);
					parm_argv[1] = const_cast<char *>(new_argv[1]);
					parm_argv[2] = NULL;
					if(j == 0)
						multi_task_core(2, parm_argv, 1, 0);
					else if(j >= (15-1))
						multi_task_core(2, parm_argv, 0, 1);
					else
						multi_task_core(2, parm_argv, 0, 0);

				}

				log_message("sub process exit %d----------------------------------------------------------------\n", getpid());
				_exit(0);
			}
			else if(ppid == getpid()){
				for (int i = 0; i < num_children; i++) {
					wait(NULL);
				}
			}
			else{
				exit(0);
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
