/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2023 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_CSI2H_UAPI_H__
#define __TS_CSI2H_UAPI_H__

#include <linux/ioctl.h>

#ifdef __KERNEL__
#include <linux/time.h>		/* replace to <sys/time.h> in user space */
#else
#include <sys/time.h>
#endif

#define MAX_MIPI_RX_PIPE_NUM	4

typedef struct _lane_cfg {
	unsigned char clk_lane_num;
	unsigned char data_lane_num;
	int data_rate;  //mbps
	unsigned char bpp;
} lane_cfg;

/*
 */
#define MIPI_RX_DEV            "/dev/mipi-rx"

/*
 * 1.start:
 *       open()
 *       STREAN_ON
 *
 * 2.stop:
 *       STREAN_OFF
 *       close()
 */

/* command for ioctl() */
#define MIPI_RX_IOC_MAGIC 'r'
#define MIPI_RX_IOC_INIT             _IOW(MIPI_RX_IOC_MAGIC, 0, lane_cfg)
#define MIPI_RX_IOC_STREAM           _IOW(MIPI_RX_IOC_MAGIC, 1, int)
#define MIPI_RX_IOC_BIT_RATE         _IOW(MIPI_RX_IOC_MAGIC, 2, int)  //unit:mbps

#define MAX_CSI2H_DEVICE_NUM 1

#endif
