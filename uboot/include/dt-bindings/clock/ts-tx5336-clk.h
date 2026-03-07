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
#define TS_CLK_TX5336_OSC_24M					0
#define TS_CLK_TX5336_NONE					    0
#define TS_CLK_TX5336_PLL0					    1  //2GHz
#define TS_CLK_TX5336_PLL1					    2  //2457.6MHz
#define TS_CLK_TX5336_PLL2					    3  //3200.0MHz
#define TS_CLK_TX5336_PLL3					    4  //1500.0MHz

#define TS_CLK_TX5336_PLL0_D2_D5_SDHC0_AHB_CLK	9   //200MHz
#define TS_CLK_TX5336_PLL0_D2_D5_SDHC1_AHB_CLK	10  //200MHz

#define TS_CLK_TX5336_PLL0_D2					11  //1GHz
#define TS_CLK_TX5336_PLL0_D4					12  //500MHz
#define TS_CLK_TX5336_PLL0_D5					13  //400MHz
#define TS_CLK_TX5336_PLL0_D8					14  //250MHz
#define TS_CLK_TX5336_PLL0_D10				    15  //200MHz
#define TS_CLK_TX5336_PLL0_D27				    16  //74.074MHz
#define TS_CLK_TX5336_PLL1_D3					17  //409.6MHz
#define TS_CLK_TX5336_PLL1_D4					18 //307.2MHz
#define TS_CLK_TX5336_PLL1_D6					19  //204.8MHz

#define TS_CLK_TX5336_PLL_CPU_CLK_MUX			20  //comp without gate
#define TS_CLK_TX5336_CPU_CLK_MUX				21  //divider
#define TS_CLK_TX5336_GIC_CLK_MUX			    22  //divider


#define TS_CLK_DAPLITE_SYS_PCLK_MUX			    24  // mux
#define TS_CLK_TX5336_PLL0_D2_GIC_CLK_DIV	    26  //divider
#define TS_CLK_DAPLITE_DBG_PCLK_DIV             27
#define TS_CLK_CPU_AXI_CLK_DIV                  28
#define TS_CLK_TX5336_PLL0_CPU_CLK_DIV          29
#define TS_CLK_TX5336_PLL1_CPU_CLK_DIV          30
#define TS_CLK_TX5336_OTPC_APB_CLK_GATE         32
#define TS_CLK_PLL1_D2_ISP_DRV_CLK              33

/* PERIPH0 */
#define TS_CLK_PERI0_APB_CLK_MUX		        40  //mux
#define TS_CLK_PERI0_APB_CLK_GATE               41
#define TS_CLK_PLL0_D2_PERI0_APB_CLK_DIV		42  //divider
#define TS_CLK_PERIPH1_QSPI_APB_CLK_GATE        43  //name warning --> qspi apb clk mount at periph0, but rename 1->0 will cause bug
#define TS_CLK_PERIPH0_IOMUX_APB_CLK_GATE       44
#define TS_CLK_PERIPH0_GPADC_APB_CLK_GATE       45
#define TS_CLK_PERIPH0_GPIO_APB_CLK_GATE        46

//QSPI
#define TS_CLK_TOP_OSC_QSPI_AHB_CLK_GATE        50
#define TS_CLK_TOP_OSC_QSPI_REF_CLK_GATE        51
#define TS_CLK_TOP_OSC_QSPI_AHB_CLK_MUX         52
#define TS_CLK_TOP_OSC_QSPI_REF_CLK_MUX         53
#define TS_CLK_TOP_OSC_QSPI_AHB_CLK_DIV         54
#define TS_CLK_TOP_OSC_QSPI_REF_CLK_DIV         55

/* PERIPH1 */
#define TS_CLK_PERI1_APB_CLK_MUX                    60  //ok
#define TS_CLK_PERI1_APB_CLK_GATE                   61  //ok
#define TS_CLK_PLL0_D2_PERI1_APB_CLK_DIV		    62  //divider

//UART
#define TS_CLK_PERIPH1_UATR0_APB_CLK_GATE                   63
#define TS_CLK_PERIPH1_PLL0_D8_UATR0_CORE_CLK_DIV           64
#define TS_CLK_PERIPH1_OSC_UART0_CORE_CLK_DIV               65
#define TS_CLK_PERIPH1_UART0_CORE_CLK_MUX                   66

#define TS_CLK_PERIPH1_UATR1_APB_CLK_GATE                   67
#define TS_CLK_PERIPH1_PLL0_D8_UATR1_CORE_CLK_DIV           68
#define TS_CLK_PERIPH1_OSC_UART1_CORE_CLK_DIV               69
#define TS_CLK_PERIPH1_UART1_CORE_CLK_MUX                   70

#define TS_CLK_PERIPH1_UART2_APB_CLK_GATE                   71
#define TS_CLK_PERIPH1_PLL0_D8_UART2_CORE_CLK_DIV           72
#define TS_CLK_PERIPH1_OSC_UART2_CORE_CLK                   73
#define TS_CLK_PERIPH1_UART2_CORE_CLK_MUX                   74

#define TS_CLK_PERIPH1_UART3_APB_CLK_GATE                   75
#define TS_CLK_PERIPH1_PLL0_D8_UATR3_CORE_CLK_DIV           76
#define TS_CLK_PERIPH1_OSC_UART3_CORE_CLK                   77
#define TS_CLK_PERIPH1_UART3_CORE_CLK_MUX                   78
//PWM
#define TS_CLK_PERIPH1_PWM_APB_CLK_GATE                     79

/* PERIPH2 */
#define TS_CLK_PERIPH2_SDHC0_AHB_CLK_GATE                   90

#define TS_CLK_PERIPH2_PLL0_SDHC0_CORE_CLK_DIV              91
#define TS_CLK_PERIPH2_OSC_SDHC0_CORE_CLK_GATE              92
#define TS_CLK_PERIPH2_SDHC0_CORE_CLK_MUX                   93

#define TS_CLK_PERIPH2_SDHC1_AHB_CLK_GATE                   94

#define TS_CLK_PERIPH2_PLL0_SDHC1_CORE_CLK_DIV              95
#define TS_CLK_PERIPH2_OSC_SDHC1_CORE_CLK_GATE              96
#define TS_CLK_PERIPH2_SDHC1_CORE_CLK_MUX                   97


#endif
