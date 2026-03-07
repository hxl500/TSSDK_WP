#!/bin/bash
set -e

export prefix_path=""

if [ -d build ];then
    rm -r build
fi

mkdir build
if [ -d install ];then
    rm -r install
fi
mkdir install

if [ -f cross_file.txt ];then
    rm cross_file.txt
fi
touch cross_file.txt

prefix_path=`pwd`/install


arch=${CROSS_COMPILE}
# echo "[constants]" > cross_file.txt
# echo "arch = '${arch}'" >> cross_file.txt

echo "[binaries]" > cross_file.txt
echo "c = '${CROSS_COMPILE}gcc'"  >> cross_file.txt
echo "cpp = '${CROSS_COMPILE}g++'" >> cross_file.txt
echo "ar = '${CROSS_COMPILE}ar'" >> cross_file.txt
echo "ld = '${CROSS_COMPILE}ld'" >> cross_file.txt
echo "strip = '${CROSS_COMPILE}strip'" >> cross_file.txt
echo  >> cross_file.txt

echo "[host_machine]" >> cross_file.txt
echo "system = 'linux'" >> cross_file.txt
echo "cpu_family = 'aarch64'" >> cross_file.txt
echo "cpu = 'aarch64'" >> cross_file.txt
echo "endian = 'little'" >> cross_file.txt
echo  >> cross_file.txt

echo "[project options]" >> cross_file.txt
# echo "libkms = 'true'" >> cross_file.txt
echo "install-test-programs = 'true'" >> cross_file.txt
echo  >> cross_file.txt

