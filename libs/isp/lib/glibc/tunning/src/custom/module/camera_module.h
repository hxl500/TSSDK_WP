#ifndef __CAMERA_MODULE_H__
#define __CAMERA_MODULE_H__


#include "csi_common.h"
#include "csi2d_common.h"
#include "sensor_common.h"
#include "actuator_common.h"
#include "tune_common.h"
#include "calibration_common.h"

typedef enum {
  CAMERA_IDX0 = 0,
  CAMERA_IDX1 = 1, 
  CAMERA_IDX2 = 2,
  CAMERA_IDX3 = 3, 
  CAMERA_NUM,
} camera_index_t;

typedef enum {
  CAMERA_HDR_MODE_NORMAL = 0,
  CAMERA_HDR_MODE_DOL2 = 1,
  CAMERA_HDR_MODE_DOL3 = 2,
  CAMERA_HDR_MODE_STITCH3 = 3,
  CAMERA_HDR_MODE_STITCH2 = 4,
  CAMERA_HDR_MODE_STAGGER2 = 5,
  CAMERA_HDR_MODE_NUM,
} camera_hdr_mode_t;
typedef enum {
  ISP_PIPE_MASTER = 0,
  ISP_PIPE_AUX0 = 1,
  ISP_PIPE_AUX1 = 2,
  ISP_PIPE_AUX2 = 3,
  ISP_PIPE_NUM,
} isp_pipe_mode_t;

typedef enum {
  CAMERA_SESSION_VISIBLE = 0,
  CAMERA_SESSION_IR = 1,
  CAMERA_SESSION_WHITELIGHT = 2,
  CAMERA_SESSION_DUMMY0,
  CAMERA_SESSION_DUMMY1,
  CAMERA_SESSION_DUMMY2,
  CAMERA_SESSION_DUMMY3,
  CAMERA_SESSION_DUMMY4,
  CAMERA_SESSION_DUMMY5,
  CAMERA_SESSION_DUMMY6,
  CAMERA_SESSION_NUM,
  CAMERA_SESSION_INV = CAMERA_SESSION_NUM,
} camera_session_mode_t;

typedef struct
{
    load_tuning_func        load_tuning;
    const char8*            json;
} sensor_tuning_t;

typedef struct
{
    load_calibration_func   load_calibration;
    const char8*            json;
} sensor_calibration_t;

typedef struct {
  camera_session_mode_t session;
  const struct {
    load_actuator_drv_func actuator;
    load_sensor_drv_func sensor;
    load_csi2d_drv_func csid;
    sensor_tuning_t tune[CAMERA_HDR_MODE_NUM];
    sensor_calibration_t calib;
  } params[CAMERA_SESSION_NUM];
} camera_module_params_t;

camera_module_params_t *get_camera_modules_inst(void);
#endif
