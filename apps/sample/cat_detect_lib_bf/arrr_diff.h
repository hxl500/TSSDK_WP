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
	int lasttime;
	int lasttimeEat;
	char first_in;
	char first_eat;
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

