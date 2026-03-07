#!/bin/bash

source ${TARGET_BUILD_DIR}/scripts/common_set.sh

# the build.sh in sub-dir will be called
# build libs in the following order:
apps_build_seq="sample/"
export TARGET_APPS_DIR=$(pwd)

export MPP_USE_PCSIMULATOR=1;

function build_apps()
{
	local app_dir
	for app_dir in ${apps_build_seq}
	do
		cd ${TARGET_APPS_DIR}/${app_dir}
		local build_script="build.sh"
		if [ -f ${build_script} ] ; then
			echo -e -n "\n=====run ${TARGET_APPS_DIR}/${app_dir}"
			echo    "build.sh====="
			${TARGET_APPS_DIR}/${app_dir}build.sh $1
		else
			echo -e -n "\n?????${TARGET_APPS_DIR}/${app_dir}:"
			echo    "no build.sh?????"
		fi
	done
}

cp $TARGET_APPS_DIR/../libs/mpp/build/out/usr/lib/libmpi.so 	$TARGET_APPS_DIR/libs/x86/
cp $TARGET_APPS_DIR/../libs/mpp/build/out/lib/libdrm.so 	$TARGET_APPS_DIR/libs/x86/

echo "TARGET_APPS_DIR=$TARGET_APPS_DIR"
cd ${TARGET_APPS_DIR}
build_apps $1
cd ${TARGET_BUILD_DIR}

cp $TARGET_APPS_DIR/sample/out/usr/bin/sample_vio_x86 	$TARGET_APPS_DIR/libs/x86/
cp $TARGET_APPS_DIR/sample/out/usr/bin/sample_region_x86 	$TARGET_APPS_DIR/libs/x86/
cp $TARGET_APPS_DIR/sample/out/usr/bin/sample_venc_x86 	$TARGET_APPS_DIR/libs/x86/
cp -f $TARGET_APPS_DIR/sample/out/usr/bin/sample_venc_rtsp_x86 	$TARGET_APPS_DIR/libs/x86/
cp $TARGET_APPS_DIR/../libs/mpp/build/out/usr/bin/vipipe2gdc2vpe2vo_app $TARGET_APPS_DIR/libs/x86/
