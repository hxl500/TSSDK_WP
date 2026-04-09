
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <math.h>
#include "mpi_sys.h"
#include "ts_cve.h"
#include "mpi_cve.h"
#include "mpi_vb.h"

#include "ts_comm_cve.h"
#include "ts_comm_vb.h"

#define SAMPLE_PRT(fmt...)	\
	do {	\
		printf("[%s]-%d: ", __func__, __LINE__);	\
		printf(fmt);	\
	} while (0)

TS_S32 SAMPLE_SYS_Init()
{
	TS_S32 s32Ret = TS_FAILURE;

	TS_MPI_SYS_Exit();
	TS_MPI_VB_Exit();

	VB_CONFIG_S stVbConf;
	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
	stVbConf.u32MaxPoolCnt = 1;
	TS_S32 blk_size = 100 * 100;
	stVbConf.astCommPool[0].u64BlkSize = blk_size;
	stVbConf.astCommPool[0].u32BlkCnt = 2;

	s32Ret = TS_MPI_VB_SetConfig(&stVbConf);

	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_VB_SetConf failed!\n");
		return TS_FAILURE;
	}

	s32Ret = TS_MPI_VB_Init();

	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_VB_Init failed!\n");
		return TS_FAILURE;
	}

	s32Ret = TS_MPI_SYS_Init();
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_SYS_Init failed!\n");
		TS_MPI_VB_Exit();
		return TS_FAILURE;
	}

	return s32Ret;
}

TS_VOID SAMPLE_SYS_Exit()
{
	TS_MPI_SYS_Exit();
	TS_MPI_VB_Exit();
}

static TS_U16 gMap[256] = {
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

static short gTestFeat[384] = {
	27573,	29796,	4624,	-24799, -31422, -9156,	21528,	32419,	13504,	-17826, -32768, -17582, 13768,	32460,
	21309,	-9434,	-31503, -24608, 4911,	29915,	27416,	-290,	-27729, -29674, -4337,	24988,	31339,	8877,
	-21746, -32376, -13240, 18069,	32765,	17337,	-14031, -32499, -21087, 9711,	31582,	24416,	-5198,	-30033,
	-27256, 580,	27882,	29550,	4049,	-25174, -31253, -8597,	21962,	32330,	12974,	-18310, -32760, -17090,
	14292,	32534,	20865,	-9988,	-31658, -24221, 5484,	30147,	27094,	-870,	-28034, -29423, -3761,	25359,
	31164,	8317,	-22176, -32281, -12707, 18550,	32752,	16842,	-14553, -32568, -20640, 10264,	31731,	24025,
	-5770,	-30260, -26929, 1160,	28183,	29294,	3473,	-25542, -31073, -8036,	22389,	32230,	12439,	-18789,
	-32742, -16593, 14812,	32598,	20414,	-10539, -31803, -23827, 6055,	30370,	26763,	-1450,	-28330, -29163,
	-3184,	25722,	30980,	7755,	-22600, -32177, -12170, 19025,	32729,	16342,	-15070, -32627, -20186, 10813,
	31871,	23627,	-6340,	-30478, -26595, 1739,	28474,	29030,	2896,	-25901, -30884, -7473,	22809,	32120,
	11900,	-19261, -32714, -16090, 15327,	32652,	19957,	-11087, -31937, -23425, 6624,	30583,	26424,	-2029,
	-28617, -28894, -2607,	26078,	30786,	7190,	-23016, -32062, -11630, 19495,	32696,	15836,	-15583, -32675,
	-19726, 11359,	32001,	23221,	-6908,	-30686, -26252, 2318,	28757,	28756,	2317,	-26252, -30686, -6907,
	23222,	32001,	11358,	-19727, -32675, -15582, 15837,	32696,	19494,	-11631, -32062, -23016, 7191,	30787,
	26077,	-2608,	-28895, -28616, -2028,	26425,	30583,	6623,	-23426, -31937, -11086, 19958,	32652,	15326,
	-16091, -32714, -19260, 11901,	32121,	22808,	-7474,	-30885, -25900, 2897,	29030,	28474,	1738,	-26595,
	-30477, -6339,	23628,	31871,	10812,	-20187, -32627, -15069, 16343,	32729,	19025,	-12171, -32177, -22599,
	7756,	30980,	25722,	-3185,	-29164, -28329, -1449,	26764,	30370,	6054,	-23828, -31802, -10538, 20415,
	32598,	14811,	-16593, -32742, -18788, 12440,	32230,	22388,	-8037,	-31074, -25541, 3474,	29295,	28182,
	1159,	-26930, -30260, -5769,	24026,	31731,	10263,	-20641, -32568, -14552, 16843,	32752,	18549,	-12708,
	-32281, -22176, 8318,	31164,	25358,	-3762,	-29424, -28033, -869,	27094,	30147,	5483,	-24222, -31657,
	-9987,	20865,	32534,	14291,	-17091, -32760, -18310, 12975,	32330,	21961,	-8598,	-31253, -25174, 4050,
	29550,	27882,	579,	-27256, -30032, -5197,	24417,	31581,	9710,	-21088, -32498, -14030, 17338,	32765,
	18068,	-13240, -32376, -21745, 8878,	31339,	24987,	-4338,	-29674, -27728, -289,	27416,	29915,	4910,
	-24609, -31503, -9433,	21309,	32460,	13767,	-17583, -32768, -17826, 13505,	32419,	21527,	-9157,	-31422,
	-24798, 4625,	29796,	27573,	-1,	-27574, -29795, -4623,	24800,	31422,	9155,	-21529, -32419, -13503,
	17827,	32767,	17582,	-13769, -32460, -21308, 9435,	31503,	24608,	-4912,	-29916, -27415, 291,	27729,
	29674,	4336,	-24988, -31338, -8876,	21747,
};
static short gTestPool[4][384] = {
	{
		// pool1
		0,
	},
	{ // pool1
	  27573, 29796, 4624,  -24799, -31422, 21679, 23343, 474,   14486, 9753,  20495, 11121, 27084, 10229, 23382,
	  2242,	 533,	25255, 22074,  4035,   25255, 3244,  14683, 22452, 25928, 8729,	 1766,	26044, 8252,  7369,
	  12859, 489,	6540,  12312,  19614,  24538, 5332,  12617, 1560,  24327, 20136, 32538, 26278, 28171, 22330,
	  6176,	 14413, 14205, 22182,  23012,  16603, 27344, 5825,  9003,  20679, 13178, 22237, 30006, 14641, 24009,
	  9429,	 24604, 13497, 772,    7694,   1940,  12742, 14957, 6789,  17691, 11922, 4958,	16544, 5445,  22626,
	  24229, 7191,	23709, 30780,  7308,   26841, 26980, 24487, 26552, 30640, 2684,	 30051, 30333, 23626, 4623,
	  10809, 21211, 3113,  20946,  20171,  9828,  1483,  7987,  1743,  7638,  22199, 10223, 1100,  24782, 4995,
	  30275, 11540, 24330, 31871,  17829,  11307, 26508, 11122, 29481, 25700, 6806,	 2533,	14113, 24379, 5586,
	  24712, 12739, 1653,  14609,  269,    7470,  14328, 26229, 24470, 5426,  22387, 6367,	28224, 2245,  31813,
	  8421,	 28733, 2702,  7638,   16464,  31198, 17000, 23473, 21108, 16794, 3486,	 11323, 23839, 20947, 27921,
	  30106, 14823, 12808, 7390,   30181,  21122, 12033, 28998, 20648, 26393, 24420, 15188, 10769, 17122, 14497,
	  28618, 239,	30644, 6205,   29578,  15215, 29212, 3784,  18799, 16082, 18451, 32599, 29562, 17641, 3247,
	  13961, 5629,	28395, 30558,  25057,  14362, 27481, 16451, 23137, 1104,  13304, 17510, 9055,  6870,  15814,
	  8254,	 21498, 23975, 4143,   17003,  27150, 13871, 3946,  24403, 18643, 30259, 24372, 23916, 1463,  26296,
	  19118, 21950, 12129, 12825,  4891,   32031, 2600,  8958,  5759,  16348, 4495,	 24108, 8960,  7129,  14009,
	  8253,	 12753, 21141, 17187,  26367,  25735, 30140, 9482,  18307, 10403, 18476, 26923, 30297, 5150,  4591,
	  18880, 23997, 17829, 22393,  3389,   17149, 20961, 5487,  21541, 6228,  2329,	 26146, 9926,  5956,  29655,
	  4891,	 24074, 30977, 20335,  18145,  10722, 13759, 15394, 791,   8600,  11145, 17292, 9082,  2838,  15583,
	  18333, 17413, 10323, 10101,  16627,  26209, 30816, 5809,  31021, 6268,  16323, 16062, 29540, 26615, 23549,
	  18902, 13347, 14870, 14699,  23723,  5421,  26272, 25489, 22245, 14088, 12111, 29803, 20422, 11731, 13017,
	  25256, 22100, 17032, 20021,  10326,  9448,  26412, 13618, 25535, 14113, 26348, 3178,	12202, 8985,  27518,
	  19968, 31216, 21231, 3563,   20579,  19043, 4325,  23697, 15733, 14890, 14266, 1974,	5679,  17218, 30698,
	  13221, 18616, 6141,  19394,  26802,  13978, 7123,  31238, 14479, 5609,  19043, 21235, 10830, 22731, 14172,
	  17802, 20439, 15355, 19133,  3359,   14749, 8383,  4916,  10236, 10101, 29497, 10028, 23063, 4993,  2600,
	  21185, 24118, 9643,  26284,  21622,  15695, 19369, 19426, 21120, 16808, 26820, 595,	31887, 16580, 2995,
	  10693, 23895, 26019, 21944,  13708,  1640,  28645, 14306, 2978 },
	{
		// pool1
		0,
	},
	{ // pool2
	  27573, 29796, 4624,  -24799, -31422, 21679, 23343, 474,   14486, 9753,  20495, 11121, 27084, 10229, 23382,
	  2242,	 533,	25255, 22074,  4035,   25255, 3244,  14683, 22452, 25928, 8729,	 1766,	26044, 8252,  7369,
	  12859, 489,	6540,  12312,  19614,  24538, 5332,  12617, 1560,  24327, 20136, 32538, 26278, 28171, 22330,
	  6176,	 14413, 14205, 22182,  23012,  16603, 27344, 5825,  9003,  20679, 13178, 22237, 30006, 14641, 24009,
	  9429,	 24604, 13497, 772,    7694,   1940,  12742, 14957, 6789,  17691, 11922, 4958,	16544, 5445,  22626,
	  24229, 7191,	23709, 30780,  7308,   26841, 26980, 24487, 26552, 30640, 2684,	 30051, 30333, 23626, 4623,
	  10809, 21211, 3113,  20946,  20171,  9828,  1483,  7987,  1743,  7638,  22199, 10223, 1100,  24782, 4995,
	  30275, 11540, 24330, 31871,  17829,  11307, 26508, 11122, 29481, 25700, 6806,	 2533,	14113, 24379, 5586,
	  24712, 12739, 1653,  14609,  269,    7470,  14328, 26229, 24470, 5426,  22387, 6367,	28224, 2245,  31813,
	  8421,	 28733, 2702,  7638,   16464,  31198, 17000, 23473, 21108, 16794, 3486,	 11323, 23839, 20947, 27921,
	  30106, 14823, 12808, 7390,   30181,  21122, 12033, 28998, 20648, 26393, 24420, 15188, 10769, 17122, 14497,
	  28618, 239,	30644, 6205,   29578,  15215, 29212, 3784,  18799, 16082, 18451, 32599, 29562, 17641, 3247,
	  13961, 5629,	28395, 30558,  25057,  14362, 27481, 16451, 23137, 1104,  13304, 17510, 9055,  6870,  15814,
	  8254,	 21498, 23975, 4143,   17003,  27150, 13871, 3946,  24403, 18643, 30259, 24372, 23916, 1463,  26296,
	  19118, 21950, 12129, 12825,  4891,   32031, 2600,  8958,  5759,  16348, 4495,	 24108, 8960,  7129,  14009,
	  8253,	 12753, 21141, 17187,  26367,  25735, 30140, 9482,  18307, 10403, 18476, 26923, 30297, 5150,  4591,
	  18880, 23997, 17829, 22393,  3389,   17149, 20961, 5487,  21541, 6228,  2329,	 26146, 9926,  5956,  29655,
	  4891,	 24074, 30977, 20335,  18145,  10722, 13759, 15394, 791,   8600,  11145, 17292, 9082,  2838,  15583,
	  18333, 17413, 10323, 10101,  16627,  26209, 30816, 5809,  31021, 6268,  16323, 16062, 29540, 26615, 23549,
	  18902, 13347, 14870, 14699,  23723,  5421,  26272, 25489, 22245, 14088, 12111, 29803, 20422, 11731, 13017,
	  25256, 22100, 17032, 20021,  10326,  9448,  26412, 13618, 25535, 14113, 26348, 3178,	12202, 8985,  27518,
	  19968, 31216, 21231, 3563,   20579,  19043, 4325,  23697, 15733, 14890, 14266, 1974,	5679,  17218, 30698,
	  13221, 18616, 6141,  19394,  26802,  13978, 7123,  31238, 14479, 5609,  19043, 21235, 10830, 22731, 14172,
	  17802, 20439, 15355, 19133,  3359,   14749, 8383,  4916,  10236, 10101, 29497, 10028, 23063, 4993,  2600,
	  21185, 24118, 9643,  26284,  21622,  15695, 19369, 19426, 21120, 16808, 26820, 595,	31887, 16580, 2995,
	  10693, 23895, 26019, 21944,  13708,  1640,  28645, 14306, 2978 },
};

static TS_S32 SAMPLE_CVE_16To8(void)
{
	TS_S32 s32Ret = TS_SUCCESS;

	CVE_HANDLE pHandle;
	CVE_TASK_ATTR_S pstTask;
	TS_S32 src_w = 640;
	TS_S32 src_h = 480;
	double alpha = 0.32;

	memset(&pstTask, 0, sizeof(CVE_TASK_ATTR_S));

	pstTask.dmaInputNum = 1;
	pstTask.pstSrc[0].au32Stride[0] = src_w * 2;
	pstTask.pstSrc[0].u32Width = src_w;
	pstTask.pstSrc[0].u32Height = src_h;
	pstTask.pstSrc[0].enType = TS_CVE_IMAGE_TYPE_S16C1;

	pstTask.dmaOutputNum = 1;
	pstTask.pstDst[0].au32Stride[0] = src_w;
	pstTask.pstDst[0].u32Width = src_w;
	pstTask.pstDst[0].u32Height = src_h;
	pstTask.pstDst[0].enType = TS_CVE_IMAGE_TYPE_U8C1;

	pstTask.pstCascade.cascadeType = TS_CVE_16TO8_MODE;

	pstTask.pstCascade.operatorCtrl.pst16to8Ctrl.enMode = TS_CVE_16BIT_TO_8BIT_MODE_S16_TO_U8_BIAS;
	pstTask.pstCascade.operatorCtrl.pst16to8Ctrl.bias = 0x02;
	pstTask.pstCascade.operatorCtrl.pst16to8Ctrl.parameterK = (TS_U16)(alpha * 65536);

	TS_S32 insize = src_w * src_h * 2;
	s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstSrc[0].au64PhyAddr[0], (void **)&pstTask.pstSrc[0].au64VirAddr[0],
				     NULL, NULL, insize);
	s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstDst[0].au64PhyAddr[0], (void **)&pstTask.pstDst[0].au64VirAddr[0],
				     NULL, NULL, insize);
	// 测试数据
	TS_S16 *pSrcData = (TS_S16 *)(uintptr_t)pstTask.pstSrc[0].au64VirAddr[0];
	for (int i = 0; i < src_w * src_h; i++) {
		pSrcData[i] = 10;
	}

	s32Ret = TS_MPI_TRP_CVE_BeginJob(&pHandle);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve begin job error\n");
		goto ReleaseVB;
	}

	s32Ret = TS_MPI_TRP_CVE_AddTask(pHandle, &pstTask);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve add task error\n");
		goto End;
	}

	CVE_RESULT_S cveResult;
	s32Ret = TS_MPI_TRP_CVE_EndJob(pHandle, &cveResult);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve end job error\n");
		goto End;
	}

	TS_U32 num = cveResult.num;

	SAMPLE_PRT("debug, output num:%d\n", num);
	TS_BOOL bRet = TS_TRUE;
	const TS_U8 *pData = (const TS_U8 *)(uintptr_t)pstTask.pstDst[0].au64VirAddr[0];
	for (int i = 0; i < src_w * src_h; i++) {
		if (5 != pData[i]) {
			bRet = TS_FALSE;
			break;
		}
	}
	if (!bRet) {
		SAMPLE_PRT("[ERROR] the result is error\n");
	} else {
		SAMPLE_PRT("[OK] the result is OK\n");
	}
End:
	s32Ret = TS_MPI_TRP_CVE_CancelJob(pHandle);
	if (TS_SUCCESS != s32Ret)
		SAMPLE_PRT("cve end job failed\n");
ReleaseVB:
	// 释放内存
	TS_MPI_SYS_MmzFree(pstTask.pstSrc[0].au64PhyAddr[0], (void *)(uintptr_t)pstTask.pstSrc[0].au64VirAddr[0]);
	TS_MPI_SYS_MmzFree(pstTask.pstDst[0].au64PhyAddr[0], (void *)(uintptr_t)pstTask.pstDst[0].au64VirAddr[0]);
	return s32Ret;
}

static TS_S32 SAMPLE_CVE_Thresh(void)
{
	TS_S32 s32Ret = TS_SUCCESS;

	CVE_HANDLE pHandle;
	CVE_TASK_ATTR_S pstTask;
	TS_S32 src_w = 640;
	TS_S32 src_h = 480;
	memset(&pstTask, 0, sizeof(CVE_TASK_ATTR_S));

	pstTask.dmaInputNum = 1;
	pstTask.pstSrc[0].au32Stride[0] = src_w;
	pstTask.pstSrc[0].u32Width = src_w;
	pstTask.pstSrc[0].u32Height = src_h;
	pstTask.pstSrc[0].enType = TS_CVE_IMAGE_TYPE_U8C1;

	pstTask.dmaOutputNum = 1;
	pstTask.pstDst[0].au32Stride[0] = src_w;
	pstTask.pstDst[0].u32Width = src_w;
	pstTask.pstDst[0].u32Height = src_h;
	pstTask.pstDst[0].enType = TS_CVE_IMAGE_TYPE_U8C1;

	pstTask.pstCascade.cascadeType = TS_CVE_THRESH_MODE;

	pstTask.pstCascade.operatorCtrl.pstThreshCtrl.enMode = TS_CVE_THRESH_MODE_U8_EN;
	pstTask.pstCascade.operatorCtrl.pstThreshCtrl.pstThrU8Ctrl.enMode = TS_CVE_THRESH_MODE_BINARY;
	pstTask.pstCascade.operatorCtrl.pstThreshCtrl.pstThrU8Ctrl.u8LowThr = 100;
	pstTask.pstCascade.operatorCtrl.pstThreshCtrl.pstThrU8Ctrl.u8HighThr = 0;
	pstTask.pstCascade.operatorCtrl.pstThreshCtrl.pstThrU8Ctrl.u8MinVal = 0;
	pstTask.pstCascade.operatorCtrl.pstThreshCtrl.pstThrU8Ctrl.u8MidVal = 0;
	pstTask.pstCascade.operatorCtrl.pstThreshCtrl.pstThrU8Ctrl.u8MaxVal = 200;

	TS_S32 insize = src_w * src_h;
	s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstSrc[0].au64PhyAddr[0], (void **)&pstTask.pstSrc[0].au64VirAddr[0],
				     NULL, NULL, insize);
	s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstDst[0].au64PhyAddr[0], (void **)&pstTask.pstDst[0].au64VirAddr[0],
				     NULL, NULL, insize);
	// 测试数据
	char *pSrcData = (char *)(uintptr_t)pstTask.pstSrc[0].au64VirAddr[0];
	memset(pSrcData, 0, insize);
	FILE *pfInput = fopen("data/480p_gray_0.raw", "rb");
	if (pfInput) {
		fread(pSrcData, 1, insize, pfInput);
		fclose(pfInput);
	}

	s32Ret = TS_MPI_TRP_CVE_BeginJob(&pHandle);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve begin job error\n");
		goto ReleaseVB;
	}

	s32Ret = TS_MPI_TRP_CVE_AddTask(pHandle, &pstTask);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve add task error\n");
		goto End;
	}

	CVE_RESULT_S cveResult;

	s32Ret = TS_MPI_TRP_CVE_EndJob(pHandle, &cveResult);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve end job error\n");
		goto End;
	}

	TS_U32 num = cveResult.num;

	SAMPLE_PRT("debug, output num:%d\n", num);
	TS_BOOL bRet = TS_TRUE;
	const TS_U8 *pData = (const TS_U8 *)(uintptr_t)pstTask.pstDst[0].au64VirAddr[0];
	for (int i = 0; i < insize; i++) {
		if (pData[i] != 0 && pData[i] != 200) {
			SAMPLE_PRT("[ERROR] cve Threshold error val = %d \n", (int)pData[i]);
			bRet = TS_FALSE;
			break;
		}
	}
	if (!bRet) {
		SAMPLE_PRT("[ERROR] the result is error\n");
	} else {
		SAMPLE_PRT("[OK] the result is ok\n");
	}
End:
	s32Ret = TS_MPI_TRP_CVE_CancelJob(pHandle);
	if (TS_SUCCESS != s32Ret)
		SAMPLE_PRT("cve end job failed\n");
ReleaseVB:
	TS_MPI_SYS_MmzFree(pstTask.pstSrc[0].au64PhyAddr[0], (void *)(uintptr_t)pstTask.pstSrc[0].au64VirAddr[0]);
	TS_MPI_SYS_MmzFree(pstTask.pstDst[0].au64PhyAddr[0], (void *)(uintptr_t)pstTask.pstDst[0].au64VirAddr[0]);

	return s32Ret;
}

static TS_S32 SAMPLE_CVE_Map(void)
{
	TS_S32 s32Ret = TS_SUCCESS;
	CVE_HANDLE pHandle;
	CVE_TASK_ATTR_S pstTask;
	TS_S32 src_w = 640;
	TS_S32 src_h = 480;
	memset(&pstTask, 0, sizeof(CVE_TASK_ATTR_S));
	// 输入数据1
	pstTask.dmaInputNum = 1;
	pstTask.pstSrc[0].au32Stride[0] = src_w;
	pstTask.pstSrc[0].u32Width = src_w;
	pstTask.pstSrc[0].u32Height = src_h;
	pstTask.pstSrc[0].enType = TS_CVE_IMAGE_TYPE_U8C1;
	// 输出数据
	pstTask.dmaOutputNum = 1;
	pstTask.pstDst[0].au32Stride[0] = src_w;
	pstTask.pstDst[0].u32Width = src_w;
	pstTask.pstDst[0].u32Height = src_h;
	pstTask.pstDst[0].enType = TS_CVE_IMAGE_TYPE_U8C1;
	// 运算模式
	pstTask.pstCascade.cascadeType = TS_CVE_MAP_MODE;
	// 运算参数
	pstTask.pstCascade.operatorCtrl.pstMapCtrl.enMode = TS_CVE_MAP_MODE_U8;
	pstTask.pstCascade.operatorCtrl.pstMapCtrl.mapRamAddr = 0;
	pstTask.pstCascade.operatorCtrl.pstMapCtrl.mapSize = 256;
	memcpy(pstTask.pstCascade.operatorCtrl.pstMapCtrl.map, gMap, 256 * sizeof(TS_U16));

	// 申请vb内存
	TS_S32 insize = src_w * src_h;
	s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstSrc[0].au64PhyAddr[0], (void **)&pstTask.pstSrc[0].au64VirAddr[0],
				     NULL, NULL, insize);

	s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstDst[0].au64PhyAddr[0], (void **)&pstTask.pstDst[0].au64VirAddr[0],
				     NULL, NULL, insize);

	// 测试数据
	int nval_1 = 0;
	char *pSrcData = (char *)(uintptr_t)pstTask.pstSrc[0].au64VirAddr[0];
	memset(pSrcData, nval_1, insize);
	// 调用流程
	s32Ret = TS_MPI_TRP_CVE_BeginJob(&pHandle);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve begin job error\n");
		goto ReleaseVB;
	}

	s32Ret = TS_MPI_TRP_CVE_AddTask(pHandle, &pstTask);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve add task error\n");
		goto End;
	}

	CVE_RESULT_S cveResult;
	s32Ret = TS_MPI_TRP_CVE_EndJob(pHandle, &cveResult);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve end job error\n");
		goto End;
	}
	// 打印结果
	TS_U32 num = cveResult.num;
	SAMPLE_PRT("debug, output num:%d\n", num);
	TS_BOOL bRet = TS_TRUE;
	const TS_U8 *pData = (const TS_U8 *)(uintptr_t)pstTask.pstDst[0].au64VirAddr[0];
	for (int i = 0; i < src_w * src_h; i++) {
		if (pData[i] != gMap[nval_1]) {
			bRet = TS_FALSE;
		}
	}
	if (!bRet) {
		SAMPLE_PRT("[ERROR] the result is error\n");
	} else {
		SAMPLE_PRT("[OK] the result is ok\n");
	}

End:
	s32Ret = TS_MPI_TRP_CVE_CancelJob(pHandle);
	if (TS_SUCCESS != s32Ret)
		SAMPLE_PRT("cve end job failed\n");
ReleaseVB:
	// 释放vb内存
	TS_MPI_SYS_MmzFree(pstTask.pstSrc[0].au64PhyAddr[0], (void *)(uintptr_t)pstTask.pstSrc[0].au64VirAddr[0]);
	TS_MPI_SYS_MmzFree(pstTask.pstDst[0].au64PhyAddr[0], (void *)(uintptr_t)pstTask.pstDst[0].au64VirAddr[0]);

	return s32Ret;
}

static TS_S32 SAMPLE_CVE_Add(void)
{
	TS_S32 s32Ret = TS_SUCCESS;
	CVE_HANDLE pHandle;
	CVE_TASK_ATTR_S pstTask;
	TS_S32 src_w = 640;
	TS_S32 src_h = 480;
	TS_U32 nExpectedValue = 59;
	memset(&pstTask, 0, sizeof(CVE_TASK_ATTR_S));
	// 输入数据
	pstTask.dmaInputNum = 2;
	pstTask.dmaOutputNum = 1;
	pstTask.pstSrc[0].au32Stride[0] = src_w;
	pstTask.pstSrc[0].u32Width = src_w;
	pstTask.pstSrc[0].u32Height = src_h;
	pstTask.pstSrc[0].enType = TS_CVE_IMAGE_TYPE_U8C1;

	pstTask.pstSrc[1].au32Stride[0] = src_w;
	pstTask.pstSrc[1].u32Width = src_w;
	pstTask.pstSrc[1].u32Height = src_h;
	pstTask.pstSrc[1].enType = TS_CVE_IMAGE_TYPE_U8C1;
	// 输出数据
	pstTask.pstDst[0].au32Stride[0] = src_w;
	pstTask.pstDst[0].u32Width = src_w;
	pstTask.pstDst[0].u32Height = src_h;
	pstTask.pstDst[0].enType = TS_CVE_IMAGE_TYPE_U8C1;
	// 运算模式
	pstTask.pstCascade.cascadeType = TS_CVE_ADD_SUB_MODE;
	// 参数配置
	pstTask.pstCascade.operatorCtrl.pstAddSubCtrl.subaddMode = TS_CVE_MODE_ADD;
	pstTask.pstCascade.operatorCtrl.pstAddSubCtrl.emode = TS_CVE_SUB_MODE_SHIFT;
	pstTask.pstCascade.operatorCtrl.pstAddSubCtrl.addCoefa = 0xFFFF;
	pstTask.pstCascade.operatorCtrl.pstAddSubCtrl.addCoefb = 0xFFFF;

	TS_S32 insize = src_w * src_h;
	s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstSrc[0].au64PhyAddr[0], (void **)&pstTask.pstSrc[0].au64VirAddr[0],
				     NULL, NULL, insize);
	s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstSrc[1].au64PhyAddr[0], (void **)&pstTask.pstSrc[1].au64VirAddr[0],
				     NULL, NULL, insize);
	s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstDst[0].au64PhyAddr[0], (void **)&pstTask.pstDst[0].au64VirAddr[0],
				     NULL, NULL, insize);
	int nval_1 = 10;
	char *pSrcData = (char *)(uintptr_t)pstTask.pstSrc[0].au64VirAddr[0];
	memset(pSrcData, nval_1, insize);
	int nval_2 = 50;
	char *pSrcData2 = (char *)(uintptr_t)pstTask.pstSrc[1].au64VirAddr[0];
	memset(pSrcData2, nval_2, insize);

	s32Ret = TS_MPI_TRP_CVE_BeginJob(&pHandle);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve begin job error\n");
		goto ReleaseVB;
	}

	s32Ret = TS_MPI_TRP_CVE_AddTask(pHandle, &pstTask);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve add task error\n");
		goto End;
	}

	CVE_RESULT_S cveResult;

	s32Ret = TS_MPI_TRP_CVE_EndJob(pHandle, &cveResult);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve end job error\n");
		goto End;
	}

	TS_U32 num = cveResult.num;

	SAMPLE_PRT("debug, output num:%d\n", num);
	TS_BOOL bRet = TS_TRUE;
	const TS_U8 *pData = (const TS_U8 *)(uintptr_t)pstTask.pstDst[0].au64VirAddr[0];
	for (int i = 0; i < 10; i++) {
		if (pData[i] != nExpectedValue) {
			bRet = TS_FALSE;
		}
	}
	if (!bRet) {
		SAMPLE_PRT("[ERROR] the result is error\n");
	} else {
		SAMPLE_PRT("[OK] the result is ok\n");
	}

End:
	s32Ret = TS_MPI_TRP_CVE_CancelJob(pHandle);
	if (TS_SUCCESS != s32Ret)
		SAMPLE_PRT("cve end job failed\n");

ReleaseVB:
	TS_MPI_SYS_MmzFree(pstTask.pstSrc[0].au64PhyAddr[0], (void *)(uintptr_t)pstTask.pstSrc[0].au64VirAddr[0]);
	TS_MPI_SYS_MmzFree(pstTask.pstSrc[1].au64PhyAddr[0], (void *)(uintptr_t)pstTask.pstSrc[1].au64VirAddr[0]);
	TS_MPI_SYS_MmzFree(pstTask.pstDst[0].au64PhyAddr[0], (void *)(uintptr_t)pstTask.pstDst[0].au64VirAddr[0]);

	return s32Ret;
}

static TS_S32 SAMPLE_CVE_Xor(void)
{
	TS_S32 s32Ret = TS_SUCCESS;
	CVE_HANDLE pHandle;
	CVE_TASK_ATTR_S pstTask;
	TS_S32 src_w = 640;
	TS_S32 src_h = 480;
	TS_U32 nExpectedValue = 17;
	memset(&pstTask, 0, sizeof(CVE_TASK_ATTR_S));
	// 输入参数
	pstTask.dmaInputNum = 2;
	pstTask.pstSrc[0].au32Stride[0] = src_w;
	pstTask.pstSrc[0].u32Width = src_w;
	pstTask.pstSrc[0].u32Height = src_h;
	pstTask.pstSrc[0].enType = TS_CVE_IMAGE_TYPE_U8C1;

	pstTask.pstSrc[1].au32Stride[0] = src_w;
	pstTask.pstSrc[1].u32Width = src_w;
	pstTask.pstSrc[1].u32Height = src_h;
	pstTask.pstSrc[1].enType = TS_CVE_IMAGE_TYPE_U8C1;
	// 输出参数
	pstTask.dmaOutputNum = 1;
	pstTask.pstDst[0].au32Stride[0] = src_w;
	pstTask.pstDst[0].u32Width = src_w;
	pstTask.pstDst[0].u32Height = src_h;
	pstTask.pstDst[0].enType = TS_CVE_IMAGE_TYPE_U8C1;
	// 运算模式
	pstTask.pstCascade.cascadeType = TS_CVE_AND_OR_XOR_MODE;
	// 运算参数
	pstTask.pstCascade.operatorCtrl.pstAndOrXorCtrl = TS_CVE_AND_MODE;

	TS_S32 insize = src_w * src_h;
	s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstSrc[0].au64PhyAddr[0], (void **)&pstTask.pstSrc[0].au64VirAddr[0],
				     NULL, NULL, insize);
	s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstSrc[1].au64PhyAddr[0], (void **)&pstTask.pstSrc[1].au64VirAddr[0],
				     NULL, NULL, insize);
	s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstDst[0].au64PhyAddr[0], (void **)&pstTask.pstDst[0].au64VirAddr[0],
				     NULL, NULL, insize);
	int nval_1 = 23;
	char *pSrcData = (char *)(uintptr_t)pstTask.pstSrc[0].au64VirAddr[0];
	memset(pSrcData, nval_1, insize);
	int nval_2 = 185;
	char *pSrcData2 = (char *)(uintptr_t)pstTask.pstSrc[1].au64VirAddr[0];
	memset(pSrcData2, nval_2, insize);

	s32Ret = TS_MPI_TRP_CVE_BeginJob(&pHandle);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve begin job error\n");
		goto ReleaseVB;
	}

	s32Ret = TS_MPI_TRP_CVE_AddTask(pHandle, &pstTask);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve add task error\n");
		goto End;
	}

	CVE_RESULT_S cveResult;

	s32Ret = TS_MPI_TRP_CVE_EndJob(pHandle, &cveResult);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve end job error\n");
		goto End;
	}

	TS_U32 num = cveResult.num;

	SAMPLE_PRT("debug, output num:%d\n", num);
	TS_BOOL bRet = TS_TRUE;
	const TS_U8 *pData = (const TS_U8 *)(uintptr_t)pstTask.pstDst[0].au64VirAddr[0];
	for (int i = 0; i < 10; i++) {
		if (pData[i] != nExpectedValue) {
			bRet = TS_FALSE;
		}
	}
	if (!bRet) {
		SAMPLE_PRT("[ERROR] the result is error\n");
	} else {
		SAMPLE_PRT("[OK] the result is ok\n");
	}

End:
	s32Ret = TS_MPI_TRP_CVE_CancelJob(pHandle);
	if (TS_SUCCESS != s32Ret)
		SAMPLE_PRT("cve end job failed\n");

ReleaseVB:
	TS_MPI_SYS_MmzFree(pstTask.pstSrc[0].au64PhyAddr[0], (void *)(uintptr_t)pstTask.pstSrc[0].au64VirAddr[0]);
	TS_MPI_SYS_MmzFree(pstTask.pstSrc[1].au64PhyAddr[0], (void *)(uintptr_t)pstTask.pstSrc[1].au64VirAddr[0]);
	TS_MPI_SYS_MmzFree(pstTask.pstDst[0].au64PhyAddr[0], (void *)(uintptr_t)pstTask.pstDst[0].au64VirAddr[0]);

	return s32Ret;
}

static TS_S32 SAMPLE_CVE_Hist(void)
{
	TS_S32 s32Ret = TS_SUCCESS;
	CVE_HANDLE pHandle;
	CVE_TASK_ATTR_S pstTask;
	TS_S32 src_w = 640;
	TS_S32 src_h = 480;
	TS_U32 nExpectedValue = src_h * src_w;
	memset(&pstTask, 0, sizeof(CVE_TASK_ATTR_S));

	pstTask.dmaInputNum = 1;
	pstTask.pstSrc[0].au32Stride[0] = src_w;
	pstTask.pstSrc[0].u32Width = src_w;
	pstTask.pstSrc[0].u32Height = src_h;
	pstTask.pstSrc[0].enType = TS_CVE_IMAGE_TYPE_U8C1;

	pstTask.dmaOutputNum = 1;
	pstTask.pstDst[0].au32Stride[0] = 1;
	pstTask.pstDst[0].u32Width = 1;
	pstTask.pstDst[0].u32Height = 256;
	pstTask.pstDst[0].enType = TS_CVE_IMAGE_TYPE_U16C1;
	// 运算模式
	pstTask.pstCascade.cascadeType = TS_CVE_HIST_MODE;

	TS_S32 insize = src_w * src_h;
	s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstSrc[0].au64PhyAddr[0], (void **)&pstTask.pstSrc[0].au64VirAddr[0],
				     NULL, NULL, insize);
	s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstDst[0].au64PhyAddr[0], (void **)&pstTask.pstDst[0].au64VirAddr[0],
				     NULL, NULL, insize);

	int nval = 3;
	char *pSrcData = (char *)(uintptr_t)pstTask.pstSrc[0].au64VirAddr[0];
	memset(pSrcData, nval, insize);

	s32Ret = TS_MPI_TRP_CVE_BeginJob(&pHandle);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve begin job error\n");
		goto ReleaseVB;
	}

	s32Ret = TS_MPI_TRP_CVE_AddTask(pHandle, &pstTask);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve add task error\n");
		goto End;
	}

	CVE_RESULT_S cveResult;

	s32Ret = TS_MPI_TRP_CVE_EndJob(pHandle, &cveResult);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve end job error\n");
		goto End;
	}

	TS_U32 num = cveResult.num;

	SAMPLE_PRT("debug, output num:%d\n", num);

	const TS_S32 *pData = (const TS_S32 *)(uintptr_t)pstTask.pstDst[0].au64VirAddr[0];
	if (pData[nval] != nExpectedValue) {
		SAMPLE_PRT("[ERROR] the result is error\n");
	} else {
		SAMPLE_PRT("[OK] the result is ok\n");
	}

End:
	s32Ret = TS_MPI_TRP_CVE_CancelJob(pHandle);
	if (TS_SUCCESS != s32Ret)
		SAMPLE_PRT("cve end job failed\n");

ReleaseVB:
	TS_MPI_SYS_MmzFree(pstTask.pstSrc[0].au64PhyAddr[0], (void *)(uintptr_t)pstTask.pstSrc[0].au64VirAddr[0]);
	TS_MPI_SYS_MmzFree(pstTask.pstDst[0].au64PhyAddr[0], (void *)(uintptr_t)pstTask.pstDst[0].au64VirAddr[0]);

	return s32Ret;
}



static TS_S32 SAMPLE_CVE_Resize(void)
{
	TS_S32 s32Ret = TS_SUCCESS;

	CVE_HANDLE pHandle;
	CVE_TASK_ATTR_S pstTask;
	TS_S32 src_w = 640;
	TS_S32 src_h = 360;
	memset(&pstTask, 0, sizeof(CVE_TASK_ATTR_S));

	pstTask.dmaInputNum = 1;
	pstTask.pstSrc[0].au32Stride[0] = src_w;
	pstTask.pstSrc[0].u32Width = src_w;
	pstTask.pstSrc[0].u32Height = src_h;
	pstTask.pstSrc[0].enType = TS_CVE_IMAGE_TYPE_ARGB;

	pstTask.dmaOutputNum = 1;
	pstTask.pstDst[0].au32Stride[0] = src_w / 2;
	pstTask.pstDst[0].u32Width = src_w / 2;
	pstTask.pstDst[0].u32Height = src_h / 2;
	pstTask.pstDst[0].enType = TS_CVE_IMAGE_TYPE_ARGB;

	pstTask.pstCascade.cascadeType = TS_CVE_RESIZE_MODE;
	pstTask.pstCascade.operatorCtrl.pstResizeCtrl.u32SubWidth = src_w;
	pstTask.pstCascade.operatorCtrl.pstResizeCtrl.u32SubHeight = src_h;

	// 申请vb资源
	TS_S32 insize = src_w * src_h * 4;
	s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstSrc[0].au64PhyAddr[0], (void **)&pstTask.pstSrc[0].au64VirAddr[0],
				     NULL, NULL, insize);
	s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstDst[0].au64PhyAddr[0], (void **)&pstTask.pstDst[0].au64VirAddr[0],
				     NULL, NULL, insize / 4);
	// 测试数据赋值
	char *pSrcData1 = (char *)(uintptr_t)pstTask.pstSrc[0].au64VirAddr[0];
	memset(pSrcData1, 0, insize);
	FILE *pfInput1 = fopen("data/360p_argb.raw", "rb");
	if (pfInput1) {
		fread(pSrcData1, 1, insize, pfInput1);
		fclose(pfInput1);
	}
	s32Ret = TS_MPI_TRP_CVE_BeginJob(&pHandle);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve begin job error\n");
		goto ReleaseVB;
	}

	s32Ret = TS_MPI_TRP_CVE_AddTask(pHandle, &pstTask);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve add task error\n");
		goto End;
	}

	CVE_RESULT_S cveResult;

	s32Ret = TS_MPI_TRP_CVE_EndJob(pHandle, &cveResult);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve end job error\n");
		goto End;
	}

	TS_U32 num = cveResult.num;

	SAMPLE_PRT("debug, output num:%d\n", num);

	const TS_U8 *pData = (const TS_U8 *)(uintptr_t)pstTask.pstDst[0].au64VirAddr[0];
	FILE *pfOutput = fopen("rst_resize.raw", "wb");
	if (pfOutput) {
		fwrite(pData, 1, insize / 4, pfOutput);
		fclose(pfOutput);
	}

End:
	s32Ret = TS_MPI_TRP_CVE_CancelJob(pHandle);
	if (TS_SUCCESS != s32Ret)
		SAMPLE_PRT("cve end job failed\n");

ReleaseVB:
	// 释放vb资源
	TS_MPI_SYS_MmzFree(pstTask.pstSrc[0].au64PhyAddr[0], (void *)(uintptr_t)pstTask.pstSrc[0].au64VirAddr[0]);
	TS_MPI_SYS_MmzFree(pstTask.pstDst[0].au64PhyAddr[0], (void *)(uintptr_t)pstTask.pstDst[0].au64VirAddr[0]);

	return s32Ret;
}

static TS_S32 SAMPLE_CVE_Affine(void)
{
	TS_S32 s32Ret = TS_SUCCESS;

	CVE_HANDLE pHandle;
	CVE_TASK_ATTR_S pstTask;
	TS_S32 src_w = 640;
	TS_S32 src_h = 360;
	memset(&pstTask, 0, sizeof(CVE_TASK_ATTR_S));

	pstTask.dmaInputNum = 1;
	pstTask.pstSrc[0].au32Stride[0] = src_w;
	pstTask.pstSrc[0].u32Width = src_w;
	pstTask.pstSrc[0].u32Height = src_h;
	pstTask.pstSrc[0].enType = TS_CVE_IMAGE_TYPE_ARGB;

	pstTask.dmaOutputNum = 1;
	pstTask.pstDst[0].au32Stride[0] = src_h;
	pstTask.pstDst[0].u32Width = src_h;
	pstTask.pstDst[0].u32Height = src_w;
	pstTask.pstDst[0].enType = TS_CVE_IMAGE_TYPE_ARGB;

	pstTask.pstCascade.cascadeType = TS_CVE_AFFINE_MODE;
	pstTask.pstCascade.operatorCtrl.pstAffineCtrl.u32SubWidth = 640;
	pstTask.pstCascade.operatorCtrl.pstAffineCtrl.u32SubHeight = 360;
	pstTask.pstCascade.operatorCtrl.pstAffineCtrl.eMode = RIGHT_ROTATE;

	pstTask.pstCascade.operatorCtrl.pstAffineCtrl.a = 0;
	pstTask.pstCascade.operatorCtrl.pstAffineCtrl.b = -1024;
	pstTask.pstCascade.operatorCtrl.pstAffineCtrl.c = -1024;
	pstTask.pstCascade.operatorCtrl.pstAffineCtrl.d = 0;
	pstTask.pstCascade.operatorCtrl.pstAffineCtrl.tx = 0;
	pstTask.pstCascade.operatorCtrl.pstAffineCtrl.ty = 360;

	TS_U32 image_dst_size = pstTask.pstDst[0].u32Width * pstTask.pstDst[0].u32Height * 4;

	TS_U32 insize = pstTask.pstSrc[0].au32Stride[0] * pstTask.pstSrc[0].u32Height * 4;
	s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstSrc[0].au64PhyAddr[0], (void **)&pstTask.pstSrc[0].au64VirAddr[0],
				     NULL, NULL, insize);
	s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstDst[0].au64PhyAddr[0], (void **)&pstTask.pstDst[0].au64VirAddr[0],
				     NULL, NULL, insize);

	// 测试数据赋值
	char *pSrcData1 = (char *)(uintptr_t)pstTask.pstSrc[0].au64VirAddr[0];
	memset(pSrcData1, 0, insize);
	FILE *pfInput1 = fopen("data/360p_argb.raw", "rb");
	if (pfInput1) {
		fread(pSrcData1, 1, insize, pfInput1);
		fclose(pfInput1);
	}

	s32Ret = TS_MPI_TRP_CVE_BeginJob(&pHandle);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve begin job error\n");
		goto ReleaseVB;
	}

	s32Ret = TS_MPI_TRP_CVE_AddTask(pHandle, &pstTask);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve add task error\n");
		goto End;
	}

	CVE_RESULT_S cveResult;

	s32Ret = TS_MPI_TRP_CVE_EndJob(pHandle, &cveResult);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve end job error\n");
		goto End;
	}

	TS_U32 num = cveResult.num;

	SAMPLE_PRT("debug, output num:%d\n", num);

	const TS_U8 *pData = (const TS_U8 *)(uintptr_t)pstTask.pstDst[0].au64VirAddr[0];
	FILE *pfOutput = fopen("rst_amraffine.raw", "wb");
	if (pfOutput) {
		fwrite(pData, 1, image_dst_size, pfOutput);
		fclose(pfOutput);
	}

End:
	s32Ret = TS_MPI_TRP_CVE_CancelJob(pHandle);
	if (TS_SUCCESS != s32Ret)
		SAMPLE_PRT("cve end job failed\n");

ReleaseVB:
	// 释放vb资源
	TS_MPI_SYS_MmzFree(pstTask.pstSrc[0].au64PhyAddr[0], (void *)(uintptr_t)pstTask.pstSrc[0].au64VirAddr[0]);
	TS_MPI_SYS_MmzFree(pstTask.pstDst[0].au64PhyAddr[0], (void *)(uintptr_t)pstTask.pstDst[0].au64VirAddr[0]);

	return s32Ret;
}

static TS_S32 SAMPLE_CVE_Match(void)
{
	TS_S32 s32Ret = TS_SUCCESS;
	CVE_HANDLE pHandle;
	CVE_TASK_ATTR_S pstTask;
	memset(&pstTask, 0, sizeof(CVE_TASK_ATTR_S));
	TS_S32 src_w = 384;
	TS_S32 src_h = 1;
	TS_S32 poolsize = 4;

	pstTask.dmaInputNum = 2;
	pstTask.pstSrc[0].au32Stride[0] = src_w * 2;
	pstTask.pstSrc[0].u32Width = src_w;
	pstTask.pstSrc[0].u32Height = src_h;
	pstTask.pstSrc[0].enType = TS_CVE_IMAGE_TYPE_S16C1;

	pstTask.pstSrc[1].au32Stride[0] = src_w * 2;
	pstTask.pstSrc[1].u32Width = src_w;
	pstTask.pstSrc[1].u32Height = poolsize;
	pstTask.pstSrc[1].enType = TS_CVE_IMAGE_TYPE_S16C1;

	pstTask.dmaOutputNum = 1;
	pstTask.pstDst[0].au32Stride[0] = (poolsize + 31) / 32 * sizeof(TS_U32);
	pstTask.pstDst[0].u32Width = sizeof(TS_U32);
	pstTask.pstDst[0].u32Height = 1;
	pstTask.pstDst[0].enType = TS_CVE_IMAGE_TYPE_U8C1;
	// 运算模式
	pstTask.pstCascade.cascadeType = TS_CVE_MATCH_MODE;
	// 运算参数
	pstTask.pstCascade.operatorCtrl.pstMatchCtrl.dimNumMatch = src_w; // xiang liang wei du
	pstTask.pstCascade.operatorCtrl.pstMatchCtrl.vectNumMatch = poolsize; // xiang liang ge shu
	pstTask.pstCascade.operatorCtrl.pstMatchCtrl.thresholdMatch = 153;

	// 申请vb资源
	TS_S32 insize = src_w * src_h * 2;
	s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstSrc[0].au64PhyAddr[0], (void **)&pstTask.pstSrc[0].au64VirAddr[0],
				     NULL, NULL, insize);
	s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstSrc[1].au64PhyAddr[0], (void **)&pstTask.pstSrc[1].au64VirAddr[0],
				     NULL, NULL, insize * poolsize);
	s32Ret = TS_MPI_SYS_MmzAlloc(&pstTask.pstDst[0].au64PhyAddr[0], (void **)&pstTask.pstDst[0].au64VirAddr[0],
				     NULL, NULL, 640);
	// 测试数据赋值
	TS_S8 *pInData = (TS_S8 *)(uintptr_t)pstTask.pstSrc[0].au64VirAddr[0];
	memcpy(pInData, (char *)gTestFeat, src_w * 2);
	TS_S8 *pInData2 = (TS_S8 *)(uintptr_t)pstTask.pstSrc[1].au64VirAddr[0];
	memcpy(pInData2, (char *)gTestPool, src_w * 2 * poolsize);

	s32Ret = TS_MPI_TRP_CVE_BeginJob(&pHandle);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve begin job error\n");
		goto ReleaseVB;
	}

	s32Ret = TS_MPI_TRP_CVE_AddTask(pHandle, &pstTask);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve add task error\n");
		goto End;
	}

	CVE_RESULT_S cveResult;

	s32Ret = TS_MPI_TRP_CVE_EndJob(pHandle, &cveResult);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("cve end job error\n");
		goto End;
	}

	TS_U32 num = cveResult.num;

	SAMPLE_PRT("debug, output num:%d\n", num);

	const TS_S32 *pData = (const TS_S32 *)(uintptr_t)pstTask.pstDst[0].au64VirAddr[0];
	printf("match val=%d\n", pData[0]);
	printf("print 32 bit data:\n");
	int numVal = pData[0];
	for (int i = 31; i >= 0; i--) {
		// printf("%d ", (numVal >> i) & 1);
		printf("%u ", ((unsigned int)numVal >> i) & 1);
	}
	printf("\n");

End:
	s32Ret = TS_MPI_TRP_CVE_CancelJob(pHandle);
	if (TS_SUCCESS != s32Ret)
		SAMPLE_PRT("cve end job failed\n");

ReleaseVB:
	// 释放vb资源
	TS_MPI_SYS_MmzFree(pstTask.pstSrc[0].au64PhyAddr[0], (void *)(uintptr_t)pstTask.pstSrc[0].au64VirAddr[0]);
	TS_MPI_SYS_MmzFree(pstTask.pstDst[0].au64PhyAddr[0], (void *)(uintptr_t)pstTask.pstDst[0].au64VirAddr[0]);

	return s32Ret;
}

TS_VOID SAMPLE_CVE_Usage(TS_CHAR *sPrgNm)
{
	printf("Usage : %s <index>\n", sPrgNm);
	printf("index:\n");
	printf("\t 0)16to8\n");
	printf("\t 1)Thresh\n");
	printf("\t 2)Map\n");
	printf("\t 3)Add\n");
	printf("\t 4)Xor\n");
	printf("\t 5)Hist\n");
	printf("\t 16)Rotate\n");
	printf("\t 17)Resize\n");
	printf("\t 18)Affine\n");
	printf("\t 19)Match\n");
}

TS_VOID SAMPLE_CVE_HandleSig(TS_S32 signo)
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
		SAMPLE_CVE_Usage(argv[0]);
		return TS_FAILURE;
	}

	if (!strncmp(argv[1], "-h", 2)) {
		SAMPLE_CVE_Usage(argv[0]);
		return TS_SUCCESS;
	}
	s32Ret = SAMPLE_SYS_Init();
	if (s32Ret != TS_SUCCESS) {
		SAMPLE_PRT("SAMPLE_SYS_Init failed\n");
		return TS_FAILURE;
	}

	s32Ret = TS_MPI_TRP_CVE_Init();
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("error, TS_MPI_TRP_CVE_Init failed\n");
		return TS_FAILURE;
	}

	s32Index = atoi(argv[1]);
	switch (s32Index) {
	case 0:
		s32Ret = SAMPLE_CVE_16To8();
		break;
	case 1:
		s32Ret = SAMPLE_CVE_Thresh();
		break;
	case 2:
		s32Ret = SAMPLE_CVE_Map();
		break;
	case 3:
		s32Ret = SAMPLE_CVE_Add();
		break;
	case 4:
		s32Ret = SAMPLE_CVE_Xor();
		break;
	case 5:
		s32Ret = SAMPLE_CVE_Hist();
		break;
	case 17:
		s32Ret = SAMPLE_CVE_Resize();
		break;
	case 18:
		s32Ret = SAMPLE_CVE_Affine();
		break;
	case 19:
		s32Ret = SAMPLE_CVE_Match();
		break;
	default:
		SAMPLE_PRT("the index %d is invaild!\n", s32Index);
		SAMPLE_CVE_Usage(argv[0]);
		s32Ret = TS_FAILURE;
		break;
	}

	TS_MPI_TRP_CVE_Exit();
	SAMPLE_SYS_Exit();
	if (TS_SUCCESS == s32Ret)
		SAMPLE_PRT("program exit normally!\n");
	else
		SAMPLE_PRT("program exit abnormally!\n");

	return s32Ret;
}

TS_S32 multi_task_core(int argc, char *argv[], int initflag, int exitflag)
{
	TS_S32 s32Ret = TS_FAILURE;
	TS_S32 s32Index;

	if (argc < 2 || argc > 2) {
		SAMPLE_CVE_Usage(argv[0]);
		return TS_FAILURE;
	}

	if (!strncmp(argv[1], "-h", 2)) {
		SAMPLE_CVE_Usage(argv[0]);
		return TS_SUCCESS;
	}

	if (initflag) {
		s32Ret = SAMPLE_SYS_Init();
		if (s32Ret != TS_SUCCESS) {
			SAMPLE_PRT("SAMPLE_SYS_Init failed\n");
			return TS_FAILURE;
		}
		s32Ret = TS_MPI_TRP_CVE_Init();
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("error: TS_MPI_TRP_CVE_Init failed\n");
			return TS_FAILURE;
		}
	}

#ifndef __TsingLite__
	signal(SIGINT, SAMPLE_CVE_HandleSig);
	signal(SIGTERM, SAMPLE_CVE_HandleSig);
#endif

	s32Index = atoi(argv[1]);
	SAMPLE_PRT("------------------------ cve test s32Index<%d> \n", s32Index);
	switch (s32Index) {
	case 0:
		s32Ret = SAMPLE_CVE_16To8();
		break;
	case 1:
		s32Ret = SAMPLE_CVE_Thresh();
		break;
	case 2:
		s32Ret = SAMPLE_CVE_Map();
		break;
	case 3:
		s32Ret = SAMPLE_CVE_Add();
		break;
	case 4:
		s32Ret = SAMPLE_CVE_Xor();
		break;
	case 5:
		s32Ret = SAMPLE_CVE_Hist();
		break;
	case 17:
		s32Ret = SAMPLE_CVE_Resize();
		break;
	case 18:
		s32Ret = SAMPLE_CVE_Affine();
		break;
	case 19:
		s32Ret = SAMPLE_CVE_Match();
		break;
	default:
		SAMPLE_PRT("the index %d is invaild!\n", s32Index);
		SAMPLE_CVE_Usage(argv[0]);
		s32Ret = TS_FAILURE;
		break;
	}

	if (exitflag) {
		TS_MPI_TRP_CVE_Exit();
		SAMPLE_SYS_Exit();
	}

	if (TS_SUCCESS == s32Ret)
		SAMPLE_PRT("program exit normally!\n");
	else
		SAMPLE_PRT("program exit abnormally!\n");

	return s32Ret;
}

int main(int argc, char *argv[])
{
	const char *new_argv[3];
	char *parm_argv[3];
	char str_i[10];
	TS_U32 u32Pid = 0;

	TS_MPI_SYS_GetPid(&u32Pid);
	if (argc < 3) {
		if (TX5368_AV200 == u32Pid || TX5336_AV200 == u32Pid)
			SAMPLE_PRT("Usage: %s <mode> [ Use 'simple' or 'multi']  [num_children] \n", argv[0]);
		else
			SAMPLE_PRT("Usage: %s <mode> [ Use 'simple']  [num_children] \n", argv[0]);
		return 1;
	}

	if (strcmp(argv[1], "simple") == 0) {
		// If "simple", execute simple_task_core

		int num_index = atoi(argv[2]);
		// sprintf(str_i, "%d", num_index);
		snprintf(str_i, sizeof(str_i), "%d", num_index);
		new_argv[0] = "simple_task_core";
		new_argv[1] = str_i;
		new_argv[2] = NULL;

		parm_argv[0] = (char *)(new_argv[0]);
		parm_argv[1] = (char *)(new_argv[1]);
		parm_argv[2] = NULL;

		return simple_task_core(2, parm_argv);
	} else if (strcmp(argv[1], "multi") == 0 && ((TX5368_AV200 == u32Pid) || (TX5336_AV200 == u32Pid))) {
		//  If "multi", execute multi_task_core
		int num_children = 3;  // Default number of child processes

		if (argc >= 3) {
			num_children = atoi(argv[2]);  // Get number of child processes from command line argument
			SAMPLE_PRT(" num_children  %d \n", num_children);
			if (num_children <= 0) {
				SAMPLE_PRT("Invalid number of children. Please provide a positive integer.\n");
				return 1;
			}
		}

		for (int i = 0; i < num_children; i++) {
			pid_t pid = fork();
			if (pid == 0) {  // Child process
				for (int j = 0; j < 15; j++) {
					snprintf(str_i, sizeof(str_i), "%d", j);
					new_argv[0] = "multi_task_core";
					new_argv[1] = str_i;
					new_argv[2] = NULL;

					parm_argv[0] = (char *)(new_argv[0]);
					parm_argv[1] = (char *)(new_argv[1]);
					parm_argv[2] = NULL;

					if (j == 0)
						multi_task_core(2, parm_argv, 1, 0);
					else if (j >= (15 - 1))
						multi_task_core(2, parm_argv, 0, 1);
					else
						multi_task_core(2, parm_argv, 0, 0);
				}
				_exit(0);
			} else if (pid > 0) {
				wait(NULL);  // Parent process waits for each child process to finish
			} else {
				perror("fork");
				return 1;
			}
		}
	} else {
		if (TX5368_AV200 == u32Pid || TX5336_AV200 == u32Pid)
			SAMPLE_PRT("Invalid mode. Use 'simple' or 'multi'.\n");
		else
			SAMPLE_PRT("Invalid mode. Use 'simple'.\n");
		return 1;
	}

	return 0;
}