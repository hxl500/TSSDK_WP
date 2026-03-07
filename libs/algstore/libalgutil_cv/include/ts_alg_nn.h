
/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: base type definitions of alg
 * Author: Tsing Micro solution-application group
 * Create: 2024/06/01
 */

#ifndef _TS_ALG_NN_H_
#define _TS_ALG_NN_H_

#include "ts_rne_c_api.h"
#include "ts_alg_type.h"

#include "ts_rne_c_api.h"
#include "ts_alg_info.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define ALG_NN_MAX_BOX_NUM 200
#define ALG_STRING_LEN 255

#define ALG_STR_INTERNAL(a)  #a             // 辅助宏，将参数宏定义展开为字符串
#define ALG_STR(a)  ALG_STR_INTERNAL(a)     // 主要宏，将参数宏定义先进行宏展开，然后再转换为字符串

typedef enum tsALG_NN_STATE_E {
	ALG_NN_STATE_IDLE,     //空闲状态
	ALG_NN_STATE_INIT,     //初始化状态
	ALG_NN_STATE_PROCESS,  //处理中状态
	ALG_NN_STATE_EXIT,     //退出状态
} ALG_NN_STATE_E;

typedef struct tsALG_NN_BOX_S {
	//the range of x and y is 0.0f~1.0f
	TS_FLOAT f32Xmin;        //检测目标的归一化左上角坐标x
	TS_FLOAT f32Ymin;        //检测目标的归一化左上角坐标y
	TS_FLOAT f32Xmax;        //检测目标的归一化右下角坐标x
	TS_FLOAT f32Ymax;        //检测目标的归一化右下角坐标y
	TS_FLOAT f32Score;       //检测目标的置信度
	TS_FLOAT f32Id;          //检测目标的类别索引
	TS_FLOAT f32KptsX[ALG_NN_MAX_BOX_NUM];    //kpts x
	TS_FLOAT f32KptsY[ALG_NN_MAX_BOX_NUM];    //kpts y
	TS_FLOAT f32KptsV[ALG_NN_MAX_BOX_NUM];    //kpts conf
	TS_FLOAT f32Reserve;
} ALG_NN_BOX_S;

typedef struct tsALG_NN_RESULT_S {
	TS_U32 u32ObjNum;                        //检测目标数量
	TS_U32 u32ClsID[ALG_NN_MAX_BOX_NUM];     //检测目标类别索引
	ALG_NN_BOX_S stBox[ALG_NN_MAX_BOX_NUM];  //检测目标的坐标、概率值
} ALG_NN_RESULT_S;

typedef struct tsALG_NN_PAD_S {
    TS_U32 pad_top;       //顶部像素填充值
    TS_U32 pad_bottom;    //底部像素填充值
    TS_U32 pad_left;      //左边像素填充值
    TS_U32 pad_right;     //右边像素填充值
}ALG_NN_PAD_S;

typedef struct tsALG_NN_CFG_S 
{
    TS_U32      u32NNType;             //NN模型类型，0:YOLOv5, 1:YOLOv8
    TS_U32      u32NNDataType;         //模型推理输出数据类型，0:uint, 1:int
    TS_U32      u32NNLayersNum;        //输出Feature包含的层次数量，默认为3，包含关键点时为1
    TS_U32      u32NNPointsNum;        //通道包含关键点数量
    TS_U32      u32NNPointValNum;      //关键点值所占通道数，默认为3, (x, y, v)
    TS_U32      u32MaxDet;             //最大检测数量

    TS_U32      u32NNInShapeCHW[3];    //模型输入类型, 如{3，384，640}
	TS_FLOAT    f32ConfThresh[20];     //置信度阈值
	TS_FLOAT    f32IouThresh[20];      //交并比阈值
	TS_FLOAT    f32KeyPntThresh;       //关键点的阈值
    TS_FLOAT    f32Anchs[3][6];        //锚点值

    TS_U32      u32NNChannel;          //[out]输出通道数, process中实时获取
    TS_U32      u32NNStrides[3];       //[out]下采样步长, process中实时获取
    TS_FLOAT    f32NNCoeff[3];         //[out]反量化系数, process中实时获取

    TS_FLOAT    *pPreSelBuf;           //[out]后处理预选目标数据，process中申请内存
    TS_U32      u32PreSelNum;          //[out]预选目标数量

    ALG_NN_PAD_S    stPadVal;       //像素填充值
    ALG_NN_STATE_E  emState;        //状态
    TS_U32          enDebugMode;    //调试模式，0:关闭，1:开启(输出中间结果)
} ALG_NN_CFG_S;  
typedef struct tsALG_NN_FILE_INFO_S
{
    TS_CHAR input_bin[ALG_STRING_LEN];    //输入文件路径
    TS_CHAR cfg_bin[ALG_STRING_LEN];      //模型指令文件路径
    TS_CHAR weight_bin[ALG_STRING_LEN];   //模型权重文件路径
    TS_CHAR model_bin[ALG_STRING_LEN];    //RNE3模型文件路径
    TS_CHAR version[ALG_STRING_LEN];      //模型版本号
	TS_U32    class_num;                  //类别数量
	TS_CHAR   cls_name[100][20];          //类别名称
	TS_U32    u32NNPointsNum;             //通道包含关键点数量
	TS_FLOAT  f32KeyPntThresh;            //关键点的阈值
	ALG_NN_PAD_S    stPadVal;             //像素填充值
} ALG_NN_FILE_INFO_S;

typedef struct tsALG_NN_CTX_S {
	ALG_StatInfo_S gStatInfo;
	ALG_NN_CFG_S   gNNCfg;
	TS_FLOAT      *gPostProcBuf;
	RNE_NET_S     *nModel;
	TS_BOOL       gRneOff;
}ALG_NN_CTX_S;

typedef struct tsALG_NMS_Param_S {
	TS_FLOAT  f32IOUThresh[20];   //交并比阈值
	TS_U32    u32DataLen;         //预选框数量
	TS_U32    u32ChnNum;          //输出通道数
	TS_U32    u32MaxDet;          //最大检测数量
	TS_U32    u32ScoreOffset;     //通道中置信度索引值，默认为4
	TS_U32    u32NNType;          //NN模型类型，0:YOLOv5, 1:YOLOv8
}ALG_NMS_Param_S;

/**
 * @brief nms for YOLOv5
 * @param[in] torchCat, pointer of input candidate BBoxes
 * @param[in] len1, torchCat data length
 * @param[in] len2, detect channel num
 * @param[out] nmsOut, pointer of BBoxes afer nms
 * @param[in] thresh, IOU threshold
 * @param[in] maxDet, max num of nms detect
 * @param[in] scoreOffset, offset of BBox score in channel
 * @return nms result num
 */
int TS_ALG_NMS_V5(float *torchCat, int len1, int len2, int *nmsOut, float thresh, int maxDet, int scoreOffset);

/**
 * @brief nms for YOLOv8
 * @param[in] bboxes, pointer of input candidate BBoxes
 * @param[in] iouThehold, IOU threshold
 * @param[in] bboxes_num, bboxes data length
 * @param[in] chn, detect channel num
 * @param[in] maxDet, max num of nms detect
 * @param[out] nmsOut, pointer of BBoxes afer nms
 * @return nms result num
 */
int TS_ALG_NMS_V8(float *bboxes,  float iouThehold, int bboxes_num, int chn, int maxDet, int *nmsOut);


/**
 * @brief NMS
 *
 * @param pfBoxes 输入的边界框数组，每个边界框由四个浮点数表示：x, y, w, h。
 * @param pfScores 输入的分数数组，每个分数对应一个边界框。
 * @param pu8Clsidx 输入的类别索引数组，每个索引对应一个边界框。
 * @param boxnum 边界框的数量。
 * @param piouthresh 输入的每个类别的IOU阈值数组。
 * @param maxDet 最大检测数量，即最多保留多少个边界框。
 * @param nmsOut 输出的经过NMS筛选后的边界框索引数组。
 * @return 返回经过NMS筛选后的边界框数量。
 */
int TS_ALG_NMS_F(float *pfBoxes, float *pfScores, int *ps32Clsidx, int boxnum, float *piouthresh, int maxDet, int agnostic, int agnostic_offset, int *nmsOut);

/**
 * @brief NMS for YOLOv5 or YOLOv8
 * @param[in] pData, pointer of input candidate BBoxes
 * @param[in] pParam, nms parameter
 * @param[out] pOut, pointer of BBoxes afer nms
 * @return 0:success, -1:fail
 */
int TS_ALG_NMS_Common(float *pData, ALG_NMS_Param_S *pParam, int *pOut);

/**
 * @brief YOLOv5/YOLOv8通用后处理
 * @param[in]  pBlobs, pointer of input BBoxes
 * @param[in]  cfg,  nn config
 * @param[out] result, nn result
 * @return 0:success, -1:fail
 */
int TS_ALG_NN_PostProcess(RNE_BLOBS_S *pBlobs,  ALG_NN_CFG_S *cfg, ALG_NN_RESULT_S *result);

/**
 * @brief 打印NN配置参数
 * @param[in]  cfg,  nn config
 * @return 0:success, -1:fail
 */
int TS_ALG_NN_PrintCfg(ALG_NN_CFG_S *cfg);

/**
 * @brief 加载txt中的NN模型参数
 * @param[in]  filename,  txt filename
 * @param[out] config,  nn config
 * @return 0:success, -1:fail
 */
int TS_ALG_NN_LoadTxtCfg(const char *filename, ALG_NN_CFG_S *config);

/**
 * @brief 加载txt中的NN文件和NN信息
 * @param[in]  filename,  txt filename
 * @param[out] config,  nn file info
 * @return 0:success, -1:fail
 */
int TS_ALG_NN_LoadTxtFileInfo(const char *filename, ALG_NN_FILE_INFO_S *config);

/**
 * @brief 保存NN的输入
 * @param[in]  image,  nn input data
 * @return 0:success, -1:fail
 */
int TS_ALG_NN_DumpInput(ALG_IMAGE_S *image);

/**
 * @brief 保存NN检测的结果
 * @param[in]  image,  nn input data
 * @param[in]  result,  nn propress result
 * @param[in]  config,  nn file info
 * @param[out]  outpath,  save resutl file
 * @return 0:success, -1:fail
 */
int TS_ALG_DrawRectangle_BIN(ALG_IMAGE_S *image, ALG_NN_RESULT_S *result, ALG_NN_FILE_INFO_S *config, const char *outpath);

/**
 * @brief 保存NN输出feature的原始8bit数据
 * @param[in]  pBlobs,  rne forward blobs data
 * @param[in]  dataType,  blobs的数据类型，0:u8, 1:s8
 * @return 0:success, -1:fail
 */
int TS_ALG_NN_DumpBlobs(RNE_BLOBS_S *pBlobs, int dataType);

/**
 * @brief 保存NN输出feature的原始数据
 * @param[in]  pBlobs,  rne forward blobs data
 * @param[in]  debugLayerName,  defautl: "./"
 * @param[in]  outputPath,  defautl: NULL
 * @param[in]  frameIndex,  defautl: 0
 * @param[in]  frameCount,  defautl: 1
 * @return 0:success, -1:fail
 */
TS_S32 TS_ALG_RNE_RecordFile(RNE_BLOBS_S *blobs, TS_CHAR *debugLayerName, TS_CHAR *outputPath, TS_S32 frameIndex, TS_S32 frameCount);

/**
 * @brief 配置RNE是否开启双队列模式，兼容多线程和多进程
 * @param[in]  pModel,  模型结构体句柄
 * @param[in]  expectCoreIdx,  useless, 方式3指定运行在哪个核上
 * @return value core idx, default 0
 */
int TS_ALG_NN_SetRneDualQueue(RNE_NET_S *pModel, int expectCoreIdx);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
