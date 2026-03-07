//#define DEBUG

/*
 * motor driver base architecture:
 *
 *       <--anticlkwise                                            clkwise-->
 *       <--cur_step--                                          cur_step++-->
 *        boundary                                                     boundary
 *           ^                                                            ^
 * 0step <<==|============================================================|==>> total_steps
*/

#include <linux/fs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/poll.h>
#include <linux/io.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/of_gpio.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/list.h>
#include <linux/delay.h>
#include <linux/types.h>
//#include <mach/hardware.h>

#include "tx_motor.h"
#include "motor_internal_struct.h"
#include "sys.h"
#include "tx_timer.h"
#include <dt-bindings/pinctrl/pinctrl-ts-tx5215.h>

/*motor status*/
#define MOTOR_STATUS_RUNNING    (1)
#define MOTOR_STATUS_STOPED     (2)

#define MOTOR_TURN_CLKWISE      (0) /*turn by clock wise*/
#define MOTOR_TURN_ANTICLKWISE  (1) /*turn by anti clock wise*/

#define ACC_PER_SECTION_COUNTER 1   /*steps in accelerating stage*/
#define DEC_PER_SECTION_COUNTER 1   /*steps in decelerating stage*/

static int motors_mutex = 0;    /*motors not working at the same time if set this paramter to 1*/

module_param(motors_mutex, int, 0644);
static int motor_major = 0;

/*motor device list*/
static LIST_HEAD(motor_list);

static int motor_hw_timer_handler(void *data);  //NOTE:why orignal code dont use spinlock protect?!

/*
 * print_array_int - a wrapper that print array in int
 * @array:      array for print
 * @num:        number of array
 */
static void print_array_int(const int *array, int num)
{
    int i;
    char string[1016]; // Todo : zhangning change 1024 to 1016
    char tmp[8];

    string[0] = '\0';

    for (i = 0; i < num; i++) {
        sprintf(tmp, "0x%02x ", array[i]);
        strcat(string, tmp);
    }

    pr_info("%s", string);
}

/*
 * speed_angle_to_speed_hz - get speed in ms by angle
 * @fix:        pointer to fix
 * @speed_angle:    speed in angle
 */
static inline int speed_angle_to_speed_hz(
    struct t_motor_fix_info *fix,
    int speed_angle)
{
    int hz;

    hz = speed_angle * fix->steps_one_circle / 360;

    return hz;
}

/*
 * speed_angle_to_speed_angle - get speed in ms by hz
 * @fix:        pointer to fix
 * @speed_hz:   speed in hz
 */
static inline int speed_hz_to_speed_angle(
    struct t_motor_fix_info *fix,
    int speed_hz)
{
    int angle;

    angle = speed_hz * 360 / fix->steps_one_circle;

    return angle;
}

/*
 * motor_set_default_fix_info - set default fix informations
 * @fix:    point to fix
 */
static void motor_set_default_fix_info(struct t_motor_fix_info *fix)
{
    int i;
    int acc_skip[] = {8, 4, 3, 2};
    int dec_skip[] = {2, 3, 4, 8};
    unsigned char pwm_tbl_cw[] = {0x09, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08};

    for (i = 0; i < sizeof(pwm_tbl_cw) / sizeof(pwm_tbl_cw[0]); i++)
        fix->pwm_tbl_cw[i] = pwm_tbl_cw[i];

    fix->pwm_tbl_cw_num = i;
    fix->steps_one_circle = 64 * 64;;

    fix->total_steps    = 2000;
    fix->boundary_steps = 20;

    for (i = 0; i < sizeof(acc_skip) / sizeof(acc_skip[0]); i++)
        fix->acc_skip[i] = acc_skip[i];

    fix->acc_skip_num   = i;
    fix->acc_per_section_counter = ACC_PER_SECTION_COUNTER;

    for (i = 0; i < sizeof(dec_skip) / sizeof(dec_skip[0]); i++)
        fix->dec_skip[i] = dec_skip[i];

    fix->dec_skip_num   = i;
    fix->dec_per_section_counter = DEC_PER_SECTION_COUNTER;
};

/*
 * check_hit_max_boundary - check if hit max boundaryo
 * @runtime:        pointer to runtime
 * @fix:            pointer to fix information
 *
 * @RETURN: 0-no, 1-yes
 */
static int check_hit_max_boundary(struct t_motor_runtime *runtime,
    struct t_motor_fix_info *fix)
{
    if (runtime->cur_step >= fix->total_steps - fix->boundary_steps)
        return 1;

    return 0;
}

/*
 * check_hit_min_boundary - check if hit min boundaryo
 * @runtime:        pointer to runtime
 * @fix:            pointer to fix information
 *
 * @RETURN: 0-no, 1-yes
 */
static int check_hit_min_boundary(struct t_motor_runtime *runtime,
    struct t_motor_fix_info *fix)
{
    if (runtime->cur_step <= fix->boundary_steps)
        return 1;

    return 0;
}

/*
 * motor_stop - set motor stop run
 *
 * @motor:  point to motor object
 *
 * NOTE: waitting for stoped finish
 */
static int motor_stop(struct t_motor *motor)
{
    unsigned long flags;
    int ret = 0;
    struct t_motor_runtime *runtime = &motor->runtime;
    struct t_motor_fix_info *fix = &motor->fix;

    pr_debug("%s %d\n", __func__, __LINE__);

    mutex_lock(&runtime->dev_mutex);
    spin_lock_irqsave(&runtime->lock, flags);

    if (runtime->status == MOTOR_STATUS_STOPED) {
        /*current status is stoped, is ok*/
        spin_unlock_irqrestore(&runtime->lock, flags);
        mutex_unlock(&runtime->dev_mutex);
        return 0;
    }

    if (runtime->move_cmd == MOTOR_CRUISE ||
        runtime->move_cmd == MOTOR_MOVE_NOLIMIT)
        /*if cruise, stoped in irq hander*/
        runtime->set_stop = 1;
    else if (runtime->remain_steps >
        fix->dec_skip_num * fix->dec_per_section_counter) {
        /*else set deceleration*/
        runtime->dest_steps =
            runtime->remain_steps =
                fix->dec_skip_num * fix->dec_per_section_counter;
    }

    spin_unlock_irqrestore(&runtime->lock, flags);
    mutex_unlock(&runtime->dev_mutex);

    /*wait for finish*/
    do {
        ret = wait_event_timeout(runtime->stop_wait,
                runtime->status == MOTOR_STATUS_STOPED,
                msecs_to_jiffies(10000));

        if (ret == 0) {
            ret = -ETIMEDOUT;
            break;
        }
    } while (ret == -ERESTARTSYS);

    ret = 0;

    return ret;
}

/*
 * t_motor_open - motor device driver opened
 *
 * @inod:
 * @file:
 */
static int tx_motor_open(struct inode *inode, struct file *file)
{
    //int minor;
    //bool found = false;
    struct t_motor *motor;

    pr_debug("%s %d\n", __func__, __LINE__);

    motor = container_of(inode->i_cdev, struct t_motor, cdev);
    mutex_lock(&motor->runtime.dev_mutex);

    if (motor->user_count <= 0)
        file->private_data = motor;

    motor->user_count++;
    mutex_unlock(&motor->runtime.dev_mutex);
    pr_debug("open tx motor device success.\n");
    return 0;
}

/*
 * tx_motor_release - motor device driver released
 *
 * @inod:
 * @file:
 */
static int tx_motor_release(struct inode *inode, struct file *file)
{
    //int minor;
    //bool found = false;
    struct t_motor *motor = file->private_data;

    pr_debug("%s %d\n", __func__, __LINE__);

    BUG_ON(motor == NULL);

    motor_stop(motor);
    motor->user_count--;

    return 0;
}

/*
 * tx_motor_dev_close - motor device released
 *
 * @motor:  point to motor object
 */
static int tx_motor_dev_close(struct t_motor *motor)
{
    BUG_ON(motor == NULL);

    pr_debug("%s\n", __func__);

    motor_stop(motor);

    pr_debug("close tx motor device success.\n");
    return 0;
}

/*
 * check_current_motor_mutex_run - check current motor is mutex
 *
 * @motor:  point to motor object
 *
 * RETURN: 0:cannot run. 1:run
 */
static int check_current_motor_mutex_run(struct t_motor *motor)
{
    int run;
    struct t_motor *other_motor;
    struct t_motor_runtime *other_runtime;
    struct t_motor_runtime *runtime = &motor->runtime;

    if (motor_list.next == &motor->list)
        other_motor = container_of(motor_list.next->next, struct t_motor, list);
    else
        other_motor = container_of(motor_list.next, struct t_motor, list);

    other_runtime = &other_motor->runtime;

    if (other_runtime->status != MOTOR_STATUS_STOPED) {
        /*if another motor is working then this motor not work and return*/
        runtime->wait_for_run = 1;
        run = 0;
    } else {
        /*another motor is not working now, so this motor will work*/
        runtime->wait_for_run = 0;
        run = 1;
    }

    return run;
}

/*
 * run_next_motor - check current motor is mutex
 *
 * @motor:  point to motor object
 *
 * RETURN: 0:cannot run. 1:run
 */
static int run_next_motor(struct t_motor *motor)
{
    int run = 0;
    struct t_motor *other_motor;
    struct t_motor_runtime *other_runtime;

    if (motor_list.next == &motor->list)
        other_motor = container_of(motor_list.next->next, struct t_motor, list);
    else
        other_motor = container_of(motor_list.next, struct t_motor, list);

    other_runtime = &other_motor->runtime;

    if (other_runtime->wait_for_run) {
        int hz;

        hz = other_motor->runtime.speed_step;
        tx_timer_start(
            motor_hw_timer_handler,
            other_motor,
            other_motor->hw_timer,
            hz);
        other_runtime->wait_for_run = 0;
        run = 1;
    }

    return run;
}

/*
 * motor_turn - control the motor turn
 *
 * @motor:  point to motor object
 * @cw:     turn in direction, clkwise or anticlkwise
 * @steps:  turn steps
 * @counter:
 */
static int motor_turn(struct t_motor *motor, int cw, int steps, int counter)
{
    int i;
    int hz;
    int ret;
    struct t_motor_runtime *runtime = &motor->runtime;

    if (motor_list.next == &motor->list) {
        i = 0;
    } else {
        i = 1;
    }

    pr_debug("%s %d, motor:%d, cur_step:%d\n",
        __func__, __LINE__, i, runtime->cur_step);

    /*forbid working at the same time*/
    if (motors_mutex)
        if (!check_current_motor_mutex_run(motor))
            return 0;

    runtime->wait_for_run = 0;
    runtime->cw = cw;
    runtime->dest_steps = steps + counter;
    runtime->remain_steps = steps;
    runtime->status = MOTOR_STATUS_RUNNING;
    runtime->counter = counter;

    hz = runtime->speed_step;
    pr_debug("%s hz=%d, dest_steps:%d, remain_steps:%d\n",
        __func__, hz, runtime->dest_steps, runtime->remain_steps);

    /*get hardware timer working for the motor*/
    /*NOTE: one timer onlay working or only one motor*/
    ret = tx_timer_start(motor_hw_timer_handler, motor, motor->hw_timer, hz);

    if (ret) {
        pr_err("%s open timer for ptz failed!\n", __func__);
    }

    return ret;
}

/*
 * turn_middle - turn to middle
 *
 * @motor:  point to motor object
 */
static void turn_middle(struct t_motor *motor)
{
    int ret;
    int x;
    int total_steps;
    unsigned long flags;
    struct t_motor_runtime *runtime = &motor->runtime;
    struct t_motor_fix_info *fix = &motor->fix;

    pr_debug("%s %d\n", __func__, __LINE__);

    total_steps = fix->total_steps;

    mutex_lock(&runtime->dev_mutex);
    spin_lock_irqsave(&runtime->lock, flags);

    /*turn to middle*/
    runtime->set_stop = 0;
    x = total_steps / 2 - runtime->cur_step;

    if (x > 0)
        motor_turn(motor, MOTOR_TURN_CLKWISE, x, 0);
    else if (x < 0)
        motor_turn(motor, MOTOR_TURN_ANTICLKWISE, 0 - x, 0);
    else {
        spin_unlock_irqrestore(&runtime->lock, flags);
        mutex_unlock(&runtime->dev_mutex);
        return;
    }

    spin_unlock_irqrestore(&runtime->lock, flags);
    mutex_unlock(&runtime->dev_mutex);

    /*wait for finish*/
    do {
        ret = wait_event_timeout(runtime->stop_wait,
                runtime->status == MOTOR_STATUS_STOPED,
                msecs_to_jiffies(10000));

        if (ret == 0) {
            ret = -ETIMEDOUT;
            break;
        }
    } while (ret == -ERESTARTSYS);

    ret = 0;
}

/*
 * motor_reset - turn to middle
 * first go to ACTIVAL boundary then go to middle
 *
 * @motor:  point to motor object
 * @rdata:  point to data structure, is return data
 *
 * NOTE: waitting for reset finish
 */
static int motor_reset(struct t_motor *motor, struct motor_reset_data *rdata)
{
    unsigned long flags;
    int ret = 0;
    int bound_steps;
    int add_steps;
    int total_steps;
    struct t_motor_runtime *runtime = &motor->runtime;
    struct t_motor_fix_info *fix = &motor->fix;

    pr_debug("%s %d\n", __func__, __LINE__);

    mutex_lock(&runtime->dev_mutex);
    spin_lock_irqsave(&runtime->lock, flags);

    /*get bound & total steps*/
    if (motor_list.next == &motor->list) {
        bound_steps = fix->boundary_steps;
        /*be sure can reach ACTIVAL boundary*/
        add_steps = 100;
        total_steps = fix->total_steps;
    } else {
        bound_steps = fix->boundary_steps;
        /*be sure can reach ACTIVAL boundary*/
        add_steps = 50;
        total_steps = fix->total_steps;
    }

    /*turn to ACTIVAL clkwise endian*/
    runtime->cur_step = total_steps/4;
    runtime->set_stop = 0;
    motor_turn(motor, MOTOR_TURN_ANTICLKWISE, total_steps/4, 0);
    spin_unlock_irqrestore(&runtime->lock, flags);

    /*can not release dev_mutex at here*/

    /*wait for finish*/
    do {
        ret = wait_event_timeout(runtime->stop_wait,
                runtime->status == MOTOR_STATUS_STOPED,
                msecs_to_jiffies(50000));

        if (ret == 0) {
            ret = -ETIMEDOUT;
            break;
        }
    } while (ret == -ERESTARTSYS);

    ret = 0;

    msleep(10);

    /*reserve boundary*/
    spin_lock_irqsave(&runtime->lock, flags);
    runtime->cur_step = bound_steps;
    runtime->set_stop = 0;
    motor_turn(motor, MOTOR_TURN_CLKWISE, total_steps, 0);
    spin_unlock_irqrestore(&runtime->lock, flags);

    /*wait for finish*/
    do {
        ret = wait_event_timeout(runtime->stop_wait,
                runtime->status == MOTOR_STATUS_STOPED,
                msecs_to_jiffies(50000));

        if (ret == 0) {
            ret = -ETIMEDOUT;
            break;
        }
    } while (ret == -ERESTARTSYS);

    /*set current status*/
    ret = 0;
    runtime->cur_step = total_steps - bound_steps;

    mutex_unlock(&runtime->dev_mutex);

    /*wait for some times*/
    msleep(10);

    /*turn to middle*/
    turn_middle(motor);

    /*return current status*/
    rdata->total_steps = fix->total_steps;
    rdata->cur_step = runtime->cur_step;

    return ret;
}

/*
 * motor_movde - move in clkwise or anticlkwise
 *
 * @motor:  point to motor object
 * @val:    move steps
 *          val > 0: clkwise
 *          val < 0: anticlkwise
 *
 * NOTE: waitting for move finish
 */
static int motor_move(struct t_motor *motor, int val)
{
    unsigned long flags;
    int dir = 0;
    int cur = 0;
    struct t_motor_runtime *runtime = &motor->runtime;
    struct t_motor_fix_info *fix = &motor->fix;

    pr_debug("%s, val:%d\n", __func__, val);

    if (!val)
        return 0;

    mutex_lock(&runtime->dev_mutex);
    spin_lock_irqsave(&runtime->lock, flags);

    /*reset stop status*/
    runtime->set_stop = 0;

    /*process dirction*/
    if (runtime->status != MOTOR_STATUS_STOPED &&
        ((runtime->cw == MOTOR_TURN_CLKWISE &&
                val > 0) ||
            (runtime->cw == MOTOR_TURN_ANTICLKWISE &&
                val < 0))) {
        /*in same direction, accelerating*/
        int acc_steps = fix->acc_skip_num * fix->acc_per_section_counter;

        cur = runtime->dest_steps - runtime->remain_steps;
        cur = cur > acc_steps ? acc_steps : cur;
    } else if (runtime->status != MOTOR_STATUS_STOPED) {
        /*in different direction, decelerating*/
        int ret;
        int dec_steps = fix->dec_skip_num * fix->dec_per_section_counter;

        if (runtime->remain_steps > dec_steps)
            runtime->dest_steps = runtime->remain_steps = dec_steps;

        /*NOTE: release lock*/
        spin_unlock_irqrestore(&runtime->lock, flags);

        /*wait for finish*/
        do {
            ret = wait_event_timeout(runtime->stop_wait,
                    runtime->status == MOTOR_STATUS_STOPED,
                    msecs_to_jiffies(10000));

            if (ret == 0) {
                ret = -ETIMEDOUT;
                break;
            }
        } while (ret == -ERESTARTSYS);

        /*motor is stop now*/
        msleep(10);
        /*NOTE: reget lock*/
        spin_lock_irqsave(&runtime->lock, flags);
    }

    /*move*/
    if (val > 0)  {
        if (runtime->move_cmd != MOTOR_MOVE_NOLIMIT &&
            check_hit_max_boundary(runtime, fix)) {
            val = 0;
            pr_debug("hit total_steps, don't mov\n");
        }

        dir = 1;
    } else if (val < 0) {
        if (runtime->move_cmd != MOTOR_MOVE_NOLIMIT &&
            check_hit_min_boundary(runtime, fix)) {
            val = 0;
            pr_debug("hit 0_steps, don't mov\n");
        } else {
            val = 0 - val;
        }

        dir = 0;
    }

    if (dir)
        motor_turn(motor, MOTOR_TURN_CLKWISE, val, cur);
    else
        motor_turn(motor, MOTOR_TURN_ANTICLKWISE, val, cur);

    /*release lock*/
    spin_unlock_irqrestore(&runtime->lock, flags);
    mutex_unlock(&runtime->dev_mutex);

    return 0;
}

/*
 * motor_cruise - move in clkwise and anticlkwise
 *
 * @motor:  point to motor object
 */
static int motor_cruise(struct t_motor *motor)
{
    int ret;
    int total_steps;

    pr_debug("%s %d\n", __func__, __LINE__);

    motor->runtime.set_stop = 0;
    total_steps = motor->fix.total_steps;

    ret = motor_move(motor, 0 - total_steps);

    return ret;
}

/*
 * tx_motor_ioctl - move ioctl
 *
 * @file:   point to the file object
 * @cmd:    user command
 * @arg:    user argument
 */
static long tx_motor_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    /*
     * 1. argument for start up
     * 2. argument for real-time
     * 3. base command
     * 4. extern command
     * */
    int val;
    int ret = 0;
    unsigned long flags;
    struct t_motor *motor = file->private_data;
    struct t_motor_fix_info *fix = &motor->fix;
    struct t_motor_runtime *runtime = &motor->runtime;

    pr_debug("%s %d\n", __func__, __LINE__);

    switch (cmd) {
    /* 1. argument for start up */
    case MOTOR_PARM: {
            struct motor_parm parm;

            if (copy_from_user(&parm, (void __user *)arg, sizeof(parm))) {
                pr_err("[%s][%d] copy to user error\n",
                    __func__, __LINE__);
                return -EFAULT;
            }

            runtime->cur_step = parm.pos;
            runtime->speed_step = parm.speed_step;

            fix->steps_one_circle = parm.steps_one_circle;
            fix->total_steps = parm.total_steps;
            fix->boundary_steps = parm.boundary_steps;
        }
        break;

    /* 2. argument for real-time */
    case MOTOR_SPEED_STEP:

        /*set speed in step(hz)*/
        if (get_user(val, (int __user *)arg)) {
            pr_err("[%s][%d] copy to user error\n",
                __func__, __LINE__);
            return -EFAULT;
        }

        runtime->speed_step = val; //HZ
        break;

    case MOTOR_SPEED_ANGLE:

        /*set speed in angle*/
        if (get_user(val, (int __user *)arg)) {
            pr_err("[%s][%d] copy to user error\n",
                __func__, __LINE__);
            return -EFAULT;
        }

        if (val > 0)
            runtime->speed_step = speed_angle_to_speed_hz(fix, val);
        else {
            pr_err("%s set spped_angle:%d is wrong\n", __func__, val);
            ret = -EINVAL;
        }

        break;

    /* 3. base command */
    case MOTOR_MOVE_LIMIT:

        /*move clkwise or anticlkwise*/
        /*auto stop*/
        /*clkwise: val > 0, anticlkwise: val < 0*/
        if (get_user(val, (int __user *)arg)) {
            pr_err("[%s][%d] copy to user error\n",
                __func__, __LINE__);
            return -EFAULT;
        }

        runtime->move_cmd = MOTOR_MOVE_LIMIT;
        ret = motor_move(motor, val);
        break;

    case MOTOR_MOVE_NOLIMIT:

        /*move clkwise or anticlkwise*/
        /*no auto stop*/
        /*clkwise: val > 0, anticlkwise: val < 0*/
        if (get_user(val, (int __user *)arg)) {
            pr_err("[%s][%d] copy to user error\n",
                __func__, __LINE__);
            return -EFAULT;
        }

        runtime->move_cmd = MOTOR_MOVE_NOLIMIT;
        ret = motor_move(motor, val);
        break;

    case MOTOR_STOP:
        /*set stop and wait for be stop finish*/
        ret = motor_stop(motor);
        break;

    case MOTOR_GET_STATUS:
        /*get status*/
        {
            struct motor_message msg;

            mutex_lock(&runtime->dev_mutex);
            spin_lock_irqsave(&runtime->lock, flags);

            msg.pos = runtime->cur_step;
            msg.status =
                runtime->status == MOTOR_STATUS_STOPED ?
                MOTOR_IS_STOP : MOTOR_IS_RUNNING;
            msg.speed_step = runtime->speed_step;
            msg.speed_angle = speed_hz_to_speed_angle(fix, msg.speed_step);
            msg.steps_one_circle = fix->steps_one_circle ;
            msg.total_steps = fix->total_steps ;
            msg.boundary_steps = fix->boundary_steps ;
            msg.attach_timer = fix->attach_timer ;
            spin_unlock_irqrestore(&runtime->lock, flags);
            mutex_unlock(&runtime->dev_mutex);

            if (copy_to_user((void __user *)arg, &msg, sizeof(msg))) {
                pr_err("[%s][%d] copy to user error\n",
                    __func__, __LINE__);
                return -EFAULT;
            }
        }
        break;

    /* 4. extern command */
    case MOTOR_RESET:
        /*first go to ACTIVAL boundary then go to middle*/
        {
            struct motor_reset_data rdata;

            runtime->move_cmd = MOTOR_RESET;
            ret = motor_reset(motor, &rdata);

            if (copy_to_user((void __user *)arg, &rdata, sizeof(rdata))) {
                pr_err("[%s][%d] copy to user error\n",
                    __func__, __LINE__);
                return -EFAULT;
            }
        }
        break;

    case MOTOR_MIDDLE:
        /*turn to middle directly*/
        runtime->move_cmd = MOTOR_MIDDLE;
        turn_middle(motor);
        break;

    case MOTOR_CRUISE:
        /*turn to the clkwise an anticlkwise until set stop*/
        runtime->move_cmd = MOTOR_CRUISE;
        motor_cruise(motor);
        break;

    case MOTOR_BOUNDARY:
        /*move to ACTIVAL boundary in clkwise or anticlkwise until be stop*/
        runtime->move_cmd = MOTOR_BOUNDARY;

        if (get_user(val, (int __user *)arg)) {
            pr_err("[%s][%d] copy to user error\n",
                __func__, __LINE__);
            return -EFAULT;
        }

        if (val < 0)
            motor_move(motor, 0 - fix->total_steps);
        else
            motor_move(motor, fix->total_steps);

        break;

    default:
        break;
    }

    return ret;
}

/*ops*/
static struct file_operations tx_motor_fops = {
    .owner = THIS_MODULE,
    .open = tx_motor_open,
    .release = tx_motor_release,
    .unlocked_ioctl = tx_motor_ioctl,
};

/*
 * acc - accelerating
 *
 * @fix:        motor fix information
 * @cur_steps:  current steps
 * @md_count:
 *
 * RETURN: 0-skip, 1-acc
 */
static int inline acc(struct t_motor_fix_info *fix, int cur_steps, int md_count)
{
    int ret;
    int index;
    int i, j;
    int acc_steps = 0;

    for (i = 0; i < fix->acc_skip_num; i++)
        for (j = 0; j < fix->acc_per_section_counter; j++) {
            acc_steps += fix->acc_skip[i];

            if (acc_steps - 1 == md_count) {
                index = i;
                ret =  1;
                goto end;
            } else if (acc_steps > md_count) {
                index = i;
                ret = 0;
                goto end;
            }
        }

    index = i;
    ret = 1;

end:

    pr_debug("acc i:%d,r:%d,mc:%d\n", index, ret, md_count);
    return ret;
}

/*
 * dec - decelerating
 *
 * @fix:        motor fix information
 * @remainer:   remain steps
 * @md_count:
 *
 * RETURN: 0-skip, 1-dec
 */
static int inline dec(struct t_motor_fix_info *fix, int remainer, int md_count)
{
    int ret;
    int index;
    int i, j;
    int dec_steps = 0;

    if (remainer == 0)
        return 0;

    for (i = 0; i < fix->dec_skip_num; i++)
        for (j = 0; j < fix->dec_per_section_counter; j++) {
            dec_steps += fix->dec_skip[i];

            if (dec_steps - 1 == md_count) {
                index = i;
                ret =  1;
                goto end;
            } else if (dec_steps > md_count) {
                index = i;
                ret = 0;
                goto end;
            }
        }

    index = i;
    ret = 1;

end:

    pr_debug("%s i:%d,r:%d,mc:%d\n", __func__, index, ret, md_count);
    return ret;
}

/*
 * min_speed - min speed
 *
 * @remainer:   remain steps
 * @counter:
 *
 * RETURN: 0-skip, 1-run
 */
static int inline min_speed(int remainer, int counter)
{
    if (remainer == 0)
        return 0;

    if ((counter % 4) == 0)
        return 1;

    return 0;
}

/*
 * is_move - check if need change pwm
 *
 * @runtime:        runtime information
 * @fix:            fix information
 * @total_steps:    total steps for this operation
 * @cur_steps:      current steps
 * @counter:
 *
 * RETURN: 0-skip, 1-run
 */
static int inline is_move(
    struct t_motor_runtime *runtime,
    struct t_motor_fix_info *fix,
    int total_steps,
    int cur_steps,
    int counter)
{
    int ret;
    int md;
    int remainter = total_steps - cur_steps;

    pr_debug("%s t:%d,c:%d\n", __func__, total_steps, cur_steps);

    if (total_steps <
        (fix->acc_skip_num * fix->acc_per_section_counter +
            fix->dec_skip_num * fix->dec_per_section_counter)) {
        md = 10;

        if (runtime->md != md) {
            runtime->md_count = 0;
            runtime->md = md;
        }

        ret = min_speed(total_steps - cur_steps, counter);
    } else if (cur_steps <= fix->acc_skip_num * fix->acc_per_section_counter) {
        md = 11;

        if (runtime->md != md) {
            runtime->md_count = 0;
            runtime->md = md;
            ret = 1;
        } else {
            ret = acc(fix, cur_steps, runtime->md_count);
            runtime->md_count++;
        }
    } else if (remainter <= fix->dec_skip_num * fix->dec_per_section_counter) {
        md = 12;

        if (runtime->md != md) {
            runtime->md_count = 0;
            runtime->md = md;
        }

        ret = dec(fix, total_steps - cur_steps, runtime->md_count);
        runtime->md_count++;
    } else {
        md = 13;

        if (runtime->md != md) {
            runtime->md_count = 0;
            runtime->md = md;
        }

        ret = 1;
    }

    return ret;
}

/*
 * output_next_pwm - pins output pwm
 *
 * @motor:  point to motor object
 *
 * RETURN: none
 */
static void output_next_pwm(struct t_motor *motor)
{
    int j, ctrl;
    struct t_motor_runtime *runtime = &motor->runtime;
    struct t_motor_fix_info *fix = &motor->fix;

    ctrl = fix->pwm_tbl_cw[runtime->pwm_index];

    if (runtime->cw == MOTOR_TURN_ANTICLKWISE)
        runtime->pwm_index =
            (runtime->pwm_index + fix->pwm_tbl_cw_num - 1) % fix->pwm_tbl_cw_num;
    else
        runtime->pwm_index = (runtime->pwm_index + 1) % fix->pwm_tbl_cw_num;

    for (j = 0; j < fix->pwm_pins_num; j++) {
        /*
         * because gpio_direction_output() hold mutex lock so cannot called in irq.
         * but gpiod_set_value() donnot hold mutex lock in tx pinctrl driver.
         * Todo : zhangning mark this
         */
        gpiod_set_value(gpio_to_desc(fix->pwm_pins[j]), !!((1<<j) & ctrl));
        // gpio_direction_output(fix->pwm_pins[j], !!((1<<j) & ctrl));
        //level = GPIO_OUT_EN | (!!((1<<j) & ctrl) ? GPIOD_OUT_HIGH : GPIOD_OUT_LOW);
        //pr_debug("%s gpio level=0x%x\n", __func__, level);
        //gpio_write(level, fix->pwm_pins[j]);
    }
}

/*
 * check_run_left_and_right - check if run left and right
 * @runtime:        pointer to runtime
 *
 * @RETURN: 0-no, 1-yes
 */
static int check_run_left_and_right(struct t_motor_runtime *runtime)
{
    if (runtime->move_cmd == MOTOR_CRUISE &&
        !runtime->set_stop &&
        runtime->status == MOTOR_STATUS_RUNNING)
        return 1;

    return 0;
}

/*
 * check_stop_new - check if need to stop now
 * @runtime:        pointer to runtime
 * @fix:            pointer to fix information
 *
 * @RETURN: 0-no, 1-yes
 */
static int check_stop_now(struct t_motor_runtime *runtime,
    struct t_motor_fix_info *fix)
{
    int check_pos = 1;

    if (runtime->move_cmd == MOTOR_MOVE_NOLIMIT)
        check_pos = 0;

    /*
     * stop conditions:
     * (1) no ramain steps
     * (2) user set stop
     * (3) current position hit boundary (except NOLIMT)
     */
    if (runtime->remain_steps <= 0 ||
        runtime->set_stop ||
        (check_pos &&
            runtime->cw == MOTOR_TURN_CLKWISE &&
            check_hit_max_boundary(runtime, fix)) ||
        (check_pos &&
            runtime->cw == MOTOR_TURN_ANTICLKWISE &&
            check_hit_min_boundary(runtime, fix)) ||
        runtime->status != MOTOR_STATUS_RUNNING)
        return 1;

    return 0;
}

/*
 * motor_hw_timer_handler - irq hander for hardware timer
 *
 * @data:       motor private data
 *
 * RETURN: 0-stop, 1-run
 */
static int motor_hw_timer_handler(void *data)
{
    struct t_motor *motor = (struct t_motor *)data;
    struct t_motor_runtime *runtime = &motor->runtime;
    struct t_motor_fix_info *fix = &motor->fix;
    //unsigned long flags;
    int j;
    //int num;
    int domore = 0;
    //int ctrl;

    spin_lock(&runtime->lock);
    /*1. check status*/
    if (check_run_left_and_right(runtime)) {
        /*for cruise, run left and right*/
        if ((runtime->cw == MOTOR_TURN_CLKWISE &&
                check_hit_max_boundary(runtime, fix)) ||
            (runtime->cw == MOTOR_TURN_ANTICLKWISE &&
                check_hit_min_boundary(runtime, fix))) {
            runtime->cw = MOTOR_TURN_ANTICLKWISE - runtime->cw;
            runtime->counter = 0;
            runtime->dest_steps = runtime->remain_steps = fix->total_steps;
        }
    } else if (check_stop_now(runtime, fix)) {
        /*be stoped or no remain steps, stop now*/
        pr_debug("motor will stop. steps=%d, status=%d\n", runtime->dest_steps, runtime->status);
        runtime->status = MOTOR_STATUS_STOPED;

        for (j = 0; j < fix->pwm_pins_num; j++) {
            /*
             * because gpio_direction_output() hold mutex lock so cannot called in irq.
             * but gpiod_set_value() donnot hold mutex lock in tx pinctrl driver.
             * Todo : zhangning mark this
             */
            gpiod_set_value(gpio_to_desc(fix->pwm_pins[j]), 0);
            //gpio_direction_output(fix->pwm_pins[j], 0);
            //gpio_write(GPIO_OUT_EN | GPIO_OUT_LOW, fix->pwm_pins[j]);
        }

        pr_debug("finish! remain_steps:%d\n", runtime->remain_steps);
        goto stop;
    }

    /*I am working*/
    domore = 1;

    /*2. check acc or dec*/
    runtime->counter++;

    if (!is_move(
            runtime,
            fix,
            runtime->dest_steps,
            runtime->dest_steps - runtime->remain_steps,
            runtime->counter)) {
        /*this timer period dont change pwm*/
        spin_unlock(&runtime->lock);
        return domore;
    }

    /*3. next pwm singal*/
    output_next_pwm(motor);

    /*4. set those step count*/
    runtime->remain_steps--;

    if (runtime->cw == MOTOR_TURN_CLKWISE)
        runtime->cur_step++;
    else
        runtime->cur_step--;
    spin_unlock(&runtime->lock);

    return domore;

stop:
    /*send stop signal TODO : zhangning */
    // TODO : zhangning
    //wtxe_up_all(&runtime->stop_wait);
    wake_up_all(&runtime->stop_wait);

    /*reset the statue*/
    runtime->set_stop = 0;
    runtime->status = MOTOR_STATUS_STOPED;
    runtime->wait_for_run = 0;

    /*stop the timer*/
    tx_timer_stop(motor->hw_timer);

    pr_debug("motor is stop now\n");

    /*triger other motor to start work*/
    if (motors_mutex)
        run_next_motor(motor);
    spin_unlock(&runtime->lock);

    return domore;
}

/*
 * motor_dev_parse - parse dev info
 *
 * @dev:        point to the device
 * @fix:        point to the fix
 * @runtime:    point to the runtime
 *
 * RETURN: 0-success, other-fail
 */
 static const struct of_device_id motor_of_match[];
static int motor_dev_parse(
    struct device *dev,
    struct t_motor_fix_info *fix,
    struct t_motor_runtime *runtime)
{
    char *part_number;
    int i;
    int num_pwm;
    int num_pwm_pins;
    struct t_motor_platdata *platdata;
	struct gpio_desc *motor_desc[4];
	char *name_motor[4] = {"motor1", "motor2", "motor3", "motor4"};

	const struct of_device_id *match;
	struct device_node *node = dev->of_node;

	match = of_match_node(motor_of_match, node);
	platdata = (struct t_motor_platdata *)match->data;

    part_number = platdata->part_number;
    if (part_number) {
        pr_info("part_number: %s\n", part_number);
        strcpy(fix->part_number, part_number);
    } else {
        fix->part_number[0] = '\0';
    }

    /*parse pwm-tbl-cw*/
    num_pwm = platdata->pwm_tbl_cw_num;
    for (i = 0; i < num_pwm; i++) {
        fix->pwm_tbl_cw[i] = platdata->pwm_tbl_cw[i];
    }

    fix->pwm_tbl_cw_num = i;

    pr_info("pwm_tbl_cw:\n");
    print_array_int(fix->pwm_tbl_cw, fix->pwm_tbl_cw_num);

    /*parse pwm-pins*/
	for (i = 0; i < 4; i++) {
		motor_desc[i] = devm_gpiod_get(dev, name_motor[i], GPIOD_OUT_HIGH);
		platdata->pwm_pins[i] = desc_to_gpio(motor_desc[i]);
	}

    num_pwm_pins = platdata->pwm_pins_num;
    for (i = 0; i < num_pwm_pins; i++) {
        fix->pwm_pins[i] = platdata->pwm_pins[i];

        /*
         * because cannot call gpio_direction_output in irq so called at here.
         * that set pin mux & direction in tx pinctrl.
         */
        gpio_direction_output(fix->pwm_pins[i], 0);
        //gpio_write(GPIO_OUT_EN | GPIO_OUT_LOW, fix->pwm_pins[i]);
    }

    fix->pwm_pins_num = i;

    pr_info("pwm_pins:\n");
    print_array_int(fix->pwm_pins, fix->pwm_pins_num);


    /*parse others property*/
    fix->steps_one_circle = platdata->steps_one_circle;
    fix->total_steps = platdata->total_steps;
    fix->boundary_steps = platdata->boundary_steps;
    fix->attach_timer = platdata->attach_timer;
    fix->index = platdata->index;
    runtime->speed_step = platdata->speed_step;

    pr_info("steps-one-circle:%d\n", fix->steps_one_circle);
    pr_info("total-steps:%d\n", fix->total_steps);
    pr_info("boundary-steps:%d\n", fix->boundary_steps);
    pr_info("attac-timer:%d\n", fix->attach_timer);
    pr_info("index:%d\n", fix->index);
    pr_info("speed-step:%d\n", runtime->speed_step);

    return 0;
}

/*
 * motor_register_cdev - register motor device
 *
 * @motor:      point to the motor
 *
 * RETURN: 0-success, other-fail
 */
static int motor_register_cdev(struct t_motor *motor)
{
    dev_t devno;
    struct device *dev;
    int err = 0;
    char name[24];

    if (!motor_major) {
        err = alloc_chrdev_region(&devno, 0, 4, "motor");

        if (err < 0) {
            pr_err("%s alloc chrdev region fail, index:%d\n",
                __func__, motor->fix.index);
            goto err_cdev;
        }

        motor_major = MAJOR(devno);
    }

    devno = MKDEV(motor_major, motor->fix.index);

    cdev_init(&motor->cdev, &tx_motor_fops);
    motor->cdev.owner = THIS_MODULE;
    err = cdev_add(&motor->cdev, devno, 1);

    if (err) {
        pr_err("%s err:%d adding cdevice err, index:%d\n",
            __func__, err, motor->fix.index);
        goto err_cdev_add;
    }

    /* auto mknod device node */
    sprintf(name, "motor%d", motor->fix.index);
    motor->class = class_create(THIS_MODULE, name);

    if (IS_ERR(motor->class)) {
        err = PTR_ERR(motor->class);
        pr_err("%s can't register class, index:%d\n",
            __func__, motor->fix.index);
        goto err_class_create;
    }

    dev = device_create(motor->class, NULL, devno, NULL, name);

    if (IS_ERR(dev)) {
        err = PTR_ERR(dev);
        pr_err("%s can't create device files, index:%d\n",
            __func__, motor->fix.index);
        goto err_device_create;
    }

    return 0;

err_device_create:
    class_destroy(motor->class);
err_class_create:
    cdev_del(&motor->cdev);
err_cdev_add:
	unregister_chrdev_region(devno, 4);
err_cdev:
    return err;
}

/*
 * motor_disregister_cdev - disregister motor device
 *
 * @motor:      point to the motor
 *
 * RETURN: 0-success, other-fail
 */
static int motor_disregister_cdev(struct t_motor *motor)
{
    dev_t devno = MKDEV(motor_major, motor->fix.index);
    device_destroy(motor->class, devno);
    class_destroy(motor->class);
    cdev_del(&motor->cdev);
	unregister_chrdev_region(devno, 4);
    return 0;
}

/*
 * tx_motor_probe - platform device probe
 *
 * @pdev:       point to platform device
 *
 * RETURN: 0-success, other-fail
 */
static int tx_motor_probe(struct platform_device *pdev)
{
    int ret;
	int timer_irq;
    struct t_motor *motor;
    // struct *t_motor_platdata;

    pr_err("%s, [%d]\n", __func__, __LINE__);

    /*alloc motor structure*/
    motor = devm_kzalloc(&pdev->dev, sizeof(struct t_motor), GFP_KERNEL);

    if (!motor) {
        pr_err("%s alloc the motor device fail.\n", __func__);
        return -ENOMEM;
    }

    /*set default*/
    motor_set_default_fix_info(&motor->fix);

    /*parse dev info*/
    ret = motor_dev_parse(&pdev->dev, &motor->fix, &motor->runtime);

    if (ret)
        return ret;

    /*motor structure init*/
    init_waitqueue_head(&motor->runtime.stop_wait);
    mutex_init(&motor->runtime.dev_mutex);
    motor->dev = &pdev->dev;
    motor->runtime.status = MOTOR_STATUS_STOPED;
    motor->user_count = 0;
    spin_lock_init(&motor->runtime.lock);

    /*register to cdev*/
    ret = motor_register_cdev(motor);

    if (ret)
        goto err_reg_cdev;

    /*get hardware timer*/
	timer_irq = platform_get_irq(pdev, 0);
    motor->hw_timer = tx_timer_probe(pdev, motor->fix.attach_timer, timer_irq);

    if (motor->hw_timer == NULL) {
        ret = -ENOENT;
        pr_err("get hw timer for ptz fail.\n");
        goto err_hw_timer;
    }

    /*set to drvdata*/
    platform_set_drvdata(pdev, motor);
    /*add motor to list*/
    list_add_tail(&motor->list, &motor_list);

    if (sys_init(motor)) {
        pr_err("%s sys_init fail\n", __func__);
        ret = -ENODEV;
        goto err_sys;
    }

    pr_debug("%s success.\n", __func__);
	pr_err("%s success.\n", __func__);

    return 0;

err_sys:
    list_del(&motor->list);
err_hw_timer:
    motor_disregister_cdev(motor);
err_reg_cdev:
    devm_kfree(motor->dev, motor);

    return ret;
}

/*
 * tx_motor_remove - platform device remove
 *
 * @pdev:       point to platform device
 *
 * RETURN: 0-success, other-fail
 */
static int tx_motor_remove(struct platform_device *pdev)
{
    struct t_motor *motor = platform_get_drvdata(pdev);

    pr_debug("%s %d\n", __func__, __LINE__);

    motor_sys_exit(motor);

    tx_timer_remove(motor->hw_timer);
    mutex_destroy(&motor->runtime.dev_mutex);
    tx_motor_dev_close(motor);
    motor_disregister_cdev(motor);

    list_del(&motor->list);
    devm_kfree(motor->dev, motor);
    return 0;
}

static struct t_motor_platdata t_motor_platdata_hor = {
    .part_number = "HZF-24BYJ48",
    .pwm_tbl_cw = {0x09, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08},
    .pwm_tbl_cw_num = 8,
    .pwm_pins = {0}, /*phase pin*/
    .pwm_pins_num = 4,
    .steps_one_circle = 4096,
    .total_steps = 2500,
    .boundary_steps = 50,
    .attach_timer = 2,
    .index = 0,
    .speed_step = 200,
};

static struct t_motor_platdata t_motor_platdata_ver = {
    .part_number = "HZF-24BYJ48",
    .pwm_tbl_cw = {0x09, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08},
    .pwm_tbl_cw_num = 8,
    .pwm_pins = {0}, /*phase pin*/
    .pwm_pins_num = 4,
    .steps_one_circle = 4096,
    .total_steps = 600,
    .boundary_steps = 50,
    .attach_timer = 3,
    .index = 1,
    .speed_step = 200,
};

static const struct of_device_id motor_of_match[] = {
	{ .compatible = "ts,tx-motor-hor",
		.data = &t_motor_platdata_hor },
	{ .compatible = "ts,tx-motor-ver",
		.data = &t_motor_platdata_ver },
	{},
};

/*driver of motor*/
static struct platform_driver tx_motor_driver = {
    .driver = {
        .name = "tx-motor",
        .bus = &platform_bus_type,
        .of_match_table = motor_of_match,
        .owner = THIS_MODULE,
    },
    .probe = tx_motor_probe,
    .remove = tx_motor_remove,
};

static int __init tx_motor_init(void)
{
	int result = 0;

	pr_info("tx_motor_init...\n");

	result = platform_driver_register(&tx_motor_driver);
	if (result < 0) {
    	pr_info("tx_motor_init error...\n");
    }

	return result;
}

static void __exit tx_motor_exit(void)
{
    platform_driver_unregister(&tx_motor_driver);
}

module_init(tx_motor_init);
module_exit(tx_motor_exit);

MODULE_DESCRIPTION("Motor driver");
MODULE_LICENSE("GPL");
