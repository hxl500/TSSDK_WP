TARGET := ts_es8388
FW_SRC := ./ts-es8388.c
FW_SRC_OBJ := $(FW_SRC:.c=.o)

obj-m += $(TARGET).o
$(TARGET)-objs += $(FW_SRC_OBJ)


default:
	make -C $(TARGET_KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(TARGET_KERNEL_DIR) M=$(PWD) clean
