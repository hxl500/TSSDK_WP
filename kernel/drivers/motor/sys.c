/**
 * @copyright Copyright (c) 2022  Tsingmicro Intelligent Technology Co., Limited
 * @file sys.c
 * @brief
 *
 * @author ymc (yangmengchao@tsingmicro.com)
 * @version 1.0
 * @date 2022-10-14
 */

#include <linux/spinlock.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#include <linux/types.h>

#include "motor_internal_struct.h"


#define MOTOR_FIX_SHOW(fmt, field)                   \
    static ssize_t show_motor_##field(struct device *dev,  \
            struct device_attribute *attr,      \
            char *buf)              \
    {                           \
        struct t_motor *motor = dev_get_drvdata(dev);  \
        struct t_motor_fix_info *fix = &motor->fix; \
        return sprintf(buf, fmt"\n", fix->field); \
    }

#define MOTOR_RUNTIME_SHOW_INT(field)                   \
    static ssize_t show_motor_##field(struct device *dev,  \
            struct device_attribute *attr,      \
            char *buf)              \
    {                           \
        unsigned long ret;    \
        unsigned long flags;    \
        struct t_motor *motor = dev_get_drvdata(dev);  \
        struct t_motor_runtime *runtime = &motor->runtime; \
        spin_lock_irqsave(&runtime->lock, flags); \
        ret = sprintf(buf, "%d\n", runtime->field); \
        spin_unlock_irqrestore(&runtime->lock, flags); \
        return ret; \
    }

#define MOTOR_RUNTIME_STORE_INT(field)                   \
    static ssize_t store_motor_##field(struct device *dev,  \
            struct device_attribute *attr,      \
            const char *buf,              \
            size_t count)              \
    {                           \
        unsigned long flags;    \
        struct t_motor *motor = dev_get_drvdata(dev);  \
        struct t_motor_runtime *runtime = &motor->runtime; \
        unsigned long val = simple_strtoul(buf, NULL, 10); \
        spin_lock_irqsave(&runtime->lock, flags); \
        runtime->field = val; \
        spin_unlock_irqrestore(&runtime->lock, flags); \
        return count; \
    }

MOTOR_RUNTIME_STORE_INT(speed_step)
MOTOR_RUNTIME_SHOW_INT(speed_step)
MOTOR_RUNTIME_SHOW_INT(cur_step)

MOTOR_FIX_SHOW("%d", attach_timer)
MOTOR_FIX_SHOW("%d", total_steps)
MOTOR_FIX_SHOW("%d", boundary_steps)
MOTOR_FIX_SHOW("%s", part_number)

static ssize_t show_motor_gpio(struct device *dev,
                               struct device_attribute *attr,
                               char *buf)
{
    int i;
    char tmp[8];
    unsigned long ret = 0;
    struct t_motor *motor = dev_get_drvdata(dev);
    struct t_motor_fix_info *fix = &motor->fix;

    for (i = 0; i < fix->pwm_pins_num; i++) {
        ret += sprintf(tmp, "0x%02x ", fix->pwm_pins[i]);
        strcat(buf, tmp);
    }
    strcat(buf, "\n");
    ret++;

    return ret;
}

static ssize_t show_motor_pwm_tbl(struct device *dev,
                                  struct device_attribute *attr,
                                  char *buf)
{
    int i;
    char tmp[8];
    unsigned long ret = 0;
    struct t_motor *motor = dev_get_drvdata(dev);
    struct t_motor_fix_info *fix = &motor->fix;

    for (i = 0; i < fix->pwm_tbl_cw_num; i++) {
        ret += sprintf(tmp, "0x%02x ", fix->pwm_tbl_cw[i]);
        strcat(buf, tmp);
    }
    strcat(buf, "\n");
    ret++;

    return ret;
}

int sys_init(struct t_motor *motor)
{
    int rc = 0;

    pr_debug("%s %d\n", __func__, __LINE__);

    motor->sys.dev_attr_gpio.attr.name = "gpio";
    motor->sys.dev_attr_gpio.attr.mode = 0444;
    motor->sys.dev_attr_gpio.show = show_motor_gpio;
    motor->sys.dev_attr_gpio.store = NULL;

    motor->sys.dev_attr_pwm_tbl.attr.name = "pwm_tbl";
    motor->sys.dev_attr_pwm_tbl.attr.mode = 0444;
    motor->sys.dev_attr_pwm_tbl.show = show_motor_pwm_tbl;
    motor->sys.dev_attr_pwm_tbl.store = NULL;

    motor->sys.dev_attr_hw_timer.attr.name = "hw_timer";
    motor->sys.dev_attr_hw_timer.attr.mode = 0444;
    motor->sys.dev_attr_hw_timer.show = show_motor_attach_timer;
    motor->sys.dev_attr_hw_timer.store = NULL;

    motor->sys.dev_attr_speed_step.attr.name = "speed_step";
    motor->sys.dev_attr_speed_step.attr.mode = 0664;
    motor->sys.dev_attr_speed_step.show = show_motor_speed_step;
    motor->sys.dev_attr_speed_step.store = store_motor_speed_step;

    motor->sys.dev_attr_total_steps.attr.name = "total_steps";
    motor->sys.dev_attr_total_steps.attr.mode = 0444;
    motor->sys.dev_attr_total_steps.show = show_motor_total_steps;
    motor->sys.dev_attr_total_steps.store = NULL;

    motor->sys.dev_attr_current_pos.attr.name = "current_pos";
    motor->sys.dev_attr_current_pos.attr.mode = 0444;
    motor->sys.dev_attr_current_pos.show = show_motor_cur_step;
    motor->sys.dev_attr_current_pos.store = NULL;

    motor->sys.dev_attr_part_number.attr.name = "part_number";
    motor->sys.dev_attr_part_number.attr.mode = 0444;
    motor->sys.dev_attr_part_number.show = show_motor_part_number;
    motor->sys.dev_attr_part_number.store = NULL;

    motor->sys.dev_attr_boundary_steps.attr.name = "boundary_steps";
    motor->sys.dev_attr_boundary_steps.attr.mode = 0444;
    motor->sys.dev_attr_boundary_steps.show = show_motor_boundary_steps;
    motor->sys.dev_attr_boundary_steps.store = NULL;

    motor->sys.motor_attrs[0] = &motor->sys.dev_attr_gpio.attr;
    motor->sys.motor_attrs[1] = &motor->sys.dev_attr_pwm_tbl.attr;
    motor->sys.motor_attrs[2] = &motor->sys.dev_attr_hw_timer.attr;
    motor->sys.motor_attrs[3] = &motor->sys.dev_attr_speed_step.attr;
    motor->sys.motor_attrs[4] = &motor->sys.dev_attr_total_steps.attr;
    motor->sys.motor_attrs[5] = &motor->sys.dev_attr_current_pos.attr;
    motor->sys.motor_attrs[6] = &motor->sys.dev_attr_boundary_steps.attr;
    motor->sys.motor_attrs[7] = &motor->sys.dev_attr_part_number.attr;
    motor->sys.motor_attrs[8] = NULL;
    motor->sys.motor_attr_grp.attrs = motor->sys.motor_attrs;
    motor->sys.motor_attr_grp.name = "motor_attrs";

    rc = sysfs_create_group(&motor->dev->kobj, &motor->sys.motor_attr_grp);
    if (rc) {
        printk(KERN_ERR "%s: can't create sysfs group '%s'\n",
               __func__, motor->sys.motor_attr_grp.name);
        goto error;
    }

error:
    return rc;
}

int motor_sys_exit(struct t_motor *motor)
{
    pr_debug("%s %d\n", __func__, __LINE__);

    sysfs_remove_group(&motor->dev->kobj, &motor->sys.motor_attr_grp);
    memset(&motor->sys, 0, sizeof(struct sys_node));

    return 0;
}
