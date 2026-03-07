#!/bin/bash
# all of sub fold will be build
# the build.sh in sub-dir will be called
source ${TARGET_BUILD_DIR}/scripts/common_set.sh

LIBS_SUB_DIRS=("algstore" "drm" "drv" "vpu" "gpu" "gdc" "isp" "jpeg" "live555" "mpp" "rtsp-server-gb" "trp" "tscv" "ffmpeg")

function build_libs()
{
    local lib_dir
    for lib_dir in "${LIBS_SUB_DIRS[@]}"
    do
        lib_dir=${TARGET_LIBS_DIR}/$lib_dir
        if [ -d "$lib_dir" ]; then
            cd ${lib_dir}
            local build_script="build.sh"
            if [ -f ${build_script} ] ; then
                echo -e -n "\n=====run ${lib_dir}/"
                echo "build.sh====="
                ${lib_dir}/build.sh $1
            else
                echo -e -n "\n?????${lib_dir}:"
                echo    "no build.sh?????"
            fi
        fi
    done
}


cd ${TARGET_LIBS_DIR}
build_libs $1
cd ${TARGET_BUILD_DIR}

