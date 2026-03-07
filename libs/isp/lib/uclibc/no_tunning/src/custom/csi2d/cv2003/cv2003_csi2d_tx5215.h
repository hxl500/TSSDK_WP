#include "csi2d_common.h"
#include "log_utils.h"


static const reg_cfg_t csi2d_434_cr_setting[] = {
};

static const reg_cfg_t csi2d_434_csi2d_setting[] = {
};

static const reg_cfg_t csi2d_434_dphy_setting[] = {
};

static const reg_cfg_t csi2d_434_syscfg_setting[] = {
};

static csi2d_drv_params_t cv2003_csi2d_params = {
	/* .cam_id = 0, */
	.type = CSI2D_HW_TYPE_TX5215_42,
	.inits = {
		.enable= 0,
		.csi2d_setting = csi2d_434_csi2d_setting,
		.dphy_setting = csi2d_434_dphy_setting,
		.grf_setting = csi2d_434_syscfg_setting,
		.cr_setting = csi2d_434_cr_setting,
	},
	.params[0] = {
		.code = CSI2D_MBUS_FMT_SRGGB10_1x10,
		.u = {
			.s = {
				.fcfg_clk = 30.0f,
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
				.fcfg_clk = 30.0f,
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
				.fcfg_clk = 30.0f,
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
				.fcfg_clk = 30.0f,
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


