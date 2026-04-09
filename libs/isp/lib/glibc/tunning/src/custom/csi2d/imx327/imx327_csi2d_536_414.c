#include "imx327_csi2d_536_414.h"


static csi2d_drv_params_t imx327_csi2d_params_536_414 = {
  /* .cam_id = 1, */
  .type = CSI2D_HW_TYPE_536_414,
  .inits = { 0 },
  .params[0] = {
    .code = CSI2D_MBUS_FMT_SRGGB10_1x10,
    .u = {
      .s = {
        .fcfg_clk = 24.0f,
        .ipi[0] = {
          .active = 1,
          .vcid = 0,
          .embedded_data_en = 0,
          .hsd_time = 100,
          .line_event_selection = 0,
          .en_video = 0,
          .en_line_start = 0,
          .en_null = 0,
          .en_blanking = 0,
          .en_embedded = 0,
          .ipi_sync_event_mode = 0,
        },
      },
    },
  },
};

csi2d_drv_params_t *load_imx327_csi2d_driver_536_414(void)
{
  return &imx327_csi2d_params_536_414;
}
