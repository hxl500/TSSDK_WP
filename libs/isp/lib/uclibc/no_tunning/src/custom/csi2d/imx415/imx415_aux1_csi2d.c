#if defined(TARGET_CHIP_TX536)
#include "imx415_aux1_csi2d_tx536.h"
#elif defined(TARGET_CHIP_TX5215) || defined(TARGET_CHIP_TX5239) || defined(TARGET_CHIP_TX5112)
#include "imx415_aux1_csi2d_tx5215.h"
#endif

csi2d_drv_params_t *
load_imx415_aux1_csi2d_driver(void)
{
  return &imx415_aux1_csi2d_params;
}
