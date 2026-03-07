//
// Created by 杨柳 on 2022/11/28.
//

#include <string.h>
#include <vp_printf.h>
#include "lv_txt.h"
#include "lv_font.h"

#define NO_BREAK_FOUND UINT32_MAX

#define LV_TXT_BREAK_CHARS " ,.;:-_"

#define LV_IS_SIGNED(t) (((t)(-1)) < ((t)0))
#define LV_UMAX_OF(t) (((0x1ULL << ((sizeof(t) * 8ULL) - 1ULL)) - 1ULL) | (0xFULL << ((sizeof(t) * 8ULL) - 4ULL)))
#define LV_SMAX_OF(t) (((0x1ULL << ((sizeof(t) * 8ULL) - 1ULL)) - 1ULL) | (0x7ULL << ((sizeof(t) * 8ULL) - 4ULL)))
#define LV_MAX_OF(t) ((unsigned long)(LV_IS_SIGNED(t) ? LV_SMAX_OF(t) : LV_UMAX_OF(t)))
#define LV_MAX(a, b) ((a) > (b) ? (a) : (b))

/**
 * Decode two encoded character from a string.
 * @param txt pointer to '\0' terminated string
 * @param letter the first decoded Unicode character or 0 on invalid data code
 * @param letter_next the second decoded Unicode character or 0 on invalid data code
 * @param ofs start index in 'txt' where to start.
 *                After the call it will point to the next encoded char in 'txt'.
 *                NULL to use txt[0] as index
 */
void lv_txt_encoded_letter_next_2(const char *txt, uint32_t *letter, uint32_t *letter_next, uint32_t *ofs) {
    *letter = lv_txt_utf8_next(txt, ofs);
    *letter_next = *letter != '\0' ? lv_txt_utf8_next(&txt[*ofs], NULL) : 0;
}

/**
 * Test if char is break char or not (a text can broken here or not)
 * @param letter a letter
 * @return false: 'letter' is not break char
 */
static inline bool _lv_txt_is_break_char(uint32_t letter) {
    uint8_t i;
    bool ret = false;

    /* each chinese character can be break */
    if (letter >= 0x4E00 && letter <= 0x9FA5) {
        return true;
    }

    /*Compare the letter to TXT_BREAK_CHARS*/
    for (i = 0; LV_TXT_BREAK_CHARS[i] != '\0'; i++) {
        if (letter == (uint32_t) LV_TXT_BREAK_CHARS[i]) {
            ret = true; /*If match then it is break char*/
            break;
        }
    }

    return ret;
}

/**
 * Get the next word of text. A word is delimited by break characters.
 *
 * If the word cannot fit in the max_width space, obey LV_TXT_LINE_BREAK_LONG_* rules.
 *
 * If the next word cannot fit anything, return 0.
 *
 * If the first character is a break character, returns the next index.
 *
 * Example calls from lv_txt_get_next_line() assuming sufficient max_width and
 * txt = "Test text\n"
 *        0123456789
 *
 * Calls would be as follows:
 *     1. Return i=4, pointing at breakchar ' ', for the string "Test"
 *     2. Return i=5, since i=4 was a breakchar.
 *     3. Return i=9, pointing at breakchar '\n'
 *     4. Parenting lv_txt_get_next_line() would detect subsequent '\0'
 *
 * TODO: Returned word_w_ptr may overestimate the returned word's width when
 * max_width is reached. In current usage, this has no impact.
 *
 * @param txt a '\0' terminated string
 * @param font pointer to a font
 * @param letter_space letter space
 * @param max_width max with of the text (break the lines to fit this size) Set CORD_MAX to avoid line breaks
 * @param flags settings for the text from 'txt_flag_type' enum
 * @param[out] word_w_ptr width (in pixels) of the parsed word. May be NULL.
 * @param force Force return the fraction of the word that can fit in the provided space.
 * @return the index of the first char of the next word (in byte index not letter index. With UTF-8 they are different)
 */
static uint32_t lv_txt_get_next_word(const char *txt, const lv_font_t *font,
                                     uint32_t letter_space, uint32_t max_width, uint32_t *word_w_ptr,
                                     bool force) {
    if (txt == NULL || txt[0] == '\0') return 0;
    if (font == NULL) return 0;

    uint32_t i = 0, i_next = 0, i_next_next = 0;  /*Iterating index into txt*/
    uint32_t letter = 0;      /*Letter at i*/
    uint32_t letter_next = 0; /*Letter at i_next*/
    lv_coord_t letter_w;
    lv_coord_t cur_w = 0;  /*Pixel Width of transversed string*/
    uint32_t word_len = 0;   /*Number of characters in the transversed word*/
    uint32_t break_index = NO_BREAK_FOUND; /*only used for "long" words*/
    uint32_t break_letter_count = 0; /*Number of characters up to the long word break point*/

    letter = lv_txt_utf8_next(txt, &i_next);
    i_next_next = i_next;

    /*Obtain the full word, regardless if it fits or not in max_width*/
    while (txt[i] != '\0') {
        letter_next = lv_txt_utf8_next(txt, &i_next_next);
        word_len++;

        letter_w = lv_font_get_glyph_width(font, letter, letter_next);
        cur_w += letter_w;

        if (letter_w > 0) {
            cur_w += letter_space;
        }

        /*Test if this character fits within max_width*/
        if (break_index == NO_BREAK_FOUND && (cur_w - letter_space) > max_width) {
            break_index = i;
            break_letter_count = word_len - 1;
            /*break_index is now pointing at the character that doesn't fit*/
        }

        /*Check for new line chars and breakchars*/
        if (letter == '\n' || letter == '\r' || _lv_txt_is_break_char(letter)) {
            /*Update the output width on the first character if it fits.
             *Must do this here in case first letter is a break character.*/
            if (i == 0 && break_index == NO_BREAK_FOUND && word_w_ptr != NULL) *word_w_ptr = cur_w;
            word_len--;
            break;
        }

        /*Update the output width*/
        if (word_w_ptr != NULL && break_index == NO_BREAK_FOUND) *word_w_ptr = cur_w;

        i = i_next;
        i_next = i_next_next;
        letter = letter_next;
    }

    /*Entire Word fits in the provided space*/
    if (break_index == NO_BREAK_FOUND) {
        if (word_len == 0 || (letter == '\r' && letter_next == '\n')) i = i_next;
        return i;
    }

#if LV_TXT_LINE_BREAK_LONG_LEN > 0
    /*Word doesn't fit in provided space, but isn't "long"*/
    if(word_len < LV_TXT_LINE_BREAK_LONG_LEN) {
        if(force) return break_index;
        if(word_w_ptr != NULL) *word_w_ptr = 0; /*Return no word*/
        return 0;
    }

    /*Word is "long," but insufficient amounts can fit in provided space*/
    if(break_letter_count < LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN) {
        if(force) return break_index;
        if(word_w_ptr != NULL) *word_w_ptr = 0;
        return 0;
    }

    /*Word is a "long", but letters may need to be better distributed*/
    {
        i = break_index;
        int32_t n_move = LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN - (word_len - break_letter_count);
        /*Move pointer "i" backwards*/
        for(; n_move > 0; n_move--) {
            _lv_txt_encoded_prev(txt, &i);
            // TODO: it would be appropriate to update the returned word width here
            // However, in current usage, this doesn't impact anything.
        }
    }
    return i;
#else
    if (force) return break_index;
    if (word_w_ptr != NULL) *word_w_ptr = 0; /*Return no word*/
    (void) break_letter_count;
    return 0;
#endif
}

uint32_t lv_txt_get_next_line(const char *txt, const lv_font_t *font,
                              uint32_t letter_space, uint32_t max_width,
                              uint32_t *used_width) {
    if (used_width) *used_width = 0;

    if (txt == NULL) return 0;
    if (txt[0] == '\0') return 0;
    if (font == NULL) return 0;

    lv_coord_t line_w = 0;


    uint32_t i = 0;                                        /*Iterating index into txt*/

    while (txt[i] != '\0' && max_width > 0) {
        uint32_t word_w = 0;
        uint32_t advance = lv_txt_get_next_word(&txt[i], font, letter_space, max_width, &word_w, i == 0);
        max_width -= word_w;
        line_w += word_w;

        if (advance == 0) {
            break;
        }

        i += advance;

        if (txt[0] == '\n' || txt[0] == '\r') break;

        if (txt[i] == '\n' || txt[i] == '\r') {
            i++;  /*Include the following newline in the current line*/
            break;
        }

    }

    /*Always step at least one to avoid infinite loops*/
    if (i == 0) {
        uint32_t letter = lv_txt_utf8_next(txt, &i);
        if (used_width != NULL) {
            line_w = lv_font_get_glyph_width(font, letter, '\0');
        }
    }

    if (used_width != NULL) {
        *used_width = line_w;
    }

    return i;
}

/**
 * Get size of a text
 * @param size_res pointer to a 'point_t' variable to store the result
 * @param text pointer to a text
 * @param font pointer to font of the text
 * @param letter_space letter space of the text
 * @param line_space line space of the text
 * @param max_width max with of the text (break the lines to fit this size) Set CORD_MAX to avoid
 * line breaks
 */
void lv_txt_get_size(lv_point_t *size_res, const char *text, const lv_font_t *font, lv_coord_t letter_space,
                     lv_coord_t line_space, lv_coord_t max_width) {
    size_res->x = 0;
    size_res->y = 0;

    if (text == NULL || text[0] == '\0') return;
    if (font == NULL) return;
    uint32_t text_len = strlen(text);
    uint32_t len = 0;
    uint32_t off = 0;
    uint32_t width = 0, tmp_width = 0;
    uint16_t letter_height = lv_font_get_line_height(font);
    /*Calc. the height and longest line*/
    while (text[off] != '\0') {
        len = text_len - off;
        tmp_width = lv_txt_get_width(&text[off], &len, font, letter_space, max_width);
        off += len;
        if (tmp_width > width)
            width = tmp_width;
        size_res->y += letter_height + line_space;
    }
    size_res->x = width;
    if (size_res->y == 0)
        size_res->y = letter_height;
    else
        size_res->y -= line_space;
}

/**
 * Give the length of a text with a given font
 * @param txt a '\0' terminate string
 * @param length length of 'txt' in byte count and not characters (Á is 1 character but 2 bytes in
 * UTF-8)
 * @param font pointer to a font
 * @param letter_space letter space
 * @param flags settings for the text from 'txt_flag_t' enum
 * @return length of a char_num long text
 */
lv_coord_t lv_txt_get_width(const char *txt, uint32_t *length, const lv_font_t *font, lv_coord_t letter_space,
                            lv_coord_t max_width) {
    if (txt == NULL) return 0;
    if (font == NULL) return 0;
    if (txt[0] == '\0') return 0;

    uint32_t i = 0;
    lv_coord_t width = 0;
    uint8_t flag = 1;
    while (i < *length) {
        uint32_t letter;
        uint32_t letter_next;
        lv_txt_encoded_letter_next_2(txt, &letter, &letter_next, &i);

        lv_coord_t char_width = lv_font_get_glyph_width(font, letter, letter_next);
        if (char_width > 0) {
            if (width + char_width < max_width)
                width += char_width;
            else {
                i -= 1;
                flag = 1;
                break;
            }
            if (width + letter_space < max_width)
                width += letter_space;
            else {
                flag = 0;
                break;
            }
        }
    }

    if (width > 0 && flag) {
        width -= letter_space; /*Trim the last letter space. Important if the text is center aligned*/
    }
    *length = i;
    return width;
}

#define LV_IS_ASCII(value)              ((value & 0x80U) == 0x00U)
#define LV_IS_2BYTES_UTF8_CODE(value)   ((value & 0xE0U) == 0xC0U)
#define LV_IS_3BYTES_UTF8_CODE(value)   ((value & 0xF0U) == 0xE0U)
#define LV_IS_4BYTES_UTF8_CODE(value)   ((value & 0xF8U) == 0xF0U)
#define LV_IS_INVALID_UTF8_CODE(value)  ((value & 0xC0U) != 0x80U)

/**
 * Decode an UTF-8 character from a string.
 * @param txt pointer to '\0' terminated string
 * @param i start byte index in 'txt' where to start.
 *          After call it will point to the next UTF-8 char in 'txt'.
 *          NULL to use txt[0] as index
 * @return the decoded Unicode character or 0 on invalid UTF-8 code
 */
static uint32_t lv_txt_utf8_next(const char *txt, uint32_t *i) {
    /**
     * Unicode to UTF-8
     * 00000000 00000000 00000000 0xxxxxxx -> 0xxxxxxx
     * 00000000 00000000 00000yyy yyxxxxxx -> 110yyyyy 10xxxxxx
     * 00000000 00000000 zzzzyyyy yyxxxxxx -> 1110zzzz 10yyyyyy 10xxxxxx
     * 00000000 000wwwzz zzzzyyyy yyxxxxxx -> 11110www 10zzzzzz 10yyyyyy 10xxxxxx
     */

    uint32_t result = 0;

    /*Dummy 'i' pointer is required*/
    uint32_t i_tmp = 0;
    if (i == NULL) i = &i_tmp;

    /*Normal ASCII*/
    if (LV_IS_ASCII(txt[*i])) {
        result = txt[*i];
        (*i)++;
    }
        /*Real UTF-8 decode*/
    else {
        /*2 bytes UTF-8 code*/
        if (LV_IS_2BYTES_UTF8_CODE(txt[*i])) {
            result = (uint32_t) (txt[*i] & 0x1F) << 6;
            (*i)++;
            if (LV_IS_INVALID_UTF8_CODE(txt[*i])) return 0;
            result += (txt[*i] & 0x3F);
            (*i)++;
        }
            /*3 bytes UTF-8 code*/
        else if (LV_IS_3BYTES_UTF8_CODE(txt[*i])) {
            result = (uint32_t) (txt[*i] & 0x0F) << 12;
            (*i)++;

            if (LV_IS_INVALID_UTF8_CODE(txt[*i])) return 0;
            result += (uint32_t) (txt[*i] & 0x3F) << 6;
            (*i)++;

            if (LV_IS_INVALID_UTF8_CODE(txt[*i])) return 0;
            result += (txt[*i] & 0x3F);
            (*i)++;
        }
            /*4 bytes UTF-8 code*/
        else if (LV_IS_4BYTES_UTF8_CODE(txt[*i])) {
            result = (uint32_t) (txt[*i] & 0x07) << 18;
            (*i)++;

            if (LV_IS_INVALID_UTF8_CODE(txt[*i])) return 0;
            result += (uint32_t) (txt[*i] & 0x3F) << 12;
            (*i)++;

            if (LV_IS_INVALID_UTF8_CODE(txt[*i])) return 0;
            result += (uint32_t) (txt[*i] & 0x3F) << 6;
            (*i)++;

            if (LV_IS_INVALID_UTF8_CODE(txt[*i])) return 0;
            result += txt[*i] & 0x3F;
            (*i)++;
        } else {
            (*i)++; /*Not UTF-8 char. Go the next.*/
        }
    }
    return result;
}