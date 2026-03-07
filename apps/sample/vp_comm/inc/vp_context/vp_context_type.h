//
// Created by 杨柳 on 2023/8/3.
//

#ifndef VP_CONTEXT_VP_CONTEXT_TYPE_H
#define VP_CONTEXT_VP_CONTEXT_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "vp_context_config.h"

typedef struct vp_context *vp_context_p;

typedef struct {
    /**
     * 品牌
     * VP_SOC_BRAND_*
     */
    uint16_t brand;
    /**
     * 型号
     * VP_SOC_MODEL_*
     */
    uint16_t model;
} vp_soc_info_t;/* SOC 信息 */

typedef struct {
    /**
     * 品牌
     * VP_NIC_BRAND_*
     */
    uint16_t brand;
    /**
     * 型号
     * VP_NIC_MODEL_*
     */
    uint16_t model;
} vp_nic_info_t /* 网卡信息 */;

typedef enum {
    /* 未定义 */
    VP_BUILD_PLATFORM_UNKNOW = 0x00,
    /* 君正T31 */
    VP_BUILD_PLATFORM_JZ_T31 = 0x01,
    /* 君正T40 */
    VP_BUILD_PLATFORM_JZ_T40 = 0x02,
    /* 君正T41 */
    VP_BUILD_PLATFORM_JZ_T41 = 0x03,
    /* 君正T23 */
    VP_BUILD_PLATFORM_JZ_T23 = 0x04,
    /* 君正T32 */
    VP_BUILD_PLATFORM_JZ_T32 = 0x05,
    /* 海思3516 */
    VP_BUILD_PLATFORM_HI_3516 = 0x11,
    /* Apple Mac System */
    VP_BUILD_PLATFORM_MAC_OS = 0xF1,
    /* Linux System */
    VP_BUILD_PLATFORM_LINUX = 0xF2,
    /* Windows System */
    VP_BUILD_PLATFORM_WIN = 0xF3,
    /* Other System */
    VP_BUILD_PLATFORM_OTHER = 0xFF,
    VP_BUILD_PLATFORM_MAX,
} vp_platform_t;

/**
 * 判断是否为君正平台
 */
#define VP_PLATFORM_IS_JZ(x) ((x) > 0x00 && (x) < 0x10)
/**
 * 判断是否为海思平台
 */
#define VP_PLATFORM_IS_HI(x) ((x) > 0x10 && (x) < 0x20)
/**
 * 判断是否为主机平台
 */
#define VP_PLATFORM_IS_HOST(x) ((x) > 0xF0 && (x) <=0xFF)

/**
 * 判断是否为设备平台
 */
#define VP_PLATFORM_IS_DEVICE(x) ((x) < 0xF0)

/**
 * 判断是否为WIFI网络设备
 */
#define VP_NIC_IS_WIFI (VP_NETWORK_MODE == VP_NETWORK_MODE_WIFI || \
                        VP_NETWORK_MODE == VP_NETWORK_MODE_WIFI_MOBILE || \
                        VP_NETWORK_MODE == VP_NETWORK_MODE_WIRE_WIFI || \
                        VP_NETWORK_MODE == VP_NETWORK_MODE_WIRE_WIFI_MOBILE)

/**
 * 判断是否为移动网络设备
 */
#define VP_NIC_IS_MOBILE (VP_NETWORK_MODE == VP_NETWORK_MODE_MOBILE || \
                          VP_NETWORK_MODE == VP_NETWORK_MODE_WIFI_MOBILE || \
                          VP_NETWORK_MODE == VP_NETWORK_MODE_WIRE_MOBILE || \
                          VP_NETWORK_MODE == VP_NETWORK_MODE_WIRE_WIFI_MOBILE)
/**
 * 判断是否为有线网络设备
 */
#define VP_NIC_IS_WIRE (VP_NETWORK_MODE == VP_NETWORK_MODE_WIRE || \
                        VP_NETWORK_MODE == VP_NETWORK_MODE_WIRE_MOBILE || \
                        VP_NETWORK_MODE == VP_NETWORK_MODE_WIRE_WIFI || \
                        VP_NETWORK_MODE == VP_NETWORK_MODE_WIRE_WIFI_MOBILE)
#ifdef __cplusplus
}
#endif

#endif //VP_CONTEXT_VP_CONTEXT_TYPE_H
