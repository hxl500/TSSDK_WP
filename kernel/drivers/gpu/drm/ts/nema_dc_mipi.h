#ifndef  __NEMC_DC_MIPI_H__
#define  __NEMC_DC_MIPI_H__
#include "nemadc_hw.h"
typedef enum {
    NEMA_REG_MODE             = 0x00,
    NEMA_REG_CLKCTRL          = 0x04,
    NEMA_REG_BGCOLOR          = 0x08,
    NEMA_REG_RESXY            = 0x0c,
    NEMA_REG_FRONTPORCHXY     = 0x14,
    NEMA_REG_BLANKINGXY       = 0x18,
    NEMA_REG_BACKPORCHXY      = 0x1c,
    NEMA_REG_CURSORXY         = 0x20,
    NEMA_REG_STARTXY          = 0x24,
    NEMA_REG_DBIB_CFG         = 0x28,
    NEMA_REG_GPIO             = 0x2c,

    NEMA_REG_LAYER0_MODE      = 0x30,
    NEMA_REG_LAYER0_STARTXY   = 0x34,
    NEMA_REG_LAYER0_SIZEXY    = 0x38,
    NEMA_REG_LAYER0_BASEADDR  = 0x3c,
    NEMA_REG_LAYER0_STRIDE    = 0x40,
    NEMA_REG_LAYER0_RESXY     = 0x44,
    NEMA_REG_LAYER0_SCALEX    = 0x48,
    NEMA_REG_LAYER0_SCALEY    = 0x4c,

    NEMA_REG_LAYER1_MODE      = 0x50,
    NEMA_REG_LAYER1_STARTXY   = 0x54,
    NEMA_REG_LAYER1_SIZEXY    = 0x58,
    NEMA_REG_LAYER1_BASEADDR  = 0x5c,
    NEMA_REG_LAYER1_STRIDE    = 0x60,
    NEMA_REG_LAYER1_RESXY     = 0x64,
    NEMA_REG_LAYER1_SCALEX    = 0x68,
    NEMA_REG_LAYER1_SCALEY    = 0x6c,

    NEMA_REG_LAYER2_MODE      = 0x70,
    NEMA_REG_LAYER2_STARTXY   = 0x74,
    NEMA_REG_LAYER2_SIZEXY    = 0x78,
    NEMA_REG_LAYER2_BASEADDR  = 0x7c,
    NEMA_REG_LAYER2_STRIDE    = 0x80,
    NEMA_REG_LAYER2_RESXY     = 0x84,
    NEMA_REG_LAYER2_SCALEX    = 0x88,
    NEMA_REG_LAYER2_SCALEY    = 0x8c,

    NEMA_REG_LAYER3_MODE      = 0x90,
    NEMA_REG_LAYER3_STARTXY   = 0x94,
    NEMA_REG_LAYER3_SIZEXY    = 0x98,
    NEMA_REG_LAYER3_BASEADDR  = 0x9c,
    NEMA_REG_LAYER3_STRIDE    = 0xa0,
    NEMA_REG_LAYER3_RESXY     = 0xa4,
    NEMA_REG_LAYER3_SCALEX    = 0xa8,
    NEMA_REG_LAYER3_SCALEY    = 0xac,

    NEMA_REG_LAYER0_UBASE     = 0xd0,
    NEMA_REG_LAYER0_VBASE     = 0xd4,
    NEMA_REG_LAYER0_UVSTRIDE  = 0xd8,
    NEMA_REG_LAYER1_UBASE     = 0xdc,
    NEMA_REG_LAYER1_VBASE     = 0xe0,
    NEMA_REG_LAYER1_UVSTRIDE  = 0xe4,

    NEMA_REG_DBIB_CMD         = 0xe8,
    NEMA_REG_DBIB_RDAT        = 0xec,

    NEMA_REG_CONFIG           = 0xf0,
    NEMA_REG_IDREG            = 0xf4,
    NEMA_REG_INTERRUPT        = 0xf8,
    NEMA_REG_STATUS           = 0xfc,
    NEMA_REG_COLMOD           = 0x100,
    NEMA_REG_CRC              = 0x184,

    NEMA_REG_PALETTE          = 0x400,
    NEMA_REG_CURSOR_IMAGE     = 0x800,
    NEMA_REG_CURSOR_LUT       = 0xA00,

    NEMA_REG_GAMMALUT_0       = 0x1000,
    NEMA_REG_GAMMALUT_1       = 0x1400,
    NEMA_REG_GAMMALUT_2       = 0x1800,
    NEMA_REG_GAMMALUT_3       = 0x1c00
} nemadc_regs_t;

typedef enum {
    MIPI_enter_idle_mode       = 0x39,
    MIPI_enter_invert_mode     = 0x21,
    MIPI_enter_normal_mode     = 0x13,
    MIPI_enter_partial_mode    = 0x12,
    MIPI_enter_sleep_mode      = 0x10,
    MIPI_exit_idle_mode        = 0x38,
    MIPI_exit_invert_mode      = 0x20,
    MIPI_exit_sleep_mode       = 0x11,
    MIPI_get_3D_control        = 0x3f,
    MIPI_get_address_mode      = 0x0b,
    MIPI_get_blue_channel      = 0x08,
    MIPI_get_diagnostic_result = 0x0f,
    MIPI_get_display_mode      = 0x0d,
    MIPI_get_green_channel     = 0x07,
    MIPI_get_pixel_format      = 0x0c,
    MIPI_get_power_mode        = 0x0a,
    MIPI_get_red_channel       = 0x06,
    MIPI_get_scanline          = 0x45,
    MIPI_get_signal_mode       = 0x0e,
    MIPI_nop                   = 0x00,
    MIPI_read_DDB_continue     = 0xa8,
    MIPI_read_DDB_start        = 0xa1,
    MIPI_read_memory_continue  = 0x3e,
    MIPI_read_memory_start     = 0x2e,
    MIPI_set_3D_control        = 0x3d,
    MIPI_set_address_mode      = 0x36,
    MIPI_set_column_address    = 0x2a,
    MIPI_set_display_off       = 0x28,
    MIPI_set_display_on        = 0x29,
    MIPI_set_gamma_curve       = 0x26,
    MIPI_set_page_address      = 0x2b,
    MIPI_set_partial_columns   = 0x31,
    MIPI_set_partial_rows      = 0x30,
    MIPI_set_pixel_format      = 0x3a,
    MIPI_set_scroll_area       = 0x33,
    MIPI_set_scroll_start      = 0x37,
    MIPI_set_tear_off          = 0x34,
    MIPI_set_tear_on           = 0x35,
    MIPI_set_tear_scanline     = 0x44,
    MIPI_set_vsync_timing      = 0x40,
    MIPI_soft_reset            = 0x01,
    MIPI_write_LUT             = 0x2d,
    MIPI_write_memory_continue = 0x3c,
    MIPI_write_memory_start    = 0x2c,
    MIPI_snapshot              = 0xff,

    MIPI_DBIB_CMD              = (1 << 30),
    MIPI_DBIB_READ             = (1 << 28),
    MIPI_DBIB_STORE_BASE_ADDR  = (1 << 27),
    MIPI_DBIB_NCSX             = (1 << 26)
} nemadc_mipi_cmd_t;

//--------------------------------------------------------------------------
typedef enum {
    MIPICFG_DBI_EN             = (1 << 31),
    MIPICFG_FRC_CSX_0          = (1 << 30),
    MIPICFG_FRC_CSX_1          = (1 << 30) | (1 << 29),
    MIPICFG_SPI_CSX_V          = (1 << 29),
    MIPICFG_DIS_TE             = (1 << 28),
    MIPICFG_SPI_STORE_IDX      = (1 << 27),
    MIPICFG_RESX               = (1 << 25),
    MIPICFG_DMA                = (1 << 24),
    MIPICFG_SPI3               = (1 << 23),
    MIPICFG_SPI4               = (1 << 22),
    MIPICFG_EN_STALL           = (1 << 21),
    MIPICFG_SPI_CPHA           = (1 << 20),
    MIPICFG_SPI_CPOL           = (1 << 19),
    MIPICFG_SPI_JDI            = (1 << 18),
    MIPICFG_SPI_HOLD           = (1 << 17),
    MIPICFG_INV_ADDR           = (1 << 16),
    MIPICFG_LP_EN              = (1 << 11),
    MIPICFG_SINGLE_RD          = (1 << 9),

    MIPICFG_NULL               = 0x00,
    MIPICFG_1RGB111_OPT0       = 0xc1,
    MIPICFG_1RGB111_OPT1       = 0xc9,
    MIPICFG_1RGB111_OPT2       = 0xd1,
    MIPICFG_1RGB111_OPT3       = 0xd9,
    MIPICFG_1RGB111_OPT4       = 0xe1,

    MIPICFG_1RGB332_OPT0       = 0xc2,
    MIPICFG_1RGB444_OPT0       = 0xc3,
    MIPICFG_1RGB565_OPT0       = 0xc5,
    MIPICFG_1RGB666_OPT0       = 0xc6,
    MIPICFG_1RGB888_OPT0       = 0xc7,

    MIPICFG_8RGB332_OPT0       = 0x12,
    MIPICFG_8RGB444_OPT0       = 0x13,
    MIPICFG_8RGB565_OPT0       = 0x15,
    MIPICFG_8RGB666_OPT0       = 0x16,
    MIPICFG_8RGB888_OPT0       = 0x17,

    MIPICFG_16RGB332_OPT0      = 0x02,
    MIPICFG_16RGB444_OPT0      = 0x03,
    MIPICFG_16RGB565_OPT0      = 0x05,
    MIPICFG_16RGB666_OPT0      = 0x06,
    MIPICFG_16RGB666_OPT1      = 0x0e,
    MIPICFG_16RGB888_OPT0      = 0x07,
    MIPICFG_16RGB888_OPT1      = 0x0f,

    MIPICFG_9RGB666_OPT0       = 0x1e

} nemadc_mipi_cfg_t;


typedef enum {

    MIPI_DCS_RGB111            = 0x01,
    MIPI_DCS_RGB332            = 0x02,
    MIPI_DCS_RGB444            = 0x03,
    MIPI_DCS_RGB565            = 0x05,
    MIPI_DCS_RGB666            = 0x06,
    MIPI_DCS_RGB888            = 0x07,

} nemac_dbi_format_mode_t;







void nemadc_MIPI_out(struct nemadc_hw_device *hwdev,int cmd);
void nemadc_MIPI_CFG_out(struct nemadc_hw_device *hwdev,int cfg);
int  nemadc_MIPI_in(struct nemadc_hw_device *hwdev);
int  nemadc_MIPI_updateregion(struct nemadc_hw_device *hwdev,
                              int start_x, int start_y,
                              int end_x,   int end_y,
                              int mode);


void nemadc_MIPI_enable(struct nemadc_hw_device *hwdev);
void nemadc_MIPI_disable(struct nemadc_hw_device *hwdev);

void nemadc_MIPI_set_mode(struct nemadc_hw_device *hwdev,int mode);
void nemadc_MIPI_set_position(struct nemadc_hw_device *hwdev,int minx, int miny, int maxx, int maxy);
void nemadc_MIPI_set_partial_mode(struct nemadc_hw_device *hwdev,int minx, int miny, int maxx, int maxy);
void nemadc_MIPI_start_frame_transfer(struct nemadc_hw_device *hwdev);

void nemadc_MIPI_init(struct nemadc_hw_device *hwdev,struct device	dev);

void nemadc_MIPI_backlight_open(void);
void nemadc_MIPI_backlight_close(void);
#endif // !1