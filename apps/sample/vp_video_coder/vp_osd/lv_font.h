/**
 * @file lv_font.h
 *
 */

#ifndef LV_FONT_H
#define LV_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/
typedef int32_t lv_coord_t;
typedef uintptr_t lv_uintptr_t;
typedef union {
    struct {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t alpha;
    } ch;
    uint32_t full;
} lv_color32_t;
typedef lv_color32_t lv_color_t;
typedef uint8_t lv_opa_t;

typedef struct {
    lv_coord_t x;
    lv_coord_t y;
} lv_point_t;
/**********************
 *      TYPEDEFS
 **********************/

/*------------------
 * General types
 *-----------------*/

struct _lv_font_t;
/** Describes the properties of a glyph.*/
typedef struct {
    const struct _lv_font_t *
            resolved_font; /**< Pointer to a font where the gylph was actually found after handling fallbacks*/
    uint16_t adv_w; /**< The glyph needs this space. Draw the next glyph after this width.*/
    uint16_t box_w; /**< Width of the glyph's bounding box*/
    uint16_t box_h; /**< Height of the glyph's bounding box*/
    int16_t ofs_x;  /**< x offset of the bounding box*/
    int16_t ofs_y;  /**< y offset of the bounding box*/
    uint8_t bpp: 4;  /**< Bit-per-pixel: 1, 2, 4, 8*/
    uint8_t is_placeholder: 1; /** Glyph is missing. But placeholder will still be displayed */
} lv_font_glyph_dsc_t;

/** The bitmaps might be upscaled by 3 to achieve subpixel rendering.*/
enum {
    LV_FONT_SUBPX_NONE,
    LV_FONT_SUBPX_HOR,
    LV_FONT_SUBPX_VER,
    LV_FONT_SUBPX_BOTH,
};

typedef uint8_t lv_font_subpx_t;

/** Describe the properties of a font*/
typedef struct _lv_font_t {
    /** Get a glyph's descriptor from a font*/
    bool (*get_glyph_dsc)(const struct _lv_font_t *, lv_font_glyph_dsc_t *, uint32_t letter, uint32_t letter_next);

    /** Get a glyph's bitmap from a font*/
    const uint8_t *(*get_glyph_bitmap)(const struct _lv_font_t *, uint32_t);

    /*Pointer to the font in a font pack (must have the same line height)*/
    lv_coord_t line_height;         /**< The real line height where any text fits*/
    lv_coord_t base_line;           /**< Base line measured from the top of the line_height*/
    uint8_t subpx: 2;             /**< An element of `lv_font_subpx_t`*/

    int8_t underline_position;      /**< Distance between the top of the underline and base line (< 0 means below the base line)*/
    int8_t underline_thickness;     /**< Thickness of the underline*/

    const void *dsc;               /**< Store implementation specific or run_time data or caching here*/
    const struct _lv_font_t *fallback;   /**< Fallback font for missing glyph. Resolved recursively */
#if LV_USE_USER_DATA
    void * user_data;               /**< Custom user data for font.*/
#endif
} lv_font_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Return with the bitmap of a font.
 * @param font_p pointer to a font
 * @param letter an UNICODE character code
 * @return pointer to the bitmap of the letter
 */
const uint8_t *lv_font_get_glyph_bitmap(const lv_font_t *font_p, uint32_t letter);

/**
 * Get the descriptor of a glyph
 * @param font_p pointer to font
 * @param dsc_out store the result descriptor here
 * @param letter an UNICODE letter code
 * @param letter_next the next letter after `letter`. Used for kerning
 * @return true: descriptor is successfully loaded into `dsc_out`.
 *         false: the letter was not found, no data is loaded to `dsc_out`
 */
bool lv_font_get_glyph_dsc(const lv_font_t *font_p, lv_font_glyph_dsc_t *dsc_out, uint32_t letter,
                           uint32_t letter_next);

/**
 * Get the width of a glyph with kerning
 * @param font pointer to a font
 * @param letter an UNICODE letter
 * @param letter_next the next letter after `letter`. Used for kerning
 * @return the width of the glyph
 */
lv_coord_t lv_font_get_glyph_width(const lv_font_t *font, uint32_t letter, uint32_t letter_next);

/**
 * Get the line height of a font. All characters fit into this height
 * @param font_p pointer to a font
 * @return the height of a font
 */
static inline lv_coord_t lv_font_get_line_height(const lv_font_t *font_p) {
    return font_p->line_height;
}

/**********************
 *      MACROS
 **********************/

#define EXTERN_FONT(x) extern const lv_font_t vp_font_##x
#define LV_FONT_EXTERN(size) EXTERN_FONT(size);

#define CONST_FONT(x) &vp_font_##x
#define LV_FONT_CONST(size) CONST_FONT(size)

#ifdef VP_FONT_SUPER_LARGE
LV_FONT_EXTERN(VP_FONT_SUPER_LARGE)
#endif

#ifdef VP_FONT_LARGE
LV_FONT_EXTERN(VP_FONT_LARGE)
#endif

#ifdef VP_FONT_MEDIUM
LV_FONT_EXTERN(VP_FONT_MEDIUM)
#endif

#ifdef VP_FONT_SMALL
LV_FONT_EXTERN(VP_FONT_SMALL)
#endif

extern const lv_font_t vp_font_20;
extern const lv_font_t vp_font_32;
extern const lv_font_t vp_font_64;
extern const lv_font_t vp_font_80;

/**
 * 获取超大号字体
 * @return
 */
static inline const lv_font_t *lv_font_super_large(void) {
    return LV_FONT_CONST(80);
}

/**
 * 获取大号字体
 * @return
 */
static inline const lv_font_t *lv_font_large(void) {
    return LV_FONT_CONST(64);
}

/**
 * 获取中号字体
 * @return
 */
static inline const lv_font_t *lv_font_medium(void) {
    return LV_FONT_CONST(32);
}

/**
 * 获取小号号字体
 * @return
 */
static inline const lv_font_t *lv_font_small(void) {
    return LV_FONT_CONST(20);
}

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*USE_FONT*/
