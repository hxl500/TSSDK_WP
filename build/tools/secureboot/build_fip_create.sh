#!/bin/bash

bl2=./bl2.bin
bl31=./bl31.bin
uboot=./u-boot.bin
fip=./fip.bin

./cert_create/cert_create -n --tfw-nvctr 0 --ntfw-nvctr 0 --trusted-key-cert ./trusted_key.crt \
	--key-alg rsa --key-size 2048 --hash-alg sha256 --rot-key rot_key.pem \
	--tb-fw-cert ./tb_fw.crt --soc-fw-cert ./soc_fw_content.crt \
	--soc-fw-key-cert ./soc_fw_key.crt --nt-fw-cert ./nt_fw_content.crt \
	--nt-fw-key-cert ./nt_fw_key.crt --tb-fw ${bl2} --soc-fw ${bl31} --nt-fw ${uboot}

../atf/fiptool create --trusted-key-cert ./trusted_key.crt --tb-fw-cert ./tb_fw.crt \
	--soc-fw-cert ./soc_fw_content.crt --soc-fw-key-cert ./soc_fw_key.crt \
	--nt-fw-cert ./nt_fw_content.crt --nt-fw-key-cert ./nt_fw_key.crt \
	--tb-fw ${bl2} --soc-fw ${bl31} --nt-fw ${uboot} ${fip}