#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "osd_truetype.h"
#include "osd_timestamp.h"

#ifndef TS_SUCCESS
#define TS_SUCCESS 0
#endif

#ifndef TS_FAILURE
#define TS_FAILURE (-1)
#endif

int osdGetSuitableFontSize(int videoW, int videoH, float *fontW, int *fontH)
{
	int neededYHeight = 64;

    if(3840 == videoW ) {
        *fontW = 140.0;
        *fontH = 140;
		neededYHeight = 64;
    } else if(2880 == videoW ) {
        *fontW = 120.0;
        *fontH = 120;
		neededYHeight = 64;
    } else if(2592 == videoW ) {
        *fontW = 100.0;
        *fontH = 100;
		neededYHeight = 64;
    } else if(2560 == videoW ) {
        *fontW = 30.0;
        *fontH = 30;
		neededYHeight = 24;
    } else if(2304 == videoW ) {
        *fontW = 76.0;
        *fontH = 76;
		neededYHeight = 60;
    } else if(1920 == videoW ) {
        *fontW = 40.0;
        *fontH = 40;
		neededYHeight = 32;
    }else if(1280 == videoW ) {
        *fontW = 40.0;
        *fontH = 40;
		neededYHeight = 32;
    } else if(960 == videoW ) {
        *fontW = 30.0;
        *fontH = 30;
		neededYHeight = 24;
    } else if(640 == videoW ) {
        *fontW = 16.0;
        *fontH = 20;
		neededYHeight = 24;
    }else if(704 == videoW ) {
        *fontW = 16.0;
        *fontH = 18;
		neededYHeight = 24;
    } else {
        *fontW = 30.0;
        *fontH = 30;
		neededYHeight = 24;
	}
    return neededYHeight;
}

int osdGetTimeStringBitmapSize_2bit(float pixels, int bitmap_h, int *strW, int *strH)
{
    int width, height;
    unsigned char *pStream = NULL;

    pStream = getArgbDataAfterHandleStr_2bit((unsigned char *)"2024-03-04 15:16:30", &width, &height, pixels, bitmap_h, NULL);
    if(NULL == pStream) {
        printf(" pBitMap null");
        return TS_FAILURE;
    }
    *strW = width;
    *strH = height;
    free(pStream);
    pStream = NULL;
    return TS_SUCCESS;
}

int osdGetTimeStringBitmapSize_1555(float pixels, int bitmap_h, int *strW, int *strH)
{
    int width, height;
    unsigned char *pStream = NULL;

    pStream = getArgbDataAfterHandleStr((unsigned char *)"2024-03-04 15:16:30", &width, &height, pixels, bitmap_h, NULL, 1);
    if(NULL == pStream) {
        printf(" pBitMap null");
        return TS_FAILURE;
    }
    *strW = width;
    *strH = height;
    free(pStream);
    pStream = NULL;
    return TS_SUCCESS;
}


int osdGetCustomStringBitmapSize_2bit(float pixels, unsigned char *str, int bitmap_h, int *strW, int *strH)
{
    int width, height;
    unsigned char *pStream = NULL;

    pStream = getArgbDataAfterHandleStr_2bit((unsigned char *)str, &width, &height, pixels, bitmap_h, NULL);
    if(NULL == pStream) {
        printf(" pBitMap null");
        return TS_FAILURE;
    }
    *strW = width;
    *strH = height;
    free(pStream);
    pStream = NULL;
    return TS_SUCCESS;
}

int osdGetCustomStringBitmapSize_1555(float pixels, unsigned char *str, int bitmap_h, int *strW, int *strH)
{
    int width, height;
    unsigned char *pStream = NULL;

    pStream = getArgbDataAfterHandleStr((unsigned char *)str, &width, &height, pixels, bitmap_h, NULL, 1);
    if(NULL == pStream) {
        printf(" pBitMap null");
        return TS_FAILURE;
    }
    *strW = width;
    *strH = height;
    free(pStream);
    pStream = NULL;
    return TS_SUCCESS;
}

int osdGetCurrentTime(unsigned char *pSzGetTimeBuff, const unsigned char inLen, unsigned char *pOutLen)
{
    if (NULL == pSzGetTimeBuff) {
        printf(" Param Is Null !.");
        return -1;
    }
    time_t timep;
    struct tm *tm_now = NULL;
    time (&timep);
    tm_now = localtime(&timep); /** local time */
    snprintf((char *)pSzGetTimeBuff, inLen, "%04d-%02d-%02d %02d:%02d:%02d",
        tm_now->tm_year + 1900,
        tm_now->tm_mon + 1,
        tm_now->tm_mday,
        tm_now->tm_hour,
        tm_now->tm_min,
        tm_now->tm_sec);
    *pOutLen = strlen((char *)pSzGetTimeBuff);
    return 0;
}

