//
// Created by 杨柳 on 2022/11/28.
//

#include <vp_printf.h>
#include <stdio.h>
#include <stdlib.h>
#include "vp_font.h"
#include "lv_font.h"
#include "lv_font_fmt_txt.h"
#include "lv_txt.h"

const uint8_t lv_bpp1_opa_table[2] = {0, 255};          /*Opacity mapping with bpp = 1 (Just for compatibility)*/
const uint8_t lv_bpp2_opa_table[4] = {0, 85, 170, 255}; /*Opacity mapping with bpp = 2*/

const uint8_t lv_bpp3_opa_table[8] = {0, 36, 73, 109,   /*Opacity mapping with bpp = 3*/
                                      146, 182, 219, 255
};

const uint8_t lv_bpp4_opa_table[16] = {0, 17, 34, 51,  /*Opacity mapping with bpp = 4*/
                                       68, 85, 102, 119,
                                       136, 153, 170, 187,
                                       204, 221, 238, 255
};

const uint8_t lv_bpp8_opa_table[256] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                                        16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
                                        32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
                                        48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
                                        64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
                                        80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
                                        96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
                                        112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
                                        128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
                                        144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
                                        160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
                                        176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
                                        192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
                                        208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
                                        224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
                                        240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
};
enum {
    LV_OPA_TRANSP = 0,
    LV_OPA_0 = 0,
    LV_OPA_10 = 25,
    LV_OPA_20 = 51,
    LV_OPA_30 = 76,
    LV_OPA_40 = 102,
    LV_OPA_50 = 127,
    LV_OPA_60 = 153,
    LV_OPA_70 = 178,
    LV_OPA_80 = 204,
    LV_OPA_90 = 229,
    LV_OPA_100 = 255,
    LV_OPA_COVER = 255,
};

#define LV_OPA_MIN 2    /*Opacities below this will be transparent*/
#define LV_OPA_MAX 253  /*Opacities above this will fully cover*/
#define LV_COLOR_DEPTH 32
#define LV_COLOR_MIX_ROUND_OFS (LV_COLOR_DEPTH == 32 ? 0: 128)

# define LV_COLOR_SET_R32(c, v) (c).ch.red = (uint8_t)((v) & 0xFF)
# define LV_COLOR_SET_G32(c, v) (c).ch.green = (uint8_t)((v) & 0xFF)
# define LV_COLOR_SET_B32(c, v) (c).ch.blue = (uint8_t)((v) & 0xFF)
# define LV_COLOR_SET_A32(c, v) (c).ch.alpha = (uint8_t)((v) & 0xFF)

# define LV_COLOR_GET_R32(c) (c).ch.red
# define LV_COLOR_GET_G32(c) (c).ch.green
# define LV_COLOR_GET_B32(c) (c).ch.blue
# define LV_COLOR_GET_A32(c) (c).ch.alpha

#define _LV_CONCAT(x, y) x ## y
#define LV_CONCAT(x, y) _LV_CONCAT(x, y)
#define LV_COLOR_SET_R(c, v) LV_CONCAT(LV_COLOR_SET_R, LV_COLOR_DEPTH)(c, v)
#define LV_COLOR_SET_G(c, v) LV_CONCAT(LV_COLOR_SET_G, LV_COLOR_DEPTH)(c, v)
#define LV_COLOR_SET_B(c, v) LV_CONCAT(LV_COLOR_SET_B, LV_COLOR_DEPTH)(c, v)
#define LV_COLOR_SET_A(c, v) LV_CONCAT(LV_COLOR_SET_A, LV_COLOR_DEPTH)(c, v)

#define LV_COLOR_GET_R(c) LV_CONCAT(LV_COLOR_GET_R, LV_COLOR_DEPTH)(c)
#define LV_COLOR_GET_G(c) LV_CONCAT(LV_COLOR_GET_G, LV_COLOR_DEPTH)(c)
#define LV_COLOR_GET_B(c) LV_CONCAT(LV_COLOR_GET_B, LV_COLOR_DEPTH)(c)
#define LV_COLOR_GET_A(c) LV_CONCAT(LV_COLOR_GET_A, LV_COLOR_DEPTH)(c)

#define LV_UDIV255(x) (((x) * 0x8081U) >> 0x17)

static lv_color_t lv_color_mix(lv_color_t c1, lv_color_t c2, uint8_t mix) {
    lv_color_t ret = {.full = 0};

    LV_COLOR_SET_R(ret, LV_UDIV255((uint16_t) LV_COLOR_GET_R(c1) * mix + LV_COLOR_GET_R(c2) *
                                                                         (255 - mix) + LV_COLOR_MIX_ROUND_OFS));
    LV_COLOR_SET_G(ret, LV_UDIV255((uint16_t) LV_COLOR_GET_G(c1) * mix + LV_COLOR_GET_G(c2) *
                                                                         (255 - mix) + LV_COLOR_MIX_ROUND_OFS));
    LV_COLOR_SET_B(ret, LV_UDIV255((uint16_t) LV_COLOR_GET_B(c1) * mix + LV_COLOR_GET_B(c2) *
                                                                         (255 - mix) + LV_COLOR_MIX_ROUND_OFS));
    if (ret.full != 0 || c2.full != 0)
        LV_COLOR_SET_A(ret, 0xFF);
    return ret;
}


#define FILL_NORMAL_MASK_PX(color)                          \
    if(*mask == LV_OPA_COVER) *dest_buf = color;            \
    else *dest_buf = lv_color_mix(color, *dest_buf, *mask); \
    mask++;                                                 \
    dest_buf++;

static void fill_normal(lv_color_t *dest_buf, uint32_t dest_stride, lv_color_t color,
                        const lv_opa_t *mask, uint32_t mask_stride, int32_t width, int32_t height) {
    int32_t w = width;
    int32_t h = height;

    int32_t x;
    int32_t y;

    int32_t x_end4 = w - 4;
    for (y = 0; y < h; y++) {
        for (x = 0; x < w && ((lv_uintptr_t) (mask) & 0x3); x++) {
            FILL_NORMAL_MASK_PX(color)
        }

        for (; x <= x_end4; x += 4) {
            uint32_t mask32 = *((uint32_t *) mask);
            if (mask32 == 0xFFFFFFFF) {
                dest_buf[0] = color;
                dest_buf[1] = color;
                dest_buf[2] = color;
                dest_buf[3] = color;
                dest_buf += 4;
                mask += 4;
            } else if (mask32) {
                FILL_NORMAL_MASK_PX(color)
                FILL_NORMAL_MASK_PX(color)
                FILL_NORMAL_MASK_PX(color)
                FILL_NORMAL_MASK_PX(color)
            } else {
                mask += 4;
                dest_buf += 4;
            }
        }

        for (; x < w; x++) {
            FILL_NORMAL_MASK_PX(color)
        }
        dest_buf += (dest_stride - w);
        mask += (mask_stride - w);
    }
}


uint32_t draw_letter_normal(lv_opa_t *mask_buf, lv_font_glyph_dsc_t *g, const uint8_t *map_p, lv_coord_t *width,
                            lv_coord_t *height) {

    const uint8_t *bpp_opa_table_p;
    uint32_t bitmask_init;
    uint32_t bitmask;
    uint32_t bpp = g->bpp;
    if (bpp == 3) bpp = 4;

    switch (bpp) {
        case 1:
            bpp_opa_table_p = lv_bpp1_opa_table;
            bitmask_init = 0x80;
            break;
        case 2:
            bpp_opa_table_p = lv_bpp2_opa_table;
            bitmask_init = 0xC0;
            break;
        case 4:
            bpp_opa_table_p = lv_bpp4_opa_table;
            bitmask_init = 0xF0;
            break;
        case 8:
            bpp_opa_table_p = lv_bpp8_opa_table;
            bitmask_init = 0xFF;
            break;       /*No opa table, pixel value will be used directly*/
        default:
            vp_warn("lv_draw_letter: invalid bpp");
            return 0; /*Invalid bpp. Can't render the letter*/
    }

    int32_t col, row;
    int32_t box_w = g->box_w;
    int32_t box_h = g->box_h;
    int32_t width_bit = (int32_t) (box_w * bpp); /*Letter width in bits*/

    /*Calculate the col/row start/end on the map*/
    int32_t col_start = 0;
    int32_t col_end = box_w;
    int32_t row_start = 0;
    int32_t row_end = box_h;

    /*Move on the map too*/
    uint32_t bit_ofs = (row_start * width_bit) + (col_start * bpp);
    map_p += bit_ofs >> 3;

    uint8_t letter_px;
    uint32_t col_bit;
    col_bit = bit_ofs & 0x7; /*"& 0x7" equals to "% 8" just faster*/

    uint32_t mask_buf_size = box_w * box_h;
    int32_t mask_p = 0;

    uint32_t col_bit_max = 8 - bpp;
    uint32_t col_bit_row_ofs = (box_w + col_start - col_end) * bpp;

    for (row = row_start; row < row_end; row++) {
        bitmask = bitmask_init >> col_bit;
        for (col = col_start; col < col_end; col++) {
            /*Load the pixel's opacity into the mask*/
            letter_px = (*map_p & bitmask) >> (col_bit_max - col_bit);
            if (letter_px) {
                mask_buf[mask_p] = bpp_opa_table_p[letter_px];
            } else {
                mask_buf[mask_p] = 0;
            }

            /*Go to the next column*/
            if (col_bit < col_bit_max) {
                col_bit += bpp;
                bitmask = bitmask >> bpp;
            } else {
                col_bit = 0;
                bitmask = bitmask_init;
                map_p++;
            }

            /*Next mask byte*/
            mask_p++;
        }

        col_bit += col_bit_row_ofs;
        map_p += (col_bit >> 3);
        col_bit = col_bit & 0x7;
    }
    *width = box_w;
    *height = box_h;
    return mask_buf_size;
}

uint32_t vp_font_draw_letter(const lv_font_t *font, uint8_t *mask, uint32_t letter,
                             lv_coord_t *width, lv_coord_t *height) {
    lv_font_glyph_dsc_t g;
    bool g_ret = lv_font_get_glyph_dsc(font, &g, letter, '\0');
    if (g_ret == false) {
        if (letter >= 0x20 &&
            letter != 0xf8ff && /*LV_SYMBOL_DUMMY*/
            letter != 0x200c) { /*ZERO WIDTH NON-JOINER*/
            vp_warn("lv_draw_letter: glyph dsc. not found for U+%X", (unsigned int) letter);
        }
        return 0;
    }
    const uint8_t *map_p = lv_font_get_glyph_bitmap(g.resolved_font, letter);
    if (map_p == NULL) {
        vp_warn("lv_draw_letter: character's bitmap not found");
        return 0;
    }
    uint32_t size = draw_letter_normal(mask, &g, map_p, width, height);
    return size;
}


const lv_font_t *vp_font_get(vp_font_size_t size) {
    switch (size) {
        case VP_FONT_SIZE_SUPER_LARGE:
            return lv_font_super_large();
        case VP_FONT_SIZE_LARGE:
            return lv_font_large();
        case VP_FONT_SIZE_MEDIUM:
            return lv_font_medium();
        case VP_FONT_SIZE_SMALL:
            return lv_font_small();
        default:
            return lv_font_medium();
    }
}

/**
 * 获取文本绘制buffer大小,并输出宽高
 *
 * @note 该方法不进行内存分配,仅计算出当前输入的txt所需要绘制的像素大小
 * 像素格式为bgra占4字节,size = width * height * 4
 * 当前方法不进行特殊字符串进行特殊计算如`换行符`,`制表符`等.
 * 所以不要在文本中插入换行符来进行排版.用户应多次计算多行文本来进行图像拼接
 *
 * @param font      [in]        需要绘制字体的大小规格
 * @param txt       [in]        需要计算的文本
 * @param width     [in,out]    输入和输出宽度,输入最大宽度来限制文本的宽,输出实际用到的宽度.
 *                              输出值可能会小于输入值,但不会大于输入值
 * @param height    [out]       通过给定的宽输出绘制的高度,
 * @param row_space [in]        每行之间的间隔
 * @param col_space [in]        每列之间的间隔
 *
 * @return 文本绘制需要的buffer长度
 */
uint32_t vp_font_draw_size(vp_font_size_t size, const char *txt, uint32_t *width, uint32_t *height,
                           int16_t row_space, int16_t col_space) {
    if (txt == NULL || txt[0] == '\0') {
        *width = 0;
        *height = 0;
        return 0;
    }
    lv_point_t p;
    const lv_font_t *font = vp_font_get(size);
    lv_txt_get_size(&p, txt, font, col_space, row_space, (lv_coord_t) *width);
    if (p.x % 2) p.x += 1;
    if (p.y % 2) p.y += 1;
    *width = p.x;
    *height = p.y;
    return p.x * p.y * sizeof(lv_color_t);
}

static inline bool vp_font_check_color(const lv_color32_t *pixel) {
    if (pixel->ch.alpha == 0) return false;
    return pixel->ch.red != 0 || pixel->ch.green != 0 || pixel->ch.blue != 0;
}

// 检查像素是否在图像范围内
static inline bool vp_font_check_bounds(int x, int y, uint32_t width, uint32_t height) {
    return x >= 0 && x < width && y >= 0 && y < height;
}

#if 0
/**
 * 将文本内容绘制成BGRA图像数据
 *
 * @note 该函数将输入的文本绘制成指定颜色的图像数据,可以给`draw_buff`给定初始值为图像添加底色.
 * `draw_buff`的大小和图像宽高请使用函数`vp_font_draw_size`计算得到,需要注意的是`row_space`,`col_space`,`x`,`y`应保持一致
 *
 * @param font      [in]        需要绘制字体的大小规格
 * @param txt       [in]        需要绘制的文本内容
 * @param draw_buff [in,out]    输入图像的背景图像数据,像素格式为BGRA,例:0xFFFF0000 为红色
 * @param color     [in]        绘制文本的颜色,像素格式为BGRA
 * @param width     [in]        图像宽
 * @param height    [in]        图像高
 * @param row_space [in]        行与行之间的间隔
 * @param col_space [in]        列与列之间的间隔
 *
 * @return 状态码
 * @retval =0 成功
 * @retval -1 失败
 */
int vp_font_draw_text(vp_font_size_t size, const char *txt, uint8_t *draw_buff, uint32_t color,
                      uint32_t width, uint32_t height,
                      int16_t row_space, int16_t col_space) {
    if (txt == NULL || txt[0] == '\0') return -1;
    const lv_font_t *font = vp_font_get(size);
    lv_color_t lv_color = {.full = color};
    uint32_t off = 0, off_line, off_x = 0, off_y = 0;
    uint32_t letter;
    uint32_t letter_next;
    uint8_t mask[8192];
    lv_coord_t char_width, diff_width, diff_height;
    uint8_t *draw_start = draw_buff;
    lv_coord_t mask_width, mask_height;
    uint16_t letter_height = lv_font_get_line_height(font);
    while (txt[off] != '\0') {
        lv_txt_encoded_letter_next_2(txt, &letter, &letter_next, &off);
        if (letter == 0) continue;
        vp_font_draw_letter(font, mask, letter, &mask_width, &mask_height);

        char_width = lv_font_get_glyph_width(font, letter, letter_next);
        if (char_width > mask_width)
            diff_width = (char_width - mask_width) / 2;
        else
            diff_width = (mask_width - char_width) / 2;
        if (mask_height < letter_height)
            diff_height = (letter_height - mask_height) / 2;
        else
            diff_height = 0;

        off_line = 0;
        while (off_line < mask_height) {
            fill_normal((lv_color_t *) (draw_start + (width * (off_line + diff_height) + diff_width) * 4), char_width,
                        lv_color,
                        mask + mask_width * off_line,
                        mask_width, (int32_t) mask_width, 1);
            off_line += 1;
        }
        off_x += char_width + col_space;
        if (off_x > width) {
            off_y += letter_height + row_space;
            if (off_y > height) break;
            draw_start = draw_buff + (off_y * width) * sizeof(lv_color_t);
        } else {
            draw_start += (char_width + col_space) * sizeof(lv_color_t);
        }
    }

    lv_color32_t *pixels = (lv_color32_t *) draw_buff;
    int min = -2, max = 2;
    if (size == VP_FONT_SIZE_SMALL) {
        min = -1;
        max = 1;
    } else if (size == VP_FONT_SIZE_LARGE) {
        min = -3;
        max = 3;
    } else if (size == VP_FONT_SIZE_SUPER_LARGE) {
        min = -4;
        max = 4;
    }
    lv_color32_t *row = pixels;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            lv_color32_t *pixel = &row[x];
            if (vp_font_check_color(pixel)) {
                for (int dy = min; dy <= max; ++dy) {
                    for (int dx = min; dx <= max; ++dx) {
                        if (dx == 0 && dy == 0) continue; // 跳过当前像素
                        int nx = x + dx;
                        int ny = y + dy;
                        if (vp_font_check_bounds(nx, ny, width, height)) {
                            pixel = &pixels[ny * width + nx];
                            if (!vp_font_check_color(pixel)) {
                                pixel->full = 0xFF000000;
                            } else {
                                pixel->full = color;
                            }
                        }
                    }
                }
            }
        }
        row += width;
    }

    return 0;
}
#else
/**
 * 将文本内容绘制成BGRA图像数据
 *
 * @note 该函数将输入的文本绘制成指定颜色的图像数据,可以给`draw_buff`给定初始值为图像添加底色.
 * `draw_buff`的大小和图像宽高请使用函数`vp_font_draw_size`计算得到,需要注意的是`row_space`,`col_space`,`x`,`y`应保持一致
 *
 * @param font      [in]        需要绘制字体的大小规格
 * @param txt       [in]        需要绘制的文本内容
 * @param draw_buff [in,out]    输入图像的背景图像数据,像素格式为BGRA,例:0xFFFF0000 为红色
 * @param color     [in]        绘制文本的颜色,像素格式为BGRA
 * @param width     [in]        图像宽
 * @param height    [in]        图像高
 * @param row_space [in]        行与行之间的间隔
 * @param col_space [in]        列与列之间的间隔
 *
 * @return 状态码
 * @retval =0 成功
 * @retval -1 失败
 */
int vp_font_draw_text(vp_font_size_t size, const char *txt, uint8_t *draw_buff, uint32_t color,
                      uint32_t width, uint32_t height, int16_t row_space, int16_t col_space, 
                      uint8_t* osd_buff, uint32_t osd_x, uint32_t osd_y, uint32_t osd_width, uint32_t osd_height) {
    if (txt == NULL || txt[0] == '\0') return -1;
    const lv_font_t *font = vp_font_get(size);
    lv_color_t lv_color = {.full = 0xFFFF00FF};
    uint32_t off = 0, off_line, off_x = 0, off_y = 0;
    uint32_t letter;
    uint32_t letter_next;
    uint8_t mask[8192];
    lv_coord_t char_width, diff_width, diff_height;
    uint8_t *draw_start = draw_buff;
    lv_coord_t mask_width, mask_height;
    uint16_t letter_height = lv_font_get_line_height(font);
    while (txt[off] != '\0') {
        lv_txt_encoded_letter_next_2(txt, &letter, &letter_next, &off);
        if (letter == 0) continue;
        vp_font_draw_letter(font, mask, letter, &mask_width, &mask_height);

        char_width = lv_font_get_glyph_width(font, letter, letter_next);
        if (char_width > mask_width)
            diff_width = (char_width - mask_width) / 2;
        else
            diff_width = (mask_width - char_width) / 2;
        if (mask_height < letter_height)
            diff_height = (letter_height - mask_height) / 2;
        else
            diff_height = 0;

        off_line = 0;
        while (off_line < mask_height) {
            fill_normal((lv_color_t *) (draw_start + (width * (off_line + diff_height) + diff_width) * 4), char_width,
                        lv_color,
                        mask + mask_width * off_line,
                        mask_width, (int32_t) mask_width, 1);
            off_line += 1;
        }
        off_x += char_width + col_space;
        if (off_x > width) {
            off_y += letter_height + row_space;
            if (off_y > height) break;
            draw_start = draw_buff + (off_y * width) * sizeof(lv_color_t);
        } else {
            draw_start += (char_width + col_space) * sizeof(lv_color_t);
        }
    }
    
    lv_color32_t *row = (lv_color32_t *) draw_buff;
    int dst_pos = 0;
    int row_pos = 0;
    osd_buff += ((osd_y * osd_width + osd_x) >> 2);
    for (int y = 0; y < height; ++y) {
        row_pos = y * osd_width;
        for (int x = 0; x < width; ++x) {
            dst_pos = row_pos + x;
            if (vp_font_check_color(&row[x])) {
                if ((x == 0 || (x + 1) == width || y == 0 || (y + 1) == height)) {
                    osd_buff[dst_pos >> 2] |= (0x1 << ((dst_pos & 3) << 1));
                } else {
                    osd_buff[dst_pos >> 2] |= (color << ((dst_pos & 3) << 1));
                }
            } else {
                if ((x > 0 && vp_font_check_color(row + x - 1)) || 
                    (x + 1 < width && vp_font_check_color(row + x + 1)) || 
                    (y > 0 && vp_font_check_color(row + x - width)) ||
                    (y + 1 < height && vp_font_check_color(row + x + width))) {
                        osd_buff[dst_pos >> 2] |= (0x1 << ((dst_pos & 3) << 1));
                    } else {
                        osd_buff[dst_pos >> 2] &= ~(3 << ((dst_pos & 3) << 1));
                    }
            }
        }
        row += width;
    }

    return 0;
}
#endif