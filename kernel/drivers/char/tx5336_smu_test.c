#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/of_platform.h>
#include <linux/delay.h>

static void __iomem *smu_base_reg;

#define SMU_IRQ_RAW              0x000
#define SMU_IRQ_MASK             0x004
#define SMU_IRQ_STAT             0x008
#define CHIP_PWR_MODE_CFG        0x00C
#define SMU_GLB_SOFT_RST         0x010
#define SMU_LP_TMR_CFG           0x014
#define XTAL_CFG                 0x020
#define TOP_PWR_SWI_TIME         0x030
#define CHIP_WAKE_UP_CFG         0x040
#define GPIO_INT_MODE            0x050
#define AON_GPIO_CTL             0x060
#define CHIP_STATUS              0x070
#define RTC_CLK_CFG              0x080
#define RTC_RST_CTL              0x084
#define IR_CTL                   0x088
#define RC32K_OSC_TRIM           0x08C
#define SMU_USR_GPR0             0x100
#define SMU_USR_GPR1             0x104
#define SMU_USR_GPR2             0x108
#define SMU_USR_GPR3             0x10C
#define SYS_CNT_RST_CFG          0x110
#define SYS_CTL_REG              0x120
#define AON_PADS_CTL0            0x130
#define AON_PADS_CTL1            0x134
#define LP_RTC_EN                0x140
#define LP_RTC_CTL               0x144
#define LP_RTC_RDAT              0x148
#define LP_RTC_CFG_TIM           0x14C
#define PLL_CLK_GATE_CTL         0x150

//#define SYS_REG_BASE             0xF038F000

static void smu_test_cpu_enter_wfi(void)
{
	unsigned int temp;

	printk("smu test cpu enter wfi.\n");

	temp = readl(smu_base_reg + SMU_IRQ_MASK);
	printk("SMU_IRQ_MASK(0x%x)=0x%x\n", SMU_IRQ_MASK, temp);

	temp = readl(smu_base_reg + CHIP_WAKE_UP_CFG);
	//temp |= (0x1 << 0);          //enable wakeup source 0
	temp &= ~(0x1 << 0);           //disable wakeup source 0
	//temp &= ~(0x1 << 1);         //use low active negative pulse
	temp |= (0x1 << 1);            //use high active positive pulse
	temp |= (0x1 << 2);            //enable rtc wakeyup
	//temp &= ~(0x1 << 2);         //disable rtc wakeyup
	temp |= (0x1 << 3);          //enable ir wakeyup
	//temp &= ~(0x1 << 3);           //disable ir wakeyup
	writel(temp, smu_base_reg + CHIP_WAKE_UP_CFG);
	temp = readl(smu_base_reg + CHIP_WAKE_UP_CFG);
	printk("CHIP_WAKE_UP_CFG(0x%x)=0x%x\n", CHIP_WAKE_UP_CFG, temp);

	temp = readl(smu_base_reg + CHIP_PWR_MODE_CFG);
	temp |= 0x1 << 0;              //enable ext mode
	writel(temp, smu_base_reg + CHIP_PWR_MODE_CFG);
	temp = readl(smu_base_reg + CHIP_PWR_MODE_CFG);
	printk("CHIP_PWR_MODE_CFG(0x%x)=0x%x\n", CHIP_PWR_MODE_CFG, temp);

	temp = readl(smu_base_reg + SYS_CTL_REG);
	temp |= 0x1 << 0;              //system remap function enable,high active
	writel(temp, smu_base_reg + SYS_CTL_REG);
	temp = readl(smu_base_reg + SYS_CTL_REG);
	printk("SYS_CTL_REG(0x%x)=0x%x\n", SYS_CTL_REG, temp);
	if ((temp & 0x1) != 0x1) {
		printk("wakeup1:remap must 1. %x ", temp);
		return ;
	}

	temp = readl(smu_base_reg + SMU_IRQ_RAW);
	temp |= (0x7ff << 0);          //write 1 clear
	writel(temp, smu_base_reg + SMU_IRQ_RAW);
	temp = readl(smu_base_reg + SMU_IRQ_RAW);
	printk("SMU_IRQ_RAW(0x%x)=0x%x\n", SMU_IRQ_RAW, temp);

	//temp = readl(smu_base_reg + CHIP_STATUS);
	//printk("CHIP_STATUS(0x%x)=0x%x\n", CHIP_STATUS, temp);

	printk("enter wfi\n");

	asm("wfi;");
}

static int tx5336_smu_test_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct resource *res;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	smu_base_reg = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(smu_base_reg)) {
		ret = PTR_ERR(smu_base_reg);
		return ret;
	}

	smu_test_cpu_enter_wfi();

	return ret;
}

static int tx5336_smu_test_remove(struct platform_device *pdev)
{
	return 0;
}


static const struct of_device_id tx5336_smu_test_match[] = {
	{ .compatible = "ts,tx5336-smu-test" },
	{ /* end of table */ },
};
MODULE_DEVICE_TABLE(of, tx5336_smu_test_match);

static struct platform_driver tx5336_smu_test_driver = {
	.probe	= tx5336_smu_test_probe,
	.remove	= tx5336_smu_test_remove,
	.driver = {
		.name = "tx5336_smu_test",
		.of_match_table = tx5336_smu_test_match,
	},
};

module_platform_driver(tx5336_smu_test_driver);
MODULE_LICENSE("GPL");

