target_bin=dt56_nonsecure.img
target_dir=dt56_pack
loader_file="../ddr_init/ddr_init.bin"
uboot_file="../uboot/u-boot.bin"

mkdir -p ${target_dir}

if [ -f "$loader_file" ]; then
	cp $loader_file ${target_dir}
	echo "copy bootloader"
else
	echo "bootloader not found"
	exit 1
fi

if [ -f "$uboot_file" ]; then
	cp $uboot_file ${target_dir}
	echo "copy uboot"
else
	echo "uboot not found"
	exit 1
fi

./pack-util -o ${target_dir}/none_secure_ddr_init.bin ${target_dir}/ddr_init.bin
./pack-util -o ${target_dir}/none_secure_uboot.bin ${target_dir}/u-boot.bin

dd if=${target_dir}/none_secure_ddr_init.bin of=${target_dir}/${target_bin} bs=512 seek=0
dd if=${target_dir}/none_secure_uboot.bin of=${target_dir}/${target_bin} bs=512 seek=128

