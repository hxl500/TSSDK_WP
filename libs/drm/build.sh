#!/bin/bash

source ${TARGET_BUILD_DIR}/scripts/common_set.sh

# $1=all|clean

# do two things:
# 	1: build
# 	2: cp the so and other need files to ${TARGET_OUT_DIR}/rootfs
function build_all()
{
	echo "================ setup env =================="
	source ./env_setup.sh
	echo "================ build drm =================="
	if [  "x""${TARGET_LIBS_TYPE}" == "x""static" ] ; then
		meson setup build --prefix=${prefix_path} -Ddefault_library=static --cross-file cross_file.txt
	else
		meson setup build --prefix=${prefix_path} --cross-file cross_file.txt
	fi
	cd build
	/bin/ninja
	/bin/ninja install
	cd ..
	echo "================ install drm =================="
	# TODO: cp files to ${TARGET_OUT_DIR}/rootfs/****
	#cp install/lib/libdrm.so* ${TARGET_OUT_DIR}/rootfs/usr/lib -ap
	#mkdir -p ${TARGET_OUT_DIR}/rootfs/usr/bin/drm/tests
	#cp build/tests/* ${TARGET_OUT_DIR}/rootfs/usr/bin/drm/tests -ap
	cp  install/include/* ${TARGET_STAGING_INC}/  -aprf
	if [  "x""${TARGET_LIBS_TYPE}" == "x""static" ] ; then
		cp install/lib/libdrm.a ${TARGET_STAGING_LIB}/ -ap
	else
		cp install/lib/libdrm.so* ${TARGET_STAGING_LIB}/ -ap
	fi
	#mkdir -p ${TARGET_STAGING_BIN}/drm/tests
	#cp build/tests/* ${TARGET_STAGING_BIN}/drm/tests -ap
	echo "================ build complete =================="
}

function clean()
{
	echo "================ clean drm =================="
	if [ -d intall ]; then
		rm -rf install
	fi
	if [ -d build ]; then
		rm -rf build
	fi
	echo "================ clean complete =================="
}

if [ $1 == "all" ] ; then
	build_all $1
else
	clean $1
fi
