#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdatomic.h>
#include <stdbool.h>

#include "video_alg_catdetect-api.h"

// 调试开关
#define CAT_MEM_DEBUG         1
// 没有传入猫ID时使用的默认名称
#define DEFAULT_CAT_NAME      "default_cat"

// 单次检测最大返回目标框数量，防止数组越界
#ifndef MAX_RES_BOX_NUM
#define MAX_RES_BOX_NUM       64
#endif

// 系统最大支持摄像头路数
#ifndef MAX_CAM_NUM
#define MAX_CAM_NUM           4
#endif

// 事件触发滑动窗口：1秒内连续5帧满足才判定事件有效
#define IN_WINDOW_MS          1000
#define IN_TRIGGER_FRAME      5

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
 */
typedef struct {
    char     nameid[64];                // 猫的唯一ID字符串
    int      cam_mask;                  // 哪些摄像头出现过这只猫，bit位标记

    long long in_times[IN_TRIGGER_FRAME]; // 进入事件滑动窗口：保存最近5帧时间戳
    int       in_cnt;                  // 进入事件累计帧数

    long long eat_times[IN_TRIGGER_FRAME]; // 进食事件滑动窗口：保存最近5帧时间戳
    int       eat_cnt;                 // 进食事件累计帧数

    int      event_type;               // 当前事件类型：0=无 1=进入 2=进食
    int      state;                    // 内部状态机编号
    int      act_cat_stable;           // 稳定输出行为，防止帧间抖动

    long long last_seen[MAX_CAM_NUM];  // 每一路摄像头最后一次出现时间
    long long in_start_time;           // 进入事件开始时间戳
    long long event_start_time;         // 整个事件起始时间
    long long eat_start_time;          // 进食事件开始时间戳

    long long event_duration;          // 事件总时长（ms）
    long long in_duration;             // 进入行为时长
    long long eat_duration;            // 进食行为时长

    int      first_in;                 // 是否是进入事件第一帧（只置1一次）
    int      first_eat;                // 是否是进食事件第一帧（只置1一次）

    int      in_event_started;          // 进入事件是否已经触发完成
    int      eat_event_started;         // 进食事件是否已经触发完成

    int      cat_first_in_count;        // 进入事件持续帧数累加：每帧+1
    int      cat_first_eat_count;       // 进食事件完整触发次数：一次事件+1
    int      eat_miss_count;            // 进食漏检连续帧数，用于防抖

    long long last_in_time;             // 最近一次进入行为的时间
    long long last_eat_time;            // 最近一次进食行为的时间

    int      deleted;                  // 该猫节点是否已删除（可复用标记）
} cat_t;

// 最多同时跟踪 MAX_CAT_DET_NUM 只猫
static cat_t g_cats[MAX_CAT_DET_NUM];
// 当前已经跟踪的猫数量（原子变量，多线程安全）
static atomic_int g_cat_count = ATOMIC_VAR_INIT(0);
// 模块初始化标记
static atomic_bool g_initialized = ATOMIC_VAR_INIT(false);
// 进食判定：检测框面积阈值
//static float EAT_Thres = 0.125f;



/*
 * 获取系统当前时间戳，单位 ms
 */
static long long get_current_time_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

/*
 * 函数：is_cat_eat
 * 功能：判断当前帧是否为进食姿态
 * 规则：
 *   1) 只有 cam == 0 才可能判定进食
 *   2) cam != 0 一律返回进入(1)，不会出现进食(2)
 *   3) cam0 满足面积 > 阈值 才判定进食
 */
int is_cat_eat(ALG_CatDetect_DET_BOX_S *box) {
    if (!box)
        return ALG_CAT_ACT_INT;

    // 非0号摄像头，永远不判定进食
    if (box->cam_id != 0)
        return ALG_CAT_ACT_INT;

    float w = box->f32Xmax - box->f32Xmin;
    float h = box->f32Ymax - box->f32Ymin;
    float area = w * h;

    // 满足面积条件 → 进食(2)，否则 → 进入(1)
    return (w > 0 && h > 0 && area > EAT_Thres) ? ALG_CAT_ACT_EAT : ALG_CAT_ACT_INT;
}

/*
 * 函数：find_or_create_cat
 * 功能：根据猫ID查找已存在的猫结构体
 *       不存在则申请一个新的空节点
 */
static cat_t *find_or_create_cat(const char *nameid) {
    if (!nameid || *nameid == 0)
        return NULL;

    // 1. 先查找是否已经存在这只猫
    for (int i = 0; i < MAX_CAT_DET_NUM; i++) {
        if (!g_cats[i].deleted && strcmp(g_cats[i].nameid, nameid) == 0)
            return &g_cats[i];
    }

    // 2. 找一个已经被删除的空闲节点复用
    for (int i = 0; i < MAX_CAT_DET_NUM; i++) {
        if (g_cats[i].deleted) {
            memset(&g_cats[i], 0, sizeof(cat_t));
            strncpy(g_cats[i].nameid, nameid, sizeof(g_cats[i].nameid) - 1);
            g_cats[i].act_cat_stable = ALG_CAT_ACT_OUT;
            g_cats[i].deleted = 0;
            return &g_cats[i];
        }
    }

    // 3. 新增一只猫
    int cnt = atomic_load(&g_cat_count);
    if (cnt < MAX_CAT_DET_NUM) {
        cat_t *cat = &g_cats[cnt];
        memset(cat, 0, sizeof(*cat));
        strncpy(cat->nameid, nameid, sizeof(cat->nameid) - 1);
        cat->act_cat_stable = ALG_CAT_ACT_OUT;
        atomic_fetch_add(&g_cat_count, 1);
        return cat;
    }

    // 达到最大猫数量，无法新增
    return NULL;
}

/*
 * 判断进入事件是否满足触发条件
 * 最近5帧在 1s 以内 → 有效进入
 */
static bool check_in_trigger(cat_t *cat, long long now) {
    if (cat->in_cnt < IN_TRIGGER_FRAME)
        return false;

    int pos = cat->in_cnt % IN_TRIGGER_FRAME;
    long long first = cat->in_times[pos];
    return (now - first) <= IN_WINDOW_MS;
}

/*
 * 判断进食事件是否满足触发条件
 * 最近5帧在 1s 以内 → 有效进食
 */
static bool check_eat_trigger(cat_t *cat, long long now) {
    if (cat->eat_cnt < IN_TRIGGER_FRAME)
        return false;

    int pos = cat->eat_cnt % IN_TRIGGER_FRAME;
    long long first = cat->eat_times[pos];
    return (now - first) <= IN_WINDOW_MS;
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
    // 非法摄像头编号直接过滤
    if (!box || box->cam_id < 0 || box->cam_id >= MAX_CAM_NUM)
        return;

    // 食物类目标不参与行为分析
    if (box->class_id == ALG_CAT_CLASS_ID_FOOD)
        return;

    // 没有ID则使用默认ID
    if (!box->nameid[0])
        strncpy(box->nameid, DEFAULT_CAT_NAME, sizeof(box->nameid) - 1);

    // 获取这只猫的状态结构体
    cat_t *cat = find_or_create_cat(box->nameid);
    if (!cat)
        return;

    int cam = box->cam_id;
    // 更新该路摄像头最后出现时间
    cat->last_seen[cam] = now;

    // 初始化帧标记
    box->first_in = 0;
    box->first_eat = 0;
    box->act = 0;

    // 判断当前帧是否进食姿态（cam0才有效）
    int eat_act = is_cat_eat(box);
    box->act_cat = eat_act;

    // ======================================================
    // 情况1：当前已经是进食状态 → 直接保持，不再降级
    // ======================================================
    if (cat->event_type == 2) {
        box->act = ALG_CAT_ACT_EAT;
        box->event_type = 2;
        box->state = 3;
        // 同步计数到输出
        cat->cat_first_eat_count++;
        box->cat_first_in_count = cat->cat_first_in_count;
        box->cat_first_eat_count = cat->cat_first_eat_count;
        return;
    }

    // ======================================================
    // 情况2：当前处于进入状态，可以升级为进食
    // ======================================================
    if (cat->event_type == 1) {
        box->act = ALG_CAT_ACT_INT;
        box->event_type = 1;
        box->state = 1;

        // 进入状态：每帧帧数 +1
        cat->cat_first_in_count++;
        box->cat_first_in_count = cat->cat_first_in_count;
        box->cat_first_eat_count = cat->cat_first_eat_count;

        // 只有 cam0 且当前帧是进食姿态，才尝试升级
        if (cam == 0 && eat_act == ALG_CAT_ACT_EAT) {
            // 加入进食滑动窗口
            cat->eat_times[cat->eat_cnt % IN_TRIGGER_FRAME] = now;
            cat->eat_cnt++;

            // 满足连续5帧 → 升级为进食事件
            if (check_eat_trigger(cat, now) && !cat->eat_event_started) {
                cat->event_type = 2;
                cat->act_cat_stable = ALG_CAT_ACT_EAT;
                cat->eat_event_started = 1;
                cat->eat_start_time = now;

                // ==========================================
                // 【关键】进食事件次数 +1（一次事件只加一次）
                // ==========================================
                cat->cat_first_eat_count++;

                // 输出更新
                box->act = ALG_CAT_ACT_EAT;
                box->event_type = 2;
                box->first_eat = 1;
                box->cat_first_eat_count = cat->cat_first_eat_count;
            }
        }
        return;
    }

    // ======================================================
    // 情况3：无事件，当前帧直接满足进食条件（cam0）
    // ======================================================
    if (cam == 0 && eat_act == ALG_CAT_ACT_EAT) {
        // 加入进食滑动窗口
        cat->eat_times[cat->eat_cnt % IN_TRIGGER_FRAME] = now;
        cat->eat_cnt++;

        // 满足连续5帧 → 直接触发进食
        if (check_eat_trigger(cat, now)) {
            cat->event_type = 2;
            cat->act_cat_stable = ALG_CAT_ACT_EAT;
            cat->eat_event_started = 1;
            cat->eat_start_time = now;

            // ==========================================
            // 【关键】进食事件次数 +1
            // ==========================================
            cat->cat_first_eat_count++;

            box->act = ALG_CAT_ACT_EAT;
            box->event_type = 2;
            box->first_eat = 1;
        }
        // 同步计数
        box->cat_first_in_count = cat->cat_first_in_count;
        box->cat_first_eat_count = cat->cat_first_eat_count;
        return;
    }

    // ======================================================
    // 情况4：无事件，尝试触发进入事件
    // ======================================================
    // 加入进入滑动窗口
    cat->in_times[cat->in_cnt % IN_TRIGGER_FRAME] = now;
    cat->in_cnt++;

    // 满足连续5帧 → 进入事件触发
    if (check_in_trigger(cat, now) && !cat->in_event_started) {
        cat->event_type = 1;
        cat->act_cat_stable = ALG_CAT_ACT_INT;
        cat->in_event_started = 1;
        cat->in_start_time = now;

        box->act = ALG_CAT_ACT_INT;
        box->event_type = 1;
        box->first_in = 1;
    }

    // 进入状态每帧帧数 +1
    if (cat->event_type == 1) {
        cat->cat_first_in_count++;
    }

    // 最终同步所有计数到输出框
    box->cat_first_in_count = cat->cat_first_in_count;
    box->cat_first_eat_count = cat->cat_first_eat_count;
}

/*
 * 函数：is_cat_out
 * 功能：超时检测
 *   长时间未出现 → 判定事件结束
 *   生成结束帧，并重置猫状态
 */
void is_cat_out(ALG_CatDetect_DET_RESULT_S *res, long long now) {
    if (!res)
        return;

    for (int i = 0; i < MAX_CAT_DET_NUM; i++) {
        cat_t *cat = &g_cats[i];

        // 跳过无效节点
        if (cat->deleted || !cat->nameid[0] || cat->event_type == 0)
            continue;

        // 取所有摄像头中最后一次出现时间
        long long latest = 0;
        for (int c = 0; c < MAX_CAM_NUM; c++) {
            if (cat->last_seen[c] > latest)
                latest = cat->last_seen[c];
        }

        // 进入/进食使用不同超时
        int timeout = (cat->event_type == 1) ? OUT_times : EatOUT_times;
        if (now - latest <= timeout)
            continue;

        // 生成事件结束框
        if (res->u32ObjNum < MAX_RES_BOX_NUM) {
            ALG_CatDetect_DET_BOX_S *box = &res->stBox[res->u32ObjNum++];
            memset(box, 0, sizeof(*box));
            strncpy(box->nameid, cat->nameid, sizeof(box->nameid) - 1);
            box->event_type = 0;
            box->cam_id = -1;

            // 根据结束前事件类型设置行为
            if (cat->event_type == 1) {
                box->act = ALG_CAT_ACT_OUT;
                box->state = 5;
            } else {
                box->act = ALG_CAT_ACT_EAT_OUT;
                box->state = 4;
            }

            // 结束帧也同步最终计数
            box->cat_first_in_count = cat->cat_first_in_count;
            box->cat_first_eat_count = cat->cat_first_eat_count;
        }

        // ==========================
        // 事件结束 → 计数清零
        // 下次事件重新开始计算
        // ==========================
        cat->cat_first_in_count = 0;     // 进入帧清零
        cat->cat_first_eat_count = 0;    // 进食次数清零

        // 重置事件状态
        cat->event_type = 0;
        cat->state = 0;
        cat->in_event_started = 0;
        cat->eat_event_started = 0;
        cat->in_cnt = 0;
        cat->eat_cnt = 0;
        cat->first_in = 0;
        cat->first_eat = 0;
    }
}

/*
 * 打印一帧所有结果信息，用于调试查看
 */
void print_result(ALG_CatDetect_DET_RESULT_S *data) {
    if (!data)
        return;

    int n = data->u32ObjNum;
    for (int i = 0; i < n && i < MAX_RES_BOX_NUM; i++) {
        printf("id=%s,class_id=%d,act=%d,act_cat=%d,act_cat_stable=%d,first_in=%d,cat_first_in=%d,first_eat=%d,cat_first_eat=%d,state=%d,event_type=%d,cat_first_in_count=%d,cat_first_eat_count=%d,DetectionConf=%f,MaxSimilarity=%f,cam_id=%d,point=%f,%f,%f,%f\\n",
               data->stBox[i].nameid,
               data->stBox[i].class_id,
               data->stBox[i].act,
               data->stBox[i].act_cat,
               data->stBox[i].act_cat_stable,
               data->stBox[i].first_in,
               data->stBox[i].cat_first_in,
               data->stBox[i].first_eat,
               data->stBox[i].cat_first_eat,
               data->stBox[i].state,
               data->stBox[i].event_type,
               data->stBox[i].cat_first_in_count,    // 进入累计帧数
               data->stBox[i].cat_first_eat_count,   // 进食累计次数
               data->stBox[i].DetectionConf,
               data->stBox[i].MaxSimilarity,
               data->stBox[i].cam_id,
               data->stBox[i].f32Xmin,
               data->stBox[i].f32Ymin,
               data->stBox[i].f32Xmax,
               data->stBox[i].f32Ymax);
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
    if (!data)
        return;

    long long now = get_current_time_ms();
    int ori_num = data->u32ObjNum;

    // 第一次调用时初始化全局猫数组
    if (!atomic_load(&g_initialized)) {
        memset(g_cats, 0, sizeof(g_cats));
        atomic_store(&g_initialized, true);
    }

    // 逐框处理进入/进食状态
    for (int i = 0; i < ori_num && i < MAX_RES_BOX_NUM; i++) {
        ALG_CatDetect_DET_BOX_S *box = &data->stBox[i];
        if (box->class_id != ALG_CAT_CLASS_ID_FOOD)
            cat_in_set(box, now);
    }

    // 检查哪些猫超时消失
    is_cat_out(data, now);

    // 打印最终结果
    print_result(data);
}
