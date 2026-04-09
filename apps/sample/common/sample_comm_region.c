#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <errno.h>
#include <pthread.h>
#include <sys/prctl.h>
#include "ts_common.h"
#include "sample_comm_region.h"
#include "loadbmp.h"
#include "osd/osd_truetype.h"
#include "osd/osd_timestamp.h"
#include "soft_line.h"
#include "osd/nv12_calcbright.h"

#define OverlayMinHandle    0
#define OverlayExMinHandle 20
#define CoverMinHandle     40
#define CoverExMinHandle   60
#define MosaicMinHandle    80
#define MosaicExMinHandle    100

TS_CHAR * Path_BMP = TS_NULL;
static RGN_HANDLE ff_hdl_ext[10] = {0,3};

#pragma pack(push, 1) // 结构体按照1字节对齐
typedef struct {
    uint16_t type; // 文件类型，必须为0x4D42
    uint32_t size; // 文件大小，单位字节
    uint32_t reserved; // 保留位，必须为0
    uint32_t offset; // 从文件头到位图数据的偏移量
    uint32_t header_size; // 信息头大小，必须为40
    int32_t width; // 图像宽度
    int32_t height; // 图像高度
    uint16_t planes; // 颜色平面数，必须为1
    uint16_t bpp; // 每个像素的位数，必须为24
    uint32_t compression; // 压缩类型，必须为0（不压缩）
    uint32_t size_image; // 图像数据大小，单位字节
    int32_t x_ppm; // 水平分辨率，单位像素/米
    int32_t y_ppm; // 垂直分辨率，单位像素/米
    uint32_t colors_used; // 调色板颜色数，必须为0（不使用调色板）
    uint32_t colors_important; // 重要颜色数，必须为0（所有颜色都是重要的）
} bmp_header;
#pragma pack(pop)

TS_S32 REGION_MST_LoadBmp(const char *filename, BITMAP_S *pstBitmap, TS_BOOL bFil, TS_U32 u16FilColor,
	PIXEL_FORMAT_E enPixelFormat)
{
	OSD_SURFACE_S Surface;
	OSD_BITMAPFILEHEADER bmpFileHeader;
	OSD_BITMAPINFO bmpInfo;
	TS_S32 s32BytesPerPix = 2;
	TS_U8 *pu8Data;
	TS_S32 R_Value;
	TS_S32 G_Value;
	TS_S32 B_Value;
	TS_S32 Gr_Value;
	TS_U8  Value_tmp;
	TS_U8  Value;
	TS_S32 s32Width;

	if (GetBmpInfo(filename, &bmpFileHeader, &bmpInfo) < 0) {
		printf("GetBmpInfo err!\n");
		return TS_FAILURE;
	}

	if (enPixelFormat == PIXEL_FORMAT_ARGB_4444) {
		Surface.enColorFmt = OSD_COLOR_FMT_RGB4444;
	} else if (enPixelFormat == PIXEL_FORMAT_ARGB_1555 || enPixelFormat == PIXEL_FORMAT_ARGB_2BPP) {
		Surface.enColorFmt = OSD_COLOR_FMT_RGB1555;
	} else if (enPixelFormat == PIXEL_FORMAT_ARGB_8888) {
		Surface.enColorFmt = OSD_COLOR_FMT_RGB8888;
		s32BytesPerPix = 4;
	} else {
		printf("enPixelFormat err %d\n", enPixelFormat);
		return TS_FAILURE;
	}

	pstBitmap->pData = malloc(s32BytesPerPix * (bmpInfo.bmiHeader.biWidth) * (bmpInfo.bmiHeader.biHeight));

	if (NULL == pstBitmap->pData)	{
		printf("malloc osd memroy err!\n");
		return TS_FAILURE;
	}

	CreateSurfaceByBitMap(filename, &Surface, (TS_U8 *)(pstBitmap->pData));

	pstBitmap->u32Width = Surface.u16Width;
	pstBitmap->u32Height = Surface.u16Height;
	pstBitmap->enPixelFormat = enPixelFormat;

	int i, j, k;
	TS_U8 *pu8Temp;

	if (PIXEL_FORMAT_ARGB_2BPP == enPixelFormat) {
		s32Width = DIV_UP(bmpInfo.bmiHeader.biWidth, 4);
		pu8Data = malloc((s32Width) * (bmpInfo.bmiHeader.biHeight));
		if (NULL == pu8Data) {
			printf("malloc osd memroy err!\n");
			return TS_FAILURE;
		}
	}
	if (PIXEL_FORMAT_ARGB_2BPP != enPixelFormat) {
		TS_U16 *pu16Temp;

		pu16Temp = (TS_U16 *)pstBitmap->pData;

		if (bFil) {
			for (i = 0; i < pstBitmap->u32Height; i++) {
				for (j = 0; j < pstBitmap->u32Width; j++) {
					if (u16FilColor == *pu16Temp)
						*pu16Temp &= 0x7FFF;

					pu16Temp++;
				}
			}
		}
	} else {
		TS_U16 *pu16Temp;

		pu16Temp = (TS_U16 *)pstBitmap->pData;
		pu8Temp = (TS_U8 *)pu8Data;
		for (i = 0; i < pstBitmap->u32Height; i++) {
			for (j = 0; j < pstBitmap->u32Width/4; j++) {
				Value = 0;
				for (k = j; k < j + 4; k++) {
					B_Value = *pu16Temp & 0x001F;
					G_Value = *pu16Temp >> 5 & 0x001F;
					R_Value = *pu16Temp >> 10 & 0x001F;
					pu16Temp++;
					Gr_Value = (R_Value * 299 + G_Value * 587 + B_Value * 144 + 500) / 1000;
					if (Gr_Value > 16)
						Value_tmp = 0x01;
					else
						Value_tmp = 0x00;
					Value = (Value << 2) + Value_tmp;
				}
				*pu8Temp = Value;
				pu8Temp++;
			}
		}
		free(pstBitmap->pData);
		pstBitmap->pData = pu8Data;
	}

	return TS_SUCCESS;
}

TS_S32 REGION_MST_UpdateCanvas(const char *filename, BITMAP_S *pstBitmap, TS_BOOL bFil,
				TS_U32 u16FilColor, SIZE_S *pstSize, TS_U32 u32Stride, PIXEL_FORMAT_E enPixelFmt)
{
	OSD_SURFACE_S Surface;
	OSD_BITMAPFILEHEADER bmpFileHeader;
	OSD_BITMAPINFO bmpInfo;

	if (GetBmpInfo(filename, &bmpFileHeader, &bmpInfo) < 0) {
		printf("GetBmpInfo err!\n");
		return TS_FAILURE;
	}

	if (PIXEL_FORMAT_ARGB_1555 == enPixelFmt)
		Surface.enColorFmt = OSD_COLOR_FMT_RGB1555;
	else if (PIXEL_FORMAT_ARGB_4444 == enPixelFmt)
		Surface.enColorFmt = OSD_COLOR_FMT_RGB4444;
	else if (PIXEL_FORMAT_ARGB_8888 == enPixelFmt)
		Surface.enColorFmt = OSD_COLOR_FMT_RGB8888;
	else {
		printf("Pixel format is not support!\n");
		return TS_FAILURE;
	}

	if (NULL == pstBitmap->pData)	{
		printf("malloc osd memroy err!\n");
		return TS_FAILURE;
	}

	CreateSurfaceByCanvas(filename, &Surface, (TS_U8 *)(pstBitmap->pData), pstSize->u32Width,
							pstSize->u32Height, u32Stride);

	pstBitmap->u32Width = Surface.u16Width;
	pstBitmap->u32Height = Surface.u16Height;

	if (PIXEL_FORMAT_ARGB_1555 == enPixelFmt)
		pstBitmap->enPixelFormat = PIXEL_FORMAT_ARGB_1555;
	else if (PIXEL_FORMAT_ARGB_4444 == enPixelFmt)
		pstBitmap->enPixelFormat = PIXEL_FORMAT_ARGB_4444;
	else if (PIXEL_FORMAT_ARGB_8888 == enPixelFmt)
		pstBitmap->enPixelFormat = PIXEL_FORMAT_ARGB_8888;

	int i, j;
	TS_U16 *pu16Temp;

	pu16Temp = (TS_U16 *)pstBitmap->pData;
	if (bFil)  {
		for (i = 0; i < pstBitmap->u32Height; i++) {
			for (j = 0; j < pstBitmap->u32Width; j++) {
				if (u16FilColor == *pu16Temp)
					*pu16Temp &= 0x7FFF;

				pu16Temp++;
			}
		}

	}

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_REGION_GetMinHandle(RGN_TYPE_E enType)
{
	TS_S32 MinHandle;

	switch (enType) {
	case OVERLAY_RGN:
	    MinHandle = OverlayMinHandle;
	break;
	case OVERLAYEX_RGN:
	    MinHandle = OverlayExMinHandle;
	break;
	case COVER_RGN:
	    MinHandle = CoverMinHandle;
	break;
	case COVEREX_RGN:
	    MinHandle = CoverExMinHandle;
	break;
	case MOSAIC_RGN:
	    MinHandle = MosaicMinHandle;
	break;
	case MOSAICEX_RGN:
	    MinHandle = MosaicExMinHandle;
	break;
	default:
	    MinHandle = -1;
	break;
	}
	return MinHandle;
}

TS_S32 SAMPLE_REGION_CreateOverLay(TS_S32 HandleNum)
{
	TS_S32 s32Ret;
	TS_S32 i;
	RGN_ATTR_S stRegion;

	stRegion.enType = OVERLAY_RGN;
	stRegion.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_ARGB_8888;
	stRegion.unAttr.stOverlay.stSize.u32Height = 144;
	stRegion.unAttr.stOverlay.stSize.u32Width  = 180;
	stRegion.unAttr.stOverlay.u32BgColor = 0x00ff00ff;
	stRegion.unAttr.stOverlay.u32CanvasNum = 2;
	for (i = OverlayMinHandle; i < HandleNum; i++) {
		s32Ret = TS_MPI_RGN_Create(i, &stRegion);
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("TS_MPI_RGN_Create failed with %#x!\n", s32Ret);
			return TS_FAILURE;
		}
	}

	return s32Ret;
}

TS_S32 SAMPLE_REGION_CreateOverLayEx(TS_S32 HandleNum)
{
	TS_S32 s32Ret;
	TS_S32 i;
	RGN_ATTR_S stRegion;

	stRegion.enType = OVERLAYEX_RGN;
	stRegion.unAttr.stOverlayEx.enPixelFmt = PIXEL_FORMAT_ARGB_8888;
	stRegion.unAttr.stOverlayEx.stSize.u32Height = 144;
	stRegion.unAttr.stOverlayEx.stSize.u32Width  = 180;
	stRegion.unAttr.stOverlayEx.u32BgColor = 0x00ff00ff;
	stRegion.unAttr.stOverlayEx.u32CanvasNum = 2;
	for (i = OverlayExMinHandle; i < OverlayExMinHandle + HandleNum; i++)	{
		s32Ret = TS_MPI_RGN_Create(i, &stRegion);
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("TS_MPI_RGN_Create failed with %#x!\n", s32Ret);
			return TS_FAILURE;
		}
	}

	return s32Ret;
}

TS_S32 SAMPLE_REGION_CreateCover(TS_S32 HandleNum)
{
	TS_S32 s32Ret;
	TS_S32 i;
	RGN_ATTR_S stRegion;

	stRegion.enType = COVER_RGN;

	for (i = CoverMinHandle; i < CoverMinHandle + HandleNum; i++) {
		s32Ret = TS_MPI_RGN_Create(i, &stRegion);
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("TS_MPI_RGN_Create failed with %#x!\n", s32Ret);
			return TS_FAILURE;
		}
	}

	return s32Ret;
}

TS_S32 SAMPLE_REGION_CreateCoverEx(TS_S32 HandleNum)
{
	TS_S32 s32Ret;
	TS_S32 i;
	RGN_ATTR_S stRegion;

	stRegion.enType = COVEREX_RGN;

	for (i = CoverExMinHandle; i < CoverExMinHandle + HandleNum; i++) {
		s32Ret = TS_MPI_RGN_Create(i, &stRegion);
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("TS_MPI_RGN_Create failed with %#x!\n", s32Ret);
			return TS_FAILURE;
		}
	}

	return s32Ret;
}

TS_S32 SAMPLE_REGION_CreateMosaic(TS_S32 HandleNum)
{
	TS_S32 s32Ret;
	TS_S32 i;
	RGN_ATTR_S stRegion;

	stRegion.enType = MOSAIC_RGN;

	for (i = MosaicMinHandle; i < MosaicMinHandle + HandleNum; i++) {
		s32Ret = TS_MPI_RGN_Create(i, &stRegion);
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("TS_MPI_RGN_Create failed with %#x!\n", s32Ret);
			return TS_FAILURE;
		}
	}

	return s32Ret;
}

TS_S32 SAMPLE_REGION_CreateMosaicEx(TS_S32 HandleNum)
{
	TS_S32 s32Ret;
	TS_S32 i;
	RGN_ATTR_S stRegion;

	stRegion.enType = MOSAICEX_RGN;

	for (i = MosaicExMinHandle; i < MosaicExMinHandle + HandleNum; i++) {
		s32Ret = TS_MPI_RGN_Create(i, &stRegion);
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("TS_MPI_RGN_Create failed with %#x!\n", s32Ret);
			return TS_FAILURE;
		}
	}

	return s32Ret;
}

TS_S32 SAMPLE_REGION_Destroy(RGN_HANDLE Handle)
{
	TS_S32 s32Ret;

	s32Ret = TS_MPI_RGN_Destroy(Handle);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_Destroy failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}
	return s32Ret;
}

TS_S32 SAMPLE_REGION_SetAttr(RGN_HANDLE Handle, RGN_ATTR_S *pstRegion)
{
	TS_S32 s32Ret;

	s32Ret = TS_MPI_RGN_SetAttr(Handle, pstRegion);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_SetAttr failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}
	return s32Ret;
}

TS_S32 SAMPLE_REGION_GetAttr(RGN_HANDLE Handle, RGN_ATTR_S *pstRegion)
{
	TS_S32 s32Ret;

	s32Ret = TS_MPI_RGN_Create(Handle, pstRegion);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_Create failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}
	return s32Ret;
}

TS_S32 SAMPLE_REGION_AttachToChn(RGN_HANDLE Handle, MPP_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr)
{
	TS_S32 s32Ret;

	s32Ret = TS_MPI_RGN_AttachToChn(Handle, pstChn, pstChnAttr);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_AttachToChn failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}
	return s32Ret;
}

TS_S32 SAMPLE_REGION_DetachFromChn(RGN_HANDLE Handle, MPP_CHN_S *pstChn)
{
	TS_S32 s32Ret;

	s32Ret = TS_MPI_RGN_DetachFromChn(Handle, pstChn);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_DetachFromChn failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}
	return s32Ret;
}

TS_S32 SAMPLE_REGION_SetDisplayAttr(RGN_HANDLE Handle, MPP_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr)
{
	TS_S32 s32Ret;

	s32Ret = TS_MPI_RGN_SetDisplayAttr(Handle, pstChn, pstChnAttr);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_SetDisplayAttr failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}
	return s32Ret;
}

TS_S32 SAMPLE_REGION_GetDisplayAttr(RGN_HANDLE Handle, MPP_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr)
{
	TS_S32 s32Ret;

	s32Ret = TS_MPI_RGN_GetDisplayAttr(Handle, pstChn, pstChnAttr);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_GetDisplayAttr failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}
	return s32Ret;
}

TS_S32 SAMPLE_REGION_SetBitMap(RGN_HANDLE Handle, BITMAP_S *pstBitmap)
{
	TS_S32 s32Ret;

	s32Ret = TS_MPI_RGN_SetBitMap(Handle, pstBitmap);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_SetBitMap failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}
	return s32Ret;
}

TS_S32 SAMPLE_REGION_GetUpCanvasInfo(RGN_HANDLE Handle, RGN_CANVAS_INFO_S *pstCanvasInfo)
{
	TS_S32 s32Ret;

	s32Ret = TS_MPI_RGN_GetCanvasInfo(Handle, pstCanvasInfo);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_GetCanvasInfo failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}


	s32Ret = TS_MPI_RGN_UpdateCanvas(Handle);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_UpdateCanvas failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}
	return s32Ret;
}

TS_S32 SAMPLE_COMM_REGION_Create(TS_S32 HandleNum, RGN_TYPE_E enType)
{
	TS_S32 s32Ret;

	if (HandleNum <= 0 || HandleNum > 24) {
		SAMPLE_PRT("HandleNum is illegal %d!\n", HandleNum);
		return TS_FAILURE;
	}
	if (enType < 0 || enType > 5) {
		SAMPLE_PRT("enType is illegal %d!\n", enType);
		return TS_FAILURE;
	}
	switch (enType) {
	case OVERLAY_RGN:
	     s32Ret = SAMPLE_REGION_CreateOverLay(HandleNum);
	break;
	case OVERLAYEX_RGN:
	     s32Ret = SAMPLE_REGION_CreateOverLayEx(HandleNum);
	break;
	case COVER_RGN:
	     s32Ret = SAMPLE_REGION_CreateCover(HandleNum);
	break;
	case COVEREX_RGN:
	     s32Ret = SAMPLE_REGION_CreateCoverEx(HandleNum);
	break;
	case MOSAIC_RGN:
	     s32Ret = SAMPLE_REGION_CreateMosaic(HandleNum);
    break;
    case MOSAICEX_RGN:
      s32Ret = SAMPLE_REGION_CreateMosaicEx(HandleNum);
	break;
	default:
	break;
	}
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("failed! HandleNum%d,entype:%d!\n", HandleNum, enType);
		return TS_FAILURE;
	}
	return s32Ret;
}

TS_S32 SAMPLE_COMM_REGION_Destroy(TS_S32 HandleNum, RGN_TYPE_E enType)
{
	TS_S32 i;
	TS_S32 s32Ret = TS_SUCCESS;
	TS_S32 MinHadle;

	if (HandleNum <= 0 || HandleNum > 24) {
		SAMPLE_PRT("HandleNum is illegal %d!\n", HandleNum);
		return TS_FAILURE;
	}
	if (enType < 0 || enType > 5) {
		SAMPLE_PRT("enType is illegal %d!\n", enType);
		return TS_FAILURE;
	}
	switch (enType) {
	case OVERLAY_RGN:
	    MinHadle  = OverlayMinHandle;
	break;
	case OVERLAYEX_RGN:
	    MinHadle  = OverlayExMinHandle;
	break;
	case COVER_RGN:
	    MinHadle  = CoverMinHandle;
	break;
	case COVEREX_RGN:
	    MinHadle  = CoverExMinHandle;
	break;
	case MOSAIC_RGN:
	    MinHadle  = MosaicMinHandle;
	break;
	case MOSAICEX_RGN:
	    MinHadle  = MosaicExMinHandle;
	break;
	default:
	break;
	}
	for (i = MinHadle; i < MinHadle + HandleNum; i++) {
		s32Ret = SAMPLE_REGION_Destroy(i);
		if (TS_SUCCESS != s32Ret)
			SAMPLE_PRT("failed!\n");
	}
	return s32Ret;
}

TS_S32 SAMPLE_COMM_REGION_AttachToChn(TS_S32 HandleNum, RGN_TYPE_E enType, MPP_CHN_S *pstMppChn)
{
	TS_S32 i;
	TS_S32 s32Ret;
	TS_S32 MinHadle;
	RGN_CHN_ATTR_S stChnAttr;

	if (HandleNum <= 0 || HandleNum > 16) {
		SAMPLE_PRT("HandleNum is illegal %d!\n", HandleNum);
		return TS_FAILURE;
	}
	if (enType < 0 || enType > 5) {
		SAMPLE_PRT("enType is illegal %d!\n", enType);
		return TS_FAILURE;
	}
	if (TS_NULL == pstMppChn) {
		SAMPLE_PRT("pstMppChn is NULL !\n");
		return TS_FAILURE;
	}
	/*set the chn config*/
	stChnAttr.bShow = TS_TRUE;
	switch (enType)  {
	case OVERLAY_RGN:
	    MinHadle = OverlayMinHandle;

	    stChnAttr.bShow = TS_TRUE;
	    stChnAttr.enType = OVERLAY_RGN;

	    stChnAttr.unChnAttr.stOverlayChn.u32BgAlpha = 128;
	    stChnAttr.unChnAttr.stOverlayChn.u32FgAlpha = 128;

	    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.bQpDisable = TS_FALSE;
	    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.bAbsQp = TS_TRUE;
	    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.s32Qp  = 30;

	    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Height = 16;
	    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Width = 16;
	    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.u32LumThresh = 128;
	    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.enChgMod = LESSTHAN_LUM_THRESH;
	    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn = TS_FALSE;
	    stChnAttr.unChnAttr.stOverlayChn.u16ColorLUT[0] = 0x2abc;
	    stChnAttr.unChnAttr.stOverlayChn.u16ColorLUT[1] = 0x7FF0;
	    stChnAttr.unChnAttr.stOverlayChn.enAttachDest = ATTACH_JPEG_MAIN;
	break;
	case OVERLAYEX_RGN:
	    MinHadle = OverlayExMinHandle;
	    stChnAttr.bShow = TS_TRUE;
	    stChnAttr.enType = OVERLAYEX_RGN;

	    stChnAttr.unChnAttr.stOverlayExChn.u32BgAlpha = 128;
	    stChnAttr.unChnAttr.stOverlayExChn.u32FgAlpha = 128;
	break;
	case COVER_RGN:
	    MinHadle = CoverMinHandle;

	    stChnAttr.bShow = TS_TRUE;
	    stChnAttr.enType = COVER_RGN;
	    stChnAttr.unChnAttr.stCoverChn.enCoverType = AREA_RECT;

	    stChnAttr.unChnAttr.stCoverChn.stRect.u32Height = 200;
	    stChnAttr.unChnAttr.stCoverChn.stRect.u32Width  = 200;

	    stChnAttr.unChnAttr.stCoverChn.u32Color      = 0x0000ffff;

	    stChnAttr.unChnAttr.stCoverChn.enCoordinate = RGN_ABS_COOR;
	break;
	case COVEREX_RGN:
	    MinHadle = CoverExMinHandle;

	    stChnAttr.bShow = TS_TRUE;
	    stChnAttr.enType = COVEREX_RGN;
	    stChnAttr.unChnAttr.stCoverExChn.enCoverType = AREA_RECT;

	    stChnAttr.unChnAttr.stCoverExChn.stRect.u32Height = 200;
	    stChnAttr.unChnAttr.stCoverExChn.stRect.u32Width  = 200;

	    stChnAttr.unChnAttr.stCoverExChn.u32Color      = 0x0000ffff;
	break;
	case MOSAIC_RGN:
	    MinHadle = MosaicMinHandle;
	    stChnAttr.enType = MOSAIC_RGN;
	    stChnAttr.unChnAttr.stMosaicChn.enBlkSize = MOSAIC_BLK_SIZE_32;
	    stChnAttr.unChnAttr.stMosaicChn.stRect.u32Height = 200;
	    stChnAttr.unChnAttr.stMosaicChn.stRect.u32Width  = 200;
	break;
	case MOSAICEX_RGN:
	    MinHadle = MosaicExMinHandle;
	    stChnAttr.enType = MOSAICEX_RGN;
	    stChnAttr.unChnAttr.stMosaicChn.enBlkSize = MOSAIC_BLK_SIZE_32;
	    stChnAttr.unChnAttr.stMosaicChn.stRect.u32Height = 200;
	    stChnAttr.unChnAttr.stMosaicChn.stRect.u32Width  = 200;
	break;
    default:
	break;
	}
	/*attach to Chn*/
	for (i = MinHadle; i < MinHadle + HandleNum; i++) {
		if (OVERLAY_RGN == enType) {
			stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = 20 + 200*(i-OverlayMinHandle);
			stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = 20 + 200*(i-OverlayMinHandle);
			stChnAttr.unChnAttr.stOverlayChn.u32Layer = i - OverlayMinHandle;
		}
		if (OVERLAYEX_RGN == enType) {
			stChnAttr.unChnAttr.stOverlayExChn.stPoint.s32X = 20 + 200*(i - OverlayExMinHandle);
			stChnAttr.unChnAttr.stOverlayExChn.stPoint.s32Y = 20 + 200*(i - OverlayExMinHandle);
			stChnAttr.unChnAttr.stOverlayExChn.u32Layer = i - OverlayExMinHandle;
		}
		if (COVER_RGN == enType)	{
			stChnAttr.unChnAttr.stCoverChn.stRect.s32X = 20 + 200*(i - CoverMinHandle);
			stChnAttr.unChnAttr.stCoverChn.stRect.s32Y = 20 + 200*(i - CoverMinHandle);
			stChnAttr.unChnAttr.stCoverChn.u32Layer = i - CoverMinHandle;
		}
		if (COVEREX_RGN == enType) {
			stChnAttr.unChnAttr.stCoverExChn.stRect.s32X = 400 + 200*(i - CoverExMinHandle);
			stChnAttr.unChnAttr.stCoverExChn.stRect.s32Y = 20 + 200*(i - CoverExMinHandle);
			stChnAttr.unChnAttr.stCoverExChn.u32Layer = i - CoverExMinHandle;
		}
		if (MOSAIC_RGN == enType) {
			stChnAttr.unChnAttr.stMosaicChn.stRect.s32X = 400 + 200*(i-MosaicMinHandle);
			stChnAttr.unChnAttr.stMosaicChn.stRect.s32Y = 20 + 200*(i-MosaicMinHandle);
			stChnAttr.unChnAttr.stMosaicChn.u32Layer = i-MosaicMinHandle;
		}
		if (MOSAICEX_RGN == enType) {
			stChnAttr.unChnAttr.stMosaicChn.stRect.s32X = 800 + 200*(i-MosaicExMinHandle);
			stChnAttr.unChnAttr.stMosaicChn.stRect.s32Y = 20 + 200*(i-MosaicExMinHandle);
			stChnAttr.unChnAttr.stMosaicChn.u32Layer = i-MosaicExMinHandle;
		}
		s32Ret = SAMPLE_REGION_AttachToChn(i, pstMppChn, &stChnAttr);
		if (TS_SUCCESS != s32Ret) {
			SAMPLE_PRT("SAMPLE_REGION_AttachToChn failed!\n");
			break;
		}
	}
	/*detach region from chn */
	if (TS_SUCCESS != s32Ret && i > 0)	{
		i--;
		for (; i >= MinHadle; i--)
			s32Ret = SAMPLE_REGION_DetachFromChn(i, pstMppChn);
	}
	return s32Ret;
}

TS_S32 SAMPLE_COMM_REGION_AttachToMultiChn(TS_S32 HandleNum, RGN_TYPE_E enType, MOD_ID_E enModId, TS_S32 s32DevId, TS_S32 chnCnt)
{
	TS_S32 i;
	TS_S32 j;
	TS_S32 tmp_index = 0;
	TS_S32 s32Ret = 0;
	TS_S32 MinHadle;
	RGN_CHN_ATTR_S stChnAttr;
	MPP_CHN_S stChn = {0};
	MPP_CHN_S *pstMppChn = &stChn;

	pstMppChn->enModId = enModId;
	pstMppChn->s32DevId = s32DevId;
	pstMppChn->s32ChnId = 0;

	if (HandleNum <= 0 || HandleNum > 16) {
		SAMPLE_PRT("HandleNum is illegal %d!\n", HandleNum);
		return TS_FAILURE;
	}
	if (enType < 0 || enType > 5) {
		SAMPLE_PRT("enType is illegal %d!\n", enType);
		return TS_FAILURE;
	}
	if (TS_NULL == pstMppChn) {
		SAMPLE_PRT("pstMppChn is NULL !\n");
		return TS_FAILURE;
	}
	/*set the chn config*/
	stChnAttr.bShow = TS_TRUE;
	switch (enType)  {
	case OVERLAY_RGN:
	    MinHadle = OverlayMinHandle;

	    stChnAttr.bShow = TS_TRUE;
	    stChnAttr.enType = OVERLAY_RGN;

	    stChnAttr.unChnAttr.stOverlayChn.u32BgAlpha = 128;
	    stChnAttr.unChnAttr.stOverlayChn.u32FgAlpha = 128;

	    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.bQpDisable = TS_FALSE;
	    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.bAbsQp = TS_TRUE;
	    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.s32Qp  = 30;

	    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Height = 16;
	    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Width = 16;
	    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.u32LumThresh = 128;
	    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.enChgMod = LESSTHAN_LUM_THRESH;
	    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn = TS_FALSE;
	    stChnAttr.unChnAttr.stOverlayChn.u16ColorLUT[0] = 0x2abc;
	    stChnAttr.unChnAttr.stOverlayChn.u16ColorLUT[1] = 0x7FF0;
	    stChnAttr.unChnAttr.stOverlayChn.enAttachDest = ATTACH_JPEG_MAIN;
	break;
	case OVERLAYEX_RGN:
	    MinHadle = OverlayExMinHandle;
	    stChnAttr.bShow = TS_TRUE;
	    stChnAttr.enType = OVERLAYEX_RGN;

	    stChnAttr.unChnAttr.stOverlayExChn.u32BgAlpha = 128;
	    stChnAttr.unChnAttr.stOverlayExChn.u32FgAlpha = 128;
	break;
	case COVER_RGN:
	    MinHadle = CoverMinHandle;

	    stChnAttr.bShow = TS_TRUE;
	    stChnAttr.enType = COVER_RGN;
	    stChnAttr.unChnAttr.stCoverChn.enCoverType = AREA_RECT;

	    stChnAttr.unChnAttr.stCoverChn.stRect.u32Height = 200;
	    stChnAttr.unChnAttr.stCoverChn.stRect.u32Width  = 200;

	    stChnAttr.unChnAttr.stCoverChn.u32Color      = 0x0000ffff;

	    stChnAttr.unChnAttr.stCoverChn.enCoordinate = RGN_ABS_COOR;
	break;
	case COVEREX_RGN:
	    MinHadle = CoverExMinHandle;

	    stChnAttr.bShow = TS_TRUE;
	    stChnAttr.enType = COVEREX_RGN;
	    stChnAttr.unChnAttr.stCoverExChn.enCoverType = AREA_RECT;

	    stChnAttr.unChnAttr.stCoverExChn.stRect.u32Height = 200;
	    stChnAttr.unChnAttr.stCoverExChn.stRect.u32Width  = 200;

	    stChnAttr.unChnAttr.stCoverExChn.u32Color      = 0x0000ffff;
	break;
	case MOSAIC_RGN:
	    MinHadle = MosaicMinHandle;
	    stChnAttr.enType = MOSAIC_RGN;
	    stChnAttr.unChnAttr.stMosaicChn.enBlkSize = MOSAIC_BLK_SIZE_32;
	    stChnAttr.unChnAttr.stMosaicChn.stRect.u32Height = 200;
	    stChnAttr.unChnAttr.stMosaicChn.stRect.u32Width  = 200;
	break;
	case MOSAICEX_RGN:
	    MinHadle = MosaicExMinHandle;
	    stChnAttr.enType = MOSAICEX_RGN;
	    stChnAttr.unChnAttr.stMosaicChn.enBlkSize = MOSAIC_BLK_SIZE_32;
	    stChnAttr.unChnAttr.stMosaicChn.stRect.u32Height = 200;
	    stChnAttr.unChnAttr.stMosaicChn.stRect.u32Width  = 200;
	break;
    default:
	break;
	}
	/*attach to Chn*/
	for	(j = 0; j < chnCnt; j++){
		pstMppChn->s32ChnId = j;
		for (i = MinHadle; i < MinHadle + HandleNum; i++) {
			if (OVERLAY_RGN == enType) {
				stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = 20 + 200*(i-OverlayMinHandle);
				stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = 20 + 200*(i-OverlayMinHandle);
				stChnAttr.unChnAttr.stOverlayChn.u32Layer = i - OverlayMinHandle;
			}
			if (OVERLAYEX_RGN == enType) {
				stChnAttr.unChnAttr.stOverlayExChn.stPoint.s32X = 20 + 200*(i - OverlayExMinHandle);
				stChnAttr.unChnAttr.stOverlayExChn.stPoint.s32Y = 20 + 200*(i - OverlayExMinHandle);
				stChnAttr.unChnAttr.stOverlayExChn.u32Layer = i - OverlayExMinHandle;
			}
			if (COVER_RGN == enType)	{
				stChnAttr.unChnAttr.stCoverChn.stRect.s32X = 20 + 200*(i - CoverMinHandle);
				stChnAttr.unChnAttr.stCoverChn.stRect.s32Y = 20 + 200*(i - CoverMinHandle);
				stChnAttr.unChnAttr.stCoverChn.u32Layer = i - CoverMinHandle;
			}
			if (COVEREX_RGN == enType) {
				stChnAttr.unChnAttr.stCoverExChn.stRect.s32X = 400 + 200*(i - CoverExMinHandle);
				stChnAttr.unChnAttr.stCoverExChn.stRect.s32Y = 20 + 200*(i - CoverExMinHandle);
				stChnAttr.unChnAttr.stCoverExChn.u32Layer = i - CoverExMinHandle;
			}
			if (MOSAIC_RGN == enType) {
				stChnAttr.unChnAttr.stMosaicChn.stRect.s32X = 400 + 200*(i-MosaicMinHandle);
				stChnAttr.unChnAttr.stMosaicChn.stRect.s32Y = 20 + 200*(i-MosaicMinHandle);
				stChnAttr.unChnAttr.stMosaicChn.u32Layer = i-MosaicMinHandle;
			}
			if (MOSAICEX_RGN == enType) {
				stChnAttr.unChnAttr.stMosaicChn.stRect.s32X = 800 + 200*(i-MosaicExMinHandle);
				stChnAttr.unChnAttr.stMosaicChn.stRect.s32Y = 20 + 200*(i-MosaicExMinHandle);
				stChnAttr.unChnAttr.stMosaicChn.u32Layer = i-MosaicExMinHandle;
			}
			s32Ret = SAMPLE_REGION_AttachToChn(i+j*HandleNum, pstMppChn, &stChnAttr);
			if (TS_SUCCESS != s32Ret) {
				SAMPLE_PRT("SAMPLE_REGION_AttachToChn failed!\n");
				break;
			}
		}
		/*detach region from chn */
		if (TS_SUCCESS != s32Ret && i > 0)	{
			tmp_index = i+j*HandleNum;
			for (; tmp_index >= MinHadle+j*HandleNum; tmp_index--)
				s32Ret = SAMPLE_REGION_DetachFromChn(tmp_index, pstMppChn);
		}
	}
	return s32Ret;
}



TS_S32 SAMPLE_COMM_REGION_DetachFrmChn(TS_S32 HandleNum, RGN_TYPE_E enType, MPP_CHN_S *pstMppChn)
{
	TS_S32 i;
	TS_S32 s32Ret = TS_SUCCESS;
	TS_S32 MinHadle;

	if (HandleNum <= 0 || HandleNum > 16) {
		SAMPLE_PRT("HandleNum is illegal %d!\n", HandleNum);
		return TS_FAILURE;
	}
	if (enType < 0 || enType > 5) {
		SAMPLE_PRT("enType is illegal %d!\n", enType);
		return TS_FAILURE;
	}
	if (TS_NULL == pstMppChn) {
		SAMPLE_PRT("pstMppChn is NULL !\n");
		return TS_FAILURE;
	}
	switch (enType)  {
	case OVERLAY_RGN:
	    MinHadle  = OverlayMinHandle;
	break;
	case OVERLAYEX_RGN:
	    MinHadle  = OverlayExMinHandle;
	break;
	case COVER_RGN:
	    MinHadle  = CoverMinHandle;
	break;
	case COVEREX_RGN:
	    MinHadle  = CoverExMinHandle;
	break;
	case MOSAIC_RGN:
	    MinHadle  = MosaicMinHandle;
    break;
    case MOSAICEX_RGN:
        MinHadle  = MosaicExMinHandle;
	break;
	default:
	break;
	}
	for (i = MinHadle; i < MinHadle + HandleNum; i++) {
		s32Ret = SAMPLE_REGION_DetachFromChn(i, pstMppChn);
		if (TS_SUCCESS != s32Ret)
			SAMPLE_PRT("SAMPLE_REGION_DetachFromChn failed! Handle:%d\n", i);
	}
	return s32Ret;
}

TS_S32 SAMPLE_COMM_REGION_DetachFrmMultiChn(TS_S32 HandleNum, RGN_TYPE_E enType, MOD_ID_E enModId, TS_S32 s32DevId, TS_S32 chnCnt)
{
	TS_S32 i;
	TS_S32 j;
	TS_S32 s32Ret = TS_SUCCESS;
	TS_S32 MinHadle;
	MPP_CHN_S stChn = {0};
	MPP_CHN_S *pstMppChn = &stChn;

	pstMppChn->enModId = enModId;
	pstMppChn->s32DevId = s32DevId;
	pstMppChn->s32ChnId = 0;

	if (HandleNum <= 0 || HandleNum > 16) {
		SAMPLE_PRT("HandleNum is illegal %d!\n", HandleNum);
		return TS_FAILURE;
	}
	if (enType < 0 || enType > 5) {
		SAMPLE_PRT("enType is illegal %d!\n", enType);
		return TS_FAILURE;
	}
	if (TS_NULL == pstMppChn) {
		SAMPLE_PRT("pstMppChn is NULL !\n");
		return TS_FAILURE;
	}
	switch (enType)  {
	case OVERLAY_RGN:
	    MinHadle  = OverlayMinHandle;
	break;
	case OVERLAYEX_RGN:
	    MinHadle  = OverlayExMinHandle;
	break;
	case COVER_RGN:
	    MinHadle  = CoverMinHandle;
	break;
	case COVEREX_RGN:
	    MinHadle  = CoverExMinHandle;
	break;
	case MOSAIC_RGN:
	    MinHadle  = MosaicMinHandle;
    break;
    case MOSAICEX_RGN:
        MinHadle  = MosaicExMinHandle;
	break;
	default:
	break;
	}
	for	(j = 0; j < chnCnt; j++){
		pstMppChn->s32ChnId = j;
		for (i = 0; i < HandleNum; i++) {
			s32Ret = SAMPLE_REGION_DetachFromChn(MinHadle+i+j*HandleNum, pstMppChn);
			if (TS_SUCCESS != s32Ret)
				SAMPLE_PRT("SAMPLE_REGION_DetachFromChn failed! Handle:%d\n", MinHadle+i+j*HandleNum);
		}
	}
	return s32Ret;
}

TS_S32 SAMPLE_COMM_REGION_SetBitMap(RGN_HANDLE Handle, PIXEL_FORMAT_E enPixelFmt)
{
	TS_S32 s32Ret;
	BITMAP_S stBitmap;

	REGION_MST_LoadBmp(Path_BMP, &stBitmap, TS_FALSE, 0, enPixelFmt);
	s32Ret = SAMPLE_REGION_SetBitMap(Handle, &stBitmap);
	if (s32Ret != TS_SUCCESS)
		SAMPLE_PRT("SAMPLE_REGION_SetBitMap failed!Handle:%d\n", Handle);

	free(stBitmap.pData);
	return s32Ret;
}

TS_S32 SAMPLE_COMM_REGION_GetUpCanvas(RGN_HANDLE Handle)
{
	TS_S32 s32Ret;
	SIZE_S stSize;
	BITMAP_S stBitmap;
	RGN_CANVAS_INFO_S stCanvasInfo;

	s32Ret = TS_MPI_RGN_GetCanvasInfo(Handle, &stCanvasInfo);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_GetCanvasInfo failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}

	stBitmap.pData   = (TS_VOID *)(TS_UL)stCanvasInfo.u64VirtAddr;
	//stBitmap.pData   = (TS_VOID *)(TS_UL)stCanvasInfo.p32VirtAddr;
	stSize.u32Width  = stCanvasInfo.stSize.u32Width;
	stSize.u32Height = stCanvasInfo.stSize.u32Height;
	REGION_MST_UpdateCanvas(Path_BMP, &stBitmap, TS_FALSE, 0, &stSize, stCanvasInfo.u32Stride,
		PIXEL_FORMAT_ARGB_1555);

	s32Ret = TS_MPI_RGN_UpdateCanvas(Handle);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_MPI_RGN_UpdateCanvas failed with %#x!\n", s32Ret);
		return TS_FAILURE;
	}
	return s32Ret;
}



#define LOG_D(fmt, ...) do {\
		if (0) printf("%s:%d:"fmt, __func__, __LINE__, ##__VA_ARGS__);\
	} while(0);

//region
#define FF_OSD_LINEWIDTH (36)
#define FF_REGION_W (FF_OSD_LINEWIDTH*6)
#define FF_REGION_H (FF_OSD_LINEWIDTH*3)
#define OSD_THREAD_PARAM_NUM (8*3+1)

#define MAX_OSD_CHN_NUM (3)
#define MAX_OSD_TIME_CHN_NUM 4
#define REGION_NUM_PER_CHN (3)

typedef struct {
	int w;
	int h;
	PIXEL_FORMAT_E format;
	char *addr_1;//Y, YUV
	char *addr_2;//U, V, UV, VU
	char *addr_3;//V, U
	int len_1;//Y, YUV
	int len_2;//U, V, UV, VU
	int len_3;//V, U
} FRAME_t;

typedef struct {
	RGN_HANDLE hdl[OSD_THREAD_PARAM_NUM];
	BITMAP_S bitmap1;
	BITMAP_S bitmap2;
	TS_S32 channelID;
	TS_U32 quit;
} OSD_THREAD_PARAM_t;

typedef struct {
	RGN_HANDLE hdl;
	TS_S32 s32DevId;
	TS_S32 channelID;
	PIXEL_FORMAT_E enPixelFormat;
} OSD_THREAD_TIME_PARAM_t;
static pthread_t t_osd_update[MAX_OSD_CHN_NUM];

static pthread_t tid_bak;

static OSD_THREAD_PARAM_t thread_param_osd_update[MAX_OSD_CHN_NUM] = {0};

static RGN_HANDLE ff_hdl = 0;
static TS_U32 g_osd_time_work_flag = 0;
static pthread_mutex_t g_osd_mutex_lock;
static TS_U32 g_sample_venc_terminal_all = 0;
static pthread_t t_osd_update_timestamp[MAX_OSD_TIME_CHN_NUM][MAX_OSD_TIME_CHN_NUM];
static OSD_THREAD_TIME_PARAM_t thread_param_osd_update_time[MAX_OSD_TIME_CHN_NUM][MAX_OSD_TIME_CHN_NUM] = {0};

static int run_flag = 1;

static RGN_HANDLE s_hdl_array[OSD_THREAD_PARAM_NUM] = {-1};
static BITMAP_S s_bitmap = {
	.enPixelFormat = PIXEL_FORMAT_ARGB_8888,
	.u32Width = FF_REGION_W,
	.u32Height = FF_REGION_H,
	.pData = NULL,
};

TS_S32 SAMPLE_COMM_DEBUG_OPEN_REGION(TS_S32 HandleNum, RGN_TYPE_E  enType, MPP_CHN_S *pstChn)
{
	TS_S32         i;
	TS_S32         s32Ret;
	TS_S32         MinHandle;

	//s32Ret = SAMPLE_REGION_MPP_VI_VPSS_VENC_START();
	//if (TS_SUCCESS != s32Ret)
	//	return  s32Ret;

	s32Ret = SAMPLE_COMM_REGION_Create(HandleNum, enType);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("SAMPLE_COMM_REGION_Create failed!\n");
		goto EXIT1;
	}

	s32Ret = SAMPLE_COMM_REGION_AttachToChn(HandleNum, enType, pstChn);
	if (TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");
		goto EXIT2;
	}

	MinHandle = SAMPLE_COMM_REGION_GetMinHandle(enType);
    SAMPLE_PRT("SAMPLE_COMM_REGION_SetBitMap MinHandle=%d, HandleNum=%d\n", MinHandle, HandleNum);
	if (OVERLAY_RGN == enType || OVERLAYEX_RGN == enType) {
		for (i = MinHandle; i < MinHandle + HandleNum; i++) {
			s32Ret = SAMPLE_COMM_REGION_SetBitMap(i, PIXEL_FORMAT_ARGB_8888);
			if (TS_SUCCESS != s32Ret) {
				SAMPLE_PRT("SAMPLE_COMM_REGION_SetBitMap failed!\n");
				goto EXIT2;
			}
		}
	}
	//PAUSE();

    return TS_SUCCESS;

EXIT2:
	s32Ret = SAMPLE_COMM_REGION_DetachFrmChn(HandleNum, enType, pstChn);
	if (TS_SUCCESS != s32Ret)
		SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");
EXIT1:
	s32Ret = SAMPLE_COMM_REGION_Destroy(HandleNum, enType);
	if (TS_SUCCESS != s32Ret)
		SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");

	//SAMPLE_REGION_MPP_VI_VPSS_VENC_END();
	return s32Ret;
}

TS_S32 SAMPLE_COMM_DEBUG_CLOSE_REGION(TS_S32 HandleNum, RGN_TYPE_E  enType, MPP_CHN_S *pstChn)
{
	TS_S32         s32Ret;

    SAMPLE_PRT("SAMPLE_COMM_REGION_DetachFrmChn 1\n");
    s32Ret = SAMPLE_COMM_REGION_DetachFrmChn(HandleNum, enType, pstChn);
    if (TS_SUCCESS != s32Ret)
        SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");

    s32Ret = SAMPLE_COMM_REGION_Destroy(HandleNum, enType);
    if (TS_SUCCESS != s32Ret)
        SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_REGION_Frame2MemSize(FRAME_t *frame)
{
	if (!frame) {
		SAMPLE_PRT("NULL!\n");
		return TS_FAILURE;
	}

	switch (frame->format) {
	case PIXEL_FORMAT_NV_12:
	case PIXEL_FORMAT_YUV_SEMIPLANAR_420:
		return frame->w * frame->h * 3 / 2;
	case PIXEL_FORMAT_ARGB_8888:
		return frame->w * frame->h * 4;
	default:
		SAMPLE_PRT("Unknown foramt %d!\n", frame->format);
		return 0;
	}
}

TS_S32 SAMPLE_COMM_REGION_Alloc_Paint_Bitmap(MPP_CHN_S *mpp_chn, BITMAP_S *bitmap, char R, char G, char B, char A)
{
	int memsize = 0;
	int i, j;
//	char *row_start = NULL;
	int stride = 0;
//	char gray_level = 0x7f;
	FILE *pFile;
	char osd_name[128] = {0};
	size_t writen;
//	char gray_backup = gray_level;
	char *tmp_pixel = NULL;

	if (!bitmap) {
		SAMPLE_PRT("NULL!\n");
		return TS_FAILURE;
	}

	switch(bitmap->enPixelFormat) {
	case PIXEL_FORMAT_RGB_888:
//		stride = bitmap->u32Width * 3;
//		memsize = bitmap->u32Height * stride;
//		bitmap->pData = (char *)malloc(memsize);
//		if (!bitmap->pData) {
//			SAMPLE_PRT("malloc for bitmap failed!\n");
//			return TS_FAILURE;
//		}
//		for (i = 0; i < bitmap->u32Height; ++i) {
//			row_start = bitmap->pData + i * stride;
//			memset(row_start, gray_level, stride);
//			gray_level++;
//		}
		break;
	case PIXEL_FORMAT_ARGB_8888:
		stride = bitmap->u32Width * 4;
		memsize = bitmap->u32Height * stride;
		bitmap->pData = (char *)malloc(memsize);
		if (!bitmap->pData) {
			SAMPLE_PRT("malloc for bitmap failed!\n");
			return TS_FAILURE;
		}
		for (i = 0; i < bitmap->u32Height; ++i) {
			for (j = 0; j < bitmap->u32Width; ++j) {
				tmp_pixel = (bitmap->pData + i * stride) + j * 4;
				*tmp_pixel = B;
				*(tmp_pixel + 1) = G;
				*(tmp_pixel + 2) = R;
				*(tmp_pixel + 3) = A;
			}
		}

		LOG_D("bitmap %p\n", bitmap);
		LOG_D("bitmap->pData %p\n", bitmap->pData);

		do {
			sprintf(osd_name, "bulit_in_osd_chn%d_%dx%d_0x%02x%02x%02x%02x.argb8888",
				mpp_chn->s32DevId, bitmap->u32Width, bitmap->u32Height, A, R, G, B);
			pFile = fopen(osd_name, "wb");
			if(!pFile) {
				SAMPLE_PRT("open file [%s] faild\n", osd_name);
				return TS_FAILURE;
			}
			writen = fwrite(bitmap->pData, 1, memsize, pFile);
			if (writen != memsize) {
				SAMPLE_PRT("writen %zd != memsize %d\n", writen, memsize);
			}
			fclose(pFile);
		} while (0);

		break;
	case PIXEL_FORMAT_ARGB_2BIT:
		stride = bitmap->u32Width;
		memsize = bitmap->u32Height * bitmap->u32Width/4;
		bitmap->pData = (char *)malloc(memsize);
		if (!bitmap->pData) {
			printf("malloc for bitmap failed!\n");
			return TS_FAILURE;
		}
		pFile = fopen("/root/vpe_osd0_128x128_1bit", "rb");
		if(!pFile) {
			printf("open file vpe_osd0_128x128_1bit faild\n");
			return TS_FAILURE;
		}
		writen = fread(bitmap->pData, 1, memsize, pFile);
		if (writen != memsize) {
			printf("writen %zd != memsize %d\n", writen, memsize);
		}
		memset(bitmap->pData,0x00,memsize/4);
		memset(bitmap->pData+memsize/4,0x55,memsize/4);		
		memset(bitmap->pData+memsize/4+memsize/4,0xAA,memsize/4);
		memset(bitmap->pData+memsize/4+memsize/4+memsize/4,0xff,memsize/4);
		fclose(pFile);	
		break;
	default:
		SAMPLE_PRT("Unknown format %d!\n", bitmap->enPixelFormat);
		return TS_FAILURE;
	}

	return TS_SUCCESS;
}


void SAMPLE_COMM_REGION_Osd_Proc_Stop(void)
{
	run_flag = 0;

	SAMPLE_PRT("BF pthread_join\n");

	pthread_join(tid_bak, TS_NULL);

	SAMPLE_PRT("AF pthread_join\n");
}

static void *SAMPLE_COMM_REGION_Osd_Refresh_Time_Proc(void *args)
{
	OSD_THREAD_TIME_PARAM_t *pParam = (OSD_THREAD_TIME_PARAM_t *)args;
	if (!pParam) {
		SAMPLE_PRT("NULL!\n");
		return NULL;
	}

	TS_CHAR name[64];
    snprintf(name, sizeof(name), "Osd_Update_time_Proc_%d", pParam->channelID);
    prctl(PR_SET_NAME, name);
	
    unsigned char length = 0;
    RGN_HANDLE handle = pParam->hdl;
    unsigned char szGetTimeInfo[128] = {0};
	//unsigned char* sTimeBitmapBuf = TS_NULL;
	BITMAP_S stBitMap = {0};
    unsigned char *pStream = TS_NULL;
    int s32Ret = TS_FAILURE;
    int width, height;
    float pixels;
	static float brightness;
	int threshold = 100;
	static bool b_change;
    int bitmap_h;
	VENC_CHN_ATTR_S        stVencChnAttr = {0};
	VIDEO_FRAME_INFO_S 	stFrameInfo = {0};

    while(!g_sample_venc_terminal_all) {
		memset(szGetTimeInfo, 0, sizeof(szGetTimeInfo));
        if (0 != osdGetCurrentTime(szGetTimeInfo, sizeof(szGetTimeInfo), &length)) {
            usleep(1000 * 1000 * 3);
            continue;
        }
        if(g_osd_time_work_flag) {
			RGN_CANVAS_INFO_S stCanvasInfo = {0};
			TS_MPI_VENC_GetChnAttr(pParam->channelID, &stVencChnAttr);
			
		    s32Ret = osdGetSuitableFontSize(stVencChnAttr.stVencAttr.u32MaxPicWidth, stVencChnAttr.stVencAttr.u32MaxPicHeight, &pixels, &bitmap_h);
		    //printf("##pixels=%f, one_word_h=%d\n", pixels, one_word_h);
		    if (s32Ret == 0) {
				printf("osdGetSuitableFontSize failed ! width or height invalid!\n");
				continue;
			}

			s32Ret = TS_MPI_VPSS_GetChnFrame(pParam->s32DevId, pParam->channelID, &stFrameInfo,1000);
			if (s32Ret != 0) {
				printf("TS_MPI_VPSS_GetChnFrame failed !\n");
				continue;
			}
			
			if(pParam->channelID == 0){
				brightness = calculate_nv12_brightness((unsigned char *)(uintptr_t)stFrameInfo.stVFrame.u64VirAddr[0],2560,1440,0,0,500,500);
				//printf("brightness = %0.3f\n",brightness);
				if(brightness < threshold)
					b_change = TS_TRUE;
				else
					b_change = TS_FALSE;
			}
			
			s32Ret = TS_MPI_VPSS_ReleaseChnFrame(pParam->s32DevId,pParam->channelID,&stFrameInfo);
			if (s32Ret != 0) {
				printf("TS_MPI_VPSS_ReleaseChnFrame failed !\n");
				continue;
			}
		    pthread_mutex_lock(&g_osd_mutex_lock);
			if(pParam->enPixelFormat == PIXEL_FORMAT_ARGB_1555){
				pStream = getArgbDataAfterHandleStr(szGetTimeInfo, &width, &height, pixels, bitmap_h, NULL,b_change);
			}else if(pParam->enPixelFormat == PIXEL_FORMAT_ARGB_2BIT){
				pStream = getArgbDataAfterHandleStr_2bit(szGetTimeInfo, &width, &height, pixels, bitmap_h, NULL);
			}
			
		    if (NULL == pStream) {
		        printf(" PDT_MEDIA_RgnLoadIniInfo Error !.");
				pthread_mutex_unlock(&g_osd_mutex_lock);
		        continue;
		    }
			stBitMap.pData = pStream;
		    stBitMap.enPixelFormat = pParam->enPixelFormat;
		    stBitMap.u32Width = width;
		    stBitMap.u32Height = height;

			TS_MPI_RGN_GetCanvasInfo(handle, &stCanvasInfo);
			if(pParam->enPixelFormat == PIXEL_FORMAT_ARGB_1555){
				memcpy((TS_VOID*)(TS_UL)(stCanvasInfo.u64VirtAddr), stBitMap.pData, stBitMap.u32Width * stBitMap.u32Height * 2);
			}else if(pParam->enPixelFormat == PIXEL_FORMAT_ARGB_2BIT){
				memcpy((TS_VOID*)(TS_UL)(stCanvasInfo.u64VirtAddr), stBitMap.pData, stBitMap.u32Width * stBitMap.u32Height / 4);
			}else{
				printf(" this is not  support !\n");
			}
			free(stBitMap.pData);
			pStream = NULL;
			
			TS_MPI_RGN_UpdateCanvas(handle);

		    if(NULL == stBitMap.pData) {
		        printf("stBitMap.pData is free!\n");
		    }

			pthread_mutex_unlock(&g_osd_mutex_lock);
        }
        usleep(990 * 1000);
    }
    return NULL;
}


TS_VOID* SAMPLE_COMM_REGION_Osd_Update_Proc(TS_VOID* param)
{
	OSD_THREAD_PARAM_t *pParam = (OSD_THREAD_PARAM_t *)param;
	if (!pParam) {
		SAMPLE_PRT("NULL!\n");
		return NULL;
	}

	TS_CHAR name[64];
    snprintf(name, sizeof(name), "Osd_Update_Proc_%d", pParam->channelID);
    prctl(PR_SET_NAME, name);

	int i;
	int ret;
	RGN_HANDLE tmp_hdl = -1;
	RGN_CANVAS_INFO_S stCanvasInfo;
	BITMAP_S *bitmap1 = &pParam->bitmap1;
	BITMAP_S *bitmap2 = &pParam->bitmap2;
	BITMAP_S *tmp_bitmap = bitmap1;
	//void *tmp_ptr = NULL;
	int memsize = 0;
	FRAME_t frame = {0};
	int loop_cnt = 0;

	for (i = 0; i < OSD_THREAD_PARAM_NUM; ++i) {
		LOG_D("hdl_array:\n");
		LOG_D("%d ", pParam->hdl[i]);
		LOG_D("\n");
	}

	LOG_D("bitmap1 %p\n", bitmap1);
	LOG_D("bitmap1->pData %p\n", bitmap1->pData);
	LOG_D("bitmap2 %p\n", bitmap2);
	LOG_D("bitmap2->pData %p\n", bitmap2->pData);

	while(!pParam->quit) {
		sleep(5);

		if (!run_flag) break;

		SAMPLE_PRT("update channel_%d, osd, loop count is %d\n", pParam->channelID, loop_cnt);
		if ((loop_cnt) % 2) {
			tmp_bitmap = bitmap1;
		} else {
			tmp_bitmap = bitmap2;
		}
		loop_cnt ++;

		for (i = 0; i < REGION_NUM_PER_CHN; ++i) {
			if (-1 == pParam->hdl[i]) {
				break;
			}
			tmp_hdl = pParam->hdl[i];

			//LOG_D("tmp_hdl %d\n", tmp_hdl);

			ret = TS_MPI_RGN_GetCanvasInfo(tmp_hdl, &stCanvasInfo);
			if (TS_SUCCESS != ret) {
				SAMPLE_PRT("TS_MPI_RGN_GetCanvasInfo failed with %#x!\n", ret);
				break;
			}

			frame.w = stCanvasInfo.stSize.u32Width;
			frame.h = stCanvasInfo.stSize.u32Height;
			frame.format = stCanvasInfo.enPixelFmt;
			//printf("frame.w=%d\n",frame.w);
			//printf("frame.h=%d\n",frame.h);
			//LOG_D("VirtAddr %p, W %d, H%d, format %d\n", (TS_VOID*)(TS_UL)(stCanvasInfo.u64VirtAddr),
			//	frame.w, frame.h, frame.format);
			//LOG_D("pData %p\n", tmp_bitmap->pData);
			memsize = SAMPLE_COMM_REGION_Frame2MemSize(&frame);
			memcpy((TS_VOID*)(TS_UL)(stCanvasInfo.u64VirtAddr), tmp_bitmap->pData,
					memsize);
			//LOG_D("memcpy end, VirtAddr %lld, pData %p, u32CanvMemSize %d, tmp %p\n", stCanvasInfo.u64VirtAddr,
			//	tmp_bitmap->pData, memsize, tmp_ptr);

			ret = TS_MPI_RGN_UpdateCanvas(tmp_hdl);
			if (TS_SUCCESS != ret) {
				SAMPLE_PRT("TS_MPI_RGN_UpdateCanvas failed with %#x!\n", ret);
				break;
			}

		}
	}

	return NULL;
}

TS_S32 SAMPLE_COMM_REGION_Osd_Update_Init(RGN_HANDLE hdl[], MPP_CHN_S *mpp_chn, BITMAP_S *bitmap1, BITMAP_S *bitmap2)
{
	int ret;

	if (!bitmap1 || !bitmap2) {
		SAMPLE_PRT("NULL!\n");
		return TS_FAILURE;
	}
	if (mpp_chn->s32DevId > MAX_OSD_CHN_NUM || mpp_chn->s32DevId < 0)
	{
		SAMPLE_PRT("s32DevId err is %d\n", mpp_chn->s32DevId);
		return TS_FAILURE;
	}

	memcpy(thread_param_osd_update[mpp_chn->s32DevId].hdl, hdl, sizeof(thread_param_osd_update[mpp_chn->s32DevId].hdl));

	thread_param_osd_update[mpp_chn->s32DevId].bitmap1 = *bitmap1;
	thread_param_osd_update[mpp_chn->s32DevId].bitmap2 = *bitmap2;
	thread_param_osd_update[mpp_chn->s32DevId].channelID = mpp_chn->s32DevId;
	thread_param_osd_update[mpp_chn->s32DevId].quit	 = 0;
//	memcpy(&thread_param_osd_update.bitmap1, bitmap1, sizeof(thread_param_osd_update.bitmap1));
//	memcpy(&thread_param_osd_update.bitmap2, bitmap2, sizeof(thread_param_osd_update.bitmap2));

	ret = pthread_create(&t_osd_update[mpp_chn->s32DevId], 0, SAMPLE_COMM_REGION_Osd_Update_Proc, (void *)&thread_param_osd_update[mpp_chn->s32DevId]);
	if (ret != TS_SUCCESS) {
		SAMPLE_PRT("pthread_create osd_update_worker failed !\n");
		return TS_FAILURE;
	}

	tid_bak = t_osd_update[mpp_chn->s32DevId];

	return TS_SUCCESS;
}
TS_S32 SAMPLE_COMM_REGION_Dettach_Dynamic_Rgn(MPP_CHN_S *mpp_chn)
{
	int ret;
	int i;

	if(NULL == mpp_chn)
	{
		SAMPLE_PRT("SAMPLE_COMM_REGION_Dettach_Dynamic_Rgn mpp_chn is NULL!\n");
		return TS_FAILURE;
	}

	thread_param_osd_update[mpp_chn->s32DevId].quit	 = 1;
	if(t_osd_update[mpp_chn->s32DevId] >= 0){
		pthread_join(t_osd_update[mpp_chn->s32DevId], 0);
	}

	for (i = 0; i < REGION_NUM_PER_CHN; ++i) 
	{
		ret = TS_MPI_RGN_DetachFromChn(thread_param_osd_update[mpp_chn->s32DevId].hdl[i], mpp_chn);
		if (TS_SUCCESS != ret) {
			SAMPLE_PRT("APP_REGION_DetachFromChn failed! Handle:%d\n", i);
		}
		ret = TS_MPI_RGN_Destroy(thread_param_osd_update[mpp_chn->s32DevId].hdl[i]);
		if (TS_SUCCESS != ret) {
			SAMPLE_PRT("TS_MPI_RGN_Destroy failed! Handle:%d\n", i);
		}
	}
	if(mpp_chn->s32DevId == 0)
		ff_hdl_ext[mpp_chn->s32DevId] = 0;
	else
		ff_hdl_ext[mpp_chn->s32DevId] = 3;

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_REGION_Attach_Dynamic_Rgn(MPP_CHN_S *mpp_chn, bool ex)
{
	int ret;
	int i;
	RGN_HANDLE hdl_array[OSD_THREAD_PARAM_NUM] = {-1};
	for (i = 0; i < OSD_THREAD_PARAM_NUM;i++) {
		hdl_array[i] = -1;
	}
	int hdl_array_idx = 0;
	int multiple_x = 0;
	int multiple_y = 0;

	RGN_ATTR_S region = {
		.enType = (ex ? OVERLAYEX_RGN : OVERLAY_RGN),
		.unAttr = {
			.stOverlay = {
				.enPixelFmt = PIXEL_FORMAT_ARGB_8888,
				.u32BgColor = 0,
				.stSize = {
					.u32Width = FF_REGION_W,
					.u32Height = FF_REGION_H,
				},
				.u32CanvasNum = 2,
			},
		},
	};
	BITMAP_S bitmap = {
		.enPixelFormat = PIXEL_FORMAT_ARGB_8888,
		.u32Width = FF_REGION_W,
		.u32Height = FF_REGION_H,
		.pData = NULL,
	};
	BITMAP_S bitmap2 = {
		.enPixelFormat = PIXEL_FORMAT_ARGB_8888,
		.u32Width = FF_REGION_W,
		.u32Height = FF_REGION_H,
		.pData = NULL,
	};
	RGN_CHN_ATTR_S chn_attr = {
		.bShow = true,
		.enType = (ex ? OVERLAYEX_RGN : OVERLAY_RGN),
		.unChnAttr = {
			.stOverlayChn = {
				.stPoint = {0, 0},//care
				.u32FgAlpha = 0,
				.u32BgAlpha = 0,
				.u32Layer = 0,
				.stQpInfo = {},
				.stInvertColor = {},
				.enAttachDest = 0,
				.u16ColorLUT = {},
			},
		},
	};

	if(NULL == bitmap.pData)
	{
		ret = SAMPLE_COMM_REGION_Alloc_Paint_Bitmap(mpp_chn, &bitmap, 0xff, 0, 0, 0xff);
		if (ret != TS_SUCCESS) {
			SAMPLE_PRT("SAMPLE_COMM_REGION_Alloc_Paint_Bitmap failed %d!\n", ret);
			return TS_FAILURE;
		}
	}

	if(NULL == bitmap2.pData)
	{
		ret = SAMPLE_COMM_REGION_Alloc_Paint_Bitmap(mpp_chn, &bitmap2, 0, 0xff, 0, 0xff);
		if (ret != TS_SUCCESS) {
			SAMPLE_PRT("SAMPLE_COMM_REGION_Alloc_Paint_Bitmap failed %d!\n", ret);
			return TS_FAILURE;
		}
	}

#ifdef OSD_RESTRUCT
	ret = TS_MPI_RGN_BatchBegin(mpp_chn->s32DevId, mpp_chn->s32ChnId);
	if (ret != TS_SUCCESS) {
		SAMPLE_PRT("TS_MPI_RGN_BatchBegin failed %d!\n", ret);
		return TS_FAILURE;
	}
#endif
	VENC_CHN_ATTR_S        stVencChnAttr = {0};
	TS_MPI_VENC_GetChnAttr(mpp_chn->s32ChnId, &stVencChnAttr);

	multiple_x = stVencChnAttr.stVencAttr.u32MaxPicWidth / FF_REGION_W;
	multiple_y = stVencChnAttr.stVencAttr.u32MaxPicHeight / FF_REGION_H;

	for (i = 0; i < REGION_NUM_PER_CHN; ++i) {

		if(i == 1){
			chn_attr.unChnAttr.stOverlayChn.stPoint.s32X = (multiple_x - 1) * FF_REGION_W;
			chn_attr.unChnAttr.stOverlayChn.stPoint.s32Y = (multiple_y - 1) * FF_REGION_H;
			//printf("[s32X = %d,s32Y = %d]\n",chn_attr.unChnAttr.stOverlayChn.stPoint.s32X,chn_attr.unChnAttr.stOverlayChn.stPoint.s32Y);
		}else if(i == 0){
			chn_attr.unChnAttr.stOverlayChn.stPoint.s32X = FF_REGION_W + FF_OSD_LINEWIDTH;
			chn_attr.unChnAttr.stOverlayChn.stPoint.s32Y = FF_REGION_H;
			//printf("[s32X = %d,s32Y = %d]\n",chn_attr.unChnAttr.stOverlayChn.stPoint.s32X,chn_attr.unChnAttr.stOverlayChn.stPoint.s32Y);
		}else if(i == 2){
			chn_attr.unChnAttr.stOverlayChn.stPoint.s32X = (multiple_x - 1) * FF_REGION_W;
			chn_attr.unChnAttr.stOverlayChn.stPoint.s32Y = FF_REGION_H;
			//printf("[s32X = %d,s32Y = %d]\n",chn_attr.unChnAttr.stOverlayChn.stPoint.s32X,chn_attr.unChnAttr.stOverlayChn.stPoint.s32Y);
		}

		ret = TS_MPI_RGN_Create(ff_hdl_ext[mpp_chn->s32DevId], &region);
		if (ret != TS_SUCCESS) {
			SAMPLE_PRT("TS_MPI_RGN_Create failed %d!\n", ret);
			return TS_FAILURE;
		}

		ret = TS_MPI_RGN_SetBitMap(ff_hdl_ext[mpp_chn->s32DevId], &bitmap);
		if (ret != TS_SUCCESS) {
			SAMPLE_PRT("TS_MPI_RGN_SetBitMap failed %d!\n", ret);
			return TS_FAILURE;
		}

		ret = TS_MPI_RGN_AttachToChn(ff_hdl_ext[mpp_chn->s32DevId], mpp_chn, &chn_attr);
		if (ret != TS_SUCCESS) {
			SAMPLE_PRT("TS_MPI_RGN_AttachToChn failed %d!\n", ret);
			return TS_FAILURE;
		}

		hdl_array[hdl_array_idx++] = ff_hdl_ext[mpp_chn->s32DevId];
		ff_hdl_ext[mpp_chn->s32DevId]++;
	}

#ifdef OSD_RESTRUCT
	ret = TS_MPI_RGN_BatchEnd(mpp_chn->s32DevId, mpp_chn->s32ChnId);
	if (ret != TS_SUCCESS) {
		SAMPLE_PRT("TS_MPI_RGN_BatchEnd failed %d!\n", ret);
		return TS_FAILURE;
	}
#endif

	for (i = 0; i < OSD_THREAD_PARAM_NUM; ++i) {
		LOG_D("hdl_array:\n");
		LOG_D("%d \n", hdl_array[i]);
		LOG_D("\n");
	}

	ret = SAMPLE_COMM_REGION_Osd_Update_Init(hdl_array, mpp_chn, &bitmap, &bitmap2);
	if (ret != TS_SUCCESS) {
		SAMPLE_PRT("osd_update_init failed %d!\n", ret);
		return TS_FAILURE;
	}

	memcpy(s_hdl_array, hdl_array, sizeof(hdl_array));

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_REGION_Osd_Update_Time(RGN_HANDLE hdl,MPP_CHN_S *mpp_chn,PIXEL_FORMAT_E enPixelFormat)
{
	int ret;
	
	if (mpp_chn->s32DevId > MAX_OSD_CHN_NUM || mpp_chn->s32DevId < 0)
	{
		SAMPLE_PRT("s32DevId err is %d\n", mpp_chn->s32DevId);
		return TS_FAILURE;
	}
	//memcpy(thread_param_osd_update_time[mpp_chn->s32DevId][mpp_chn->s32ChnId].hdl, hdl, sizeof(thread_param_osd_update_time[mpp_chn->s32DevId][mpp_chn->s32ChnId].hdl));
	thread_param_osd_update_time[mpp_chn->s32DevId][mpp_chn->s32ChnId].hdl = hdl;
	thread_param_osd_update_time[mpp_chn->s32DevId][mpp_chn->s32ChnId].s32DevId = mpp_chn->s32DevId;
	thread_param_osd_update_time[mpp_chn->s32DevId][mpp_chn->s32ChnId].channelID = mpp_chn->s32ChnId;
	thread_param_osd_update_time[mpp_chn->s32DevId][mpp_chn->s32ChnId].enPixelFormat = enPixelFormat;
	ret = pthread_create(&t_osd_update_timestamp[mpp_chn->s32DevId][mpp_chn->s32ChnId], 0, SAMPLE_COMM_REGION_Osd_Refresh_Time_Proc, (void *)&thread_param_osd_update_time[mpp_chn->s32DevId][mpp_chn->s32ChnId]);
	if (ret != TS_SUCCESS) {
		SAMPLE_PRT("pthread_create osd_update_worker failed !\n");
		return TS_FAILURE;
	}

	return TS_SUCCESS;
}
TS_S32 SAMPLE_COMM_REGION_Osd_Timestamp(MPP_CHN_S *mpp_chn, bool ex,PIXEL_FORMAT_E enPixelFormat)
{
	int ret;
	//RGN_HANDLE hdl_array[OSD_THREAD_PARAM_NUM] = {-1};
	//int hdl_array_idx = 0;
	
	pthread_mutex_init(&g_osd_mutex_lock, NULL);
	g_osd_time_work_flag = 1;
	RGN_ATTR_S region = {
		.enType = (ex ? OVERLAYEX_RGN : OVERLAY_RGN),
		.unAttr = {
			.stOverlay = {
				.enPixelFmt = enPixelFormat,
				.u32BgColor = 0,
				.stSize = {
					.u32Width = FF_REGION_W,
					.u32Height = FF_REGION_H,
				},
				.u32CanvasNum = 2,
			},
		},
	};
	BITMAP_S bitmap = {
		.enPixelFormat = enPixelFormat,
		.u32Width = FF_REGION_W,
		.u32Height = FF_REGION_H,
		.pData = NULL,
	};

	RGN_CHN_ATTR_S chn_attr = {
		.bShow = true,
		.enType = (ex ? OVERLAYEX_RGN : OVERLAY_RGN),
		.unChnAttr = {
			.stOverlayChn = {
				.stPoint = {0, 0},//care
				.u32FgAlpha = 0,
				.u32BgAlpha = 0,
				.u32Layer = 0,
				.stQpInfo = {},
				.stInvertColor = {},
				.enAttachDest = 0,
				.u16ColorLUT = {},
			},
		},
	};

#ifdef OSD_RESTRUCT
	ret = TS_MPI_RGN_BatchBegin(mpp_chn->s32DevId, mpp_chn->s32ChnId);
	if (ret != TS_SUCCESS) {
		LOG_D("TS_MPI_RGN_BatchBegin failed %d!\n", ret);
		return TS_FAILURE;
	}
#endif

	VENC_CHN_ATTR_S        stVencChnAttr = {0};
	TS_MPI_VENC_GetChnAttr(mpp_chn->s32ChnId, &stVencChnAttr);


	unsigned char length = 0;
    unsigned char szGetTimeInfo[128] = {0};
    int neededYHeight, width, height, bitmap_w, bitmap_h, one_word_h;
	float pixels;
	unsigned char *pStream = TS_NULL;
	//printf("stVencChnAttr.stVencAttr.u32MaxPicWidth=%d\n",stVencChnAttr.stVencAttr.u32MaxPicWidth);
	//if(mpp_chn->s32ChnId >= 1){
	//	stVencChnAttr.stVencAttr.u32MaxPicWidth=640;
	//}
	neededYHeight = osdGetSuitableFontSize(stVencChnAttr.stVencAttr.u32MaxPicWidth, stVencChnAttr.stVencAttr.u32MaxPicHeight, &pixels, &one_word_h);
    //LOG("##pixels=%f, one_word_h=%d\n", pixels, one_word_h);
    if (neededYHeight == 0) {
		LOG_D("osdGetSuitableFontSize failed ! width or height invalid!\n");
		return TS_FAILURE;
	}
	
	if(enPixelFormat == PIXEL_FORMAT_ARGB_2BIT){
		osdGetTimeStringBitmapSize_2bit(pixels, one_word_h, &bitmap_w, &bitmap_h);
	}else if(enPixelFormat == PIXEL_FORMAT_ARGB_1555){
		osdGetTimeStringBitmapSize_1555(pixels, one_word_h, &bitmap_w, &bitmap_h);
	}else{
		LOG_D("this PixelFormat not support!\n");
		return -1;
	}
    
	region.unAttr.stOverlay.stSize.u32Width = bitmap_w;
	region.unAttr.stOverlay.stSize.u32Height = bitmap_h;
	//printf("bitmap_w=%d,bitmap_h=%d\n",bitmap_w,bitmap_h);
	//printf("neededYHeight=%d\n",neededYHeight);
	ret = TS_MPI_RGN_Create(ff_hdl, &region);
	if (ret != TS_SUCCESS) {
		LOG_D("TS_MPI_RGN_Create %d failed %d!\n", ff_hdl, ret);
		return TS_FAILURE;
	}

	//bitmap
	osdGetCurrentTime(szGetTimeInfo, sizeof(szGetTimeInfo), &length);
	if(enPixelFormat == PIXEL_FORMAT_ARGB_2BIT){
    	pStream = getArgbDataAfterHandleStr_2bit((unsigned char *)&szGetTimeInfo, &width, &height, pixels, bitmap_h, TS_NULL);
	}else if(enPixelFormat == PIXEL_FORMAT_ARGB_1555){
		pStream = getArgbDataAfterHandleStr((unsigned char *)&szGetTimeInfo, &width, &height, pixels, bitmap_h, TS_NULL, TS_TRUE);
	}else{
		LOG_D("this PixelFormat not support!\n");
		return -2;
	}
    bitmap.pData = pStream;
    bitmap.enPixelFormat = enPixelFormat;
    bitmap.u32Width = width;
    bitmap.u32Height = height;

	ret = TS_MPI_RGN_SetBitMap(ff_hdl, &bitmap);
	if (ret != TS_SUCCESS) {
		LOG_D("TS_MPI_RGN_SetBitMap %d failed %d!\n", ff_hdl, ret);
		return TS_FAILURE;
	}

	free(bitmap.pData);
	pStream = NULL;
	
    memset(&chn_attr, 0, sizeof(RGN_CHN_ATTR_S));
    chn_attr.bShow = TS_TRUE;
    chn_attr.enType = OVERLAY_RGN;
    chn_attr.unChnAttr.stOverlayChn.stPoint.s32X = 16;
    chn_attr.unChnAttr.stOverlayChn.stPoint.s32Y = neededYHeight;
    chn_attr.unChnAttr.stOverlayChn.u32BgAlpha = 0;
    chn_attr.unChnAttr.stOverlayChn.u32FgAlpha = 0;
    chn_attr.unChnAttr.stOverlayChn.stQpInfo.bQpDisable = TS_FALSE;
    chn_attr.unChnAttr.stOverlayChn.stQpInfo.bAbsQp = TS_TRUE;
    chn_attr.unChnAttr.stOverlayChn.stQpInfo.s32Qp  = 30;
    chn_attr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Height = 16;
    chn_attr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Width = 16;
    chn_attr.unChnAttr.stOverlayChn.stInvertColor.u32LumThresh = 128;
	
	ret = TS_MPI_RGN_AttachToChn(ff_hdl, mpp_chn, &chn_attr);
	if (ret != TS_SUCCESS) {
		LOG_D("TS_MPI_RGN_AttachToChn %d failed %d!\n", ff_hdl, ret);
		return TS_FAILURE;
	}
	
	
#ifdef OSD_RESTRUCT
	ret = TS_MPI_RGN_BatchEnd(mpp_chn->s32DevId, mpp_chn->s32ChnId);
	if (ret != TS_SUCCESS) {
		LOG_D("TS_MPI_RGN_BatchEnd failed %d!\n", ret);
		return TS_FAILURE;
	}
#endif

	if(enPixelFormat == PIXEL_FORMAT_ARGB_2BIT){
		VENC_ROI_ATTR_S stRoiAttr = {};
		
		if((mpp_chn->s32DevId == 0 ) && (mpp_chn->s32ChnId == 0)){
			ret = TS_MPI_VENC_GetRoiAttr(0, 0,&stRoiAttr);
		}else if((mpp_chn->s32DevId == 1 ) && (mpp_chn->s32ChnId == 0)){
			ret = TS_MPI_VENC_GetRoiAttr(1, 0,&stRoiAttr);
		}
		
		if (TS_SUCCESS != ret) {
			SAMPLE_PRT("Get Roi Attr failed for %#x!", ret);
			
		}
		stRoiAttr.bAbsQp   = TS_FALSE;//TS_TRUE;
		stRoiAttr.bEnable  = 1;
		stRoiAttr.s32Qp    = 37;
		stRoiAttr.u32Index = 0;
		stRoiAttr.stRect.u32Height = bitmap_h;//水印高 16位对齐
		stRoiAttr.stRect.u32Width = bitmap_w; //水印宽 16位对齐
		if((mpp_chn->s32DevId == 0 ) && (mpp_chn->s32ChnId == 0)){
			ret = TS_MPI_VENC_SetRoiAttr(0, &stRoiAttr);
		}else if((mpp_chn->s32DevId == 1 ) && (mpp_chn->s32ChnId == 0)){
			ret = TS_MPI_VENC_SetRoiAttr(1, &stRoiAttr);
		}
		
		if (TS_SUCCESS != ret)
		{
			SAMPLE_PRT("Set Roi Attr failed for %#x!", ret);
			
		}
	}
	
	ret = SAMPLE_COMM_REGION_Osd_Update_Time(ff_hdl,mpp_chn,enPixelFormat);
	if (ret != TS_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_REGION_Osd_Update_Time failed %d!\n", ret);
		return TS_FAILURE;
	}
	ff_hdl++;
	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_REGION_Dettach_Common_Rgn(MPP_CHN_S *mpp_chn)
{
	int ret;
	int i;

	if(NULL == mpp_chn)
	{
		SAMPLE_PRT("SAMPLE_COMM_REGION_Dettach_Common_Rgn mpp_chn is NULL!\n");
		return TS_FAILURE;
	}
	
	if(g_osd_time_work_flag == 1){
		g_osd_time_work_flag = TS_FALSE;
		g_sample_venc_terminal_all = 1;
        pthread_join(t_osd_update_timestamp[mpp_chn->s32DevId][mpp_chn->s32ChnId], 0);
	}


	if (TS_SUCCESS != TS_MPI_RGN_DetachAllOSDInChn(mpp_chn)) {
		SAMPLE_PRT("TS_MPI_RGN_DetachAllOSDInChn failed!\n");
	} else {
		SAMPLE_PRT("TS_MPI_RGN_DetachAllOSDInChn OK.\n");
	}

	for (i = 0; i < ff_hdl; ++i) {
	
		ret = TS_MPI_RGN_DetachFromChn(i, mpp_chn);
		if (TS_SUCCESS != ret) {
			SAMPLE_PRT("APP_REGION_DetachFromChn failed! Handle:%d\n", i);
		}
		ret = TS_MPI_RGN_Destroy(i);
		if (TS_SUCCESS != ret) {
			SAMPLE_PRT("TS_MPI_RGN_Destroy failed! Handle:%d\n", i);
		}
	}
	ff_hdl = 0;
	
	return TS_SUCCESS;
}


TS_S32 SAMPLE_COMM_REGION_Osd_Chinese(MPP_CHN_S *mpp_chn, bool ex,PIXEL_FORMAT_E enPixelFormat, int osd_num, int word_width)
{
	int ret;
	RGN_ATTR_S region = {
		.enType = (ex ? OVERLAYEX_RGN : OVERLAY_RGN),
		.unAttr = {
			.stOverlay = {
				.enPixelFmt = enPixelFormat,
				.u32BgColor = 0,
				.stSize = {
					.u32Width = FF_REGION_W,
					.u32Height = FF_REGION_H,
				},
				.u32CanvasNum = 2,
			},
		},
	};
	BITMAP_S bitmap = {
		.enPixelFormat = enPixelFormat,
		.u32Width = FF_REGION_W,
		.u32Height = FF_REGION_H,
		.pData = NULL,
	};

	RGN_CHN_ATTR_S chn_attr = {
		.bShow = true,
		.enType = (ex ? OVERLAYEX_RGN : OVERLAY_RGN),
		.unChnAttr = {
			.stOverlayChn = {
				.stPoint = {0, 0},//care
				.u32FgAlpha = 0,
				.u32BgAlpha = 0,
				.u32Layer = 0,
				.stQpInfo = {},
				.stInvertColor = {},
				.enAttachDest = 0,
				.u16ColorLUT = {},
			},
		},
	};

#ifdef OSD_RESTRUCT
	ret = TS_MPI_RGN_BatchBegin(mpp_chn->s32DevId, mpp_chn->s32ChnId);
	if (ret != TS_SUCCESS) {
		LOG_D("TS_MPI_RGN_BatchBegin failed %d!\n", ret);
		return TS_FAILURE;
	}
#endif

	VENC_CHN_ATTR_S        stVencChnAttr = {0};
	TS_MPI_VENC_GetChnAttr(mpp_chn->s32ChnId, &stVencChnAttr);

	unsigned char szGetTimeInfo[128] = {"在一个区域设置覆盖区域设置三二个汉字主码流副码流子码流都正常显示"};
	int neededYHeight, width, height, bitmap_w, bitmap_h, one_word_h;
	float pixels;
	unsigned char *pStream = TS_NULL;

	neededYHeight = osdGetSuitableFontSize(stVencChnAttr.stVencAttr.u32MaxPicWidth, stVencChnAttr.stVencAttr.u32MaxPicHeight, &pixels, &one_word_h);
	switch(word_width){
		case 16:
			pixels = 16;
			one_word_h = 20;
			break;
		case 32:
			pixels = 30;
			one_word_h = 30;
			break;
		case 64:
			pixels = 76;
			one_word_h = 76;
			neededYHeight = 38;
			break;
		default:
			break;
	}
	osdGetCustomStringBitmapSize_1555(pixels, szGetTimeInfo, one_word_h, &bitmap_w, &bitmap_h);

	region.unAttr.stOverlay.stSize.u32Width = bitmap_w;
	region.unAttr.stOverlay.stSize.u32Height = bitmap_h;
	pStream = getArgbDataAfterHandleStr((unsigned char *)&szGetTimeInfo, &width, &height, pixels, bitmap_h, TS_NULL, TS_TRUE);

	bitmap.pData = pStream;
	bitmap.enPixelFormat = enPixelFormat;
	bitmap.u32Width = width;
	bitmap.u32Height = height;
	if(osd_num != 1 && osd_num != 3 && osd_num != 5)
		osd_num = 1;
	for (int i = 0; i < osd_num; ++i) {
		ret = TS_MPI_RGN_Create(ff_hdl, &region);
		if (ret != TS_SUCCESS) {
			LOG_D("TS_MPI_RGN_Create %d failed %d!\n", ff_hdl, ret);
			return TS_FAILURE;
		}

		ret = TS_MPI_RGN_SetBitMap(ff_hdl, &bitmap);
		if (ret != TS_SUCCESS) {
			LOG_D("TS_MPI_RGN_SetBitMap %d failed %d!\n", ff_hdl, ret);
			return TS_FAILURE;
		}

		memset(&chn_attr, 0, sizeof(RGN_CHN_ATTR_S));
		chn_attr.bShow = TS_TRUE;
		chn_attr.enType = OVERLAY_RGN;
		chn_attr.unChnAttr.stOverlayChn.stPoint.s32X = 16;
		chn_attr.unChnAttr.stOverlayChn.stPoint.s32Y = neededYHeight + i*neededYHeight*2;
		chn_attr.unChnAttr.stOverlayChn.u32BgAlpha = 0;
		chn_attr.unChnAttr.stOverlayChn.u32FgAlpha = 0;
		chn_attr.unChnAttr.stOverlayChn.stQpInfo.bQpDisable = TS_FALSE;
		chn_attr.unChnAttr.stOverlayChn.stQpInfo.bAbsQp = TS_TRUE;
		chn_attr.unChnAttr.stOverlayChn.stQpInfo.s32Qp  = 30;
		chn_attr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Height = 16;
		chn_attr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Width = 16;
		chn_attr.unChnAttr.stOverlayChn.stInvertColor.u32LumThresh = 128;
		
		ret = TS_MPI_RGN_AttachToChn(ff_hdl, mpp_chn, &chn_attr);
		if (ret != TS_SUCCESS) {
			LOG_D("TS_MPI_RGN_AttachToChn %d failed %d!\n", ff_hdl, ret);
			return TS_FAILURE;
		}
	
		ff_hdl++;
	}
	
	free(bitmap.pData);
	pStream = NULL;
	
#ifdef OSD_RESTRUCT
	ret = TS_MPI_RGN_BatchEnd(mpp_chn->s32DevId, mpp_chn->s32ChnId);
	if (ret != TS_SUCCESS) {
		LOG_D("TS_MPI_RGN_BatchEnd failed %d!\n", ret);
		return TS_FAILURE;
	}
#endif
	return TS_SUCCESS;
}



TS_S32 SAMPLE_COMM_REGION_Start_Rgn(MPP_CHN_S *mpp_chn, bool ex, int type)
{
	int ret;
	int i;
	int hdl_array_idx = 0;

	RGN_ATTR_S region = {
		.enType = (ex ? OVERLAYEX_RGN : OVERLAY_RGN),
		.unAttr = {
			.stOverlay = {
				.enPixelFmt = PIXEL_FORMAT_ARGB_8888,
				.u32BgColor = 0,
				.stSize = {
					.u32Width = FF_REGION_W,
					.u32Height = FF_REGION_H,
				},
				.u32CanvasNum = 2,
			},
		},
	};

	RGN_CHN_ATTR_S chn_attr = {
		.bShow = true,
		.enType = (ex ? OVERLAYEX_RGN : OVERLAY_RGN),
		.unChnAttr = {
			.stOverlayChn = {
				.stPoint = {0, 0},//care
				.u32FgAlpha = 0,
				.u32BgAlpha = 0,
				.u32Layer = 0,
				.stQpInfo = {},
				.stInvertColor = {},
				.enAttachDest = 0,
				.u16ColorLUT = {},
			},
		},
	};

	RGN_ATTR_S region_cover = {
		.enType = (ex ? COVEREX_RGN : COVER_RGN),
		.unAttr = {},
	};

	RGN_CHN_ATTR_S chn_attr_cover = {
		.bShow = true,
		.enType = (ex ? COVEREX_RGN : COVER_RGN),
		.unChnAttr = {
			.stCoverChn = {
				.enCoverType = AREA_RECT,
				.stRect = {0, FF_REGION_H + 64, FF_REGION_W, FF_REGION_H},//care
				.u32Color = 0,
				.u32Layer = 0,
				.enCoordinate = RGN_ABS_COOR,
			},
		},
	};

	RGN_ATTR_S region_mosaic = {
		.enType = (ex ? MOSAICEX_RGN : MOSAIC_RGN),
		.unAttr = {},
	};

	RGN_CHN_ATTR_S chn_attr_mosaic = {
		.bShow = true,
		.enType = (ex ? MOSAICEX_RGN : MOSAIC_RGN),
		.unChnAttr = {
			.stMosaicChn = {
				.stRect = {0, (FF_REGION_H + 64)*2, FF_REGION_W, FF_REGION_H},//care
				.enBlkSize = MOSAIC_BLK_SIZE_64,
				.u32Layer = 0,
			},
		},
	};

	if (type == 0) {
			ret = SAMPLE_COMM_REGION_Alloc_Paint_Bitmap(mpp_chn, &s_bitmap, 0x7f, 0x7f, 0x7f, 0xff);
			if (ret != TS_SUCCESS) {
				SAMPLE_PRT("SAMPLE_COMM_REGION_Alloc_Paint_Bitmap failed %d!\n", ret);
				return TS_FAILURE;
			}

		//	ret = TS_MPI_RGN_BatchBegin(mpp_chn->s32ChnId);
		//	if (ret != TS_SUCCESS) {
		//		SAMPLE_PRT("TS_MPI_RGN_BatchBegin failed %d!\n", ret);
		//		return TS_FAILURE;
		//	}

			for (i = 0; i < 8; ++i) {
				chn_attr.unChnAttr.stOverlayChn.stPoint.s32X = i * (FF_REGION_W + FF_OSD_LINEWIDTH);
		//		chn_attr.unChnAttr.stOverlayChn.stPoint.s32Y = i * FF_OSD_LINEWIDTH;

				ret = TS_MPI_RGN_Create(ff_hdl, &region);
				if (ret != TS_SUCCESS) {
					SAMPLE_PRT("TS_MPI_RGN_Create failed %d!\n", ret);
					return TS_FAILURE;
				}

				ret = TS_MPI_RGN_SetBitMap(ff_hdl, &s_bitmap);
				if (ret != TS_SUCCESS) {
					SAMPLE_PRT("TS_MPI_RGN_Create failed %d!\n", ret);
					return TS_FAILURE;
				}

				ret = TS_MPI_RGN_AttachToChn(ff_hdl, mpp_chn, &chn_attr);
				if (ret != TS_SUCCESS) {
					SAMPLE_PRT("TS_MPI_RGN_Create failed %d!\n", ret);
					return TS_FAILURE;
				}

				s_hdl_array[hdl_array_idx++] = ff_hdl;
				ff_hdl++;
			}
	}

	if (type == 1) {
		for (i = 0; i < 8; ++i) {
			chn_attr_cover.unChnAttr.stCoverChn.stRect.s32X = i * (FF_REGION_W + FF_OSD_LINEWIDTH);
			//white: 0xffffffff, black: 0x00000000, green: 0x00ff0000
			chn_attr_cover.unChnAttr.stCoverChn.u32Color = 0xff<<((i%4)*8);
			printf("color argb 0x%x", chn_attr_cover.unChnAttr.stCoverChn.u32Color);

			ret = TS_MPI_RGN_Create(ff_hdl, &region_cover);
			if (ret != TS_SUCCESS) {
				SAMPLE_PRT("TS_MPI_RGN_Create failed %d!\n", ret);
				return TS_FAILURE;
			}

			ret = TS_MPI_RGN_AttachToChn(ff_hdl, mpp_chn, &chn_attr_cover);
			if (ret != TS_SUCCESS) {
				SAMPLE_PRT("TS_MPI_RGN_Create failed %d!\n", ret);
				return TS_FAILURE;
			}

			s_hdl_array[hdl_array_idx++] = ff_hdl;
			ff_hdl++;
		}
	}

	if (type == 2) {
		for (i = 0; i < 8; ++i) {
			chn_attr_mosaic.unChnAttr.stMosaicChn.stRect.s32X = i * (FF_REGION_W + FF_OSD_LINEWIDTH);
			chn_attr_mosaic.unChnAttr.stMosaicChn.enBlkSize = i;
			if (chn_attr_mosaic.unChnAttr.stMosaicChn.enBlkSize >= MOSAIC_BLK_SIZE_BUTT) {
				chn_attr_mosaic.unChnAttr.stMosaicChn.enBlkSize = MOSAIC_BLK_SIZE_64;
			}

			ret = TS_MPI_RGN_Create(ff_hdl, &region_mosaic);
			if (ret != TS_SUCCESS) {
				SAMPLE_PRT("TS_MPI_RGN_Create failed %d!\n", ret);
				return TS_FAILURE;
			}

			ret = TS_MPI_RGN_AttachToChn(ff_hdl, mpp_chn, &chn_attr_mosaic);
			if (ret != TS_SUCCESS) {
				SAMPLE_PRT("TS_MPI_RGN_Create failed %d!\n", ret);
				return TS_FAILURE;
			}

			s_hdl_array[hdl_array_idx++] = ff_hdl;
			ff_hdl++;
		}
	}

//	ret = TS_MPI_RGN_BatchEnd(mpp_chn->s32ChnId);
//	if (ret != TS_SUCCESS) {
//		SAMPLE_PRT("TS_MPI_RGN_BatchEnd failed %d!\n", ret);
//		return TS_FAILURE;
//	}

	for (i = 0; i < OSD_THREAD_PARAM_NUM; ++i) {
		LOG_D("hdl_array:\n");
		LOG_D("%d \n", s_hdl_array[i]);
		LOG_D("\n");
	}

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_REGION_Stop_Rgn(MPP_CHN_S *mpp_chn)
{
	int ret;
	int i;
	RGN_HANDLE tmp_hdl;

	for (i = 0; i < OSD_THREAD_PARAM_NUM; ++i) {
		tmp_hdl = s_hdl_array[i];
		if (tmp_hdl == -1) {
			continue;
		}

		ret = TS_MPI_RGN_DetachFromChn(tmp_hdl, mpp_chn);
		if (TS_SUCCESS != ret) {
			SAMPLE_PRT("APP_REGION_DetachFromChn failed! Handle:%d\n", tmp_hdl);
			return TS_FAILURE;
		}
		ret = TS_MPI_RGN_Destroy(tmp_hdl);
		if (TS_SUCCESS != ret) {
			SAMPLE_PRT("TS_MPI_RGN_Destroy failed! Handle:%d\n", tmp_hdl);
			return TS_FAILURE;
		}

		s_hdl_array[i] = -1;
	}

//TODO
//	free(s_bitmap.pData);
//	s_bitmap.pData = NULL;

	return TS_SUCCESS;
}


