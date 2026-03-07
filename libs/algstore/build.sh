#!/bin/bash
# $1=all/clean
source ${TARGET_BUILD_DIR}/scripts/common_set.sh

function build_all()
{
    local lib_dir
    for lib_dir in ./*
    do
        if [ -d "${lib_dir}" ]; then
            cd ${lib_dir}
            if [ -d "./lib" ]; then
                if [ -d "./lib/${TARGET_LIBC}" ]; then
                    if [ -z "$(ls ./lib/${TARGET_LIBC})" ]; then
                        echo "no file to copy"
                    else
                        mkdir -p ${TARGET_STAGING_LIB}/algstore
                        cp ./lib/${TARGET_LIBC}/* ${TARGET_STAGING_LIB}/algstore -aprf
                    fi
                else
                    echo "this ${lib_dir} has no ${TARGET_LIBC} dir"
                fi
            else
                echo "this dir ${lib_dir} no lib dir"
            fi

            if [ -d "./include" ]; then
                mkdir -p ${TARGET_STAGING_INC}/algstore
                cp ./include/* ${TARGET_STAGING_INC}/algstore -aprf
            else
                echo "this dir ${lib_dir} no include dir"
            fi

            if [ -d ./model/v* ]; then
                cp ./model/v*/*.cfg ${TARGET_STAGING_INC} -aprf
                cp ./model/v*/*.weight ${TARGET_STAGING_INC} -aprf
                cp ./model/v*/*.h ${TARGET_STAGING_INC} -aprf
            elif [ -d ./model ]; then
                find ./model/ -maxdepth 1 \( -name *.cfg -o -name *.h -o -name *.weight \) -exec cp {} ${TARGET_STAGING_INC}/ -aprf \;
                if [ $(basename ${lib_dir}) == "libalgaiisp" ]; then
                    aiisp_model_dir=${TARGET_STAGING_INC}/nfr
                elif [ $(basename ${lib_dir}) == "libalgaiisp_rfr" ]; then
                    aiisp_model_dir=${TARGET_STAGING_INC}/rfr
                fi
                if [ "${aiisp_model_dir}" != "" ]; then
                    mkdir -p ${aiisp_model_dir}
                    for modeln in ./model/model[1-9]
                    do
                        if [ -d ${modeln} ]; then
                            cp ${modeln} ${aiisp_model_dir}/ -aprf
                        fi
                    done
                fi
            else
                echo "this dir ${lib_dir} no model dir"
            fi
            cd -
        fi
    done
	echo "build done"
}

function clean()
{
	echo "clean"
}

if [ $1 == "all" ] ; then
	build_all $1
elif [ $1 == "clean" ] ; then
	clean $1
else
	echo "Not support, should be all/clean"
	false
fi
