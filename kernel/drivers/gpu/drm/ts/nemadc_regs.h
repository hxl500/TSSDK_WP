/***************************************************************************
 * COPYRIGHT NOTICE
 * Copyright 2022 Tsingmicro, Inc.
 * All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * Nema DC dpu driver
 ***************************************************************************/


#ifndef __NEMADC_REGS_H__
#define __NEMADC_REGS_H__


#define NEMADC_REG_MODE				0x0000
#define NEMADC_REG_CLKCTRL			0x0004
#define NEMADC_REG_BGCOLOR			0x0008
#define NEMADC_REG_RESXY			0x000c
#define NEMADC_REG_FRONTPORCHXY		0x0014
#define NEMADC_REG_BLANKINGXY		0x0018
#define NEMADC_REG_BACKPORCHXY		0x001c

#define NEMADC_REG_LV1_BASE			0x0030
#define NEMADC_REG_LG1_BASE			0x0050
#define NEMADC_REG_LG2_BASE			0x0070
#define NEMADC_REG_LG3_BASE			0x0090

#define NEMADC_REG_CONFIG			0x00f0	//zhc add
#define NEMADC_REG_ID				0x00f4
#define NEMADC_REG_INTERRUPT		0x00f8
#define NEMADC_REG_STATUS			0x00fc
#define NEMADC_REG_GAMMA_LUT		0x0400


#define   NM_MODE_DPI_CFG_SHF		12
#define   NM_DPI_RGB888				0
#define   NM_DPI_RGB666_pack		1
#define   NM_DPI_RGB666_loose		2
#define   NM_DPI_RGB565_pack		3
#define   NM_DPI_RGB565_loose1		4
#define   NM_DPI_RGB565_loose2		5
#define   NM_DPI_RGB_MAX			NM_DPI_RGB565_loose2
#define   NM_MODE_GAMMA_EN_SHF		20
#define   NM_MODE_ENABLE_SHF		31

#define   NM_INT_HSYNC_MSK		(1 << 2)
#define   NM_INT_VSYNC_MSK		(1 << 3)

#define   NM_ID_VAL					0x87452365


#define NEMADC_ADDR_SPACE_SIZE		0x2000

#define NEMADC_REG_CLK_D5  		0x05
#define NEMADC_REG_CLK_D10  	0x0a

typedef enum {
	NEMA_MODE_CONTROLLER_ACTIVE = 31,
	NEMA_MODE_VSYNC_POLARITY = 28,
	NEMA_MODE_HSYNC_POLARITY = 27,
	NEMA_MODE_DE_POLARITY = 26,
	NEMA_MODE_PIXEL_CLOCK_POLARITY = 22,
	NEMA_MODE_TEST_MODE = 0
} nema_mode_bit_e;








#define NEMADC_ENABLE  1 << 31



//zhc add
#define NEMADC_CURSOR       		1 << 30
#define NEMADC_REG_CURSORXY         0x20

#endif /* __NEMADC_REGS_H__ */
