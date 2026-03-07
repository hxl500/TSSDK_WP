dtb-$(CONFIG_ARCH_TS) += ${TARGET_DTB}

ifneq ($(strip $(TARGET_DTB_2)),)
dtb-$(CONFIG_ARCH_TS) += ${TARGET_DTB_2}
endif

always		:= $(dtb-y)
subdir-y	:= $(dts-dirs)
clean-files	:= *.dtb
