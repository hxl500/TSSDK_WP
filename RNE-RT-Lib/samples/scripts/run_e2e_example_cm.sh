#!/bin/bash

# Function: Print message with different colors
# Parameter 1: Color code for the message
# Parameter 2: Message content
print_message() {
    local color=$1
    local message=$2
    echo -e "\033[1;${color}m ${message} \033[0m"
}

# Absolute path of working environment
model_input_format="int_chw"
sample_name=${2:-rne_simple_forward}
compiled_model=${1:-../compiled_model/test_gp}
model_r_cfg=$(find $compiled_model -maxdepth 1 -name "*_r.cfg")
model_r_weight=$(find $compiled_model -maxdepth 1 -name "*_r.weight")
model_input=$(find $compiled_model -maxdepth 1 \( -name "*.bin" -o -name "*.data" -o -name "*.input" \) -print -quit)
print_message "33" "model_r_cfg: $model_r_cfg"
print_message "33" "model_r_weight: $model_r_weight"
print_message "33" "model_input: $model_input"
print_message "33" "model_input_format: $model_input_format"
if [ -z "$model_r_cfg" ] || [ -z "$model_r_weight" ] || [ -z "$model_input" ]; then
    print_message "31" "==========Runtime model: ${sample_name} FAILED!=========="
    exit 1
fi

cd ../$sample_name
make clean
cmake -S . -B build_linux_a7_uclibc
if [ $result != 0 ]; then
    print_message "31" "==========Runtime model: ${sample_name} FAILED!=========="
    exit 1
fi  
cmake --build build_linux_a7_uclibc
result=$?
if [ $result != 0 ] && [ ! -f ../$sample_name/build_linux_a7_uclibc/Release/$sample_name.elf ]; then
    print_message "31" "==========Runtime model: ${sample_name} FAILED!=========="
    exit 1
fi

./../$sample_name/build_sim/Release/$sample_name.elf $model_r_cfg $model_r_weight $model_input $model_input_format
result=$?
if [ $result != 0 ]; then
    print_message "31" "==========Runtime model: ${sample_name} FAILED!=========="
    exit 1
fi

print_message "32" "==========Runtime model: ${sample_name} SUCCESS!=========="
