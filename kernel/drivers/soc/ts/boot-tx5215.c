#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/reboot.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>

#define TS_TX5215_SMU_IRQ_RAW_CFG      0x0
#define TS_TX5215_SMU_USR_GPR3         0x10C

#define BOOT_BIT0               (1 << 0)
#define BOOT_BIT1               (1 << 1)
#define BOOT_BIT2               (1 << 2)
#define BOOT_BIT3               (1 << 3)

#define IS_COLD_BOOT(x)         ((x) & BOOT_BIT0 && !((x) & (BOOT_BIT1 | BOOT_BIT2)))
#define IS_SOFT_REBOOT(x)       (((x) & (BOOT_BIT0 | BOOT_BIT1)) == (BOOT_BIT0 | BOOT_BIT1))
#define IS_WATCHDOG_REBOOT(x)   (((x) & (BOOT_BIT0 | BOOT_BIT2)) == (BOOT_BIT0 | BOOT_BIT2))
#define IS_PANIC_REBOOT(x)      ((x) & BOOT_BIT3)

#define USR_GPR3_DATA           0

struct boot_mode_data {
    char reason[32];
} boot_mode_data;

static void __iomem *smu_base;

void save_panic_reason(void)
{
    if (smu_base) {
        void __iomem *reg = smu_base + TS_TX5215_SMU_USR_GPR3;
        iowrite32(BOOT_BIT3, reg);
    } else {
        pr_err(KERN_ERR "SMU base address not initialized\n");
    }
}
EXPORT_SYMBOL(save_panic_reason);

static void load_boot_mode(void)
{
    void __iomem *reg;
    void __iomem *panic_reg;
    u32 reason;
    u32 panic_reason;
    
    reg = smu_base + TS_TX5215_SMU_IRQ_RAW_CFG;
    panic_reg = smu_base + TS_TX5215_SMU_USR_GPR3;

    reason = ioread32(reg);
    panic_reason = ioread32(panic_reg);

    //pr_info("boot_mode: reason=0x%08x, panic=0x%08x\n", reason, panic_reason);

    if (IS_PANIC_REBOOT(panic_reason)) {
        strcpy(boot_mode_data.reason, "panic");
        iowrite32(USR_GPR3_DATA, panic_reg);
        iowrite32(BOOT_BIT2, reg);
        iowrite32(BOOT_BIT1, reg);
        return;
    }

    if (IS_WATCHDOG_REBOOT(reason)) {
        strcpy(boot_mode_data.reason, "watchdog");
        iowrite32(BOOT_BIT2, reg);
    } else if (IS_SOFT_REBOOT(reason)) {
        strcpy(boot_mode_data.reason, "reboot");
        iowrite32(BOOT_BIT1, reg);
    } else if (IS_COLD_BOOT(reason)) {
        strcpy(boot_mode_data.reason, "coldboot");
    } else {
        strcpy(boot_mode_data.reason, "unknown");
    }
}

static ssize_t boot_mode_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%s\n", boot_mode_data.reason);
}

static struct kobj_attribute boot_mode_attr = __ATTR(boot_mode, 0444, boot_mode_show, NULL);

static int __init boot_mode_init(void)
{
    int ret;

    struct device_node *np = of_find_compatible_node(NULL, NULL, "ts,tx5215-smu");
    if (np) {
        smu_base = of_iomap(np, 0);
        of_node_put(np);
    }

    if (!smu_base) {
        pr_err(KERN_ERR "Failed to map SMU base address\n");
        return -ENODEV;
    }

    load_boot_mode();

    ret = sysfs_create_file(kernel_kobj, &boot_mode_attr.attr);
    if (ret) {
        pr_err("Failed to create sysfs file for boot mode\n");
        iounmap(smu_base);
        return ret;
    }

    return 0;
}

static void __exit boot_mode_exit(void)
{
    sysfs_remove_file(kernel_kobj, &boot_mode_attr.attr);
    if (smu_base) {
        iounmap(smu_base);
    }
}

module_init(boot_mode_init);
module_exit(boot_mode_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("TX5215 Boot Mode Module");
