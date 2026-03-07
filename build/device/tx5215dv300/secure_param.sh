#!/bin/bash

export SECURE_BOOT_PRIV_KEY="${TARGET_BUILD_DIR}/tools/pack-util/priv.key"
export SECURE_BOOT_USER_ID="${TARGET_BUILD_DIR}/tools/pack-util/user-id.txt"
export SECURE_BOOT_USR_ROOT_KEY="00112233445566778899aabbccddeeff"
export SECURE_BOOT_IV="aabbccddeeff00112233445566778899"
export SECURE_BOOT_PUBLIC_KEY_HASH="${TARGET_BUILD_DIR}/tools/pack-util/public-key-hash.txt"
export SECURE_BOOT_IMAGE_VERSION="1"

export SECURE_BOOT_USER_ID_IS="y"

#########################
## SECURE_BOOT_MODE = 1, SECURE_BOOT_SIG_ENCRY
## SECURE_BOOT_MODE = 2, SECURE_BOOT_SIG_NOT_ENCRY
#########################
export SECURE_BOOT_MODE="1"

function gen_secure_boot_param()
{
	local priv_key=${TARGET_BUILD_DIR}/tools/pack-util/priv.key

	if [ ! -f ${priv_key} ]; then
		cd ${TARGET_BUILD_DIR}/tools/pack-util/
		openssl ecparam -genkey -name SM2 -out priv.key
		openssl ec -in priv.key -pubout -out pub.key
		echo "generate sm2 new priv and pub key."
		cd -
	fi
}

