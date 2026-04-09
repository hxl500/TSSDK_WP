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
#include <drm/drm_fb_cma_helper.h>
#include <drm/drm_gem_cma_helper.h>
#include <drm/drm_plane_helper.h>
#include <drm/drm_print.h>

#include "nemadc_hw.h"
#include "nemadc_drv.h"

/* Layer specific register offsets */
#define NEMADC_LAYER_CONTROL		0x000
#define NM_LAYER_ENABLE		(1 << 31)
#define NEMADC_LAYER_AHBLOCK  (1 << 27)


#define   NM_LAYER_FORMAT_MASK		0x1f
#define NEMADC_LAYER_START		0x004
#define NEMADC_LAYER_SIZE		0x008
#define NEMADC_LAYER_ADDR_P0		0x00C
#define NEMADC_LAYER_STRIDE0		0x010
#define NEMADC_LAYER_RESOL		0x014
#define NEMADC_LAYER_SCALEX		0x018
#define NEMADC_LAYER_SCALEY		0x01C
#define   NM_LAYER_H_VAL(x)		(((x) & 0xffff) << 16)
#define   NM_LAYER_V_VAL(x)		(((x) & 0xffff) << 0)
#define NEMADC_LAYER_ADDR_PU		0xD0
#define NEMADC_LAYER_ADDR_PV		0xD4
#define NEMADC_LAYER_STRIDE_U		0xD8


/*
 * This 4-entry look-up-table is used to determine the full 8-bit alpha value
 * for formats with 1- or 2-bit alpha channels.
 * We set it to give 100%/0% opacity for 1-bit formats and 100%/66%/33%/0%
 * opacity for 2-bit formats.
 */
#define NEMADC_ALPHA_LUT 0xffaa5500

static void nemadc_de_plane_destroy(struct drm_plane *plane)
{
	struct nemadc_plane *mp = to_nemadc_plane(plane);

	if (mp->base.fb)
		drm_framebuffer_unreference(mp->base.fb);

	drm_plane_helper_disable(plane);
	drm_plane_cleanup(plane);
	devm_kfree(plane->dev->dev, mp);
}

/*
 * Replicate what the default ->reset hook does: free the state pointer and
 * allocate a new empty object. We just need enough space to store
 * a nemadc_plane_state instead of a drm_plane_state.
 */
static void nemadc_plane_reset(struct drm_plane *plane)
{
	struct nemadc_plane_state *state = to_nemadc_plane_state(plane->state);

	if (state)
		__drm_atomic_helper_plane_destroy_state(&state->base);
	kfree(state);
	plane->state = NULL;
	state = kzalloc(sizeof(*state), GFP_KERNEL);
	if (state) {
		state->base.plane = plane;
		state->base.rotation = DRM_MODE_ROTATE_0;
		state->alpha = 0xff;
		plane->state = &state->base;
	}
}

static struct
drm_plane_state *nemadc_duplicate_plane_state(struct drm_plane *plane)
{
	struct nemadc_plane_state *state, *m_state;

	if (!plane->state)
		return NULL;

	state = kmalloc(sizeof(*state), GFP_KERNEL);
	if (!state)
		return NULL;

	m_state = to_nemadc_plane_state(plane->state);
	__drm_atomic_helper_plane_duplicate_state(plane, &state->base);
	state->rotmem_size = m_state->rotmem_size;
	state->format = m_state->format;
	state->n_planes = m_state->n_planes;
	state->alpha = m_state->alpha;
	return &state->base;
}

static void nemadc_destroy_plane_state(struct drm_plane *plane,
				       struct drm_plane_state *state)
{
	struct nemadc_plane_state *m_state = to_nemadc_plane_state(state);

	__drm_atomic_helper_plane_destroy_state(state);
	kfree(m_state);
}

static void nemadc_plane_atomic_print_state(struct drm_printer *p,
					    const struct drm_plane_state *state)
{
	struct nemadc_plane_state *ms = to_nemadc_plane_state(state);

	drm_printf(p, "\trotmem_size=%u\n", ms->rotmem_size);
	drm_printf(p, "\tformat_id=%u\n", ms->format);
	drm_printf(p, "\tn_planes=%u\n", ms->n_planes);
	drm_printf(p, "\alpha=%u\n", ms->alpha);
}
 int nemadc_plane_atomic_set_property(struct drm_plane *plane,
					     struct drm_plane_state *state,
					     struct drm_property *property,
					     uint64_t val)
{

	struct nemadc_plane *mp = to_nemadc_plane(plane);
	struct nemadc_plane_state *ms = to_nemadc_plane_state(state);

	if (property == mp->p_alpha)
		ms->alpha = val;
	else if (property == mp->p_zpos)
		ms->zpos = val;
	else
		return -EINVAL;

	return 0;
}
//rcdu->props.alpha
 int nemadc_plane_atomic_get_property(struct drm_plane *plane,
	const struct drm_plane_state *state, struct drm_property *property,
	uint64_t *val)
{
	
	struct nemadc_plane *mp = to_nemadc_plane(plane);
	struct nemadc_plane_state *ms = to_nemadc_plane_state(state);

	if (property == mp->p_alpha)
		*val = ms->alpha;
	else if (property == mp->p_zpos)
		*val = ms->zpos;
	else
		return -EINVAL;

	return 0;
}
static const struct drm_plane_funcs nemadc_plane_funcs = {
	.update_plane = drm_atomic_helper_update_plane,
	.disable_plane = drm_atomic_helper_disable_plane,
	.destroy = nemadc_de_plane_destroy,
	.reset = nemadc_plane_reset,
	.atomic_duplicate_state = nemadc_duplicate_plane_state,
	.atomic_destroy_state = nemadc_destroy_plane_state,
	.atomic_print_state = nemadc_plane_atomic_print_state,
	.atomic_set_property = nemadc_plane_atomic_set_property,
	.atomic_get_property = nemadc_plane_atomic_get_property,
};

static int nemadc_check_scaling(struct nemadc_plane *mp,
				   struct drm_plane_state *state)
{
	struct drm_crtc_state *crtc_state =
		drm_atomic_get_existing_crtc_state(state->state, state->crtc);
	struct nemadc_crtc_state *mc;
	struct drm_rect clip = { 0 };
	u32 src_w, src_h;
	int ret;

	if (!crtc_state)
		return -EINVAL;

	mc = to_nemadc_crtc_state(crtc_state);

	clip.x2 = crtc_state->adjusted_mode.hdisplay;
	clip.y2 = crtc_state->adjusted_mode.vdisplay;
	ret = drm_plane_helper_check_state(state, &clip, 0, INT_MAX, true,
		true);
	if (ret)
		return ret;

	/* not support scaling */
	src_w = state->src_w >> 16;
	src_h = state->src_h >> 16;
	if ((state->crtc_w != src_w) || (state->crtc_h != src_h)) {
		DRM_INFO("%s not support scale\n", __func__);
		return -EINVAL;
	}

	return 0;
}

static int nemadc_plane_check(struct drm_plane *plane,
				 struct drm_plane_state *state)
{
	struct nemadc_plane *mp = to_nemadc_plane(plane);
	struct nemadc_plane_state *ms = to_nemadc_plane_state(state);
	struct drm_framebuffer *fb;
	int i, ret;

	if (!state->crtc || !state->fb)
		return 0;

	fb = state->fb;

	ms->format = nemadc_hw_get_format_id(&mp->hwdev->map, mp->layer->id,
					    fb->format->format);
	if (ms->format == NEMADC_INVALID_FORMAT_ID)
		return -EINVAL;

	ms->n_planes = fb->format->num_planes;
	for (i = 0; i < ms->n_planes; i++) {
		if (!nemadc_hw_pitch_valid(mp->hwdev, fb->pitches[i])) {
			DRM_DEBUG_KMS("Invalid pitch %u for plane %d\n",
				      fb->pitches[i], i);
			return -EINVAL;
		}
	}

	if ((state->crtc_w > mp->hwdev->max_width) ||
	    (state->crtc_h > mp->hwdev->max_height) ||
	    (state->crtc_w < mp->hwdev->min_width) ||
	    (state->crtc_h < mp->hwdev->min_height))
		return -EINVAL;

	/*
	 * video layers can accept 3 plane formats only if
	 * fb->pitches[1] == fb->pitches[2] since they don't have a
	 * third plane stride register.
	 */
	if (ms->n_planes == 3 &&
	    !(mp->hwdev->features & NEMADC_DEVICE_LV_HAS_3_STRIDES) &&
	    (state->fb->pitches[1] != state->fb->pitches[2]))
		return -EINVAL;

	ret = nemadc_check_scaling(mp, state);
	if (ret)
		return ret;

	return 0;
}

static void nemadc_set_plane_pitches(struct nemadc_plane *mp,
					int num_planes, unsigned int pitches[3])
{
	int num_strides = num_planes;
	u16 base;

	if (num_planes == 3)
		num_strides = (mp->hwdev->features & NEMADC_DEVICE_LV_HAS_3_STRIDES) ? 3 : 2;
	
	base = mp->layer->base;
	nemadc_hw_write(mp->hwdev, pitches[0], base + NEMADC_LAYER_STRIDE0);
	nemadc_hw_write(mp->hwdev, pitches[1] | (pitches[2] << 16), NEMADC_LAYER_STRIDE_U);
}

static int nemadc_plane_get_addr(struct drm_plane *plane, int img_plane)
{
	int reg_off = NEMADC_LAYER_ADDR_P0;
	u32 format;
	struct nemadc_plane *mp;

	mp = to_nemadc_plane(plane);
	format = plane->state->fb->format->format;

	switch (img_plane) {
	case 0:
		reg_off = mp->layer->base + NEMADC_LAYER_ADDR_P0;
		break;
	case 1:
		if (format == DRM_FORMAT_YVU420)
			reg_off = NEMADC_LAYER_ADDR_PV;
		else
			reg_off = NEMADC_LAYER_ADDR_PU;
		break;
	case 2:
		if (format == DRM_FORMAT_YVU420)
			reg_off = NEMADC_LAYER_ADDR_PU;
		else
			reg_off = NEMADC_LAYER_ADDR_PV;
		break;
	default:
		DRM_ERROR("%s input plane %d error\n", __func__, img_plane);
		reg_off = NEMADC_LAYER_ADDR_P0;
		break;
	}

	return reg_off;
}

static void nemadc_plane_update(struct drm_plane *plane,struct drm_plane_state *old_state)
{
	struct nemadc_plane *mp;
	const struct nemadc_hw_regmap *map;
	struct nemadc_plane_state *ms = to_nemadc_plane_state(plane->state);
	u32 src_w, src_h, dest_w, dest_h, val;
	int i;
	u32 reg_off;
	dma_addr_t fb_addr;

	mp = to_nemadc_plane(plane);
	map = &mp->hwdev->map;

	/* convert src values from Q16 fixed point to integer */
	src_w = plane->state->src_w >> 16;
	src_h = plane->state->src_h >> 16;
	dest_w = plane->state->crtc_w;
	dest_h = plane->state->crtc_h;
	
	//DRM_INFO("--mp->layer->base %x--\n", mp->layer->base);
	val = nemadc_hw_read(mp->hwdev, mp->layer->base);
	val = (val & ~NM_LAYER_FORMAT_MASK) | ms->format;
	//DRM_INFO("--val %d--\n", val);
	nemadc_hw_write(mp->hwdev, val, mp->layer->base);
	//DRM_INFO("--mp->n_planes %d--\n", ms->n_planes);

	//baseaddr
	for (i = 0; i < ms->n_planes; i++)
	{
		reg_off = nemadc_plane_get_addr(plane, i);
		//DRM_INFO("--reg_off %d--\n", reg_off);
		fb_addr = drm_fb_cma_get_gem_addr(plane->state->fb, plane->state, i);
		nemadc_hw_write(mp->hwdev, lower_32_bits(fb_addr), reg_off);
	}
	
	nemadc_set_plane_pitches(mp, ms->n_planes,plane->state->fb->pitches);

	nemadc_hw_write(mp->hwdev, NM_LAYER_H_VAL(src_w) | NM_LAYER_V_VAL(src_h),mp->layer->base + NEMADC_LAYER_RESOL);

	nemadc_hw_write(mp->hwdev, NM_LAYER_H_VAL(dest_w) | NM_LAYER_V_VAL(dest_h),mp->layer->base + NEMADC_LAYER_SIZE);

	nemadc_hw_write(mp->hwdev,NM_LAYER_H_VAL(plane->state->crtc_x) | NM_LAYER_V_VAL(plane->state->crtc_y),mp->layer->base + NEMADC_LAYER_START);

	
//	DRM_INFO("ms->zpos %x \r\n",ms->zpos);
	DRM_INFO("ms->alpha %x \r\n",ms->alpha);
	/* set the 'enable layer' bit */
	val =  val | NM_LAYER_ENABLE | NEMADC_LAYER_AHBLOCK | (1 << 30) | (ms->alpha << 16) | (0x52 << 8);
	//val = val | NM_LAYER_ENABLE | NEMADC_LAYER_AHBLOCK | (1 << 30) | (ms->alpha << 16) | (ms->zpos << 12) | (0x52 << 8);
	nemadc_hw_write(mp->hwdev, val, mp->layer->base + NEMADC_LAYER_CONTROL);
}

static void nemadc_plane_disable(struct drm_plane *plane,
				    struct drm_plane_state *state)
{
	struct nemadc_plane *mp = to_nemadc_plane(plane);

	nemadc_hw_clearbits(mp->hwdev, NM_LAYER_ENABLE,
		mp->layer->base + NEMADC_LAYER_CONTROL);
}

static const struct drm_plane_helper_funcs nemadc_plane_helper_funcs = {
	.atomic_check = nemadc_plane_check,
	.atomic_update = nemadc_plane_update,
	.atomic_disable = nemadc_plane_disable,
};

int nemadc_planes_init(struct drm_device *drm)
{
	struct nemadc_drm *namedc = drm->dev_private;
	const struct nemadc_hw_regmap *map = &namedc->dev->map;
	struct nemadc_plane *plane = NULL;
	enum drm_plane_type plane_type;
	unsigned long crtcs = BIT(drm->mode_config.num_crtc);
	u32 *formats;
	int ret, i, j, n;

	formats = kcalloc(map->n_pixel_formats, sizeof(*formats), GFP_KERNEL);
	if (!formats) {
		ret = -ENOMEM;
		goto cleanup;
	}

	for (i = 0; i < map->n_layers; i++) {
		u8 id = map->layers[i].id;

		plane = kzalloc(sizeof(*plane), GFP_KERNEL);
		if (!plane) {
			ret = -ENOMEM;
			goto cleanup;
		}

		/* build the list of DRM supported formats based on the map */
		for (n = 0, j = 0;  j < map->n_pixel_formats; j++) {
			if ((map->pixel_formats[j].layer & id) == id)
				formats[n++] = map->pixel_formats[j].format;
		}

		plane_type = (i == 0) ? DRM_PLANE_TYPE_PRIMARY :
					DRM_PLANE_TYPE_OVERLAY;
		ret = drm_universal_plane_init(drm, &plane->base, crtcs,
					       &nemadc_plane_funcs, formats,
					       n, NULL, plane_type, NULL);
		if (ret < 0)
			goto cleanup;

		drm_plane_helper_add(&plane->base,
				     &nemadc_plane_helper_funcs);
		plane->hwdev = namedc->dev;
		plane->layer = &map->layers[i];
		//zhc zpos
		plane->p_zpos = drm_property_create_range(drm, 0, "p_zpos", 0, 8);
		if (plane->p_zpos == NULL)
			return -ENOMEM;
		drm_object_attach_property(&plane->base.base,plane->p_zpos, 0);
		plane->p_alpha = drm_property_create_range(drm, 0, "p_alpha", 0, 255);
		if (plane->p_alpha == NULL)
			return -ENOMEM;
		drm_object_attach_property(&plane->base.base, plane->p_alpha, 255);
		}

	kfree(formats);

	return 0;

cleanup:
	nemadc_planes_destroy(drm);
	kfree(formats);

	return ret;
}

void nemadc_planes_destroy(struct drm_device *drm)
{
	struct drm_plane *p, *pt;

	list_for_each_entry_safe(p, pt, &drm->mode_config.plane_list, head) {
		drm_plane_cleanup(p);
		kfree(p);
	}
}
