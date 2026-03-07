#ifndef _SOFT_ALPHABET_OVERLAY_H__
#define _SOFT_ALPHABET_OVERLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

int overlay_id(int id, unsigned char *yuvOut, int yuvW, int yuvH, int x, int y);
int overlay_letter(char *name, unsigned char *yuvOut, int yuvW, int yuvH, int x, int y);
int overlay_letter_confidence(char *name, float confidence, unsigned char *yuvOut, int yuvW, int yuvH, int x, int y);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif
