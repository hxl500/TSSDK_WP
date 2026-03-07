/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Tsingmicro SoC tx5336 Firmware layer
 *
 *  Copyright (C) 2022-2022 Tsingmicro, Inc.
 *
 */
#ifndef _DT_BINDINGS_CLK_TS_TX5336_H
#define _DT_BINDINGS_CLK_TS_TX5336_H

/* clock id for dts */
#define TS_CLK_TX5336_OSC_24M				0
#define TS_CLK_TX5336_NONE					0
#define TS_CLK_TX5336_PLL0					1  
#define TS_CLK_TX5336_PLL1					2  
#define TS_CLK_TX5336_PLL2					3
#define TS_CLK_TX5336_PLL3					4

#define TS_CLK_TX5336_PLL0_D2					10  
#define TS_CLK_TX5336_PLL0_D4					11  
#define TS_CLK_TX5336_PLL0_D8					13
#define TS_CLK_TX5336_PLL0_D20					14
#define TS_CLK_TX5336_PLL0_D40                  15  

#define TS_CLK_TX5336_PLL1_D2					16  
#define TS_CLK_TX5336_PLL1_D4					17  
#define TS_CLK_TX5336_PLL1_D8					18  

#define TS_CLK_TX5336_GIC_CLK                  49 //comp
#define TS_CLK_TX5336_CPU_PLL0_CCLK            50 //divider
#define TS_CLK_TX5336_CPU_PLL1_CCLK            51 //divider
#define TS_CLK_TX5336_PLL_MUX				   52 //mux
#define TS_CLK_TX5336_CPU_CCLK				   53 //mux
#define TS_CLK_TX5336_CPU_ACLK				   54 //divider

#define TS_CLK_TX5336_PLL0_TOPNOC_CCLK         60 //divider
#define TS_CLK_TX5336_PLL1_D2_TOPNOC_CLK       61 //divider
#define TS_CLK_TX5336_PLL_TOPNOC_CLK           62 //mux
#define TS_CLK_TX5336_TOPNOC_CLK               63 //mux
#define TS_CLK_TX5336_PERI0_APB_CLK            64 //comp
#define TS_CLK_TX5336_OTPC_APB_CLK             66 //gate
#define TS_CLK_TX5336_GPADC_APB_CLK            67 //gate
#define TS_CLK_TX5336_IOMUX_APB_CLK            68 //gate
#define TS_CLK_TX5336_SYSREG_APB_CLK           69 //gate
#define TS_CLK_TX5336_DAPLITE_APB_CLK          70 //gate
#define TS_CLK_TX5336_GPIO_APB_CLK             71 //gate
#define TS_CLK_TX5336_WDT_APB_CLK              72 //gate

#define TS_CLK_TX5336_PERI1_APB_CLK            73 //comp
#define TS_CLK_TX5336_PWM_APB_CLK              74 //gate
#define TS_CLK_TX5336_USI0_APB_CLK             75 //gate
#define TS_CLK_TX5336_USI1_APB_CLK             76 //gate

#define TS_CLK_TX5336_PERI2_AHB_CLK            77 //comp
#define TS_CLK_TX5336_HDMA0_AHB_CLK            78 //gate
#define TS_CLK_TX5336_HDMA1_AHB_CLK            79 //gate

#define TS_CLK_TX5336_QSPI_APB_CLK              80 //gate
#define TS_CLK_TX5336_QSPI_AHB_CLK              81 //comp
#define TS_CLK_TX5336_QSPI_REF_CLK              82 //comp

#define TS_CLK_TX5336_AUD_CODEC_AHB_CLK         83 //comp_nomux
#define TS_CLK_TX5336_AUD_DAC_CCLK              84 //comp_nomux
#define TS_CLK_TX5336_AUD_DAC_PBCLK             85 //comp_nomux
#define TS_CLK_TX5336_AUD_ED_TMR_STRB_CLK       86 //comp_nomux
#define TS_CLK_TX5336_AUD_ADC_CCLK              87 //comp_nomux

#define TS_CLK_TX5336_SPI0_APB_CLK              88 //gate
#define TS_CLK_TX5336_SPI0_CORE_CLK             89 //comp
#define TS_CLK_TX5336_SPI1_APB_CLK              90 //gate
#define TS_CLK_TX5336_SPI1_CORE_CLK             91 //comp
#define TS_CLK_TX5336_SPI2_APB_CLK              92 //gate
#define TS_CLK_TX5336_SPI2_CORE_CLK             93 //comp

#define TS_CLK_TX5336_TIME0_APB_CLK             94 //gate
#define TS_CLK_TX5336_TIME0_3_CLK               95 //gate
#define TS_CLK_TX5336_TIME0_4_CLK               96 //gate
#define TS_CLK_TX5336_TIME0_5_CLK               97 //comp_nomux
#define TS_CLK_TX5336_TIME0_6_CLK               98 //comp_nomux
#define TS_CLK_TX5336_TIME0_7_CLK               99 //comp_nomux
#define TS_CLK_TX5336_TIME0_8_CLK               100 //comp_nomux

#define TS_CLK_TX5336_TIME1_APB_CLK             101 //gate
#define TS_CLK_TX5336_TIME1_3_CLK               102 //gate
#define TS_CLK_TX5336_TIME1_4_CLK               103 //gate
#define TS_CLK_TX5336_TIME1_5_CLK               104 //div
#define TS_CLK_TX5336_TIME1_6_CLK               105 //div
#define TS_CLK_TX5336_TIME1_7_CLK               106 //div
#define TS_CLK_TX5336_TIME1_8_CLK               107 //div
/*
To be added
*/
#define TS_CLK_TX5336_UART0_APB_CLK             150 // gate
#define TS_CLK_TX5336_UART0_CORE_CLK            151 // comp
#define TS_CLK_TX5336_UART1_APB_CLK             152 // gate
#define TS_CLK_TX5336_UART1_CORE_CLK            153 // comp
#define TS_CLK_TX5336_UART2_APB_CLK             154 // gate
#define TS_CLK_TX5336_UART2_CORE_CLK            155 // comp
#define TS_CLK_TX5336_UART3_APB_CLK             156 // gate
#define TS_CLK_TX5336_UART3_CORE_CLK            157 // comp

#define TS_CLK_TX5336_I2S_APB_CLK 				158 //gate
#define TS_CLK_TX5336_I2S_O_CLK				    159 //comp without mux
#define TS_CLK_TX5336_I2S_M_CLK				    160 //comp without mux
#define TS_CLK_TX5336_I2S_O_CLK_O				161 //gate
#define TS_CLK_TX5336_SDHC0_AHB_CLK             162 // gate
#define TS_CLK_TX5336_SDHC0_CCLK                163 // comp
#define TS_CLK_TX5336_SDHC1_AHB_CLK             164 // gate
#define TS_CLK_TX5336_SDHC1_CCLK                165 // comp
#define TS_CLK_TX5336_USB_AHB_CLK               166
#define TS_CLK_TX5336_USB_REF_CLK               167
#define TS_CLK_TX5336_USB_REF_PLL0_D4_CLK       168
#define TS_CLK_TX5336_SDHC0_SMPL_CLK            169
#define TS_CLK_TX5336_SDHC1_SMPL_CLK            170

/*
To be added gmac
*/
#define TS_CLK_TX5336_DDR_APB_CLK              180
#define TS_CLK_TX5336_DDRC_APB_CLK             181
#define TS_CLK_TX5336_DDRPHY_APB_CLK           182 
#define TS_CLK_TX5336_DDRC_CORE_APB_CLK        183 
#define TS_CLK_TX5336_DDRPHY_CORE_APB_CLK      184 
#define TS_CLK_TX5336_UMCTL_P0_CLK             185 
#define TS_CLK_TX5336_UMCTL_P1_PLL0_CLK        186
#define TS_CLK_TX5336_UMCTL_P1_PLL1_D2_CLK     187
#define TS_CLK_TX5336_UMCTL_P1_CLK             188
#define TS_CLK_TX5336_UMCTL_P2_PLL0_CLK        189
#define TS_CLK_TX5336_UMCTL_P2_PLL1_D2_CLK     190
#define TS_CLK_TX5336_UMCTL_P2_CLK             191
#define TS_CLK_TX5336_UMCTL_P3_CLK             192
#define TS_CLK_TX5336_UMCTL_P4_PLL0_CLK        193
#define TS_CLK_TX5336_UMCTL_P4_PLL1_D2_CLK     194
#define TS_CLK_TX5336_UMCTL_P4_CLK             195
#define TS_CLK_TX5336_UMCTL_P5_CLK             196

#define TS_CLK_TX5336_RNE_AHB_CLK              197
#define TS_CLK_TX5336_RNE_AXI_CLK              198
#define TS_CLK_TX5336_RNE_CORE_CLK             199
#define TS_CLK_TX5336_RNE_CORE_PLL0_CLK        200
#define TS_CLK_TX5336_RNE_CORE_PLL1_D2_CLK     201
#define TS_CLK_TX5336_RNE_CORE_PLL_CLK         202
#define TS_CLK_TX5336_CGRA_CLK                 203
#define TS_CLK_TX5336_MEMBUF_AXI_CLK           204
#define TS_CLK_TX5336_VI_AHB_CLK               205
#define TS_CLK_TX5336_VI_APB_CLK               206
#define TS_CLK_TX5336_VI_NOC_PLL0_CLK          207
#define TS_CLK_TX5336_VI_NOC_PLL1_D2_CLK       208
#define TS_CLK_TX5336_VI_NOC_CLK               209
#define TS_CLK_TX5336_ISP_CORE_CLK             210
#define TS_CLK_TX5336_ISP_AXI_PLL0_CLK         211
#define TS_CLK_TX5336_ISP_AXI_PLL1_D2_CLK      212
#define TS_CLK_TX5336_ISP_AXI_CLK              213
#define TS_CLK_TX5336_ISP_DRV_PLL0_CLK         214
#define TS_CLK_TX5336_ISP_DRV_PLL1_D2_CLK      215
#define TS_CLK_TX5336_ISP_DRV_CLK              216
#define TS_CLK_TX5336_VPE_AHB_CLK              217
#define TS_CLK_TX5336_VPE_AXI_PLL0_CLK         218
#define TS_CLK_TX5336_VPE_AXI_PLL1_D2_CLK      219
#define TS_CLK_TX5336_VPE_AXI_CLK              220
#define TS_CLK_TX5336_VPE_ISP_CLK              221
#define TS_CLK_TX5336_VPE_CORE_PLL0_CLK        222
#define TS_CLK_TX5336_VPE_CORE_PLL1_D2_CLK     223
#define TS_CLK_TX5336_VPE_CORE_CLK             224
#define TS_CLK_TX5336_DOF_APB_CLK              225
#define TS_CLK_TX5336_DOF_CORE_CLK             226
#define TS_CLK_TX5336_DOF_AXI_PLL0_CLK         227
#define TS_CLK_TX5336_DOF_AXI_PLL1_D2_CLK      228
#define TS_CLK_TX5336_DOF_AXI_CLK              229
#define TS_CLK_TX5336_EBD_AHB_CLK              230 //todo
#define TS_CLK_TX5336_EBD_CORE_CLK             231
#define TS_CLK_TX5336_EBD_AXI_CLK              232
#define TS_CLK_TX5336_MIPI_CSI0_APB_CLK        233
#define TS_CLK_TX5336_MIPI_CSI1_APB_CLK        234
#define TS_CLK_TX5336_MIPI_CSI2_APB_CLK        235
#define TS_CLK_TX5336_MIPI_CSI3_APB_CLK        236
#define TS_CLK_TX5336_MIPI_RXPHY0_APB_CLK      237
#define TS_CLK_TX5336_MIPI_RXPHY1_APB_CLK      238
#define TS_CLK_TX5336_MIPI_CSI0_IPI0_CLK       239
#define TS_CLK_TX5336_MIPI_CSI0_IPI1_CLK       240
#define TS_CLK_TX5336_MIPI_CSI1_IPI_CLK        241
#define TS_CLK_TX5336_MIPI_CSI2_IPI0_CLK       242
#define TS_CLK_TX5336_MIPI_CSI2_IPI1_CLK       243
#define TS_CLK_TX5336_MIPI_CSI3_IPI_CLK        244
#define TS_CLK_TX5336_MIPI_TXCLKESC_CLK        245
#define TS_CLK_TX5336_CV_AHB_CLK               246
#define TS_CLK_TX5336_CV_APB_CLK               247
#define TS_CLK_TX5336_CV_NOC_CLK               248
#define TS_CLK_TX5336_HASH_CLK                 249
#define TS_CLK_TX5336_SKE_CLK                  250
#define TS_CLK_TX5336_SPACC_CLK                251
#define TS_CLK_TX5336_PKE_CLK                  252
#define TS_CLK_TX5336_TRNG_CLK                 253
#define TS_CLK_TX5336_AMR_AHB_CLK              254
#define TS_CLK_TX5336_AMR_AXI_CLK              255
#define TS_CLK_TX5336_AMR_CORE_CLK             256
#define TS_CLK_TX5336_CVE_AHB_CLK              257
#define TS_CLK_TX5336_CVE_AXI_CLK              258
#define TS_CLK_TX5336_CVE_CORE_CLK             259
#define TS_CLK_TX5336_BIS_APB_CLK              260
#define TS_CLK_TX5336_BIS_AXI_CLK              261
#define TS_CLK_TX5336_BIS_CORE_PLL0_CLK        262
#define TS_CLK_TX5336_BIS_CORE_PLL1_D2_CLK     263
#define TS_CLK_TX5336_BIS_CORE_CLK             264
#define TS_CLK_TX5336_GDC_APB_CLK              265
#define TS_CLK_TX5336_GDC_AXI_CLK              266
#define TS_CLK_TX5336_GDC_CORE_PLL0_CLK        267
#define TS_CLK_TX5336_GDC_CORE_PLL1_D2_CLK     268
#define TS_CLK_TX5336_GDC_CORE_CLK             269
#define TS_CLK_TX5336_DPU_AHB_CLK              270
#define TS_CLK_TX5336_DPU_AXI_CLK              271
#define TS_CLK_TX5336_DPU_CORE_CLK             272
#define TS_CLK_TX5336_MIPI_DSI_APB_CLK         273
#define TS_CLK_TX5336_MIPI_TXPHY_APB_CLK       274
#define TS_CLK_TX5336_MIPI_REF_CLK             275
#define TS_CLK_TX5336_MIPI_REF_PLL0_D4_CLK     324  //zhc
#define TS_CLK_TX5336_ADMA_AHB_CLK             276
#define TS_CLK_TX5336_ADMA_AXI_CLK             277
#define TS_CLK_TX5336_CHIP_OCLK0_OSC_CLK       278
#define TS_CLK_TX5336_CHIP_OCLK0_pll0_d4_CLK   279
#define TS_CLK_TX5336_CHIP_OCLK0_pll1_d8_CLK   280
#define TS_CLK_TX5336_CHIP_OCLK0_CLK           281
#define TS_CLK_TX5336_CHIP_OCLK1_OSC_CLK       282
#define TS_CLK_TX5336_CHIP_OCLK1_pll0_d4_CLK   283
#define TS_CLK_TX5336_CHIP_OCLK1_pll1_d8_CLK   284
#define TS_CLK_TX5336_CHIP_OCLK1_CLK           288
#define TS_CLK_TX5336_CHIP_OCLK2_OSC_CLK       289
#define TS_CLK_TX5336_CHIP_OCLK2_pll0_d4_CLK   290
#define TS_CLK_TX5336_CHIP_OCLK2_pll1_d8_CLK   291
#define TS_CLK_TX5336_CHIP_OCLK2_CLK           292
#define TS_CLK_TX5336_CHIP_OCLK3_OSC_CLK       293
#define TS_CLK_TX5336_CHIP_OCLK3_pll0_d4_CLK   294
#define TS_CLK_TX5336_CHIP_OCLK3_pll1_d8_CLK   295
#define TS_CLK_TX5336_CHIP_OCLK3_CLK           296
#define TS_CLK_TX5336_PDM0_APB_CLK             297 
#define TS_CLK_TX5336_PDM0_M_CLK               298
#define TS_CLK_TX5336_PDM1_APB_CLK             299
#define TS_CLK_TX5336_PDM1_M_CLK               300
#define TS_CLK_TX5336_PDM2_APB_CLK             301
#define TS_CLK_TX5336_PDM2_M_CLK               302
#define TS_CLK_TX5336_PDM3_APB_CLK             303
#define TS_CLK_TX5336_PDM3_M_CLK               304
#define TS_CLK_TX5336_VPU_NOC_PLL0_CLK         305
#define TS_CLK_TX5336_VPU_NOC_PLL1_D2_CLK      306
#define TS_CLK_TX5336_VPU_NOC_CLK              307
#define TS_CLK_TX5336_VPU_CORE_CLK             308

#define TS_CLK_TX5336_I2C0_APB_CLK             310  //GATE ONLY
#define TS_CLK_TX5336_I2C0_CCLK                311  //COMP
#define TS_CLK_TX5336_I2C1_APB_CLK             312  //GATE ONLY
#define TS_CLK_TX5336_I2C1_CCLK                313  //COMP
#define TS_CLK_TX5336_I2C2_APB_CLK             314  //GATE ONLY
#define TS_CLK_TX5336_I2C2_CCLK                315  //COMP
#define TS_CLK_TX5336_I2C3_APB_CLK             316  //GATE ONLY
#define TS_CLK_TX5336_I2C3_CCLK                317  //COMP
#define TS_CLK_TX5336_I2C4_APB_CLK             318  //GATE ONLY
#define TS_CLK_TX5336_I2C4_CCLK                319  //COMP
#define TS_CLK_TX5336_I2C5_APB_CLK             320  //GATE ONLY
#define TS_CLK_TX5336_I2C5_CCLK                321  //COMP
#define TS_CLK_TX5336_I2C6_APB_CLK             322  //GATE ONLY
#define TS_CLK_TX5336_I2C6_CCLK                323  //COMP

#define TS_CLK_TX5336_MAX                      350
#endif
