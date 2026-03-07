/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Tsingmicro SoC tx5112 Firmware layer
 *
 *  Copyright (C) 2022-2022 Tsingmicro, Inc.
 *
 */
#ifndef _DT_BINDINGS_CLK_TS_TX5112_H
#define _DT_BINDINGS_CLK_TS_TX5112_H

/* clock id for dts */
#define TS_CLK_TX5112_OSC_24M					0
#define TS_CLK_TX5112_NONE					0
#define TS_CLK_TX5112_PLL0					1  //2GHz
#define TS_CLK_TX5112_PLL1					2  //1228.8MHz
#define TS_CLK_TX5112_PLL2					3  //1228.8MHz

#define TS_CLK_TX5112_PLL0_D2					10  //1GHz
#define TS_CLK_TX5112_PLL0_D4					11  //500MHz
#define TS_CLK_TX5112_PLL0_D5					12  //400MHz
#define TS_CLK_TX5112_PLL0_D8					13  //250MHz
#define TS_CLK_TX5112_PLL0_D10				14  //200MHz
#define TS_CLK_TX5112_PLL0_D27				15  //74.074MHz
#define TS_CLK_TX5112_PLL1_D3					16  //409.6MHz
#define TS_CLK_TX5112_PLL1_D4					17  //307.2MHz
#define TS_CLK_TX5112_PLL1_D6					18  //204.8MHz
#define TS_CLK_TX5112_PLL1_D8					19  //153.6MHz





#define TS_CLK_TX5112_CPU_CCLK				20  //comp without gate
#define TS_CLK_TX5112_CPU_ACLK				21  //divider
#define TS_CLK_TX5112_CPU_DBG_PCLK			22  //divider

#define TS_CLK_TX5112_AXI_AHB_APB_CLK				23 //mux axi/ahb/apb

#define TS_CLK_TX5112_CORE_APB_CLK			24  //divider
#define TS_CLK_TX5112_CORE_AHB_CLK			25  //divider
#define TS_CLK_TX5112_CORE_AXI_CLK			26  //divider


#define TS_CLK_TX5112_PLL0_D40				27  //50MHz


//AHB ENBALE
#define TS_CLK_TX5112_SKE_CLK					30  //gate
#define TS_CLK_TX5112_OCRAM_ACLK				31  //gate
#define TS_CLK_TX5112_MEM_DMA_ACLK			32  //gate
#define TS_CLK_TX5112_AUD_CODEC_HCLK			33  //gate
#define TS_CLK_TX5112_OSPI_HCLK				34  //gate
#define TS_CLK_TX5112_GMAC_HCLK				35  //gate
#define TS_CLK_TX5112_USB2C_HCLK				36  //gate
#define TS_CLK_TX5112_SDHC1_HCLK				37  //gate
#define TS_CLK_TX5112_SDHC0_HCLK				38  //gate
#define TS_CLK_TX5112_PERI_DMA1_HCLK			39  //gate
#define TS_CLK_TX5112_PERI_DMA0_HCLK			40  //gate
#define TS_CLK_TX5112_BOOTROM_HCLK			41  //gate

//APB ENABLE
#define TS_CLK_TX5112_GPIO_B_PCLK				50  //gate
#define TS_CLK_TX5112_GPIO_A_PCLK				51  //gate
#define TS_CLK_TX5112_PDM_PCLK				52  //gate
#define TS_CLK_TX5112_SYS_REG_PCLK			53  //gate
#define TS_CLK_TX5112_OTPC_PCLK				54  //gate
#define TS_CLK_TX5112_OSPI_PCLK				55  //gate
#define TS_CLK_TX5112_PWM_PCLK				56  //gate
#define TS_CLK_TX5112_TMR_PCLK				57  //gate
#define TS_CLK_TX5112_WDT_PCLK				58  //gate
#define TS_CLK_TX5112_GPIO_PCLK				59  //gate
#define TS_CLK_TX5112_ADC_PCLK				60  //gate
#define TS_CLK_TX5112_I2S0_PCLK				61  //gate
#define TS_CLK_TX5112_USI1_PCLK				62  //gate
#define TS_CLK_TX5112_USI0_PCLK				63  //gate
#define TS_CLK_TX5112_UART1_PCLK				64  //gate
#define TS_CLK_TX5112_UART0_PCLK				65  //gate

#define TS_CLK_TX5112_SD1_CLK_SMP_SHIFT_ALIEND	70
#define TS_CLK_TX5112_SD1_CLK_SMP_SHIFT_1_10		71
#define TS_CLK_TX5112_SD1_CLK_SMP_SHIFT_2_10		72
#define TS_CLK_TX5112_SD1_CLK_SMP_SHIFT_3_10		73
#define TS_CLK_TX5112_SD1_CLK_SMP_SHIFT_4_10		74
#define TS_CLK_TX5112_SD1_CLK_SMP_SHIFT_5_10		75
#define TS_CLK_TX5112_SD1_CLK_SMP_SHIFT_6_10		76
#define TS_CLK_TX5112_SD1_CLK_SMP_SHIFT_7_10		77
#define TS_CLK_TX5112_SD1_CLK_SMP_SHIFT_8_10		78
#define TS_CLK_TX5112_SD1_CLK_SMP_SHIFT_9_10		79

#define TS_CLK_TX5112_SD1_CCLK_DRV_SHIFT_1_10		80
#define TS_CLK_TX5112_SD1_CCLK_DRV_SHIFT_2_10		81
#define TS_CLK_TX5112_SD1_CCLK_DRV_SHIFT_3_10		82
#define TS_CLK_TX5112_SD1_CCLK_DRV_SHIFT_4_10		83
#define TS_CLK_TX5112_SD1_CCLK_DRV_SHIFT_5_10		84
#define TS_CLK_TX5112_SD1_CCLK_DRV_SHIFT_6_10		85
#define TS_CLK_TX5112_SD1_CCLK_DRV_SHIFT_7_10		86
#define TS_CLK_TX5112_SD1_CCLK_DRV_SHIFT_8_10		87
#define TS_CLK_TX5112_SD1_CCLK_DRV_SHIFT_9_10		88

#define TS_CLK_TX5112_SD0_CLK_SMP_SHIFT_ALIEND	90
#define TS_CLK_TX5112_SD0_CLK_SMP_SHIFT_1_10		91
#define TS_CLK_TX5112_SD0_CLK_SMP_SHIFT_2_10		92
#define TS_CLK_TX5112_SD0_CLK_SMP_SHIFT_3_10		93
#define TS_CLK_TX5112_SD0_CLK_SMP_SHIFT_4_10		94
#define TS_CLK_TX5112_SD0_CLK_SMP_SHIFT_5_10		95
#define TS_CLK_TX5112_SD0_CLK_SMP_SHIFT_6_10		96
#define TS_CLK_TX5112_SD0_CLK_SMP_SHIFT_7_10		97
#define TS_CLK_TX5112_SD0_CLK_SMP_SHIFT_8_10		98
#define TS_CLK_TX5112_SD0_CLK_SMP_SHIFT_9_10		99

#define TS_CLK_TX5112_SD0_CCLK_DRV_SHIFT_1_10		100
#define TS_CLK_TX5112_SD0_CCLK_DRV_SHIFT_2_10		101
#define TS_CLK_TX5112_SD0_CCLK_DRV_SHIFT_3_10		102
#define TS_CLK_TX5112_SD0_CCLK_DRV_SHIFT_4_10		103
#define TS_CLK_TX5112_SD0_CCLK_DRV_SHIFT_5_10		104
#define TS_CLK_TX5112_SD0_CCLK_DRV_SHIFT_6_10		105
#define TS_CLK_TX5112_SD0_CCLK_DRV_SHIFT_7_10		106
#define TS_CLK_TX5112_SD0_CCLK_DRV_SHIFT_8_10		107
#define TS_CLK_TX5112_SD0_CCLK_DRV_SHIFT_9_10		108

#define TS_CLK_TX5112_SD0_CCLK				110  //comp without gate
#define TS_CLK_TX5112_SD0_CCLK_SMPL			112  //gate
#define TS_CLK_TX5112_SD0_CCLK_DRV			113  //gate
#define TS_CLK_TX5112_SD0_CCLK_SMP_EDGE		114  //gate
#define TS_CLK_TX5112_SD0_CCLK_DRV_EDGE		115  //gate
#define TS_CLK_TX5112_SD0_CCLK_SMP_PH		116  //mux
#define TS_CLK_TX5112_SD0_CCLK_DRV_PH		117  //mux

#define TS_CLK_TX5112_SD1_CCLK				120  //comp without gate
#define TS_CLK_TX5112_SD1_CCLK_SMPL			121  //gate
#define TS_CLK_TX5112_SD1_CCLK_DRV			122  //gate
#define TS_CLK_TX5112_SD1_CCLK_SMP_EDGE		123  //gate
#define TS_CLK_TX5112_SD1_CCLK_DRV_EDGE		124  //gate
#define TS_CLK_TX5112_SD1_CCLK_SMP_PH		125  //mux
#define TS_CLK_TX5112_SD1_CCLK_DRV_PH		126  //mux


#define TS_CLK_TX5112_OSPI_REF_CLK			130  //comp
#define TS_CLK_TX5112_USB_REF_CLK				131  //comp without div

#define TS_CLK_TX5112_I2C1_ICCLK				132  //comp without gate
#define TS_CLK_TX5112_I2C0_ICCLK				133  //comp without gate
#define TS_CLK_TX5112_I2C1_PCLK				134  //gate
#define TS_CLK_TX5112_I2C0_PCLK				135  //gate
#define TS_CLK_TX5112_I2C2_ICCLK				136  //comp without gate
#define TS_CLK_TX5112_I2C2_PCLK				137  //gate

#define TS_CLK_TX5112_UART0_SCLK				138  //div
#define TS_CLK_TX5112_UART1_SCLK				139  //div
#define TS_CLK_TX5112_SPI_SSICLK				140  //comp without gate
#define TS_CLK_TX5112_SPI_PCLK				141  //gate
#define TS_CLK_TX5112_TMR_T4CLK				142  //comp without mux
#define TS_CLK_TX5112_TMR_T3CLK				143  //comp without mux
#define TS_CLK_TX5112_TMR_T6CLK				144  //comp without mux
#define TS_CLK_TX5112_TMR_T5CLK				145  //comp without mux
#define TS_CLK_TX5112_TMR_T8CLK				146  //comp without mux
#define TS_CLK_TX5112_TMR_T7CLK				147  //comp without mux

#define TS_CLK_TX5112_I2S0_MCLK				150  //comp without mux
#define TS_CLK_TX5112_I2S0_OCLK				151  //comp without mux
#define TS_CLK_TX5112_I2S0_OCLK_O				152  //gate
#define TS_CLK_TX5112_PDM_MCLK				153  //comp without mux
#define TS_CLK_TX5112_AUD_DAC_PBCLK			154  //comp without mux
#define TS_CLK_TX5112_AUD_DAC_CCLK			155  //comp without mux
#define TS_CLK_TX5112_AUD_DAC_PBCLK_INV		156  //gate
#define TS_CLK_TX5112_ED_TMR_STRB				157  //div
#define TS_CLK_TX5112_AUD_TMR_STRB			158  //gate
#define TS_CLK_TX5112_AUD_ADC_CCLK			159  //comp without mux

#define TS_CLK_TX5112_MCTL_P3_ACLK			160  //comp without mux
#define TS_CLK_TX5112_MCTL_P2_ACLK			161  //comp without mux
#define TS_CLK_TX5112_MCTL_P1_ACLK			162  //comp without mux
#define TS_CLK_TX5112_MCTL_P0_ACLK			163  //comp without mux

#define TS_CLK_TX5112_VI_DDR_ACLK				164  //comp without mux
#define TS_CLK_TX5112_DDRC_CORE_CLK			165  //gate
#define TS_CLK_TX5112_DDR_PHY_PCLK			166  //gate
#define TS_CLK_TX5112_UMCTL_PCLK				167  //gate
#define TS_CLK_TX5112_AI_DDR_ACLK               168  //comp without mux

#define TS_CLK_TX5112_HDR_SCLK				170  //gate
#define TS_CLK_TX5112_VPE_ISP_CLK				171  //gate
#define TS_CLK_TX5112_MIPI_RX1_PIXCLK			172  //gate
#define TS_CLK_TX5112_MIPI_RX0_PIXCLK1		173  //gate
#define TS_CLK_TX5112_MIPI_RX0_PIXCLK0		174  //gate
#define TS_CLK_TX5112_MIPI_RX1_PCLK			175  //gate
#define TS_CLK_TX5112_MIPI_RX0_PCLK			176  //gate
#define TS_CLK_TX5112_HDR_HCLK				177  //gate
#define TS_CLK_TX5112_VPE_HCLK				178  //gate
#define TS_CLK_TX5112_MIPI_PHY_PCLK 		179  //gate
#define TS_CLK_TX5112_VI_CFG_HCLK				180  //gate
#define TS_CLK_TX5112_HDR_ACLK				181  //gate
#define TS_CLK_TX5112_VPE_ACLK				182  //gate
#define TS_CLK_TX5112_ISP_ACLK				183  //gate
#define TS_CLK_TX5112_ISP_SCLK				184  //comp without mux
#define TS_CLK_TX5112_VPE_CCLK				185  //comp without mux
#define TS_CLK_TX5112_MIPI_TXCLKESC			186  //comp without mux

#define TS_CLK_TX5112_AMR_CCLK				190  //comp without mux
#define TS_CLK_TX5112_AMR_HCLK				191  //gate
#define TS_CLK_TX5112_AMR_ACLK				192  //gate
#define TS_CLK_TX5112_RNE_ACLK				193  //comp without mux
#define TS_CLK_TX5112_RNE_CCLK				194  //comp
#define TS_CLK_TX5112_AI_ACLK				196  //gate
#define TS_CLK_TX5112_RNE_HCLK				197  //gate

#define TS_CLK_TX5112_H265_CCLK				200  //comp
#define TS_CLK_TX5112_VPU_ACLK				201  //div
#define TS_CLK_TX5112_VPU_HCLK				202  //gate
#define TS_CLK_TX5112_H265_PCLK				203  //gate
#define TS_CLK_TX5112_H265_ACLK				204  //gate
#define TS_CLK_TX5112_VPU_PCLK				205  //gate
#define TS_CLK_TX5112_H264_ACLK				206  //gate

#define TS_CLK_TX5112_CHIP_OCLK_I3			207
#define TS_CLK_TX5112_CHIP_OCLK_I2			208
#define TS_CLK_TX5112_CHIP_OCLK_I1			209
#define TS_CLK_TX5112_CHIP_OCLK_I0			210

#define TS_CLK_TX5112_MAX                   300
#endif
