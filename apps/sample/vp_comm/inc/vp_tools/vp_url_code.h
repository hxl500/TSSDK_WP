//
// Created by dyb on 2024/7/17.
//

#ifndef VP_TOOLS_VP_URL_CODE_H
#define VP_TOOLS_VP_URL_CODE_H

#include <stdint.h>

/**
 * URL编码
 *
 * @param src       [in]        源字符串
 * @param src_len   [in]        源字符串长度
 * @param dest      [in]        编码字符串存储地址
 * @param dest_len  [in,out]    输入编码字符串可存储长度,输出编码字符串实际长度
 *
 * @return 错误码或实际编码字符串长度
 * @retval >= 0 编码成功, 返回实际编码字符串长度
 * @retval \< 0 编码失败
 */
int vp_url_encode(const char *src, uint32_t src_len, char *dest, uint32_t *dest_len);

/**
 * URL解码
 *
 * @param src       [in]        源字符串
 * @param src_len   [in]        源字符串长度
 * @param dest      [in]        解码字符串存储地址
 * @param dest_len  [in,out]    输入解码字符串可存储长度,输出解码字符串实际长度
 *
 * @return 错误码或实际解码字符串长度
 * @retval >= 0 解码成功, 返回实际解码字符串长度
 * @retval \< 0 解码失败
 */
int vp_url_decode(const char *src, uint32_t src_len, char *dest, uint32_t *dest_len);

#endif //VP_TOOLS_VP_URL_CODE_H
