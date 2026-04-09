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
    TS_FLOAT f32Xmin;        // 检测框左上角X坐标 (0.0f~1.0f 归一化)
    TS_FLOAT f32Ymin;        // 检测框左上角Y坐标 (0.0f~1.0f 归一化)
    TS_FLOAT f32Xmax;        // 检测框右下角X坐标 (0.0f~1.0f 归一化)
    TS_FLOAT f32Ymax;        // 检测框右下角Y坐标 (0.0f~1.0f 归一化)
	float DetectionConf;     // 猫脸检测置信度 (0.0~1.0)
	float MaxSimilarity;     // 与数据库图片的最大相似度 (0.0~1.0)
	char nameid[64];         // 猫咪身份标识 (由embedding模型识别)
	int act;                 // 当前行为动作 (ALG_CAT_ACT_OUT/INT/EAT/EAT_OUT)
	int class_id;            // 检测目标类别 (ALG_CAT_CLASS_ID_FACE/FOOD/HEAD/SIDE)
	char first_in;           // 1=首次触发进入事件 (本次检测周期内)
	char first_eat;          // 1=首次触发进食事件 (本次检测周期内)
	int cam_id;              // 摄像头ID (0=上方摄像头, 1=下方摄像头)
	int act_cat;             // 猫的当前进食状态 (录像编码使用) 每一帧的实时事件状态
	int act_cat_stable;      // 防抖后的稳定状态 (防抖事件内真的实际状态)
	char cat_first_in;      // 1=首次进入标志 (录像编码使用)
	char cat_first_eat;      // 1=首次进食标志 (录像编码使用)
	int state;               // 猫咪状态机 (0=ABSENT, 1=INCOMING, 2=IN_DONE, 3=EATING, 4=EAT_DONE, 5=OUT)//防抖下的实时状态
	int event_type;          // 当前事件类型 (0=NONE, 1=IN, 2=EAT)
	int cat_first_in_count;  // 累计首次进入触发次数 (cat_first_in=1时累加)
	int cat_first_eat_count; // 累计首次进食触发次数 (cat_first_eat=1时累加)
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

// /**
//  * @brief 垂直拼接NV12格式的视频帧
//  *
//  * @param Src_NV12_Top  顶部视频帧的NV12数据指针
//  * @param Src_NV12_Bottom 底部视频帧的NV12数据指针
//  * @param Dst_NV12  输出拼接后的NV12数据指针
//  * @param Width  视频帧的宽度
//  * @param Height 视频帧的高度
//  * @param StrideY  Y平面的行 stride
//  * @param StrideUV UV平面的行 stride
//  *
//  * @return 0 成功 -1 失败
//  */
// TS_S32 TS_NV12_Vertical_Concat_Correct(const TS_U8 *Src_NV12_Top,
//                                  const TS_U8 *Src_NV12_Bottom,
//                                  TS_U8 *Dst_NV12,
//                                  TS_S32 Width, TS_S32 Height,
//                                  TS_S32 StrideY, TS_S32 StrideUV);

// /**
//  * @brief 缩放NV12格式的视频帧
//  *
//  * @param Src  输入视频帧的NV12数据指针
//  * @param Src_Width  输入视频帧的宽度
//  * @param Src_Height 输入视频帧的高度
//  * @param Dst  输出缩放后的NV12数据指针
//  * @param Dst_Width  输出视频帧的宽度
//  * @param Dst_Height 输出视频帧的高度
//  * @param Keep_Aspect  是否保持宽高比，1表示保持，0表示不保持
//  *
//  * @return 0 成功 -1 失败
//  */
// TS_S32 TS_NV12_Scale_Ex(TS_U8 *Src, TS_S32 Src_Width, TS_S32 Src_Height,
//                    TS_U8* Dst, TS_S32 Dst_Width, TS_S32 Dst_Height,
//                   TS_S32 Keep_Aspect);

// // int nv12_scale_fit_fast(const uint8_t *src_nv12,
// //                          int src_w, int src_h, int src_stride,
// //                          uint8_t *dst_nv12,
// //                          int dst_w, int dst_h);
// TS_S32 TS_NV12_Scale_Fit_Fast(TS_U8 *Src_NV12, TS_S32 Src_Width, TS_S32 Src_Height,TS_S32 Src_Stride,
//                    TS_U8* Dst_NV12, TS_S32 Dst_Width, TS_S32 Dst_Height);

#endif
