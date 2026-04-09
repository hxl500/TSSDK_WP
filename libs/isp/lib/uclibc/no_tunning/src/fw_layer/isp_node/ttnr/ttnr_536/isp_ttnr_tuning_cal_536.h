#ifndef __ISP_TTNR_TUNING_CAL_536_H__
#define __ISP_TTNR_TUNING_CAL_536_H__

#include "tune_utils.h"

#define TTNR_CAL_TONE_CURVE_NUM_MAX (20)
#define TTNR_CAL_TONE_CURVE_SIZE (256)

typedef struct cal_ttnr_param cal_ttnr_param_t;
typedef struct cal_ttnr_tone_curves cal_ttnr_tone_curves_t;

struct cal_ttnr_tone_curves {
  u32 valid_num;
  u32 tone_curve[TTNR_CAL_TONE_CURVE_NUM_MAX][TTNR_CAL_TONE_CURVE_SIZE];
};

struct cal_ttnr_param {
  struct cal_ttnr_tone_curves tone_curves; //tone_curve_tbl;
};

#endif
