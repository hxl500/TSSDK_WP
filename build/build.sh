#!/bin/bash
# info    : =====
# warning : ?????
# error   : *****
source ${TARGET_BUILD_DIR}/scripts/common_set.sh

. ${TARGET_BUILD_DIR}/device/${TARGET_CHIP}/genimage.sh
. ${TARGET_BUILD_DIR}/utils/rootfs_sz.sh
. ${TARGET_BUILD_DIR}/tools/initrd/reconfig.sh

function usage()
{
	echo -e "usage: "
	echo -e "$0 [-t xxx] [-c xxx] [-y xxx] [-p] [-s xxx] [-g] [--b2 xxx] [--b3 xxx] [--sec] [--tunning_close]"
	echo -e "  [-t uboot/kernel/libs]:build one component"
	echo -e "  [-c clean/distclean]: clean"
	echo -e "  [-y]: release mode"
	echo -e "  [-d]: support docker"
	echo -e "  [-s ramfs/emmc/sd/sflash/nand/sd_update]: set image format, defualt emmc"
	echo -e "  [-p]: strip libs, spinor default strip, others not strip"
	echo -e "  [--np], do not strip"
	echo -e "  [-g]: generate image, default when build.sh"
	echo -e "  [-v version]: rootfs version write to file /etc/issue"
	echo -e "  [--b2 bl2_bin_name], use bootload2 assign be user"
	echo -e "        file should put to prebuilts/tsbin/\${chip}/"
	echo -e "  [--b3 bl3_bin_name], use bootload3 assign be user"
	echo -e "        file should put to prebuilts/tsbin/\${chip}/"
	echo -e "  [--sec], generate secure image, eg. secure fip for tx536"
	echo -e "  [--tunning_close], disable isp tunning function, close isp tunning build"


	echo ""
	echo -e "ex: build all:         ./build.sh"
	echo -e "ex: build uboot:       ./build.sh -t uboot"
	echo -e "ex: build kernel:      ./build.sh -t kernel"
	echo -e "ex: build libs:        ./build.sh -t libs"
	echo -e "ex: build apps:        ./build.sh -t apps"

	echo ""
	echo -e "ex: clean all:    ./build.sh -c clean/distclean"
	echo -e "ex: clean uboot:  ./build.sh -t uboot -c clean/distclean"
	echo -e "ex: clean kernel: ./build.sh -t kernel -c clean/distclean"
	echo -e "ex: clean libs:   ./build.sh -t libs -c clean/distclean"
	echo -e "ex: clean apps:   ./build.sh -t apps -c clean/distclean"

	echo ""
	echo -e "ex: build & gen ramfs image(default):  ./build.sh [-s ramfs]"
	echo -e "ex: build & gen emmc/ext4 image:       ./build.sh -s emmc"
	echo -e "ex: build & gen sdcard/fat32 image:    ./build.sh -s sd"
	echo -e "ex: build & gen sflash/squashfs image: ./build.sh -s sflash"
	echo -e "ex: build & gen nand/ubi image:        ./build.sh -s nand"

	echo ""
	echo -e "ex: force to use bootloader2:  ./build.sh --b2 bl2_bin_name"
	echo -e "ex: force to use bootloader3:  ./build.sh --b3 bl31_bin_name"

	echo ""
	echo -e "ex: build uboot and gen image:  ./build.sh -t uboot -g"

	echo ""
	echo -e "ex: build and gen secure image:  ./build.sh --sec"
        echo ""
        echo -e "ex: disable isp tunning, close isp tunning build:  ./build.sh --tunning_close"
	echo ""
	exit 1
}

function build_uboot()
{
	echo -e "\n\n"
	echo -e "=============================================================="
	echo -e "\e[7m=====build uboot=====\e[0m"
	echo -e "=============================================================="
	echo TARGET_UBOOT_DIR ${TARGET_UBOOT_DIR}
	echo TARGET_UBOOT_DEFCONFIG ${TARGET_UBOOT_DEFCONFIG}

	cd ${TARGET_UBOOT_DIR}
	make ${TARGET_UBOOT_DEFCONFIG}
	make all -j8

	if [ "${secure}" == "y" ] ; then
		gen_fip_bin_secure
	else
		gen_fip_bin
	fi
}

function build_kernel()
{
	#build_kernel_ramfs  #debug on fpga
	echo -e "\n\n"
	echo -e "=============================================================="
	echo -e "\e[7m=====build kernel=====\e[0m"
	echo -e "=============================================================="

	if [ ! -f ${TARGET_UBOOT_DIR}/tools/mkimage ]; then
		build_uboot
	fi

	cd ${TARGET_KERNEL_DIR}
	if [ "x"${TARGET_SDK_BOOT_MODE} == "x""fastboot" -o \
		 "x"${TARGET_KERNEL_DRV_EXPORT} == "x""enable" ] ; then
		rm -rf ${TARGET_INSTALL_MOD_PATH} 2> /dev/null
	fi
	if [ ${image} = "ramfs" ]; then
		echo "make ${TARGET_KERNEL_RAMDISK_DEFCONFIG}"
		make ${TARGET_KERNEL_RAMDISK_DEFCONFIG}
	else
		echo -e "make \033[32m${TARGET_KERNEL_DEFCONFIG}\033[0m"
		if [ "x"${TARGET_SDK_BOOT_MODE} == "x""fastboot" ] ; then
			config_defconfig
		fi
		make ${TARGET_KERNEL_DEFCONFIG}
	fi
	make -j8
	if [ "x"${TARGET_SDK_BOOT_MODE} == "x""fastboot" -o \
		 "x"${TARGET_KERNEL_DRV_EXPORT} == "x""enable" ] ; then
		make modules_install INSTALL_MOD_PATH=${TARGET_INSTALL_MOD_PATH}
		export_drv
		in_tree_drv_strip
	fi
	genimg_kernel_fit
}

function build_kernel_ramfs()
{
	echo -e "\n\n"
	echo -e "=============================================================="
	echo -e "\e[7m=====build kernel ramfs=====\e[0m"
	echo -e "=============================================================="

	if [ ! -f ${TARGET_UBOOT_DIR}/tools/mkimage ]; then
		build_uboot
	fi

	cd ${TARGET_KERNEL_DIR}
	echo "make ${TARGET_KERNEL_RAMDISK_DEFCONFIG}"
	make ${TARGET_KERNEL_RAMDISK_DEFCONFIG}

	make -j8
	genimg_kernel_rootfs_fit

}

function clean_uboot()
{
	echo -e "\n\n"
	echo -e "=============================================================="
	echo -e "\e[7m=====clean uboot=====\e[0m"
	echo -e "=============================================================="
	cd ${TARGET_UBOOT_DIR}
	make ${clean}
}

function clean_kernel()
{
	echo -e "\n\n"
	echo -e "=============================================================="
	echo -e "\e[7m=====clean kernel=====\e[0m"
	echo -e "=============================================================="
	cd ${TARGET_KERNEL_DIR}
	if [ "x"${TARGET_SDK_BOOT_MODE} == "x""fastboot" -o \
		"x"${TARGET_KERNEL_DRV_EXPORT} == "x""enable" ] ; then
		rm -rf ${TARGET_INSTALL_MOD_PATH} 2> /dev/null
	fi
	make ${clean}
}

function build_libs()
{
	echo -e "\n\n"
	echo -e "=============================================================="
	echo -e "\e[7m=====libs $1=====\e[0m"
	echo -e "=============================================================="

	local build_opt
	if [ "$1" == "all" -o "$1" == "clean" ] ; then
		build_opt=$1
	else
		echo "*****libs args($1) error, should be all|clean*****"
	fi

	local build_script=${TARGET_LIBS_DIR}/build.sh

	source ${TARGET_LIBS_DIR}/build.sh ${build_opt}
}

function build_apps()
{
	echo -e "\n\n"
	echo -e "=============================================================="
	echo -e "\e[7m=====apps $1=====\e[0m"
	echo -e "=============================================================="

	local build_opt
	if [ "$1" == "all" -o "$1" == "clean" ] ; then
		build_opt=$1
	else
		echo "*****apps args($1) error, should be all|clean*****"
	fi

	local build_script=${TARGET_APPS_DIR}/build.sh

	${TARGET_APPS_DIR}/build.sh ${build_opt}
}

function rebuild_kernel()
{
	local opt_image=$1

	echo -e "\n\n"
	echo -e "=============================================================="
	echo -e "\e[7m=====rebuild kernel $1=====\e[0m"
	echo -e "=============================================================="

	if [ "x"${opt_image} == "x""initrd" ] ; then
		build_kernel
	fi
}

function gen_image()
{
	echo -e "\n\n"
	echo -e "=============================================================="
	echo -e "\e[7m=====generate image $1=====\e[0m"
	echo -e "=============================================================="

        if [ "x"${BUILD_ISP_TUNNING} == "x""n" ] ; then
                rm -f ${TARGET_OUT_DIR}/rootfs/usr/lib/libpcap*

        fi

	# sdk so
	if [ "x"${TARGET_LIBS_TYPE} == "x""shared" ] ; then
		if [ "x"${TARGET_SYSTEM_PARTITION} == "x""enable" ] ; then
			cp ${TARGET_STAGING_LIB}/*.so*   ${TARGET_SYSTEM_PART_PATH}/lib/ -d
		else
			LIB_FILE=`find ${TARGET_STAGING_LIB}/ -name "*.so*"`
			if [ -n "$LIB_FILE" ]; then
				cp ${TARGET_STAGING_LIB}/*.so*   ${TARGET_OUT_DIR}/rootfs/usr/lib/ -d
			fi
		fi
	fi

#	# move to prebuilts/custom_lunch, delete later
#	if [ ${TARGET_CHIPSERIES} == "tx536" ] ; then
#		cp ${TARGET_STAGING_INC}/body_detect_cfg.cfg    ${TARGET_OUT_DIR}/rootfs/usr/bin
#		cp ${TARGET_STAGING_INC}/body_detect_weight.weight    ${TARGET_OUT_DIR}/rootfs/usr/bin
#		cp ${TARGET_STAGING_INC}/face_detect_cfg.cfg    ${TARGET_OUT_DIR}/rootfs/usr/bin
#		cp ${TARGET_STAGING_INC}/face_detect_weight.weight    ${TARGET_OUT_DIR}/rootfs/usr/bin
#	elif [ ${TARGET_CHIPSERIES} == "tx521" ] || [ ${TARGET_CHIPSERIES} == "ct_v200" ] ; then
#		cp ${TARGET_STAGING_INC}/pcpp_detect_cfg.cfg    ${TARGET_OUT_DIR}/rootfs/usr/bin
#		cp ${TARGET_STAGING_INC}/pcpp_detect_weight.weight    ${TARGET_OUT_DIR}/rootfs/usr/bin
#	else
#		cp ${TARGET_STAGING_INC}/body_detect_cfg.cfg    ${TARGET_OUT_DIR}/rootfs/usr/bin
#		cp ${TARGET_STAGING_INC}/body_detect_weight.weight    ${TARGET_OUT_DIR}/rootfs/usr/bin
#	fi

	# sdk ko
	pre_cons=${TARGET_PREBUILTS_DIR}/custom_lunch/${LUNCH_SEL_NUM}/construct_pre_build.sh
	if [ -e ${pre_cons} ]; then
		${pre_cons}
	fi

#	post_cons=${TARGET_PREBUILTS_DIR}/custom_lunch/${LUNCH_SEL_NUM}/construct_post_build.sh
#	if [ -e ${post_cons} ]; then
#		${post_cons}
#	fi

#	LIB_FILE=`find ${TARGET_STAGING_MODULE}/ -name "*.ko"`
#	if [ -n "$LIB_FILE" ]; then
#		cp ${TARGET_STAGING_MODULE}/*.ko   ${TARGET_OUT_DIR}/rootfs/usr/lib/modules/
#	fi

	if [ "x"${TARGET_SDK_BOOT_MODE} == "x""fastboot" -a "x"${TARGET_SYSTEM_PARTITION} == "x""enable" ] ; then
		if [ "x"${TARGET_FAST_APP} == "x""aov_loop" ] ; then
		    echo -e "[\033[32maov_loop in ${TARGET_OUT_DIR}/rootfs, no need copy.\033[0m...]"
		else
			cp ${TARGET_STAGING_BIN}/${TARGET_FAST_APP}   	 ${TARGET_OUT_DIR}/rootfs/usr/bin/
		fi
		#cp ${TARGET_STAGING_BIN}/sample_algo   	 ${TARGET_OUT_DIR}/system/bin/
		#cp ${TARGET_STAGING_BIN}/sample_debug_client   	 ${TARGET_OUT_DIR}/system/bin/
		#cp ${TARGET_STAGING_BIN}/sample_algo   	 ${TARGET_OUT_DIR}/rootfs/usr/bin/
		# static link may lead to bin file total size beyond the partition size
		# rsync -auvH --exclude=${TARGET_FAST_APP}  ${TARGET_STAGING_BIN}/*  ${TARGET_SYSTEM_PART_PATH}/bin/
	#else
	#	LIB_FILE=`find ${TARGET_STAGING_BIN}/ -type f`
	#	if [ -n "$LIB_FILE" ]; then
	#		cp ${TARGET_STAGING_BIN}/*   ${TARGET_OUT_DIR}/rootfs/usr/bin/
	#	fi
	fi
	local opt_image=$1
	local opt_secure=$2

	genimg_entry ${opt_image} ${opt_secure}
	rebuild_kernel ${opt_image}
}

function export_drv()
{
	${TARGET_BUILD_DIR}/device/${TARGET_CHIP}/export_driver.sh
	if [ $? -ne 0 ]; then
		echo "Export drivers error"
		exit 1
	fi
}

function strip()
{
	local path=$1
	local para=$([ "$2" == "NONE" ] && echo "" || echo $2)
    echo "======================================================================================="
	echo "[-strip:]"
	echo "[--PATH:${path}]"
	echo "[--PARA: ${para}]"

	if ls $path &> /dev/null; then
		${CROSS_COMPILE}strip ${para} ${path} 2> /dev/null
	else
		echo "[--No exit specific files]"
	fi
    echo "[-EXIT]"
    echo "======================================================================================="
}

function libs_strip()
{
    echo "======================================================================================="
	echo "[libs_strip:]"
    echo "======================================================================================="

	if [ "x"${TARGET_LIBS_TYPE} == "x""shared" ] ; then
        strip "${TARGET_STAGING_LIB}/*.so*" "NONE"
	fi
  	if [ "x"${TARGET_SYSTEM_PARTITION} == "x""enable" ] ; then
        strip "${TARGET_SYSTEM_PART_PATH}/lib/*.so*" "NONE"
	fi
    strip "${TARGET_STAGING_MODULE}/*.ko" "-g"
    echo "======================================================================================="
}

function in_tree_drv_strip()
{
    echo "======================================================================================="
	echo "[IN-TREE drv strip:]"
    echo "======================================================================================="

	strip "${TARGET_OUT_DIR}/rootfs/usr/lib/modules/*.ko" "-g"
	if [ "x"${TARGET_SYSTEM_PARTITION} == "x""enable" ] ; then
	    strip "${TARGET_SYSTEM_PART_PATH}/lib/modules/*.ko" "-g"
	fi
    echo "======================================================================================="
}

function bins_strip()
{
	echo "======================================================================================="
	echo "[bins_strip:]"
    echo "======================================================================================="

	for target in ${TARGET_STAGING_BIN}/*; do
		if [ -f "$target" ]; then
			if file "$target" | grep -q "ELF" && ! file "$target" | grep -q "no section header"; then
				strip "$target" "NONE"
				#echo "Stripped: $target"
			else
				echo "Not an ELF file: $target"
			fi
		else
			echo "Invalid target: $target"
		fi
	done

   echo "======================================================================================="
}

function build()
{
	echo "clean:${clean}"
	echo "target:${target}"

	if [ "${clean}" != "" ]		# clean
	then
		case $target in
		uboot)
			clean_uboot
			;;
		kernel)
			clean_kernel
			;;
		libs)
			build_libs clean
			;;
		apps)
			build_apps clean
			;;
		"")
			echo -e "\n\n=====clean all====="
			clean_uboot
			build_libs clean
			build_apps clean
			clean_kernel
			;;
		esac
	else						# build

		if [[ "x"${TARGET_IMAGE_TYPE} == "x""sflash" || "x"${TARGET_IMAGE_TYPE} == "x""initrd" ]] && \
		 [[ "x"${build_strip} == "x""n" && "x"${force_n_strip} == "x""n" ]] ; then
			echo "default strip file when startup image in spi flash"
			build_strip="y"
		fi

		if [ "x"${TARGET_IMAGE_TYPE} == "x""nand" -a \
		"x"${build_strip} == "x""n" -a "x"${force_n_strip} == "x""n" ] ; then
			echo "default strip file when startup image in nand flash"
			build_strip="y"
		fi

		if [ "x"${TARGET_IMAGE_TYPE} == "x""emmc" -a \
		"x"${build_strip} == "x""n" -a "x"${force_n_strip} == "x""n" ] ; then
			echo "default strip file when startup image in emmc flash"
			build_strip="y"
		fi

		case $target in
		uboot)
			build_uboot
			if [ "x"${genimage} == "x""y" ] ; then
				gen_image ${image} ${secure}
			fi
			;;
		libs)
			build_libs all
			if [ "x"${build_strip} == "x""y" ] ; then
				libs_strip
			fi

			if [ "x"${genimage} == "x""y" ] ; then
				gen_image ${image} ${secure}
			fi
			;;
		apps)
			build_apps all
			if [ "x"${build_strip} == "x""y" ] ; then
				bins_strip
			fi
			if [ "x"${genimage} == "x""y" ] ; then
				gen_image ${image} ${secure}
			fi
			;;
		kernel)
			build_kernel
			if [ "x"${genimage} == "x""y" ] ; then
				gen_image ${image} ${secure}
			fi
			;;
		"")
			echo -e "\n\n=====build all====="
			if [ "x"${TARGET_CHIP} != "x""x86" ] ; then
				build_uboot
			fi
			if [ "x"${TARGET_CHIP} != "x""x86" ] ; then
				build_kernel
			fi
			build_libs all
			build_apps all
			if [ "x"${build_strip} == "x""y" ] ; then
				libs_strip
				bins_strip
			fi

			if [ "x"${TARGET_CHIP} != "x""x86" ] ; then
				gen_image ${image} ${secure}
			fi
			;;
		esac

		rootfs_show_sz "after build"
		rootfs_show_new_fl_libs
		rootfs_show_new_fl_apps
	fi
}

################################################################################
export BUILD_TYPE="debug"
export BUILD_ISP_TUNNING="y"
#echo build_tunning:$BUILD_ISP_TUNNING
target=""
clean=""
image=${TARGET_IMAGE_TYPE}
genimage="n"
secure="n"
build_strip="n"
force_n_strip="n"
version="v1.0"
if [ "$#" = "0" ] ; then
	rm -rf ${TARGET_OUT_DIR}
fi

echo "selected lunch: ${LUNCH_SEL}"
set -- $(getopt -u -o t:c:s:v:y:hgp -l b2:,b3:,sec,tunning_close,tunning_open,np,cov -- $@)

while [ -n "$1" ]
do
	case "$1" in
	-t)
		target=$2
		shift
		;;
	-c)
		clean=$2
		shift
		;;
	-s)
		image=$2
		TARGET_IMAGE_TYPE=$2
		shift
		;;
	-g)
		genimage="y"
		#shift
		;;
	-y)
		BUILD_TYPE=$2
		#shift
		;;
	-d)
		export DOCKER_BUILD="y"
		#shift
		;;
	-v)
		version=$2
		sed -i '/Welcome to Tsingmicro (Alpha ver)/i\'"${version}"'' ${TARGET_PREBUILTS_DIR}/rootfs/etc/issue
		shift
		;;
	-p)
		export build_strip="y"
		;;
	--np)
		force_n_strip="y"
		build_strip="n"
		echo "force to not strip"
		;;
	--b2)
		export TARGET_LOADER_2=${TARGET_PREBUILTS_DIR}/tsbin/${TARGET_CHIP}/$2
		shift
		;;
	--b3)
		export TARGET_LOADER_3=${TARGET_PREBUILTS_DIR}/tsbin/${TARGET_CHIP}/$2
		shift
		;;
	--sec)
		secure="y"
		# shift
		;;
	--tunning_close)
		BUILD_ISP_TUNNING="n"
		;;
	--tunning_open)
		BUILD_ISP_TUNNING="y"
		;;
	--cov)
		export ENABLE_COVERAGE="y"
		;;
	-h)
		usage
		exit
		;;
	?)
		usage
		exit
		;;
	--)
		shift
		break
		;;
	esac

	shift
done

#echo xxxx_build_tunning:$BUILD_ISP_TUNNING

if [ "${clean}" == "" ]; then
	if [ "x""${target}" == "x""" ]; then
		rm -rf ${TARGET_OUT_DIR}
	fi
	mkdir -p ${TARGET_OUT_DIR}
	mkdir -p ${TARGET_STAGING_INC}
	mkdir -p ${TARGET_STAGING_LIB}
	mkdir -p ${TARGET_STAGING_BIN}
	mkdir -p ${TARGET_STAGING_MODULE}
	if [ "x"${TARGET_SYSTEM_PARTITION} == "x""enable" ] ; then
		mkdir -p ${TARGET_SYSTEM_PART_PATH}
	fi

	rootfs_show_init
	if [ "x"${TARGET_CHIP} != "x""x86" ] ; then
		mkdir -p ${TARGET_OUT_DIR}/rootfs

		uclibc_suff="_${TARGET_LIBC}"
		if [ "x"${TARGET_ROOTFS_TYPE} == "x""tiny" ] ; then
			rootfs_type="_${TARGET_ROOTFS_TYPE}"
		fi
		echo -e "[TARGET_LIBC: \033[32m$TARGET_LIBC\033[0m]"
		echo -e "[rootfs_type: \033[32m$TARGET_ROOTFS_TYPE\033[0m]"
		echo -e "[rootfs_path: \033[32m${TARGET_PREBUILTS_DIR} \
		/rootfs_${TARGET_BITS}${rootfs_type}${uclibc_suff}.tar.gz\033[0m]"

		tar -xvf ${TARGET_PREBUILTS_DIR}/rootfs_${TARGET_BITS}${rootfs_type}${uclibc_suff}.tar.gz \
			-C ${TARGET_OUT_DIR}/rootfs/

		if [ -d ${TARGET_TOP_DIR}/libs/sdk/lib ] ; then
			cp -r ${TARGET_TOP_DIR}/libs/sdk/lib/* ${TARGET_OUT_DIR}/rootfs/usr/lib/
		fi
		if [ -d ${TARGET_TOP_DIR}/libs/sdk/lib/include ]; then
			cp -r ${TARGET_TOP_DIR}/libs/sdk/lib/include ${TARGET_OUT_DIR}/rootfs/usr/
		fi
		if [ -d ${TARGET_TOP_DIR}/libs/sdk/ko/ ]; then
			cp -r ${TARGET_TOP_DIR}/libs/sdk/ko/* ${TARGET_OUT_DIR}/rootfs/usr/lib/modules/
		fi
		rootfs_show_sz "prebuilts/rootfs"

#		# move to prebuilts/custom_lunch, delete later
#		if [ "x"${TARGET_LIBC} == "x""uclibc" ] && [ -f device/${TARGET_CHIP}/insmod_nfs ]; then
#				cp device/${TARGET_CHIP}/insmod_* \
#					${TARGET_OUT_DIR}/rootfs/etc/init.d/
#				chmod +x ${TARGET_OUT_DIR}/rootfs/etc/init.d/insmod_*
#		fi

#		# move to prebuilts/custom_lunch, delete later
#		# fastboot use less scripts
#		if [ "x"${TARGET_SDK_BOOT_MODE} != "x""fastboot" ] ; then
#			# cp init scrit to /etc/inid.d
#			if [ -f device/${TARGET_CHIP}/S*load_ko ];then
#				cp device/${TARGET_CHIP}/S*load_ko \
#					${TARGET_OUT_DIR}/rootfs/etc/init.d/
#				chmod +x ${TARGET_OUT_DIR}/rootfs/etc/init.d/S*load_ko
#			fi
#
#			if [ -f device/${TARGET_CHIP}/S*coredump_conf.sh ];then
#				cp device/${TARGET_CHIP}/S*coredump* \
#					${TARGET_OUT_DIR}/rootfs/etc/profile.d/
#				chmod +x ${TARGET_OUT_DIR}/rootfs/etc/profile.d/S*coredump*
#			fi
#		fi

# 		# move to prebuilts/custom_lunch, delete later
#		# not recommend, too many things in ${TARGET_ROOTFS_CUSTOM}
#		if [ "x"${TARGET_ROOTFS_CUSTOM} != "x""" ] ; then
#			cp -r ${TARGET_ROOTFS_CUSTOM}/* ${TARGET_OUT_DIR}/rootfs/
#		fi

#		# nothing in this folder, if need, add cp in prebuilts/custom_lunch
#		# delate later
#		if [ "x"${TARGET_SYSTEM_PARTITION} == "x""enable" ] ; then
#			cp -r ${TARGET_PREBUILTS_DIR}/fastboot_${TARGET_BITS}/system/*  ${TARGET_SYSTEM_PART_PATH}/
#		fi

# 		# move to prebuilts/custom_lunch, delete later
#		if [ "x"${TARGET_SDK_BOOT_MODE} == "x""fastboot" ] ; then
#			cp -r ${TARGET_PREBUILTS_DIR}/drv_load_tools/${TARGET_CHIP}/* ${TARGET_OUT_DIR}/rootfs/usr/bin
#		fi

# 		# move to prebuilts/custom_lunch, delete later
#		# not recommend, too many things in rootfs_spinand_0
		if [ "x"${TARGET_IMAGE_TYPE} == "x""nand" ] ; then
		cp -r ${TARGET_PREBUILTS_DIR}/develop_${TARGET_BITS}/staging/rootfs_spinand_0/* ${TARGET_OUT_DIR}/rootfs/
			ln -s ${TARGET_OUT_DIR}/rootfs ${TARGET_OUT_DIR}/romfs
		fi
	else
		mkdir -p ${TARGET_OUT_DIR}/rootfs
	fi
else
	if [ "${target}" == "" ]; then
		rm -rf ${TARGET_OUT_DIR}
	fi
fi

build

