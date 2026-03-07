/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: mpi definitions of cve
 * Author: Tsing Micro solution-application group
 * Create: 2022/07/20
 */
#ifndef _TS_CVE_H_
#define _TS_CVE_H_

#include "ts_comm_cve.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define TS_CVE_JOB_MAX 16
#define TS_CVE_CASCADS_MAX 12
#if defined(DX5215) || defined(DX5112)
#define TS_CVE_DMA_INPUT_ALL_MAX 2
#define TS_CVE_DMA_INPUT_MAX 2
#define TS_CVE_DMA_OUTPUT_ALL_MAX 2
#define TS_CVE_DMA_OUTPUT_MAX 2
#else
#define TS_CVE_DMA_INPUT_ALL_MAX 4
#define TS_CVE_DMA_INPUT_MAX 4
#define TS_CVE_DMA_OUTPUT_ALL_MAX 4
#define TS_CVE_DMA_OUTPUT_MAX 4
#endif


/// 实现 16bit 数据到 8bit 的线性转化，支持4种转换模式
typedef enum tsCVE_16BIT_TO_8BIT_MODE_E {
	TS_CVE_16BIT_TO_8BIT_MODE_S16_TO_S8 = 0x0,			/// 将 16 位有符号整数（S16）转换为 8 位有符号整数（S8）
	TS_CVE_16BIT_TO_8BIT_MODE_S16_TO_U8_ABS = 0x1,		/// 将 16 位有符号整数（S16）转换为 8 位无符号整数（U8），绝对值转换
	TS_CVE_16BIT_TO_8BIT_MODE_S16_TO_U8_BIAS = 0x2,		/// 将 16 位有符号整数（S16）转换为 8 位无符号整数（U8），带有偏置的转换
	TS_CVE_16BIT_TO_8BIT_MODE_U16_TO_U8 = 0x3,			/// 将 16 位无符号整数（U16）转换为 8 位无符号整数（U8）
	TS_CVE_16BIT_TO_8BIT_MODE_BUTT
} CVE_16BIT_TO_8BIT_MODE_E;

typedef struct tsCVE_16BIT_TO_8BIT_CTRL_S {
	CVE_16BIT_TO_8BIT_MODE_E enMode;
	// TS_U16 u16Denominator;
	// TS_U8  u8Numerator;
	TS_U16 parameterK;
	TS_S8 bias;
} CVE_16BIT_TO_8BIT_CTRL_S;

/*
 *Add control parameters,created, 2013-07-17
 */
typedef struct tsCVE_ADD_CTRL_S {
	TS_U16 u0q16x; /*x of "xA+yB"*/
	TS_U16 u0q16y; /*y of "xA+yB"*/
} CVE_ADD_CTRL_S;

// 加减法
typedef enum tsCVE_ADD_SUB_TYPE_E {
	TS_CVE_MODE_SUB = 0x0,
	TS_CVE_MODE_ADD = 0x1,
	TS_CVE_MODE_BULL
} CVE_ADD_SUB_TYPE_E;

typedef enum tsCVE_SUB_MODE_E {
	TS_CVE_SUB_MODE_SHIFT = 0x0, 	/// 输出为差值的绝对值，输出为 U8C1	 Iout
	TS_CVE_SUB_MODE_ABS = 0x1,		/// 输出为差值的一半，输出为 U8C1
 	TS_CVE_SUB_MODE_BUTT
} CVE_SUB_MODE_E;

typedef struct tsCVE_ADD_SUB_CTRL_S {
	CVE_ADD_SUB_TYPE_E subaddMode; /* 0:sub,1:add */
	CVE_SUB_MODE_E emode;
	TS_U16 addCoefa;
	TS_U16 addCoefb;
} CVE_ADD_SUB_CTRL_S;

typedef enum tsCVE_ROTATION_MODE_E {
	TS_ROTATION_ARGB = 0x0,
	TS_ROTATION_ABGR = 0x1,
	TS_ROTATION_BGRA = 0x2,
	TS_ROTATION_RGBA = 0x3,
	TS_ROTATION_YUV422_YUYV = 0x4,
	TS_ROTATION_YUV422_YVYU = 0x5,
	TS_ROTATION_YUV422_UYVY = 0x6,
	TS_ROTATION_YUV422_VYUY = 0x7,
	TS_ROTATION_YUV400 = 0X8,
	TS_ROTATION_YUV422SP_NV12 = 0X10,
	TS_ROTATION_YUV420SP_NV12 = 0X11,
	TS_ROTATION_YUV422SP_NV21 = 0X12,
	TS_ROTATION_YUV420SP_NV21 = 0X13,
	TS_ROTATION_YUV422_SP = 0X20,
	TS_ROTATION_YUV420_SP = 0X21,
}CVE_ROTATION_MODE_E;

typedef enum tsCVE_ROTATION_DEGREE_E {
	TS_ROTATION_90 = 0x0,
	TS_ROTATION_180 = 0x1,
	TS_ROTATION_270 = 0x2,
}CVE_ROTATION_DEGREE_E;

// 实现两灰度图像逐像素相与、相或、相异或功能，输入输出图像格式为 U8C1 
typedef enum tsCVE_AND_OR_XOR_E {
	TS_CVE_AND_MODE,	// 相与
	TS_CVE_OR_MODE,		// 相或
	TS_CVE_XOR_MODE		// 相异或
} CVE_AND_OR_XOR_E;

// 阈值化算子使用不同的阈值使图像阈值化。根据输入输出数据格式的不同，阈值化算子包含三个子模块，即 cve_thresh_u8、cve_thresh_u16、cve_thresh_s16。
// cve_thresh_u8 支持U8C1到U8C1的转换，支持8种运算方式
// cve_thresh_u16 支持U16C1到U8C1的转换，支持2种运算方式
// cve_thresh_s16 支持S16C1到U8C1、S8C1的转换，支持4种运算方式
typedef enum tsCVE_THRESH_S16_MODE_E {
	TS_CVE_THRESH_S16_MODE_S16_TO_S8_MIN_MID_MAX = 0x0,		/// -32768<=lowThr<=highThr<=32767 -128<=minVal midVal maxVal<=127
	TS_CVE_THRESH_S16_MODE_S16_TO_S8_MIN_ORI_MAX = 0x1,		/// -129<=lowThr<=highThr<=127 -128<=minVal midVal maxVal<=127
	TS_CVE_THRESH_S16_MODE_S16_TO_U8_MIN_MID_MAX = 0x2,		/// -32768<=lowThr<=highThr<=32767 0<=minVal midVal maxVal<=255
	TS_CVE_THRESH_S16_MODE_S16_TO_U8_MIN_ORI_MAX = 0x3,		/// -1<=lowThr<=highThr<=255 0<=minVal oriVal maxVal<=255

	TS_CVE_THRESH_S16_MODE_BUTT
} CVE_THRESH_S16_MODE_E;

/*
 *Type of the Thresh_unsigned short mode
 */
typedef enum tsCVE_THRESH_U16_MODE_E {
	TS_CVE_THRESH_U16_MODE_U16_TO_U8_MIN_MID_MAX = 0x0,		/// 将16位的 srcVal 转换为8位的 dstVal，并将其映射到 U8 的 minVal、midVal 或 maxVal [0 <= lowThr <= highThr <= 65536]
	TS_CVE_THRESH_U16_MODE_U16_TO_U8_MIN_ORI_MAX = 0x1,		/// 将16位的 srcVal 转换为8位的 dstVal，并将其映射到 U8 的 minVal、oriVal 或 maxVal [0 <= lowThr <= highThr <= 255]

	TS_CVE_THRESH_U16_MODE_BUTT
} CVE_THRESH_U16_MODE_E;

typedef enum tsCVE_THRESH_MODE_E {
	TS_CVE_THRESH_MODE_U8_EN = 0x0,
	TS_CVE_THRESH_MODE_U16_EN,
	TS_CVE_THRESH_MODE_S16_EN,
} CVE_THRESH_MODE_E;

typedef struct tsCVE_THRESH_S16_CTRL_S {
	CVE_THRESH_S16_MODE_E enMode;
	TS_S16 s16LowThr; /*User-defined threshold*/
	TS_S16 s16HighThr; /*User-defined threshold*/
	CVE_8BIT_U un8MinVal; /*Minimum value when tri-level thresholding*/
	CVE_8BIT_U un8MidVal; /*Middle value when tri-level thresholding*/
	CVE_8BIT_U un8MaxVal; /*Maxmum value when tri-level thresholding*/
} CVE_THRESH_S16_CTRL_S;

typedef struct tsCVE_THRESH_U16_CTRL_S {
	CVE_THRESH_U16_MODE_E enMode;
	TS_U16 u16LowThr;
	TS_U16 u16HighThr;
	TS_U8 u8MinVal;
	TS_U8 u8MidVal;
	TS_U8 u8MaxVal;
} CVE_THRESH_U16_CTRL_S;

typedef enum tsCVE_THRESH_U8_MODE_E {

	TS_CVE_THRESH_MODE_BINARY = 0x0, 		/// 如果 srcVal 小于或等于 lowThr，dstVal 设置为 minVal；否则，dstVal 设置为 maxVal
	TS_CVE_THRESH_MODE_TRUNC = 0x1, 		/// 如果 srcVal 小于或等于 lowThr，dstVal 设置为 srcVal；否则，dstVal 设置为 maxVal
	TS_CVE_THRESH_MODE_TO_MINVAL = 0x2, 	/// 如果 srcVal 小于或等于 lowThr，dstVal 设置为 minVal；否则，dstVal 设置为 srcVal
	TS_CVE_THRESH_MODE_MIN_MID_MAX = 0x3,	/// 根据某种规则，将 srcVal 映射到 minVal、midVal 或 maxVal [lowThr, highThr]
	TS_CVE_THRESH_MODE_ORI_MID_MAX = 0x4,	/// 将 srcVal 映射到 oriVal、midVal 或 maxVal [lowThr, highThr]
	TS_CVE_THRESH_MODE_MIN_MID_ORI = 0x5,	///  将 srcVal 映射到 minVal、midVal 或 oriVal [lowThr, highThr]
	TS_CVE_THRESH_MODE_MIN_ORI_MAX = 0x6,	/// 将 srcVal 映射到 minVal、oriVal 或 maxVal [lowThr, highThr]
	TS_CVE_THRESH_MODE_ORI_MID_ORI = 0x7,	/// 将 srcVal 映射到 oriVal、midVal 或 oriVal [lowThr, highThr]
	TS_CVE_THRESH_MODE_BUTT
} CVE_THRESH_U8_MODE_E;

typedef struct tsCVE_THRESH_U8_CTRL_S {
	CVE_THRESH_U8_MODE_E enMode;
	TS_U8 u8LowThr; /*user-defined threshold,  0<=u8LowThr<=255 */
	TS_U8 u8HighThr;
	TS_U8 u8MinVal; /*Minimum value when tri-level thresholding*/
	TS_U8 u8MidVal; /*Middle value when tri-level thresholding, if enMode<2, u32MidVal is not used; */
	TS_U8 u8MaxVal; /*Maxmum value when tri-level thresholding*/
} CVE_THRESH_U8_CTRL_S;

typedef struct tsCVE_THRESH_CTRL_S {
	CVE_THRESH_MODE_E enMode;
	CVE_THRESH_S16_CTRL_S pstThrS16Ctrl;
	CVE_THRESH_U8_CTRL_S pstThrU8Ctrl;
	CVE_THRESH_U16_CTRL_S pstThrU16Ctrl;
} CVE_THRESH_CTRL_S;

/// map算子实现对源图像中的每个像素，查找map查找表中的值，赋予目标图像相应像素查找表中的值的功能
typedef enum tsCVE_MAP_MODE_E {
	TS_CVE_MAP_MODE_U8 = 0x0,	/// 支持 U8C1 到 U8C1 的映射
	TS_CVE_MAP_MODE_U16 = 0x1,	/// 支持 U8C1 到 U16C1 的映射
	TS_CVE_MAP_MODE_S16 = 0x2,	/// 支持 U8C1 到 S16C1 的映射
	TS_CVE_MAP_MODE_BUTT
} CVE_MAP_MODE_E;

typedef struct tsCVE_MAP_CTRL_S {
	TS_U8 mapRamAddr;
	CVE_MAP_MODE_E enMode;
	TS_U16 mapSize;
	TS_U16 map[256];
} CVE_MAP_CTRL_S;

/// 滤波硬件加速算子中有很多的滤波算法，包括高斯滤波、膨胀、腐蚀、sobel滤波、顺序统计量滤波等，但是这些算子的卷积核大小不是全部相同的
typedef enum tsCVE_LBP_CMP_MODE_E {
	TS_CVE_LBP_CMP_MODE_MED = 0x0,		/// 中值
	TS_CVE_LBP_CMP_MODE_NORMAL = 0x1, /* P(x)-P(center)>= un8BitThr.s8Val, s(x)=1; else s(x)=0; */	/// LBP普通
	TS_CVE_LBP_CMP_MODE_ABS = 0x2, /* Abs(P(x)-P(center))>=un8BitThr.u8Val, s(x)=1; else s(x)=0; */ /// LBP绝对值
	TS_CVE_LBP_CMP_MODE_BUTT
} CVE_LBP_CMP_MODE_E;

typedef enum tsCVE_ORD_MODE_E {
	TS_CVE_ORD_MODE_MED = 0x0,
	TS_CVE_ORD_MODE_MIN = 0x1,
	TS_CVE_ORD_CMP_MODE_BUTT
} CVE_ORD_MODE_E;

typedef enum tsCVE_FLT_MODE_E {
	TS_CVE_FILTER_MODE_ORDINARY = 0x0,
	TS_CVE_FILTER_MODE_SUBTRACT_BY_ORIGINAL = 0x1,
	TS_CVE_FILTER_MODE_BINARIZE_WITH_NOISE = 0x2,
	TS_CVE_FILTER_MODE_RESERVE = 0x3,
	TS_CVE_FILTER_MODE_DILATE = 0x4,
	TS_CVE_FILTER_MODE_ERODE = 0x5
} CVE_FLT_MODE_E;


/// 统计滤波 3x3窗口排序滤波
typedef struct tsCVE_LBP3x3_CTRL_S {
	CVE_LBP_CMP_MODE_E enMode;
	TS_U8 threshLbp3x3;
} CVE_LBP3x3_CTRL_S;

typedef struct tsCVE_ORD5x5_CTRL_S {
	CVE_ORD_MODE_E enMode;
} CVE_ORD5x5_CTRL_S;

typedef struct tsCVE_FLT5x5_CTRL_S {
	CVE_FLT_MODE_E enMode;
	TS_U8 coefFlt5x5_00_04[5];
	TS_U8 coefFlt5x5_10_14[5];
	TS_U8 coefFlt5x5_20_24[5];
	TS_U8 coefFlt5x5_30_34[5];
	TS_U8 coefFlt5x5_40_44[5];
	TS_U8 fltThresh;
	TS_U8 shiftNumFlt5x5;
} CVE_FLT5x5_CTRL_S;

typedef enum tsCVE_SOB_MODE_E {
	TS_SOBEL_CTRL_BOTH,
	TS_SOBEL_CTRL_HOR,
	// TS_SOBEL_CTRL_VER,
	TS_SOBEL_CTRL_NCC = 3,
	TS_SOBEL_CTRL_BOTH_MAG,
	TS_SOBEL_CTRL_MAG,
	TS_SOBEL_CTRL_GLPF,
} CVE_SOB_MODE_E;

typedef struct tsCVE_SOB5x5_CTRL_S {
	TS_U8 shiftNumSob5x5;
	CVE_SOB_MODE_E modeSob5x5;
	TS_U8 coefSob5x5V[5];
	TS_U8 coefSob5x5H[5];
	TS_U16 mag_param[8];
	TS_U8 mag_qparam;
	TS_U16 sob5x5Thresh;
} CVE_SOB5x5_CTRL_S;

typedef enum tsCVE_TOF_MODE_E {
	TS_TOF_DATA_0_4,
	TS_TOF_DATA_1_3,
} CVE_TOF_MODE_E;

typedef enum tsCVE_TOF_SIM_OR_MIX_E {
	TS_SIMPLE_MODE,
	TS_MIX_MODE,
} CVE_TOF_SIM_OR_MIX_E;

typedef struct tsCVE_TOF_CTRL_S {
	//CVE_TOF_SIM_OR_MIX_E mixMode; //0:simple 1:mix
	CVE_TOF_MODE_E tofMode;
} CVE_TOF_CTRL_S;

typedef struct tsCVE_DISP_CTRL_S {
	TS_U32 dispCnsta_8_39;
	TS_U32 dispCnsta_0_7;
	TS_U32 dispCnstb;
	TS_U8 constMin_disp;
	TS_U8 negateDisp;
	TS_U16 cosntMaxDisp;
} CVE_DISP_CTRL_S;

typedef struct tsCVE_MATCH_CTRL_S {
	TS_U16 dimNumMatch;
	TS_U16 thresholdMatch;
	TS_U32 vectNumMatch;
} CVE_MATCH_CTRL_S;

typedef struct tsCVE_NCCMTH_CTRL_S {
	TS_U8 windowNccM;
	TS_U16 dispNumNccM;
	TS_U16 uniqratioNccM;
	TS_U8 uniqratioQNccM;
} CVE_NCCMTH_CTRL_S;

typedef enum tsCVE_PSP_SRCIMG_TYPE_E {
	TS_GRAY_SCALE,
	TS_YUV420SPN12,
	TS_YUV420SPN21,
} CVE_PSP_SRCIMG_TYPE_E;

typedef enum tsCVE_PSP_TRANSMODE_E {
	TS_NO_BCR_SIMILAR_CONVERT,
	TS_SIMILAR_CONVERT,
	TS_AFFINE_CONVERT,
} CVE_PSP_TRANSMODE_E;

typedef enum tsCVE_PSP_CSCMODE_E {
	TS_NO_COLOR_SPACE_CONVERT,
	TS_BT_601_YUV2RGB_VIDEO_CONVERT,
	TS_BT_709_YUV2RGB_VIDEO_CONVERT,
	TS_BT_601_YUV2RGB_IMG_CONVERT,
	TS_BT_709_YUV2RGB_IMG_CONVERT,
} CVE_PSP_CSCMODE_E;

typedef enum tsCVE_PSP_DOUTMODE_E {
	TS_GRAYSCALE,
	TS_YUV420N12,
	TS_YUV420N21,
	TS_ARGB8888,
	TS_ABGR8888,
	TS_RGBA8888,
	TS_BGRA8888,
	TS_RGB888,
	TS_BGR888,
} CVE_PSP_DOUTMODE_E;

#pragma pack(1)
typedef struct tsCVE_PSP_ROI_S {
	TS_U32 u16Left : 12;
	TS_U32 u16Rigth : 12;
	TS_U32 u16Top : 12;
	TS_U32 u16Bottom : 12;
	TS_U32 u16Stride0 : 12;
	TS_U32 u16Stride1 : 12;
	TS_U32 u16DstWidth : 12;
	TS_U32 u16DstHeight : 12;
	TS_U32 u16BlockYSize : 12;
	TS_U32 u16BlockXSize : 12;
} CVE_PSP_ROI_S;
#pragma pack()

typedef struct tsCVE_PSP_MATRIX_S {
	TS_U16 Matrix0;
	TS_U16 Matrix1;
	TS_U16 Matrix2;
	TS_U16 Matrix3;
	TS_U16 Matrix4;
	TS_U16 Matrix5;
	TS_U16 reserve0;
	TS_U16 reserve1;
} CVE_PSP_MATRIX_S;

typedef struct tsCVE_PSP_CTRL_S {
	CVE_PSP_SRCIMG_TYPE_E pspSrcmode;
	CVE_PSP_TRANSMODE_E pspTransmode;
	CVE_PSP_CSCMODE_E pspCscmode;
	CVE_PSP_DOUTMODE_E pspDoutmode;
	TS_U8 pspPpairNum;
	TS_U8 pspRoiNum;
	TS_U64 pspRoiStaddr;
	TS_U64 pspMtrxStaddr;
	TS_U64 pspPpStaddr;
} CVE_PSP_CTRL_S;

typedef struct tsCVE_ROTATION_CTRL_S
{
	CVE_ROTATION_MODE_E eFormat;
	CVE_ROTATION_DEGREE_E eDegree;
}CVE_ROTATION_CTRL_S;

typedef struct tsCVE_RESIZE_CTRL_S
{
	TS_U32 u32SubWidth;         /* The width of the sub image */
	TS_U32 u32SubHeight;        /* The height of the sub image */
}CVE_RESIZE_CTRL_S;

typedef enum tsCVE_AFFINE_CTRL_E {
	LEFT_ROTATE,
	RIGHT_ROTATE,
	OTHER_ROTATE,
	BOTTOM_NUM
} CVE_AFFINE_MODE_E;

typedef struct tsCVE_AFFINE_CTRL_S
{
	TS_U32 u32SubWidth;         /* The width of the sub image */
	TS_U32 u32SubHeight;        /* The height of the sub image */
	TS_U16 a;
	TS_U16 b;
	TS_U16 tx;
	TS_U16 c;
	TS_U16 d;
	TS_U16 ty;
	CVE_AFFINE_MODE_E eMode;
}CVE_AFFINE_CTRL_S;


typedef enum tsCVE_BIS_JOINT_MODE_E {
    TS_BIS_TWO_EYE_MODE = 1,
    TS_BIS_THREE_EYE_MODE = 2,
    TS_BIS_FOUR_EYE_MODE = 3,
    TS_BIS_NONE_MODE,
}BIS_JOINT_MODE_E;

typedef  struct tsCVE_BIS_INFO_S {
    BIS_JOINT_MODE_E joint_mode;
}CVE_BIS_INFO_S;

typedef enum tsCVE_CRC_FORMAT_E
{
	TS_CRC_FORMAT_8 = 1,
	TS_CRC_FORMAT_16 = 2,
	TS_CRC_FORMAT_32 = 4,
} CVE_CRC_FORMAT_E;

typedef enum tsCVE_CRC_XOR_FORMAT_E
{
	TS_CRC_XOR_FORMAT_0 = 0,
	TS_CRC_XOR_FORMAT_55 = 1,
	TS_CRC_XOR_FORMAT_aa = 2,
	TS_CRC_XOR_FORMAT_ff = 3,
}CVE_CRC_XOR_FORMAT_E;

typedef struct tsCVE_CRC_CTRL_S {

	TS_U32 crc_src_data_lenght;
	CVE_CRC_FORMAT_E crc_format;
	TS_U8 crc_refout;
	TS_U8 gen_check;
	CVE_CRC_XOR_FORMAT_E crc_xor_format;
	TS_U8 crc_refin;
	TS_U32 crc_result;
	TS_U8 crc_check_result;
	TS_U32 crc_dream_crc_value;
} CVE_CRC_CTRL_S;


typedef union tsCVE_OPERATOR_CTRL_U {
	CVE_ADD_SUB_CTRL_S pstAddSubCtrl;
	CVE_16BIT_TO_8BIT_CTRL_S pst16to8Ctrl;
	CVE_AND_OR_XOR_E pstAndOrXorCtrl;
	CVE_MAP_CTRL_S pstMapCtrl;
	CVE_THRESH_CTRL_S pstThreshCtrl;
	CVE_LBP3x3_CTRL_S pstLbp3x3Ctrl;
	CVE_ORD5x5_CTRL_S pstOrd5x5Ctrl;
	CVE_FLT5x5_CTRL_S pstFlt5x5Ctrl;
	CVE_SOB5x5_CTRL_S pstSob5x5Ctrl;
	CVE_TOF_CTRL_S pstTofCtrl;
	CVE_DISP_CTRL_S pstDispCtrl;
	CVE_MATCH_CTRL_S pstMatchCtrl;
	CVE_NCCMTH_CTRL_S pstNccMthCtrl;
	CVE_PSP_CTRL_S pstPspCtrl;
	CVE_ROTATION_CTRL_S pstRotateCtrl;
	CVE_CRC_CTRL_S pstCrcCtrl;
	CVE_RESIZE_CTRL_S pstResizeCtrl;
	CVE_AFFINE_CTRL_S pstAffineCtrl;
	CVE_BIS_INFO_S   pstBisCtrl;
} CVE_OPERATOR_CTRL_U;

typedef enum tsCVE_TASK_MODE_E {
	TS_CVE_16TO8_MODE,
	TS_CVE_THRESH_MODE,
	TS_CVE_MAP_MODE,
	TS_CVE_ADD_SUB_MODE,
	TS_CVE_AND_OR_XOR_MODE,
	TS_CVE_LBP3x3_MODE,
	TS_CVE_ORD5x5_MODE,
	TS_CVE_FLT5x5_MODE,
	TS_CVE_SOB5x5_MODE,
	TS_CVE_HIST_MODE,
	TS_CVE_TOF_MODE,
	TS_CVE_DISP2DEP_MODE,
	TS_CVE_MATCH_MODE,
	TS_CVE_NCCCAL_MODE,
	TS_CVE_NCCMTH_MODE,
	TS_CVE_PSP_MODE,
	TS_CVE_ROTATE_MODE,
	TS_CVE_CRC_MODE,
	TS_CVE_CASCADE_MODE,
	TS_CVE_AFFINE_MODE,
	TS_CVE_RESIZE_MODE,
	TS_CVE_DMA_COPY_MODE,
	TS_CVE_BIS_MODE,
} CVE_TASK_MODE_E;

typedef struct tsCVE_CASCADE_S {
	CVE_TASK_MODE_E cascadeType;
	CVE_OPERATOR_CTRL_U operatorCtrl;
} CVE_CASCADE_S;

typedef struct tsCVE_TASK_ATTR_S {
	TS_U8 dmaInputNum;
	CVE_IMAGE_S pstSrc[TS_CVE_DMA_INPUT_MAX];
	TS_U8 dmaOutputNum;
	CVE_IMAGE_S pstDst[TS_CVE_DMA_OUTPUT_MAX];
	CVE_CASCADE_S pstCascade;
} CVE_TASK_ATTR_S;

typedef struct tsCVE_RESULT_S {
	TS_U8 num;
	CVE_IMAGE_S image[TS_CVE_DMA_OUTPUT_MAX];
} CVE_RESULT_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /*_TS_CVE_H_*/
