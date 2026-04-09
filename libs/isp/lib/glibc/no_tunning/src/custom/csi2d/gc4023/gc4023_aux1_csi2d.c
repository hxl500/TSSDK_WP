#include "csi2d_common.h"
#include "log_utils.h"


static const reg_cfg_t csi2d_cr_setting[] = {
};

static const reg_cfg_t csi2d_csi2d_setting[] = {
};

static const reg_cfg_t csi2d_dphy_setting[] = {
};

static const reg_cfg_t csi2d_syscfg_setting[] = {
};

static csi2d_drv_params_t gc4023_aux1_csi2d_params = {
	/* .cam_id = 0, */
	.type = CSI2D_HW_TYPE_TX5215_21,
	.inits = {
		.enable = 0,
		.csi2d_setting = csi2d_csi2d_setting,
		.dphy_setting = csi2d_dphy_setting,
		.grf_setting = csi2d_syscfg_setting,
		.cr_setting = csi2d_cr_setting,
	},
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
		.code = CSI2D_MBUS_FMT_SRGGB10_1x10,
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

	.params[2] = {
		.code = CSI2D_MBUS_FMT_SRGGB10_1x10,
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
	.params[3] = {
		.code = CSI2D_MBUS_FMT_SRGGB10_1x10,
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
};

csi2d_drv_params_t *
load_gc4023_aux1_csi2d_driver(void)
{
	return &gc4023_aux1_csi2d_params;
}

