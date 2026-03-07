
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "ts_type.h"

#define PATH_OCOTP "/sys/bus/nvmem/devices/ts-ocotp0/nvmem"

#define OCOTP_MAX_WORD_SIZE  32
#define BYTE_OFFSET_OF_PID   17

#define READ_BYTES  (OCOTP_MAX_WORD_SIZE * 4)

TS_S32 efuse_read(void) 
{
    TS_S32 efuse_fd;
    TS_CHAR ocotp_buff[READ_BYTES];

    efuse_fd = open(PATH_OCOTP, O_RDONLY);
    if (efuse_fd < 0) {
        perror("Failed to open OTPC sysfs file");
        return -1;
    }

    memset(ocotp_buff, 0, sizeof(ocotp_buff));

    if (read(efuse_fd, ocotp_buff, sizeof(ocotp_buff)) < 0) {
        perror("Failed to read OTPC data");
        close(efuse_fd);
        return -1;
    }

    printf("OTPC data (hex):\n");
    for (int i = 0; i < READ_BYTES; i += 16) {
        printf("0x%08x  ", i);
        for (int j = 0; j < 16; j++) {
            if (i + j < READ_BYTES) {
                printf("%02x ", (unsigned char)ocotp_buff[i + j]);
            } else {
                printf("   ");
            }
        }
        printf("\n");
    }

    close(efuse_fd);
    return 0;
}

//Caution with efuse_write operations!!!
TS_S32 efuse_write(TS_S32 value) 
{
    TS_S32 efuse_fd;

    efuse_fd = open(PATH_OCOTP, O_WRONLY);
    if (efuse_fd < 0) {
        perror("Failed to open OTPC sysfs file");
        return -1;
    }

    if (write(efuse_fd, &value, sizeof(value)) < 0) {
        perror("Failed to write OTPC data");
        close(efuse_fd);
        return -1;
    }
    printf("Write value 0x%x", value);
    close(efuse_fd);
    return 0;
}

TS_S32 main(TS_S32 argc, TS_CHAR *argv[]) 
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <read|write>\n", argv[0]);
        return -1;
    }

    if (strcmp(argv[1], "read") == 0) {
        efuse_read();
    } 
    else {
        fprintf(stderr, "Invalid operation. Use 'read' or 'write'.\n");
        return -1;
    }

    return 0;
}

