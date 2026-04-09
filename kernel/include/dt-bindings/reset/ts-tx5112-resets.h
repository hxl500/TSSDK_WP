/* SPDX-License-Identifier: GPL-2.0 */
/*
 *  Copyright (C) 2022-2022 Tsingmicro, Inc.
 */

#ifndef _DT_BINDINGS_TS_TX5112_RESETS_H
#define _DT_BINDINGS_TS_TX5112_RESETS_H

#define TS_CPU_DAP_DBG_RST         0
#define TS_CPU_DAP_SYS_RST         1
#define TS_CPU_SOC_DBG_RST         2
#define TS_CPU_L2_RST              3
#define TS_CPU_DBG_RST             4
#define TS_CPU_CORE_RST            5
#define TS_BUS_PD_VI_SYS_RST       6
#define TS_BUS_PD_DDR_SYS_RST      7
#define TS_BUS_PD_AI_SYS_RST       8
#define TS_BUS_PD_VPU_SYS_RST      9
#define TS_AXI_MEM_DMA_ARST        10
#define TS_AXI_OCRAM_ARST          11
#define TS_AXI_SKE_RST             12
#define TS_AXI_UMCTL_PRST          13
#define TS_AXI_MCTL_P0_ARST        14
#define TS_AXI_MCTL_P1_ARST        15
#define TS_AXI_MCTL_P2_ARST        16
#define TS_AXI_MCTL_P3_ARST        17
#define TS_AXI_DDRC_CORE_RST       18
#define TS_AHB_PERI_DMA0_HRST      19
#define TS_AHB_PERI_DMA1_HRST      20
#define TS_AHB_BOOTM_HRST          21
#define TS_AHB_AUD_CODEC_RST       22
#define TS_APB_USI0_PRST           23
#define TS_APB_USI1_PRST           24
#define TS_APB_ADC_PRST            25
#define TS_APB_GPIO_PRST           26
#define TS_APB_WDT_PRST            27
#define TS_APB_PWM_PRST            28
#define TS_APB_OTPC_PRST           29
#define TS_APB_GPIO_B_PRST         30
#define TS_APB_GPIO_A_PRST         31
#define TS_APB_SYS_REG_PRST        32
#define TS_APB_WDT_TICK_RST        33
#define TS_SDHC0_RST               34
#define TS_SDHC1_RST               36
#define TS_OSPI_REF_RST            38
#define TS_OSPI_HRST               39
#define	TS_OSPI_PRST               40
#define TS_USB2C_HRST              41
#define TS_USB_PHY_RST             42
#define TS_GMAC_RST                43
#define TS_I2C0_RST                45
#define TS_I2C1_RST                47
#define TS_I2C2_RST                49
#define TS_UART0_RST               51
#define TS_UART1_RST               53
#define TS_SPI_RST                 55
#define TS_TIMER_PRST              57
#define TS_TMR_T1RST               58
#define TS_TMR_T2RST               59
#define TS_TMR_T3RST               60
#define TS_TMR_T4RST               61
#define TS_TMR_T5RST               62
#define TS_TMR_T6RST               63
#define TS_TMR_T7RST               64
#define TS_TMR_T8RST               65
#define TS_I2S0_RST                66
#define TS_PDM_RST                 68
#define TS_DDR_PHY_PRST            70
#define TS_DDR_PHY_RST             71
#define TS_HDR_HRST                72
#define TS_HDR_ARST                73
#define TS_HDR_SRST                74
#define TS_MIPI_RX0_PRST           75
#define TS_MIPI_RX1_PRST           76
#define TS_MIPI_PHY_PRST           77
#define TS_ISP_SRST                78
#define TS_ISP_ARST                79
#define TS_ISP_HRST                80
#define TS_VPE_CRST                81
#define TS_VPE_ARST                82
#define TS_VPE_HRST                83
#define TS_VPE_ISP_RST             84
#define TS_AMR_RST                 85
#define TS_RNE_CRST                88
#define TS_RNE_ARST                89
#define TS_RNE_HRST                90
#define TS_VPU_ARST                91
#define TS_VPU_HRST                92
#define TS_H265_CCLK_RST           93
#define TS_H264_CCLK_RST           94
#define TS_RST_MAX                 95

#endif
