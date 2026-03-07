#!/bin/bash
cp ../../kernel/arch/arm64/boot/Image .
cp ../../kernel/arch/arm64/boot/dts/tsingmicro/ts-tx536-fpga.dtb .
../tools/mkimage -f ts-tx536.its ts-tx536.itb
