#include "log_utils.h"
#include "tune_common.h"
#include "params_printer.h"
#include "json_to_params.h"

static tune_params_t g_tuningParams =
{
  #include "tuning_gc2083_normal_slave.h"
};

static BOOL g_isJsonLoaded = FALSE;

static void load_gc2083_normal_tuning_json(const char8 *filename)
{
  if (ISP_RET_SUCCESS == Json_To_Params_From_File(filename, JSON_TUNING, (void*)&g_tuningParams))
    g_isJsonLoaded = TRUE;
}

tune_params_t *load_gc2083_tuning_linear_slave(const char8 *filename)
{
  FW_FUNC_ENTER();

  if (!g_isJsonLoaded)
    load_gc2083_normal_tuning_json(filename);

  FW_FUNC_EXIT();

  return &g_tuningParams;
}
