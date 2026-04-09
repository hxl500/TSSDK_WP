#include "log_utils.h"
#include "tune_common.h"
#include "params_printer.h"
#include "json_to_params.h"
#include "camera_module.h"

static tune_params_t g_tuningParams[] =
{
  {
  #include "tuning_imx415_dual_linear.h"
  },
  {
  #include "tuning_imx415_dual_linear_aux1.h"
  },
};

static BOOL g_isJsonLoaded[] = {TRUE, TRUE, TRUE, TRUE};

static void load_imx415_normal_tuning_json(const char8 *filename, int index)
{
  if (ISP_RET_SUCCESS == Json_To_Params_From_File(filename, JSON_TUNING, (void*)&g_tuningParams[index]))
    g_isJsonLoaded[index] = TRUE;
}

static tune_params_t *__load_imx415_normal_tuning(const char8 *filename, int index)
{
  if (g_isJsonLoaded[index])
    load_imx415_normal_tuning_json(filename, index);

  return &g_tuningParams[index];
}

tune_params_t *load_imx415_dual_linear_tuning(const char8 *filename)
{
  FW_FUNC_ENTER();
  return __load_imx415_normal_tuning(filename, CAMERA_IDX0);
  FW_FUNC_EXIT();
}

tune_params_t *load_imx415_dual_linear_aux1_tuning(const char8 *filename)
{
  FW_FUNC_ENTER();
  return __load_imx415_normal_tuning(filename, CAMERA_IDX1);
  FW_FUNC_EXIT();
}
