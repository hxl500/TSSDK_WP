#!/bin/sh

loader_file="../ddr_init/ddr_init.bin"
if [ -f "$loader_file" ]; then
	cp $loader_file .
	echo "copy loader here"
fi

#user_root_key=178899644653aabbccdd112233445566
user_root_key=00112233445566778899aabbccddeeff

mkdir -p secure_boot_test_bin

./pack-util -o none_secure_ddr_init.bin ddr_init.bin
mv none_secure_ddr_init.bin ./secure_boot_test_bin

./pack-util \
	--output secure_ddr_init_encrypt_sig_user_id.bin \
	--private-key priv.key \
	--user-root-key ${user_root_key} \
	--iv aabbccddeeff00112233445566778899 \
	--public-key-hash public-key-hash.txt \
	--boot-image-version 10 \
	--user-id user-id.txt \
	ddr_init.bin
mv secure_ddr_init_encrypt_sig_user_id.bin ./secure_boot_test_bin

./pack-util \
	--output secure_ddr_init_encrypt_sig.bin \
	--private-key priv.key \
	--user-root-key ${user_root_key} \
	--iv aabbccddeeff00112233445566778899 \
	--public-key-hash public-key-hash.txt \
	--boot-image-version 10 \
	ddr_init.bin
mv secure_ddr_init_encrypt_sig.bin ./secure_boot_test_bin

./pack-util \
	--output secure_ddr_init_sig_user_id.bin \
	--private-key priv.key \
	--public-key-hash public-hash.txt \
	--boot-image-version 10 \
	--user-id user-id.txt \
	ddr_init.bin
mv secure_ddr_init_sig_user_id.bin ./secure_boot_test_bin

./pack-util \
	--output secure_ddr_init_sig.bin \
	--private-key priv.key \
	--public-key-hash public-hash.txt \
	--boot-image-version 10 \
	ddr_init.bin
mv secure_ddr_init_sig.bin ./secure_boot_test_bin

program_dir="../bootloader_program"
if [ -d "$program_dir" ]; then
	install public-key-hash.txt $program_dir
	echo "move public-key-hash.txt to programmer"
fi

mv public-key-hash.txt ./secure_boot_test_bin
mv public-hash.txt ./secure_boot_test_bin

