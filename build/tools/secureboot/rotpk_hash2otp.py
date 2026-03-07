#!/usr/bin/python3

import sys

WORD_PER_LINE = 4
SHA256_LEN = 256 // 8


def usage():
    print("convert rotpk hash bin file to otp format, output to stdout")
    print("if you want save result to a file, use redirection function")
    print("eg. rotpk_hash2otp  rotpk_hash.bin > output.dat")
    print("")
    print("usage:")
    print("rotpk_hash2otp input")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        usage()
        sys.exit(1)
    fd = open(sys.argv[1], "rb")
    if not fd:
        print("open input file failed");
        sys.exit(2)

    buf = fd.read()
    fd.close()
    assert(len(buf) == SHA256_LEN)

    # print("ROTPK SHA256 TO OTP FORMAT TRANSFER:")
    i = 0
    while i < len(buf):
        tmp = buf[i : (i + WORD_PER_LINE)]
        # little endian
        print("0x", end = "");
        for j in range(WORD_PER_LINE):
            print("{:02x}".format(tmp[len(tmp) - 1 - j]), end = "");
            j += 1
        if i != len(buf) - WORD_PER_LINE:
            print(",")
        i += WORD_PER_LINE
    print(",")