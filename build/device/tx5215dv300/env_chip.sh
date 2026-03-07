#!/bin/bash

export CROSS_COMPILE=""
export TARGET_ARCH="arm"
export TARGET_CPU_TYPE="cortex-a7"
export TARGET_CHIPSERIES="TX5215BX"
export TARGET_CHIPNAME="tx5215dv300"
export TARGET_LIBC=""

function compiler_glibc()
{
	cd ${TARGET_PREBUILTS_DIR}/host/gcc

	if [ ! -d gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf ]
	then
		echo "=====uncompress \
gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf,please wait====="
		tar -xf \
gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf.tar.xz
	else
		echo "=====\
gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf exist====="
	fi

	export CROSS_COMPILE="${TARGET_PREBUILTS_DIR}/host/gcc/\
gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf/bin/\
arm-none-linux-gnueabihf-"
	echo CROSS_COMPILE:${CROSS_COMPILE}

	export TARGET_LIBC="glibc"
	cd ${TARGET_BUILD_DIR}
}

function compiler_uclibc()
{
	local compiler_dir="gcc-ts-10.3-2023.10-x86_64-arm-none-linux-uclibcgnueabihf"
	local compiler_name="gcc-ts-10.3-2023.10-x86_64-arm-none-linux-uclibcgnueabihf.tar.xz"

	cd ${TARGET_PREBUILTS_DIR}/host/gcc

	if [ ! -d ${compiler_dir} ]
	then
		echo "=====uncompress ${compiler_name},please wait====="
		tar -xf ${compiler_name}
	else
		echo "=====${compiler_dir}exist====="
	fi

	export CROSS_COMPILE="${TARGET_PREBUILTS_DIR}/host/gcc/${compiler_dir}/bin/\
arm-ts-linux-uclibcgnueabihf-"
	echo CROSS_COMPILE:${CROSS_COMPILE}

	export TARGET_LIBC="uclibc"
	cd ${TARGET_BUILD_DIR}
}
