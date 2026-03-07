/*****************************************************************************
* Copyright (C) 2021,Tsing Micro Technology Co., Ltd,All rights reserved.
* FileName     : qw_dma.h
* Author       : yangguoyu
* Data         :
* OverView     :
* 
* History      :
* Revisor      :
* Date         :
* Description  :
*
*****************************************************************************/

#ifndef __QW_DMA_PL330_H__
#define __QW_DMA_PL330_H__

#include <stdint.h>
#include <stdbool.h>
//#include "base_type.h"
/*
 * Register offset
 * */
#define DSR					0x000
#define DSR_STATUS_SHIFT	0
#define DSR_STATUS_MASK		0x00F
#define DPC					0x004
#define INTEN				0x020
#define INT_EVENT_RIS		0x024
#define INTMIS				0x028
#define INTCLR				0x02C

#define FSRD  0x30
#define FSRC  0x34
#define FTRD  0x38
#define FTR_BASE 0x40
#define FTR(n) (FTR_BASE + (n)*0x4)

#define CSR_BASE			0x100
#define CSR(n)				(CSR_BASE + (n)*0x8) // n = 0:7
#define CSR_CHANNEL_STATUS_SH	0
#define CSR_CHANNEL_STATUS_MK	0x00F

#define CPC_BASE			0x104
#define CPC(n)				(CPC_BASE + (n)*0x8) // n = 0:7

#define SAR_BASE			0x400
#define SAR(n)				(SAR_BASE + (n)*0x8) // n = 0:7

#define DAR_BASE			0x404
#define DAR(n)				(DAR_BASE + (n)*0x8) // n = 0:7

#define CCR_BASE			0x408
#define CCR(n)				(CCR_BASE + (n)*0x8) // n = 0:7

#define LC0_BASE			0x40C
#define LC0(n)				(LC0_BASE + (n)*0x8) // n = 0:7
#define LC1_BASE			0x410
#define LC1(n)				(LC1_BASE + (n)*0x8) // n = 0:7

#define CR_BASE					0xE00
#define CR(n)					(CR_BASE  + (n)*0x4) // n = 0:4
#define CR0_PERIF_REQ_SUPP		(1 << 0)
#define CR0_BOOT_EN_SET		(1 << 1)
#define CR0_BOOT_MAN_NS		(1 << 2)
#define CR0_NUM_CHANNELS_SH		4
#define CR0_NUM_CHANNELS_MK		0x007
#define CR0_NUM_PERIF_REQ_SH	12
#define CR0_NUM_PERIF_REQ_MK	0x01F
#define CR0_NUM_EVENT_SHIFT		17
#define CR0_NUM_EVENT_MASK		0x01F

#define CRD						0xE14
#define CRD_BUS_WIDTH_SHIFT		0
#define CRD_BUS_WIDTH_MASK		0x007
#define CRD_BUF_DEPTH_SHIFT		20
#define CRD_BUF_DEPTH_MASK		0x3FF

#define DBGSTATUS				0xD00
#define DBG_BUSY_MASK			(1 << 0)
#define DBGCMD					0xD04
#define DBGINST0				0xD08
#define DBGINST1				0xD0C

#define PERIPH_ID_BASE    0xfe0
#define PERI_ID(n)  (PERIPH_ID_BASE + (n)*0x4)

#define	shift_and_mask(a, x, y)	(((a) >> (x)) & (y))

/*
 * Available states encoding
 * */
#define STOPPED				0x000
#define EXECUTING			0x001
#define CACHE_MISS			0x002
#define UPDATING_PC			0x003
#define WAIT_EVENT			0x004
#define BARRIER				0x005
#define WAIT_PERIPH			0x007
#define KILLING				0x008
#define COMPLETING			0x009
#define FAULT_COMPLETING	0x00E
#define FAULTING			0x00F
#define INVALID_STATE		0x010

/*
 * Commands encoding
 */

/*
 * DMAMOV
 */
#define DMAMOV			0x0BC
#define _SAR			0x0 // 0b000
#define _CCR			0x1 // 0b001
#define _DAR			0x2 // 0b010
#define DMAMOV_SIZE		6 // command + type + address = 1 + 1 + 4

/*
 * DMAEND
 */
#define DMAEND			0x000
#define DMAEND_SIZE		1

/*
 * DMALP
 */
#define DMALP			0x020
#define DMALP_SIZE		2

/*
 * DMALPEND
 */
#define DMALPEND		0x028
#define DMALPEND_SIZE		2

/*
 * DMALD
 * */
#define DMALD			0x004
#define DMALD_SIZE		1

/*
 * DMAST
 * */
#define DMAST			0x008
#define DMAST_SIZE		1

/*
 * DMARMB
 * */
#define DMARMB			0x012
#define DMARMB_SIZE		1

/*
 * DMAWMB
 * */
#define DMAWMB			0x013
#define DMAWMB_SIZE		1

/*
 * DMAGO
 */
#define DMAGO			0x0A0
#define DMAGO_SIZE		6

/*
 * DMASEV
 * */
#define DMASEV			0x034
#define DMASEV_SIZE		2

/*
 * DMAKILL
 * */
#define DMAKILL			0x001
#define DMAKILL_SIZE		1

/*
 * DMAWFP
 * */
#define DMAWFP			0x30
#define DMAWFP_SIZE		2


/*
 * DMALDP
 * */
#define DMALDP			0x25
#define DMALDP_SIZE		2

/*
 * DMASTP
 * */
#define DMASTP			0x29
#define DMASTP_SIZE		2

/*
 * DMAFLUSHP
 * */
#define DMAFLUSHP			0x35	
#define DMAFLUSHP_SIZE		2


/*
 * Channel Control Register - CCR
 */


#define CCR_SRCPRI		(1 << 8)
#define CCR_DSTPRI		(1 << 22)
#define CCR_SRCNS		(1 << 9)
#define CCR_DSTNS		(1 << 23)
#define CCR_SRCIA		(1 << 10)
#define CCR_DSTIA		(1 << 24)

#define CCR_SRCINC				(1 << 0)		// source control
#define CCR_SRCBURSTSIZE_SHIFT	1
#define CCR_SRCBURSTLEN_SHIFT	4
#define CCR_SRCPROTCTRL_SHIFT	8
#define CCR_SRCCACHECTRL_SHIFT	11

#define CCR_DSTINC				(1 << 14)	// destination control
#define CCR_DSTBURSTSIZE_SHIFT	15
#define CCR_DSTBURSTLEN_SHIFT	18
#define CCR_DSTPROTCTRL_SHIFT	22
#define CCR_DSTCACHECTRL_SHIFT	25
#define CCR_ENDIANSWAPSZ_SHIFT	28

// default values
#define INC_DEF_VAL		1
#define CCR_PROTCTRL_DEF_VAL	(0 << 0) | (1 << 1) | (0 << 2)
#define CCR_BURSTLEN_DEF_VAL	1
#define CCR_BURSTSIZE_DEF_VAL	2
#define CCR_CACHECTRL_DEF_VAL	0

// CCR limit val
#define CCR_BURSTSIZE_MAX	16 // bytes
#define CCR_BURSTLEN_MAX	16 // data transfers

#define BRST_LEN(ccr)		((((ccr) >> CCR_SRCBURSTLEN_SHIFT) & 0xf) + 1)
#define BRST_SIZE(ccr)		(1 << (((ccr) >> CCR_SRCBURSTSIZE_SHIFT) & 0x7))

#define BYTE_TO_BURST(b, ccr)	((b) / BRST_SIZE(ccr) / BRST_LEN(ccr))
#define BURST_TO_BYTE(c, ccr)	((c) * BRST_SIZE(ccr) * BRST_LEN(ccr))

#define DMA330_INT_NUM 2

/*
 * IDs
 *
 * The channels IDs are 0,1,...,status->channels
 * Since there could be 8 channels maximum, the manager
 * thread ID is 8
 * */
#define MANAGER_ID		8
#define true 1
#define false 0

enum DMAMOV_type {
	SAR = 0,
	CCR,
	DAR,
};

enum DMA_LOOP_REGISTER {
	LOOP_CNT_0_REG = 0,
	LOOP_CNT_1_REG = 1,
};

enum LOOP_START_TYPE {
	BY_DMALP = 0,
	BY_DMALPFE,
};

enum request_type {
	SINGLE,
	BURST,
	ALWAYS,
};

enum transfer_type {
	DMA_MEM_TO_MEM,
	DMA_MEM_TO_DEV,
	DMA_DEV_TO_MEM,
};

struct controller_config {
#define DMAC_MODE_NS	(1 << 0)
	unsigned char non_secure_mode;
	uint8_t data_bus_width;
	uint32_t num_events;
};

/*
 * move to another file
 * CCR configuration
 * */
enum dst_src {
	DST,
	SRC,
};

struct pl330_status {
	u32 channels; // # of channels available
	struct channel_thread *ch_threads;

	/*
	 * the controller supports 32 events.
	 * The envent i is allocated if
	 * allocated_events[i] == 1
	 * */
	u32 allocated_events;

	u8 * mcpu_cod;
	u32 mcpu_cod_size;
	u32 regs; // pointer to the first pl330 register
	int highest_irq_num;

	struct controller_config ccfg;

	/*err*/
	int8_t reset_mngr;
	int8_t reset_chan;
	int8_t reset_dmac;
};

struct req_config {
	// source and destination
	u32 src;
	u32 dst;

	// bytes to transfer
	int size;

	// type of the transfer (mem to mem, mem to dev, dev to mem)
	enum transfer_type t_type;

	/*
	 * channel configuration
	 */
	// increment value
	unsigned int src_inc;
	unsigned int dst_inc;

	// burst size
	unsigned int burst_size;

	// burst length
	unsigned int burst_len;

	// channel to which submit the request
	unsigned int chan_id;
	
	/*
	 * For now, the SRC & DST protection levels
	 * and burst size/length are assumed same.
	 */
	u8 nonsecure;
	u8 privileged;
	u8 insnaccess;

	unsigned int src_prot_ctrl;
	unsigned int dst_prot_ctrl;

	// cache control value
	unsigned int src_cache_ctrl;
	unsigned int dst_cache_ctrl;

	// arise an interrupt when the transfer is completed
	unsigned char int_fin;

	// callback to be called when the request has been served
	void (*callback)(void *user_data);
	void *user_data;

	uint8_t peri;
	uint32_t ccr;

	/*variable : for test */
	bool force_secure_axi_aces;
	bool force_mcpu_code;
	uintptr_t force_mcpu_code_addr;
};

enum channel_thread_state {
	FREE,
	ALLOCATED,
};

struct channel_thread {
	enum channel_thread_state state;
	/*
	 * event to fire when the transfer completes
	 * */
	int event_id;

	// callback when finished
	void (* callback)(void *user_data);
	void *user_data;
};

int qw_dma_pl330_init(bool nonsec);
void qw_dma330_prep_memcpy(struct req_config *config, uintptr_t src, uintptr_t dst, uint32_t size);
int qw_dma330_requestChannel(uint8_t *chan);
int qw_dma_pl330_submit(struct req_config *config);
void qw_dma330_update(void);
void qw_dma330_releaseChannel(uint8_t id);
uint32_t qw_dma330_abort_status(void);
void qw_dma330_stop_threads(void);
void qw_dma330_remove(void);

#endif
