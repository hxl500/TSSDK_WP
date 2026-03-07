#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <unistd.h>

//#include "stub.h"
//#include "soc.h"
//#include "base_type.h"
#include "qw_dma330.h"
//#include "errno-base.h"
//#include "debug.h"
#include "sys_exception.h"
//#include "sys_sec_region.h"

#define qw_printf printf

static inline void delay(int cnt)
{
	int i;
	for(i=0; i<cnt; i++)
		asm("nop");
}

static volatile uint32_t completed;
void done_callback(void *user_data)
{
	//struct req_config *config_p = user_data;
	completed = 1;
}

int qw_dma_testcase_once(bool nonsec, int chan, uint8_t *src, uint8_t *dst, uint32_t size)
{
	int ret = 0;
	struct req_config config;
	uint8_t channel_id;

	/*nonsec = true, sec = false*/
	ret = qw_dma_pl330_init(nonsec);
	if(ret)
	{
		return ret;
	}
	memset(&config, 0, sizeof(config));

	/**/
	qw_dma330_prep_memcpy(&config, (uintptr_t)src, (uintptr_t)dst, size);

	channel_id = (uint8_t)chan;
	ret = qw_dma330_requestChannel(&channel_id);
	if(ret < 0) {
		qw_printf("fail! No channels available!\n");
		return ret;
	}

	if(channel_id < DMA330_INT_NUM) {
		//config.int_fin	= true;
		config.int_fin	= false;
	}

	else
		config.int_fin	= false;

	config.callback = done_callback;
	config.user_data = &config;
	config.chan_id = channel_id;

	completed = 0;
	qw_dma_pl330_submit(&config);
	chan = channel_id;
	qw_printf("chan%d is working\n", chan);

	/**/
	while(!completed)
	{
		if(!config.int_fin)
		{
			/*没中断的channel*/
			qw_dma330_update();
			delay(10000);
		}
	}
	qw_printf("done\n");

	int i;
	for(i=0; i<size; i++)
	{
		if(src[i] != dst[i])
		{
			ret = -10;
			qw_printf("diff: [%d] %02x %02x\n", i, src[i], dst[i]);
			break;
		}
	}

	qw_dma330_releaseChannel(channel_id);
	qw_dma330_remove(/*&config*/);
	return ret;
}

#define DMATEST_BLOCK_SIZE 0x100
#define DMATEST_M2M_SIZE (DMATEST_BLOCK_SIZE-10)
u8 test_src[DMATEST_BLOCK_SIZE] = "123456789qwertyuhfgh\n";
u8 test_dst[DMATEST_BLOCK_SIZE] = "mmmmmmmmmmmmmmmmmmmm\n";

/*
 * DMA in secure mode accses ddr secure/nonsecure
 * */
int sample_dma_testcase1(void)
{
	qw_printf("In %s test\n", __func__);
	int ret;
	uint8_t *src;
	uint8_t *dst;
	uint8_t patn;

	patn = 0xf;
	src = test_src;
	dst = test_dst;
	memset(src, patn&0xff, DMATEST_BLOCK_SIZE);
	memset(dst, (patn+1)&0xff, DMATEST_BLOCK_SIZE);
	ret = qw_dma_testcase_once(false, 0, src, dst, DMATEST_M2M_SIZE);
	if(ret)
	{
		qw_printf("FAIL\n");
		return ret;
	}

	src = (uint8_t *)0xc0000000;
	dst = (uint8_t *)0xc1000000;
	memset(src, patn&0xff, 0x10000);
	memset(dst, (patn+1)&0xff, 0x10000);
	ret = qw_dma_testcase_once(false, 0, src, dst, 0x10000);
	if(ret)
	{
		qw_printf("FAIL\n");
		return ret;
	}
	qw_printf("SUCC\n");
	return 0;
}

/*
 * DMA in nonsecure mode accses ddr secure/nonsecure
 * */
int sample_dma_testcase2(void)
{
	qw_printf("In %s test\n", __func__);
	int ret;
	uint8_t *src;
	uint8_t *dst;
	uint8_t patn;

	patn = 0xf;
	src = (uint8_t *)0x10000000;
	dst = (uint8_t *)0x51000000;
	memset(src, patn&0xff, 0x811110);
	memset(dst, (patn+1)&0xff, 0x811110);
	ret = qw_dma_testcase_once(true, 0, src, dst, 0x811110);
	if(ret)
	{
		qw_printf("FAIL\n");
		return ret;
	}
#if 0
	src = (uint8_t *)0x60000000;
	dst = (uint8_t *)0x61000000;
	memset(src, patn&0xff, 0x10000);
	memset(dst, (patn+1)&0xff, 0x10000);
	ret = qw_dma_testcase_once(true, 0, src, dst, 0x10000);
	if(ret)
	{
		qw_printf("FAIL\n");
		return ret;
	}
	qw_printf("SUCC\n");
#endif

	return 0;
}

/*
 * DMA in secure mode accses mcu_sram secure/nonsecure
 * */
int sample_dma_testcase3(void)
{
	qw_printf("In %s test\n", __func__);
	int ret;
	uint8_t *src;
	uint8_t *dst;
	uint8_t patn;

	patn = 0xf;
	src = (uint8_t *)0xf1200000;
	dst = (uint8_t *)0xb0000000;
	memset(src, patn&0xff, DMATEST_BLOCK_SIZE);
	memset(dst, (patn+1)&0xff, DMATEST_BLOCK_SIZE);
	ret = qw_dma_testcase_once(false, 1, src, dst, DMATEST_M2M_SIZE);
	if(ret)
	{
		qw_printf("FAIL\n");
		return ret;
	}

	src = (uint8_t *)0xf1200000;
	dst = (uint8_t *)0xc3000000;
	memset(src, patn&0xff, DMATEST_BLOCK_SIZE);
	memset(dst, (patn+1)&0xff, DMATEST_BLOCK_SIZE);
	ret = qw_dma_testcase_once(false, 1, src, dst, DMATEST_M2M_SIZE);
	if(ret)
	{
		qw_printf("FAIL\n");
		return ret;
	}
	qw_printf("SUCC\n");
	return 0;
}

/*
 * DMA in nonsecure mode accses mcu_sram secure/nonsecure
 * */
int sample_dma_testcase4(void)
{
	qw_printf("In %s test\n", __func__);
	int ret;
	uint8_t *src;
	uint8_t *dst;
	uint8_t patn;

	patn = 0xf;
	src = (uint8_t *)0xf1200000;
	dst = (uint8_t *)0xb0000000;
	memset(src, patn&0xff, DMATEST_BLOCK_SIZE);
	memset(dst, (patn+1)&0xff, DMATEST_BLOCK_SIZE);
	ret = qw_dma_testcase_once(true, 1, src, dst, DMATEST_M2M_SIZE);
	if(ret)
	{
		qw_printf("FAIL\n");
		return ret;
	}

	src = (uint8_t *)0xf1200000;
	dst = (uint8_t *)0xc1000000;
	memset(src, patn&0xff, DMATEST_BLOCK_SIZE);
	memset(dst, (patn+1)&0xff, DMATEST_BLOCK_SIZE);
	ret = qw_dma_testcase_once(true, 1, src, dst, DMATEST_M2M_SIZE);
	if(ret)
	{
		qw_printf("FAIL\n");
		return ret;
	}
	qw_printf("SUCC\n");
	return 0;
}

int qw_dma_testcase_abort(bool nonsec, int chan, uint8_t *src, uint8_t *dst, uint32_t size)
{
	int ret = 0;
	struct req_config config;
	uint8_t channel_id;

	/*nonsec = true, sec = false*/
	ret = qw_dma_pl330_init(nonsec);
	if(ret)
	{
		return ret;
	}
	memset(&config, 0, sizeof(config));

	/**/
	qw_dma330_prep_memcpy(&config, (uintptr_t)src, (uintptr_t)dst, size);

	channel_id = (uint8_t)chan;
	ret = qw_dma330_requestChannel(&channel_id);
	if(ret < 0) {
		qw_printf("fail! No channels available!\n");
		return ret;
	}

	if(channel_id < DMA330_INT_NUM)
		config.int_fin	= true;
	else
		config.int_fin	= false;

	config.callback = done_callback;
	config.user_data = &config;
	config.chan_id = channel_id;
	config.force_secure_axi_aces = true;

	qw_dma_pl330_submit(&config);
	chan = channel_id;
	qw_printf("chan%d is working\n", chan);

	/**/
	while(1)
	{
		if(!config.int_fin)
		{
			/*没中断的channel*/
			qw_dma330_update();
			delay(10000);
		}

		if(qw_dma330_abort_status())
		{
			ret = -110;
			break;
		}
	}
	qw_printf("done\n");


	int i;
	for(i=0; i<size; i++)
	{
		if(src[i] != dst[i])
		{
			qw_printf("diff: [%d] %02x %02x\n", i, src[i], dst[i]);
			break;
		}
	}

	qw_dma330_releaseChannel(channel_id);
	qw_dma330_remove(/*&config*/);
	return ret;
}


/*
 * trigger abort
 * */
int sample_dma_testcase5(void)
{
	qw_printf("In %s test\n", __func__);
	int ret;
	uint8_t *src;
	uint8_t *dst;
	uint8_t patn;

	patn = 0xf;
	src = test_src;
	dst = test_dst;
	memset(src, patn&0xff, DMATEST_BLOCK_SIZE);
	memset(dst, (patn+1)&0xff, DMATEST_BLOCK_SIZE);
	ret = qw_dma_testcase_abort(true, 2, src, dst, DMATEST_M2M_SIZE);
	if(ret != -110)
	{
		qw_printf("FAIL\n");
		return ret;
	}

	qw_printf("SUCC\n");
	return 0;
}

int qw_dma_testcase_aces_code_fail(bool nonsec, int chan, uint8_t *src, uint8_t *dst, uint32_t size)
{
	int ret = 0;
	struct req_config config;
	uint8_t channel_id;

	/*nonsec = true, sec = false*/
	ret = qw_dma_pl330_init(nonsec);
	if(ret)
	{
		return ret;
	}
	memset(&config, 0, sizeof(config));

	/**/
	qw_dma330_prep_memcpy(&config, (uintptr_t)src, (uintptr_t)dst, size);

	channel_id = (uint8_t)chan;
	ret = qw_dma330_requestChannel(&channel_id);
	if(ret < 0) {
		qw_printf("fail! No channels available!\n");
		return ret;
	}

	if(channel_id < DMA330_INT_NUM)
		config.int_fin	= true;
	else
		config.int_fin	= false;

	config.callback = done_callback;
	config.user_data = &config;
	config.chan_id = channel_id;
	config.force_mcpu_code = true;
	config.force_mcpu_code_addr = 0xc0000000;

	completed = 0;
	qw_dma_pl330_submit(&config);
	chan = channel_id;
	qw_printf("chan%d is working\n", chan);

	//need an error
	if(!config.int_fin)
	{
		/*没中断的channel*/
		delay(100000);
		qw_dma330_update();
	}

	//check status

	int i;
	for(i=0; i<size; i++)
	{
		if(src[i] != dst[i])
		{
			ret = -10;
			qw_printf("diff: [%d] %02x %02x\n", i, src[i], dst[i]);
			break;
		}
	}

	qw_dma330_releaseChannel(channel_id);
	qw_dma330_remove(/*&config*/);
	return ret;
}

/*
 * access dma code fail
 * */
int sample_dma_testcase6(void)
{
	qw_printf("In %s test\n", __func__);
	int ret;
	uint8_t *src;
	uint8_t *dst;
	uint8_t patn;

	patn = 0xf;
	src = test_src;
	dst = test_dst;
	memset(src, patn&0xff, DMATEST_BLOCK_SIZE);
	memset(dst, (patn+1)&0xff, DMATEST_BLOCK_SIZE);
	ret = qw_dma_testcase_aces_code_fail(true, 3, src, dst, DMATEST_M2M_SIZE);
	if(ret)
	{
		qw_printf("FAIL\n");
		return ret;
	}

	qw_printf("SUCC\n");
	return 0;
}


/*
 * DMA in secure mode accses aimem secure/nonsecure
 * */
int sample_dma_testcase7(void)
{
	qw_printf("In %s test\n", __func__);
	int ret;
	uint8_t *src;
	uint8_t *dst;
	uint8_t patn;

	patn = 0xf;
	src = (uint8_t *)0xf0000000;
	dst = test_dst;
	memset(src, patn&0xff, DMATEST_BLOCK_SIZE);
	memset(dst, (patn+1)&0xff, DMATEST_BLOCK_SIZE);
	ret = qw_dma_testcase_once(false, 0, src, dst, DMATEST_M2M_SIZE);
	if(ret)
	{
		qw_printf("FAIL\n");
		return ret;
	}

	src = (uint8_t *)0xf0100000;
	dst = (uint8_t *)test_dst;
	memset(src, patn&0xff, DMATEST_BLOCK_SIZE);
	memset(dst, (patn+1)&0xff, DMATEST_BLOCK_SIZE);
	ret = qw_dma_testcase_once(false, 0, src, dst, DMATEST_M2M_SIZE);
	if(ret)
	{
		qw_printf("FAIL\n");
		return ret;
	}
	qw_printf("SUCC\n");
	return 0;
}

/*
 * DMA in nonsecure mode accses aimem secure/nonsecure
 * */
int sample_dma_testcase8(void)
{
	qw_printf("In %s test\n", __func__);
	int ret;
	uint8_t *src;
	uint8_t *dst;
	uint8_t patn;

	patn = 0xf;
	src = (uint8_t *)0xf0000000;
	dst = test_dst;
	memset(src, patn&0xff, DMATEST_BLOCK_SIZE);
	memset(dst, (patn+1)&0xff, DMATEST_BLOCK_SIZE);
	ret = qw_dma_testcase_once(true, 0, src, dst, DMATEST_M2M_SIZE);
	if(ret)
	{
		qw_printf("FAIL\n");
		return ret;
	}

	src = (uint8_t *)0xf0100000;
	dst = (uint8_t *)test_dst;
	memset(src, patn&0xff, DMATEST_BLOCK_SIZE);
	memset(dst, (patn+1)&0xff, DMATEST_BLOCK_SIZE);
	ret = qw_dma_testcase_once(true, 0, src, dst, DMATEST_M2M_SIZE);
	if(ret)
	{
		qw_printf("FAIL\n");
		return ret;
	}
	qw_printf("SUCC\n");
	return 0;
}

typedef struct{
	int chan;
	uint8_t *src;
	uint8_t *dst;
	uint32_t size;
}dmatest_xfer_t;

void multi_done_callback(void *user_data)
{
	struct req_config *config_p = user_data;
	completed &= (~(1<<config_p->chan_id));
}

int qw_dma_testcase_multi(bool nonsec, dmatest_xfer_t *xfer, int num)
{
	int i,ret = 0;
	struct req_config *config_p;
	uint8_t channel_id;

	config_p = malloc(num * sizeof(struct req_config));
	if(config_p == NULL)
	{
		return -2;
	}
	memset(config_p, 0, num * sizeof(struct req_config));

	/*nonsec = true, sec = false*/
	ret = qw_dma_pl330_init(nonsec);
	if(ret)
	{
		goto _multi_chan_test_err1;
	}

	completed = 0;
	/**/
	for(i=0; i< num; i++)
	{
		qw_dma330_prep_memcpy(config_p+i, (uintptr_t)xfer[i].src, (uintptr_t)xfer[i].dst, xfer[i].size);

		channel_id = (uint8_t)xfer[i].chan;
		ret = qw_dma330_requestChannel(&channel_id);
		if(ret < 0) {
			qw_printf("fail! No channels available!\n");
			goto _multi_chan_test_err;
		}

		if(channel_id < DMA330_INT_NUM)
			(config_p+i)->int_fin	= true;
		else
			(config_p+i)->int_fin	= false;

		(config_p+i)->callback = multi_done_callback;
		(config_p+i)->user_data = (config_p+i);
		(config_p+i)->chan_id = channel_id;

		qw_printf("chan%d is ready\n", channel_id);
		completed |= (1<<channel_id);
	}

	for(i=0; i< num; i++)
		qw_dma_pl330_submit(config_p+i);

	/**/
	while(completed & 0x3);

	/**/
	while(completed & 0xf)
	{
		/*没中断的channel*/
		qw_dma330_update();
		delay(10000);
	}

	qw_printf("done\n");

_multi_chan_test_err:
	for(i=0; i< num; i++)
		qw_dma330_releaseChannel((config_p+i)->chan_id);

	qw_dma330_remove();
_multi_chan_test_err1:
	free(config_p);
	return ret;
}

/*
 * DMA multi channel test
 * */
int sample_dma_testcase9(void)
{
	qw_printf("In %s test\n", __func__);
	int ret=0,i,n;
	uint8_t patn;
	dmatest_xfer_t xfer[4] = {
			{0, (uint8_t *)0xb0000000, (uint8_t *)0xb0100000, 0x10000},
			{1, (uint8_t *)0xb1000000, (uint8_t *)0xb1100000, 0x10000},
			{2, (uint8_t *)0xb2000000, (uint8_t *)0xb2100000, 0x10000},
			{3, (uint8_t *)0xb3000000, (uint8_t *)0xb3100000, 0x10000}
	};

	for(i=0; i< 4; i++)
	{
		patn = 0x10 + i;
		memset(xfer[i].src, patn, 0x10000);
		memset(xfer[i].dst, patn+1, 0x10000);
	}

	ret = qw_dma_testcase_multi(false, xfer, 4);
	if(ret)
	{
		qw_printf("FAIL\n");
		return ret;
	}

	for(n=0; n<4; n++)
	{
		for(i=0; i<xfer[n].size; i++)
		{
			if(xfer[n].src[i] != xfer[n].dst[i])
			{
				qw_printf("multi[%d] diff: [%d] %02x %02x\n", n, i, xfer[n].src[i], xfer[n].dst[i]);
				return -1;
			}
		}
	}

	qw_printf("SUCC\n");
	return ret;
}


int sample_dmac_pl330(void)
{
	int ret = 0;
	qw_printf("ss working %d\n", 11);

	//security_ddr_region_en(0, 0xc0000000, 0xc2000000);

	ret |= sample_dma_testcase1();
	ret |= sample_dma_testcase2();
	ret |= sample_dma_testcase3();
	ret |= sample_dma_testcase4();
	ret |= sample_dma_testcase5();
	ret |= sample_dma_testcase6();
	//ret |= sample_dma_testcase7();
	//ret |= sample_dma_testcase8();
	ret |= sample_dma_testcase9();

	return ret;
}




