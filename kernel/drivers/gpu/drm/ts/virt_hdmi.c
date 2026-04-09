/*
 * Copyright (c) 2016, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include <linux/clk.h>
#include <linux/component.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/phy/phy.h>
#include <linux/pm_runtime.h>
#include <linux/reset.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_crtc.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_of.h>
#include <drm/drm_panel.h>
#include <drm/drmP.h>
#include <linux/gpio.h>

#define DRIVER_NAME    "virt-hdmi"

static struct drm_encoder encoder;
static struct drm_connector connector;

static int virt_hdmi_connector_get_modes(struct drm_connector *connector)
{
	int count;

	count = drm_add_modes_noedid(connector, 8192, 8192);
	//drm_set_preferred_mode(connector, 1280, 720);
	drm_set_preferred_mode(connector, 240, 320);

	return count;
}

static struct drm_encoder *virt_hdmi_connector_best_encoder(
	struct drm_connector *connector)
{
	return &encoder;
}

static const struct drm_connector_helper_funcs virt_hdmi_conn_helper_funcs = {
	.get_modes = virt_hdmi_connector_get_modes,
	.best_encoder = virt_hdmi_connector_best_encoder,
};


static const struct drm_connector_funcs virt_hdmi_connector_funcs = {
	.dpms = drm_helper_connector_dpms,
	.fill_modes = drm_helper_probe_single_connector_modes,
	.destroy = drm_connector_cleanup,
	.reset = drm_atomic_helper_connector_reset,
	.atomic_duplicate_state = drm_atomic_helper_connector_duplicate_state,
	.atomic_destroy_state = drm_atomic_helper_connector_destroy_state,
};

static const struct drm_encoder_funcs virt_hdmi_encoder_funcs = {
	.destroy = drm_encoder_cleanup,
};

static int virt_hdmi_bind(struct device *dev, struct device *master,
				 void *data)
{
	struct drm_device *drm = data;
	int ret = 0;

#ifdef CONFIG_ARCH_TS_TX5215
	struct gpio_desc *pwr;
	pwr = devm_gpiod_get(dev, "pwr", GPIOD_OUT_HIGH);
	if (IS_ERR(pwr)) {
		dev_err(dev, "%s ERROR: Can't get reset GPIO\n",__func__);
		return PTR_ERR(pwr);
	}
	gpiod_set_value_cansleep(pwr, 1);
#endif

	encoder.possible_crtcs = drm_of_find_possible_crtcs(drm,
								dev->of_node);
	DRM_DEBUG_KMS("possible_crtcs = 0x%x\n", encoder.possible_crtcs);

	ret = drm_encoder_init(drm, &encoder, &virt_hdmi_encoder_funcs,
		DRM_MODE_ENCODER_VIRTUAL, NULL);
	if (ret) {
		dev_err(dev, "Failed to initialize encoder with drm\n");
		return ret;
	}
	ret = drm_connector_init(drm, &connector, &virt_hdmi_connector_funcs,
		DRM_MODE_CONNECTOR_VIRTUAL);
	if (ret) {
		dev_err(dev, "Failed to initialize connector\n");
		goto encoder_cleanup;
	}
	drm_connector_helper_add(&connector, &virt_hdmi_conn_helper_funcs);
	connector.polled = DRM_CONNECTOR_POLL_CONNECT |
		DRM_CONNECTOR_POLL_DISCONNECT;

	ret = drm_mode_connector_attach_encoder(&connector, &encoder);
	if (ret) {
		dev_err(dev, "Failed to attach panel: %d\n", ret);
		goto connector_cleanup;
	}
	return 0;

connector_cleanup:
	drm_connector_cleanup(&connector);
encoder_cleanup:
	drm_encoder_cleanup(&encoder);
	return ret;
}

static void virt_hdmi_unbind(struct device *dev, struct device *master,
	void *data)
{

}

static const struct component_ops virt_hdmi_ops = {
	.bind	= virt_hdmi_bind,
	.unbind	= virt_hdmi_unbind,
};

static int virt_hdmi_probe(struct platform_device *pdev)
{
	return component_add(&pdev->dev, &virt_hdmi_ops);
}

static int virt_hdmi_remove(struct platform_device *pdev)
{
	component_del(&pdev->dev, &virt_hdmi_ops);

	return 0;
}

static const struct of_device_id virt_hdmi_dt_ids[] = {
	{ .compatible = "ts,ts-virt-hdmi", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, virt_hdmi_dt_ids);

static struct platform_driver virt_hdmi_driver = {
	.probe		= virt_hdmi_probe,
	.remove		= virt_hdmi_remove,
	.driver		= {
		.of_match_table = virt_hdmi_dt_ids,
		.owner = THIS_MODULE,
		.name	= DRIVER_NAME,
	},
};
module_platform_driver(virt_hdmi_driver);

MODULE_DESCRIPTION("VIRT HDMI driver");
MODULE_AUTHOR("ymc");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" DRIVER_NAME);
