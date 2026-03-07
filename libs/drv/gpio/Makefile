CROSS_COMPILE?=arm-ts-linux-uclibcgnueabihf-
CC:=$(CROSS_COMPILE)gcc

KERNEL_TARGET := vp_gpio_drv

FW_SRC := ./vp_gpio.c
FW_SRC_OBJ := $(FW_SRC:.c=.o)

obj-m += $(KERNEL_TARGET).o
$(KERNEL_TARGET)-objs += $(FW_SRC_OBJ)

# 添加 TX5215DV300_GPIO 宏
VP_CFLAGS += -DTX5215DV300_GPIO

all: $(KERNEL_TARGET)

$(KERNEL_TARGET):
	@echo "Building kernel module $@"
	$(MAKE) -C $(TARGET_KERNEL_DIR) M=$(PWD) modules EXTRA_CFLAGS="$(VP_CFLAGS)"

clean:
	$(MAKE) -C $(TARGET_KERNEL_DIR) M=$(PWD) clean

.PHONY: all clean
