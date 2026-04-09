/**
 * @file lvgl_demo.h
 * Include all LVGL related headers
 */

#ifndef LVGL_DEMO_H
#define LVGL_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************
 * CURRENT VERSION OF LVGL
 ***************************/

 int lvgl_start_demo_slideshow(unsigned int fps);

 int lvgl_start_demos(char * info[], int size, unsigned int fps);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_H*/
