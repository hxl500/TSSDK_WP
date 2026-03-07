#!/bin/bash

source ${TARGET_BUILD_DIR}/scripts/common_set.sh

function build_new_trp()
{
    local lib_dir
    for lib_dir in ./*
    do
        if [ -d "$lib_dir" ]; then
            cd ${lib_dir}
            local build_script="build.sh"
            if [ -f ${build_script} ] ; then
                echo -e -n "\n=====run ${lib_dir}/"
                echo "build.sh====="
                ./build.sh $1
            else
                echo -e -n "\nerror! can't find build.sh in ${lib_dir}\n"
            fi
	    cd -
        fi
    done
}

build_new_trp $1
