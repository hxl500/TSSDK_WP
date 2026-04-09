#ifndef NV12_CALCBRIGHT_H
#define NV12_CALCBRIGHT_H

#include <stdio.h>
#include <stdlib.h>
/**
 * @brief calculate_nv12_brightness 计算一个图特定区域的平均亮度值
 * @param nv12_data nv12图的数据指针
 * @param width 图的宽度
 * @param height 图的高度
 * @param start_x   区域的左上角坐标
 * @param start_y   区域的右上角坐标
 * @param region_width  区域宽度
 * @param region_height 区域高度
 * @return 返回亮度的平均值
 */
float calculate_nv12_brightness(unsigned char *nv12_data, int width, int height, int start_x, int start_y, int region_width, int region_height) {
    if (nv12_data == NULL || width == 0 || height == 0) {
        fprintf(stderr, "Invalid input: nv12_data is NULL or width or height is 0.\n");
        return -1;
    }
    // 确保区域在图像范围内
    if (start_x < 0 || start_y < 0 || start_x + region_width > width || start_y + region_height > height) {
        fprintf(stderr, "Error: Region out of bounds.\n");
        return -1;
    }

    float total_brightness = 0.0f;
    //int y_plane_size = width * height;

    // 计算指定区域的 Y 值总和
    for (int y = start_y; y < start_y + region_height; y++) {
        for (int x = start_x; x < start_x + region_width; x++) {
            int index = y * width + x; // 计算 Y 平面中的索引
            total_brightness += nv12_data[index]; // Y 值就是亮度
        }
    }

    // 计算区域内的像素数量
    int pixel_count = region_width * region_height;

    // 计算平均亮度
    float average_brightness = total_brightness / pixel_count;
    return average_brightness;
}

// 测试用例
int test_bright() {
    const char *filename = "720.raw"; // 替换为你的 NV12 图像路径
    int width = 1280; // 图像宽度
    int height = 720; // 图像高度

    // 打开文件并读取数据
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return -1;
    }

    unsigned char *nv12_data = (unsigned char *)malloc(width * height * 3 / 2); // NV12 大小
    fread(nv12_data, 1, width * height * 3 / 2, file);
    fclose(file);

    // 指定要计算的区域
    int start_x = 100; // 区域左上角 X 坐标
    int start_y = 100; // 区域左上角 Y 坐标
    int region_width = 200; // 区域宽度
    int region_height = 150; // 区域高度

    // 计算区域的亮度
    float average_brightness = calculate_nv12_brightness(nv12_data, width, height, start_x, start_y, region_width, region_height);
    if (average_brightness >= 0) {
        printf("Average Brightness of the specified region: %d\n", (int)average_brightness);
    }

    // 释放内存
    free(nv12_data);
    return 0;
}

#endif // NV12_CALCBRIGHT_H
