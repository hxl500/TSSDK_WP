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
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "app_config.h"
#include "memory_map.h"

static u32                  g_cur_CamId;
static sanity_test_params_t g_stest_params[MAX_CAMERA];

///stest relate file name
static char8* const g_fileName[STFILE_NUM_MAX] =
{
    [STFILE_YUVMD5_RES]   = (char8* const)"/mnt/stest_res/md5_res.txt",
    [STFILE_APICHK_RES]   = (char8* const)"/mnt/stest_res/apic_res.txt",
    [STFILE_IMAGE_RAW16]  = (char8* const)"/mnt/VI_H.RAW",
    [STFILE_ISPREG_LIST]  = (char8* const)"/mnt/stest_res/IspRegList.Txt",
};

static char8* const gDirectionName[] = {
    [META_EVT_DIRT_WRITE]   = (char8* const)"write",
    [META_EVT_DIRT_READ]    = (char8* const)"read",
};

///define api descript string
static char8* const g_apiTypeLable[API_TYPE_TOTAL] =
{
    [API_TYPE_MOTOR]        = (char8* const)"ActrApi",
    [API_TYPE_SENSOR]       = (char8* const)"SensApi",
    [API_TYPE_ISP]          = (char8* const)"IspApi",
    [API_TYPE_SPECIAL]      = (char8* const)"SpeApi",
};

static char8* const g_md5ModName[STEST_ISP_MD5_NUM_MAX] =
{
    [STEST_ISP_MD5_R_L]    = (char8* const)"isp_md5_r_l: ",
    [STEST_ISP_MD5_R_H]    = (char8* const)"isp_md5_r_h: ",
    [STEST_ISP_MD5_G_L]    = (char8* const)"isp_md5_g_l: ",
    [STEST_ISP_MD5_G_H]    = (char8* const)"isp_md5_g_h: ",
    [STEST_ISP_MD5_B_L]    = (char8* const)"isp_md5_b_l: ",
    [STEST_ISP_MD5_B_H]    = (char8* const)"isp_md5_b_h: ",
};

static const char8*  g_EvtNameTable[API_TYPE_TOTAL][STEST_EVTNAME_ARRAY_NUM] = {
    {
    },
    {
    },
    {
        [2] = (char8* const)"GGC_ENABLE_0",    //type=2, id=2
        [3] = (char8* const)"COLOR_MATRIX_ENABLE_0",    //type=2, id=3
        [5] = (char8* const)"DPC_BYPASS_0",    //type=2, id=5
        [6] = (char8* const)"RNR_BYPASS_0",    //type=2, id=6
        [7] = (char8* const)"DMS_BYPASS_0",    //type=2, id=7
        [8] = (char8* const)"LSH_BYPASS_0",    //type=2, id=8
        [9] = (char8* const)"CNR_BYPASS_0",    //type=2, id=9
        [10] = (char8* const)"MVD_BYPASS_0",    //type=2, id=10
        [11] = (char8* const)"TNR_BYPASS_0",    //type=2, id=11
        [12] = (char8* const)"CAC_BYPASS_0",    //type=2, id=12
        [13] = (char8* const)"LTM_BYPASS_0",    //type=2, id=13
        [14] = (char8* const)"CMB_BYPASS_0",    //type=2, id=14
        [15] = (char8* const)"DPF_BYPASS_0",    //type=2, id=15
        [16] = (char8* const)"DEBUG_OUT_MODE_0",    //type=2, id=16
        [17] = (char8* const)"SKIP_FRAME_0",    //type=2, id=17
        [18] = (char8* const)"OUT_INDEX_0",    //type=2, id=18
        [19] = (char8* const)"DISP_TYPE_0",    //type=2, id=19
        [20] = (char8* const)"CMB_OUT_MODE_0",    //type=2, id=20
        [21] = (char8* const)"CMB_NP_MULT_LM_0",    //type=2, id=21
        [22] = (char8* const)"CMB_NP_MULT_MS_0",    //type=2, id=22
        [23] = (char8* const)"CMB_MOV_LM_MULT_0",    //type=2, id=23
        [24] = (char8* const)"CMB_MOV_MS_MULT_0",    //type=2, id=24
        [25] = (char8* const)"CMB_THRESH1_LM_0",    //type=2, id=25
        [26] = (char8* const)"CMB_THRESH2_LM_0",    //type=2, id=26
        [27] = (char8* const)"CMB_THRESH1_MS_0",    //type=2, id=27
        [28] = (char8* const)"CMB_THRESH2_MS_0",    //type=2, id=28
        [29] = (char8* const)"CMB_OUTPUT_ALPHA_LM_0",    //type=2, id=29
        [30] = (char8* const)"CMB_OUTPUT_ALPHA_MS_0",    //type=2, id=30
        [31] = (char8* const)"CMB_OUTPUT_LM_0",    //type=2, id=31
        [32] = (char8* const)"CMB_OUTPUT_MS_0",    //type=2, id=32
        [33] = (char8* const)"CMB_BL_LONG_R_0",    //type=2, id=33
        [34] = (char8* const)"CMB_BL_LONG_GR_0",    //type=2, id=34
        [35] = (char8* const)"CMB_BL_LONG_GB_0",    //type=2, id=35
        [36] = (char8* const)"CMB_BL_LONG_B_0",    //type=2, id=36
        [37] = (char8* const)"CMB_BL_MEDIUM_R_0",    //type=2, id=37
        [38] = (char8* const)"CMB_BL_MEDIUM_GR_0",    //type=2, id=38
        [39] = (char8* const)"CMB_BL_MEDIUM_GB_0",    //type=2, id=39
        [40] = (char8* const)"CMB_BL_MEDIUM_B_0",    //type=2, id=40
        [41] = (char8* const)"CMB_BL_SHORT_R_0",    //type=2, id=41
        [42] = (char8* const)"CMB_BL_SHORT_GR_0",    //type=2, id=42
        [43] = (char8* const)"CMB_BL_SHORT_GB_0",    //type=2, id=43
        [44] = (char8* const)"CMB_BL_SHORT_B_0",    //type=2, id=44
        [45] = (char8* const)"DPC_DEBUG_0",    //type=2, id=45
        [46] = (char8* const)"DPC_GE_EN_0",    //type=2, id=46
        [47] = (char8* const)"DPC_INTPL_ALPHA_0",    //type=2, id=47
        [48] = (char8* const)"DPC_DIV_SLP_0",    //type=2, id=48
        [49] = (char8* const)"DPC_DIV_THR_0",    //type=2, id=49
        [50] = (char8* const)"DPC_STDEV_MASK_THR_0",    //type=2, id=50
        [51] = (char8* const)"DPC_DP_DIS_THR_0",    //type=2, id=51
        [52] = (char8* const)"DPC_CUR_MED_THR_0",    //type=2, id=52
        [53] = (char8* const)"DPC_GE_STR_0",    //type=2, id=53
        [54] = (char8* const)"DPC_GE_THR_0",    //type=2, id=54
        [55] = (char8* const)"DPC_GE_SENS_0",    //type=2, id=55
        [56] = (char8* const)"DPC_GE_SLOPE_0",    //type=2, id=56
        [57] = (char8* const)"DPC_NP_GAIN_0",    //type=2, id=57
        [58] = (char8* const)"DPC_NP_OFFSET_0",    //type=2, id=58
        [59] = (char8* const)"RNR_BLT_LF_NP_GAIN_0",    //type=2, id=59
        [60] = (char8* const)"RNR_BLT_LF_NP_STR_0",    //type=2, id=60
        [64] = (char8* const)"BLACK_LEVEL_ENABLE_0",    //type=2, id=64
        [65] = (char8* const)"BLACK_LEVEL_R_0",    //type=2, id=65
        [66] = (char8* const)"BLACK_LEVEL_GR_0",    //type=2, id=66
        [67] = (char8* const)"BLACK_LEVEL_GB_0",    //type=2, id=67
        [68] = (char8* const)"BLACK_LEVEL_B_0",    //type=2, id=68
        [70] = (char8* const)"LSC_ENABLE_0",    //type=2, id=70
        [71] = (char8* const)"LSC_MESH_RADIAL_SWITCH_0",    //type=2, id=71
        [72] = (char8* const)"LSC_STRENGTH_0",    //type=2, id=72
        [73] = (char8* const)"LSC_MESH_FRAC_0",    //type=2, id=73
        [74] = (char8* const)"LSC_RADIAL_FRAC_0",    //type=2, id=74
        [75] = (char8* const)"WB_GAIN_ENABLE_0",    //type=2, id=75
        [76] = (char8* const)"CHANNEL_GAIN_R_0",    //type=2, id=76
        [77] = (char8* const)"CHANNEL_GAIN_G_0",    //type=2, id=77
        [78] = (char8* const)"CHANNEL_GAIN_B_0",    //type=2, id=78
        [83] = (char8* const)"LTM_SPATIAL_FILTER_TIME_0",    //type=2, id=83
        [85] = (char8* const)"LTM_CONVERGE_SPEED_0",    //type=2, id=85
        [87] = (char8* const)"DMS_HF_DETAIL_ENH_EN_0",    //type=2, id=87
        [88] = (char8* const)"DMS_HF_DETAIL_EHN_STR_0",    //type=2, id=88
        [89] = (char8* const)"DMS_SHARPEN_EN_0",    //type=2, id=89
        [90] = (char8* const)"DMS_SHARPEN_STR_0",    //type=2, id=90
        [91] = (char8* const)"DMS_NP_OFF_ABS_0",    //type=2, id=91
        [92] = (char8* const)"DMS_NP_OFF_0",    //type=2, id=92
        [93] = (char8* const)"DMS_GREY_SMALIAS_SLP_0",    //type=2, id=93
        [94] = (char8* const)"DMS_GREEN_USERB_THR_0",    //type=2, id=94
        [95] = (char8* const)"DMS_GREEN_USERB_SLP_0",    //type=2, id=95
        [96] = (char8* const)"DMS_DX_THR_0",    //type=2, id=96
        [97] = (char8* const)"DMS_DX_SLP_0",    //type=2, id=97
        [98] = (char8* const)"DMS_DP_THR_0",    //type=2, id=98
        [99] = (char8* const)"DMS_DP_SLP_0",    //type=2, id=99
        [101] = (char8* const)"DMS_DUSH_OFFSET_0",    //type=2, id=101
        [102] = (char8* const)"DMS_DUSH_THR_0",    //type=2, id=102
        [103] = (char8* const)"DMS_DUSH_SLP_0",    //type=2, id=103
        [104] = (char8* const)"DMS_DPX_THR_0",    //type=2, id=104
        [105] = (char8* const)"DMS_DPX_SLP_0",    //type=2, id=105
        [106] = (char8* const)"DMS_FCRATE_STR_0",    //type=2, id=106
        [107] = (char8* const)"DMS_FC_SLP_0",    //type=2, id=107
        [108] = (char8* const)"DMS_ALIAS_THR_0",    //type=2, id=108
        [109] = (char8* const)"DMS_ALIAS_SLP_0",    //type=2, id=109
        [110] = (char8* const)"DMS_BLDALIAS_THR_0",    //type=2, id=110
        [111] = (char8* const)"DMS_BLDALIAS_SLP_0",    //type=2, id=111
        [112] = (char8* const)"DMS_SAT_THR_0",    //type=2, id=112
        [113] = (char8* const)"DMS_SAT_SLP_0",    //type=2, id=113
        [114] = (char8* const)"DMS_BD_OPTION_0",    //type=2, id=114
        [115] = (char8* const)"DMS_DEBUG_MODE_0",    //type=2, id=115
        [116] = (char8* const)"COLOR_MATRIX_RR_0",    //type=2, id=116
        [117] = (char8* const)"COLOR_MATRIX_RG_0",    //type=2, id=117
        [118] = (char8* const)"COLOR_MATRIX_RB_0",    //type=2, id=118
        [119] = (char8* const)"COLOR_MATRIX_GR_0",    //type=2, id=119
        [120] = (char8* const)"COLOR_MATRIX_GG_0",    //type=2, id=120
        [121] = (char8* const)"COLOR_MATRIX_GB_0",    //type=2, id=121
        [122] = (char8* const)"COLOR_MATRIX_BR_0",    //type=2, id=122
        [123] = (char8* const)"COLOR_MATRIX_BG_0",    //type=2, id=123
        [124] = (char8* const)"COLOR_MATRIX_BB_0",    //type=2, id=124
        [126] = (char8* const)"DPF_SAD_SLP_0",    //type=2, id=126
        [127] = (char8* const)"DPF_SAD_THR_0",    //type=2, id=127
        [128] = (char8* const)"DPF_HUE1_SLOPE_0",    //type=2, id=128
        [129] = (char8* const)"DPF_HUE1_TH_0",    //type=2, id=129
        [130] = (char8* const)"DPF_HUE2_SLOPE_0",    //type=2, id=130
        [131] = (char8* const)"DPF_HUE2_TH_0",    //type=2, id=131
        [132] = (char8* const)"DPF_HUE_STR_0",    //type=2, id=132
        [133] = (char8* const)"DPF_SAT1_SLOPE_0",    //type=2, id=133
        [134] = (char8* const)"DPF_SAT1_TH_0",    //type=2, id=134
        [135] = (char8* const)"DPF_SAT_STR_0",    //type=2, id=135
        [136] = (char8* const)"DPF_LUMA11_SLOPE_0",    //type=2, id=136
        [137] = (char8* const)"DPF_LUMA11_TH_0",    //type=2, id=137
        [138] = (char8* const)"DPF_LUMA12_SLOPE_0",    //type=2, id=138
        [139] = (char8* const)"DPF_LUMA12_TH_0",    //type=2, id=139
        [140] = (char8* const)"DPF_LUMA21_SLOPE_0",    //type=2, id=140
        [141] = (char8* const)"DPF_LUMA21_TH_0",    //type=2, id=141
        [142] = (char8* const)"DPF_LUMA22_SLOPE_0",    //type=2, id=142
        [143] = (char8* const)"DPF_LUMA22_TH_0",    //type=2, id=143
        [144] = (char8* const)"DPF_LUMA_STR_0",    //type=2, id=144
        [145] = (char8* const)"DPF_HSL_SLOPE_0",    //type=2, id=145
        [146] = (char8* const)"DPF_HSL_TH_0",    //type=2, id=146
        [147] = (char8* const)"DPF_PURPLE_STR_0",    //type=2, id=147
        [148] = (char8* const)"DPF_SATUR_STR_0",    //type=2, id=148
        [149] = (char8* const)"DPF_DEBUG_MODE_0",    //type=2, id=149
        [150] = (char8* const)"MVD_DEBUG_0",    //type=2, id=150
        [151] = (char8* const)"MVD_NP_GAIN_0",    //type=2, id=151
        [152] = (char8* const)"MVD_NP_OFFSET_0",    //type=2, id=152
        [153] = (char8* const)"MVD_PIPEA_THR_0",    //type=2, id=153
        [156] = (char8* const)"MVD_PIPEB_ERODE1_0",    //type=2, id=156
        [157] = (char8* const)"MVD_PIPEB_DILATE_0",    //type=2, id=157
        [158] = (char8* const)"MVD_PIPEB_ERODE2_0",    //type=2, id=158
        [159] = (char8* const)"MVD_PIPEB_MAP_REFINE_THR_0",    //type=2, id=159
        [160] = (char8* const)"MVD_PIPEAB_TRI_DIR_STR_0",    //type=2, id=160
        [162] = (char8* const)"MVD_BLACK_LEVEL_R_0",    //type=2, id=162
        [163] = (char8* const)"MVD_BLACK_LEVEL_GR_0",    //type=2, id=163
        [164] = (char8* const)"MVD_BLACK_LEVEL_GB_0",    //type=2, id=164
        [165] = (char8* const)"MVD_BLACK_LEVEL_B_0",    //type=2, id=165
        [166] = (char8* const)"TNR_MOTION_2P_MODE_0",    //type=2, id=166
        [167] = (char8* const)"TNR_NR2D_PYRAMID_WT_0",    //type=2, id=167
        [168] = (char8* const)"TNR_GLOBAL_NOISE_THR_0",    //type=2, id=168
        [169] = (char8* const)"TNR_NR2D_GRAD_SLOPE_0",    //type=2, id=169
        [170] = (char8* const)"TNR_NR2D_YNOISE_THR_0",    //type=2, id=170
        [171] = (char8* const)"TNR_NR2D_CNOISE_THR_0",    //type=2, id=171
        [172] = (char8* const)"TNR_NR2D_NOISE_BACK_RATIO_0",    //type=2, id=172
        [173] = (char8* const)"TNR_MOTION_THR_0",    //type=2, id=173
        [174] = (char8* const)"TNR_MOTION_CURVE_OFFSET_0",    //type=2, id=174
        [175] = (char8* const)"TNR_MOTION_CURVE_MID_0",    //type=2, id=175
        [176] = (char8* const)"TNR_MOTION_CURVE_END_0",    //type=2, id=176
        [177] = (char8* const)"TNR_MOTION_CURVE_Y1_0",    //type=2, id=177
        [178] = (char8* const)"TNR_MOTION_CURVE_Y2_0",    //type=2, id=178
        [179] = (char8* const)"TNR_MOTION_CURVE_Y3_0",    //type=2, id=179
        [180] = (char8* const)"TNR_NR2D_CURVE_OFFSET_0",    //type=2, id=180
        [181] = (char8* const)"TNR_NR2D_CURVE_MID_0",    //type=2, id=181
        [182] = (char8* const)"TNR_NR2D_CURVE_END_0",    //type=2, id=182
        [183] = (char8* const)"TNR_NR2D_CURVE_Y1_0",    //type=2, id=183
        [184] = (char8* const)"TNR_NR2D_CURVE_Y2_0",    //type=2, id=184
        [185] = (char8* const)"TNR_NR2D_CURVE_Y3_0",    //type=2, id=185
        [186] = (char8* const)"TNR_NR2D_CURVE_C1_0",    //type=2, id=186
        [187] = (char8* const)"TNR_NR2D_CURVE_C2_0",    //type=2, id=187
        [188] = (char8* const)"TNR_NR2D_CURVE_C3_0",    //type=2, id=188
        [189] = (char8* const)"TNR_NR3D_CURVE_OFFSET_0",    //type=2, id=189
        [190] = (char8* const)"TNR_NR3D_CURVE_MID_0",    //type=2, id=190
        [191] = (char8* const)"TNR_NR3D_CURVE_END_0",    //type=2, id=191
        [192] = (char8* const)"TNR_NR3D_CURVE_Y1_0",    //type=2, id=192
        [193] = (char8* const)"TNR_NR3D_CURVE_Y2_0",    //type=2, id=193
        [194] = (char8* const)"TNR_NR3D_CURVE_Y3_0",    //type=2, id=194
        [195] = (char8* const)"TNR_NR3D_CURVE_C1_0",    //type=2, id=195
        [196] = (char8* const)"TNR_NR3D_CURVE_C2_0",    //type=2, id=196
        [197] = (char8* const)"TNR_NR3D_CURVE_C3_0",    //type=2, id=197
        [198] = (char8* const)"CNR_MOTION_NOISE_GAIN_0",    //type=2, id=198
        [199] = (char8* const)"CNR_LUMA_NOISE_OFFSET_0",    //type=2, id=199
        [200] = (char8* const)"CNR_LUMA_NOISE_MID_0",    //type=2, id=200
        [201] = (char8* const)"CNR_LUMA_NOISE_END_0",    //type=2, id=201
        [202] = (char8* const)"CNR_LUMA_NOISE_Y1_0",    //type=2, id=202
        [203] = (char8* const)"CNR_LUMA_NOISE_Y2_0",    //type=2, id=203
        [204] = (char8* const)"CNR_LUMA_NOISE_Y3_0",    //type=2, id=204
        [205] = (char8* const)"CNR_RFILT_SAT_OFFSET_0",    //type=2, id=205
        [206] = (char8* const)"CNR_RFILT_SAT_MID_0",    //type=2, id=206
        [207] = (char8* const)"CNR_RFILT_SAT_END_0",    //type=2, id=207
        [208] = (char8* const)"CNR_RFILT_SAT_Y1_0",    //type=2, id=208
        [209] = (char8* const)"CNR_RFILT_SAT_Y2_0",    //type=2, id=209
        [210] = (char8* const)"CNR_RFILT_SAT_Y3_0",    //type=2, id=210
        [211] = (char8* const)"CNR_RFILT_DIFF_OFFSET_0",    //type=2, id=211
        [212] = (char8* const)"CNR_RFILT_DIFF_MID_0",    //type=2, id=212
        [213] = (char8* const)"CNR_RFILT_DIFF_END_0",    //type=2, id=213
        [214] = (char8* const)"CNR_RFILT_DIFF_Y1_0",    //type=2, id=214
        [215] = (char8* const)"CNR_RFILT_DIFF_Y2_0",    //type=2, id=215
        [216] = (char8* const)"CNR_RFILT_DIFF_Y3_0",    //type=2, id=216
        [217] = (char8* const)"CNR_GAUSS_NOISE_OFFSET_0",    //type=2, id=217
        [218] = (char8* const)"CNR_GAUSS_NOISE_GAIN_0",    //type=2, id=218
        [219] = (char8* const)"CNR_UVDIFF_SFT_0",    //type=2, id=219
        [220] = (char8* const)"CNR_UV_BLEND_OFFSET_0",    //type=2, id=220
        [221] = (char8* const)"CNR_UV_BLEND_MID_0",    //type=2, id=221
        [222] = (char8* const)"CNR_UV_BLEND_END_0",    //type=2, id=222
        [223] = (char8* const)"CNR_UV_BLEND_Y1_0",    //type=2, id=223
        [224] = (char8* const)"CNR_UV_BLEND_Y2_0",    //type=2, id=224
        [225] = (char8* const)"CNR_UV_BLEND_Y3_0",    //type=2, id=225
        [226] = (char8* const)"CNR_FC_SUP_EN_0",    //type=2, id=226
        [227] = (char8* const)"CNR_FC_SUP_OFFSET_0",    //type=2, id=227
        [228] = (char8* const)"CNR_FC_SUP_MID_0",    //type=2, id=228
        [229] = (char8* const)"CNR_FC_SUP_END_0",    //type=2, id=229
        [230] = (char8* const)"CNR_FC_SUP_Y1_0",    //type=2, id=230
        [231] = (char8* const)"CNR_FC_SUP_Y2_0",    //type=2, id=231
        [232] = (char8* const)"CNR_FC_SUP_Y3_0",    //type=2, id=232
        [234] = (char8* const)"LSH_MOTION_R_0",    //type=2, id=234
        [235] = (char8* const)"LSH_LUM_OFFSET_0",    //type=2, id=235
        [236] = (char8* const)"LSH_LUM_MID_0",    //type=2, id=236
        [237] = (char8* const)"LSH_LUM_END_0",    //type=2, id=237
        [238] = (char8* const)"LSH_LUM_Y1_0",    //type=2, id=238
        [239] = (char8* const)"LSH_LUM_Y2_0",    //type=2, id=239
        [240] = (char8* const)"LSH_LUM_Y3_0",    //type=2, id=240
        [241] = (char8* const)"LSH_EDGE_NLM_SLP_0",    //type=2, id=241
        [242] = (char8* const)"LSH_EDGE_N2W_NORM_R_0",    //type=2, id=242
        [243] = (char8* const)"LSH_EDGE_THIN_OFFSET_0",    //type=2, id=243
        [244] = (char8* const)"LSH_EDGE_THIN_MID_0",    //type=2, id=244
        [245] = (char8* const)"LSH_EDGE_THIN_END_0",    //type=2, id=245
        [246] = (char8* const)"LSH_EDGE_THIN_Y1_0",    //type=2, id=246
        [247] = (char8* const)"LSH_EDGE_THIN_Y2_0",    //type=2, id=247
        [248] = (char8* const)"LSH_EDGE_THIN_Y3_0",    //type=2, id=248
        [249] = (char8* const)"LSH_NR_FREQ_OFFSET_0",    //type=2, id=249
        [250] = (char8* const)"LSH_NR_FREQ_MID_0",    //type=2, id=250
        [251] = (char8* const)"LSH_NR_FREQ_END_0",    //type=2, id=251
        [252] = (char8* const)"LSH_NR_FREQ_Y1_0",    //type=2, id=252
        [253] = (char8* const)"LSH_NR_FREQ_Y2_0",    //type=2, id=253
        [254] = (char8* const)"LSH_NR_FREQ_Y3_0",    //type=2, id=254
        [255] = (char8* const)"LSH_NEG_STR1_0",    //type=2, id=255
        [256] = (char8* const)"LSH_POS_STR1_0",    //type=2, id=256
        [257] = (char8* const)"LSH_NEG_MIN_OFFSET1_0",    //type=2, id=257
        [258] = (char8* const)"LSH_POS_MAX_OFFSET1_0",    //type=2, id=258
        [259] = (char8* const)"LSH_NEG_CLIP_RATIO1_0",    //type=2, id=259
        [260] = (char8* const)"LSH_POS_CLIP_RATIO1_0",    //type=2, id=260
        [261] = (char8* const)"LSH_DETAIL_NW_ALPHA_0",    //type=2, id=261
        [262] = (char8* const)"LSH_DETAIL_NR_R_0",    //type=2, id=262
        [263] = (char8* const)"LSH_DETAIL_TYPE_0",    //type=2, id=263
        [264] = (char8* const)"LSH_DETAIL_FREQ_OFFSET_0",    //type=2, id=264
        [265] = (char8* const)"LSH_DETAIL_FREQ_MID_0",    //type=2, id=265
        [266] = (char8* const)"LSH_DETAIL_FREQ_END_0",    //type=2, id=266
        [267] = (char8* const)"LSH_DETAIL_FREQ_Y1_0",    //type=2, id=267
        [268] = (char8* const)"LSH_DETAIL_FREQ_Y2_0",    //type=2, id=268
        [269] = (char8* const)"LSH_DETAIL_FREQ_Y3_0",    //type=2, id=269
        [270] = (char8* const)"LSH_DETAIL_LUM_OFFSET_0",    //type=2, id=270
        [271] = (char8* const)"LSH_DETAIL_LUM_MID_0",    //type=2, id=271
        [272] = (char8* const)"LSH_DETAIL_LUM_END_0",    //type=2, id=272
        [273] = (char8* const)"LSH_DETAIL_LUM_Y1_0",    //type=2, id=273
        [274] = (char8* const)"LSH_DETAIL_LUM_Y2_0",    //type=2, id=274
        [275] = (char8* const)"LSH_DETAIL_LUM_Y3_0",    //type=2, id=275
        [276] = (char8* const)"LSH_NEG_STR2_0",    //type=2, id=276
        [277] = (char8* const)"LSH_POS_STR2_0",    //type=2, id=277
        [278] = (char8* const)"LSH_NEG_MIN_OFFSET2_0",    //type=2, id=278
        [279] = (char8* const)"LSH_POS_MAX_OFFSET2_0",    //type=2, id=279
        [280] = (char8* const)"LSH_NEG_CLIP_RATIO2_0",    //type=2, id=280
        [281] = (char8* const)"LSH_POS_CLIP_RATIO2_0",    //type=2, id=281
        [282] = (char8* const)"LSH_EDGE_NLM_WEIGHT_TABLE_0",    //type=2, id=282
        [283] = (char8* const)"SDE_ENABLE_0",    //type=2, id=283
        [288] = (char8* const)"SDE_BRIGHTNESS_0",    //type=2, id=288
        [289] = (char8* const)"SDE_SATURATION_0",    //type=2, id=289
        [290] = (char8* const)"SDE_HUE_0",    //type=2, id=290
        [291] = (char8* const)"SDE_NEG_EN_0",    //type=2, id=291
        [292] = (char8* const)"SDE_CLIP_Y_LOW_0",    //type=2, id=292
        [293] = (char8* const)"SDE_CLIP_Y_HIGH_0",    //type=2, id=293
        [294] = (char8* const)"SDE_CONTRAST_ALL_0",    //type=2, id=294
        [295] = (char8* const)"SDE_CONTRAST_Y_0",    //type=2, id=295
        [296] = (char8* const)"SDE_CONTRAST_U_0",    //type=2, id=296
        [297] = (char8* const)"SDE_CONTRAST_V_0",    //type=2, id=297
        [326] = (char8* const)"OSD_MODE_0",    //type=2, id=326
        [327] = (char8* const)"TPG_MODE_0",    //type=2, id=327
        [328] = (char8* const)"TPG_SFT_SPEED_0",    //type=2, id=328
        [383] = (char8* const)"LSH_DLTI_EN_0",    //type=2, id=383
        [384] = (char8* const)"LSH_DLTI_POS_OFFSET_0",    //type=2, id=384
        [385] = (char8* const)"LSH_DLTI_POS_MID_0",    //type=2, id=385
        [386] = (char8* const)"LSH_DLTI_POS_END_0",    //type=2, id=386
        [387] = (char8* const)"LSH_DLTI_POS_Y1_0",    //type=2, id=387
        [388] = (char8* const)"LSH_DLTI_POS_Y2_0",    //type=2, id=388
        [389] = (char8* const)"LSH_DLTI_POS_Y3_0",    //type=2, id=389
        [390] = (char8* const)"LSH_DLTI_NEG_OFFSET_0",    //type=2, id=390
        [391] = (char8* const)"LSH_DLTI_NEG_MID_0",    //type=2, id=391
        [392] = (char8* const)"LSH_DLTI_NEG_END_0",    //type=2, id=392
        [393] = (char8* const)"LSH_DLTI_NEG_Y1_0",    //type=2, id=393
        [394] = (char8* const)"LSH_DLTI_NEG_Y2_0",    //type=2, id=394
        [395] = (char8* const)"LSH_DLTI_NEG_Y3_0",    //type=2, id=395
    },
    {
        [3] = (char8* const)"SENSOR_EXPO_TIME_L_LINE_0",    //type=3, id=3
        [4] = (char8* const)"SENSOR_EXPO_TIME_M_LINE_0",    //type=3, id=4
        [5] = (char8* const)"SENSOR_EXPO_TIME_S_LINE_0",    //type=3, id=5
        [6] = (char8* const)"SENSOR_EXPO_TIME_L_US_0",    //type=3, id=6
        [7] = (char8* const)"SENSOR_EXPO_TIME_M_US_0",    //type=3, id=7
        [8] = (char8* const)"SENSOR_EXPO_TIME_S_US_0",    //type=3, id=8
        [10] = (char8* const)"SENSOR_GAIN_0",    //type=3, id=10
        [11] = (char8* const)"SENSOR_MAX_FPS_0",    //type=3, id=11
        [12] = (char8* const)"SENSOR_MIN_FPS_0",    //type=3, id=12
        [14] = (char8* const)"ISP_GAIN_0",    //type=3, id=14
        [15] = (char8* const)"AE_DISABLE_0",    //type=3, id=15
        [16] = (char8* const)"AE_LOG_ENABLE_0",    //type=3, id=16
        [20] = (char8* const)"AE_CONVERGE_STEP_0",    //type=3, id=20
        [21] = (char8* const)"AE_MAX_CONVERGE_SPEED_0",    //type=3, id=21
        [22] = (char8* const)"AE_STABLE_IN_RANGE_0",    //type=3, id=22
        [23] = (char8* const)"AE_STABLE_OUT_RANGE_0",    //type=3, id=23
        [24] = (char8* const)"AE_SATURATION_THR_0",    //type=3, id=24
        [25] = (char8* const)"AE_STATS_Y_SELECT_IDX_0",    //type=3, id=25
        [26] = (char8* const)"AE_STABLE_TIMEOUT_FRAME_0",    //type=3, id=26
        [31] = (char8* const)"AE_MANUAL_SET_PARAM_ENABLE_0",    //type=3, id=31
        [32] = (char8* const)"AE_TARGET_0",    //type=3, id=32
        [33] = (char8* const)"AE_HIGH_CONTR_TARGET_0",    //type=3, id=33
        [34] = (char8* const)"AE_LOW_LEVEL_0",    //type=3, id=34
        [35] = (char8* const)"AE_HIGH_LEVEL_0",    //type=3, id=35
        [36] = (char8* const)"AE_LOW_CONTRAST_0",    //type=3, id=36
        [37] = (char8* const)"AE_HIGH_CONTRAST_0",    //type=3, id=37
        [40] = (char8* const)"AE_LONG_TARGET_0",    //type=3, id=40
        [41] = (char8* const)"AE_LONG_RATIO_MAX_0",    //type=3, id=41
        [42] = (char8* const)"AE_MID_TARGET_0",    //type=3, id=42
        [43] = (char8* const)"AE_MID_RATIO_MAX_0",    //type=3, id=43
        [44] = (char8* const)"AE_HDR_SAT_MAX_PCENT_0",    //type=3, id=44
        [47] = (char8* const)"AE_MANUAL_ROI_ENABLE_0",    //type=3, id=47
        [48] = (char8* const)"AE_MANUAL_ROI_X_0",    //type=3, id=48
        [49] = (char8* const)"AE_MANUAL_ROI_Y_0",    //type=3, id=49
        [50] = (char8* const)"AE_MANUAL_ROI_W_0",    //type=3, id=50
        [51] = (char8* const)"AE_MANUAL_ROI_H_0",    //type=3, id=51
        [52] = (char8* const)"AE_STATS_SEL_0",    //type=3, id=52
        [53] = (char8* const)"AE_WEIGHT_TBL_SEL_0",    //type=3, id=53
        [55] = (char8* const)"AE_HL_SUP_0",    //type=3, id=55
        [63] = (char8* const)"ANTI_FLICKER_ENABLE_0",    //type=3, id=63
        [64] = (char8* const)"ANTI_FLICKER_FREQUENCY_0",    //type=3, id=64
        [65] = (char8* const)"ANTI_FLICKER_FORCE_0",    //type=3, id=65
        [80] = (char8* const)"WB_GAIN_R_0",    //type=3, id=80
        [81] = (char8* const)"WB_GAIN_G_0",    //type=3, id=81
        [82] = (char8* const)"WB_GAIN_B_0",    //type=3, id=82
        [83] = (char8* const)"AWB_DISABLE_0",    //type=3, id=83
        [84] = (char8* const)"AWB_LOG_ENABLE_0",    //type=3, id=84
        [85] = (char8* const)"AWB_CONVERGE_STEP_0",    //type=3, id=85
        [86] = (char8* const)"AWB_STABLE_RANGE_0",    //type=3, id=86
        [87] = (char8* const)"AWB_MANUAL_ROI_ENABLE_0",    //type=3, id=87
        [88] = (char8* const)"AWB_MANUAL_ROI_X_0",    //type=3, id=88
        [89] = (char8* const)"AWB_MANUAL_ROI_Y_0",    //type=3, id=89
        [90] = (char8* const)"AWB_MANUAL_ROI_W_0",    //type=3, id=90
        [91] = (char8* const)"AWB_MANUAL_ROI_H_0",    //type=3, id=91
        [92] = (char8* const)"AWB_PREF_GAIN_R_0",    //type=3, id=92
        [93] = (char8* const)"AWB_PREF_GAIN_B_0",    //type=3, id=93
        [94] = (char8* const)"AWB_LOWLIGHT_LUX_THR_0",    //type=3, id=94
        [95] = (char8* const)"AWB_LOWCT_RGPOS_THR_0",    //type=3, id=95
        [97] = (char8* const)"CCM_SATURATION_0",    //type=3, id=97
        [98] = (char8* const)"CCM_MANUAL_INDEX_ENABLE_0",    //type=3, id=98
        [99] = (char8* const)"CCM_INDEX_0",    //type=3, id=99
        [100] = (char8* const)"CCM_INTER_PARAM_0",    //type=3, id=100
        [102] = (char8* const)"GGC_DYNAMIC_SW_ENABLE_0",    //type=3, id=102
        [103] = (char8* const)"GGC_LOWISO_THR_0",    //type=3, id=103
        [104] = (char8* const)"GGC_HIGHISO_THR_0",    //type=3, id=104
        [105] = (char8* const)"GGC_CONVERGE_SPEED_0",    //type=3, id=105
        [106] = (char8* const)"GGC_BASIC_SELECTION_0",    //type=3, id=106
        [107] = (char8* const)"GGC_CONTRAST_ENHANCE_ENABLE_0",    //type=3, id=107
        [108] = (char8* const)"GGC_CONTRAST_AUTO_MODE_0",    //type=3, id=108
        [109] = (char8* const)"GGC_CONTRAST_AUTO_STR_0",    //type=3, id=109
        [110] = (char8* const)"GGC_CONTRAST_MANUAL_STR_0",    //type=3, id=110
        [111] = (char8* const)"GGC_DEHAZE_STRENGTH_LIGHT_0",    //type=3, id=111
        [112] = (char8* const)"GGC_DEHAZE_STRENGTH_DARK_0",    //type=3, id=112
        [113] = (char8* const)"GGC_GLOBAL_LUMINATION_ADJ_0",    //type=3, id=113
        [114] = (char8* const)"GGC_LOCAL_LUM_ADJ_NODE_0_0",    //type=3, id=114
        [115] = (char8* const)"GGC_LOCAL_LUM_ADJ_NODE_1_0",    //type=3, id=115
        [116] = (char8* const)"GGC_LOCAL_LUM_ADJ_NODE_2_0",    //type=3, id=116
        [117] = (char8* const)"GGC_LOCAL_LUM_ADJ_NODE_3_0",    //type=3, id=117
        [118] = (char8* const)"GGC_LOCAL_LUM_ADJ_NODE_4_0",    //type=3, id=118
        [119] = (char8* const)"GGC_LOCAL_LUM_ADJ_NODE_5_0",    //type=3, id=119
        [120] = (char8* const)"GGC_LOCAL_LUM_ADJ_NODE_6_0",    //type=3, id=120
        [121] = (char8* const)"GGC_LOCAL_ADAPTIVE_NODE_0_0",    //type=3, id=121
        [122] = (char8* const)"GGC_LOCAL_ADAPTIVE_NODE_1_0",    //type=3, id=122
        [123] = (char8* const)"GGC_LOCAL_ADAPTIVE_NODE_2_0",    //type=3, id=123
        [124] = (char8* const)"GGC_LOCAL_ADAPTIVE_NODE_3_0",    //type=3, id=124
        [125] = (char8* const)"GGC_LOCAL_ADAPTIVE_NODE_4_0",    //type=3, id=125
        [126] = (char8* const)"GGC_LOCAL_ADAPTIVE_NODE_5_0",    //type=3, id=126
        [127] = (char8* const)"GGC_LOCAL_ADAPTIVE_NODE_6_0",    //type=3, id=127
        [128] = (char8* const)"GGC_CONTRAST_SMOOTH_RATE_0",    //type=3, id=128
        [164] = (char8* const)"LTM_GAIN_0",    //type=3, id=164
        [165] = (char8* const)"LTM_DARK_PRC_0",    //type=3, id=165
        [166] = (char8* const)"LTM_BRIGHT_PRC_0",    //type=3, id=166
        [167] = (char8* const)"LTM_MIN_LIGHTEN_0",    //type=3, id=167
        [168] = (char8* const)"LTM_MAX_LIGHTEN_0",    //type=3, id=168
        [169] = (char8* const)"LTM_CONTRAST_MIN_0",    //type=3, id=169
        [170] = (char8* const)"LTM_CONTRAST_MAX_0",    //type=3, id=170
        [171] = (char8* const)"LTM_GDG_ENABLE_0",    //type=3, id=171
        [172] = (char8* const)"LTM_TARGET_LDR_0",    //type=3, id=172
        [173] = (char8* const)"LTM_HI_TARGET_PRC_0",    //type=3, id=173
        [174] = (char8* const)"LTM_DARK_LEVEL_0",    //type=3, id=174
        [175] = (char8* const)"LTM_DARK_MIN_PRC_0",    //type=3, id=175
        [176] = (char8* const)"LTM_DARK_MAX_PRC_0",    //type=3, id=176
        [177] = (char8* const)"LTM_DARK_GAIN_RATIO_0",    //type=3, id=177
        [178] = (char8* const)"LTM_AMPLIFYALPHA_0",    //type=3, id=178
        [179] = (char8* const)"LTM_AMPLIFYALPHA_MAX_0",    //type=3, id=179
        [180] = (char8* const)"AWB_SDE_CT_COOL_0",    //type=3, id=180
        [181] = (char8* const)"AWB_SDE_CT_WARM_0",    //type=3, id=181
        [182] = (char8* const)"LSH_SDE_SHARPEN_0",    //type=3, id=182
        [184] = (char8* const)"MANUAL_MODE_ALL_MODULE_0",    //type=3, id=184
        [185] = (char8* const)"MANUAL_MODE_CMB_0",    //type=3, id=185
        [186] = (char8* const)"MANUAL_MODE_DPC_0",    //type=3, id=186
        [187] = (char8* const)"MANUAL_MODE_RNR_0",    //type=3, id=187
        [188] = (char8* const)"MANUAL_MODE_BLC_0",    //type=3, id=188
        [189] = (char8* const)"MANUAL_MODE_LSC_0",    //type=3, id=189
        [190] = (char8* const)"MANUAL_MODE_LTM_0",    //type=3, id=190
        [191] = (char8* const)"MANUAL_MODE_DMS_0",    //type=3, id=191
        [192] = (char8* const)"MANUAL_MODE_DPF_0",    //type=3, id=192
        [193] = (char8* const)"MANUAL_MODE_CCM_0",    //type=3, id=193
        [194] = (char8* const)"MANUAL_MODE_GGC_0",    //type=3, id=194
        [196] = (char8* const)"MANUAL_MODE_MVD_0",    //type=3, id=196
        [197] = (char8* const)"MANUAL_MODE_TNR_0",    //type=3, id=197
        [198] = (char8* const)"MANUAL_MODE_CNR_0",    //type=3, id=198
        [199] = (char8* const)"MANUAL_MODE_LSH_0",    //type=3, id=199
        [200] = (char8* const)"MANUAL_MODE_SDE_0",    //type=3, id=200
        [201] = (char8* const)"FACE_DETECT_IMG_W_0",    //type=3, id=201
        [202] = (char8* const)"FACE_DETECT_IMG_H_0",    //type=3, id=202
        [203] = (char8* const)"FACE_NUM_0",    //type=3, id=203
        [204] = (char8* const)"FACE_INDEX_0",    //type=3, id=204
        [205] = (char8* const)"FACE_CONFDENCE_0",    //type=3, id=205
        [206] = (char8* const)"FACE_ROT_ANGLE_0",    //type=3, id=206
        [207] = (char8* const)"FACE_X_0",    //type=3, id=207
        [208] = (char8* const)"FACE_Y_0",    //type=3, id=208
        [209] = (char8* const)"FACE_W_0",    //type=3, id=209
        [210] = (char8* const)"FACE_H_0",    //type=3, id=210
        [211] = (char8* const)"FACE_LEFT_EYE_L_X_0",    //type=3, id=211
        [212] = (char8* const)"FACE_LEFT_EYE_L_Y_0",    //type=3, id=212
        [213] = (char8* const)"FACE_LEFT_EYE_R_X_0",    //type=3, id=213
        [214] = (char8* const)"FACE_LEFT_EYE_R_Y_0",    //type=3, id=214
        [215] = (char8* const)"FACE_RIGHT_EYE_L_X_0",    //type=3, id=215
        [216] = (char8* const)"FACE_RIGHT_EYE_L_Y_0",    //type=3, id=216
        [217] = (char8* const)"FACE_RIGHT_EYE_R_X_0",    //type=3, id=217
        [218] = (char8* const)"FACE_RIGHT_EYE_R_Y_0",    //type=3, id=218
        [219] = (char8* const)"FACE_MOUSE_L_X_0",    //type=3, id=219
        [220] = (char8* const)"FACE_MOUSE_L_Y_0",    //type=3, id=220
        [221] = (char8* const)"FACE_MOUSE_R_X_0",    //type=3, id=221
        [222] = (char8* const)"FACE_MOUSE_R_Y_0",    //type=3, id=222
        [223] = (char8* const)"FACE_NOSE_X_0",    //type=3, id=223
        [224] = (char8* const)"FACE_NOSE_Y_0",    //type=3, id=224
        [225] = (char8* const)"FACE_AE_ENABLE_0",    //type=3, id=225
        [226] = (char8* const)"FACE_STATS_SEL_0",    //type=3, id=226
        [227] = (char8* const)"FACE_AE_CONVERGE_STEP_0",    //type=3, id=227
        [228] = (char8* const)"FACE_AE_STABLE_RANGE_0",    //type=3, id=228
        [229] = (char8* const)"FACE_RAW_TARGET_0",    //type=3, id=229
        [230] = (char8* const)"FACE_YUV_TARGET_0",    //type=3, id=230
        [231] = (char8* const)"FACE_SCALE_0",    //type=3, id=231
        [232] = (char8* const)"FACE_CROP_TOP_0",    //type=3, id=232
        [233] = (char8* const)"FACE_CROP_BOT_0",    //type=3, id=233
        [234] = (char8* const)"FACE_CROP_LEF_0",    //type=3, id=234
        [235] = (char8* const)"FACE_CROP_RIG_0",    //type=3, id=235
        [236] = (char8* const)"FACE_IN_CNT_THR_0",    //type=3, id=236
        [237] = (char8* const)"FACE_OUT_CNT_THR_0",    //type=3, id=237
        [238] = (char8* const)"FACE_SMALL_SIZE_THR_0",    //type=3, id=238
        [239] = (char8* const)"AWB_MANUAL_SET_PARAM_ENABLE_0",    //type=3, id=239
        [240] = (char8* const)"AWB_DETECT_ZONE_INDEX_0",    //type=3, id=240
        [241] = (char8* const)"AWB_DETECT_ZONE_ENABLE_0",    //type=3, id=241
        [242] = (char8* const)"AWB_DETECT_ZONE_LUX_START_0",    //type=3, id=242
        [243] = (char8* const)"AWB_DETECT_ZONE_LUX_END_0",    //type=3, id=243
        [244] = (char8* const)"AWB_DETECT_ZONE_LUX_BUF_0",    //type=3, id=244
        [245] = (char8* const)"AWB_DETECT_ZONE_LEFT_0",    //type=3, id=245
        [246] = (char8* const)"AWB_DETECT_ZONE_RIGHT_0",    //type=3, id=246
        [247] = (char8* const)"AWB_DETECT_ZONE_TOP_0",    //type=3, id=247
        [248] = (char8* const)"AWB_DETECT_ZONE_BOT_0",    //type=3, id=248
        [249] = (char8* const)"AWB_MLCZ_INDEX_0",    //type=3, id=249
        [250] = (char8* const)"AWB_MLCZ_ENABLE_0",    //type=3, id=250
        [251] = (char8* const)"AWB_MLCZ_DETECT_ZONE_IDX_0",    //type=3, id=251
        [252] = (char8* const)"AWB_MLCZ_LUX_START_0",    //type=3, id=252
        [253] = (char8* const)"AWB_MLCZ_LUX_END_0",    //type=3, id=253
        [254] = (char8* const)"AWB_MLCZ_LUX_BUF_0",    //type=3, id=254
        [255] = (char8* const)"AWB_MLCZ_HIT_PRC_THR_0",    //type=3, id=255
        [256] = (char8* const)"AWB_MLCZ_HIT_PRC_BUF_0",    //type=3, id=256
        [257] = (char8* const)"AWB_MLCZ_LEFT_0",    //type=3, id=257
        [258] = (char8* const)"AWB_MLCZ_RIGHT_0",    //type=3, id=258
        [259] = (char8* const)"AWB_MLCZ_TOP_0",    //type=3, id=259
        [260] = (char8* const)"AWB_MLCZ_BOT_0",    //type=3, id=260
        [261] = (char8* const)"AWB_PREF_COLOR_INDEX_0",    //type=3, id=261
        [262] = (char8* const)"AWB_PREF_COLOR_ENABLE_0",    //type=3, id=262
        [263] = (char8* const)"AWB_PREF_COLOR_REF_LIGHT_IDX_0",    //type=3, id=263
        [264] = (char8* const)"AWB_PREF_COLOR_LUX_START_0",    //type=3, id=264
        [265] = (char8* const)"AWB_PREF_COLOR_LUX_END_0",    //type=3, id=265
        [266] = (char8* const)"AWB_PREF_COLOR_LUX_BUF_0",    //type=3, id=266
        [267] = (char8* const)"AWB_PREF_COLOR_OFFSET_THR_X_0",    //type=3, id=267
        [268] = (char8* const)"AWB_PREF_COLOR_OFFSET_THR_Y_0",    //type=3, id=268
        [269] = (char8* const)"AWB_PREF_COLOR_CLIP_RATIO_X_0",    //type=3, id=269
        [270] = (char8* const)"AWB_PREF_COLOR_CLIP_RATIO_Y_0",    //type=3, id=270
        [271] = (char8* const)"AWB_PREF_COLOR_SHIFT_GAIN_R_0",    //type=3, id=271
        [272] = (char8* const)"AWB_PREF_COLOR_SHIFT_GAIN_B_0",    //type=3, id=272
        [273] = (char8* const)"AWB_LIMIT_Y_INDEX_0",    //type=3, id=273
        [274] = (char8* const)"AWB_LIMIT_Y_ENABLE_0",    //type=3, id=274
        [275] = (char8* const)"AWB_LIMIT_Y_REF_LIGHT_IDX_0",    //type=3, id=275
        [276] = (char8* const)"AWB_LIMIT_Y_LUX_START_0",    //type=3, id=276
        [277] = (char8* const)"AWB_LIMIT_Y_LUX_END_0",    //type=3, id=277
        [278] = (char8* const)"AWB_LIMIT_Y_LUX_BUF_0",    //type=3, id=278
        [279] = (char8* const)"AWB_LIMIT_Y_PRC_0",    //type=3, id=279
        [280] = (char8* const)"AWB_LIMIT_Y_WEIGHT_REDUCE_0",    //type=3, id=280
        [281] = (char8* const)"AWB_LIGHT_SOURCE_INDEX_0",    //type=3, id=281
        [282] = (char8* const)"AWB_LIGHT_SOURCE_ENABLE_0",    //type=3, id=282
        [283] = (char8* const)"AWB_LIGHT_SOURCE_REF_LIGHT_IDX_0",    //type=3, id=283
        [284] = (char8* const)"AWB_LIGHT_SOURCE_LUX_START_0",    //type=3, id=284
        [285] = (char8* const)"AWB_LIGHT_SOURCE_LUX_END_0",    //type=3, id=285
        [286] = (char8* const)"AWB_LIGHT_SOURCE_LUX_BUF_0",    //type=3, id=286
        [287] = (char8* const)"AWB_LIGHT_SOURCE_SCALE_LEFT_0",    //type=3, id=287
        [288] = (char8* const)"AWB_LIGHT_SOURCE_SCALE_RIGHT_0",    //type=3, id=288
        [289] = (char8* const)"AWB_LIGHT_SOURCE_SCALE_TOP_0",    //type=3, id=289
        [290] = (char8* const)"AWB_LIGHT_SOURCE_SCALE_BOT_0",    //type=3, id=290
        [291] = (char8* const)"AWB_LIGHT_SOURCE_PROB_H_0",    //type=3, id=291
        [292] = (char8* const)"AWB_LIGHT_SOURCE_PROB_A_0",    //type=3, id=292
        [293] = (char8* const)"AWB_LIGHT_SOURCE_PROB_TL84_0",    //type=3, id=293
        [294] = (char8* const)"AWB_LIGHT_SOURCE_PROB_CWF_0",    //type=3, id=294
        [295] = (char8* const)"AWB_LIGHT_SOURCE_PROB_D65_0",    //type=3, id=295
        [296] = (char8* const)"AWB_LIGHT_SOURCE_PROB_DF_0",    //type=3, id=296
        [297] = (char8* const)"AWB_LIGHT_SOURCE_PROB_SHADE_0",    //type=3, id=297
        [298] = (char8* const)"FACE_AWB_ENABLE_0",    //type=3, id=298
        [299] = (char8* const)"FACE_AWB_ENABLE_NR_0",    //type=3, id=299
        [300] = (char8* const)"FACE_AWB_WEIGHT_0",    //type=3, id=300
        [301] = (char8* const)"FACE_AWB_REF_TARGET_TOL_0",    //type=3, id=301
        [302] = (char8* const)"FACE_AWB_FACE_SCALE_0",    //type=3, id=302
        [303] = (char8* const)"FACE_AWB_FACE_SIZE_RATIO_L_0",    //type=3, id=303
        [304] = (char8* const)"FACE_AWB_FACE_SIZE_RATIO_H_0",    //type=3, id=304
        [305] = (char8* const)"FACE_AWB_DIST_THR_L_0",    //type=3, id=305
        [306] = (char8* const)"FACE_AWB_DIST_THR_H_0",    //type=3, id=306
        [307] = (char8* const)"FACE_AWB_PREF_GAIN_R_0",    //type=3, id=307
        [308] = (char8* const)"FACE_AWB_PREF_GAIN_B_0",    //type=3, id=308
        [309] = (char8* const)"AWB_EXTRA_COLOR_INDEX_0",    //type=3, id=309
        [310] = (char8* const)"AWB_EXTRA_COLOR_ENABLE_0",    //type=3, id=310
        [311] = (char8* const)"AWB_EXTRA_COLOR_LUX_START_0",    //type=3, id=311
        [312] = (char8* const)"AWB_EXTRA_COLOR_LUX_END_0",    //type=3, id=312
        [313] = (char8* const)"AWB_EXTRA_COLOR_LUX_BUF_0",    //type=3, id=313
        [314] = (char8* const)"AWB_EXTRA_COLOR_DETECT_PRC_THR_0",    //type=3, id=314
        [315] = (char8* const)"AWB_EXTRA_COLOR_DETECT_PRC_BUF_0",    //type=3, id=315
        [316] = (char8* const)"AWB_EXTRA_COLOR_LEFT_0",    //type=3, id=316
        [317] = (char8* const)"AWB_EXTRA_COLOR_RIGHT_0",    //type=3, id=317
        [318] = (char8* const)"AWB_EXTRA_COLOR_TOP_0",    //type=3, id=318
        [319] = (char8* const)"AWB_EXTRA_COLOR_BOT_0",    //type=3, id=319
        [320] = (char8* const)"AWB_EXTRA_COLOR_LIGHT_IDX_0",    //type=3, id=320
        [321] = (char8* const)"AWB_EXTRA_COLOR_WEIGHT_0",    //type=3, id=321
        [322] = (char8* const)"AWB_EXTRA_COLOR_PREF_GAIN_R_0",    //type=3, id=322
        [323] = (char8* const)"AWB_EXTRA_COLOR_PREF_GAIN_B_0",    //type=3, id=323
        [324] = (char8* const)"AWB_SPECIAL_GAIN_H_R_0",    //type=3, id=324
        [325] = (char8* const)"AWB_SPECIAL_GAIN_H_B_0",    //type=3, id=325
        [326] = (char8* const)"AWB_SPECIAL_GAIN_A_R_0",    //type=3, id=326
        [327] = (char8* const)"AWB_SPECIAL_GAIN_A_B_0",    //type=3, id=327
        [328] = (char8* const)"AWB_SPECIAL_GAIN_TL84_R_0",    //type=3, id=328
        [329] = (char8* const)"AWB_SPECIAL_GAIN_TL84_B_0",    //type=3, id=329
        [330] = (char8* const)"AWB_SPECIAL_GAIN_CWF_R_0",    //type=3, id=330
        [331] = (char8* const)"AWB_SPECIAL_GAIN_CWF_B_0",    //type=3, id=331
        [332] = (char8* const)"AWB_SPECIAL_GAIN_D65_R_0",    //type=3, id=332
        [333] = (char8* const)"AWB_SPECIAL_GAIN_D65_B_0",    //type=3, id=333
        [334] = (char8* const)"AWB_SPECIAL_GAIN_DF_R_0",    //type=3, id=334
        [335] = (char8* const)"AWB_SPECIAL_GAIN_DF_B_0",    //type=3, id=335
        [336] = (char8* const)"AWB_SPECIAL_GAIN_SHADE_R_0",    //type=3, id=336
        [337] = (char8* const)"AWB_SPECIAL_GAIN_SHADE_B_0",    //type=3, id=337
        [338] = (char8* const)"AWB_SPECIAL_GAIN_WEIGHT_H_0",    //type=3, id=338
        [339] = (char8* const)"AWB_SPECIAL_GAIN_WEIGHT_A_0",    //type=3, id=339
        [340] = (char8* const)"AWB_SPECIAL_GAIN_WEIGHT_TL84_0",    //type=3, id=340
        [341] = (char8* const)"AWB_SPECIAL_GAIN_WEIGHT_CWF_0",    //type=3, id=341
        [342] = (char8* const)"AWB_SPECIAL_GAIN_WEIGHT_D65_0",    //type=3, id=342
        [343] = (char8* const)"AWB_SPECIAL_GAIN_WEIGHT_DF_0",    //type=3, id=343
        [344] = (char8* const)"AWB_SPECIAL_GAIN_WEIGHT_SHADE_0",    //type=3, id=344
        [354] = (char8* const)"DOL2_NP_LGAIN_0",    //type=3, id=354
        [355] = (char8* const)"DOL2_NP_MGAIN_0",    //type=3, id=355
        [356] = (char8* const)"DOL2_NP_TRANS_GAIN_0",    //type=3, id=356
        [357] = (char8* const)"DOL2_NP_FIT_ORDER_0",    //type=3, id=357
    },
};

#if 0
///define all isp mod name
static char8* const g_ispModName[ISP_MODS_MAX] =
{
    [ISP_MODS_AAC   ]  =(char8* const)"AAC",
    [ISP_MODS_AAY   ]  =(char8* const)"AAY",
    [ISP_MODS_AEP   ]  =(char8* const)"AEP",
    [ISP_MODS_AEC   ]  =(char8* const)"AEC",
    [ISP_MODS_AWB   ]  =(char8* const)"AWB",
    [ISP_MODS_AF    ]  =(char8* const)"AF",
    [ISP_MODS_BLC   ]  =(char8* const)"BLC",
    [ISP_MODS_CAC   ]  =(char8* const)"CAC",
    [ISP_MODS_CCM   ]  =(char8* const)"CCM",
    [ISP_MODS_CMB   ]  =(char8* const)"CMB",
    [ISP_MODS_CNR   ]  =(char8* const)"CNR"
    [ISP_MODS_CSC   ]  =(char8* const)"CSC",
    [ISP_MODS_CSC2  ]  =(char8* const)"CSC2",
    [ISP_MODS_DCP   ]  =(char8* const)"DCP",
    [ISP_MODS_DFG   ]  =(char8* const)"DFG",
    [ISP_MODS_DGN   ]  =(char8* const)"DGN",
    [ISP_MODS_DMS   ]  =(char8* const)"DMS",
    [ISP_MODS_DPC   ]  =(char8* const)"DPC",
    [ISP_MODS_DPF   ]  =(char8* const)"DPF",
    [ISP_MODS_GDC   ]  =(char8* const)"GDC",
    [ISP_MODS_GGC   ]  =(char8* const)"GGC"
    [ISP_MODS_IFBC  ]  =(char8* const)"IFBC",
    [ISP_MODS_IFBD  ]  =(char8* const)"IFBD",
    [ISP_MODS_L2Q   ]  =(char8* const)"L2Q",
    [ISP_MODS_LNC   ]  =(char8* const)"LNC",
    [ISP_MODS_LSC   ]  =(char8* const)"LSC",
    [ISP_MODS_LSH   ]  =(char8* const)"LSH",
    [ISP_MODS_LTM   ]  =(char8* const)"LTM",
    [ISP_MODS_LTM_SW]  =(char8* const)"LTM_SW",
    [ISP_MODS_MVD   ]  =(char8* const)"MVD",
    [ISP_MODS_NP    ]  =(char8* const)"NP "
    [ISP_MODS_NRP   ]  =(char8* const)"NRP",
    [ISP_MODS_PSC   ]  =(char8* const)"PSC",
    [ISP_MODS_PTM   ]  =(char8* const)"PTM",
    [ISP_MODS_Q2L   ]  =(char8* const)"Q2L",
    [ISP_MODS_R2B   ]  =(char8* const)"R2B",
    [ISP_MODS_RIR   ]  =(char8* const)"RIR",
    [ISP_MODS_RNR   ]  =(char8* const)"RNR",
    [ISP_MODS_RTNR  ]  =(char8* const)"RTNR",
    [ISP_MODS_SCA   ]  =(char8* const)"SCA",
    [ISP_MODS_SCB   ]  =(char8* const)"SCB"
    [ISP_MODS_SCD   ]  =(char8* const)"SCD",
    [ISP_MODS_SCE   ]  =(char8* const)"SCE",
    [ISP_MODS_SDE   ]  =(char8* const)"SDE",
    [ISP_MODS_TPG   ]  =(char8* const)"TPG",
    [ISP_MODS_TOP   ]  =(char8* const)"TOP",
    [ISP_MODS_VIN   ]  =(char8* const)"VIN",
    [ISP_MODS_VOUT0 ]  =(char8* const)"VOUT0",
    [ISP_MODS_VOUT1 ]  =(char8* const)"VOUT1",
    [ISP_MODS_WBG   ]  =(char8* const)"WBG",
    [ISP_MODS_YNR   ]  =(char8* const)"YNR"
    [ISP_MODS_YTNR  ]  =(char8* const)"YTNR",
    [ISP_MODS_VOUT2 ]  =(char8* const)"VOUT2",
    [ISP_MODS_VOUT3 ]  =(char8* const)"VOUT3",
};
#endif

/// define stest param token list
char8* const g_paramStestToken[] =
{
    [PARAM_STEST_CAMID]    = (char8* const)"camid",
    [PARAM_STEST_CASE]     = (char8* const)"caseid",
    [PARAM_STEST_MAX]      = NULL,
};

static isp_ret_t Stest_API_Cmp_EvtData(FileHandle*     pFILE,
                                    stest_event_data*  pSetData,
                                    stest_event_data*  pGetData)
{
    u32           apiType[2] = {0};
    u32           evtId[2]   = {0};
    isp_ret_t     res     = ISP_RET_SUCCESS;

    if (!pFILE || !pSetData || !pGetData ||
        (!pSetData->sDataInit && !pGetData->sDataInit))
    {
        res = ISP_RET_INVALID;
    }
    else
    {
        apiType[0] = pSetData->sEvtNewId >> 16;
        evtId[0]   = pSetData->sEvtNewId & 0xFFFF;
        apiType[1] = pGetData->sEvtNewId >> 16;
        evtId[1]   = pGetData->sEvtNewId & 0xFFFF;

        if (apiType[0] >= API_TYPE_TOTAL ||
            apiType[1] >= API_TYPE_TOTAL ||
            evtId[0] >= STEST_EVTNAME_ARRAY_NUM ||
            evtId[1] >= STEST_EVTNAME_ARRAY_NUM)
        {
            res = ISP_RET_INVALID;
            STEST_ERR("Invalid Write/Read apitype/evtId");
            fprintf(pFILE, "Invalid API TYPE S(%d %d) != G(%d %d) \n",
                        apiType[0], evtId[0], apiType[1], evtId[1]);
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        if (pSetData->sDataInit == TRUE &&
            pGetData->sDataInit == TRUE)
        {
            if (pSetData->sEvtNewId != pGetData->sEvtNewId)
            {
                res = ISP_RET_ERROR;
                fprintf(pFILE, "MisMatch [(%s %d(%s) %d] != G[%s %d(%s) %d] \n",
                            g_apiTypeLable[apiType[0]],
                            evtId[0],
                            g_EvtNameTable[apiType[0]][evtId[0]],
                            pSetData->sEvtVal,
                            g_apiTypeLable[apiType[1]],
                            evtId[1],
                            g_EvtNameTable[apiType[1]][evtId[1]],
                            pGetData->sEvtVal);
            }
        }
        else
        {
            res = ISP_RET_ERROR;
            if (pSetData->sDataInit)
            {
                fprintf(pFILE, "NA Read S[%s %d(%s) %d] != G[%s %d(%s) %d] \n",
                            g_apiTypeLable[apiType[0]],
                            evtId[0],
                            g_EvtNameTable[apiType[0]][evtId[0]],
                            pSetData->sEvtVal,
                            g_apiTypeLable[apiType[1]],
                            evtId[1],
                            g_EvtNameTable[apiType[1]][evtId[1]],
                            pGetData->sEvtVal);
            }
            else
            {
                fprintf(pFILE, "NA Write S[%s %d(%s) %d] != G[%s %d(%s) %d] \n",
                            g_apiTypeLable[apiType[0]],
                            evtId[0],
                            g_EvtNameTable[apiType[0]][evtId[0]],
                            pSetData->sEvtVal,
                            g_apiTypeLable[apiType[1]],
                            evtId[1],
                            g_EvtNameTable[apiType[1]][evtId[1]],
                            pGetData->sEvtVal);
            }
        }
    }

    if (ISP_RET_SUCCESS == res)
    {
        if (pSetData->sEvtVal == pGetData->sEvtVal)
        {
            fprintf(pFILE, "PASS S=G(%s %d(%s) %d)\n",
                        g_apiTypeLable[apiType[0]],
                        evtId[0],
                        g_EvtNameTable[apiType[0]][evtId[0]],
                        pSetData->sEvtVal);
        }
        else
        {
            fprintf(pFILE, "NEQ DATA %s %d(%s) S[%d] != G[%d]\n",
                        g_apiTypeLable[apiType[0]],
                        evtId[0],
                        g_EvtNameTable[apiType[0]][evtId[0]],
                        pSetData->sEvtVal,
                        pGetData->sEvtVal);
        }
    }

    return res;
}

static isp_ret_t Stest_APICRes_Save(u32          camId,
                                    FileHandle*  pFILE)
{
    sanity_test_params_t*  pStestParams = NULL;
    u32                    i = 0, j     = 0;
    u32                    apiSNum[API_TYPE_TOTAL + 1] = {0};
    u32                    apiGNum[API_TYPE_TOTAL + 1] = {0};
    isp_ret_t              res          = ISP_RET_SUCCESS;

    STEST_FUNC_ENTER();
    if (!pFILE || camId >= MAX_CAMERA)
    {
        STEST_ERR("Error! API FILE HANDLE is NULL\n");
        res = ISP_RET_INVALID;
    }

    if (ISP_RET_SUCCESS == res)
    {
        stest_event_params*      pEvtSetParams   = NULL;
        stest_event_params*      pEvtGetParams   = NULL;
        stest_event_data*        pEvtSetData     = NULL;
        stest_event_data*        pEvtGetData     = NULL;
        pStestParams = &g_stest_params[camId];

        for (i = 0; i < API_TYPE_TOTAL; i ++)
        {
            pEvtSetParams = &pStestParams->sEvtPrms[i][META_EVT_DIRT_WRITE];
            apiSNum[i] = pEvtSetParams->sEvtCnt;
            pEvtGetParams = &pStestParams->sEvtPrms[i][META_EVT_DIRT_READ];
            apiGNum[i] += pEvtGetParams->sEvtCnt;

            apiSNum[API_TYPE_TOTAL] += apiSNum[i];
            apiGNum[API_TYPE_TOTAL] += apiGNum[i];
        }

        fprintf(pFILE, "STEST Set API Total Num(%d) =",
                                apiSNum[API_TYPE_TOTAL]);
        fprintf(pFILE, "%s(%d) + %s(%d) + %s(%d) + %s(%d) \n",
                        g_apiTypeLable[API_TYPE_MOTOR],
                        apiSNum[API_TYPE_MOTOR],
                        g_apiTypeLable[API_TYPE_SENSOR],
                        apiSNum[API_TYPE_SENSOR],
                        g_apiTypeLable[API_TYPE_ISP],
                        apiSNum[API_TYPE_ISP],
                        g_apiTypeLable[API_TYPE_SPECIAL],
                        apiSNum[API_TYPE_SPECIAL]);

        fprintf(pFILE, "STEST Get API Total Num(%d) =",
                                apiSNum[API_TYPE_TOTAL]);
        fprintf(pFILE, "%s(%d) + %s(%d) + %s(%d) + %s(%d) \n",
                        g_apiTypeLable[API_TYPE_MOTOR],
                        apiGNum[API_TYPE_MOTOR],
                        g_apiTypeLable[API_TYPE_SENSOR],
                        apiGNum[API_TYPE_SENSOR],
                        g_apiTypeLable[API_TYPE_ISP],
                        apiGNum[API_TYPE_ISP],
                        g_apiTypeLable[API_TYPE_SPECIAL],
                        apiGNum[API_TYPE_SPECIAL]);

        for (i = 0; i < API_TYPE_TOTAL; i ++)
        {
            pEvtSetParams = &pStestParams->sEvtPrms[i][META_EVT_DIRT_WRITE];
            pEvtGetParams = &pStestParams->sEvtPrms[i][META_EVT_DIRT_READ];
            for (j = 0; j < STEST_EVTNAME_ARRAY_NUM; j ++)
            {
                pEvtSetData = &pEvtSetParams->sEvtPrms[j];
                pEvtGetData = &pEvtGetParams->sEvtPrms[j];
                Stest_API_Cmp_EvtData(pFILE,
                                      pEvtSetData,
                                      pEvtGetData);
            }
        }
    }
    STEST_FUNC_EXIT();

    return res;
}

///read back ddr process md5 register value and write to md5 result file
static isp_ret_t Stest_Ddrp_MD5_Save(u32         camId,
                                    FileHandle*  pFILE)
{
    isp_ret_t                res           = ISP_RET_SUCCESS;
    STEST_FUNC_ENTER();

    if (!pFILE || camId >= MAX_CAMERA)
    {
        STEST_ERR("Error! API FILE HANDLE is NULL\n");
        res = ISP_RET_INVALID;
    }

    ///write MD5 result to md5_Res.txt
    if (ISP_RET_SUCCESS == res)
    {
        STEST_INFO("YUV MD5 VALUE RECORD");
        fprintf(pFILE, "*************************************\n");
        fprintf(pFILE, "****** camId(%d) YUV MD5 VALUE ******\n", camId);
        fprintf(pFILE, "*************************************\n");
        u32 regaddr = 0, regval  = 0;

        for (u32 i = 0; i < STEST_ISP_MD5_NUM_MAX; i ++)
        {
            regaddr = TOP_MD5_REG_OFFSET + i * 4;
            regval = ISP_Read_Register(regaddr);
            fprintf(pFILE, "<%s> regaddr(0x%x), regval(0x%x)\n",
                g_md5ModName[i], regaddr, regval);
        }
        fprintf(pFILE, "\n******************END****************\n");
    }
    STEST_FUNC_EXIT();

    return res;
}

isp_ret_t Stest_Res_File_Save(u32                camId,
                              stest_file_type_t  resType)
{
    FileHandle*           pFile      = NULL;
    isp_ret_t             res        = ISP_RET_SUCCESS;
    STEST_FUNC_ENTER();

    if (camId >= MAX_CAMERA
        || resType >= STFILE_NUM_MAX)
    {
        STEST_ERR("Error! camId(%d) resType(%d)\n", camId, resType);
        res = ISP_RET_INVALID;
    }

    ///Open res file
    if (ISP_RET_SUCCESS == res)
    {
        pFile = FS_OS_Open(g_fileName[resType], "a+");
        if (!pFile)
        {
            STEST_ERR("camid(%d) file(%s) open error\n",
                            camId, g_fileName[resType]);
            res = ISP_RET_ERROR;
        }
    }

    ///write res data to res file
    if (ISP_RET_SUCCESS == res)
    {
        if (STFILE_APICHK_RES == resType)
        {
            res = Stest_APICRes_Save(camId, pFile);
        }
        else if (STFILE_YUVMD5_RES == resType)
        {
            res = Stest_Ddrp_MD5_Save(camId, pFile);
        }
    }

    /// close res file
    if (pFile)
    {
        FS_OS_Close(pFile);
    }
    STEST_FUNC_EXIT();

    return res;
}


isp_ret_t Stest_RegList_Start_Record(u32            camId)
{
    sanity_test_params_t*  pSanityParams = NULL;
    isp_ret_t              res           = ISP_RET_SUCCESS;
    STEST_FUNC_ENTER();

    if (camId >= MAX_CAMERA)
    {
        STEST_ERR("Error! camId(%d)\n", camId);
        res = ISP_RET_INVALID;
    }
    else
    {
        pSanityParams = &g_stest_params[camId];
        if (pSanityParams->pFileHdl)
        {
            STEST_INFO("[%s] already open\n",
                        g_fileName[STFILE_ISPREG_LIST]);
            res = ISP_RET_INGORED_FAILURE;
        }
    }

    ///Open res file
    if (ISP_RET_SUCCESS == res)
    {
        pSanityParams->pFileHdl = FS_OS_Open(g_fileName[STFILE_ISPREG_LIST], "a+");
        if (!pSanityParams->pFileHdl)
        {
            STEST_ERR("camid(%d) file(%s) open failed\n",
                            camId, g_fileName[STFILE_ISPREG_LIST]);
            res = ISP_RET_ERROR;
        }
        else
        {
            STEST_INFO("Start Record DDR Process");
            fprintf(pSanityParams->pFileHdl, "********************************************************\n");
            fprintf(pSanityParams->pFileHdl, "****** Start Record camId(%d) DDR Process Reglist ******\n", camId);
            fprintf(pSanityParams->pFileHdl, "********************************************************\n");
        }
    }
    STEST_FUNC_EXIT();

    return res;
}

isp_ret_t Stest_RegList_Stop_Record(u32            camId)
{
    sanity_test_params_t*  pSanityParams = NULL;
    isp_ret_t              res           = ISP_RET_SUCCESS;

    STEST_FUNC_ENTER();

    if (camId >= MAX_CAMERA)
    {
        STEST_ERR("Error! camId(%d)\n", camId);
        res = ISP_RET_INVALID;
    }

    ///Open res file
    if (ISP_RET_SUCCESS == res)
    {
        pSanityParams = &g_stest_params[camId];
        if (pSanityParams->pFileHdl)
        {
            STEST_INFO("Close reglist file handle");
            fprintf(pSanityParams->pFileHdl, "**************************End***************************\n");
            fprintf(pSanityParams->pFileHdl, "********************************************************\n");
            FS_OS_Close(pSanityParams->pFileHdl);
            pSanityParams->pFileHdl = NULL;
        }
        else
        {
            STEST_ERR("camId(%d) reglist file handle already closed\n", camId);
            res = ISP_RET_ERROR;
        }
    }
    STEST_FUNC_EXIT();

    return res;
}


///define read sdcard raw data to CPU Mem
static isp_ret_t Stest_Read_Raw_Data(u32  camId)
{
    stest_ddrp_params*       pDDRParams   = NULL;
    struct mem_block*        pMem_block   = NULL;
    FileHandle*              pFILE        = NULL;
    size_t                   rawSize      = 0;
    isp_ret_t                res          = ISP_RET_SUCCESS;
    STEST_FUNC_ENTER();

    if (camId >= MAX_CAMERA)
    {
        STEST_ERR("Invalid camId(%d)\n", camId);
        res = ISP_RET_INVALID;
    }
    else
    {
        pDDRParams = &g_stest_params[camId].sDDRParams;
    }

    if(pDDRParams && pDDRParams->sDDRPInit)
    {
        STEST_INFO("DDRP Already Init\n");
        res = ISP_RET_INGORED_FAILURE;
    }

    ///open raw file
    if (ISP_RET_SUCCESS == res)
    {
        memset(pDDRParams, 0, sizeof(stest_ddrp_params));
        pFILE = FS_OS_Open(g_fileName[STFILE_IMAGE_RAW16], "rb");
        if (!pFILE)
        {
            STEST_ERR("file[%s] open failed\n", g_fileName[STFILE_IMAGE_RAW16]);
            res = ISP_RET_ERROR;
        }
    }

    ///alloc cma raw buffer
    if (ISP_RET_SUCCESS == res)
    {
        pMem_block = &pDDRParams->sMemBlock;
        memset(pMem_block, 0, sizeof(struct mem_block));
        strcpy((char *)pMem_block->name, "ddr_raw");
        pMem_block->len = RAW16_1080P_DSIZE;
        pMem_block->isUseBuffer = 0;
        res = Isp_Cma_Alloc(pMem_block);
        if (ISP_RET_SUCCESS == res)
        {
            pthread_mutex_init(&pDDRParams->sCma_mutex, NULL);
        }
        else
        {
            STEST_ERR("raw cma alloc buffer failed res(%d)\n", res);
        }
    }

    ///copy raw data to cma buffer
    if (ISP_RET_SUCCESS == res)
    {
        rawSize = FS_OS_Read(pFILE,
                             (void *)pMem_block->memAddr,
                             RAW16_1080P_DSIZE);
        if (RAW16_1080P_DSIZE != rawSize)
        {
            res = ISP_RET_ERROR;
            STEST_ERR("file(%s) size(0x%x) != 1080p raw16 size(0x%x)\n",
                        g_fileName[STFILE_IMAGE_RAW16],
                        rawSize,
                        RAW16_1080P_DSIZE);
            Isp_Cma_Free(pMem_block);
            pthread_mutex_destroy(&pDDRParams->sCma_mutex);
        }
    }

    ///close raw file
    if (pFILE)
    {
        FS_OS_Close(pFILE);
        pFILE = NULL;
    }

    if (ISP_RET_SUCCESS == res)
    {
        pDDRParams->sDDRPInit = TRUE;
    }
    STEST_FUNC_EXIT();

    return res;
}

///define transfer raw cma buffer(CPU side) to isp vin ddr(FPGA side)
static isp_ret_t Stest_Load_Raw_To_Vin(u32  camId)
{
    stest_ddrp_params*       pDDRParams    = NULL;
    isp_ret_t                res           = ISP_RET_SUCCESS;
    STEST_FUNC_ENTER();

    pDDRParams = &g_stest_params[camId].sDDRParams;
    if (pDDRParams->sDDRPInit == FALSE)
    {
        STEST_INFO("DDRP Load Raw Failed\n");
        res = ISP_RET_ERROR;
    }

    ///copy cpu buffer to fpga ddr memory
    if (ISP_RET_SUCCESS == res)
    {
        pthread_mutex_lock(&pDDRParams->sCma_mutex);
        res = Isp_Xdma(VIN_BUFFER_LONG_ADDR,
                       pDDRParams->sMemBlock.phyAddr,
                       pDDRParams->sMemBlock.len,
                       DMA_LOCAL_TO_REMOTE);
        pthread_mutex_unlock(&pDDRParams->sCma_mutex);
    }

    return res;
}

///read back ddr process md5 register value and write to md5 result file
static isp_ret_t Stest_Dump_Isp_Reg_List(u32  camId)
{
#if 1
    STEST_FUNC_ENTER();
    STEST_FUNC_EXIT();
    return ISP_RET_SUCCESS;
#else
    FileHandle*              pFile         = NULL;
    //u32                      regaddr       = 0;
    //u32                      regval        = 0;
    isp_ret_t                res           = ISP_RET_SUCCESS;

    if (g_stest_params[camId].sDDRParams.sDDRPInit == TRUE)
    {
        STEST_INFO("DDRP Already Init\n");
        res = ISP_RET_INGORED_FAILURE;
    }

    ///write Isp reg list to reg_Res.txt
    if (ISP_RET_SUCCESS == res)
    {
        pFile = g_stest_params[camId].sFILE[STFILE_ISPREG_LIST];
        if (NULL != pFile)
        {
            fprintf(pFile, "******Dump camId(%d) submods(53) reg List:  ******\n",
                                camId);
            fprintf(pFile, "******************************************\n\n");
        }
        else
        {
            STEST_ERR("Error! MD5 File open failed\n");
            res = ISP_RET_ERROR;
        }
    }

    return res;
#endif
}

isp_ret_t Stest_Update_Glb_Params(
                sanity_test_params_t*  pStestParams)
{
    u32                camId       = 0;
    stest_case_id_t    stCaseId    = STEST_CASE_INVALID;
    isp_ret_t          res         = ISP_RET_SUCCESS;

    if (!pStestParams)
    {
        STEST_ERR("Error! pStestParams is NULL\n");
        res = ISP_RET_INVALID;
    }
    else
    {
        camId    = pStestParams->sCamId;
        stCaseId = pStestParams->sTCaseId;
        if ((camId >= MAX_CAMERA)
            || (stCaseId < STEST_CASE_DDR_PRO_START)
            || (stCaseId >= STEST_CASE_NUM_MAX))
        {
            STEST_ERR("Invalid camId(%d)/stCaseId(%d)\n", camId, stCaseId);
            res = ISP_RET_INVALID;
        }
    }

    STEST_INFO("stCaseId : %d", stCaseId);
    if (ISP_RET_SUCCESS == res)
    {
        switch (stCaseId) {
            case STEST_CASE_DDR_PRO_START:
            {
                res = Stest_Read_Raw_Data(camId);
                break;
            }
            case STEST_CASE_DDR_PRO_END:
            {
                break;
            }
            case STEST_CASE_APIC_START:
            {
                break;
            }
            case STEST_CASE_DDRP_REGLIST_END:
            {
                res = Stest_RegList_Stop_Record(camId);
                break;
            }
            case STEST_CASE_DDRP_REGLIST_START:
            {
                res = Stest_RegList_Start_Record(camId);
                break;
            }
            case STEST_CASE_APIC_END:
            {
                 res = Stest_Res_File_Save(camId, STFILE_APICHK_RES);
                break;
            }
            case STEST_CASE_DDRP_LD_SDRAW:
            {
                res = Stest_Load_Raw_To_Vin(camId);
                break;
            }
            //case STEST_CASE_DDRP_LD_RAWTXT:
            case STEST_CASE_DDRP_GEN_MD5:
            {
                res = Stest_Res_File_Save(camId, STFILE_YUVMD5_RES);
                break;
            }
            case STEST_CASE_DDRP_DUMP_REGLIST:
            {
                res = Stest_Dump_Isp_Reg_List(camId);
                break;
            }
            default:
                STEST_INFO("invalid stCaseId: ", stCaseId);
                res = ISP_RET_INVALID;
                break;
        }
    }

    if (ISP_RET_SUCCESS == res
        || ISP_RET_INGORED_FAILURE == res)
    {
        g_stest_params[camId].sCamId   = camId;
        g_stest_params[camId].sTCaseId = stCaseId;
        g_cur_CamId                    = camId;
    }

    return res;
}

static isp_ret_t Stest_Api_Params_Check(stest_event_t*  pStestEvt)
{
    sanity_test_params_t*    pStestParams = NULL;
    isp_ret_t                res          = ISP_RET_SUCCESS;

    if (pStestEvt &&
        pStestEvt->camId < MAX_CAMERA &&
        pStestEvt->type < API_TYPE_TOTAL &&
        pStestEvt->direction < META_EVT_DIRT_MAX &&
        pStestEvt->evtId < STEST_EVTNAME_ARRAY_NUM)
    {
        pStestParams  = &g_stest_params[pStestEvt->camId];
        if (pStestParams->sTCaseId != STEST_CASE_APIC_START)
        {
            STEST_ERR("Invalid sTCaseId(%d)\n", pStestParams->sTCaseId);
            res = ISP_RET_INGORED_FAILURE;
        }
    }
    else
    {
        STEST_ERR("Invalid pStestEvt params\n");
        res = ISP_RET_INVALID;
    }

    return res;
}

isp_ret_t Stest_Api_Evt_Params(void*  pApiData)
{
    stest_event_t*           pStestEvt    = (stest_event_t *)pApiData;
    sanity_test_params_t*    pStestParams = NULL;
    stest_event_params*      pEvtParams   = NULL;
    isp_ret_t                res          = ISP_RET_SUCCESS;

    res = Stest_Api_Params_Check(pStestEvt);
    /// do sanity test check api
    if (ISP_RET_SUCCESS == res)
    {
        u32 type  = pStestEvt->type;
        u32 evtId = pStestEvt->evtId;
        pStestParams = &g_stest_params[pStestEvt->camId];
        pEvtParams = &pStestParams->sEvtPrms[type][pStestEvt->direction];
        if (!pEvtParams->sEvtPrms[evtId].sDataInit)
        {
            pEvtParams->sEvtCnt += 1;
            pEvtParams->sEvtPrms[evtId].sDataInit = TRUE;
        }
        pEvtParams->sEvtPrms[evtId].sEvtNewId =
                        (type << 16) | (evtId & 0xFFFF);
        pEvtParams->sEvtPrms[evtId].sEvtVal = pStestEvt->data;
        STEST_INFO("STEST type(%d) evtId(%d) <%s> Val(0x%x)",
                    type,
                    evtId,
                    gDirectionName[pStestEvt->direction],
                    pEvtParams->sEvtPrms[evtId].sEvtVal);
    }

    return res;
}

static isp_ret_t Stest_Reglist_Status_Chk(u32            regLenb,
                                          isp_hw_req_t*  pHwReq)
{
    isp_ret_t  res    = ISP_RET_SUCCESS;

    if (g_cur_CamId >= MAX_CAMERA ||
        !g_stest_params[g_cur_CamId].pFileHdl ||
        (g_stest_params[g_cur_CamId].sTCaseId
                != STEST_CASE_DDRP_REGLIST_START))
    {
        res = ISP_RET_INVALID;
    }

    if (ISP_RET_SUCCESS == res)
    {
        if (regLenb)
        {
            if (!pHwReq || !pHwReq->argv)
            {
                STEST_ERR("pHwReq/pHwReq->argv is NULL");
                res = ISP_RET_INVALID;
            }
        }
    }

    return res;
}

//print isp & sensor & motor register write and read information for debug.
void Isp_V4l_Hw_Debug_Print(isp_hw_req_t*  pHwReq,
                            drv_direction  direct)
{
    isp_hw_list_cfg_t*      pListReq    = NULL;
    isp_reg_list_t*         pIspReglist = NULL;
    isp_hw_regs_cfg_t*      pRegsCfg    = NULL;
    drv_regs_array_info_t*  pRegsInfo   = NULL;
    u32                     regListSize = 0;
    isp_ret_t               res         = ISP_RET_SUCCESS;

    res = Stest_Reglist_Status_Chk(1, pHwReq);
    if (res == ISP_RET_SUCCESS)
    {
        STEST_FUNC_ENTER();
        FileHandle* pFile = g_stest_params[g_cur_CamId].pFileHdl;
        if (pHwReq->devType == DRV_DEV_ISP)
        {
            pListReq = (isp_hw_list_cfg_t *)pHwReq->argv;
            pIspReglist = pListReq->pUpdateList;

            while (NULL != pIspReglist && NULL != pIspReglist->cfg)
            {
                regListSize = pIspReglist->num;
                //OSIF_CFG("list size = %d", regListSize);
                for (s32 i = 0; i < regListSize; i ++)
                {
                    if (direct == DRV_DIRECTION_WRITE)
                    {
                        fprintf(pFile, "ISP_REG_WRITE regaddr:0x%x regval:0x%x dsof:%d\n",
                            pIspReglist->cfg[i].addr,
                            pIspReglist->cfg[i].val,
                            pIspReglist->cfg[i].sofDelay);
                    }
                    else
                    {
                        fprintf(pFile, "ISP_REG_READ regaddr:0x%x regval:0x%x dsof:%d\n",
                            pIspReglist->cfg[i].addr,
                            pIspReglist->cfg[i].val,
                            pIspReglist->cfg[i].sofDelay);
                    }
                }
                pIspReglist = pIspReglist->next;
            }
        }
        else if (pHwReq->devType == DRV_DEV_SENSOR || pHwReq->devType == DRV_DEV_ACTUOAOR)
        {
            pRegsCfg  = (isp_hw_regs_cfg_t *)pHwReq->argv;
            pRegsInfo = pRegsCfg->pRegsInfo;
            if (NULL == pRegsInfo)
            {
                res = ISP_RET_INVALID;
            }
            else
            {
                regListSize = pRegsInfo->settingSize;
            }
            if (regListSize > 1)
            {
                STEST_INFO("read list size = %d", regListSize);
            }

            for (s32 i = 0; i < regListSize; i ++)
            {
                if (pRegsInfo->direction == DRV_DIRECTION_WRITE)
                {
                    if (pHwReq->devType == DRV_DEV_SENSOR)
                    {
                        fprintf(pFile, "I2C_REG_WRITE hwchnl:%d slvaddr:0x%x regDT:%d regAT:%d regaddr:0x%x regval:0x%x dUs:%d\n",
                            pRegsCfg->channel,
                            pRegsCfg->slaveAddr,
                            pRegsInfo->regDataType,
                            pRegsInfo->regAddrType,
                            pRegsInfo->settings[i].regAddr,
                            pRegsInfo->settings[i].regData,
                            pRegsInfo->settings[i].delayUs);
                    }
                    else
                    {
                        fprintf(pFile, "SPI_REG_WRITE hwchnl:%d regDT:%d regAT:%d regaddr:0x%x regval:0x%x dUs:%d\n",
                            pRegsCfg->channel,
                            pRegsInfo->regDataType,
                            pRegsInfo->regAddrType,
                            pRegsInfo->settings[i].regAddr,
                            pRegsInfo->settings[i].regData,
                            pRegsInfo->settings[i].delayUs);
                    }
                }
                else
                {
                    if (pHwReq->devType == DRV_DEV_SENSOR)
                    {
                        fprintf(pFile, "I2C_REG_READ hwchnl:%d slvaddr:0x%x regDT:%d regAT:%d regaddr:0x%x regval:0x%x dUs:%d\n",
                            pRegsCfg->channel,
                            pRegsCfg->slaveAddr,
                            pRegsInfo->regDataType,
                            pRegsInfo->regAddrType,
                            pRegsInfo->settings[i].regAddr,
                            pRegsInfo->settings[i].regData,
                            pRegsInfo->settings[i].delayUs);
                    }
                    else
                    {
                        fprintf(pFile, "SPI_REG_READ hwchnl:%d regDT:%d regAT:%d regaddr:%x regval:%x dUs:%d",
                            pRegsCfg->channel,
                            pRegsInfo->regDataType,
                            pRegsInfo->regAddrType,
                            pRegsInfo->settings[i].regAddr,
                            pRegsInfo->settings[i].regData,
                            pRegsInfo->settings[i].delayUs);
                    }
                }
            }
        }
        STEST_FUNC_EXIT();
    }
}
