#include "supported_module.h"


static supported_camera_modules_t supported = {0};
static BOOL initialized = FALSE;

static inline void __supported_module_session_update(
		sensor_module_load_t *pipe,
		camera_module_params_t *camera,
		camera_session_mode_t session)
{
	pipe->load_actuator = camera->params[session].actuator;
	pipe->load_sensor = camera->params[session].sensor;
	pipe->load_csi2d = camera->params[session].csid;
	pipe->calibration = camera->params[session].calib;
	memcpy(pipe->tuning, camera->params[session].tune, sizeof(camera->params[session].tune));
}

static isp_ret_t supported_module_bind(isp_pipe_mode_t pid, u32 camid)
{
	isp_ret_t ret = ISP_RET_SUCCESS;
	sensor_module_load_t *pipe = NULL;
	camera_module_params_t *cameras = NULL;

	if (pid >= ISP_PIPE_NUM) ret = ISP_RET_INVALID;

	if (ISP_RET_SUCCESS == ret) {
		pipe = &supported.pSensorModuleData[pid];
		if (pipe->camId >= CAMERA_NUM || camid >= CAMERA_NUM) ret = ISP_RET_INVALID;
	}

	if (ISP_RET_SUCCESS == ret) {
		cameras = get_camera_modules_inst();

		pipe->camId = camid; 
		__supported_module_session_update(pipe, &cameras[camid], cameras[camid].session);
	}

	return ret;
}

static isp_ret_t
supported_module_set_session(isp_pipe_mode_t pid, camera_session_mode_t mode)
{
  isp_ret_t ret = ISP_RET_SUCCESS;
  sensor_module_load_t *pipe = NULL;
  camera_module_params_t *cameras = NULL;

  if (pid >= ISP_PIPE_NUM) ret = ISP_RET_INVALID;

  if (ISP_RET_SUCCESS == ret) {
    pipe = &supported.pSensorModuleData[pid];
    if (pipe->camId >= CAMERA_NUM || mode >= CAMERA_SESSION_NUM) ret = ISP_RET_INVALID;
  }

  if (ISP_RET_SUCCESS == ret) {
    cameras = get_camera_modules_inst();
    if (cameras[pipe->camId].session == mode) {
      ret = ISP_RET_INGORED_FAILURE;
      SENSOR_DBG("supported.set: pipe[%d].cam[%d].session[%d] ingore update", pid, pipe->camId, mode);
    }

    if (ISP_RET_SUCCESS == ret) {
      __supported_module_session_update(pipe, &cameras[pipe->camId], mode);
      cameras[pipe->camId].session = mode;

      SENSOR_CFG("supported.set: pipe[%d].cam[%d].session[%d].load{actuator=%p,sensor=%p,csid=%p,calib{%p,\"%s\"}}",
        pid, pipe->camId, mode,
        pipe->load_actuator, pipe->load_sensor, pipe->load_csi2d,
        pipe->calibration.load_calibration, pipe->calibration.json);
      SENSOR_CFG("supported.set: pipe[%d].cam[%d].session[%d].load{tune{nor{%p,\"%s\"},dol2{%p,\"%s\"},dol3{%p,\"%s\"}}}",
        pid, pipe->camId, mode,
        pipe->tuning[0].load_tuning, pipe->tuning[0].json,
        pipe->tuning[1].load_tuning, pipe->tuning[1].json,
        pipe->tuning[2].load_tuning, pipe->tuning[2].json);
      SENSOR_CFG("supported.set: pipe[%d].cam[%d].session[%d].load{tune{stitch3{%p,\"%s\"},stitch2{%p,\"%s\"}}}",
        pid, pipe->camId, mode,
        pipe->tuning[3].load_tuning, pipe->tuning[3].json,
        pipe->tuning[4].load_tuning, pipe->tuning[4].json);
    }
  }

  return ret;
}

static camera_session_mode_t
supported_module_get_session(isp_pipe_mode_t pid)
{
  isp_ret_t ret = ISP_RET_SUCCESS;
  sensor_module_load_t *pipe = NULL;
  camera_module_params_t *cameras = NULL;
  camera_session_mode_t session = CAMERA_SESSION_NUM;

  if (pid >= ISP_PIPE_NUM) ret = ISP_RET_INVALID;

  if (ISP_RET_SUCCESS == ret) {
    pipe = &supported.pSensorModuleData[pid];
    if (pipe->camId >= CAMERA_NUM) ret = ISP_RET_INVALID;
  }

  if (ISP_RET_SUCCESS == ret) {
    cameras = get_camera_modules_inst();
    session = cameras[pipe->camId].session;
    SENSOR_CFG("supported.get: pipe[%d].cam[%d]{session=%d}", pid, pipe->camId, session);
  }

  return session;
}

static supported_camera_module_ops_t
supported_ops = {
  .bind = NULL,
  .set = supported_module_set_session,
  .get = supported_module_get_session,
};

static void
__supported_shadow_printer(sensor_module_load_t *handle, u32 pid, u32 session) //all session
{
  actuator_drv_params_t *actuator = (handle->load_actuator) ? handle->load_actuator():NULL;
  sensor_drv_params_t *sensor = (handle->load_sensor) ? handle->load_sensor():NULL;
  csi2d_drv_params_t *csi2d = (handle->load_csi2d) ? handle->load_csi2d():NULL;
  calibration_params_t *calib = (handle->calibration.load_calibration) ? handle->calibration.load_calibration(NULL):NULL;
  tune_params_t *normal = (handle->tuning[0].load_tuning) ? handle->tuning[0].load_tuning(NULL):NULL;
  tune_params_t *dol2 = (handle->tuning[1].load_tuning) ? handle->tuning[1].load_tuning(NULL):NULL;
  tune_params_t *dol3 = (handle->tuning[2].load_tuning) ? handle->tuning[2].load_tuning(NULL):NULL;
  tune_params_t *stitch3 = (handle->tuning[3].load_tuning) ? handle->tuning[3].load_tuning(NULL):NULL;
  tune_params_t *stitch2 = (handle->tuning[4].load_tuning) ? handle->tuning[4].load_tuning(NULL):NULL;

  SENSOR_CFG(
    "supported.initial: pipe[%d].cam[%d].session[%d].load{actuator=%p,sensor=%p,csid=%p,calib{%p,\"%s\"}}",
    pid, handle->camId, session,
    handle->load_actuator, handle->load_sensor, handle->load_csi2d,
    handle->calibration.load_calibration, handle->calibration.json);
  SENSOR_CFG(
    "supported.initial: pipe[%d].cam[%d].session[%d].load{tune{nor{%p,\"%s\"},dol2{%p,\"%s\"},dol2{%p,\"%s\"}}}",
    pid, handle->camId, session,
    handle->tuning[0].load_tuning, handle->tuning[0].json,
    handle->tuning[1].load_tuning, handle->tuning[1].json,
    handle->tuning[2].load_tuning, handle->tuning[2].json);
  SENSOR_CFG(
    "supported.initial: pipe[%d].cam[%d].session[%d].load{tune{stitch3{%p,\"%s\"},stitch2{%p,\"%s\"}}}",
    pid, handle->camId, session,
    handle->tuning[3].load_tuning, handle->tuning[3].json,
    handle->tuning[4].load_tuning, handle->tuning[4].json);

  SENSOR_CFG(
    "supported.initial: pipe[%d].cam[%d].session[%d].data{actuator=%p,sensor=%p,csid=%p,calib=%p}",
    pid, handle->camId, session, actuator, sensor, csi2d, calib);
  SENSOR_CFG(
    "supported.initial: pipe[%d].cam[%d].session[%d].data{tune{nor=%p,dol2=%p,dol2=%p}}",
    pid, handle->camId, session, normal, dol2, dol3);
  SENSOR_CFG(
    "supported.initial: pipe[%d].cam[%d].session[%d].data{tune{stitch3=%p,stitch2=%p}}",
    pid, handle->camId, session, stitch3, stitch2);
}


static supported_camera_modules_t *
supported_modules_get_inst(void)
{
  static sensor_module_load_t shadow[CAMERA_NUM] = {0};

  isp_ret_t ret = ISP_RET_SUCCESS;
  sensor_module_load_t *handle = NULL;
  camera_module_params_t *cameras = NULL;
  camera_session_mode_t session = CAMERA_SESSION_NUM;
  u32 pnum = 0;

  if (FALSE == initialized) {
    cameras = get_camera_modules_inst();
    if (NULL == cameras) {
        SENSOR_ERR("");
        ret = ISP_RET_INVALID;
    }
    if (ISP_RET_SUCCESS == ret) {
      for (u32 i = CAMERA_IDX0; i < CAMERA_NUM; ++i) {
        session = cameras[i].session;
        if (session >= CAMERA_SESSION_NUM) continue;

        handle = &shadow[pnum++];
        handle->camId = i;
        __supported_module_session_update(handle, &cameras[i], session);

        /* for (u32 j = CAMERA_SESSION_VISIBLE; j < CAMERA_SESSION_NUM; ++j) { */
        /*   __supported_shadow_printer(handle, pnum - 1, j); */
        /* } */
      }

      if (pnum > 0) {
        supported.pSensorModuleData = shadow;
        supported.numOfModules = pnum;
        supported.ops = &supported_ops;
      }

      initialized = TRUE;
      SENSOR_CFG("supported.initial: camera{supported=%p,actnum=%d}}", shadow, pnum);
    }
  }
  SENSOR_CFG("supported.initial: camera{supported=%p,actnum=%d}}", shadow, pnum);

  return (TRUE == initialized) ? &supported:NULL;
}

supported_camera_modules_t *
Get_Supported_Camera_Modules(void)
{
  return supported_modules_get_inst();
}
