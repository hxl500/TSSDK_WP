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


#include <linux/clk.h>
#include <linux/types.h>
#include <linux/io.h>
#include <drm/drmP.h>
#include <video/videomode.h>
#include <video/display_timing.h>

#include "nemadc_drv.h"
#include "nemadc_hw.h"
#ifdef CONFIG_DRM_NEMADC_SPI
	#include "nema_dc_mipi.h"
#endif


#define NEMADC_COMMON_FORMATS \
	/*    fourcc,   layers supporting the format,      internal id   */ \
	{ DRM_FORMAT_C8, NM_VIDEO1 | NM_GRPHC1 | NM_GRPHC2 | NM_GRPHC3, \
		NEMADC_PALETTE8 }, \
	{ DRM_FORMAT_RGBX5551, NM_VIDEO1 | NM_GRPHC1 | NM_GRPHC2 | NM_GRPHC3, \
		NEMADC_RGBA5551 }, \
	{ DRM_FORMAT_RGBA8888, NM_VIDEO1 | NM_GRPHC1 | NM_GRPHC2 | NM_GRPHC3, \
		NEMADC_RGBA8888 }, \
	{ DRM_FORMAT_RGB332, NM_VIDEO1 | NM_GRPHC1 | NM_GRPHC2 | NM_GRPHC3, \
		NEMADC_RGB332 }, \
	{ DRM_FORMAT_RGB565, NM_VIDEO1 | NM_GRPHC1 | NM_GRPHC2 | NM_GRPHC3, \
		NEMADC_RGB565 }, \
	{ DRM_FORMAT_ARGB8888, NM_VIDEO1 | NM_GRPHC1 | NM_GRPHC2 | NM_GRPHC3, \
		NEMADC_ARGB8888 }, \
	{ DRM_FORMAT_R8, NM_VIDEO1 | NM_GRPHC1 | NM_GRPHC2 | NM_GRPHC3, \
		NEMADC_L8 }, \
	{ DRM_FORMAT_RGB888, NM_VIDEO1 | NM_GRPHC1 | NM_GRPHC2 | NM_GRPHC3, \
		NEMADC_RGB24 }, \
	{ DRM_FORMAT_ABGR8888, NM_VIDEO1 | NM_GRPHC1 | NM_GRPHC2 | NM_GRPHC3, \
		NEMADC_ABGR8888 }, \
	{ DRM_FORMAT_BGRA8888, NM_VIDEO1 | NM_GRPHC1 | NM_GRPHC2 | NM_GRPHC3, \
		NEMADC_BGRA8888 }, \
	{ DRM_FORMAT_C8, NM_VIDEO1 | NM_GRPHC1 | NM_GRPHC2 | NM_GRPHC3, \
		NEMADC_PALETTE8 }, \
	{ DRM_FORMAT_YVYU, NM_VIDEO1, NEMADC_422_YVYU }, \
	{ DRM_FORMAT_VYUY, NM_VIDEO1, NEMADC_422_VYUY }, \
	{ DRM_FORMAT_YVU420, NM_VIDEO1, NEMADC_420_YV12 }, \
	{ DRM_FORMAT_YUV420, NM_VIDEO1, NEMADC_420_YV12 } \

static const struct nemadc_format_id namedc_formats[] = {
	NEMADC_COMMON_FORMATS,
};

static const struct nemadc_layer namedc_layers[] = {
	{ NM_VIDEO1, NEMADC_REG_LV1_BASE},
	{ NM_GRPHC1, NEMADC_REG_LG1_BASE},
	{ NM_GRPHC2, NEMADC_REG_LG2_BASE},
	{ NM_GRPHC3, NEMADC_REG_LG3_BASE},
};
/* Set display timing */
static void namedc_modeset(struct nemadc_hw_device* hwdev,
	struct videomode* mode)
{
	u32 val;
	// u32 reg_val;

#ifdef CONFIG_DRM_NEMADC_SPI
	nemadc_MIPI_backlight_open();
	nemadc_MIPI_out(hwdev,MIPI_DBIB_CMD | MIPI_set_display_on);
	nemadc_MIPI_out(hwdev,MIPI_DBIB_CMD  | MIPI_write_memory_start);
#endif
	
	val = (mode->hactive << 16) | (mode->vactive);
	nemadc_hw_write(hwdev, val, NEMADC_REG_RESXY);
	val += (mode->hfront_porch << 16) | (mode->vfront_porch);
	nemadc_hw_write(hwdev, val, NEMADC_REG_FRONTPORCHXY);
	val += ((mode->hsync_len) << 16) | (mode->vsync_len);
	nemadc_hw_write(hwdev, val, NEMADC_REG_BLANKINGXY);
	val += ((mode->hback_porch) << 16) | (mode->vback_porch);
	nemadc_hw_write(hwdev, val, NEMADC_REG_BACKPORCHXY);

}

static int namedc_query_hw(struct nemadc_hw_device* hwdev)
{
	hwdev->min_width = 4;
	hwdev->min_height = 4;
	hwdev->max_width = 3840;
	hwdev->max_height = 3840;

	return 0;
}

static void namedc_hw_enable(struct nemadc_hw_device* hwdev)
{
#ifdef CONFIG_DRM_NEMADC_SPI
	static int i = 3;
#endif

	nemadc_hw_setbits(hwdev, 1 << NM_MODE_ENABLE_SHF, NEMADC_REG_MODE);
#ifdef CONFIG_DRM_NEMADC_SPI
	if( i -- > 0)
	{
		nemadc_MIPI_out(hwdev,MIPI_DBIB_CMD | MIPI_set_display_on);
		nemadc_MIPI_out(hwdev,MIPI_DBIB_CMD  | MIPI_write_memory_start);
	}
#endif
}

static void namedc_hw_disable(struct nemadc_hw_device* hwdev)
{
	nemadc_hw_clearbits(hwdev, 1 << NM_MODE_ENABLE_SHF, NEMADC_REG_MODE);
#ifdef CONFIG_DRM_NEMADC_SPI
	nemadc_MIPI_out(hwdev,MIPI_DBIB_CMD | MIPI_set_display_off);
	nemadc_MIPI_backlight_close();
#endif
}


const struct nemadc_hw_device nemadc_device = {
		.map = {
			.features = NEMADC_REGMAP_HAS_CLEARIRQ |
					NEMADC_DEVICE_LV_HAS_3_STRIDES,
			.n_layers = ARRAY_SIZE(namedc_layers),
			.layers = namedc_layers,
			.irq_map = {
				.irq_mask = 0,
				.vsync_irq = NM_INT_VSYNC_MSK,
			},
			.pixel_formats = namedc_formats,
			.n_pixel_formats = ARRAY_SIZE(namedc_formats),
#ifdef CONFIG_ARCH_TS_TX5336
			.bus_align_bytes = 4,
#else
			.bus_align_bytes = 16,
#endif 
			
		},
		.query_hw = namedc_query_hw,
		.enable = namedc_hw_enable,
		.disable = namedc_hw_disable,
		.modeset = namedc_modeset,
		.features = 0,
};

u8 nemadc_hw_get_format_id(const struct nemadc_hw_regmap* map,
	u8 layer_id, u32 format)
{
	unsigned int i;

	for (i = 0; i < map->n_pixel_formats; i++)
	{
		if (((map->pixel_formats[i].layer & layer_id) == layer_id) &&
			(map->pixel_formats[i].format == format))
			return map->pixel_formats[i].id;
	}

	return NEMADC_INVALID_FORMAT_ID;
}


static irqreturn_t nemadc_irq(int irq, void* arg)
{
	struct drm_device* drm = arg;
	struct nemadc_drm* namedc = drm->dev_private;
	struct nemadc_hw_device* hwdev;
	u32 status, reg;
	irqreturn_t ret = IRQ_NONE;

	hwdev = namedc->dev;

	/*
	 * if we are suspended it is likely that we were invoked because
	 * we share an interrupt line with some other driver, don't try
	 * to read the hardware registers
	 */
	if (hwdev->pm_suspended)
		return IRQ_NONE;

	reg = NEMADC_REG_INTERRUPT;
	status = nemadc_hw_read(hwdev, reg);
	if (status & NM_INT_HSYNC_MSK)
	{
		ret = IRQ_HANDLED;
		nemadc_hw_clearbits(hwdev, NM_INT_HSYNC_MSK, reg);
	}
	if (status & NM_INT_VSYNC_MSK)
	{
		atomic_set(&namedc->config_valid, 1);
		nemadc_hw_clearbits(hwdev, NM_INT_VSYNC_MSK, reg);
		drm_crtc_handle_vblank(&namedc->crtc);
		ret = IRQ_WAKE_THREAD;
	}

	//DRM_INFO("%s() status 0x%x\n", __func__, status);
	return ret;
}

static irqreturn_t nemadc_irq_thread_handler(int irq, void* arg)
{
	struct drm_device* drm = arg;
	struct nemadc_drm* namedc = drm->dev_private;

	wake_up(&namedc->wq);

	return IRQ_HANDLED;
}

int nemadc_irq_hw_init(struct drm_device* drm, int irq)
{
	int ret;
	
	ret = devm_request_threaded_irq(drm->dev, irq, nemadc_irq,
			nemadc_irq_thread_handler, IRQF_SHARED, "nemadc", drm);
	if (ret < 0)
	{
		DRM_ERROR("failed to install NEMADC IRQ handler\n");
		return ret;
	}
	return 0;
}

void nemadc_irq_hw_fini(struct drm_device* drm)
{
	struct nemadc_drm* namedc = drm->dev_private;
	struct nemadc_hw_device* hwdev = namedc->dev;
	u32 reg;

	// TODO: confirm how to disable
	reg = NEMADC_REG_INTERRUPT;
	nemadc_hw_clearbits(hwdev, NM_INT_VSYNC_MSK | NM_INT_HSYNC_MSK, reg);
}




