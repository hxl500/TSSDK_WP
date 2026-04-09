
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

#include "tscv.hpp"

#include "opencv2/opencv.hpp"

#define SAMPLE_PRT(fmt...)   \
	do {\
		printf("[%s]-%d: ", __func__, __LINE__);\
		printf(fmt);\
	} while (0)

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
//    TS_MPI_VB_ExitModCommPool(VB_UID_VDEC);
    TS_MPI_VB_Exit();
    return;
}

void SAMPLE_TSCV_CreateVb2(TS_S32 nsize, TS_U64 &u64PhyAddr, TS_U64 &u64VirAddr)
{
    VB_POOL_CONFIG_S stVbPoolCfg;
    uint32_t vbPool = 0;
    stVbPoolCfg.u64BlkSize = nsize;
    stVbPoolCfg.u32BlkCnt = 2;
    vbPool = TS_MPI_VB_CreatePool(&stVbPoolCfg);
    VB_BLK VbHandle = TS_MPI_VB_GetBlock(vbPool, nsize, TS_NULL);
    if (VB_INVALID_HANDLE == VbHandle)
    {
        SAMPLE_PRT("TS_MPI_VB_GetBlock failed!\n");
        return;
    }
    u64PhyAddr = TS_MPI_VB_Handle2PhysAddr(VbHandle);
    TS_S32 ret = TS_MPI_VB_MmapPool(vbPool);
    (void)ret;
    void *viraddr = NULL;
    TS_MPI_VB_GetBlockVirAddr(vbPool, u64PhyAddr, &viraddr);
    u64VirAddr = (TS_U64)viraddr;
    printf("SAMPLE_TSCV_CreateVb2 u64VirAddr=%lld u64VirAddr=%lld\n", u64VirAddr, u64PhyAddr);
}

void SAMPLE_TSCV_ReleaseVb(TS_U64 &phy_addr)
{
    VB_BLK blk_id =  TS_MPI_VB_PhysAddr2Handle(phy_addr);
    VB_POOL pool_id = TS_MPI_VB_Handle2PoolId(blk_id);
    TS_S32 ret = TS_MPI_VB_MunmapPool(pool_id);
    (void)ret;
    ret = TS_MPI_VB_DestroyPool(pool_id);
}

TS_S32 SAMPLE_TSCV_ResizeVgs()
{
    TS_CHAR szInFileName[128] = "1080p_nv12.raw";
    TS_CHAR szOutFileName[128] = "out_file_cv_resize.yuv";
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

    for(int i=0; i<20; i++)
    {
        gettimeofday(&tmStart, NULL);
        tscv::resizeVgs(yuvIn, yuvOut, dstSize);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::resizeVgs 1920x1080@NV12 to 640x360@NV12 time cost: %dus\n", nTmCost);
    }
    
    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
    }

    // 释放vb
	TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);
	return 0;
}

TS_S32 SAMPLE_TSCV_ResizeVgs_2()
{
    TS_CHAR szInFileName[128] = "360p_nv12.raw";
    TS_CHAR szOutFileName[128] = "out_file_cv_resize.yuv";
    TS_CHAR szOutFileNameWithPid[128];
    TS_S32 src_w = 640;
    TS_S32 src_h = 360;
    TS_S32 dst_w = 1920;
    TS_S32 dst_h = 1080;
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
    for(int i=0; i<20; i++)
    {
        gettimeofday(&tmStart, NULL);
        tscv::resizeVgs(yuvIn, yuvOut, dstSize);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::resizeVgs 640x360@NV12 to 1920x1080@NV12 time cost: %dus\n", nTmCost);
    }
    
    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
    }

    // 释放vb
	TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);
	return 0;
}

TS_S32 SAMPLE_TSCV_ResizeVgs_argb()
{
    TS_CHAR szInFileName[128] = "1080p_argb.raw";
    TS_CHAR szOutFileName[128] = "out_file_cv_resize_argb.yuv";
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
        return TS_FAILURE;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
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
    for(int i=0; i<20; i++)
    {
        gettimeofday(&tmStart, NULL);
        tscv::resizeVgs(yuvIn, yuvOut, dstSize,tscv::TS_IMGTYPE_ARGB8888);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::resizeVgs 1920x1080@ARGB to 640x360@ARGB time cost: %dus\n", nTmCost);
    }
    
    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
    }

    // 释放vb
    TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);
    return 0;
}

TS_S32 SAMPLE_TSCV_ResizeVgs_argb_2()
{
    TS_CHAR szInFileName[128] = "360p_argb.raw";
    TS_CHAR szOutFileName[128] = "out_file_cv_resize_argb_big.yuv";
    TS_CHAR szOutFileNameWithPid[128]; 
    TS_S32 src_w = 640;
    TS_S32 src_h = 360;
    TS_S32 dst_w = 1920;
    TS_S32 dst_h = 1080;
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
        return TS_FAILURE;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
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
    for(int i=0; i<20; i++)
    {
        gettimeofday(&tmStart, NULL);
        tscv::resizeVgs(yuvIn, yuvOut, dstSize,tscv::TS_IMGTYPE_ARGB8888);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::resizeVgs 640x360@ARGB to 1920x1080@ARGB time cost: %dus\n", nTmCost);
    }
    
    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
    }

    // 释放vb
    TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);
    return 0;
}

TS_S32 SAMPLE_TSCV_CvtColorVgs_nv12_argb()
{
    TS_CHAR szInFileName[128] = "1080p_nv12.raw";
    TS_CHAR szOutFileName[128] = "out_file_cv_cvtColor_nv12_argb.yuv";
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
        return TS_FAILURE;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
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
    for(int i=0; i<20; i++) 
    {
        gettimeofday(&tmStart, NULL);
        tscv::cvtColorVgs(yuvIn, yuvOut, tscv::TS_YUV420SP2ARGB);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::cvtColorVgs 1920x1080@NV12 to 1920x1080@ARGB time cost: %dus\n", nTmCost);
    }
    
    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
    }

    // 释放vb
	TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);
	return 0;
}

TS_S32 SAMPLE_TSCV_CvtColorVgs_argb_nv12()
{
    TS_CHAR szInFileName[128] = "1080p_argb.raw";
    TS_CHAR szOutFileName[128] = "out_file_cv_cvtColor_argb_nv12.yuv";
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
        return TS_FAILURE;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
    }
    yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
    yuvOut.data = (uchar*)yuvOut.mpi_viraddr;
 
    //加载测试数据
    FILE *pFileRead = fopen(szInFileName, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", szInFileName);
        return TS_FAILURE;
    }
    fread((char*)yuvIn.data, 1, insize, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;
    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    for(int i=0; i<20; i++) 
    {
        gettimeofday(&tmStart, NULL);
        tscv::cvtColorVgs(yuvIn, yuvOut, tscv::TS_ARGB2YUV420SP);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::cvtColorVgs 1920x1080@ARGB to 1920x1080@NV12 time cost: %dus\n", nTmCost);
    }

    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        printf("out file w=%d h=%d, size=%d ptr=%p\n", nw, nh, outsize, (void*)yuvOut.data);
        fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
    }

    // 释放vb
	TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);
	return 0;
}

TS_S32 SAMPLE_TSCV_ResizeCscVgs_nv12_argb()
{
    TS_CHAR szInFileName[128] = "1080p_nv12.raw";
    TS_CHAR szOutFileName[128] = "out_file_cv_resize_csc_nv12_argb.yuv";
    TS_CHAR szOutFileNameWithPid[128];
    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = 640;
    TS_S32 dst_h = 360;
    cv::Size dstSize(640, 360);
    cv::Mat yuvIn(src_h*3/2, src_w, CV_8UC1);
    cv::Mat yuvOut(dst_h, dst_w, CV_8UC4);
    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_YUV420SP_NV12);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_ARGB8888);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
    }
    yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
    yuvOut.data = (uchar*)yuvOut.mpi_viraddr;

    //加载测试数据
    FILE *pFileRead = fopen(szInFileName, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", szInFileName);
        return TS_FAILURE;
    }
    fread((char*)yuvIn.data, 1, insize, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;
    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    for(int i=0; i<20; i++) 
    {
        gettimeofday(&tmStart, NULL);
        tscv::resizeCvtColorVgs(yuvIn, yuvOut, dstSize, tscv::TS_YUV420SP2ARGB);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::resizeCvtColorVgs 1920x1080@NV12 to 640x360@ARGB time cost: %dus\n", nTmCost);
    }
    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
    }

    // 释放vb
	TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);
	return 0;
}

TS_S32 SAMPLE_TSCV_ResizeCscVgs_argb_nv12()
{
    TS_CHAR szInFileName[128] = "1080p_argb.raw";
    TS_CHAR szOutFileName[128] = "out_file_cv_resize_csc_argb_nv12.yuv";
    TS_CHAR szOutFileNameWithPid[128];
    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = 640;
    TS_S32 dst_h = 360;
    cv::Size dstSize(640, 360);
    cv::Mat yuvIn(src_h, src_w, CV_8UC4);
    cv::Mat yuvOut(dst_h*3/2, dst_w, CV_8UC1);
    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_ARGB8888);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_YUV420SP_NV12);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
    }
    yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
    yuvOut.data = (uchar*)yuvOut.mpi_viraddr;
 
    //加载测试数据
    FILE *pFileRead = fopen(szInFileName, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", szInFileName);
        return TS_FAILURE;
    }
    fread((char*)yuvIn.data, 1, insize, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;
    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    for(int i=0; i<20; i++) 
    {
        gettimeofday(&tmStart, NULL);
        tscv::resizeCvtColorVgs(yuvIn, yuvOut, dstSize, tscv::TS_ARGB2YUV420SP);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::resizeCvtColorVgs 1920x1080@ARGB to 640x360@NV12 time cost: %dus\n", nTmCost);
    }
    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
    }

    // 释放vb
	TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);
	return 0;
}

TS_S32 SAMPLE_TSCV_Padding_nv12()
{
    TS_CHAR szInFileName[128] = "1080p_nv12.raw";
    TS_CHAR szOutFileName[128] = "out_file_cv_padding_nv12.yuv";
    TS_CHAR szOutFileNameWithPid[128];
    TS_S32 left = 32;
    TS_S32 right = 32;
    TS_S32 top = 32;
    TS_S32 bottom = 40;
    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = src_w + left + right;
    TS_S32 dst_h = src_h + top + bottom;
    cv::Mat yuvIn(src_h*3/2, src_w, CV_8UC1);
    cv::Mat yuvOut(dst_h*3/2, dst_w, CV_8UC1);
    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_YUV420SP_NV12);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_YUV420SP_NV12);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
    }
    yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
    yuvOut.data = (uchar*)yuvOut.mpi_viraddr;
 
    //加载测试数据
    FILE *pFileRead = fopen(szInFileName, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", szInFileName);
        return TS_FAILURE;
    }
    fread((char*)yuvIn.data, 1, insize, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;
    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    for(int i=0; i<20; i++) 
    {
        gettimeofday(&tmStart, NULL);
        tscv::copyMakeBorderVgs(yuvIn, yuvOut, top, bottom, left, right, 0xFFFF00, tscv::TS_IMGTYPE_YUV420SP_NV12);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::copyMakeBorderVgs 1920x1080@NV12 to 1984x1152@NV12 time cost: %dus\n", nTmCost);
    }
    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
    }
    // 释放vb
	TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);
	return 0;
}

TS_S32 SAMPLE_TSCV_Padding_argb()
{
    TS_CHAR szInFileName[128] = "1080p_argb.raw";
    TS_CHAR szOutFileName[128] = "out_file_cv_padding_argb.yuv";
    TS_CHAR szOutFileNameWithPid[128];
    TS_S32 left = 32;
    TS_S32 right = 32;
    TS_S32 top = 32;
    TS_S32 bottom = 40;
    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = src_w + left + right;
    TS_S32 dst_h = src_h + top + bottom;
    cv::Mat yuvIn(src_h, src_w, CV_8UC4);
    cv::Mat yuvOut(dst_h, dst_w, CV_8UC4);
    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_ARGB8888);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_ARGB8888);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
    }
    yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
    yuvOut.data = (uchar*)yuvOut.mpi_viraddr;
 
    //加载测试数据
    FILE *pFileRead = fopen(szInFileName, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", szInFileName);
        return TS_FAILURE;
    }
    fread((char*)yuvIn.data, 1, insize, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;
    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    for(int i=0; i<20; i++) 
    {
        gettimeofday(&tmStart, NULL);
        tscv::copyMakeBorderVgs(yuvIn, yuvOut, top, bottom, left, right, 0xFFFF00, tscv::TS_IMGTYPE_ARGB8888);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::copyMakeBorderVgs 1920x1080@ARGB to 1984x1152@ARGB time cost: %dus\n", nTmCost);
    }
    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
    }
    // 释放vb
	TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);
	return 0;
}

TS_S32 SAMPLE_TSCV_Padding_scale_argb()
{
    TS_CHAR szInFileName[128] = "1080p_argb.raw";
    TS_CHAR szOutFileName[128] = "out_file_cv_padding_scale_argb.yuv";
    TS_CHAR szOutFileNameWithPid[128];
    TS_S32 left = 20;
    TS_S32 right = 20;
    TS_S32 top = 0;
    TS_S32 bottom = 0;
    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = 640;
    TS_S32 dst_h = 400;
    cv::Mat yuvIn(src_h, src_w, CV_8UC4);
    cv::Mat yuvOut(dst_h, dst_w, CV_8UC4);
    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_ARGB8888);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_ARGB8888);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
    }

    yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
    yuvOut.data = (uchar*)yuvOut.mpi_viraddr;
 
    //加载测试数据
    FILE *pFileRead = fopen(szInFileName, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", szInFileName);
        return TS_FAILURE;
    }
    fread((char*)yuvIn.data, 1, insize, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;
    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    for(int i=0; i<20; i++) 
    {
        gettimeofday(&tmStart, NULL);
        tscv::copyMakeBorderVgs(yuvIn, yuvOut, top, bottom, left, right, 0xFFFF00, tscv::TS_IMGTYPE_ARGB8888);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::copyMakeBorderVgs 1920x1080@ARGB to 640x400@ARGB time cost: %dus\n", nTmCost);
    }
    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
    } 
    // 释放vb
	TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);
	return 0;
}

TS_S32 SAMPLE_TSCV_Crop_argb()
{
    TS_CHAR szInFileName[128] = "1080p_argb.raw";
    TS_CHAR szOutFileName[128] = "out_file_cv_crop_argb.yuv";
    TS_CHAR szOutFileNameWithPid[128];
    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = 640;
    TS_S32 dst_h = 640;
    cv::Mat yuvIn(src_h, src_w, CV_8UC4);
    cv::Mat yuvOut(dst_h, dst_w, CV_8UC4);
    cv::Rect rect(320, 320, 640, 640);
    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_ARGB8888);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_ARGB8888);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
    }
    yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
    yuvOut.data = (uchar*)yuvOut.mpi_viraddr;
 
    //加载测试数据
    FILE *pFileRead = fopen(szInFileName, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", szInFileName);
        return TS_FAILURE;
    }
    fread((char*)yuvIn.data, 1, insize, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;
    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    for(int i=0; i<20; i++) 
    {
        gettimeofday(&tmStart, NULL);
        tscv::cropVgs(yuvIn, yuvOut, rect, 0, 0, tscv::TS_IMGTYPE_ARGB8888);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::cropVgs 1920x1080@ARGB to 640x640@ARGB time cost: %dus\n", nTmCost);
    }
    
    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
    }

    // 释放vb
	TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);
	return 0;
}

TS_S32 SAMPLE_TSCV_Flip_argb()
{
    TS_CHAR szInFileName[128] = "1080p_argb.raw";
    TS_CHAR szOutFileName[128] = "out_file_cv_flip_argb.yuv";
    TS_CHAR szOutFileNameWithPid[128];
    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = 1920;
    TS_S32 dst_h = 1080;
    cv::Mat yuvIn(src_h, src_w, CV_8UC4);
    cv::Mat yuvOut(dst_h, dst_w, CV_8UC4);
    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_ARGB8888);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_ARGB8888);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
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
    for(int i=0; i<20; i++) 
    {
        gettimeofday(&tmStart, NULL);
        tscv::flipVgs(yuvIn, yuvOut, 0, tscv::TS_IMGTYPE_ARGB8888);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::flipVgs 1920x1080@ARGB to 1920x1080@ARGB time cost: %dus\n", nTmCost);
    }
    
    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
    }
    // 释放vb
	TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);
	return 0;
}

TS_S32 SAMPLE_TSCV_Flip_nv12()
{
    TS_CHAR szInFileName[128] = "1080p_nv12.raw";
    TS_CHAR szOutFileName[128] = "out_file_cv_flip_nv12.yuv";
    TS_CHAR szOutFileNameWithPid[128];
    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = 1920;
    TS_S32 dst_h = 1080;
    cv::Mat yuvIn(src_h*3/2, src_w, CV_8UC4);
    cv::Mat yuvOut(dst_h*3/2, dst_w, CV_8UC4);
    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_YUV420SP_NV12);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_YUV420SP_NV12);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
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
    for(int i=0; i<20; i++)
    {
        gettimeofday(&tmStart, NULL);
        tscv::flipVgs(yuvIn, yuvOut, 0, tscv::TS_IMGTYPE_YUV420SP_NV12);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::flipVgs 1920x1080@NV12 to 1920x1080@NV12 time cost: %dus\n", nTmCost);
    }
    
    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
    }
    // 释放vb
	TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);
	return 0;
}

TS_S32 SAMPLE_TSCV_CropResize_argb()
{
    TS_CHAR szInFileName[128] = "1080p_argb.raw";
    TS_CHAR szOutFileName[128] = "out_file_cv_cropResize_argb.yuv";
    TS_CHAR szOutFileNameWithPid[128];
    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = 320;
    TS_S32 dst_h = 320;
    cv::Mat yuvIn(src_h, src_w, CV_8UC4);
    cv::Mat yuvOut(dst_h, dst_w, CV_8UC4);
    cv::Rect rect(320, 320, 640, 640);
    cv::Size dsize(320, 320);
    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_ARGB8888);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_ARGB8888);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
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
    for(int i=0; i<20; i++)
    {
        gettimeofday(&tmStart, NULL);
        tscv::cropResizeVgs(yuvIn, yuvOut, rect, dsize, tscv::TS_IMGTYPE_ARGB8888, tscv::TS_IMGTYPE_ARGB8888);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::cropResizeVgs 1920x1080@ARGB to 320x320@NVARGB time cost: %dus\n", nTmCost);
    }
    
    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
    }
    // 释放vb
	TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);    
	return 0;
}

TS_S32 SAMPLE_TSCV_CropResize_argb_nv12()
{
    TS_CHAR szInFileName[128] = "1080p_argb.raw";
    TS_CHAR szOutFileName[128] = "out_file_cv_cropResize_argb_nv12.yuv";
    TS_CHAR szOutFileNameWithPid[128];
    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = 320;
    TS_S32 dst_h = 320;
    cv::Mat yuvIn(src_h, src_w, CV_8UC4);
    cv::Mat yuvOut(dst_h*3/2, dst_w, CV_8UC1);
    cv::Rect rect(320, 320, 640, 640);
    cv::Size dsize(320, 320);
    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_ARGB8888);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_YUV420SP_NV12);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
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
    for(int i=0; i<20; i++)
    {
        gettimeofday(&tmStart, NULL);
        tscv::cropResizeVgs(yuvIn, yuvOut, rect, dsize, tscv::TS_IMGTYPE_ARGB8888, tscv::TS_IMGTYPE_YUV420SP_NV12);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::cropResizeVgs 1920x1080@ARGB to 320x320@NV12 time cost: %dus\n", nTmCost);
    }
    
    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
    }
    // 释放vb
	TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);
	return 0;
}

TS_S32 SAMPLE_TSCV_CropResize_nv12_argb()
{
    TS_CHAR szInFileName[128] = "1080p_nv12.raw";
    TS_CHAR szOutFileName[128] = "out_file_cv_cropResize_nv12_argb.yuv";
    TS_CHAR szOutFileNameWithPid[128];
    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 dst_w = 320;
    TS_S32 dst_h = 320;
    cv::Mat yuvIn(src_h*3/2, src_w, CV_8UC1);
    cv::Mat yuvOut(dst_h, dst_w, CV_8UC4);
    cv::Rect rect(320, 320, 640, 640);
    cv::Size dsize(320, 320);
    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_YUV420SP_NV12);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(tscv::TS_IMGTYPE_ARGB8888);
    TS_S32 ret = 0;

    ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
    }
    ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
    if(ret != 0){
        printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
        return TS_FAILURE;
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
    for(int i=0; i<20; i++)
    {
        gettimeofday(&tmStart, NULL);
        tscv::cropResizeVgs(yuvIn, yuvOut, rect, dsize, tscv::TS_IMGTYPE_YUV420SP_NV12, tscv::TS_IMGTYPE_ARGB8888);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::cropResizeVgs 1920x1080@NV12 to 320x320@ARGB time cost: %dus\n", nTmCost);
    }
    
    //保存测试数据
    sprintf(szOutFileNameWithPid, "%s_%d", (TS_CHAR*)szOutFileName, getpid());
    pFileRead = fopen(szOutFileNameWithPid, "wb");
    if (TS_NULL != pFileRead) {
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
    }
    // 释放vb
	TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
    TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);
	return 0;
}

TS_VOID SAMPLE_TSCV_Usage(TS_CHAR *sPrgNm)
{
	printf("Usage : %s <index>\n", sPrgNm);
	printf("index:\n");
	printf("\t 0) resize nv12 1080p->360p\n");
    printf("\t 1) resize argb 1080p->360p\n");
	printf("\t 2) cvtColor nv12->argb\n");
	printf("\t 3) cvtColor argb->nv12\n");
    printf("\t 4) resizeCsc 1080p_nv12->360p_argb\n");
    printf("\t 5) resizeCsc 1080p_argb->360p_nv12\n");
    printf("\t 6) padding nv12 1080p->1984x1152\n");
    printf("\t 7) padding argb 1080p->1984x1152\\n");
    printf("\t 8) padding scale argb 1080p->640x400\n");
    printf("\t 9) crop argb 1080p->640x640\n");
    printf("\t 10)flip argb \n");
    printf("\t 11)flip nv12 \n");
    printf("\t 12)crop resize argb 1080p->crop640x640->scale320x320\n");
    printf("\t 13)crop resizeCsc argb nv12 1080p->crop640x640->scale320x320\n");
    printf("\t 14)crop resizeCsc nv12 argb 1080p->crop640x640->scale320x320\n");
    printf("\t 15) resize nv12 360p->1080p\n");
    printf("\t 16) resize argb 360p->1080p\n");
    printf("\t 17) resize prefermance test\n");
    printf("\t 18) opencv resize prefermance test\n");
    printf("\t 19) new opencv interface test\n");
}

typedef struct tag_TEST_STU_s {
    char    name[50];
    char    src[50];
    char    dst[50];
    int     src_w;
    int     src_h;
    tscv::tsImgType src_fmt;
    int     dst_w;
    int     dst_h;
    tscv::tsImgType dst_fmt;
}TEST_STU;

TEST_STU test_data[] = {
    {"8k->4k", "/root/nfs/image/8k_argb_7680_4320.raw", "out_resize_argb1.yuv", 7680, 4320, tscv::TS_IMGTYPE_ARGB8888, 3840, 2160, tscv::TS_IMGTYPE_ARGB8888},
    {"8k->2k", "/root/nfs/image/8k_argb_7680_4320.raw", "out_resize_argb2.yuv", 7680, 4320, tscv::TS_IMGTYPE_ARGB8888, 2560, 1440, tscv::TS_IMGTYPE_ARGB8888},
    {"8k->1080p", "/root/nfs/image/8k_argb_7680_4320.raw", "out_resize_argb3.yuv", 7680, 4320, tscv::TS_IMGTYPE_ARGB8888, 1920, 1080, tscv::TS_IMGTYPE_ARGB8888},
    {"8k->720p", "/root/nfs/image/8k_argb_7680_4320.raw", "out_resize_argb4.yuv", 7680, 4320, tscv::TS_IMGTYPE_ARGB8888, 1280, 720, tscv::TS_IMGTYPE_ARGB8888},
    {"8k->360p", "/root/nfs/image/8k_argb_7680_4320.raw", "out_resize_argb5.yuv", 7680, 4320, tscv::TS_IMGTYPE_ARGB8888, 640, 360, tscv::TS_IMGTYPE_ARGB8888},

    {"360p->720p", "/root/nfs/image/360p_argb_640_360.raw", "out_resize_argb6.yuv", 640, 360, tscv::TS_IMGTYPE_ARGB8888, 1280, 720, tscv::TS_IMGTYPE_ARGB8888},
    {"360p->1080p", "/root/nfs/image/360p_argb_640_360.raw", "out_resize_argb7.yuv", 640, 360, tscv::TS_IMGTYPE_ARGB8888, 1920, 1080, tscv::TS_IMGTYPE_ARGB8888},
    {"360p->2k", "/root/nfs/image/360p_argb_640_360.raw", "out_resize_argb8.yuv", 640, 360, tscv::TS_IMGTYPE_ARGB8888, 2560, 1440, tscv::TS_IMGTYPE_ARGB8888},
    {"360p->4k", "/root/nfs/image/360p_argb_640_360.raw", "out_resize_argb9.yuv", 640, 360, tscv::TS_IMGTYPE_ARGB8888, 3840, 2160, tscv::TS_IMGTYPE_ARGB8888},
    {"360p->8k", "/root/nfs/image/360p_argb_640_360.raw", "out_resize_argb10.yuv", 640, 360, tscv::TS_IMGTYPE_ARGB8888, 7680, 4320, tscv::TS_IMGTYPE_ARGB8888},

    {"8k->4k", "/root/nfs/image/8k_nv12_7680_4320.raw", "out_resize_nv1.yuv", 7680, 4320, tscv::TS_IMGTYPE_YUV420SP_NV12, 3840, 2160, tscv::TS_IMGTYPE_YUV420SP_NV12},
    {"8k->2k", "/root/nfs/image/8k_nv12_7680_4320.raw", "out_resize_nv2.yuv", 7680, 4320, tscv::TS_IMGTYPE_YUV420SP_NV12, 2560, 1440, tscv::TS_IMGTYPE_YUV420SP_NV12},
    {"8k->1080p", "/root/nfs/image/8k_nv12_7680_4320.raw", "out_resize_nv3.yuv", 7680, 4320, tscv::TS_IMGTYPE_YUV420SP_NV12, 1920, 1080, tscv::TS_IMGTYPE_YUV420SP_NV12},
    {"8k->720p", "/root/nfs/image/8k_nv12_7680_4320.raw", "out_resize_nv4.yuv", 7680, 4320, tscv::TS_IMGTYPE_YUV420SP_NV12, 1280, 720, tscv::TS_IMGTYPE_YUV420SP_NV12},
    {"8k->360p", "/root/nfs/image/8k_nv12_7680_4320.raw", "out_resize_nv5.yuv", 7680, 4320, tscv::TS_IMGTYPE_YUV420SP_NV12, 640, 360, tscv::TS_IMGTYPE_YUV420SP_NV12},

    {"360p->720p", "/root/nfs/image/360p_nv12_640_360.raw", "out_resize_nv6.yuv", 640, 360, tscv::TS_IMGTYPE_YUV420SP_NV12, 1280, 720, tscv::TS_IMGTYPE_YUV420SP_NV12},
    {"360p->1080p", "/root/nfs/image/360p_nv12_640_360.raw", "out_resize_nv7.yuv", 640, 360, tscv::TS_IMGTYPE_YUV420SP_NV12, 1920, 1080, tscv::TS_IMGTYPE_YUV420SP_NV12},
    {"360p->2k", "/root/nfs/image/360p_nv12_640_360.raw", "out_resize_nv8.yuv", 640, 360, tscv::TS_IMGTYPE_YUV420SP_NV12, 2560, 1440, tscv::TS_IMGTYPE_YUV420SP_NV12},
    {"360p->4k", "/root/nfs/image/360p_nv12_640_360.raw", "out_resize_nv9.yuv", 640, 360, tscv::TS_IMGTYPE_YUV420SP_NV12, 3840, 2160, tscv::TS_IMGTYPE_YUV420SP_NV12},
    {"360p->8k", "/root/nfs/image/360p_nv12_640_360.raw", "out_resize_nv10.yuv", 640, 360, tscv::TS_IMGTYPE_YUV420SP_NV12, 7680, 4320, tscv::TS_IMGTYPE_YUV420SP_NV12}
};

TS_S32 SAMPLE_TSCV_RESIZE_PERFORMANCE()
{
    for(int i=0; i<20; i++)
    {
        TEST_STU sub = test_data[i];
        TS_S32 src_w = sub.src_w;
        TS_S32 src_h = sub.src_h;
        TS_S32 dst_w = sub.dst_w;
        TS_S32 dst_h = sub.dst_h;
        float factor = sub.src_fmt == tscv::TS_IMGTYPE_YUV420SP_NV12 ? 1.5 : 1;
        int type = (sub.src_fmt == tscv::TS_IMGTYPE_YUV420SP_NV12 ? CV_8UC1 : CV_8UC4);
        tscv::tsImgType imgtype = sub.src_fmt == tscv::TS_IMGTYPE_YUV420SP_NV12 ? tscv::TS_IMGTYPE_YUV420SP_NV12 : tscv::TS_IMGTYPE_ARGB8888;
        cv::Size srcSize(src_w, src_h*factor);
        cv::Size dstSize(dst_w, dst_h);
        cv::Mat yuvIn(srcSize, type);
        //cv::Mat yuvOut(dstSize, type);
        cv::Mat yuvOut(dst_h*factor, dst_w, type);
        TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(sub.src_fmt);
        TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(imgtype);
        TS_S32 ret = 0;

        ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
        if(ret != 0){
            printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
            return TS_FAILURE;
        }
        ret = TS_MPI_SYS_MmzAlloc(&yuvOut.mpi_phyaddr, (void**)&yuvOut.mpi_viraddr, NULL, NULL, outsize);
        if(ret != 0){
            printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
            return TS_FAILURE;
        }
        yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
        yuvOut.data = (uchar*)yuvOut.mpi_viraddr;

        //加载测试数据
        FILE *pFileRead = fopen(sub.src, "rb");
        if (TS_NULL == pFileRead)
        {
            SAMPLE_PRT("can't open file %s\n", sub.src);
            return -1;
        }
        fread((char*)yuvIn.data, 1, insize, pFileRead);
        fclose(pFileRead);
        pFileRead = NULL;
        printf("--------------%s------------------\n", sub.name);
        struct timeval tmStart, tmEnd;
        int nTmCost = 0;
        for(int i=0; i<20; i++)
        {
            gettimeofday(&tmStart, NULL);
            tscv::resizeVgs(yuvIn, yuvOut, dstSize, imgtype);
            gettimeofday(&tmEnd, NULL);
            nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
            printf("tscv::resizeVgs time cost: %dus\n", nTmCost);
        }
        //保存测试数据
        pFileRead = fopen(sub.dst, "wb");
        if (TS_NULL != pFileRead) {
            printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
            fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
            fclose(pFileRead);
        }
        // 释放vb
        TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
        TS_MPI_SYS_MmzFree(yuvOut.mpi_phyaddr,(TS_VOID *)yuvOut.mpi_viraddr);
    }
    
    return 0;

}

TS_U32 SAMPLE_CV_RESIZE_PERFORMANCE()
{
    for(int i=0; i<20; i++)
    {
        TEST_STU sub = test_data[i];
        TS_S32 src_w = sub.src_w;
        TS_S32 src_h = sub.src_h;
        TS_S32 dst_w = sub.dst_w;
        TS_S32 dst_h = sub.dst_h;
        float factor = sub.src_fmt == tscv::TS_IMGTYPE_YUV420SP_NV12 ? 1.5 : 1;
        int type = (sub.src_fmt == tscv::TS_IMGTYPE_YUV420SP_NV12 ? CV_8UC1 : CV_8UC4);
        cv::Size srcSize(src_w, src_h*factor);
        cv::Size dstSize(dst_w, dst_h);
        cv::Mat yuvIn(srcSize, type);
        //cv::Mat yuvOut(dstSize, type);
        cv::Mat yuvOut;
        TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(sub.src_fmt);
        TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(sub.dst_fmt);
        TS_S32 ret = 0;
        ret = TS_MPI_SYS_MmzAlloc(&yuvIn.mpi_phyaddr, (void**)&yuvIn.mpi_viraddr, NULL, NULL, insize);
        if(ret != 0){
            printf("vb create failed , ret=%d, %s %d\n", ret, __func__, __LINE__);
            return TS_FAILURE;
        }
        yuvIn.data = (uchar*)yuvIn.mpi_viraddr;
        yuvIn.data = new uchar[insize];

        //加载测试数据
        FILE *pFileRead = fopen(sub.src, "rb");
        if (TS_NULL == pFileRead)
        {
            SAMPLE_PRT("can't open file %s\n", sub.src);
            return -1;
        }
        fread((char*)yuvIn.data, 1, insize, pFileRead);
        fclose(pFileRead);
        pFileRead = NULL;
        printf("--------------%s------------------\n", sub.name);
        struct timeval tmStart, tmEnd;
        int nTmCost = 0;
        for(int i=0; i<20; i++)
        {
            gettimeofday(&tmStart, NULL);
            cv::resize(yuvIn, yuvOut, dstSize);
            gettimeofday(&tmEnd, NULL);
            nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
            printf("cv::resize time cost: %dus\n", nTmCost);
        }

        //保存测试数据
        pFileRead = fopen(sub.dst, "wb");
        if (TS_NULL != pFileRead) {
            printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
            fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
            fclose(pFileRead);
        }
        // 释放vb
	    TS_MPI_SYS_MmzFree(yuvIn.mpi_phyaddr,(TS_VOID *)yuvIn.mpi_viraddr);
        delete[] yuvIn.data;
    }
    
    return 0;
}



TS_U32 SAMPLE_CV_NEW_INTERFACE()
{
    TEST_STU sub = test_data[0];
    TS_S32 src_w = sub.src_w;
    TS_S32 src_h = sub.src_h;
    TS_S32 dst_w = sub.dst_w;
    TS_S32 dst_h = sub.dst_h;
    float factor = sub.src_fmt == tscv::TS_IMGTYPE_YUV420SP_NV12 ? 1.5 : 1;
    int type = (sub.src_fmt == tscv::TS_IMGTYPE_YUV420SP_NV12 ? CV_8UC1 : CV_8UC4);
    tscv::tsImgType imgtype = sub.src_fmt == tscv::TS_IMGTYPE_YUV420SP_NV12 ? tscv::TS_IMGTYPE_YUV420SP_NV12 : tscv::TS_IMGTYPE_ARGB8888;
    cv::Size srcSize(src_w, src_h*factor);
    cv::Size dstSize(dst_w, dst_h);
    TS_S32 outsize = dst_w*dst_h*SAMPLE_TSCV_GetBitSizeByFmt(sub.dst_fmt);
    printf("factor=%f type=%d\n", factor, type);
    //cv::Mat yuvIn(srcSize, type);
    cv::Mat yuvIn((int)(src_h*factor), src_w, type, SAMPLE_TSCV_CreateVb2, SAMPLE_TSCV_ReleaseVb);
    //cv::Mat yuvOut(dstSize, type);
    //cv::Mat yuvOut(dst_h*factor, dst_w, type);
    unsigned long long phyaddr = 0;
    unsigned long long viraddr = 0;
    SAMPLE_TSCV_CreateVb2(outsize, phyaddr, viraddr);
    void *pData = (uchar*)viraddr;
    cv::Mat yuvOut(dst_h, dst_w, type, pData, phyaddr, SAMPLE_TSCV_ReleaseVb);

    TS_S32 insize = src_w*src_h*SAMPLE_TSCV_GetBitSizeByFmt(sub.src_fmt);


    //加载测试数据
    FILE *pFileRead = fopen(sub.src, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", sub.src);
        return -1;
    }
    fread((char*)yuvIn.data, 1, insize, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;
    struct timeval tmStart, tmEnd;
    int nTmCost = 0;
    for(int i=0; i<1; i++)
    {
        gettimeofday(&tmStart, NULL);
        tscv::resizeVgs(yuvIn, yuvOut, dstSize, imgtype);
        gettimeofday(&tmEnd, NULL);
        nTmCost = (tmEnd.tv_sec - tmStart.tv_sec)*1000000 + (tmEnd.tv_usec - tmStart.tv_usec);
        printf("tscv::resizeVgs time cost: %dus\n", nTmCost);
    }
    //保存测试数据
    pFileRead = fopen(sub.dst, "wb");
    if (TS_NULL != pFileRead) {
        printf("out file w=%d h=%d, size=%d ptr=%p\n", dst_w, dst_h, outsize, (void*)yuvOut.data);
        fwrite((char*)yuvOut.data, 1, outsize, pFileRead);
        fclose(pFileRead);
    }
    return 0;
}



TS_U32 SAMPLE_TSCV_OPENCV()
{
    printf("hello\n");
    cv::Mat src_img = cv::imread("./test_tg.jpg");
    if (src_img.empty()) {
        printf("input is error\n");
        return -1; 
    }
    cv::Size destsize(1920,1080);
    cv::Mat dest(destsize, CV_8UC3);
    cv::cvtColor(src_img, dest, CV_BGR2RGB);
    cv::imwrite("./dest.jpg",dest);
    printf("dest size w=%d h=%d\n",dest.size().width, dest.size().height);
    return 0;
}


TS_S32 simple_task_core(int argc,  char *argv[])
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

    switch (s32Index) {
    case 0:
		s32Ret = SAMPLE_TSCV_ResizeVgs();
		break;
	case 1:
		s32Ret = SAMPLE_TSCV_ResizeVgs_argb();
		break;
    case 2:
		s32Ret = SAMPLE_TSCV_CvtColorVgs_nv12_argb();
		break;
    case 3:
		s32Ret = SAMPLE_TSCV_CvtColorVgs_argb_nv12();
		break;
	case 4:
		s32Ret = SAMPLE_TSCV_ResizeCscVgs_nv12_argb();
		break;
	case 5:
		s32Ret = SAMPLE_TSCV_ResizeCscVgs_argb_nv12();
		break;
    case 6:
		s32Ret = SAMPLE_TSCV_Padding_nv12();
		break;
    case 7:
		s32Ret = SAMPLE_TSCV_Padding_argb();
		break;
    case 8:
		s32Ret = SAMPLE_TSCV_Padding_scale_argb();
		break;
    case 9:
		s32Ret = SAMPLE_TSCV_Crop_argb();
		break;
    case 10:
		s32Ret = SAMPLE_TSCV_Flip_argb();
		break;
    case 11:
		s32Ret = SAMPLE_TSCV_Flip_nv12();
		break;
    case 12:
        s32Ret =SAMPLE_TSCV_CropResize_argb();
        break;
    case 13:
        s32Ret = SAMPLE_TSCV_CropResize_argb_nv12();
        break;
    case 14:
        s32Ret = SAMPLE_TSCV_CropResize_nv12_argb();
        break;
    case 15:
		s32Ret = SAMPLE_TSCV_ResizeVgs_2();
		break;
	case 16:
		s32Ret = SAMPLE_TSCV_ResizeVgs_argb_2();
		break;
    case 17:
		s32Ret = SAMPLE_TSCV_RESIZE_PERFORMANCE();
		break;
    case 18:
        s32Ret = SAMPLE_CV_RESIZE_PERFORMANCE();
        break;
    case 19:
         s32Ret = SAMPLE_CV_NEW_INTERFACE();
        break;
    case 50:
        s32Ret = SAMPLE_TSCV_OPENCV();
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

    return 1;
}

TS_S32 multi_task_core(int argc, char *argv[], bool initflag, bool exitflag)
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

	VB_CONFIG_S stVbConf;
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt = 1;
	TS_S32 blk_size = 100 * 100;
	stVbConf.astCommPool[0].u64BlkSize = blk_size;
	stVbConf.astCommPool[0].u32BlkCnt = 2;

	s32Ret = TS_FAILURE;
	if (initflag) {
		s32Ret = SAMPLE_SYS_Init(&stVbConf);

		if (s32Ret != TS_SUCCESS) {
			SAMPLE_PRT("SAMPLE_SYS_Init failed, s32Ret:%d\n", s32Ret);
			return -1;
		}
	}

	s32Index = atoi(argv[1]);

	log_message("==============%s=s32Index<%d>==================\n", __FUNCTION__, s32Index);

	switch (s32Index) {
	case 0:
		s32Ret = SAMPLE_TSCV_ResizeVgs();
		break;
	case 1:
		s32Ret = SAMPLE_TSCV_ResizeVgs_argb();
		break;
	case 2:
		s32Ret = SAMPLE_TSCV_CvtColorVgs_nv12_argb();
		break;
	case 3:
		s32Ret = SAMPLE_TSCV_CvtColorVgs_argb_nv12();
		break;
	case 4:
		s32Ret = SAMPLE_TSCV_ResizeCscVgs_nv12_argb();
		break;
	case 5:
		s32Ret = SAMPLE_TSCV_ResizeCscVgs_argb_nv12();
		break;
	case 6:
		s32Ret = SAMPLE_TSCV_Padding_nv12();
		break;
	case 7:
		s32Ret = SAMPLE_TSCV_Padding_argb();
		break;
	case 8:
		s32Ret = SAMPLE_TSCV_Padding_scale_argb();
		break;
	case 9:
		s32Ret = SAMPLE_TSCV_Crop_argb();
		break;
	case 10:
		s32Ret = SAMPLE_TSCV_Flip_argb();
		break;
	case 11:
		s32Ret = SAMPLE_TSCV_Flip_nv12();
		break;
	case 12:
		s32Ret = SAMPLE_TSCV_CropResize_argb();
		break;
	case 13:
		s32Ret = SAMPLE_TSCV_CropResize_argb_nv12();
		break;
	case 14:
		s32Ret = SAMPLE_TSCV_CropResize_nv12_argb();
		break;
	case 15:
		s32Ret = SAMPLE_TSCV_ResizeVgs_2();
		break;
	case 16:
		s32Ret = SAMPLE_TSCV_ResizeVgs_argb_2();
		break;
	case 17:
		s32Ret = SAMPLE_TSCV_RESIZE_PERFORMANCE();
		break;
	case 18:
		s32Ret = SAMPLE_CV_RESIZE_PERFORMANCE();
		break;
	case 19:
		s32Ret = SAMPLE_CV_NEW_INTERFACE();
		break;
	case 50:
		s32Ret = SAMPLE_TSCV_OPENCV();
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
	if (exitflag)
		SAMPLE_SYS_Exit();

	return 1;
}

int main(int argc, char *argv[])
{
	const char *new_argv[3];
	char *parm_argv[3];
	char str_i[10];
	TS_U32 u32Pid = 0;
	int run_case = 0;

	TS_MPI_SYS_GetPid(&u32Pid);
	if (argc < 3) {
		if(TX5368_AV200 == u32Pid)
			SAMPLE_PRT("Usage: %s <mode> [ Use 'simple' or 'multi']  [num_children] \n", argv[0]);
		else
			SAMPLE_PRT("Usage: %s <mode> [ Use 'simple']  [num_children] \n", argv[0]);
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
 	} else if (strcmp(argv[1], "multi") == 0 && TX5368_AV200 == u32Pid) {
        
		// If "multi", execute multi_task_core
		int num_children = 3; // Default number of child processes
		if (argc >= 4) {
			num_children = atoi(argv[2]); // Get number of child processes from command line argument
			run_case = atoi(argv[3]);
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
				//for (int j = 0; j < 15; j++) 
				{
					//int j = 0;
					sprintf(str_i, "%d", run_case);
					new_argv[0] = "multi_task_core";
					new_argv[1] = str_i;
					new_argv[2] = NULL;

					parm_argv[0] = const_cast<char *>(new_argv[0]);
					parm_argv[1] = const_cast<char *>(new_argv[1]);
					parm_argv[2] = NULL;
					multi_task_core(2, parm_argv, 1, 1);

				}
				
				log_message("sub process exit %d----------------------------------------------------------------\n", getpid());
				for (int i = 0; i < num_children-1; i++) {
					wait(NULL);
				}
				_exit(0);
			}
			else if(ppid == getpid()){
				for (int i = 0; i < num_children; i++) {
					wait(NULL);
				}
			}
			else{
				for (int i = 0; i < num_children-2; i++) {
					wait(NULL);
				}
				exit(0);
			}
		}
		
	}else {
		if(TX5368_AV200 == u32Pid)
			SAMPLE_PRT("Invalid mode. Use 'simple' or 'multi'.\n");
		else
			SAMPLE_PRT("Invalid mode. Use 'simple'.\n");
		return 1;
	}

	return 0;
}