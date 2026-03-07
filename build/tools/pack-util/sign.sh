#!/bin/bash

if [ ! $# -eq 1 ]; then
    echo 'usage: sign.sh input-file'
    exit 1
fi

boot_img=$1
boot_img_version=0x10
flash_img=flash-secure
flash_img_none_secure=flash-none-secure
pubkey_hash=pubkey-hash.txt

function sign()
{
    echo "generate secure flash image $flash_img.img and $flash_img.pat"

    if [ -z "$user_id" ]; then
        ./pack-util $verbose --output $flash_img.img --private-key priv.key --user-root-key $otp_key \
        --soc-vendor-key $fls_key --iv $fls_iv --public-key-hash $pubkey_hash \
        --boot-image-version $boot_img_version $boot_img
    else
        ./pack-util $verbose --output $flash_img.img --private-key priv.key --user-root-key $otp_key \
        --soc-vendor-key $fls_key --iv $fls_iv --user-id $user_id --public-key-hash $pubkey_hash \
        --boot-image-version $boot_img_version $boot_img
    fi
    ./bin2pat.py 4 $flash_img.img > $flash_img.pat

    ./pack-util $verbose --verify --user-root-key $otp_key --public-key-hash $pubkey_hash $flash_img.img 1>/dev/null

    echo "generate none secure flash image $flash_img_none_secure.img and $flash_img_none_secure.pat"
    ./pack-util $verbose -o $flash_img_none_secure.img $boot_img
    ./bin2pat.py 4 $flash_img_none_secure.img > $flash_img_none_secure.pat
    ./gen-sd-image $flash_img.img $flash_img.sd
    ./bin2pat.py 512 $flash_img.sd > $flash_img.sd.pat
    ./gen-sd-image $flash_img_none_secure.img $flash_img_none_secure.sd
    ./bin2pat.py 512 $flash_img_none_secure.sd > $flash_img_none_secure.sd.pat
}

function gen_ec_key()
{
    openssl ecparam -genkey -name SM2 -out priv.key 2>/dev/null
    openssl ec -in priv.key -pubout -out pub.key 2>/dev/null
}

gen_ec_key

otp_key='000102030405060708090a0b0c0d0e0f'
fls_key='abcdef0123456789abcdef0123456789'
fls_iv='9876543210abcdef9876543210abcdef'
user_id=user-id.txt
echo -n 'HelloWorld' > $user_id

sign

