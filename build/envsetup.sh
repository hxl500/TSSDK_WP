#!/bin/bash

# these variable will be export for all build scripts
# top
export TARGET_TOP_DIR=""
export TARGET_BUILD_DIR=""
export TARGET_OUT_DIR=""
export TARGET_UBOOT_DIR=""
export TARGET_KERNEL_DIR=""
export TARGET_PREBUILTS_DIR=""
export TARGET_LIBS_DIR=""
export TARGET_APPS_DIR=""
export TARGET_FAST_APP=""
export TARGET_FAST_APP_PARA=""

export TARGET_STAGING_INC=""
export TARGET_STAGING_LIB=""
export TARGET_STAGING_MODULE=""
export TARGET_STAGING_BIN=""

export TARGET_KERNEL_DRV_EXPORT=""
export TARGET_KERNEL_TYPE=""
export TARGET_LIBS_TYPE=""
export TARGET_MPP_PRELOAD=""
export TARGET_SDK_BOOT_MODE=""
export TARGET_KERNEL_SYSFS=""
export TARGET_FS_MOUNT_MODE=""

export TARGET_ROOTFS_TYPE=""
export TARGET_SYSTEM_PARTITION=""
export TARGET_KERNEL_RELEASE=""
export TARGET_NET_TYPE=""
export TARGET_SDK_ISP_DRV_PACK=""
export TARGET_DYNAMIC_DEV_FILE=""
export TARGET_SYSTEM_PART_PATH=""
export TARGET_INSTALL_MOD_PATH=""
export TARGET_TEST_MODE=""


export TARGET_VENTOR=""
export TARGET_CHIP=""
export TARGET_BOARD=""
export TARGET_VER=""
export TARGET_MODE=""		# debug | release
export TARGET_BITS=""
export DEBUG=""			# y | n

# chip
export TARGET_ARCH=""
export TARGET_CPU_TYPE=""

# board
export CROSS_COMPILE=""		# include path,
export TARGET_UBOOT_DEFCONFIG=""
export TARGET_KERNEL_DEFCONFIG=""
export TARGET_DTB=""
export TARGET_FIT_ITS=""
export TARGET_FIT_ITB=""

export TARGET_LOADER_2=""
export TARGET_LOADER_2_NAND=""
export TARGET_LOADER_3=""

export TARGET_LIBC=""

export TARGET_ROOTFS_CUSTOM=""

# misc
export CHK_APPS=""
export CHK_LIBS=""
export CHK_DRV=""
export LUNCH_SEL_NUM=""
export LUNCH_SEL=""

function print_env_dir()
{
	echo "========================================="
	echo "TARGET_TOP_DIR=${TARGET_TOP_DIR}"
	echo "TARGET_BUILD_DIR=${TARGET_BUILD_DIR}"
	echo "TARGET_OUT_DIR=${TARGET_OUT_DIR}"
	echo "TARGET_UBOOT_DIR=${TARGET_UBOOT_DIR}"
	echo "TARGET_KERNEL_DIR=${TARGET_KERNEL_DIR}"
	echo "TARGET_PREBUILTS_DIR=${TARGET_PREBUILTS_DIR}"
	echo "TARGET_LIBS_DIR=${TARGET_LIBS_DIR}"
	echo "TARGET_APPS_DIR=${TARGET_APPS_DIR}"
	echo "TARGET_STAGING_INC=${TARGET_STAGING_INC}"
	echo "TARGET_STAGING_LIB=${TARGET_STAGING_LIB}"
	echo "TARGET_STAGING_MODULE=${TARGET_STAGING_MODULE}"
	echo "TARGET_STAGING_BIN=${TARGET_STAGING_BIN}"

	# echo "CHK_APPS=${CHK_APPS}"
	# echo "CHK_LIBS=${CHK_LIBS}"
	# echo "CHK_DRV=${CHK_DRV}"
	echo "========================================="
}

function print_env_lunch()
{
	echo "========================================="
	echo "TARGET_VENTOR=${TARGET_VENTOR}"
	echo "TARGET_CHIPSERIES=${TARGET_CHIPSERIES}" #| tr '[:lower:]' '[:upper:]'
	echo "TARGET_CHIPNAME=${TARGET_CHIPNAME}"
	echo "TARGET_CHIP=${TARGET_CHIP}"
	echo "TARGET_BOARD=${TARGET_BOARD}"
	echo "TARGET_VER=${TARGET_VER}"
	echo "TARGET_MODE=${TARGET_MODE}"
	echo "TARGET_BITS=${TARGET_BITS}"
	echo "DEBUG=${DEBUG}"
	echo ""
	echo "TARGET_ARCH=${TARGET_ARCH}"
	echo "TARGET_CPU_TYPE=${TARGET_CPU_TYPE}"
	echo ""
	echo "CROSS_COMPILE=${CROSS_COMPILE}"
	echo "TARGET_UBOOT_DEFCONFIG=${TARGET_UBOOT_DEFCONFIG}"
	echo "TARGET_KERNEL_DEFCONFIG=${TARGET_KERNEL_DEFCONFIG}"
	echo "TARGET_DTB=${TARGET_DTB}"
	echo "TARGET_FIT_ITS=${TARGET_FIT_ITS}"
	echo "TARGET_FIT_ITB=${TARGET_FIT_ITB}"
	echo "TARGET_LOADER_2=${TARGET_LOADER_2}"
	echo "TARGET_LOADER_2_NAND=${TARGET_LOADER_2_NAND}"
	echo "TARGET_LOADER_3=${TARGET_LOADER_3}"
	echo "TARGET_LIBC=${TARGET_LIBC}"
	echo "TARGET_ROOTFS_CUSTOM=${TARGET_ROOTFS_CUSTOM}"
	echo "LUNCH_SEL=${LUNCH_SEL}"
	# common env import from fastboot
	echo "TARGET_IMAGE_TYPE=${TARGET_IMAGE_TYPE}"
	echo "TARGET_LIBS_TYPE=${TARGET_LIBS_TYPE}"
	echo "TARGET_MPP_PRELOAD=${TARGET_MPP_PRELOAD}"
	echo "TARGET_SDK_ISP_DRV_PACK=${TARGET_SDK_ISP_DRV_PACK}"
	echo "TARGET_ROOTFS_TYPE=${TARGET_ROOTFS_TYPE}"
	echo "TARGET_KERNEL_TYPE=${TARGET_KERNEL_TYPE}"
	echo "TARGET_KERNEL_DRV_EXPORT=${TARGET_KERNEL_DRV_EXPORT}"
	echo "TARGET_KERNEL_SYSFS=${TARGET_KERNEL_SYSFS}"
	echo "TARGET_FS_MOUNT_MODE=${TARGET_FS_MOUNT_MODE}"
	echo "TARGET_INSTALL_MOD_PATH=${TARGET_INSTALL_MOD_PATH}"
	# fastboot private env
	echo "==============BOOT MODE=================="
	echo "TARGET_SDK_BOOT_MODE=${TARGET_SDK_BOOT_MODE}"
	echo "TARGET_KERNEL_RELEASE=${TARGET_KERNEL_RELEASE}"
	echo "TARGET_NET_TYPE=${TARGET_NET_TYPE}"
	echo "TARGET_FAST_APP=${TARGET_FAST_APP}"
	echo "TARGET_FAST_APP_PARA=${TARGET_FAST_APP_PARA}"
	echo "TARGET_TEST_MODE=${TARGET_TEST_MODE}"
	echo "TARGET_DYNAMIC_DEV_FILE=${TARGET_DYNAMIC_DEV_FILE}"
	echo "TARGET_SYSTEM_PARTITION=${TARGET_SYSTEM_PARTITION}"
	echo "TARGET_SYSTEM_PART_PATH=${TARGET_SYSTEM_PART_PATH}"
	echo "========================================="
}

function print_lunch_menu()
{
	local i=1
	local choice
	echo -e "\nchoose valid configuration number for lunch:"
	for choice in ${LUNCH_MENU_CHOICES[@]}
	do
		echo "	$choice"
	done
}

unset LUNCH_MENU_CHOICES
function add_lunch_combo()
{
	local new_combo=$1
	local combo_num
	local c

	combo_num=$(echo "${new_combo}" | grep -o "^[0-9]*\.")
	combo_num=${combo_num%*\.}
	if [ "x"${combo_num} = "x" ] ; then
		echo "*****add lunch err, combo_num err*****"
		echo "*****err combo: $1*****"
		return
	fi
	for c in ${LUNCH_MENU_CHOICES[@]} ; do
		if [ "$new_combo" = "$c" ] ; then
			return
		fi
	done

	if [ "x"${LUNCH_MENU_CHOICES[${combo_num}]} != "x" ] ; then
		echo "*****add lunch err, ${combo_num} exit*****"
		return
	fi
	LUNCH_MENU_CHOICES[${combo_num}]=$new_combo
}

function lunch()
{
	cd ${TARGET_BUILD_DIR}

	local answer
	local selection
	local valid_lunch
	local valid_num

	if [ "$1" ] ; then
		answer=$1
	else
		print_lunch_menu
		echo -n "Which would you like?  "
		read answer
	fi

	if [ "$1" = "?" ]; then
		for valid_lunch in ${LUNCH_MENU_CHOICES[@]} ; do
		valid_num=$(echo "${valid_lunch}" | grep -o "^[0-9]*\.")
		valid_num=${valid_num%.}
		if [ "$valid_num" = "$LUNCH_SEL_NUM" ];then
			selection=$valid_lunch
			echo "selected:${selection}"
		fi
		done
		return 1;
	fi

	if [ -z "$answer" ]
	then
		echo "*****Invalid lunch combo1: $answer*****"
		return 1
	elif (echo -n $answer | grep -q -e "^[0-9]*$")
	then
		for valid_lunch in ${LUNCH_MENU_CHOICES[@]} ; do
			valid_num=$(echo "${valid_lunch}" | grep -o "^[0-9]*\.")
			valid_num=${valid_num%.}
			if [ "$valid_num" = "$answer" ];then
				selection=$valid_lunch
				echo "selection:${selection}"
				LUNCH_SEL_NUM=$valid_num
				LUNCH_SEL=$valid_lunch
			fi
		done
	fi

	if [ -z "$selection" ]
	then
		echo "*****Invalid lunch combo2: $selection*****"
		return 1
	fi

	selection=${selection#*\.}
	echo "selection:${selection}"
	local vendor=$(echo $selection | cut -f 1 -d "_")
	local chip=$(echo $selection | cut -f 2 -d "_")
	local board=$(echo $selection | cut -f 3 -d "_")
	local ver=$(echo $selection | cut -f 4 -d "_")
	local mode=$(echo $selection | cut -f 5 -d "_")
	local bit=$(echo $selection | cut -f 6 -d "_")

	if [ "$selection" != "${vendor}_${chip}_${board}_${ver}_${mode}_${bit}" ];then
		echo "*****lunch combo error*****"
		echo "	format: vendor_chip_board_[debug|release]_[32|64]"
		echo "Ex: ts_tx511_haps_debug_64"
		return 1
	else
		echo "lunch:${selection}"
	fi

	TARGET_VENTOR=${vendor}
	TARGET_CHIP=${chip}
	TARGET_BOARD=${board}
	TARGET_VER=${ver}
	TARGET_MODE=${mode}
	TARGET_BITS=${bit}

	# source chip env
	local chip_env=${TARGET_BUILD_DIR}/device/${chip}/env_chip.sh
	if [ -f ${chip_env} ] ; then
		echo "source ${chip_env}"
		source ${chip_env}
	else
		echo "?????chip env config file not exist: ${chip_env}.?????"
	fi

	# source board env
	local board_env=${TARGET_BUILD_DIR}/device/${chip}/board/
	board_env=${board_env}env_${board}_${ver}_${mode}_${bit}.sh
	echo $board_env
	if [ -f ${board_env} ] ; then
		echo "source ${board_env}"
		source ${board_env}
	else
		echo "?????board env config file not exist: ${board_env}.?????"
	fi

	export DEBUG=n
	if [ "$mode" = "debug" ];then
		echo "Debug Mode"
		export DEBUG=y
	elif [ "$mode" = "release" ];then
		echo "Release Mode"
	else
		echo "*****Unknown Mode, not debug, release and fpga*****"
		return 1
	fi

	if [ "$bit" != "32" -a "$bit" != "64" ];then
		echo "*****Unknown ${bit}-bit, should be 32 or 64*****"
	else
		echo "${bit}-bit"
	fi
	if [ "$board" = "ptzspinand" ];then
		board="ptz"
	fi


	print_env_lunch
}

function check_bash()
{
	if [ "x$SHELL" != "x/bin/bash" ]; then
		case `ps -o command -p $$` in
			*bash*)
				;;
			*)
				echo "?????WARNING: Only bash is supported?????"
				;;
		esac
	fi
}

# find ./device/${chip}/add_lunch_combo.sh, add lunch option
function find_chip_lunch()
{
	cd ${TARGET_BUILD_DIR}/device

	local chip_fold
	for chip_fold in $(ls)
	do
		local add_lunch=${chip_fold}/add_lunch_combo.sh
		if [ -f ${add_lunch} ] ; then
			echo "source ${add_lunch}"
			source ${add_lunch}
		fi
	done

	cd ${TARGET_BUILD_DIR}
}

function cp_git_pre_commit()
{
	cd ${TARGET_BUILD_DIR}

	local git_hook=.git/hooks
	local pre_commit_drv=utils/pre-commit-drv
	local pre_commit_uboot=utils/pre-commit-uboot
	local pre_commit_libs=utils/pre-commit-libs
	# local pre_commit_apps=utils/pre-commit-apps


	cp -f ${pre_commit_drv} ${TARGET_BUILD_DIR}/${git_hook}/pre-commit
	rm -rf ${TARGET_BUILD_DIR}/${git_hook}/commit-msg
	rm -rf ${TARGET_BUILD_DIR}/${git_hook}/prepare-commit-msg
	chmod +x ${TARGET_BUILD_DIR}/${git_hook}/pre-commit


	cp -f ${pre_commit_uboot} ${TARGET_UBOOT_DIR}/${git_hook}/pre-commit
	rm -rf ${TARGET_UBOOT_DIR}/${git_hook}/commit-msg
	rm -rf ${TARGET_UBOOT_DIR}/${git_hook}/prepare-commit-msg
	chmod +x ${TARGET_UBOOT_DIR}/${git_hook}/pre-commit


	cp -f ${pre_commit_drv} ${TARGET_KERNEL_DIR}/${git_hook}/pre-commit
	rm -rf ${TARGET_KERNEL_DIR}/${git_hook}/commit-msg
	rm -rf ${TARGET_KERNEL_DIR}/${git_hook}/prepare-commit-msg
	chmod +x ${TARGET_KERNEL_DIR}/${git_hook}/pre-commit


	cp -f ${pre_commit_libs} ${TARGET_LIBS_DIR}/${git_hook}/pre-commit
	rm -rf ${TARGET_LIBS_DIR}/${git_hook}/commit-msg
	rm -rf  ${TARGET_LIBS_DIR}/${git_hook}/prepare-commit-msg
	chmod +x ${TARGET_LIBS_DIR}/${git_hook}/pre-commit


	# cp -f ${pre_commit_apps} ${TARGET_APPS_DIR}/${git_hook}/pre-commit
	# rm -rf ${TARGET_APPS_DIR}/${git_hook}/commit-msg
	# rm -rf ${TARGET_APPS_DIR}/${git_hook}/prepare-commit-msg
	# chmod +x ${TARGET_APPS_DIR}/${git_hook}/pre-commit

	cd ${TARGET_BUILD_DIR}

	checkpatch=${TARGET_KERNEL_DIR}/scripts/checkpatch.pl
	CHK_APPS="${checkpatch} -f --ignore NEW_TYPEDEFS \
		--ignore CONSTANT_COMPARISON \
		--ignore NOT_UNIFIED_DIFF \
		--ignore COMPLEX_MACRO \
		--ignore DATE_TIME \
		--ignore SYMBOLIC_PERMS \
		--max-line-length=120 --show-types"
	CHK_LIBS="${checkpatch} -f --ignore NEW_TYPEDEFS \
		--ignore CONSTANT_COMPARISON \
		--ignore NOT_UNIFIED_DIFF \
		--ignore COMPLEX_MACRO \
		--ignore UNDOCUMENTED_DT_STRING \
		--ignore LOGGING_CONTINUATION \
		--ignore NEEDLESS_IF \
		--ignore PREFER_PR_LEVEL \
		--ignore DATE_TIME \
		--ignore SYMBOLIC_PERMS \
		--max-line-length=120 --show-types"
	CHK_DRV="${checkpatch} -f --ignore NEW_TYPEDEFS \
		--ignore UNDOCUMENTED_DT_STRING \
		--ignore LOGGING_CONTINUATION \
		--ignore NEEDLESS_IF \
		--ignore COMPLEX_MACRO \
		--ignore CONFIG_DESCRIPTION \
		--ignore LONG_LINE_STRING \
		--ignore SYMBOLIC_PERMS \
		--max-line-length=120 \
		--show-types"
}

function cp_git_commit_msg()
{
	cd ${TARGET_BUILD_DIR}

	local git_hook=.git/hooks
	local commit_msg_drv=utils/commit-msg
	local commit_msg_libs=utils/commit-msg
	# local commit_msg_apps=utils/commit-msg

	cp -f ${commit_msg_drv} ${TARGET_BUILD_DIR}/${git_hook}/commit-msg
	chmod +x ${TARGET_BUILD_DIR}/${git_hook}/commit-msg


	cp -f ${commit_msg_drv} ${TARGET_UBOOT_DIR}/${git_hook}/commit-msg
	chmod +x ${TARGET_UBOOT_DIR}/${git_hook}/commit-msg


	cp -f ${commit_msg_drv} ${TARGET_KERNEL_DIR}/${git_hook}/commit-msg
	chmod +x ${TARGET_KERNEL_DIR}/${git_hook}/commit-msg


	cp -f ${commit_msg_libs} ${TARGET_LIBS_DIR}/${git_hook}/commit-msg
	chmod +x ${TARGET_LIBS_DIR}/${git_hook}/commit-msg


	# cp -f ${commit_msg_apps} ${TARGET_APPS_DIR}/${git_hook}/commit-msg
	# chmod +x ${TARGET_APPS_DIR}/${git_hook}/commit-msg

	cd ${TARGET_BUILD_DIR}
}

################################################################################

check_bash

cd ..
TARGET_TOP_DIR=$(pwd)
TARGET_BUILD_DIR=${TARGET_TOP_DIR}/build
TARGET_OUT_DIR=${TARGET_TOP_DIR}/out
TARGET_UBOOT_DIR=${TARGET_TOP_DIR}/uboot
TARGET_KERNEL_DIR=${TARGET_TOP_DIR}/kernel
TARGET_PREBUILTS_DIR=${TARGET_TOP_DIR}/prebuilts
TARGET_LIBS_DIR=${TARGET_TOP_DIR}/libs
TARGET_APPS_DIR=${TARGET_TOP_DIR}/apps

TARGET_STAGING_INC=${TARGET_OUT_DIR}/staging/include
TARGET_STAGING_LIB=${TARGET_OUT_DIR}/staging/usr/lib
TARGET_STAGING_MODULE=${TARGET_OUT_DIR}/staging/module
TARGET_STAGING_BIN=${TARGET_OUT_DIR}/staging/usr/bin

if [ -d ${TARGET_BUILD_DIR}/.git ] ; then
	cp_git_pre_commit
	cp_git_commit_msg
fi
print_env_dir

find_chip_lunch

