/**
 * @file ts_alg_pts.h
 * @author yhc
 * @brief passenger traffic statistics 客流量统计
 * @version 1.0
 * @date 2023-11-28
 * @remark 客流量统计中增加了基线(左边点坐标和右边点坐标)和感兴趣区域(左上角，右下角)，
 * 			基线是当人型检测框的框底部中心点进入时，算法会认为人是进入了(从上到下)
 * 			基线是当人型检测框的框底部中心点离开时，算法会认为人是出去了（从下到上）
 * 			感兴趣区域就是客流统计时关注的区域
 * 			1.对一张图处理时先进行人型检测，检测出人型框的位置，然后带入到轨迹跟踪中进行轨迹跟踪处理
 * 			2.对轨迹跟踪后的结果进行客流统计算法的检测，判断该图中人的活动方向和记录轨迹点
 *
 * @copyright Copyright (c) 2022 tsingmicro
 *
 */
#ifndef _TS_ALG_PTS_H_
#define _TS_ALG_PTS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define MAX_PTS_NUM     40		// max face detect number

typedef struct tsALG_PTS_BOX_S{
	TS_U32	u32TrackId;
    // the range of x and y is 0.0f~1.0f
    TS_U32 u32X;
    TS_U32 u32Y;
    TS_U32 u32Width;
    TS_U32 u32Height;
    TS_FLOAT f32Score;
    TS_FLOAT f32Reserve;
} ALG_PTS_BOX_S;

typedef struct tsALG_PTS_S{
	TS_U32 u32TrackNum;
	ALG_PTS_BOX_S stBox[MAX_PTS_NUM];
} ALG_PTS_S;

typedef struct tsALG_PTS_INPUT_S {
	TS_U32 baseLine[4];		// x1,y1,x2,y2 723, 513,1781, 403
	TS_U32 focuseRect[4];	// x1,y1,w,h  725, 101, 1749, 651
} ALG_PTS_INPUT_S;

typedef struct tsALG_PTS_RESULTS_S {
	TS_U32 u32Num;
	ALG_PTS_S ptsRes;
	TS_U32 u32InNum;
	TS_U32 u32OutNum;
} ALG_PTS_RESULTS_S;

typedef struct tsALG_PTS_PARAM_S {
	TS_U32 baseLine[4];		// x1,y1,x2,y2 723, 513,1781, 403
	TS_U32 focuseRect[4];	// x1,y1,w,h  725, 101, 1749, 651
	TS_FLOAT f32TrackThresh;
	//TS_FLOAT hightThresh;
	TS_FLOAT f32MatchThresh;
	TS_U32 u32MaxTimeLoss;
} ALG_PTS_PARAM_S;


/**
 * @brief passenger traffic statistics init
 *
 * @param ppHandle : algo handle
 * @param pParam   : input param
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code
 */
TS_S32 TS_ALG_PTS_Init(TS_VOID **ppHandle, ALG_PTS_INPUT_S *pParam);

/**
 * @brief passenger traffic statistics process
 *
 * @param pHandle   : Net handle
 * @param pImage    : input src image
 * @param pImageDet : input detect image
 * @param pResult   : passenger traffic statistics result
 *
 * @return  0 or error code
 */
TS_S32 TS_ALG_PTS_Process(TS_VOID *pHandle, ALG_PTS_S *pInput, ALG_PTS_RESULTS_S *pResult);

/**
 * @brief passenger traffic statistics exit
 *
 * @param pHandle   : Net handle
 *
 * @return  0 or error code
 */
TS_S32 TS_ALG_PTS_Exit(TS_VOID *pHandle);

/**
 * @brief set passenger traffic statistics param
 *
 * @param pHandle : Net handle
 * @param pParam  : input param
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_PTS_SetParam(TS_VOID *pHandle, ALG_PTS_PARAM_S *pParam);

/**
 * @brief get passenger traffic statistics param
 *
 * @param handle : Net handle
 * @param pParam  : input param
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_PTS_GetParam(TS_VOID *pHandle, ALG_PTS_PARAM_S *pParam);

/**
 * @brief passenger traffic statistics get version
 *
 * @return version string
 */
const TS_CHAR* TS_ALG_PTS_GetVersion();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
