#!/bin/bash
boot_img=boot.img
boot_img_size=1024
boot_img_version=0x10
flash_img=flash.img
pubkey_hash=pubkey-hash.txt

round=100

# verbose=--verbose


function verify()
{
    if [ ! -z "$verbose" ]; then
        echo "generate flash image $flash_img"
        echo
        echo
        echo '---------------------------------------'
    fi

    if [ -z "$user_id" ]; then
        ./pack-util $verbose --output $flash_img --private-key priv.key --user-root-key $otp_key \
        --soc-vendor-key $fls_key --iv $fls_iv --public-key-hash $pubkey_hash \
        --boot-image-version $boot_img_version $boot_img
    else
        ./pack-util $verbose --output $flash_img --private-key priv.key --user-root-key $otp_key \
        --soc-vendor-key $fls_key --iv $fls_iv --user-id $user_id --public-key-hash $pubkey_hash \
        --boot-image-version $boot_img_version $boot_img
    fi

    if [ ! -z "$verbose" ]; then
        echo '---------------------------------------'
        echo
        echo
        echo '---------------------------------------'
        echo "verify flash image $flash_img"
        echo '---------------------------------------'
    fi

    ./pack-util $verbose --verify --user-root-key $otp_key --public-key-hash $pubkey_hash $flash_img 1>/dev/null

    if [ $? -eq 0 ]; then
        echo 'verify success'
    else
        echo 'verify failed'
        exit 1
    fi
}

function clean()
{
    rm -rf *.key
    rm -rf *.img
    rm -rf *.bin
    rm -rf *.txt
}

function gen_ec_key()
{
    openssl ecparam -genkey -name SM2 -out priv.key 2>/dev/null
    openssl ec -in priv.key -pubout -out pub.key 2>/dev/null
}

clean
gen_ec_key

otp_key='12345678876543211234567887654321'
fls_key='abcdef0123456789abcdef0123456789'
fls_iv='9876543210abcdef9876543210abcdef'
user_id=user-id.txt
echo -n 'HelloWorld' > $user_id
dd if=/dev/zero of=$boot_img bs=1 count=$boot_img_size 2>/dev/null

set -e

# quick verify
verify
# default user id
user_id=
verify

function random_oct()
{
    local oct_len=$1
    dd if=/dev/urandom bs=1 count=$oct_len 2>/dev/null | xxd -p
}

function random_file()
{
    echo $1 $2
    local bin_file=$1
    local bin_len=$2
    dd if=/dev/urandom of=$bin_file bs=1 count=$bin_len 2>/dev/null
}

function random_verify()
{
    otp_key=$(random_oct 16)
    fls_key=$(random_oct 16)
    fls_iv=$(random_oct 16)

    user_id_len=$(( $RANDOM % 8191 ))
    if [ $user_id_len -eq 0 ]; then
        # default user id
        user_id=
    else
        user_id=user-id.txt
    fi
    random_file $user_id $user_id_len
    boot_img_size=$(( $RANDOM % (64 * 1024) ))
    random_file $boot_img $boot_img_size

    echo "OTP key: $otp_key"
    echo "Flash key: $fls_key"
    echo "Flash IV: $fls_iv"
    echo "User ID length: $user_id_len"
    if [ ! $user_id_len -eq 0 ]; then
        echo -n "User ID: "
        xxd -p -l 16 $user_id
    fi
    
    verify
    return $?
}

for i in $(seq $round); do
    echo "$i-th round"
    echo '------------------------------------------'
    clean
    gen_ec_key
    random_verify
    if [ $? != 0 ]; then
        exit 1
    fi
done

