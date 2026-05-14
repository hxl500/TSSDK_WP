#ifndef __CAT_ARRAY_DIFF_H
#define __CAT_ARRAY_DIFF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "ipc_base.h"
#include "video_alg_catdetect-api.h"

#define MAX_CAM_NUM       2
#define MAX_CAT_DET_NUM    32    // 与 stBox 数组上限对齐
#define SHAKE_WINDOW_MS    1000
#define SHAKE_THRESHOLD    5
#define MAX_SHAKE_RECORDS  20

typedef enum {
    SHAKE_EVENT_IDLE    = 0,
    SHAKE_EVENT_STARTED = 1,
    SHAKE_EVENT_ENDED   = 2
} shake_event_state_t;

typedef struct {
    long long timestamp;
    char cat_id[32];
    int cam_id;
} shake_record_t;

typedef struct {
    shake_record_t records[MAX_SHAKE_RECORDS];
    int head;
    int tail;
    int count;
} shake_window_t;

//extern shake_window_t g_shake_windows[MAX_CAM_NUM];
//extern shake_window_t g_eat_shake_windows[MAX_CAM_NUM];

struct cat_in {
    char nameid[32];
    int  act;
    int  act_cat;
    int  act_cat_stable;
    long long lasttime;
    long long lasttimeEat;
    int  cam_id;
    char first_in;
    char cat_first_in;
    char first_eat;
    char cat_first_eat;
    int  event_type;
    int  event_state;
    long long event_start_time;
    long long in_start_time;
    long long eat_start_time;
    int  state;
    int  cat_first_in_count;
    int  cat_first_eat_count;
    int  eat_detect_count;
    long long event_duration;
    long long in_duration;
    long long eat_duration;
    int  deleted;
};

void set_result(ALG_CatDetect_DET_RESULT_S *data);
void set_eta_thres(const float Thres);
void set_out_times(const int times);
void set_picpath_head(char *path);
void set_eat_out_times(const int times);
void set_eat_shake_out_times(const int times);

//void shake_window_init(shake_window_t *window);
// int  shake_window_add(shake_window_t *window, const char *cat_id, int cam_id, long long timestamp);
int shake_window_add(shake_window_t *window, int cam_id, long long now);
// int  shake_window_check_event(shake_window_t *window, const char *cat_id, int cam_id, long long current_time);
int shake_window_check_event(shake_window_t *window, int cam_id, long long now);
void shake_window_end_event(shake_window_t *window);
int  shake_window_get_count(shake_window_t *window, const char *cat_id, int cam_id, long long current_time);
void shake_window_cleanup(shake_window_t *window, long long current_time);

//int  get_cat_duration(const char *cat_id, long long *event_duration, long long *in_duration, long long *eat_duration);

#ifdef __cplusplus
}
#endif

#endif
