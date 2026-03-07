#ifndef __OSD_TRUETYPE_H__
#define __OSD_TRUETYPE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#include <stdbool.h>
unsigned char * getArgbDataAfterHandleStr(unsigned char *text,int *width,int *height, float pixels ,int bitmap_h,unsigned char* inputBitmap,bool b_change);
unsigned char * getArgbDataAfterHandleStr_2bit(unsigned char *text,int *width,int *height, float pixels ,int bitmap_h,  unsigned char* inputBitmap);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
