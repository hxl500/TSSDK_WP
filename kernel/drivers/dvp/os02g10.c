#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/mod_devicetable.h>
#include <linux/log2.h>
#include <linux/bitops.h>
#include <linux/jiffies.h>
#include <linux/property.h>
#include <linux/acpi.h>
#include <linux/i2c.h>
#include <linux/gpio/consumer.h>


#define OS02G10_CHIP_ID 0x56024700

static struct i2c_client *os02g10_client;

struct os02g10_regval_list {
	unsigned char reg;
	unsigned char data;
};

static struct os02g10_regval_list os02g10_sw_reset[2] = {
	{0xfd, 0x00},
	{0xfc, 0x01},
};

static struct os02g10_regval_list os02g10_1920_1080_18m_5fps[] = {
	{0xfd, 0x00},
	{0x36, 0x01},

	{0xfd, 0x00},
	{0x36, 0x00},

	{0xfd, 0x00},
	//{0x20, 0x00},
//	delay 5ms

	{0xfd, 0x00},
	{0x1b, 0x00},
	{0x1e, 0xaf},

	{0xfd, 0x00},
#if 0
	{0x2e, 0x0c},
	{0x2f, 0x03},
	{0x30, 0x08},
#else
	{0x2e, 0x11},
	//{0x2f, 0x01},//48M
	//{0x2f, 0x02},//31M
	{0x2f, 0x03},//24M
	//{0x2f, 0x05}, //16M
	{0x30, 0x08},
#endif
	{0x35, 0x04},
	{0x41, 0x06},
	{0x42, 0x33},
	{0x43, 0x00},
	{0x44, 0x20},
	//PAGE1
	{0xfd, 0x01},
	//{0x03, 0x00},
	{0x03, 0x02},
	{0x04, 0x4c},
	{0x06, 0x00},
	//{0x0d, 0x01}, //test pattern....
	{0x24, 0x30},
	{0x01, 0x01},
	{0x19, 0x50},
	{0x1a, 0x0c},
	{0x1b, 0x0d},
	{0x1c, 0x00},
	{0x1d, 0x75},
	{0x1e, 0x52},
	{0x22, 0x14},
	{0x25, 0x44},
	{0x26, 0x0f},
	{0x3c, 0xca},
	{0x3d, 0x4a},
	{0x40, 0x0f},
	{0x43, 0x38},
	{0x46, 0x00},
	{0x47, 0x00},
	{0x49, 0x32},
	{0x50, 0x01},
	{0x51, 0x28},
	{0x52, 0x20},
	{0x53, 0x03},
	{0x57, 0x16},
	{0x59, 0x01},
	{0x5a, 0x01},
	{0x5d, 0x04},
	{0x6a, 0x04},
	{0x6b, 0x03},
	{0x6e, 0x28},
	{0x71, 0xbe},
	{0x72, 0x06},
	{0x73, 0x38},
	{0x74, 0x06},
	{0x79, 0x00},
	{0x7a, 0xb2},
	{0x7b, 0x10},
	{0x8f, 0x80},
	{0x91, 0x38},
	{0x92, 0x02},
	{0x9d, 0x03},
	{0x9e, 0x55},
	{0xb8, 0x70},
	{0xb9, 0x70},
	{0xba, 0x70},
	{0xbb, 0x70},
	{0xbc, 0x00},
	{0xc0, 0x00},
	{0xc1, 0x00},
	{0xc2, 0x00},
	{0xc3, 0x00},
	{0xc4, 0x6e},
	{0xc5, 0x6e},
	{0xc6, 0x6b},
	{0xc7, 0x6b},
	{0xcc, 0x11},
	{0xcd, 0xe0},
	{0xd0, 0x1b},
	{0xd2, 0x76},
	{0xd3, 0x68},
	{0xd4, 0x68},
	{0xd5, 0x73},
	{0xd6, 0x73},
	{0xe8, 0x55},
	{0xf0, 0x40},
	{0xf1, 0x40},
	{0xf2, 0x40},
	{0xf3, 0x40},
	{0xf4, 0x00},
	{0xfa, 0x1c},
	{0xfb, 0x33},
	{0xfc, 0xff},
	{0xfe, 0x01},
	//PAGE3
	{0xfd, 0x03},
	{0x03, 0x67},
	{0x00, 0x59},
	{0x04, 0x11},
	{0x05, 0x04},
	{0x06, 0x0c},
	{0x07, 0x08},
	{0x08, 0x08},
	{0x09, 0x4f},
	{0x0b, 0x08},
	{0x0d, 0x26},
	{0x0f, 0x00},
	//PAGE2
	{0xfd, 0x02},
	{0x34, 0xfe},
	{0x36, 0x08},
	{0x5e, 0x22},
	//{0xa1, 0x06},
	{0xa1, 0x01},
	{0xa3, 0x38},
	//{0xa5, 0x02},
	{0xa5, 0x01},
	{0xa7, 0x80},
	//PAGE1
	{0xfd, 0x01},
	{0x05, 0x02},
    //{0x05, 0x06}, //vblank[15:8]
  	{0x06, 0x2b}, //vblank[7:0]

	//{0x09, 0x00}, //hblank[11:8] add by lyy...
	//{0x0a, 0x00}, //hblank[7:0] 16 fsp
	//{0x09, 0x0c}, //hblank[11:8] add by lyy...
	//{0x09, 0x06},
	//{0x0a, 0x20}, //hblank[7:0] 5fsp
	{0x01, 0x01},
	//PAGE1
	{0xfd, 0x01},
};

static int i2c_sensor_read(unsigned char reg, unsigned char *data)
{
	int ret;
	struct i2c_client *client = os02g10_client;
	struct i2c_msg msgs[2];

	msgs[0].addr  = client->addr;
	msgs[0].flags = 0;
	msgs[0].len   = 1;
	msgs[0].buf   = &reg;

	msgs[1].addr  = client->addr;
	msgs[1].flags = I2C_M_RD;
	msgs[1].len   = 1;
	msgs[1].buf   = data;

	ret = i2c_transfer(client->adapter, msgs, 2);
	if (ret < 0) {
		pr_err("%s---i2c_transfer error!  ret = %d\n", __func__, ret);
		return -1;
	}

	mdelay(20);

	pr_err("[%s]: %d---read [0x%x] success! ret = %d\n", __func__, __LINE__, reg, ret);
	
	return 0;
}

static int i2c_sensor_write(unsigned char reg, unsigned char data)
{
	int ret;
	struct i2c_client *client = os02g10_client;
	struct i2c_msg msgs[1];
	unsigned char buf[2];

	buf[0] = reg;
	buf[1] = data;

	msgs[0].addr  = client->addr;
	msgs[0].flags = 0;
	msgs[0].len   = 2;
	msgs[0].buf   = buf;

	ret = i2c_transfer(client->adapter, msgs, 1);
	if (ret < 0) {
		pr_err("%s---i2c_transfer error! ret = %d\n", __func__, ret);
		return -1;
	}

	mdelay(20);

	pr_err("[%s]: %d---write [0x%x : 0x%x] success! ret = %d\n", __func__, __LINE__, reg, data, ret);

	return 0;
}

static unsigned int os02g10_get_id(void)
{
	unsigned char data[4] = {0};
	int ret;
	unsigned int id = 0;

	pr_err("[%s]: %d---\n", __func__, __LINE__);

	ret = i2c_sensor_write(0xfd, 0x00);
	if (ret < 0) {
		pr_err("%s---sensor write:error!\n", __func__);
		return -1;
	}

	ret = i2c_sensor_read(0x02, &data[0]);
	ret += i2c_sensor_read(0x03, &data[1]);
	ret += i2c_sensor_read(0x04, &data[2]);
	ret += i2c_sensor_read(0x05, &data[3]);
	if (ret < 0) {
		pr_err("[%s]: %d---read id error!\n", __func__, __LINE__);
		return -1;
	}

	id = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
	
	return id;	
}

static int os02g10_sw_rst(void)
{
	int ret, i;
	struct os02g10_regval_list *pdata = (struct os02g10_regval_list *)os02g10_sw_reset;
	
	for (i = 0; i < sizeof(os02g10_sw_reset) / sizeof(os02g10_sw_reset[0]); i++) {
		ret = i2c_sensor_write(pdata[i].reg, pdata[i].data);
		if (ret < 0) {
			pr_err("[%s]: %d---sensor write:error!\n", __func__, __LINE__);
			return -1;
		}
		mdelay(20);
	}

		pr_err("[%s]: %d---sw reset success!\n", __func__, __LINE__);

	return 0;
}

static void os02g10_power_rst(void)
{
	struct i2c_client *client = os02g10_client;
	struct gpio_desc *os02g10_gpio1;
	struct gpio_desc *os02g10_gpio2;
	
	os02g10_gpio1 = devm_gpiod_get_index(&client->dev, "os02g10", 0, GPIOD_OUT_LOW);
	os02g10_gpio2 = devm_gpiod_get_index(&client->dev, "os02g10", 1, GPIOD_OUT_LOW);

	gpiod_set_value(os02g10_gpio2, 1);
	gpiod_set_value(os02g10_gpio1, 1);
	mdelay(10);

	gpiod_set_value(os02g10_gpio2, 0);
	mdelay(10);

	gpiod_set_value(os02g10_gpio1, 0);
	mdelay(100);

	pr_err("[%s] %d---power_reset success!\n", __func__, __LINE__);

	return;
}

static int os02g10_sensor_init(void)
{
	int ret = 0, i;
	unsigned int id = 0;
	struct os02g10_regval_list *pdata = (struct os02g10_regval_list *)os02g10_1920_1080_18m_5fps;
	
	os02g10_power_rst();
	
	ret = os02g10_sw_rst();
	if (ret < 0) {
		pr_err("[%s]: %d---sensor write:error!\n", __func__, __LINE__);
		goto err;
	}

	id = os02g10_get_id();

	if(id != OS02G10_CHIP_ID) {
		pr_err("[[%s]: %d---os02g10_id error! id = 0x%08x\n", __func__, __LINE__, id);
		goto err;
	}
	
	pr_err("[%s]: %d---detected os02g10 sensor! id = 0x%08x\n", __func__, __LINE__, id);

	for (i = 0; i < sizeof(os02g10_1920_1080_18m_5fps) / sizeof(os02g10_1920_1080_18m_5fps[0]); i++) {
		ret = i2c_sensor_write(pdata[i].reg, pdata[i].data);
		if (ret < 0) {
			pr_err("[%s]: %d---sensor write:error!\n", __func__, __LINE__);
			goto err;
		}
		mdelay(20);
		if (pdata[i].reg == 0x20) {
			mdelay(20);
			continue;
		}
	}

	pr_err("[%s]: %d---os02g10 init ok!\n", __func__, __LINE__);

	return 0;
	
err:
	return -1;
}

static int os02g10_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret = 0;
	
	os02g10_client = client;

	pr_err("[%s]: %d---enter!\n", __func__, __LINE__);

	ret = os02g10_sensor_init();
	if (ret < 0) {
		pr_err("[%s]: %d---os02g10 init error!\n", __func__, __LINE__);
		return -1;
	}
	
	return 0;
}

static int os02g10_remove(struct i2c_client *client)
{
	pr_err("[%s]: %d---enter!\n", __func__, __LINE__);
	
	return 0;
}

static const struct of_device_id of_os02g10_match[] = {
	{ .compatible = "os02g10"},
	{ /* END OF LIST */ }
};
MODULE_DEVICE_TABLE(of, of_os02g10_match);

static const struct i2c_device_id os02g10_ids[] = {
	{"os02g10", 0},
	{ }
};

MODULE_DEVICE_TABLE(i2c, os02g10_ids);

static struct i2c_driver os02g10_driver = {
	.driver = {
		.name = "os02g10_sensor",
		.of_match_table  = of_os02g10_match,
	},
	.probe = os02g10_probe,
	.remove = os02g10_remove,
	.id_table = os02g10_ids,
};

static int __init os02g10_init(void)
{
	pr_err("[%s]: %d---enter!\n", __func__, __LINE__);
	
	return i2c_add_driver(&os02g10_driver);
}
module_init(os02g10_init);

static void __exit os02g10_exit(void)
{
	pr_err("[%s]: %d---enter!\n", __func__, __LINE__);
	
	i2c_del_driver(&os02g10_driver);
}
module_exit(os02g10_exit);

MODULE_DESCRIPTION("image sensor os02g10 driver");
MODULE_LICENSE("GPL v2");
