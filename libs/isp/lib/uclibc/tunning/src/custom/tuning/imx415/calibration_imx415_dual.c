#include "log_utils.h"
#include "calibration_common.h"
#include "params_printer.h"
#include "json_to_params.h"
#include "camera_module.h"

static calibration_params_t g_calibrationParams[] =
{
  {
  #include "calibration_imx415_dual.h"
  },
  {
  #include "calibration_imx415_dual_aux1.h"
  },
};

static BOOL g_isJsonLoaded[] = {TRUE, TRUE, TRUE, TRUE};

static void load_imx415_calibration_json(const char8 *filename, int index)
{
  if (ISP_RET_SUCCESS == Json_To_Params_From_File(filename, JSON_CALIBRATION, (void*)&g_calibrationParams[index]))
    g_isJsonLoaded[index] = TRUE;
}

static calibration_params_t *__load_imx415_calibration(const char8 *filename, int index)
{
  if (g_isJsonLoaded[index])
    load_imx415_calibration_json(filename, index);

  return &g_calibrationParams[index];
}

calibration_params_t *load_imx415_dual_calibration(const char8 *filename)
{
  FW_FUNC_ENTER();
  return __load_imx415_calibration(filename, CAMERA_IDX0);
  FW_FUNC_EXIT();
}

calibration_params_t *load_imx415_dual_aux1_calibration(const char8 *filename)
{
  FW_FUNC_ENTER();
  return __load_imx415_calibration(filename, CAMERA_IDX1);
  FW_FUNC_EXIT();
}
