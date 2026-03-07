//
// Created by 杨柳 on 2022/2/22.
//

#ifndef VP_CAMERA_VP_CRC32_H
#define VP_CAMERA_VP_CRC32_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
* 计算数据的CRC32值
* @param crc 初始CRC值
* @param data 需要计算的数据内容
* @param size 数据长度
* @return crc32校验值
*/
uint32_t vp_crc32_value(uint32_t crc, uint8_t *data, uint32_t size);

/**
 * @brief 计算数据CRC32值(用于升级包计算)
 * 
 * @param crc 初始CRC值
 * @param p   需要计算的数据内容 
 * @param len 数据长度
 * @return uint32_t  CRC32检验值
 */
uint32_t vp_crc32_value_v2(uint32_t crc, const uint8_t *p, uint32_t len);

#ifdef __cplusplus
}
#endif
#endif //VP_CAMERA_VP_CRC32_H
