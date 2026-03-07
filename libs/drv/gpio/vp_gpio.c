#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/sysfs.h>
#include <linux/regulator/consumer.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/poll.h>
#include "vp_gpio.h"
#include <asm/io.h>
#include <asm/bug.h>
#include <asm/cacheflush.h>
typedef enum
{
    DS_2_MA = 2,
    DS_3_MA,
    DS_4_MA,
    RESERVED5,
    DS_6_MA,
    RESERVED7,
    DS_8_MA,
    DS_9_MA,
    RESERVED10,
    DS_11_MA,
    DS_12_MA,
    RESERVED13,
    DS_14_MA,
    RESERVED15,
    DS_16_MA,
    DS_17_MA,
    RESERVED18,
    DS_19_MA,
    DS_20_MA,
    RESERVED21,
    DS_22_MA,
    RESERVED23,
    DS_24_MA,
} gpio_drv_level_t;


/*寄存器偏移地址*/
#define PXPDS0S 0x500 /* Port Driver-strength dr2r Register */
#define PXPDS1S 0x504 /* Port Driver-strength dr4r Register */
#define PXPDS2S 0x508 /* Port Driver-strength dr8r Register */
/*gpio open drain select*/
#define SCD_GPIO_ODR_REG		0x50c
/*gpio pull up select*/
#define SCD_GPIO_PUR_REG		0x510
/*gpio pull down select*/
#define SCD_GPIO_PDR_REG		0x514

/*gpio port control*/
#define SCD_GPIO_PCTL_REG		0x52c

#define GPIO_IOBASE 0xF0000000//GPIOA base
#define PAD_GROUP_A		0x0
#define PAD_GROUP_B		0x10000
#define PAD_GROUP_C		0x20000
#define PAD_GROUP_D		0x30000
#define PAD_GROUP_E		0x40000
#define PAD_GROUP_F		0x50000
#ifdef TX5215DV300_GPIO
#define PAD_GROUP_G		0x60000
#define PAD_GROUP_J		0x80000
#define PAD_GROUP_K		0x90000
#endif
static vp_gpio_dev_t *_gpio_dev = NULL;

#define VP_GPIO_PIN(dev, c) (dev->base[c->port] + c->pin)

char g_gpio_labels[VP_GPIO_PORT_MAX][16] = {
    "f2448000.gpio",
    "f244c000.gpio",
};

//Conversion between port and pin
//exp: gpio port = 0, pin = 0  -> GPIOA0  -> pad_offset = PAD_GROUP_A, reg_offset = 0 -> PA0
//     gpio port = 0, pin = 20 -> GPIOA20 -> pad_offset = PAD_GROUP_B, reg_offset = 4 -> PC4
static void pin_to_offset(unsigned int port, int pin, unsigned int *pad_offset,unsigned int *reg_offset)
{
    if (port == 0)//GPIOA -> PA0-7,PB0-7,PC0-7,PD0-7
    {
        switch (pin) {
            case 0 ... 7:
                *pad_offset = PAD_GROUP_A;
                *reg_offset = pin - 0;
                break;
            case 8 ... 15:
                *pad_offset = PAD_GROUP_B;
                *reg_offset = pin - 8;
                break;
            case 16 ... 23:
                *pad_offset = PAD_GROUP_C;
                *reg_offset = pin - 16;
                break;
#ifdef TX5215DV300_GPIO //TX5215DV300 A组GPIO 0-31 PA-PD
            case 24 ... 31:
                *pad_offset = PAD_GROUP_D;
                *reg_offset = pin - 24;
                break;
            default:
                *pad_offset = -1;//
                *reg_offset = -1;
                break;
#else
            case 24 ... 28:
                *pad_offset = PAD_GROUP_D;
                *reg_offset = pin - 24;
                break;
            case 29 ... 31:
                *pad_offset = PAD_GROUP_E;
                *reg_offset = pin - 29;
                break;
            default:
                *pad_offset = -1;//
                *reg_offset = -1;
                break;
#endif
        }
    } else if (port == 1){
        switch (pin){
#ifdef TX5215DV300_GPIO
        case 0 ... 7:
            *pad_offset = PAD_GROUP_E;
            *reg_offset = pin - 0;
            break;
        case 8 ... 11:
            *pad_offset = PAD_GROUP_F;
            *reg_offset = pin - 8;
            break;
        case 12:
            *pad_offset = PAD_GROUP_J;
            *reg_offset = pin - 6;//GPIOB12 --> PJ6，你真特殊
            break;
        case 13 ... 15:
            *pad_offset = PAD_GROUP_F;
            *reg_offset = pin - 8;
            break;
        case 16 ... 18:
            *pad_offset = PAD_GROUP_G;
            *reg_offset = pin - 16;
            break;
        case 19 ... 24:
            *pad_offset = PAD_GROUP_J;
            *reg_offset = pin - 19;
            break;
        case 25 ... 31:
            *pad_offset = PAD_GROUP_K;
            *reg_offset = pin - 25;
            break;
        default:
            *pad_offset = -1;
            *reg_offset = -1;
            break;
#else
        case 0 ... 2:
            *pad_offset = PAD_GROUP_E;
            *reg_offset = pin + 3 - 0;
            break;
        case 3 ... 9:
            *pad_offset = PAD_GROUP_F;
            *reg_offset = pin - 3;
            break;
        default:
            *pad_offset = -1;
            *reg_offset = -1;
            break;
#endif
        }
    }
    if (*pad_offset == -1 || *reg_offset == -1) {
        pr_err("pin_to_offset: invalid pin %d for port %d\n", pin, port);
    }
}
#if 0
static irqreturn_t vp_gpio_isr(int irq, void *data)
{
    uint32_t irq_value = 0;
    uint32_t irq_bit;
    vp_gpio_config_t *confs = (vp_gpio_config_t *)data;
    if (_gpio_dev == NULL || confs == NULL)
        return IRQ_HANDLED;
    irq_bit = (1L << confs->pin);
    confs->value = gpio_get_value_cansleep(VP_GPIO_PIN(confs));
    irq_value = _gpio_dev->irqs[confs->port] & irq_bit;
    if (irq_value)
        return IRQ_HANDLED;
    if ((confs->irq == VP_GPIO_IRQ_FE || confs->irq == VP_GPIO_IRQ_LO) && confs->value != 0)
        return IRQ_HANDLED;
    if ((confs->irq == VP_GPIO_IRQ_RE || confs->irq == VP_GPIO_IRQ_HI) && confs->value != 1)
        return IRQ_HANDLED;
    _gpio_dev->irqs[confs->port] |= irq_bit;

    wake_up(&_gpio_dev->poll_wait);

    return IRQ_HANDLED;
}
#endif

/*-------------------------------------------------------------------------*/

static ssize_t vp_gpio_read(struct file *file, char __user *buf, size_t count,
                            loff_t *offset)
{
    vp_gpio_dev_t *dev = file->private_data;
    (void)dev;
    // TODO 接收GPIO脉冲信号
    return -EPERM;
}

static ssize_t vp_gpio_write(struct file *file, const char __user *buf,
                             size_t count, loff_t *offset)
{
    vp_gpio_dev_t *dev = file->private_data;
    (void)dev;
    // TODO 发送GPIO脉冲信号
    return -EPERM;
}

static int gpiochip_match_name(struct gpio_chip *chip, void *data)
{
	const char *name = data;
	return !strcmp(chip->label, name);
}

static struct gpio_chip *find_chip_by_name(const char *name)
{
	return gpiochip_find((void *)name, gpiochip_match_name);
}

static int vp_gpio_open(struct inode *inode, struct file *file)
{
    vp_gpio_dev_t *dev = kzalloc(sizeof(vp_gpio_dev_t), GFP_KERNEL);
    struct gpio_chip *chip = NULL;
    int p;

    if (IS_ERR_OR_NULL(dev))
    {
        pr_err("vp_gpio_open: _gpio_dev is ERR_PTR %ld\n", PTR_ERR(_gpio_dev));
        return -ENOMEM;
    }
    memset(dev, 0, sizeof(vp_gpio_dev_t));
    init_waitqueue_head(&dev->poll_wait);
    file->private_data = dev;
    _gpio_dev = dev;

    for (p = 0; p < VP_GPIO_PORT_MAX; p++) {
            chip = find_chip_by_name(g_gpio_labels[p]);
            if (!chip) {
                pr_err("Failed to get GPIO chip\n");
                return -2;
            } else {
                printk("gpio%d base%d\n", p, chip->base);
                dev->base[p] = chip->base;
            }
    }
    return 0;
}

static int vp_gpio_release(struct inode *inode, struct file *file)
{
    vp_gpio_config_t *confs = NULL;
    vp_gpio_dev_t *dev = file->private_data;

    int p, n;
    for (p = 0; p < VP_GPIO_PORT_MAX; p++)
    {
        confs = _gpio_dev->confs[p];
        for (n = 0; n < VP_GPIO_MAX_LEN; n++)
        {
            if (confs[n].irq != VP_GPIO_IRQ_NONE)
            {
                disable_irq_nosync(confs[n].irq_id);
                free_irq(confs[n].irq_id, &confs[n]);
            }
            if (confs->req)
            {
                gpio_free(GPIO_PIN(dev, confs[n].port, confs[n].pin));
            }
        }
    }
    wake_up(&_gpio_dev->poll_wait);
    _gpio_dev = NULL;
    kfree(file->private_data);//free
    file->private_data = NULL;
    return 0;
}

#define GPIO_ADDR(reg) ((volatile unsigned long *)(reg))

static inline void gpio_write(int val, unsigned long reg)
{
    *GPIO_ADDR(reg) = val;
}
//port : GPIOA -> 0, GPIOB -> 1
static int jz_gpio_set_driver_strength(vp_gpio_port_t port, unsigned int pin, int ds)
{
    int val;
    int i;
    unsigned int pad_offset, reg_offset;
    void __iomem *base;

    pin_to_offset(port,pin,&pad_offset,&reg_offset);
    if (pad_offset == -1 || reg_offset == -1) {
        pr_err("Invalid pin offset\n");
        return -EINVAL;
    }
    base = ioremap(GPIO_IOBASE + pad_offset,0x100000);
    if (!base) {
        pr_err("ioremap failed for GPIO strength setting\n");
        return -ENOMEM;
    }
    // printk(KERN_INFO "jz_gpio_set_driver_strength: port %d, pin %d, ds %d\n", port, pin, ds);
    if (ds < DS_2_MA)
    {
        return -1;
    }
    else if (DS_2_MA == ds)
    {
        //clear other driver strength
        for(i = 0; i < 3; i++){
            int mode_offset = i * 0x4 + PXPDS0S;
            val = readl((void __iomem *)(base + mode_offset));
            val &= ~(1 << reg_offset);
            writel(val,(void __iomem *)( base + mode_offset));
        }

        //set driver strength
        val = readl((void __iomem *)(base + PXPDS0S));
        val |= 1 << reg_offset;
        writel(val,(void __iomem *)( base + PXPDS0S));
    }
    else if (DS_4_MA == ds)
    {
        //clear other driver strength
        for(i = 0; i < 3; i++){
            int mode_offset = i * 0x4 + PXPDS0S;
            val = readl((void __iomem *)(base + mode_offset));
            val &= ~(1 << reg_offset);
            writel(val,(void __iomem *)( base + mode_offset));
        }
        //set driver strength
        val = readl((void __iomem *)(base + PXPDS1S));
        val |= 1 << reg_offset;
        writel(val,(void __iomem *)( base + PXPDS1S));
    }
    else if (DS_8_MA == ds)
    {
        //clear other driver strength
        for(i = 0; i < 3; i++){
            int mode_offset = i * 0x4 + PXPDS0S;
            val = readl((void __iomem *)(base + mode_offset));
            val &= ~(1 << reg_offset);
            writel(val,(void __iomem *)( base + mode_offset));
        }
        //set driver strength
        val = readl((void __iomem *)(base + PXPDS2S));
        val |= 1 << reg_offset;
        writel(val,(void __iomem *)( base + PXPDS2S));
    }else{
        return -1;
    }
    iounmap(base);
    return 0;
}

static int vp_gpio_request(vp_gpio_dev_t *dev, vp_gpio_config_t *conf)
{
    uint32_t gpio = VP_GPIO_PIN(dev, conf);
    int ret;
    char lable[32];
    memset(lable, 0, sizeof(lable));
    sprintf(lable, "VP_GPIO_%d_%d", conf->port, conf->pin);
    ret = gpio_request(gpio, NULL);
    if (ret)
    {
        printk("vp_gpio_request:%d error:%d\n", gpio, ret);
        return ret;
    }

    conf->req = 1;
    return 0;
}

void jzgpio_set_func_gpio(void __iomem *base, unsigned int reg_offset)
{
    unsigned int val;

    val = readl((void __iomem *)(base + SCD_GPIO_PCTL_REG ));
    reg_offset *= 4;
    val &= ~(0xf << reg_offset);
    val |= (VP_GPIO_FUNC_8 << reg_offset);
    writel(val, base + SCD_GPIO_PCTL_REG );
}

int jzgpio_set_func(vp_gpio_dev_t *dev, int port, int pin, int func, int only_pull)
{
    unsigned int val;
    unsigned int pad_offset, reg_offset;
    void __iomem *base;

    pin_to_offset(port, pin, &pad_offset, &reg_offset);

    if (pad_offset == -1 || reg_offset == -1) {
        pr_err("Invalid pin offset\n");
        return -EINVAL;
    }
    //pr_info("%s port:%d, func:%d, pin:%d, pad_offset:%d,reg_offset:%d\n",
    //    __func__, port, func, pin, pad_offset, reg_offset);

    base = ioremap(GPIO_IOBASE + pad_offset,0x100000);
    if (!base) {
        pr_err("ioremap failed in jzgpio_set_func\n");
        return -ENOMEM;
    }
    switch (func) {
        case VP_GPIO_FUNC_0:
            val = readl((void __iomem *)(base + SCD_GPIO_PCTL_REG));
            // printk(KERN_DEBUG "readl val:%#x\n",val);
            reg_offset *= 4;//
            val &= ~(0xf << reg_offset);
            val |= (VP_GPIO_FUNC_0 << reg_offset);
            // printk(KERN_DEBUG "writel val:%#x\n",val);
            writel(val, base + SCD_GPIO_PCTL_REG);
            break;
        case VP_GPIO_FUNC_1:
            val = readl((void __iomem *)(base + SCD_GPIO_PCTL_REG));
            reg_offset *= 4;
            val &= ~(0xf << reg_offset);
            val |= (VP_GPIO_FUNC_1 << reg_offset);
            writel(val, base + SCD_GPIO_PCTL_REG);
            break;
        case VP_GPIO_FUNC_2:
            val = readl((void __iomem *)(base + SCD_GPIO_PCTL_REG ));
            reg_offset *= 4;
            val &= ~(0xf << reg_offset);
            val |= (VP_GPIO_FUNC_2 << reg_offset);
            writel(val, base + SCD_GPIO_PCTL_REG);
            break;
        case VP_GPIO_FUNC_3:
            val = readl((void __iomem *)(base + SCD_GPIO_PCTL_REG ));
            reg_offset *= 4;
            val &= ~(0xf << reg_offset);
            val |= (VP_GPIO_FUNC_3 << reg_offset);
            writel(val, base + SCD_GPIO_PCTL_REG );
            break;
        case VP_GPIO_FUNC_4:
            val = readl((void __iomem *)(base + SCD_GPIO_PCTL_REG ));
            reg_offset *= 4;
            val &= ~(0xf << reg_offset);
            val |= (VP_GPIO_FUNC_4 << reg_offset);
            writel(val, base + SCD_GPIO_PCTL_REG);
            break;
        case VP_GPIO_FUNC_5:
            val = readl((void __iomem *)(base + SCD_GPIO_PCTL_REG ));
            reg_offset *= 4;
            val &= ~(0xf << reg_offset);
            val |= (VP_GPIO_FUNC_5 << reg_offset);
            writel(val, base + SCD_GPIO_PCTL_REG );
            break;
        case VP_GPIO_FUNC_6:
            val = readl((void __iomem *)(base + SCD_GPIO_PCTL_REG ));
            reg_offset *= 4;
            val &= ~(0xf << reg_offset);
            val |= (VP_GPIO_FUNC_6 << reg_offset);
            writel(val, base + SCD_GPIO_PCTL_REG );
            break;
        case VP_GPIO_FUNC_7:
            val = readl((void __iomem *)(base + SCD_GPIO_PCTL_REG ));
            reg_offset *= 4;
            val &= ~(0xf << reg_offset);
            val |= (VP_GPIO_FUNC_7 << reg_offset);
            writel(val, base + SCD_GPIO_PCTL_REG );
            break;
        case VP_GPIO_FUNC_8:
            jzgpio_set_func_gpio(base, reg_offset);
            break;
        case VP_GPIO_OUTPUT_LOW: /* 低电平输出 */
            gpio_direction_output(GPIO_PIN(dev, port, pin), 0);
            jzgpio_set_func_gpio(base, reg_offset);
            break;
        case VP_GPIO_OUTPUT_HIG: /* 高电平输出 */
            gpio_direction_output(GPIO_PIN(dev, port, pin), 1);
            jzgpio_set_func_gpio(base, reg_offset);
            break;
        case VP_GPIO_INPUT_PULL_HIZ:
            // PUR clear 0
            val = readl((void __iomem *)(base + SCD_GPIO_PUR_REG ));
            val &= ~(1 << reg_offset);
            writel(val , base + SCD_GPIO_PUR_REG );
            //PDR clear 1
            val = readl((void __iomem *)(base + SCD_GPIO_PDR_REG ));
            val &= ~(1 << reg_offset);
            writel(val , base + SCD_GPIO_PDR_REG );
            if (!only_pull) {
                gpio_direction_input(GPIO_PIN(dev, port, pin));
                jzgpio_set_func_gpio(base, reg_offset);
            }
            break;
        case VP_GPIO_INPUT_PULL_LOW:
            // PUR clear 0
            val = readl((void __iomem *)(base + SCD_GPIO_PUR_REG ));
            val &= ~(1 << reg_offset);
            writel(val , base + SCD_GPIO_PUR_REG );
            //PDR set 1
            val = readl(base + SCD_GPIO_PDR_REG);
            val |= 1 << reg_offset;
            writel(val, base + SCD_GPIO_PDR_REG);
            if (!only_pull) {
                gpio_direction_input(GPIO_PIN(dev, port, pin));
                jzgpio_set_func_gpio(base, reg_offset);
            }
            break;
        case VP_GPIO_INPUT_PULL_HIG:
            val = readl((void __iomem *)(base + SCD_GPIO_PDR_REG ));
            val &= ~(1 << reg_offset);
            writel(val , base + SCD_GPIO_PDR_REG );

            val = readl(base + SCD_GPIO_PUR_REG);
            val |= 1 << reg_offset;
            writel(val, base + SCD_GPIO_PUR_REG);
            if (!only_pull) {
                gpio_direction_input(GPIO_PIN(dev, port, pin));
                jzgpio_set_func_gpio(base, reg_offset);
            }
            break;
        default:
            return -EINVAL;
    }
    iounmap(base);

    return 0;
}
static int vp_gpio_set_pull(vp_gpio_dev_t *dev, vp_gpio_config_t *conf,
    vp_gpio_pull_level_t pull)
{
    uint8_t jz_pull = 0;
    int ret;
    switch (pull)
    {
    case VP_GPIO_PULL_HIZ:
        //由于设置function和上下拉使用同一个jzgpio_set_func接口，所以偏移10
        jz_pull = VP_GPIO_INPUT_PULL_HIZ;
        break;
    case VP_GPIO_PULL_DOWN:
        jz_pull = VP_GPIO_INPUT_PULL_LOW;
        break;
    case VP_GPIO_PULL_UP:
        jz_pull = VP_GPIO_INPUT_PULL_HIG;
        break;
    default:
        return -1;
    }

    ret = jzgpio_set_func(dev, conf->port, (conf->pin), jz_pull, 1);
    if (ret == 0)
        conf->pull = pull;
    return ret;
}
static int vp_gpio_set_func(vp_gpio_dev_t *dev, vp_gpio_config_t *conf,
    vp_gpio_func_t func)
{
    // uint8_t jz_pull = 0, jz_func = 0, pull = 0;
    vp_gpio_func_t jz_func = 0;
    vp_gpio_pull_level_t pull = 0;
    int ret;

    switch (func)
    {
    case VP_GPIO_FUNC_0:
        jz_func = VP_GPIO_FUNC_0;
        break;
    case VP_GPIO_FUNC_1:
        jz_func = VP_GPIO_FUNC_1;
        break;
    case VP_GPIO_FUNC_2:
        jz_func = VP_GPIO_FUNC_2;
        break;
    case VP_GPIO_FUNC_3:
        jz_func = VP_GPIO_FUNC_3;
        break;
    case VP_GPIO_FUNC_4:
        jz_func = VP_GPIO_FUNC_4;
        break;
    case VP_GPIO_FUNC_5:
        jz_func = VP_GPIO_FUNC_5;
        break;
    case VP_GPIO_FUNC_6:
        jz_func = VP_GPIO_FUNC_6;
        break;
    case VP_GPIO_FUNC_7:
        jz_func = VP_GPIO_FUNC_7;
        break;
    case VP_GPIO_FUNC_8:
        jz_func = VP_GPIO_FUNC_8;
        break;
    case VP_GPIO_OUTPUT_LOW: /* 低电平输出 */
        jz_func = VP_GPIO_OUTPUT_LOW;
        break;
    case VP_GPIO_OUTPUT_HIG: /* 高电平输出 */
        jz_func = VP_GPIO_OUTPUT_HIG;
        break;
    case VP_GPIO_INPUT_PULL_HIZ: /* 输入无拉 */
        jz_func = VP_GPIO_INPUT_PULL_HIZ;
        //jz_pull = VP_GPIO_PULL_HIZ;//unuse, to eliminate compiler warnings
        pull = VP_GPIO_PULL_HIZ;
        break;
    case VP_GPIO_INPUT_PULL_LOW: /* 输入下拉 */
        jz_func = VP_GPIO_INPUT_PULL_LOW;
        // jz_pull = GPIO_PULL_DOWN;
        pull = VP_GPIO_PULL_DOWN;
        break;
    case VP_GPIO_INPUT_PULL_HIG: /* 输入上拉 */
        jz_func = VP_GPIO_INPUT_PULL_HIG;
        // jz_pull = GPIO_PULL_UP;
        pull = VP_GPIO_PULL_UP;
        break;
    default:
        ret = -EINVAL;
        break;
    }
    ret = jzgpio_set_func(dev, conf->port, (conf->pin), jz_func, 0);
    if (ret == 0)
    {
        conf->func = func;
        if (pull != 0)
            conf->pull = pull;
    }
    return ret;
}
static long vp_gpio_ioctl(struct file *file, unsigned code, unsigned long value)
{
    vp_gpio_dev_t *dev = file->private_data;
    vp_gpio_ioctl_t ioctl;
    vp_gpio_config_t *conf;
    uint32_t gpio;
    int ret = 0;

    if (copy_from_user(&ioctl, (void __user *)value, sizeof(vp_gpio_ioctl_t))){
        ret = -1; goto err;
    }
    if (ioctl.port >= VP_GPIO_PORT_MAX || ioctl.pin >= VP_GPIO_MAX_LEN)
    {
        printk("gpio port:%d pin:%d error, max port:%d max pin:%d \n",
            ioctl.port, ioctl.pin, VP_GPIO_PORT_MAX - 1, VP_GPIO_MAX_LEN - 1);
       ret = -2; goto err;
    }
    gpio = GPIO_PIN(dev, ioctl.port, ioctl.pin);
    conf = &dev->confs[ioctl.port][ioctl.pin];
    conf->port = ioctl.port;
    conf->pin = ioctl.pin;
    switch (code)
    {
    case VP_GPIO_REQ://请求GPIO资源
    {
        if (conf->req == 0) {
            ret = vp_gpio_request(dev, conf);
            if (ret) {
                pr_err("request err %d\n", ret);
                ret = -3; goto err;
            }
        }
    }
    break;
    case VP_GPIO_UNREQ://释放GPIO资源
    {
        if (conf->req)
            gpio_free(gpio);
        conf->req = 0;
    }
    break;
    case VP_GPIO_SET_FUNC://设置GPIO功能，输入/输出等等
    {
        vp_gpio_func_t func = ioctl.data.func;
        if (func > VP_GPIO_INPUT_PULL_HIG)
        {
            ret = -4; goto err;
            break;
        }
        ret = vp_gpio_set_func(dev, conf, func);
        if (ret)
        {
            ret = -5; goto err;
            break;
        }
    }
    break;
    case VP_GPIO_GET_FUNC://获取当前GPIO功能
    {
        ioctl.data.func = conf->func;
        if (copy_to_user((void __user *)value, &ioctl, sizeof(vp_gpio_ioctl_t))) {
            ret = -6; goto err;
        }
    }
    break;
    case VP_GPIO_SET_PULL://设置GPIO上拉/下拉
    {
        int ret;
        vp_gpio_pull_level_t pull = ioctl.data.pull;
        if (pull > VP_GPIO_PULL_UP)
        {
            ret = -7; goto err;
            break;
        }
        ret = vp_gpio_set_pull(dev, conf, pull);
        if (ret)
        {
            ret = -8; goto err;
            break;
        }
    }
    break;
    case VP_GPIO_GET_PULL://获取当前GPIO上拉/下拉
    {
        ioctl.data.pull = conf->pull;
        if (copy_to_user((void __user *)value, &ioctl, sizeof(vp_gpio_ioctl_t))) {
            ret = -9; goto err;
        }
    }
    break;
    case VP_GPIO_SET_DIRIVE_STRENGET://设置当前GPIO驱动强度
    {
        vp_gpio_drv_level_t level = ioctl.data.level;
        int ret;
        switch (level)
        {
        case VP_GPIO_DS_MIN:
            ret = jz_gpio_set_driver_strength(conf->port, conf->pin, DS_2_MA);
            break;
        case VP_GPIO_DS_LOW:
            ret = jz_gpio_set_driver_strength(conf->port, conf->pin, DS_2_MA);
            break;
        case VP_GPIO_DS_MID:
            ret = jz_gpio_set_driver_strength(conf->port, conf->pin, DS_4_MA);
            break;
        case VP_GPIO_DS_MAX:
            ret = jz_gpio_set_driver_strength(conf->port, conf->pin, DS_8_MA);
            break;
        default:
            ret = -10; goto err;
            break;
        }
        if (ret)
        {
            pr_err("set driver strength err %d\n", ret);
            ret = -11; goto err;
            break;
        }
        conf->level = level;
    }
    break;
    case VP_GPIO_GET_DIRIVE_STRENGET://获取当前GPIO驱动强度
    {
        ioctl.data.level = conf->level;
        if (copy_to_user((void __user *)value, &ioctl, sizeof(vp_gpio_ioctl_t))) {
            ret = -12; goto err;
        }
    }
    break;
    case VP_GPIO_SET_VALUE://设置GPIO电平
    {
        if (conf->req == 0) {
            ret = -13; goto err;
        }
        switch (ioctl.data.value)
        {
        case VP_GPIO_LOW:
            gpio_set_value(gpio, 0);
            break;
        case VP_GPIO_HIG:
            gpio_set_value(gpio, 1);
            break;
        default:
            ret = -14; goto err;
            break;
        }
        conf->value = ioctl.data.value;
    }
    break;
    case VP_GPIO_GET_VALUE:
    {
        int ret;
        if (conf->req == 0) {
            ret = -16; goto err;
        }
        ret = gpio_get_value(gpio);
        if (ret < 0)
        {
            pr_err("get gpio value err %d\n", ret);
            ret = -17; goto err;
        }
        conf->value = (ret == 0 ? VP_GPIO_LOW : VP_GPIO_HIG);
        ioctl.data.value = conf->value;
        if (copy_to_user((void __user *)value, &ioctl, sizeof(vp_gpio_ioctl_t))) {
            ret = -18; goto err;
        }
    }
    break;
#if 0
    case VP_GPIO_GET_IRQ:
    {
        if (conf->req == 0)
            return -EINVAL;
        memcpy(ioctl.data.irqs, dev->irqs, sizeof(dev->irqs));
        if (copy_to_user((void __user *)value, &ioctl, sizeof(vp_gpio_ioctl_t)))
            status = -EFAULT;
        memset(dev->irqs, 0, sizeof(dev->irqs));
    }
    break;

    case VP_GPIO_REG_IRQ://注册中断
    {
        int irq_id, irq_flag, ret;
        vp_gpio_irq_t irq = ioctl.data.irq;
        if (conf->req == 0)
            return -EINVAL;
        switch (irq)
        {
        case VP_GPIO_IRQ_LO:
            irq_flag = IRQF_TRIGGER_LOW;
            break;
        case VP_GPIO_IRQ_HI:
            irq_flag = IRQF_TRIGGER_HIGH;
            break;
        case VP_GPIO_IRQ_FE:
            irq_flag = IRQF_TRIGGER_FALLING;
            break;
        case VP_GPIO_IRQ_RE:
            irq_flag = IRQF_TRIGGER_RISING;
            break;
        case VP_GPIO_IRQ_EDGE:
            ret = 0;
            irq_flag = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING;
            break;
        default:
            ret = -EINVAL;
            break;
        }
        irq_id = gpio_to_irq(gpio);
        if (irq_id < 0)
        {
            status = -EFAULT;
            break;
        }
        ret = request_irq(irq_id, vp_gpio_isr, IRQF_NO_SUSPEND | irq_flag, "vp_gpio_intr", (void *)conf);
        if (ret)
        {
            status = -EFAULT;
            break;
        }
        conf->irq_id = irq_id;
        conf->irq = irq;
        disable_irq(irq_id);
        enable_irq(irq_id);
    }
    break;
    case VP_GPIO_UNREG_IRQ://反注册中断
    {
        vp_gpio_irq_t irq = conf->irq;
        int irq_id = conf->irq_id;
        if (irq == VP_GPIO_IRQ_NONE)
            break;
        disable_irq_nosync(irq_id);
        free_irq(irq_id, conf);
        conf->irq = VP_GPIO_IRQ_NONE;
    }
    break;
#endif
    default:
        ret = -19; goto err;
        break;
    }
    return 0;

err:
    pr_err("%s error %d\n", __func__, ret);
    return ret;
}

unsigned int vp_gpio_poll(struct file *file, struct poll_table_struct *wait)
{
    vp_gpio_dev_t *dev = file->private_data;
    unsigned int mask = 0;
    if (_gpio_dev == NULL || dev == NULL)
        return POLLERR;
    poll_wait(file, &dev->poll_wait, wait); // 不会立即休眠
    if (_gpio_dev == NULL || dev == NULL)
        return POLLERR;
    if (dev->irqs) // 判断是否有数据
        mask = POLLIN | POLLPRI;
    return mask;
}

/*-------------------------------------------------------------------------*/

static const char gpio_shortname[] = "vp_gpio";

static const struct file_operations vp_gpio_fops = {
    .owner = THIS_MODULE,
    .read = vp_gpio_read,
    .write = vp_gpio_write,
    .open = vp_gpio_open,
    .poll = vp_gpio_poll,
    .release = vp_gpio_release,
    .unlocked_ioctl = vp_gpio_ioctl,
    .compat_ioctl = vp_gpio_ioctl};

static struct miscdevice vp_gpio_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = gpio_shortname,
    .fops = &vp_gpio_fops,
};

static int __init vp_gpio_init(void)
{
    int ret;

    printk("gpio driver ver:2.1\n");
    ret = misc_register(&vp_gpio_device);
    if (ret)
        return ret;

    return 0;
}

module_init(vp_gpio_init);

static void __exit vp_gpio_deinit(void)
{
    misc_deregister(&vp_gpio_device);
}
module_exit(vp_gpio_deinit);
MODULE_DESCRIPTION("VP GPIO to JZ GPIO");
MODULE_LICENSE("GPL");