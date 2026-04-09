#include "csi2d_common.h"


static csi2d_drv_params_t imx415_aux1_csi2d_params = {
  /* .cam_id = 1, */
  .type = CSI2D_HW_TYPE_536_414,
  .inits = {
    .enable= 0,
  },
  .params[0] = {
    .code = CSI2D_MBUS_FMT_SGBRG12_1x12,
    .u = {
      .s = {
        .fcfg_clk = 24.0f,
        .ipi[0] = {
          .active = 1,
          .vcid = 0,
          .embedded_data_en = 0,
          .hsd_time = 100,
          .line_event_selection = 1,
          .en_video = 1,
          .en_line_start = 0,
          .en_null = 0,
          .en_blanking = 0,
          .en_embedded = 0,
          .ipi_sync_event_mode = 0,
        },
      },
    },
  },
  .params[1] = {
    .code = CSI2D_MBUS_FMT_SGBRG12_1x12,
    .u = {
      .s = {
        .fcfg_clk = 24.0f,
        .ipi[0] = {
          .active = 1,
          .vcid = 0,
          .embedded_data_en = 0,
          .hsd_time = 100,
          .line_event_selection = 1,
          .en_video = 1,
          .en_line_start = 0,
          .en_null = 0,
          .en_blanking = 0,
          .en_embedded = 0,
          .ipi_sync_event_mode = 0,
        },
        .ipi[1] = {
          .active = 0,
        },
        .ipi[2] = {
          .active = 0,
        },
      },
    },
  },
  .params[2] = {
    /* .code = CSI2D_MBUS_FMT_SRGGB12_1x12, */
    /* .code = CSI2D_MBUS_FMT_SGRBG12_1x12, */
    .code = CSI2D_MBUS_FMT_SGBRG12_1x12,
    /* .code = CSI2D_MBUS_FMT_SBGGR12_1x12, */
    /* .code = CSI2D_MBUS_FMT_SGBRG10_1x10, */
    .u = {
      .s = {
        .fcfg_clk = 24.0f,
        .ipi[0] = {
          .active = 1,
          .vcid = 0,
          .embedded_data_en = 0,
          .hsd_time = 100,
          .line_event_selection = 1,
          .en_video = 1,
          .en_line_start = 0,
          .en_null = 0,
          .en_blanking = 0,
          .en_embedded = 0,
          .ipi_sync_event_mode = 0,
        },
        .ipi[1] = {
          .active = 0,
        },
        .ipi[2] = {
          .active = 0,
        },
      },
    },
  },
  .params[3] = {
    .code = CSI2D_MBUS_FMT_SGBRG12_1x12,
    .u = {
      .s = {
        .fcfg_clk = 24.0f,
        .ipi[0] = {
          .active = 1,
          .vcid = 0,
          .embedded_data_en = 0,
          .hsd_time = 100,
          .line_event_selection = 1,
          .en_video = 1,
          .en_line_start = 0,
          .en_null = 0,
          .en_blanking = 0,
          .en_embedded = 0,
          .ipi_sync_event_mode = 0,
        },
      },
    },
  },
  .params[4] = {
    .code = CSI2D_MBUS_FMT_SGBRG12_1x12,
    .u = {
      .s = {
        .fcfg_clk = 24.0f,
        .ipi[0] = {
          .active = 1,
          .vcid = 0,
          .embedded_data_en = 0,
          .hsd_time = 100,
          .line_event_selection = 1,
          .en_video = 1,
          .en_line_start = 0,
          .en_null = 0,
          .en_blanking = 0,
          .en_embedded = 0,
          .ipi_sync_event_mode = 0,
        },
      },
    },
  },
  .params[5] = {
    .code = CSI2D_MBUS_FMT_SGBRG12_1x12,
    .u = {
      .s = {
        .fcfg_clk = 24.0f,
        .ipi[0] = {
          .active = 1,
          .vcid = 0,
          .embedded_data_en = 0,
          .hsd_time = 100,
          .line_event_selection = 1,
          .en_video = 1,
          .en_line_start = 0,
          .en_null = 0,
          .en_blanking = 0,
          .en_embedded = 0,
          .ipi_sync_event_mode = 0,
        },
      },
    },
  },
  .params[6] = {
    .code = CSI2D_MBUS_FMT_SGBRG12_1x12,
    .u = {
      .s = {
        .fcfg_clk = 24.0f,
        .ipi[0] = {
          .active = 1,
          .vcid = 0,
          .embedded_data_en = 0,
          .hsd_time = 100,
          .line_event_selection = 1,
          .en_video = 1,
          .en_line_start = 0,
          .en_null = 0,
          .en_blanking = 0,
          .en_embedded = 0,
          .ipi_sync_event_mode = 0,
        },
      },
    },
  },
  .params[7] = {
    .code = CSI2D_MBUS_FMT_SGBRG12_1x12,
    .u = {
      .s = {
        .fcfg_clk = 24.0f,
        .ipi[0] = {
          .active = 1,
          .vcid = 0,
          .embedded_data_en = 0,
          .hsd_time = 100,
          .line_event_selection = 1,
          .en_video = 1,
          .en_line_start = 0,
          .en_null = 0,
          .en_blanking = 0,
          .en_embedded = 0,
          .ipi_sync_event_mode = 0,
        },
      },
    },
  },
    .params[8] = {
    .code = CSI2D_MBUS_FMT_SGBRG12_1x12,
    .u = {
      .s = {
        .fcfg_clk = 24.0f,
        .ipi[0] = {
          .active = 1,
          .vcid = 0,
          .embedded_data_en = 0,
          .hsd_time = 100,
          .line_event_selection = 1,
          .en_video = 1,
          .en_line_start = 0,
          .en_null = 0,
          .en_blanking = 0,
          .en_embedded = 0,
          .ipi_sync_event_mode = 0,
        },
      },
    },
  },
    .params[9] = {
    .code = CSI2D_MBUS_FMT_SGBRG12_1x12,
    .u = {
      .s = {
        .fcfg_clk = 24.0f,
        .ipi[0] = {
          .active = 1,
          .vcid = 0,
          .embedded_data_en = 0,
          .hsd_time = 100,
          .line_event_selection = 1,
          .en_video = 1,
          .en_line_start = 0,
          .en_null = 0,
          .en_blanking = 0,
          .en_embedded = 0,
          .ipi_sync_event_mode = 0,
        },
      },
    },
  },
    .params[10] = {
    .code = CSI2D_MBUS_FMT_SGBRG12_1x12,
    .u = {
      .s = {
        .fcfg_clk = 24.0f,
        .ipi[0] = {
          .active = 1,
          .vcid = 0,
          .embedded_data_en = 0,
          .hsd_time = 100,
          .line_event_selection = 1,
          .en_video = 1,
          .en_line_start = 0,
          .en_null = 0,
          .en_blanking = 0,
          .en_embedded = 0,
          .ipi_sync_event_mode = 0,
        },
      },
    },
  },
    .params[11] = {
    .code = CSI2D_MBUS_FMT_SGBRG12_1x12,
    .u = {
      .s = {
        .fcfg_clk = 24.0f,
        .ipi[0] = {
          .active = 1,
          .vcid = 0,
          .embedded_data_en = 0,
          .hsd_time = 100,
          .line_event_selection = 1,
          .en_video = 1,
          .en_line_start = 0,
          .en_null = 0,
          .en_blanking = 0,
          .en_embedded = 0,
          .ipi_sync_event_mode = 0,
        },
      },
    },
  },
    .params[12] = {
    .code = CSI2D_MBUS_FMT_SGBRG12_1x12,
    .u = {
      .s = {
        .fcfg_clk = 24.0f,
        .ipi[0] = {
          .active = 1,
          .vcid = 0,
          .embedded_data_en = 0,
          .hsd_time = 100,
          .line_event_selection = 1,
          .en_video = 1,
          .en_line_start = 0,
          .en_null = 0,
          .en_blanking = 0,
          .en_embedded = 0,
          .ipi_sync_event_mode = 0,
        },
      },
    },
  },
    .params[13] = {
    .code = CSI2D_MBUS_FMT_SGBRG12_1x12,
    .u = {
      .s = {
        .fcfg_clk = 24.0f,
        .ipi[0] = {
          .active = 1,
          .vcid = 0,
          .embedded_data_en = 0,
          .hsd_time = 100,
          .line_event_selection = 1,
          .en_video = 1,
          .en_line_start = 0,
          .en_null = 0,
          .en_blanking = 0,
          .en_embedded = 0,
          .ipi_sync_event_mode = 0,
        },
      },
    },
  },
  .params[14] = {
    .code = CSI2D_MBUS_FMT_SGBRG12_1x12,
    .u = {
      .s = {
        .fcfg_clk = 24.0f,
        .ipi[0] = {
          .active = 1,
          .vcid = 0,
          .embedded_data_en = 0,
          .hsd_time = 100,
          .line_event_selection = 1,
          .en_video = 1,
          .en_line_start = 0,
          .en_null = 0,
          .en_blanking = 0,
          .en_embedded = 0,
          .ipi_sync_event_mode = 0,
        },
        .ipi[1] = {
          .active = 0,
        },
        .ipi[2] = {
          .active = 0,
        },
      },
    },
  },
};	

