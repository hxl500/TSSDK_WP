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

#include "actuator_ms41929.h"

static const u8 gucNibbleBitsMirror[] =
{
    0x0, 0x8, 0x4, 0xC, 0x2, 0xA, 0x6, 0xE,
    0x1, 0x9, 0x5, 0xD, 0x3, 0xB, 0x7, 0xF,
};


/// Switch the bit order of a byte.
static u8 Mirror_Byte(u8 src)
{
    u8 nibbleL = src & 0x0f;
    u8 nibbleH = (src >> 4) & 0x0f;
    u8 tmpL = gucNibbleBitsMirror[nibbleH];
    u8 tmpH = gucNibbleBitsMirror[nibbleL];
    u8 res  = ((tmpH << 4) & 0xf0) | ((tmpL << 0) & 0x0f);

    return res;
}


static drv_sensor_reg_info_t ms41929_init_setting[] =
{
    {.regAddr = 0x0b, .regData = 0x0000, .delayUs = 0},
    {.regAddr = 0x2c, .regData = 0x0004, .delayUs = 0},
    {.regAddr = 0x20, .regData = 0x1e03, .delayUs = 0},
    {.regAddr = 0x22, .regData = 0x0002, .delayUs = 0},
    {.regAddr = 0x23, .regData = 0xc0c0, .delayUs = 0},
    {.regAddr = 0x24, .regData = 0x0400, .delayUs = 0},
    {.regAddr = 0x25, .regData = 0x0400, .delayUs = 0},
    {.regAddr = 0x27, .regData = 0x0002, .delayUs = 0},
    {.regAddr = 0x28, .regData = 0xc0c0, .delayUs = 0},
    {.regAddr = 0x29, .regData = 0x0c00, .delayUs = 0},
    {.regAddr = 0x2a, .regData = 0x0400, .delayUs = 0},
};

/// Actuator init callback, initial configration should be filled here
static u32 ms41929_init(actuator_init_params_t* initParams, drv_regs_array_info_t* pRegInfo)
{
    s32 i = 0;
    s32 initSize = sizeof(ms41929_init_setting) / sizeof(ms41929_init_setting[0]);
    pRegInfo->direction   = DRV_DIRECTION_WRITE;
    pRegInfo->regAddrType = DRV_ADDR_TYPE_BYTE;
    pRegInfo->regDataType = DRV_DATA_TYPE_WORD;
    for(i = 0; i < initSize; i++)
    {
        pRegInfo->settings[pRegInfo->settingSize].regAddr =
            Mirror_Byte((ms41929_init_setting[i].regAddr & 0x3F) | (0x00 << 6));

        pRegInfo->settings[pRegInfo->settingSize].regData =
            (Mirror_Byte((u8)(ms41929_init_setting[i].regData >> 8)) << 8) |
             Mirror_Byte((u8)(ms41929_init_setting[i].regData >> 0));

        pRegInfo->settingSize ++;
    }
    return 0;
}


/// Actuator configure callback, should fill the register value according to the configParams here
static u32 ms41929_config(actuator_config_params_t *pConfigParams, drv_regs_array_info_t *pRegInfo)
{

    // Reg[0x29]:
    //   [13:12]: MICROCD, micro step selection, 2'b11=64, 2'b10=128, others=256.
    //    [11]: LEDA, 1=on, 0=off.
    //    [10]: ENDISCD, motor driver AB disable(=0)/enable(=1).
    //     [9]: BRAKECD, 1=Brake motor AB, 0=normal running.
    //     [8]: CCWCWCD, move direction, 0=forward, 1=backward.
    //   [ 7: 0]: PSUMCD, (PSUMAB/8)=total steps.

    // Reg[0x24]:
    //   [13:12]: MICROAB, micro step selection, 2'b11=64, 2'b10=128, others=256.
    //    [11]: LEDB, 1=on, 0=off.
    //    [10]: ENDISAB, motor driver AB disable(=0)/enable(=1).
    //     [9]: BRAKEAB, 1=Brake motor AB, 0=normal running.
    //     [8]: CCWCWAB, move direction, 0=forward, 1=backward.
    //   [ 7: 0]: PSUMAB, (PSUMAB/8)=total steps.

    /// MICROCD means focus , MICROAB means zoom
    u32 focusRegData        = 0x0c00;    /// Initial MICROCD register value
    u32 zoomRegData         = 0x0400;
    const u32 focusRegAddr  = 0x29;
    const u32 zoomRegAddr   = 0x24;

    pRegInfo->regAddrType = DRV_ADDR_TYPE_BYTE;
    pRegInfo->regDataType = DRV_DATA_TYPE_WORD;
    pRegInfo->direction   = DRV_DIRECTION_WRITE;

    if (pConfigParams->currFocusPos == pConfigParams->nextFocusPos
        && pConfigParams->currZoomPos == pConfigParams->nextZoomPos)
    {
        focusRegData  = 0x0c00;
        zoomRegData   = 0x0400;
    }

    if (pConfigParams->currFocusPos > pConfigParams->nextFocusPos )
    {
        focusRegData |= 48;            /// Set total steps
        focusRegData |= (0x01 << 8);   /// Set bit[8] to '1'
    }
    else if (pConfigParams->currFocusPos < pConfigParams->nextFocusPos )
    {
        focusRegData |= 48;           /// Set total steps
        focusRegData &= ~(0x01 <<8);  /// Set bit[8] to '0'
    }
    else if (pConfigParams->currZoomPos > pConfigParams->nextZoomPos )
    {
        zoomRegData  |= 48;            /// Set total steps
        zoomRegData  |= (0x01 << 8);   /// Set bit[8] to '1'
    }
    else if (pConfigParams->currZoomPos < pConfigParams->nextZoomPos )
    {
        zoomRegData  |= 48;           /// Set total steps
        zoomRegData  &= ~(0x01 <<8);  /// Set bit[8] to '0'
    }

    pRegInfo->settings[pRegInfo->settingSize].regAddr =
        Mirror_Byte((focusRegAddr & 0x3F) | (0x00 << 6));

    pRegInfo->settings[pRegInfo->settingSize].regData =
        (Mirror_Byte((u8)(focusRegData >> 8)) << 8) |
         Mirror_Byte((u8)(focusRegData >> 0));

    pRegInfo->settingSize ++;


    pRegInfo->settings[pRegInfo->settingSize].regAddr =
        Mirror_Byte((zoomRegAddr & 0x3F) | (0x00 << 6));

    pRegInfo->settings[pRegInfo->settingSize].regData =
        (Mirror_Byte((u8)(zoomRegData >> 8)) << 8) |
         Mirror_Byte((u8)(zoomRegData >> 0));

    pRegInfo->settingSize ++;

    /// RegAddr: 0x2a  RegData: 0x300
    pRegInfo->settings[pRegInfo->settingSize].regAddr =
        Mirror_Byte((0x2a & 0x3F) | (0x00 << 6));

    pRegInfo->settings[pRegInfo->settingSize].regData =
        (Mirror_Byte((u8)(0x300 >> 8)) << 8) |
         Mirror_Byte((u8)(0x300 >> 0));

    pRegInfo->settingSize ++;

//  SENSOR_CFG("focusReg Addr: %x Data: %x ", focusRegAddr, focusRegData);
//  SENSOR_CFG("zoomReg Addr: %x Data: %x ", zoomRegAddr, zoomRegData);

    return 0;
}


static u32 ms41929_IR_Cut_Process(actuator_config_params_t *pConfigParams, drv_regs_array_info_t *pRegInfo)
{

    pRegInfo->regAddrType = DRV_ADDR_TYPE_BYTE;
    pRegInfo->regDataType = DRV_DATA_TYPE_WORD;
    pRegInfo->direction   = DRV_DIRECTION_WRITE;

    /* turn on ir cut*/
    if (pConfigParams->irCutEnable)
    {
        pRegInfo->settings[pRegInfo->settingSize].regAddr =
            Mirror_Byte((0x2c & 0x3F) | (0x00 << 6));

        pRegInfo->settings[pRegInfo->settingSize].regData =
            (Mirror_Byte((u8)(0x05 >> 8)) << 8) |
             Mirror_Byte((u8)(0x05 >> 0));

        pRegInfo->settings[pRegInfo->settingSize].delayUs = 10000; ///delay 10ms
        pRegInfo->settingSize ++;

        pRegInfo->settings[pRegInfo->settingSize].regAddr =
            Mirror_Byte((0x2c & 0x3F) | (0x00 << 6));

        pRegInfo->settings[pRegInfo->settingSize].regData =
            (Mirror_Byte((u8)(0x04 >> 8)) << 8) |
             Mirror_Byte((u8)(0x04 >> 0));

        pRegInfo->settingSize ++;

    }
    else
    {
        /* turn off iq cut*/
        pRegInfo->settings[pRegInfo->settingSize].regAddr =
            Mirror_Byte((0x2c & 0x3F) | (0x00 << 6));

        pRegInfo->settings[pRegInfo->settingSize].regData =
            (Mirror_Byte((u8)(0x06 >> 8)) << 8) |
             Mirror_Byte((u8)(0x06 >> 0));

        pRegInfo->settings[pRegInfo->settingSize].delayUs = 10000; ///delay 10ms
        pRegInfo->settingSize ++;

        pRegInfo->settings[pRegInfo->settingSize].regAddr =
            Mirror_Byte((0x2c & 0x3F) | (0x00 << 6));

        pRegInfo->settings[pRegInfo->settingSize].regData =
            (Mirror_Byte((u8)(0x04 >> 8)) << 8) |
             Mirror_Byte((u8)(0x04 >> 0));

        pRegInfo->settingSize ++;

    }

    return 0;
}

static drv_power_setting_t ms41929_power_up_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x02, .bitVal = 0x00, .delayUs = 20000}, /// 20ms
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x02, .bitVal = 0x01, .delayUs = 20000},
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x04, .bitVal = 0x00, .delayUs = 10000},
};


static drv_power_setting_t ms41929_power_down_setting[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x02, .bitVal = 0x00, .delayUs = 20000},
};

static drv_power_setting_t ms41929_plus_powerCfg[] =
{
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x04, .bitVal = 0x01, .delayUs = 1400}, /// pulse duration 1.4ms
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x04, .bitVal = 0x00, .delayUs = 0}
};

static actuator_plus_cfg_t ms41929_pulse_cfg =
{
    .period    = 33000,               /// 33ms
    .powerCfg   =
    {
        .powerSetting = ms41929_plus_powerCfg,
        .size         = sizeof(ms41929_plus_powerCfg) / sizeof(ms41929_plus_powerCfg[0]),
    },
};

/// Actuator driver parameters
static actuator_drv_params_t ms41929_actuator_params =
{
    .name      = "ms41929_actuator",
    .slaveInfo =
    {
        .hwType  = DRV_HW_SPI,
        .channel = 0,
    },
    .actuatorType = DRV_ACTUATOR_STEPPER_FOCUS_ZOOM,

    .powerUpSettings   =
    {
        .powerSetting  = ms41929_power_up_setting,
        .size = sizeof(ms41929_power_up_setting) / sizeof(ms41929_power_up_setting[0]),
    },
    .powerDownSettings =
    {
        .powerSetting  = ms41929_power_down_setting,
        .size = sizeof(ms41929_power_down_setting) / sizeof(ms41929_power_down_setting[0]),
    },

    .initSettings =
    {
        .direction   = DRV_DIRECTION_WRITE,
        .regAddrType = DRV_ADDR_TYPE_BYTE,
        .regDataType = DRV_DATA_TYPE_WORD,
        .settingSize = sizeof(ms41929_init_setting) / sizeof(ms41929_init_setting[0]),
        .settings    = ms41929_init_setting,
    },

    .initFunc     = ms41929_init,
    .configFunc   = ms41929_config,
    .irCfgFunc    = ms41929_IR_Cut_Process,
    .pulseCfg     = &ms41929_pulse_cfg, /// only for stepper
    .focusParams  =
    {
        .initPosition = 200,
        .minPosition  = 0,
        .maxPosition  = 400,
    },
    .zoomParams  =
    {
        .initPosition = 40,
        .minPosition  = 0,
        .maxPosition  = 120,
    },

};

actuator_drv_params_t * Load_Ms41929_Actuator_Driver(void)
{
    return &ms41929_actuator_params;
}


