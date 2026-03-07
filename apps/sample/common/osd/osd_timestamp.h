#ifndef __OSD_TIMESTAMP_H__
#define __OSD_TIMESTAMP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

int osdGetSuitableFontSize(int videoW, int videoH, float *fontW, int *fontH);
int osdGetTimeStringBitmapSize_1555(float pixels, int bitmap_h, int *strW, int *strH);
int osdGetTimeStringBitmapSize_2bit(float pixels, int bitmap_h, int *strW, int *strH);
int osdGetCustomStringBitmapSize_1555(float pixels, unsigned char *str, int bitmap_h, int *strW, int *strH);
int osdGetCustomStringBitmapSize_2bit(float pixels, unsigned char *str, int bitmap_h, int *strW, int *strH);
int osdGetCurrentTime(unsigned char *pSzGetTimeBuff, const unsigned char inLen, unsigned char *pOutLen);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
