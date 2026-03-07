/***************************************************************************
 * COPYRIGHT NOTICE
 * Copyright 2022 Tsingmicro, Inc.
 * All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * Nema DC dpu driver
 ***************************************************************************/


#ifndef __NEMADC_HW_H__
#define __NEMADC_HW_H__

#include <linux/bitops.h>
#include "nemadc_regs.h"

struct videomode;
struct clk;


/* format value of NEMADC */
enum {
	NEMADC_PALETTE8  = 0x00,
	NEMADC_RGBA5551  = 0x01,
	NEMADC_RGBA8888  = 0x02,
	/* 0x03 reserve */
	NEMADC_RGB332	 = 0x04,
	NEMADC_RGB565	 = 0x05,
	NEMADC_ARGB8888  = 0x06,
	NEMADC_L8	 = 0x07,
	NEMADC_L1	 = 0x08,	/* drm not support */
	NEMADC_L4	 = 0x09,	/* drm not support */
	NEMADC_422_VYUY	 = 0x0a,
	NEMADC_RGB24	 = 0x0b,
	NEMADC_422_YVYU	 = 0x0c,
	NEMADC_ABGR8888  = 0x0d,
	NEMADC_BGRA8888  = 0x0e,
	NEMADC_420_YV12  = 0x10,	/* Y-V-U */
	/* 0x11 reserve */
	NEMADC_TSC4	 = 0x12,
	NEMADC_TSC6	 = 0x13,
	NEMADC_TSC6A	 = 0x14,
	NEMADC_INVALID_FORMAT_ID = 0xff
};


/* NEMADC layer IDs */
enum {
	NM_VIDEO1 = BIT(0),
	NM_GRPHC1 = BIT(1),
	NM_GRPHC2 = BIT(2),
	NM_GRPHC3 = BIT(3),
};

struct nemadc_format_id {
	u32 format;		/* DRM fourcc */
	u8 layer;		/* bitmask of layers supporting it */
	u8 id;			/* used internally */
};


/*
 * hide the differences between register maps
 * by using a common structure to hold the
 * base register offsets
 */

struct nemadc_irq_map {
	u32 irq_mask;	/* mask of IRQs that can be enabled in the block */
	u32 vsync_irq;	/* IRQ bit used for signaling during VSYNC */
};

struct nemadc_layer {
	u16 id;			/* layer ID */
	u16 base;		/* address offset for the register bank */
};


/* regmap features */
#define NEMADC_REGMAP_HAS_CLEARIRQ	(1 << 0)

struct nemadc_hw_regmap {
	/* bitmap with register map features */
	const u8 features;

	/* list of supported layers */
	const u8 n_layers;
	const struct nemadc_layer *layers;

	const struct nemadc_irq_map irq_map;

	/* list of supported pixel formats for each layer */
	const struct nemadc_format_id *pixel_formats;
	const u8 n_pixel_formats;

	/* pitch alignment requirement in bytes */
	const u8 bus_align_bytes;
};

/* device features */
/* Unlike DP550/650, DP500 has 3 stride registers in its video layer. */
#define NEMADC_DEVICE_LV_HAS_3_STRIDES	BIT(0)

struct nemadc_hw_device {
	const struct nemadc_hw_regmap map;
	void __iomem *regs;

	/* APB clock */
	struct clk *hclk;
	/* core clocl */
	struct clk *cclk;

	/* TODO: */
	/* AXI clock, no use, delete later */
	struct clk *aclk;
	/* main clock for display core, no use, delete later */
	struct clk *mclk;
	/* pixel clock for display core, no use, delete later */
	struct clk *pxlclk;

	/*
	 * Validate the driver instance against the hardware bits
	 */
	int (*query_hw)(struct nemadc_hw_device *hwdev);

	/*
	 * Enable the hardware
	 */
	void (*enable)(struct nemadc_hw_device *hwdev);

	/*
	 * Disable the hardware
	 */
	void (*disable)(struct nemadc_hw_device *hwdev);

	/*
	 * Set a new mode in hardware. Requires the hardware to be in
	 * configuration mode before this function is called.
	 */
	void (*modeset)(struct nemadc_hw_device *hwdev, struct videomode *m);

	u8 features;

	int min_width, min_height;
	int max_width, max_height;

	/* track the device PM state */
	bool pm_suspended;

	/* size of memory used for rotating layers, up to two banks available */
	u32 rotation_memory[2];
};


extern const struct nemadc_hw_device nemadc_device;

static inline u32 nemadc_hw_read(struct nemadc_hw_device *hwdev, u32 reg)
{
	WARN_ON(hwdev->pm_suspended);
	return readl(hwdev->regs + reg);
}

static inline void nemadc_hw_write(struct nemadc_hw_device *hwdev,
				   u32 value, u32 reg)
{
	WARN_ON(hwdev->pm_suspended);
	writel(value, hwdev->regs + reg);
}

static inline void nemadc_hw_setbits(struct nemadc_hw_device *hwdev,
				     u32 mask, u32 reg)
{
	u32 data = nemadc_hw_read(hwdev, reg);

	data |= mask;
	nemadc_hw_write(hwdev, data, reg);
}

static inline void nemadc_hw_clearbits(struct nemadc_hw_device *hwdev,
				       u32 mask, u32 reg)
{
	u32 data = nemadc_hw_read(hwdev, reg);

	data &= ~mask;
	nemadc_hw_write(hwdev, data, reg);
}


static inline void nemadc_hw_disable_irq(struct nemadc_hw_device *hwdev,
	u32 irq)
{
	nemadc_hw_clearbits(hwdev, irq, NEMADC_REG_INTERRUPT);
}

static inline void nemadc_hw_enable_irq(struct nemadc_hw_device *hwdev,
	u32 irq)
{
	//DRM_INFO("--%s--\r\n", __func__);
	nemadc_hw_setbits(hwdev, irq, NEMADC_REG_INTERRUPT);
}

int nemadc_irq_hw_init(struct drm_device *drm, int irq);
void nemadc_irq_hw_fini(struct drm_device *drm);


u8 nemadc_hw_get_format_id(const struct nemadc_hw_regmap *map,
			   u8 layer_id, u32 format);

static inline bool nemadc_hw_pitch_valid(struct nemadc_hw_device *hwdev,
					 unsigned int pitch)
{
	return !(pitch & (hwdev->map.bus_align_bytes - 1));
}



/*
 * background color components are defined as 12bits values,
 * they will be shifted right when stored on hardware that
 * supports only 8bits per channel
 */
#define NEMADC_BGND_COLOR_R		0x000
#define NEMADC_BGND_COLOR_G		0x000
#define NEMADC_BGND_COLOR_B		0x000

#define NEMADC_GAMMA_LUT_NUM		256

#define NEMADC_GAMMA_LUT_SIZE		4096

#endif  /* __NEMADC_HW_H__ */
