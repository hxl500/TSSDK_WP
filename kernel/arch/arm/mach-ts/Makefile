#
# Makefile for Hisilicon processors family
#

CFLAGS_platmcpm.o	:= -march=armv7-a

obj-y	+= board-dt.o
#obj-$(CONFIG_MCPM)		+= platmcpm.o
obj-$(CONFIG_SMP)		+= platsmp.o mach-ts.o