#include "stdio.h"
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include "string.h"
#include "math.h"
#define t_MOTOR_IOC_MAGIC       'm'

// application
typedef enum
{
    MOVE_UP = 0x01,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    STOP_MOVE,
} MSG_TYPE_E;
	
/* 1. argument for start up */
/*
 * MOTOR_PARM:          set paramters
 */
#define MOTOR_PARM          _IOW(t_MOTOR_IOC_MAGIC, 0x00, int)

/* 2. argument for real-time */
/*
 * MOTOR_SPEED_STEP:    set speed in step(hz)
 * MOTOR_SPEED_ANGLE:   set speed in angle
 * */
#define MOTOR_SPEED_STEP    _IOW(t_MOTOR_IOC_MAGIC, 0x20, int)
#define MOTOR_SPEED_ANGLE   _IOW(t_MOTOR_IOC_MAGIC, 0x21, int)

/* 3. base command */
/*
 * MOTOR_MOVE_LIMIT:    move clkwise or anticlkwise
 *                      clkwise: val > 0, anticlkwise: val < 0
 * MOTOR_MOVE_NOLIMIT:  move clkwise or anticlkwise
 *                      clkwise: val > 0, anticlkwise: val < 0
 * MOTOR_STOP:          set stop and wait for be stop finish
 * MOTOR_GET_STATUS:    get status
 * */
#define MOTOR_MOVE_LIMIT    _IOW(t_MOTOR_IOC_MAGIC, 0x40, int)
#define MOTOR_MOVE_NOLIMIT  _IOW(t_MOTOR_IOC_MAGIC, 0x41, int)
#define MOTOR_STOP          _IOW(t_MOTOR_IOC_MAGIC, 0x42, int)
#define MOTOR_GET_STATUS    _IOW(t_MOTOR_IOC_MAGIC, 0x43, int)


#define MOTOR_MOVE MOTOR_MOVE_LIMIT
#define MOTOR_SPEED MOTOR_SPEED_STEP

// application

#define MOTO_HORIZON            "/dev/motor0"
#define MOTO_VERTICAL           "/dev/motor1"

/* 4. extern command */
/*
 * MOTOR_RESET:         first go to ACTIVAL boundary then go to middle
 * MOTOR_MIDDLE:        turn to middle directly
 * MOTOR_CRUISE:        turn to the clkwise an anticlkwise until set stop
 * MOTOR_BOUNDARY:      move to ACTIVAL boundary
 *                      in clkwise or anticlkwise until be stop
 * */
#define MOTOR_RESET         _IOW(t_MOTOR_IOC_MAGIC, 0x60, int)
#define MOTOR_MIDDLE        _IOW(t_MOTOR_IOC_MAGIC, 0x61, int)
#define MOTOR_CRUISE        _IOW(t_MOTOR_IOC_MAGIC, 0x62, int)
#define MOTOR_BOUNDARY      _IOW(t_MOTOR_IOC_MAGIC, 0x63, int)

#define MOTO_RANGE_STEP360      4096*2
#define MOTO_ONE_ROUND_DEGREE   360
#define MOTO_VERTICAL           "/dev/motor1"
#define MOTO_HORIZON            "/dev/motor0"
#define DEGREE2STEP(degree)     (degree * MOTO_RANGE_STEP360 / MOTO_ONE_ROUND_DEGREE)
#define STEP2DEGREE(step)       (step * MOTO_ONE_ROUND_DEGREE / MOTO_RANGE_STEP360)
#define USERONESTEPDEGREE       5
// 电机横向转动的总步数
#define PTZ_MOTO_HORIZON_TOTAL_STEP  DEGREE2STEP(360)//DEGREE2STEP(360)
// 电机上下转动的总步数
#define PTZ_MOTO_VERTICAL_TOTAL_STEP DEGREE2STEP(70)//DEGREE2STEP(40)
// 因为当前电机不支持获取转轴姿态角度，设备启动的时候都需要对电机进行复位操作以保证预置点的位置恢复准确，为了增加用户体验电机复位时的转动速度调到很大，快速完成此过程
#define PTZ_RESETSPEED 800
// 设备正常工作时候的电机速度
#define PTZ_SPEED_HOR  600
#define PTZ_SPEED_VER  400

// 电机转动接口同步方式，MOVE_ASYNC 时候接口不会等待电机转动完成会直接返回
#define MOVE_SYNC  1
#define MOVE_ASYNC 0

#define CHECK_RET(express,name) \
    do{ \
        TS_S32 Ret; \
        Ret = express; \
        if (TS_SUCCESS != Ret) \
        { \
            printf("\033[0;31m%s failed at %s: LINE: %d with %#x!\033[0m\n", \
            name, __FUNCTION__, __LINE__, Ret); \
            return TS_FAILURE; \
        } \
    }while(0)

#define VERBOSE_JUMP(express, fail) \
    do{ \
        if (!(express)) \
        { \
            printf("\033[0;31mfailed at %s: LINE: %d!\033[0m\n", \
            __FUNCTION__, __LINE__); \
            goto fail; \
        } \
    }while(0)

#define VERBOSE_RETURN(express) \
    do{ \
        if (!(express)) \
        { \
           ;\
        } \
    }while(0)

#define _RETURN_VAL(express, val) \
    do{ \
        if (!(express)) \
        { \
            printf("\033[0;31mfailed at %s: LINE: %d!\033[0m\n", \
            __FUNCTION__, __LINE__); \
            return val; \
        } \
    }while(0)

#define VERBOSE_RETURN_VAL(express, val) \
    do{ \
        if (!(express)) \
        { \
            printf("\033[0;31mfailed at %s: LINE: %d!\033[0m\n", \
            __FUNCTION__, __LINE__); \
            return val; \
        } \
    }while(0)

/*
 * motor_status:
 * @MOTOR_IS_STOP:      motor is stoped now
 * @MOTOR_IS_RUNNING:   motor is running now
 * */
enum motor_status {
    MOTOR_IS_STOP = 0,
    MOTOR_IS_RUNNING,
};

typedef struct motor_parm {
    int pos;
    int speed_step;
    int steps_one_circle;
    int total_steps;
    int boundary_steps;
}MOTRO_PARA_T;

typedef struct motor_reset_data {
    int total_steps;
    int cur_step;
}MOTRO_RESET_T;

typedef struct motor_message {
    enum motor_status status;
    int pos;
    int speed_step;
    int speed_angle;
    int steps_one_circle;
    int total_steps;
    int boundary_steps;
    int attach_timer;
}MOTRO_STATUS_T;

// user define
typedef struct motor_handle {
    // TODO mtx
    int hor;
    int ver;
    bool is_init;
}MOTRO_HANDLE_T;

static MOTRO_HANDLE_T g_handle = {.is_init = false}; 

/**
 * 打开一个驱动。
 * 打开成功返回驱动描述符，失败返回 0。
 */
static int __open_driver(char *drv, int clk_freq, int range_step, int init_step)
{
    int fd = 0;
    struct motor_parm parm;

    fd = open(drv, O_RDWR);
    _RETURN_VAL(fd > 0, 0);

    parm.pos = init_step;
    parm.speed_step = clk_freq;
    parm.steps_one_circle = MOTO_RANGE_STEP360;
    parm.total_steps = range_step;
    parm.boundary_steps = 0;
    VERBOSE_JUMP(ioctl(fd, MOTOR_PARM, &parm) == 0, __failed_open_driver);

    return fd;

__failed_open_driver:

    close(fd);
    return 0;
}
/*
static int __postion(int fd)
{
    struct motor_message msg = {0};
    _RETURN_VAL(fd > 0, 0);
    _RETURN_VAL(ioctl(fd, MOTOR_GET_STATUS, &msg) == 0, 0);
    return STEP2DEGREE(msg.pos);
}
*/
static int __postion_step(int fd)
{
    struct motor_message msg = {0};
    _RETURN_VAL(fd > 0, 0);
    _RETURN_VAL(ioctl(fd, MOTOR_GET_STATUS, &msg) == 0, 0);
    return msg.pos;
}

/**
 * 设置电机的工作时钟。
 */
static void __clock_hz(int fd, int hz)
{
    VERBOSE_RETURN(fd > 0);
    VERBOSE_RETURN(ioctl(fd, MOTOR_SPEED_STEP, &hz) == 0);
}

/**
 * 检查电机是否停止。
 */
static bool __stopped(int fd)
{
    struct motor_message msg = {0};
    _RETURN_VAL(fd > 0, true);  ///< 没有设备返回停止标识。
    VERBOSE_RETURN_VAL(ioctl(fd, MOTOR_GET_STATUS, &msg) == 0, true);

    return MOTOR_IS_STOP == msg.status ? true : false;
}

/**
 * 等待电机停止。
 *
 */
static void __wait(int fd, int time_ms)
{
    /// 等待停止。
    while (!__stopped(fd)) {
        usleep(1000 * 100);
    }
}
/**
 * 云台转动到一个相对角度。
 */
static int __go(int fd, int degree, int sync, bool savepos)
{
    int pos = DEGREE2STEP(degree);
    VERBOSE_RETURN(fd > 0);
    VERBOSE_RETURN(ioctl(fd, MOTOR_MOVE_LIMIT, &pos) == 0);
    // VERBOSE_RETURN(sync);  ///< 非同步模式立即返回。
    // __wait(fd, -1);
    return 0;
}

/**
 * 云台转动到一个相对角度。
 */
static int __goto_step(int fd, int step_to)
{
    VERBOSE_RETURN(fd > 0);
    int pos = __postion_step(fd);
    pos = -(pos - step_to);
    VERBOSE_RETURN(ioctl(fd, MOTOR_MOVE_LIMIT, &pos) == 0);
    return 0;
}

static int __stop(int fd)
{
    VERBOSE_RETURN(fd > 0);
    struct motor_message msg = {0};
    VERBOSE_RETURN_VAL(ioctl(fd, MOTOR_GET_STATUS, &msg) == 0, true);
    if(MOTOR_IS_RUNNING == msg.status)
    {
        VERBOSE_RETURN(ioctl(fd, MOTOR_STOP, NULL) == 0);
    }
    return 0;
}

static int __middle(int fd)
{
    VERBOSE_RETURN(fd > 0);
    VERBOSE_RETURN(ioctl(fd, MOTOR_MIDDLE, NULL) == 0);
	return 0;
}

static int __reset(int fd)
{
    VERBOSE_RETURN(fd > 0);
    struct motor_reset_data rdata;
    VERBOSE_RETURN(ioctl(fd, MOTOR_RESET, &rdata) == 0);
    printf("totol:%d,cur:%d\n",rdata.total_steps,rdata.cur_step);
	return 0;
}

/*
static int __status(int fd,struct motor_message *s)
{
    _RETURN_VAL(s, -1);
    _RETURN_VAL(fd > 0, -1);
    _RETURN_VAL(ioctl(fd, MOTOR_GET_STATUS, s) == 0, 0);
    return 0;
}

static void __info(struct motor_message *s)
{
    VERBOSE_RETURN(s);
    VERBOSE_RETURN(g_handle.is_init == false);
    if(s)
    {
        printf("attach_timer:%d",s->attach_timer);
        printf("\tboundary_steps:%d",s->boundary_steps);
        printf("\tpos:%d",s->pos);
        printf("\tspeed_angle:%d",s->speed_angle);
        printf("\tspeed_step:%d",s->speed_step);
        printf("\tstatus:%d",s->status);
        printf("total_steps:%d\n",s->total_steps);
    }
}
*/

int ts_motor_init()
{
    _RETURN_VAL(false == g_handle.is_init,-1);
    g_handle.hor = __open_driver(MOTO_HORIZON, PTZ_RESETSPEED, PTZ_MOTO_HORIZON_TOTAL_STEP, 0);
    _RETURN_VAL(g_handle.hor > 0, 0);
    g_handle.ver = __open_driver(MOTO_VERTICAL, PTZ_RESETSPEED, PTZ_MOTO_VERTICAL_TOTAL_STEP, 0);
    _RETURN_VAL(g_handle.ver > 0, 0);
    printf("hor:%d,ver:%d\n",__postion_step(g_handle.hor),__postion_step(g_handle.ver));
   //usleep(1000 * 1000);
    __reset(g_handle.hor);
    __reset(g_handle.ver);
    __clock_hz(g_handle.hor, PTZ_SPEED_HOR);
    __clock_hz(g_handle.ver, PTZ_SPEED_VER);

#if 0
    while(__postion_step(g_handle.hor)>0)
    {
        __go(g_handle.hor,-20,MOVE_ASYNC,0);
        usleep(200 * 1000);
        printf("hor:%d,ver:%d\n",__postion_step(g_handle.hor),__postion_step(g_handle.ver));
    }
    __middle(g_handle.hor);
    printf("hor:%d,ver:%d\n",__postion_step(g_handle.hor),__postion_step(g_handle.ver));
    while(1);
 #endif

    g_handle.is_init = true;
    printf("motor init ok\n");
    return 0;
}

int ts_motor_is_init()
{
    if(g_handle.is_init == true)
        return 1;
    else
        return 0;

}

int motor_op(MSG_TYPE_E dir, int degree)
{
    int move = 0;
    if(degree < 0 || false == g_handle.is_init)
    {
        printf("step :%d init:%d erro \n",degree,g_handle.is_init);
    }

    switch (dir)
    {
    case MOVE_UP:
        move = -degree;
        __go(g_handle.ver,move,MOVE_ASYNC,0);
        break;

    case MOVE_DOWN:
        move = degree;
        __go(g_handle.ver,move,MOVE_ASYNC,0);
        break;

    case MOVE_LEFT:
        move = degree;
        __go(g_handle.hor,move,MOVE_ASYNC,0);
        break;

    case MOVE_RIGHT:
        move = -degree;
        __go(g_handle.hor,move,MOVE_ASYNC,0);
        break;
    default:
        break;
    }
    return 0;
}


int motor_op_one_step(MSG_TYPE_E dir)
{
    return motor_op(dir,USERONESTEPDEGREE);
}

int ts_motor_op_limt(MSG_TYPE_E dir)
{
    int degree = 0;
    switch (dir)
    {
    case MOVE_UP:
    case MOVE_DOWN:
        degree = PTZ_MOTO_VERTICAL_TOTAL_STEP;
        break;
    case MOVE_LEFT:
    case MOVE_RIGHT:
        degree = PTZ_MOTO_HORIZON_TOTAL_STEP;
        break;
    default:
        break;
    }
    return motor_op(dir,degree);
}

int ts_motor_op(MSG_TYPE_E dir)
{
    return ts_motor_op_limt(dir);
}

int ts_motor_set_speed(int x,int y)
{
    __clock_hz(g_handle.hor, x);
    __clock_hz(g_handle.ver, y);
    return 0;
}

void ts_motor_stop()
{
    __stop(g_handle.hor);
    __stop(g_handle.ver);
    return ;
}

int ts_motor_in_mid_pos()
{
    if(__postion_step(g_handle.hor) == 2048)
        return 1;
    else 
        return 0;
}

int ts_motor_reset_to_middle()
{
    __middle(g_handle.hor);
    __middle(g_handle.ver);
    //printf("turn to middle, pos:%d\n",__postion_step(g_handle.hor));
    return 0;
}

int ts_motor_get_posion(int *hor_val,int *ver_val)
{
    _RETURN_VAL(hor_val,-1);
    _RETURN_VAL(ver_val,-1);
    *hor_val = __postion_step(g_handle.hor);
    *ver_val = __postion_step(g_handle.ver);
    // printf("ptz get possion hor:%d,ver:%d\n",*hor_val,*ver_val);
    return 0;
}

int ts_motor_set_posion(int hor_val,int ver_val)
{
    //printf("*******go to hor:%d,ver:%d\n",hor_val,ver_val);
    __goto_step(g_handle.hor,hor_val);
    __goto_step(g_handle.ver,ver_val);
    VERBOSE_RETURN(MOVE_SYNC);
    __wait(g_handle.hor, -1);
    return 0;
}

int ts_motor_run(int hor_val,int ver_val)
{
    __go(g_handle.hor,hor_val,MOVE_ASYNC,0);
    __go(g_handle.ver,ver_val,MOVE_ASYNC,0);
    return 0;
}
