TARGET := ts_es8516_es7243
FW_SRC := ./ts-es8516-es7243.c
FW_SRC_OBJ := $(FW_SRC:.c=.o)

obj-m += $(TARGET).o
$(TARGET)-objs += $(FW_SRC_OBJ)


default:
	make -C $(TARGET_KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(TARGET_KERNEL_DIR) M=$(PWD) clean
