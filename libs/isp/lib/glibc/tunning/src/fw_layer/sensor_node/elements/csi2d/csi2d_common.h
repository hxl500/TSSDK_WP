#ifndef __CSI2D_COMMON_H__
#define __CSI2D_COMMON_H__

#include "sensor_common.h"

#define CSI2D_IPI_NUM_MAX (3)

#define CSI2D_DRV_SETTING_MAX (DRV_SENSOR_SETTING_MAX)
#define CSI2D_MBUS_FMT_MASK (0xF)
#define REG_SOF_DELAY_INFINATE (0xFFFFFFFF)

typedef enum csi2d_mbus_formats mbus_format_e;
typedef enum csi2d_hw_type csi2d_hw_type_e;

#if defined(TARGET_CHIP_TX536)
#define CSI2D_DEVS_NUM (3)
enum csi2d_muxer_ipi_id {
	MUXER_MIPI0_IPI_0 = 0,
	MUXER_MIPI0_IPI_1 = 1,
	MUXER_MIPI0_IPI_2 = 2,
	MUXER_MIPI1_IPI_0 = 3,
	MUXER_MIPI2_IPI_0 = 4,
	MUXER_INPUT_TOTAL_NUM
};

enum csi2d_muxer_chn_id {
	MUXER_ISP_CHN_0 = 0,
	MUXER_ISP_CHN_1 = 1,
	MUXER_ISP_CHN_2 = 2,
	MUXER_OUTPUT_TOTAL_NUM
};
#elif defined(TARGET_CHIP_TX5215) || defined(TARGET_CHIP_TX5239) || defined(TARGET_CHIP_TX5112)
#define CSI2D_DEVS_NUM (2)
enum csi2d_muxer_ipi_id {
	MUXER_MIPI0_IPI_0 = 0,
	MUXER_MIPI0_IPI_1 = 1,
	MUXER_MIPI1_IPI_0 = 2,
	MUXER_INPUT_TOTAL_NUM
};

enum csi2d_muxer_chn_id {
	MUXER_ISP_CHN_0 = 0,
	MUXER_ISP_CHN_1 = 1,
	MUXER_OUTPUT_TOTAL_NUM
};
#else
#error undefined target chip
#endif

enum csi2d_hw_type {
	CSI2D_HW_TYPE_TOP = 0,

	CSI2D_HW_TYPE_536_BASE = 0x10,
	CSI2D_HW_TYPE_536_434 = CSI2D_HW_TYPE_536_BASE + 0,
	CSI2D_HW_TYPE_536_414 = CSI2D_HW_TYPE_536_BASE + 1,
	CSI2D_HW_TYPE_536_212 = CSI2D_HW_TYPE_536_BASE + 2,

	CSI2D_HW_TYPE_TX5215_BASE = 0x20,
	CSI2D_HW_TYPE_TX5215_42 = CSI2D_HW_TYPE_TX5215_BASE + 0,
	CSI2D_HW_TYPE_TX5215_21 = CSI2D_HW_TYPE_TX5215_BASE + 1,

	CSI2D_HW_TYPE_BOTTOM
};

enum csi2d_mbus_formats {
	CSI2D_MBUS_FMT_INVAL = 0x0000,
	CSI2D_MBUS_FMT_FIXED = 0x0001,

	CSI2D_MBUS_FMT_BAYER_RAW10_BASE = 0x1000,
	CSI2D_MBUS_FMT_SRGGB10_1x10 = 0x1001,
	CSI2D_MBUS_FMT_SGRBG10_1x10 = 0x1002,
	CSI2D_MBUS_FMT_SGBRG10_1x10 = 0x1004,
	CSI2D_MBUS_FMT_SBGGR10_1x10 = 0x1008,

	CSI2D_MBUS_FMT_BAYER_RAW12_BASE = 0x1010,
	CSI2D_MBUS_FMT_SRGGB12_1x12 = 0x1011,
	CSI2D_MBUS_FMT_SGRBG12_1x12 = 0x1012,
	CSI2D_MBUS_FMT_SGBRG12_1x12 = 0x1014,
	CSI2D_MBUS_FMT_SBGGR12_1x12 = 0x1018,

	CSI2D_MBUS_FMT_YUV422_8_BASE = 0x2000,
	CSI2D_MBUS_FMT_UYVY8_2X8 = 0x2001,
};

enum csi2d_data_type {
	CSI2D_DATA_TYPE_EMBEDDED_DATA_8BIT = 0x12,
	CSI2D_DATA_TYPE_YUV420_8BIT = 0x18,
	CSI2D_DATA_TYPE_YUV420_10BIT = 0x19,
	CSI2D_DATA_TYPE_YUV420_8BIT_LEGACY = 0x1a,
	CSI2D_DATA_TYPE_YUV420_8BIT_SHIFTED = 0x1c,
	CSI2D_DATA_TYPE_YUV420_10BIT_SHIFTED = 0x1d,
	CSI2D_DATA_TYPE_YUV422_8BIT = 0x1e,
	CSI2D_DATA_TYPE_YUV422_10BIT = 0x1f,
	CSI2D_DATA_TYPE_RGB444 = 0x20,
	CSI2D_DATA_TYPE_RGB555 = 0x21,
	CSI2D_DATA_TYPE_RGB565 = 0x22,
	CSI2D_DATA_TYPE_RGB666 = 0x23,
	CSI2D_DATA_TYPE_RGB888 = 0x24,
	CSI2D_DATA_TYPE_RAW_24BIT = 0x27,
	CSI2D_DATA_TYPE_RAW_6BIT = 0x28,
	CSI2D_DATA_TYPE_RAW_7BIT = 0x29,
	CSI2D_DATA_TYPE_RAW_8BIT = 0x2a,
	CSI2D_DATA_TYPE_RAW_10BIT = 0x2b,
	CSI2D_DATA_TYPE_RAW_12BIT = 0x2c,
	CSI2D_DATA_TYPE_RAW_14BIT = 0x2d,
	CSI2D_DATA_TYPE_RAW_16BIT = 0x2e,
	CSI2D_DATA_TYPE_RAW_20BIT = 0x2f,
};

enum csi2d_format_compress_mode {
	CSI2D_FORMAT_UNCOMPRESSED_8_BIT,
	CSI2D_FORMAT_UNCOMPRESSED_10_BIT,
	CSI2D_FORMAT_UNCOMPRESSED_12_BIT,
};

typedef struct csi2d_capability csi2d_capability_t;
struct csi2d_capability {
	u8 lane_num_max;
	u32 lane_rate_min;
	u32 lane_rate_max;
	u8 vc_num_max;
};

struct csi2d_params {	
	u32 code;
	union {
		struct {	
			f32 fcfg_clk;
			struct ipi_interface {
				u32 active :1;

				u32 vcid :2;
				u32 embedded_data_en :1;

				u32 hsd_time :12;

				u32 line_event_selection :1;
				u32 en_video :1;
				u32 en_line_start :1;
				u32 en_null :1;
				u32 en_blanking :1;
				u32 en_embedded :1;
				u32 ipi_sync_event_mode :1;
			} ipi[CSI2D_IPI_NUM_MAX];
			u32 ipi_num;
		}s;
		u32 data[32];
	}u;
}; 

struct csi2d_drv_params {
	// s32 cam_id;
	enum csi2d_hw_type type;

	struct {
		BOOL enable;
		const reg_cfg_t *cr_setting;
		const reg_cfg_t *grf_setting;
		const reg_cfg_t *dphy_setting;
		const reg_cfg_t *csi2d_setting;
	} inits;

	struct csi2d_params params[CSI2D_DRV_SETTING_MAX];
	//u32 pid;
};

typedef struct csi2d_drv_params csi2d_drv_params_t;
typedef csi2d_drv_params_t *(*load_csi2d_drv_func)(void);
#endif
