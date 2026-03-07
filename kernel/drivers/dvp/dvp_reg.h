/*****************************************************************************
 * Copyright (C) 2022,Tsing Micro Technology Co., Ltd,All rights reserved.
 * Description:
 *
 *****************************************************************************/
 
#ifndef __DVP_REG_H__
#define __DVP_REG_H__

#include <uapi/linux/types.h>
#include <linux/types.h>

/* Includes ------------------------------------------------------------------*/
/******************************************************************************/
/*																			  */
/*									  DVP									 */
/*																			  */
/******************************************************************************/
/********************  Bits definition for DVP_CR register	******************/
#define DVP_CR_SENSOR_MODE					 ((unsigned int)0x00000C00)
#define DVP_CR_YUV_MODE 					 ((unsigned int)0x00000300)
#define DVP_CR_DMA_WR_EN					 ((unsigned int)0x00000080)
#define DVP_CR_CLK_POLAR					 ((unsigned int)0x00000040)
#define DVP_CR_V_POLAR						 ((unsigned int)0x00000020)
#define DVP_CR_H_POLAR						 ((unsigned int)0x00000010)
#define DVP_CR_ADDR_SEL3					 ((unsigned int)0x00000008)
#define DVP_CR_ADDR_SEL2					 ((unsigned int)0x00000004)
#define DVP_CR_ADDR_SEL1					 ((unsigned int)0x00000002)
#define DVP_CR_CAPTURE_EN					 ((unsigned int)0x00000001)

/********************  Bits definition for DVP_SR register	******************/
#define DVP_SR_STAT 						 ((unsigned int)0x0000001F)

/********************  Bits definition for DVP_IMG register  ****************/
#define DVP_IMGADDR1						 ((unsigned int)0xFFFFFFFF)
#define DVP_IMGADDR2						 ((unsigned int)0xFFFFFFFF)
#define DVP_IMGADDR3						 ((unsigned int)0xFFFFFFFF)

/********************  Bits definition for DVP_STOP register  ****************/
#define DVP_STOP							 ((unsigned int)0x00000001)

/********************  Bits definition for DVP_pre register  ****************/
#define DVP_PRE 							 ((unsigned int)0x00000001)

/********************  Bits definition for DVP_HPARA register  ****************/
#define DVP_HPARA_STRIDE					 ((unsigned int)0xFFFF0000)
#define DVP_HPARA_WIDTH 					 ((unsigned int)0x0000FFFF)

/********************  Bits definition for DVP_VPARA register  ****************/
#define DVP_VPARA_CUTX2 					 ((unsigned int)0xFF000000)
#define DVP_VPARA_CUTX1 					 ((unsigned int)0x00FF0000)
#define DVP_VPARA_HEIGHT					 ((unsigned int)0x0000FFFF)

/********************  Bits definition for DVP_IER register  *****************/
#define DVP_IER_ADDR1_IE					 ((unsigned int)0x00000001)
#define DVP_IER_ADDR2_IE					 ((unsigned int)0x00000002)
#define DVP_IER_ADDR3_IE					 ((unsigned int)0x00000004)
#define DVP_IER_FIFO_ERR					 ((unsigned int)0x00000008)

/********************  Bits definition for DVP_IFR register  ****************/
#define DVP_IFR_ADDR1_FE					 ((unsigned int)0x00000001)
#define DVP_IFR_ADDR2_FE					 ((unsigned int)0x00000002)
#define DVP_IFR_ADDR3_FE					 ((unsigned int)0x00000004)
#define DVP_IFR_FIFO_ERR					 ((unsigned int)0x00000008)

/********************  Bits definition for DVP_ICR register  *****************/
#define DVP_ICR_ADDR1_CE					 ((unsigned int)0x00000001)
#define DVP_ICR_ADDR2_CE					 ((unsigned int)0x00000002)
#define DVP_ICR_ADDR3_CE					 ((unsigned int)0x00000004)
#define DVP_ICR_FIFO_ERR					 ((unsigned int)0x00000008)

/********************  Bits definition for DVP_KRY register  *****************/
#define DVP_KRY_PARA						 ((unsigned int)0x7FF)
/********************  Bits definition for DVP_KRV register  *****************/
#define DVP_KRV_PARA						 ((unsigned int)0x7FF)
/********************  Bits definition for DVP_KGY register  *****************/
#define DVP_KGY_PARA						 ((unsigned int)0x7FF)
/********************  Bits definition for DVP_KGU register  *****************/
#define DVP_KGU_PARA						 ((unsigned int)0x7FF)
/********************  Bits definition for DVP_KGV register  *****************/
#define DVP_KGV_PARA						 ((unsigned int)0x7FF)
/********************  Bits definition for DVP_KBY register  *****************/
#define DVP_KBY_PARA						 ((unsigned int)0x7FF)
/********************  Bits definition for DVP_KBV register  *****************/
#define DVP_KBV_PARA						 ((unsigned int)0x7FF) 

union DVP_GCTRL_U {
	struct {
		unsigned int capture_en : 1;      //[0:0]
		unsigned int addr_sel1 : 1;       //[1:1]
		unsigned int addr_sel2 : 1;       //[2:2]
		unsigned int addr_sel3 : 1;       //[3:3]
		unsigned int hpolar : 1;          //[4:4]
		unsigned int vpolar : 1;          //[5:5]
		unsigned int clkpolar : 1;        //[6:6]
		unsigned int dma_er_en : 1;       //[7:7]
		unsigned int yuv422mode : 2;      //[8:9]
		unsigned int sensor_mode : 2;     //[10:11]
		unsigned int rgb_senquence : 2;   //[12:13]
		unsigned int bytes_sel : 2;       //[14:15]
		unsigned int counter_mode : 1;    //[16:16]
		unsigned int reserved0 : 15;      //[15:31]
	};
	unsigned int u32;
};

union DVP_STAT_U {
	struct {
		unsigned int stat : 16;           //[0:15]
		unsigned int reserved0 : 16;      //[16:31]
	};
	unsigned int u32;
};

union DVP_IMGADDR1_U {
	struct {
		unsigned int img_addr1 : 32;      //[0:31]
	};
	unsigned int u32;
};

union DVP_IMGADDR2_U {
	struct {
		unsigned int img_addr2 : 32;      //[0:31]
	};
	unsigned int u32;
};

union DVP_IMGADDR3_U {
	struct {
		unsigned int img_addr3 : 32;      //[0:31]
	};
	unsigned int u32;
};

union DVP_CTRL_STAT_U {
	struct {
		unsigned int row_cnt : 16;        //[0:15]
		unsigned int cur_stat : 4;        //[16:19]
		unsigned int cnt_eq : 1;          //[20:20]
		unsigned int reserved0 : 11;      //[21:31]
	};
	unsigned int u32;
};

union DVP_HPARM_U {
	struct {
		unsigned int img_width : 13;      //[0:12]
		unsigned int reserved0 : 3;       //[13:15]
		unsigned int img_stride : 13;     //[16:28]
		unsigned int reserved1 : 3;       //[29:31]
	};
	unsigned int u32;
};

union DVP_VPARM_U {
	struct {
		unsigned int img_heigth : 13;      //[0:12]
		unsigned int reserved0 : 3;        //[13:15]
		unsigned int cutx1 : 8;            //[16:23]
		unsigned int cutx2: 8;             //[24:31]
	};
	unsigned int u32;
};

union DVP_IER_U {
	struct {
		unsigned int ier_0 : 1;            //[0:0]
		unsigned int ier_1 : 1;            //[1:1]
		unsigned int ier_2 : 1;            //[2:2]
		unsigned int ier_3 : 1;            //[3:3]
		unsigned int reserved0 : 28;       //[4:31]
	};
	unsigned int u32;
};

union DVP_IFR_U {
	struct {
		unsigned int ifr_0 : 1;            //[0:0]
		unsigned int ifr_1 : 1;            //[1:1]
		unsigned int ifr_2 : 1;            //[2:2]
		unsigned int ifr_3 : 1;            //[3:3]
		unsigned int reserved0 : 28;       //[4:31]
	};
	unsigned int u32;
};

union DVP_ICR_U {
	struct {
		unsigned int icr_0 : 1;            //[0:0]
		unsigned int icr_1 : 1;            //[1:1]
		unsigned int icr_2 : 1;            //[2:2]
		unsigned int icr_3 : 1;            //[3:3]
		unsigned int reserved0 : 28;       //[4:31]
	};
	unsigned int u32;
};

union DVP_PARM_KRY_U {
	struct {
		unsigned int parm_kry : 11;        //[0:10]
		unsigned int reserved0 : 21;       //[11:31]
	};
	unsigned int u32;
};

union DVP_PARM_KRV_U {
	struct {
		unsigned int parm_krv : 11;        //[0:10]
		unsigned int reserved0 : 21;       //[11:31]
	};
	unsigned int u32;
};

union DVP_PARM_KGY_U {
	struct {
		unsigned int parm_kgy : 11;        //[0:10]
		unsigned int reserved0 : 21;       //[11:31]
	};
	unsigned int u32;
};

union DVP_PARM_KGU_U {
	struct {
		unsigned int parm_kgu : 11;        //[0:10]
		unsigned int reserved0 : 21;       //[11:31]
	};
	unsigned int u32;
};

union DVP_PARM_KGV_U {
	struct {
		unsigned int parm_kgv : 11;        //[0:10]
		unsigned int reserved0 : 21;       //[11:31]
	};
	unsigned int u32;
};

union DVP_PARM_KBY_U {
	struct {
		unsigned int parm_kby : 11;        //[0:10]
		unsigned int reserved0 : 21;       //[11:31]
	};
	unsigned int u32;
};

union DVP_PARM_KBU_U {
	struct {
		unsigned int parm_kbu : 11;        //[0:10]
		unsigned int reserved0 : 21;       //[11:31]
	};
	unsigned int u32;
};

typedef union {
	struct {
		unsigned int dvp_capture_en : 1;	//[0:0]
		unsigned int dvp_addr_sel1 : 1;		//[1:1]
		unsigned int dvp_addr_sel2 : 1;		//[2:2]
		unsigned int dvp_addr_sel3 : 1;		//[3:3]
		unsigned int dvp_h_polor : 1;		//[4:4]
		unsigned int dvp_v_polor : 1;		//[5:5]
		unsigned int dvp_clk_polor : 1;		//[6:6]
		unsigned int dvp_dma_wr_en : 1;		//[7:7]
		unsigned int dvp_yuv_mode : 2;		//[8:9]
		unsigned int dvp_snesor_mode : 2;	//[10:11]
		unsigned int dvp_rgb_senquence : 2;	//[12:13]
		unsigned int dvp_byte_sel : 2;		//[14:15]
		unsigned int dvp_counter_mode : 1;	//[16:16]
		unsigned int reserved0 : 16;		//[30:31]
	};
	unsigned int u32;
}dvp_gcr;

typedef struct {
	unsigned int dvp_kry;
	unsigned int dvp_krv;
	unsigned int dvp_kgy;
	unsigned int dvp_kgu;
	unsigned int dvp_kgv;
	unsigned int dvp_kby;
	unsigned int dvp_kbu;
}dvp_para;

typedef struct {
	unsigned int dvp_addr1;
	unsigned int dvp_addr2;
	unsigned int dvp_addr3;
}dvp_image_addr;

typedef struct {
	unsigned short dvp_width;
	unsigned short dvp_strdie;
}dvp_hpara;

typedef struct {
	unsigned short dvp_height;
	unsigned char dvp_cutx1;
	unsigned char dvp_cutx2;
}dvp_vpara;

struct DVP_REG {
	volatile union DVP_GCTRL_U dvp_gctrl;			      /*Address offset: 0x0  */
	volatile union DVP_STAT_U dvp_stat;		              /*Address offset: 0x4  */
	volatile union DVP_IMGADDR1_U dvp_imgaddr1;			  /*Address offset: 0x8  */
	volatile union DVP_IMGADDR2_U dvp_imgaddr2;			  /*Address offset: 0xC  */
	volatile union DVP_IMGADDR3_U dvp_imgaddr3;			  /*Address offset: 0x10 */
	volatile unsigned int reserved0[3];
	volatile union DVP_CTRL_STAT_U dvp_ctrl_stat;         /*Address offset: 0x20 */
	volatile unsigned int reserved1[6];
	volatile union DVP_HPARM_U dvp_hparm;			      /*Address offset: 0x3C */
	volatile union DVP_VPARM_U dvp_vparm;			      /*Address offset: 0x40 */
	volatile union DVP_IER_U dvp_ier;			          /*Address offset: 0x44 */
	volatile union DVP_IFR_U dvp_ifr;			          /*Address offset: 0x48 */
	volatile union DVP_ICR_U dvp_icr;			          /*Address offset: 0x4C */
	volatile union DVP_PARM_KRY_U dvp_parm_kry;			  /*Address offset: 0x50 */
	volatile union DVP_PARM_KRV_U dvp_parm_krv;			  /*Address offset: 0x54 */
	volatile union DVP_PARM_KGY_U dvp_parm_kgy;			  /*Address offset: 0x58 */
	volatile union DVP_PARM_KGU_U dvp_parm_kgu;			  /*Address offset: 0x5C */
	volatile union DVP_PARM_KGV_U dvp_parm_kgv;			  /*Address offset: 0x60 */
	volatile union DVP_PARM_KBY_U dvp_parm_kby;			  /*Address offset: 0x64 */
	volatile union DVP_PARM_KBU_U dvp_parm_kbu;			  /*Address offset: 0x68 */
};

#endif