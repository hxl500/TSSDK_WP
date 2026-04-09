#include "log_utils.h"
#include "tune_common.h"
#include "params_printer.h"
#include "json_to_params.h"

#include "../../sensor/sc2337p/sc2337p_sensor.h"

static tune_params_t g_tuningParams =
{
#ifndef LENS_2P8MM
  #include "tuning_sc2337p_linear_dual_aux.h"
#else
  #include "tuning_sc2337p_linear_dual_aux_2p8mm.h"
#endif
};

static BOOL g_isJsonLoaded = FALSE;

static void load_sc2337p_normal_tuning_json(const char8 *filename)
{
  if (ISP_RET_SUCCESS == Json_To_Params_From_File(filename, JSON_TUNING, (void*)&g_tuningParams))
    g_isJsonLoaded = TRUE;
}

tune_params_t *load_sc2337p_normal_tuning_aux(const char8 *filename)
{
  FW_FUNC_ENTER();

  if (!g_isJsonLoaded)
    load_sc2337p_normal_tuning_json(filename);

  FW_FUNC_EXIT();

  return &g_tuningParams;
}
