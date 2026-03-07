#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alphabet.h"
#include "soft_alphabet_overlay.h"

typedef struct
{
   char Y;
   char U;
   char V;
} YuvColor;

typedef struct {
	int width;
	int height;
	unsigned char *LetterArray[ALPHABET_NUM]
} YuvFont;

static YuvColor s_color_table[12] =
{
 {0x00, 0x00, 0x00}, // green
 {0x00, 0x00, 0xff}, // red
 {0x00, 0xff, 0x00}, // blue
 {0x00, 0xff, 0xff}, // purple
 {0xff, 0x00, 0x00}, // dark green
 {0xff, 0x00, 0xff}, // yellow
 {0xff, 0xff, 0x00}, // light blue
 {0xff, 0xff, 0xff}, // light purple
 {0x00, 0x80, 0x80}, // dark black
 {0x80, 0x80, 0x80}, // gray
 {0xff, 0x80, 0x80}, // white
};

#define 	FONT_COUNT			(3)
YuvFont s_font_table[FONT_COUNT] = {
	{20, 32, NULL}, 
	{18, 28, NULL}, 
	{14, 22, NULL}
};


static char *alphabet = { "abcdefghijklmnopqrstuvwxyz0123456789-:." };
static int overlay_digital(unsigned char *yuvOut, const unsigned char *osd, int osdW, int osdH, int yuvW, int yuvH,
			   int x, int y, int color)
{
	int i = 0, j = 0;
	int ret = 0;
	if (x > yuvW || y > yuvH || x + osdW > yuvW || y + osdH > yuvH || color < 0 || color >= sizeof(s_color_table) / sizeof(YuvColor)) {
		printf("pram err: x=%d, y=%d, owsW=%d, osdH=%d, yuvW=%d, yuvH=%d.\n", x, y, osdW, osdH, yuvW, yuvH);
		return 0;
	}
	for (i = 0; i < osdH; i++) {
		for (j = 0; j < osdW; j++) {
			unsigned char osddata = osd[osdW * i + j];
			if (osddata != 0x00) {
				yuvOut[yuvW * (i + y) + j + x] = s_color_table[color].Y;
			}
		}
	}
	yuvOut += yuvW * yuvH;
	for (i = 0; i < osdH / 2; i++) {
		for (j = 0; j < osdW / 2; j++) {
			unsigned char osddata = osd[osdW * i * 2 + j * 2];
			if (osddata != 0x00) {
				yuvOut[yuvW * (i + y / 2) + x + j * 2] = s_color_table[color].U; //u
				yuvOut[yuvW * (i + y / 2) + x + j * 2 + 1] = s_color_table[color].V; //v
			}
		}
	}
	return ret;
}

/**
 * 使用最近邻插值法缩小灰度图像
 *
 * @param src_img     指向原始灰度图像数据的指针 (二维数组展平为一维)
 * @param src_width   原始图像宽度
 * @param src_height  原始图像高度
 * @param dst_img     指向目标（缩小后）灰度图像数据的指针，需预先分配内存
 * @param dst_width   目标图像宽度
 * @param dst_height  目标图像高度
 */
void resize_grayscale_nearest_neighbor(const unsigned char* src_img,
                                       int src_width, int src_height,
                                       unsigned char* dst_img,
                                       int dst_width, int dst_height) {
    // 计算缩放比例
    double x_ratio = (double)src_width / dst_width;
    double y_ratio = (double)src_height / dst_height;

    // 遍历目标图像的每一个像素
    for (int y = 0; y < dst_height; y++) {
        for (int x = 0; x < dst_width; x++) {
            // 找到最近邻的源图像坐标
            int src_x = (int)(x * x_ratio);
            int src_y = (int)(y * y_ratio);

            // 确保不会越界
            if (src_x >= src_width) src_x = src_width - 1;
            if (src_y >= src_height) src_y = src_height - 1;

            // 将源图像中的像素值复制到目标图像
            dst_img[y * dst_width + x] = src_img[src_y * src_width + src_x];
        }
    }
}

/**
* 使用双线性插值法缩小灰度图像
*
* @param src_img     指向原始灰度图像数据的指针 (二维数组展平为一维)
* @param src_width   原始图像宽度
* @param src_height  原始图像高度
* @param dst_img     指向目标（缩小后）灰度图像数据的指针，需预先分配内存
* @param dst_width   目标图像宽度
* @param dst_height  目标图像高度
*/
void resize_grayscale_bilinear(const unsigned char* src_img,
	int src_width, int src_height,
	unsigned char* dst_img,
	int dst_width, int dst_height) {

	// 遍历目标图像的每一个像素
	for (int new_y = 0; new_y < dst_height; new_y++) {
		for (int new_x = 0; new_x < dst_width; new_x++) {

			// 将目标图像的坐标映射回原图像的浮点坐标
			// 使用"区域重采样"方法，确保覆盖整个像素区域
			double src_x = (new_x + 0.5) * ((double)src_width / dst_width) - 0.5;
			double src_y = (new_y + 0.5) * ((double)src_height / dst_height) - 0.5;

			// 限制坐标范围，防止越界
			if (src_x < 0) src_x = 0;
			if (src_y < 0) src_y = 0;

			// 找到包围 (src_x, src_y) 的四个整数坐标点
			int x0 = (int)src_x; // 左侧
			int y0 = (int)src_y; // 上侧
			int x1 = x0 + 1;     // 右侧
			int y1 = y0 + 1;     // 下侧

			// 再次检查边界，防止 x1 或 y1 越界
			if (x1 >= src_width)  x1 = src_width - 1;
			if (y1 >= src_height) y1 = src_height - 1;

			// 获取四个邻近点的像素值
			unsigned char p00 = src_img[y0 * src_width + x0]; // 左上
			unsigned char p10 = src_img[y0 * src_width + x1]; // 右上
			unsigned char p01 = src_img[y1 * src_width + x0]; // 左下
			unsigned char p11 = src_img[y1 * src_width + x1]; // 右下

			// 计算在 x 和 y 方向上的权重 (距离)
			double dx = src_x - x0;
			double dy = src_y - y0;

			// 双线性插值计算
			// 先在 x 方向插值
			double interpolated_top = p00 * (1 - dx) + p10 * dx;
			double interpolated_bottom = p01 * (1 - dx) + p11 * dx;
			// 再在 y 方向插值
			double interpolated_value = interpolated_top * (1 - dy) + interpolated_bottom * dy;

			// 四舍五入并裁剪到 [0, 255] 范围
			unsigned char pixel_value = (unsigned char)(interpolated_value + 0.5);
			if (pixel_value > 255) pixel_value = 255;

			// 将计算出的像素值赋给目标图像
			dst_img[new_y * dst_width + new_x] = pixel_value;
		}
	}
}

int overlay_letter(char *name, unsigned char *yuvOut, int yuvW, int yuvH, int x, int y, int color, int font)
{
	//int units_digit = 0;
	int temp_x = (x / 2) * 2 + 8; //x need devided by 2
	int alphabet_index = 0;
	int name_index = 0;
	int units_letter_num = strlen(name);
	//printf("letter len :%d\n", units_letter_num);
	int postion = temp_x;
	int ret = 0;
	const unsigned char * pOsdData = NULL;
	int osd_width = 0;
	int osd_hight = 0;
	for (name_index = 0; name_index < units_letter_num; name_index++) {
		for (alphabet_index = 0; alphabet_index < ALPHABET_NUM; alphabet_index++) {
			if (name[name_index] == alphabet[alphabet_index]) {
				if (font >= 0 && font < FONT_COUNT) {
					if (s_font_table[font].LetterArray[alphabet_index] == NULL) {
						s_font_table[font].LetterArray[alphabet_index] = (unsigned char*)malloc(s_font_table[font].width * s_font_table[font].height);
						resize_grayscale_bilinear(LetterArray[alphabet_index], digital_width, digital_height, s_font_table[font].LetterArray[alphabet_index], 
							s_font_table[font].width, s_font_table[font].height);
					}
					pOsdData = s_font_table[font].LetterArray[alphabet_index];
					osd_width = s_font_table[font].width;
					osd_hight = s_font_table[font].height;
				} else {
					pOsdData = LetterArray[alphabet_index];
					osd_width = digital_width;
					osd_hight = digital_height;
				}
				overlay_digital(yuvOut, pOsdData, osd_width, osd_hight, yuvW, yuvH, postion, y + 8, color);
				postion += letter_position_width;
				break;
			}
		}
	}
	return ret;
}