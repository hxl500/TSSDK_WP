#include "log_utils.h"
#include "calibration_common.h"
#include "params_printer.h"
#include "json_to_params.h"

static calibration_params_t g_calParams =
{
    #include "calibration_sc2337p_dual.h"
};

static BOOL g_isJsonLoaded = FALSE;

static void load_sc2337p_calibration_json(const char *filename)
{
  if (ISP_RET_SUCCESS == Json_To_Params_From_File(filename, JSON_CALIBRATION, (void*)&g_calParams))
    g_isJsonLoaded = TRUE;
}

calibration_params_t *load_sc2337p_calibration(const char *filename)
{
  FW_FUNC_ENTER();

  if (!g_isJsonLoaded)
    load_sc2337p_calibration_json(filename);

  FW_FUNC_EXIT();

  return &g_calParams;
}
