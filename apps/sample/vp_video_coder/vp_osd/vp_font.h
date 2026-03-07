//
// Created by 杨柳 on 2022/11/28.
//

#ifndef VP_FRAMEWORK_VP_FONT_H
#define VP_FRAMEWORK_VP_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum {
    /* 大号字体 */
    VP_FONT_SIZE_LARGE,

    /* 中号字体 */
    VP_FONT_SIZE_MEDIUM,

    /* 小号字体 */
    VP_FONT_SIZE_SMALL,

    /* 超大号字体 */
    VP_FONT_SIZE_SUPER_LARGE,
} vp_font_size_t;


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
                           int16_t row_space, int16_t col_space);


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
                      uint8_t* osd_buff, uint32_t osd_x, uint32_t osd_y, uint32_t osd_width, uint32_t osd_height);

#ifdef __cplusplus
}
#endif

#endif //VP_FRAMEWORK_VP_FONT_H
