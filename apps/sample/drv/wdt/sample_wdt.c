/*
* /mnt/d/work/code/tx511_mpw/prebuilts/host/gcc/gcc-arm-10.2-2020.11-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-gcc
* -static watchdog-simple.c -o watchdog-simple
*
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include <sys/signal.h>

//watchdog
#define WATCHDOG_IOCTL_BASE     'W'

struct watchdog_info {
	unsigned int options;          /* Options the card/driver supports */
	unsigned int firmware_version; /* Firmware version of the card */
	char identity[32];     /* Identity of the board */
};

#define WDIOC_GETSUPPORT        _IOR(WATCHDOG_IOCTL_BASE, 0, struct watchdog_info)
#define WDIOC_GETSTATUS         _IOR(WATCHDOG_IOCTL_BASE, 1, int)
#define WDIOC_GETBOOTSTATUS     _IOR(WATCHDOG_IOCTL_BASE, 2, int)
#define WDIOC_GETTEMP           _IOR(WATCHDOG_IOCTL_BASE, 3, int)
#define WDIOC_SETOPTIONS        _IOR(WATCHDOG_IOCTL_BASE, 4, int)
#define WDIOC_KEEPALIVE         _IOR(WATCHDOG_IOCTL_BASE, 5, int)
#define WDIOC_SETTIMEOUT        _IOWR(WATCHDOG_IOCTL_BASE, 6, int)
#define WDIOC_GETTIMEOUT        _IOR(WATCHDOG_IOCTL_BASE, 7, int)
#define WDIOC_SETPRETIMEOUT     _IOWR(WATCHDOG_IOCTL_BASE, 8, int)
#define WDIOC_GETPRETIMEOUT     _IOR(WATCHDOG_IOCTL_BASE, 9, int)
#define WDIOC_GETTIMELEFT       _IOR(WATCHDOG_IOCTL_BASE, 10, int)
#define	 WATCHDOG_ENABLE_MULTICORE	_IOR(WATCHDOG_IOCTL_BASE, 11, int)
#define WATCHDOG_DISABLE_MULTICORE	_IOR(WATCHDOG_IOCTL_BASE, 12, int)

#define WDIOF_OVERHEAT          0x0001  /* Reset due to CPU overheat */
#define WDIOF_FANFAULT          0x0002  /* Fan failed */
#define WDIOF_EXTERN1           0x0004  /* External relay 1 */
#define WDIOF_EXTERN2           0x0008  /* External relay 2 */
#define WDIOF_POWERUNDER        0x0010  /* Power bad/power fault */
#define WDIOF_CARDRESET         0x0020  /* Card previously reset the CPU */
#define WDIOF_POWEROVER         0x0040  /* Power over voltage */
#define WDIOF_SETTIMEOUT        0x0080  /* Set timeout (in seconds) */
#define WDIOF_MAGICCLOSE        0x0100  /* Supports magic close char */
#define WDIOF_PRETIMEOUT        0x0200  /* Pretimeout (in seconds), get/set */
#define WDIOF_KEEPALIVEPING     0x8000  /* Keep alive ping reply */

#define WDIOS_DISABLECARD       0x0001  /* Turn off the watchdog timer */
#define WDIOS_ENABLECARD        0x0002  /* Turn on the watchdog timer */
#define WDIOS_TEMPPANIC         0x0004  /* Kernel panic on temperature trip */

int wdt_fd;

void stop_signal()
{
	int val = 0 , ret = 0 ;

	val = WDIOS_DISABLECARD ;
	ret = ioctl(wdt_fd, WDIOC_SETOPTIONS, &val) ;
	if (ret < 0)
		printf("ioctl WDIOC_GETSUPPORT failed with %d.\n", ret);

	printf("===watchdow will be closed===\n") ;
	close(wdt_fd) ;
	exit(0);

}

void print_usage(const char *prog)
{
	printf("Usage: %s\n", prog);
	printf("  -p --interval   \n");
	printf("  -t --timeout    \n");
	printf("  -s --time       \n");
	printf("  -k --kthread 	  \n");

	exit(1);
}

int main(int argc, char *argv[])
{
	int ret = 0;
	static int count = 0;
	//int val;
	struct watchdog_info wdt_info;

	signal(SIGINT, stop_signal) ;

	unsigned int interval = 0;
	unsigned int timeout = 0;
	unsigned int time = -1;
	unsigned int kthread = -1;
	static const char short_options[] = "p:t:s:k:";
	static const struct option long_options[] = {
		{"interval", 1, 0, 'p'},
		{"timeout", 1, 0, 't'},
		{"time", 1, 0, 's'},
		{"kthread", 1, 0, 'k'},
		{NULL, 0, 0, 0},
	};

	int cmd_ret;

	while ((cmd_ret = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
		switch (cmd_ret) {
		case 'p':
			interval = atoi(optarg);
			break;
		case 't':
			timeout = atoi(optarg);
			break;
		case 's':
			time = atoi(optarg);
			break;
		case 'k':
			kthread = atoi(optarg);
			break;
		default:
			print_usage(argv[0]);
			break;
		}
	}
	printf("Interval: %d\n", interval);
	printf("Timeout: %d\n", timeout);
	printf("Time: %d\n", time);
	printf("kthread: %d\n", kthread);

	wdt_fd = open("/dev/watchdog0", O_RDWR);
	if(wdt_fd < 0)
	{
		printf("open /dev/watchdog0 failed.\n");
	}
#if 1
	/* get watchdog infomation struct */
	ret = ioctl(wdt_fd, WDIOC_GETSUPPORT, &wdt_info);
	if (ret < 0)
		printf("ioctl WDIOC_GETSUPPORT failed.\n");
	else
	{
		printf("options = 0x%x,id = %s\n", wdt_info.options, wdt_info.identity);
	}
#endif
	/* set watchdog timeout */
	ioctl(wdt_fd, WDIOC_SETTIMEOUT, &timeout);
	if (ret < 0)
		printf("ioctl WDIOC_SETTIMEOUT failed.\n");

	/* enable kthread */
	ret = ioctl(wdt_fd, WATCHDOG_ENABLE_MULTICORE);

	while(1) {
		/* feed watchdog */
		ioctl(wdt_fd, WDIOC_KEEPALIVE, NULL);
		printf("food watchdog, count = %d \n",count);

		if (kthread != -1) {
			if (count == 10) {
				ioctl(wdt_fd, WATCHDOG_DISABLE_MULTICORE, &kthread);
				printf("disable kthread:%d\n", kthread);
			}
		}

		if (time != -1) {
			if (time <= count * interval) {
				printf("stop feed watchdog\n");
				break;
			}
		}
		count++;
		sleep(interval);
	}

	close(wdt_fd);
	return 0;
}
