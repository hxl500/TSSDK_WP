#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

#include "ts_alg_type.h"
#include "ts_alg_log.h"
#include "ts_alg_imgproc.h"
#include "mpi_vb.h"

#include "ts_alg_body_detect_demo.h"


	#include "body_detect_cfg_v1_5_tx521x.h"
	#include "body_detect_weight_v1_5_tx521x.h"

#include "./alg_colorconvert/ts_alg_color_convert.h"

	#include "opencv2/opencv.hpp"
	#include "tscv.hpp"

#include "ts_type.h"
#include "mpi_sys.h"
#include "mpi_vb.h"


#define SAMPLE_PRT(fmt...)   \
	do {\
		printf("[%s]-%d: ", __func__, __LINE__);\
		printf(fmt);\
	} while (0)

static long SAMPLE_TSCV_GetTime()
{
	struct timeval tv1;
	gettimeofday(&tv1, NULL);
	return (long)(tv1.tv_sec*1000000) + (tv1.tv_usec);
}

TS_FLOAT SAMPLE_TSCV_GetBitSizeByFmt(PIXEL_FORMAT_E pixel_fmt)
{
	TS_FLOAT fVal = 0;
	switch (pixel_fmt)
	{
	case PIXEL_FORMAT_ARGB_8888:
	case PIXEL_FORMAT_ABGR_8888:
	case PIXEL_FORMAT_RGBA_8888:
	case PIXEL_FORMAT_BGRA_8888:
		fVal = 4.0;
		break;
	case PIXEL_FORMAT_YVU_SEMIPLANAR_420:
	case PIXEL_FORMAT_YUV_SEMIPLANAR_420:
	case PIXEL_FORMAT_NV_12:
	case PIXEL_FORMAT_NV_21:
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

	system("echo VPSS=0 > /proc/mpp/logmpp");
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
    // TS_MPI_VB_ExitModCommPool(VB_UID_USER);
    TS_MPI_VB_DestroyPool(VB_UID_USER);
    TS_MPI_VB_Exit();
    return;
}

//{
//    VB_BLK blk_id =  TS_MPI_VB_PhysAddr2Handle(phy_addr);
//    VB_POOL pool_id = TS_MPI_VB_Handle2PoolId(blk_id);
//    TS_S32 ret = TS_MPI_VB_MunmapPool(pool_id);
//    ret = TS_MPI_VB_DestroyPool(pool_id);
//}

#define ALG_BODY_DETECT_IMAGE_C 4
#define ALG_BODY_DETECT_IMAGE_H 384
#define ALG_BODY_DETECT_IMAGE_W 640

// #include "bodydetect_bgra_640x384.h"
static TS_S32 SAMPLE_ALG_Detection(void *data, ALG_BODY_RESULT_S *result)
{

	TS_S32 s32Ret;
	TS_VOID *phandle = NULL;
	ALG_MODEL_INIT_S param;

	param.pGraph = body_detect_cfg_v1_5_tx521x;
	param.pWeight = body_detect_weight_v1_5_tx521x;
	param.u32GraphSize = sizeof(body_detect_cfg_v1_5_tx521x);
	param.u32WeightSize = sizeof(body_detect_weight_v1_5_tx521x);
	ALG_LOGD("model tx5215\n");


	param.eImageType = ALG_IMAGE_TYPE_INT_HWC_RGB0;//ALG_IMAGE_TYPE_INT_HWC;
	param.bRneOff = TS_FALSE;

	TS_U8 *pImgBuf = (TS_U8 *)malloc(ALG_BODY_DETECT_IMAGE_C * ALG_BODY_DETECT_IMAGE_H * ALG_BODY_DETECT_IMAGE_W);
	if(TS_NULL == pImgBuf) {
		ALG_LOGE("error, malloc failed!\n");
		return TS_FAILURE;
	}

	memset(pImgBuf, 114, ALG_BODY_DETECT_IMAGE_C * ALG_BODY_DETECT_IMAGE_H * ALG_BODY_DETECT_IMAGE_W);

	TS_ALG_SetLogLevel(ALG_LOG_DEBUG);

	s32Ret = TS_ALG_BodyDetect_Init(&phandle, &param);
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_BodyDetectInit error\n");
		goto ALG_RELEASE;
	}

	ALG_LOGD("version:%s\n" ,TS_ALG_BodyDetect_GetVersion());
	ALG_IMAGE_S image;
	image.s32C = ALG_BODY_DETECT_IMAGE_C;
	image.s32H = ALG_BODY_DETECT_IMAGE_H;
	image.s32W = ALG_BODY_DETECT_IMAGE_W;
	image.pData = pImgBuf;
	// image.pData = bodydetect_bgra_640x384;

	memcpy(pImgBuf + 12*image.s32C*image.s32W, data, image.s32W*image.s32C*(image.s32H-24));

	s32Ret = TS_ALG_BodyDetect_Process(phandle, &image, result);
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_BodyDetectProcess error\n");
		goto ALG_RELEASE;
	}

	ALG_LOGD("u32BodyNum:%d\n", result->u32BodyNum);
	for(TS_U32 i = 0; i < result->u32BodyNum; i++) {

		ALG_LOGD("body:%d --score:%f xmin:%f ymin:%f xmax:%f ymax:%f\n", i, result->stBox[i].f32Score,
				result->stBox[i].f32Xmin, result->stBox[i].f32Ymin, result->stBox[i].f32Xmax, result->stBox[i].f32Ymax);
	}
ALG_RELEASE:

	s32Ret =TS_ALG_BodyDetect_Exit(phandle);
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_BodyDetectRelease error\n");
		return TS_FAILURE;
	}

	return s32Ret;
}


TS_S32 SAMPLE_TEST_AlgDetect()
{
	TS_CHAR szInFileName[128] = "input_rgba.bin";
    TS_CHAR szOutFileName[128] = "output_rgba.bin";
    TS_S32 src_w = 1920;
    TS_S32 src_h = 1080;
    TS_S32 src_c = 4;
    TS_S32 dst_w = 640;
    TS_S32 dst_h = 360;

	TS_S32 s32Ret = TS_FAILURE;

	cv::Size srcSize(src_w, src_h);
    cv::Mat argbIn(srcSize, CV_8UC4);

	ALG_IMAGE_S image1, image2;
	image1.s32C = 4;
	image1.s32H = src_h;
	image1.s32W = src_w;

	system("echo N > /sys/module/printk/parameters/ignore_loglevel");
	system("echo 4 > /proc/sys/kernel/printk");

	s32Ret = TS_MPI_TRP_CVE_Init();
	if(0 != s32Ret) {
		ALG_LOGE("TS_MPI_TRP_CVE_Init error\n");
		return -1;
	}

	VB_CONFIG_S stVbConf;
    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
    stVbConf.u32MaxPoolCnt = 1;
    stVbConf.astCommPool[0].u64BlkSize = 10000;
    stVbConf.astCommPool[0].u32BlkCnt = 2;
	s32Ret = TS_MPI_VB_SetConfig(&stVbConf);
	if (TS_SUCCESS != s32Ret)
    {
        ALG_LOGE("TS_MPI_VB_SetConf failed!\n");
        return TS_FAILURE;
    }
	VB_POOL poolId;
	TS_U32 blkSize = src_w * src_h * src_c;
	TS_U32 blkCnt = 2;
	s32Ret = TS_MPI_VB_Init();
	if(TS_SUCCESS != s32Ret) {
		ALG_LOGE("SAMPLE_SYS_Init failed, s32Ret:%d\n", s32Ret);
		return TS_FAILURE;
	}

	VB_POOL_CONFIG_S stVbPoolCfg;
	stVbPoolCfg.u64BlkSize = blkSize;
    stVbPoolCfg.u32BlkCnt = blkCnt;
	poolId = TS_MPI_VB_CreatePool(&stVbPoolCfg);
	VB_BLK VbHandle = TS_MPI_VB_GetBlock(poolId, blkSize, TS_NULL);
	if (VB_INVALID_HANDLE == VbHandle)
    {
        ALG_LOGE("TS_MPI_VB_GetBlock failed!\n");
        return -1;
    }
	TS_U64 phyAddrSrc = TS_MPI_VB_Handle2PhysAddr(VbHandle);
	if(0 == phyAddrSrc){
		ALG_LOGE("TS_MPI_VB_Handle2PhysAddr err! ret:%lld\n", phyAddrSrc);
		return -1;
	}
    s32Ret = TS_MPI_VB_MmapPool(poolId);
	void *virAddrSrc = NULL;
	TS_MPI_VB_GetBlockVirAddr(poolId, phyAddrSrc, &virAddrSrc);

	ALG_LOGD("phyAddrSrc:0x%llx virAddrSrc:%p \n", phyAddrSrc, virAddrSrc);
	image1.pDataPhy = phyAddrSrc;
	image1.pData = virAddrSrc;

	// image2
	VB_BLK VbHandle2 = TS_MPI_VB_GetBlock(poolId, dst_w*dst_h*src_c, TS_NULL);
	if (VB_INVALID_HANDLE == VbHandle2)
    {
        ALG_LOGE("TS_MPI_VB_GetBlock failed!\n");
        return -1;
    }
	TS_U64 phyAddrSrc2 = TS_MPI_VB_Handle2PhysAddr(VbHandle2);
	if(0 == phyAddrSrc2){
		ALG_LOGE("TS_MPI_VB_Handle2PhysAddr err! ret:%lld\n", phyAddrSrc2);
		return -1;
	}
	void *virAddrSrc2 = NULL;
	TS_MPI_VB_GetBlockVirAddr(poolId, phyAddrSrc2, &virAddrSrc2);

	ALG_LOGD("phyAddrSrc:0x%llx virAddrSrc:%p \n", phyAddrSrc2, virAddrSrc2);
	image2.pDataPhy = phyAddrSrc2;
	image2.pData = virAddrSrc2;

	//加载测试数据
    FILE *pFileRead = fopen(szInFileName, "rb");
    if (TS_NULL == pFileRead)
    {
        SAMPLE_PRT("can't open file %s\n", szInFileName);
        return TS_FAILURE;
    }
    fread((char*)image1.pData, src_w*src_c, src_h, pFileRead);
    fclose(pFileRead);
    pFileRead = NULL;

	argbIn.data = (uchar*)image1.pData;

	s32Ret = TS_MPI_TRP_CVE_Resize(image1.pDataPhy, image1.pData, image2.pDataPhy, image2.pData,
								src_w, src_h, src_w, src_h, dst_w, dst_h, src_c);
	if(0 != s32Ret) {
		ALG_LOGE("TS_MPI_TRP_CVE_Resize error\n");
		return TS_FAILURE;
	}
	printf("TS_MPI_TRP_CVE_Resize rgba 1920x1080 to rgba 640*360\n");

	#if 0
	FILE* pOut = fopen("model_input.rgba", "wb");
	fwrite(image2.pData, dst_w*4, dst_h, pOut);
	fclose(pOut);
	#endif

	//网络推理
	ALG_BODY_RESULT_S result;
	SAMPLE_ALG_Detection(image2.pData, &result);

	// 画矩形框
	for(TS_U32 i = 0; i < result.u32BodyNum; i++) {
		int w = result.stBox[i].f32Xmax*src_w - result.stBox[i].f32Xmin*src_w;
		int h = result.stBox[i].f32Ymax*src_h - result.stBox[i].f32Ymin*src_h;
		int x = result.stBox[i].f32Xmin*src_w;
		int y = result.stBox[i].f32Ymin*src_h;
		//printf("w:%d h:%d x:%d y:%d\n",w,h,x,y);
		cv::Rect rect(x, y, w, h);
		cv::rectangle(argbIn, rect, cv::Scalar(0, 255, 255), 2);
	}

	// 保存图像
	//cv::imwrite("save.jpg", argbOut);
	printf("---------------------------\n");
	//保存画框后的原图数据
    pFileRead = fopen(szOutFileName, "wb");
    if (TS_NULL != pFileRead) {
        TS_S32 size = src_w*src_h*src_c;
        printf("out file w=%d h=%d, size=%d ptr=%p\n", src_w, src_h, size, (void*)argbIn.data);
        TS_S32 ret = fwrite((char*)argbIn.data, 1, size, pFileRead);
        fclose(pFileRead);
        printf("success to save out file, ret=%d\n", ret);
    }

	// release mem

    TS_S32 ret = TS_MPI_VB_MunmapPool(poolId);
    ret = TS_MPI_VB_DestroyPool(poolId);
	(void)ret;
	TS_MPI_VB_Exit();

	TS_MPI_TRP_CVE_Exit();
	return 0;
}

#if 1
#include "alg_colorconvert/nv12_360p.h"  //640*360
// #include "alg_colorconvert/out_nv12.h"  //640*360
TS_S32 SAMPLE_TEST_AlgColorConvert()
{
#define IMG_W	640
#define IMG_H	360

	TS_U8 *imageS = (TS_U8 *)malloc(IMG_W * IMG_H * 4 * sizeof(TS_U8));
	FILE *fp0 = fopen("test_rgb.rgb", "wb");
	FILE *fp1 = fopen("test_rgba.rgba", "wb");

	long t0 = SAMPLE_TSCV_GetTime();
	int ret = TS_ALG_NV12ToRGB(nv12_360p, imageS, IMG_W, IMG_H, ALG_COLOR_TYPE_RGB24);
	if(ret < 0){
		return TS_FAILURE;
	}
	long t1 = SAMPLE_TSCV_GetTime();
	SAMPLE_PRT("nv12 -> rgb timecost: %ld\n", t1-t0);
	fwrite(imageS, 1, IMG_W*IMG_H*3, fp0);

	t0 = SAMPLE_TSCV_GetTime();
	ret = TS_ALG_NV12ToRGB(nv12_360p, imageS, IMG_W, IMG_H, ALG_COLOR_TYPE_RGBA32);
	if(ret < 0){
		return TS_FAILURE;
	}
	t1 = SAMPLE_TSCV_GetTime();
	SAMPLE_PRT("nv12 -> rgba timecost: %ld\n", t1-t0);
	fwrite(imageS, 1, IMG_W*IMG_H*4, fp1);
	printf("success \n");
	fclose(fp0);
	fclose(fp1);
	free(imageS);

	return ret;
}
#endif

static TS_VOID SAMPLE_TSALG_Usage(TS_CHAR *sPrgNm)
{
	printf("Usage : %s <index>\n", sPrgNm);
	printf("index:\n");

	printf("\t 0)detect\n");
	printf("\t 1)nv12torgb\n");
}

static TS_VOID SAMPLE_TSALG_HandleSig(TS_S32 signo)
{
	if (SIGINT == signo || SIGTERM == signo) {
		printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
	}

	exit(-1);
}

TS_S32 main(int argc, char *argv[])
{
	TS_S32 s32Ret = TS_FAILURE;
	TS_S32 s32Index;

	if (argc < 2 || argc > 2) {
		SAMPLE_TSALG_Usage(argv[0]);
		return TS_FAILURE;
	}

	if (!strncmp(argv[1], "-h", 2)) {
		SAMPLE_TSALG_Usage(argv[0]);
		return TS_SUCCESS;
	}

#ifndef __TsingLite__
	signal(SIGINT, SAMPLE_TSALG_HandleSig);
	signal(SIGTERM, SAMPLE_TSALG_HandleSig);
#endif

	s32Index = atoi(argv[1]);
	switch (s32Index) {
	case 0:
		s32Ret = SAMPLE_TEST_AlgDetect();
		break;
	case 1:
		s32Ret = SAMPLE_TEST_AlgColorConvert();
		break;
	default:
		SAMPLE_PRT("the index %d is invaild!\n", s32Index);
		SAMPLE_TSALG_Usage(argv[0]);
		s32Ret = TS_FAILURE;
		break;
	}

	if (TS_SUCCESS == s32Ret)
		SAMPLE_PRT("program exit normally!\n");
	else
		SAMPLE_PRT("program exit abnormally!\n");

	return s32Ret;
}


