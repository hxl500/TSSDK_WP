#include "log_utils.h"
#include "calibration_common.h"
#include "params_printer.h"
#include "json_to_params.h"

static calibration_params_t g_calParams =
{
    #include "calibration_imx415_ir.h"
};

static BOOL g_isJsonLoaded = TRUE;

static void load_imx415_calibration_json(const char *filename)
{
  if (ISP_RET_SUCCESS == Json_To_Params_From_File(filename, JSON_CALIBRATION, (void*)&g_calParams))
    g_isJsonLoaded = TRUE;
}

calibration_params_t *load_imx415_ir_calibration(const char *filename)
{
  FW_FUNC_ENTER();

  if (!g_isJsonLoaded)
    load_imx415_calibration_json(filename);

  FW_FUNC_EXIT();

  return &g_calParams;
}
