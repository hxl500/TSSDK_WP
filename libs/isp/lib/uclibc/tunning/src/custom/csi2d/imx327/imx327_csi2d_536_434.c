#include "imx327_csi2d_536_434.h"


#define CSI2D_DEVS_NUM_536 (3)

#define CSI2D_BASEADDR_536_434 0x83BC0000
#define CSI2D_BASEADDR_536_412 0x83BC1000
#define CSI2D_BASEADDR_536_212 0x83BC2000

#define GRF_BASEADDR_536 0x83BD000

static const reg_cfg_t csi2d_536_434_cr_setting[] = {
	{ 0x0, 0x0, REG_SOF_DELAY_INFINATE }
};

//front
static const reg_cfg_t csi2d_536_434_csi2d_setting[] = {
  /* { CSI2D_BASEADDR_536_434 + 0x08, 0x00000000, 0 }, */
  /* { CSI2D_BASEADDR_536_434 + 0x08, 0x00000001, 0 }, */

  { CSI2D_BASEADDR_536_434 + 0x04, 0x00000001, 0 },
  { CSI2D_BASEADDR_536_434 + 0xa0, 0x00000000, 0 },
  { CSI2D_BASEADDR_536_434 + 0xa0, 0x00000111, 0 },

  { CSI2D_BASEADDR_536_434 + 0x80, 0x01010100, 0 },
  { CSI2D_BASEADDR_536_434 + 0x84, 0x00000000, 0 },
  { CSI2D_BASEADDR_536_434 + 0x88, 0x0000002c, 0 },
  { CSI2D_BASEADDR_536_434 + 0x8c, 0x00000100, 0 },
  { CSI2D_BASEADDR_536_434 + 0x98, 0x00000064, 0 },
  { CSI2D_BASEADDR_536_434 + 0xac, 0x00000000, 0 },

  { CSI2D_BASEADDR_536_434 + 0x300, 0x00000000, 0 },

  { CSI2D_BASEADDR_536_434 + 0x44, 0x00000000, 0 },
  { CSI2D_BASEADDR_536_434 + 0x40, 0x00000000, 0 },
  { CSI2D_BASEADDR_536_434 + 0x50, 0x00000001, 0 },
  { CSI2D_BASEADDR_536_434 + 0x50, 0x00000000, 0 },

	{ 0x0, 0x0, REG_SOF_DELAY_INFINATE }
};

static const reg_cfg_t csi2d_536_434_dphy_setting[] = {
  { 0x0001, 0xbb, 0 },
  { 0x0002, 0x16, 0 },
  
  { 0x00e5, 0x01, 0 },

  { 0x00e2, 0xb6, 0 },
  { 0x00e3, 0x01, 0 },
  { 0x00e4, 0x01, 0 },

  { 0x060a, 0xb6, 0 },
  { 0x060b, 0x01, 0 },
  { 0x060c, 0x01, 0 },

  { 0x0008, 0x26, 0 },

  { 0x00e4, 0x31, 0 },

	{ 0x0, 0x0, REG_SOF_DELAY_INFINATE }
};

//back
static const reg_cfg_t csi2d_536_434_syscfg_setting[] = {
  { CSI2D_BASEADDR_536_434 + 0x40, 0x00000001, 0 },
  { CSI2D_BASEADDR_536_434 + 0x44, 0x00000001, 0 },

	{0x0, 0x0, REG_SOF_DELAY_INFINATE}
};

static csi2d_drv_params_t imx327_csi2d_params_536_434 = {
  /* .cam_id = 0, */
  .type = CSI2D_HW_TYPE_536_434,
  .inits = {
    .enable= 0,
    .csi2d_setting = csi2d_536_434_csi2d_setting,
    .dphy_setting = csi2d_536_434_dphy_setting,
    .grf_setting = csi2d_536_434_syscfg_setting,
    .cr_setting = csi2d_536_434_cr_setting,
  },
  .params[0] = {
    .code = CSI2D_MBUS_FMT_SRGGB12_1x12,
    .u = {
      .s = {
        .fcfg_clk = 37.0f,
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
          .ipi_sync_event_mode = 0,	//auto
        },
      },
    },
  },
  .params[5] = {
    .code = CSI2D_MBUS_FMT_SRGGB12_1x12,
    .u = {
      .s = {
        .fcfg_clk = 37.0f,
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

csi2d_drv_params_t *load_imx327_csi2d_driver_536_434(void)
{
  return &imx327_csi2d_params_536_434;
}

