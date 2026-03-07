#ifndef VP_TOOLS_VP_UTF8_H_
#define VP_TOOLS_VP_UTF8_H_

#include <stddef.h>
#include <stdlib.h>

#include <stdint.h>

typedef int32_t vp_urf8_char;

/**
 * 比较utf8字符串
 *
 * @note 不区分大小写
 *
 * @param src1  [in]    utf8字符串1
 * @param src2  [in]    utf8字符串2
 *
 * @return 比较值
 * @retval -1   src1 < src2
 * @retval  0   src1 == src2
 * @retval  1   src1 > src2
 */
int vp_utf8_case_cmp(const char *src1, const char *src2);

/**
 * 追加utf8字符串
 * @param dst   [in]    目标utf8字符串
 * @param src   [in]    原始utf8字符串
 * @return 返回目标utf8字符串
 */
char *vp_utf8_cat(char *dst, const char *src);

/**
 * utf8字符串查找
 * @param src   [in]    utf8字符串
 * @param chr   [in]    utf8字符
 * @return utf8字符串
 */
char *vp_utf8_chr(const char *src, vp_urf8_char chr);

/**
 * 比较utf8字符串
 *
 * @note 区分大小写
 *
 * @param src1  [in]    utf8字符串1
 * @param src2  [in]    utf8字符串2
 *
 * @return 比较值
 * @retval -1   src1 < src2
 * @retval  0   src1 == src2
 * @retval  1   src1 > src2
 */
int vp_utf8_cmp(const char *src1, const char *src2);

/**
 * utf8字符串复制
 *
 * @param dst   [in]    目标utf8字符串
 * @param src   [in]    原始utf8字符串
 *
 * @return 目标utf8字符串
 */
char *vp_utf8_cpy(char *dst, const char *src);

/**
 * 查找uft8字符串中不包含reject的字符的数量
 *
 * @param src        [in]    utf8字符串
 * @param reject     [out]   不包含的utf8字符串
 *
 * @return utf8字符数量
 */
size_t vp_utf8_cspn(const char *src, const char *reject);

/**
 * utf8字符串复制
 *
 * @param src [in]    utf8字符串
 *
 * @return utf8字符串,内部申请内存
 */
char *vp_utf8_dup(const char *src);

/**
 * utf8字符串长度
 *
 * @param str [in]    utf8字符串
 *
 * @return utf8字符串长度
 */
size_t vp_utf8_len(const char *str);

/**
 * utf8字符串长度
 *
 * @param str   [in]    utf8字符串
 * @param n     [in]    utf8字符串长度
 *
 * @return utf8字符串长度
 */
size_t vp_utf8_num_len(const char *str, size_t n);

/**
 * 比较utf8字符串
 *
 * @note 不区分大小写
 *
 * @param src1  [in]    utf8字符串1
 * @param src2  [in]    utf8字符串2
 * @param n     [in]    比较字符数量
 *
 * @return 比较值
 * @retval -1   src1 < src2
 * @retval  0   src1 == src2
 * @retval  1   src1 > src2
 */
int vp_utf8_num_case_cmp(const char *src1, const char *src2, size_t n);

/**
 * 追加utf8字符串
 *
 * @param dst   [in]    目标utf8字符串
 * @param src   [in]    原始utf8字符串
 * @param n     [in]    追加字符数量
 *
 * @return 返回目标utf8字符串
 */
char *vp_utf8_num_cat(char *dst, const char *src, size_t n);

/**
 * 比较utf8字符串
 *
 * @note 区分大小写
 *
 * @param src1  [in]    utf8字符串1
 * @param src2  [in]    utf8字符串2
 * @param n     [in]    比较字符数量
 *
 * @return 比较值
 * @retval -1   src1 < src2
 * @retval  0   src1 == src2
 * @retval  1   src1 > src2
 */
int vp_utf8_num_cmp(const char *src1, const char *src2, size_t n);

/**
 * utf8字符串拷贝
 *
 * @param dst   [in]    目标utf8字符串
 * @param src   [in]    原始utf8字符串
 * @param n     [in]    拷贝字符数量
 *
 * @return 目标utf8字符串
 */
char *vp_utf8_num_cpy(char *dst, const char *src, size_t n);

/**
 * utf8字符串复制
 *
 * @param src [in]    utf8字符串
 * @param n   [in]    复制字符数量
 *
 * @return utf8字符串,内部申请内存
 */
char *vp_utf8_num_dup(const char *src, size_t n);

/**
 * utf8字符串查找
 *
 * @note 在 utf8 字符串 str 中定位 utf8 字符串 accept 中任意字节的第一次出现位置，如果未找到匹配项，则定位 0。
 *
 * @param str        [in]    utf8字符串
 * @param accept     [in]    utf8字符串
 *
 * @return utf8字符串
 */
char *vp_utf8_pbrk(const char *str, const char *accept);

/**
 * utf8字符串查找
 *
 * @note 在UTF-8编码的字符串src中查找UTF-8编码的字符chr最后一次出现的位置
 *
 * @param src   [in]    utf8字符串
 * @param chr   [in]    utf8字符
 * @return
 */
char *vp_utf8_rchr(const char *src, int chr);

/**
 * utf8字符串长度
 *
 * @param str [in]    utf8字符串
 *
 * @return utf8字符串长度
 */
size_t vp_utf8_size(const char *str);

/**
 * utf8字符串长度
 *
 * @note 排除字符串末尾的空终止符
 *
 * @param str [in]    utf8字符串
 *
 * @return utf8字符串长度
 */
size_t vp_utf8_size_lazy(const char *str);

/**
 * utf8字符串长度
 *
 * @note 排除字符串末尾的空终止符
 *
 * @param str [in]    utf8字符串
 * @param n   [in]    字符串长度
 *
 * @return utf8字符串长度
 */
size_t vp_utf8_num_size_lazy(const char *str, size_t n);

/**
 * 统计src字符串中所有字符，这些字符必须全部出现在accept字符串中
 *
 * @param src       [in]    utf8字符串
 * @param accept    [in]    utf8字符串
 *
 * @return 统计数量
 */
size_t vp_utf8_spn(const char *src, const char *accept);

/**
 * 主字符串中查找子字符串首次出现的起始索引
 *
 * @param haystack  [in]    主utf8字符串
 * @param needle    [in]    子utf8字符串
 *
 * @return 子字符串起始字指针
 */
char *vp_utf8_str(const char *haystack, const char *needle);

/**
 * 主字符串中查找子字符串首次出现的起始索引
 *
 * @note 不区分大小写
 *
 * @param haystack  [in]    主utf8字符串
 * @param needle    [in]    子utf8字符串
 *
 * @return 子字符串起始字指针
 */
char *vp_utf8_case_str(const char *haystack, const char *needle);

/**
 * 校验是否为utf8字符串
 *
 * @param str   [in]    校验字符串
 *
 * @return 无法校验的字符串或状态码
 * @retval NULL    校验成功
 * @retval 非NULL  校验失败,返回非utf8字符串指针
 */
char *vp_utf8_valid(const char *str);

/**
 * 校验是否为utf8字符串
 *
 * @param str   [in]    校验字符串
 * @param n     [in]    校验字符串长度
 *
 * @return 无法校验的字符串或状态码
 * @retval NULL    校验成功
 * @retval 非NULL  校验失败,返回非utf8字符串指针
 */
char *vp_utf8_num_valid(const char *str, size_t n);

/* Given a null-terminated string, makes the string valid by replacing invalid
 * codepoints with a 1-byte replacement. Returns 0 on success. */
/**
 * 矫正utf8字符串
 *
 * @note 给定一个以空字符结尾的字符串，通过用 1 字节替换无效代码点使字符串有效。成功时返回 0。
 *
 * @param str           [in]    utf8字符串
 * @param replacement   [in]    utf8替换字符
 *
 * @return 错误码
 * @retval  0 成功
 * @retval !0 失败
 */
int vp_utf8_make_valid(char *str, char replacement);

/**
 * 将 out_codepoint 设置为 str 中的当前 utf8 代码点
 * 并返回 str 中当前代码点之后的下一个 utf8 代码点的地址。
 *
 * @param str               [in]    utf8字符串
 * @param out_codepoint     [in]    utf8字符串
 *
 * @return utf8字符串
 */
char *vp_utf8_codepoint(const char *str, vp_urf8_char *out_codepoint);

/**
 * 计算下一个utf8字符大小
 *
 * @param str [in]    utf8字符串
 *
 * @return uft8字符大小
 */
size_t vp_utf8_codepoint_next_size(const char *str);

/**
 * 计算utf8字符大小
 *
 * @param chr [in]     utf8字符
 *
 * @return utf8字符大小
 */
size_t vp_utf8_codepoint_size(vp_urf8_char chr);

/**
 * 将utf8字符写入给定的字符串，并返回写入的utf8字符后的下一个位置的地址。
 * 将缓冲区中剩余的字节数传递给 n。如果没有足够的空间容纳代码点，
 * 此函数将返回 null。
 *
 * @param str   [in]    utf8字符串
 * @param chr   [in]    utf8字符
 * @param n     [in]    剩余字符长度
 *
 * @return utf8字符串
 * @retval NULL    无空间
 * @retval 非NULL  uft8字符串
 */
char *vp_utf8_cat_codepoint(char *str, vp_urf8_char chr, size_t n);

/**
 * 判断是否为小写
 *
 * @param chr [in]     utf8字符
 *
 * @return 是否为小写
 * @retval 1 是小写
 * @retval 0 不是小写
 */
int vp_utf8_lower(vp_urf8_char chr);

/**
 * 判断是否为大写
 *
 * @param chr [in]     utf8字符
 *
 * @return 是否为大写
 * @retval 1 是大写
 * @retval 0 不是大写
 */
int vp_utf8_upper(vp_urf8_char chr);

/**
 * 将utf8字符串转换为小写
 *
 * @param str [in]     utf8字符串
 */
void vp_utf8_lwr(char *str);

/**
 * 将utf8字符串转换为大写
 *
 * @param str [in]     utf8字符串
 */
void vp_utf8_upr(char *str);

/**
 * 如果可能的话，将utf8字符变为小写。
 *
 * @param cp    [in]    utf8字符
 *
 * @return 转换后的utf8字符
 */
vp_urf8_char vp_utf8_lwr_codepoint(vp_urf8_char cp);

/**
 * 如果可能的话，将utf8字符变为大写。
 *
 * @param cp    [in]    utf8字符
 *
 * @return 转换后的utf8字符
 */
vp_urf8_char vp_utf8_upr_codepoint(vp_urf8_char cp);

/**
 * utf8字符串复制
 *
 * @note 将out_codepoint设置为str中的当前utf8字符，并返回 str 中当前 utf8字符之前的 utf8字符的地址。
 *
 * @param str
 * @param out_codepoint
 * @return
 */
char *vp_utf8_rcodepoint(const char *str, vp_urf8_char *out_codepoint);

/**
 * utf8字符串复制
 *
 * @param src               [in]    utf8字符串
 * @param alloc_func_ptr    [in]    内存申请函数
 * @param user_data         [in]    用户数据
 *
 * @return utf8字符串,内部申请内存
 */
char *vp_utf8_dup_ex(const char *src,
                     char *(*alloc_func_ptr)(char *, size_t),
                     char *user_data);

/**
 * utf8字符串复制
 *
 * @param src            [in]    utf8字符串
 * @param n              [in]    复制字符数量
 * @param alloc_func_ptr [in]    内存申请函数
 * @param user_data      [in]    用户数据
 *
 * @return utf8字符串,内部申请内存
 */
char *vp_utf8_num_dup_ex(const char *src, size_t n,
                         char *(*alloc_func_ptr)(char *, size_t),
                         char *user_data);

#endif /* VP_TOOLS_VP_UTF8_H_ */