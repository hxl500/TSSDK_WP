/*
 * (C) COPYRIGHT 2016 ARM Limited. All rights reserved.
 * Author: Liviu Dudau <Liviu.Dudau@arm.com>
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * ARM Mali DP500/DP550/DP650 KMS/DRM driver structures
 */

#ifndef __NEMADC_DRV_H__
#define __NEMADC_DRV_H__

#include <linux/mutex.h>
#include <linux/wait.h>
#include <drm/drmP.h>
#include "nemadc_hw.h"

struct nemadc_drm {
	struct nemadc_hw_device *dev;
	struct drm_fbdev_cma *fbdev;
	struct drm_crtc crtc;
	wait_queue_head_t wq;
	atomic_t config_valid;
	struct drm_atomic_state *pm_state;
	u32 core_id;
};

#define crtc_to_nemadc_device(x) container_of(x, struct nemadc_drm, crtc)

struct nemadc_plane {
	struct drm_plane base;
	struct nemadc_hw_device *hwdev;
	const struct nemadc_layer *layer;
	struct drm_property *p_alpha;
	struct drm_property *p_zpos;
};

struct nemadc_plane_state {
	struct drm_plane_state base;

	/* size of the required rotation memory if plane is rotated */
	u32 rotmem_size;
	/* internal format ID */
	u8 format;
	u8 n_planes;
	unsigned int alpha;
	unsigned int zpos;
};

#define to_nemadc_plane(x) container_of(x, struct nemadc_plane, base)
#define to_nemadc_plane_state(x) \
	container_of(x, struct nemadc_plane_state, base)

struct nemadc_crtc_state {
	struct drm_crtc_state base;
	u32 gamma_coeffs[NEMADC_GAMMA_LUT_NUM];
};

#define to_nemadc_crtc_state(x) container_of(x, struct nemadc_crtc_state, base)

int nemadc_planes_init(struct drm_device *drm);
void nemadc_planes_destroy(struct drm_device *drm);
int nemadc_crtc_init(struct drm_device *drm);






/* test */

typedef enum {
	DPU_SIGNAL_ACTIVE_LOW = 0,
	DPU_SIGNAL_ACTIVE_HIGH = 1,
} dpu_signal_polarity_e;



typedef struct __nemadc_display_t {
	u32 resx, resy;
	u32 fpx, fpy;
	u32 bpx, bpy;
	u32 blx, bly;
} nemadc_display_t;


typedef struct {
	u32 resolution_x;      ///< Horizontal valid data width
	u32 resolution_y;      ///< Vertical valid data width
	u32 hsync_width;       ///< HSync pulse width
	u32 vsync_width;       ///< VSync pluse width
	u32 front_porch_x;     ///< HSync front porch
	u32 front_porch_y;     ///< VSync front porch
	u32 back_porch_x;      ///< HSync back porch
	u32 back_porch_y;      ///< VSync back porch
} dpu_cfg_display_timing_t;

typedef struct {
	dpu_signal_polarity_e vsync;        ///< vsync signal polarity
	dpu_signal_polarity_e hsync;        ///< hsync signal polarity
	dpu_signal_polarity_e deta_enable;  ///< deta_enable signal polarity
	dpu_signal_polarity_e pixel_clock;  ///< pixel_clock signal polarity
} dpu_cfg_display_signal_polarity_t;

#define TEST_DPU_RGBA(r,g,b,a) ((r)<<24 | (g)<<16 | (b)<< 8 | a)
typedef enum {
	TEST_DPU_RGBA_BLACK = TEST_DPU_RGBA(0x00, 0x00, 0x00, 0xff),
	TEST_DPU_RGBA_RED = TEST_DPU_RGBA(0xff, 0x00, 0x00, 0xff),
	TEST_DPU_RGBA_GREEN = TEST_DPU_RGBA(0x00, 0xff, 0x00, 0xff),
	TEST_DPU_RGBA_BLUE = TEST_DPU_RGBA(0x00, 0x00, 0xff, 0xff),
	TEST_DPU_RGBA_WHITE = TEST_DPU_RGBA(0xff, 0xff, 0xff, 0xff),
} test_dpu_rgba_e;

#endif  /* __NEMADC_DRV_H__ */
