/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Tsingmicro SoC Tx536 Firmware layer
 *
 *  Copyright (C) 2022-2022 Tsingmicro, Inc.
 *
 */
#ifndef _DT_BINDINGS_CLK_TS_TX536_H
#define _DT_BINDINGS_CLK_TS_TX536_H

/* clock id for dts */
#define TS_CLK_TX536_OSC_24M				0
#define TS_CLK_TX536_NONE					0
#define TS_CLK_TX536_CPU_PLL				1
#define TS_CLK_TX536_BUS_PLL				2
#define TS_CLK_TX536_DDR_PLL				3
#define TS_CLK_TX536_DISP_PLL				4
#define TS_CLK_TX536_AUD_PLL				5
#define TS_CLK_TX536_AI_PLL					6
#define TS_CLK_TX536_MCU_PLL				7

#define TS_CLK_TX536_CPU_PLL_D2				10
#define TS_CLK_TX536_CPU_PLL_D4				11
#define TS_CLK_TX536_CPU_PLL_D6				12
#define TS_CLK_TX536_CPU_PLL_D8				13
#define TS_CLK_TX536_CPU_PLL_D24			14
#define TS_CLK_TX536_CPU_PLL_D32			15 //50M
#define TS_CLK_TX536_CPU_PLL_D64			16 //25M
#define TS_CLK_TX536_BUS_PLL_D2				17
#define TS_CLK_TX536_BUS_PLL_D3				18
#define TS_CLK_TX536_BUS_PLL_D4				19
#define TS_CLK_TX536_BUS_PLL_D6				20
#define TS_CLK_TX536_BUS_PLL_D8				21
#define TS_CLK_TX536_DISP_PLL_D2			22
#define TS_CLK_TX536_DISP_PLL_D4			23
#define TS_CLK_TX536_DISP_PLL_D8			24
#define TS_CLK_TX536_DISP_PLL_D16			25 //74.25M
#define TS_CLK_TX536_DISP_PLL_D32			26 //37.125M
#define TS_CLK_TX536_MCU_PLL_D2				27 //648M
#define TS_CLK_TX536_MCU_PLL_D3				28 //532M
#define TS_CLK_TX536_MCU_PLL_D4				29 //324M
#define TS_CLK_TX536_MCU_PLL_D6				30 //216M
#define TS_CLK_TX536_MCU_PLL_D8				31 //162M
#define TS_CLK_TX536_MCU_PLL_D18			32 //72M
#define TS_CLK_TX536_MCU_PLL_D24			33 //54M
#define TS_CLK_TX536_MCU_PLL_D36			34 //36M
#define TS_CLK_TX536_MCU_PLL_D48			35 //27M
#define TS_CLK_TX536_MCU_PLL_D54			36 //24M
#define TS_CLK_TX536_MCU_PLL_D108			37 //12M
#define TS_CLK_TX536_BUS_PLL_D12			38 //166.7M
#define TS_CLK_TX536_BUS_PLL_D16			39

#define TS_CLK_TX536_CPU_PLL_D16			40 //100M

#define TS_CLK_TX536_A53_CLKIN				50 //comp without gate
#define TS_CLK_TX536_A53_ACLKM				51 //divider
#define TS_CLK_TX536_A53_DBG_PCLK			52 //divider
#define TS_CLK_TX536_A53_ATCLK				53 //divider
#define TS_CLK_TX536_A53_CNT_CLK			54 //divider
#define TS_CLK_TX536_DMAC_1_ACP_ACLK		55 //divider

#define TS_CLK_TX536_CX400_ATCLK			56 //comp
#define TS_CLK_TX536_CX400_DAP_CLK			57 //comp
#define TS_CLK_TX536_CX400_TSCLK			58 //gate
#define TS_CLK_TX536_CX400_TRACE_CLK		59 //gate, no mux found in register
#define TS_CLK_TX536_CX400_DP_CLK			60 //comp without div

#define TS_CLK_TX536_TOP_NOC_CLK			61 //comp without gate
#define TS_CLK_TX536_TOP_BUS_HCLK			62 //divider
#define TS_CLK_TX536_TOP_BUS_PCLK			63 //divider

#define TS_CLK_TX536_DMAC_0_ACLK			64 //gate
#define TS_CLK_TX536_DMAC_1_ACLK			65 //gate
#define TS_CLK_TX536_BOOTROM_HCLK			66 //gate
#define TS_CLK_TX536_MSHC_0_HCLK			67 //comp
#define TS_CLK_TX536_MSHC_1_HCLK			68 //comp
#define TS_CLK_TX536_GMAC_BUS_CLK			69 //gate
#define TS_CLK_TX536_MMU_ACLK				70 //gate
#define TS_CLK_TX536_GIC400_ACLK			71 //gate
#define TS_CLK_TX536_CRYPTO_BUS_CLK			72 //comp
#define TS_CLK_TX536_USB3C_BUS_CLK			73 //comp

#define TS_CLK_TX536_BT1120_TX_BUS_CLK		74 //gate
#define TS_CLK_TX536_BT1120_RX_BUS_CLK		75 //gate
#define TS_CLK_TX536_OSPI_BUS_CLK			76 //gate
#define TS_CLK_TX536_NFC_HCLK				77 //gate

#define TS_CLK_TX536_GDC_ACLK				78 //comp
#define TS_CLK_TX536_GDC_PCLK				79 //gate

#define TS_CLK_TX536_CVE_ACLK				80 //comp
#define TS_CLK_TX536_CVE_HCLK				81 //gate
#define TS_CLK_TX536_GPU_ACLK				82 //comp
#define TS_CLK_TX536_GPU_PCLK				83 //gate

#define TS_CLK_TX536_TOP_WDT_PCLK			84 //gate
#define TS_CLK_TX536_SEC_WDT_PCLK			85 //gate
#define TS_CLK_TX536_TOP_TMR_PCLK			86 //gate
#define TS_CLK_TX536_SEC_TMR_PCLK			87 //gate
#define TS_CLK_TX536_TOP_GPIO_PCLK			88 //gate
#define TS_CLK_TX536_OTPC_PCLK				89 //gate
#define TS_CLK_TX536_ADC_PCLK				90 //gate
#define TS_CLK_TX536_TOP_PADC_PCLK			91 //gate
#define TS_CLK_TX536_MAILBOX_PCLK			92 //gate

#define TS_CLK_TX536_SPI_0_PCLK				93 //gate
#define TS_CLK_TX536_SPI_1_PCLK				94 //gate
#define TS_CLK_TX536_SPI_2_PCLK				95 //gate
#define TS_CLK_TX536_SPI_3_PCLK				96 //gate
#define TS_CLK_TX536_SPI_4_PCLK				97 //gate
#define TS_CLK_TX536_I2S_0_PCLK				98 //gate
#define TS_CLK_TX536_I2S_1_PCLK				99 //gate
#define TS_CLK_TX536_UART_1_PCLK			100 //gate
#define TS_CLK_TX536_UART_2_PCLK			101 //gate
#define TS_CLK_TX536_UART_3_PCLK			102 //gate
#define TS_CLK_TX536_UART_4_PCLK			103 //gate
#define TS_CLK_TX536_UART_5_PCLK			104 //gate
#define TS_CLK_TX536_UART_6_PCLK			105 //gate
#define TS_CLK_TX536_USI_2_PCLK				106 //gate
#define TS_CLK_TX536_USI_3_PCLK				107 //gate
#define TS_CLK_TX536_I2C_2_PCLK				108 //gate
#define TS_CLK_TX536_I2C_3_PCLK				109 //gate
#define TS_CLK_TX536_I2C_4_PCLK				110 //gate
#define TS_CLK_TX536_I2C_5_PCLK				111 //gate
#define TS_CLK_TX536_I2C_6_PCLK				112 //gate
#define TS_CLK_TX536_I2C_7_PCLK				113 //gate
#define TS_CLK_TX536_I2C_8_PCLK				114 //gate
#define TS_CLK_TX536_I2C_9_PCLK				115 //gate

#define TS_CLK_TX536_GMAC_ACLK				116 //comp without gate
#define TS_CLK_TX536_MSHC_0_BCLK			117 //comp
#define TS_CLK_TX536_MSHC_1_BCLK			118 //comp
#define TS_CLK_TX536_USB_PHY_REFCLK			119 //comp without divider
#define TS_CLK_TX536_USB_CTL_REFCLK			120 //comp without divider
#define TS_CLK_TX536_NFC_CLKB				121 //comp without gate

#define TS_CLK_TX536_GDC_MCLK				122 //divider
#define TS_CLK_TX536_BT1120TX_ACLK			123 //comp without gate
#define TS_CLK_TX536_BT1120RX_ACLK			124 //comp without gate
#define TS_CLK_TX536_BT1120TX_HCLK			125 //divider
#define TS_CLK_TX536_OSPI_HCLK				126 //comp without gate
#define TS_CLK_TX536_OSPI_REF_CLK			127 //comp without gate

#define TS_CLK_TX536_CVE_CCLK				128 //comp without mux
#define TS_CLK_TX536_GPU_CCLK				129 //comp without mux
#define TS_CLK_TX536_I2S0_OCLK				130 //comp without mux
#define TS_CLK_TX536_I2S1_OCLK				131 //comp without mux
#define TS_CLK_TX536_I2S0_MCLK				132 //comp without mux
#define TS_CLK_TX536_I2S1_MCLK				133 //comp without mux
#define TS_CLK_TX536_I2C_2_ICCLK			134 //comp without gate
#define TS_CLK_TX536_I2C_3_ICCLK			135 //comp without gate
#define TS_CLK_TX536_I2C_4_ICCLK			136 //comp without gate
#define TS_CLK_TX536_I2C_5_ICCLK			137 //comp without gate
#define TS_CLK_TX536_I2C_6_ICCLK			138 //comp without gate
#define TS_CLK_TX536_I2C_7_ICCLK			139 //comp without gate
#define TS_CLK_TX536_I2C_8_ICCLK			140 //comp without gate
#define TS_CLK_TX536_I2C_9_ICCLK			141 //comp without gate
#define TS_CLK_TX536_UART_1_SCLK			142 //comp without gate
#define TS_CLK_TX536_UART_2_SCLK			143 //comp without gate
#define TS_CLK_TX536_UART_3_SCLK			144 //comp without gate
#define TS_CLK_TX536_UART_4_SCLK			145 //comp without gate
#define TS_CLK_TX536_UART_5_SCLK			146 //comp without gate
#define TS_CLK_TX536_UART_6_SCLK			147 //comp without gate
#define TS_CLK_TX536_SPI_0_SSICLK			148 //comp without gate
#define TS_CLK_TX536_SPI_1_SSICLK			149 //comp without gate
#define TS_CLK_TX536_SPI_2_SSICLK			150 //comp without gate
#define TS_CLK_TX536_SPI_3_SSICLK			151 //comp without gate
#define TS_CLK_TX536_SPI_4_SSICLK			152 //comp without gate

#define TS_CLK_TX536_TOP_TMR_T3CLK			153 //gate, parent unknown
#define TS_CLK_TX536_TOP_TMR_T4CLK			154 //gate
#define TS_CLK_TX536_TOP_TMR_T5CLK			155 //comp without mux
#define TS_CLK_TX536_TOP_TMR_T6CLK			156 //comp without mux
#define TS_CLK_TX536_TOP_TMR_T7CLK			157 //comp without mux
#define TS_CLK_TX536_TOP_TMR_T8CLK			158 //comp without mux
#define TS_CLK_TX536_SEC_TMR_T2CLK			159 //comp without mux
#define TS_CLK_TX536_SEC_TMR_T3CLK			160 //comp without mux
#define TS_CLK_TX536_CHIP_OCLK_0			161 //comp without divider
#define TS_CLK_TX536_CHIP_OCLK_1			162 //comp without divider
#define TS_CLK_TX536_CHIP_OCLK_2			163 //comp without divider
#define TS_CLK_TX536_CHIP_OCLK_3			164 //comp without divider

#define TS_CLK_TX536_MII_RXCLK				165 //gate from eth phy
#define TS_CLK_TX536_MII_TXCLK				166 //gate from eth phy
#define TS_CLK_TX536_MAC_CLK_RMII			167 //gate, parent unknown
#define TS_CLK_TX536_RMII_TX_CLK			168 //gate out to mac & eth phy
#define TS_CLK_TX536_GMII_TX_CLK			169 //gate out
#define TS_CLK_TX536_PTP_REF_CLK			170 //comp without mux, not set
#define TS_CLK_TX536_EPHY_REF_CLK			171 //comp without mux, not set


#define TS_CLK_TX536_VO_BUS_CLK				180 //comp without mux
#define TS_CLK_TX536_DP650_PCLK				181 //gate
#define TS_CLK_TX536_DMA2MIPI_PCLK			182 //gate
#define TS_CLK_TX536_CSI_DEV_PCLK			183 //gate
#define TS_CLK_TX536_DSI_HOST_PCLK			184 //gate
#define TS_CLK_TX536_HDMI_TX_PCLK			185 //gate
#define TS_CLK_TX536_DP650_ACLK				186 //comp without mux
#define TS_CLK_TX536_DMA2MIPI_ACLK			187 //comp without mux
#define TS_CLK_TX536_NEMADC_HCLK			188 //comp without mux
#define TS_CLK_TX536_HDMI_TX_HCLK			189 //comp without mux

#define TS_CLK_TX536_NEMADC_CCLK			190 //comp without mux
#define TS_CLK_TX536_DMA2MIPI_CCLK			191 //comp without mux
#define TS_CLK_TX536_DP650_MCLK				192 //comp without mux
#define TS_CLK_TX536_DP650_PXL_CLK			193 //comp without mux
#define TS_CLK_TX536_DSI_HOST_IPI_CLK		194 //gate
#define TS_CLK_TX536_MIPI_TXPHY_REF_CLK		195 //comp without div
#define TS_CLK_TX536_MIPI_TXPHY_CFG_CLK		196 //comp without div
#define TS_CLK_TX536_HDMI_TX_PXL_CLK		197 //gate
#define TS_CLK_TX536_HDMI_TX_CFG_CLK		198 //gate

#define TS_CLK_TX536_MCTL_P0_ACLK			199 //comp without mux
#define TS_CLK_TX536_MCTL_P1_ACLK			200 //comp without mux
#define TS_CLK_TX536_MCTL_P2_ACLK			201 //comp without mux
#define TS_CLK_TX536_MCTL_P3_ACLK			202 //comp without mux
#define TS_CLK_TX536_MCTL_P4_ACLK			203 //comp without mux
#define TS_CLK_TX536_MCTL_P5_ACLK			204 //comp without mux
#define TS_CLK_TX536_MCTL_PCLK				205 //gate
#define TS_CLK_TX536_MCTL_DDRC_CLK			206 //gate

#define TS_CLK_TX536_VI_BUS_PCLK			220 //comp without mux
#define TS_CLK_TX536_CSI_HOST_0_PCLK		221 //gate
#define TS_CLK_TX536_CSI_HOST_1_PCLK		222 //gate
#define TS_CLK_TX536_CSI_HOST_2_PCLK		223 //gate
#define TS_CLK_TX536_VPE_PCLK				224 //gate
#define TS_CLK_TX536_HDR_PCLK				225 //gate

#define TS_CLK_TX536_ISP_SCLK				226 //comp
#define TS_CLK_TX536_CSI_HOST_0_PXLCLK0		227 //gate
#define TS_CLK_TX536_CSI_HOST_0_PXLCLK1		228 //gate
#define TS_CLK_TX536_CSI_HOST_0_PXLCLK2		229 //gate
#define TS_CLK_TX536_CSI_HOST_1_PXLCLK0		230 //gate
#define TS_CLK_TX536_CSI_HOST_2_PXLCLK0		231 //gate
#define TS_CLK_TX536_TNR_CCLK				232 //gate
#define TS_CLK_TX536_VPE_CCLK				233 //comp without mux
#define TS_CLK_TX536_MIPI_RXPHY0_CFGCLK		234 //comp without div
#define TS_CLK_TX536_MIPI_RXPHY1_CFGCLK		235 //comp without div
#define TS_CLK_TX536_MIPI_RXPHY2_CFGCLK		236 //comp without div
#define TS_CLK_TX536_ISP_ACLK				237 //comp without mux
#define TS_CLK_TX536_TNR_HCLK				238 //gate
#define TS_CLK_TX536_HDR_ACLK				239 //gate
#define TS_CLK_TX536_VPE_ACLK				240 //comp without mux
#define TS_CLK_TX536_TNR_ACLK				241 //gate

#define TS_CLK_TX536_WAVE_ACLK				250 //comp without mux
#define TS_CLK_TX536_CODA_ACLK				251 //comp without mux
#define TS_CLK_TX536_VPU_CFG_PCLK			252 //comp without mux
#define TS_CLK_TX536_WAVE_PCLK				253 //gate
#define TS_CLK_TX536_CODA_PCLK				254 //gate
#define TS_CLK_TX536_WAVE_CCLK				255 //comp without mux
#define TS_CLK_TX536_WAVE_BCLK				256 //comp without mux
#define TS_CLK_TX536_CODA_CCLK				257 //comp without mux


#define TS_CLK_TX536_AISYS_HCLK				280 //div
#define TS_CLK_TX536_RNE_HCLK				281 //gate
#define TS_CLK_TX536_RCE_HCLK				282 //gate
#define TS_CLK_TX536_AI_ISP_HCLK			283 //gate
#define TS_CLK_TX536_RNE_ACLK				284 //comp without mux
#define TS_CLK_TX536_RCE_ACLK				285 //comp without mux
#define TS_CLK_TX536_AI_MEM_ACLK			286 //comp


#define TS_CLK_TX536_RCE_CCLK				300 //comp without mux
#define TS_CLK_TX536_RCE_MEM_CLK			301 //gate, no parent found
#define TS_CLK_TX536_RNE_CCLK				302 //comp without mux
#define TS_CLK_TX536_RNE_MEM_CLK			303 //gate, no parent found
#define TS_CLK_TX536_AI_ISP_CCLK			304 //gate, not set

#define TS_CLK_TX536_MCU_CLK_START			500
#define TS_CLK_TX536_MCU_NOC_CLK			500 //comp without gate
#define TS_CLK_TX536_MCU_BUS_PCLK			501 //div
#define TS_CLK_TX536_MCU_DDR_ACLK			502 //comp
#define TS_CLK_TX536_PWM_PCLK				503 //gate
#define TS_CLK_TX536_MCU_PADC_PCLK			504 //gate
#define TS_CLK_TX536_MCU_TMR_PCLK			505 //gate
#define TS_CLK_TX536_MCU_WDT_PCLK			506 //gate
#define TS_CLK_TX536_UART_0_PCLK			507 //gate
#define TS_CLK_TX536_I2C_0_PCLK				508 //gate
#define TS_CLK_TX536_I2C_1_PCLK				509 //gate
#define TS_CLK_TX536_USI_0_PCLK				510 //gate
#define TS_CLK_TX536_USI_1_PCLK				511 //gate
#define TS_CLK_TX536_MCU_GPIO_PCLK			512 //gate

#define TS_CLK_TX536_MCU_RAM_ACLK			513 //gate
#define TS_CLK_TX536_DVP_PCLK				514 //gate
#define TS_CLK_TX536_DVP_ACLK				515 //comp without mux
#define TS_CLK_TX536_SYS_REG_PCLK			516 //gate
#define TS_CLK_TX536_MCU_RNE_ACLK			517 //gate ?? duplicate with 284
#define TS_CLK_TX536_DMAC2_HCLK				518 //gate
#define TS_CLK_TX536_AUDIO_HCLK				519 //gate
#define TS_CLK_TX536_AUDIO_PCLK				520 //comp without div

#define TS_CLK_TX536_DVP_MCLK				521 //comp without mux
#define TS_CLK_TX536_AUDIO_MCLK				522 //comp without div
#define TS_CLK_TX536_UART_0_SCLK			523 //comp without gate
#define TS_CLK_TX536_I2C_0_ICCLK			524 //comp without gate
#define TS_CLK_TX536_I2C_1_ICCLK			525 //comp without gate

#define TS_CLK_TX536_TMR_T2CLK				526 //comp whithout mux
#define TS_CLK_TX536_TMR_T3CLK				527 //comp whithout mux
#define TS_CLK_TX536_TMR_T4CLK				528 //comp whithout mux
#define TS_CLK_TX536_MCU_CLK24M				529 //gate
#define TS_CLK_TX536_CHIP_OCLK_4			530 //comp without div
#define TS_CLK_TX536_MAX					640
#endif
