/*****************************************************************************
* Copyright (C) 2021,Tsing Micro Technology Co., Ltd,All rights reserved.
* FileName     : sys_reset.h
* Author       :
* Data         : 2020-03-15
* Description  : system reset
*
*****************************************************************************/
#ifndef __SYS_SMU__
#define __SYS_SMU__

//#include "io.h"
#include <stdio.h>

#define SMU_REG_IRQ_RAW0			0x00
#define SMU_REG_IRQ_RAW1			0x04
#define SMU_REG_IRQ_RAW2			0x08
#define SMU_REG_IRQ_MASK0			0x10
#define SMU_REG_IRQ_MASK1			0x14
#define SMU_REG_IRQ_MASK2			0x18
#define SMU_REG_IRQ_STAT0			0x20
#define SMU_REG_IRQ_STAT1			0x24
#define SMU_REG_IRQ_STAT2			0x28

#define SMU_CHIP_PWR_MODE_CFG		0x30
#define SMU_BOOT_RST_CTL			0x34
#define SMU_CHIP_GLB_SOFT_RST		0x38
#define SMU_LP_TMR_CFG				0x3C

#define SMU_DDR_PWR_CTL				0x40
#define SMU_DDR_BUS_IF_STAT			0x44
#define SMU_DDR_BUS_LP_CTL			0x48

#define SMU_AI_PWR_CTL				0x50
#define SMU_AI_BUS_IF_STAT			0x54
#define SMU_AI_BUS_LP_CTL			0x58

#define SMU_VPU_PWR_CTL				0x60
#define SMU_VPU_BUS_IF_STAT			0x64
#define SMU_VPU_BUS_LP_CTL			0x68

#define SMU_REG_VI_PWR_CTL			0x70
#define SMU_REG_VI_PWR_SWI_TIME		0x74
#define SMU_REG_VI_BUS_IF_STAT		0x78
#define SMU_REG_VI_LP_CTL			0x7C

#define SMU_REG_VO_PWR_CTL			0x80
#define SMU_REG_VO_PWR_SWI_TIME		0x84
#define SMU_REG_VO_BUS_IF_STAT		0x88
#define SMU_REG_VO_LP_CTL			0x8C

#define SMU_TOP_PWR_CTL				0x90
#define SMU_TOP_PWR_SWI_TIME		0x94
#define SMU_TOP_BUS_IF_STAT			0x98
#define SMU_TOP_BUS_LP_CTL			0x9C

#define SMU_A53_CPU0_PWR_CTL		0xA0
#define SMU_A53_CPU1_PWR_CTL		0xA4
#define SMU_A53_SCU_PWR_CTL			0xA8
#define SMU_A53_CPU_PWR_SWI_TIME	0xAC
#define SMU_A53_SCU_PWR_SWI_TIME	0xB0
#define SMU_A53_SYS_IF_STAT			0xB4
#define SMU_A53_BUS_LP_CTL			0xB8

#define SMU_CX400_PWR_CTL			0xC0
#define SMU_CX400_PWR_SWI_TIME		0xC4
#define SMU_CX400_BUS_IF_STAT		0xC8
#define SMU_CX400_BUS_LP_CTL		0xCC

#define SMU_MCU_PWR_SWI_TIME		0xD4
#define SMU_MCU_BUS_IF_STAT			0xD8
#define SMU_MCU_BUS_LP_CTL			0xDC

#define SMU_CHIP_WAKE_UP_CFG		0xE0
#define SMU_GPIO_INT_MODE			0xE4
#define SMU_AON_GPIO_CTL			0xE8
#define SMU_CHIP_STATUS				0xEC

#define SMU_RTC_CLK_CFG				0xF0
#define SMU_RTC_RST_CTL				0xF4
#define SMU_SYS_CNT_RST_CTL			0xF8
#define SMU_RC32K_OSC_TRIM			0xFC

#define SMU_USR_GPRO				0x100
#define SMU_USR_GPR1				0x104
#define SMU_USR_GPR2				0x108
#define SMU_USR_GPR3				0x10C

#define SMU_SYS_CNT_CLK_CFG			0x110
#define SMU_SHRAM_RET_EN			0x114
#define SMU_LP_RTC_EN				0x118
#define SMU_LP_TRC_CTL				0x11C

#define SMU_LP_RTC_RDAT				0x120
#define SMU_LP_RTC_CFG_TIM			0x124
#define SMU_AON_PADS_CTL			0x130

#define SMU_CK804_BOOT_CFG0			0x400
#define SMU_CK804_BOOT_CFG1			0x404
#define SMU_CK804_BOOT_CFG2			0x408
#define SMU_CK804_BOOT_CFG3			0x40C
#define SMU_CK804_BOOT_CFG4			0x410

#define SMU_CK804_DAHB_BASE			0x414
#define SMU_CK804_DAHB_MASK			0x418
#define SMU_CK804_IAHB_BASE			0x41C
#define SMU_CK804_IAHB_MASK			0x420


void drv_smu_pwrup_vo(void);
void drv_smu_pwrup_vi(void);
void drv_smu_pwrup_ddr(void);

void ts_smu_pwrup_vo(void);
void ts_smu_pwrup_vi(void);
void ts_smu_pwrup_ddr(void);
void ts_smu_pwrup_ai(void);

void smmu_set_sgrf_act(uint32_t exp_data, uint8_t lshift);

#endif
