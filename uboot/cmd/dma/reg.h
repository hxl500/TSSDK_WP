/*****************************************************************************
* Copyright (C) 2021,Tsing Micro Technology Co., Ltd,All rights reserved.
* FileName     : reg.h
* Author       :
* Data         :
* OverView     :
*
* History      :
* Revisor      :
* Date         :
* Description  :
*
*****************************************************************************/
#ifndef _REG_H
#define _REG_H

/* TOP System */
#define OSPI_MEM_BASE       0xE0000000UL  	 /* 128MB,OSPI flash memory addr  */
#define OSPI0_BASE			OSPI_MEM_BASE
#define AI_MEM				0xF0000000UL	 /* 2MB */
#define AI_CFG_RNE			0xF0200000UL	 /* 2MB */
#define AI_CFG_RCE			0xF0400000UL	 /* 2MB */
#define AI_CFG_AIISP		0xF0600000UL	 /* 2MB */

#define SYS_MMU_BASE      	0xF0800000UL	 /* 4MB */
#define USB3C_BASE          0xF0C00000UL	 /* 1MB */

#define SPACC_BASE          0xF0D00000UL
#define PKE_BASE            0xF0D10000UL
#define HASH_BASE           0xF0D20000UL
#define SKE_BASE            0xF0D21000UL
#define TRNG_BASE           0xF0D22000UL
#define ROM_BASE            0xF0D40000UL  /* 64KB,BOOTROM data memory     */
#define GMAC_BASE           0xF0D60000UL
#define SDMMC0_BASE         0xF0D70000UL //not good
#define SDMMC1_BASE         0xF0D71000UL
#define GDC_BASE			0xF0D72000UL
#define CVE_BASE			0xF0D73000UL
#define GPU_BASE			0xF0D74000UL

#define I2C0_BASE           0xF1281000UL
#define I2C1_BASE           0xF1282000UL
#define I2C2_BASE           0xF0D80000UL
#define I2C3_BASE           0xF0D81000UL
#define I2C4_BASE           0xF0D82000UL
#define I2C5_BASE           0xF0D83000UL
#define I2C6_BASE           0xF0D84000UL
#define I2C7_BASE           0xF0D85000UL
#define I2C8_BASE           0xF0D86000UL //...........
#define I2C9_BASE           0xF0D87000UL
#define USI2_BASE           0xF0D88000UL
#define USI3_BASE           0xF0D89000UL
#define UART1_BASE          0xF0D8A000UL
#define UART2_BASE          0xF0D8B000UL
#define UART3_BASE          0xF0D8C000UL
#define UART4_BASE          0xF0D8D000UL
#define UART5_BASE          0xF0D8E000UL
#define UART6_BASE          0xF0D8F000UL
#define SEC_TMR_BASE        0xF0D90000UL  /* Security timer               */
#define SYS_TMR_BASE        0xF0D91000UL  /* General timer                */
#define OTPC_BASE           0xF0D92000UL
#define ADC_BASE            0xF0D93000UL
#define GPIO_BASE           0xF0D94000UL
#define GPIO_PORTA_BASE     0xF0D94000UL
#define GPIO_PORTB_BASE     0xF0D94400UL
#define GPIO_PORTC_BASE     0xF0D94800UL
#define GPIO_PORT_BASE      0xF0D94C00UL
#define PADCTRL_BASE        0xF0D95000UL
#define MAILBOX_BASE        0xF0D96000UL
#define SEC_WDT_BASE        0xF0D97000UL  /* Security WDT                 */
#define CPU_WDT_BASE        0xF0D98000UL  /* A53 WDT                      */
#define CGU_BASE            0xF0D9A000UL  /* clock generating unit        */
#define RGU_BASE            0xF0D9B000UL  /* reset generating unit        */
#define BT1120_TX_BASE      0xF0D9C000UL
#define BT1120_RX_BASE      0xF0D9D000UL
#define OSPI_BASE           0xF0D9E000UL  /* OSPI_0 controller            */

#define SPI0_BASE           0xF0DA0000UL
#define SPI1_BASE           0xF0DA1000UL
#define SPI2_BASE           0xF0DA2000UL
#define SPI3_BASE           0xF0DA3000UL
#define SPI4_BASE           0xF0DA4000UL
#define I2S0_BASE           0xF0DA5000UL
#define I2S1_BASE           0xF0DA6000UL

#define NFC_BASE           0xF0DA8000UL

#define CX400_BASE          0xF0DC0000UL  /* CoreSight 400 debug bus      */

#define DP650_BASE          0xF0E00000UL  /* DPU                          */
#define HDMITX_BASE         0xF0E20000UL  /* HDMI tx                      */
#define DMA2MIPI_BASE       0xF0E30000UL  /* dma to mipi                  */
#define CSIDEV_BASE         0xF0E31000UL  /* csi device                   */
#define DSIHOST_BASE        0xF0E32000UL  /* dsi host                     */
#define NEMADC_BASE         0xF0E34000UL  /* DPU-lite                     */

#define ISP_BASE            0xF0E40000UL
#define CSI_HOST_0_BASE     0xF0E50000UL
#define CSI_HOST_1_BASE     0xF0E51000UL
#define HDR_BASE            0xF0E52000UL
#define MIPI2DMA_BASE		0xF0E53000UL
#define VPE_BASE            0xF0E54000UL
#define TNR_BASE            0xF0E58000UL

#define WAVE_BASE			0xF0E60000UL
#define CODA_BASE			0xF0E70000UL
#define DDRC_BASE           0xF0E80000UL  /* ddr ctrl */
#define DDRC_PHY_BASE       0xF0E88000UL  /* ddr phy */
#define GIC400_BASE         0xF0E90000UL  /* GIC-400 interrupt controller */

#define DMAC1_BASE			0xF0E98000UL  /* DMAC 1 configure register */
#define DMAC0_USER_BASE		0xF0E9A000UL  /* DMAC 0 Non-secured configure register */
#define DMAC0_SEC_BASE		0xF0E9B000UL  /* DMAC 0 secured configure register */
#define NOC_BASE 			0xF0EA0000UL	/* NOC configuration port       */

/* MCU system */
#define RNE_BASE			0xF1000000UL  /* 2M */
#define SHRAM_BASE          0xF1200000UL  /* 512KB                        */
#define DVP_BASE			0xF1280000UL
#define I2C0_BASE			0xF1281000UL
#define I2C1_BASE			0xF1282000UL
#define UART0_BASE			0xF1283000UL
#define USI0_BASE			0xF1284000UL
#define USI1_BASE			0xF1285000UL
#define MCU_TIMER_BASE		0xF1286000UL
#define MCU_WDT_BASE		0xF1287000UL
#define PWM_BASE            0xF1288000UL
#define MCU_GPIO_BASE		0xF1289000UL
#define MCU_PADC_BASE		0xF128A000UL
#define MCU_CLKG_RSTGG_BASE	0xF128B000UL	/* CLK gen and reset gen */
#define MCU_GRF_BASE		0xF128C000UL
#define MCU_SGRF_BASE		0xF128D000UL

#define AUDIO_DATA_BASE		0xF1290000UL
#define AUDIO_REG_BASE		0xF12A0000UL
#define DMAC2_BASE			0xF12A1000UL

/* always-on system */
#define SMU_BASE			0xF12A2000UL
#define RTC_BASE        	0xF12A3000UL
#define SCNT_BASE        	0xF12A4000UL

#endif
