/*****************************************************************************
* Copyright (C) 2021,Tsing Micro Technology Co., Ltd,All rights reserved.
* FileName     : qw_dma.c
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "qw_dma330.h"
//#include "debug.h"
//#include "devices.h"
//#include "stub.h"
//#include "soc.h"
#include "sys_smmu.h"
#include "sys_exception.h"
#include "reg.h"

#define qw_printf printf

#if 0
#define DBG_PRT(FMT, ...)	qw_printf(FMT, ## __VA_ARGS__)
#else
#define DBG_PRT(FMT, ...)
#endif

#define WARN_ON(cond)	\
	((cond) ? qw_printf("Internal warning(%s:%d, %s): %s\n",	\
			__FILE__, __LINE__, __func__, #cond) : 0)

struct pl330_status *g_pl330Status = NULL;
static inline u32 dma330_read_reg(u32 addr)
{
    return *((volatile u32 *)(uintptr_t)addr);
}

static inline void dma330_write_reg(u32 val, u32 addr)
{
    *(volatile u32 *)(uintptr_t)addr = val;
}

struct CR0_conf {
	unsigned char perif_req_support;
	unsigned char mode;
	u32 num_channels;
	u32 num_perif_req;
	u32 num_events;
};

static void CR0_read_conf(struct CR0_conf *conf)
{
	u32 cr0_reg;

	cr0_reg =  dma330_read_reg(g_pl330Status->regs + CR(0));

	conf->perif_req_support = (cr0_reg & CR0_PERIF_REQ_SUPP) ? true : false;
	conf->mode = (cr0_reg & CR0_BOOT_MAN_NS) ? true : false;

	conf->num_channels =  shift_and_mask(cr0_reg,
			CR0_NUM_CHANNELS_SH, CR0_NUM_CHANNELS_MK) + 1;

	conf->num_perif_req = shift_and_mask(cr0_reg,
			CR0_NUM_PERIF_REQ_SH, CR0_NUM_PERIF_REQ_MK) + 1;

	conf->num_events = shift_and_mask(cr0_reg,
			CR0_NUM_EVENT_SHIFT, CR0_NUM_EVENT_MASK) + 1;
}

struct CRD_conf {
	u32 bus_width;
	u32 buf_depth;
};

static void CRD_read_conf(struct CRD_conf *conf)
{
	u32 crd_reg, tmp;

	crd_reg = *((u32 *)(uintptr_t)(g_pl330Status->regs + CRD));

	tmp = shift_and_mask(crd_reg,
			CRD_BUS_WIDTH_SHIFT, CRD_BUS_WIDTH_MASK);
	conf->bus_width = 8 * (1 << tmp);

	conf->buf_depth = shift_and_mask(crd_reg,
			CRD_BUF_DEPTH_SHIFT, CRD_BUF_DEPTH_MASK) + 1;
}

static inline u32 insert_DMAEND(u8 *buffer)
{
	buffer[0] = DMAEND;

	DBG_PRT("DMAEND\n");

	return DMAEND_SIZE;
}

static inline u32 insert_DMAMOV(u8 *buffer, enum DMAMOV_type type,
		u32 dst)
{
	buffer[0] = DMAMOV;
	switch(type) {
		case SAR:
			buffer[1] = _SAR;
			break;
		case CCR:
			buffer[1] = _CCR;
			break;
		case DAR:
			buffer[1] = _DAR;
			break;
		default:
			qw_printf("insert_DMAMOV()\n");
	}
	u32 *ptr = (u32 *)&buffer[2];
	*ptr = dst;

	return DMAMOV_SIZE;
}

static inline u32 insert_DMAFLUSHP(u8 *buf, u8 peri)
{

	buf[0] = DMAFLUSHP;

	peri &= 0x1f;
	peri <<= 3;
	buf[1] = peri;

	return DMAFLUSHP_SIZE;
}

static inline u32 insert_DMALP(u8 *buffer, enum DMA_LOOP_REGISTER type,
		u8 count)
{
	switch(type) {
		case LOOP_CNT_0_REG:
			buffer[0] = DMALP;
			break;
		case LOOP_CNT_1_REG:
			buffer[0] = DMALP | (1 << 1);
			break;
		default:
			qw_printf("insert_DMALP()\n");
	}

	buffer[1] = count - 1;

	DBG_PRT("DMALP LOOP_CNT_%d_REG, count: %d\n", type, count);

	return DMALP_SIZE;
}

struct args_DMALPEND {
	enum request_type type;  // burst, single
	enum DMA_LOOP_REGISTER loop_cnt_num;
	u8 backflip_jump;
};

static inline u32 insert_DMALPEND(u8 *buffer, enum LOOP_START_TYPE type,
		struct args_DMALPEND *args)
{
	buffer[0] = DMALPEND;
	u8 lc = args->loop_cnt_num;

	switch(type) {
		case BY_DMALP:
			// TODO add "forever" support
			// set by dmalp
			buffer[0] |= 1 << 4;
			// set dma loop register
			buffer[0] |= lc << 2;

			switch(args->type) {
			case SINGLE:
				buffer[0] |= (0 << 1) | (1 << 0);
			case BURST:
				buffer[0] |= (1 << 1) | (1 << 0);
			case ALWAYS:
				break;
			default:
				qw_printf("insert_DMALPEND(), type error\n");
			}

			buffer[1] = args->backflip_jump;
			break;
		case BY_DMALPFE:
			/*TODO*/
			break;
		default:
			qw_printf("insert_DMALPEND()\n");
	}

	return DMALPEND_SIZE;
}

// TODO SINGLE and BURST cases
static inline u32 insert_DMALD(u8 *buf,enum request_type type)
{
	buf[0] = DMALD;

	if (type == SINGLE)
		buf[0] |= (0 << 1) | (1 << 0);
	else if (type == BURST)
		buf[0] |= (1 << 1) | (1 << 0);

	return DMALD_SIZE;
}

static inline u32 insert_DMALDP(u8 *buf,enum request_type type, u8 peri)
{

	buf[0] = DMALDP;

	if (type == BURST)
		buf[0] |= (1 << 1);

	peri &= 0x1f;
	peri <<= 3;
	buf[1] = peri;

	return DMALDP_SIZE;
}

// TODO SINGLE and BURST cases
static inline u32 insert_DMAST(u8 *buf,enum request_type type)
{
	buf[0] = DMAST;

	if (type == SINGLE)
		buf[0] |= (0 << 1) | (1 << 0);
	else if (type == BURST)
		buf[0] |= (1 << 1) | (1 << 0);


	return DMAST_SIZE;
}

static inline u32 insert_DMASTP(u8 *buf,enum request_type type, u8 peri)
{

	buf[0] = DMASTP;

	if (type == BURST)
		buf[0] |= (1 << 1);

	peri &= 0x1f;
	peri <<= 3;
	buf[1] = peri;

	return DMASTP_SIZE;
}

static inline u32 insert_DMAWFP(u8 *buf,enum request_type type, u8 peri)
{

	buf[0] = DMAWFP;

	if (type == SINGLE)
		buf[0] |= (0 << 1) | (0 << 0);
	else if (type == BURST)
		buf[0] |= (1 << 1) | (0 << 0);
	else
		buf[0] |= (0 << 1) | (1 << 0);

	peri &= 0x1f;
	peri <<= 3;
	buf[1] = peri;

	return DMAWFP_SIZE;
}


static inline u32 insert_DMARMB(u8 *buf)
{
	buf[0] = DMARMB;
	return DMARMB_SIZE;
}

static inline u32 insert_DMAWMB(u8 *buf)
{
	buf[0] = DMAWMB;
	return DMAWMB_SIZE;
}

static inline u32 insert_DMAGO(u8 *buf, u8 channel,
		u32 address, unsigned char nonsecure)
{
	buf[0] = DMAGO;
	buf[0] |= (nonsecure) ? (1 << 1) : (0 << 1);
	buf[1] = channel & 0x7;

	*((u32 *)&buf[2]) = address;

	return DMAGO_SIZE;
}

static inline u32 insert_DMASEV(u8 *buf, u8 event)
{
	buf[0] = DMASEV;

	event &= 0x1f;
	buf[1] = (event << 3); // see page 4-15

	DBG_PRT("DMASEV event: %d\n", event);

	return DMASEV_SIZE;
}

static inline u32 insert_DMAKILL(u8 *buf)
{
	buf[0] = DMAKILL;

	return DMAKILL_SIZE;
}

static inline void qw_dma330_submitToDBGINST(u8 *dbg_instrs, u32 thread_id)
{
	u32 val;

	val = (dbg_instrs[0] << 16) | (dbg_instrs[1] << 24);
	if(thread_id < MANAGER_ID) {
		val |= (1 << 0);
		val |= (thread_id << 8);
	}

	dma330_write_reg(val, g_pl330Status->regs + DBGINST0);

	val = dbg_instrs[5];
	val = val << 8 | dbg_instrs[4];
	val = val << 8 | dbg_instrs[3];
	val = val << 8 | dbg_instrs[2];
	dma330_write_reg(val, g_pl330Status->regs + DBGINST1);

	asm("dsb ish");
	dma330_write_reg(0,g_pl330Status->regs + DBGCMD);
}

static unsigned char qw_dma330_isIdle(void)
{
	u32 regValue;
	regValue = dma330_read_reg(g_pl330Status->regs + DBGSTATUS);
	if (regValue & DBG_BUSY_MASK){
		return false;
	} else {
		return true;
	}
}

static inline void qw_dma330_prepareCCR(u32 * ccr, struct req_config *config)
{
	*ccr = 0;

	*ccr |= (((config->burst_len - 1) & 0xf) << CCR_SRCBURSTLEN_SHIFT);
	*ccr |= (((config->burst_len - 1) & 0xf) << CCR_DSTBURSTLEN_SHIFT);

	*ccr |= (((config->burst_size) & 0x7) << CCR_SRCBURSTSIZE_SHIFT);
	*ccr |= (((config->burst_size) & 0x7) << CCR_DSTBURSTSIZE_SHIFT);

	if(config->src_inc)
		*ccr |= CCR_SRCINC;
	if(config->dst_inc)
		*ccr |= CCR_DSTINC;

	/* We set same protection levels for Src and DST for now */
	if (config->privileged)
		*ccr |= CCR_SRCPRI | CCR_DSTPRI;
	if (!config->force_secure_axi_aces && config->nonsecure)
		*ccr |= CCR_SRCNS | CCR_DSTNS;
	if (config->insnaccess)
		*ccr |= CCR_SRCIA | CCR_DSTIA;

	*ccr |= ((config->src_cache_ctrl & 0x7) << CCR_SRCCACHECTRL_SHIFT);
	*ccr |= ((config->dst_cache_ctrl & 0x7) << CCR_DSTCACHECTRL_SHIFT);
}

static inline int _ldst_memtomem(uint8_t buf[])
{
	int off = 0;

#if 0
	off += insert_DMALD(&buf[off], ALWAYS);
	off += insert_DMAST(&buf[off], ALWAYS);
#else
	off += insert_DMALD(&buf[off], ALWAYS);
	off += insert_DMARMB( &buf[off]);
	off += insert_DMAST( &buf[off], ALWAYS);
	off += insert_DMAWMB( &buf[off]);
#endif
	return off;
}

static inline uint32_t _emit_load(uint8_t buf[], enum request_type cond, enum transfer_type direction, uint8_t peri)
{
	int off = 0;

	switch (direction) {
	case DMA_MEM_TO_MEM:
		/* fall through */
	case DMA_MEM_TO_DEV:
		off += insert_DMALD( &buf[off], cond);
		break;

	case DMA_DEV_TO_MEM:
		if (cond == ALWAYS) {
			off += insert_DMALDP( &buf[off], SINGLE,
				peri);
			off += insert_DMALDP( &buf[off], BURST,
				peri);
		} else {
			off += insert_DMALDP( &buf[off], cond,
				peri);
		}
		break;

	default:
		/* this code should be unreachable */
		WARN_ON(1);
		break;
	}

	return off;
}

static inline uint32_t _emit_store(uint8_t buf[], enum request_type cond, enum transfer_type direction, uint8_t peri)
{
	int off = 0;

	switch (direction) {
	case DMA_MEM_TO_MEM:
		/* fall through */
	case DMA_DEV_TO_MEM:
		off += insert_DMAST( &buf[off], cond);
		break;

	case DMA_MEM_TO_DEV:
		if (cond == ALWAYS) {
			off += insert_DMASTP( &buf[off], SINGLE,
				peri);
			off += insert_DMASTP( &buf[off], BURST,
				peri);
		} else {
			off += insert_DMASTP( &buf[off], cond,
				peri);
		}
		break;

	default:
		/* this code should be unreachable */
		WARN_ON(1);
		break;
	}

	return off;
}

static inline uint32_t _ldst_peripheral(uint8_t buf[], enum request_type cond, struct req_config *config)
{
	u32 off = 0;

	off += insert_DMAWFP( &buf[off], cond, config->peri);
	off += _emit_load( &buf[off], cond, config->t_type,
			config->peri);
	off += _emit_store( &buf[off], cond, config->t_type,
			config->peri);

	return off;
}

static uint32_t _bursts(u8 *buf, struct req_config *config)
{
	uint32_t off = 0;

	switch (config->t_type) {
	case DMA_MEM_TO_DEV:
		/* fall through */
	case DMA_DEV_TO_MEM:
		off += _ldst_peripheral(&buf[off], BURST, config);
		break;

	case DMA_MEM_TO_MEM:
		off += _ldst_memtomem( &buf[off]);
		break;

	default:
		/* this code should be unreachable */
		WARN_ON(1);
		break;
	}

	return off;
}

/*
 * transfer dregs with single transfers to peripheral, or a reduced size burst
 * for mem-to-mem.
 */
static uint32_t _dregs(u8 *buf, struct req_config *config, int transfer_length)
{
	int off = 0;
	int dregs_ccr;

	if(transfer_length == 0)
		return 0;

	switch (config->t_type) {
	case DMA_MEM_TO_DEV:
		/* fall through */
	case DMA_DEV_TO_MEM:
		off += _ldst_peripheral(&buf[off], SINGLE, config);
		break;

	case DMA_MEM_TO_MEM:
		dregs_ccr = config->ccr;

		dregs_ccr &= ~((0xf << CCR_SRCBURSTLEN_SHIFT) |
			(0xf << CCR_DSTBURSTLEN_SHIFT));
		dregs_ccr |= (((transfer_length - 1) & 0xf) <<
			CCR_SRCBURSTLEN_SHIFT);
		dregs_ccr |= (((transfer_length - 1) & 0xf) <<
			CCR_DSTBURSTLEN_SHIFT);

		off += insert_DMAMOV( &buf[off], CCR, dregs_ccr);
		off += _ldst_memtomem( &buf[off]);
		break;

	default:
		/* this code should be unreachable */
		WARN_ON(1);
		break;
	}

	return off;
}

static uint32_t _inner_outer_loop(u8 *buf, u32 in_cnt, u32 out_cnt,struct req_config *config)
{
	int off = 0;
	int out_off, in_off = 0;
	struct args_DMALPEND args;

	if(out_cnt > 1) {
		// outer loop : LOOP_CNT_1_REG
		off += insert_DMALP(&buf[off], LOOP_CNT_1_REG, out_cnt);
		out_off = off;
		DBG_PRT("outer_loop_off:%u, cnt: %u\n", out_off, out_cnt);
	}
	// inner loop : LOOP_CNT_0_REG
	off += insert_DMALP(&buf[off], LOOP_CNT_0_REG, in_cnt);
	in_off = off;
	DBG_PRT("inner_loop_off:%u, cnt: %u\n", in_off, in_cnt);

	/*
	 * Load&Store operations
	 * */
	off += _bursts(&buf[off], config);

	// insert end of inner loop
	args.type = ALWAYS;
	args.loop_cnt_num = LOOP_CNT_0_REG;
	args.backflip_jump = off - in_off;
	off += insert_DMALPEND(&buf[off], BY_DMALP, &args);
	DBG_PRT("inner_loop_end:%u, backjmp: %d\n", off, args.backflip_jump);

	if(out_cnt > 1) {
		args.type = ALWAYS;
		args.loop_cnt_num = LOOP_CNT_1_REG;
		args.backflip_jump = off - out_off;
		off += insert_DMALPEND(&buf[off], BY_DMALP, &args);
		DBG_PRT("outer_loop_end:%u, backjmp: %d\n", off, args.backflip_jump);
	}

	return off;
}

static int _loops(u8 *buf_cmds, struct req_config *config, uint32_t burst_count)
{
	int off = 0;
	/*
	 * full loop means two nested loop of
	 * 256 iterations each
	 * */
	int full_loop_cnt = 0;
	int full_loop_len = 65536; // 256*256
	unsigned long remaining_burst = 0;

	DBG_PRT("set up loops:\n");
	DBG_PRT("burst_cnt:%u\n", burst_count);

	full_loop_cnt = burst_count / full_loop_len;
	remaining_burst = burst_count % full_loop_len;

	DBG_PRT("full_loop_cnt:%u\n", full_loop_cnt);
	DBG_PRT("remaining_burst:%lu\n", remaining_burst);

	while(full_loop_cnt--) {
		off +=  _inner_outer_loop(&buf_cmds[off], 256, 256, config);
	}

	// there could be n < 256*256 bursts left
	// TODO add loop to handle more than one full loop
	if(remaining_burst >= 256) {
		off += _inner_outer_loop(&buf_cmds[off], 256, remaining_burst/256, config);
		remaining_burst %= 256;
		DBG_PRT("remaining_burst_1:%lu\n", remaining_burst);
	}

	// there could be n < 256 bursts left
	if(remaining_burst) {
		off += _inner_outer_loop(&buf_cmds[off], remaining_burst, 0, config);
	}

	return off;
}

/*
 * insert required commands to set up the request.
 * */
int qw_dma330_generateCmdBufFromReq(u8 *cmds_buf, struct req_config *config)
{
	u32 off = 0;
	uint32_t size = config->size;
	uint32_t bursts;
	int num_dregs;

	u32 ccr_conf;
	qw_dma330_prepareCCR(&ccr_conf, config);
	config->ccr = ccr_conf;

	bursts = BYTE_TO_BURST(size, ccr_conf);
	num_dregs = (size - BURST_TO_BYTE(bursts, ccr_conf)) / BRST_SIZE(ccr_conf);

	DBG_PRT("burst: %d, remaining: %d\n", bursts, num_dregs);

	// add instructions to configure CCR, SAR and DAR
	off += insert_DMAMOV(cmds_buf, CCR, ccr_conf);
	off += insert_DMAMOV(&cmds_buf[off], SAR, config->src);
	off += insert_DMAMOV(&cmds_buf[off], DAR, config->dst);

	// set up loops, if any TODO handle src and dst burst size/length
	off += _loops(&cmds_buf[off], config, bursts);
	off += _dregs(&cmds_buf[off], config, num_dregs);

	// see the event enabled in enable_int_for_req()
	off += insert_DMASEV(&cmds_buf[off], config->chan_id);

	// terminate transaction
	off += insert_DMAEND(&cmds_buf[off]);

	DBG_PRT("code size %d\n", off);
	return off;
}


void qw_dma330_prep_memcpy(struct req_config *config, uintptr_t src, uintptr_t dst, uint32_t size)
{
	int burst;
	config->src = (u32)(uintptr_t)src;
	config->dst = (u32)(uintptr_t)dst;
	config->size = size;

	burst = g_pl330Status->ccfg.data_bus_width / 8;
	while ((src | dst | size) & (burst - 1))
		burst /= 2;

	config->burst_size = 0;
	while (burst != (1 << config->burst_size))
		config->burst_size++;

	DBG_PRT("burst size %d\n" ,burst);

	config->burst_len = CCR_BURSTLEN_MAX;
	config->src_inc = config->dst_inc = INC_DEF_VAL;
	config->src_cache_ctrl = config->dst_cache_ctrl = CCR_CACHECTRL_DEF_VAL;

	config->privileged		= 0;

	if(g_pl330Status->ccfg.non_secure_mode & DMAC_MODE_NS)
		config->nonsecure		= 1;
	else
		config->nonsecure		= 0;

	config->insnaccess		= 0;
	config->t_type 			= DMA_MEM_TO_MEM;

	config->callback 		= NULL;
	config->user_data 		= NULL;

	config->force_secure_axi_aces = false;
	config->force_mcpu_code = false;
}

/*
 * For the channel num. i, we activate the event i
 * */
void qw_dma330_enableIntReq(struct req_config *config)
{
	u32 intRegValue;
	if(config->int_fin) {

		intRegValue = dma330_read_reg(g_pl330Status->regs + INTEN);
		intRegValue |= (1 << config->chan_id);
		dma330_write_reg(intRegValue,g_pl330Status->regs + INTEN);
	}
}

int qw_dma330_submitReq(u32 address, struct req_config *conf)
{
	u8 ins_debug[6] = {0, 0, 0, 0, 0, 0};
	if(!qw_dma330_isIdle()) {
		return -1;
	}

	// enable interrupt
	qw_dma330_enableIntReq(conf);

	insert_DMAGO(ins_debug, conf->chan_id, address, conf->nonsecure);

	g_pl330Status->ch_threads[conf->chan_id].callback =
				conf->callback;
	g_pl330Status->ch_threads[conf->chan_id].user_data =
				conf->user_data;

	qw_dma330_submitToDBGINST(ins_debug, MANAGER_ID);
	return 0;
}


static u32 qw_dma330_getThreadState(u32 id)
{
	u32 state_reg, state;
	if(id == MANAGER_ID) {
		state_reg = dma330_read_reg(g_pl330Status->regs + DSR);
		state = shift_and_mask(state_reg,
				DSR_STATUS_SHIFT, DSR_STATUS_MASK);
		switch(state) {
		case STOPPED:
		case EXECUTING:
		case CACHE_MISS:
		case UPDATING_PC:
		case WAIT_EVENT:
		case FAULTING:
			return state;
		default:
			return INVALID_STATE;
		}
	} else {
		state_reg = dma330_read_reg(g_pl330Status->regs + CSR(id));
		state = shift_and_mask(state_reg,
				CSR_CHANNEL_STATUS_SH, CSR_CHANNEL_STATUS_MK);
		switch(state) {
		case STOPPED:
		case EXECUTING:
		case CACHE_MISS:
		case UPDATING_PC:
		case WAIT_EVENT:
		case BARRIER:
		case WAIT_PERIPH:
		case KILLING:
		case COMPLETING:
		case FAULT_COMPLETING:
		case FAULTING:
			return state;
		default:
			return INVALID_STATE;
		}
	}
}

static void qw_dma330_stopThread(u32 id)
{
	u8 ins_debug[6] = {0, 0, 0, 0, 0, 0};
	u32 intRegValue, state;

	if(id > MANAGER_ID) {
		qw_printf("invalid channel id\n");
	}

	state = qw_dma330_getThreadState(id);
	if(state == INVALID_STATE) {
		qw_printf("invalid state\n");
	}

	switch(state) {
		case STOPPED:
		case KILLING:
		case COMPLETING:
			// nothing to do
			return;
		default:
			break;
	}

	// stop interrupt for channel id
	intRegValue = dma330_read_reg(g_pl330Status->regs + INTEN);
	intRegValue &= ~(1 << g_pl330Status->ch_threads[id].event_id);
	dma330_write_reg(intRegValue, g_pl330Status->regs + INTEN);
	DBG_PRT("closing event %d for thread %d", g_pl330Status->ch_threads[id].event_id, id);

	insert_DMAKILL(ins_debug);
	qw_dma330_submitToDBGINST(ins_debug, id);
}

int qw_dma330_requestChannel(uint8_t *chan)
{
	int i;
	int ret = -1;

	i = *chan;
	if(i < g_pl330Status->channels)
	{
		if(g_pl330Status->ch_threads[i].state != FREE)
			qw_printf("channel is already allocated\n");

		// the event is already allocated
		if(g_pl330Status->allocated_events & (1 << i))
			qw_printf("event already allocated\n");

		g_pl330Status->ch_threads[i].state = ALLOCATED;
		g_pl330Status->ch_threads[i].event_id = i;
		g_pl330Status->allocated_events |= (1 << i);
		ret = 0;
	}
	else
	{
		for(i = 0; i <= g_pl330Status->channels; i++) {
			if(g_pl330Status->ch_threads[i].state == FREE) {
				ret = 0;
				*chan = i;
				if(g_pl330Status->allocated_events & (1 << i)) {
					// the event is already allocated
					qw_printf("event already allocated\n");
				}
				g_pl330Status->ch_threads[i].state = ALLOCATED;
				g_pl330Status->ch_threads[i].event_id = i;
				g_pl330Status->allocated_events |= (1 << i);
				break;
			}
		}
	}
	DBG_PRT("allocated thread %d\n", *chan);

	return ret;
}

void qw_dma330_releaseChannel(uint8_t id)
{
	qw_dma330_stopThread(id);

	g_pl330Status->allocated_events 		&= ~(1 << id);
	g_pl330Status->ch_threads[id].state 	= FREE;
	g_pl330Status->ch_threads[id].event_id 	= -1;
}

void qw_dma330_stop_threads()
{
	int i;

	// stop the manager
	qw_dma330_stopThread(MANAGER_ID);

	// stop all the channels
	for(i = 0; i < g_pl330Status->channels; i++) {
		qw_dma330_stopThread(i);
	}
}

void qw_dma330_remove()
{
	qw_dma330_stop_threads();
	free(g_pl330Status->ch_threads);
	free(g_pl330Status->mcpu_cod);
	free(g_pl330Status);
}

/**/
int qw_dma330_init(u32 base_regs)
{
	struct CRD_conf crd_conf;
	struct CR0_conf cr0_conf;
	int i;

	g_pl330Status = (struct pl330_status *)malloc(sizeof(struct pl330_status));

	if(g_pl330Status) {
		memset(g_pl330Status, 0, sizeof(struct pl330_status));
	}
	else {
		qw_printf("error during init\n");
	}

	g_pl330Status->regs = base_regs;

	g_pl330Status->allocated_events = 0;

	// grab number of channels available
	CRD_read_conf(&crd_conf);
	CR0_read_conf(&cr0_conf);

	g_pl330Status->ccfg.data_bus_width = crd_conf.bus_width;
	g_pl330Status->ccfg.num_events = cr0_conf.num_events;

	g_pl330Status->channels = cr0_conf.num_channels;
	DBG_PRT("device init, num channel: %d\n", g_pl330Status->channels);

	g_pl330Status->ch_threads = malloc(g_pl330Status->channels*sizeof(struct channel_thread));
	struct channel_thread free_state = {FREE, -1};
	for(i = 0; i < g_pl330Status->channels; i++) {
		g_pl330Status->ch_threads[i] = free_state;
	}

	if (cr0_conf.mode)
		g_pl330Status->ccfg.non_secure_mode |= DMAC_MODE_NS;
	else
		g_pl330Status->ccfg.non_secure_mode &= ~DMAC_MODE_NS;

	g_pl330Status->mcpu_cod_size = 1024;
	g_pl330Status->mcpu_cod = (u8 *)malloc(g_pl330Status->channels * g_pl330Status->mcpu_cod_size);
	if(g_pl330Status->mcpu_cod == NULL)
	{
		qw_printf("malloc failed\n");
		return -1;
	}

	return 0;
}

void qw_dma330_update()
{
	uint32_t val;
	int ev;

	/*fault proc*/
	val = dma330_read_reg(g_pl330Status->regs + FSRD);
	if(val & 0x1)
	{
		g_pl330Status->reset_mngr = val;
		val = dma330_read_reg(g_pl330Status->regs + FTRD);
		qw_printf("Manager fault: %08x\n", val);
	}

	val = dma330_read_reg(g_pl330Status->regs + FSRC);
	val &= ((1 << g_pl330Status->channels) - 1);
	g_pl330Status->reset_chan |= val;
	if (val) //chan fault happen
	{
		int i = 0;
		while (i < g_pl330Status->channels)
		{
			if (val & (1 << i))
			{
				qw_printf( "Reset Channel-%d\t CS-%08x FTC-%08x\n",
					i, dma330_read_reg(g_pl330Status->regs + CSR(i)),
					dma330_read_reg(g_pl330Status->regs + FTR(i)));
				qw_dma330_stopThread(i);
			}
			i++;
		}
	}

	/* Check which event happened i.e, thread notified */
	val = dma330_read_reg(g_pl330Status->regs + INT_EVENT_RIS);
	if(val & ~((1 << g_pl330Status->ccfg.num_events) - 1))
	{
		g_pl330Status->reset_dmac = true;
		qw_printf("%s:%d Unexpected!\n", __func__, __LINE__);
		return;
	}

	for (ev = 0; ev < g_pl330Status->ccfg.num_events; ev++) {
		if (val & (1 << ev)) { /* Event occurred */
			struct channel_thread *thrd;
			uint32_t inten 	= dma330_read_reg(g_pl330Status->regs + INTEN);

			/* Clear the event */
			if(inten & (1 << ev))
			{
				dma330_write_reg((1 << ev), g_pl330Status->regs + INTCLR);
			}

			/**/
			thrd = &g_pl330Status->ch_threads[ev];
			if(thrd->state == FREE)
			{
				qw_printf("events unknown\n");
				continue;
			}

			if(thrd->callback)
				thrd->callback(thrd->user_data);
		}
	}
}

int irq_num[3] = { INT_ID_DMA0_INT0, INT_ID_DMA0_INT1, INT_ID_DMA0_ABORT};
void qw_arm_dmac_irqhandler(uint32_t data)
{
	int __maybe_unused irq = data;
	DBG_PRT("recv irq: %d\n", irq);
	qw_dma330_update();
}

void DMAC1_IRQHandler(void *data)
{
    int __maybe_unused irq_index = *(int *)data;

    //dw_dmac_irqhandler(irq_index);
}

void DMAC0_IRQHandler(void *data)
{
    int irq_index = *(int *)data;

    qw_arm_dmac_irqhandler(irq_index);
}


struct {
    uint32_t base;
    uint32_t irq;
    void *handler;
}
static const sg_dmac_config[] = {
    {DMAC0_SEC_BASE, INT_ID_DMA0_INT0, DMAC0_IRQHandler},
    {DMAC0_USER_BASE, INT_ID_DMA0_INT0, DMAC0_IRQHandler},
    {DMAC1_BASE, INT_ID_DMA1, DMAC1_IRQHandler},
};

int32_t target_get_dmac(int32_t idx, uint32_t *base, uint32_t *irq, void **handler)
{
	if(idx > 2 || idx < 0)
	{
		return -1;
	}
    if (base != NULL) {
        *base = sg_dmac_config[idx].base;
    }

    if (irq != NULL) {
        *irq = sg_dmac_config[idx].irq;
    }

    if (handler != NULL) {
        *handler = sg_dmac_config[idx].handler;
    }

    return idx;
}



int qw_dma_pl330_init(bool nonsec)
{
	int ret;
    uint32_t base;
    uint32_t irq;
	void *handler;
	struct IrqTableEntry Entry;
	int idx;
	uint32_t temp;

	smmu_dma0_bypass();
	smmu_dma0_lp_en();
	//check
	while(!smmu_dma0_is_idle());

	if(nonsec)
	{
		dma330_write_reg(0x1, RGU_BASE + 0x20);
		//[1] manager_ns = 1, [7:2] irq_ns = 0x3f;
		temp = (0x1<<1) | (0x3f<<2) | (0x3 << 8);
		dma330_write_reg(temp, MCU_SGRF_BASE + 0x30);
		dma330_write_reg(0x0, RGU_BASE + 0x20);
	}
	else
	{
		dma330_write_reg(0x1, RGU_BASE + 0x20);
		//[1] manager_ns = 0, [7:2] irq_ns = 0,
		dma330_write_reg(0x0, MCU_SGRF_BASE + 0x30);
		dma330_write_reg(0x0, RGU_BASE + 0x20);
	}

	smmu_dma0_lp_quit();

	//idx=0是secure模式, =1 nonsec模式
	idx = nonsec?1:0;
	ret = target_get_dmac(idx, &base, &irq, &handler);
	if (ret == -1) {
		return -1;
	}

	Entry.Handler 	= handler;
	Entry.data 		= &irq_num[0];
    drv_irq_register(INT_ID_DMA0_INT0, &Entry);
	Entry.data 		= &irq_num[1];
    drv_irq_register(INT_ID_DMA0_INT1, &Entry);
	Entry.data 		= &irq_num[2];
    drv_irq_register(INT_ID_DMA0_ABORT, &Entry);

    drv_irq_enable(INT_ID_DMA0_INT0);  //INT[0]
    drv_irq_enable(INT_ID_DMA0_INT1);  //INT[1]
    drv_irq_enable(INT_ID_DMA0_ABORT);  //abort

	/**/
	return qw_dma330_init(base);
}

int qw_dma_pl330_submit(struct req_config *config)
{
	uint8_t *cmd_buf;
	cmd_buf = &g_pl330Status->mcpu_cod[g_pl330Status->mcpu_cod_size * config->chan_id];
	if(config->force_mcpu_code)
	{
		cmd_buf = (uint8_t *)config->force_mcpu_code_addr;
	}

	qw_dma330_generateCmdBufFromReq(cmd_buf, config);
	return qw_dma330_submitReq((u32)(uintptr_t)cmd_buf, config);
}


uint32_t qw_dma330_abort_status()
{
	uint32_t temp = 0;
	if(g_pl330Status->reset_mngr)
		temp |= 0x1;

	if(g_pl330Status->reset_chan)
		temp |= 0x2;

	if(g_pl330Status->reset_dmac)
		temp |= 0x4;

	return temp;
}

