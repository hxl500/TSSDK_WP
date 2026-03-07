
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "ts_type.h"

#define TEMP_FILE_4 "/sys/bus/iio/devices/iio:device0/in_temp4_input"
#define TEMP_FILE_5 "/sys/bus/iio/devices/iio:device0/in_temp5_input"
#define TEMP_FILE_6 "/sys/bus/iio/devices/iio:device0/in_temp6_input"

TS_S32 read_temperature(const TS_CHAR *file_path, TS_DOUBLE *temp_in_celsius) {
    TS_S32 t_sensor_fd;
    TS_CHAR buffer[64];
    TS_S32 temperature;

    t_sensor_fd = open(file_path, O_RDONLY);
    if (t_sensor_fd < 0) {
        perror("Failed to open t_sensor file");
        return -1;
    }

    if (read(t_sensor_fd, buffer, sizeof(buffer)) < 0) {
        perror("Failed to read t_sensor value");
        close(t_sensor_fd);
        return -1;
    }

    temperature = atoi(buffer);
    *temp_in_celsius = temperature / 1000.0;

    close(t_sensor_fd);
    return 0;
}

TS_S32 main() {
    double temp4_in_celsius, temp5_in_celsius, temp6_in_celsius;

    if (read_temperature(TEMP_FILE_4, &temp4_in_celsius) == 0) {
        printf("Current temperature (in_temp4): %.2f °C\n", temp4_in_celsius);
    }

    if (read_temperature(TEMP_FILE_5, &temp5_in_celsius) == 0) {
        printf("Current temperature (in_temp5): %.2f °C\n", temp5_in_celsius);
    }

    if (read_temperature(TEMP_FILE_6, &temp6_in_celsius) == 0) {
        printf("Current temperature (in_temp6): %.2f °C\n", temp6_in_celsius);
    }

    return 0;
}

