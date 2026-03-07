/********************************************************************************
 *************************Copyright (C), 2023, ScienTech Inc.**************************
 ********************************************************************************
 * @file     	 : bitmap.h
 * @brief   	 : .C file function description
 * @author       : Tsing Micro
 * @version      : 1.0
 * @date         : 2023-03-09
********************************************************************************/
#ifndef _BITMAP_H_
#define _BITMAP_H_

#ifdef __cplusplus
#if __cplusplus
				extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdbool.h>

#pragma pack(1)
/*************************** 位图标准信息(54字节) ******************************************/
/* 位图文件头 （位图头的字节数 = 位图文件字节数 - 位图图像数据字节数）*/
typedef struct {
	unsigned short 	bfType; 	  //位图文件的类型，必须为BM
	unsigned int	bfSize; 	  //文件大小，以字节为单位
	unsigned short 	bfReserved1; //位图文件保留字，必须为0
	unsigned short 	bfReserved2; //位图文件保留字，必须为0
	unsigned int	bfOffBits;  //位图文件头到数据的偏移量，以字节为单位
} BMPFILEHEADER_T;
/* 位图信息头 */

typedef struct BMP_INFO
{
	unsigned int bInfoSize;            // 信息头的大小
	unsigned int bWidth;               // 图像的宽度
	unsigned int bHeight;              // 图像的高度
	unsigned short bPlanes;               // 图像的位面数
	unsigned short bBitCount;             // 每个像素的位数
	unsigned int bCompression;         // 压缩类型
	unsigned int bmpImageSize;         // 图像的大小,以字节为单位
	unsigned int bXPelsPerMeter;       // 水平分辨率
	unsigned int bYPelsPerMeter;       // 垂直分辨率
	unsigned int bClrUsed;             // 使用的色彩数
	unsigned int bClrImportant;        // 重要的颜色数
} BMPINFOHEADER_T;            // 40 字节

/* 彩色表:调色板 */
typedef struct{
	unsigned short b:5;
	unsigned short g:5;
	unsigned short r:5;
	unsigned short a:1;
}RGB1555_T;
#pragma pack()

typedef struct{
	int width;
	int height;
	char* pRGB;
}BITMAP_INFO_T;

int bitmapSaveArgb1555Bmp(char* name, unsigned char *bitmap,int width,int height);

int bitmapToArgb1555(unsigned char *dst, int dstLength, unsigned char *bgrbuf,int width,int height,bool b_change);

int bitmapToArgb2bit(unsigned char *dst, int dstLength, unsigned char *bgrbuf,int width,int height);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
