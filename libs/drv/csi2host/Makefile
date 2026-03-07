TARGET := mipi-rx
FW_SRC := ./csi2_host.c
FW_SRC_OBJ := $(FW_SRC:.c=.o)

obj-m += $(TARGET).o
$(TARGET)-objs += $(FW_SRC_OBJ)
KBUILD_EXTMOD = ${PWD}/../sys

default:
	make -C $(TARGET_KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(TARGET_KERNEL_DIR) M=$(PWD) clean
