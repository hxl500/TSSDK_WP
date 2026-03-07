#ifndef __CAT_ARRAY_DIFF_H
#define __CAT_ARRAY_DIFF_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdint.h>
#include "ipc_base.h"
#include "video_alg_catdetect-api.h"
int  get_catName_id(float *src,char *str);
int  change_catName_info(struct cat_data*data,int datanum, struct catNameInfo *cat_nameinfo,int namenum);
void normalize_array(float *arr);
int  change_catName_info2(struct catPicInfo*data,int datanum, struct catNameInfo *cat_nameinfo,int namenum);
int get_catName_name(int id, char*name);
float get_simi();
struct cat_in{
	char nameid[64];
	int act;
	long long lasttime;
	long long lasttimeEat;
	char first_in;
	char first_eat;
	char event_type;  // 0: 无事件, 1: 进入事件, 2: 进食事件
	int act_cat;//检测出的猫当前进食事件 结束 进入 进食 进食结束     录像编码使用字段
	char cat_first_in;//1表示第一次进入						 	录像编码使用字段
	char cat_first_eat;//1表示第一次吃东西					 	录像编码使用字段
};
void set_result(ALG_CatDetect_DET_RESULT_S *data);
void set_eta_thres(const float Thres);
void set_out_times(const int times);
void set_picpath_head(char *path);

void set_eat_out_times(const int times);
void set_eat_shake_out_times(const int times);

#ifdef __cplusplus
}
#endif

#endif

