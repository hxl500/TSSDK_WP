/*****************************************************************************
* Copyright (C) 2021,Tsing Micro Technology Co., Ltd,All rights reserved.
* FileName     : sys_irq.c
* Author       :
* Data         :
* OverView     :
*
* History      :
* Revisor      :
* Date         :
* Description  :
*
* GIC hardware information of TX511:
* 	GICv2(GIC400), with security extentions
* 	Implemented priority bits [7:3]
*
*****************************************************************************/
#include <string.h>
#include <stdio.h>

#include "sys_exception.h"
//#include </usr/include/assert.h>
#include "log.h"
#include "MP_GIC.h"
#include "v8_aarch64.h"
//#include "debug.h"

struct IrqTableEntry g_IrqTable[INT_MAX_NUM];

void drv_irq_register(uint32_t irq_id, struct IrqTableEntry *Entry)
{
	assert(irq_id <= INT_MAX_ID);
	assert(Entry != NULL);

	g_IrqTable[irq_id].Handler = Entry->Handler;
	g_IrqTable[irq_id].data = Entry->data;
}

void drv_irq_unregister(uint32_t irq_id)
{
	assert(irq_id <= INT_MAX_ID);

	g_IrqTable[irq_id].Handler = NULL;
	g_IrqTable[irq_id].data = NULL;
}


void drv_irq_enable(uint32_t irq_num)
{
	uint32_t reg_off, bit_shift;

	reg_off = 4 * (irq_num / 32);
	reg_off += GIC_DIST_ENABLE_SET;
	bit_shift = irq_num % 32;
	*(uint32_t *)(GIC_DIST_BASE + reg_off) = (1 << bit_shift);
}

void drv_irq_disable(uint32_t irq_num)
{
	uint32_t reg_off, bit_shift;

	reg_off = 4 * (irq_num / 32);
	reg_off += GIC_DIST_ENABLE_CLEAR;
	bit_shift = irq_num % 32;
	*(uint32_t *)(GIC_DIST_BASE + reg_off) = (1 << bit_shift);
}

#if 0
void drv_irq_sendSGI(uint32_t irq_id, sgi_tlf filter, uint32_t target)
{
	SendSGI(irq_id, filter, target, 1);
}
#endif
/*
 * Some initializations are completed by startup.s, include:
 * 		All interrupt route to group1
 * 		PriorityMask in cpu interface set to 0x10, so NS world can modify
 * This function does the other initializations and modify PriorityMask
 * 		All interrupt default priotiry set to 0x0e, and can be modified by ohter
 * 		function, the valid value is from 0 to 0x0e
 * 		The lower value has high priority.
 *
 */
void drv_exception_init(void)
{
	uint32_t cpu_id, irq_id;
	uint8_t *reg_prio_base = (uint8_t *)(GIC_DIST_BASE + GIC_DIST_PRI);
	uint8_t *reg_target_base = (uint8_t *)(GIC_DIST_BASE + GIC_DIST_TARGET);
	uint32_t reg_off, bit_shift;
	uint32_t reg_val;

	memset(g_IrqTable, 0, sizeof(g_IrqTable));

	cpu_id = GetCPUID();

	/* only cpu0 init Distributor */
	if (cpu_id == 0) {
		DisableGICD();
		/*
		 * Config priority, tx511 implemented 5 bits(7:3) priority, in the NS
		 * world, we can only use [6:3], hardware will shift right, so we need
		 * shift left before write to the reg.
		 * The GICD_IPRIORITYRn reg are byte-accessible.
		 */
		for (irq_id = 0; irq_id < INT_MAX_NUM; irq_id++)
			reg_prio_base[irq_id] = (GICD_DEFAULT_PRIO << 1);

		/* the default trigger is level for spi, no need to modify
		 * gdc signale edge triger. modify it
		 */
		reg_off = INT_ID_GDC / 16;
		reg_off = GIC_DIST_CONFIG + 4 * reg_off;
		bit_shift = INT_ID_GDC % 16;
		bit_shift *= 2;
		reg_val = GIC_DIST_RD(reg_off);
		reg_val &= ~(3 << bit_shift);
		reg_val |= GICD_CFG_EDGE << bit_shift;
		GIC_DIST_WR(reg_off, reg_val);

		/* Set spi target to cpu0 */
		for (irq_id = 32; irq_id < INT_MAX_NUM; irq_id++)
			reg_target_base[irq_id] = GICD_TARGET_CPU0;
	}

	/* init cpu interface */
	/* The priority mask is 0xF */
	GIC_CPU_INTF_WR(GIC_CPU_PRIMASK, (GICC_INT_PRI_THRESHOLD << 1));

	if (cpu_id == 0)
		EnableGICD();
	EnableGICC();
}

/*
 * The entry for all irq interrupt
 */
void irqHandler(void)
{
	unsigned int ID;

	/* acknowledge the interrupt, form pending to active */
	ID = ReadIAR();

	// Check for reserved IDs
	if ((1020 <= ID) && (ID <= 1023)) {
		//printf("irqHandler() - Reserved INTID %d\n\n", ID);
		return;
	}
	if (ID > INT_MAX_ID) {
		printf("irqHandler() - unsupport interrupt id %d\n\n", ID);
		return;
	}


	//qw_printf("ID = %d\n", ID);

	if (g_IrqTable[ID].Handler)
		g_IrqTable[ID].Handler(g_IrqTable[ID].data);

	// Write the End of Interrupt register to tell the GIC
	// we've finished handling the interrupt
	WriteEOI(ID);
}


void fiqHandler(void)
{

}

