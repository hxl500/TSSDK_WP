#include <string.h>
#include <stdlib.h>  // for strtol
#include "mpi_sys.h"
#include "ts_cve.h"
#include "mpi_cve.h"
#include "mpi_vb.h"

#define BIS_SRC_WIDTH 	1088
#define BIS_SRC_HEIGHT 	1920

#define SAMPLE_PRT(fmt...)                                                                                             \
	do {                                                                                                           \
		printf("[%s]-%d: ", __func__, __LINE__);                                                               \
		printf(fmt);                                                                                           \
	} while (0)

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
    TS_MPI_SYS_Exit();
    TS_MPI_VB_Exit();
}

static TS_U32 SAMPLE_BIS_TWO()
{
	TS_S32 s32Ret = TS_FAILURE;
    CVE_TASK_ATTR_S pstTask;
    memset(&pstTask, 0, sizeof(CVE_TASK_ATTR_S));
    TS_S32 src_w = BIS_SRC_WIDTH;
    TS_S32 src_h = BIS_SRC_HEIGHT;
    TS_S32 dst_w = BIS_SRC_WIDTH*2;
    TS_S32 dst_h = BIS_SRC_HEIGHT;
	CVE_HANDLE pBisHandle;
    s32Ret = TS_MPI_TRP_CVE_BeginJob(&pBisHandle);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("CVE pBisHandle BeginJob failed!\n");
		return TS_FAILURE;
	}
    // 输入
    pstTask.dmaInputNum = 2;
    pstTask.pstSrc[0].au32Stride[0] = src_w;
    pstTask.pstSrc[0].u32Width = src_w;
    pstTask.pstSrc[0].u32Height = src_h;
    pstTask.pstSrc[0].enType = TS_CVE_IMAGE_TYPE_U8C1;

    pstTask.pstSrc[1].au32Stride[0] = src_w;
    pstTask.pstSrc[1].u32Width = src_w;
    pstTask.pstSrc[1].u32Height = src_h;
    pstTask.pstSrc[1].enType = TS_CVE_IMAGE_TYPE_U8C1;

    // 输出
    pstTask.pstDst[0].au32Stride[0] = dst_w;
    pstTask.pstDst[0].u32Width = dst_w;
    pstTask.pstDst[0].u32Height = dst_h;
    pstTask.pstDst[0].enType = TS_CVE_IMAGE_TYPE_U8C1;
    // 运算模式
    pstTask.pstCascade.cascadeType = TS_CVE_BIS_MODE;
    // 运算参数 TS_BIS_TWO_EYE_MODE TS_BIS_THREE_EYE_MODE TS_BIS_FOUR_EYE_MODE 
    pstTask.pstCascade.operatorCtrl.pstBisCtrl.joint_mode = TS_BIS_TWO_EYE_MODE;
    // 申请VB资源
    TS_S32 insize = src_w*src_h*3/2;
    TS_S32 outsize = dst_w*dst_h*3/2;
    s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstSrc[0].au64PhyAddr[0],
                    (void**)&pstTask.pstSrc[0].au64VirAddr[0], NULL, NULL, insize);
    s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstSrc[1].au64PhyAddr[0],
                    (void**)&pstTask.pstSrc[1].au64VirAddr[0], NULL, NULL, insize);
    s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstDst[0].au64PhyAddr[0],
                    (void**)&pstTask.pstDst[0].au64VirAddr[0], NULL, NULL, outsize);
    // 测试数据赋值
    TS_S8* pSrcData1 = (TS_S8*)pstTask.pstSrc[0].au64VirAddr[0];
    memset(pSrcData1, 0, insize);
    FILE* pfInput1 = fopen("data/two_0_nv12.yuv", "rb");
    if(pfInput1) {
        fread(pSrcData1, 1, insize, pfInput1);
        fclose(pfInput1);
    }
    TS_S8* pSrcData2 = (TS_S8*)pstTask.pstSrc[1].au64VirAddr[0];
    memset(pSrcData2, 0, insize);
    FILE* pfInput2 = fopen("data/two_1_nv12.yuv", "rb");
    if(pfInput2) {
        fread(pSrcData2, 1, insize, pfInput2);
        fclose(pfInput2);
    }
    // 调用流程
    s32Ret = TS_MPI_TRP_CVE_AddTask(pBisHandle, &pstTask);
    CVE_RESULT_S cveResult;
    s32Ret = TS_MPI_TRP_CVE_EndJob(pBisHandle, &cveResult);
    // 验证结果
    TS_U32 num = cveResult.num;
    SAMPLE_PRT("debug, output num %d\n", num);
    const TS_S8* pData = (const TS_S8*)pstTask.pstDst[0].au64VirAddr[0];
    char cname[100] = {0};
    sprintf(cname, "rst_bistwo_%d_%d.bin", cveResult.image[0].u32Width, cveResult.image[0].u32Height);
    outsize = cveResult.image[0].u32Width*cveResult.image[0].u32Height*3/2;
    FILE* pfOutput= fopen(cname, "wb");
    if(pfOutput) {
        fwrite(pData, 1, outsize, pfOutput);
        fclose(pfOutput);
    }

    // 释放资源
    TS_MPI_SYS_MmzFree(pstTask.pstSrc[0].au64PhyAddr[0], (void*)pstTask.pstSrc[0].au64VirAddr[0]);
    TS_MPI_SYS_MmzFree(pstTask.pstSrc[1].au64PhyAddr[0], (void*)pstTask.pstSrc[1].au64VirAddr[0]);
    TS_MPI_SYS_MmzFree(pstTask.pstDst[0].au64PhyAddr[0], (void*)pstTask.pstDst[0].au64VirAddr[0]);
	return TS_SUCCESS;
}

static TS_U32 SAMPLE_BIS_THREE()
{
	TS_S32 s32Ret = TS_FAILURE;
    CVE_TASK_ATTR_S pstTask;
    memset(&pstTask, 0, sizeof(CVE_TASK_ATTR_S));
    TS_S32 src_w = BIS_SRC_WIDTH;
    TS_S32 src_h = BIS_SRC_HEIGHT;
    TS_S32 dst_w = BIS_SRC_WIDTH*3;
    TS_S32 dst_h = BIS_SRC_HEIGHT;

	CVE_HANDLE pBisHandle;
    s32Ret = TS_MPI_TRP_CVE_BeginJob(&pBisHandle);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("CVE pBisHandle BeginJob failed!\n");
		return TS_FAILURE;
	}

    // 输入
    pstTask.dmaInputNum = 3;
    pstTask.pstSrc[0].au32Stride[0] = src_w;
    pstTask.pstSrc[0].u32Width = src_w;
    pstTask.pstSrc[0].u32Height = src_h;
    pstTask.pstSrc[0].enType = TS_CVE_IMAGE_TYPE_U8C1;

    pstTask.pstSrc[1].au32Stride[0] = src_w;
    pstTask.pstSrc[1].u32Width = src_w;
    pstTask.pstSrc[1].u32Height = src_h;
    pstTask.pstSrc[1].enType = TS_CVE_IMAGE_TYPE_U8C1;

    pstTask.pstSrc[2].au32Stride[0] = src_w;
    pstTask.pstSrc[2].u32Width = src_w;
    pstTask.pstSrc[2].u32Height = src_h;
    pstTask.pstSrc[2].enType = TS_CVE_IMAGE_TYPE_U8C1;

    // 输出
    pstTask.pstDst[0].au32Stride[0] = dst_w;
    pstTask.pstDst[0].u32Width = dst_w;
    pstTask.pstDst[0].u32Height = dst_h;
    pstTask.pstDst[0].enType = TS_CVE_IMAGE_TYPE_U8C1;
    // 运算模式
    pstTask.pstCascade.cascadeType = TS_CVE_BIS_MODE;
    // 运算参数 TS_BIS_TWO_EYE_MODE TS_BIS_THREE_EYE_MODE TS_BIS_FOUR_EYE_MODE 
    pstTask.pstCascade.operatorCtrl.pstBisCtrl.joint_mode = TS_BIS_THREE_EYE_MODE;
    // 申请VB资源
    TS_S32 insize = src_w*src_h*3/2;
    TS_S32 outsize = dst_w*dst_h*3/2;
    s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstSrc[0].au64PhyAddr[0],
                    (void**)&pstTask.pstSrc[0].au64VirAddr[0], NULL, NULL, insize);

    s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstSrc[1].au64PhyAddr[0],
                    (void**)&pstTask.pstSrc[1].au64VirAddr[0], NULL, NULL, insize);
    
    s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstSrc[2].au64PhyAddr[0],
                    (void**)&pstTask.pstSrc[2].au64VirAddr[0], NULL, NULL, insize);
    
    s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstDst[0].au64PhyAddr[0],
                    (void**)&pstTask.pstDst[0].au64VirAddr[0], NULL, NULL, outsize);
    
    // 测试数据赋值
    TS_S8* pSrcData1 = (TS_S8*)pstTask.pstSrc[0].au64VirAddr[0];
    memset(pSrcData1, 0, insize);
    FILE* pfInput1 = fopen("data/three_0_nv12.yuv", "rb");
    if(pfInput1) {
        fread(pSrcData1, 1, insize, pfInput1);
        fclose(pfInput1);
    }
    TS_S8* pSrcData2 = (TS_S8*)pstTask.pstSrc[1].au64VirAddr[0];
    memset(pSrcData2, 0, insize);
    FILE* pfInput2 = fopen("data/three_1_nv12.yuv", "rb");
    if(pfInput2) {
        fread(pSrcData2, 1, insize, pfInput2);
        fclose(pfInput2);
    }
    TS_S8* pSrcData3 = (TS_S8*)pstTask.pstSrc[2].au64VirAddr[0];
    memset(pSrcData3, 0, insize);
    FILE* pfInput3 = fopen("data/three_2_nv12.yuv", "rb");
    if(pfInput3) {
        fread(pSrcData3, 1, insize, pfInput3);
        fclose(pfInput3);
    }

    // 调用流程
    s32Ret = TS_MPI_TRP_CVE_AddTask(pBisHandle, &pstTask);
    CVE_RESULT_S cveResult;
    s32Ret = TS_MPI_TRP_CVE_EndJob(pBisHandle, &cveResult);
    // 验证结果
    TS_U32 num = cveResult.num;
    SAMPLE_PRT("debug, output num %d\n", num);
    char cname[100] = {0};
    sprintf(cname, "rst_bisThree_%d_%d.bin", cveResult.image[0].u32Width, cveResult.image[0].u32Height);
    outsize = cveResult.image[0].u32Width*cveResult.image[0].u32Height*3/2;
    const TS_S8* pData = (const TS_S8*)pstTask.pstDst[0].au64VirAddr[0];
    FILE* pfOutput= fopen(cname, "wb");
    if(pfOutput) {
        fwrite(pData, 1, outsize, pfOutput);
        fclose(pfOutput);
    }

    // 释放资源
    TS_MPI_SYS_MmzFree(pstTask.pstSrc[0].au64PhyAddr[0], (void*)pstTask.pstSrc[0].au64VirAddr[0]);
    TS_MPI_SYS_MmzFree(pstTask.pstSrc[1].au64PhyAddr[0], (void*)pstTask.pstSrc[1].au64VirAddr[0]);
    TS_MPI_SYS_MmzFree(pstTask.pstSrc[2].au64PhyAddr[0], (void*)pstTask.pstSrc[2].au64VirAddr[0]);
    TS_MPI_SYS_MmzFree(pstTask.pstDst[0].au64PhyAddr[0], (void*)pstTask.pstDst[0].au64VirAddr[0]);
	return TS_SUCCESS;
}

static TS_U32 SAMPLE_BIS_FOUR()
{
	TS_S32 s32Ret = TS_FAILURE;
    CVE_TASK_ATTR_S pstTask;
    memset(&pstTask, 0, sizeof(CVE_TASK_ATTR_S));
    TS_S32 src_w = BIS_SRC_WIDTH;
    TS_S32 src_h = BIS_SRC_HEIGHT;
    TS_S32 dst_w = BIS_SRC_WIDTH*4;
    TS_S32 dst_h = BIS_SRC_HEIGHT;

	CVE_HANDLE pBisHandle;
    s32Ret = TS_MPI_TRP_CVE_BeginJob(&pBisHandle);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("CVE pBisHandle BeginJob failed!\n");
		return TS_FAILURE;
	}

    // 输入
    pstTask.dmaInputNum = 4;
    pstTask.pstSrc[0].au32Stride[0] = src_w;
    pstTask.pstSrc[0].u32Width = src_w;
    pstTask.pstSrc[0].u32Height = src_h;
    pstTask.pstSrc[0].enType = TS_CVE_IMAGE_TYPE_U8C1;

    pstTask.pstSrc[1].au32Stride[0] = src_w;
    pstTask.pstSrc[1].u32Width = src_w;
    pstTask.pstSrc[1].u32Height = src_h;
    pstTask.pstSrc[1].enType = TS_CVE_IMAGE_TYPE_U8C1;

    pstTask.pstSrc[2].au32Stride[0] = src_w;
    pstTask.pstSrc[2].u32Width = src_w;
    pstTask.pstSrc[2].u32Height = src_h;
    pstTask.pstSrc[2].enType = TS_CVE_IMAGE_TYPE_U8C1;

    pstTask.pstSrc[3].au32Stride[0] = src_w;
    pstTask.pstSrc[3].u32Width = src_w;
    pstTask.pstSrc[3].u32Height = src_h;
    pstTask.pstSrc[3].enType = TS_CVE_IMAGE_TYPE_U8C1;

    // 输出
    pstTask.pstDst[0].au32Stride[0] = dst_w;
    pstTask.pstDst[0].u32Width = dst_w;
    pstTask.pstDst[0].u32Height = dst_h;
    pstTask.pstDst[0].enType = TS_CVE_IMAGE_TYPE_U8C1;
    // 运算模式
    pstTask.pstCascade.cascadeType = TS_CVE_BIS_MODE;
    // 运算参数 TS_BIS_TWO_EYE_MODE TS_BIS_THREE_EYE_MODE TS_BIS_FOUR_EYE_MODE 
    pstTask.pstCascade.operatorCtrl.pstBisCtrl.joint_mode = TS_BIS_FOUR_EYE_MODE;
    // 申请VB资源
    TS_S32 insize = src_w*src_h*3/2;
    TS_S32 outsize = dst_w*dst_h*3/2;
    s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstSrc[0].au64PhyAddr[0],
                    (void**)&pstTask.pstSrc[0].au64VirAddr[0], NULL, NULL, insize);
    
    s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstSrc[1].au64PhyAddr[0],
                    (void**)&pstTask.pstSrc[1].au64VirAddr[0], NULL, NULL, insize);
    
    s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstSrc[2].au64PhyAddr[0],
                    (void**)&pstTask.pstSrc[2].au64VirAddr[0], NULL, NULL, insize);
    
    s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstSrc[3].au64PhyAddr[0],
                    (void**)&pstTask.pstSrc[3].au64VirAddr[0], NULL, NULL, insize);
    
    s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstDst[0].au64PhyAddr[0],
                    (void**)&pstTask.pstDst[0].au64VirAddr[0], NULL, NULL, outsize);
    
    // 测试数据赋值
    TS_S8* pSrcData1 = (TS_S8*)pstTask.pstSrc[0].au64VirAddr[0];
    memset(pSrcData1, 0, insize);
    FILE* pfInput1 = fopen("data/four_0_nv12.yuv", "rb");
    if(pfInput1) {
        fread(pSrcData1, 1, insize, pfInput1);
        fclose(pfInput1);
    }
    TS_S8* pSrcData2 = (TS_S8*)pstTask.pstSrc[1].au64VirAddr[0];
    memset(pSrcData2, 0, insize);
    FILE* pfInput2 = fopen("data/four_1_nv12.yuv", "rb");
    if(pfInput2) {
        fread(pSrcData2, 1, insize, pfInput2);
        fclose(pfInput2);
    }
    TS_S8* pSrcData3 = (TS_S8*)pstTask.pstSrc[2].au64VirAddr[0];
    memset(pSrcData3, 0, insize);
    FILE* pfInput3 = fopen("data/four_2_nv12.yuv", "rb");
    if(pfInput3) {
        fread(pSrcData3, 1, insize, pfInput3);
        fclose(pfInput3);
    }
    TS_S8* pSrcData4 = (TS_S8*)pstTask.pstSrc[3].au64VirAddr[0];
    memset(pSrcData4, 0, insize);
    FILE* pfInput4 = fopen("data/four_3_nv12.yuv", "rb");
    if(pfInput4) {
        fread(pSrcData4, 1, insize, pfInput4);
        fclose(pfInput4);
    }
    // 调用流程
    s32Ret = TS_MPI_TRP_CVE_AddTask(pBisHandle, &pstTask);
    CVE_RESULT_S cveResult;
    s32Ret = TS_MPI_TRP_CVE_EndJob(pBisHandle, &cveResult);
    // 验证结果
    TS_U32 num = cveResult.num;
    SAMPLE_PRT("debug, output num %d\n", num);
    char cname[100] = {0};
    sprintf(cname, "rst_bisFour_%d_%d.bin", cveResult.image[0].u32Width, cveResult.image[0].u32Height);
    outsize = cveResult.image[0].u32Width*cveResult.image[0].u32Height*3/2;
    const TS_S8* pData = (const TS_S8*)pstTask.pstDst[0].au64VirAddr[0];
    FILE* pfOutput= fopen(cname, "wb");
    if(pfOutput) {
        fwrite(pData, 1, outsize, pfOutput);
        fclose(pfOutput);
    }

    // 释放资源
    TS_MPI_SYS_MmzFree(pstTask.pstSrc[0].au64PhyAddr[0], (void*)pstTask.pstSrc[0].au64VirAddr[0]);
    TS_MPI_SYS_MmzFree(pstTask.pstSrc[1].au64PhyAddr[0], (void*)pstTask.pstSrc[1].au64VirAddr[0]);
    TS_MPI_SYS_MmzFree(pstTask.pstSrc[2].au64PhyAddr[0], (void*)pstTask.pstSrc[2].au64VirAddr[0]);
    TS_MPI_SYS_MmzFree(pstTask.pstSrc[3].au64PhyAddr[0], (void*)pstTask.pstSrc[3].au64VirAddr[0]);
    TS_MPI_SYS_MmzFree(pstTask.pstDst[0].au64PhyAddr[0], (void*)pstTask.pstDst[0].au64VirAddr[0]);
	return TS_SUCCESS;
}

void usage()
{
    printf("Usage: \n");
    printf("This program runs sample functions based on an index value passed as a command-line argument.\n");
    printf("You must provide an index value (0, 1, or 2) as an argument.\n");
    printf("Example:\n)");
    printf("  ./program 0   - Run SAMPLE_BIS_TWO()\n");
    printf("  ./program 1   - Run SAMPLE_BIS_THREE()\n");
    printf("  ./program 2   - Run SAMPLE_BIS_FOUR()\n");
}

int main(int argc, char* argv[])
{
	TS_S32 s32Ret;
	// Check if the user has provided an argument
    if (argc != 2) {
        printf("[Error]: Invalid number of arguments.\n");
        usage();
        return 1;
    }
	s32Ret = SAMPLE_SYS_Init();
	if(s32Ret != TS_SUCCESS) {
		SAMPLE_PRT("SAMPLE_SYS_Init failed\n");
		return TS_FAILURE;
	}
	int index = atoi(argv[1]);
	if(index == 0) {
		SAMPLE_BIS_TWO();
	} else if (index == 1) {
		SAMPLE_BIS_THREE();
	} else if (index ==2 ) {
		SAMPLE_BIS_FOUR();
	} else {
		printf("[Error]: Invalid index. Please provide a value between 0 and 2.\n");
	}
	SAMPLE_SYS_Exit();
}