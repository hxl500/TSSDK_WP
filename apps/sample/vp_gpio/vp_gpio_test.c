#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdint.h>
// #include "vp_gpio.h"

typedef enum {
	VP_GPIO_PORT_A = 0,
	VP_GPIO_PORT_B,
	VP_GPIO_PORT_MAX,
} vp_gpio_port_t;

#define VP_GPIO_MAX_LEN 32

#define VP_GPIO_SET_FUNC 1
#define VP_GPIO_GET_FUNC 2
typedef enum {
	VP_GPIO_FUNC_0 = 0x00, /* GPIO FUNC 0 */
	VP_GPIO_FUNC_1, /* GPIO FUNC 1 */
	VP_GPIO_FUNC_2, /* GPIO FUNC 2 */
	VP_GPIO_FUNC_3, /* GPIO FUNC 3 */
	VP_GPIO_FUNC_4, /* GPIO FUNC 4 */
	VP_GPIO_FUNC_5, /* GPIO FUNC 5 */
	VP_GPIO_FUNC_6, /* GPIO FUNC 6 */
	VP_GPIO_FUNC_7, /* GPIO FUNC 7 */
	VP_GPIO_FUNC_8, /* GPIO FUNC 8 */
	VP_GPIO_OUTPUT_LOW, /* 9, 低电平输出 */
	VP_GPIO_OUTPUT_HIG, /* 10, 高电平输出 */
	VP_GPIO_INPUT_PULL_HIZ, /* 11, 输入无拉 */
	VP_GPIO_INPUT_PULL_LOW, /* 12, 输入下拉 */
	VP_GPIO_INPUT_PULL_HIG, /* 13, 输入上拉 */
} vp_gpio_func_t;

#define VP_GPIO_SET_VALUE 3
#define VP_GPIO_GET_VALUE 4
typedef enum {
	VP_GPIO_LOW = 0, /* 低电平 */
	VP_GPIO_HIG, /* 高电平 */
} vp_gpio_value_t;

#define VP_GPIO_REG_IRQ 5
#define VP_GPIO_UNREG_IRQ 6
typedef enum {
	VP_GPIO_IRQ_NONE, /* 无中断 */
	VP_GPIO_IRQ_LO, /* 低电平触发中断 */
	VP_GPIO_IRQ_HI, /* 高电平触发中断 */
	VP_GPIO_IRQ_FE, /* 下降沿触发中断 */
	VP_GPIO_IRQ_RE, /* 上升沿触发中断 */
	VP_GPIO_IRQ_EDGE, /* 边沿沿触发中断 */
} vp_gpio_irq_t;

#define VP_GPIO_SET_DIRIVE_STRENGET 7
#define VP_GPIO_GET_DIRIVE_STRENGET 8
typedef enum {
	VP_GPIO_DS_UNKNOW = 0, /* 未知驱动能力 */
	VP_GPIO_DS_MIN, /* GPIO最小驱动能力 */
	VP_GPIO_DS_LOW, /* GPIO驱动能力低 */
	VP_GPIO_DS_MID, /* GPIO驱动能力中 */
	VP_GPIO_DS_MAX /* GPIO最大驱动能力 */
} vp_gpio_drv_level_t;

#define VP_GPIO_SET_PULL 9
#define VP_GPIO_GET_PULL 10
typedef enum {
	VP_GPIO_PULL_NONE = 0, /* 默认配置 */
	VP_GPIO_PULL_HIZ, /* GPIO禁用上拉和下拉 */
	VP_GPIO_PULL_DOWN, /* GPIO下拉 */
	VP_GPIO_PULL_UP /* GPIO上拉 */
} vp_gpio_pull_level_t;

#define VP_GPIO_REQ 11
#define VP_GPIO_UNREQ 12

/*
#define VP_GPIO_GET_IRQ 13
*/

typedef struct {
	uint8_t port;
	uint8_t pin;
	union {
		uint32_t irqs[VP_GPIO_PORT_MAX];
		vp_gpio_func_t func;
		vp_gpio_value_t value;
		vp_gpio_irq_t irq;
		vp_gpio_drv_level_t level;
		vp_gpio_pull_level_t pull;
	} data;
} vp_gpio_ioctl_t;

typedef struct {
	uint32_t irq_id; /* 中断ID */
	uint8_t req : 2;
	uint8_t pin : 6; /* GPIO脚位:0-31 */
	uint8_t port : 4; /* GPIO端口:PA,PB,PC,PD */
	uint8_t value : 4; /* 脚位高低电平 */
	uint8_t pull : 4; /* 上拉下拉使能状态 */
	uint8_t func : 4; /* 功能定义 */
	uint8_t irq : 4; /* 中断信息 */
	uint8_t level : 4; /* 驱动能力 */
} vp_gpio_config_t;

void usage(char *prog_name)
{
	printf("Usage: %s <port> <pin> <test_mode> [value]\n", prog_name);
    printf("port: GPIO port, 0 or 1\n");
    printf(" pin: GPIOA:0-32,GPIOB:0-9\n");
	printf("Test modes:\n");
	printf("  1                    Request GPIO\n");
	printf("  2 <func_value>       Set pinmux function, see vp_gpio_func_t\n");
	printf("  3 <pull_value>       Set pinmux pull-up/pull-down:\n");
	printf("                         2 = Pull down\n");
	printf("                         3 = Pull up\n");
	printf("  4 <level_value>      Set pinmux drive strength:\n");
	printf("                         2 = 2mA\n");
	printf("                         3 = 4mA\n");
	printf("                         4 = 8mA\n");
	printf("  5 <input_func> <driver>    GPIO Input test\n");
	printf("                         		input_func: \n");
	printf("                         			11 = VP_GPIO_INPUT_PULL_HIZ\n");
	printf("                         			12 = VP_GPIO_INPUT_PULL_LOW\n");
	printf("                         			13 = VP_GPIO_INPUT_PULL_HIG\n");
	printf("                         		driver(strength): \n");
	printf("                         			2 = 2mA\n");
	printf("                         			3 = 4mA\n");
	printf("                         			4 = 8mA\n");
	printf("  6 <out_func> <driver> <value>  GPIO Input test\n");
	printf("                         			output_func: \n");
	printf("                         				9 = VP_GPIO_OUTPUT_LOW\n");
	printf("                         				10 = VP_GPIO_OUTPUT_HIG\n");
	printf("                         			driver(strength): \n");
	printf("                         				2 = 2mA\n");
	printf("                         				3 = 4mA\n");
	printf("                         				4 = 8mA\n");
	printf("                         			value: \n");
	printf("                         				0 低电平\n");
	printf("                         				1 高电平\n");
	printf("Examples:\n");
	printf("  %s 0 20 1              # Request GPIOA_20.\n", prog_name);
	printf("  %s 0 20 2 8            # Set GPIOA_20 function 8\n", prog_name);
	printf("  %s 0 20 3 2            # Enable pull-down resistor\n", prog_name);
	printf("  %s 0 20 4 3            # Set drive strength to 4mA\n", prog_name);
}

int main(int argc, char *argv[])
{
	const char *device_path = "/dev/vp_gpio";
	int fd;
	vp_gpio_ioctl_t ioctl_data;

	if (argc < 3) {
		usage(argv[0]);
		return -1;
	}
	fd = open(device_path, O_RDWR);
	if (fd < 0) {
		perror("Failed to open device");
		return EXIT_FAILURE;
	}

	// 设置GPIO端口号和引脚号
	// ioctl_data.port = 0; // GPIOA
	// ioctl_data.pin = 1; // GPIOA1
    ioctl_data.port = atoi(argv[1]); // GPIOA
	ioctl_data.pin = atoi(argv[2]); // GPIOA1

	if (atoi(argv[3]) == 1) {
		// 测试1: 请求GPIO资源 需要设备树配置!
		printf("Testing VP_GPIO_REQ,need DTS support!\n");
		if (ioctl(fd, VP_GPIO_REQ, &ioctl_data) < 0) {
			perror("VP_GPIO_REQ failed");
		} else {
			printf("VP_GPIO_REQ success\n");
		}
		if (ioctl(fd, VP_GPIO_UNREQ, &ioctl_data) < 0) {
			perror("VP_GPIO_UNREQ failed");
		} else {
			printf("VP_GPIO_UNREQ success\n");
		}
	} else if (atoi(argv[3]) == 2) {
		// 测试2: 设置GPIO为FUNC 测试gpioa1 func 0 1 2通过
		printf("Testing VP_GPIO_SET_FUNC\n");
		ioctl_data.data.func = atoi(argv[4]);
		printf("input VP_GPIO_SET_FUNC %d\n", ioctl_data.data.func);
		if (ioctl(fd, VP_GPIO_SET_FUNC, &ioctl_data) < 0) {
			perror("VP_GPIO_SET_FUNC failed");
		} else {
			printf("VP_GPIO_SET_FUNC success\n");
		}
	} else if (atoi(argv[3]) == 3) {
		// 测试3: 设置GPIO上下拉
		printf("Testing VP_GPIO_SET_PULL\n");
		ioctl_data.data.pull = atoi(argv[4]); //2下拉 3上拉
		printf("input VP_GPIO_SET_PULL %d\n", ioctl_data.data.pull);
		if (ioctl(fd, VP_GPIO_SET_PULL, &ioctl_data) < 0) {
			perror("VP_GPIO_SET_PULL failed");
		} else {
			printf("VP_GPIO_SET_PULL success\n");
		}
	} else if (atoi(argv[3]) == 4) {
		// 测试4: 驱动强度 输入2 3 4 对应2 4 8ma
		printf("Testing VP_GPIO_SET_DIRIVE_STRENGET\n");
		ioctl_data.data.level = atoi(argv[4]); //
		printf("input VP_GPIO_SET_DIRIVE_STRENGET %d\n", ioctl_data.data.level);
		if (ioctl(fd, VP_GPIO_SET_DIRIVE_STRENGET, &ioctl_data) < 0) {
			perror("VP_GPIO_SET_DIRIVE_STRENGET failed");
		} else {
			printf("VP_GPIO_SET_DIRIVE_STRENGET success\n");
		}
	} else if (atoi(argv[3]) == 5) {
		// 测试5: 驱动强度 输入2 3 4 对应2 4 8ma
		printf("Testing gpio input mode \n");
		if (ioctl(fd, VP_GPIO_REQ, &ioctl_data) < 0) {
			perror("VP_GPIO_REQ failed");
		} else {
			printf("VP_GPIO_REQ success\n");
		}

		ioctl_data.data.func = atoi(argv[4]);
		if (ioctl(fd, VP_GPIO_SET_FUNC, &ioctl_data) < 0) {
			perror("VP_GPIO_SET_FUNC failed");
		} else {
			printf("VP_GPIO_SET_FUNC success\n");
		}

		ioctl_data.data.level = atoi(argv[5]); //
		printf("input VP_GPIO_SET_DIRIVE_STRENGET %d\n", ioctl_data.data.level);
		if (ioctl(fd, VP_GPIO_SET_DIRIVE_STRENGET, &ioctl_data) < 0) {
			perror("VP_GPIO_SET_DIRIVE_STRENGET failed");
		} else {
			printf("VP_GPIO_SET_DIRIVE_STRENGET success\n");
		}

		printf("input any key to get gpio input value\n");
		getchar();

		if (ioctl(fd, VP_GPIO_GET_VALUE, &ioctl_data) < 0)
			perror("VP_GPIO_GET_VALUE failed");
		else
    		printf("VP_GPIO_GET_VALUE success, val %d\n", ioctl_data.data.value);

		if (ioctl(fd, VP_GPIO_UNREQ, &ioctl_data) < 0)
			perror("VP_GPIO_UNREQ failed");
		else
			printf("VP_GPIO_UNREQ success\n");
	} else if (atoi(argv[3]) == 6) {
		// 测试5: 驱动强度 输入2 3 4 对应2 4 8ma
		printf("Testing gpio output mode \n");
		if (ioctl(fd, VP_GPIO_REQ, &ioctl_data) < 0) {
			perror("VP_GPIO_REQ failed");
		} else {
			printf("VP_GPIO_REQ success\n");
		}

		ioctl_data.data.func = atoi(argv[4]);
		if (ioctl(fd, VP_GPIO_SET_FUNC, &ioctl_data) < 0) {
			perror("VP_GPIO_SET_FUNC failed");
		} else {
			printf("VP_GPIO_SET_FUNC success\n");
		}

		ioctl_data.data.level = atoi(argv[5]); //
		printf("input VP_GPIO_SET_DIRIVE_STRENGET %d\n", ioctl_data.data.level);
		if (ioctl(fd, VP_GPIO_SET_DIRIVE_STRENGET, &ioctl_data) < 0) {
			perror("VP_GPIO_SET_DIRIVE_STRENGET failed");
		} else {
			printf("VP_GPIO_SET_DIRIVE_STRENGET success\n");
		}

		ioctl_data.data.value = atoi(argv[6]); //
		printf("input any key to out gpio value %d\n", ioctl_data.data.value);
		getchar();

		if (ioctl(fd, VP_GPIO_SET_VALUE, &ioctl_data) < 0)
			perror("VP_GPIO_GET_VALUE failed");
		else
    		printf("VP_GPIO_GET_VALUE success\n");

		if (ioctl(fd, VP_GPIO_UNREQ, &ioctl_data) < 0)
			perror("VP_GPIO_UNREQ failed");
		else
			printf("VP_GPIO_UNREQ success\n");
	}

	close(fd);
	return 0;
}