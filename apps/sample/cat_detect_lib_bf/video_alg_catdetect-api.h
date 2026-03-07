#ifndef __VIDEO_ALG_BODYDET_API_H__
#define __VIDEO_ALG_BODYDET_API_H__

//#include "sample_alg_cpm.h"
#include "ts_alg_type.h"
#include <stdint.h>
enum{
	ALG_CAT_DET_OK,
	ALG_CAT_DET_INITOK,
	ALG_CAT_DET_BADPIC,
};
enum{
	ALG_CAT_ACT_OUT = 0,
	ALG_CAT_ACT_INT,
	ALG_CAT_ACT_EAT,
	ALG_CAT_ACT_EAT_OUT,
};

enum{
	ALG_CAT_CLASS_ID_FACE = 0,
	ALG_CAT_CLASS_ID_FOOD,
	ALG_CAT_CLASS_ID_HEAD,
	ALG_CAT_CLASS_ID_SIDE,
};

#define MAX_CAT_DET_NUM     25		// max obj detect number

typedef struct tsALG_CatDetect_DET_BOX_S {
    // the range of x and y is 0.0f~1.0f
    TS_FLOAT f32Xmin;
    TS_FLOAT f32Ymin;
    TS_FLOAT f32Xmax;
    TS_FLOAT f32Ymax;
	float DetectionConf; //这个猫的检测框置信度
	float MaxSimilarity; //这个猫与数据库中图片的最大相似度
	char nameid[64];
	int act;//检测出的猫进入事件 结束 进入 进食 进食结束
	int class_id;//ALG_CAT_CLASS_ID 检测出的猫行为 正脸 余粮 头顶 离开
	char first_in;//1表示第一次进入
	char first_eat;//1表示第一次吃东西
	int cam_id;//0表示上方摄像头1，1表示下方摄像头1
	int act_cat;//检测出的猫当前进食事件 结束 进入 进食 进食结束     录像编码使用字段
	char cat_first_in;//1表示第一次进入						 	录像编码使用字段
	char cat_first_eat;//1表示第一次吃东西					 	录像编码使用字段
} ALG_CatDetect_DET_BOX_S;

typedef struct tsALG_CatDetect_DET_RESULT_S {
	TS_U32 u32ObjNum;
	ALG_CatDetect_DET_BOX_S stBox[MAX_CAT_DET_NUM];
} ALG_CatDetect_DET_RESULT_S;
	

typedef struct {
	TS_U8 *yolo_model_cfg;//yolo模型
	TS_U8 *yolo_model_weight; //yolo模型
	TS_U8 *embedding_model_cfg;//embedding模型
	TS_U8 *embedding_model_weight;//embedding模型
	TS_U8 *food_model_cfg;//food模型
	TS_U8 *food_model_weight;//food模型
	TS_U8 *model_version_file;//模型版本信息
	//库状态回调
	int (*cat_callback)(TS_U8 cat_time,void* userdata);
	//用户自定义参数
	void *userdata;
}ALG_CAT_MODEL_INIT_S;


typedef struct tsALG_CatDetect_DET_PARAM_S {
	TS_FLOAT DetectionConfThres; // 猫脸检测框的置信度阈值。值越高，出框更少但更准确。//default 0.4
	TS_FLOAT SimilarityThres_Day; // 猫脸比对相似度的阈值。值越高，识别率更低但更准确。//default 0.825
	TS_FLOAT SimilarityThres_Night; // 猫脸比对相似度的阈值。值越高，识别率更低但更准确。//default 0.6
	TS_FLOAT EAT_Thres;//猫吃食物判断，默认使用0.25，越大，识别率越低.
	int OUT_times;//猫离开的时间,默认设置3s
	int EAT_OUT_times;//猫吃饭结束的时间,默认设置10s
} ALG_CatDetect_DET_PARAM_S;

/**
 * @brief pcpp detect solution initiate
 *
 * @param ppHandle : pcpp handle
 * @param pParam   : input param
 *
 * @attention : If the function fails, you also need to call the release api
 * @return error code
 */
TS_S32 TS_ALG_CatDetect_Init(TS_VOID **pHandle, ALG_CAT_MODEL_INIT_S *param);

/**
 * @brief pcpp detect release memory
 *
 * @param pHandle : pcpp detect handle
 *
 * @return error code
 */
TS_S32 VIDEO_ALG_CatDetect_Exit(TS_VOID *pHandle);


/**
 * @brief pcpp detect process
 *
 * @param pHandle : pcpp detect handle
 * @param pImage  : input yuv image address
 * @param pImageDet  : input rgba image address
 * @param pResult : pcpp detect result
 *
 * @return error code
 */

TS_S32 VIDEO_ALG_CatDetect_Proc(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_IMAGE_S *pImageDet, ALG_CatDetect_DET_RESULT_S *pResult, TS_U8 cam_id);



/**
 * @brief set pcpp detect param
 *
 * @param pParam  : input param
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_CatDetect_SetParam(ALG_CatDetect_DET_PARAM_S *pParam);

/**
 * @brief get pcpp detect param
 *
 * @param pParam  : input param
 *
 * @return 0 or error code
 */
TS_S32 TS_ALG_CatDetect_GetParam(ALG_CatDetect_DET_PARAM_S *pParam);



//return 0 or error code
TS_S32 CatConfigRenew(TS_CHAR*url,TS_CHAR*sn);
TS_S32 CatSetPicDir(TS_CHAR*path);
TS_VOID CatSetGrayMode(TS_CHAR flag);//设置夜间模式,0表示白天，1表示夜间
TS_CHAR CatGetGrayMode(TS_VOID);//获取夜间模式,0表示白天，1表示夜间



//获取运行代码的版本号
TS_CHAR* CatLibVerGet(void);
//获取当前yolo模型的版本号
TS_CHAR* CatYoloModelVerGet(void);
//获取当前embedding模型的版本号
TS_CHAR* CatEmbeddingModelVerGet(void);


#endif
