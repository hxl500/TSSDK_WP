#!/bin/bash

openssl genrsa -out rot_key.pem 2048
openssl rsa -in rot_key.pem -pubout -outform DER -out rot_public_key.bin
openssl dgst -sha256 -binary -out rotpk_sha256.bin rot_public_key.bin
python3 rotpk_hash2otp.py rotpk_sha256.bin > rotpk_sha256otp.dat
rm rot_public_key.bin