/* DO NOT EDIT */
#ifndef vp_printf_config_h
#define vp_printf_config_h

#define VP_PRINTF_VERSION_MAJOR (1)
#define VP_PRINTF_VERSION_MINOR (0)
#define VP_PRINTF_VERSION_PATCH (0)
#define VP_PRINTF_VERSION_TWEAK (4)
#define VP_PRINTF_VERSION "1.0.0.4"

#pragma region "日志输出等级选项"

// 无日志输出
#define VP_PRINTF_LEVEL_NONE 0x00

// 仅输出错误日志
#define VP_PRINTF_LEVEL_ERROR 0x01

// 输出异常日志
#define VP_PRINTF_LEVEL_WARN 0x02

// 输出消息日志
#define VP_PRINTF_LEVEL_INFO 0x03

// 输出调试日志
#define VP_PRINTF_LEVEL_DEBUG 0x04

// 输出调用栈日志
#define VP_PRINTF_LEVEL_STACK 0x05

// 全日志输出
#define VP_PRINTF_LEVEL_ALL 0xFF

#pragma endregion


/**
 * 日志启用标识
 */
#define VP_PRINTF_ENABLE 0x01

/**
 * 日志输出等级
 */
#define VP_PRINTF_LEVEL VP_PRINTF_LEVEL_ALL

#endif
