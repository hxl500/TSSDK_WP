#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <math.h>
#include <signal.h>

#include "video_alg_catdetect-api.h"
#include "mpi_ae.h"
#include "mpi_vi.h"

// 调试开关
#define CAT_MEM_DEBUG         1
// 没有传入猫ID时使用的默认名称
#define DEFAULT_CAT_NAME      "default_cat"

// 段错误调试日志开关
#define SEG_DEBUG             0

// 对齐检查宏
#define IS_ALIGNED(ptr, alignment) (((uintptr_t)(ptr) & ((alignment) - 1)) == 0)

#if SEG_DEBUG
#include <pthread.h>
#include <stdint.h>
#include <string.h>
#define SEG_LOG(fmt, ...) do { \
    char _seg_buf[512]; \
    int _seg_len = snprintf(_seg_buf, sizeof(_seg_buf), "[SEG_DBG][%s:%d][tid=%lu] " fmt "\n", \
            __func__, __LINE__, (unsigned long)pthread_self(), ##__VA_ARGS__); \
    if (_seg_len > 0 && _seg_len < (int)sizeof(_seg_buf)) { \
        write(STDERR_FILENO, _seg_buf, _seg_len); \
    } \
} while(0)
#define SEG_CHECK_ALIGN(ptr, alignment) do { \
    if (!IS_ALIGNED(ptr, alignment)) { \
        char _align_buf[128]; \
        int _align_len = snprintf(_align_buf, sizeof(_align_buf), \
            "[SEG_DBG][%s:%d] ALIGNMENT ERROR: %p not aligned to %d\n", \
            __func__, __LINE__, (void*)(ptr), (alignment)); \
        if (_align_len > 0 && _align_len < (int)sizeof(_align_buf)) { \
            write(STDERR_FILENO, _align_buf, _align_len); \
        } \
    } \
} while(0)
#else
#define SEG_LOG(fmt, ...) ((void)0)
#define SEG_CHECK_ALIGN(ptr, alignment) ((void)0)
#endif

// 单次检测最大返回目标框数量，防止数组越界
#ifndef MAX_RES_BOX_NUM
#define MAX_RES_BOX_NUM       64
#endif

// 系统最大支持摄像头路数
#ifndef MAX_CAM_NUM
#define MAX_CAM_NUM           2
#endif

// 最大同时跟踪的猫数量
#ifndef MAX_CAT_DET_NUM
#define MAX_CAT_DET_NUM       16
#endif

// 事件触发滑动窗口：1秒内连续5帧满足才判定事件有效
#define IN_WINDOW_MS          1000
#define IN_TRIGGER_FRAME      5

// 防抖滑动窗口：1秒内检测帧数阈值
#define STABLE_WINDOW_MS      1000
#define STABLE_THRESHOLD      5

// cam1 进食事件严格判定参数
#define CAM1_EAT_AREA_THRES       0.25f
#define CAM1_EAT_YMIN_THRES       0.35f
#define CAM1_EAT_YMAX_THRES       0.70f
#define CAM1_EAT_TRIGGER_FRAME    7
#define CAM1_EAT_WINDOW_MS        1500

// 事件超时消失时间
// 进入事件超时3s未出现则认为离开
#define IN_OUT_TIMEOUT_MS     3000
// 进食事件超时10s未出现则认为进食结束
#define EAT_OUT_TIMEOUT_MS    10000

// 全局参数
static float EAT_Thres            = 0.125f;
static int OUT_times             = 3000;    // 进入超时（ms）
static int EatOUT_times          = 10000;   // 进食超时（ms）
// 参数设置
/*
 * 设置进食面积阈值
 * 阈值越大，判定进食需要的检测框越大
 */
void set_eta_thres(const float Thres)    { EAT_Thres = Thres; }
void set_out_times(const int times)      { OUT_times = (times <= 0) ? 3000 : times * 1000; }
void set_eat_out_times(const int times)  { EatOUT_times = (times <= 0) ? 10000 : times * 1000; }


/*
 * 猫跟踪状态结构体
 * 每一只猫独立一个结构体，保存全程状态
 * 注意：ARM架构要求long long类型8字节对齐，使用aligned属性确保对齐
 */
typedef struct __attribute__((aligned(8))) {
    long long in_times[MAX_CAM_NUM][IN_TRIGGER_FRAME];  // 每个摄像头独立的进入事件滑动窗口
    long long eat_times[IN_TRIGGER_FRAME]; // 进食事件滑动窗口：保存最近5帧时间戳
    long long stable_times[MAX_CAM_NUM][IN_TRIGGER_FRAME]; // 每个摄像头独立的防抖滑动窗口
    long long last_seen[MAX_CAM_NUM];      // 每一路摄像头最后一次出现时间
    long long in_start_time;               // 进入事件开始时间戳
    long long event_start_time;            // 整个事件起始时间
    long long eat_start_time;              // 进食事件开始时间戳
    long long event_duration;              // 事件总时长（ms）
    long long in_duration;                 // 进入行为时长
    long long eat_duration;                // 进食行为时长
    long long last_in_time;                // 最近一次进入行为的时间
    long long last_eat_time;               // 最近一次进食行为的时间

    char     nameid[64];                   // 猫的唯一ID字符串
    int      cam_mask;                     // 哪些摄像头出现过这只猫，bit位标记
    int      in_cnt[MAX_CAM_NUM];          // 每个摄像头独立的进入事件累计帧数
    int      eat_cnt;                      // 进食事件累计帧数
    int      stable_cnt[MAX_CAM_NUM];      // 每个摄像头独立的防抖滑动窗口累计帧数
    int      event_type;                   // 当前事件类型：0=无 1=进入 2=进食
    int      sta;                          // 内部状态机编号
    int      act_sta;                      // 稳定输出行为，防止帧间抖动
    int      f_in;                         // 是否是进入事件第一帧（只置1一次）
    int      f_eat;                        // 是否是进食事件第一帧（只置1一次）
    int      in_event_started;             // 进入事件是否已经触发完成
    int      eat_event_started;            // 进食事件是否已经触发完成
    int      cnt_in[MAX_CAM_NUM];          // 每个摄像头的进入事件帧数累加
    int      cnt_eat[MAX_CAM_NUM];         // 每个摄像头的进食事件帧数累加
    int      eat_miss_count;               // 进食漏检连续帧数，用于防抖
    int      deleted;                      // 该猫节点是否已删除（可复用标记）
} cat_t;

// 确保全局数组也8字节对齐
static cat_t g_cats[MAX_CAT_DET_NUM] __attribute__((aligned(8)));
// 当前已经跟踪的猫数量（原子变量，多线程安全）
static atomic_int g_cat_count = ATOMIC_VAR_INIT(0);
// 模块初始化标记
static atomic_bool g_initialized = ATOMIC_VAR_INIT(false);
// 进食判定：检测框面积阈值
//static float EAT_Thres = 0.125f;

// 段错误信号处理函数（嵌入式系统兼容）
static void segfault_handler(int sig) {
    const char msg[] = "\n[SEGFAULT] Error: signal ";
    write(STDERR_FILENO, msg, sizeof(msg) - 1);
    
    char sig_buf[16];
    int len = snprintf(sig_buf, sizeof(sig_buf), "%d\n", sig);
    write(STDERR_FILENO, sig_buf, len);
    
    // 打印关键调试信息
    char debug_buf[256];
    len = snprintf(debug_buf, sizeof(debug_buf), 
        "[SEGFAULT] g_cats addr: %p, size: %zu\n", 
        (void*)g_cats, sizeof(g_cats));
    write(STDERR_FILENO, debug_buf, len);
    
    // 尝试打印寄存器信息（ARM）
#if defined(__arm__)
    register unsigned long sp __asm__("sp");
    register unsigned long lr __asm__("lr");
    len = snprintf(debug_buf, sizeof(debug_buf), 
        "[SEGFAULT] SP: 0x%lx, LR: 0x%lx\n", sp, lr);
    write(STDERR_FILENO, debug_buf, len);
#endif
    
    signal(sig, SIG_DFL);
    raise(sig);
}

// 注册段错误处理函数
__attribute__((constructor))
static void init_segfault_handler(void) {
    signal(SIGSEGV, segfault_handler);
    signal(SIGBUS, segfault_handler);
    signal(SIGABRT, segfault_handler);
}



/*
 * 获取系统当前时间戳，单位 ms
 */
static long long get_current_time_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

// ============================================================
// ISP目标过近检测模块
// 原理：当猫靠近食盆进食时，身体遮挡摄像头导致YOLOv5检测不到
//       此时ISP会自动增加曝光时间和增益来补偿亮度下降
//       通过监测ISP曝光参数的突变来判断目标过近
// ============================================================

#define ISP_CLOSE_DETECT_ENABLE     0

#define ISP_CLOSE_LUM_DROP_RATIO    0.15f
#define ISP_CLOSE_GAIN_RISE_RATIO   1.3f
#define ISP_CLOSE_EXP_RISE_RATIO    1.2f
#define ISP_CLOSE_BLOCKED_LUM_RATIO 0.10f
#define ISP_CLOSE_CONFIRM_FRAMES    3
#define ISP_CLOSE_BASELINE_FRAMES   5
#define ISP_HISTORY_SIZE            16
#define ISP_MAX_FAIL_COUNT          5

#define ISP_NIGHT_LUM_DROP_RATIO    0.25f
#define ISP_NIGHT_GAIN_RISE_RATIO   1.5f
#define ISP_NIGHT_EXP_RISE_RATIO    1.4f
#define ISP_NIGHT_BLOCKED_LUM_RATIO 0.15f
#define ISP_NIGHT_LUM_RISE_RATIO    1.8f

#define ISP_CAM1_LUM_DROP_RATIO     0.25f
#define ISP_CAM1_GAIN_RISE_RATIO    1.8f
#define ISP_CAM1_EXP_RISE_RATIO     1.5f
#define ISP_CAM1_BLOCKED_LUM_RATIO  0.08f
#define ISP_CAM1_CONFIRM_FRAMES     5
#define ISP_CAM1_NIGHT_LUM_DROP_RATIO    0.35f
#define ISP_CAM1_NIGHT_GAIN_RISE_RATIO   2.0f
#define ISP_CAM1_NIGHT_EXP_RISE_RATIO    1.8f
#define ISP_CAM1_NIGHT_BLOCKED_LUM_RATIO 0.10f
#define ISP_CAM1_NIGHT_LUM_RISE_RATIO    2.5f

typedef enum {
    ISP_CLOSE_NORMAL = 0,
    ISP_CLOSE_SUSPECT,
    ISP_CLOSE_CONFIRMED,
    ISP_CLOSE_BLOCKED
} isp_close_state_t;

typedef struct {
    uint32_t base_lum;
    uint32_t base_gain;
    uint32_t base_exp_time;
    int base_initialized;
    int baseline_count;

    uint32_t cur_lum;
    uint32_t cur_gain;
    uint32_t cur_exp_time;

    isp_close_state_t close_state;
    int close_count;

    uint32_t lum_history[ISP_HISTORY_SIZE];
    uint32_t gain_history[ISP_HISTORY_SIZE];
    uint32_t exp_history[ISP_HISTORY_SIZE];
    int history_idx;
    int history_count;

    int is_night_mode;
    int last_daynight_mode;
} isp_cam_data_t;

static isp_cam_data_t g_isp_cam[MAX_CAM_NUM];
static int g_isp_available = -1;
static int g_isp_fail_count = 0;

static int query_isp_exposure(uint8_t cam_id, uint32_t *lum, uint32_t *gain, uint32_t *exp_time) {
    if (cam_id >= MAX_CAM_NUM || !lum || !gain || !exp_time) {
        return -1;
    }

    if (g_isp_available == 0) {
        return -1;
    }

    *lum = 0;
    *gain = 0;
    *exp_time = 0;

    ISP_EXP_INFO_S exp_info;
    memset(&exp_info, 0, sizeof(ISP_EXP_INFO_S));

    int ret = TS_MPI_ISP_QueryExposureInfo(cam_id, &exp_info);
    if (ret != 0) {
        g_isp_fail_count++;
        if (g_isp_fail_count >= ISP_MAX_FAIL_COUNT) {
            g_isp_available = 0;
            //printf("[ISP_CLOSE] ISP unavailable after %d failures, disabled\n", g_isp_fail_count);
        }
        return -1;
    }

    if (g_isp_available == -1) {
        g_isp_available = 1;
        g_isp_fail_count = 0;
        //printf("[ISP_CLOSE] ISP initialized successfully\n");
    }
    g_isp_fail_count = 0;

    *lum = exp_info.u32AveLum;
    *gain = exp_info.u32TotalGain;
    *exp_time = exp_info.u32ExpTime;

    //printf("lum=%u, gain=%u, exp_time=%u\n", lum, gain, exp_time);
    //printf("lum=%u, gain=%u, exp_time=%u\n", exp_info.u32AveLum, exp_info.u32TotalGain, exp_info.u32ExpTime);
    // printf("[ISP_RAW] cam=%d AveLum=%u TotalGain=%u ExpTime=%u AGain=%u DGain=%u ISPDGain=%u Exposure=%u Fps=%u\n",
    //        cam_id, exp_info.u32AveLum, exp_info.u32TotalGain, exp_info.u32ExpTime,
    //        exp_info.u32AGain, exp_info.u32DGain, exp_info.u32ISPDGain, exp_info.u32Exposure, exp_info.u32Fps);

    return 0;
}

static int query_isp_daynight_mode(uint8_t cam_id) {
    if (cam_id >= MAX_CAM_NUM) return 0;

    TS_S32 mode = 0;
    int ret = TS_MPI_VI_GetDayNight(cam_id, &mode);
    if (ret != 0) {
        return -1;
    }

    return mode;
}

static void update_isp_baseline(uint8_t cam_id, uint32_t lum, uint32_t gain, uint32_t exp_time) {
    if (cam_id >= MAX_CAM_NUM) return;

    isp_cam_data_t *cam = &g_isp_cam[cam_id];

    int cur_mode = query_isp_daynight_mode(cam_id);//当前的模式白天黑夜
    if (cur_mode >= 0 && cur_mode != cam->last_daynight_mode) {
        printf("[ISP_CLOSE] cam=%d daynight mode changed: %d->%d, reset baseline\n",
               cam_id, cam->last_daynight_mode, cur_mode);
        cam->is_night_mode = (cur_mode == 1) ? 1 : 0;
        cam->last_daynight_mode = cur_mode;
        cam->base_initialized = 0;
        cam->baseline_count = 0;
        cam->history_idx = 0;
        cam->history_count = 0;
        cam->close_count = 0;
        cam->close_state = ISP_CLOSE_NORMAL;
        memset(cam->lum_history, 0, sizeof(cam->lum_history));
        memset(cam->gain_history, 0, sizeof(cam->gain_history));
        memset(cam->exp_history, 0, sizeof(cam->exp_history));
    }

    int idx = cam->history_idx % ISP_HISTORY_SIZE;
    cam->lum_history[idx] = lum;
    cam->gain_history[idx] = gain;
    cam->exp_history[idx] = exp_time;
    cam->history_idx++;
    if (cam->history_count < ISP_HISTORY_SIZE) {
        cam->history_count++;
    }

    if (!cam->base_initialized && cam->history_count >= ISP_CLOSE_BASELINE_FRAMES) {
        uint32_t sum_lum = 0, sum_gain = 0, sum_exp = 0;
        int start = (cam->history_idx - ISP_CLOSE_BASELINE_FRAMES + ISP_HISTORY_SIZE) % ISP_HISTORY_SIZE;
        for (int i = 0; i < ISP_CLOSE_BASELINE_FRAMES; i++) {
            int hidx = (start + i) % ISP_HISTORY_SIZE;
            sum_lum += cam->lum_history[hidx];
            sum_gain += cam->gain_history[hidx];
            sum_exp += cam->exp_history[hidx];
        }
        cam->base_lum = sum_lum / ISP_CLOSE_BASELINE_FRAMES;
        cam->base_gain = sum_gain / ISP_CLOSE_BASELINE_FRAMES;
        cam->base_exp_time = sum_exp / ISP_CLOSE_BASELINE_FRAMES;
        cam->base_initialized = 1;

        printf("[ISP_CLOSE] cam=%d baseline: lum=%u gain=%u exp=%u (night=%d)\n",
               cam_id, cam->base_lum, cam->base_gain, cam->base_exp_time, cam->is_night_mode);
    }

    cam->cur_lum = lum;
    cam->cur_gain = gain;
    cam->cur_exp_time = exp_time;
}

static int detect_close_target_by_isp(uint8_t cam_id) {
    if (cam_id >= MAX_CAM_NUM) return 0;

    isp_cam_data_t *cam = &g_isp_cam[cam_id];

    if (!cam->base_initialized) {
        printf("[ISP_DET] cam=%d baseline not initialized yet (count=%d/%d)\n",
               cam_id, cam->history_count, ISP_CLOSE_BASELINE_FRAMES);
        return 0;
    }

    float lum_drop_ratio, gain_rise_ratio, exp_rise_ratio, blocked_lum_ratio;
    float lum_rise_ratio = 0.0f;
    int confirm_frames;
    int is_night = cam->is_night_mode;
    int is_cam1 = (cam_id == 1) ? 1 : 0;

    if (is_cam1) {
        if (is_night) {
            lum_drop_ratio    = ISP_CAM1_NIGHT_LUM_DROP_RATIO;
            gain_rise_ratio   = ISP_CAM1_NIGHT_GAIN_RISE_RATIO;
            exp_rise_ratio    = ISP_CAM1_NIGHT_EXP_RISE_RATIO;
            blocked_lum_ratio = ISP_CAM1_NIGHT_BLOCKED_LUM_RATIO;
            lum_rise_ratio    = ISP_CAM1_NIGHT_LUM_RISE_RATIO;
        } else {
            lum_drop_ratio    = ISP_CAM1_LUM_DROP_RATIO;
            gain_rise_ratio   = ISP_CAM1_GAIN_RISE_RATIO;
            exp_rise_ratio    = ISP_CAM1_EXP_RISE_RATIO;
            blocked_lum_ratio = ISP_CAM1_BLOCKED_LUM_RATIO;
        }
        confirm_frames = ISP_CAM1_CONFIRM_FRAMES;
    } else {
        if (is_night) {
            lum_drop_ratio    = ISP_NIGHT_LUM_DROP_RATIO;
            gain_rise_ratio   = ISP_NIGHT_GAIN_RISE_RATIO;
            exp_rise_ratio    = ISP_NIGHT_EXP_RISE_RATIO;
            blocked_lum_ratio = ISP_NIGHT_BLOCKED_LUM_RATIO;
            lum_rise_ratio    = ISP_NIGHT_LUM_RISE_RATIO;
        } else {
            lum_drop_ratio    = ISP_CLOSE_LUM_DROP_RATIO;
            gain_rise_ratio   = ISP_CLOSE_GAIN_RISE_RATIO;
            exp_rise_ratio    = ISP_CLOSE_EXP_RISE_RATIO;
            blocked_lum_ratio = ISP_CLOSE_BLOCKED_LUM_RATIO;
        }
        confirm_frames = ISP_CLOSE_CONFIRM_FRAMES;
    }

    if (cam->base_lum > 0 && cam->cur_lum < cam->base_lum * blocked_lum_ratio) {
        cam->close_count++;
        if (cam->close_count >= confirm_frames) {
            cam->close_state = ISP_CLOSE_BLOCKED;
            printf("[ISP_DET] cam=%d => BLOCKED confirmed (count=%d>=%d) night=%d cam1=%d\n",
                   cam_id, cam->close_count, confirm_frames, is_night, is_cam1);
            return 2;
        }
        cam->close_state = ISP_CLOSE_SUSPECT;
        printf("[ISP_DET] cam=%d => BLOCKED suspect (count=%d/%d) night=%d cam1=%d\n",
               cam_id, cam->close_count, confirm_frames, is_night, is_cam1);
        return 1;
    }

    float lum_ratio = (cam->base_lum > 0) ? (float)cam->cur_lum / (float)cam->base_lum : 1.0f;
    float gain_ratio = (cam->base_gain > 0) ? (float)cam->cur_gain / (float)cam->base_gain : 1.0f;
    float exp_ratio = (cam->base_exp_time > 0) ? (float)cam->cur_exp_time / (float)cam->base_exp_time : 1.0f;

    int close_indicators = 0;

    if (is_night) {
        if (lum_ratio < (1.0f - lum_drop_ratio)) close_indicators++;
        if (lum_rise_ratio > 0.0f && lum_ratio > lum_rise_ratio) close_indicators++;
        if (gain_ratio > gain_rise_ratio) close_indicators++;
        if (exp_ratio > exp_rise_ratio) close_indicators++;
    } else {
        if (lum_ratio < (1.0f - lum_drop_ratio)) close_indicators++;
        if (gain_ratio > gain_rise_ratio) close_indicators++;
        if (exp_ratio > exp_rise_ratio) close_indicators++;
    }

    printf("[ISP_DET] cam=%d cur: lum=%u gain=%u exp=%u | base: lum=%u gain=%u exp=%u | night=%d cam1=%d indicators=%d\n",
           cam_id, cam->cur_lum, cam->cur_gain, cam->cur_exp_time,
           cam->base_lum, cam->base_gain, cam->base_exp_time,
           is_night, is_cam1, close_indicators);

    if (close_indicators >= 2) {
        cam->close_count++;
        if (cam->close_count >= confirm_frames) {
            cam->close_state = ISP_CLOSE_CONFIRMED;
            printf("[ISP_DET] cam=%d => CONFIRMED (count=%d>=%d) night=%d cam1=%d\n",
                   cam_id, cam->close_count, confirm_frames, is_night, is_cam1);
            return 2;
        }
        cam->close_state = ISP_CLOSE_SUSPECT;
        printf("[ISP_DET] cam=%d => SUSPECT (count=%d/%d) night=%d cam1=%d\n",
               cam_id, cam->close_count, confirm_frames, is_night, is_cam1);
        return 1;
    }

    cam->close_count = 0;
    cam->close_state = ISP_CLOSE_NORMAL;
    return 0;
}

static void reset_isp_close_data(void) {
    memset(g_isp_cam, 0, sizeof(g_isp_cam));
}

static void reset_isp_close_state(void) {
    for (int cam = 0; cam < MAX_CAM_NUM; cam++) {
        g_isp_cam[cam].close_count = 0;
        g_isp_cam[cam].close_state = ISP_CLOSE_NORMAL;
    }
}

/*
 * 函数：is_cat_eat
 * 功能：判断当前帧是否为进食姿态
 * 规则：
 *   1) 只有 cam == 1 才可能判定进食
 *   2) cam != 1 一律返回进入(1)，不会出现进食(2)
 *   3) cam1 满足面积 > 阈值 才判定进食
 */
int is_cat_eat(ALG_CatDetect_DET_BOX_S *box) {
    if (!box)
        return ALG_CAT_ACT_INT;

    if (box->cam_id != 1 || box->cls_id == 1)
        return ALG_CAT_ACT_INT;

    float w = box->Xmax - box->Xmin;
    float h = box->Ymax - box->Ymin;
    float area = w * h;

    if (w <= 0 || h <= 0)
        return ALG_CAT_ACT_INT;

    if (area <= EAT_Thres)
        return ALG_CAT_ACT_INT;

    float center_y = (box->Ymin + box->Ymax) / 2.0f;

    if (center_y < CAM1_EAT_YMIN_THRES || center_y > CAM1_EAT_YMAX_THRES)
        return ALG_CAT_ACT_INT;

    if (area <= CAM1_EAT_AREA_THRES)
        return ALG_CAT_ACT_INT;

    return ALG_CAT_ACT_EAT;
}

/*
 * 函数：find_or_create_cat
 * 功能：获取全局唯一的猫状态结构体
 *       nameid 不参与条件判断，仅赋默认值
 * 注意：last_seen 不在此处初始化，而是在事件触发时初始化
 *       避免未检测到的摄像头误判超时
 */
static cat_t *find_or_create_cat(const char *nameid) {
    (void)nameid;  // 不使用 nameid 参数

    // 使用第一个猫结构体作为全局唯一状态
    cat_t *cat = &g_cats[0];
    if (!cat->nameid[0]) {
        // 首次使用，初始化基础字段
        strncpy(cat->nameid, DEFAULT_CAT_NAME, sizeof(cat->nameid) - 1);
        cat->nameid[sizeof(cat->nameid) - 1] = '\0';
        cat->deleted = 0;
        
        // last_seen 初始化为 -1，表示从未检测到
        // 事件触发时才会被初始化为当前时间
        for (int c = 0; c < MAX_CAM_NUM; c++) {
            cat->last_seen[c] = -1;
        }
        
        for (int c = 0; c < MAX_CAM_NUM; c++) {
            cat->cnt_in[c] = 0;
            cat->cnt_eat[c] = 0;
            cat->in_cnt[c] = 0;
            cat->stable_cnt[c] = 0;
        }
        cat->eat_cnt = 0;
        cat->event_type = 0;
        cat->in_event_started = 0;
        cat->eat_event_started = 0;
        
        SEG_LOG("cat initialized: last_seen set to -1 (not detected yet)");
    }
    return cat;
}

/*
 * 判断进入事件是否满足触发条件
 * 任意一个摄像头最近5帧在 1s 以内 → 有效进入
 * 返回值：满足触发的摄像头ID（0或1），-1表示不满足
 */
static int check_in_trigger(cat_t *cat, long long now) {
    if (!cat)
        return -1;

    // 检查每个摄像头是否满足触发条件
    for (int cam = 0; cam < MAX_CAM_NUM; cam++) {
        if (cat->in_cnt[cam] < IN_TRIGGER_FRAME)
            continue;

        int pos = cat->in_cnt[cam] % IN_TRIGGER_FRAME;
        // 边界检查
        if (pos < 0 || pos >= IN_TRIGGER_FRAME) {
            SEG_LOG("ERROR: check_in_trigger cam=%d pos=%d out of range", cam, pos);
            continue;
        }
        long long first = cat->in_times[cam][pos];
        if ((now - first) <= IN_WINDOW_MS) {
            SEG_LOG("cam %d in trigger satisfied: now=%lld, first=%lld, diff=%lld",
                    cam, now, first, now - first);
            return cam;  // 返回满足条件的摄像头ID
        }
    }
    return -1;  // 没有摄像头满足条件
}

/*
 * 判断进食事件是否满足触发条件
 * 最近5帧在 1s 以内 → 有效进食
 */
static bool check_eat_trigger(cat_t *cat, long long now, int cam_id) {
    if (!cat)
        return false;

    int trigger_frame = IN_TRIGGER_FRAME;
    long long window_ms = IN_WINDOW_MS;

    if (cam_id == 1) {
        trigger_frame = CAM1_EAT_TRIGGER_FRAME;
        window_ms = CAM1_EAT_WINDOW_MS;
    }

    if (cat->eat_cnt < trigger_frame)
        return false;

    int pos = cat->eat_cnt % IN_TRIGGER_FRAME;
    if (pos < 0 || pos >= IN_TRIGGER_FRAME) {
        SEG_LOG("ERROR: check_eat_trigger pos=%d out of range", pos);
        return false;
    }
    long long first = cat->eat_times[pos];
    return (now - first) <= window_ms;
}

/*
 * 判断防抖状态：1s内检测帧数是否达到阈值（按摄像头独立判断）
 * 用于过滤未检测到的帧，提高视频识别效率
 * 返回值：1=稳定（1s内>=5帧），0=不稳定
 */
static int check_stable(cat_t *cat, int cam_id, long long now) {
    if (!cat || cam_id < 0 || cam_id >= MAX_CAM_NUM)
        return 0;
    if (cat->stable_cnt[cam_id] < STABLE_THRESHOLD)
        return 0;

    int pos = cat->stable_cnt[cam_id] % IN_TRIGGER_FRAME;
    if (pos < 0 || pos >= IN_TRIGGER_FRAME) {
        return 0;
    }
    long long first = cat->stable_times[cam_id][pos];
    // 1s内有5帧以上检测 → 稳定
    return ((now - first) <= STABLE_WINDOW_MS) ? 1 : 0;
}

/*
 * 更新防抖滑动窗口（按摄像头独立更新）
 */
static void update_stable_window(cat_t *cat, int cam_id, long long now) {
    if (!cat || cam_id < 0 || cam_id >= MAX_CAM_NUM)
        return;
    int idx = cat->stable_cnt[cam_id] % IN_TRIGGER_FRAME;
    if (idx >= 0 && idx < IN_TRIGGER_FRAME) {
        cat->stable_times[cam_id][idx] = now;
        cat->stable_cnt[cam_id]++;
    }
}

/*
 * 函数：cat_in_set
 * 功能：单只猫单帧状态处理主逻辑
 * 流程：
 *   1. 更新出现时间
 *   2. 判断当前是进入/进食
 *   3. 状态机跳转：无 → 进入 → 进食
 *   4. 计数累加：帧数/次数
 *   5. 结果回写到box
 */
void cat_in_set(ALG_CatDetect_DET_BOX_S *box, long long now) {
    SEG_LOG("ENTER: box=%p, now=%lld", (void*)box, now);

    // 非法摄像头编号直接过滤
    if (!box) {
        SEG_LOG("ERROR: box is NULL");
        return;
    }

    // 对齐检查
    SEG_CHECK_ALIGN(box, 4);

    if (box->cam_id < 0 || box->cam_id >= MAX_CAM_NUM) {
        SEG_LOG("EXIT: cam_id=%d out of range [0,%d)", box->cam_id, MAX_CAM_NUM);
        return;
    }

    // nameid 赋默认值，不参与条件判断
    strncpy(box->nameid, DEFAULT_CAT_NAME, sizeof(box->nameid) - 1);
    box->nameid[sizeof(box->nameid) - 1] = '\0';

    // 获取全局唯一的猫状态结构体
    cat_t *cat = find_or_create_cat(box->nameid);
    if (!cat) {
        SEG_LOG("ERROR: cat is NULL");
        return;
    }

    // 对齐检查
    SEG_CHECK_ALIGN(cat, 8);

    int cam = box->cam_id;
    SEG_LOG("cat=%p, event_type=%d, cam=%d, in_cnt[cam]=%d, eat_cnt=%d",
            (void*)cat, cat->event_type, cam, cat->in_cnt[cam], cat->eat_cnt);

    // 边界检查：cam 已在上面验证
    cat->last_seen[cam] = now;

    // 初始化帧标记
    box->f_in = 0;
    box->f_eat = 0;
    box->act = 0;

    // 判断当前帧是否进食姿态（cam1才有效）
    int eat_act = is_cat_eat(box);
    box->act_cat = eat_act;
    SEG_LOG("cam=%d, eat_act=%d", cam, eat_act);

    // ======================================================
    // 更新防抖滑动窗口（按摄像头独立更新）
    // ======================================================
    update_stable_window(cat, cam, now);

    // ======================================================
    // 情况1：当前已经是进食状态 → 直接保持，不再降级
    // ======================================================
    if (cat->event_type == 2) {
        box->act = ALG_CAT_ACT_EAT;
        box->evt = 2;
        box->sta = 3;

        box->act_sta = check_stable(cat, cam, now);

        cat->cnt_eat[cam]++;
        box->cnt_in = cat->cnt_in[cam];
        box->cnt_eat = cat->cnt_eat[cam];
        return;
    }

    // ======================================================
    // 情况2：当前处于进入状态，可以升级为进食
    // ======================================================
    if (cat->event_type == 1) {
        box->act = ALG_CAT_ACT_INT;
        box->evt = 1;
        box->sta = 1;

        box->act_sta = check_stable(cat, cam, now);

        cat->cnt_in[cam]++;
        box->cnt_in = cat->cnt_in[cam];
        box->cnt_eat = cat->cnt_eat[cam];

        // 只有 cam1 且当前帧是进食姿态，才尝试升级
        if (cam == 1 && eat_act == ALG_CAT_ACT_EAT) {
            // 加入进食滑动窗口（边界检查）
            int idx = cat->eat_cnt % IN_TRIGGER_FRAME;
            SEG_LOG("eat upgrade: idx=%d, eat_cnt=%d", idx, cat->eat_cnt);
            if (idx >= 0 && idx < IN_TRIGGER_FRAME) {
                cat->eat_times[idx] = now;
                cat->eat_cnt++;
            }

            // 满足连续5帧 → 升级为进食事件
            if (check_eat_trigger(cat, now, cam) && !cat->eat_event_started) {
                cat->event_type = 2;
                cat->act_sta = ALG_CAT_ACT_EAT;
                cat->eat_event_started = 1;
                cat->eat_start_time = now;

                cat->cnt_eat[cam]++;

                box->act = ALG_CAT_ACT_EAT;
                box->evt = 2;
                box->f_eat = 1;
                box->cnt_eat = cat->cnt_eat[cam];
            }
        }
        return;
    }

    // ======================================================
    // 情况3：无事件，cam1检测到进食姿态
    // 进食事件可直接触发，不需要先进入事件
    // ======================================================
    if (cam == 1 && eat_act == ALG_CAT_ACT_EAT) {
        // 1. 加入进食滑动窗口
        int eat_idx = cat->eat_cnt % IN_TRIGGER_FRAME;
        if (eat_idx >= 0 && eat_idx < IN_TRIGGER_FRAME) {
            cat->eat_times[eat_idx] = now;
            cat->eat_cnt++;
        }

        // 2. 同时加入进入滑动窗口（进食帧也算进入帧）
        int in_idx = cat->in_cnt[cam] % IN_TRIGGER_FRAME;
        if (in_idx >= 0 && in_idx < IN_TRIGGER_FRAME) {
            cat->in_times[cam][in_idx] = now;
            cat->in_cnt[cam]++;
        }

        // 3. 优先检查是否满足进食触发条件（直接触发进食事件）
        if (check_eat_trigger(cat, now, cam) && !cat->eat_event_started) {
            cat->event_type = 2;
            cat->eat_event_started = 1;
            cat->eat_start_time = now;
            cat->in_event_started = 1;

            cat->cnt_eat[cam]++;

            box->act = ALG_CAT_ACT_EAT;
            box->evt = 2;
            box->f_eat = 1;
            box->cnt_eat = cat->cnt_eat[cam];
        }
        // 4. 如果不满足进食条件，检查是否满足进入触发条件
        else if (!cat->in_event_started) {
            int trigger_cam = check_in_trigger(cat, now);
            if (trigger_cam >= 0) {
                SEG_LOG("in event triggered by cam %d (eat frame)", trigger_cam);
                cat->event_type = 1;
                cat->in_event_started = 1;
                cat->in_start_time = now;

                box->act = ALG_CAT_ACT_INT;
                box->evt = 1;
                box->f_in = 1;
            }
        }

        box->act_sta = check_stable(cat, cam, now);

        if (cat->event_type == 1) {
            cat->cnt_in[cam]++;
        }

        box->cnt_in = cat->cnt_in[cam];
        box->cnt_eat = cat->cnt_eat[cam];
        return;
    }

    // ======================================================
    // 情况4：无事件，尝试触发进入事件
    // ======================================================
    // 加入当前摄像头的进入滑动窗口
    {
        int idx = cat->in_cnt[cam] % IN_TRIGGER_FRAME;
        SEG_LOG("in accumulate: cam=%d, idx=%d, in_cnt=%d", cam, idx, cat->in_cnt[cam]);
        if (idx >= 0 && idx < IN_TRIGGER_FRAME) {
            cat->in_times[cam][idx] = now;
            cat->in_cnt[cam]++;
        }
    }

    // 检查任意摄像头是否满足进入触发条件
    int trigger_cam = check_in_trigger(cat, now);
    if (trigger_cam >= 0 && !cat->in_event_started) {
        SEG_LOG("in event triggered by cam %d", trigger_cam);
        cat->event_type = 1;
        cat->in_event_started = 1;
        cat->in_start_time = now;

        // 注意：不在此处初始化 last_seen
        // last_seen 只在 cat_in_set 中检测到猫时更新
        // 未检测到的摄像头 last_seen 保持为 -1，不参与超时判断

        box->act = ALG_CAT_ACT_INT;
        box->evt = 1;
        box->f_in = 1;
    }

    box->act_sta = check_stable(cat, cam, now);

    if (cat->event_type == 1) {
        cat->cnt_in[cam]++;
    }

    box->cnt_in = cat->cnt_in[cam];
    box->cnt_eat = cat->cnt_eat[cam];
    SEG_LOG("EXIT: event_type=%d, cam=%d, in_count=%d, eat_count=%d",
            cat->event_type, cam, cat->cnt_in[cam], cat->cnt_eat[cam]);
}

/*
 * 函数：is_cat_out
 * 功能：超时检测
 *   长时间未出现 → 判定事件结束
 *   生成结束帧，并重置猫状态
 */
void is_cat_out(ALG_CatDetect_DET_RESULT_S *res, long long now) {
    SEG_LOG("ENTER: res=%p, now=%lld", (void*)res, now);
    if (!res) {
        SEG_LOG("ERROR: res is NULL");
        return;
    }
    SEG_LOG("res->u32ObjNum=%u", res->u32ObjNum);

    for (int i = 0; i < MAX_CAT_DET_NUM; i++) {
        cat_t *cat = &g_cats[i];

        // 跳过无效节点
        if (cat->deleted || !cat->nameid[0] || cat->event_type == 0)
            continue;

        SEG_LOG("checking cat[%d]: event_type=%d", i, cat->event_type);

        // 检查所有已检测到的摄像头是否都超时
        // 所有摄像头分别连续检测不到3s(进入)/10s(进食)以上，事件才结束
        int all_timeout = 1;  // 假设所有已检测的摄像头都超时
        int has_active_cam = 0;  // 是否有摄像头检测到过猫
        for (int c = 0; c < MAX_CAM_NUM; c++) {
            // 跳过从未检测到的摄像头
            if (cat->last_seen[c] < 0) {
                SEG_LOG("cat[%d] cam[%d]: never detected, skip timeout check", i, c);
                continue;
            }
            has_active_cam = 1;  // 至少有一个摄像头检测到过猫
            
            // 进入/进食使用不同超时
            int timeout = (cat->event_type == 1) ? OUT_times : EatOUT_times;
            SEG_LOG("cat[%d] cam[%d]: last_seen=%lld, now=%lld, timeout=%d, diff=%lld",
                    i, c, cat->last_seen[c], now, timeout, now - cat->last_seen[c]);
            // 如果任意一个摄像头未超时，则事件不结束
            if (now - cat->last_seen[c] <= timeout) {
                all_timeout = 0;
                SEG_LOG("cat[%d] cam[%d]: NOT timeout, event continues", i, c);
            }
        }

        // 如果没有摄像头检测到过猫，或者不是所有已检测的摄像头都超时，事件继续
        if (!has_active_cam || !all_timeout)
            continue;

        SEG_LOG("cat[%d]: TIMEOUT, generating out box", i);

        // 生成事件结束框
        if (res->u32ObjNum < MAX_RES_BOX_NUM) {
            ALG_CatDetect_DET_BOX_S *box = &res->stBox[res->u32ObjNum];
            SEG_CHECK_ALIGN(box, 4);
            SEG_LOG("out box: idx=%u, box=%p", res->u32ObjNum, (void*)box);
            memset(box, 0, sizeof(*box));
            strncpy(box->nameid, cat->nameid, sizeof(box->nameid) - 1);
            box->nameid[sizeof(box->nameid) - 1] = '\0';
            box->Conf = 0.0f;
            box->evt = 0;
            box->cam_id = 0;
            if (cat->event_type == 1) {
                box->act = ALG_CAT_ACT_OUT;
                box->sta = 5;
            } else {
                box->act = ALG_CAT_ACT_EAT_OUT;
                box->sta = 4;
            }

            int total_in_count = 0, total_eat_count = 0;
            for (int c = 0; c < MAX_CAM_NUM; c++) {
                total_in_count += cat->cnt_in[c];
                total_eat_count += cat->cnt_eat[c];
            }
            box->cnt_in = total_in_count;
            box->cnt_eat = total_eat_count;
            box->act_sta = 0;
            res->u32ObjNum++;
        } else {
            SEG_LOG("WARN: res->u32ObjNum >= MAX_RES_BOX_NUM, skip out box");
        }

        for (int c = 0; c < MAX_CAM_NUM; c++) {
            cat->cnt_in[c] = 0;
            cat->cnt_eat[c] = 0;
            cat->in_cnt[c] = 0;
            cat->stable_cnt[c] = 0;
            cat->last_seen[c] = -1;
        }

        cat->event_type = 0;
        cat->sta = 0;
        cat->act_sta = 0;
        cat->in_event_started = 0;
        cat->eat_event_started = 0;
        cat->eat_cnt = 0;
        cat->f_in = 0;
        cat->f_eat = 0;

#if ISP_CLOSE_DETECT_ENABLE
        reset_isp_close_state();
#endif
    }
    SEG_LOG("EXIT");
}

/*
 * 打印一帧所有结果信息，用于调试查看
 */
void print_result(ALG_CatDetect_DET_RESULT_S *data) {
    if (!data)
        return;

    int n = data->u32ObjNum;
    if (n > MAX_RES_BOX_NUM) {
        SEG_LOG("WARN: print_result n=%d > MAX_RES_BOX_NUM=%d, clamped", n, MAX_RES_BOX_NUM);
        n = MAX_RES_BOX_NUM;
    }
    for (int i = 0; i < n; i++) {
        if (!IS_ALIGNED(&data->stBox[i], 4)) {
            SEG_LOG("WARN: stBox[%d] not aligned", i);
            continue;
        }
        if (data->stBox[i].cls_id < 0) {
            SEG_LOG("WARN: stBox[%d] has invalid cls_id=%d, skip", i, data->stBox[i].cls_id);
            continue;
        }
        data->stBox[i].Conf = roundf(data->stBox[i].Conf * 100) / 100;
        data->stBox[i].Xmin = roundf(data->stBox[i].Xmin * 10000) / 10000;
        data->stBox[i].Ymin = roundf(data->stBox[i].Ymin * 10000) / 10000;
        data->stBox[i].Xmax = roundf(data->stBox[i].Xmax * 10000) / 10000;
        data->stBox[i].Ymax = roundf(data->stBox[i].Ymax * 10000) / 10000;
    printf("id=%s,cls_id=%d,act=%d,act_cat=%d,act_sta=%d,f_in=%d,cat_f_in=%d,f_eat=%d,cat_f_eat=%d,sta=%d,evt=%d",
       data->stBox[i].nameid,
       data->stBox[i].cls_id,
       data->stBox[i].act,
       data->stBox[i].act_cat,
       data->stBox[i].act_sta,
       data->stBox[i].f_in,
       data->stBox[i].cat_f_in,
       data->stBox[i].f_eat,
       data->stBox[i].cat_f_eat,
       data->stBox[i].sta,
       data->stBox[i].evt);

printf("cnt_in=%d,cnt_eat=%d,Conf=%.7g,Sim=%.7g,cam_id=%d,point=%.7g,%.7g,%.7g,%.7g\n",
       data->stBox[i].cnt_in,
       data->stBox[i].cnt_eat,
       data->stBox[i].Conf,
       data->stBox[i].Sim,
       data->stBox[i].cam_id,
       data->stBox[i].Xmin,
       data->stBox[i].Ymin,
       data->stBox[i].Xmax,
       data->stBox[i].Ymax);

        // printf("id=%s,class_id=%d,act=%d,act_cat=%d,act_cat_stable=%d,first_in=%d,cat_first_in=%d,first_eat=%d,cat_first_eat=%d,state=%d,event_type=%d,cat_first_in_count=%d,cat_first_eat_count=%d,DetectionConf=%f,MaxSimilarity=%f,cam_id=%d,point=%f,%f,%f,%f\n",
        //        data->stBox[i].nameid,
        //        data->stBox[i].class_id,
        //        data->stBox[i].act,
        //        data->stBox[i].act_cat,
        //        data->stBox[i].act_cat_stable,
        //        data->stBox[i].first_in,
        //        data->stBox[i].cat_first_in,
        //        data->stBox[i].first_eat,
        //        data->stBox[i].cat_first_eat,
        //        data->stBox[i].state,
        //        data->stBox[i].event_type,
        //        data->stBox[i].cat_first_in_count,    // 进入累计帧数
        //        data->stBox[i].cat_first_eat_count,   // 进食累计次数
        //        data->stBox[i].DetectionConf,
        //        data->stBox[i].MaxSimilarity,
        //        data->stBox[i].cam_id,
        //        data->stBox[i].f32Xmin,
        //        data->stBox[i].f32Ymin,
        //        data->stBox[i].f32Xmax,
        //        data->stBox[i].f32Ymax);
    }
}

/*
 * 函数：set_result
 * 功能：外部调用的总入口
 * 流程：
 *   1. 初始化
 *   2. 逐框处理行为
 *   3. 超时判断
 *   4. 打印调试
 */
void set_result(ALG_CatDetect_DET_RESULT_S *data) {
    SEG_LOG("ENTER: data=%p", (void*)data);
    if (!data) {
        SEG_LOG("ERROR: data is NULL");
        return;
    }

    // 对齐检查
    SEG_CHECK_ALIGN(data, 4);
    SEG_CHECK_ALIGN(&data->stBox[0], 4);

    long long now = get_current_time_ms();
    int ori_num = data->u32ObjNum;
    SEG_LOG("now=%lld, u32ObjNum=%u", now, ori_num);

    // 边界检查：防止 u32ObjNum 异常大导致越界
    if (ori_num < 0 || ori_num > MAX_RES_BOX_NUM) {
        SEG_LOG("WARN: u32ObjNum=%u invalid, clamped to 0", ori_num);
        data->u32ObjNum = 0;
        return;
    }

    // 第一次调用时初始化全局猫数组
    if (!atomic_load(&g_initialized)) {
        SEG_LOG("First call, initializing g_cats");
        memset(g_cats, 0, sizeof(g_cats));
        atomic_store(&g_initialized, true);
    }

    int valid_num = 0;
    for (int i = 0; i < ori_num; i++) {
        ALG_CatDetect_DET_BOX_S *box = &data->stBox[i];
        SEG_CHECK_ALIGN(box, 4);
        SEG_LOG("processing box[%d]: cls_id=%d, cam_id=%d", i, box->cls_id, box->cam_id);
        
        if (valid_num != i) {
            data->stBox[valid_num] = data->stBox[i];
        }
        valid_num++;
        
        cat_in_set(&data->stBox[valid_num - 1], now);
    }
    
    // 更新有效检测框数量
    data->u32ObjNum = valid_num;
    SEG_LOG("filtered: ori_num=%d, valid_num=%u", ori_num, data->u32ObjNum);

    // ============================================================
    // ISP目标过近检测集成
    // 原理：当进入事件已触发但YOLOv5检测不到目标时，
    //       通过ISP曝光参数判断是否为目标过近导致检测失败
    //       如果确认目标过近，则升级为进食事件并维持事件不超时
    // ============================================================
#if ISP_CLOSE_DETECT_ENABLE
    {
        cat_t *cat = &g_cats[0];
        int has_active_event = (cat->nameid[0] && cat->event_type >= 1) ? 1 : 0;

        if (has_active_event && valid_num == 0) {
            for (int cam = 0; cam < MAX_CAM_NUM; cam++) {
                uint32_t lum = 0, gain = 0, exp_time = 0;
                if (query_isp_exposure(cam, &lum, &gain, &exp_time) == 0) {
                    update_isp_baseline(cam, lum, gain, exp_time);
                } else {
                    printf("[ISP_CLOSE] cam=%d query failed, g_isp_available=%d fail_count=%d\n",
                           cam, g_isp_available, g_isp_fail_count);
                }
            }

            printf("[ISP_CLOSE] valid_num=%d event_type=%d in_started=%d eat_started=%d\n",
                   valid_num, cat->event_type, cat->in_event_started, cat->eat_event_started);

            for (int cam = 0; cam < MAX_CAM_NUM; cam++) {
                int close_result = detect_close_target_by_isp(cam);
                if (close_result >= 2) {
                    if (cam == 1 && cat->event_type == 1) {
                        printf("[ISP_CLOSE] cam=1 fluctuation ignored for entry->eating upgrade\n");
                        continue;
                    }

                    cat->last_seen[cam] = now;

                    if (cat->event_type == 1) {
                        cat->event_type = 2;
                        cat->eat_event_started = 1;
                        cat->eat_start_time = now;
                        cat->act_sta = ALG_CAT_ACT_EAT;

                        if (data->u32ObjNum < MAX_RES_BOX_NUM) {
                            ALG_CatDetect_DET_BOX_S *box = &data->stBox[data->u32ObjNum];
                            memset(box, 0, sizeof(*box));
                            strncpy(box->nameid, cat->nameid, sizeof(box->nameid) - 1);
                            box->nameid[sizeof(box->nameid) - 1] = '\0';
                            box->Conf = 0.0f;
                            box->act = ALG_CAT_ACT_EAT;
                            box->evt = 2;
                            box->f_eat = 1;
                            box->cam_id = cam;
                            box->act_cat = ALG_CAT_ACT_EAT;
                            cat->cnt_eat[cam]++;
                            box->cnt_in = cat->cnt_in[cam];
                            box->cnt_eat = cat->cnt_eat[cam];
                            data->u32ObjNum++;
                        }

                        printf("[ISP_CLOSE] cam=%d close target confirmed, upgrade entry->eating\n", cam);
                    } else if (cat->event_type == 2) {
                        cat->cnt_eat[cam]++;

                        if (data->u32ObjNum < MAX_RES_BOX_NUM) {
                            ALG_CatDetect_DET_BOX_S *box = &data->stBox[data->u32ObjNum];
                            memset(box, 0, sizeof(*box));
                            strncpy(box->nameid, cat->nameid, sizeof(box->nameid) - 1);
                            box->nameid[sizeof(box->nameid) - 1] = '\0';
                            box->Conf = 0.0f;
                            box->act = ALG_CAT_ACT_EAT;
                            box->evt = 2;
                            box->cam_id = cam;
                            box->act_cat = ALG_CAT_ACT_EAT;
                            box->cnt_in = cat->cnt_in[cam];
                            box->cnt_eat = cat->cnt_eat[cam];
                            data->u32ObjNum++;
                        }

                        printf("[ISP_CLOSE] cam=%d close target confirmed, maintain eating\n", cam);
                    }

                    break;
                }
            }
        } else {
            if (has_active_event && valid_num > 0) {
                reset_isp_close_state();
            }
        }
    }
#endif

    // 检查哪些猫超时消失（即使没有检测结果也要检测超时）
    SEG_LOG("calling is_cat_out");
    is_cat_out(data, now);

    // 打印最终结果
    SEG_LOG("calling print_result, final u32ObjNum=%u", data->u32ObjNum);
    print_result(data);
    SEG_LOG("EXIT");
}
