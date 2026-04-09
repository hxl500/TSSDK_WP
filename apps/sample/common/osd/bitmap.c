/********************************************************************************
 *************************Copyright (C), 2023, ScienTech Inc.**************************
 ********************************************************************************
 * @file     	 : bitmap.c
 * @brief   	 : .C file function description
 * @author       : Tsing Micro
 * @version      : 1.0
 * @date         : 2023-03-09
********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitmap.h"

int bitmapSaveArgb1555Bmp(char* name, unsigned char *bitmap,int width,int height)
{
	int bmpsize;
	int bitcount;
	bmpsize = width *height *2; // 每个像素点2字节
	bitcount = 16;

	// 位图第一部分，文件信息
	BMPFILEHEADER_T bfh;
	bfh.bfType = 0x4d42;  //bm
	bfh.bfSize = bmpsize  // data size
		+ sizeof( BMPFILEHEADER_T ) // first section size
		+ sizeof( BMPINFOHEADER_T ) // second section size
		;
	bfh.bfReserved1 = 0; // reserved
	bfh.bfReserved2 = 0; // reserved
	bfh.bfOffBits = bfh.bfSize - bmpsize;
	// 位图第二部分，数据信息
	BMPINFOHEADER_T bih;
	bih.bInfoSize = sizeof(BMPINFOHEADER_T);
	bih.bWidth = width;
	bih.bHeight = height;
	bih.bPlanes = 1;
	bih.bBitCount = bitcount;
	bih.bCompression = 0;
	bih.bmpImageSize = bmpsize;
	bih.bXPelsPerMeter = 0;
	bih.bYPelsPerMeter = 0;
	bih.bClrUsed = 0;
	bih.bClrImportant = 0;

	FILE * fp = fopen(name,"wb");
	if( !fp ) return -1;

	fwrite( &bfh, 1, sizeof(BMPFILEHEADER_T), fp );
	fwrite( &bih, 1, sizeof(BMPINFOHEADER_T), fp );
	fwrite( bitmap, 1, bmpsize, fp );
	fclose( fp );

	return 0;
}


int bitmapToArgb1555(unsigned char *dst, int dstLength, unsigned char *bgrbuf,int width,int height,bool b_change)
{
    if(dstLength < width * height * 2 )
    {
        printf("dst buffer is too small!");
        return -1;
    }
    int i,j;
#if 0
    j = height-1;
    unsigned int count = 0;

    for (; j != -1; j += -1) {
        for (i=0; i < width; ++i) {
            unsigned char *d = (unsigned char *)bgrbuf + (j*width+i);
            if(d[0])
            {
                unsigned int a = 1;
                unsigned int r = (d[0]*31 +127)/255;
                unsigned int g = (d[0]*31 +127)/255;
                unsigned int b = (d[0]*31 +127)/255;
                unsigned short argb1555 = (a << 15) | (r <<  10) | (g << 5) | b;
                dst[count*2+0] = argb1555 &0xff;
                dst[count*2+1] =  (argb1555 >>8)&0xff;;
            }
            else
            {
                dst[count*2+0] = d[0];
                dst[count*2+1] = d[0];
            }
            count++;
        }
    }
#else
    unsigned int count = 0;
	if(b_change) {
		for (j=0 ; j < height; j ++) {
	        for (i=0; i < width; ++i) {
	            unsigned char *d = (unsigned char *)bgrbuf + (j*width+i);
	            if(d[0])
	            {

	                dst[count*2+0] = 0xff;
	                dst[count*2+1] = 0xff;;
	            }
	            else
	            {
	                dst[count*2+0] = 0;//d[0];
	                dst[count*2+1] = 0;//d[0];
	            }
	            count++;
	        }
	    }
	}else {
		for (j=0 ; j < height; j ++) {
	        for (i=0; i < width; ++i) {
	            unsigned char *d = (unsigned char *)bgrbuf + (j*width+i);
	            if(d[0])
	            {

	                dst[count*2+0] = 0x00;
	                dst[count*2+1] = 0x80;;
	            }
	            else
	            {
	                dst[count*2+0] = 0;//d[0];
	                dst[count*2+1] = 0;//d[0];
	            }
	            count++;
	        }
	    }
	}
    
#endif
    return 0;
}

int bitmapToArgb2bit(unsigned char *dst, int dstLength, unsigned char *bgrbuf,int width,int height)
{
	if(dstLength < (width * height) / 4 )
	{
		printf("dst buffer is too small!");
		return -1;
	}
	
	
	memset(dst,0,dstLength);
	
	for(int i =0; i<width *height;++i)
	{
		if(0 == bgrbuf[i])
		{
			//dst[i / 4] &= ~(1 << (i % 4));
		}
		else
		{
			dst[i / 4] |= (1 << ( (i % 4) * 2));

			dst[i / 4] |= (1 << ( (i % 4) * 2 +1));
		
		}	
	}
	
	return 0;
}


