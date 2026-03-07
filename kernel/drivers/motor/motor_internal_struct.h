#ifndef __MOTOR_INTERNAL_STRUCT_H__
#define __MOTOR_INTERNAL_STRUCT_H__

#include <linux/device.h>
#include <linux/cdev.h>

/*
 * motor fix informations
 * @pwm_tbl_cw:     pwm table in clkwise
 * @pwm_tbl_cw_num: a number of pwm table
 * @pwm_pins_num:   also as a number of phase. 4phase, 2phase and so on.
 * @steps_one_circle:   steps in one circle (360 degree)
 *
 * @total_steps:    total steps in the direction
 * @boundary_steps: reserved stesp for boundaries
 * @attach_timer:   index of timer
 * @index:          index of the device file, motorX
 * @acc_skip:
 * @acc_skip_num:
 * @dec_skip:
 * @dec_skip_num:
 */
struct t_motor_fix_info {
    char part_number[64];
    int pwm_tbl_cw[24];
    int pwm_tbl_cw_num;
    int pwm_pins[16]; /*phase pin*/
    int pwm_pins_num;
    int steps_one_circle;

    int total_steps;
    int boundary_steps;
    int attach_timer;
    int index;

    int acc_skip[24];
    int acc_skip_num;
    int acc_per_section_counter;
    int dec_skip[24];
    int dec_skip_num;
    int dec_per_section_counter;
};

/*
 * motor runtime informations
 * @cw:     indicate clkwise or inticlkwise
 * @dest_steps: plan for turning number os steps
 * @remain_steps:   remain steps for this plan
 * @cur_steps:  cur position in steps
 * @speed_step:         speed in step(hz).
 * @counter:
 * @pwm_index:  point to pwm_tbl_cw
 * @status:     current status
 * @set_stop:   the signal for be stoped
 * @move_cmd:   the command
 * @wait_for_run:   wait for running if motors_mutex is valid
 * @md:
 * @md_count:
 * @dev_mutex:  protect for command
 * @lock:       protect for irq handler structure
 * @stop_wait:  waitting for the stopped
 */
struct t_motor_runtime {
    int cw;
    int dest_steps;
    int remain_steps;
    int cur_step;
    int speed_step;
    int counter;
    int pwm_index;
    int status;

    int set_stop;
    unsigned long move_cmd;

    int wait_for_run;
    int md;
    int md_count;

    struct mutex dev_mutex;
    spinlock_t lock;
    wait_queue_head_t stop_wait;
};

struct sys_node {
    struct device_attribute dev_attr_gpio;
    struct device_attribute dev_attr_pwm_tbl;
    struct device_attribute dev_attr_hw_timer;
    struct device_attribute dev_attr_speed_step;
    struct device_attribute dev_attr_total_steps;
    struct device_attribute dev_attr_current_pos;
    struct device_attribute dev_attr_boundary_steps;
    struct device_attribute dev_attr_part_number;

    struct attribute *motor_attrs[64];
    struct attribute_group motor_attr_grp;
};

struct t_motor {
    struct device *dev;
    struct cdev cdev;
    struct t_motor_runtime runtime;
    struct t_motor_fix_info fix;

    int user_count;

    struct class *class;
    struct list_head list;
    void *hw_timer;

    struct sys_node sys;
};

struct t_motor_platdata {
    char part_number[64];
    int pwm_tbl_cw[24];
    int pwm_tbl_cw_num;
    int pwm_pins[16]; /*phase pin*/
    int pwm_pins_num;
    int steps_one_circle;

    int total_steps;
    int boundary_steps;
    int attach_timer;
    int index;
    int speed_step;
};


#endif
