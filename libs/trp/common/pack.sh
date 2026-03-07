#!/bin/bash

# $1=src|lib libc_type=glibc(&&/|))uclibc
# do 2 things:
# 	1. cp the packaged file to dir lib/ module/ include/
#   2. gen new build.sh & del no need files
source ${TARGET_BUILD_DIR}/scripts/common_set.sh

function copy()
{
    if [ -d ./build/out ]; then
        if [ -d ./lib/${TARGET_LIBC} ]; then
            echo "./lib/${TARGET_LIBC} already exists"
        else
            mkdir -p ./lib/${TARGET_LIBC}
        fi

        if [ -d ./include ]; then
            echo "trp common include already exists"
	else
	    mkdir -p ./include
        fi

        cp build/out/usr/lib/* ./lib/${TARGET_LIBC} -aprf
        cp src/trp.h ./include/ -aprf
    else
        echo "please compile first!"
        false
    fi
}

function delete()
{
    rm -rf src build
}

function generate()
{
	build_file_new=./build.sh
	if [ -f $build_file_new ]; then
		rm $build_file_new
	fi
	touch $build_file_new
	cat << _EOF_ >> $build_file_new
#!/bin/bash
# \$1=all/clean
source \${TARGET_BUILD_DIR}/scripts/common_set.sh

function build_all()
{
	cp ./lib/\${TARGET_LIBC}/* \${TARGET_STAGING_LIB} -aprf
    cp ./include/* \${TARGET_STAGING_INC} -aprf
    echo "build done"
}

function clean()
{
	echo "clean"
}

if [ \$1 == "all" ] ; then
	build_all \$1
elif [ \$1 == "clean" ] ; then
	clean \$1
else
	echo "Not support, should be all/clean"
	false
fi
_EOF_

	chmod +x $build_file_new
}

echo "====trp common pack begin $1 $2 ${TARGET_LIBC}===="

if [ $1 == "lib" ]; then
    if [ $2 == "gen" ]; then
        echo "trp common copy lib"
        copy
    elif [ $2 == "clean" ]; then
        echo "trp common clean src"
        delete
        generate
    fi
elif [ $1 == "src" ]; then
    echo "not support src"
    if [ $2 == "gen" ]; then
        echo "not support src gen"
    elif [ $2 == "clean" ]; then
        echo "not support src clean"
    fi
fi
echo "====trp common pack done===="
