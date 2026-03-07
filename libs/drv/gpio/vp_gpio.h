#ifndef _VP_GPIO__H_
#define _VP_GPIO__H_
#include <linux/wait.h>
#include <linux/semaphore.h>
#include <linux/wait.h>
#include <linux/sched.h>

typedef enum
{
    VP_GPIO_PORT_A = 0,
    VP_GPIO_PORT_B,
    VP_GPIO_PORT_MAX,
} vp_gpio_port_t;

#define VP_GPIO_MAX_LEN 32

#define VP_GPIO_SET_FUNC 1
#define VP_GPIO_GET_FUNC 2
typedef enum
{
    VP_GPIO_FUNC_0 = 0x00,  /* GPIO FUNC 0 */
    VP_GPIO_FUNC_1,         /* GPIO FUNC 1 */
    VP_GPIO_FUNC_2,         /* GPIO FUNC 2 */
    VP_GPIO_FUNC_3,         /* GPIO FUNC 3 */
    VP_GPIO_FUNC_4,         /* GPIO FUNC 4 */
    VP_GPIO_FUNC_5,         /* GPIO FUNC 5 */
    VP_GPIO_FUNC_6,         /* GPIO FUNC 6 */
    VP_GPIO_FUNC_7,         /* GPIO FUNC 7 */
    VP_GPIO_FUNC_8,         /* GPIO FUNC 8 */
    VP_GPIO_OUTPUT_LOW,     /* 低电平输出, gpio func, out 0 */
    VP_GPIO_OUTPUT_HIG,     /* 高电平输出, gpio func, out 1 */
    VP_GPIO_INPUT_PULL_HIZ, /* 输入无拉, gpio func */
    VP_GPIO_INPUT_PULL_LOW, /* 输入下拉, gpio func */
    VP_GPIO_INPUT_PULL_HIG, /* 输入上拉, gpio func */
} vp_gpio_func_t;

#define VP_GPIO_SET_VALUE 3
#define VP_GPIO_GET_VALUE 4
typedef enum
{
    VP_GPIO_LOW = 0, /* 低电平 */
    VP_GPIO_HIG,     /* 高电平 */
} vp_gpio_value_t;

#define VP_GPIO_REG_IRQ 5
#define VP_GPIO_UNREG_IRQ 6
typedef enum
{
    VP_GPIO_IRQ_NONE, /* 无中断 */
    VP_GPIO_IRQ_LO,   /* 低电平触发中断 */
    VP_GPIO_IRQ_HI,   /* 高电平触发中断 */
    VP_GPIO_IRQ_FE,   /* 下降沿触发中断 */
    VP_GPIO_IRQ_RE,   /* 上升沿触发中断 */
    VP_GPIO_IRQ_EDGE, /* 边沿沿触发中断 */
} vp_gpio_irq_t;

#define VP_GPIO_SET_DIRIVE_STRENGET 7
#define VP_GPIO_GET_DIRIVE_STRENGET 8
typedef enum
{
    VP_GPIO_DS_UNKNOW = 0, /* 未知驱动能力 */
    VP_GPIO_DS_MIN,        /* GPIO最小驱动能力 2MA*/
    VP_GPIO_DS_LOW,        /* GPIO驱动能力低   2MA*/
    VP_GPIO_DS_MID,        /* GPIO驱动能力中   4MA*/
    VP_GPIO_DS_MAX         /* GPIO最大驱动能力 8MA*/
} vp_gpio_drv_level_t;

#define VP_GPIO_SET_PULL 9  /* only set pull, regard less gpio input or output */
#define VP_GPIO_GET_PULL 10
typedef enum
{
    VP_GPIO_PULL_NONE = 0, /* 默认配置 */
    VP_GPIO_PULL_HIZ,      /* GPIO禁用上拉和下拉 */
    VP_GPIO_PULL_DOWN,     /* GPIO下拉 */
    VP_GPIO_PULL_UP       /* GPIO上拉 */
} vp_gpio_pull_level_t;

#define VP_GPIO_REQ 11
#define VP_GPIO_UNREQ 12

#define VP_GPIO_GET_IRQ 13
/************************ JZ_GPIO_PIN_DEF STA ************************/
#ifndef GPIO_PA
#define GPIO_PA(n) (0 * 32 + n)
#endif
#ifndef GPIO_PB
#define GPIO_PB(n) (1 * 32 + n)
#endif
#ifndef GPIO_PC
#define GPIO_PC(n) (2 * 32 + n)
#endif
#ifndef GPIO_PD
#define GPIO_PD(n) (3 * 32 + n)
#endif
#ifndef GPIO_PIN
#define GPIO_PIN(dev, p, n) (dev->base[p] + n)
#endif
/************************ JZ_GPIO_PIN_DEF END ************************/

typedef struct
{
    uint8_t port;
    uint8_t pin;
    union
    {
        uint32_t irqs[VP_GPIO_PORT_MAX];
        vp_gpio_func_t func;
        vp_gpio_value_t value;
        vp_gpio_irq_t irq;
        vp_gpio_drv_level_t level;
        vp_gpio_pull_level_t pull;
    } data;
} vp_gpio_ioctl_t;

typedef struct
{
    uint32_t irq_id; /* 中断ID */
    uint8_t req : 2;
    uint8_t pin : 6;   /* GPIO脚位:0-31 */
    uint8_t port : 4;  /* GPIO端口:PA,PB,PC,PD */
    uint8_t value : 4; /* 脚位高低电平 */
    uint8_t pull : 4;  /* 上拉下拉使能状态 */
    uint8_t func : 4;  /* 功能定义 */
    uint8_t irq : 4;   /* 中断信息 */
    uint8_t level : 4; /* 驱动能力 */
} vp_gpio_config_t;

typedef struct
{
    wait_queue_head_t poll_wait;                               /* POLL 唤醒 */
    uint32_t irqs[VP_GPIO_PORT_MAX];                           /* 中断标记 */
    vp_gpio_config_t confs[VP_GPIO_PORT_MAX][VP_GPIO_MAX_LEN]; /* GPIO配置信息 */
    uint32_t base[VP_GPIO_PORT_MAX];
} vp_gpio_dev_t;
#endif