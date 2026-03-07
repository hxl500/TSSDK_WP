/************************************************************************************************
 * This source code is CONFIDENTIAL and any use by you is subject to the terms of the agreement
 * between you and Arm China or the terms of the agreement between you and the party authorised
 * by Arm China to disclose this document to you.
 *
 * The confidential and proprietary information contained in this file may only be used by a
 * person authorised under and to the extent permitted by a subsisting licensing agreement from
 * Arm China.
 *
 * This entire notice must be reproduced on all copies of this file and copies of this file may
 * only be made by a person if such person is permitted to do so under the terms of a subsisting
 * license agreement from Arm China.
 *
 * Release Information: Linlon-i5-r0p2-00eac0
 *
 * Copyright (C) 2021 Arm Technology (China) Co. Ltd. All rights reserved.
 *
 ***********************************************************************************************/
#ifndef __ARMCB_ISP_H__
#define __ARMCB_ISP_H__

#ifdef __linux__
#include <linux/types.h>
#include <linux/videodev2.h>
#endif

/// Define data type
typedef enum
{
    DRV_DATA_TYPE_INVALID       = 0,
    DRV_DATA_TYPE_BYTE          = 1,
    DRV_DATA_TYPE_WORD          = 2,
    DRV_DATA_TYPE_DWORD         = 3,
    DRV_DATA_TYPE_WORD_REVERSE  = 4,
    DRV_DATA_TYPE_MAX
} drv_data_type;

/// Define address type
typedef enum
{
    DRV_ADDR_TYPE_INVALID       = 0,
    DRV_ADDR_TYPE_BYTE          = 1,
    DRV_ADDR_TYPE_WORD          = 2,
    DRV_ADDR_TYPE_WORD_REVERSE  = 3,
    DRV_ADDR_TYPE_MAX
} drv_addr_type;

/// Define direction
typedef enum
{
    DRV_DIRECTION_WRITE         = 0,
    DRV_DIRECTION_READ          = 1,
    DRV_DIRECTION_POLL          = 2,
} drv_direction;

/// Define type of hardware connect
typedef enum
{
    DRV_HW_I2C = 0,
    DRV_HW_SPI,
    DRV_HW_ISP,
    DRV_HW_MAX,
} hw_type_t;

/// Define type of hardware bus connect
typedef enum
{
    HW_BUS_I2C = 0 ,
    HW_BUS_SPI,
    HW_BUS_GPIO,
    HW_BUS_PMIC,
    HW_BUS_AHB,
    HW_BUS_AHB_POWER,
    HW_BUS_DMA_REG,
    HW_BUS_DMA_SRAM,
    HW_BUS_MAX,
} hw_bus_type_t;

/// Define type of device
typedef enum
{
    DRV_DEV_SENSOR = 0,
    DRV_DEV_ACTUOAOR,
    DRV_DEV_ISP,
    DRV_DEV_CSI,
    DRV_DEV_MAX,
} dev_type_t;

typedef enum
{
    DRV_STREAM_INIT             =0,
    DRV_STREAM_ON               =1,
    DRV_STREAM_START            =2,
    DRV_STREAM_UNINIT           =3,
    DRV_STREAM_CAM_FPS_SYNC     =4,
    DRV_STREAM_BUTT,
} drv_streamctrl_type_t;

/// Define power type
typedef enum
{
    DRV_POWER_INVALID         = 0,
    DRV_POWER_REG             = 1,
    DRV_POWER_GPIO            = 2,
    DRV_POWER_PMIC            = 3,
    DRV_POWER_MAX,
} drv_power_type;

/// Define register address and value
typedef struct
{
    u32 addr;       /// register address
    u32 val;        /// register configure value
    u32 sofDelay;   /// Delay(us) of register operation;
} reg_cfg_t;

/// Define sensor single register information
typedef struct
{
    u32 regAddr;                       /// Address of register
    u32 regData;                       /// Data of register
    u32 delayUs;                       /// Delay(us) of register operation;
} drv_sensor_reg_info_t;


/// Define register array information
typedef struct
{
    drv_data_type           regDataType;                              /// register data type
    drv_addr_type           regAddrType;                              /// register address type
    drv_direction           direction;                                /// Read or write
    drv_streamctrl_type_t   stream_ctrl_type;                         /// Init OR SteamOn
    u32                     settingSize;                              /// Size of register array
    u32                     settingCnt;
    drv_sensor_reg_info_t  *settings;                                 /// Register array
} drv_regs_array_info_t;

/// register configuration structure
typedef struct __isp_reg_list_t
{
    u16                         num;      /// size of isp register cfg buf
    reg_cfg_t                   *cfg;      /// address and value pair
    struct __isp_reg_list_t     *next;     /// point to next register hw update block
} isp_reg_list_t;

/// Define registers array configure
typedef struct
{
    u32                         camd_id;
    u32                         channel;
    u32                         slaveAddr;
    drv_regs_array_info_t       *pRegsInfo;
} isp_hw_regs_cfg_t;

/// Define register list configure
typedef struct
{
    u32                         frameIndex;
    struct __isp_reg_list_t     *pUpdateList;
} isp_hw_list_cfg_t;

/// Define hardware request
typedef struct
{
    hw_type_t                   hwType;    /// Hardware type
    dev_type_t                  devType;   /// Device type
	u32                         cam_id;    /// cam id
    u32                         cam_mode;  /// cam mode
    void                        *argv;     /// argv of hardware request
} isp_hw_req_t;

/// I2C setting
typedef struct __cmd_i2c_setting
{
    u32             channel;
    u32             slaveAddr;
    drv_direction   direct;
    drv_addr_type   regAddrType;
    drv_data_type   regDataType;
    u32             regAddr;
    u32            *ptrUser;
    u32             val;
    u32             delayUs;
} cmd_i2c_setting_t;

/// SPI setting
typedef struct __cmd_spi_setting
{
    u32             channel;
    u32             slaveAddr;
    drv_direction   direct;
    drv_addr_type   regAddrType;
    drv_data_type   regDataType;
    u32             regAddr;
    u32            *ptrUser;
    u32             val;
    u32             delayUs;
} cmd_spi_setting_t;

/// AHB setting
typedef struct __cmd_ahb_setting
{
    drv_direction   direct;
    u32             regAddr;
    u32            *ptrUser;
    u32             val;
    u32             delayUs;
} cmd_ahb_setting_t;

/// AHB Power setting
typedef struct __cmd_ahb_power_setting
{
    u32             regAddr;       /// reg address
    u32             bitMask;       /// bit mask of register data
    BOOL            bitVal;        /// bit value of register data
    u32             delayUs;       /// delay by us
} cmd_ahb_power_setting_t;

/// DMA AHB setting
typedef struct __cmd_dma_reg_setting
{
    u32         regAddrOffset;
    u32         val;
} cmd_dma_reg_setting_t;

/// DMA SRAM setting
typedef struct __cmd_dma_sram_setting
{
    u64         sramAddrStart;
    u32         val[1];
} cmd_dma_sram_setting_t;

/// cmd apply condition
typedef enum __cmd_condition
{
    CMD_COND_IMMEDIATELY  = 0,
    CMD_COND_ISP_INPUT_SOF,
    CMD_COND_ISP_OUTPUT_SOF,
    CMD_COND_STREAMOFF,
    CMD_COND_POWERDOWN,
    CMD_COND_MAX,
} cmd_condition_t;

/// cmd type
typedef enum __cmd_type
{
    CMD_TYPE_POWERUP    = 0,
    CMD_TYPE_INIT,
    CMD_TYPE_STREAMON,
    CMD_TYPE_UPDATE,
    CMD_TYPE_STREAMOFF,
    CMD_TYPE_POWERDOWN,
    CMD_TYPE_STARTUP,
    CMD_TYPE_SHUTDOWN,
    CMD_TYPE_QUERY_DYFPS_SYNC_DONE,
    CMD_TYPE_MAX,
} cmd_type_t;

/// cmd shared info
typedef struct __cmd_info
{
    u32                camId;
    dev_type_t         dev;
    hw_bus_type_t      bus;
    cmd_type_t         cmdType;
    cmd_condition_t    triggerCond;
    u32                maxCmdCnt;
    u32                maxBufNum;
} cmd_info_t;

/// cmd buf status
typedef enum __cmd_status
{
    CMD_STATUS_IDEL = 0,
    CMD_STATUS_ACQUIRED,
    CMD_STATUS_SUBMIT,
    CMD_STATUS_APPLIED,
    CMD_STATUS_EFFECTIVE,
    CMD_STATUS_OVERDUE,
    CMD_STATUS_MAX,
} cmd_status_t;

/// cmd setting
typedef union __cmd_setting
{
    cmd_i2c_setting_t        i2c[1];
    cmd_spi_setting_t        spi[1];
    cmd_ahb_setting_t        ahb[1];
    cmd_ahb_power_setting_t  ahbPower[1];
    cmd_dma_reg_setting_t    dmaReg[1]; /// @TODO: require extra ahb cmd for dma trigger
    cmd_dma_sram_setting_t   dmsSram;
} cmd_setting_t;

typedef struct __cmd_priv
{
    void  *pStreamBuf;
    void **ppCb;
} cmd_priv_t;

/// Continue command buffer
typedef struct __cmd_buf
{
    cmd_info_t             info;
    cmd_priv_t             priv;
    cmd_status_t           status;
    u32                    applyFrameId;
    u32                    effectFrameId;
    u32                    cmdCnt;
    cmd_setting_t          settings;
} cmd_buf_t;

typedef struct priv_cmd_s {
	dev_type_t      dev;
	cmd_type_t      cmd_type;
} priv_cmd_t;

typedef struct
{
    u8               name[64];
    u8               isUseBuffer;
    s32              id;
    u32              offset;
    u32              len;
    /// @TODO: change addr type to void*
    u32              phyAddr;
    INTPTR              memAddr;
    void             *kernelAddr;
} mem_block_t;

struct armcb_isp_stat_isr_info
{
    u32              statType;
    u32              sensorId;
    u32              flags;
#ifndef PC_SIMU
    struct timeval   timeStamp;
#endif
};

struct armcb_isp_stat_event_status
{
    u32              frameNumber;
    u16              configCounter;
    u8               bufErr;
};

struct armcb_dma_req
{
    u32              size;
    u32              rmtAddr;
    void             *buf;
};

struct armcb_xdma_req
{
    u32              size;
    u32              direction;
    u32              rmtAddr;
    u32              localAddr;
};

/*
 * Private IOCTLs
 *
 * VIDIOC_ARMCB_ISP_STAT_EN: Enable/disable a statistics module
 * VIDIOC_ARMCB_ISP_STAT_BUF_REQ: Require statistics buffer
 * VIDIOC_ARMCB_ISP_STAT_BUF_QUERY: Query statistics buffer info
 * VIDIOC_ARMCB_ISP_STAT_BUF_DQ: Enqueue the statistics buffer
 * VIDIOC_ARMCB_ISP_STAT_BUF_EQ: Dequeue the statistics buffer
 * VIDIOC_ARMCB_ISP_STAT_STREAM_ON: Start statistics
 * VIDIOC_ARMCB_ISP_STAT_STREAM_OFF: Stop statistics
 */
#define ARMCB_IOCTL_CMD_PRIVATE_START  (BASE_VIDIOC_PRIVATE) /* 192-255 are private */

#define ARMCB_VIDIOC_S_REG_LIST        (ARMCB_IOCTL_CMD_PRIVATE_START + 0)
#define ARMCB_VIDIOC_G_REG_LIST        (ARMCB_IOCTL_CMD_PRIVATE_START + 1)
#define ARMCB_VIDIOC_APPLY_CMD         (ARMCB_IOCTL_CMD_PRIVATE_START + 2)

#define VIDIOC_ARMCB_ISP_STAT_EN \
    _IOWR('V', BASE_VIDIOC_PRIVATE + 2, unsigned long)
#define VIDIOC_ARMCB_ISP_STAT_BUF_REQ \
    _IOWR('V', BASE_VIDIOC_PRIVATE + 3, struct v4l2_requestbuffers)
#define VIDIOC_ARMCB_ISP_STAT_BUF_QUERY \
    _IOWR('V', BASE_VIDIOC_PRIVATE + 4, struct v4l2_buffer)
#define VIDIOC_ARMCB_ISP_STAT_BUF_DQ \
    _IOWR('V', BASE_VIDIOC_PRIVATE + 5, struct v4l2_buffer)
#define VIDIOC_ARMCB_ISP_STAT_BUF_EQ \
    _IOWR('V', BASE_VIDIOC_PRIVATE + 6, struct v4l2_buffer)
#define VIDIOC_ARMCB_ISP_STAT_STREAM_ON \
    _IOWR('V', BASE_VIDIOC_PRIVATE + 7, int)
#define VIDIOC_ARMCB_ISP_STAT_STREAM_OFF \
    _IOWR('V', BASE_VIDIOC_PRIVATE + 8, int)

#define CMEM_IOCTL_MAGIC             'm'
#define CMEM_ALLOCATE                _IOW(CMEM_IOCTL_MAGIC, 1, mem_block_t)
#define CMEM_FREE                    _IOW(CMEM_IOCTL_MAGIC, 2, mem_block_t)
#define ARMCB_VIDIOC_ISP_XDMA        _IOWR(CMEM_IOCTL_MAGIC, 3, struct armcb_xdma_req)
#define ARMCB_VIDIOC_ISP_DMA_COPY    194

#define VIDIOC_S_PLATFORM_HW_INIT          0x8000

#endif
