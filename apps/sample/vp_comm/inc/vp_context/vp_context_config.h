/* DO NOT EDIT */
#ifndef vp_context_config_h
#define vp_context_config_h

#define VP_CONTEXT_VERSION_MAJOR (1)
#define VP_CONTEXT_VERSION_MINOR (0)
#define VP_CONTEXT_VERSION_PATCH (0)
#define VP_CONTEXT_VERSION_TWEAK (11)
#define VP_CONTEXT_VERSION "1.0.0.11"

#pragma region "编译平台选项"

// 君正T31
#define VP_SDK_PLATFORM_T31 0x01

// 君正T40
#define VP_SDK_PLATFORM_T40 0x02

// 君正T41
#define VP_SDK_PLATFORM_T41 0x03

// 君正T23
#define VP_SDK_PLATFORM_T23 0x04

// 君正T32
#define VP_SDK_PLATFORM_T32 0x05

// 海思3516
#define VP_SDK_PLATFORM_HI3516 0x11

// MacOs
#define VP_SDK_PLATFORM_MAC 0xF1

// Linux
#define VP_SDK_PLATFORM_LINUX 0xF2

// Windows
#define VP_SDK_PLATFORM_WINDOWS 0xF3

#pragma endregion

#pragma region "电源工作模式选项"

// 长供电模式
#define VP_POWER_TYPE_LONG 0x00

// 低功耗模式
#define VP_POWER_TYPE_LOW 0x01

#pragma endregion

#pragma region "网络模式"

// 无网络模式
#define VP_NETWORK_MODE_NONE 0x00

// 有线网络
#define VP_NETWORK_MODE_WIRE 0x01

// WIFI网络
#define VP_NETWORK_MODE_WIFI 0x02

// 移动网络
#define VP_NETWORK_MODE_MOBILE 0x03

// WIFI+移动网络
#define VP_NETWORK_MODE_WIFI_MOBILE 0x10

// 有线+WIFI网络
#define VP_NETWORK_MODE_WIRE_WIFI 0x11

// 有线+移动网络
#define VP_NETWORK_MODE_WIRE_MOBILE 0x12

// 有线+WIFI+移动网络
#define VP_NETWORK_MODE_WIRE_WIFI_MOBILE 0x13

#pragma endregion

#pragma region "SOC品牌选项"

// 君正SOC
#define VP_SOC_BRAND_JZ 0x01

// 海思SOC
#define VP_SOC_BRAND_HI 0x00

#pragma endregion

#pragma region "SOC型号选项"

// 君正T31
#define VP_SOC_MODEL_JZ_T31 0x01

// 君正T31ZL
#define VP_SOC_MODEL_JZ_T31ZL 0x02

// 君正T31ZC
#define VP_SOC_MODEL_JZ_T31ZC 0x03

// 君正T40
#define VP_SOC_MODEL_JZ_T40 0x11

// 君正T41
#define VP_SOC_MODEL_JZ_T41 0x21

// 君正T41L
#define VP_SOC_MODEL_JZ_T41L 0x22

// 君正T41N
#define VP_SOC_MODEL_JZ_T41N 0x23

// 君正T41ZX
#define VP_SOC_MODEL_JZ_T41ZX 0x24

// 君正T41ZL
#define VP_SOC_MODEL_JZ_T41ZL 0x25

// 君正T41ZN
#define VP_SOC_MODEL_JZ_T41ZN 0x26

// 君正T41ZM
#define VP_SOC_MODEL_JZ_T41ZM 0x27

// 君正T41ZG
#define VP_SOC_MODEL_JZ_T41ZG 0x28

// 君正T41A
#define VP_SOC_MODEL_JZ_T41A 0x29

// 君正T23
#define VP_SOC_MODEL_JZ_T23 0x31

// 君正T23N
#define VP_SOC_MODEL_JZ_T23N 0x32

// 君正T23ZN
#define VP_SOC_MODEL_JZ_T23ZN 0x33

// 君正T23DL
#define VP_SOC_MODEL_JZ_T23DL 0x34

// 君正T23DL 2目
#define VP_SOC_MODEL_JZ_T23DL_2 0x35

// 君正T23ZN V9
#define VP_SOC_MODEL_JZ_T23ZN_V9 0x36

// 君正T23ZN 2目
#define VP_SOC_MODEL_JZ_T23ZN_2 0x37

// 君正T23ZN 3目
#define VP_SOC_MODEL_JZ_T23ZN_3 0x38

// 君正T32
#define VP_SOC_MODEL_JZ_T32 0x41

// 君正T32LQ
#define VP_SOC_MODEL_JZ_T32LQ 0x42

// 君正T32NQ
#define VP_SOC_MODEL_JZ_T32NQ 0x43

// 海思3516CV610
#define VP_SOC_MODEL_HI_3516CV610 0x51

// 海思3516CV608
#define VP_SOC_MODEL_HI_3516CV608 0x52

#pragma endregion

#pragma region "网卡品牌选项"

// 无网卡品牌
#define VP_NIC_BRAND_NONE 0x00

// 海思
#define VP_NIC_BRAND_HI 0x01

// 爱科微
#define VP_NIC_BRAND_AIC 0x05

// 合宙
#define VP_NIC_BRAND_HZ 0x02

// 移远
#define VP_NIC_BRAND_QC 0x03

// 天喻
#define VP_NIC_BRAND_TY 0x04

#pragma endregion

#pragma region "网卡型号选项"

// 无网卡型号
#define VP_NIC_MODEL_NONE 0x00

// 海思3861
#define VP_NIC_MODEL_HI_3861L 0x01

// 爱科微8800
#define VP_NIC_MODEL_AIC_8800 0x02

// 合宙720UG
#define VP_NIC_MODEL_HZ_720UG 0x11

// 合宙722UG
#define VP_NIC_MODEL_HZ_722UG 0x12

// 合宙724UG
#define VP_NIC_MODEL_HZ_724UG 0x13

// 合宙720SL
#define VP_NIC_MODEL_HZ_720SL 0x14

// 合宙727SE
#define VP_NIC_MODEL_HZ_727SE 0x15

// 合宙720C
#define VP_NIC_MODEL_HZ_720C 0x16

// 合宙780E
#define VP_NIC_MODEL_HZ_780E 0x17

// 合宙780EU
#define VP_NIC_MODEL_HZ_780EU 0x18

// 合宙780ET
#define VP_NIC_MODEL_HZ_780ET 0x19

// 合宙600E
#define VP_NIC_MODEL_HZ_600E 0x1A

// 合宙660EA
#define VP_NIC_MODEL_HZ_660EA 0x1A

// 移远EC25
#define VP_NIC_MODEL_QC_EC25 0x1A

// 移远EG915
#define VP_NIC_MODEL_QC_EG915 0x21

// 天喻SS401
#define VP_NIC_MODEL_TY_SS401 0x31

#pragma endregion

#pragma region "系统服务平台选项"

// 维拍(Veepai)
#define VP_SDK_OS_VEEPAI 0x01

// 涂鸦
#define VP_SDK_OS_TUYA 0x02

// 自定义
#define VP_SDK_OS_CUSTOM 0xFF

#pragma endregion


/**
 * 系统服务平台
 */
#define VP_SDK_OS_TYPE VP_SDK_OS_VEEPAI

/**
 * 编译平台
 */
#define VP_SDK_PLATFORM VP_SDK_PLATFORM_LINUX

/**
 * 功耗模式
 */
#define VP_POWER_TYPE VP_POWER_TYPE_LOW

/**
 * 网络模式
 */
#define VP_NETWORK_MODE VP_NETWORK_MODE_MOBILE

/**
 * SOC品牌
 */
#define VP_SOC_BRAND VP_SOC_BRAND_JZ

/**
 * SOC型号
 */
#define VP_SOC_MODEL VP_SOC_MODEL_JZ_T41ZM

/**
 * 网卡品牌
 */
#define VP_NIC_BRAND VP_NIC_BRAND_HZ

/**
 * 网卡型号
 */
#define VP_NIC_MODEL VP_NIC_MODEL_HZ_780E

#endif
