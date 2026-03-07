/**
 * Created by 杨柳 on 2023/2/16.
 *
 * 观察者模块,用于管理SDK模块间事件分发.
 *
 * 通过使用`vp_observers_listen`对事件进行监听.
 * 其他模块通过`vp_observers_post`推送事件
 *
 */
#ifndef VP_OBSERVERS_H
#define VP_OBSERVERS_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct vp_obs_item *vp_obs_item_p;

typedef void (*vp_obs_callback_f)(uint32_t type, uint32_t event, void *args, void *user_info);

/**
 * 会话事件类型
 */
#define VP_OBS_TYPE_SESSION         0x00000100
/**
 * 新会话接入事件
 * 事件参数:int 会话ID
 */
#define VP_OBS_EVENT_SESSION_NEW    (1 << 0)
/**
 * 会话结束事件
 * 事件参数:int 会话ID
 */
#define VP_OBS_EVENT_SESSION_DEL    (1 << 1)

/**
 * WIFI事件类型
 */
#define VP_OBS_TYPE_WIFI            0x00001000
/**
 * WIFI AP 事件
 * 事件参数:@[vp_wifi_ap_event_info_t *]
 */
#define VP_OBS_EVENT_WIFI_AP        (1 << 0)
/**
 * WIFI STA事件
 * 事件参数:@[vp_wifi_sta_event_info_t *]
 */
#define VP_OBS_EVENT_WIFI_STA       (1 << 1)

/**
 * PIR事件类型
 */
#define VP_OBS_TYPE_PIR             0x00001001
/**
 * PIR状态更改事件
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 已触发
 */
#define VP_OBS_EVENT_PIR_STATE      (1 << 0)

/**
 * 电池事件类型
 */
#define VP_OBS_TYPE_BATTERY         0x00001002
/**
 * 电池充电状态更改事件
 * 事件参数:uint8_t 充电状态
 * @var 0 未充电
 * @var 1 正在充电
 */
#define VP_OBS_EVENT_BATTERY_CHARGE (1 << 1)

/**
 * 电池低电事件
 * 事件参数:uint8_t 电量百分比值
 * @var 0 未触发
 * @var 1 已触发
 */
#define VP_OBS_EVENT_BATTERY_LOW_VOL (1 << 2)

/**
 * 按键推送事件类型
 */
#define VP_OBS_TYPE_KEY             0x00001003
/**
 * 按键状态事件
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 已触发
 */
#define VP_OBS_EVENT_KEY_STATE      (1 << 2)
/**
 * 按键短按事件
 * 事件参数:uint8_t 触发状态(始终为1)
 * @var 1 已触发
 */
#define VP_OBS_EVENT_KEY_SHORT      (1 << 10)
/**
 * 按键长按事件
 * 事件参数:uint8_t 触发状态(始终为1)
 * @var 1 已触发
 */
#define VP_OBS_EVENT_KEY_LONE       (1 << 11)

/**
 * 电源按键长按事件
 * 事件参数:uint8_t 触发状态(始终为1)
 * @var 1 已触发
 */
#define VP_OBS_EVENT_POWER_KEY_LONE       (1 << 12)

/**
 * 3861事件类型
 */
#define VP_OBS_TYPE_3861            0x00001004
/**
 * 3861状态事件
 * 事件参数:vp_hi3861_state_t 状态枚举
 */
#define VP_OBS_EVENT_3861_STATE     (1 << 0)
/**
 * 4G模组事件类型
 */
#define VP_OBS_TYPE_MOBILE          0x00001005
/**
 * 4G模组状态事件
 * 事件参数:vp_mobile_state_t 状态枚举
 */
#define VP_OBS_EVENT_MOBILE_STATE   (1 << 0)

/**
 * 4G模组网络状态相关信息
 * 事件参数:vp_network_info_t 状态枚举
 */
#define VP_OBS_EVENT_MOBILE_NETWORK_INFO   (1 << 1)

/**
 * MCU事件类型
 */
#define VP_OBS_TYPE_MCU            0x00001006
/**
 * MCU状态事件
 * 事件参数:vp_mcu_state_t 状态枚举
 */
#define VP_OBS_EVENT_MCU_STATE     (1 << 0)
/**
 * MCU事件触发
 * 事件参数:vp_mcu_state_t 状态枚举
 */
#define VP_OBS_EVENT_MCU_TRIGGER     (1 << 1)

/**
 * 电源管理模块
 */
#define VP_OBS_TYPE_LPC             0x00001007
/**
 * 电源管理模块准备就绪
 */
#define VP_OBS_EVENT_LPC_READY      (1 << 0)

/**
 * 电源管理模块准备休眠
 */
#define VP_OBS_EVENT_LPC_SLEEP      (1 << 1)

/**
 * 电源管理模块准备进入AOV休眠
 */
#define VP_OBS_EVENT_LPC_AOV_SLEEP   (1 << 2)

/**
 * 电源管理模块准备进入AOV休眠前控制白光灯
 */
#define VP_OBS_EVENT_LPC_AOV_SLEEP_SET_WLED   (1 << 3)

/**
 * 复位闪灯
 */
#define VP_OBS_EVENT_LPC_RESET_LED   (1 << 4)

/**
 * 8800事件类型
 */
#define VP_OBS_TYPE_8800            0x00001008
/**
 * 8800状态事件
 * 事件参数:vp_aic8800_state_t 状态枚举
 */
#define VP_OBS_EVENT_8800_STATE     (1 << 0)

/**
 * SMART LINK事件类型
 */
#define VP_OBS_TYPE_SMART_LINK          0x00001009

/**
 * 配网信息事件
 * 事件参数: vp_smart_config_t 配网信息结构体
 */
#define VP_OBS_EVENT_SMART_LINK_INFO     (1 << 0)

/**
 * 配网信息事件
 * 事件参数: vp_smart_config_4g_t 配网信息结构体
 */
#define VP_OBS_EVENT_SMART_LINK_4G_INFO   (1 << 1)

/**
 * 报警事件类型
 */
#define VP_OBS_TYPE_ALARM           0x000010FF
/**
 * 防拆报警事件
 * 事件参数:uint8_t 触发状态
 * @var  触发状态和对应sensor
 */
#define VP_OBS_EVENT_ALARM_TAMPER   (1 << 0)
/**
 * 移动侦测报警事件
 * 事件参数:uint8_t 触发状态
 * @var  触发状态和对应sensor
 */
#define VP_OBS_EVENT_ALARM_MOTION   (1 << 1)
/**
 * 人形侦测报警事件
 * 事件参数:uint8_t 触发状态
  * @var  触发状态和对应sensor
 */
#define VP_OBS_EVENT_ALARM_PERSON   (1 << 2)
/**
 * 烟雾报警事件
 * 事件参数:uint8_t 触发状态
 * @var  触发状态和对应sensor
 */
#define VP_OBS_EVENT_ALARM_SMOKE     (1 << 3)
/**
 * 火焰报警事件
 * 事件参数:uint8_t 触发状态
 * @var  触发状态和对应sensor
 */
#define VP_OBS_EVENT_ALARM_FIRE     (1 << 4)
/**
 * 包裹报警事件
 * 事件参数:uint8_t 触发状态
 * @var  触发状态和对应sensor
 */
#define VP_OBS_EVENT_ALARM_PACKAGE     (1 << 5)
/**
 * 区域入侵报警事件
 * 事件参数:uint8_t 触发状态
 * @var  触发状态和对应sensor
 */
#define VP_OBS_EVENT_ALARM_INTRUSION    (1 << 6)
/**
 * 人员逗留报警事件
 * 事件参数:uint8_t 触发状态
 * @var  触发状态和对应sensor
 */
#define VP_OBS_EVENT_ALARM_STAY         (1 << 7)
/**
 * 车辆违停报警事件
 * 事件参数:uint8_t 触发状态
 * @var  触发状态和对应sensor
 */
#define VP_OBS_EVENT_ALARM_ILLEGAL_PARKING (1 << 8)
/**
 * 越线报警事件
 * 事件参数:uint8_t 触发状态
 * @var  触发状态和对应sensor
 */
#define VP_OBS_EVENT_ALARM_EXCEED_LINE    (1 << 9)
/**
 * 离岗报警事件
 * 事件参数:uint8_t 触发状态
 * @var  触发状态和对应sensor
 */
#define VP_OBS_EVENT_ALARM_PERSON_OFFLINE      (1 << 10)
/**
 * 车辆逆行报警事件
 * 事件参数:uint8_t 触发状态
 * @var  触发状态和对应sensor
 */
#define VP_OBS_EVENT_ALARM_RETROGRADE      (1 << 11)
/**
 * 声音侦测报警事件
 * 事件参数:uint8_t 触发状态
 * @var  触发状态和对应sensor
 */
#define VP_OBS_EVENT_ALARM_SOUND      (1 << 12)

/**
 * 一键呼叫报警事件
 * 事件参数:uint8_t 触发状态
 * @var  触发状态和对应sensor
 */
#define VP_OBS_EVENT_ALARM_ONE_CALL      (1 << 13)

/**
 * 全部报警事件
 */
#define VP_OBS_EVENT_ALARM_ALL             (VP_OBS_EVENT_ALARM_TAMPER | VP_OBS_EVENT_ALARM_MOTION | VP_OBS_EVENT_ALARM_PERSON | \
                                            VP_OBS_EVENT_ALARM_SMOKE | VP_OBS_EVENT_ALARM_FIRE | VP_OBS_EVENT_ALARM_PACKAGE |   \
                                            VP_OBS_EVENT_ALARM_INTRUSION | VP_OBS_EVENT_ALARM_STAY | VP_OBS_EVENT_ALARM_ILLEGAL_PARKING | \
                                            VP_OBS_EVENT_ALARM_EXCEED_LINE | VP_OBS_EVENT_ALARM_PERSON_OFFLINE | VP_OBS_EVENT_ALARM_RETROGRADE | \
                                            VP_OBS_EVENT_ALARM_SOUND | VP_OBS_EVENT_ALARM_ONE_CALL)


/**
 * 状态灯事件类型
 */
#define VP_OBS_TYPE_LED        0x00001100

/**
 * 蓝绿灯常亮事件类型
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 已触发
 */
#define VP_OBS_EVENT_LED_ON    (1 << 0)

/**
 * 蓝绿灯常灭事件类型
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 已触发
 */
#define VP_OBS_EVENT_LED_OFF    (1 << 1)

/**
 * 蓝灯常亮事件类型
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 已触发
 */
#define VP_OBS_EVENT_BLUE_LED_ON    (1 << 2)

/**
 * 绿灯常亮事件类型
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 已触发
 */
#define VP_OBS_EVENT_GREEN_LED_ON    (1 << 3)

/**
 * 蓝灯慢闪事件类型
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 已触发
 */
#define VP_OBS_EVENT_BLUE_LED_BLINK    (1 << 4)

/**
 * 绿灯慢闪事件类型
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 已触发
 */
#define VP_OBS_EVENT_GREEN_LED_BLINK    (1 << 5)

/**
 * 蓝灯快闪事件类型
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 已触发
 */
#define VP_OBS_EVENT_BLUE_LED_FAST_BLINK    (1 << 6)

/**
 * 绿灯快闪事件类型
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 已触发
 */
#define VP_OBS_EVENT_GREEN_LED_FAST_BLINK    (1 << 7)

/**
 * 蓝绿灯交替慢闪事件类型
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 已触发
 */
#define VP_OBS_EVENT_BLUE_GREEN_BLINK    (1 << 8)

/**
 * 蓝绿灯交替快闪事件类型
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 已触发
 */
#define VP_OBS_EVENT_BLUE_GREEN_FAST_BLINK    (1 << 9)

/**
 * 蓝绿灯慢速双闪事件类型
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 已触发
 */
#define VP_OBS_EVENT_BLUE_GREEN_SIM_BLINK    (1 << 10)

/**
 * 蓝绿灯快速速双闪事件类型
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 已触发
 */
#define VP_OBS_EVENT_BLUE_GREEN_SIM_FAST_BLINK    (1 << 11)

/**
 * 工厂模式事件类型
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 已触发
 */
#define VP_OBS_EVENT_FACTORY_LED_MODE      (1 << 12)


/**
 * 全部状态灯事件类型
 */
#define VP_OBS_TYPE_LED_ALL                    (VP_OBS_EVENT_LED_ON | VP_OBS_EVENT_LED_OFF \
                                                | VP_OBS_EVENT_BLUE_LED_ON | VP_OBS_EVENT_GREEN_LED_ON \
                                                | VP_OBS_EVENT_BLUE_LED_BLINK | VP_OBS_EVENT_GREEN_LED_BLINK \
                                                | VP_OBS_EVENT_BLUE_LED_FAST_BLINK | VP_OBS_EVENT_GREEN_LED_FAST_BLINK \
                                                | VP_OBS_EVENT_BLUE_GREEN_BLINK | VP_OBS_EVENT_BLUE_GREEN_FAST_BLINK   \
                                                | VP_OBS_EVENT_BLUE_GREEN_SIM_BLINK | VP_OBS_EVENT_BLUE_GREEN_SIM_FAST_BLINK  \
                                                | VP_OBS_EVENT_FACTORY_LED_MODE)
/**
 * 移动侦测过滤事件类型
 */
#define VP_OBS_TYPE_MOTION_FILTER        0x00001200
/**
 * 白光灯操作过滤事件
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 开关触发
 */
#define VP_OBS_EVENT_WL_FILTER  (1 << 0)
/**
 * 夜视切换过滤事件
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 已触发
 */
#define VP_OBS_EVENT_DAY_NIGHT_FILTER   (1 << 1)

/**
 * 全部过滤事件
 */
#define VP_OBS_EVENT_MOTION_FILTER_ALL             (VP_OBS_EVENT_WL_FILTER | VP_OBS_EVENT_DAY_NIGHT_FILTER)

/**
 * 提示音下载播放事件类型
 */
#define VP_OBS_TYPE_VOICE_PROMPT        0x00001300
/**
 * 提示音下载播放事件
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 开关触发
 */
#define VP_OBS_EVENT_VOICE_UPDATE  (1 << 0)

/**
 * AOV事件类型
 */
#define VP_OBS_TYPE_AOV        0x00001400
/**
 * AOV录像事件
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 开关触发
 */
#define VP_OBS_EVENT_AOV_RECORD  (1 << 0)

/**
 * AOV云存变更事件
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 开关触发
 */
#define VP_OBS_EVENT_AOV_CLOUD  (1 << 1)

/**
 * AOV异常事件
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 开关触发
 */
#define VP_OBS_EVENT_AOV_ERROR  (1 << 2)

/**
 * 视频参数更新事件类型
 */
#define VP_OBS_TYPE_VIDEO_PARAM_CHANGE        0x00001600
/**
 * 视频参数osd更新事件
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 开关触发
 */
#define VP_OBS_EVENT_OSD_CHANGE       (1 << 0)
/**
 * 视频参数区域更新事件
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 开关触发
 */
#define VP_OBS_EVENT_DETECT_AREA_CHANGE       (1 << 1)
/**
 * 全部视频参数更新事件
 */
#define VP_OBS_EVENT_VIDEO_PARAM_CHANGE_ALL             (VP_OBS_EVENT_OSD_CHANGE | VP_OBS_EVENT_DETECT_AREA_CHANGE)

/**
 * 涂鸦产测事件类型
 */
#define VP_OBS_TYPE_TUYA_FACTORY_TEST      0x00001700
/**
 * 白光灯状态更新事件
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 开关触发
 */
#define VP_OBS_EVENT_TUYA_WHITE_CTRL      (1 << 0)
/**
 * IRCUT状态更新事件
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 开关触发
 */
#define VP_OBS_EVENT_TUYA_IRCUT_CTRL      (1 << 1)
/**
 * 设备复位事件
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 开关触发
 */
#define VP_OBS_EVENT_TUYA_RESET           (1 << 2)
/**
 * 设备复位事件
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 开关触发
 */
#define VP_OBS_EVENT_TUYA_WRITE_PID       (1 << 3)
/**
 * 设备复位事件
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 开关触发
 */
#define VP_OBS_EVENT_TUYA_WRITE_UUID       (1 << 4)


/**
 * GPIO 事件
 */
#define VP_OBS_TYPE_GPIO            0x00001800

/**
 * GPIO更改事件
 * 事件参数:uint8_t 就绪状态
 * @var 0 未就绪
 * @var 1 已就绪
 */
#define VP_OBS_EVENT_GPIO_READY     (1 << 0)


/**
 * 系统事件类型
 */
#define VP_OBS_TYPE_SYSTEM          0x0000F000

/**
 * 时间更改事件
 * 事件参数:@[struct timespec]
 */
#define VP_OBS_EVENT_SYSTEM_TIME    (1 << 0)

/**
 * 网络状态更改事件
 * 事件参数:uint8_t 外网连接状态
 * @var 0 无法连接外部网络
 * @var 1 成功外部连接网络
 */
#define VP_OBS_EVENT_SYSTEM_NETWORK (1 << 1)

/**
 * 设备重置事件
 * 事件参数:uint8_t 设备重启
 * @var 0 未触发
 * @var 1 触发
 */
#define VP_OBS_EVENT_RESET     (1 << 2)

/**
 * 设备关机事件
 * 事件参数:uint8_t 设备关机
 * @var 0 未触发
 * @var 1 触发
 */
#define VP_OBS_EVENT_SHUTDOWN    (1 << 3)

/**
 * 设备重启事件
 * 事件参数:uint8_t 设备重启
 * @var 0 未触发
 * @var 1 触发
 */
#define VP_OBS_EVENT_REBOOT     (1 << 4)

/**
 * 有线网络接入
 */
#define VP_OBS_EVENT_WIRED_INPUT (1 << 5)

/**
 * 有线网络移除
 */
#define VP_OBS_EVENT_WIRED_REMOVE (1 << 6)

/**
 * 蓝牙服务程序错误
 */
#define VP_OBS_EVENT_BLE_SERVER_ERROR (1 << 7)

/**
 * 设备属性事件类型
 */
#define VP_OBS_TYPE_DEVICE             0x0000FF00
/**
 * 设备ID更改事件
 * 事件参数:@[const char *] 设备ID
 */
#define VP_OBS_EVENT_DEVICE_DID         (1 << 0)
/**
 * 设备虚拟ID更改事件
 * 事件参数:@[const char *] 设备虚拟ID
 */
#define VP_OBS_EVENT_DEVICE_VID         (1 << 1)
/**
 * 进入产测模式事件
 * 事件参数:uint8_t 产测模式状态
 * @var 0 未触发
 * @var 1 触发
 */
#define VP_OBS_EVENT_ENTRY_FACTORY_MODE     (1 << 2)
/**
 * 重新授权事件
 * 事件参数:uint8_t 重新授权
 * @var 0 未触发
 * @var 1 触发
 */
#define VP_OBS_EVENT_AUTH     (1 << 3)

/**
 * 获取授权成功事件
 * 事件参数:uint8_t 重新授权
 * @var 0 未触发
 * @var 1 触发
 */
#define VP_OBS_EVENT_AUTH_SUCCESS     (1 << 4)

/**
 * 设备唤醒事件
 * 事件参数:uint8_t 设备唤醒
 * @var 0 未触发
 * @var 1 触发
 */
#define VP_OBS_EVENT_DEVICE_WAKEUP    (1 << 5)

/**
 * 设备生产参数变更事件
 * 事件参数:uint8_t 参数变更
 * @var 0 未触发
 * @var 1 触发
 */
#define VP_OSB_EVENT_FACTORY_PARAM_CHANGE     (1 << 6)

/**
 * 设备绑定结束事件
 * 事件参数:uint8_t 绑定结束
 * @var 0 未触发
 * @var 1 触发
 */
#define VP_OBS_EVENT_BIND_END       (1 << 7)

/**
 * P2P状态更改事件
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 开关触发
 */
#define VP_OBS_EVENT_P2P_CHANGE     (1 << 8)

/**
 * 设备ID删除事件
 * 事件参数:uint8_t 删除
 * @var 0 未触发
 * @var 1 触发
 */
#define VP_OBS_EVENT_DEVICE_DID_DEL (1 << 9)

/**
 * 存储卡事件类型
 */
#define VP_OBS_TYPE_STORAGE             0x0000EE00
/**
 * 存储卡状态更改事件
 * 事件参数:@[vp_storage_state_t] 状态
 */
#define VP_OBS_EVENT_STORAGE_STATE      (1 << 0)

/**
 * 时间计划事件类型
 */
#define VP_OBS_TYPE_TIMEPLAN            0x0000EE01
/**
 * 移动侦测计划状态更改事件
 */
#define VP_OBS_EVENT_TIMEPLAN_MOTION        (1 << 0)
/**
 * 定时录像计划状态更改事件
 */
#define VP_OBS_EVENT_TIMEPLAN_RECORD        (1 << 1)
/**
 * 白光灯计划状态更改事件
 */
#define VP_OBS_EVENT_TIMEPLAN_WHITE_LIGHT   (1 << 2)
/**
 * 隐私位计划状态更改事件
 */
#define VP_OBS_EVENT_TIMEPLAN_PRIVACY       (1 << 2)

/**
 * ISP事件类型
 */
#define VP_OBS_TYPE_ISP                 0x0000EF00

/**
 * 光敏状态更改事件
 */
#define VP_OBS_EVENT_ISP_PHOTOSENSITIVE_STATE    (1 << 0)

/**
 * 远近焦镜头切换事件
 * 事件参数:uint8_t 触发状态
 * @var 0 未触发
 * @var 1 开关触发
 */
#define VP_OBS_EVENT_ISP_MIPI_SWITCH_CHANGE     (1 << 1)

/**
 * 初始化观察者模块
 */
void vp_observers_init();

/**
 * 添加事件监听
 *
 * @param type      [in]    事件类型
 * @param event     [in]    事件ID
 * @param callback  [in]    回调函数
 * @param user_info [in]    回调参数
 *
 * @return 监听对象
 *
 * @retval !NULL 成功
 * @retval  NULL 失败
 */
vp_obs_item_p vp_observers_add(uint32_t type, uint32_t event, vp_obs_callback_f callback, void *user_info);

/**
 * 移除监听对象
 *
 * @param type  [in]    事件类型
 * @param event [in]    监听对象
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_observers_rmv(uint32_t type, vp_obs_item_p item);

/**
 * 推送事件给监听者
 *
 * @param type  [in]    事件类型
 * @param event [in]    事件ID
 * @param args  [in]    事件参数
 *
 * @return 推送成功个数
 *
 * @retval > 0 成功个数
 * @retval < 0 推送失败
 */
int vp_observers_post(uint32_t type, uint32_t event, void *args);

/**
 * 反初始化观察者模块
 */
void vp_observers_deinit();

#ifdef __cplusplus
}
#endif

#endif
    