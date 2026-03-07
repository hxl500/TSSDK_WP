# dpu, arm malidp
mali-dp-y := malidp_drv.o malidp_hw.o malidp_planes.o malidp_crtc.o
obj-$(CONFIG_DRM_MALIDP)	+= mali-dp.o

# dpu, nemadc
nemadc-y := nemadc_drv.o nemadc_hw.o nemadc_planes.o nemadc_crtc.o
obj-$(CONFIG_DRM_NEMADC)	+= nemadc.o

# virtual hdmi, dpu connect lcd directly
obj-$(CONFIG_DRM_TS_VIRT_HDMI)	+= virt_hdmi.o

# hdmi
obj-$(CONFIG_DRM_TS_DW_HDMI) += dw_hdmi_ts.o

# mipi
obj-$(CONFIG_DRM_TS_DW_MIPI_DSI)	+= dw-mipi-dsi.o

# spi
obj-$(CONFIG_DRM_NEMADC_SPI)	+= nema_dc_mipi.o

