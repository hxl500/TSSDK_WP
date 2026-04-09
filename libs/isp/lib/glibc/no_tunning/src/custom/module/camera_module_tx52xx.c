#include "camera_module.h"

extern sensor_drv_params_t *load_imx415_sensor_driver(void);
extern csi2d_drv_params_t *load_imx415_csi2d_driver_434(void);

extern sensor_drv_params_t *load_imx415_aux1_sensor_driver(void);
extern csi2d_drv_params_t *load_imx415_aux1_csi2d_driver(void);

extern sensor_drv_params_t *load_imx415_aux2_sensor_driver(void);
extern csi2d_drv_params_t *load_imx415_aux2_csi2d_driver(void);

extern calibration_params_t *load_imx415_calibration(const char8 *);
extern tune_params_t *load_imx415_normal_tuning(const char8 *);
extern tune_params_t* load_imx415_dol2_tuning(const char8 *);

extern calibration_params_t *load_imx415_ir_calibration(const char8 *);
extern tune_params_t *load_imx415_normal_ir_tuning(const char8 *);
extern tune_params_t* load_imx415_dol2_ir_tuning(const char8 *);

extern calibration_params_t *load_imx415_dual_calibration(const char8 *);
extern calibration_params_t *load_imx415_dual_ir_calibration(const char8 *);
extern tune_params_t *load_imx415_dual_linear_tuning(const char8 *);
extern tune_params_t* load_imx415_dual_linear_ir_tuning(const char8 *);

extern calibration_params_t *load_imx415_dual_aux1_calibration(const char8 *);
extern calibration_params_t *load_imx415_dual_aux1_ir_calibration(const char8 *);
extern tune_params_t *load_imx415_dual_linear_aux1_tuning(const char8 *);
extern tune_params_t* load_imx415_dual_linear_aux1_ir_tuning(const char8 *);

//gc5603
extern csi2d_drv_params_t *load_gc5603_csi2d_driver(void);
extern calibration_params_t *load_gc5603_calibration(const char8 *);
extern tune_params_t *load_gc5603_normal_tuning(const char8 *);
extern calibration_params_t *load_gc5603_ir_calibration(const char8 *);
extern tune_params_t *load_gc5603_ir_tuning(const char8 *);

extern sensor_drv_params_t *load_gc4023_sensor_driver(void);
extern sensor_drv_params_t* load_gc4023_aux1_sensor_driver(void);
extern csi2d_drv_params_t *load_gc4023_csi2d_driver(void);
extern csi2d_drv_params_t *load_gc4023_aux1_csi2d_driver(void);
//gc4023
extern tune_params_t *load_gc4023_normal_tuning(const char8 *);
extern tune_params_t *load_gc4023_normal_ir_tuning(const char8 *);

extern tune_params_t *load_gc4023_tuning_linear_slave(const char8 *);
extern tune_params_t *load_gc4023_tuning_linear_master(const char8 *);
extern tune_params_t *load_gc4023_tuning_linear_dual_ir(const char8 *);

extern calibration_params_t *load_gc4023_calibration(const char8 *);
extern calibration_params_t *load_gc4023_calibration_ir(const char8 *);
extern calibration_params_t *load_gc4023_calibration_dual(const char8 *);
extern calibration_params_t *load_gc4023_calibration_dual_ir(const char8 *);
extern actuator_drv_params_t * load_normal_actuator_driver(void);
extern actuator_drv_params_t * load_normal_actuator_aux1_driver(void);

extern sensor_drv_params_t *load_sc200ai_sensor_driver(void);
extern sensor_drv_params_t* load_sc200ai_aux1_sensor_driver(void);
extern csi2d_drv_params_t *load_sc200ai_csi2d_driver(void);
extern csi2d_drv_params_t *load_sc200ai_aux1_csi2d_driver(void);

extern tune_params_t *load_sc200ai_normal_tuning(const char8 *);
extern calibration_params_t *load_sc200ai_calibration(const char8 *);
extern calibration_params_t *load_sc200ai_ir_calibration(const char8 *);
extern tune_params_t *load_sc200ai_normal_ir_tuning(const char8 *);

extern tune_params_t *load_gc2083_normal_tuning(const char8 *);
extern calibration_params_t *load_gc2083_calibration(const char8 *);
extern calibration_params_t *load_gc2083_ir_calibration(const char8 *);
extern tune_params_t *load_gc2083_normal_ir_tuning(const char8 *);
extern sensor_drv_params_t *load_gc2083_sensor_driver(void);
extern sensor_drv_params_t* load_gc2083_aux1_sensor_driver(void);
extern csi2d_drv_params_t *load_gc2083_csi2d_driver(void);
extern csi2d_drv_params_t *load_gc2083_aux1_csi2d_driver(void);
extern tune_params_t *load_gc2083_tuning_linear_slave(const char8 *);
extern tune_params_t *load_gc2083_normal_ir_slave_tuning(const char8 *);


extern sensor_drv_params_t *load_k17_sensor_driver(void);
extern csi2d_drv_params_t *load_k17_csi2d_driver(void);
extern tune_params_t *load_k17_normal_tuning(const char8 *);
extern tune_params_t *load_k17_dol2_tuning(const char8 *);
extern calibration_params_t *load_k17_calibration(const char8 *);
extern calibration_params_t *load_k17_ir_calibration(const char8 *);
extern tune_params_t *load_k17_normal_ir_tuning(const char8 *);

extern sensor_drv_params_t *load_sc501ai_sensor_driver(void);
extern csi2d_drv_params_t *load_sc501ai_csi2d_driver(void);
extern tune_params_t *load_sc501ai_normal_tuning(const char8 *);
extern calibration_params_t *load_sc501ai_calibration(const char8 *);
extern calibration_params_t *load_sc501ai_ir_calibration(const char8 *);
extern tune_params_t *load_sc501ai_normal_ir_tuning(const char8 *);

extern sensor_drv_params_t *load_sc430ai_sensor_driver(void);
extern csi2d_drv_params_t *load_sc430ai_csi2d_driver(void);
extern tune_params_t *load_sc430ai_normal_tuning(const char8 *);
extern calibration_params_t *load_sc430ai_calibration(const char8 *);
extern calibration_params_t *load_sc430ai_ir_calibration(const char8 *);
extern tune_params_t *load_sc430ai_normal_ir_tuning(const char8 *);

extern sensor_drv_params_t *load_hi556_sensor_driver(void);
extern sensor_drv_params_t* load_hi556_aux1_sensor_driver(void);
extern csi2d_drv_params_t *load_hi556_csi2d_driver(void);
extern csi2d_drv_params_t *load_hi556_aux1_csi2d_driver(void);
extern tune_params_t *load_hi556_normal_tuning(const char8 *);
extern calibration_params_t *load_hi556_calibration(const char8 *);
extern calibration_params_t *load_hi556_ir_calibration(const char8 *);
extern tune_params_t *load_hi556_normal_ir_tuning(const char8 *);

//cv2003
extern sensor_drv_params_t *load_cv2003_sensor_driver(void);
// extern csi2d_drv_params_t *load_cv2003_csi2d_driver_434(void);
extern csi2d_drv_params_t *load_cv2003_csi2d_driver(void);
extern sensor_drv_params_t *load_cv2003_aux1_sensor_driver(void);
extern csi2d_drv_params_t *load_cv2003_aux1_csi2d_driver(void);

// extern sensor_drv_params_t *load_cv2003_aux2_sensor_driver(void);
// extern csi2d_drv_params_t *load_cv2003_aux2_csi2d_driver(void);
extern calibration_params_t *load_cv2003_calibration(const char8 *);
// extern tune_params_t *load_cv2003_normal_tuning(const char8 *);
// extern tune_params_t* load_cv2003_dol2_tuning(const char8 *);
extern calibration_params_t *load_cv2003_ir_calibration(const char8 *);
extern tune_params_t *load_cv2003_normal_ir_tuning(const char8 *);
// extern tune_params_t* load_cv2003_dol2_ir_tuning(const char8 *);
// extern calibration_params_t *load_cv2003_dual_calibration(const char8 *);
extern calibration_params_t *load_cv2003_calibration(const char8 *);
// extern calibration_params_t *load_cv2003_dual_ir_calibration(const char8 *);
// extern tune_params_t *load_cv2003_dual_linear_tuning(const char8 *);
extern tune_params_t *load_cv2003_normal_tuning(const char8 *filename);
// extern tune_params_t* load_cv2003_dual_linear_ir_tuning(const char8 *);
// extern calibration_params_t *load_cv2003_dual_aux1_calibration(const char8 *);
// extern calibration_params_t *load_cv2003_dual_aux1_ir_calibration(const char8 *);
// extern tune_params_t *load_cv2003_dual_linear_aux1_tuning(const char8 *);
// extern tune_params_t* load_cv2003_dual_linear_aux1_ir_tuning(const char8 *);

dim_t supportedImageDim [] =
{
    {3840, 2160},
    {2688, 1860},
	{2688, 1520},
    {2688, 1512},
    {2592, 1944},
    {2592, 1536},
    {2560, 1440},
    {2304, 1296},
    {1920, 1080},
    {1600, 900},
    {1280, 720},
    {800,  600},
    {640,  480},
};

u32 supportedImageDimNum = sizeof (supportedImageDim ) / sizeof(supportedImageDim [0]);

#if defined(TARGET_BOARD_EVB132)
static camera_module_params_t camera_module[CAMERA_NUM] = {
	[CAMERA_IDX0] = {
		.session = CAMERA_SESSION_VISIBLE,

		.params = {
			[CAMERA_SESSION_VISIBLE] = {
				.actuator = load_normal_actuator_driver,
				.sensor = load_imx415_sensor_driver,
				.csid = load_imx415_csi2d_driver_434,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_imx415_dual_linear_tuning, "/root/cam0.sen0.tun"},
					[CAMERA_HDR_MODE_DOL2] = {load_imx415_dol2_tuning, STORAGE_ROOT_DIR"cam0.sen0.hdr1.tun"},
					[CAMERA_HDR_MODE_STAGGER2] = {load_imx415_dol2_tuning, STORAGE_ROOT_DIR"cam0.sen0.hdr1.tun"},
				},
				.calib = {load_imx415_dual_calibration, "/root/cam0.sen0.cal"},
			},

			[CAMERA_SESSION_IR] = {
				.actuator = load_normal_actuator_driver,
				.sensor = load_imx415_sensor_driver,
				.csid = load_imx415_csi2d_driver_434,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_imx415_dual_linear_ir_tuning, "/root/cam0.sen1.tun"},
					[CAMERA_HDR_MODE_DOL2] = {load_imx415_dol2_ir_tuning, STORAGE_ROOT_DIR"cam0.sen1.hdr1.tun"},
					[CAMERA_HDR_MODE_STAGGER2] = {load_imx415_dol2_tuning, STORAGE_ROOT_DIR"cam0.sen0.hdr1.tun"},
				},
				.calib = {load_imx415_dual_ir_calibration, "/root/cam0.sen1.cal"},
			},
		},
	},

	[CAMERA_IDX1] = {
		.session = CAMERA_SESSION_VISIBLE,

		.params = {
			[CAMERA_SESSION_VISIBLE] = {
				.actuator = load_normal_actuator_aux1_driver,
				.sensor = load_imx415_aux1_sensor_driver,
				.csid = load_imx415_aux1_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_imx415_dual_linear_aux1_tuning, "/root/cam1.sen0.tun"},
				},
				.calib = {load_imx415_dual_aux1_calibration, "/root/cam1.sen0.cal"},
			},

			[CAMERA_SESSION_IR] = {
				.actuator = load_normal_actuator_aux1_driver,
				.sensor = load_imx415_aux1_sensor_driver,
				.csid = load_imx415_aux1_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_imx415_dual_linear_ir_tuning, "/root/cam1.sen1.tun"},
				},
				.calib = {load_imx415_dual_ir_calibration, "/root/cam1.sen1.cal"},
			},
		},
	},
	[CAMERA_IDX2] = { .session = CAMERA_SESSION_INV, },
	[CAMERA_IDX3] = { .session = CAMERA_SESSION_INV, },
};
#elif defined(TARGET_BOARD_AOV)
static camera_module_params_t camera_module[CAMERA_NUM] = {
	[CAMERA_IDX0] = {
		.session = CAMERA_SESSION_VISIBLE,

		.params = {
			[CAMERA_SESSION_VISIBLE] = {
				.actuator = load_normal_actuator_driver,
				.sensor = load_cv2003_sensor_driver,
				.csid = load_cv2003_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_cv2003_normal_tuning, STORAGE_ROOT_DIR"cam0.sen0.hdr0.tun"},
					//[CAMERA_HDR_MODE_DOL2] = {load_cv2003_dol2_tuning, STORAGE_ROOT_DIR"cam0.sen0.hdr1.tun"},
					// [CAMERA_HDR_MODE_NORMAL] = {load_cv2003_tuning_linear_slave, "cv2003_nor.tun"},
				},
				.calib = {load_cv2003_calibration, "cv2003.cal"},
			},

			[CAMERA_SESSION_IR] = {
				.actuator = load_normal_actuator_driver,
				.sensor = load_cv2003_sensor_driver,
				.csid = load_cv2003_csi2d_driver,
				.tune = {
					// [CAMERA_HDR_MODE_NORMAL] = {load_cv2003_normal_ir_slave_tuning, "cv2003_nor_ir.tun"},
				},
				.calib = {load_cv2003_ir_calibration, "cv2003.cal"},
			},
		},
	},

	[CAMERA_IDX1] = {
		.session = CAMERA_SESSION_VISIBLE,

		.params = {
			[CAMERA_SESSION_VISIBLE] = {
				.actuator = load_normal_actuator_aux1_driver,
				.sensor = load_cv2003_aux1_sensor_driver,
				.csid = load_cv2003_aux1_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_cv2003_normal_tuning, STORAGE_ROOT_DIR"cam1.sen0.hdr0.tun"},
				//	[CAMERA_HDR_MODE_NORMAL] = {load_cv2003_dual_linear_aux1_tuning, STORAGE_ROOT_DIR"cam1.sen0.hdr0.tun"},
				},
				.calib = {load_cv2003_calibration, STORAGE_ROOT_DIR"cam1.sen0.cal"},
				//.calib = {load_cv2003_dual_aux1_calibration, STORAGE_ROOT_DIR"cam1.sen0.cal"},
			},

			[CAMERA_SESSION_IR] = {
				.actuator = load_normal_actuator_aux1_driver,
				.sensor = load_cv2003_aux1_sensor_driver,
				.csid = load_cv2003_aux1_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_cv2003_normal_ir_tuning, STORAGE_ROOT_DIR"cam1.sen1.hdr0.tun"},
				},
				.calib = {load_cv2003_ir_calibration, "cv2003_ir.cal"},
			},
		},
	},

	[CAMERA_IDX2] = { .session = CAMERA_SESSION_INV, },
	[CAMERA_IDX3] = { .session = CAMERA_SESSION_INV, },
};
#elif defined(gc2083)
static camera_module_params_t camera_module[CAMERA_NUM] = {
	[CAMERA_IDX0] = {
		.session = CAMERA_SESSION_VISIBLE,

		.params = {
			[CAMERA_SESSION_VISIBLE] = {
				.actuator = load_normal_actuator_driver,
				.sensor = load_gc2083_sensor_driver,
				.csid = load_gc2083_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_gc2083_normal_tuning, "gc2083_nor.tun"},
				},
				.calib = {load_gc2083_calibration, "gc2083.cal"},
			},

			[CAMERA_SESSION_IR] = {
				.actuator = load_normal_actuator_driver,
				.sensor = load_gc2083_sensor_driver,
				.csid = load_gc2083_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_gc2083_normal_ir_tuning, "gc2083_nor_ir.tun"},
				},
				.calib = {load_gc2083_ir_calibration, "gc2083.cal"},
			},
		},
	},

	[CAMERA_IDX1] = {
		.session = CAMERA_SESSION_VISIBLE,

		.params = {
			[CAMERA_SESSION_VISIBLE] = {
				.actuator = load_normal_actuator_aux1_driver,
				.sensor = load_gc2083_aux1_sensor_driver,
				.csid = load_gc2083_aux1_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_gc2083_tuning_linear_slave, "gc2083_nor.tun"},
				},
				.calib = {load_gc2083_calibration, "gc2083_ir.cal"},
			},

			[CAMERA_SESSION_IR] = {
				.actuator = load_normal_actuator_aux1_driver,
				.sensor = load_gc2083_aux1_sensor_driver,
				.csid = load_gc2083_aux1_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_gc2083_normal_ir_slave_tuning, "gc2083_nor_ir.tun"},
				},
				.calib = {load_gc2083_ir_calibration, "gc2083_ir.cal"},
			},
		},
	},
	[CAMERA_IDX2] = { .session = CAMERA_SESSION_INV, },
	[CAMERA_IDX3] = { .session = CAMERA_SESSION_INV, },
};
#elif defined(K17)
static camera_module_params_t camera_module[CAMERA_NUM] = {
  [CAMERA_IDX0] = {
    .session = CAMERA_SESSION_VISIBLE,

    .params = {
      [CAMERA_SESSION_VISIBLE] = {
        .actuator = load_normal_actuator_driver,
        .sensor = load_k17_sensor_driver,
        .csid = load_k17_csi2d_driver,
        .tune = {
          [CAMERA_HDR_MODE_NORMAL] = {load_k17_normal_tuning, "k17.tun"},
          [CAMERA_HDR_MODE_DOL2] = {load_k17_dol2_tuning, "k17_dol2.tun"},
        },
        .calib = {load_k17_calibration, "k17.cal"},
      },

      [CAMERA_SESSION_IR] = {
        .actuator = load_normal_actuator_driver,
        .sensor = load_k17_sensor_driver,
        .csid = load_k17_csi2d_driver,
        .tune = {
          [CAMERA_HDR_MODE_NORMAL] = {load_k17_normal_ir_tuning, "k17_nor_ir.tun"},
        },
        .calib = {load_k17_ir_calibration, "k17_ir.cal"},
      },
    },
  },
  [CAMERA_IDX1] = { .session = CAMERA_SESSION_INV, },
  [CAMERA_IDX2] = { .session = CAMERA_SESSION_INV, },
  [CAMERA_IDX3] = { .session = CAMERA_SESSION_INV, },
};
#elif defined(GC4653)
extern sensor_drv_params_t *load_gc4653_sensor_driver(void);
extern csi2d_drv_params_t *load_gc4653_csi2d_driver(void);

extern tune_params_t *load_gc4653_normal_tuning(const char8 *);
extern calibration_params_t *load_gc4653_calibration(const char8 *);
extern calibration_params_t *load_gc4653_ir_calibration(const char8 *);
extern tune_params_t *load_gc4653_normal_ir_tuning(const char8 *);

static camera_module_params_t camera_module[CAMERA_NUM] = {
            [CAMERA_IDX0] = {
              .session = CAMERA_SESSION_VISIBLE,

              .params = {
                [CAMERA_SESSION_VISIBLE] = {
                  .actuator = load_normal_actuator_driver,
                  .sensor = load_gc4653_sensor_driver,
                  .csid = load_gc4653_csi2d_driver,
                  .tune = {
                    [CAMERA_HDR_MODE_NORMAL] = {load_gc4653_normal_tuning, "gc4653.tun"},
                  },
                  .calib = {load_gc4653_calibration, "gc4653.cal"},
                },

                [CAMERA_SESSION_IR] = {
                  .actuator = load_normal_actuator_driver,
                  .sensor = load_gc4653_sensor_driver,
                  .csid = load_gc4653_csi2d_driver,
                  .tune = {
                    [CAMERA_HDR_MODE_NORMAL] = {load_gc4653_normal_ir_tuning, "gc4653_nor_ir.tun"},
                  },
                  .calib = {load_gc4653_ir_calibration, "gc4653_ir.cal"},
                },
              },
            },
            [CAMERA_IDX1] = { .session = CAMERA_SESSION_INV, },
            [CAMERA_IDX2] = { .session = CAMERA_SESSION_INV, },
            [CAMERA_IDX3] = { .session = CAMERA_SESSION_INV, },
          };
#elif(defined(CV4002))
extern sensor_drv_params_t *load_cv4002_sensor_driver(void);
extern csi2d_drv_params_t *load_cv4002_csi2d_driver(void);
extern tune_params_t *load_cv4002_normal_tuning(const char8 *);
extern calibration_params_t *load_cv4002_calibration(const char8 *);
extern calibration_params_t *load_cv4002_ir_calibration(const char8 *);
extern tune_params_t *load_cv4002_normal_ir_tuning(const char8 *);
extern calibration_params_t *load_cv4002_wl_calibration(const char8 *);
extern tune_params_t *load_cv4002_normal_wl_tuning(const char8 *);
extern actuator_drv_params_t * load_normal_actuator_driver(void);
          static camera_module_params_t camera_module[CAMERA_NUM] = {
            [CAMERA_IDX0] = {
              .session = CAMERA_SESSION_VISIBLE,

              .params = {
                [CAMERA_SESSION_VISIBLE] = {
                  .actuator = load_normal_actuator_driver,
                  .sensor = load_cv4002_sensor_driver,
                  .csid = load_cv4002_csi2d_driver,
                  .tune = {
                    [CAMERA_HDR_MODE_NORMAL] = {load_cv4002_normal_tuning, "cv4002_nor.tun"},
                  },
                  .calib = {load_cv4002_calibration, "cv4002.cal"},
                },

                [CAMERA_SESSION_IR] = {
                  .actuator = load_normal_actuator_driver,
                  .sensor = load_cv4002_sensor_driver,
                  .csid = load_cv4002_csi2d_driver,
                  .tune = {
                    [CAMERA_HDR_MODE_NORMAL] = {load_cv4002_normal_ir_tuning, "cv4002_nor_ir.tun"},
                  },
                  .calib = {load_cv4002_ir_calibration, "cv4002_ir.cal"},
                },

                [CAMERA_SESSION_WHITELIGHT] = {
                  .actuator = load_normal_actuator_driver,
                  .sensor = load_cv4002_sensor_driver,
                  .csid = load_cv4002_csi2d_driver,
                  .tune = {
                    [CAMERA_HDR_MODE_NORMAL] = {load_cv4002_normal_wl_tuning, "cv4002_nor_wl.tun"},
                  },
                  .calib = {load_cv4002_wl_calibration, "cv4002_wl.cal"},
                },
              },
            },
            [CAMERA_IDX1] = { .session = CAMERA_SESSION_INV, },
            [CAMERA_IDX2] = { .session = CAMERA_SESSION_INV, },
            [CAMERA_IDX3] = { .session = CAMERA_SESSION_INV, },
          };

#elif defined(SC2336P)
extern sensor_drv_params_t *load_sc2336p_sensor_driver(void);
extern sensor_drv_params_t* load_sc2336p_aux1_sensor_driver(void);
extern csi2d_drv_params_t *load_sc2336p_csi2d_driver(void);
extern csi2d_drv_params_t *load_sc2336p_aux1_csi2d_driver(void);
extern tune_params_t *load_sc2336p_normal_tuning(const char8 *);
extern calibration_params_t *load_sc2336p_calibration(const char8 *);
extern calibration_params_t *load_sc2336p_ir_calibration(const char8 *);
extern calibration_params_t *load_sc2336p_wl_calibration(const char8 *);
extern tune_params_t *load_sc2336p_normal_ir_tuning(const char8 *);
extern tune_params_t *load_sc2336p_normal_wl_tuning(const char8 *);

static camera_module_params_t camera_module[CAMERA_NUM] = {
	[CAMERA_IDX0] = {
		.session = CAMERA_SESSION_VISIBLE,

		.params = {
			[CAMERA_SESSION_VISIBLE] = {
				.actuator = load_normal_actuator_driver,
				.sensor = load_sc2336p_sensor_driver,
				.csid = load_sc2336p_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_sc2336p_normal_tuning, "sc2336p_nor.tun"},
				},
				.calib = {load_sc2336p_calibration, "sc2336p.cal"},
			},

			[CAMERA_SESSION_IR] = {
				.actuator = load_normal_actuator_driver,
				.sensor = load_sc2336p_sensor_driver,
				.csid = load_sc2336p_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_sc2336p_normal_ir_tuning, "sc2336p_nor_ir.tun"},
				},
				.calib = {load_sc2336p_ir_calibration, "sc2336p.cal"},
			},

			[CAMERA_SESSION_WHITELIGHT] = {
				.actuator = load_normal_actuator_driver,
				.sensor = load_sc2336p_sensor_driver,
				.csid = load_sc2336p_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_sc2336p_normal_wl_tuning, "sc2336p_nor_wl.tun"},
				},
				.calib = {load_sc2336p_wl_calibration, "sc2336p.cal"},
			},
		},
	},

	[CAMERA_IDX1] = {
		.session = CAMERA_SESSION_VISIBLE,

		.params = {
			[CAMERA_SESSION_VISIBLE] = {
				.actuator = load_normal_actuator_aux1_driver,
				.sensor = load_sc2336p_aux1_sensor_driver,
				.csid = load_sc2336p_aux1_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_sc2336p_normal_tuning/*tuning_linear_slave*/, "sc2336p_nor.tun"},
				},
				.calib = {load_sc2336p_calibration, "sc2336p_ir.cal"},
			},

			[CAMERA_SESSION_IR] = {
				.actuator = load_normal_actuator_aux1_driver,
				.sensor = load_sc2336p_aux1_sensor_driver,
				.csid = load_sc2336p_aux1_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_sc2336p_normal_ir_tuning, "sc2336p_nor_ir.tun"},
				},
				.calib = {load_sc2336p_ir_calibration, "sc2336p_ir.cal"},
			},

			[CAMERA_SESSION_WHITELIGHT] = {
				.actuator = load_normal_actuator_aux1_driver,
				.sensor = load_sc2336p_aux1_sensor_driver,
				.csid = load_sc2336p_aux1_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_sc2336p_normal_wl_tuning, "sc2336p_nor_wl.tun"},
				},
				.calib = {load_sc2336p_wl_calibration, "sc2336p_wl.cal"},
			},
		},
	},
	[CAMERA_IDX2] = { .session = CAMERA_SESSION_INV, },
	[CAMERA_IDX3] = { .session = CAMERA_SESSION_INV, },
};

#elif defined(C3390)
extern sensor_drv_params_t *load_c3390_sensor_driver(void);
extern csi2d_drv_params_t *load_c3390_csi2d_driver(void);

extern tune_params_t *load_c3390_normal_tuning(const char8 *);
extern calibration_params_t *load_c3390_calibration(const char8 *);
extern calibration_params_t *load_c3390_ir_calibration(const char8 *);
extern tune_params_t *load_c3390_normal_ir_tuning(const char8 *);

static camera_module_params_t camera_module[CAMERA_NUM] = {
	[CAMERA_IDX0] = {
		.session = CAMERA_SESSION_VISIBLE,

		.params = {
			[CAMERA_SESSION_VISIBLE] = {
				.actuator = load_normal_actuator_driver,
				.sensor = load_c3390_sensor_driver,
				.csid = load_c3390_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_c3390_normal_tuning, "c3390_nor.tun"},
				},
				.calib = {load_c3390_calibration, "c3390.cal"},
			},

			[CAMERA_SESSION_IR] = {
				.actuator = load_normal_actuator_driver,
				.sensor = load_c3390_sensor_driver,
				.csid = load_c3390_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_c3390_normal_ir_tuning, "c3390_nor_ir.tun"},
				},
				.calib = {load_c3390_ir_calibration, "c3390_ir.cal"},
			},
		},
	},
	[CAMERA_IDX1] = { .session = CAMERA_SESSION_INV, },
	[CAMERA_IDX2] = { .session = CAMERA_SESSION_INV, },
	[CAMERA_IDX3] = { .session = CAMERA_SESSION_INV, },
};
#elif defined(TARGET_BOARD_HLDUAL)  //hi556
static camera_module_params_t camera_module[CAMERA_NUM] = {
	[CAMERA_IDX0] = {
	.session = CAMERA_SESSION_VISIBLE,

		.params = {
			[CAMERA_SESSION_VISIBLE] = {
				.actuator = load_normal_actuator_driver,
				.sensor = load_hi556_sensor_driver,
				.csid = load_hi556_csi2d_driver,
				.tune = {
				[CAMERA_HDR_MODE_NORMAL] = {load_hi556_normal_tuning, "hi556_nor.tun"},
				},
			.calib = {load_hi556_calibration, "hi556.cal"},
			},

			[CAMERA_SESSION_IR] = {
				.actuator = load_normal_actuator_driver,
				.sensor = load_hi556_sensor_driver,
				.csid = load_hi556_csi2d_driver,
				.tune = {
				[CAMERA_HDR_MODE_NORMAL] = {load_hi556_normal_ir_tuning, "hi556_nor_ir.tun"},
				},
				.calib = {load_hi556_ir_calibration, "hi556_ir.cal"},
			},
		},
	},
	[CAMERA_IDX1] = {
		.session = CAMERA_SESSION_VISIBLE,

		.params = {
			[CAMERA_SESSION_VISIBLE] = {
				.actuator = load_normal_actuator_aux1_driver,
				.sensor = load_hi556_aux1_sensor_driver,
				.csid = load_hi556_aux1_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_hi556_normal_tuning, "hi556_nor.tun"},
				},
				.calib = {load_hi556_calibration, "hi556.cal"},
			},

			[CAMERA_SESSION_IR] = {
				.actuator = load_normal_actuator_aux1_driver,
				.sensor = load_hi556_aux1_sensor_driver,
				.csid = load_hi556_aux1_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_hi556_normal_ir_tuning, "hi556_nor_ir.tun"},
				},
				.calib = {load_hi556_ir_calibration, "hi556_ir.cal"},
			},
		},
	},

	[CAMERA_IDX2] = { .session = CAMERA_SESSION_INV, },
	[CAMERA_IDX3] = { .session = CAMERA_SESSION_INV, },
};
#elif defined(TARGET_BOARD_EVB88) || defined(TARGET_BOARD_FPGA)
static camera_module_params_t camera_module[CAMERA_NUM] = {
  [CAMERA_IDX0] = {
    .session = CAMERA_SESSION_VISIBLE,

    .params = {
      [CAMERA_SESSION_VISIBLE] = {
        .actuator = load_normal_actuator_driver,
        .sensor = load_imx415_sensor_driver,
        .csid = load_imx415_csi2d_driver_434,
        .tune = {
          [CAMERA_HDR_MODE_NORMAL] = {load_imx415_normal_tuning, "imx415_nor.tun"},
          [CAMERA_HDR_MODE_DOL2] = {load_imx415_dol2_tuning, "imx415_dol2.tun"},
        },
        .calib = {load_imx415_calibration, "imx415.cal"},
      },

      [CAMERA_SESSION_IR] = {
        .actuator = load_normal_actuator_driver,
        .sensor = load_imx415_sensor_driver,
        .csid = load_imx415_csi2d_driver_434,
        .tune = {
          [CAMERA_HDR_MODE_NORMAL] = {load_imx415_normal_ir_tuning, "imx415_nor_ir.tun"},
          [CAMERA_HDR_MODE_DOL2] = {load_imx415_dol2_ir_tuning, "imx415_dol2_ir.tun"},
        },
        .calib = {load_imx415_ir_calibration, "imx415_ir.cal"},
      },
    },
  },
  [CAMERA_IDX1] = { .session = CAMERA_SESSION_INV, },
  [CAMERA_IDX2] = { .session = CAMERA_SESSION_INV, },
  [CAMERA_IDX3] = { .session = CAMERA_SESSION_INV, },
};
#elif defined(TARGET_BOARD_IPC) || defined(TARGET_BOARD_PTZ)
static camera_module_params_t camera_module[CAMERA_NUM] = {
	[CAMERA_IDX0] = {
		.session = CAMERA_SESSION_VISIBLE,

		.params = {
			[CAMERA_SESSION_VISIBLE] = {
				.actuator = load_normal_actuator_driver,
				.sensor = load_gc4023_sensor_driver,
				.csid = load_gc4023_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_gc4023_normal_tuning, "gc4023_nor.tun"},
				},
				.calib = {load_gc4023_calibration, "gc4023.cal"},
			},

			[CAMERA_SESSION_IR] = {
				.actuator = load_normal_actuator_driver,
				.sensor = load_gc4023_sensor_driver,
				.csid = load_gc4023_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_gc4023_normal_ir_tuning, "gc4023_nor_ir.tun"},
				},
				.calib = {load_gc4023_calibration_ir, "gc4023_ir.cal"},
			},
		},
	},
	[CAMERA_IDX1] = { .session = CAMERA_SESSION_INV, },
	[CAMERA_IDX2] = { .session = CAMERA_SESSION_INV, },
	[CAMERA_IDX3] = { .session = CAMERA_SESSION_INV, },
};
#elif(defined(SC501AI))
static camera_module_params_t camera_module[CAMERA_NUM] = {
  [CAMERA_IDX0] = {
    .session = CAMERA_SESSION_VISIBLE,

    .params = {
      [CAMERA_SESSION_VISIBLE] = {
        .actuator = load_normal_actuator_driver,
        .sensor = load_sc501ai_sensor_driver,
        .csid = load_sc501ai_csi2d_driver,
        .tune = {
          [CAMERA_HDR_MODE_NORMAL] = {load_sc501ai_normal_tuning, "sc501ai.tun"},
        },
        .calib = {load_sc501ai_calibration, "sc501ai.cal"},
      },

      [CAMERA_SESSION_IR] = {
        .actuator = load_normal_actuator_driver,
        .sensor = load_sc501ai_sensor_driver,
        .csid = load_sc501ai_csi2d_driver,
        .tune = {
          [CAMERA_HDR_MODE_NORMAL] = {load_sc501ai_normal_ir_tuning, "sc501ai_nor_ir.tun"},
        },
        .calib = {load_sc501ai_ir_calibration, "sc501ai_ir.cal"},
      },
    },
  },
  [CAMERA_IDX1] = { .session = CAMERA_SESSION_INV, },
  [CAMERA_IDX2] = { .session = CAMERA_SESSION_INV, },
  [CAMERA_IDX3] = { .session = CAMERA_SESSION_INV, },
};
#elif(defined(SC430AI))
static camera_module_params_t camera_module[CAMERA_NUM] = {
  [CAMERA_IDX0] = {
    .session = CAMERA_SESSION_VISIBLE,

    .params = {
      [CAMERA_SESSION_VISIBLE] = {
        .actuator = load_normal_actuator_driver,
        .sensor = load_sc430ai_sensor_driver,
        .csid = load_sc430ai_csi2d_driver,
        .tune = {
          [CAMERA_HDR_MODE_NORMAL] = {load_sc430ai_normal_tuning, "sc430ai_nor.tun"},
        },
        .calib = {load_sc430ai_calibration, "sc430ai.cal"},
      },

      [CAMERA_SESSION_IR] = {
        .actuator = load_normal_actuator_driver,
        .sensor = load_sc430ai_sensor_driver,
        .csid = load_sc430ai_csi2d_driver,
        .tune = {
          [CAMERA_HDR_MODE_NORMAL] = {load_sc430ai_normal_ir_tuning, "sc430ai_nor_ir.tun"},
        },
        .calib = {load_sc430ai_ir_calibration, "sc430ai_ir.cal"},
      },
    },
  },
  [CAMERA_IDX1] = { .session = CAMERA_SESSION_INV, },
  [CAMERA_IDX2] = { .session = CAMERA_SESSION_INV, },
  [CAMERA_IDX3] = { .session = CAMERA_SESSION_INV, },
};
#elif defined(TARGET_BOARD_EVB)
static camera_module_params_t camera_module[CAMERA_NUM] = {
	[CAMERA_IDX0] = {
		.session = CAMERA_SESSION_VISIBLE,

		.params = {
			[CAMERA_SESSION_VISIBLE] = {
				.actuator = load_normal_actuator_driver,
				.sensor = load_gc4023_sensor_driver,
				.csid = load_gc4023_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_gc4023_normal_tuning, "gc4023_nor.tun"},
				},
				.calib = {load_gc4023_calibration_dual, "gc4023.cal"},
			},

			[CAMERA_SESSION_IR] = {
				.actuator = load_normal_actuator_driver,
				.sensor = load_gc4023_sensor_driver,
				.csid = load_gc4023_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_gc4023_normal_ir_tuning, "gc4023_nor_ir.tun"},
				},
				.calib = {load_gc4023_calibration_dual_ir, "gc4023_ir.cal"},
			},
		},
	},
	[CAMERA_IDX1] = {
		.session = CAMERA_SESSION_VISIBLE,

		.params = {
			[CAMERA_SESSION_VISIBLE] = {
				.actuator = load_normal_actuator_aux1_driver,
				.sensor = load_gc4023_aux1_sensor_driver,
				.csid = load_gc4023_aux1_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_gc4023_normal_tuning, "gc4023_nor_slave.tun"},
				},
				.calib = {load_gc4023_calibration_dual, "gc4023.cal"},
			},

			[CAMERA_SESSION_IR] = {
				.actuator = load_normal_actuator_aux1_driver,
				.sensor = load_gc4023_aux1_sensor_driver,
				.csid = load_gc4023_aux1_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_gc4023_normal_ir_tuning, "gc4023_nor_ir.tun"},
				},
				.calib = {load_gc4023_calibration_dual_ir, "gc4023_ir.cal"},
			},
		},
	},

	[CAMERA_IDX2] = { .session = CAMERA_SESSION_INV, },
	[CAMERA_IDX3] = { .session = CAMERA_SESSION_INV, },
};
#elif defined(TARGET_BOARD_AIO)
#if 1 //gc4023
static camera_module_params_t camera_module[CAMERA_NUM] = {
	[CAMERA_IDX0] = {
		.session = CAMERA_SESSION_VISIBLE,

		.params = {
			[CAMERA_SESSION_VISIBLE] = {
				.actuator = load_normal_actuator_driver,
				.sensor = load_gc4023_sensor_driver,
				.csid = load_gc4023_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_gc4023_normal_tuning, "gc4023_nor.tun"},
				},
				.calib = {load_gc4023_calibration_dual, "gc4023.cal"},
			},

			[CAMERA_SESSION_IR] = {
				.actuator = load_normal_actuator_driver,
				.sensor = load_gc4023_sensor_driver,
				.csid = load_gc4023_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_gc4023_normal_ir_tuning, "gc4023_nor_ir.tun"},
				},
				.calib = {load_gc4023_calibration_dual_ir, "gc4023_ir.cal"},
			},
		},
	},
	[CAMERA_IDX1] = {
		.session = CAMERA_SESSION_VISIBLE,

		.params = {
			[CAMERA_SESSION_VISIBLE] = {
				.actuator = load_normal_actuator_aux1_driver,
				.sensor = load_gc4023_aux1_sensor_driver,
				.csid = load_gc4023_aux1_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_gc4023_normal_tuning, "gc4023_nor_slave.tun"},
				},
				.calib = {load_gc4023_calibration_dual, "gc4023.cal"},
			},

			[CAMERA_SESSION_IR] = {
				.actuator = load_normal_actuator_aux1_driver,
				.sensor = load_gc4023_aux1_sensor_driver,
				.csid = load_gc4023_aux1_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_gc4023_normal_ir_tuning, "gc4023_nor_ir.tun"},
				},
				.calib = {load_gc4023_calibration_dual_ir, "gc4023_ir.cal"},
			},
		},
	},

	[CAMERA_IDX2] = { .session = CAMERA_SESSION_INV, },
	[CAMERA_IDX3] = { .session = CAMERA_SESSION_INV, },
};
#else  //sc200ai
static camera_module_params_t camera_module[CAMERA_NUM] = {
	[CAMERA_IDX0] = {
	.session = CAMERA_SESSION_VISIBLE,

		.params = {
			[CAMERA_SESSION_VISIBLE] = {
				.actuator = load_normal_actuator_driver,
				.sensor = load_sc200ai_sensor_driver,
				.csid = load_sc200ai_csi2d_driver,
				.tune = {
				[CAMERA_HDR_MODE_NORMAL] = {load_sc200ai_normal_tuning, "gc4023_nor.tun"},
				},
			.calib = {load_sc200ai_calibration, "sc200ai.cal"},
			},

			[CAMERA_SESSION_IR] = {
				.actuator = load_normal_actuator_driver,
				.sensor = load_sc200ai_sensor_driver,
				.csid = load_sc200ai_csi2d_driver,
				.tune = {
				[CAMERA_HDR_MODE_NORMAL] = {load_sc200ai_normal_ir_tuning, "sc200ai_nor_ir.tun"},
				},
				.calib = {load_sc200ai_ir_calibration, "sc200ai_ir.cal"},
			},
		},
	},
	[CAMERA_IDX1] = {
		.session = CAMERA_SESSION_VISIBLE,

		.params = {
			[CAMERA_SESSION_VISIBLE] = {
				.actuator = load_normal_actuator_aux1_driver,
				.sensor = load_sc200ai_aux1_sensor_driver,
				.csid = load_sc200ai_aux1_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_sc200ai_normal_tuning, "sc200ai_nor.tun"},
				},
				.calib = {load_sc200ai_calibration, "sc200ai.cal"},
			},

			[CAMERA_SESSION_IR] = {
				.actuator = load_normal_actuator_aux1_driver,
				.sensor = load_sc200ai_aux1_sensor_driver,
				.csid = load_sc200ai_aux1_csi2d_driver,
				.tune = {
					[CAMERA_HDR_MODE_NORMAL] = {load_sc200ai_normal_ir_tuning, "sc200ai_nor_ir.tun"},
				},
				.calib = {load_sc200ai_ir_calibration, "sc200ai_ir.cal"},
			},
		},
	},

	[CAMERA_IDX2] = { .session = CAMERA_SESSION_INV, },
	[CAMERA_IDX3] = { .session = CAMERA_SESSION_INV, },
};
#endif
#endif

camera_module_params_t *get_camera_modules_inst(void)
{
	return camera_module;
}
