/* SPDX-License-Identifier: GPL-2.0 */
/*
 * (C) Copyright 2022 TsingMicro, Inc.
 */

#ifndef _BOARD_TX52XX_COMMON_BOARD_H
#define _BOARD_TX52XX_COMMON_BOARD_H


/** Function definitions */
void mem_map_fill(void);
int fdt_get_board_mac_cnt(void);
u64 fdt_get_board_mac_addr(void);
const char *fdt_get_board_model(void);
const char *fdt_get_board_serial(void);
const char *fdt_get_board_revision(void);

int board_late_init_xilinx(void);

#endif /* BOARD_TX52XX_COMMON_BOARD_H */
