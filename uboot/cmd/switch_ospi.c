// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2022 TsingMicro.Inc
 */

#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <env.h>
#ifdef CONFIG_ARCH_TS_TX536
#include <tx536.h>
#endif

#ifdef CONFIG_ARCH_TS_TX5215
#include <tx52xx.h>
#endif

#ifdef CONFIG_ARCH_TS_TX5112
#include <tx51xx.h>
#endif

#if defined(CONFIG_ARCH_TS_TX536)

#endif

#if defined(CONFIG_ARCH_TS_TX5215) || defined(CONFIG_ARCH_TS_TX5112)

#define SOC_GPIOA_ADDRBASE 0xf2448000UL  //dt56 dt57 is same
#define SOC_GPIOB_ADDRBASE 0xf244c000UL
#define SOC_IOMUX_ADDRBASE 0xf0000000UL

#if 0
#define DW_GPIO_DATA_REG			0x000		/*gpio data*/
#define DW_GPIO_DIR_REG				0x004		/*gpio direction*/
#define DW_GPIO_INT_EN_REG			0x030		/*gpio intr enable*/
#define DW_GPIO_INT_LEV_REG			0x038		/*gpio intr level*/
#define DW_GPIO_INT_POL_REG			0x03c		/*gpio intr polarity*/
#define DW_GPIO_INT_STA_REG			0x040		/*gpio intr status*/
#define DW_GPIO_INT_EOI_REG			0x04c		/*gpio intr clear*/

#define SCD_GPIO_IS_REG 			0x404		/*gpio interrupt sense*/
#define SCD_GPIO_IBE_REG			0x408		/*gpio both edges*/
#define SCD_GPIO_IEV_REG			0x40c		/*gpio interrupt event*/
#define SCD_GPIO_IM_REG 			0x410		/*gpio interrupt mask*/
#define SCD_GPIO_RIS_REG			0x414		/*gpio raw interrupt status*/
#define SCD_GPIO_MIS_REG			0x418		/*gpio mask interrupt status*/
#define SCD_GPIO_ICR_REG			0x41c		/*gpio interrupt clear*/
#define SCD_GPIO_AFSEL_REG			0x420		/*gpio altermate function select*/
#define SCD_GPIO_R2R_REG			0x500		/*gpio 2-mA driver select*/
#define SCD_GPIO_R4R_REG			0x504		/*gpio 4-mA driver select*/
#define SCD_GPIO_R8R_REG			0x508		/*gpio 8-mA driver select*/
#define SCD_GPIO_ODR_REG			0x50c		/*gpio open drain select*/
#define SCD_GPIO_PUR_REG			0x510		/*gpio pull up select*/
#define SCD_GPIO_PDR_REG			0x514		/*gpio pull down select*/
#define SCD_GPIO_SLR_REG			0x518		/*gpio rate control select*/
#define SCD_GPIO_DEN_REG			0x51c		/*gpio digital enabel*/
#define SCD_GPIO_AMSEL_REG			0x528		/*gpio analog mode select*/
#define SCD_GPIO_PCTL_REG			0x52c		/*gpio port control*/
#define SCD_GPIO_HE_REG				0x530		/*gpio hold control*/
#define SCD_GPIO_ST0_REG			0x534		/*gpio st0 choose*/
#define SCD_GPIO_ST1_REG			0x538		/*gpio st1 choose*/
#define SCD_GPIO_PE_REG				0x53c		/*gpio pull enable*/
#define SCD_GPIO_PS_REG				0x540		/*gpio pull select*/
#define SCD_GPIO_SL_REG				0x544		/*gpio slew rate control */
#define SCD_GPIO_MSC_REG			0x548		/*gpio msc select*/
#define SCD_GPIO_STB_REG			0x54c		/*gpio schmitt trigger*/
#define SCD_GPIO_PID4_REG			0xfd0		/*gpio peripheral identification4*/
#define SCD_GPIO_PID5_REG			0xfd4		/*gpio peripheral identification5*/
#define SCD_GPIO_PID6_REG			0xfd8		/*gpio peripheral identification6*/
#define SCD_GPIO_PID7_REG			0xfdc		/*gpio peripheral identification7*/
#define SCD_GPIO_PID0_REG			0xfe0		/*gpio peripheral identification0*/
#define SCD_GPIO_PID1_REG			0xfe4		/*gpio peripheral identification1*/
#define SCD_GPIO_PID2_REG			0xfe8		/*gpio peripheral identification2*/
#define SCD_GPIO_PID3_REG			0xfec		/*gpio peripheral identification3*/
#define SCD_GPIO_CID0_REG			0xff0		/*gpio primecell identification0*/
#define SCD_GPIO_CID1_REG			0xff4		/*gpio primecell identification1*/
#define SCD_GPIO_CID2_REG			0xff8		/*gpio primecell identification2*/
#define SCD_GPIO_CID3_REG			0xffc		/*gpio primecell identification3*/

#define SCD_REG_OFFSET				0x10000		/*gpio A->M register offset*/
#define SCD_REG_GOURP_MASK			0xff		/*gpio register greoup mask*/
#endif

#if defined(CONFIG_ARCH_TS_TX5215)
typedef enum {
	PA0 = 0,
	PA1,
	PA2,
	PA3,
	PA4,
	PA5,
	PA6,
	PA7,
	PB0,
	PB1,
	PB2,
	PB3,
	PB4,
	PB5,
	PB6,
	PB7,
	PC0,
	PC1,
	PC2,
	PC3,
	PC4,
	PC5,
	PC6,
	PC7,
	PD0,
	PD1,
	PD2,
	PD3,
	PD4,
	PD5,
	PD6,
	PD7,
	PE0 = 32,
	PE1,
	PE2,
	PE3,
	PE4,
	PE5,
	PE6,
	PE7,
	PF0,
	PF1,
	PF2,
	PF3,
	PJ6,//special-->GPIOB12
	PF5,
	PF6,
	PF7,
	PG0,
	PG1,
	PG2,
	PJ0,
	PJ1,
	PJ2,
	PJ3,
	PJ4,
	PJ5,
	PK0,
	PK1,
	PK2,
	PK3,
	PK4,
	PK5,
	PK6,
	PMAX,
} pin_name_e;
#else

typedef enum {
	PA0 = 0,
	PA1,
	PA2,
	PA3,
	PA4,
	PA5,
	PA6,
	PA7,
	PB0,
	PB1,
	PB2,
	PB3,
	PB4,
	PB5,
	PB6,
	PB7,
	PC0,
	PC1,
	PC2,
	PC3,
	PC4,
	PC5,
	PC6,
	PC7,
	PD0,
	PD1,
	PD2,
	PD3,
	PD4,
	PE0,
	PE1,
	PE2,
	PE3 = 32,
	PE4,
	PE5,
	PF0,
	PF1,
	PF2,
	PF3,
	PF4,
	PF5,
	PF6,
	PMAX,
} pin_name_e;
#endif


#define GPIO_PERGROUP_NUM 32

#define GPIO_LOW		  0
#define GPIO_HIGH		  1

#define GPIO_INTR_DISABLE		  0
#define GPIO_INTR_ENABLE		  1

typedef enum{
	FUN_SEL_0,
	FUN_SEL_1,
	FUN_SEL_2,
	FUN_SEL_3,
	FUN_SEL_4,
	FUN_SEL_5,
	FUN_SEL_6,
	FUN_SEL_7,
	FUN_SEL_GPIO,
} gpio_fun_sel_e;

typedef enum{
	VOLT_SEL_33,
	VOLT_SEL_18,
} gpio_vol_sel_e;

typedef void *gpio_pin_handle_t;

#define ERRNO_DRV_START 0
typedef enum {
	DRV_ERROR = ERRNO_DRV_START,   ///< Unspecified error
	DRV_ERROR_BUSY, 			   ///< Driver is busy
	DRV_ERROR_TIMEOUT,			   ///< Timeout occurred
	DRV_ERROR_UNSUPPORTED,		   ///< Operation not supported
	DRV_ERROR_PARAMETER,		   ///< Parameter error
	DRV_ERROR_SPECIFIC			   ///< Start of driver specific errors
} drv_err_e;


/****** GPIO specific error codes *****/
typedef enum {
	GPIO_ERROR_MODE  = (DRV_ERROR_SPECIFIC + 1),	  ///< Specified Mode not supported
	GPIO_ERROR_DIRECTION,						 ///< Specified direction not supported
	GPIO_ERROR_IRQ_MODE,						 ///< Specified irq mode not supported
	GPIO_ERROR_FNUC_MODE,
} gpio_error_e;

/*----- GPIO Control Codes: Mode -----*/
typedef enum {
	GPIO_MODE_D2MA				= 0,
	GPIO_MODE_D4MA,
	GPIO_MODE_D8MA,
	GPIO_MODE_OPEN_DRAIN,
	GPIO_MODE_PULLUP,
	GPIO_MODE_PULLDOWN,
	GPIO_MODE_RATE_CTRL,
	GPIO_MODE_DIAG_SELECT,
	GPIO_MODE_ANALOG_SELECT,
} gpio_mode_e;

/*----- GPIO Control Codes: Mode Parameters: Data Bits -----*/
typedef enum {
	GPIO_DIRECTION_INPUT			 = 0,	 ///< gpio as input
	GPIO_DIRECTION_OUTPUT,					 ///< gpio as output
} gpio_direction_e;

/*----- GPIO Control Codes: Mode Parameters: Parity -----*/
typedef enum {
	GPIO_IRQ_MODE_RISING_EDGE	  = 0,		 ///< interrupt mode for rising edge
	GPIO_IRQ_MODE_FALLING_EDGE, 			 ///< interrupt mode for falling edge
	GPIO_IRQ_MODE_DOUBLE_EDGE,				 ///< interrupt mode for double edge
	GPIO_IRQ_MODE_LOW_LEVEL,				 ///< interrupt mode for low level
	GPIO_IRQ_MODE_HIGH_LEVEL,				 ///< interrupt mode for high level
} gpio_irq_mode_e;

typedef void (*gpio_event_cb_t)(u32 idx);	///< gpio Event call back.

s32 scd_gpio_register(u32 gpio_pin, gpio_event_cb_t cb_event);
s32 scd_gpio_init(s32 idx, u32 gpio_base);
u32 scd_gpio_irq_state(s32 idx);
void scd_gpio_irq_clear(u32 gpio);
void scd_gpio_irqhandler(s32 idx);
s32 scd_gpio_uninit(u32 gpio);
s32 scd_gpio_config(u32 gpio, gpio_mode_e mode);
s32 scd_gpio_read(u32 gpio, bool *value);
s32 scd_gpio_write(u32 gpio, bool value);
s32 scd_gpio_set_direction(u32 gpio, gpio_direction_e direction);
s32 scd_gpio_set_irq_mode(u32 gpio, gpio_irq_mode_e irq_mode);
void scd_gpio_irq_enable(u32 gpio);
void scd_gpio_irq_disable(u32 gpio);
s32 scd_gpio_set_irq(u32 gpio, gpio_irq_mode_e mode, bool enable);
s32 scd_gpio_func_sel(u32 gpio, gpio_fun_sel_e mode);
s32 scd_gpio_vol_sel(u32 gpio, gpio_vol_sel_e mode);


#define DW_GPIO_DATA_REG			0x000		/*gpio data*/
#define DW_GPIO_DIR_REG				0x004		/*gpio direction*/
#define DW_GPIO_INT_EN_REG			0x030		/*gpio intr enable*/
#define DW_GPIO_INT_LEV_REG			0x038		/*gpio intr level*/
#define DW_GPIO_INT_POL_REG			0x03c		/*gpio intr polarity*/
#define DW_GPIO_INT_STA_REG			0x040		/*gpio intr status*/
#define DW_GPIO_INT_EOI_REG			0x04c		/*gpio intr clear*/

#define SCD_GPIO_IS_REG 			0x404		/*gpio interrupt sense*/
#define SCD_GPIO_IBE_REG			0x408		/*gpio both edges*/
#define SCD_GPIO_IEV_REG			0x40c		/*gpio interrupt event*/
#define SCD_GPIO_IM_REG 			0x410		/*gpio interrupt mask*/
#define SCD_GPIO_RIS_REG			0x414		/*gpio raw interrupt status*/
#define SCD_GPIO_MIS_REG			0x418		/*gpio mask interrupt status*/
#define SCD_GPIO_ICR_REG			0x41c		/*gpio interrupt clear*/
#define SCD_GPIO_AFSEL_REG			0x420		/*gpio altermate function select*/
#define SCD_GPIO_R2R_REG			0x500		/*gpio 2-mA driver select*/
#define SCD_GPIO_R4R_REG			0x504		/*gpio 4-mA driver select*/
#define SCD_GPIO_R8R_REG			0x508		/*gpio 8-mA driver select*/
#define SCD_GPIO_ODR_REG			0x50c		/*gpio open drain select*/
#define SCD_GPIO_PUR_REG			0x510		/*gpio pull up select*/
#define SCD_GPIO_PDR_REG			0x514		/*gpio pull down select*/
#define SCD_GPIO_SLR_REG			0x518		/*gpio rate control select*/
#define SCD_GPIO_DEN_REG			0x51c		/*gpio digital enabel*/
#define SCD_GPIO_AMSEL_REG			0x528		/*gpio analog mode select*/
#define SCD_GPIO_PCTL_REG			0x52c		/*gpio port control*/
#define SCD_GPIO_HE_REG				0x530		/*gpio hold control*/
#define SCD_GPIO_ST0_REG			0x534		/*gpio st0 choose*/
#define SCD_GPIO_ST1_REG			0x538		/*gpio st1 choose*/
#define SCD_GPIO_PE_REG				0x53c		/*gpio pull enable*/
#define SCD_GPIO_PS_REG				0x540		/*gpio pull select*/
#define SCD_GPIO_SL_REG				0x544		/*gpio slew rate control */
#define SCD_GPIO_MSC_REG			0x548		/*gpio msc select*/
#define SCD_GPIO_STB_REG			0x54c		/*gpio schmitt trigger*/
#define SCD_GPIO_PID4_REG			0xfd0		/*gpio peripheral identification4*/
#define SCD_GPIO_PID5_REG			0xfd4		/*gpio peripheral identification5*/
#define SCD_GPIO_PID6_REG			0xfd8		/*gpio peripheral identification6*/
#define SCD_GPIO_PID7_REG			0xfdc		/*gpio peripheral identification7*/
#define SCD_GPIO_PID0_REG			0xfe0		/*gpio peripheral identification0*/
#define SCD_GPIO_PID1_REG			0xfe4		/*gpio peripheral identification1*/
#define SCD_GPIO_PID2_REG			0xfe8		/*gpio peripheral identification2*/
#define SCD_GPIO_PID3_REG			0xfec		/*gpio peripheral identification3*/
#define SCD_GPIO_CID0_REG			0xff0		/*gpio primecell identification0*/
#define SCD_GPIO_CID1_REG			0xff4		/*gpio primecell identification1*/
#define SCD_GPIO_CID2_REG			0xff8		/*gpio primecell identification2*/
#define SCD_GPIO_CID3_REG			0xffc		/*gpio primecell identification3*/

#define SCD_REG_OFFSET				0x10000		/*gpio A->M register offset*/
#define SCD_REG_GOURP_MASK			0xff		/*gpio register greoup mask*/

#define DRV_ERRNO_GPIO_BASE 			0x81010000
#define ERR_GPIO(errno) (DRV_ERRNO_GPIO_BASE | errno)
#define GPIO_NULL_PARAM_CHK(para)	 HANDLE_PARAM_CHK(para, ERR_GPIO(DRV_ERROR_PARAMETER))
typedef void (*gpio_event_cb_t)(u32 idx);	///< gpio Event call back.

typedef void *gpio_gpio_priv_t;

typedef struct {
	u32 pin;		/*pin name gpio*/
	u32 off;		/*pin offset in gpio*/
	u32 base;		/*gpio base reg*/
	u32 iogroup;	/*gpio iomux group name*/
	u32 iobase;		/*gpio iomux base reg*/
	u32 iooff;		/*gpio iomux off in group*/
	u32 irq;
	gpio_event_cb_t cb;
} gpio_priv_t;

s32 scd_gpio_to_iomux(u32 gpio_pin, u32 *iogroup, u32 *iooff)
{
	if (gpio_pin < 0 || gpio_pin >= PMAX) {
		return -1;
	}


#if defined(CONFIG_ARCH_TS_TX5215)
	/*gpioB 12*/
	if(gpio_pin == PJ6) {
		*iogroup = 8;
		*iooff = 6;
		return 0;
	}

	if(gpio_pin <= PG2) {
		*iogroup = gpio_pin / 8;
		*iooff = gpio_pin % 8;
	} else if(gpio_pin <= PJ5) {
		*iogroup = 8;
		*iooff = (gpio_pin - PG2 - 1) % 8;
	} else if(gpio_pin <= PK6) {
		*iogroup = 9;
		*iooff = (gpio_pin - PJ5 - 1) % 8;
	} else {
		return -1;
	}
#else

	if (gpio_pin < 0 || gpio_pin >= PMAX) {
		return -1;
	}

	if(gpio_pin <= PD4) {
		*iogroup = gpio_pin / 8;
		*iooff = gpio_pin % 8;
	} else if(gpio_pin <= PE5) {
		*iogroup = 4;
		*iooff = (gpio_pin - PD4 - 1) % 8;
	} else if(gpio_pin <= PF6) {
		*iogroup = 5;
		*iooff = (gpio_pin - PE5 - 1) % 8;
	} else {
		return -1;
	}

#endif

	return 0;
}

struct {
	u32 base;
	u32 iobase;
}gs_gpio_config[] = {
	{SOC_GPIOA_ADDRBASE, SOC_IOMUX_ADDRBASE},
	{SOC_GPIOB_ADDRBASE, SOC_IOMUX_ADDRBASE}
};

s32 scd_gpio_priv(s32 pin, gpio_priv_t *gpio_priv)
{
	s32 ret;
	u32 idx, base, iobase, iogroup, iooff;

	if(pin > PMAX)
		return 1;

	idx = pin / GPIO_PERGROUP_NUM;
	base = gs_gpio_config[idx].base;
	iobase = gs_gpio_config[idx].iobase;
	gpio_priv->base = base;
	gpio_priv->off = pin % GPIO_PERGROUP_NUM;
	gpio_priv->pin = pin;

	ret = scd_gpio_to_iomux(gpio_priv->pin, &iogroup, &iooff);
	if(ret)
		return ret;

	gpio_priv->iogroup = iogroup;
	gpio_priv->iooff = iooff;
	gpio_priv->iobase = iobase + gpio_priv->iogroup * SCD_REG_OFFSET;
	return 0;
}

s32 scd_gpio_set_direction(u32 gpio, gpio_direction_e direction)
{
	u32 val, ret;
	u32 offset;

	//gpio_priv_t *gpio_priv = (gpio_priv_t *)&gpio_handle[gpio];

	gpio_priv_t gpio_priv;
	
	ret = scd_gpio_priv(gpio, &gpio_priv);
	if(ret)
		return ret;

	val = readl(gpio_priv.base + DW_GPIO_DIR_REG);
	offset = gpio_priv.off;

	if (direction == GPIO_DIRECTION_INPUT) {
		val &= ~(1	<< offset);
	} else if (direction == GPIO_DIRECTION_OUTPUT) {
		val |= (1 << offset);
	} else {
		return ERR_GPIO(GPIO_ERROR_DIRECTION);
	}

	writel(val, gpio_priv.base + DW_GPIO_DIR_REG);
	return 0;
}



s32 scd_gpio_func_sel(u32 gpio, gpio_fun_sel_e mode)
{
	s32 ret;
	u32 val;
	u32 offset;
	gpio_priv_t gpio_priv;
	
	ret = scd_gpio_priv(gpio, &gpio_priv);
	if(ret)
		return ret;

	offset = gpio_priv.iooff;

	val = readl(gpio_priv.iobase + SCD_GPIO_PCTL_REG);
	offset *= 4;
	val &= ~(0xf << offset);
	val |= (mode << offset);

	writel(val, gpio_priv.iobase + SCD_GPIO_PCTL_REG);

	return 0;
}

s32 scd_gpio_vol_sel(u32 gpio, gpio_vol_sel_e mode)
{
	int ret;
	u32 val;
	u32 offset;
	gpio_priv_t gpio_priv;
	
	ret = scd_gpio_priv(gpio, &gpio_priv);
	if(ret)
		return ret;

	offset = gpio_priv.iooff;

	val = readl(gpio_priv.iobase + SCD_GPIO_MSC_REG);

	val &= ~(0x1 << offset);
	val |= (mode << offset);

	writel(val, gpio_priv.iobase + SCD_GPIO_MSC_REG);

	return 0;
}

/*offset*/
#define FLASH_DATA_WID_OFS 0
#define FLASH_VOLT_VAL_OFS 8

/*field*/
#define CFG_DEV_DATA_WID(_cfg_) ((_cfg_>>FLASH_DATA_WID_OFS) & FLASH_DATA_WIDTH_MASK)
#define CFG_DEV_VOLT_VAL(_cfg_) ((_cfg_>>FLASH_VOLT_VAL_OFS) & FLASH_VOLT_VAL_MASK)

/**/
#define LINE1_18V_CFG ((FLASH_DATA_WIDTH_1<<FLASH_DATA_WID_OFS) | (FLASH_VOLT_1_8V<<FLASH_VOLT_VAL_OFS))
#define LINE4_18V_CFG ((FLASH_DATA_WIDTH_4<<FLASH_DATA_WID_OFS) | (FLASH_VOLT_1_8V<<FLASH_VOLT_VAL_OFS))
#define LINE1_33V_CFG ((FLASH_DATA_WIDTH_1<<FLASH_DATA_WID_OFS) | (FLASH_VOLT_3_3V<<FLASH_VOLT_VAL_OFS))

typedef enum {
	FLASH_VOLT_1_8V = 0,
	FLASH_VOLT_3_3V = 1,
	FLASH_VOLT_VAL_MASK = 1,
} flash_volt_e;

int iomux_ospi_cfg(int idx, uint32_t cfg)
{
#if defined(CONFIG_ARCH_TS_TX5215) 
	if(CFG_DEV_VOLT_VAL(cfg) == FLASH_VOLT_1_8V)
	{
		scd_gpio_func_sel(PJ0, FUN_SEL_0);
		scd_gpio_func_sel(PJ1, FUN_SEL_0);
		scd_gpio_func_sel(PJ2, FUN_SEL_0);
		scd_gpio_func_sel(PJ3, FUN_SEL_0);
		scd_gpio_func_sel(PJ4, FUN_SEL_0);
		scd_gpio_func_sel(PJ5, FUN_SEL_0);

		scd_gpio_vol_sel(PJ0, VOLT_SEL_18);
		scd_gpio_vol_sel(PJ1, VOLT_SEL_18);
		scd_gpio_vol_sel(PJ2, VOLT_SEL_18);
		scd_gpio_vol_sel(PJ3, VOLT_SEL_18);
		scd_gpio_vol_sel(PJ4, VOLT_SEL_18);
		scd_gpio_vol_sel(PJ5, VOLT_SEL_18);
	}else{
		scd_gpio_func_sel(PE1, FUN_SEL_2);
		scd_gpio_func_sel(PE2, FUN_SEL_2);
		scd_gpio_func_sel(PE3, FUN_SEL_2);
		scd_gpio_func_sel(PE4, FUN_SEL_2);
		scd_gpio_func_sel(PE5, FUN_SEL_2);
		scd_gpio_func_sel(PE6, FUN_SEL_2);

		scd_gpio_vol_sel(PE1, VOLT_SEL_33);
		scd_gpio_vol_sel(PE2, VOLT_SEL_33);
		scd_gpio_vol_sel(PE3, VOLT_SEL_33);
		scd_gpio_vol_sel(PE4, VOLT_SEL_33);
		scd_gpio_vol_sel(PE5, VOLT_SEL_33);
		scd_gpio_vol_sel(PE6, VOLT_SEL_33);
		
		/*connect pj0~pj5 to other interface*/
		scd_gpio_func_sel(PJ0, FUN_SEL_4);
		scd_gpio_func_sel(PJ1, FUN_SEL_4);
		scd_gpio_func_sel(PJ2, FUN_SEL_4);
		scd_gpio_func_sel(PJ3, FUN_SEL_4);
		scd_gpio_func_sel(PJ4, FUN_SEL_4);
		scd_gpio_func_sel(PJ5, FUN_SEL_4);
	}
#else

    uint32_t gpio;
    for(gpio = PE0; gpio <= PE5; gpio++)
    {
        scd_gpio_func_sel(gpio, FUN_SEL_0);

        if(CFG_DEV_VOLT_VAL(cfg) == FLASH_VOLT_1_8V)
            scd_gpio_vol_sel(gpio, VOLT_SEL_18); //don't functon, only support 3.3v
        else
            scd_gpio_vol_sel(gpio, VOLT_SEL_33);
    }

#endif

#define GPIO_E_OFFSET               SOC_IOMUX_ADDRBASE + (4*SCD_REG_OFFSET)
	// set DS of ospi 
	writel(0xff, GPIO_E_OFFSET + SCD_GPIO_R4R_REG);
	writel(0xff, GPIO_E_OFFSET + SCD_GPIO_R8R_REG);
	writel(0xff, GPIO_E_OFFSET + SCD_GPIO_SL_REG);
	writel(0xff, GPIO_E_OFFSET + SCD_GPIO_STB_REG);

	return 0;
}

int iomux_emmc_cfg(int idx, uint32_t cfg)
{
#if defined(CONFIG_ARCH_TS_TX5215) 
	scd_gpio_func_sel(PJ0, FUN_SEL_4);
	scd_gpio_func_sel(PJ1, FUN_SEL_4);
	scd_gpio_func_sel(PJ2, FUN_SEL_4);
	scd_gpio_func_sel(PJ3, FUN_SEL_4);
	scd_gpio_func_sel(PJ4, FUN_SEL_4);
	scd_gpio_func_sel(PJ5, FUN_SEL_4);
	scd_gpio_func_sel(PJ6, FUN_SEL_GPIO);

	scd_gpio_vol_sel(PJ0, VOLT_SEL_18);
	scd_gpio_vol_sel(PJ1, VOLT_SEL_18);
	scd_gpio_vol_sel(PJ2, VOLT_SEL_18);
	scd_gpio_vol_sel(PJ3, VOLT_SEL_18);
	scd_gpio_vol_sel(PJ4, VOLT_SEL_18);
	scd_gpio_vol_sel(PJ5, VOLT_SEL_18);
	scd_gpio_vol_sel(PJ6, VOLT_SEL_18);
	scd_gpio_set_direction(PJ6, GPIO_DIRECTION_INPUT);

	#else
    uint32_t gpio;
    for(gpio = PE0; gpio <= PE5; gpio++)
    {
        scd_gpio_func_sel(gpio, FUN_SEL_4);

        if(CFG_DEV_VOLT_VAL(cfg) == FLASH_VOLT_1_8V)
            scd_gpio_vol_sel(gpio, VOLT_SEL_18); //don't functon, only support 3.3v
        else
            scd_gpio_vol_sel(gpio, VOLT_SEL_33);

    }
	scd_gpio_func_sel(PD4, FUN_SEL_GPIO); //add by yangyang.liu 20230525
	scd_gpio_set_direction(PD4, GPIO_DIRECTION_INPUT);

	#endif
	return 0;
}

static int do_switch_1p8(struct cmd_tbl *cmdtp, int flag, int argc,
		    char * const argv[])
{
	bool save_env = false;

	iomux_ospi_cfg(0, FLASH_VOLT_1_8V<<FLASH_VOLT_VAL_OFS);

	if (save_env)
		env_save();

	return 0;
}

static int do_switch_3p3(struct cmd_tbl *cmdtp, int flag, int argc,
		    char * const argv[])
{
	bool save_env = false;

	iomux_ospi_cfg(0, FLASH_VOLT_3_3V<<FLASH_VOLT_VAL_OFS);
	if (save_env)
		env_save();

	return 0;
}


static int do_switch_emmc(struct cmd_tbl *cmdtp, int flag, int argc,
		    char * const argv[])
{
	bool save_env = false;

	iomux_emmc_cfg(0, 0);
	if (save_env)
		env_save();

	return 0;
}

#endif

U_BOOT_CMD(
	switch_ospi_1p8,	1,	1,	do_switch_1p8,
	"exec switch_1p8 command",
	"switch ospi votage."
);

U_BOOT_CMD(
	switch_ospi_3p3,	1,	1,	do_switch_3p3,
	"exec switch_3p3 command",
	"switch ospi votage."
);

U_BOOT_CMD(
	switch_emmc,	1,	1,	do_switch_emmc,
	"exec switch_emmc_1p8 command",
	"switch emmc 1p8 votage."
);
