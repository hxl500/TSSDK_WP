#!/usr/bin/python3

import sys

def usage():
    print("convert binary file to given pad file, output to stdout")
    print("if you want save result to a file, use redirection function")
    print("eg. bin2pad 4 input.bin > output.pad")
    print()
    print("usage:")
    print("bin2pad block-size input")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        usage()
        sys.exit(1)
    fd = open(sys.argv[2], "rb")
    if not fd:
        print("open input file failed");
        sys.exit(2)

    block_size = int(sys.argv[1])
    buf = fd.read()
    if (len(buf) % block_size != 0):
        pad_size = block_size - (len(buf) % block_size)
        pad_array = bytearray(pad_size)
        buf += pad_array

    assert(len(buf) % block_size == 0)

    i = 0
    while i < len(buf):
        tmp = buf[i : (i + block_size)]
        # little endian
        print("@{:08x}  ".format(i // block_size), end = "")
        for j in range(block_size):
            print("{:02x}".format(tmp[len(tmp) - 1 - j]), end = "");
            j += 1
        if i != len(buf) - block_size:
            print()
        i += block_size
    
    fd.close()
