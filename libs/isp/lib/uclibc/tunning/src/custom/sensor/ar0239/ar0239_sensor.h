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

#ifndef __AR0239_SENSOR_H__
#define __AR0239_SENSOR_H__

#include "sensor_common.h"
#include "log_utils.h"

isp_ret_t AR0239_Fill_Exposure_Reg(sensor_apply_exp_param_t * pExpParam, drv_regs_array_info_t * pRegInfo);

static drv_sensor_reg_info_t  ar0239_normal_setting[] =
{
    { .regAddr = 0x3088, .regData = 0x8000, .delayUs = 0x0,},  // SEQ_CTRL_PORT
    { .regAddr = 0x3086, .regData = 0x4558, .delayUs = 0x0,},  // SEQ_DATA_PORT
    { .regAddr = 0x3086, .regData = 0x72A6, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x4A31, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x43C5, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x2A08, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x42CE, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xCB8E, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x0345, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x782A, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x1C7B, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xCA3D, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xFF3D, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xFF3D, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xEA2A, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x0C3D, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x102A, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x0D2A, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x1D35, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x2A0D, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x3D10, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x4558, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x2A0C, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x8E03, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x2A1C, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x3DFF, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x3DFF, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x3DEA, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x2A0C, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x62CC, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xCF2A, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x288E, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x0036, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x2A08, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x3D64, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x7ACD, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x3D04, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x442A, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x982C, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x4BA4, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xC257, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x0043, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x0C2D, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x6343, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x5FC4, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xC52A, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xFCC8, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xC553, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x0553, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x078E, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x034D, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x2BF8, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x2924, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x29AE, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x3F07, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x45DC, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x5C19, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x494C, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x095C, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x1B2A, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xFE8E, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x0F3F, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x092B, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xB82B, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x984E, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x45CC, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x1645, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x8C3F, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x008E, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x0D29, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xBA60, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x3F00, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x5C0B, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x2AFA, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x3F32, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x5C03, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x3F23, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x3E10, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x5F2A, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xF22B, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x902B, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x80C7, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x3F08, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x6029, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xAAD0, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x2983, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x5F4D, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x1945, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xAC2A, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xFAC6, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x3E05, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x2AFB, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x5D88, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x103F, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x2F45, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x8829, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x9221, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x2B04, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x853F, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x042A, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xFA8E, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x098D, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x4D4E, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x482B, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x804C, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x0C60, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x3F30, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xC7C3, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x3F0B, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x2AF2, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x3F08, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x3F08, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x2982, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xC6D0, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xD245, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x58D3, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x5F4D, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x192A, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xFAC9, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x2A98, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x8E00, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x1244, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x4A04, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x4316, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x0543, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x1658, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x4316, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x5A43, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x1606, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x4316, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x0743, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x168E, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x0345, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x783F, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x052A, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x9D5D, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x8811, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x2B04, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x530D, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x8629, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x448D, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x4D2B, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x643F, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x2045, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x5825, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x3F05, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x8E00, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x2A98, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x1244, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x4BC1, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0xA6B9, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x2C2C, .delayUs = 0x0,},
    { .regAddr = 0x3086, .regData = 0x2C2C, .delayUs = 0x0,},    // SEQ_DATA_PORT
    { .regAddr = 0x3ED8, .regData = 0xF9EE, .delayUs = 20000,},    // DAC_LD_12_13

///delay ms
    { .regAddr = 0x3EFC, .regData = 0xE0FF, .delayUs = 0x0,},    // DAC_LD_48_49
    { .regAddr = 0x30AE, .regData = 0x0003, .delayUs = 0x0,},    // X_ODD_INC_CB
    { .regAddr = 0x3092, .regData = 0x006F, .delayUs = 0x0,},    // ROW_NOISE_CONTROL
    { .regAddr = 0x317E, .regData = 0x8F70, .delayUs = 0x0,},    // DELTA_DK_CLIP
    { .regAddr = 0x3180, .regData = 0x8040, .delayUs = 0x0,},    // DELTA_DK_CONTROL
    { .regAddr = 0x3ED2, .regData = 0x00B9, .delayUs = 0x0,},    // DAC_LD_6_7
    { .regAddr = 0x3ED6, .regData = 0x14A1, .delayUs = 0x0,},    // DAC_LD_10_11
    { .regAddr = 0x3ED8, .regData = 0xE7CC, .delayUs = 0x0,},    // DAC_LD_12_13
    { .regAddr = 0x3EDA, .regData = 0x6B40, .delayUs = 0x0,},    // DAC_LD_14_15
    { .regAddr = 0x3EDC, .regData = 0x08C5, .delayUs = 0x0,},    // DAC_LD_16_17
    { .regAddr = 0x3EDE, .regData = 0x11A1, .delayUs = 0x0,},    // DAC_LD_18_19
    { .regAddr = 0x3EE0, .regData = 0x118A, .delayUs = 0x0,},    // DAC_LD_20_21
    { .regAddr = 0x3EE2, .regData = 0x31C6, .delayUs = 0x0,},    // DAC_LD_22_23
    { .regAddr = 0x3EE4, .regData = 0x0D77, .delayUs = 0x0,},    // DAC_LD_24_25
    { .regAddr = 0x3EFE, .regData = 0x96F2, .delayUs = 0x0,},    // DAC_LD_50_51
    { .regAddr = 0x3F04, .regData = 0x0C18, .delayUs = 0x0,},    // DAC_LD_56_57
    { .regAddr = 0x3ED0, .regData = 0xC400, .delayUs = 0x0,},    // DAC_LD_4_5
    { .regAddr = 0x3ED4, .regData = 0x03CF, .delayUs = 0x0,},    // DAC_LD_8_9
    { .regAddr = 0x3ECE, .regData = 0x40F3, .delayUs = 0x0,},    // DAC_LD_2_3
    { .regAddr = 0x3F02, .regData = 0x2E1C, .delayUs = 0x0,},    // DAC_LD_54_55
    { .regAddr = 0x3EF4, .regData = 0x5805, .delayUs = 0x0,},    // DAC_LD_40_41
    { .regAddr = 0x3EF2, .regData = 0x9464, .delayUs = 0x0,},    // DAC_LD_38_39
    { .regAddr = 0x3EF2, .regData = 0x9464, .delayUs = 0x0,},    // DAC_LD_38_39
    { .regAddr = 0x3EF2, .regData = 0x9464, .delayUs = 0x0,},    // DAC_LD_38_39
    { .regAddr = 0x3EF4, .regData = 0x5805, .delayUs = 0x0,},    // DAC_LD_40_41
    { .regAddr = 0x3EF4, .regData = 0x5805, .delayUs = 0x0,},    // DAC_LD_40_41
    { .regAddr = 0x3EF4, .regData = 0x5805, .delayUs = 0x0,},    // DAC_LD_40_41
    { .regAddr = 0x3EF6, .regData = 0x5253, .delayUs = 0x0,},    // DAC_LD_42_43
    { .regAddr = 0x3EF6, .regData = 0x5253, .delayUs = 0x0,},    // DAC_LD_42_43
    { .regAddr = 0x3EF6, .regData = 0x5253, .delayUs = 0x0,},    // DAC_LD_42_43
    { .regAddr = 0x3EF8, .regData = 0x54F4, .delayUs = 0x0,},    // DAC_LD_44_45
    { .regAddr = 0x3EF8, .regData = 0x54F4, .delayUs = 0x0,},    // DAC_LD_44_45
    { .regAddr = 0x3EF8, .regData = 0x54F4, .delayUs = 0x0,},    // DAC_LD_44_45
    { .regAddr = 0x3F4C, .regData = 0x4B3C, .delayUs = 0x0,},    // PIX_DEF_1D_DDC_LO_DEF
    { .regAddr = 0x3F4C, .regData = 0x52BC, .delayUs = 0x0,},    // PIX_DEF_1D_DDC_LO_DEF
    { .regAddr = 0x3F4E, .regData = 0x573C, .delayUs = 0x0,},    // PIX_DEF_1D_DDC_HI_DEF
    { .regAddr = 0x3F4E, .regData = 0x3E3C, .delayUs = 0x0,},    // PIX_DEF_1D_DDC_HI_DEF
    { .regAddr = 0x3F50, .regData = 0x400B, .delayUs = 0x0,},    // PIX_DEF_1D_DDC_EDGE
    { .regAddr = 0x3F50, .regData = 0x380B, .delayUs = 0x0,},    // PIX_DEF_1D_DDC_EDGE
    { .regAddr = 0x31E0, .regData = 0x0003, .delayUs = 0x0,},    // PIX_DEF_ID
    { .regAddr = 0x3324, .regData = 0x001C, .delayUs = 0x0,},    // ATTENUATION_CTRL
    { .regAddr = 0x30B4, .regData = 0x01C1, .delayUs = 0x0,},    // TEMPSENS_CTRL_REG
    { .regAddr = 0x31AC, .regData = 0x0C0C, .delayUs = 0x0,},    // DATA_FORMAT_BITS   Noted by wsh: A=10-bit, C=12-bit.

     /* // Original setting from the vendor, but don't work on Arena.
    { .regAddr = 0x3030, .regData = 0x0080, .delayUs = 0x0,},    // PLL_MULTIPLIER
    { .regAddr = 0x302E, .regData = 0x0009, .delayUs = 0x0,},    // PRE_PLL_CLK_DIV
    { .regAddr = 0x302C, .regData = 0x0001, .delayUs = 0x0,},    // VT_SYS_CLK_DIV
    { .regAddr = 0x302A, .regData = 0x0006, .delayUs = 0x0,},    // VT_PIX_CLK_DIV
    { .regAddr = 0x3038, .regData = 0x0001, .delayUs = 0x0,},    // OP_SYS_CLK_DIV
    { .regAddr = 0x3036, .regData = 0x000C, .delayUs = 0x0,},    // OP_PIX_CLK_DIV
     */
     //--
    { .regAddr = 0x3030, .regData = 0x0056, .delayUs = 0x0,},    // PLL_MULTIPLIER
    { .regAddr = 0x302E, .regData = 0x0004, .delayUs = 0x0,},    // PRE_PLL_CLK_DIV
    { .regAddr = 0x302C, .regData = 0x0002, .delayUs = 0x0,},    // VT_SYS_CLK_DIV
    { .regAddr = 0x302A, .regData = 0x0006, .delayUs = 0x0,},    // VT_PIX_CLK_DIV
    { .regAddr = 0x3038, .regData = 0x0002, .delayUs = 0x0,},    // OP_SYS_CLK_DIV
    { .regAddr = 0x3036, .regData = 0x000c, .delayUs = 0x0,},    // OP_PIX_CLK_DIV
     //--

    { .regAddr = 0x3082, .regData = 0x0001, .delayUs = 0x0,},    // OPERATION_MODE_CTRL
    { .regAddr = 0x318E, .regData = 0x0000, .delayUs = 0x0,},    // HDR_MC_CTRL3
    { .regAddr = 0x31D0, .regData = 0x0000, .delayUs = 0x0,},    // COMPANDING
    { .regAddr = 0x3004, .regData = 0x0008, .delayUs = 0x0,},    // X_ADDR_START             Noted by wsh: Horizontal size. 0x787-0x02+1=0x780=1920.
    { .regAddr = 0x3008, .regData = 0x078f, .delayUs = 0x0,},    // X_ADDR_END
    { .regAddr = 0x3002, .regData = 0x0002, .delayUs = 0x0,},    // Y_ADDR_START             Noted by wsh: Vertical size. 0x439-0x02+1=0x438=1080.
    { .regAddr = 0x3006, .regData = 0x044f, .delayUs = 0x0,},    // Y_ADDR_END
    { .regAddr = 0x30a6, .regData = 0x0001, .delayUs = 0x0,},    // Y_ODD_INC
    { .regAddr = 0x300C, .regData = 0x045C, .delayUs = 0x0,},    // LINE_LENGTH_PCK          Noted by wsh: Pixels in a line, active + hblank.  1116
    { .regAddr = 0x300A, .regData = 0x09ec, .delayUs = 0x0,},    // FRAME_LENGTH_LINES        2540 = 15fps
    { .regAddr = 0x3012, .regData = 0x044B, .delayUs = 0x0,},    // COARSE_INTEGRATION_TIME
    { .regAddr = 0x301A, .regData = 0x2058, .delayUs = 0x0,},    // RESET_REGISTER
    { .regAddr = 0x31AE, .regData = 0x0204, .delayUs = 0x0,},    // SERIAL_FORMAT
    { .regAddr = 0x3354, .regData = 0x002C, .delayUs = 0x0,},    // MIPI_CNTRL               Noted by wsh: 2C=RAW12, 2B=RAW10.
    { .regAddr = 0x30B0, .regData = 0x022A, .delayUs = 0x0,},    // DIGITAL_TEST
    { .regAddr = 0x31B0, .regData = 0x0090, .delayUs = 0x0,},    // FRAME_PREAMBLE
    { .regAddr = 0x31B2, .regData = 0x0065, .delayUs = 0x0,},    // LINE_PREAMBLE
    { .regAddr = 0x31B4, .regData = 0x2A86, .delayUs = 0x0,},    // MIPI_TIMING_0
    { .regAddr = 0x31B6, .regData = 0x21D6, .delayUs = 0x0,},    // MIPI_TIMING_1
    { .regAddr = 0x31B8, .regData = 0x6049, .delayUs = 0x0,},    // MIPI_TIMING_2
    { .regAddr = 0x31BA, .regData = 0x0208, .delayUs = 0x0,},    // MIPI_TIMING_3
    { .regAddr = 0x31BC, .regData = 0x8007, .delayUs = 0x0,},    // MIPI_TIMING_4
    { .regAddr = 0x3064, .regData = 0x1802, .delayUs = 0x0,},    // SMIA_TEST
    { .regAddr = 0x301A, .regData = 0x205C, .delayUs = 0x0,},    // RESET_REGISTER
    { .regAddr = 0x3F50, .regData = 0x080B, .delayUs = 0x0,},    // PIX_DEF_1D_DDC_EDGE
    { .regAddr = 0x3040, .regData = 0xc000, .delayUs = 0x0,},    // PIX_DEF_1D_DDC_EDGE

};

static drv_sensor_reg_info_t  ar0239_streamOn_setting[] =
{

};

static drv_sensor_reg_info_t  ar0239_streamOff_setting[] =
{

};

static drv_sensor_exp_param_t ar0239_normal_exparams =
{
    .maxIntegrationTime = 2539  ,
    .minIntegrationTime = 1     ,
    .maxAnalogGain      = 33532,
    .minAnalogGain      = 1024  ,
    .lineTime           = 121360,
    .limitShortMid      = 0     ,
    .limitLong          = 0     ,
    .expLineMargin      = 1     ,
};


static drv_power_setting_t ar0239_power_up_setting[] = {
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 100000}, /// 100ms
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 100000}
};


static drv_power_setting_t ar0239_power_down_setting[] = {
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x00, .delayUs = 100000},
    {.type = DRV_POWER_REG, .regAddr = 0x83c40004, .bitMask  = 0x01, .bitVal = 0x01, .delayUs = 100000}
};

static sensor_drv_params_t ar0239_sensor_params =
{
    .sensorId     = 0x6613,
    .sensorIdAddr = 0x1A30,
    .name         = "ar0239",
    .slaveInfo =
    {
        .hwType      = DRV_HW_I2C,
        .slaveAddr   = (0x20 >> 1),
        .channel     = 0,
        .regDataType = DRV_ADDR_TYPE_WORD_REVERSE,
        .regAddrType = DRV_DATA_TYPE_WORD_REVERSE,
    },
    .powerUpSettings  =
    {
        .powerSetting = ar0239_power_up_setting,
        .size = sizeof(ar0239_power_up_setting) / sizeof(ar0239_power_up_setting[0]),
    },
    .powerDownSettings =
    {
        .powerSetting  = ar0239_power_down_setting,
        .size = sizeof(ar0239_power_down_setting) / sizeof(ar0239_power_down_setting[0]),
    },
    .activesize =
    {
        .width  = 1920,
        .height = 1080
    },
    .initSettings =
    {
        .regDataType = DRV_DATA_TYPE_BYTE,
        .regAddrType = DRV_ADDR_TYPE_WORD,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = NULL,
        .settingSize = 0,
    },
    .modeSettings[0] =
    {
        .maxFps = 15,
        .minFps = 15,
        .frameLength = 2540,
        .expLineMargin = 1,
        .dim =
        {
            .width  = 1920,
            .height = 1080
        },
        .hdrMode   = SENSOR_NORMAL_MODE,
        .regConfig =
        {
            .regDataType = DRV_DATA_TYPE_WORD_REVERSE,
            .regAddrType = DRV_ADDR_TYPE_WORD_REVERSE,
            .direction   = DRV_DIRECTION_WRITE,
            .settings    = ar0239_normal_setting,
            .settingSize = sizeof(ar0239_normal_setting) / sizeof (ar0239_normal_setting[0]),

        },
        .expParams   = &ar0239_normal_exparams,
    },

    .streamOnSetting =
    {
        .regDataType = DRV_DATA_TYPE_WORD_REVERSE,
        .regAddrType = DRV_ADDR_TYPE_WORD_REVERSE,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = ar0239_streamOn_setting,
        .settingSize = sizeof(ar0239_streamOn_setting) / sizeof (ar0239_streamOn_setting[0]),
    },

    .streamOffSetting =
    {
        .regDataType = DRV_DATA_TYPE_WORD_REVERSE,
        .regAddrType = DRV_ADDR_TYPE_WORD_REVERSE,
        .direction   = DRV_DIRECTION_WRITE,
        .settings    = ar0239_streamOff_setting,
        .settingSize = sizeof(ar0239_streamOff_setting) / sizeof (ar0239_streamOff_setting[0]),
    },

    .fill_exposure_reg = AR0239_Fill_Exposure_Reg,
};


sensor_drv_params_t* Load_AR0239_Sensor_Driver(void);

#endif
