/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: base type definitions of alg
 * Author: Tsing Micro solution-application group
 * Create: 2024/08/05
 */

#ifndef _TS_ALG_RW_H_
#define _TS_ALG_RW_H_

#include <stdio.h>
#include "ts_alg_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * @brief 加载输入数据
 * @param[in] pSrc, 输入rgb数据
 * @param[out] pDst, 输出rgba数据
 * @param[in] imgW, 输入数据宽度
 * @param[in] imgH, 输入数据高度
 * @param[in] imgH, 输入数据通道数
 * @return 0:success, -1:fail
 */
TS_S32 TS_ALG_HWC3ToHWC4(TS_U8 *pSrc, TS_U8 *pDst, TS_U32 imgW, TS_U32 imgH);

/**
 * @brief 加载输入数据
 * @param[in] pfile, 输入文件路径
 * @param[in] imgW, 输入数据宽度
 * @param[in] imgH, 输入数据高度
 * @param[in] imgH, 输入数据通道数
 * @param[in] pHeadData, 输入head文件的数组 【 pfile为NULL时使用 】
 * @param[in] pHeadData, head数组大小
 * @param[out] pInput, 输出加载的输入数据指针的指针
 * @return 0:success, -1:fail
 */
TS_S32 TS_ALG_LoadInputZ(char *pfile, TS_U32 imgW, TS_U32 imgH, TS_U32 imgC,  TS_U8 *pHeadData, TS_U32 headSize, TS_U8 **pInput);

/**
 * @brief 加载输入数据
 * @param[in] argc, main函数输入参数数量
 * @param[in] argv, main函数输入参数
 * @param[in] pHeadData, 十六进制头文件格式的输入数据指针
 * @param[in] dataSize, 输入数据大小[w*h*c]
 * @param[out] pInput, 输出加载的输入数据指针
 * @return 0:success, -1:fail
 */
TS_S32 TS_ALG_LoadInputExt(int argc, char **argv, TS_U8 *pHeadData, TS_UL dataSize, TS_U8 *pInput);


/**
 * @brief 加载2个输入数据
 * @param[in] argc, main函数输入参数数量
 * @param[in] argv, main函数输入参数
 * @param[in] pHeadData1, 十六进制头文件格式的输入数据指针
 * @param[in] dataSize1, 输入数据大小[w*h*c]
 * @param[out] pInput1, 第一输出加载的输入数据指针
 * @param[in] pHeadData2, 十六进制头文件格式的输入数据指针
 * @param[in] dataSize2, 输入数据大小[w*h*c]
 * @param[out] pInput2, 第二输出加载的输入数据指针
 * @return 0:success, -1:fail
 */
TS_S32 TS_ALG_LoadDualInput(int argc, char **argv, TS_U8 *pHeadData1, TS_UL dataSize1, TS_U8 *pInput1,
							TS_U8 *pHeadData2, TS_UL dataSize2, TS_U8 *pInput2);

/**
 * @brief 连续加载多帧输入数据
 * @param[in] argc, main函数输入参数数量
 * @param[in] argv, main函数输入参数
 * @param[in] pHeadData, 十六进制头文件格式的输入数据指针
 * @param[in] dataSize, 输入数据大小[w*h*c]
 * @param[in] enLoad, 使能加载
 * @param[out] pInput, 输出加载的输入数据指针
 * @return 0:success, -1:fail
 */
TS_S32 TS_ALG_LoadMultiInput(int argc, char **argv, TS_U8 *pHeadData, TS_UL dataSize, TS_U8 enLoad, TS_U8 *pInput);

TS_UL TS_ALG_GetFileSize(FILE *pf);

/**
 * @brief 加载模型配置和权重
 * @param[in] pCfgPath, 模型配置文件路径
 * @param[in] pWeightPath, 模型权重文件路径
 * @param[out] pParam, 模型初始化结构体
 * @return 0:success, -1:fail
 */
TS_S32 TS_ALG_LoadCfgWeightOnce(const TS_CHAR* pCfgPath,const TS_CHAR* pWeightPath, ALG_MODEL_INIT_S *pParam);
TS_S32 TS_ALG_LoadCfgWeightFile(const TS_CHAR* pCfgPath,const TS_CHAR* pWeightPath, TS_U8** pCfgBuf, TS_U8** pWeightBuf, TS_UL* cfgSize, TS_UL* weightSize);
TS_S32 TS_ALG_FreeCfgWeightOnce(ALG_MODEL_INIT_S *pParam);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

