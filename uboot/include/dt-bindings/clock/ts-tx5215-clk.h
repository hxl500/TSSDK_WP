/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Tsingmicro SoC tx5215 Firmware layer
 *
 *  Copyright (C) 2022-2022 Tsingmicro, Inc.
 *
 */
#ifndef _DT_BINDINGS_CLK_TS_TX5215_H
#define _DT_BINDINGS_CLK_TS_TX5215_H

/* clock id for dts */
#define TS_CLK_TX5215_OSC_24M					0
#define TS_CLK_TX5215_NONE					0
#define TS_CLK_TX5215_PLL0					1  //2GHz
#define TS_CLK_TX5215_PLL1					2  //1228.8MHz
#define TS_CLK_TX5215_PLL2					3

#define TS_CLK_TX5215_PLL0_D2					10  //1GHz
#define TS_CLK_TX5215_PLL0_D4					11  //500MHz
#define TS_CLK_TX5215_PLL0_D5					12  //400MHz
#define TS_CLK_TX5215_PLL0_D8					13  //250MHz
#define TS_CLK_TX5215_PLL0_D10					14  //200MHz
#define TS_CLK_TX5215_PLL1_D3					15  //409.6MHz
#define TS_CLK_TX5215_PLL1_D4					16  //307.2MHz
#define TS_CLK_TX5215_PLL1_D6					17  //204.8MHz
#define TS_CLK_TX5215_PLL0_D40					18  //50MHz


#define TS_CLK_TX5215_CPU_CCLK				50  //comp without gate
#define TS_CLK_TX5215_CPU_JTAG_CLK			51  //comp without gate
#define TS_CLK_TX5215_CPU_ACLK				52  //divider
#define TS_CLK_TX5215_CPU_DBG_PCLK			53  //divider

#define TS_CLK_TX5215_TOP_AHB_CLK				54  //comp without gate
#define TS_CLK_TX5215_TOP_AXI_CLK				55  //comp without gate
#define TS_CLK_TX5215_PERI_APB_CLK			56  //divider

#define TS_CLK_TX5215_GMAC_RCLK				60  //mux
#define TS_CLK_TX5215_GMAC_ACLK				61  //comp

#define TS_CLK_TX5215_TRNG_CLK				70  //gate
#define TS_CLK_TX5215_PKE_CLK					71  //gate
#define TS_CLK_TX5215_HASH_CLK				72  //gate
#define TS_CLK_TX5215_SPACC_CLK				73  //gate
#define TS_CLK_TX5215_SKE_CLK					74  //gate
#define TS_CLK_TX5215_OCRAM_ACLK				75  //gate
#define TS_CLK_TX5215_MEM_DMA_ACLK			76  //gate
#define TS_CLK_TX5215_AUD_CODEC_HCLK			77  //gate
#define TS_CLK_TX5215_OSPI_HCLK				78  //gate
#define TS_CLK_TX5215_GMAC_HCLK				79  //gate
#define TS_CLK_TX5215_USB2C_HCLK				80  //gate
#define TS_CLK_TX5215_SDHC1_HCLK				81  //gate
#define TS_CLK_TX5215_SDHC0_HCLK				82  //gate
#define TS_CLK_TX5215_PERI_DMA1_HCLK			83  //gate
#define TS_CLK_TX5215_PERI_DMA0_HCLK			84  //gate
#define TS_CLK_TX5215_BOOTROM_HCLK			85  //gate

#define TS_CLK_TX5215_I2S0_PCLK				86  //gate
#define TS_CLK_TX5215_USI1_PCLK				87  //gate
#define TS_CLK_TX5215_USI0_PCLK				88  //gate
#define TS_CLK_TX5215_UART2_PCLK				89  //gate
#define TS_CLK_TX5215_UART1_PCLK				90  //gate
#define TS_CLK_TX5215_UART0_PCLK				91  //gate
#define TS_CLK_TX5215_I2C3_PCLK				92  //gate
#define TS_CLK_TX5215_I2C2_PCLK				93  //gate
#define TS_CLK_TX5215_I2C1_PCLK				94  //gate
#define TS_CLK_TX5215_I2C0_PCLK				95  //gate

#define TS_CLK_TX5215_GPIO_B_PCLK				96  //gate
#define TS_CLK_TX5215_GPIO_A_PCLK				97  //gate
#define TS_CLK_TX5215_PDM_PCLK				98  //gate
#define TS_CLK_TX5215_SPI_PCLK				99  //gate
#define TS_CLK_TX5215_SYS_REG_PCLK			100  //gate
#define TS_CLK_TX5215_OTPC_PCLK				101  //gate
#define TS_CLK_TX5215_OSPI_PCLK				102  //gate
#define TS_CLK_TX5215_PWM_PCLK				103  //gate
#define TS_CLK_TX5215_TMR_PCLK				104  //gate
#define TS_CLK_TX5215_WDT_PCLK				105  //gate
#define TS_CLK_TX5215_GPIO_PCLK				106  //gate
#define TS_CLK_TX5215_ADC_PCLK				107  //gate

#define TS_CLK_TX5215_SD0_CCLK_DIV1			110  //comp without gate
#define TS_CLK_TX5215_SD0_CCLK_DIV2			111  //comp without mux
#define TS_CLK_TX5215_SD0_CCLK_ALIEND			112  //fix clk with sd0_cclk
#define TS_CLK_TX5215_SD0_CCLK_SHIFT_1_10		113
#define TS_CLK_TX5215_SD0_CCLK_SHIFT_2_10		114
#define TS_CLK_TX5215_SD0_CCLK_SHIFT_3_10		115
#define TS_CLK_TX5215_SD0_CCLK_SHIFT_4_10		116
#define TS_CLK_TX5215_SD0_CCLK_SHIFT_5_10		117
#define TS_CLK_TX5215_SD0_CCLK_SHIFT_6_10		118
#define TS_CLK_TX5215_SD0_CCLK_SHIFT_7_10		119
#define TS_CLK_TX5215_SD0_CCLK_SHIFT_8_10		120
#define TS_CLK_TX5215_SD0_CCLK_SHIFT_9_10		121
#define TS_CLK_TX5215_SD0_CCLK_SMPL			122  //comp without divider
#define TS_CLK_TX5215_SD0_CCLK_DRV			123  //comp without divider

#define TS_CLK_TX5215_SD1_CCLK_DIV1			124  //comp without gate
#define TS_CLK_TX5215_SD1_CCLK_DIV2			125  //comp without mux
#define TS_CLK_TX5215_SD1_CCLK_ALIEND			126  //fix clk with sd1_cclk
#define TS_CLK_TX5215_SD1_CCLK_SHIFT_1_10		127
#define TS_CLK_TX5215_SD1_CCLK_SHIFT_2_10		128
#define TS_CLK_TX5215_SD1_CCLK_SHIFT_3_10		129
#define TS_CLK_TX5215_SD1_CCLK_SHIFT_4_10		130
#define TS_CLK_TX5215_SD1_CCLK_SHIFT_5_10		131
#define TS_CLK_TX5215_SD1_CCLK_SHIFT_6_10		132
#define TS_CLK_TX5215_SD1_CCLK_SHIFT_7_10		133
#define TS_CLK_TX5215_SD1_CCLK_SHIFT_8_10		134
#define TS_CLK_TX5215_SD1_CCLK_SHIFT_9_10		135
#define TS_CLK_TX5215_SD1_CCLK_SMPL			136  //comp without divider
#define TS_CLK_TX5215_SD1_CCLK_DRV			137  //comp without divider

#define TS_CLK_TX5215_OSPI_REF_CLK			138  //comp
#define TS_CLK_TX5215_USB_PHY_CLK				139  //comp without divider

#define TS_CLK_TX5215_EPHY_REF_CLK			140
#define TS_CLK_TX5215_PTP_REF_CLK				141
#define TS_CLK_TX5215_GMII_TX_CLK				142
#define TS_CLK_TX5215_RMII_TX_CLK				143
#define TS_CLK_TX5215_RMII_CLK				144
#define TS_CLK_TX5215_MII_TX_CLK				145
#define TS_CLK_TX5215_MII_RX_CLK				146

#define TS_CLK_TX5215_I2C1_ICCLK				150  //comp without gate
#define TS_CLK_TX5215_I2C0_ICCLK				151  //comp without gate
#define TS_CLK_TX5215_I2C3_ICCLK				152  //comp without gate
#define TS_CLK_TX5215_I2C2_ICCLK				153  //comp without gate

#define TS_CLK_TX5215_UART0_SCLK				160
#define TS_CLK_TX5215_UART1_SCLK				161
#define TS_CLK_TX5215_UART2_SCLK				162

#define TS_CLK_TX5215_SPI_SSICLK				170  //comp without gate
#define TS_CLK_TX5215_TMR_T8CLK				171  //comp without mux
#define TS_CLK_TX5215_TMR_T7CLK				172  //comp without mux
#define TS_CLK_TX5215_TMR_T6CLK				173  //comp without mux
#define TS_CLK_TX5215_TMR_T5CLK				174  //comp without mux
#define TS_CLK_TX5215_TMR_T4CLK				175  //comp without mux
#define TS_CLK_TX5215_TMR_T3CLK				176  //comp without mux

#define TS_CLK_TX5215_I2S0_OCLK				180  //comp without mux
#define TS_CLK_TX5215_I2S0_MCLK				181  //comp without mux
#define TS_CLK_TX5215_PDM_MCLK				182  //comp without mux

#define TS_CLK_TX5215_DIV0_CODEC_DAC_CCLK		183  //divider
#define TS_CLK_TX5215_DIV1_CODEC_DAC_CCLK		184  //comp without mux
#define TS_CLK_TX5215_AUD_DAC_PBCLK			185  //comp without mux

#define TS_CLK_TX5215_DIV0_CODEC_ADC_CCLK		186  //divider
#define TS_CLK_TX5215_DIV1_CODEC_ADC_CCLK		187  //comp without mux

#define TS_CLK_TX5215_DDRC_CORE_CLK			188  //gate
#define TS_CLK_TX5215_DDR_PHY_PCLK			189  //gate
#define TS_CLK_TX5215_UMCTL_PCLK				190  //gate
#define TS_CLK_TX5215_MCTL_P5_ACLK			191  //comp without mux
#define TS_CLK_TX5215_MCTL_P4_ACLK			192  //comp without mux
#define TS_CLK_TX5215_MCTL_P3_ACLK			193  //comp without mux
#define TS_CLK_TX5215_MCTL_P2_ACLK			194  //comp without mux
#define TS_CLK_TX5215_MCTL_P1_ACLK			195  //comp without mux
#define TS_CLK_TX5215_MCTL_P0_ACLK			196  //comp without mux

#define TS_CLK_TX5215_VI_CFG_HCLK				200  //gate
#define TS_CLK_TX5215_MIPI_PHY_PCLK			201  //gate
#define TS_CLK_TX5215_MIPI_RX1_PCLK			202  //gate
#define TS_CLK_TX5215_MIPI_RX0_PCLK			203  //gate
#define TS_CLK_TX5215_HDR_HCLK				204  //gate
#define TS_CLK_TX5215_VPE_HCLK				205  //gate
#define TS_CLK_TX5215_VI_DDR_ACLK				206  //comp without mux
#define TS_CLK_TX5215_HDR_ACLK				207  //gate
#define TS_CLK_TX5215_VPE_ACLK				208  //gate
#define TS_CLK_TX5215_ISP_ACLK				209  //gate

#define TS_CLK_TX5215_ISP_SCLK				210  //comp without mux
#define TS_CLK_TX5215_MIPI_RX1_PIXCLK			211  //gate
#define TS_CLK_TX5215_MIPI_RX0_PIXCLK1		212  //gate
#define TS_CLK_TX5215_MIPI_RX0_PIXCLK0		213  //gate
#define TS_CLK_TX5215_HDR_SCLK				214  //gate
#define TS_CLK_TX5215_VPE_ISP_CLK				215  //gate
#define TS_CLK_TX5215_VPE_CCLK				216  //comp without mux
#define TS_CLK_TX5215_MIPI_TXCLKESC			217  //comp without mux
#define TS_CLK_TX5215_CV_DDR_ACLK				218  //comp without mux
#define TS_CLK_TX5215_DISP_DDR_ACLK			219  //comp without mux
#define TS_CLK_TX5215_DVP_HCLK				220  //gate
#define TS_CLK_TX5215_CVE_HCLK				221  //gate
#define TS_CLK_TX5215_AMR_HCLK				222  //gate
#define TS_CLK_TX5215_DISP_CFG_HCLK			223  //gate

#define TS_CLK_TX5215_NEMADC_HCLK				224  //gate
#define TS_CLK_TX5215_DVP_ACLK				225  //gate
#define TS_CLK_TX5215_CVE_ACLK				226  //gate
#define TS_CLK_TX5215_AMR_ACLK				227  //gate
#define TS_CLK_TX5215_NEMADC_CCLK				228  //comp without mux
#define TS_CLK_TX5215_CVE_CCLK				229  //comp without mux
#define TS_CLK_TX5215_AMR_CCLK				230  //comp without mux

#define TS_CLK_TX5215_RNE_HCLK				231  //gate
#define TS_CLK_TX5215_RNE_ACLK				232  //gate
#define TS_CLK_TX5215_RNE_CCLK				233  //comp

#define TS_CLK_TX5215_VPU_ACLK				240  //divider
#define TS_CLK_TX5215_VPU_HCLK				241  //gate
#define TS_CLK_TX5215_H265_CCLK				242  //gate
#define TS_CLK_TX5215_H265_PCLK				243  //gate
#define TS_CLK_TX5215_H265_ACLK				244  //gate
#define TS_CLK_TX5215_H264_PCLK				245  //gate
#define TS_CLK_TX5215_H264_ACLK				246  //gate

#define TS_CLK_TX5215_CHIP_OCLK_I3			247
#define TS_CLK_TX5215_CHIP_OCLK_I2			248
#define TS_CLK_TX5215_CHIP_OCLK_I1			249
#define TS_CLK_TX5215_CHIP_OCLK_I0			250

#endif
