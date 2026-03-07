/*
 * Copyright (c) 2014, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/clk.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/component.h>

#include <drm/drm_of.h>
#include <drm/drmP.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_edid.h>
#include <drm/bridge/dw_hdmi.h>

//#include "ts_drm_drv.h"
//#include "ts_drm_vop.h"

struct ts_crtc_state {
	struct drm_crtc_state base;
	int output_type;
	int output_mode;
};
#define to_ts_crtc_state(s) \
		container_of(s, struct ts_crtc_state, base)

#define DRIVER_NAME    "ts-dwhdmi"

/**
 * struct ts_hdmi_chip_data - splite the grf setting of kind of chips
 * @lcdsel_grf_reg: grf register offset of lcdc select
 * @lcdsel_big: reg value of selecting vop big for HDMI
 * @lcdsel_lit: reg value of selecting vop little for HDMI
 */
struct ts_hdmi_chip_data {
	u32	lcdsel_grf_reg;
	u32	lcdsel_big;
	u32	lcdsel_lit;
};

struct ts_hdmi {
	struct device *dev;
	struct drm_encoder encoder;
	const struct ts_hdmi_chip_data *chip_data;
	struct clk *pxl;
	struct clk *pclk;
};

#define to_ts_hdmi(x)	container_of(x, struct ts_hdmi, x)

static const struct dw_hdmi_mpll_config ts_mpll_cfg[] = {
	{
		27000000, {
			{ 0x00b3, 0x0000},
			{ 0x2153, 0x0000},
			{ 0x40f3, 0x0000}
		},
	}, {
		36000000, {
			{ 0x00b3, 0x0000},
			{ 0x2153, 0x0000},
			{ 0x40f3, 0x0000}
		},
	}, {
		40000000, {
			{ 0x00b3, 0x0000},
			{ 0x2153, 0x0000},
			{ 0x40f3, 0x0000}
		},
	}, {
		54000000, {
			{ 0x0072, 0x0001},
			{ 0x2142, 0x0001},
			{ 0x40a2, 0x0001},
		},
	}, {
		65000000, {
			{ 0x0072, 0x0001},
			{ 0x2142, 0x0001},
			{ 0x40a2, 0x0001},
		},
	}, {
		66000000, {
			{ 0x013e, 0x0003},
			{ 0x217e, 0x0002},
			{ 0x4061, 0x0002}
		},
	}, {
		74250000, {
			{ 0x0072, 0x0001},
			{ 0x2145, 0x0002},
			{ 0x4061, 0x0002}
		},
	}, {
		83500000, {
			{ 0x0072, 0x0001},
		},
	}, {
		108000000, {
			{ 0x0051, 0x0002},
			{ 0x2145, 0x0002},
			{ 0x4061, 0x0002}
		},
	}, {
		106500000, {
			{ 0x0051, 0x0002},
			{ 0x2145, 0x0002},
			{ 0x4061, 0x0002}
		},
	}, {
		146250000, {
			{ 0x0051, 0x0002},
			{ 0x2145, 0x0002},
			{ 0x4061, 0x0002}
		},
	}, {
		148500000, {
			{ 0x0051, 0x0003},
			{ 0x214c, 0x0003},
			{ 0x4064, 0x0003}
		},
	}, {
		~0UL, {
			{ 0x00a0, 0x000a },
			{ 0x2001, 0x000f },
			{ 0x4002, 0x000f },
		},
	}
};

static const struct dw_hdmi_curr_ctrl ts_cur_ctr[] = {
	/*      pixelclk    bpp8    bpp10   bpp12 */
	{
		40000000,  { 0x0018, 0x0018, 0x0018 },
	}, {
		65000000,  { 0x0028, 0x0028, 0x0028 },
	}, {
		66000000,  { 0x0038, 0x0038, 0x0038 },
	}, {
		74250000,  { 0x0028, 0x0038, 0x0038 },
	}, {
		83500000,  { 0x0028, 0x0038, 0x0038 },
	}, {
		146250000, { 0x0038, 0x0038, 0x0038 },
	}, {
		148500000, { 0x0000, 0x0038, 0x0038 },
	}, {
		~0UL,      { 0x0000, 0x0000, 0x0000},
	}
};

static const struct dw_hdmi_phy_config ts_phy_config[] = {
	/*pixelclk   symbol   term   vlev*/
	{ 74250000,  0x8009, 0x0004, 0x0272},
	{ 148500000, 0x802b, 0x0004, 0x028d},
	{ 297000000, 0x8039, 0x0005, 0x028d},
	{ ~0UL,	     0x0000, 0x0000, 0x0000}
};

static int ts_hdmi_parse_dt(struct ts_hdmi *hdmi)
{
	int ret;

	hdmi->pxl = devm_clk_get(hdmi->dev, "pxl");
	if (PTR_ERR(hdmi->pxl) == -ENOENT) {
		hdmi->pxl = NULL;
	} else if (PTR_ERR(hdmi->pxl) == -EPROBE_DEFER) {
		return -EPROBE_DEFER;
	} else if (IS_ERR(hdmi->pxl)) {
		dev_err(hdmi->dev, "failed to get grf clock\n");
		return PTR_ERR(hdmi->pxl);
	}

	ret = clk_prepare_enable(hdmi->pxl);
	if (ret) {
		dev_err(hdmi->dev, "Failed to enable HDMI pxl: %d\n", ret);
		return ret;
	}

	hdmi->pclk = devm_clk_get(hdmi->dev, "pclk");
	if (IS_ERR(hdmi->pclk)) {
		ret = PTR_ERR(hdmi->pclk);
		dev_err(hdmi->dev, "Unable to get HDMI pclk clk: %d\n", ret);
		return ret;
	}

	ret = clk_prepare_enable(hdmi->pclk);
	if (ret) {
		dev_err(hdmi->dev, "Cannot enable HDMI pclk clock: %d\n", ret);
		return ret;
	}

	return 0;
}

static enum drm_mode_status
dw_hdmi_ts_mode_valid(struct drm_connector *connector,
			    const struct drm_display_mode *mode)
{
	const struct dw_hdmi_mpll_config *mpll_cfg = ts_mpll_cfg;
	int pclk = mode->clock * 1000;
	bool valid = false;
	int i;

	for (i = 0; mpll_cfg[i].mpixelclock != (~0UL); i++) {
		if (pclk == mpll_cfg[i].mpixelclock) {
			valid = true;
			break;
		}
	}

	return (valid) ? MODE_OK : MODE_BAD;
}

static const struct drm_encoder_funcs dw_hdmi_ts_encoder_funcs = {
	.destroy = drm_encoder_cleanup,
};

static void dw_hdmi_ts_encoder_disable(struct drm_encoder *encoder)
{
}

static bool
dw_hdmi_ts_encoder_mode_fixup(struct drm_encoder *encoder,
				    const struct drm_display_mode *mode,
				    struct drm_display_mode *adj_mode)
{
	return true;
}

static void dw_hdmi_ts_encoder_mode_set(struct drm_encoder *encoder,
					      struct drm_display_mode *mode,
					      struct drm_display_mode *adj_mode)
{
	// struct ts_hdmi *hdmi = to_ts_hdmi(encoder);

	// clk_set_rate(hdmi->pxl, adj_mode->clock * 1000);
}

static void dw_hdmi_ts_encoder_enable(struct drm_encoder *encoder)
{
	return;
}

static int
dw_hdmi_ts_encoder_atomic_check(struct drm_encoder *encoder,
				      struct drm_crtc_state *crtc_state,
				      struct drm_connector_state *conn_state)
{
	struct ts_crtc_state *s = to_ts_crtc_state(crtc_state);

	s->output_type = DRM_MODE_CONNECTOR_HDMIA;

	return 0;
}

static const struct drm_encoder_helper_funcs dw_hdmi_ts_encoder_helper_funcs = {
	.mode_fixup = dw_hdmi_ts_encoder_mode_fixup,
	.mode_set   = dw_hdmi_ts_encoder_mode_set,
	.enable     = dw_hdmi_ts_encoder_enable,
	.disable    = dw_hdmi_ts_encoder_disable,
	.atomic_check = dw_hdmi_ts_encoder_atomic_check,
};

static struct ts_hdmi_chip_data tx536_chip_data = {
	.lcdsel_grf_reg = -1,
};

static const struct dw_hdmi_plat_data tx536_hdmi_drv_data = {
	.mode_valid = dw_hdmi_ts_mode_valid,
	.mpll_cfg   = ts_mpll_cfg,
	.cur_ctr    = ts_cur_ctr,
	.phy_config = ts_phy_config,
	.phy_data = &tx536_chip_data,
};

static const struct of_device_id dw_hdmi_ts_dt_ids[] = {
	{ .compatible = "ts,dw-hdmi",
	  .data = &tx536_hdmi_drv_data
	},
	{},
};
MODULE_DEVICE_TABLE(of, dw_hdmi_ts_dt_ids);

static int dw_hdmi_ts_bind(struct device *dev, struct device *master,
				 void *data)
{
	struct platform_device *pdev = to_platform_device(dev);
	const struct dw_hdmi_plat_data *plat_data;
	const struct of_device_id *match;
	struct drm_device *drm = data;
	struct drm_encoder *encoder;
	struct ts_hdmi *hdmi;
	int ret;

	printk(KERN_INFO "%s %d\n", __func__, __LINE__);
	if (!pdev->dev.of_node)
		return -ENODEV;

	hdmi = devm_kzalloc(&pdev->dev, sizeof(*hdmi), GFP_KERNEL);
	if (!hdmi)
		return -ENOMEM;

	match = of_match_node(dw_hdmi_ts_dt_ids, pdev->dev.of_node);
	plat_data = match->data;
	hdmi->dev = &pdev->dev;
	hdmi->chip_data = plat_data->phy_data;
	encoder = &hdmi->encoder;

	encoder->possible_crtcs = drm_of_find_possible_crtcs(drm, dev->of_node);
	/*
	 * If we failed to find the CRTC(s) which this encoder is
	 * supposed to be connected to, it's because the CRTC has
	 * not been registered yet.  Defer probing, and hope that
	 * the required CRTC is added later.
	 */
	if (encoder->possible_crtcs == 0)
		return -EPROBE_DEFER;

	ret = ts_hdmi_parse_dt(hdmi);
	if (ret) {
		dev_err(hdmi->dev, "Unable to parse OF data\n");
		return ret;
	}
	drm_encoder_helper_add(encoder, &dw_hdmi_ts_encoder_helper_funcs);
	drm_encoder_init(drm, encoder, &dw_hdmi_ts_encoder_funcs,
			 DRM_MODE_ENCODER_TMDS, NULL);

	ret = dw_hdmi_bind(pdev, encoder, plat_data);

	/*
	 * If dw_hdmi_bind() fails we'll never call dw_hdmi_unbind(),
	 * which would have called the encoder cleanup.  Do it manually.
	 */
	if (ret)
		drm_encoder_cleanup(encoder);

	return ret;
}

static void dw_hdmi_ts_unbind(struct device *dev, struct device *master,
				    void *data)
{
	return dw_hdmi_unbind(dev);
}

static const struct component_ops dw_hdmi_ts_ops = {
	.bind	= dw_hdmi_ts_bind,
	.unbind	= dw_hdmi_ts_unbind,
};

static int dw_hdmi_ts_probe(struct platform_device *pdev)
{
	printk(KERN_INFO "%s %d\n", __func__, __LINE__);
	return component_add(&pdev->dev, &dw_hdmi_ts_ops);
}

static int dw_hdmi_ts_remove(struct platform_device *pdev)
{
	component_del(&pdev->dev, &dw_hdmi_ts_ops);

	return 0;
}

struct platform_driver dw_hdmi_ts_pltfm_driver = {
	.probe  = dw_hdmi_ts_probe,
	.remove = dw_hdmi_ts_remove,
	.driver = {
		.name = DRIVER_NAME,
		.of_match_table = dw_hdmi_ts_dt_ids,
	},
};
module_platform_driver(dw_hdmi_ts_pltfm_driver);

MODULE_DESCRIPTION("TS DW HDMI driver");
MODULE_AUTHOR("lgh");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" DRIVER_NAME);
