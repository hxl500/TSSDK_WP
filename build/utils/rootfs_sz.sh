#!/bin/bash

rootfs_sz_fl=${TARGET_OUT_DIR}/rootfs_sz/rootfs_size.txt
fl_start_libs=""
fl_start_apps=""

function rootfs_show_init()
{
	rm -rf ${TARGET_OUT_DIR}/rootfs_sz
	mkdir -p ${TARGET_OUT_DIR}/rootfs_sz
	mkdir -p ${TARGET_OUT_DIR}/rootfs_sz/libs
	mkdir -p ${TARGET_OUT_DIR}/rootfs_sz/apps
	touch ${rootfs_sz_fl}
	date > ${rootfs_sz_fl}
}

function rootfs_show_sz()
{
	echo -e "\n\e[7m=====rootfs size:=====\e[0m" | tee -a $rootfs_sz_fl
	echo "$1:$(du -h -s ${TARGET_OUT_DIR}/rootfs | cut -f 1)" | tee -a $rootfs_sz_fl
	echo -e "=====================\n" | tee -a $rootfs_sz_fl
}


function rootfs_get_fl_libs()
{
	fl_start_libs=`find $TARGET_OUT_DIR/rootfs/ -type f`
}

function rootfs_set_new_fl_libs()
{
	local fl_end=""
	local fl_new=""
	local find=""
	local szfile=""

	if [ "x"$1 == "x" ]; then
		echo "no libs input folder when statisic rootfs size"
		return -1
	fi

	szfile=${TARGET_OUT_DIR}/rootfs_sz/libs/${1##*/}

	fl_end=`find $TARGET_OUT_DIR/rootfs/ -type f`

	for end in $fl_end
	do
		find="false"
		for start in $fl_start_libs
		do
			if [ "x"$end == "x"$start ] ; then
				find="true"
				break
			fi
		done

		if [ "x"$find == "x""false" ] ; then
			fl_new="$fl_new $end"
		fi
	done

	if [ "x""$fl_new" != "x" ] ; then
		echo $fl_new | sed "s/ /\n/g" > ${szfile}.tmp
	fi
}



function rootfs_get_fl_apps()
{
	fl_start_apps=`find $TARGET_OUT_DIR/rootfs/ -type f`
}

function rootfs_set_new_fl_apps()
{
	local fl_end=""
	local fl_new=""
	local find=""
	local szfile=""

	if [ "x"$1 == "x" ]; then
		echo "no apps input folder when statisic rootfs size"
		return -1
	fi

	szfile=${TARGET_OUT_DIR}/rootfs_sz/apps/${1##*/}

	fl_end=`find $TARGET_OUT_DIR/rootfs/ -type f`
	for end in $fl_end
	do
		find="false"
		for start in $fl_start_apps
		do
			if [ "x"$end == "x"$start ] ; then
				find="true"
				break
			fi
		done

		if [ "x"$find == "x""false" ] ; then
			fl_new="$fl_new $end"
		fi
	done

	if [ "x""$fl_new" != "x" ]; then
		echo $fl_new | sed "s/ /\n/g" > ${szfile}.tmp
	fi
}


function rootfs_show_new_fl_libs()
{
	file_list=`ls ${TARGET_OUT_DIR}/rootfs_sz/libs/`

	echo -e "\n====rootfs new file by libs=====" | tee -a $rootfs_sz_fl
	echo -e "====$(date)=====" | tee -a $rootfs_sz_fl

	for file in ${file_list}
	do
		long_name_tmp=${TARGET_OUT_DIR}/rootfs_sz/libs/$file
		long_name=${long_name_tmp%.tmp}
		cat ${long_name_tmp} | xargs du -k | sort -nrk 1 >> ${long_name}
		total_sz=$(awk 'BEGIN{sum=0} {sum+=$1} END{print sum}' ${long_name})
		echo -e "\n***${file%.tmp}(${total_sz}KB):***" | tee -a $rootfs_sz_fl
		cat ${long_name} | tee -a $rootfs_sz_fl
		rm ${long_name_tmp}
	done
}

function rootfs_show_new_fl_apps()
{
	file_list=`ls ${TARGET_OUT_DIR}/rootfs_sz/apps/`

	echo -e "\n====rootfs new file by apps:=====" | tee -a $rootfs_sz_fl
	echo -e "====$(date)=====" | tee -a $rootfs_sz_fl

	for file in ${file_list}
	do
		long_name_tmp=${TARGET_OUT_DIR}/rootfs_sz/apps/$file
		long_name=${long_name_tmp%.tmp}
		cat ${long_name_tmp} | xargs du -k | sort -nrk 1 >> ${long_name}
		total_sz=$(awk 'BEGIN{sum=0} {sum+=$1} END{print sum}' ${long_name})
		echo -e "\n***${file%.tmp}(${total_sz}KB):***" | tee -a $rootfs_sz_fl
		cat ${long_name}  | tee -a $rootfs_sz_fl
		rm ${long_name_tmp}
	done
}
