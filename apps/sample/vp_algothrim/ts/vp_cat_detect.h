#ifndef __VP_CAT_DETECT_H__
#define __VP_CAT_DETECT_H__

#include <stdint.h>
#include <stdbool.h>
#include "../vp_algorithm_type.h"
#include "../../vp_video_coder/vp_video_coder_type.h"
#include "video_alg_catdetect-api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VP_CAT_DEFAULT_SCORE       (0.0f)
#define VP_CAT_ACTIVITY_SCORE      (0.0f)

typedef struct vp_cat_detect_handle *vp_cat_detect_handle_p;

vp_cat_detect_handle_p vp_cat_detect_create(uint32_t width, uint32_t height, vp_cat_detect_param_t *param);

int vp_cat_detect_get_param(vp_cat_detect_handle_p handle, vp_cat_detect_param_t *param);

int vp_cat_detect_set_param(vp_cat_detect_handle_p handle, vp_cat_detect_param_t *param);

int vp_cat_detect_process(vp_cat_detect_handle_p handle, ALG_IMAGE_S *rgba_image, uint8_t *yuv_data, uint8_t cam_id);

int vp_cat_detect_result(vp_cat_detect_handle_p handle, vp_cat_detect_result_t *result);

int vp_cat_detect_resume(vp_cat_detect_handle_p handle);

int vp_cat_detect_pause(vp_cat_detect_handle_p handle);

void vp_cat_detect_destroy(vp_cat_detect_handle_p handle);

int vp_cat_detect_should_skip_preprocess(vp_cat_detect_handle_p handle);

#ifdef __cplusplus
}
#endif

#endif
