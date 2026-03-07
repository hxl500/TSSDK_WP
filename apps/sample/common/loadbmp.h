#ifndef __LOAD_BMP_H__
#define __LOAD_BMP_H__

#include "ts_type.h"

/* the color format OSD supported */
typedef enum tsOSD_COLOR_FMT_E {
	OSD_COLOR_FMT_RGB444    = 0,
	OSD_COLOR_FMT_RGB4444   = 1,
	OSD_COLOR_FMT_RGB555    = 2,
	OSD_COLOR_FMT_RGB565    = 3,
	OSD_COLOR_FMT_RGB1555   = 4,
	OSD_COLOR_FMT_RGB888    = 6,
	OSD_COLOR_FMT_RGB8888   = 7,
	OSD_COLOR_FMT_BUTT
} OSD_COLOR_FMT_E;

typedef struct tsOSD_RGB_S {
	TS_U8   u8B;
	TS_U8   u8G;
	TS_U8   u8R;
	TS_U8   u8Reserved;
} OSD_RGB_S;

typedef struct tsOSD_SURFACE_S {
	OSD_COLOR_FMT_E enColorFmt;         /* color format */
	TS_U8  *pu8PhyAddr;               /* physical address */
	TS_U16  u16Height;                /* operation height */
	TS_U16  u16Width;                 /* operation width */
	TS_U16  u16Stride;                /* surface stride */
	TS_U16  u16Reserved;
} OSD_SURFACE_S;

typedef struct tag_OSD_Logo {
	TS_U32    width;        /* out */
	TS_U32    height;       /* out */
	TS_U32    stride;       /* in */
	TS_U8    *pRGBBuffer;   /* in/out */
} OSD_LOGO_T;

typedef struct tag_OSD_BITMAPINFOHEADER {
	TS_U16      biSize;
	TS_U32       biWidth;
	TS_S32       biHeight;
	TS_U16       biPlanes;
	TS_U16       biBitCount;
	TS_U32      biCompression;
	TS_U32      biSizeImage;
	TS_U32       biXPelsPerMeter;
	TS_U32       biYPelsPerMeter;
	TS_U32      biClrUsed;
	TS_U32      biClrImportant;
} OSD_BITMAPINFOHEADER;

typedef struct tag_OSD_BITMAPFILEHEADER {
	TS_U32   bfSize;
	TS_U16    bfReserved1;
	TS_U16    bfReserved2;
	TS_U32   bfOffBits;
} OSD_BITMAPFILEHEADER;

typedef struct tag_OSD_RGBQUAD {
	TS_U8    rgbBlue;
	TS_U8    rgbGreen;
	TS_U8    rgbRed;
	TS_U8    rgbReserved;
} OSD_RGBQUAD;

typedef struct tag_OSD_BITMAPINFO {
	OSD_BITMAPINFOHEADER    bmiHeader;
	OSD_RGBQUAD                 bmiColors[1];
} OSD_BITMAPINFO;

typedef struct tsOSD_COMPONENT_INFO_S {
	int alen;
	int rlen;
	int glen;
	int blen;
} OSD_COMP_INFO;

TS_S32 LoadImage(const TS_CHAR *filename, OSD_LOGO_T *pVideoLogo);
TS_S32 LoadBitMap2Surface(const TS_CHAR *pszFileName, const OSD_SURFACE_S *pstSurface, TS_U8 *pu8Virt);
TS_S32 CreateSurfaceByBitMap(const TS_CHAR *pszFileName, OSD_SURFACE_S *pstSurface, TS_U8 *pu8Virt);
TS_S32 CreateSurfaceByCanvas(const TS_CHAR *pszFileName, OSD_SURFACE_S *pstSurface, TS_U8 *pu8Virt,
							TS_U32 u32Width, TS_U32 u32Height, TS_U32 u32Stride);
TS_S32 GetBmpInfo(const TS_CHAR *filename, OSD_BITMAPFILEHEADER *pBmpFileHeader, OSD_BITMAPINFO *pBmpInfo);


#endif /* End of #ifndef __LOAD_BMP_H__*/

