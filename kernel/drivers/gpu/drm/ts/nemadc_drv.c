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

#include <linux/module.h>
#include <linux/clk.h>
#include <linux/component.h>
#include <linux/console.h>
#include <linux/of_device.h>
#include <linux/of_graph.h>
#include <linux/of_reserved_mem.h>
#include <linux/pm_runtime.h>

#include <drm/drmP.h>
#include <drm/drm_atomic.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_crtc.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_fb_cma_helper.h>
#include <drm/drm_gem_cma_helper.h>
#include <drm/drm_of.h>

#include "nemadc_drv.h"
#include "nemadc_regs.h"
#include "nemadc_hw.h"
#ifdef CONFIG_DRM_NEMADC_SPI
	#include "nema_dc_mipi.h"
#endif

#define NEMADC_CONF_VALID_TIMEOUT	250

#define MAX_OUTPUT_CHANNELS	3


static void nemadc_write_gamma_table(struct nemadc_hw_device *hwdev,
				     u32 data[NEMADC_GAMMA_LUT_NUM])
{
	int i;

	for (i = 0; i < NEMADC_GAMMA_LUT_NUM; ++i)
		nemadc_hw_write(hwdev, data[i], NEMADC_REG_GAMMA_LUT + i);
}

static void nemadc_atomic_commit_update_gamma(struct drm_crtc *crtc,
					      struct drm_crtc_state *old_state)
{
	struct nemadc_drm *namedc = crtc_to_nemadc_device(crtc);
	struct nemadc_hw_device *hwdev = namedc->dev;
	u32 mask;

	if (!crtc->state->color_mgmt_changed)
		return;

	mask = (1 << NM_MODE_DPI_CFG_SHF);
	if (!crtc->state->gamma_lut) {
		nemadc_hw_clearbits(hwdev, mask, NEMADC_REG_MODE);
	} else {
		struct nemadc_crtc_state *mc =
			to_nemadc_crtc_state(crtc->state);

		if (!old_state->gamma_lut || (crtc->state->gamma_lut->base.id !=
					      old_state->gamma_lut->base.id))
			nemadc_write_gamma_table(hwdev, mc->gamma_coeffs);

		nemadc_hw_setbits(hwdev, mask, NEMADC_REG_MODE);
	}

}

/*
 * set the "config valid" bit and wait until the hardware acts on it
 */
static int nemadc_enable(struct drm_device *drm)
{
	struct nemadc_drm *namedc = drm->dev_private;
	struct nemadc_hw_device *hwdev = namedc->dev;

	hwdev->enable(hwdev);
	return 0;
}

static void nemadc_output_poll_changed(struct drm_device *drm)
{
	struct nemadc_drm *namedc = drm->dev_private;

	drm_fbdev_cma_hotplug_event(namedc->fbdev);
}



void nemadc_atomic_commit_hw_done(struct drm_atomic_state* state)
{
	struct drm_pending_vblank_event *event;
	struct drm_device *drm = state->dev;
	struct nemadc_drm *namedc = drm->dev_private;
	struct nemadc_hw_device* hwdev = namedc->dev;
	struct drm_vblank_crtc* vblank;

	event = namedc->crtc.state->event;
	if (event) {
		namedc->crtc.state->event = NULL;

		spin_lock_irq(&drm->event_lock);
		if (drm_crtc_vblank_get(&namedc->crtc) == 0)
			drm_crtc_arm_vblank_event(&namedc->crtc, event);
		
		else
			drm_crtc_send_vblank_event(&namedc->crtc, event);
		
		spin_unlock_irq(&drm->event_lock);
	}
	drm_atomic_helper_commit_hw_done(state);

	if (namedc->crtc.enabled)
	{
		vblank = &drm->vblank[drm_crtc_index(&namedc->crtc)];
		/* only set config_valid if the CRTC is enabled */
		nemadc_enable(drm);
		if (vblank->enabled)
			nemadc_hw_enable_irq(hwdev, hwdev->map.irq_map.vsync_irq);

	}
}

static void nemadc_atomic_commit_tail(struct drm_atomic_state *state)
{
	struct drm_device *drm = state->dev;
	struct drm_crtc *crtc;
	struct drm_crtc_state *old_crtc_state;
	int i;

	pm_runtime_get_sync(drm->dev);

	drm_atomic_helper_commit_modeset_disables(drm, state);

	for_each_old_crtc_in_state(state, crtc, old_crtc_state, i)
		nemadc_atomic_commit_update_gamma(crtc, old_crtc_state);

	drm_atomic_helper_commit_planes(drm, state, 0);
	
	drm_atomic_helper_commit_modeset_enables(drm, state);
	
	nemadc_atomic_commit_hw_done(state);

	drm_atomic_helper_wait_for_vblanks(drm, state);

	pm_runtime_put(drm->dev);

	drm_atomic_helper_cleanup_planes(drm, state);
}

static const struct drm_mode_config_helper_funcs nemadc_mode_config_helpers = {
	.atomic_commit_tail = nemadc_atomic_commit_tail,
};

static const struct drm_mode_config_funcs nemadc_mode_config_funcs = {
	.fb_create = drm_fb_cma_create,
	.output_poll_changed = nemadc_output_poll_changed,
	.atomic_check = drm_atomic_helper_check,
	.atomic_commit = drm_atomic_helper_commit,
};

static int nemadc_init(struct drm_device *drm)
{
	int ret;
	struct nemadc_drm *namedc = drm->dev_private;
	struct nemadc_hw_device *hwdev = namedc->dev;

	drm_mode_config_init(drm);

	drm->mode_config.min_width = hwdev->min_width;
	drm->mode_config.min_height = hwdev->min_height;
	drm->mode_config.max_width = hwdev->max_width;
	drm->mode_config.max_height = hwdev->max_height;
	drm->mode_config.funcs = &nemadc_mode_config_funcs;
	drm->mode_config.helper_private = &nemadc_mode_config_helpers;

	ret = nemadc_crtc_init(drm);
	if (ret) {
		drm_mode_config_cleanup(drm);
		return ret;
	}

	return 0;
}

static void nemadc_fini(struct drm_device *drm)
{
	nemadc_planes_destroy(drm);
	drm_mode_config_cleanup(drm);
}

static int nemadc_irq_init(struct platform_device *pdev)
{
	int irq, ret = 0;
	struct drm_device *drm = dev_get_drvdata(&pdev->dev);

	/* fetch the interrupts from DT */
	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		DRM_ERROR("no 'DE' IRQ specified!\n");
		return irq;
	}

	ret = nemadc_irq_hw_init(drm, irq);
	if (ret)
		return ret;


	return 0;
}

static void nemadc_lastclose(struct drm_device *drm)
{
	struct nemadc_drm *namedc = drm->dev_private;

	drm_fbdev_cma_restore_mode(namedc->fbdev);
}

DEFINE_DRM_GEM_CMA_FOPS(fops);

static struct drm_driver nemadc_driver = {
	.driver_features = DRIVER_GEM | DRIVER_MODESET | DRIVER_ATOMIC |
			   DRIVER_PRIME,
	.lastclose = nemadc_lastclose,
	.gem_free_object_unlocked = drm_gem_cma_free_object,
	.gem_vm_ops = &drm_gem_cma_vm_ops,
	.dumb_create = drm_gem_cma_dumb_create,
	
	.prime_handle_to_fd = drm_gem_prime_handle_to_fd,
	.prime_fd_to_handle = drm_gem_prime_fd_to_handle,
	.gem_prime_export = drm_gem_prime_export,
	.gem_prime_import = drm_gem_prime_import,
	.gem_prime_get_sg_table = drm_gem_cma_prime_get_sg_table,
	.gem_prime_import_sg_table = drm_gem_cma_prime_import_sg_table,
	.gem_prime_vmap = drm_gem_cma_prime_vmap,
	.gem_prime_vunmap = drm_gem_cma_prime_vunmap,
	.gem_prime_mmap = drm_gem_cma_prime_mmap,
	
	.fops = &fops,
	.name = "nemadc",
	.desc = "Nema DC Display Processor driver",
	.date = "20220711",
	.major = 1,
	.minor = 0,
};

static const struct of_device_id  nemadc_drm_of_match[] = {
	{
		.compatible = "thinksil,nemadc",
		.data = &nemadc_device
	},
	{},
};
MODULE_DEVICE_TABLE(of, nemadc_drm_of_match);


static bool nemadc_has_sufficient_address_space(const struct resource *res,
					const struct of_device_id *dev_id)
{
	resource_size_t res_size = resource_size(res);
	if (res_size < NEMADC_ADDR_SPACE_SIZE)
		return false;

	return true;
}

static ssize_t nemadc_core_id_show(struct device *dev, struct device_attribute *attr,
			    char *buf)
{
	struct drm_device *drm = dev_get_drvdata(dev);
	struct nemadc_drm *namedc = drm->dev_private;

	return snprintf(buf, PAGE_SIZE, "%08x\n", namedc->core_id);
}

DEVICE_ATTR_RO(nemadc_core_id);

static int nemadc_init_sysfs(struct device *dev)
{
	int ret = device_create_file(dev, &dev_attr_nemadc_core_id);

	if (ret)
		DRM_ERROR("failed to create device file for core_id\n");

	return ret;
}

static void nemadc_fini_sysfs(struct device *dev)
{
	device_remove_file(dev, &dev_attr_nemadc_core_id);
}

static int nemadc_runtime_pm_suspend(struct device *dev)
{
	struct drm_device *drm = dev_get_drvdata(dev);
	struct nemadc_drm *namedc = drm->dev_private;
	struct nemadc_hw_device *hwdev = namedc->dev;

	hwdev->pm_suspended = true;
	// clk_disable_unprepare(hwdev->mclk);
	// clk_disable_unprepare(hwdev->aclk);
	// clk_disable_unprepare(hwdev->pclk);

#ifdef CONFIG_ARCH_TS_TX5336
	clk_disable_unprepare(hwdev->aclk);
#endif
	clk_disable_unprepare(hwdev->hclk);
	clk_disable_unprepare(hwdev->cclk);

	return 0;
}

static int nemadc_runtime_pm_resume(struct device *dev)
{
	struct drm_device *drm = dev_get_drvdata(dev);
	struct nemadc_drm *namedc = drm->dev_private;
	struct nemadc_hw_device *hwdev = namedc->dev;

	// clk_prepare_enable(hwdev->pclk);
	// clk_prepare_enable(hwdev->mclk);

#ifdef CONFIG_ARCH_TS_TX5336
	clk_prepare_enable(hwdev->aclk);
#endif
	clk_prepare_enable(hwdev->hclk);
	clk_prepare_enable(hwdev->cclk);

	hwdev->pm_suspended = false;

	return 0;
}





/*******test*******/
// void nemadc_timing(struct nemadc_hw_device* hwdev,int resx, int fpx, int blx, int bpx,
// 	int resy, int fpy, int bly, int bpy)
// {
// 	int dc_fpx = resx + fpx;
// 	int dc_blx = dc_fpx + blx;
// 	int dc_bpx = dc_blx + bpx;
// 	int dc_fpy = resy + fpy;
// 	int dc_bly = dc_fpy + bly;
// 	int dc_bpy = dc_bly + bpy;

// 	nemadc_hw_write(hwdev, (resx << 16) + resy, NEMADC_REG_RESXY);
// 	nemadc_hw_write(hwdev, (dc_fpx << 16) + dc_fpy, NEMADC_REG_FRONTPORCHXY);
// 	nemadc_hw_write(hwdev, (dc_blx << 16) + dc_bly, NEMADC_REG_BLANKINGXY);
// 	nemadc_hw_write(hwdev, (dc_bpx << 16) + dc_bpy, NEMADC_REG_BACKPORCHXY);
// }





static int nemadc_bind(struct device *dev)
{
	struct resource *res;
	struct drm_device *drm;
	struct nemadc_drm *namedc;
	struct nemadc_hw_device *hwdev;
	struct platform_device *pdev = to_platform_device(dev);
	struct of_device_id const *dev_id;
	/* number of lines for the R, G and B output */
	u8 output_width[MAX_OUTPUT_CHANNELS];
	int ret = 0/*, i*/;
	u32 version, clk_ctrl/*,val,out_depth = 0*/;

	namedc = devm_kzalloc(dev, sizeof(*namedc), GFP_KERNEL);
	if (!namedc)
		return -ENOMEM;

	hwdev = devm_kzalloc(dev, sizeof(*hwdev), GFP_KERNEL);
	if (!hwdev)
		return -ENOMEM;

	/*
	 * copy the associated data from nemadc_drm_of_match to avoid
	 * having to keep a reference to the OF node after binding
	 */
	memcpy(hwdev, of_device_get_match_data(dev), sizeof(*hwdev));
	namedc->dev = hwdev;
	/*  Get reg info */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	hwdev->regs = devm_ioremap_resource(dev, res);
	if (IS_ERR(hwdev->regs))
		return PTR_ERR(hwdev->regs);

	hwdev->hclk = devm_clk_get(dev, "hclk");
	if (IS_ERR(hwdev->hclk))
		return PTR_ERR(hwdev->hclk);
	hwdev->cclk = devm_clk_get(dev, "cclk");
	if (IS_ERR(hwdev->cclk))
		return PTR_ERR(hwdev->cclk);
#ifdef CONFIG_ARCH_TS_TX5336
	hwdev->aclk = devm_clk_get(dev, "aclk");
	if (IS_ERR(hwdev->aclk))
		return PTR_ERR(hwdev->aclk);
#endif

	/* Get the optional framebuffer memory resource */
	ret = of_reserved_mem_device_init(dev);
	if (ret && ret != -ENODEV)
		return ret;
	
	drm = drm_dev_alloc(&nemadc_driver, dev);
	if (IS_ERR(drm)) {
		ret = PTR_ERR(drm);
		goto alloc_fail;
	}
	
	drm->dev_private = namedc;
	dev_set_drvdata(dev, drm);

	/* Enable power management */
	pm_runtime_enable(dev);

	/* Resume device to enable the clocks */
	if (pm_runtime_enabled(dev))
		pm_runtime_get_sync(dev);
	else
		nemadc_runtime_pm_resume(dev);

	dev_id = of_match_device(nemadc_drm_of_match, dev);
	if (!dev_id) {
		ret = -EINVAL;
		goto query_hw_fail;
	}

	if (!nemadc_has_sufficient_address_space(res, dev_id)) {
		DRM_ERROR("Insufficient address space in device-tree.\n");
		ret = -EINVAL;
		goto query_hw_fail;
	}

	ret = hwdev->query_hw(hwdev);
	if (ret) {
		DRM_ERROR("Invalid HW configuration\n");
		goto query_hw_fail;
	}

	version = nemadc_hw_read(hwdev, NEMADC_REG_ID);
	if (version != NM_ID_VAL)
	{
		DRM_ERROR("NemaDC id %x error\n", version);
		goto query_hw_fail;
	}
	DRM_INFO("found NemaDC, id %x\n", version);

	namedc->core_id = version;

	//dpu clk div 10
	clk_ctrl = nemadc_hw_read(hwdev, NEMADC_REG_CLKCTRL);
	clk_ctrl &= ~(0x3f);

#ifndef CONFIG_ARCH_TS_TX5215
#ifdef CONFIG_DRM_NEMADC_MIPI
	clk_ctrl |= 0;
#else
	#ifdef CONFIG_DRM_NEMADC_SPI
		clk_ctrl |= NEMADC_REG_CLK_D5;
	#else	
		clk_ctrl |= NEMADC_REG_CLK_D10;
	#endif
#endif
#endif

	nemadc_hw_write(hwdev,clk_ctrl ,NEMADC_REG_CLKCTRL);

	/* set the number of lines used for output of RGB data */
	ret = of_property_read_u8_array(dev->of_node,"namedc-output-port-lines",output_width, MAX_OUTPUT_CHANNELS);
	if (ret)
		goto query_hw_fail;
	
	// val = nemadc_hw_read(hwdev, NEMADC_REG_MODE);
	// val &= ~(0x7 << NM_MODE_DPI_CFG_SHF);
	// val |= (0x4 << NM_MODE_DPI_CFG_SHF);
	//nemadc_hw_write(hwdev, val, NEMADC_REG_MODE);

	
	// ret = of_property_read_u8_array(dev->of_node,
	// 	"namedc-output-port-lines",
	// 	output_width, MAX_OUTPUT_CHANNELS);
	// if (ret)
	// 	goto query_hw_fail;
	// for (i = 0; i < MAX_OUTPUT_CHANNELS; i++)
	// 	out_depth = (out_depth << 8) | (output_width[i] & 0xf);
	// DRM_INFO("out_depth %x\n", out_depth);
	// // nemadc_hw_write(hwdev, out_depth, NEMADC_REG_MODE);

#ifdef CONFIG_DRM_NEMADC_SPI
	nemadc_MIPI_init(hwdev,pdev->dev);
#endif
	
	atomic_set(&namedc->config_valid, 0);
	init_waitqueue_head(&namedc->wq);

	ret = nemadc_init(drm);
	if (ret < 0)
		goto query_hw_fail;

	ret = nemadc_init_sysfs(dev);
	if (ret)
		goto init_fail;

	/* Set the CRTC's port so that the encoder component can find it */
	namedc->crtc.port = of_graph_get_port_by_id(dev->of_node, 0);

	ret = component_bind_all(dev, drm);
	if (ret) {
		DRM_ERROR("Failed to bind all components\n");
		goto bind_fail;
	}

	ret = nemadc_irq_init(pdev);
	if (ret < 0)
		goto irq_init_fail;

	drm->irq_enabled = true;

	ret = drm_vblank_init(drm, drm->mode_config.num_crtc);
	drm_crtc_vblank_reset(&namedc->crtc);
	if (ret < 0) {
		DRM_ERROR("failed to initialise vblank\n");
		goto vblank_fail;
	}
	pm_runtime_put(dev);

	drm_mode_config_reset(drm);

	namedc->fbdev = drm_fbdev_cma_init(drm, 32,
					   drm->mode_config.num_connector);

	if (IS_ERR(namedc->fbdev)) {
		ret = PTR_ERR(namedc->fbdev);
		namedc->fbdev = NULL;
		goto fbdev_fail;
	}

	drm_kms_helper_poll_init(drm);

	ret = drm_dev_register(drm, 0);
	if (ret)
		goto register_fail;
	
	return 0;

register_fail:
	if (namedc->fbdev) {
		drm_fbdev_cma_fini(namedc->fbdev);
		namedc->fbdev = NULL;
	}
	drm_kms_helper_poll_fini(drm);
fbdev_fail:
	pm_runtime_get_sync(dev);
vblank_fail:
	nemadc_irq_hw_fini(drm);
	drm->irq_enabled = false;
irq_init_fail:
	component_unbind_all(dev, drm);
bind_fail:
	of_node_put(namedc->crtc.port);
	namedc->crtc.port = NULL;
init_fail:
	nemadc_fini_sysfs(dev);
	nemadc_fini(drm);
query_hw_fail:
	pm_runtime_put(dev);
	if (pm_runtime_enabled(dev))
		pm_runtime_disable(dev);
	else
		nemadc_runtime_pm_suspend(dev);
	drm->dev_private = NULL;
	dev_set_drvdata(dev, NULL);
	drm_dev_unref(drm);
alloc_fail:
	of_reserved_mem_device_release(dev);

	return ret;
}

static void nemadc_unbind(struct device *dev)
{
	struct drm_device *drm = dev_get_drvdata(dev);
	struct nemadc_drm *namedc = drm->dev_private;

	drm_dev_unregister(drm);
	if (namedc->fbdev) {
		drm_fbdev_cma_fini(namedc->fbdev);
		namedc->fbdev = NULL;
	}
	drm_kms_helper_poll_fini(drm);
	pm_runtime_get_sync(dev);
	nemadc_irq_hw_fini(drm);
	component_unbind_all(dev, drm);
	of_node_put(namedc->crtc.port);
	namedc->crtc.port = NULL;
	nemadc_fini_sysfs(dev);
	nemadc_fini(drm);
	pm_runtime_put(dev);
	if (pm_runtime_enabled(dev))
		pm_runtime_disable(dev);
	else
		nemadc_runtime_pm_suspend(dev);
	drm->dev_private = NULL;
	dev_set_drvdata(dev, NULL);
	drm_dev_unref(drm);
	of_reserved_mem_device_release(dev);
}

static const struct component_master_ops nemadc_master_ops = {
	.bind = nemadc_bind,
	.unbind = nemadc_unbind,
};

static int nemadc_compare_dev(struct device *dev, void *data)
{
	struct device_node *np = data;

	return dev->of_node == np;
}

static int nemadc_platform_probe(struct platform_device *pdev)
{
	struct device_node *port;
	struct component_match *match = NULL;

	if (!pdev->dev.of_node)
		return -ENODEV;

	/* there is only one output port inside each device, find it */
	port = of_graph_get_remote_node(pdev->dev.of_node, 0, 0);
	if (!port)
		return -ENODEV;

	drm_of_component_match_add(&pdev->dev, &match, nemadc_compare_dev,port);
	of_node_put(port);
	return component_master_add_with_match(&pdev->dev, &nemadc_master_ops,match);
}

static int nemadc_platform_remove(struct platform_device *pdev)
{
	component_master_del(&pdev->dev, &nemadc_master_ops);
	return 0;
}

static int __maybe_unused nemadc_pm_suspend(struct device *dev)
{
	struct drm_device *drm = dev_get_drvdata(dev);
	struct nemadc_drm *namedc = drm->dev_private;

	drm_kms_helper_poll_disable(drm);
	console_lock();
	drm_fbdev_cma_set_suspend(namedc->fbdev, 1);
	console_unlock();
	namedc->pm_state = drm_atomic_helper_suspend(drm);
	if (IS_ERR(namedc->pm_state)) {
		console_lock();
		drm_fbdev_cma_set_suspend(namedc->fbdev, 0);
		console_unlock();
		drm_kms_helper_poll_enable(drm);
		return PTR_ERR(namedc->pm_state);
	}

	return 0;
}

static int __maybe_unused nemadc_pm_resume(struct device *dev)
{
	struct drm_device *drm = dev_get_drvdata(dev);
	struct nemadc_drm *nemadc = drm->dev_private;

	drm_atomic_helper_resume(drm, nemadc->pm_state);
	console_lock();
	drm_fbdev_cma_set_suspend(nemadc->fbdev, 0);
	console_unlock();
	drm_kms_helper_poll_enable(drm);

	return 0;
}

static const struct dev_pm_ops nemadc_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(nemadc_pm_suspend, nemadc_pm_resume)
	SET_RUNTIME_PM_OPS(nemadc_runtime_pm_suspend, nemadc_runtime_pm_resume,
		NULL)
};

static struct platform_driver nemadc_platform_driver = {
	.probe		= nemadc_platform_probe,
	.remove		= nemadc_platform_remove,
	.driver	= {
		.name = "nemadc",
		.pm = &nemadc_pm_ops,
		.of_match_table	= nemadc_drm_of_match,
	},
};

module_platform_driver(nemadc_platform_driver);

MODULE_DESCRIPTION("NemaDC DRM driver");
MODULE_LICENSE("GPL v2");
