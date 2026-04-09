#ifndef TSALG_ALG_LIB_H
#define TSALG_ALG_LIB_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include "ts_alg_type.h"

#include "soft_line.h"
int init_my_crop_resize_detect(TS_U32 widthMax, TS_U32 heightMax);
char my_crop_resize_detect(ALG_IMAGE_S *srcImg, RECT *prect,char *idstr);
char my_crop_resize_detect_food(ALG_IMAGE_S *srcImg, RECT *prect,char *idstr);

TS_S32 ALG_RSN_Detect_Init();
TS_S32 ALG_RSN_Detect_Process(ALG_IMAGE_S *image, TS_FLOAT rsn_result[]);
TS_S32 ALG_RSN_Detect_FOOD_Init();
TS_S32 ALG_FOOD_RSN_Detect_Process(ALG_IMAGE_S *image, TS_FLOAT rsn_result[]);
int test_crop_resize_detect();
int rsn_detect_file(char *src_file,int *result);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
