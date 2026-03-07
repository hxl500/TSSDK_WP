#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#if 1
#include "video_alg_catdetect-api.h"
#include "arrr_diff.h"
#else
#define TS_U32 int
#define TS_FLOAT float
#define MAX_CAT_DET_NUM     25
enum{
	ALG_CAT_ACT_OUT = 0,
	ALG_CAT_ACT_INT,
	ALG_CAT_ACT_EAT,
};
enum{
	ALG_CAT_CLASS_ID_FACE = 0,
	ALG_CAT_CLASS_ID_FOOD,
	ALG_CAT_CLASS_ID_HEAD,
	ALG_CAT_CLASS_ID_SIDE,
};

typedef struct tsALG_CatDetect_DET_BOX_S {
    // the range of x and y is 0.0f~1.0f
    TS_FLOAT f32Xmin;
    TS_FLOAT f32Ymin;
    TS_FLOAT f32Xmax;
    TS_FLOAT f32Ymax;
	float DetectionConf; //这个猫的检测框置信度
	float MaxSimilarity; //这个猫与数据库中图片的最大相似度
	char id[32];//检测出的猫id
	int act;//检测出的猫行为
	int class_id;//检测出的猫行为
	char first_in;
	char first_eat;
} ALG_CatDetect_DET_BOX_S;

typedef struct tsALG_CatDetect_DET_RESULT_S {
	TS_U32 u32ObjNum;
	ALG_CatDetect_DET_BOX_S stBox[MAX_CAT_DET_NUM];
} ALG_CatDetect_DET_RESULT_S;
struct cat_in{
	char id[32];
	int act;
	int lasttime;
	int lasttimeEat;
	char first_in;
	char first_eat;

};
void set_result(ALG_CatDetect_DET_RESULT_S *data);

#endif

static float EAT_Thres= 0.25;
static int OUT_times= 3000;
static int EatOUT_times= 10000;
static int EAT_OUT_SHAKE_times = 10000;
static int has_eat_event = 0;//进食标记
static long long get_current_time_ms()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

struct cat_in allcat[MAX_CAT_DET_NUM];
int cat_num = 0;
void set_eta_thres(const float Thres)
{
	EAT_Thres = Thres;
	return;
}
void set_out_times(const int times)
{
	if(times <= 0){
		OUT_times = 3*1000;
		return;
	}
	OUT_times = times * 1000;
}

void set_eat_out_times(const int times)
{
	if(times <= 0){
		EatOUT_times = 10000;
		return;
	}
	EatOUT_times = times * 1000;
}

void set_eat_shake_out_times(const int times)
{
	if(times <= 0){
		EAT_OUT_SHAKE_times = 10000;
		return;
	}
	EAT_OUT_SHAKE_times = times * 1000;
}

// int is_cat_eat(ALG_CatDetect_DET_BOX_S *data){
// 	if((data->f32Xmax - data->f32Xmin)*(data->f32Ymax- data->f32Ymin) > EAT_Thres){
// 		return ALG_CAT_ACT_EAT;
// 	}
// 	return ALG_CAT_ACT_INT;
// }
int is_cat_eat(ALG_CatDetect_DET_BOX_S *data){
	const float rectCenterY = (data->f32Ymin + data->f32Ymax) * 0.5f;
	//if(0 == data->cam_id && rectCenterY>0.25f && rectCenterY<0.5f){
	if(0 == data->cam_id && (data->f32Xmax - data->f32Xmin)*(data->f32Ymax- data->f32Ymin) > EAT_Thres){
		return ALG_CAT_ACT_EAT;//进食
	}
	return ALG_CAT_ACT_INT;
}
void cat_in_set(ALG_CatDetect_DET_BOX_S *data, long long now, int force_eat_event){
	int i;
	if(cat_num >= MAX_CAT_DET_NUM)
		return;
	
	for(i=0;i<cat_num;i++){
		if(strcmp(allcat[i].nameid,data->nameid) == 0){
			
			if(force_eat_event == 1 || (data->act == ALG_CAT_ACT_EAT && data->cam_id == 0)){
				if(allcat[i].first_eat == 0){
					data->first_eat = 1;
					
					allcat[i].first_eat = 1;
					allcat[i].event_type = 2;
					
					printf("id=%s,first_eat,now=%lld\n",data->nameid,now);
				}
				allcat[i].lasttimeEat = now;
				allcat[i].lasttime = now;
				//allcat[i].act_cat = ALG_CAT_ACT_EAT;
				//data->act_cat = ALG_CAT_ACT_EAT;
				data->act = ALG_CAT_ACT_EAT;
			}else{
				if(allcat[i].first_in == 0 && allcat[i].event_type != 2){
					data->first_in = 1;
					data->cat_first_in = 1;
					allcat[i].first_in = 1;
					allcat[i].event_type = 1;
					
					printf("id=%s,first in,now=%lld\n",data->nameid,now);
				}
				allcat[i].lasttime = now;
				//allcat[i].act_cat = ALG_CAT_ACT_INT;
				//data->act_cat = ALG_CAT_ACT_INT;
				data->act = ALG_CAT_ACT_INT;
			}
			if((data->act_cat == ALG_CAT_ACT_EAT && data->cam_id == 0))
			{
				if(allcat[i].cat_first_eat == 0){
					data->cat_first_eat = 1;
					allcat[i].cat_first_eat = 1;
					printf("id=%s,cat_first_eat,now=%lld\n",data->nameid,now);
				}
				
			}
			else
			{
				if(allcat[i].cat_first_in == 0){
					data->cat_first_in = 1;
					allcat[i].cat_first_in = 1;
					printf("id=%s,cat_first_in,now=%lld\n",data->nameid,now);
				}
			}
			return;		
		}
	}
	strcpy(allcat[cat_num].nameid,data->nameid);
	allcat[cat_num].act = data->act;
	//allcat[cat_num].act_cat = data->act;
	allcat[cat_num].lasttime = now;
	allcat[cat_num].first_in = 1;
	allcat[cat_num].cat_first_in = 1;
	data->first_in = 1;
	data->cat_first_in = 1;
	allcat[cat_num].event_type = 1;
	printf("id=%s,first in,now=%lld\n",data->nameid,now);
	
	if(force_eat_event == 1 || (data->act == ALG_CAT_ACT_EAT && data->cam_id == 0)){
		allcat[cat_num].first_eat = 1;
		//allcat[cat_num].cat_first_eat = 1;
		data->first_eat = 1;
		data->cat_first_eat = 1;
		allcat[cat_num].lasttimeEat = now;
		allcat[cat_num].event_type = 2;
		//allcat[cat_num].act_cat = ALG_CAT_ACT_EAT;
		allcat[cat_num].act = ALG_CAT_ACT_EAT;
		//data->act_cat = ALG_CAT_ACT_EAT;
		data->act = ALG_CAT_ACT_EAT;
		printf("id=%s,first_eat in,now=%lld\n",data->nameid,now);
	}
	
	cat_num++;

	return;
}

void is_cat_out(ALG_CatDetect_DET_RESULT_S *data, long long now){
	int i = 0;
	if(data->u32ObjNum >= MAX_CAT_DET_NUM-1)
		return;
	for(i=0;i<cat_num;i++){
		if(data->u32ObjNum >= MAX_CAT_DET_NUM - 1)
			return;
		
		if(allcat[i].event_type == 2){
			if(now - allcat[i].lasttimeEat > EatOUT_times){
				memset(&data->stBox[data->u32ObjNum],0,sizeof(ALG_CatDetect_DET_BOX_S));
				strcpy(data->stBox[data->u32ObjNum].nameid,allcat[i].nameid);
				data->stBox[data->u32ObjNum].act = ALG_CAT_ACT_EAT_OUT;
				data->stBox[data->u32ObjNum].class_id = ALG_CAT_CLASS_ID_FACE;
				data->stBox[data->u32ObjNum].f32Xmin = 0;
				data->stBox[data->u32ObjNum].f32Ymin = 0;
				data->stBox[data->u32ObjNum].f32Xmax = 0;
				data->stBox[data->u32ObjNum].f32Ymax = 0;
				printf("id=%s,eat out,now=%lld,EatOUT_times=%d\n",allcat[i].nameid,now,EatOUT_times);
				allcat[i].first_eat = 0;
				allcat[i].event_type = 0;
				data->u32ObjNum++;
				has_eat_event = 0;
			}
			data->stBox[data->u32ObjNum].act_cat = ALG_CAT_ACT_EAT_OUT;
			allcat[i].cat_first_eat = 0;
			allcat[i].act_cat = ALG_CAT_ACT_OUT;

		}else if(allcat[i].event_type == 1){
			if(now - allcat[i].lasttime > OUT_times){
				memset(&data->stBox[data->u32ObjNum],0,sizeof(ALG_CatDetect_DET_BOX_S));
				strcpy(data->stBox[data->u32ObjNum].nameid,allcat[i].nameid);
				data->stBox[data->u32ObjNum].act = ALG_CAT_ACT_OUT;
				data->stBox[data->u32ObjNum].class_id = ALG_CAT_CLASS_ID_FACE;
				data->stBox[data->u32ObjNum].f32Xmin = 0;
				data->stBox[data->u32ObjNum].f32Ymin = 0;
				data->stBox[data->u32ObjNum].f32Xmax = 0;
				data->stBox[data->u32ObjNum].f32Ymax = 0;
				printf("id=%s,out,now=%lld,OUT_times=%d\n",allcat[i].nameid,now,OUT_times);
				allcat[i].first_in = 0;
				allcat[i].event_type = 0;
				data->u32ObjNum++;
			}
			data->stBox[data->u32ObjNum].act_cat = ALG_CAT_ACT_OUT;
			allcat[i].cat_first_in = 0;
			allcat[i].act_cat = ALG_CAT_ACT_OUT;
		}
		else
		{

		}
	}
	
	for(i=0;i<cat_num;i++){
		if(allcat[i].event_type == 0){
			if(i < cat_num - 1){
				memcpy(&allcat[i],&allcat[i+1],(cat_num-1 -i)*sizeof(struct cat_in));			
			}
			i--;
			cat_num -= 1;	
		}		
	}

	return;
}


void print_result(ALG_CatDetect_DET_RESULT_S *data){
	int i;
	if(data->u32ObjNum > 0){
		printf("u32ObjNum=%d\n",data->u32ObjNum);
	}
	else
	{
		 //printf("no cat\n");
		// data->stBox[data->u32ObjNum].DetectionConf = 0;
		// data->stBox[data->u32ObjNum].f32Xmin = 0;
		// data->stBox[data->u32ObjNum].f32Ymin = 0;
		// data->stBox[data->u32ObjNum].f32Xmax = 0;
		// data->stBox[data->u32ObjNum].f32Ymax = 0;
		// printf("id=%d,class_id=%d,act=%d,first_in=%d,first_eat=%d,DetectionConf=%f,MaxSimilarity=%f,cam_id=%d,point=%f,%f,%f,%f\n",
		// 	data->stBox[data->u32ObjNum].nameid,
		// 	data->stBox[data->u32ObjNum].class_id,
		// 	data->stBox[data->u32ObjNum].act,
		// 	data->stBox[data->u32ObjNum].first_in,
		// 	data->stBox[data->u32ObjNum].first_eat,
		// 	data->stBox[data->u32ObjNum].DetectionConf,
		// 	data->stBox[data->u32ObjNum].MaxSimilarity,
		// 	data->stBox[data->u32ObjNum].cam_id,
		// 	data->stBox[data->u32ObjNum].f32Xmin,
		// 	data->stBox[data->u32ObjNum].f32Ymin,
		// 	data->stBox[data->u32ObjNum].f32Xmax,
		// 	data->stBox[data->u32ObjNum].f32Ymax);
		// printf("id=%s,class_id=%d,act=%d,act_cat=%d,first_in=%d,cat_first_in=%d,first_eat=%d,cat_first_eat=%d,DetectionConf=%f,MaxSimilarity=%f,cam_id=%d,point=%f,%f,%f,%f\n",
		// 	data->stBox[data->u32ObjNum].nameid,
		// 	data->stBox[data->u32ObjNum].class_id,
		// 	data->stBox[data->u32ObjNum].act,
		// 	data->stBox[data->u32ObjNum].act_cat,
		// 	data->stBox[data->u32ObjNum].first_in,
		// 	data->stBox[data->u32ObjNum].cat_first_in,
		// 	data->stBox[data->u32ObjNum].first_eat,
		// 	data->stBox[data->u32ObjNum].cat_first_eat,
		// 	data->stBox[data->u32ObjNum].DetectionConf,
		// 	data->stBox[data->u32ObjNum].MaxSimilarity,
		// 	data->stBox[data->u32ObjNum].cam_id,
		// 	data->stBox[data->u32ObjNum].f32Xmin,
		// 	data->stBox[data->u32ObjNum].f32Ymin,
		// 	data->stBox[data->u32ObjNum].f32Xmax,
		// 	data->stBox[data->u32ObjNum].f32Ymax);
	}
	
	for(i=0;i<data->u32ObjNum;i++){
		printf("id=%s,class_id=%d,act=%d,act_cat=%d,first_in=%d,cat_first_in=%d,first_eat=%d,cat_first_eat=%d,DetectionConf=%f,MaxSimilarity=%f,cam_id=%d,point=%f,%f,%f,%f\n",
			data->stBox[i].nameid,
			data->stBox[i].class_id,
			data->stBox[i].act,
			data->stBox[i].act_cat,
			data->stBox[i].first_in,
			data->stBox[i].cat_first_in,
			data->stBox[i].first_eat,
			data->stBox[i].cat_first_eat,
			data->stBox[i].DetectionConf,
			data->stBox[i].MaxSimilarity,
			data->stBox[i].cam_id,
			data->stBox[i].f32Xmin,
			data->stBox[i].f32Ymin,
			data->stBox[i].f32Xmax,
			data->stBox[i].f32Ymax);
	}
}
/*处理检测结果并生成事件*/

void set_result(ALG_CatDetect_DET_RESULT_S *data){
	int i;
	long long now = get_current_time_ms();
	int original_u32ObjNum = data->u32ObjNum;

	for(i=0;i<original_u32ObjNum;i++){
		if(data->stBox[i].class_id != ALG_CAT_CLASS_ID_FOOD){
			data->stBox[i].act = is_cat_eat(&data->stBox[i]);
			data->stBox[i].act_cat = data->stBox[i].act;
			allcat[i].act_cat = data->stBox[i].act_cat;
			printf("id=%s,act=%d\n",data->stBox[i].nameid,data->stBox[i].act);
			data->stBox[i].first_in = 0;
			data->stBox[i].cat_first_in = 0;
			data->stBox[i].first_eat = 0;
			data->stBox[i].cat_first_eat = 0;
			
			if(data->stBox[i].act == ALG_CAT_ACT_EAT && data->stBox[i].cam_id == 0){
				has_eat_event = 1;
			}
		}else{
			data->stBox[i].act = ALG_CAT_ACT_INT;
			data->stBox[i].act_cat = ALG_CAT_ACT_INT;
			data->stBox[i].first_in = 0;
			data->stBox[i].cat_first_in = 0;
			data->stBox[i].first_eat = 0;
			data->stBox[i].cat_first_eat = 0;
		}
	}
	for(i=0;i<original_u32ObjNum;i++){
		if(data->stBox[i].class_id != ALG_CAT_CLASS_ID_FOOD){
			cat_in_set(&data->stBox[i],now,has_eat_event);	
		}
	}
	
	is_cat_out(data,now);

	print_result(data);
	return;
}
#if 0
int main() {
	ALG_CatDetect_DET_RESULT_S data;
	set_eta_thres(0.25);
	set_out_times(5);
	set_eat_out_times(10);
	set_eat_shake_out_times(10);

	data.u32ObjNum= 1;
	strcpy(data.stBox[0].id,"1");
	data.stBox[0].f32Xmin = 0;
	data.stBox[0].f32Xmax = 1;
	data.stBox[0].f32Ymin = 0;
	data.stBox[0].f32Ymax = 1;
	data.stBox[0].class_id = ALG_CAT_CLASS_ID_FACE;
    set_result(&data);
	sleep(10);
	data.u32ObjNum= 1;
	strcpy(data.stBox[0].id,"2");
	data.stBox[0].f32Xmin = 0;
	data.stBox[0].f32Xmax = 0;
	data.stBox[0].f32Ymin = 0;
	data.stBox[0].f32Ymax = 0;
	data.stBox[0].class_id = ALG_CAT_CLASS_ID_FACE;
    set_result(&data);
	sleep(2);
	data.u32ObjNum= 1;
	strcpy(data.stBox[0].id,"1");
	data.stBox[0].f32Xmin = 0;
	data.stBox[0].f32Xmax = 0;
	data.stBox[0].f32Ymin = 0;
	data.stBox[0].f32Ymax = 0;
	data.stBox[0].class_id = ALG_CAT_CLASS_ID_FACE;
    set_result(&data);

	sleep(2);

	data.u32ObjNum= 1;
	strcpy(data.stBox[0].id,"2");
	data.stBox[0].f32Xmin = 0;
	data.stBox[0].f32Xmax = 1;
	data.stBox[0].f32Ymin = 0;
	data.stBox[0].f32Ymax = 1;	
	data.stBox[0].class_id = ALG_CAT_CLASS_ID_FACE;
	set_result(&data);
	while(1){
		data.u32ObjNum= 0;
		strcpy(data.stBox[0].id,"2");
		strcpy(data.stBox[0].id,"2");
		data.stBox[0].f32Xmin = 0;
		data.stBox[0].f32Xmax = 1;
		data.stBox[0].f32Ymin = 0;
		data.stBox[0].f32Ymax = 1;	
		set_result(&data);
		sleep(1);

	}

    return 0;
}
#endif
