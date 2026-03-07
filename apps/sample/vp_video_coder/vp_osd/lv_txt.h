//
// Created by 杨柳 on 2022/11/28.
//

#ifndef VP_FRAMEWORK_LV_TXT_H
#define VP_FRAMEWORK_LV_TXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lv_font.h"

/**
 * Decode an UTF-8 character from a string.
 * @param txt pointer to '\0' terminated string
 * @param i start byte index in 'txt' where to start.
 *          After call it will point to the next UTF-8 char in 'txt'.
 *          NULL to use txt[0] as index
 * @return the decoded Unicode character or 0 on invalid UTF-8 code
 */
static uint32_t lv_txt_utf8_next(const char *txt, uint32_t *i);

/**
 * Give the length of a text with a given font
 * @param txt a '\0' terminate string
 * @param length length of 'txt' in byte count and not characters (Á is 1 character but 2 bytes in
 * UTF-8)
 * @param font pointer to a font
 * @param letter_space letter space
 * @param max_width line max width
 * @return length of a char_num long text
 */
lv_coord_t lv_txt_get_width(const char *txt, uint32_t *length, const lv_font_t *font, lv_coord_t letter_space,lv_coord_t max_width);

/**
 * Get size of a text
 * @param size_res pointer to a 'point_t' variable to store the result
 * @param text pointer to a text
 * @param font pointer to font of the text
 * @param letter_space letter space of the text
 * @param line_space line space of the text
 * @param flags settings for the text from ::lv_text_flag_t
 * @param max_width max with of the text (break the lines to fit this size) Set CORD_MAX to avoid
 * line breaks
 */
void lv_txt_get_size(lv_point_t *size_res, const char *text, const lv_font_t *font, lv_coord_t letter_space,
                     lv_coord_t line_space, lv_coord_t max_width);

/**
 * Decode two encoded character from a string.
 * @param txt pointer to '\0' terminated string
 * @param letter the first decoded Unicode character or 0 on invalid data code
 * @param letter_next the second decoded Unicode character or 0 on invalid data code
 * @param ofs start index in 'txt' where to start.
 *                After the call it will point to the next encoded char in 'txt'.
 *                NULL to use txt[0] as index
 */
void lv_txt_encoded_letter_next_2(const char *txt, uint32_t *letter, uint32_t *letter_next, uint32_t *ofs);

#ifdef __cplusplus
}
#endif

#endif //VP_FRAMEWORK_LV_TXT_H
