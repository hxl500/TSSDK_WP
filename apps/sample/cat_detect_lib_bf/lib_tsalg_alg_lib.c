
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

#include "ts_alg_type.h"
#include "ts_alg_log.h"
#include "tsalg_alg_lib.h"
#include "ts_alg_log.h"
//#include "ts_alg_amr.h"
#include "mpi_vb.h"
//#include "logServer.h"
#include "ipc_base.h"
#include "arrr_diff.h"
//#include "ts_alg_color_convert.h"

#include "ts_alg_imgproc.h"
#include "ts_alg_body_detect_v2.h"
#define TEST_MODE_OUT 0

#if TEST_MODE_OUT
#define MODE_OUT_PRE "/media/mmcblk0p1/testfile/test_mode_out/test_out_%d.rgba"
#define MODE_OUTBIT_PRE "/media/mmcblk0p1/testfile/test_mode_out/test_out_%d.bit"
#define MODE_OUTORI_PRE "/media/mmcblk0p1/testfile/test_mode_out/test_ori_%d.yuv"
char out_name[128] = {0};
char out_name_bit[128] = {0};
char out_name_ori[128] = {0};
#endif

#ifndef LOGE
#define LOGE printf
#endif
int TS_ALG_Soft_Resize(const unsigned char* pSrc, unsigned char* pDst, int srcW, int srcH, int RoiW, int RoiH, int dstW, int dstH, int channels, unsigned char *pTmp)
{
	unsigned char* DstBuf = pDst;
	const unsigned char* SrcBuf = pSrc;
	//printf("srcW=%d,srcH=%d,RoiW=%d,RoiH=%d,dstW=%d,dstH=%d\n",srcW,srcH,RoiW,RoiH,dstW,dstH);

	int ch = 0;
	int u, v = 0;
	int cu, cv;

	int iy;
	int ix;

	float x;

	if ((pTmp == NULL) || (pSrc == NULL) || (pDst == NULL))
	{
		printf("parameter null\n");
		return -1;
	}

	int *pCv = (int*)pTmp;
	int *pV = pCv + dstW;
	int* pIx = pV + dstW;


	//int nSrcWidthStep = srcW;
	int nDstWidthStep = dstW * channels;
	float scale_w = (float)RoiW / (float)dstW;
	float scale_h = (float)RoiH / (float)dstH;

	for (int i = 0; i < dstW; ++i)
	{
		x = ((float)(i)+0.5f) * scale_w - 0.5f;
		pV[i] = (int)((x - (int)(x)) * 2048);
		pCv[i] = 2048 - pV[i];

		ix = (int)x;
		if (ix + 1 >= RoiW) {
			ix = RoiW - 2;
		}

		pIx[i] = ix;
	}

	//int dst_offset =0;
	for (int h = 0; h < dstH; ++h)
	{
		float y = ((float)(h)+0.5f) * scale_h - 0.5f;
		unsigned char* pDstLine = DstBuf + h * nDstWidthStep;

		u = (int)((y - (int)(y)) * 2048);
		cu = 2048 - u;

		iy = (int)y;
		if (iy + 1 >= RoiH) {
			iy = RoiH - 2;
		}

		int cor[2];
		cor[0] = iy * srcW;
		cor[1] = (iy + 1) * srcW;

		for (int w = 0; w < dstW; ++w)
		{
			v = pV[w];
			cv = pCv[w];
			ix = pIx[w];

			int pro[4] = { cu * cv, cu * v, u * cv, u * v };

			int dst_offset = w * channels;
			int src_x0_0 = (cor[0] + ix) * channels;
			int src_x0_1 = src_x0_0 + channels;
			int src_x1_0 = (cor[1] + ix) * channels;
			int src_x1_1 = src_x1_0 + channels;

			for (ch = 0; ch < channels; ch++)
			{
				pDstLine[dst_offset++] = (unsigned char)((SrcBuf[src_x0_0 + ch] * pro[0]
														+ SrcBuf[src_x0_1 + ch] * pro[1]
														+ SrcBuf[src_x1_0 + ch] * pro[2]
														+ SrcBuf[src_x1_1 + ch] * pro[3]) >> 22);
			}
		}
	}
	return 0;
}

int rsn_detect_file(char *src_file,int *result)
{
    TS_S32 dst_w = 224; //256;//640;
    TS_S32 dst_h = 224; //256;//360;

    //printf("(%s)%d###########################\n", __func__, __LINE__);

	TS_S32 s32Ret = TS_FAILURE;

    TS_VOID *pdstData;
    TS_U64 pdstDataPhy;

	TS_S32 src_c = 4; //SAMPLE_TSCV_GetBitSizeByFmt(PIXEL_FORMAT_RGBA_8888);
//    TS_S32 insize = src_w*src_h*src_c;


	TS_U32 allocSize = dst_w*dst_h*src_c;
	s32Ret = TS_MPI_SYS_MmzAlloc(&pdstDataPhy, (void**)&pdstData, NULL, TS_NULL, allocSize);
    if(TS_SUCCESS != s32Ret)
    {
        LOGE("TS_MPI_SYS_MmzAlloc err:0x%x",s32Ret);
        return TS_FAILURE;
    }

    FILE *file = fopen(src_file, "rb");
    if (TS_NULL == file)
    {
        LOGE("can't open file %s\n", src_file);
        goto RSN_DETECT_FILE_END;
    }

    int r = fread((char*)pdstData, 1, allocSize, file);
    if(r<1)
    {
    	LOGE("can't read file %s\n", src_file);
    }
    fclose(file);
#if 0
    ALG_IMAGE_S dstImg = {0};
    dstImg.s32W = dst_w;
    dstImg.s32H = dst_h;
    dstImg.s32C = src_c;
    dstImg.pData = pdstData;
    dstImg.pDataPhy = pdstDataPhy;
#endif
    TS_S32 rsn_reault[128];
	//while(1){
	memset(rsn_reault,0,sizeof(rsn_reault));
    //ALG_RSN_Detect_Process(&dstImg, rsn_reault);
	int j;
	printf("rsn_detect_file=%s",src_file);
	for(j=0;j<128;j++){
		printf("%d ",rsn_reault[j]);
	}
	printf("\n");
	//}

	memcpy(result,rsn_reault,sizeof(rsn_reault));

RSN_DETECT_FILE_END:
    // 释放mmz buf
    TS_MPI_SYS_MmzFree(pdstDataPhy,(TS_VOID *)pdstData);
//	SAMPLE_TSCV_ReleaseVb(argbIn.mpi_phyaddr);
//	SAMPLE_TSCV_ReleaseVb(argbOut.mpi_phyaddr);
	return TS_SUCCESS;
}

int test_crop_resize_detect()
{
    char *src_file[] = {
        "/root/sd/t/1-0.rgba",
        "/root/sd/t/1-15.rgba",
        "/root/sd/t/2-0.rgba",
        "/root/sd/t/2-23.rgba",
    };

    TS_S32 dst_w = 224; //256;//640;
    TS_S32 dst_h = 224; //256;//360;

    //printf("(%s)%d###########################\n", __func__, __LINE__);

	TS_S32 s32Ret = TS_FAILURE;

    TS_VOID *pdstData;
    TS_U64 pdstDataPhy;

	TS_S32 src_c = 4; //SAMPLE_TSCV_GetBitSizeByFmt(PIXEL_FORMAT_RGBA_8888);
//    TS_S32 insize = src_w*src_h*src_c;


	TS_U32 allocSize = dst_w*dst_h*src_c;
	s32Ret = TS_MPI_SYS_MmzAlloc(&pdstDataPhy, (void**)&pdstData, NULL, TS_NULL, allocSize);
    if(TS_SUCCESS != s32Ret)
    {
        LOGE("TS_MPI_SYS_MmzAlloc err:0x%x",s32Ret);
        return TS_FAILURE;
    }

    int i;
    for(i = 0; i < sizeof(src_file)/sizeof(char*); i++) {
        FILE *file = fopen(src_file[i], "rb");
        if (TS_NULL == file)
        {
            LOGE("can't open file %s\n", src_file[i]);
            continue;
        }
        int r = fread((char*)pdstData, 1, allocSize, file);
        if(r < 1)
        {
		LOGE("can't read file %s\n", src_file[i]);
        }
        fclose(file);
#if 0
        ALG_IMAGE_S dstImg = {0};
        dstImg.s32W = dst_w;
        dstImg.s32H = dst_h;
        dstImg.s32C = src_c;
        dstImg.pData = pdstData;
        dstImg.pDataPhy = pdstDataPhy;
#endif
        TS_S32 rsn_reault[128];
		memset(rsn_reault,0,sizeof(rsn_reault));
        //ALG_RSN_Detect_Process(&dstImg, rsn_reault);
		int j;
		printf("rsn_detect_file=%s",src_file[i]);
		for(j=0;j<128;j++){
			printf("%d ",rsn_reault[j]);
		}
		printf("\n");

    }

    // 释放mmz buf
    TS_MPI_SYS_MmzFree(pdstDataPhy,(TS_VOID *)pdstData);
//	SAMPLE_TSCV_ReleaseVb(argbIn.mpi_phyaddr);
//	SAMPLE_TSCV_ReleaseVb(argbOut.mpi_phyaddr);
	return TS_SUCCESS;
}
//extern char  get_catName_id(int *src);
//extern void normalize_array(float *arr);
#if 0
static int wifidata_file(char*buf,size_t size,char*file){
    char head[256];
    FILE*fp;
    fp = fopen(file,"w");
	if(fp == NULL){
		return -1;
	}
	int tmplen = 0;
	while(size-tmplen> 0){
		//printf("tmplen=%d\n",tmplen);
		if(size-tmplen > 512){
			 fwrite(&buf[tmplen],1,512,fp);
			 tmplen+=512;
		}else{
			 fwrite(&buf[tmplen],1,size-tmplen,fp);
			 tmplen+=size-tmplen;
		}
	}
    fclose(fp);
    return 0;
}
#endif

//static int numi = 0;

float golden_data[128] =
{
0.105838,
-0.047731,-0.037354,-0.157719,0.168095,-0.066408,0.066408,-0.066408,0.029053,0.116214,0.010376,0.026978,-0.051881,0.093386,-0.101687,-0.008301,0.074709,
-0.037354,-0.078859,-0.134891,0.083010,-0.130741,0.101687,0.047731,0.022828,0.026978,0.037354,0.172245,0.037354,0.083010,0.074709,-0.205449,-0.037354,
0.089236,0.029053,-0.157719,-0.066408,0.056032,0.060182,0.014527,-0.078859,-0.051881,0.026978,0.045655,0.022828,-0.105838,0.029053,-0.093386,-0.101687,
-0.060182,0.041505,-0.120364,0.060182,-0.112063,-0.163944,0.018677,-0.008301,0.056032,-0.070558,-0.066408,-0.022828,0.107913,0.037354,0.018677,-0.045655,
-0.112063,0.064333,0.060182,-0.126590,-0.126590,-0.041505,-0.056032,-0.168095,-0.112063,0.029053,-0.041505,-0.112063,0.056032,-0.064333,0.037354,0.047731,
-0.041505,-0.093386,0.172245,0.078859,0.116214,-0.041505,-0.047731,0.134891,0.126590,0.060182,0.182622,-0.126590,0.145267,-0.201299,-0.018677,0.029053,
0.163944,0.060182,0.087160,-0.105838,0.000000,0.097537,0.029053,0.060182,0.056032,0.093386,0.014527,0.083010,-0.045655,-0.176396,-0.186772,-0.056032,
-0.124515,0.037354,-0.051881,-0.097537,0.004150,-0.051881,-0.004150,-0.060182,0.018677,-0.101687,-0.074709,0.026978,-0.033204,0.029053,0.051881
};
float test_data[128] =
{
-0.074212,0.082275,-0.047310,0.006453,0.085335,-0.175609,-0.034716,0.133076,0.018125,-0.123320,-0.049209,0.025218,-0.063090,0.152566,0.059229,-0.066230,
-0.007800,-0.093555,0.050622,0.093153,0.115271,-0.106774,-0.138167,-0.031105,-0.038710,-0.039256,0.110402,0.008886,-0.026044,-0.026048,-0.082398,-0.123796,
0.060152,-0.066053,-0.133247,-0.166508,-0.006585,0.021358,0.164505,-0.032580,-0.023994,-0.011780,0.051314,-0.017247,0.000345,-0.099082,-0.075391,-0.084814,
0.027955,0.018215,0.007154,-0.010606,0.074563,0.026345,-0.087061,0.052169,0.072446,0.120206,-0.121466,-0.011050,-0.110252,-0.047833,-0.045267,0.000529,
-0.177405,0.032235,-0.130532,0.012194,0.002232,-0.014030,-0.068411,-0.079661,0.079071,-0.066128,-0.063631,-0.192099,-0.062538,0.028358,-0.130084,-0.063607,
0.122958,-0.009686,-0.075179,0.108077,-0.097193,-0.172912,0.046958,0.119434,-0.042904,0.175990,0.166158,0.022577,0.112140,-0.031635,-0.150888,0.006622,
0.071356,0.148065,0.102713,0.206766,-0.044532,-0.023518,0.046552,-0.085055,0.043623,-0.030340,-0.051062,-0.028805,0.010194,-0.042257,-0.121362,0.008292,
-0.020959,0.123634,-0.007502,-0.181888,-0.114864,-0.078392,-0.152570,-0.007003,-0.144144,0.043730,0.035349,-0.045110,-0.059912,-0.000418,-0.017806,-0.182444
};
void get_rsn_reault(char*buf,float *rsn_reault){
	 uint32_t vbPool = 0;
	 VB_BLK vbHandle = 0;
    ALG_IMAGE_S AlgoFaceRecogIn = {0};
	VB_POOL_CONFIG_S stVbPoolCfg = {0};
    TS_S32 dst_w = 224; //256;//640;
    TS_S32 dst_h = 224; //256;//360;
    TS_S32 s32Ret = TS_FAILURE;

    AlgoFaceRecogIn.s32W = dst_w;
    AlgoFaceRecogIn.s32H = dst_h;
    AlgoFaceRecogIn.s32C = 4;
	TS_S32 alg_rgb_size = AlgoFaceRecogIn.s32H * AlgoFaceRecogIn.s32W * AlgoFaceRecogIn.s32C;
	stVbPoolCfg.u32BlkCnt = 1;
    stVbPoolCfg.u64BlkSize = alg_rgb_size+64;
	vbPool = TS_MPI_VB_CreatePool(&stVbPoolCfg);
	vbHandle = TS_MPI_VB_GetBlock(vbPool, stVbPoolCfg.u64BlkSize, TS_NULL);
    if (VB_INVALID_HANDLE == vbHandle)
    {
        printf("TS_MPI_VB_GetBlock failed!\n");
    }
	AlgoFaceRecogIn.pDataPhy = TS_MPI_VB_Handle2PhysAddr(vbHandle);
    if (0 == AlgoFaceRecogIn.pDataPhy)
    {
        printf("TS_MPI_VB_Handle2PhysAddr failed!\n");
    }
    s32Ret = TS_MPI_VB_MmapPool(vbPool);
    if (s32Ret != 0)
    {
        printf("TS_MPI_VB_MmapPool failed!.\n");
    }

	s32Ret = TS_MPI_VB_GetBlockVirAddr(vbPool, AlgoFaceRecogIn.pDataPhy, (TS_VOID **)(&AlgoFaceRecogIn.pData));
	if (s32Ret != 0)
    {
        printf("TS_MPI_VB_GetBlockVirAddr failed!\n");
    }
	memset(AlgoFaceRecogIn.pData, 114, alg_rgb_size);
	memcpy(AlgoFaceRecogIn.pData,buf,alg_rgb_size);
	//TS_FLOAT rsn_reault[128];
	memset(rsn_reault,0,sizeof(TS_FLOAT)*CAT_ARRAY_LEN);
    ALG_RSN_Detect_Process(&AlgoFaceRecogIn, rsn_reault);
    TS_MPI_VB_MunmapPool(vbPool);
	TS_MPI_VB_ReleaseBlock(vbHandle);
	TS_MPI_VB_DestroyPool(vbPool);	
    return;

	
}
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} RGBA;

static void desaturate_rgba(RGBA* data, int width, int height) {
    for (int i = 0; i < width * height; i++) {
        // 使用亮度公式计算灰度值
        uint8_t gray = (uint8_t)(0.299f * data[i].r +
                               0.587f * data[i].g +
                               0.114f * data[i].b);

        // 保持RGBA格式，但RGB通道设为相同灰度值
        data[i].r = gray;
        data[i].g = gray;
        data[i].b = gray;
        // Alpha通道保持不变
    }
}

int rsn_detect_file2(char *src_file,float *result,float *result2)
{
	   TS_S32 dst_w = 224; //256;//640;
    TS_S32 dst_h = 224; //256;//360;
    printf("rsn file =%s\n",src_file);
    FILE *file = fopen(src_file, "rb");
    if (TS_NULL == file)
    {
        printf("can't open file %s\n", src_file);
	return -1;
	//goto RSN_DETECT_FILE_END;
    }
	char *buf = malloc(dst_w*dst_h*4);
    int r = fread((char*)buf, 1, dst_w*dst_h*4, file);
    if(r < 1)
    {
    	printf("can't read file %s\n", src_file);
		free(buf);
		fclose(file);
		return -2;
    }
    fclose(file);
	get_rsn_reault(buf,result);
	
	desaturate_rgba((RGBA*)buf,dst_w,dst_h);
	get_rsn_reault(buf,result2);
	free(buf);
	printf("rsn file ok =%s\n",src_file);
	return 0;
}

char JSON_LOCAL[128];

char my_crop_resize_detect_food(ALG_IMAGE_S *srcImg, RECT *prect,char *idstr){
    TS_S32 s32Ret = TS_FAILURE;
    uint32_t vbPool = 0;
	//TS_U64 u64PhyAddr = 0;
	VB_POOL_CONFIG_S stVbPoolCfg = {0};
	VB_BLK vbHandle = 0;
    TS_S32 dst_w = 224; //256;//640;
    TS_S32 dst_h = 224; //256;//360;
    printf("(%s)%d###########################\n", __func__, __LINE__);

    ALG_IMAGE_S AlgoFaceRecogIn = {0};
    AlgoFaceRecogIn.s32W = dst_w;
    AlgoFaceRecogIn.s32H = dst_h;
    AlgoFaceRecogIn.s32C = 4;
    TS_S32 alg_rgb_size = AlgoFaceRecogIn.s32H * AlgoFaceRecogIn.s32W * AlgoFaceRecogIn.s32C;
	stVbPoolCfg.u32BlkCnt = 1;
    stVbPoolCfg.u64BlkSize = alg_rgb_size+64;
	vbPool = TS_MPI_VB_CreatePool(&stVbPoolCfg);
	vbHandle = TS_MPI_VB_GetBlock(vbPool, stVbPoolCfg.u64BlkSize, TS_NULL);
    if (VB_INVALID_HANDLE == vbHandle)
    {
        printf("TS_MPI_VB_GetBlock failed!\n");
    }

	AlgoFaceRecogIn.pDataPhy = TS_MPI_VB_Handle2PhysAddr(vbHandle);
    if (0 == AlgoFaceRecogIn.pDataPhy)
    {
        printf("TS_MPI_VB_Handle2PhysAddr failed!\n");
    }
    s32Ret = TS_MPI_VB_MmapPool(vbPool);
    if (s32Ret != 0)
    {
        printf("TS_MPI_VB_MmapPool failed!.\n");
    }

	s32Ret = TS_MPI_VB_GetBlockVirAddr(vbPool, AlgoFaceRecogIn.pDataPhy, (TS_VOID **)(&AlgoFaceRecogIn.pData));
	if (s32Ret != 0)
    {
        printf("TS_MPI_VB_GetBlockVirAddr failed!\n");
    }

	memset(AlgoFaceRecogIn.pData, 114, alg_rgb_size);
	TS_FLOAT rsn_reault[CAT_ARRAY_LEN];
	printf("go to resize_rgba_withjson\n");
	//resize_rgba_withjson(JSON_LOCAL, srcImg->pData,srcImg->s32W,srcImg->s32H, AlgoFaceRecogIn.pData);
	ALG_RSN_Detect_Process(&AlgoFaceRecogIn, rsn_reault);
    TS_MPI_VB_MunmapPool(vbPool);
	TS_MPI_VB_ReleaseBlock(vbHandle);
	TS_MPI_VB_DestroyPool(vbPool);
	prect->left = rsn_reault[0];
	return 0;
	

}



#define S_MIN(a, b) ((a) < (b) ? (a) : (b))




char my_crop_resize_detect(ALG_IMAGE_S *srcImg, RECT *prect,char *idstr)
{
    static int wr_x_id = 0;
    TS_S32 s32Ret = TS_FAILURE;
    uint32_t vbPool = 0;
	//TS_U64 u64PhyAddr = 0;
	VB_POOL_CONFIG_S stVbPoolCfg = {0};
	VB_BLK vbHandle = 0;
    TS_S32 dst_w = 224; //256;//640;
    TS_S32 dst_h = 224; //256;//360;
	char id;
    //printf("(%s)%d###########################\n", __func__, __LINE__);

    ALG_IMAGE_S AlgoFaceRecogIn = {0};
    AlgoFaceRecogIn.s32W = dst_w;
    AlgoFaceRecogIn.s32H = dst_h;
    AlgoFaceRecogIn.s32C = 4;
    TS_S32 alg_rgb_size = AlgoFaceRecogIn.s32H * AlgoFaceRecogIn.s32W * AlgoFaceRecogIn.s32C;
	stVbPoolCfg.u32BlkCnt = 1;
    stVbPoolCfg.u64BlkSize = alg_rgb_size+64;
	vbPool = TS_MPI_VB_CreatePool(&stVbPoolCfg);
	vbHandle = TS_MPI_VB_GetBlock(vbPool, stVbPoolCfg.u64BlkSize, TS_NULL);
    if (VB_INVALID_HANDLE == vbHandle)
    {
        printf("TS_MPI_VB_GetBlock failed!\n");
    }

	AlgoFaceRecogIn.pDataPhy = TS_MPI_VB_Handle2PhysAddr(vbHandle);
    if (0 == AlgoFaceRecogIn.pDataPhy)
    {
        printf("TS_MPI_VB_Handle2PhysAddr failed!\n");
    }
    s32Ret = TS_MPI_VB_MmapPool(vbPool);
    if (s32Ret != 0)
    {
        printf("TS_MPI_VB_MmapPool failed!.\n");
    }

	s32Ret = TS_MPI_VB_GetBlockVirAddr(vbPool, AlgoFaceRecogIn.pDataPhy, (TS_VOID **)(&AlgoFaceRecogIn.pData));
	if (s32Ret != 0)
    {
        printf("TS_MPI_VB_GetBlockVirAddr failed!\n");
    }

	memset(AlgoFaceRecogIn.pData, 114, alg_rgb_size);

    ALG_IMAGE_S stImage;
    stImage.s32C = 1;
    stImage.s32H = AlgoFaceRecogIn.s32H;
    stImage.s32W = AlgoFaceRecogIn.s32W;
    stImage.pData = malloc(stImage.s32H*stImage.s32W*2);
    memset(stImage.pData, 0, stImage.s32H*stImage.s32W*2);


    ALG_SUB_IMAGE_S sub;
	//rect.left = pTmpResult->stBox[i].f32Xmin * pImage->s32W;
	//rect.top = pTmpResult->stBox[i].f32Ymin * pImage->s32H;
	//rect.right = pTmpResult->stBox[i].f32Xmax * pImage->s32W;
	//rect.bottom = pTmpResult->stBox[i].f32Ymax * pImage->s32H;
    sub.s32X = S_MIN(prect->left,srcImg->s32W);
    sub.s32Y = S_MIN(srcImg->s32H,prect->top);
    sub.s32W = S_MIN(srcImg->s32W,prect->right)- sub.s32X;
    sub.s32H = S_MIN(srcImg->s32H,prect->bottom)- sub.s32Y;
	if(sub.s32W < 0){
		sub.s32W  = 0;
	}
	if(sub.s32H <0){
		sub.s32H = 0;
	}
	//printf("sub.s32X=%d,sub.s32Y=%d,sub.s32W=%d,sub.s32H=%d\n",sub.s32X,sub.s32Y,sub.s32W,sub.s32H);

    //sub.s32X = prect->left;
    //sub.s32Y = prect->top;
   // sub.s32W = prect->right - prect->left;
   // sub.s32H = prect->bottom - prect->top;
    //ALG_LOGD("srcImg->s32W %d\n",srcImg->s32W);
    //ALG_LOGD("sub.s32X %d,sub.s32Y %d,sub.s32W %d,sub.s32H %d\n",sub.s32X,sub.s32Y,sub.s32W,sub.s32H);
    TS_U8 *pSrcY = srcImg->pData;
    TS_U8 *pSrcUv = pSrcY + srcImg->s32W * srcImg->s32H;

    TS_U8 *pY = pSrcY + (sub.s32Y*srcImg->s32W*srcImg->s32C) + sub.s32X*srcImg->s32C;
    TS_U8 *pUv = pSrcUv + (sub.s32Y >> 1)*srcImg->s32W*srcImg->s32C + (sub.s32X & 0xfffffffe)*srcImg->s32C;

    ALG_IMAGE_S *pTmpImg = &stImage;
    TS_U8 *pTmpBuf = (TS_U8 *)stImage.pData + stImage.s32H*stImage.s32W*3/2;
    //TS_U32 time0 = TIME_CACL_GET();
    //ALG_LOGD("resize y\n");
    s32Ret = TS_ALG_Soft_Resize(pY, (TS_U8 *)pTmpImg->pData, srcImg->s32W, srcImg->s32H, sub.s32W, sub.s32H, pTmpImg->s32W, pTmpImg->s32H, pTmpImg->s32C, pTmpBuf);
    if(0 != s32Ret) {
        ALG_LOGE("TS_ALG_FaceBigAngle_Process error\n");
        return -1;
    }

    //ALG_LOGD("resize uv\n");
    s32Ret = TS_ALG_Soft_Resize(pUv, (TS_U8 *)pTmpImg->pData + AlgoFaceRecogIn.s32W*AlgoFaceRecogIn.s32H, srcImg->s32W>>1, srcImg->s32H>>1, sub.s32W>>1, sub.s32H>>1, AlgoFaceRecogIn.s32W>>1, AlgoFaceRecogIn.s32H>>1, 2, pTmpBuf);
    if(0 != s32Ret) {
        ALG_LOGE("TS_ALG_FaceBigAngle_Process error\n");
        return -1;
    }

    TS_ALG_YUV2RGB((TS_U8 *)pTmpImg->pData, (TS_U8 *)pTmpImg->pData + AlgoFaceRecogIn.s32W*AlgoFaceRecogIn.s32H, (TS_U8*)AlgoFaceRecogIn.pData, AlgoFaceRecogIn.s32W, AlgoFaceRecogIn.s32H, AlgoFaceRecogIn.s32W, AlgoFaceRecogIn.s32H, ALG_RGB_TYPE_RGBA32);

    #if 0
            FILE *pfOutput = fopen("vlcsnap-2024-10-25-10h37m30s086.rgba", "wb");
            if(pfOutput == NULL) {
                ALG_LOGE("open bin file error\n");
                return TS_FAILURE;
            }
            fwrite((TS_U8 *)AlgoFaceRecogIn.pData, 112*112*4, 1, pfOutput);
            fclose(pfOutput);
    #endif

#if 0
    TS_U64 start_off = crop_y * srcImg->s32W * srcImg->s32C + crop_x * srcImg->s32C;
    TS_U64 presizeDataPhy = srcImg->pDataPhy + start_off;
    TS_VOID *presizeData = (TS_U8 *)srcImg->pData + start_off;

    printf("(%s)%d###########################TS_MPI_TRP_CVE_Resize src s32W:%d s32H:%d s32C:%d\n",
            __func__, __LINE__, srcImg->s32W, srcImg->s32H, srcImg->s32C);
    s32Ret = TS_MPI_TRP_CVE_Resize(presizeDataPhy, presizeData, pdstDataPhy, (void*)pdstData,
            srcImg->s32W, srcImg->s32H, crop_w, crop_h, dst_w, dst_h, srcImg->s32C);

    if(wr_flag) {
        char out_filename[40];

    	if(1) {
    	    sprintf(out_filename, "cveresize_in_%dx%d_%02d%02d.rgba", srcImg->s32W, srcImg->s32H, wr_y_id, wr_x_id);
        	FILE* pOut = fopen(out_filename, "wb");
        	fwrite(srcImg->pData, srcImg->s32W*4, srcImg->s32H, pOut);
        	fclose(pOut);
    	}

    	if(1) {
    	    sprintf(out_filename, "cveresize_out_%dx%d_%02d%02d.rgba", dst_w, dst_h, wr_y_id, wr_x_id);
        	FILE* pOut = fopen(out_filename, "wb");
        	fwrite(pdstData, dst_w*4, dst_h, pOut);
        	fclose(pOut);
    	}

        wr_x_id++;
        crop_x += crop_w;
        if(crop_x + crop_w > srcImg->s32W) {
            crop_x = 0;
            crop_y += crop_h;

            wr_x_id = 0;
            wr_y_id++;
        }
        if(crop_y + crop_h > srcImg->s32H) {
            wr_flag = 0;
        }
    }
#else



    if(0) {
		char out_filename[40];
		FILE* pOut;


        sprintf(out_filename, "/media/mmcblk0p1/pic2/src_in_%d.yuv", wr_x_id);
        pOut = fopen(out_filename, "wb");
        //fwrite(pdstData, dst_w*4, dst_h, pOut);
		fwrite(srcImg->pData, srcImg->s32W*srcImg->s32H*1.5,1, pOut);
        fclose(pOut);

        sprintf(out_filename, "/media/mmcblk0p1/pic2/cresize_in_%d.rgba", wr_x_id);
        pOut = fopen(out_filename, "wb");
        fwrite((TS_U8 *)AlgoFaceRecogIn.pData, AlgoFaceRecogIn.s32W*AlgoFaceRecogIn.s32H*4, 1, pOut);
        fclose(pOut);

        wr_x_id++;
    }



#if 0
	if(numi < 200){
		memset(picpath,0,128);
		sprintf(picpath,"%spicdata.%d",PIC_TMP_V_PATH,numi);
		strcat(picpath,".rgb");
		wifidata_file(pdstData,dst_w*4*dst_h,picpath);
	}else{
		printf("numi error\n");
	}
#endif
#endif


	TS_FLOAT rsn_reault[CAT_ARRAY_LEN];
	//while(1){
    //pdstData 是224 * 224 即算法裁剪过后的图片 可以在这里输出

	//
	memset(rsn_reault,0,sizeof(rsn_reault));
    ALG_RSN_Detect_Process(&AlgoFaceRecogIn, rsn_reault);

	//rsn_reault 图片经过算法后计算的值 可以在这里输出
#if TEST_MODE_OUT
	static int set_name_flag = 1;
	if(set_name_flag)
	{
		// 使用当前时间作为随机数生成器的种子
		srand(time(0));

		// 生成一个0到99之间的随机数
		int random_number = rand() % 100;
		sprintf(out_name,MODE_OUT_PRE, random_number);
		sprintf(out_name_bit,MODE_OUTBIT_PRE,random_number);
		sprintf(out_name_ori,MODE_OUTORI_PRE,random_number);
		set_name_flag = 0;
	}
	FILE* pOut;
	FILE* pOutBit;
	FILE* pOutOri;
	pOut = fopen(out_name, "ab+");
	pOutBit = fopen(out_name_bit, "ab+");
	pOutOri = fopen(out_name_ori, "ab+");
	//fwrite(pdstData, dst_w*4, dst_h, pOut);//buf max imgae = W*H*4
	fwrite((TS_U8 *)AlgoFaceRecogIn.pData, AlgoFaceRecogIn.s32W*4,AlgoFaceRecogIn.s32H, pOut);
	//fwrite(srcImg->pData, srcImg->s32W*4,srcImg->s32H, pOutOri);
	fwrite(srcImg->pData, srcImg->s32W*1.5,srcImg->s32H, pOutOri);
	fwrite(rsn_reault,sizeof(TS_S32),128,pOutBit);
	fclose(pOut);
	fclose(pOutBit);
	fclose(pOutOri);
	printf("TEST_MODE_OUT %s imgDet w:%d h:%d object\n",
		out_name, dst_w, dst_h);
#endif
	//
	//int j;
#if 0
	printf("rsn_detect_file,numi=%d\n",numi);
	for(j=0;j<128;j++){
		printf("%d ",rsn_reault[j]);
	}
	printf("\n");
#endif

	//}

    // 释放mmz buf
    //TS_MPI_SYS_MmzFree(pdstDataPhy,(TS_VOID *)pdstData);
//	SAMPLE_TSCV_ReleaseVb(argbIn.mpi_phyaddr);
//	SAMPLE_TSCV_ReleaseVb(argbOut.mpi_phyaddr);


#if 0
	printf("numi error begin\n");
	if(numi < 200){
		printf("numi error1\n");
		memset(picpath,0,128);
		sprintf(picpath,"%spicdata.%d",PIC_TMP_V_PATH,numi++);
		strcat(picpath,".rgba.dat");
		wifidata_file((char*)rsn_reault,128*4,picpath);
	}else{
		printf("numi error2\n");
	}
#endif
	 wr_x_id++;
	//normalize_array(rsn_reault);
   	//extern float compare_vector(float *dst,float *src);
    //compare_vector(golden_data,rsn_reault);

	//id = get_catName_id(rsn_reault);



    TS_MPI_VB_MunmapPool(vbPool);
	TS_MPI_VB_ReleaseBlock(vbHandle);
	TS_MPI_VB_DestroyPool(vbPool);

	id = get_catName_id(rsn_reault,idstr);
	if(stImage.pData)
	{
		free(stImage.pData);
		stImage.pData =NULL;
	}
	return id;

	//return TS_SUCCESS;
}
