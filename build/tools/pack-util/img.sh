#!/bin/bash
dir="./img_output"
if [ ! -d "$dir" ]
then
mkdir $dir
fi

./pack-util \
  --output ${dir}/secure_ddr_init.bin \
  --private-key priv.key \
  --user-root-key 00112233445566778899aabbccddeeff \
  --iv aabbccddeeff00112233445566778899 \
  --public-key-hash public-key-hash.txt \
  --boot-image-version 10 \
  --user-id user-id.txt \
  ddr_init.bin

./pack-util \
  --output ${dir}/secure_uboot.bin \
  --private-key priv.key \
  --user-root-key 00112233445566778899aabbccddeeff \
  --iv aabbccddeeff00112233445566778899 \
  --public-key-hash public-key-hash.txt \
  --boot-image-version 10 \
  --user-id user-id.txt \
  uboot.bin

cd ${dir}
cat secure_ddr_init.bin secure_uboot.bin > secure_img_sig_encrypt.bin 
rm secure_ddr_init.bin secure_uboot.bin
echo "${dir}/creat secure_img_sig_encrypt.bin"
cd ..

./pack-util \
  --output ${dir}/secure_ddr_init.bin \
  --private-key priv.key \
  --public-key-hash public-key-hash.txt \
  --boot-image-version 10 \
  --user-id user-id.txt \
  ddr_init.bin

./pack-util \
  --output ${dir}/secure_uboot.bin \
  --private-key priv.key \
  --public-key-hash public-key-hash.txt \
  --boot-image-version 10 \
  --user-id user-id.txt \
  uboot.bin

cd ${dir}
cat secure_ddr_init.bin secure_uboot.bin > secure_img_sig.bin 
rm secure_ddr_init.bin secure_uboot.bin
echo "${dir}/creat secure_img_sig.bin"
cd ..
