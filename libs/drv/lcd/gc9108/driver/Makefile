KERNELDIR := $(TARGET_KERNEL_DIR)
CURRENT_PATH := $(shell pwd)

obj-m := gc9108.o

build: kernel_modules

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean