#!/bin/bash

# the build.sh in sub-dir will be called
# build libs in the following order:

source ${TARGET_BUILD_DIR}/scripts/common_set.sh

#TARGET_STAGING_BIN=${TARGET_OUT_DIR}/staging/usr/bin
#TARGET_APPS_DIR=${TARGET_TOP_DIR}/apps

apps_build_seq="sample/"

#TARGET_APPS_DIR_TSCV=${TARGET_TOP_DIR}/apps
apps_build_seq_tscv="sample/"

TARGET_APPS_SAMPLE_DIR=""

export MPP_USE_PCSIMULATOR=0;
export ENABLE_MPP_TRACE=0;

function build_apps()
{
	local app_dir

	TARGET_APPS_SAMPLE_DIR=${TARGET_APPS_DIR}/${apps_build_seq}out/usr/bin
	echo "build dir:  ${apps_build_seq}"
	echo "sample bin output dir:  ${TARGET_APPS_SAMPLE_DIR}"

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

	if [ "$1" != "clean" ] ; then
		cp ${TARGET_APPS_SAMPLE_DIR}/* ${TARGET_STAGING_BIN} -aprf
	fi
}


cd ${TARGET_APPS_DIR}
build_apps $1
cd ${TARGET_BUILD_DIR}

