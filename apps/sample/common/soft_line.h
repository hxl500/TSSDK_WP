#ifndef _SOFT_LINE_H
#define _SOFT_LINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define _SOFT_LINE(fmt...)   \
	do {\
		printf("[%s]-%d: ", __func__, __LINE__);\
		printf(fmt);\
	} while (0)

typedef struct{
   int left;
   int top;
   int right;
   int bottom;
}RECT;

typedef enum
{
	YUV_GREEN,
	YUV_RED,
	YUV_BLUE,
	YUV_PURPLE,
	YUV_DARK_GREEN,
	YUV_YELLOW,
	YUV_LIGHT_BLUE,
	YUV_LIGHT_PURPLE,
	YUV_DARK_BLACK,
	YUV_GRAY,
	YUV_WHITE,
	YUV_COLOR_MAX,
}enYuvColorIdx;

int YUY_Draw_Piont(unsigned char * YuvBuf, int ImgW, int ImgH, int fmt,int PntX, int PntY, int Color);

int YUY_Draw_Line(unsigned char * YuvBuf, int ImgW, int ImgH, int fmt,int SPntX, int SPntY,int EPntX, int EPntY, int Color,int lineWidth);
int YUV_Draw_Rect_C(unsigned char *YuvBuf, int ImgW, int ImgH, int fmt,RECT* Rect, int Color,int lineWidth);

int YUY_Draw_num(unsigned char * YuvBuf, int ImgW, int ImgH, int fmt,int SPntX, int SPntY,int id, int Color);
int get_time_ms(char *buff, int len);

int YUV_Draw_Rect(unsigned char *YuvBuf, int ImgW, int ImgH, int fmt, RECT* Rect, int Color, int lineWidth);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif

