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


#include <drm/drmP.h>
#include <drm/drm_atomic.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_crtc.h>
#include <drm/drm_crtc_helper.h>
#include <linux/clk.h>
#include <linux/pm_runtime.h>
#include <video/videomode.h>

#include "nemadc_drv.h"
#include "nemadc_hw.h"

static enum drm_mode_status nemadc_crtc_mode_valid(struct drm_crtc *crtc,const struct drm_display_mode *mode)
{
	struct nemadc_drm *namedc = crtc_to_nemadc_device(crtc);
	struct nemadc_hw_device *hwdev = namedc->dev;

	/*
	 * check that the hardware can drive the required clock rate,
	 * but skip the check if the clock is meant to be disabled(req_rate = 0)
	 */
	long rate, req_rate = mode->crtc_clock * 1000;

	if (req_rate) {
		/*
		 * TODO: switch to request clk
		 * rate = clk_round_rate(hwdev->pxlclk, req_rate);
		 * if (rate != req_rate) {
		 *	DRM_DEBUG_DRIVER("pxlclk doesn't support %ld Hz\n",
		 *			 req_rate);
		 *	return MODE_NOCLOCK;
		 * }
		 */
		rate = clk_get_rate(hwdev->pxlclk);
		DRM_DEBUG_DRIVER("reqclk: %ldHz, pxlclk %ld Hz\n", req_rate,
			rate);
	}

	return MODE_OK;
}

static void nemadc_crtc_atomic_enable(struct drm_crtc *crtc,
				      struct drm_crtc_state *old_state)
{
	struct nemadc_drm *namedc = crtc_to_nemadc_device(crtc);
	struct nemadc_hw_device *hwdev = namedc->dev;
	struct videomode vm;
	int err = pm_runtime_get_sync(crtc->dev->dev);

	if (err < 0) {
		DRM_DEBUG_DRIVER(
			"Failed to enable runtime power management: %d\n", err);
		return;
	}

	drm_display_mode_to_videomode(&crtc->state->adjusted_mode, &vm);
	clk_prepare_enable(hwdev->pxlclk);

	/* We rely on firmware to set mclk to a sensible level. */
	/*
	 * TODO:
	 * clk_set_rate(hwdev->pxlclk,
	 * crtc->state->adjusted_mode.crtc_clock * 1000);
	 */
	DRM_DEBUG_DRIVER("reqclk: %dHz, pxlclk %ld Hz\n",
		crtc->state->adjusted_mode.crtc_clock * 1000,
		clk_get_rate(hwdev->pxlclk));

	hwdev->modeset(hwdev, &vm);
	hwdev->enable(hwdev);
	drm_crtc_vblank_on(crtc);
}

static void nemadc_crtc_atomic_disable(struct drm_crtc *crtc,
				       struct drm_crtc_state *old_state)
{
	struct nemadc_drm *namedc = crtc_to_nemadc_device(crtc);
	struct nemadc_hw_device *hwdev = namedc->dev;
	int err;

	drm_crtc_vblank_off(crtc);
	hwdev->disable(hwdev);
	clk_disable_unprepare(hwdev->pxlclk);

	err = pm_runtime_put(crtc->dev->dev);
	if (err < 0) {
		DRM_DEBUG_DRIVER(
			"Failed to disable runtime power management: %d\n",
			err);
	}
}

static void nemadc_generate_gamma_table(struct drm_property_blob *lut_blob,
					u32 lut[NEMADC_GAMMA_LUT_NUM])
{
	struct drm_color_lut *state_lut = (struct drm_color_lut *)lut_blob->data;
	int i;

	for (i = 0; i < NEMADC_GAMMA_LUT_NUM; ++i)
		lut[i] = ((state_lut->red & 0xFF) << 16)
			| ((state_lut->green & 0xFF) << 8)
			| ((state_lut->red & 0xFF) << 0);
}

/*
 * Check if there is a new gamma LUT and if it is of an acceptable size. Also,
 * reject any LUTs that use distinct red, green, and blue curves.
 */
static int nemadc_crtc_atomic_check_gamma(struct drm_crtc *crtc,
					  struct drm_crtc_state *state)
{
	struct nemadc_crtc_state *mc = to_nemadc_crtc_state(state);
	struct drm_color_lut *lut;
	size_t lut_size;
	int i;

	if (!state->color_mgmt_changed || !state->gamma_lut)
		return 0;

	if (crtc->state->gamma_lut &&
	    (crtc->state->gamma_lut->base.id == state->gamma_lut->base.id))
		return 0;

	if (state->gamma_lut->length % sizeof(struct drm_color_lut))
		return -EINVAL;

	lut_size = state->gamma_lut->length / sizeof(struct drm_color_lut);
	if (lut_size != NEMADC_GAMMA_LUT_SIZE)
		return -EINVAL;

	lut = (struct drm_color_lut *)state->gamma_lut->data;
	for (i = 0; i < lut_size; ++i)
		if (!((lut[i].red == lut[i].green) &&
		      (lut[i].red == lut[i].blue)))
			return -EINVAL;

	if (!state->mode_changed) {
		int ret;

		state->mode_changed = true;
		/*
		 * Kerneldoc for drm_atomic_helper_check_modeset mandates that
		 * it be invoked when the driver sets ->mode_changed. Since
		 * changing the gamma LUT doesn't depend on any external
		 * resources, it is safe to call it only once.
		 */
		ret = drm_atomic_helper_check_modeset(crtc->dev, state->state);
		if (ret)
			return ret;
	}

	nemadc_generate_gamma_table(state->gamma_lut, mc->gamma_coeffs);
	return 0;
}


static int nemadc_crtc_atomic_check(struct drm_crtc *crtc,
				    struct drm_crtc_state *state)
{
	int ret;

	ret = nemadc_crtc_atomic_check_gamma(crtc, state);
	return ret;
}

static const struct drm_crtc_helper_funcs nemadc_crtc_helper_funcs = {
	.mode_valid = nemadc_crtc_mode_valid,
	.atomic_check = nemadc_crtc_atomic_check,
	.atomic_enable = nemadc_crtc_atomic_enable,
	.atomic_disable = nemadc_crtc_atomic_disable,
};

static struct drm_crtc_state *nemadc_crtc_duplicate_state(struct drm_crtc *crtc)
{
	struct nemadc_crtc_state *state, *old_state;

	if (WARN_ON(!crtc->state))
		return NULL;

	old_state = to_nemadc_crtc_state(crtc->state);
	state = kmalloc(sizeof(*state), GFP_KERNEL);
	if (!state)
		return NULL;

	__drm_atomic_helper_crtc_duplicate_state(crtc, &state->base);
	memcpy(state->gamma_coeffs, old_state->gamma_coeffs,
	       sizeof(state->gamma_coeffs));

	return &state->base;
}

static void nemadc_crtc_reset(struct drm_crtc *crtc)
{
	struct nemadc_crtc_state *state = NULL;

	if (crtc->state) {
		state = to_nemadc_crtc_state(crtc->state);
		__drm_atomic_helper_crtc_destroy_state(crtc->state);
	}

	kfree(state);
	state = kzalloc(sizeof(*state), GFP_KERNEL);
	if (state) {
		crtc->state = &state->base;
		crtc->state->crtc = crtc;
	}
}

static void nemadc_crtc_destroy_state(struct drm_crtc *crtc,
				      struct drm_crtc_state *state)
{
	struct nemadc_crtc_state *nemadc_state = NULL;

	if (state) {
		nemadc_state = to_nemadc_crtc_state(state);
		__drm_atomic_helper_crtc_destroy_state(state);
	}

	kfree(nemadc_state);
}

static int nemadc_crtc_enable_vblank(struct drm_crtc *crtc)
{
	struct nemadc_drm *namedc = crtc_to_nemadc_device(crtc);
	struct nemadc_hw_device *hwdev = namedc->dev;

	nemadc_hw_enable_irq(hwdev, hwdev->map.irq_map.vsync_irq);
	return 0;
}

static void nemadc_crtc_disable_vblank(struct drm_crtc *crtc)
{
	struct nemadc_drm *namedc = crtc_to_nemadc_device(crtc);
	struct nemadc_hw_device *hwdev = namedc->dev;

	nemadc_hw_disable_irq(hwdev, hwdev->map.irq_map.vsync_irq);
}

static const struct drm_crtc_funcs nemadc_crtc_funcs = {
	.gamma_set = drm_atomic_helper_legacy_gamma_set,
	.destroy = drm_crtc_cleanup,
	.set_config = drm_atomic_helper_set_config,
	.page_flip = drm_atomic_helper_page_flip,
	.reset = nemadc_crtc_reset,
	.atomic_duplicate_state = nemadc_crtc_duplicate_state,
	.atomic_destroy_state = nemadc_crtc_destroy_state,
	.enable_vblank = nemadc_crtc_enable_vblank,
	.disable_vblank = nemadc_crtc_disable_vblank,
};

int nemadc_crtc_init(struct drm_device *drm)
{
	struct nemadc_drm *namedc = drm->dev_private;
	struct drm_plane *primary = NULL, *plane;
	int ret;

	ret = nemadc_planes_init(drm);
	if (ret < 0) {
		DRM_ERROR("Failed to initialise planes\n");
		return ret;
	}

	drm_for_each_plane(plane, drm) {
		if (plane->type == DRM_PLANE_TYPE_PRIMARY) {
			primary = plane;
			break;
		}
	}

	if (!primary) {
		DRM_ERROR("no primary plane found\n");
		ret = -EINVAL;
		goto crtc_cleanup_planes;
	}

	ret = drm_crtc_init_with_planes(drm, &namedc->crtc, primary, NULL,
					&nemadc_crtc_funcs, NULL);
	if (ret)
		goto crtc_cleanup_planes;

	drm_crtc_helper_add(&namedc->crtc, &nemadc_crtc_helper_funcs);
	drm_mode_crtc_set_gamma_size(&namedc->crtc, NEMADC_GAMMA_LUT_SIZE);
	/* No inverse-gamma: it is per-plane. */
	drm_crtc_enable_color_mgmt(&namedc->crtc, 0, true,
		NEMADC_GAMMA_LUT_SIZE);

	return 0;

crtc_cleanup_planes:
	nemadc_planes_destroy(drm);

	return ret;
}
