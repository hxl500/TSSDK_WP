// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2022 TsingMicro.Inc
 */

#include <common.h>
#include <command.h>
#include <dm.h>
#include <asm/io.h>
#include <env.h>
#include <dm/pinctrl.h>
#include <dm/uclass-internal.h>
#include <linux/err.h>
#include <asm/gpio.h>
#include <tx536.h>
#include <linux/delay.h>

#define DEVICE_ID_1_NAME	"PD21"
#define DEVICE_ID_2_NAME	"PD22"
#define MP_TO_COPX_CTRL_NAME	"PD8"
#define MP_TO_COP0_CTRL_NAME	"PD8"
#define MP_TO_COP1_CTRL_NAME	"PD25"
#define MP_TO_COP2_CTRL_NAME	"PD26"

#define DEVICE_ID_1_PIN		(21 + (32 * 3))
#define DEVICE_ID_2_PIN		(22 + (32 * 3))
#define MP_TO_COPX_CTRL_PIN	(8 + (32 * 3))
#define MP_TO_COP0_CTRL_PIN	(8 + (32 * 3))
#define MP_TO_COP1_CTRL_PIN	(25 + (32 * 3))
#define MP_TO_COP2_CTRL_PIN	(26 + (32 * 3))

/* 1. get boardname from dts(evb return 0.)
 * 2. get device id from gpio.
 * 3. modify device_id, ipaddr, serverip, slave bootcmd.
 */

int get_gpio_value(uint32_t gpio, const char *str_gpio)
{
	int value;
	int ret;

	if (str_gpio == NULL)
		return -1;

#if defined(CONFIG_DM_GPIO)
	/*
	 * TODO(sjg@chromium.org): For now we must fit into the existing GPIO
	 * framework, so we look up the name here and convert it to a GPIO number.
	 * Once all GPIO drivers are converted to driver model, we can change the
	 * code here to use the GPIO uclass interface instead of the numbered
	 * GPIO compatibility layer.
	 */
	ret = gpio_lookup_name(str_gpio, NULL, NULL, &gpio);
	if (ret)
		printf("GPIO: '%s' not found\n", str_gpio);

#else
	/* turn the gpio name into a gpio number */
	ret = name_to_gpio(str_gpio);
	if (ret >= 0)
		gpio = ret;

#endif
	/* grab the pin before we tweak it */
	ret = gpio_request(gpio, "cmd_gpio");
	if (ret && ret != -EBUSY) {
		printf("gpio: requesting pin %u failed\n", gpio);
		return -1;
	}
	gpio_direction_input(gpio);
	value = gpio_get_value(gpio);
	//printf("gpio: pin %s (gpio %u) value is ", str_gpio, gpio);

	if (IS_ERR_VALUE(value)) {
		printf("unknown (ret=%d)\n", value);
		goto err;
	}

	if (ret != -EBUSY)
		gpio_free(gpio);
	/*
	 * Whilst wrong, the legacy gpio input command returns the pin
	 * value, or CMD_RET_FAILURE (which is indistinguishable from a
	 * valid pin value).
	 */
	return value;

err:
	if (ret != -EBUSY)
		gpio_free(gpio);
	return -1;
}

int get_box_device_id(void)
{
#if 0
	struct udevice *dev;

#define LIMIT_DEVNAME	30
	uclass_foreach_dev_probe(UCLASS_PINCTRL, dev) {
		printf("| %-*.*s| %-*.*s| %s\n",
		       LIMIT_DEVNAME, LIMIT_DEVNAME, dev->name,
		       LIMIT_DEVNAME, LIMIT_DEVNAME, dev->driver->name,
		       dev->parent->name);
	}
#endif
	int dev_id = 1, ret;

	ret = get_gpio_value(DEVICE_ID_1_PIN, DEVICE_ID_1_NAME);
	if (ret < 0)
		goto err;

	dev_id = ret;

	ret = get_gpio_value(DEVICE_ID_2_PIN, DEVICE_ID_2_NAME);
	if (ret < 0)
		goto err;

	dev_id |= (ret << 1);

	return dev_id;

err:
	return -1;
}

int set_gpio_value(uint32_t gpio, const char *str_gpio, int value)
{
	int ret;

	if (str_gpio == NULL)
		return -1;

#if defined(CONFIG_DM_GPIO)
	/*
	 * TODO(sjg@chromium.org): For now we must fit into the existing GPIO
	 * framework, so we look up the name here and convert it to a GPIO number.
	 * Once all GPIO drivers are converted to driver model, we can change the
	 * code here to use the GPIO uclass interface instead of the numbered
	 * GPIO compatibility layer.
	 */
	ret = gpio_lookup_name(str_gpio, NULL, NULL, &gpio);
	if (ret)
		printf("GPIO: '%s' not found\n", str_gpio);

#else
	/* turn the gpio name into a gpio number */
	ret = name_to_gpio(str_gpio);
	if (ret >= 0)
		gpio = ret;

#endif
	/* grab the pin before we tweak it */
	ret = gpio_request(gpio, "cmd_gpio");
	if (ret && ret != -EBUSY) {
		printf("gpio: requesting pin %u failed\n", gpio);
		return -1;
	}
	gpio_direction_output(gpio, value);
	//printf("gpio: pin %s (gpio %u) value is ", str_gpio, gpio);

	if (IS_ERR_VALUE(value)) {
		printf("unknown (ret=%d)\n", value);
		goto err;
	}

	if (ret != -EBUSY)
		gpio_free(gpio);
	/*
	 * Whilst wrong, the legacy gpio input command returns the pin
	 * value, or CMD_RET_FAILURE (which is indistinguishable from a
	 * valid pin value).
	 */
	return value;

err:
	if (ret != -EBUSY)
		gpio_free(gpio);
	return -1;
}

int wait_mast_processor_done(void)
{
	int mp_status = 0, i = 0, ret;

	while (mp_status == 0) {
		udelay(1 * 1000000);
		ret = get_gpio_value(MP_TO_COPX_CTRL_PIN, MP_TO_COPX_CTRL_NAME);
		if (ret < 0)
			printf("Can not get MP_TO_COPX_CTRL %d status\r\n", MP_TO_COPX_CTRL_PIN);

		mp_status = ret;
		printf("Wait mast processor setup %6d\r", ++i);
	}

	printf("Wait mast processor setup done.\r\n");
	return mp_status;
}

static int do_get_device_id(struct cmd_tbl *cmdtp, int flag, int argc,
		    char * const argv[])
{
	char boardname[32];
	struct tx536_model *models = NULL;
	uint32_t models_count, index, device_id = 1;
	bool save_env = false;
	const char *str_cmd;

	models = (struct tx536_model *)tx536_models_scheme;
	models_count = ARRAY_SIZE(tx536_models_scheme);
	snprintf(boardname, sizeof(boardname), "%s", env_get("boardname"));

	for (index = 0; index < models_count; index++) {
		if (strncmp(boardname, models[index].boardname, sizeof(boardname)) == 0) {
			//debug ("%s: get board %s\n", __func__, models[index].boardname);
			break;
		}
	}

	switch (index) {
	case TX536_BOX:
		if (argc > 1) {
			str_cmd = argv[1];
			device_id = simple_strtoul(str_cmd, NULL, 10);
		} else {
			device_id = get_box_device_id();
		}
		if (device_id >= 0 && device_id != env_get_ulong("device_id", 10, 0)) {
			env_set_ulong("device_id", device_id);
			printf("%s device_id %d\n", argc > 1 ? "set":"get", device_id);
		}

		switch (device_id) {
		case 1:
		if (strcmp(env_get("ipaddr"), env_get("ipaddr1"))) {
			env_set("ipaddr", env_get("ipaddr1"));
			env_set("serverip", env_get("ipaddr0"));
			env_set("bootmode", "tftp_kernel");
			save_env = true;
		}

		if (env_get_ulong("wait_mp", 10, 0) == 1)
			wait_mast_processor_done();

		break;
		case 2:
		if (strcmp(env_get("ipaddr"), env_get("ipaddr2"))) {
			env_set("ipaddr", env_get("ipaddr2"));
			env_set("serverip", env_get("ipaddr0"));
			env_set("bootmode", "tftp_kernel");
			save_env = true;
		}
		if (env_get_ulong("wait_mp", 10, 0) == 1)
			wait_mast_processor_done();

		break;
		case 3:
		if (strcmp(env_get("ipaddr"), env_get("ipaddr3"))) {
			env_set("ipaddr", env_get("ipaddr3"));
			env_set("serverip", env_get("ipaddr0"));
			env_set("bootmode", "tftp_kernel");
			save_env = true;
		}
		if (env_get_ulong("wait_mp", 10, 0) == 1)
			wait_mast_processor_done();

		break;
		case 0:
			set_gpio_value(MP_TO_COP0_CTRL_PIN, MP_TO_COP0_CTRL_NAME, 0);
			set_gpio_value(MP_TO_COP1_CTRL_PIN, MP_TO_COP1_CTRL_NAME, 0);
			set_gpio_value(MP_TO_COP2_CTRL_PIN, MP_TO_COP2_CTRL_NAME, 0);

		default:
		break;
		}
	break;

	case TX536_EVB:
	case TX536_EVB_BOX:
	default:
		if (env_get_ulong("device_id", 10, 0) != 0)
			env_set_ulong("device_id", 0);

	break;
	}

	if (save_env)
		env_save();

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	get_device_id,	2,	1,	do_get_device_id,
	"exec get_device_id command",
	"judge board mode and get boot id value."
);
