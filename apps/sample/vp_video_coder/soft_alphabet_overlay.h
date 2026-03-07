#ifndef _SOFT_ALPHABET_OVERLAY_H__
#define _SOFT_ALPHABET_OVERLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

int overlay_letter(char *name, unsigned char *yuvOut, int yuvW, int yuvH, int x, int y, int color, int font);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif
