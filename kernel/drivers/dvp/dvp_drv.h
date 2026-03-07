/*****************************************************************************
 * Copyright (C) 2022,Tsing Micro Technology Co., Ltd,All rights reserved.
 * Description:
 *
 *****************************************************************************/
 
#ifndef __DVP_DRV_H__
#define __DVP_DRV_H__

#include <uapi/linux/types.h>
#include <linux/kthread.h>
#include <linux/cdev.h>

#include "dvp_reg.h"

#define DVP_WRITE_DDR_ADDR    0
#define DVP_ENABLE            1

typedef enum {
	DISABLE = 0,
	ENABLE = !DISABLE,
} FunctionalState;

typedef enum {
	DVP_EVENT = 0,
} dvp_event_e;

typedef void (*dvp_event_cb_t)(int idx, dvp_event_e event, void *user_data);

const char *dvp_reset_id[3] = {
	"dvp_hrst",
	"dvp_arst",
	"dvp_pxrst"
};

struct dvp_hw_res {
	void __iomem *base_reg;
	resource_size_t regs_start;
	resource_size_t regs_end;
	int irq;
	
	int idx;
	dvp_event_cb_t cb_event;
	void *user_data;
};

struct dvp_dev {
	struct class *class;
	struct device *dev;
	struct cdev cdev;
	dev_t devno;
	struct dvp_hw_res hw_res;
	unsigned int irq_status;
	dvp_gcr gcr;
	dvp_para para;
	dvp_image_addr addr;
	dvp_hpara hpara;
	dvp_vpara vpara;
	struct clk *hclk;
	struct clk *aclk;
};

static int dvp_cfg_register_os02g10(struct dvp_dev* dvp);

#endif