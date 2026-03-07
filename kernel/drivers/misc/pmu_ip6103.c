/*
 * Copyright (C) 2022 TsingMicro.
 * IP6103 power manager unit driver.
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/mutex.h>

#define IP6103_DRV_NAME	"ip6103"
#define DRIVER_VERSION		"1.0"

/*
 * Defines
 */
//#define PMU_IIC_IDX		  0
//#define PMU_SLAVE_ADDR	0x30
//#define IIC_ADD_SIZE_BIT		8

#define MUL_10K(x)		 (x * 10000)
typedef enum{
	PMU_DISABLE = 0,
	PMU_ENABLE =  1
} pmu_state_e;

typedef enum{
	LDO0 = 0,
	LDO1 = 1,
	LDO2 = 2,
	LDO4 = 4,
	LDO5 = 5,
	LDO6 = 6
} pmu_ldox_num_e;

typedef enum{
	DCDC0 = 0,
	DCDC1 = 1,
	DCDC2 = 2,
	DCDC3 = 3
} pmu_dcdcx_num_e;

/*IP6103 Register Define*/
#define PMU_CONTROL_0			0x00
#define PMU_CONTROL_1			0x01
#define PMU_CONTROL_2			0x02
#define PMU_CONTROL_3			0x03
#define PMU_CONTROL_4			0x04
#define PMU_CONTROL_5			0x05
#define PMU_CONTROL_6			0x06
#define PMU_CONTROL_7			0x07
#define PMU_CONTROL_8			0x08
#define PMU_CONTROL_9			0x11

#define PMU_IRC_CTL_0			0x13
#define PMU_IRC_CTL_1			0x14
#define PMU_IRC_CTL_2			0x15
#define PMU_IRC_CTL_3			0x16
#define PMU_IRC_CTL_4			0x17
#define PMU_IRC_CTL_5			0x18
#define PMU_IRC_CTL_6			0x19
#define PMU_IRC_CTL_7			0x1A

#define PMU_OWR_OFF_FLAG		0x1B

#define PMU_DCDC_CTL			0x20	/*DCDC Control*/
#define PMU_DCDC0_0				0x21	/*DCDC0 Voltage Setting*/
#define PMU_DCDC0_1				0x21	/*DCDC0 Stepwize Interval*/
#define PMU_DCDC1_0				0x28	/*DCDC1 Voltage Setting*/
#define PMU_DCDC1_1				0x29	/*DCDC1 Stepwize Interval*/
#define PMU_DCDC2_0				0x2F	/*DCDC2 Voltage Setting*/
#define PMU_DCDC2_1				0x30	/*DCDC2 Stepwize Interval*/
#define PMU_DCDC3_0				0x36	/*DCDC3 Voltage Setting*/
#define PMU_DCDC3_1				0x37	/*DCDC3 Stepwize Interval*/

#define PMU_SW_LDO_CTL_0		0x40
#define PMU_SW_LDO_CTL_1		0x41	/*LDOx Enable*/
#define PMU_LDO0				0x42	/*LDO0 Voltage Setting*/
#define PMU_LDO1				0x44	/*LDO1 Voltage Setting*/
#define PMU_LDO2				0x46	/*LDO2 Voltage Setting*/
#define PMU_LDO4				0x4A	/*LDO4 Voltage Setting*/
#define PMU_LDO5				0x4C	/*LDO5 Voltage Setting*/
#define PMU_LDO6				0x4E	/*LDO6 Voltage Setting*/

#define PMU_SVCC				0x51	/*SVCC Voltage Setting*/

#define PMU_INTS_0				0x60
#define PMU_INTS_1				0x61	/*LDOx Overcurrent Interrupt Enable*/
#define PMU_INTS_2				0x62
#define PMU_INTS_3				0x63	/*LDOx Overcurrent Interrupt Flag*/

#define PMU_RTC					0x70
#define PMU_RTC_ALARM_SEC		0x71
#define PMU_RTC_ALARM_MIN		0x72
#define PMU_RTC_ALARM_HOUR		0x73
#define PMU_RTC_ALARM_DAY		0x74
#define PMU_RTC_ALARM_MONTH		0x75
#define PMU_RTC_ALARM_YEAR		0x76
#define PMU_RTC_TIME_SEC		0x77
#define PMU_RTC_TIME_MIN		0x78
#define PMU_RTC_TIME_HOUR		0x79
#define PMU_RTC_TIME_DAY		0x7A
#define PMU_RTC_TIME_MONTH		0x7B
#define PMU_RTC_TIME_YEAR		0x7C

#define PMU_MFP_0				0x80
#define PMU_MFP_1				0x81
#define PMU_MFP_2				0x82
#define PMU_MFP_3				0x83
#define PMU_MFP_4				0x84
#define PMU_MFP_5				0x85
#define PMU_MFP_6				0x86
#define PMU_MFP_7				0x87

#define PMU_I2C_0				0xFD
#define PMU_I2C_1				0xFE	/*PMU I2C Address Reg*/

/*bit define*/
#define PMU_DCDC_CTL_FREQ_SPREAD_SHIFT		   0x7
#define PMU_DCDC_CTL_DC3_EN_SHIFT			   0x3
#define PMU_DCDC_CTL_DC2_EN_SHIFT			   0x2
#define PMU_DCDC_CTL_DC1_EN_SHIFT			   0x1
#define PMU_DCDC_CTL_DC0_EN_SHIFT			   0x0

#define PMU_DCDC0_0_VOL_SET_SHIFT			   0x0
#define PMU_DCDC0_0_VOL_SET_MASK			   0x7F
#define PMU_DCDC1_0_VOL_SET_SHIFT			   0x0
#define PMU_DCDC1_0_VOL_SET_MASK			   0x7F
#define PMU_DCDC2_0_VOL_SET_SHIFT			   0x0
#define PMU_DCDC2_0_VOL_SET_MASK			   0x7F
#define PMU_DCDC3_0_VOL_SET_SHIFT			   0x0
#define PMU_DCDC3_0_VOL_SET_MASK			   0x7F

#define PMU_MFP_2_EXTP_EN_MFP_SHIFT			   0x3
#define PMU_MFP_2_EXTP_EN_MFP_MASK			   0x3
#define PMU_MFP_2_GPIO2_OEN_SHIFT			   0x2
#define PMU_MFP_2_GPIO2_IEN_SHIFT			   0x1
#define PMU_MFP_2_GPIO2_DATA_SHIFT			   0x0

#define PMU_MFP_3_LOSC_MFP_SHIFT				0x6
#define PMU_MFP_3_LOSCO_GPIO1_OEN_SHIFT			0x5
#define PMU_MFP_3_LOSCO_GPIO1_IEN_SHIFT			0x4
#define PMU_MFP_3_LOSCO_GPIO1_DATA_SHIFT		0x3
#define PMU_MFP_3_LOSCI_GPIO0_OEN_SHIFT			0x2
#define PMU_MFP_3_LOSCI_GPIO0_IEN_SHIFT			0x1
#define PMU_MFP_3_LOSCI_GPIO0_DATA_SHIFT		0x0

/* DCDC voltage config info. unit 0.1mv */
#define PMU_DCDC_STEP			(125)//0.0125l
#define PMU_DCDC0_MIN_VOL		(6000)//0.6l
#define PMU_DCDC0_MAX_VOL		(21750)//2.175l
#define PMU_DCDC1_MIN_VOL		(6000)//0.6l
#define PMU_DCDC1_MAX_VOL		(21750)//2.175l
#define PMU_DCDC2_MIN_VOL		(6000)//0.6l
#define PMU_DCDC2_MAX_VOL		(21750)//2.175l
#define PMU_DCDC3_MIN_VOL		(22000)//2.2l
#define PMU_DCDC3_MAX_VOL		(35000)//3.5l

/*DCDC voltage config info*/
#define PMU_LDO_STEP			(250)//0.025l
#define PMU_LDO_MIN_VOL			(7000)//0.7l
#define PMU_LDO_MAX_VOL			(34000)//3.4l

/*MFP*/
typedef enum{
	LOSC_AS_GPIO = 0,
	LOSC_FUNC = 1
} pmu_losc_func_e;

typedef enum{
	IR_AS_GPIO = 0,
	IR_AS_EXTP_EN = 1,
	IR_AS_IR = 2,
	IR_AS_IRQS = 3
} pmu_ir_func_e;

typedef enum{
	GPIO_INPUT = 0,
	GPIO_OUTPUT = 1
} pmu_gpio_dir_e;

typedef enum{
	GPIO_LOW = 0,
	GPIO_HIGH = 1
} pmu_gpio_value_e;

/*
 * Structs
 */
struct ip6103_data {
	struct i2c_client *client;
	struct mutex update_lock;

	unsigned int power_state:1; //1:open all
	//unsigned int operating_mode:1; //1:open all.
};

/*
 * Management functions
 */
int pmu_i2c_addr_read(struct i2c_client *client)
{
	int ret;

	ret = i2c_smbus_read_byte_data(client, PMU_I2C_1);
	if (ret < 0) {
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);
		return ret;
	}

	dev_info(&client->dev, "PMU_I2C_1 = 0x%x\n", ret);

	return ret;
}

int32_t pmu_dcdcx_enable(struct i2c_client *client, pmu_dcdcx_num_e dcdcx, pmu_state_e flag)
{
	int32_t	 ret = 0;
	uint8_t	 value = 0;
	uint8_t dcdcx_en_shift = 0;

	dcdcx_en_shift = dcdcx;

	ret = i2c_smbus_read_byte_data(client, PMU_DCDC_CTL);
	if (ret < 0) {
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);
		return ret;
	}
	value = ret;
	value &= ~(1<<dcdcx_en_shift);
	value |= (flag<<dcdcx_en_shift);

	ret = i2c_smbus_write_byte_data(client, PMU_DCDC_CTL, value);
	if (ret) {
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);
		return ret;
	}

	return ret;
}

int32_t pmu_dcdcx_voltage_set(struct i2c_client *client, pmu_dcdcx_num_e dcdcx, uint32_t vol)
{
	int32_t	 ret = 0;
	uint8_t	 value = 0;
	uint8_t	 vol_value = 0;
	uint8_t reg = 0;
	uint32_t vol_min = 0;
	uint32_t vol_max = 0;

	switch (dcdcx) {
	case DCDC0:
		vol_min = PMU_DCDC0_MIN_VOL;
		vol_max = PMU_DCDC0_MAX_VOL;
		reg = PMU_DCDC0_0;
		break;
	case DCDC1:
		vol_min = PMU_DCDC1_MIN_VOL;
		vol_max = PMU_DCDC1_MAX_VOL;
		reg = PMU_DCDC1_0;
		break;
	case DCDC2:
		vol_min = PMU_DCDC2_MIN_VOL;
		vol_max = PMU_DCDC2_MAX_VOL;
		reg = PMU_DCDC2_0;
		break;
	case DCDC3:
		vol_min = PMU_DCDC3_MIN_VOL;
		vol_max = PMU_DCDC3_MAX_VOL;
		reg = PMU_DCDC3_0;
		break;

	default:
		break;
	}

	if (vol < vol_min || vol > vol_max) {
		dev_info(&client->dev, "%s(%d):vol=%d is out of range!\n", __func__, __LINE__, vol);
		return 1;
	}

	vol_value = DIV_ROUND_CLOSEST(vol - vol_min, PMU_DCDC_STEP);
	vol_value = DIV_ROUND_CLOSEST((uint32_t)(vol - vol_min), PMU_DCDC_STEP);
	dev_info(&client->dev, "%s(%d):dcdc%d set value=0x%x\n", __func__, __LINE__, dcdcx, vol_value);

	ret = i2c_smbus_read_byte_data(client, reg);
	if (ret < 0)
		return ret;

	value = ret;
	value &= ~(PMU_DCDC0_0_VOL_SET_MASK << PMU_DCDC0_0_VOL_SET_SHIFT);
	value |= ((vol_value & PMU_DCDC0_0_VOL_SET_MASK) << PMU_DCDC0_0_VOL_SET_SHIFT);

	ret = i2c_smbus_write_byte_data(client, reg, value);

	ret = i2c_smbus_read_byte_data(client, reg);
	if (ret < 0) {
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);
		return ret;
	}
	value = ret;
	dev_info(&client->dev, "reg = 0x%x\n", value);

	return ret;
}

int32_t pmu_ldox_enable(struct i2c_client *client, pmu_ldox_num_e ldox, pmu_state_e flag)
{
	int32_t ret = 0;
	uint8_t value = 0;
	uint8_t ldox_en_shift = 0;

	ldox_en_shift = ldox;

	ret = i2c_smbus_read_byte_data(client, PMU_SW_LDO_CTL_1);
	if (ret < 0) {
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);
		return ret;
	}
	value = ret;

	value &= ~(1<<ldox_en_shift);
	value |= (flag<<ldox_en_shift);

	ret = i2c_smbus_write_byte_data(client, PMU_SW_LDO_CTL_1, value);
	if (ret) {
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);
		return ret;
	}

	ret = i2c_smbus_read_byte_data(client, PMU_SW_LDO_CTL_1);
	if (ret < 0)
		return ret;

	value = ret;
	dev_info(&client->dev, "PMU_SW_LDO_CTL_1 = 0x%x\n", value);

	return ret;
}

int32_t pmu_ldox_voltage_set(struct i2c_client *client, pmu_ldox_num_e ldox, uint32_t vol)
{
	int32_t ret = 0;
	uint8_t value = 0;
	uint8_t reg = 0;

	if (vol < PMU_LDO_MIN_VOL || vol > PMU_LDO_MAX_VOL) {
		dev_info(&client->dev, "%s(%d):ldo%d,vol=%d is out of range!\n", __func__, __LINE__, ldox, vol);
		return 1;
	}

	switch (ldox) {
	case LDO0:
		reg = PMU_LDO0;
		break;
	case LDO1:
		reg = PMU_LDO1;
		break;
	case LDO2:
		reg = PMU_LDO2;
		break;
	case LDO4:
		reg = PMU_LDO4;
		break;
	case LDO5:
		reg = PMU_LDO5;
		break;
	case LDO6:
		reg = PMU_LDO6;
		break;
	default:
		break;
	}

	value =	 DIV_ROUND_CLOSEST((vol - PMU_LDO_MIN_VOL), PMU_LDO_STEP);
	dev_info(&client->dev, "%s(%d):ldo%d set value=0x%x\n", __func__, __LINE__, ldox, value);

	ret = i2c_smbus_write_byte_data(client, reg, value);

	return ret;
}

int pmu_losc_mfp_sel(struct i2c_client *client, pmu_losc_func_e losc_fun)
{
	int32_t ret = 0;
	uint8_t value = 0;

	ret = i2c_smbus_read_byte_data(client, PMU_MFP_3);
	if (ret < 0) {
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);
		return ret;
	}
	value = ret;

	value &= ~(1<<PMU_MFP_3_LOSC_MFP_SHIFT);
	value |= (losc_fun<<PMU_MFP_3_LOSC_MFP_SHIFT);

	ret = i2c_smbus_write_byte_data(client, PMU_MFP_3, value);
	if (ret)
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);


	return ret;
}

int pmu_losco_gpio1_dir_set(struct i2c_client *client, pmu_gpio_dir_e gpio_dir)
{
	int32_t ret = 0;
	uint8_t value = 0;

	ret = i2c_smbus_read_byte_data(client, PMU_MFP_3);
	if (ret < 0) {
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);
		return ret;
	}
	value = ret;

	if (gpio_dir == GPIO_INPUT) {
		value &= ~(1<<PMU_MFP_3_LOSCO_GPIO1_OEN_SHIFT);
		value |= (1<<PMU_MFP_3_LOSCO_GPIO1_IEN_SHIFT);
	} else{
		value &= ~(1<<PMU_MFP_3_LOSCO_GPIO1_IEN_SHIFT);
		value |= (1<<PMU_MFP_3_LOSCO_GPIO1_OEN_SHIFT);
	}

	ret = i2c_smbus_write_byte_data(client, PMU_MFP_3, value);
	if (ret) {
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);
		return ret;
	}

	return 0;
}

int pmu_losco_gpio1_value_set(struct i2c_client *client, pmu_gpio_value_e data)
{
	int32_t ret = 0;
	uint8_t value = 0;

	ret = i2c_smbus_read_byte_data(client, PMU_MFP_3);
	if (ret < 0) {
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);
		return ret;
	}
	value = ret;

	value &= ~(1<<PMU_MFP_3_LOSCO_GPIO1_DATA_SHIFT);
	value |= (data<<PMU_MFP_3_LOSCO_GPIO1_DATA_SHIFT);

	ret = i2c_smbus_write_byte_data(client, PMU_MFP_3, value);
	if (ret)
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);


	return ret;
}

int pmu_losci_gpio0_dir_set(struct i2c_client *client, pmu_gpio_dir_e gpio_dir)
{
	int32_t ret = 0;
	uint8_t value = 0;

	ret = i2c_smbus_read_byte_data(client, PMU_MFP_3);
	if (ret < 0) {
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);
		return ret;
	}
	value = ret;

	if (gpio_dir == GPIO_INPUT) {
		value &= ~(1<<PMU_MFP_3_LOSCI_GPIO0_OEN_SHIFT);
		value |= (1<<PMU_MFP_3_LOSCI_GPIO0_IEN_SHIFT);
	} else{
		value &= ~(1<<PMU_MFP_3_LOSCI_GPIO0_IEN_SHIFT);
		value |= (1<<PMU_MFP_3_LOSCI_GPIO0_OEN_SHIFT);
	}

	ret = i2c_smbus_write_byte_data(client, PMU_MFP_3, value);
	if (ret)
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);


	return ret;
}

int pmu_losci_gpio0_value_set(struct i2c_client *client, pmu_gpio_value_e data)
{
	int32_t ret = 0;
	uint8_t value = 0;

	ret = i2c_smbus_read_byte_data(client, PMU_MFP_3);
	if (ret < 0) {
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);
		return ret;
	}
	value = ret;

	value &= ~(1<<PMU_MFP_3_LOSCI_GPIO0_DATA_SHIFT);
	value |= (data<<PMU_MFP_3_LOSCI_GPIO0_DATA_SHIFT);

	ret = i2c_smbus_write_byte_data(client, PMU_MFP_3, value);
	if (ret)
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);


	return ret;
}


int pmu_ir_mfp_sel(struct i2c_client *client, pmu_ir_func_e ir_fun)
{
	int32_t ret = 0;
	uint8_t value = 0;

	ret = i2c_smbus_read_byte_data(client, PMU_MFP_2);
	if (ret < 0) {
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);
		return ret;
	}
	value = ret;

	value &= ~(PMU_MFP_2_EXTP_EN_MFP_MASK<<PMU_MFP_2_EXTP_EN_MFP_SHIFT);
	value |= (ir_fun<<PMU_MFP_2_EXTP_EN_MFP_SHIFT);

	ret = i2c_smbus_write_byte_data(client, PMU_MFP_2, value);
	if (ret)
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);


	return ret;
}

int pmu_ir_gpio2_dir_set(struct i2c_client *client, pmu_gpio_dir_e gpio_dir)
{
	int32_t ret = 0;
	uint8_t value = 0;

	ret = i2c_smbus_read_byte_data(client, PMU_MFP_2);
	if (ret < 0) {
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);
		return ret;
	}
	value = ret;

	if (gpio_dir == GPIO_INPUT) {
		value &= ~(1<<PMU_MFP_2_GPIO2_OEN_SHIFT);
		value |= (1<<PMU_MFP_2_GPIO2_IEN_SHIFT);
	} else{
		value &= ~(1<<PMU_MFP_2_GPIO2_IEN_SHIFT);
		value |= (1<<PMU_MFP_2_GPIO2_OEN_SHIFT);
	}

	ret = i2c_smbus_write_byte_data(client, PMU_MFP_2, value);
	if (ret)
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);


	return ret;
}

int pmu_ir_gpio2_value_set(struct i2c_client *client, pmu_gpio_value_e data)
{
	int32_t ret = 0;
	uint8_t value = 0;

	ret = i2c_smbus_read_byte_data(client, PMU_MFP_2);
	if (ret < 0) {
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);
		return ret;
	}
	value = ret;

	value &= ~(1<<PMU_MFP_2_GPIO2_DATA_SHIFT);
	value |= (data<<PMU_MFP_2_GPIO2_DATA_SHIFT);

	ret = i2c_smbus_write_byte_data(client, PMU_MFP_2, value);
	if (ret)
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);


	return ret;
}

static int ip6103_set_power_state(struct i2c_client *client, int state)
{
	struct ip6103_data *data = i2c_get_clientdata(client);
	int ret;

	if (state == 0) {
		//ret = pmu_dcdcx_voltage_set(client, DCDC0, 0.9); //RCE_0V9
		ret += pmu_dcdcx_enable(client, DCDC0, PMU_DISABLE);

		//ret += pmu_dcdcx_voltage_set(client, DCDC2, 0.9); //AE_0V9
		ret += pmu_dcdcx_enable(client, DCDC2, PMU_DISABLE);

		//ret += pmu_ldox_voltage_set(client, LDO2, 1.8); //ADC_1V8
		ret += pmu_ldox_enable(client, LDO2, PMU_DISABLE);

		//ret += pmu_losc_mfp_sel(client, LOSC_AS_GPIO);
		//ret += pmu_losci_gpio0_dir_set(client, GPIO_OUTPUT);
		ret += pmu_losci_gpio0_value_set(client, GPIO_LOW); //VPU_EN

		//ret += pmu_losco_gpio1_dir_set(client, GPIO_OUTPUT);
		// ret += pmu_losco_gpio1_value_set(client, GPIO_LOW); //DDR_EN, USED

		//ret += pmu_ir_mfp_sel(client, IR_AS_GPIO);
		//ret += pmu_ir_gpio2_dir_set(client, GPIO_OUTPUT); //RNE_PWR_EN
		ret += pmu_ir_gpio2_value_set(client, GPIO_LOW);

	} else {
		ret = pmu_dcdcx_voltage_set(client, DCDC0, MUL_10K(0.9)); //RCE_0V9
		ret += pmu_dcdcx_enable(client, DCDC0, PMU_ENABLE);

		ret += pmu_dcdcx_voltage_set(client, DCDC2, MUL_10K(0.9)); //AE_0V9
		ret += pmu_dcdcx_enable(client, DCDC2, PMU_ENABLE);

		ret += pmu_ldox_voltage_set(client, LDO2, MUL_10K(1.8)); //ADC_1V8
		ret += pmu_ldox_enable(client, LDO2, PMU_ENABLE);

		ret += pmu_losc_mfp_sel(client, LOSC_AS_GPIO);
		ret += pmu_losci_gpio0_dir_set(client, GPIO_OUTPUT);
		ret += pmu_losci_gpio0_value_set(client, GPIO_HIGH); //VPU_EN

		// ret += pmu_losco_gpio1_dir_set(client, GPIO_OUTPUT);
		// ret += pmu_losco_gpio1_value_set(client, GPIO_HIGH); //DDR_EN, USED

		ret += pmu_ir_mfp_sel(client, IR_AS_GPIO);
		ret += pmu_ir_gpio2_dir_set(client, GPIO_OUTPUT); //RNE_PWR_EN
		ret += pmu_ir_gpio2_value_set(client, GPIO_HIGH);
	}

	data->power_state = state;

	return ret;
}

/*
 * SysFS support
 */
static ssize_t ip6103_show_power_state(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct ip6103_data *data = i2c_get_clientdata(to_i2c_client(dev));

	return sprintf(buf, "%u\n", data->power_state);
}

static ssize_t ip6103_store_power_state(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct ip6103_data *data = i2c_get_clientdata(client);
	unsigned long val;
	int ret;

	ret = kstrtoul(buf, 0, &val);
	if (ret) {
		dev_err(dev, "kstrtoul error: %d\n", ret);
		return ret;
	}
	if (val > 1)
		return -EINVAL;

	mutex_lock(&data->update_lock);
	ret = ip6103_set_power_state(client, val);
	mutex_unlock(&data->update_lock);

	if (ret < 0)
		return ret;

	return count;
}

static DEVICE_ATTR(power_state, 0644 /*S_IWUSR | S_IRUGO*/,
		   ip6103_show_power_state, ip6103_store_power_state);


static struct attribute *ip6103_attributes[] = {
	&dev_attr_power_state.attr,
	//&dev_attr_operating_mode.attr,
	//&dev_attr_lux1_input.attr,
	NULL
};

static const struct attribute_group ip6103_attr_group = {
	.attrs = ip6103_attributes,
};

/*
 * Initialization function
 */

static int ip6103_init_client(struct i2c_client *client)
{
	struct ip6103_data *data = i2c_get_clientdata(client);
	int err, ret;

	/*
	 * Probe the chip. To do so we try to power up all the device.
	 */

	err = pmu_i2c_addr_read(client);
	//if (err < 0)
		//return err;

	ret = pmu_dcdcx_voltage_set(client, DCDC0, 0.9); //RCE_0V9
	ret += pmu_dcdcx_enable(client, DCDC0, PMU_ENABLE);

	ret += pmu_dcdcx_voltage_set(client, DCDC2, 0.9); //AE_0V9
	ret += pmu_dcdcx_enable(client, DCDC2, PMU_ENABLE);

	ret += pmu_ldox_voltage_set(client, LDO2, 1.8); //ADC_1V8
	ret += pmu_ldox_enable(client, LDO2, PMU_ENABLE);

	ret += pmu_losc_mfp_sel(client, LOSC_AS_GPIO);
	ret += pmu_losci_gpio0_dir_set(client, GPIO_OUTPUT);
	ret += pmu_losci_gpio0_value_set(client, GPIO_HIGH); //VPU_EN

	// ret += pmu_losco_gpio1_dir_set(client, GPIO_OUTPUT);
	// ret += pmu_losco_gpio1_value_set(client, GPIO_HIGH); //DDR_EN, USED

	ret += pmu_ir_mfp_sel(client, IR_AS_GPIO);
	ret += pmu_ir_gpio2_dir_set(client, GPIO_OUTPUT); //RNE_PWR_EN
	ret += pmu_ir_gpio2_value_set(client, GPIO_HIGH);

	data->power_state = 1;

	return 0;
}

/*
 * I2C init/probing/exit functions
 */

static struct i2c_driver ip6103_driver;
static int ip6103_probe(struct i2c_client *client,
				   const struct i2c_device_id *id)
{
	struct i2c_adapter *adapter = to_i2c_adapter(client->dev.parent);
	struct ip6103_data *data;
	int err = 0;

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_WRITE_BYTE
						| I2C_FUNC_SMBUS_READ_BYTE_DATA)) {
		err = -EIO;
		goto exit;
	}

	data = kzalloc(sizeof(struct ip6103_data), GFP_KERNEL);
	if (!data) {
		err = -ENOMEM;
		goto exit;
	}
	data->client = client;
	i2c_set_clientdata(client, data);

	mutex_init(&data->update_lock);

	/* Initialize the IP6103 chip */
	err = ip6103_init_client(client);
	if (err)
		goto exit_kfree;

	/* Register sysfs hooks */
	err = sysfs_create_group(&client->dev.kobj, &ip6103_attr_group);
	if (err)
		goto exit_kfree;

	dev_info(&client->dev, "support ver. %s enabled\n", DRIVER_VERSION);

	return 0;

exit_kfree:
	kfree(data);
exit:
	return err;
}

static int ip6103_remove(struct i2c_client *client)
{
	sysfs_remove_group(&client->dev.kobj, &ip6103_attr_group);

	/* Power down the device */
	ip6103_set_power_state(client, 0);

	kfree(i2c_get_clientdata(client));

	return 0;
}

#ifdef CONFIG_PM_SLEEP

static int ip6103_suspend(struct device *dev)
{
	return ip6103_set_power_state(to_i2c_client(dev), 0);
}

static int ip6103_resume(struct device *dev)
{
	return ip6103_set_power_state(to_i2c_client(dev), 1);
}

static SIMPLE_DEV_PM_OPS(ip6103_pm_ops, ip6103_suspend, ip6103_resume);
#define IP6103_PM_OPS (&ip6103_pm_ops)

#else

#define IP6103_PM_OPS NULL

#endif /* CONFIG_PM_SLEEP */

static const struct i2c_device_id ip6103_id[] = {
	{ "ip6103", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, ip6103_id);

static const struct of_device_id ip6103_of_match[] = {
	{ .compatible = "pmu,ip6103" },
	{ }
};
MODULE_DEVICE_TABLE(of, ip6103_of_match);

static struct i2c_driver ip6103_driver = {
	.driver = {
		.name	= IP6103_DRV_NAME,
		.of_match_table = ip6103_of_match,
		.pm	= IP6103_PM_OPS,
	},
	.probe	= ip6103_probe,
	.remove	= ip6103_remove,
	.id_table = ip6103_id,
};

module_i2c_driver(ip6103_driver);

MODULE_AUTHOR("TsingMicro");
MODULE_DESCRIPTION("ip6103 driver");
MODULE_LICENSE("GPL v2");
MODULE_VERSION(DRIVER_VERSION);
