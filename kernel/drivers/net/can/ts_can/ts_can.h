#ifndef TS_CAN_H_
#define TS_CAN_H_

#include <linux/netdevice.h>
#include <linux/can.h>
#include <linux/can/dev.h>
#include <linux/can/error.h>
#include <linux/can/led.h>
#include <linux/can/rx-offload.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>


#define DRV_NAME			"tscan"



enum tscan_ide
{
	TSCAN_ID_STD,
	TSCAN_ID_EXT,
};

enum tscan_rtr
{
	TSCAN_RTR_DATA,
	TSCAN_RTR_REMOTE,
};


union tscan_mod
{
	struct
	{
		unsigned int reset_mode : 1;			 //[0:0]
		unsigned int listen_only_mode : 1;		 //[1:1]
		unsigned int selftest_mode : 1;			 //[2:2]
		unsigned int appeptance_filter_mode : 1; //[3:3]
		unsigned int sleep_mode : 1;			 //[4:4]
	};
	unsigned int u32;
};

union tscan_cmr
{
	struct
	{
		unsigned int tr : 1;  //[0:0]:transmission request
		unsigned int at : 1;  //[1:1]:abort transmission
		unsigned int rrb : 1; //[2:2]:release receive buffer
		unsigned int cdo : 1; //[3:3]:bit clear data overrun
		unsigned int srr : 1; //[4:4]:self reception request
	};
	unsigned int u32;
};

union tscan_sr
{
	struct
	{
		unsigned int rbs : 1; //[0:0]
		unsigned int dos : 1; //[1:1]
		unsigned int tbs : 1; //[2:2]
		unsigned int tcs : 1; //[3:3]
		unsigned int rs : 1;  //[4:4]
		unsigned int ts : 1;  //[5:5]
		unsigned int es : 1;  //[6:6]
		unsigned int bs : 1;  //[7:7]
	};
	unsigned int u32;
};

union tscan_ir
{
	struct
	{
		unsigned int ri : 1;  //[0:0]
		unsigned int ti : 1;  //[1:1]
		unsigned int ei : 1;  //[2:2]
		unsigned int doi : 1; //[3:3]
		unsigned int wui : 1; //[4:4]
		unsigned int epi : 1; //[5:5]
		unsigned int ali : 1; //[6:6]
		unsigned int bei : 1; //[7:7]
	};
	unsigned int u32;
};

union tscan_ier
{
	struct
	{
		unsigned int rie : 1;  //[0:0]
		unsigned int tie : 1;  //[1:1]
		unsigned int eie : 1;  //[2:2]
		unsigned int doie : 1; //[3:3]
		unsigned int wuie : 1; //[4:4]
		unsigned int epie : 1; //[5:5]
		unsigned int alie : 1; //[6:6]
		unsigned int beie : 1; //[7:7]
	};
	unsigned int u32;
};

union tscan_btr0
{
	struct
	{
		unsigned int brp : 6;  //[0:5]
		unsigned int sjw : 2;  //[6:7]
	};
	unsigned int u32;
};

union tscan_btr1
{
	struct
	{
		unsigned int tseg1 : 4; //[0:3]
		unsigned int tseg2 : 3; //[4:6]
		unsigned int sam : 1;	//[7:7]
	};
	unsigned int u32;
};

union tscan_ocr
{
	struct
	{
		unsigned int ocmode : 2; //[0:1]
	};
	unsigned int u32;
};

union tscan_alc
{
	struct
	{
		unsigned int alc : 5; //[0:4]
	};
	unsigned int u32;
};

union tscan_ecc
{
	struct
	{
		unsigned int segment_code : 5; //[0:4]
		unsigned int direction : 1;	   //[5:5]
		unsigned int error_code : 2;   //[6:7]
	};
	unsigned int u32;
};

union tscan_rbsa
{
	struct
	{
		unsigned int rbsa0 : 1;		//[0:0]
		unsigned int rbsa1 : 1;		//[1:1]
		unsigned int rbsa2 : 1;		//[2:2]
		unsigned int rbsa3 : 1;		//[3:3]
		unsigned int rbsa4 : 1;		//[4:4]
		unsigned int rbsa5 : 1;		//[5:5]
		unsigned int reserved : 26; //[6:31]
	};
	unsigned int u32;
};

union tscan_frame
{
	struct
	{
		unsigned int dlc : 4;		//[0:3]
		unsigned int res : 2;		//[4:5]
		unsigned int rtr : 1;		//[6:6]
		unsigned int ff : 1;		//[7:7]
		unsigned int reserved : 24; //[8:31]
	};
	unsigned int u32;
};

struct standard_frame
{
	union tscan_frame frame_info;
	u32 id[2];
	u32 data[8];
};

struct extended_frame
{
	union tscan_frame frame_info;
	u32 id[4];
	u32 data[8];
};

union tscan_cdr
{
	struct
	{
		unsigned int cdr : 3;		//[0:2]
		unsigned int clock_off : 1; //[3:3]
		unsigned int reserved : 2;	//[4:5]
		unsigned int cdr6 : 1;		//[6:6]
		unsigned int cdr7 : 1;		//[7:7]
	};
	unsigned int u32;
};

struct tscan_regs
{
	volatile union tscan_mod MOD;	  // 00H mode
	volatile union tscan_cmr CMR;	  // 01H command
	volatile union tscan_sr SR;	// 02H status
	volatile union tscan_ir IR;	  // 03H interrupt
	volatile union tscan_ier IER;	  // 04H interrupt enable
	volatile u32 RESERVED0;	  // 05H
	volatile union tscan_btr0 BTR0; // 06H bus timing 0
	volatile union tscan_btr1 BTR1; // 07H bus timing 1
	volatile union tscan_ocr OCR;	  // 08H output control register (10:normal output mode 11:clock output mode)
	volatile u32 RESERVED1;	  // 09H
	volatile u32 RESERVED2;	  // 0AH
	volatile union tscan_alc ALC;	  // 0BH arbitration lost capture
	volatile union tscan_ecc ECC;	  // 0CH error code capture
	volatile union tscan_rbsa EWLR; // 0DH error warning limit
	volatile u32 RXERR;		  // 0EH receive error counter
	volatile u32 TXERR;		  // 0FH transmit error counter
	union					// 10H-1cH
	{
		union
		{
			union tscan_frame frame_info;
			struct standard_frame standard_frame;
			struct extended_frame extended_frame;
		};
		struct
		{
			volatile u32 ACR[4]; // 10H-13H
			volatile u32 AMR[4]; // 14H-17H
		};
	};
	volatile u32 RMC;			// 1DH receive message counter
	volatile u32 RBSA;			// 1EH receive buffer start address
	volatile union tscan_cdr CDR;			// 1FH clock divider
	volatile u32 RX_FIFO[64];	// 20H-5FH
	volatile u32 TX_FIFO[13];	// 60H-6CH

};

/* Structure of the message buffer */
struct tscan_mb {
	union
	{
		union tscan_frame frame_info;
		struct standard_frame standard_frame;
		struct extended_frame extended_frame;
	};
};



struct tscan_priv {
	struct can_priv can;
	// struct can_rx_offload offload;
	struct net_device* dev;

	struct tscan_regs __iomem* regs;
	struct tscan_mb __iomem* tx_mb;
	struct tscan_mb __iomem* tx_mb_reserved;
	u8 tx_mb_idx;
	u32 reg_ctrl_default;
	u32 reg_imask1_default;
	u32 reg_imask2_default;

	struct clk* clk_ipg;
	struct clk* clk_per;
	struct clk* pclk;
	
	//const struct tscan_devtype_data* devtype_data;
	// struct regulator* reg_xceiver;
};














#endif /* TS_CAN_H_ */
