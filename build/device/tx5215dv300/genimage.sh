#!/bin/bash

function genimg_update_ini()
{
	local ini=$TARGET_OUT_DIR/update.ini

	#if [ ! -f ${ini} ]; then
		echo "[env]" > ${ini}
		echo "swupdate_reset_env = y" >> ${ini}
		echo "[uboot]" >> ${ini}
		echo "swupdate_fip_flag = y" >> ${ini}
		echo "fit_name = fip_${TARGET_CHIP}_${TARGET_BOARD}.bin" >> ${ini}
		#echo "fit_mmc_size = 0x1000" >> ${ini}
		echo "[kernel]" >> ${ini}
		echo "swupdate_kernel_flag = y" >> ${ini}
		echo "kernel_name = ${TARGET_FIT_ITB}" >> ${ini}
		#echo "kernel_mmc_size = 0x8000" >> ${ini}

		if [ "x"${TARGET_IMAGE_TYPE} == "x""sflash" ]; then
			echo "[rootfs]" >> ${ini}
			echo "swupdate_rootfs_flag = y" >> ${ini}
			echo "rootfs_name = rootfs.squashfs" >> ${ini}
			echo "[usrfs]" >> ${ini}
			echo "swupdate_usrfs_flag = y" >> ${ini}
			echo "rootfs_name = usrfs.squashfs" >> ${ini}
		elif [ "x"${TARGET_IMAGE_TYPE} == "x""initrd" ]; then
			echo "[rootfs]" >> ${ini}
			echo "swupdate_rootfs_flag = y" >> ${ini}
			echo "rootfs_name = rootfs.initrd" >> ${ini}
			echo "[usrfs]" >> ${ini}
			echo "swupdate_usrfs_flag = y" >> ${ini}
			echo "rootfs_name = system.squashfs" >> ${ini}
		else
			echo "[rootfs]" >> ${ini}
			echo "swupdate_rootfs_flag = y" >> ${ini}
			echo "rootfs_name = rootfs.ext4.gz" >> ${ini}
		fi
	#fi
}

function genimg_firmware_version_ini()
{
	if [ ! -d "${TARGET_KERNEL_DIR}/.git" -o ! -d "${TARGET_UBOOT_DIR}/.git" ]; then
		echo ".git not exist, genimg_firmware_version_ini bypass!"
		return 0
	else
		echo ".git exist, run genimg_firmware_version_ini!"
	fi

	local ini=$TARGET_OUT_DIR/rootfs/etc/firmware_version.ini

	local tag_ver=`cd $TARGET_UBOOT_DIR; git describe --tags --abbrev=0`
	local uboot_ver=`cd $TARGET_UBOOT_DIR; git describe --abbrev=12 | \
			awk -F- '{printf("-%05d-%s", $(NF-1),substr($(NF),0,13))}'`
	uboot_ver="$tag_ver$uboot_ver"

	tag_ver=`cd $TARGET_KERNEL_DIR; git describe --tags --abbrev=0`
	local kernel_ver=`cd $TARGET_KERNEL_DIR; git describe --abbrev=12 | \
			awk -F- '{printf("-%05d-%s", $(NF-1),substr($(NF),0,13))}'`
	kernel_ver="$tag_ver$kernel_ver"

	tag_ver=`cd $TARGET_OUT_DIR/../prebuilts/; git describe --tags --abbrev=0`
	local rootfs_ver=`cd $TARGET_OUT_DIR/../prebuilts/; git describe --abbrev=12 | \
			awk -F- '{printf("-%05d-%s", $(NF-1),substr($(NF),0,13))}'`
	rootfs_ver="$tag_ver$rootfs_ver"
	#local libs_ver=`cd $TARGET_OUT_DIR/../libs/; git describe --abbrev=12 | \
	#		awk -F- '{printf("-%05d-%s", $(NF-1),substr($(NF),0,13))}'`

	if [ ! -d "$TARGET_OUT_DIR/rootfs/etc/" ]; then
		mkdir -p $TARGET_OUT_DIR/rootfs/etc/
	else
		rm -f $ini
	fi

	echo "[uboot]" > ${ini}
	echo "uboot_ver = $uboot_ver" >> ${ini}

	echo "[kernel]" >> ${ini}
	echo "kernel_ver = $kernel_ver" >> ${ini}

	echo "[rootfs]" >> ${ini}
	echo "rootfs_ver = $rootfs_ver" >> ${ini}

	#echo "[libs]" >> ${ini}
	#echo "libs_ver = $libs_ver" >> ${ini}
}

function genimg_sflash_cramfs()
{
	if [ "x"${TARGET_ROOTFS_CUSTOM} != "x""" ] ; then
		FAKEROOTDONTTRYCHOWN=1 fakeroot -- ${TARGET_BUILD_DIR}/tools/cramfs/fakeroot_custom_0
	else
		FAKEROOTDONTTRYCHOWN=1 fakeroot -- ${TARGET_BUILD_DIR}/tools/cramfs/fakeroot
	fi
	genimg_update_ini
}

function genimg_sflash_squashfs()
{
    if [ "x"${TARGET_ROOTFS_CUSTOM} != "x""" ] ; then
		FAKEROOTDONTTRYCHOWN=1 fakeroot -- ${TARGET_BUILD_DIR}/tools/squashfs/fakeroot_custom_0
	else
		FAKEROOTDONTTRYCHOWN=1 fakeroot -- ${TARGET_BUILD_DIR}/tools/squashfs/fakeroot
	fi
	genimg_update_ini
}

function genimg_initrd_squashfs()
{
	FAKEROOTDONTTRYCHOWN=1 fakeroot -- ${TARGET_BUILD_DIR}/tools/initrd/fakeroot
	genimg_update_ini
}

function gen_fip_kernel_bin()
{
	echo -e "\n\n=====gen fit bin====="

	local packtool=${TARGET_BUILD_DIR}/tools/pack-util/pack-util
	local bl2=${TARGET_LOADER_2%.bin}_start_kernel.bin
	local bl2_short
	local bl2_short_path
	local kernel=${TARGET_OUT_DIR}/${TARGET_FIT_ITB}
	local fip
	local fip_uart

	bl2_short=${bl2##*/}
	bl2_short=${bl2_short%.bin}

	fip=fip_${TARGET_CHIP}_${TARGET_BOARD}_${bl2_short}.bin
	fip_uart=fip_uart_${TARGET_CHIP}_${TARGET_BOARD}_${bl2_short}.bin

	if [ ! -f ${packtool} ]; then
		cd ${TARGET_BUILD_DIR}/tools/pack-util/
		make
		cd -
	fi

	if [ ! -f ${packtool} ] \
	  || [ -z ${bl2} ] \
	  || [ ! -f ${kernel} ] ; then
		echo "gen fip bin error, file not exist"
		echo "fiptool: ${packtool}"
		echo "bl2: ${bl2}"
		echo "kernel: ${kernel}"
		exit 1
	fi

	${packtool} -o ${TARGET_OUT_DIR}/ddr_init.bin ${bl2}
	${packtool} -o ${TARGET_OUT_DIR}/kernel.bin ${kernel}

	dd if=${TARGET_OUT_DIR}/ddr_init.bin of=${TARGET_OUT_DIR}/${fip} bs=512 seek=0
	dd if=${TARGET_OUT_DIR}/kernel.bin of=${TARGET_OUT_DIR}/${fip} bs=512 seek=128
	if [ $? == 0 ] ; then
		echo "generate $TARGET_OUT_DIR/${fip} success"
		#cp $TARGET_OUT_DIR/${fip} $TARGET_OUT_DIR/fip_${TARGET_CHIP}_${TARGET_BOARD}.bin
		mv ${TARGET_OUT_DIR}/ddr_init.bin $TARGET_OUT_DIR/${fip_uart}
	else
		echo "generate $TARGET_OUT_DIR/${fip} fail"
	fi

	rm  ${TARGET_OUT_DIR}/kernel.bin
}

function genimg_kernel_rootfs_fit()
{

	echo -e "\n\n=====packet fit====="

	local mkimage=${TARGET_UBOOT_DIR}/tools/mkimage
	local image=${TARGET_KERNEL_DIR}/arch/${TARGET_ARCH}/boot/Image
	local zImage=${TARGET_KERNEL_DIR}/arch/${TARGET_ARCH}/boot/zImage
	local dtb="${TARGET_KERNEL_DIR}/arch/${TARGET_ARCH}/boot/dts/"
	local dtb=${dtb}"${TARGET_VENTOR}/${TARGET_DTB}"

	local its=${TARGET_UBOOT_DIR}/fit/${TARGET_FIT_ITS}


	cd ${TARGET_OUT_DIR}

	cp ${image} ./
	cp ${zImage} ./
	cp ${dtb} ./
	cp ${its} ./

	echo mk ${mkimage}
	echo its ${its}
	echo itb ${TARGET_FIT_ITB}

	#${mkimage} -f ${TARGET_FIT_ITS} ${TARGET_FIT_ITB_RAMDISK}

	cat zImage > ${TARGET_FIT_ITB}; cat ${TARGET_DTB} >> ${TARGET_FIT_ITB}
	rm -rf Image zImage ${TARGET_FIT_ITS} ${TARGET_DTB}
}

function gen_kernel_bin()
{
	echo -e "\n\n=====gen kernel bin====="

	local packtool=${TARGET_BUILD_DIR}/tools/pack-util/pack-util
	local kernel=${TARGET_OUT_DIR}/${TARGET_FIT_ITB}
	local fip

	fip=fip_${TARGET_CHIP}_${TARGET_BOARD}.bin

	if [ ! -f ${packtool} ]; then
		cd ${TARGET_BUILD_DIR}/tools/pack-util/
		make
		cd -
	fi

	if [ ! -f ${fip} ]; then
		gen_fip_bin
	fi

	if [ ! -f ${packtool} ] \
	  || [ ! -f ${kernel} ]  ; then
		echo "gen fip bin error, file not exist"
		echo "fiptool: ${packtool}"
		echo "kernel: ${kernel}"
		exit 1
	fi
	if [ "x"${TARGET_KERNEL_TYPE} == "x""compressed" ] ; then
		${packtool} -o ${TARGET_OUT_DIR}/kernel.bin ${kernel}
	else
		cp ${kernel} kernel.bin
	fi
	dd if=${TARGET_OUT_DIR}/kernel.bin of=${TARGET_OUT_DIR}/${fip} bs=512 seek=1024
	if [ $? == 0 ] ; then
		echo "generate $TARGET_OUT_DIR/${fip} success"
	else
		echo "generate $TARGET_OUT_DIR/${fip} fail"
	fi

	rm ${TARGET_OUT_DIR}/kernel.bin
}

function genimg_kernel_fit()
{

	echo -e "\n\n=====packet fit====="

	local mkimage=${TARGET_UBOOT_DIR}/tools/mkimage
	local image=${TARGET_KERNEL_DIR}/arch/${TARGET_ARCH}/boot/Image
	local zImage=${TARGET_KERNEL_DIR}/arch/${TARGET_ARCH}/boot/zImage
	local dtb="${TARGET_KERNEL_DIR}/arch/${TARGET_ARCH}/boot/dts/"
	local dtb=${dtb}"${TARGET_VENTOR}/${TARGET_DTB}"

	local its=${TARGET_UBOOT_DIR}/fit/${TARGET_FIT_ITS}


	cd ${TARGET_OUT_DIR}

	cp ${image} ./
	cp ${zImage} ./
	cp ${dtb} ./
	cp ${its} ./

	echo mk ${mkimage}
	echo its ${its}
	echo itb ${TARGET_FIT_ITB}

	echo "TARGET_KERNEL_TYPE=${TARGET_KERNEL_TYPE}"

	#${mkimage} -f ${TARGET_FIT_ITS} ${TARGET_FIT_ITB}
	if [ "x"${TARGET_KERNEL_TYPE} == "x""compressed" ] ; then
		cat zImage > ${TARGET_FIT_ITB}; cat ${TARGET_DTB} >> ${TARGET_FIT_ITB}
	else
		local packtool=${TARGET_BUILD_DIR}/tools/pack-util/pack-util

		if [ ! -f ${packtool} ]; then
			cd ${TARGET_BUILD_DIR}/tools/pack-util/
			make
			cd -
		fi
		${packtool} -o dtb.bin ${TARGET_DTB}
		${packtool} -o Image.bin Image
		cat Image.bin > ${TARGET_FIT_ITB}; cat dtb.bin >> ${TARGET_FIT_ITB}
		rm -rf dtb.bin Image.bin
	fi
	if [ "x"${TARGET_SDK_BOOT_MODE} == "x""fastboot" ] ; then
		gen_kernel_bin
	fi

	#gen_fip_kernel_bin
}

function genimg_emmc_ext4()
{
	#local genimage_tool=${TARGET_BUILD_DIR}/tools/genimage/genimage
	#local genimage_cfg=${TARGET_BUILD_DIR}/device/tx536/genimage_emmc.cfg
	#local image_name=emmc.img

	#genimg_kernel_fit

	FAKEROOTDONTTRYCHOWN=1 fakeroot -- ${TARGET_BUILD_DIR}/tools/ext4/fakeroot 64

	gzip -9 -c -n ${TARGET_OUT_DIR}/rootfs.ext4 > ${TARGET_OUT_DIR}/rootfs.ext4.gz

	cd ${TARGET_OUT_DIR}/rootfs
	tar -zcf ../rootfs.tar.gz *
	cd -

	#${genimage_tool} --config ${genimage_cfg}
	#gzip -kf ${image_name}
	#rm -rf ${TARGET_OUT_DIR}/tmp
	genimg_update_ini
}

function genimg_nand_ubi()
{
	FAKEROOTDONTTRYCHOWN=1 fakeroot -- ${TARGET_BUILD_DIR}/tools/mtd/fakeroot
	genimg_update_ini
}

function gen_fip_bin_secure()
{
	echo -e "\n\n=====gen fit bin secure ====="

	local packtool=${TARGET_BUILD_DIR}/tools/pack-util/pack-util
	local bl2=${TARGET_LOADER_2}
	local bl2_short
	local uboot=${TARGET_UBOOT_DIR}/u-boot.bin
	local fip
	local fip_uart
	local iv
	local usr_root_key
	local usr_id_is

	bl2_short=${TARGET_LOADER_2##*/}
	bl2_short=${bl2_short%.bin}

	fip=fip_${TARGET_CHIP}_${TARGET_BOARD}_${bl2_short}.bin
	fip_uart=fip_uart_${TARGET_CHIP}_${TARGET_BOARD}_${bl2_short}.bin

	if [ ! -f ${packtool} ]; then
		cd ${TARGET_BUILD_DIR}/tools/pack-util/
		make
		cd -
	fi

	if [ ! -f ${packtool} ] \
	  || [ -z ${bl2} ] \
	  || [ ! -f ${uboot} ] ; then
		echo "gen fip bin error, file not exist"
		echo "fiptool: ${packtool}"
		echo "bl2: ${bl2}"
		echo "uboot: ${uboot}"
		exit 1
	fi

	source ${TARGET_BUILD_DIR}/device/${TARGET_CHIP}/secure_param.sh

	gen_secure_boot_param

	if [ ${SECURE_BOOT_MODE} == 1 ] ; then
		iv="--iv ${SECURE_BOOT_IV}"
		usr_root_key="--user-root-key ${SECURE_BOOT_USR_ROOT_KEY}"
		fip=fip_${TARGET_CHIP}_${TARGET_BOARD}_${bl2_short}_sig_encry.bin
		fip_uart=fip_uart_${TARGET_CHIP}_${TARGET_BOARD}_${bl2_short}_sig_encry.bin
	else
		iv=""
		usr_root_key=""
		fip=fip_${TARGET_CHIP}_${TARGET_BOARD}_${bl2_short}_only_sig.bin
		fip_uart=fip_uart_${TARGET_CHIP}_${TARGET_BOARD}_${bl2_short}_only_sig.bin
	fi

	if [ ${SECURE_BOOT_USER_ID_IS} == y ] ; then
		usr_id_is="--user-id ${SECURE_BOOT_USER_ID} "
	else
		usr_id_is=""
	fi

	${packtool} --output ${TARGET_OUT_DIR}/ddr_init.bin --private-key ${SECURE_BOOT_PRIV_KEY} \
			${usr_root_key} ${iv} ${usr_id_is}\
			--public-key-hash ${SECURE_BOOT_PUBLIC_KEY_HASH} \
			--boot-image-version ${SECURE_BOOT_IMAGE_VERSION} \
			${bl2}

	${packtool} --output ${TARGET_OUT_DIR}/uboot.bin --private-key ${SECURE_BOOT_PRIV_KEY} \
			${usr_root_key} ${iv} ${usr_id_is} \
			--public-key-hash ${SECURE_BOOT_PUBLIC_KEY_HASH} \
			--boot-image-version ${SECURE_BOOT_IMAGE_VERSION} \
			${uboot}

	dd if=${TARGET_OUT_DIR}/ddr_init.bin of=${TARGET_OUT_DIR}/${fip} bs=512 seek=0
	dd if=${TARGET_OUT_DIR}/uboot.bin of=${TARGET_OUT_DIR}/${fip} bs=512 seek=128

	if [ $? == 0 ] ; then
		echo "generate secure $TARGET_OUT_DIR/${fip} success"
		cp $TARGET_OUT_DIR/${fip} $TARGET_OUT_DIR/fip_${TARGET_CHIP}_${TARGET_BOARD}.bin
		cp ${SECURE_BOOT_PUBLIC_KEY_HASH} $TARGET_OUT_DIR/
		mv ${TARGET_OUT_DIR}/ddr_init.bin $TARGET_OUT_DIR/${fip_uart}
	else
		echo "generate secure $TARGET_OUT_DIR/${fip} fail"
	fi

	rm ${TARGET_OUT_DIR}/uboot.bin
}

function gen_fip_bin()
{
	echo -e "\n\n=====gen fit bin====="

	local packtool=${TARGET_BUILD_DIR}/tools/pack-util/pack-util
	local bl2=${TARGET_LOADER_2}
	local bl2_short
	local uboot=${TARGET_UBOOT_DIR}/u-boot.bin
	local fip
	local fip_uart
	local preboot=${TARGET_PREBUILTS_DIR}/tsbin/${TARGET_CHIP}/preboot.bin
	local nand_flash=${TARGET_LOADER_2_NAND}

# set -x
	bl2_short=${TARGET_LOADER_2##*/}
	bl2_short=${bl2_short%.bin}

	fip=fip_${TARGET_CHIP}_${TARGET_BOARD}_${bl2_short}.bin
	fip_uart=fip_uart_${TARGET_CHIP}_${TARGET_BOARD}_${bl2_short}.bin

	if [ ! -f ${packtool} ]; then
		cd ${TARGET_BUILD_DIR}/tools/pack-util/
		make
		cd -
	fi

	if [ ! -f ${packtool} ] \
	  || [ -z ${bl2} ] \
	  || [ ! -f ${uboot} ] ; then
		echo "gen fip bin error, file not exist"
		echo "fiptool: ${packtool}"
		echo "bl2: ${bl2}"
		echo "uboot: ${uboot}"
		exit 1
	fi

	${packtool} -o ${TARGET_OUT_DIR}/ddr_init.bin ${bl2}
	${packtool} -o ${TARGET_OUT_DIR}/uboot.bin ${uboot}

	dd if=${TARGET_OUT_DIR}/ddr_init.bin of=${TARGET_OUT_DIR}/${fip} bs=512 seek=0
	dd if=${TARGET_OUT_DIR}/uboot.bin of=${TARGET_OUT_DIR}/${fip} bs=512 seek=128
	if [ $? == 0 ] ; then
		echo "generate $TARGET_OUT_DIR/${fip} success"
		cp $TARGET_OUT_DIR/${fip} $TARGET_OUT_DIR/fip_${TARGET_CHIP}_${TARGET_BOARD}.bin
		mv ${TARGET_OUT_DIR}/ddr_init.bin $TARGET_OUT_DIR/${fip_uart}
	else
		echo "generate $TARGET_OUT_DIR/${fip} fail"
	fi

	rm ${TARGET_OUT_DIR}/uboot.bin
	if [ "x"${TARGET_IMAGE_TYPE} == "x""nand" ]; then
		if [ -f ${preboot} ] \
		&& [ -f ${nand_flash} ] ; then
			echo "gen nand flash fip bin"
			${packtool} -o ${TARGET_OUT_DIR}/preboot_p.bin ${preboot}
			${packtool} -o ${TARGET_OUT_DIR}/ddr_init_p.bin ${nand_flash}
			${packtool} -o ${TARGET_OUT_DIR}/uboot_p.bin ${uboot}
			nand_flash_fip=fip_${TARGET_CHIP}_${TARGET_BOARD}_nand_flash.bin
			touch ${TARGET_OUT_DIR}/${nand_flash_fip}
			dd if=${TARGET_OUT_DIR}/preboot_p.bin of=${TARGET_OUT_DIR}/${nand_flash_fip}
			dd if=${TARGET_OUT_DIR}/ddr_init_p.bin of=${TARGET_OUT_DIR}/${nand_flash_fip} bs=1024 seek=2
			dd if=${TARGET_OUT_DIR}/uboot_p.bin of=${TARGET_OUT_DIR}/${nand_flash_fip} bs=1024 seek=64
			rm ${TARGET_OUT_DIR}/{preboot_p.bin,ddr_init_p.bin,uboot_p.bin}
			mv ${TARGET_OUT_DIR}/${nand_flash_fip} $TARGET_OUT_DIR/fip_${TARGET_CHIP}_${TARGET_BOARD}.bin
		fi
	fi

# set +x
}

################################################################################
function genimg_entry()
{
	local image=$1
	sd_up="false"

	if [ $1 == "sd_update" ] ; then
		image="emmc"
		sd_up="true"
	fi

	genimg_firmware_version_ini

	case ${image} in
	ramfs)
		#genimg_kernel_fit
		genimg_kernel_rootfs_fit #for debug on fpga
		exit
		;;
	emmc)
		#genimg_kernel_fit
		genimg_emmc_ext4
		exit
		;;
	sd)
		genimg_kernel_fit
		exit
		;;
	sflash)
		#genimg_sflash_cramfs
		genimg_sflash_squashfs
		#genimg_kernel_rootfs_fit #for debug on fpga
		exit
		;;
	nand)
		genimg_nand_ubi
		exit
		;;
	initrd)
		genimg_initrd_squashfs
		# no exit,because initrd need rebuild kernel
		# exit
		;;
	?)
		echo "???????unkown input when generate image????????"
		exit
		;;
	esac
}

