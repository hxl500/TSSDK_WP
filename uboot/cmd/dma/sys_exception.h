
/*****************************************************************************
* Copyright (C) 2021,Tsing Micro Technology Co., Ltd,All rights reserved.
* FileName     : sys_irq.h
* Author       :
* Data         :
* OverView     :
*
* History      :
* Revisor      :
* Date         :
* Description  :
*
*****************************************************************************/

#ifndef __SYS_EXCEPTION_H__
#define __SYS_EXCEPTION_H__

#include <stdint.h>
#include "reg.h"
//#include "MP_GIC.h"

/* interrupt id list */
typedef enum IRQn {
	INT_ID_CNTHPIRQ			= 26  ,	 /* Hypervisor physical timer event */
	INT_ID_CNTVIRQ			= 27  ,  /* Virtual physical timer event */
	INT_ID_CNTPSIRQ			= 29  ,  /* Secure physical timer event */
	INT_ID_CNTPNSIRQ		= 30  ,  /* Non-secure physical timer event */
	INT_ID_NS_WDT			= 32  ,
	INT_ID_S_WDT			= 33  ,
	INT_ID_GPIO				= 34  ,
	INT_ID_ADC				= 35  ,
	INT_ID_NS_TIMER0		= 36  ,
	INT_ID_NS_TIMER1		= 37  ,
	INT_ID_NS_TIMER2		= 38  ,
	INT_ID_NS_TIMER3		= 39  ,
	INT_ID_NS_TIMER4		= 40  ,
	INT_ID_NS_TIMER5		= 41  ,
	INT_ID_NS_TIMER6		= 42  ,
	INT_ID_NS_TIMER7		= 43  ,
	INT_ID_S_TIMER0			= 44  ,
	INT_ID_S_TIMER1			= 45  ,
	INT_ID_S_TIMER2			= 46  ,
	INT_ID_I2S0				= 47  ,
	INT_ID_I2S1				= 48  ,
	INT_ID_SPI0				= 49  ,
	INT_ID_SPI1				= 50  ,
	INT_ID_SPI2				= 51  ,
	INT_ID_SPI3				= 52  ,
	INT_ID_SPI4				= 53  ,
	INT_ID_UART0			= 54  ,
	INT_ID_UART1			= 55  ,
	INT_ID_UART2			= 56  ,
	INT_ID_UART3			= 57  ,
	INT_ID_UART4			= 58  ,
	INT_ID_UART5			= 59  ,
	INT_ID_USI2				= 60  ,
	INT_ID_USI3				= 61  ,
	INT_ID_I2C2				= 62  ,
	INT_ID_I2C3				= 63  ,
	INT_ID_I2C4				= 64  ,
	INT_ID_I2C5				= 65  ,
	INT_ID_I2C6				= 66  ,
	INT_ID_I2C7				= 67  ,
	INT_ID_I2C8				= 68  ,
	INT_ID_I2C9				= 69  ,
	INT_ID_EXT_ERR			= 70  ,   /* bus error respons */
	INT_ID_CTI0				= 71  ,
	INT_ID_CTI1				= 72  ,
	INT_ID_PMU0				= 73  ,
	INT_ID_PMU1				= 74  ,
	INT_ID_BT1120_TX		= 75  ,
	INT_ID_BT1120_RX		= 76  ,
	INT_ID_RESERVE_77		= 77  ,
	INT_ID_OSPI				= 78  ,
	INT_ID_DMA2MIPI			= 79  ,
	INT_ID_DSI_HOST			= 80  ,
	INT_ID_CSI_DEVICE		= 81  ,
	INT_ID_HDMI				= 82  ,
	INT_ID_DPU_LITE			= 83  ,
	INT_ID_DP650_DE			= 84  ,
	INT_ID_DP650_SE			= 85  ,
	INT_ID_CSI_HOST0		= 86  ,
	INT_ID_CSI_HOST1		= 87  ,
	INT_ID_MIPI_RX_EMBED	= 88  ,
	INT_ID_RESERVE_89		= 89  ,
	INT_ID_ISP				= 90  ,
	INT_ID_GMAC				= 91  ,
	INT_ID_SD0				= 92  ,
	INT_ID_SD0_WAKEUP		= 93  ,
	INT_ID_SD1				= 94  ,
	INT_ID_SD1_WAKEUP		= 95  ,
	INT_ID_CRYPTO_HASH		= 96  ,
	INT_ID_CRYPTO_PKE		= 97  ,
	INT_ID_CRYPTO_SKE		= 98  ,
	INT_ID_CRYPTO_TRAN		= 99  ,
	INT_ID_CRYPTO_SPACC		= 100 ,
	INT_ID_USB3				= 101 ,
	INT_ID_NFC				= 102 ,
	INT_ID_VPU_WAVE			= 103 ,
	INT_ID_VPU_CODA			= 104 ,
	INT_ID_DMA0_INT0		= 105 ,
	INT_ID_DMA0_INT1		= 106 ,
	INT_ID_DMA0_ABORT		= 107 ,
	INT_ID_DMA1				= 108 ,
	INT_ID_GDC				= 109 ,
	INT_ID_MAILBOX_CPU0		= 110 ,
	INT_ID_OTPC				= 111 ,
	INT_ID_RTC				= 112 ,
	INT_ID_SMU				= 113 ,
	INT_ID_MCU_GPIO			= 114 ,
	INT_ID_I2C0				= 115 ,
	INT_ID_I2C1				= 116 ,
	INT_ID_MCU_WDT			= 117 ,
	INT_ID_PWM				= 118 ,
	INT_ID_MCU_TIMER0		= 119 ,
	INT_ID_MCU_TIMER1		= 120 ,
	INT_ID_MCU_TIMER2		= 121 ,
	INT_ID_MCU_TIMER3		= 122 ,
	INT_ID_MCU_TIMER4		= 123 ,
	INT_ID_MCU_UART			= 124 ,
	INT_ID_MCU_USI0			= 125 ,
	INT_ID_MCU_USI1			= 126 ,
	INT_ID_WAVE_IDLE		= 127 ,
	INT_ID_GPU				= 128 ,
	INT_ID_CVE				= 129 ,
	INT_ID_AI_ISP0			= 130 ,
	INT_ID_AI_ISP1			= 131 ,
	INT_ID_AI_ISP2			= 132 ,
	INT_ID_AI_ISP3			= 133 ,
	INT_ID_RCE				= 134 ,
	INT_ID_CGRA_RNE			= 135 ,
	INT_ID_DMA2				= 136 ,
	INT_ID_DVP2DMA			= 137 ,
	INT_ID_SHRAM0_CONFLICT  = 138 ,
	INT_ID_ACODEC			= 139 ,
	INT_ID_RNE_NANO			= 142 ,
	INT_ID_TNR				= 143 ,
	INT_ID_LPRTC			= 144 ,

	INT_MAX_ID
}IRQn_Type;

#define INT_MAX_NUM	(INT_MAX_ID + 1)

#define INT_ID_3DNR			143

/* distributor and cpu interface */
#define GIC_DIST_OFF			0x1000
#define GIC_CPU_INTF_OFF		0x2000
#define GIC_DIST_BASE			(GIC400_BASE + GIC_DIST_OFF)
#define GIC_CPU_INTF_BASE		(GIC400_BASE + GIC_CPU_INTF_OFF)

#define GIC_DIST_RD(reg)		  (*((uint32_t  *)(GIC_DIST_BASE + reg)))
#define GIC_DIST_WR(reg, val)	  (*((uint32_t *)(GIC_DIST_BASE + reg)) = val)
#define GIC_CPU_INTF_RD(reg)	  (*((uint32_t *)(GIC_CPU_INTF_BASE + reg)))
#define GIC_CPU_INTF_WR(reg, val) (*((uint32_t *)(GIC_CPU_INTF_BASE + reg)) = val)

/* distributor reg offset */
#define GIC_DIST_CTRL			0x000
#define GIC_DIST_CTR			0x004
#define GIC_DIST_IIDR			0x008
#define GIC_DIST_IGROUP			0x080
#define GIC_DIST_ENABLE_SET		0x100
#define GIC_DIST_ENABLE_CLEAR	0x180
#define GIC_DIST_PENDING_SET	0x200
#define GIC_DIST_PENDING_CLEAR	0x280
#define GIC_DIST_ACTIVE_SET		0x300
#define GIC_DIST_ACTIVE_CLEAR	0x380
#define GIC_DIST_PRI			0x400
#define GIC_DIST_TARGET			0x800
#define GIC_DIST_CONFIG			0xc00
#define GIC_DIST_SOFTINT		0xf00
#define GIC_DIST_SGI_PENDING_CLEAR	0xf10
#define GIC_DIST_SGI_PENDING_SET	0xf20

#define GICD_DEFAULT_PRIO		0x0e
#define GICD_TARGET_CPU0		0x01

#define GICD_ENABLE				0x1
#define GICD_DISABLE			0x0
#define GICD_INT_ACTLOW_LVLTRIG	0x0
#define GICD_INT_EN_CLR_X32		0xffffffff
#define GICD_INT_EN_SET_SGI		0x0000ffff
#define GICD_INT_EN_CLR_PPI		0xffff0000

#define GICD_CFG_LEVEL			0
#define GICD_CFG_EDGE			2

#define GICD_IIDR_IMPLEMENTER_SHIFT	0
#define GICD_IIDR_IMPLEMENTER_MASK	(0xfff << GICD_IIDR_IMPLEMENTER_SHIFT)
#define GICD_IIDR_REVISION_SHIFT	12
#define GICD_IIDR_REVISION_MASK		(0xf << GICD_IIDR_REVISION_SHIFT)
#define GICD_IIDR_VARIANT_SHIFT		16
#define GICD_IIDR_VARIANT_MASK		(0xf << GICD_IIDR_VARIANT_SHIFT)
#define GICD_IIDR_PRODUCT_ID_SHIFT	24
#define GICD_IIDR_PRODUCT_ID_MASK	(0xff << GICD_IIDR_PRODUCT_ID_SHIFT)


/* cpu interface reg offset */
#define GIC_CPU_CTRL			0x00
#define GIC_CPU_PRIMASK			0x04
#define GIC_CPU_BINPOINT		0x08
#define GIC_CPU_INTACK			0x0c
#define GIC_CPU_EOI				0x10
#define GIC_CPU_RUNNINGPRI		0x14
#define GIC_CPU_HIGHPRI			0x18
#define GIC_CPU_ALIAS_BINPOINT	0x1c
#define GIC_CPU_ACTIVEPRIO		0xd0
#define GIC_CPU_IDENT			0xfc
#define GIC_CPU_DEACTIVATE		0x1000

#define GICC_ENABLE				0x1
#define GICC_INT_PRI_THRESHOLD	0xf0

#define GIC_CPU_CTRL_EnableGrp0_SHIFT	0
#define GIC_CPU_CTRL_EnableGrp0			(1 << GIC_CPU_CTRL_EnableGrp0_SHIFT)
#define GIC_CPU_CTRL_EnableGrp1_SHIFT	1
#define GIC_CPU_CTRL_EnableGrp1			(1 << GIC_CPU_CTRL_EnableGrp1_SHIFT)
#define GIC_CPU_CTRL_AckCtl_SHIFT		2
#define GIC_CPU_CTRL_AckCtl				(1 << GIC_CPU_CTRL_AckCtl_SHIFT)
#define GIC_CPU_CTRL_FIQEn_SHIFT		3
#define GIC_CPU_CTRL_FIQEn				(1 << GIC_CPU_CTRL_FIQEn_SHIFT)
#define GIC_CPU_CTRL_CBPR_SHIFT			4
#define GIC_CPU_CTRL_CBPR				(1 << GIC_CPU_CTRL_CBPR_SHIFT)
#define GIC_CPU_CTRL_EOImodeNS_SHIFT	9
#define GIC_CPU_CTRL_EOImodeNS			(1 << GIC_CPU_CTRL_EOImodeNS_SHIFT)

#define GICC_IAR_INT_ID_MASK		0x3ff
#define GICC_INT_SPURIOUS			1023
#define GICC_DIS_BYPASS_MASK		0x1e0



typedef void (*InterruptHandler)(void *data);
struct IrqTableEntry
{
	InterruptHandler Handler;
	void *data;
};


/**
  \brief	   init the gic hardware
  \param[in]   void
  \return      None.
*/
void drv_exception_init(void);


/**
  \brief       register irq handler.
  \param[in]   irq_id Number of IRQ.
  \param[in]   irq_handler IRQ Handler.
  \return      None.
*/
void drv_irq_register(uint32_t irq_id, struct IrqTableEntry *Entry);

/**
  \brief       unregister irq handler.
  \param[in]   irq_id Number of IRQ.
  \return      None.
*/
void drv_irq_unregister(uint32_t irq_id);


/**
  \brief       enable irq.
  \param[in]   irq_id Number of IRQ.
  \return      None.
*/
void drv_irq_enable(uint32_t irq_id);

/**
  \brief       disable irq.
  \param[in]   irq_id Number of IRQ.
  \return      None.
*/
void drv_irq_disable(uint32_t irq_id);

/**
  \brief       send a software generation interrupt
  \param[in]   irq_id Number of IRQ.
  \param[in]   filter identify which processors to interrupt (self, all
                      but self, or mask specified in target, see MP_GIC.h
  \param[in]   target bitmap of procesors to interrupt when required by
                      filter
  \return      None.
*/
//void drv_irq_sendSGI(uint32_t irq_id, sgi_tlf filter, uint32_t target);


#endif
