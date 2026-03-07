TARGET := ts_extern_spk
FW_SRC := ./es8156.c
FW_SRC_OBJ := $(FW_SRC:.c=.o)

obj-m += $(TARGET).o
$(TARGET)-objs += $(FW_SRC_OBJ)


default:
	make -C $(TARGET_KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(TARGET_KERNEL_DIR) M=$(PWD) clean
