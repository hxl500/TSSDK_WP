#!/bin/bash
# info    : =====
# warning : ?????
# error   : *****

# help:
#     in build:
#         source envsetup.sh
#         lunch xxx
#         source tools/ko_all_kern_chk/ko_chk.sh
#     in drv folder(generagte result in ./tmp)
#         ko_chk
#
#echo -e "=============================================================="
#echo -e "=====run command ko_chk in ko file folder====="
#echo -e "=============================================================="


kernel_sym=${TARGET_KERNEL_DIR}/Module.symvers
kernel_ts_api="./tmp/kernel_ts_api"
kernel_need_wrap_api="./tmp/kernel_need_wrap_api"
result_api="./tmp/result_api"
result_struct="./tmp/result_struct"
result_not_wrap_ex_api="./tmp/result_not_wrap_ex_api"

struct_list="mutex spinlock video_device wait_queue_head kthread_worker semaphore vb2_queue v4l2_device cdev"
struct_t_list="spinlock_t wait_queue_head_t"
dangerous_words="pdev->dev.of_node \->of_node"

function spec_macro_2_file()
{
	echo -e "ts_wait_event_interruptible" >> $kernel_ts_api
	echo -e "ts_wait_event_timeout" >> $kernel_ts_api
	echo -e "ts_wait_event_interruptible_timeout" >> $kernel_ts_api
	echo -e "ts_wait_event_interruptible_hrtimeout" >> $kernel_ts_api

	echo -e "wait_event_interruptible" >> $kernel_need_wrap_api
	echo -e "wait_event_timeout" >> $kernel_need_wrap_api
	echo -e "wait_event_interruptible_timeout" >> $kernel_need_wrap_api
	echo -e "wait_event_interruptible_hrtimeout" >> $kernel_need_wrap_api
}

function ko_chk()
{
	local ko_file_list
	local need_wrap
	local warn_stuct
	local not_wrap_api
	local not_wrap_api_need_mod

	# clean tmp file
	mkdir -p ./tmp
	cut -f 2  $kernel_sym  | grep "^ts_"  > $kernel_ts_api
	sed "s/^ts_//" "./tmp/kernel_ts_api" > $kernel_need_wrap_api
	echo -e "These api may need be wrapped:\n\n" > $result_api
	echo -e "These struct should not be used:" > $result_struct
	echo -e "These api not wrap, please check:" > $result_not_wrap_ex_api

	spec_macro_2_file

	ko_file_list=$(find . -type f \( -name "*.c" -o -name "*.h" \) -not -name "*.mod.c")
	echo ko_file_list:$ko_file_list

	while IFS= read -r line
	do
		need_wrap=$(grep -n "${line}" $ko_file_list | grep -v "ts_${line}" | grep -v "_${line}")
		if [ "x""$need_wrap" != "x" ] ; then
			echo -e "\n==============================================================" >> $result_api
			echo -e "=====api: $line====="						 >> $result_api
			echo -e "==============================================================" >> $result_api
			echo -e "$need_wrap" >> $result_api
		fi
	done < $kernel_need_wrap_api


	# current
	warn_stuct=$(grep -n "current\->" $ko_file_list)
	if [ "x""$warn_stuct" != "x" ] ; then
		echo -e "\n==============================================================" >> $result_struct
		echo -e "=====struct: current====="					 >> $result_struct
		echo -e "==============================================================" >> $result_struct
		echo -e "$warn_stuct" >> $result_struct
	fi


	for struct in $struct_list 
	do
		warn_stuct=$(grep -n "struct.*${struct}" $ko_file_list | grep -v "struct.*${struct}.*\*")
		if [ "x""$warn_stuct" != "x" ] ; then
			echo -e "\n==============================================================" >> $result_struct
			echo -e "=====struct: ${struct}, use pointer====="			 >> $result_struct
			echo -e "==============================================================" >> $result_struct
			echo -e "$warn_stuct" >> $result_struct
		fi
	done

	for struct in $struct_list
	do
		warn_stuct=$(grep -n "sizeof(struct ${struct})" $ko_file_list)
		if [ "x""$warn_stuct" != "x" ] ; then
			echo -e "\n==============================================================" >> $result_struct
			echo -e "=====struct: sizeof(${struct}), do not use in this way====="	 >> $result_struct
			echo -e "==============================================================" >> $result_struct
			echo -e "$warn_stuct" >> $result_struct
		fi
	done

	for struct_t in $struct_t_list 
	do
		warn_stuct=$(grep -n "${struct_t}" $ko_file_list | grep -v "${struct_t}.*\*")
		if [ "x""$warn_stuct" != "x" ] ; then
			echo -e "\n==============================================================" >> $result_struct
			echo -e "=====struct_t: ${struct_t}, use pointer====="			 >> $result_struct
			echo -e "==============================================================" >> $result_struct
			echo -e "$warn_stuct" >> $result_struct
		fi
	done

	for struct_t in $struct_t_list
	do
		warn_stuct=$(grep -n "sizeof(${struct_t})" $ko_file_list)
		if [ "x""$warn_stuct" != "x" ] ; then
			echo -e "\n==============================================================" >> $result_struct
			echo -e "=====struct_t: sizeof(${struct_t}), do not use in this way=====" >> $result_struct
			echo -e "==============================================================" >> $result_struct
			echo -e "$warn_stuct" >> $result_struct
		fi
	done

	for dangerous_word in $dangerous_words 
	do
		warn_stuct=$(grep -n "${dangerous_word}" $ko_file_list)
		if [ "x""$warn_stuct" != "x" ] ; then
			echo -e "\n==============================================================" >> $result_struct
			echo -e "=====dangerous_word: ${dangerous_word}, need check====="			 >> $result_struct
			echo -e "==============================================================" >> $result_struct
			echo -e "$warn_stuct" >> $result_struct
		fi
	done

	# list not wrap api, some hidden api like __dynamic_pr_debug should not be used
	not_wrap_api=$(find -name *.ko -exec readelf -a {} \; | grep UND | grep -v "ts_")
	if [ "x""$not_wrap_api" != "x" ] ; then
		echo -e "\n==============================================================" >> $result_not_wrap_ex_api
		echo -e "=====not wrap api from kernel or other ko, be careful====="	>> $result_not_wrap_ex_api
		echo -e "==============================================================" >> $result_not_wrap_ex_api
		echo -e "$not_wrap_api" >> $result_not_wrap_ex_api
	fi
	not_wrap_api_need_mod=$(grep -n "__dynamic_pr_debug" $result_not_wrap_ex_api)
	if [ "x""$not_wrap_api_need_mod" != "x" ] ; then
		echo -e "\n============================================================" >> $result_not_wrap_ex_api
		echo -e "=====do not use  __dynamic_pr_debug                     =====" >> $result_not_wrap_ex_api
		echo -e "=====need #include <soc/ts/wrap/debug.h> at top of c file====" >> $result_not_wrap_ex_api
		echo -e "=============================================================" >> $result_not_wrap_ex_api
	fi
}

if [ "x"${TARGET_KERNEL_DIR} == "x" ] ; then
	echo -e "**************************************************************"
	echo -e "******please lunch first******"
	echo -e "**************************************************************"
fi



