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
#ifndef __TUNING_AR0239_NORMAL_H__
#define __TUNING_AR0239_NORMAL_H__
/*
*
* tuneVersion
*
*/
{
    0, 0, 0, 63212, /// major, minor, debug, check
},
/*
*
* sw3aParams
*
*/
{
    /*
    *
    * sw3aAlgoVersion
    *
    */
    {
        0, 1, 0, 0, /// major, minor, debug, check
    },
    /*
    *
    * sw3aTuneVersion
    *
    */
    {
        0, 1, 0, 16212, /// major, minor, debug, check
    },
    /*
    *
    * ae
    *
    */
    {
        1     ,  /// aeEnable
        0     ,  /// aeHdrExpBaseSel
        2     ,  /// aeStatsSel
        0     ,  /// antiFlickerEnable
        50    ,  /// antiFlickerFrequency
        4096  ,  /// default 4096,[410:40960] manual,lux index compensation to reference module, value is 4096*refY/testY
        1     ,  /// exposureTableEnable
        /*
        *
        * aeRoi
        *
        */
        {
            0     ,  /// aeManualRoiEnable
            {1   , 1   , 13  , 13  },  /// roi left top width height
        },
        /*
        *
        * aeWeightTab
        *
        */
        {
            1     ,  /// aeWeightTabSel
            {
                {
                    0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,
                    0   , 7   , 8   , 8   , 9   , 9   , 10  , 10  , 10  , 9   , 9   , 8   , 8   , 7   , 0   ,
                    0   , 8   , 9   , 9   , 10  , 11  , 11  , 11  , 11  , 11  , 10  , 9   , 9   , 8   , 0   ,
                    0   , 8   , 9   , 10  , 11  , 12  , 12  , 12  , 12  , 12  , 11  , 10  , 9   , 8   , 0   ,
                    0   , 9   , 10  , 11  , 12  , 13  , 13  , 13  , 13  , 13  , 12  , 11  , 10  , 9   , 0   ,
                    0   , 9   , 11  , 12  , 13  , 13  , 14  , 14  , 14  , 13  , 13  , 12  , 11  , 9   , 0   ,
                    0   , 10  , 11  , 12  , 13  , 14  , 14  , 14  , 14  , 14  , 13  , 12  , 11  , 10  , 0   ,
                    0   , 10  , 11  , 12  , 13  , 14  , 14  , 15  , 14  , 14  , 13  , 12  , 11  , 10  , 0   ,
                    0   , 10  , 11  , 12  , 13  , 14  , 14  , 14  , 14  , 14  , 13  , 12  , 11  , 10  , 0   ,
                    0   , 9   , 11  , 12  , 13  , 13  , 14  , 14  , 14  , 13  , 13  , 12  , 11  , 9   , 0   ,
                    0   , 9   , 10  , 11  , 12  , 13  , 13  , 13  , 13  , 13  , 12  , 11  , 10  , 9   , 0   ,
                    0   , 8   , 9   , 10  , 11  , 12  , 12  , 12  , 12  , 12  , 11  , 10  , 9   , 8   , 0   ,
                    0   , 8   , 9   , 9   , 10  , 11  , 11  , 11  , 11  , 11  , 10  , 9   , 9   , 8   , 0   ,
                    0   , 7   , 8   , 8   , 9   , 9   , 10  , 10  , 10  , 9   , 9   , 8   , 8   , 7   , 0   ,
                    0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,
                },
                {
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                },
                {
                    0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,
                    0   , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 0   ,
                    0   , 14  , 14  , 14  , 14  , 14  , 14  , 14  , 14  , 14  , 14  , 14  , 14  , 14  , 0   ,
                    0   , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 0   ,
                    0   , 12  , 12  , 12  , 12  , 12  , 12  , 12  , 12  , 12  , 12  , 12  , 12  , 12  , 0   ,
                    0   , 11  , 11  , 11  , 11  , 11  , 11  , 11  , 11  , 11  , 11  , 11  , 11  , 11  , 0   ,
                    0   , 10  , 10  , 10  , 10  , 10  , 10  , 10  , 10  , 10  , 10  , 10  , 10  , 10  , 0   ,
                    0   , 9   , 9   , 9   , 9   , 9   , 9   , 9   , 9   , 9   , 9   , 9   , 9   , 9   , 0   ,
                    0   , 8   , 8   , 8   , 8   , 8   , 8   , 8   , 8   , 8   , 8   , 8   , 8   , 8   , 0   ,
                    0   , 7   , 7   , 7   , 7   , 7   , 7   , 7   , 7   , 7   , 7   , 7   , 7   , 7   , 0   ,
                    0   , 6   , 6   , 6   , 6   , 6   , 6   , 6   , 6   , 6   , 6   , 6   , 6   , 6   , 0   ,
                    0   , 5   , 5   , 5   , 5   , 5   , 5   , 5   , 5   , 5   , 5   , 5   , 5   , 5   , 0   ,
                    0   , 4   , 4   , 4   , 4   , 4   , 4   , 4   , 4   , 4   , 4   , 4   , 4   , 4   , 0   ,
                    0   , 3   , 3   , 3   , 3   , 3   , 3   , 3   , 3   , 3   , 3   , 3   , 3   , 3   , 0   ,
                    0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,
                },
                {
                    0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,
                    0   , 3   , 3   , 3   , 3   , 3   , 3   , 3   , 3   , 3   , 3   , 3   , 3   , 3   , 0   ,
                    0   , 4   , 4   , 4   , 4   , 4   , 4   , 4   , 4   , 4   , 4   , 4   , 4   , 4   , 0   ,
                    0   , 5   , 5   , 5   , 5   , 5   , 5   , 5   , 5   , 5   , 5   , 5   , 5   , 5   , 0   ,
                    0   , 6   , 6   , 6   , 6   , 6   , 6   , 6   , 6   , 6   , 6   , 6   , 6   , 6   , 0   ,
                    0   , 7   , 7   , 7   , 7   , 7   , 7   , 7   , 7   , 7   , 7   , 7   , 7   , 7   , 0   ,
                    0   , 8   , 8   , 8   , 8   , 8   , 8   , 8   , 8   , 8   , 8   , 8   , 8   , 8   , 0   ,
                    0   , 9   , 9   , 9   , 9   , 9   , 9   , 9   , 9   , 9   , 9   , 9   , 9   , 9   , 0   ,
                    0   , 10  , 10  , 10  , 10  , 10  , 10  , 10  , 10  , 10  , 10  , 10  , 10  , 10  , 0   ,
                    0   , 11  , 11  , 11  , 11  , 11  , 11  , 11  , 11  , 11  , 11  , 11  , 11  , 11  , 0   ,
                    0   , 12  , 12  , 12  , 12  , 12  , 12  , 12  , 12  , 12  , 12  , 12  , 12  , 12  , 0   ,
                    0   , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 0   ,
                    0   , 14  , 14  , 14  , 14  , 14  , 14  , 14  , 14  , 14  , 14  , 14  , 14  , 14  , 0   ,
                    0   , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 0   ,
                    0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,
                },
                {
                    0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,
                    0   , 15  , 14  , 13  , 12  , 11  , 10  , 9   , 8   , 7   , 6   , 5   , 4   , 3   , 0   ,
                    0   , 15  , 14  , 13  , 12  , 11  , 10  , 9   , 8   , 7   , 6   , 5   , 4   , 3   , 0   ,
                    0   , 15  , 14  , 13  , 12  , 11  , 10  , 9   , 8   , 7   , 6   , 5   , 4   , 3   , 0   ,
                    0   , 15  , 14  , 13  , 12  , 11  , 10  , 9   , 8   , 7   , 6   , 5   , 4   , 3   , 0   ,
                    0   , 15  , 14  , 13  , 12  , 11  , 10  , 9   , 8   , 7   , 6   , 5   , 4   , 3   , 0   ,
                    0   , 15  , 14  , 13  , 12  , 11  , 10  , 9   , 8   , 7   , 6   , 5   , 4   , 3   , 0   ,
                    0   , 15  , 14  , 13  , 12  , 11  , 10  , 9   , 8   , 7   , 6   , 5   , 4   , 3   , 0   ,
                    0   , 15  , 14  , 13  , 12  , 11  , 10  , 9   , 8   , 7   , 6   , 5   , 4   , 3   , 0   ,
                    0   , 15  , 14  , 13  , 12  , 11  , 10  , 9   , 8   , 7   , 6   , 5   , 4   , 3   , 0   ,
                    0   , 15  , 14  , 13  , 12  , 11  , 10  , 9   , 8   , 7   , 6   , 5   , 4   , 3   , 0   ,
                    0   , 15  , 14  , 13  , 12  , 11  , 10  , 9   , 8   , 7   , 6   , 5   , 4   , 3   , 0   ,
                    0   , 15  , 14  , 13  , 12  , 11  , 10  , 9   , 8   , 7   , 6   , 5   , 4   , 3   , 0   ,
                    0   , 15  , 14  , 13  , 12  , 11  , 10  , 9   , 8   , 7   , 6   , 5   , 4   , 3   , 0   ,
                    0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,
                },
                {
                    0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,
                    0   , 3   , 4   , 5   , 6   , 7   , 8   , 9   , 10  , 11  , 12  , 13  , 14  , 15  , 0   ,
                    0   , 3   , 4   , 5   , 6   , 7   , 8   , 9   , 10  , 11  , 12  , 13  , 14  , 15  , 0   ,
                    0   , 3   , 4   , 5   , 6   , 7   , 8   , 9   , 10  , 11  , 12  , 13  , 14  , 15  , 0   ,
                    0   , 3   , 4   , 5   , 6   , 7   , 8   , 9   , 10  , 11  , 12  , 13  , 14  , 15  , 0   ,
                    0   , 3   , 4   , 5   , 6   , 7   , 8   , 9   , 10  , 11  , 12  , 13  , 14  , 15  , 0   ,
                    0   , 3   , 4   , 5   , 6   , 7   , 8   , 9   , 10  , 11  , 12  , 13  , 14  , 15  , 0   ,
                    0   , 3   , 4   , 5   , 6   , 7   , 8   , 9   , 10  , 11  , 12  , 13  , 14  , 15  , 0   ,
                    0   , 3   , 4   , 5   , 6   , 7   , 8   , 9   , 10  , 11  , 12  , 13  , 14  , 15  , 0   ,
                    0   , 3   , 4   , 5   , 6   , 7   , 8   , 9   , 10  , 11  , 12  , 13  , 14  , 15  , 0   ,
                    0   , 3   , 4   , 5   , 6   , 7   , 8   , 9   , 10  , 11  , 12  , 13  , 14  , 15  , 0   ,
                    0   , 3   , 4   , 5   , 6   , 7   , 8   , 9   , 10  , 11  , 12  , 13  , 14  , 15  , 0   ,
                    0   , 3   , 4   , 5   , 6   , 7   , 8   , 9   , 10  , 11  , 12  , 13  , 14  , 15  , 0   ,
                    0   , 3   , 4   , 5   , 6   , 7   , 8   , 9   , 10  , 11  , 12  , 13  , 14  , 15  , 0   ,
                    0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,
                },
            },
        },
        /*
        *
        * aeConvParam
        *
        */
        {
            6     ,  /// aeConvergeStep default 6,[3:32]
            60    ,  /// aeMaxConvSpeed default 60,[30 : 200] 100 base percent
            4     ,  /// aeStableInRange default 4,[2:255] AE convergence stability range in
            64    ,  /// aeStableOutRange default 32,[5:512] AE convergence stability range out
            960   ,  /// aeSaturationThr default 960,[0:1023] AE Saturation Threshold, use to calculation linaer Y
            2     ,  /// aeStatsYIdx default 3,[0,3] which stats Y use to judge ae stable out
            15    ,  /// aeChgFrameCnt default 15,[0, 4096] AE change time out frames
        },
        /*
        *
        * aecLuxConvTabSBase
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]lux index 0, 50, 100, ...
                /// [1]aeTarget, default 250,[0:1023] Base brightness target
                /// [2]aeHighContrTarget, default 60,[0:100] high dynamic range target
                /// [3]aeLowLevel, default 15000,[0:100000] dark area percent 100000 is 100%
                /// [4]aeHighLevel, default 95000,[0:100000] bright area percent 100000 is 100%
                /// [5]aeLowContrast, default 15,[0:255] dynamic range lower bound
                /// [6]aeHighContrast, default 25,[0:255] dynamic range upper bound
                /// [7]aeLongTarget, default 40,[0:100] HDR Long Target for DOL2/3/4
                /// [8]aeLmRatioMax, default 32,[1:1024] HDR Max L/M ratio
                /// [9]aeMidTarget, default 40,[0:100] HDR Mid Target for DOL3/4
                /// [10]aeMsRatioMax, default 32,[1:1024] HDR Max M/S ratio
                /// [11]aeShortTarget, default 40,[0:100] HDR Short Target for DOL4
                /// [12]aeSvsRatioMax, default 32,[1:128] HDR Max S/VS ratio
                /// [13]aeSaturationMaxPercent, default 10240, [0:102400]AE Saturation Max Pcent use for Long base DOL, 1024 is %1
                /// 0     1      2      3      4      5      6      7      8      9      10     11     12     13
                {50    , {220  , 60   , 20000, 92000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {100   , {210  , 60   , 20000, 92000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {150   , {200  , 60   , 20000, 92000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {200   , {190  , 60   , 20000, 92000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {250   , {180  , 60   , 20000, 93500, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {300   , {160  , 60   , 20000, 95000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {350   , {140  , 55   , 20000, 97000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {400   , {135  , 50   , 20000, 97000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {450   , {120  , 45   , 20000, 97000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {500   , {110  , 45   , 20000, 97000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
            },
        },
        /*
        *
        * aecLuxConvTabLBase
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]lux index 0, 50, 100, ...
                /// [1]aeTarget, default 250,[0:1023] Base brightness target
                /// [2]aeHighContrTarget, default 60,[0:100] high dynamic range target
                /// [3]aeLowLevel, default 15000,[0:100000] dark area percent 100000 is 100%
                /// [4]aeHighLevel, default 95000,[0:100000] bright area percent 100000 is 100%
                /// [5]aeLowContrast, default 15,[0:255] dynamic range lower bound
                /// [6]aeHighContrast, default 25,[0:255] dynamic range upper bound
                /// [7]aeLongTarget, default 40,[0:100] HDR Long Target for DOL2/3/4
                /// [8]aeLmRatioMax, default 32,[1:1024] HDR Max L/M ratio
                /// [9]aeMidTarget, default 40,[0:100] HDR Mid Target for DOL3/4
                /// [10]aeMsRatioMax, default 32,[1:1024] HDR Max M/S ratio
                /// [11]aeShortTarget, default 40,[0:100] HDR Short Target for DOL4
                /// [12]aeSvsRatioMax, default 32,[1:128] HDR Max S/VS ratio
                /// [13]aeSaturationMaxPercent, default 10240, [0:102400]AE Saturation Max Pcent use for Long base DOL, 1024 is %1
                /// 0     1      2      3      4      5      6      7      8      9      10     11     12     13
                {50    , {220  , 60   , 20000, 90000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {100   , {210  , 60   , 20000, 90000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {150   , {200  , 60   , 20000, 91000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {200   , {190  , 60   , 20000, 92000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {250   , {180  , 60   , 20000, 93500, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {300   , {160  , 60   , 20000, 95000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {350   , {140  , 55   , 20000, 97000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {400   , {135  , 50   , 20000, 97000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {450   , {120  , 45   , 20000, 97000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {500   , {110  , 45   , 20000, 97000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
            },
        },
        /*
        *
        * aecLuxConvTabSBaseHlSup
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]lux index 0, 50, 100, ...
                /// [1]aeTarget, default 250,[0:1023] Base brightness target
                /// [2]aeHighContrTarget, default 60,[0:100] high dynamic range target
                /// [3]aeLowLevel, default 15000,[0:100000] dark area percent 100000 is 100%
                /// [4]aeHighLevel, default 95000,[0:100000] bright area percent 100000 is 100%
                /// [5]aeLowContrast, default 15,[0:255] dynamic range lower bound
                /// [6]aeHighContrast, default 25,[0:255] dynamic range upper bound
                /// [7]aeLongTarget, default 40,[0:100] HDR Long Target for DOL2/3/4
                /// [8]aeLmRatioMax, default 32,[1:1024] HDR Max L/M ratio
                /// [9]aeMidTarget, default 40,[0:100] HDR Mid Target for DOL3/4
                /// [10]aeMsRatioMax, default 32,[1:1024] HDR Max M/S ratio
                /// [11]aeShortTarget, default 40,[0:100] HDR Short Target for DOL4
                /// [12]aeSvsRatioMax, default 32,[1:128] HDR Max S/VS ratio
                /// [13]aeSaturationMaxPercent, default 10240, [0:102400]AE Saturation Max Pcent use for Long base DOL, 1024 is %1
                /// 0     1      2      3      4      5      6      7      8      9      10     11     12     13
                {50    , {220  , 80   , 20000, 99700, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10   }},
                {100   , {210  , 80   , 20000, 99700, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10   }},
                {150   , {200  , 80   , 20000, 99700, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10   }},
                {200   , {190  , 80   , 20000, 99700, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10   }},
                {250   , {180  , 80   , 20000, 99700, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10   }},
                {300   , {160  , 80   , 20000, 99700, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10   }},
                {350   , {140  , 80   , 20000, 99700, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10   }},
                {400   , {135  , 80   , 20000, 99700, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10   }},
                {450   , {120  , 80   , 20000, 99700, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10   }},
                {500   , {110  , 80   , 20000, 99700, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10   }},
            },
        },
        /*
        *
        * faceAecLuxConvTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]faceAeLuxIdx
                /// [1]faceAeStatsSel (0:RAW 1:YUV)
                /// [2]faceAeConvergeStep (Face AE Convergance Step, 1-32)
                /// [3]faceAeStableRange (Face Stable Range, 1-255)
                /// [4]faceAeRawTarget (FACE_STATS_SEL = 0 use raw target, 10bit)
                /// [5]faceAeYuvTarget (FACE_STATS_SEL = 1 use yuv Target, 10bit)
                /// [6]faceAeScale (100 base,10 - 1000)
                /// [7]faceAeCropTop
                /// [8]faceAeCropBot
                /// [9]faceAeCropLef
                /// [10]faceAeCropRig
                /// [11]faceAeInCntThr (face in count(1-100) greater than this value, judge as had face enter)
                /// [12]faceAeOutCntThr (face out count(1-100) greater than this value, judge as face disappear)
                /// [13]faceAeSmallSizeThr (face size (face_width * face_height / img_width * img_height), less than this value, judge as small face. 10000 base range:1-10000)
                /// 0     1      2      3      4      5      6      7      8      9      10     11     12     13
                {50    , {1    , 10   , 2    , 188  , 600  , 100  , 2    , 1    , 1    , 1    , 4    , 8    , 1    }},
                {100   , {1    , 10   , 2    , 188  , 600  , 100  , 2    , 1    , 1    , 1    , 4    , 8    , 1    }},
                {150   , {1    , 10   , 2    , 188  , 600  , 100  , 2    , 1    , 1    , 1    , 4    , 8    , 1    }},
                {200   , {1    , 10   , 2    , 188  , 600  , 100  , 2    , 1    , 1    , 1    , 4    , 8    , 1    }},
                {250   , {1    , 10   , 2    , 188  , 600  , 100  , 2    , 1    , 1    , 1    , 4    , 8    , 1    }},
                {300   , {1    , 10   , 2    , 188  , 600  , 100  , 2    , 1    , 1    , 1    , 4    , 8    , 1    }},
                {350   , {1    , 10   , 2    , 188  , 600  , 100  , 2    , 1    , 1    , 1    , 4    , 8    , 1    }},
                {400   , {1    , 10   , 2    , 188  , 600  , 100  , 2    , 1    , 1    , 1    , 4    , 8    , 1    }},
                {450   , {1    , 10   , 2    , 188  , 600  , 100  , 2    , 1    , 1    , 1    , 4    , 8    , 1    }},
                {500   , {1    , 10   , 2    , 188  , 600  , 100  , 2    , 1    , 1    , 1    , 4    , 8    , 1    }},
            },
        },
        /*
        *
        * aecExpKneePoint
        *
        */
        {
            5     ,  /// validSize
            {
                /// [0]index 0,1,2,...10
                /// [1]frameRate
                /// [2]totalGain
                /// [3]incrementPriority; 0: exp time 1:gain
                /// 0     1      2      3
                {0     , {100000, 1024 , 0    }},
                {1     , {25   , 1024 , 1    }},
                {2     , {25   , 32768, 0    }},
                {3     , {15   , 32768, 1    }},
                {4     , {15   , 2097152, 0    }},
            },
        },
    },
    /*
    *
    * awb
    *
    */
    {
        1     ,  /// awbEnable
        6     ,  /// awbConvergeStep
        1     ,  /// awbStableRange
        0     ,  /// awbLumaWeightEnable
        256   ,  /// awbPrefGainR
        256   ,  /// awbPrefGainB
        420   ,  /// awbLowlightLuxThr
        3800  ,  /// awbLowctRgPosThr
        4     ,  /// awbDarkPixelThr
        4090  ,  /// awbSatPixelThr
        50    ,  /// awbDarkPrcThr
        25    ,  /// awbSatPrcThr
        /*
        *
        * awbRoi
        *
        */
        {
            0     ,  /// awbManualRoiEnable
            {0   , 0   , 15  , 15  },  /// roi left top width height
        },
        /*
        *
        * awbLightSourceAreaScale
        *
        */
        {
            3     ,  /// validSize
            {
                /// [0] index 0,1,2,3 .. 10
                /// [1] default 0, [0]disable, [1]enable
                /// [2] default 0 [0: 6] light source index
                /// [3] lux index region start [1:1000]
                /// [4] lux index region end [1:1000]
                /// [5] lux index left and right buffer [1:256]
                /// [6] default 256 [0: 512] light source area left scale 256 is 1x, 512 is up scale 2x, 0 is down scale 2x
                /// [7] default 256 [0: 512] light source area right scale 256 is 1x, 512 is up scale 2x, 0 is down scale 2x
                /// [8] default 256 [0: 512] light source area top scale 256 is 1x, 512 is up scale 2x, 0 is down scale 2x
                /// [9] default 256 [0: 512] light source area bottom scale 256 is 1x, 512 is up scale 2x, 0 is down scale 2x
                /// 0     1      2      3      4      5      6      7      8      9
                {0     , {1    , 0    , 0    , 1000 , 20   , 256  , 256  , 1024 , 256  }},
                {1     , {1    , 2    , 0    , 1000 , 20   , 256  , 256  , 1024 , 256  }},
                {2     , {1    , 4    , 0    , 1000 , 20   , 256  , 256  , 400  , 256  }},
            },
        },
        /*
        *
        * awbDetectZone
        *
        */
        {
            1     ,  /// validSize
            {
                /// [0] index 0,1,2,3 .. 10
                /// [1] default 0, [0]disable, [1]enable
                /// [2] lux index region start [1:1000]
                /// [3] lux index region end [1:1000]
                /// [4] lux index left and right buffer [1:256]
                /// [5] zone left [-1000:1000]
                /// [6] zone right [-1000:1000]
                /// [7] zone top [-1000:1000]
                /// [8] zone bottom [-1000:1000]
                /// 0     1      2      3      4      5      6      7      8
                {0     , {0    , 190  , 250  , 20   , -53  , 114  , -234 , -265 }},
            },
        },
        /*
        *
        * awbMisleadColorZone
        *
        */
        {
            1     ,  /// validSize
            {
                /// [0] index 0,1,2,3 .. 10
                /// [1] default 0, [0]disable, [1]enable
                /// [2] detect color zone index, 0:6 is H/A/TL84/CWF/D65/DF/SHADE. [0:16]
                /// [3] lux index region start [1:1000]
                /// [4] lux index region end [1:1000]
                /// [5] lux index left and right buffer [1:256]
                /// [6] detect point percent threshold [0:1024]
                /// [7] detect white point percent buffer [0:512]
                /// [8] zone left [-1000:1000]
                /// [9] zone right [-1000:1000]
                /// [10] zone top [-1000:1000]
                /// [11] zone bottom [-1000:1000]
                /// 0     1      2      3      4      5      6      7      8      9      10     11
                {0     , {0    , 6    , 0    , 1000 , 10   , 40   , 50   , -78  , 52   , -225 , -248 }},
            },
        },
        /*
        *
        * awbPrefColorZone
        *
        */
        {
            0     ,  /// validSize
            {
                /// [0] index 0,1,2,3 .. 10
                /// [1] default 0, [0]disable, [1]enable
                /// [2] ref light source index (0:6) H/A/TL84/CWF/D65/DF/SHADE
                /// [3] lux index low thr(0:1000)
                /// [4] lux index high thr(0:1000)
                /// [5] lux index interpolation left and right buf(0:255)
                /// [6] percent of light source area width, x large than Thr then clip x,(0:256) 256 is 100%
                /// [7] percent of light source area height, y large than Thr then clip y,(0:256) 256 is 100%
                /// [8] X offset clip ratio (0:256) 256 is 100%
                /// [9] X offset clip ratio (0:256) 256 is 100%
                /// [10] shift R gain 256 base (128:512)
                /// [11] shift B gain 256 base (128:512)
                /// 0     1      2      3      4      5      6      7      8      9      10     11
            },
        },
        /*
        *
        * awbLightsourceProb
        *
        */
        {
            1     ,  /// validSize
            {
                /// [0] lux index
                /// [1] H prob[0:256]
                /// [2] A prob[0:256]
                /// [3] TL84 prob[0:256]
                /// [4] CWF prob[0:256]
                /// [5] D65 prob[0:256]
                /// [6] DF prob[0:256]
                /// [7] Shade prob[0:256]
                /// 0     1      2      3      4      5      6      7
                {50    , {256  , 256  , 256  , 256  , 256  , 256  , 256  }},
            },
        },
        /*
        *
        * awbStatisticsShift
        *
        */
        {
            7     ,  /// validSize
            {
                /// [0] index 0,1,2,3 .. 10, 0-6 must set as H/A/TL84/CWF/D65/DF/SHADE
                /// [1] default 0, [0]disable, [1]enable
                /// [2] light source index [0:6]
                /// [3] lux index region start [1:1000]
                /// [4] lux index region end [1:1000]
                /// [5] lux index left and right buffer [1:256]
                /// [6] default 0[0: 256] limitY threshold Prc
                /// [7] default 0[0: 256] weight reduce percent 256 is 100%
                /// 0     1      2      3      4      5      6      7
                {0     , {1    , 0    , 0    , 1000 , 10   , 32   , 256  }},
                {1     , {1    , 1    , 0    , 1000 , 10   , 32   , 256  }},
                {2     , {1    , 2    , 0    , 1000 , 10   , 32   , 256  }},
                {3     , {1    , 3    , 0    , 1000 , 10   , 96   , 256  }},
                {4     , {1    , 4    , 0    , 1000 , 10   , 32   , 256  }},
                {5     , {1    , 5    , 0    , 1000 , 10   , 128  , 256  }},
                {6     , {1    , 6    , 0    , 1000 , 10   , 32   , 256  }},
            },
        },
        /*
        *
        * awbExtraColorZone
        *
        */
        {
            1     ,  /// validSize
            {
                /// [0] index 0,1,2,3 .. 10
                /// [1] default 0, [0]disable, [1]enable
                /// [2] lux index region start [1:1000]
                /// [3] lux index region end [1:1000]
                /// [4] lux index left and right buffer [1:256]
                /// [5] detect point percent threshold [0:1024]
                /// [6] detect white point percent buffer [0:512]
                /// [7] zone left [-1000:1000]
                /// [8] zone right [-1000:1000]
                /// [9] zone top [-1000:1000]
                /// [10] zone bottom [-1000:1000]
                /// [11] light source index [0:7] H/A/TL84/CWF/D65/DF/SHADE/NULL
                /// [12] extra color weight[0:256]
                /// [13] shiftRGain, default 256 [128: 512]
                /// [14] shiftBGain, default 256 [128: 512]
                /// 0     1      2      3      4      5      6      7      8      9      10     11     12     13     14
                {0     , {1    , 300  , 1000 , 20   , 256  , 128  , -14  , 635  , -23  , -121 , 7    , 256  , 256  , 256  }},
            },
        },
        /*
        *
        * awbSpecialGain
        *
        */
        {
            3     ,  /// validSize
            {
                /// [0] lux index
                /// [1] default 256 [256: 1024] use for H
                /// [2] default 256 [256: 1024] use for H
                /// [3] default 256 [256: 1024] use for A
                /// [4] default 256 [256: 1024] use for A
                /// [5] default 256 [256: 1024] use for TL84
                /// [6] default 256 [256: 1024] use for TL84
                /// [7] default 256 [256: 1024] use for CWF
                /// [8] default 256 [256: 1024] use for CWF
                /// [9] default 256 [256: 1024] use for D65
                /// [10] default 256 [256: 1024] use for D65
                /// [11] default 256 [256: 1024] use for DF
                /// [12] default 256 [256: 1024] use for DF
                /// [13] default 256 [256: 1024] use for Shade
                /// [14] default 256 [256: 1024] use for Shade
                /// 0     1      2      3      4      5      6      7      8      9      10     11     12     13     14
                {100   , {361  , 632  , 361  , 632  , 400  , 553  , 452  , 460  , 432  , 501  , 520  , 453  , 432  , 501  }},
                {150   , {361  , 632  , 361  , 632  , 432  , 500  , 452  , 480  , 432  , 501  , 520  , 453  , 432  , 501  }},
                {250   , {361  , 632  , 361  , 632  , 432  , 530  , 432  , 520  , 432  , 501  , 432  , 501  , 432  , 501  }},
            },
        },
        /*
        *
        * awbSpecialGainWeight
        *
        */
        {
            1     ,  /// validSize
            {
                /// [0] lux index
                /// [1] H weight[0:256]
                /// [2] A weight[0:256]
                /// [3] TL84 weight[0:256]
                /// [4] CWF weight[0:256]
                /// [5] D65 weight[0:256]
                /// [6] DF weight[0:256]
                /// [7] Shade weight[0:256]
                /// 0     1      2      3      4      5      6      7
                {250   , {0    , 0    , 0    , 0    , 0    , 0    , 0    }},
            },
        },
        /*
        *
        * awbLumaWeightLut
        *
        */
        {
            3     ,  /// validSize
            {
                {
                    150   ,  /// region
                    {
                        {
                            32  , 48  , 64  , 128 , 192 , 224 , 256 , 256 , 256 , 256 , 256 , 224 , 196 , 128 , 48  , 32  ,
                        },
                    },
                },
                {
                    250   ,  /// region
                    {
                        {
                            32  , 128 , 196 , 256 , 256 , 256 , 256 , 256 , 256 , 256 , 224 , 192 , 128 , 64  , 48  , 32  ,
                        },
                    },
                },
                {
                    350   ,  /// region
                    {
                        {
                            32  , 128 , 196 , 256 , 256 , 256 , 256 , 256 , 256 , 256 , 224 , 192 , 128 , 64  , 48  , 32  ,
                        },
                    },
                },
            },
        },
        /*
        *
        * awbNonNeutralPredict
        *
        */
        {
            1     ,  /// validSize
            {
                /// [0] lux index
                /// [1] neutralPointPrcThrL[0:256]
                /// [2] neutralPointPrcThrH[0:256]
                /// [3] defaultGainWeight[0:256]
                /// [4] defaultGainR[256:2048]
                /// [5] defaultGainG[256:2048]
                /// [6] defaultGainB[256:2048]
                /// [7] enqueNeutralPointThr[0:256]
                /// [8] enqueCWFDFThr[0:256]
                /// 0     1      2      3      4      5      6      7      8
                {500   , {0    , 0    , 128  , 512  , 256  , 512  , 128  , 128  }},
            },
        },
        /*
        *
        * awbShrinkParam
        *
        */
        {
            7     ,  /// validSize
            {
                /// [0] index 0,1,2,3 .. 6, 0-6 must set as H/A/TL84/CWF/D65/DF/SHADE
                /// [1] default 0, [0]disable, [1]enable
                /// [2] light source index [0:6]
                /// [3] shrink ratio of the light source [0:128]
                /// [4] enable left, [0]disable, [1]enable
                /// [5] enable right, [0]disable, [1]enable
                /// [6] enable top, [0]disable, [1]enable
                /// [7] enable bot, [0]disable, [1]enable
                /// [8] left object, 0,1,2,3 .. 6 for H/A/TL84/CWF/D65/DF/SHADE
                /// [9] right object, 0,1,2,3 .. 6 for H/A/TL84/CWF/D65/DF/SHADE
                /// [10] top object, 0,1,2,3 .. 6 for H/A/TL84/CWF/D65/DF/SHADE
                /// [11] bot object, 0,1,2,3 .. 6 for H/A/TL84/CWF/D65/DF/SHADE
                /// 0     1      2      3      4      5      6      7      8      9      10     11
                {0     , {0    , 0    , 10   , 1    , 0    , 0    , 1    , 2    , 0    , 0    , 1 }},
                {1     , {0    , 1    , 10   , 1    , 0    , 1    , 0    , 2    , 1    , 0    , 1 }},
                {2     , {0    , 2    , 10   , 1    , 1    , 0    , 1    , 4    , 0    , 2    , 3 }},
                {3     , {0    , 3    , 10   , 1    , 0    , 1    , 0    , 5    , 3    , 2    , 3 }},
                {4     , {0    , 4    , 10   , 1    , 1    , 0    , 1    , 6    , 2    , 4    , 5 }},
                {5     , {0    , 5    , 10   , 0    , 1    , 1    , 0    , 5    , 3    , 4    , 5 }},
                {6     , {0    , 6    , 10   , 0    , 1    , 0    , 0    , 6    , 4    , 6    , 6 }},
            },
        },
        /*
        *
        * faceAwbParam
        *
        */
        {
            0     ,  /// face awb enable
            1     ,  /// enable exclude face roi white point
            40    ,  /// face awb max weight
            8     ,  /// face reference target tolerance range
            120   ,  /// faceScale, [10, 1000] 100 base
            60    ,  /// face size ratio threshold low, 1024 base
            100   ,  /// face size ratio threshold high, 1024 base
            50    ,  /// distance low threshold between face XY and face target
            200   ,  /// distance high threshold between face XY and face target
            256   ,  /// face awb preference r gain
            256   ,  /// face awb preference b gain
        },
    },
    /*
    *
    * af
    *
    */
    {
        1     ,  /// afEnable
        0     ,  /// afSourceSelYuv
        4     ,  /// afBackjumpCompStep
        1     ,  /// afMoveSkipFrame
        /*
        *
        * afRoi
        *
        */
        {
            1     ,  /// afManualRoiEnable
            {5   , 5   , 5   , 5   },  /// roi left top width height
        },
        5     ,  /// afRoiBlockNumX
        5     ,  /// afRoiBlockNumY
        20    ,  /// afNearRange
        70    ,  /// afFarRange
        4000  ,  /// afBloomingThr
        80    ,  /// afBloomingCntThr
        1023  ,  /// afBloomingGainThr
        65536 ,  /// afLowlightGainThr
        50    ,  /// afMacroSearchEndThr
        /*
        *
        * afWeight
        *
        */
        {
            1   , 1   , 1   , 1   , 1   ,
            1   , 2   , 2   , 2   , 1   ,
            1   , 2   , 2   , 2   , 1   ,
            1   , 2   , 2   , 2   , 1   ,
            1   , 1   , 1   , 1   , 1   ,
        },
        /*
        *
        * afGammaLut
        *
        */
        {
            0   , 794 , 1135, 1386, 1591, 1768, 1925, 2068, 2199, 2321,
            2435, 2543, 2645, 2743, 2836, 2925, 3011, 3094, 3174, 3251,
            3327, 3399, 3470, 3540, 3607, 3673, 3737, 3800, 3861, 3921,
            3980, 4038, 4095,
        },
        40    ,  /// afMotorZoomInit
        /*
        *
        * afZoomFocusMappingTab
        *
        */
        {
            7     ,  /// validSize
            {
                /// [0]afZoomVal
                /// [1]afNearEndPos
                /// [2]afFarEndPos
                /// [3]afNearMarginPercent
                /// [4]afFarMarginPercent
                /// [5]afMoveStep
                /// 0     1      2      3      4      5
                {20    , {340  , 400  , 30   , 50   , 1    }},
                {30    , {280  , 380  , 30   , 50   , 1    }},
                {40    , {220  , 320  , 30   , 50   , 1    }},
                {50    , {170  , 250  , 30   , 50   , 1    }},
                {60    , {140  , 200  , 45   , 35   , 1    }},
                {70    , {120  , 160  , 45   , 35   , 1    }},
                {80    , {70   , 150  , 45   , 35   , 1    }},
            },
        },
        /*
        *
        * afParamByLux
        *
        */
        {
            2     ,  /// validSize
            {
                /// [0]lux index
                /// [1]default 15,[0:50] Manual, If current average luma is larger than Thr*Ref_Value,  change cnt + 1
                /// [2]default 6,[0:50] Manual, change cnt > ChangeCntThr, judge as scene change
                /// [3]default 3,[0:50] Manual, If current (average luma - previous frame average luma) is smaller then StableThr, stable cnt +1
                /// [4]default 6,[0:50] Manual, stable cnt > StableCntThr, judge as scene stable
                /// [5]default 5,[0:100] Manual, exposure thange > Thr then judge as change and trigger AF
                /// [6]default 1,[0:1] Manual, 1 - scene change reference FV change
                /// [7]default 20,[0:50] Manual, curr FV > Thr*Ref_Value then change cnt + 1
                /// [8]default 2,[0:50] Manual, change cnt > ChangeCntThr, judge as scene change
                /// [9]default 10,[0:50] Manual, curr fv - previous frame fv < StableThr then stable cnt +1
                /// [10]default 5,[0:50] Manual, stable cnt > StableCntThr, judge as scene stable
                /// [11]default 1060,[1000:1500] Manual, fv increase ratio > xxx/1000 judge as fv inc
                /// [12]default 980,[500:1000] Manual, fv decrease ratio < xxx/1000 judge as fv dec
                /// [13]default 86,[50:100] Manual, FVIncDecPeakRatio for find the peak, 100 base
                /// [14]default 86,[50:100] Manual, FVDecDecPeakRatio for find the peak, 100 base
                /// [15]default 100,[10:1000]Manual, lowlight scene could set large ROI to do contrast af, 100 base
                /// 0     1      2      3      4      5      6      7      8      9      10     11     12     13     14     15
                {300   , {15   , 6    , 3    , 6    , 5    , 1    , 20   , 2    , 10   , 5    , 1060 , 980  , 86   , 86   , 100  }},
                {400   , {30   , 6    , 3    , 6    , 5    , 1    , 20   , 2    , 10   , 5    , 1060 , 980  , 92   , 90   , 200  }},
            },
        },
        /*
        *
        * faceAfRoiParam
        *
        */
        {
            1     ,  /// validSize
            {
                /// [0] lux index
                /// [1]default 15,[0:50] Manual, If current average luma is larger than Thr*Ref_Value,  change cnt + 1
                /// [2]default 6,[0:50] Manual, change cnt > ChangeCntThr, judge as scene change
                /// [3]default 3,[0:50] Manual, If current (average luma - previous frame average luma) is smaller then StableThr, stable cnt +1
                /// [4]default 6,[0:50] Manual, stable cnt > StableCntThr, judge as scene stable
                /// [5]default 5,[0:100] Manual, exposure thange > Thr then judge as change and trigger AF
                /// [6]default 1,[0:1] Manual, 1 - scene change reference FV change
                /// [7]default 20,[0:50] Manual, curr FV > Thr*Ref_Value then change cnt + 1
                /// [8]default 2,[0:50] Manual, change cnt > ChangeCntThr, judge as scene change
                /// 0     1      2      3      4      5      6      7      8
                {200   , {130  , 140  , 300  , 60   , 60   , 4    , 8    , 20   }},
            },
        },
    },
},
/*
*
* ispParams
*
*/
{
    /*
    *
    * ispTuneVersion
    *
    */
    {
        0, 1, 0, 46944, /// major, minor, debug, check
    },
    /*
    *
    * gainRange
    *
    */
    {
        65536 ,  /// maxDigitalGain
        4096  ,  /// minDigitalGain
        1024  ,  /// maxRChannelGain
        256   ,  /// minRChannelGain
        1024  ,  /// maxGChannelGain
        256   ,  /// minGChannelGain
        1792  ,  /// maxBChannelGain
        256   ,  /// minBChannelGain
    },
    /*
    *
    * blc
    *
    */
    {
        1     ,  /// blcTuneEnable
        /*
        *
        * blcGainInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]normalBlcR
                /// [2]normalBlcGr
                /// [3]normalBlcGb
                /// [4]normalBlcB
                /// [5]lncBlcL
                /// [6]lncBlcM
                /// [7]lncBlcS
                /// [8]lncBlcVs
                /// [9]dolLBlcR
                /// [10]dolLBlcGr
                /// [11]dolLBlcGb
                /// [12]dolLBlcB
                /// [13]dolMBlcR
                /// [14]dolMBlcGr
                /// [15]dolMBlcGb
                /// [16]dolMBlcB
                /// [17]dolSBlcR
                /// [18]dolSBlcGr
                /// [19]dolSBlcGb
                /// [20]dolSBlcB
                /// 0          1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     17     18     19     20
                {   1 * 1024, {42803, 42767, 42769, 42770, 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    }},
                {   2 * 1024, {42928, 42823, 42840, 42841, 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    }},
                {   4 * 1024, {42895, 42815, 42817, 42815, 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    }},
                {   8 * 1024, {43077, 42955, 42977, 42966, 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    }},
                {  16 * 1024, {41278, 41111, 41172, 41144, 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    }},
                {  32 * 1024, {39681, 39621, 39517, 39621, 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    }},
                {  64 * 1024, {39681, 39621, 39517, 39621, 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    }},
                { 128 * 1024, {39681, 39621, 39517, 39621, 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    }},
                { 256 * 1024, {39681, 39621, 39517, 39621, 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    }},
                { 512 * 1024, {39681, 39621, 39517, 39621, 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 0    }},
            },
        },
    },
    /*
    *
    * lsc
    *
    */
    {
        1     ,  /// lscTuneEnable
        /*
        *
        * lscGainInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]lscStrength
                /// 0          1
                {   1 * 1024, {128  }},
                {   2 * 1024, {256  }},
                {   4 * 1024, {256  }},
                {   8 * 1024, {192  }},
                {  16 * 1024, {128  }},
                {  32 * 1024, {128  }},
                {  64 * 1024, {128  }},
                { 128 * 1024, {64   }},
                { 256 * 1024, {0    }},
                { 512 * 1024, {0    }},
            },
        },
    },
    /*
    *
    * ltm
    *
    */
    {
        1     ,  /// ltmTuneEnable
        20    ,  /// ltmSpatialFilterTime
        6     ,  /// ltmTitHistOffset
        /*
        *
        * ltmRatioInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]ratio
                /// [1]ltmFilterTime
                /// 0     1
                {24576 , {5    }},
                {49152 , {5    }},
                {73728 , {5    }},
                {98304 , {5    }},
                {122880, {5    }},
                {147456, {5    }},
                {172032, {5    }},
                {237568, {10   }},
                {245760, {10   }},
                {270336, {10   }},
            },
        },
        /*
        *
        * ltmGainInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]ltmAmplifyalpha
                /// 0          1
                {   1 * 1024, {1024 }},
                {   2 * 1024, {1024 }},
                {   4 * 1024, {1024 }},
                {   8 * 1024, {1024 }},
                {  16 * 1024, {800  }},
                {  32 * 1024, {800  }},
                {  64 * 1024, {600  }},
                { 128 * 1024, {364  }},
                { 256 * 1024, {64   }},
                { 512 * 1024, {0    }},
            },
        },
        /*
        *
        * ltmLuxInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]lux
                /// [1]ltmMaxGain[0:1024]
                /// [2]ltmAmplifyalpha[0:4095]
                /// [3]ltmDarkPrc[0:1000]
                /// [4]ltmBrightPrc[0:1000]
                /// [5]ltmDarkContrastMin[0:1023]
                /// [6]ltmDarkContrastMax[0:1023]
                /// [7]ltmMinLighten[0:255]
                /// [8]ltmMaxLighten[0:255]
                /// [9]ltmGdgEnable[0:1]
                /// [10]ltmTargetLdr[1:1023]
                /// [11]ltmHiTargetPrc[0:100]
                /// [12]ltmDarkLevel[0:1023]
                /// [13]ltmDarkMinPrc[0:100]
                /// [14]ltmDarkMaxPrc[0:100]
                /// [15]ltmDarkGainRatio[100:1000]
                /// [16]ltmAmplifyalphaMax[0:4095]
                /// [17]ltmContrastLimitThr[0:65535]
                /// [18]ltmContrastLimitStr[0:512]
                /// [19]ltmFwdMappingAlphaMin[0:4095]
                /// [20]ltmFwdMappingAlphaMax[0:4095]
                /// [21]ltmAmplifyStrMin[0:255]
                /// [22]ltmAmplifyStrMax[0:255]
                /// [23]ltmTitLocalContrast[0:1024]
                /// 0     1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     17     18     19     20     21     22     23
                {50    , {256  , 1300 , 100  , 990  , 1    , 85   , 50   , 160  , 0    , 160  , 99   , 5    , 30   , 50   , 100  , 2000 , 4000 , 256  , 0    , 0    , 128  , 128  , 1024 }},
                {100   , {256  , 1300 , 100  , 990  , 1    , 85   , 50   , 160  , 0    , 150  , 99   , 5    , 30   , 50   , 100  , 2000 , 4000 , 256  , 0    , 0    , 128  , 128  , 1024 }},
                {150   , {256  , 1300 , 100  , 990  , 1    , 85   , 50   , 160  , 0    , 140  , 99   , 5    , 30   , 50   , 100  , 2000 , 4000 , 256  , 0    , 0    , 128  , 128  , 1024 }},
                {200   , {228  , 1300 , 100  , 990  , 1    , 85   , 50   , 160  , 0    , 130  , 99   , 5    , 30   , 50   , 100  , 2000 , 4000 , 256  , 0    , 0    , 128  , 128  , 1024 }},
                {250   , {200  , 1300 , 100  , 990  , 1    , 60   , 50   , 160  , 0    , 120  , 99   , 5    , 30   , 50   , 100  , 2000 , 4000 , 256  , 0    , 0    , 128  , 128  , 1024 }},
                {300   , {175  , 1000 , 100  , 990  , 1    , 60   , 50   , 160  , 0    , 100  , 99   , 5    , 30   , 50   , 100  , 2000 , 4000 , 256  , 0    , 0    , 128  , 128  , 1024 }},
                {350   , {125  , 600  , 100  , 990  , 1    , 60   , 50   , 150  , 0    , 90   , 99   , 5    , 30   , 50   , 100  , 2000 , 4000 , 256  , 0    , 0    , 128  , 128  , 1024 }},
                {400   , {100  , 100  , 100  , 990  , 1    , 60   , 50   , 150  , 0    , 80   , 99   , 5    , 30   , 50   , 100  , 1000 , 4000 , 256  , 0    , 0    , 128  , 128  , 1024 }},
                {450   , {100  , 0    , 100  , 990  , 1    , 60   , 50   , 150  , 0    , 70   , 99   , 5    , 30   , 50   , 100  , 600  , 4000 , 256  , 0    , 0    , 128  , 128  , 1024 }},
                {500   , {100  , 0    , 100  , 990  , 1    , 60   , 50   , 150  , 0    , 70   , 99   , 5    , 30   , 50   , 100  , 0    , 4000 , 256  , 0    , 0    , 128  , 128  , 1024 }},
            },
        },
        /*
        *
        * ltmHlSupLuxInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]lux
                /// [1]ltmMaxGain[0:1024]
                /// [2]ltmAmplifyalpha[0:4095]
                /// [3]ltmDarkPrc[0:1000]
                /// [4]ltmBrightPrc[0:1000]
                /// [5]ltmDarkContrastMin[0:1023]
                /// [6]ltmDarkContrastMax[0:1023]
                /// [7]ltmMinLighten[0:255]
                /// [8]ltmMaxLighten[0:255]
                /// [9]ltmGdgEnable[0:1]
                /// [10]ltmTargetLdr[1:1023]
                /// [11]ltmHiTargetPrc[0:100]
                /// [12]ltmDarkLevel[0:1023]
                /// [13]ltmDarkMinPrc[0:100]
                /// [14]ltmDarkMaxPrc[0:100]
                /// [15]ltmDarkGainRatio[100:1000]
                /// [16]ltmAmplifyalphaMax[0:4095]
                /// [17]ltmContrastLimitThr[0:65535]
                /// [18]ltmContrastLimitStr[0:512]
                /// [19]ltmFwdMappingAlphaMin[0:4095]
                /// [20]ltmFwdMappingAlphaMax[0:4095]
                /// [21]ltmAmplifyStrMin[0:255]
                /// [22]ltmAmplifyStrMax[0:255]
                /// [23]ltmTitLocalContrast[0:1024]
                /// 0     1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     17     18     19     20     21     22     23
                {50    , {256  , 2048 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 160  , 99   , 5    , 30   , 50   , 100  , 2048 , 4000 , 256  , 0    , 0    , 128  , 128  , 1024 }},
                {100   , {256  , 2048 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 150  , 99   , 5    , 30   , 50   , 100  , 2048 , 4000 , 256  , 0    , 0    , 128  , 128  , 1024 }},
                {150   , {256  , 3072 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 140  , 99   , 5    , 30   , 50   , 100  , 3072 , 4000 , 256  , 0    , 0    , 128  , 128  , 1024 }},
                {200   , {228  , 3072 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 130  , 99   , 5    , 30   , 50   , 100  , 3072 , 4000 , 256  , 0    , 0    , 128  , 128  , 1024 }},
                {250   , {176  , 3072 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 120  , 99   , 5    , 30   , 50   , 100  , 3072 , 4000 , 256  , 0    , 0    , 128  , 128  , 1024 }},
                {300   , {100  , 1750 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 100  , 99   , 5    , 30   , 50   , 100  , 1750 , 4000 , 256  , 0    , 0    , 128  , 128  , 1024 }},
                {350   , {100  , 875  , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 90   , 99   , 5    , 30   , 50   , 100  , 875  , 4000 , 256  , 0    , 0    , 128  , 128  , 1024 }},
                {400   , {100  , 0    , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 80   , 99   , 5    , 30   , 50   , 100  , 0    , 4000 , 256  , 0    , 0    , 128  , 128  , 1024 }},
                {450   , {100  , 0    , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 70   , 99   , 5    , 30   , 50   , 100  , 0    , 4000 , 256  , 0    , 0    , 128  , 128  , 1024 }},
                {500   , {100  , 0    , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 70   , 99   , 5    , 30   , 50   , 100  , 0    , 4000 , 256  , 0    , 0    , 128  , 128  , 1024 }},
            },
        },
        /*
        *
        * ltmCurveInterpTab
        *
        */
        {
            2     ,  /// validSize
            {
                {
                    30    ,  /// region
                    {
                        {
                            0   , 14045, 21659, 26541, 30293, 33302, 35786, 37881, 39677, 41239, 42612, 43831, 44921, 45904, 46795, 47610,
                            48356, 49045, 49683, 50276, 50830, 51350, 51839, 52301, 52739, 53156, 53553, 53933, 54298, 54650, 54989, 55319,
                            55639, 55951, 56256, 56555, 56849, 57139, 57426, 57710, 57992, 58273, 58553, 58833, 59114, 59396, 59679, 59965,
                            60253, 60544, 60839, 61138, 61441, 61749, 62062, 62380, 62704, 63035, 63372, 63716, 64067, 64425, 64792, 65167,
                            65535,
                        },
                    },
                },
                {
                    50    ,  /// region
                    {
                        {
                            0   , 24045, 30659, 34541, 37293, 38502, 39786, 40681, 41577, 42439, 43512, 44431, 45221, 46084, 46885, 47610,
                            48356, 49045, 49683, 50276, 50830, 51350, 51839, 52301, 52739, 53156, 53553, 53933, 54298, 54650, 54989, 55319,
                            55639, 55951, 56256, 56555, 56849, 57139, 57426, 57710, 57992, 58273, 58553, 58833, 59114, 59396, 59679, 59965,
                            60253, 60544, 60839, 61138, 61441, 61749, 62062, 62380, 62704, 63035, 63372, 63716, 64067, 64425, 64792, 65167,
                            65535,
                        },
                    },
                },
            },
        },
    },
    /*
    *
    * ccm
    *
    */
    {
        1     ,  /// ccmTuneEnable
        /*
        *
        * ccmGainInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]ccmSaturation
                /// 0          1
                {   1 * 1024, {280  }},
                {   2 * 1024, {280  }},
                {   4 * 1024, {280  }},
                {   8 * 1024, {280  }},
                {  16 * 1024, {256  }},
                {  32 * 1024, {256  }},
                {  64 * 1024, {240  }},
                { 128 * 1024, {240  }},
                { 256 * 1024, {220  }},
                { 512 * 1024, {160  }},
            },
        },
        /*
        *
        * ccmLuxInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]lux
                /// [1]ccmSaturation
                /// 0     1
                {50    , {350  }},
                {100   , {350  }},
                {150   , {300  }},
                {200   , {280  }},
                {250   , {280  }},
                {300   , {280  }},
                {350   , {270  }},
                {400   , {270  }},
                {450   , {270  }},
                {500   , {270  }},
            },
        },
    },
    /*
    *
    * ggc
    *
    */
    {
        1     ,  /// ggcTuneEnable
        0     ,  /// ggcBasicSelection
    },
    /*
    *
    * dgn
    *
    */
    {
        1     ,  /// dgnTuneEnable
        /*
        *
        * dgnGainSwitchTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]blcSthEn
                /// 0          1
                {   1 * 1024, {0    }},
                {   2 * 1024, {0    }},
                {   4 * 1024, {0    }},
                {   8 * 1024, {0    }},
                {  16 * 1024, {0    }},
                {  32 * 1024, {0    }},
                {  64 * 1024, {0    }},
                { 128 * 1024, {0    }},
                { 256 * 1024, {0    }},
                { 512 * 1024, {0    }},
            },
        },
        /*
        *
        * dgnGainInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]blcSthUpRatio
                /// 0          1
                {   1 * 1024, {0    }},
                {   2 * 1024, {0    }},
                {   4 * 1024, {0    }},
                {   8 * 1024, {0    }},
                {  16 * 1024, {0    }},
                {  32 * 1024, {0    }},
                {  64 * 1024, {0    }},
                { 128 * 1024, {0    }},
                { 256 * 1024, {0    }},
                { 512 * 1024, {0    }},
            },
        },
    },
    /*
    *
    * dpc
    *
    */
    {
        1     ,  /// dpcTuneEnable
        /*
        *
        * dpcGainSwitchTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]dpcByp
                /// [2]dpcGeEn
                /// [3]dpcTitMultNrEn
                /// 0          1      2      3
                {   1 * 1024, {0    , 0    , 0    }},
                {   2 * 1024, {0    , 0    , 0    }},
                {   4 * 1024, {0    , 0    , 0    }},
                {   8 * 1024, {0    , 0    , 0    }},
                {  16 * 1024, {0    , 0    , 0    }},
                {  32 * 1024, {0    , 0    , 0    }},
                {  64 * 1024, {0    , 0    , 0    }},
                { 128 * 1024, {0    , 0    , 0    }},
                { 256 * 1024, {0    , 0    , 0    }},
                { 512 * 1024, {0    , 0    , 0    }},
            },
        },
        /*
        *
        * dpcGainInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]dpcIntplAlpha
                /// [2]dpcDivSlp
                /// [3]dpcDivThr
                /// [4]dpcStdevMaskThr
                /// [5]dpcDpDisThr
                /// [6]dpcCurMedThr
                /// [7]dpcGeStr
                /// [8]dpcGeThr
                /// [9]dpcGeSen
                /// [10]dpcGeSlope
                /// [11]dpcNpGain
                /// [12]dpcNpOffset
                /// [13]dpcTitMultThrA
                /// [14]dpcTitMultThrB
                /// [15]dpcTitBlcR
                /// [16]dpcTitBlcGr
                /// [17]dpcTitBlcGb
                /// [18]dpcTitBlcB
                /// 0          1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     17     18
                {   1 * 1024, {0    , 1    , 0    , 10   , 200  , 16   , 100  , 0    , 256  , 256  , 64   , 0    , 5    , 0    , 240  , 240  , 240  , 240  }},
                {   2 * 1024, {0    , 1    , 0    , 10   , 200  , 16   , 100  , 0    , 256  , 256  , 64   , 0    , 5    , 0    , 240  , 240  , 240  , 240  }},
                {   4 * 1024, {0    , 1    , 0    , 10   , 200  , 16   , 100  , 0    , 256  , 256  , 64   , 0    , 5    , 0    , 240  , 240  , 240  , 240  }},
                {   8 * 1024, {0    , 1    , 0    , 10   , 200  , 16   , 100  , 0    , 256  , 256  , 64   , 0    , 5    , 0    , 240  , 240  , 240  , 240  }},
                {  16 * 1024, {0    , 1    , 0    , 10   , 200  , 16   , 100  , 0    , 256  , 256  , 64   , 0    , 5    , 0    , 240  , 240  , 240  , 240  }},
                {  32 * 1024, {0    , 2    , 0    , 10   , 300  , 16   , 100  , 0    , 256  , 256  , 64   , 0    , 5    , 0    , 240  , 240  , 240  , 240  }},
                {  64 * 1024, {0    , 4    , 0    , 10   , 300  , 16   , 100  , 0    , 256  , 256  , 64   , 0    , 5    , 0    , 240  , 240  , 240  , 240  }},
                { 128 * 1024, {0    , 4    , 0    , 10   , 400  , 16   , 100  , 0    , 256  , 256  , 64   , 0    , 5    , 0    , 240  , 240  , 240  , 240  }},
                { 256 * 1024, {0    , 4    , 0    , 10   , 500  , 16   , 100  , 0    , 256  , 256  , 64   , 0    , 5    , 0    , 240  , 240  , 240  , 240  }},
                { 512 * 1024, {0    , 4    , 0    , 10   , 500  , 16   , 100  , 0    , 256  , 256  , 64   , 0    , 5    , 0    , 240  , 240  , 240  , 240  }},
            },
        },
    },
    /*
    *
    * rnr
    *
    */
    {
        1     ,  /// rnrTuneEnable
        /*
        *
        * rnrGainSwitchTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]rnrNpOffsetMid
                /// [2]rnrNpOffsetEnd
                /// [3]rnrNlmFreqLayerMid
                /// [4]rnrNlmFreqLayerEnd
                /// [5]rnrMontionMid
                /// [6]rnrMontionEnd
                /// 0          1      2      3      4      5      6
                {   1 * 1024, {10   , 10   , 4    , 4    , 6    , 6    }},
                {   2 * 1024, {10   , 10   , 4    , 4    , 6    , 6    }},
                {   4 * 1024, {10   , 10   , 4    , 4    , 6    , 6    }},
                {   8 * 1024, {10   , 10   , 4    , 4    , 6    , 6    }},
                {  16 * 1024, {10   , 10   , 4    , 4    , 6    , 6    }},
                {  32 * 1024, {10   , 10   , 4    , 4    , 6    , 6    }},
                {  64 * 1024, {10   , 10   , 4    , 4    , 6    , 6    }},
                { 128 * 1024, {10   , 10   , 4    , 4    , 6    , 6    }},
                { 256 * 1024, {10   , 10   , 4    , 4    , 6    , 6    }},
                { 512 * 1024, {10   , 10   , 4    , 4    , 6    , 6    }},
            },
        },
        /*
        *
        * rnrGainInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]rnrNpGain
                /// [2]rnrNpLumBlc
                /// [3]rnrNpOffsetOffset
                /// [4]rnrNpOffsetR1
                /// [5]rnrNpOffsetR2
                /// [6]rnrNpOffsetR3
                /// [7]rnrFreNoiseR
                /// [8]rnrNlmFreqLayerOffset
                /// [9]rnrNlmFreqLayerR1
                /// [10]rnrNlmFreqLayerR2
                /// [11]rnrNlmFreqLayerR3
                /// [12]rnrMontionOffset
                /// [13]rnrMotNr3dR1
                /// [14]rnrMotNr3dR2
                /// [15]rnrMotNr3dR3
                /// [16]rnrMotNr2dR1
                /// [17]rnrMotNr2dR2
                /// [18]rnrMotNr2dR3
                /// [19]rnrRegModuMvBlendStr
                /// [20]rnrGlbStr
                /// [21]rnrPryNoiseR0
                /// [22]rnrPryNoiseR1
                /// [23]rnrSqrtRatio
                /// [24]rnrNlmTable
                /// [25]rnrDirPyrR1
                /// [26]rnrDirPyrR2
                /// [27]rnrDirPyrR3
                /// [28]rnrStableMotStage1
                /// [29]rnrStableMotSlope1
                /// [30]rnrStableMotStage2
                /// [31]rnrStableMotSlope2
                /// [32]rnrStableMotR1
                /// [33]rnrStableMotR2
                /// [34]rnrStableMotR3
                /// 0          1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     17     18     19     20     21     22     23     24     25     26     27     28     29     30     31     32     33     34
                {   1 * 1024, {256  , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 1024 , 1024 , 1024 , 0    , 245  , 100  , 0    , 30   , 195  , 256  , 240  , 180  , 256  , 85   , 1    , 8    , 256  , 256  , 256  , 0    , 6    , 0    , 6    , 0    , 0    , 0    }},
                {   2 * 1024, {256  , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 1024 , 1024 , 1024 , 0    , 245  , 100  , 0    , 30   , 195  , 256  , 240  , 180  , 256  , 85   , 1    , 8    , 256  , 256  , 256  , 0    , 6    , 0    , 6    , 0    , 0    , 0    }},
                {   4 * 1024, {256  , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 1024 , 1024 , 1024 , 0    , 245  , 100  , 0    , 30   , 195  , 256  , 240  , 180  , 256  , 85   , 1    , 8    , 256  , 256  , 256  , 0    , 6    , 0    , 6    , 0    , 0    , 0    }},
                {   8 * 1024, {256  , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 1024 , 1024 , 1024 , 0    , 245  , 100  , 0    , 30   , 195  , 256  , 240  , 180  , 256  , 85   , 1    , 8    , 256  , 256  , 256  , 0    , 6    , 0    , 6    , 0    , 0    , 0    }},
                {  16 * 1024, {256  , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 1024 , 1024 , 1024 , 0    , 245  , 100  , 0    , 30   , 195  , 256  , 240  , 180  , 256  , 85   , 1    , 8    , 256  , 256  , 256  , 0    , 6    , 0    , 6    , 0    , 0    , 0    }},
                {  32 * 1024, {256  , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 1024 , 1024 , 1024 , 0    , 245  , 100  , 0    , 30   , 195  , 256  , 240  , 180  , 256  , 85   , 1    , 8    , 256  , 256  , 256  , 0    , 6    , 0    , 6    , 0    , 0    , 0    }},
                {  64 * 1024, {256  , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 1024 , 1024 , 1024 , 0    , 245  , 100  , 0    , 30   , 195  , 256  , 240  , 180  , 256  , 85   , 1    , 8    , 256  , 256  , 256  , 0    , 6    , 0    , 6    , 0    , 0    , 0    }},
                { 128 * 1024, {256  , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 1024 , 1024 , 1024 , 0    , 245  , 100  , 0    , 30   , 195  , 256  , 240  , 180  , 256  , 85   , 1    , 8    , 256  , 256  , 256  , 0    , 6    , 0    , 6    , 0    , 0    , 0    }},
                { 256 * 1024, {256  , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 1024 , 1024 , 1024 , 0    , 245  , 100  , 0    , 30   , 195  , 256  , 240  , 180  , 256  , 85   , 1    , 8    , 256  , 256  , 256  , 0    , 6    , 0    , 6    , 0    , 0    , 0    }},
                { 512 * 1024, {256  , 0    , 0    , 0    , 0    , 0    , 0    , 0    , 1024 , 1024 , 1024 , 0    , 245  , 100  , 0    , 30   , 195  , 256  , 240  , 180  , 256  , 85   , 1    , 8    , 256  , 256  , 256  , 0    , 6    , 0    , 6    , 0    , 0    , 0    }},
            },
        },
        /*
        *
        * rnrRadialLutTab
        *
        */
        {
            10    ,  /// validSize
            {
                {
                    1024  ,  /// region
                    {
                        {
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 ,
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 212 , 214 , 216 , 218 , 220 , 225 , 230 , 235 , 240 ,
                            255 ,
                        },
                    },
                },
                {
                    2048  ,  /// region
                    {
                        {
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 ,
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 212 , 214 , 216 , 218 , 220 , 225 , 230 , 235 , 240 ,
                            255 ,
                        },
                    },
                },
                {
                    4096  ,  /// region
                    {
                        {
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 ,
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 212 , 214 , 216 , 218 , 220 , 225 , 230 , 235 , 240 ,
                            255 ,
                        },
                    },
                },
                {
                    8192  ,  /// region
                    {
                        {
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 ,
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 212 , 214 , 216 , 218 , 220 , 225 , 230 , 235 , 240 ,
                            255 ,
                        },
                    },
                },
                {
                    16384 ,  /// region
                    {
                        {
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 ,
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 212 , 214 , 216 , 218 , 220 , 225 , 230 , 235 , 240 ,
                            255 ,
                        },
                    },
                },
                {
                    32768 ,  /// region
                    {
                        {
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 ,
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 212 , 214 , 216 , 218 , 220 , 225 , 230 , 235 , 240 ,
                            255 ,
                        },
                    },
                },
                {
                    65536 ,  /// region
                    {
                        {
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 ,
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 212 , 214 , 216 , 218 , 220 , 225 , 230 , 235 , 240 ,
                            255 ,
                        },
                    },
                },
                {
                    131072,  /// region
                    {
                        {
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 ,
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 212 , 214 , 216 , 218 , 220 , 225 , 230 , 235 , 240 ,
                            255 ,
                        },
                    },
                },
                {
                    262144,  /// region
                    {
                        {
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 ,
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 212 , 214 , 216 , 218 , 220 , 225 , 230 , 235 , 240 ,
                            255 ,
                        },
                    },
                },
                {
                    524288,  /// region
                    {
                        {
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 ,
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 212 , 214 , 216 , 218 , 220 , 225 , 230 , 235 , 240 ,
                            255 ,
                        },
                    },
                },
            },
        },
    },
    /*
    *
    * np
    *
    */
    {
        1     ,  /// npTuneEnable
        128   ,  /// npLGain
        182   ,  /// npMGain
        0     ,  /// npTransGain
        4     ,  /// npFitOrder
        /*
        *
        * npLutTab
        *
        */
        {
            10    ,  /// validSize
            {
                {
                    1024  ,  /// region
                    {
                        {
                            2   , 3   , 3   , 4   , 4   , 5   , 5   , 5   , 5   , 6   , 6   , 6   , 7   , 7   , 7   , 7   ,
                            7   , 8   , 8   , 8   , 8   , 8   , 9   , 9   , 9   , 9   , 9   , 9   , 10  , 10  , 10  , 10  ,
                            10  , 10  , 10  , 11  , 11  , 11  , 11  , 11  , 11  , 11  , 12  , 12  , 12  , 12  , 12  , 12  ,
                            12  , 12  , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 14  , 14  , 14  , 14  , 14  ,
                            14  , 14  , 14  , 14  , 14  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 16  ,
                            16  , 16  , 16  , 16  , 16  , 16  , 16  , 16  , 16  , 16  , 17  , 17  , 17  , 17  , 17  , 17  ,
                            17  , 17  , 17  , 17  , 17  , 17  , 18  , 18  , 18  , 18  , 18  , 18  , 18  , 18  , 18  , 18  ,
                            18  , 18  , 19  , 19  , 19  , 19  , 19  , 19  , 19  , 19  , 19  , 19  , 19  , 19  , 19  , 20  ,
                        },
                    },
                },
                {
                    2048  ,  /// region
                    {
                        {
                            2   , 3   , 4   , 5   , 5   , 6   , 6   , 7   , 7   , 8   , 8   , 8   , 9   , 9   , 9   , 10  ,
                            10  , 10  , 11  , 11  , 11  , 11  , 12  , 12  , 12  , 12  , 13  , 13  , 13  , 13  , 14  , 14  ,
                            14  , 14  , 14  , 15  , 15  , 15  , 15  , 15  , 16  , 16  , 16  , 16  , 16  , 17  , 17  , 17  ,
                            17  , 17  , 17  , 18  , 18  , 18  , 18  , 18  , 18  , 19  , 19  , 19  , 19  , 19  , 19  , 20  ,
                            20  , 20  , 20  , 20  , 20  , 20  , 21  , 21  , 21  , 21  , 21  , 21  , 21  , 22  , 22  , 22  ,
                            22  , 22  , 22  , 22  , 22  , 23  , 23  , 23  , 23  , 23  , 23  , 23  , 23  , 24  , 24  , 24  ,
                            24  , 24  , 24  , 24  , 24  , 25  , 25  , 25  , 25  , 25  , 25  , 25  , 25  , 26  , 26  , 26  ,
                            26  , 26  , 26  , 26  , 26  , 26  , 27  , 27  , 27  , 27  , 27  , 27  , 27  , 27  , 27  , 27  ,
                        },
                    },
                },
                {
                    4096  ,  /// region
                    {
                        {
                            3   , 4   , 6   , 7   , 7   , 8   , 9   , 10  , 10  , 11  , 11  , 12  , 12  , 13  , 13  , 14  ,
                            14  , 14  , 15  , 15  , 16  , 16  , 16  , 17  , 17  , 17  , 18  , 18  , 18  , 19  , 19  , 19  ,
                            19  , 20  , 20  , 20  , 21  , 21  , 21  , 21  , 22  , 22  , 22  , 22  , 23  , 23  , 23  , 23  ,
                            24  , 24  , 24  , 24  , 25  , 25  , 25  , 25  , 26  , 26  , 26  , 26  , 26  , 27  , 27  , 27  ,
                            27  , 27  , 28  , 28  , 28  , 28  , 28  , 29  , 29  , 29  , 29  , 29  , 30  , 30  , 30  , 30  ,
                            30  , 31  , 31  , 31  , 31  , 31  , 31  , 32  , 32  , 32  , 32  , 32  , 33  , 33  , 33  , 33  ,
                            33  , 33  , 34  , 34  , 34  , 34  , 34  , 34  , 35  , 35  , 35  , 35  , 35  , 35  , 36  , 36  ,
                            36  , 36  , 36  , 36  , 36  , 37  , 37  , 37  , 37  , 37  , 37  , 38  , 38  , 38  , 38  , 38  ,
                        },
                    },
                },
                {
                    8192  ,  /// region
                    {
                        {
                            1   , 4   , 6   , 8   , 9   , 10  , 11  , 13  , 14  , 15  , 15  , 16  , 17  , 18  , 19  , 19  ,
                            20  , 21  , 22  , 22  , 23  , 23  , 24  , 25  , 25  , 26  , 26  , 27  , 27  , 28  , 29  , 29  ,
                            30  , 30  , 31  , 31  , 32  , 32  , 32  , 33  , 33  , 34  , 34  , 35  , 35  , 36  , 36  , 36  ,
                            37  , 37  , 38  , 38  , 38  , 39  , 39  , 40  , 40  , 40  , 41  , 41  , 41  , 42  , 42  , 43  ,
                            43  , 43  , 44  , 44  , 44  , 45  , 45  , 45  , 46  , 46  , 46  , 47  , 47  , 47  , 48  , 48  ,
                            48  , 49  , 49  , 49  , 50  , 50  , 50  , 51  , 51  , 51  , 51  , 52  , 52  , 52  , 53  , 53  ,
                            53  , 54  , 54  , 54  , 54  , 55  , 55  , 55  , 56  , 56  , 56  , 56  , 57  , 57  , 57  , 57  ,
                            58  , 58  , 58  , 59  , 59  , 59  , 59  , 60  , 60  , 60  , 60  , 61  , 61  , 61  , 61  , 62  ,
                        },
                    },
                },
                {
                    16384 ,  /// region
                    {
                        {
                            5   , 9   , 11  , 13  , 15  , 17  , 18  , 19  , 20  , 22  , 23  , 24  , 25  , 26  , 27  , 27  ,
                            28  , 29  , 30  , 31  , 32  , 32  , 33  , 34  , 34  , 35  , 36  , 36  , 37  , 38  , 38  , 39  ,
                            40  , 40  , 41  , 41  , 42  , 43  , 43  , 44  , 44  , 45  , 45  , 46  , 46  , 47  , 47  , 48  ,
                            48  , 49  , 49  , 50  , 50  , 51  , 51  , 52  , 52  , 53  , 53  , 53  , 54  , 54  , 55  , 55  ,
                            56  , 56  , 56  , 57  , 57  , 58  , 58  , 59  , 59  , 59  , 60  , 60  , 61  , 61  , 61  , 62  ,
                            62  , 62  , 63  , 63  , 64  , 64  , 64  , 65  , 65  , 65  , 66  , 66  , 67  , 67  , 67  , 68  ,
                            68  , 68  , 69  , 69  , 69  , 70  , 70  , 70  , 71  , 71  , 71  , 72  , 72  , 72  , 73  , 73  ,
                            73  , 74  , 74  , 74  , 75  , 75  , 75  , 76  , 76  , 76  , 77  , 77  , 77  , 77  , 78  , 78  ,
                        },
                    },
                },
                {
                    32768 ,  /// region
                    {
                        {
                            7   , 12  , 16  , 18  , 21  , 23  , 25  , 27  , 28  , 30  , 32  , 33  , 34  , 36  , 37  , 38  ,
                            40  , 41  , 42  , 43  , 44  , 45  , 46  , 47  , 48  , 49  , 50  , 51  , 52  , 53  , 54  , 55  ,
                            56  , 56  , 57  , 58  , 59  , 60  , 60  , 61  , 62  , 63  , 63  , 64  , 65  , 66  , 66  , 67  ,
                            68  , 68  , 69  , 70  , 71  , 71  , 72  , 72  , 73  , 74  , 74  , 75  , 76  , 76  , 77  , 78  ,
                            78  , 79  , 79  , 80  , 81  , 81  , 82  , 82  , 83  , 83  , 84  , 85  , 85  , 86  , 86  , 87  ,
                            87  , 88  , 88  , 89  , 89  , 90  , 90  , 91  , 91  , 92  , 93  , 93  , 94  , 94  , 95  , 95  ,
                            96  , 96  , 97  , 97  , 97  , 98  , 98  , 99  , 99  , 100 , 100 , 101 , 101 , 102 , 102 , 103 ,
                            103 , 104 , 104 , 104 , 105 , 105 , 106 , 106 , 107 , 107 , 108 , 108 , 108 , 109 , 109 , 110 ,
                        },
                    },
                },
                {
                    65536 ,  /// region
                    {
                        {
                            7   , 12  , 16  , 18  , 21  , 23  , 25  , 27  , 28  , 30  , 32  , 33  , 34  , 36  , 37  , 38  ,
                            40  , 41  , 42  , 43  , 44  , 45  , 46  , 47  , 48  , 49  , 50  , 51  , 52  , 53  , 54  , 55  ,
                            56  , 56  , 57  , 58  , 59  , 60  , 60  , 61  , 62  , 63  , 63  , 64  , 65  , 66  , 66  , 67  ,
                            68  , 68  , 69  , 70  , 71  , 71  , 72  , 72  , 73  , 74  , 74  , 75  , 76  , 76  , 77  , 78  ,
                            78  , 79  , 79  , 80  , 81  , 81  , 82  , 82  , 83  , 83  , 84  , 85  , 85  , 86  , 86  , 87  ,
                            87  , 88  , 88  , 89  , 89  , 90  , 90  , 91  , 91  , 92  , 93  , 93  , 94  , 94  , 95  , 95  ,
                            96  , 96  , 97  , 97  , 97  , 98  , 98  , 99  , 99  , 100 , 100 , 101 , 101 , 102 , 102 , 103 ,
                            103 , 104 , 104 , 104 , 105 , 105 , 106 , 106 , 107 , 107 , 108 , 108 , 108 , 109 , 109 , 110 ,
                        },
                    },
                },
                {
                    131072,  /// region
                    {
                        {
                            7   , 12  , 16  , 18  , 21  , 23  , 25  , 27  , 28  , 30  , 32  , 33  , 34  , 36  , 37  , 38  ,
                            40  , 41  , 42  , 43  , 44  , 45  , 46  , 47  , 48  , 49  , 50  , 51  , 52  , 53  , 54  , 55  ,
                            56  , 56  , 57  , 58  , 59  , 60  , 60  , 61  , 62  , 63  , 63  , 64  , 65  , 66  , 66  , 67  ,
                            68  , 68  , 69  , 70  , 71  , 71  , 72  , 72  , 73  , 74  , 74  , 75  , 76  , 76  , 77  , 78  ,
                            78  , 79  , 79  , 80  , 81  , 81  , 82  , 82  , 83  , 83  , 84  , 85  , 85  , 86  , 86  , 87  ,
                            87  , 88  , 88  , 89  , 89  , 90  , 90  , 91  , 91  , 92  , 93  , 93  , 94  , 94  , 95  , 95  ,
                            96  , 96  , 97  , 97  , 97  , 98  , 98  , 99  , 99  , 100 , 100 , 101 , 101 , 102 , 102 , 103 ,
                            103 , 104 , 104 , 104 , 105 , 105 , 106 , 106 , 107 , 107 , 108 , 108 , 108 , 109 , 109 , 110 ,
                        },
                    },
                },
                {
                    262144,  /// region
                    {
                        {
                            7   , 12  , 16  , 18  , 21  , 23  , 25  , 27  , 28  , 30  , 32  , 33  , 34  , 36  , 37  , 38  ,
                            40  , 41  , 42  , 43  , 44  , 45  , 46  , 47  , 48  , 49  , 50  , 51  , 52  , 53  , 54  , 55  ,
                            56  , 56  , 57  , 58  , 59  , 60  , 60  , 61  , 62  , 63  , 63  , 64  , 65  , 66  , 66  , 67  ,
                            68  , 68  , 69  , 70  , 71  , 71  , 72  , 72  , 73  , 74  , 74  , 75  , 76  , 76  , 77  , 78  ,
                            78  , 79  , 79  , 80  , 81  , 81  , 82  , 82  , 83  , 83  , 84  , 85  , 85  , 86  , 86  , 87  ,
                            87  , 88  , 88  , 89  , 89  , 90  , 90  , 91  , 91  , 92  , 93  , 93  , 94  , 94  , 95  , 95  ,
                            96  , 96  , 97  , 97  , 97  , 98  , 98  , 99  , 99  , 100 , 100 , 101 , 101 , 102 , 102 , 103 ,
                            103 , 104 , 104 , 104 , 105 , 105 , 106 , 106 , 107 , 107 , 108 , 108 , 108 , 109 , 109 , 110 ,
                        },
                    },
                },
                {
                    524288,  /// region
                    {
                        {
                            7   , 12  , 16  , 18  , 21  , 23  , 25  , 27  , 28  , 30  , 32  , 33  , 34  , 36  , 37  , 38  ,
                            40  , 41  , 42  , 43  , 44  , 45  , 46  , 47  , 48  , 49  , 50  , 51  , 52  , 53  , 54  , 55  ,
                            56  , 56  , 57  , 58  , 59  , 60  , 60  , 61  , 62  , 63  , 63  , 64  , 65  , 66  , 66  , 67  ,
                            68  , 68  , 69  , 70  , 71  , 71  , 72  , 72  , 73  , 74  , 74  , 75  , 76  , 76  , 77  , 78  ,
                            78  , 79  , 79  , 80  , 81  , 81  , 82  , 82  , 83  , 83  , 84  , 85  , 85  , 86  , 86  , 87  ,
                            87  , 88  , 88  , 89  , 89  , 90  , 90  , 91  , 91  , 92  , 93  , 93  , 94  , 94  , 95  , 95  ,
                            96  , 96  , 97  , 97  , 97  , 98  , 98  , 99  , 99  , 100 , 100 , 101 , 101 , 102 , 102 , 103 ,
                            103 , 104 , 104 , 104 , 105 , 105 , 106 , 106 , 107 , 107 , 108 , 108 , 108 , 109 , 109 , 110 ,
                        },
                    },
                },
            },
        },
    },
    /*
    *
    * dms
    *
    */
    {
        1     ,  /// dmsTuneEnable
        0     ,  /// dmsSmallaliasDetThresh
        1     ,  /// dmsBdOption
        75    ,  /// dmsDuShOffset
        /*
        *
        * dmsGainSwitchTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]dmsNpOffAbs
                /// [2]dmsSharpenEn
                /// [3]dmsHfDetailEnhEn
                /// 0          1      2      3
                {   1 * 1024, {1    , 1    , 1    }},
                {   2 * 1024, {1    , 1    , 1    }},
                {   4 * 1024, {1    , 1    , 1    }},
                {   8 * 1024, {1    , 1    , 1    }},
                {  16 * 1024, {1    , 1    , 1    }},
                {  32 * 1024, {1    , 1    , 1    }},
                {  64 * 1024, {1    , 1    , 1    }},
                { 128 * 1024, {1    , 1    , 1    }},
                { 256 * 1024, {1    , 0    , 0    }},
                { 512 * 1024, {1    , 0    , 0    }},
            },
        },
        /*
        *
        * dmsGainInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]dmsNpOff
                /// [2]dmsGreySmalliasSlp
                /// [3]dmsGreenUseRbThr
                /// [4]dmsGreenUseRbSlp
                /// [5]dmsDxSlp
                /// [6]dmsDxThr
                /// [7]dmsDpSlp
                /// [8]dmsDpThr
                /// [9]dmsDuSlp
                /// [10]dmsDuThr
                /// [11]dmsDuShSlp
                /// [12]dmsDuShThresh
                /// [13]dmsDpxSlp
                /// [14]dmsDpxThr
                /// [15]dmsFcSlp
                /// [16]dmsAliasThresh
                /// [17]dmsAliasSlope
                /// [18]dmsBldaliasThr
                /// [19]dmsBldaliasSlp
                /// [20]dmsSatThr
                /// [21]dmsSatSlp
                /// [22]dmsSharpenStr
                /// [23]dmsHfDetailEnhStr
                /// [24]dmsFcrateStr
                /// [25]dmsDxDetailThd1
                /// [26]dmsDxDetailThd2
                /// [27]dmsFcFiltSize
                /// [28]dmsTitDitherThrMin
                /// [29]dmsTitDitherThrMax
                /// [30]dmsTitDitherSlope
                /// 0          1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     17     18     19     20     21     22     23     24     25     26     27     28     29     30
                {   1 * 1024, {40   , 5000 , 253  , 1350 , 235  , 50   , 255  , 50   , 125  , 0    , 64   , 50   , 160  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 6000 , 96   , 0    , 196  , 384  , 128  , 100  , 4095 , 2000 }},
                {   2 * 1024, {40   , 5000 , 253  , 1350 , 235  , 50   , 255  , 50   , 122  , 0    , 64   , 50   , 160  , 50   , 255  , 90   , 140  , 435  , 201  , 369  , 178  , 6000 , 96   , 0    , 196  , 384  , 128  , 100  , 4095 , 2000 }},
                {   4 * 1024, {40   , 5000 , 253  , 1350 , 235  , 50   , 235  , 50   , 100  , 0    , 64   , 50   , 160  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 6000 , 96   , 0    , 196  , 384  , 128  , 100  , 4095 , 2000 }},
                {   8 * 1024, {40   , 5000 , 253  , 1350 , 235  , 50   , 235  , 50   , 100  , 0    , 64   , 50   , 160  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 6000 , 96   , 0    , 196  , 384  , 128  , 100  , 4095 , 2000 }},
                {  16 * 1024, {40   , 5000 , 253  , 1350 , 235  , 50   , 235  , 50   , 100  , 0    , 64   , 50   , 160  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 6000 , 64   , 0    , 196  , 384  , 128  , 100  , 4095 , 2000 }},
                {  32 * 1024, {20   , 5000 , 253  , 1350 , 235  , 40   , 235  , 50   , 100  , 0    , 48   , 50   , 180  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 4096 , 64   , 0    , 196  , 384  , 128  , 100  , 4095 , 2000 }},
                {  64 * 1024, {20   , 5000 , 253  , 1350 , 235  , 25   , 235  , 50   , 100  , 0    , 32   , 50   , 180  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 1000 , 64   , 0    , 196  , 384  , 128  , 100  , 4095 , 2000 }},
                { 128 * 1024, {20   , 5000 , 253  , 1350 , 140  , 25   , 180  , 50   , 100  , 0    , 32   , 50   , 180  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 500  , 32   , 0    , 196  , 384  , 128  , 100  , 4095 , 2000 }},
                { 256 * 1024, {10   , 2400 , 253  , 1350 , 140  , 0    , 180  , 50   , 80   , 0    , 32   , 50   , 120  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 200  , 32   , 0    , 196  , 384  , 128  , 100  , 4095 , 2000 }},
                { 512 * 1024, {10   , 2400 , 253  , 1350 , 140  , 0    , 180  , 50   , 80   , 0    , 32   , 50   , 120  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 200  , 32   , 0    , 196  , 384  , 128  , 100  , 4095 , 2000 }},
            },
        },
        /*
        *
        * dmsNpLutTab
        *
        */
        {
            10    ,  /// validSize
            {
                {
                    1024  ,  /// region
                    {
                        {
                            2   , 3   , 3   , 4   , 4   , 5   , 5   , 5   , 5   , 6   , 6   , 6   , 7   , 7   , 7   , 7   ,
                            7   , 8   , 8   , 8   , 8   , 8   , 9   , 9   , 9   , 9   , 9   , 9   , 10  , 10  , 10  , 10  ,
                            10  , 10  , 10  , 11  , 11  , 11  , 11  , 11  , 11  , 11  , 12  , 12  , 12  , 12  , 12  , 12  ,
                            12  , 12  , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 14  , 14  , 14  , 14  , 14  ,
                            14  , 14  , 14  , 14  , 14  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 16  ,
                            16  , 16  , 16  , 16  , 16  , 16  , 16  , 16  , 16  , 16  , 17  , 17  , 17  , 17  , 17  , 17  ,
                            17  , 17  , 17  , 17  , 17  , 17  , 18  , 18  , 18  , 18  , 18  , 18  , 18  , 18  , 18  , 18  ,
                            18  , 18  , 19  , 19  , 19  , 19  , 19  , 19  , 19  , 19  , 19  , 19  , 19  , 19  , 19  , 20  ,
                        },
                    },
                },
                {
                    2048  ,  /// region
                    {
                        {
                            2   , 3   , 4   , 5   , 5   , 6   , 6   , 7   , 7   , 8   , 8   , 8   , 9   , 9   , 9   , 10  ,
                            10  , 10  , 11  , 11  , 11  , 11  , 12  , 12  , 12  , 12  , 13  , 13  , 13  , 13  , 14  , 14  ,
                            14  , 14  , 14  , 15  , 15  , 15  , 15  , 15  , 16  , 16  , 16  , 16  , 16  , 17  , 17  , 17  ,
                            17  , 17  , 17  , 18  , 18  , 18  , 18  , 18  , 18  , 19  , 19  , 19  , 19  , 19  , 19  , 20  ,
                            20  , 20  , 20  , 20  , 20  , 20  , 21  , 21  , 21  , 21  , 21  , 21  , 21  , 22  , 22  , 22  ,
                            22  , 22  , 22  , 22  , 22  , 23  , 23  , 23  , 23  , 23  , 23  , 23  , 23  , 24  , 24  , 24  ,
                            24  , 24  , 24  , 24  , 24  , 25  , 25  , 25  , 25  , 25  , 25  , 25  , 25  , 26  , 26  , 26  ,
                            26  , 26  , 26  , 26  , 26  , 26  , 27  , 27  , 27  , 27  , 27  , 27  , 27  , 27  , 27  , 27  ,
                        },
                    },
                },
                {
                    4096  ,  /// region
                    {
                        {
                            3   , 4   , 6   , 7   , 7   , 8   , 9   , 10  , 10  , 11  , 11  , 12  , 12  , 13  , 13  , 14  ,
                            14  , 14  , 15  , 15  , 16  , 16  , 16  , 17  , 17  , 17  , 18  , 18  , 18  , 19  , 19  , 19  ,
                            19  , 20  , 20  , 20  , 21  , 21  , 21  , 21  , 22  , 22  , 22  , 22  , 23  , 23  , 23  , 23  ,
                            24  , 24  , 24  , 24  , 25  , 25  , 25  , 25  , 26  , 26  , 26  , 26  , 26  , 27  , 27  , 27  ,
                            27  , 27  , 28  , 28  , 28  , 28  , 28  , 29  , 29  , 29  , 29  , 29  , 30  , 30  , 30  , 30  ,
                            30  , 31  , 31  , 31  , 31  , 31  , 31  , 32  , 32  , 32  , 32  , 32  , 33  , 33  , 33  , 33  ,
                            33  , 33  , 34  , 34  , 34  , 34  , 34  , 34  , 35  , 35  , 35  , 35  , 35  , 35  , 36  , 36  ,
                            36  , 36  , 36  , 36  , 36  , 37  , 37  , 37  , 37  , 37  , 37  , 38  , 38  , 38  , 38  , 38  ,
                        },
                    },
                },
                {
                    8192  ,  /// region
                    {
                        {
                            1   , 4   , 6   , 8   , 9   , 10  , 11  , 13  , 14  , 15  , 15  , 16  , 17  , 18  , 19  , 19  ,
                            20  , 21  , 22  , 22  , 23  , 23  , 24  , 25  , 25  , 26  , 26  , 27  , 27  , 28  , 29  , 29  ,
                            30  , 30  , 31  , 31  , 32  , 32  , 32  , 33  , 33  , 34  , 34  , 35  , 35  , 36  , 36  , 36  ,
                            37  , 37  , 38  , 38  , 38  , 39  , 39  , 40  , 40  , 40  , 41  , 41  , 41  , 42  , 42  , 43  ,
                            43  , 43  , 44  , 44  , 44  , 45  , 45  , 45  , 46  , 46  , 46  , 47  , 47  , 47  , 48  , 48  ,
                            48  , 49  , 49  , 49  , 50  , 50  , 50  , 51  , 51  , 51  , 51  , 52  , 52  , 52  , 53  , 53  ,
                            53  , 54  , 54  , 54  , 54  , 55  , 55  , 55  , 56  , 56  , 56  , 56  , 57  , 57  , 57  , 57  ,
                            58  , 58  , 58  , 59  , 59  , 59  , 59  , 60  , 60  , 60  , 60  , 61  , 61  , 61  , 61  , 62  ,
                        },
                    },
                },
                {
                    16384 ,  /// region
                    {
                        {
                            5   , 9   , 11  , 13  , 15  , 17  , 18  , 19  , 20  , 22  , 23  , 24  , 25  , 26  , 27  , 27  ,
                            28  , 29  , 30  , 31  , 32  , 32  , 33  , 34  , 34  , 35  , 36  , 36  , 37  , 38  , 38  , 39  ,
                            40  , 40  , 41  , 41  , 42  , 43  , 43  , 44  , 44  , 45  , 45  , 46  , 46  , 47  , 47  , 48  ,
                            48  , 49  , 49  , 50  , 50  , 51  , 51  , 52  , 52  , 53  , 53  , 53  , 54  , 54  , 55  , 55  ,
                            56  , 56  , 56  , 57  , 57  , 58  , 58  , 59  , 59  , 59  , 60  , 60  , 61  , 61  , 61  , 62  ,
                            62  , 62  , 63  , 63  , 64  , 64  , 64  , 65  , 65  , 65  , 66  , 66  , 67  , 67  , 67  , 68  ,
                            68  , 68  , 69  , 69  , 69  , 70  , 70  , 70  , 71  , 71  , 71  , 72  , 72  , 72  , 73  , 73  ,
                            73  , 74  , 74  , 74  , 75  , 75  , 75  , 76  , 76  , 76  , 77  , 77  , 77  , 77  , 78  , 78  ,
                        },
                    },
                },
                {
                    32768 ,  /// region
                    {
                        {
                            7   , 12  , 16  , 18  , 21  , 23  , 25  , 27  , 28  , 30  , 32  , 33  , 34  , 36  , 37  , 38  ,
                            40  , 41  , 42  , 43  , 44  , 45  , 46  , 47  , 48  , 49  , 50  , 51  , 52  , 53  , 54  , 55  ,
                            56  , 56  , 57  , 58  , 59  , 60  , 60  , 61  , 62  , 63  , 63  , 64  , 65  , 66  , 66  , 67  ,
                            68  , 68  , 69  , 70  , 71  , 71  , 72  , 72  , 73  , 74  , 74  , 75  , 76  , 76  , 77  , 78  ,
                            78  , 79  , 79  , 80  , 81  , 81  , 82  , 82  , 83  , 83  , 84  , 85  , 85  , 86  , 86  , 87  ,
                            87  , 88  , 88  , 89  , 89  , 90  , 90  , 91  , 91  , 92  , 93  , 93  , 94  , 94  , 95  , 95  ,
                            96  , 96  , 97  , 97  , 97  , 98  , 98  , 99  , 99  , 100 , 100 , 101 , 101 , 102 , 102 , 103 ,
                            103 , 104 , 104 , 104 , 105 , 105 , 106 , 106 , 107 , 107 , 108 , 108 , 108 , 109 , 109 , 110 ,
                        },
                    },
                },
                {
                    65536 ,  /// region
                    {
                        {
                            13  , 20  , 25  , 29  , 32  , 35  , 37  , 40  , 42  , 44  , 46  , 48  , 50  , 52  , 54  , 55  ,
                            57  , 59  , 60  , 62  , 63  , 65  , 66  , 67  , 69  , 70  , 71  , 72  , 74  , 75  , 76  , 77  ,
                            78  , 79  , 81  , 82  , 83  , 84  , 85  , 86  , 87  , 88  , 89  , 90  , 91  , 92  , 93  , 94  ,
                            95  , 96  , 97  , 98  , 98  , 99  , 100 , 101 , 102 , 103 , 104 , 105 , 105 , 106 , 107 , 108 ,
                            109 , 109 , 110 , 111 , 112 , 113 , 113 , 114 , 115 , 116 , 116 , 117 , 118 , 119 , 119 , 120 ,
                            121 , 122 , 122 , 123 , 124 , 125 , 125 , 126 , 127 , 127 , 128 , 129 , 129 , 130 , 131 , 131 ,
                            132 , 133 , 133 , 134 , 135 , 135 , 136 , 137 , 137 , 138 , 138 , 139 , 140 , 140 , 141 , 142 ,
                            142 , 143 , 143 , 144 , 145 , 145 , 146 , 146 , 147 , 148 , 148 , 149 , 149 , 150 , 151 , 151 ,
                        },
                    },
                },
                {
                    131072,  /// region
                    {
                        {
                            30  , 39  , 45  , 50  , 55  , 58  , 62  , 65  , 68  , 71  , 74  , 76  , 79  , 81  , 83  , 86  ,
                            88  , 90  , 92  , 94  , 96  , 98  , 99  , 101 , 103 , 105 , 106 , 108 , 110 , 111 , 113 , 114 ,
                            116 , 117 , 119 , 120 , 122 , 123 , 124 , 126 , 127 , 129 , 130 , 131 , 132 , 134 , 135 , 136 ,
                            137 , 139 , 140 , 141 , 142 , 143 , 145 , 146 , 147 , 148 , 149 , 150 , 151 , 153 , 154 , 155 ,
                            156 , 157 , 158 , 159 , 160 , 161 , 162 , 163 , 164 , 165 , 166 , 167 , 168 , 169 , 170 , 171 ,
                            172 , 173 , 174 , 175 , 176 , 177 , 177 , 178 , 179 , 180 , 181 , 182 , 183 , 184 , 185 , 186 ,
                            186 , 187 , 188 , 189 , 190 , 191 , 192 , 192 , 193 , 194 , 195 , 196 , 197 , 197 , 198 , 199 ,
                            200 , 201 , 201 , 202 , 203 , 204 , 205 , 205 , 206 , 207 , 208 , 208 , 209 , 210 , 211 , 212 ,
                        },
                    },
                },
                {
                    262144,  /// region
                    {
                        {
                            30  , 39  , 45  , 50  , 55  , 58  , 62  , 65  , 68  , 71  , 74  , 76  , 79  , 81  , 83  , 86  ,
                            88  , 90  , 92  , 94  , 96  , 98  , 99  , 101 , 103 , 105 , 106 , 108 , 110 , 111 , 113 , 114 ,
                            116 , 117 , 119 , 120 , 122 , 123 , 124 , 126 , 127 , 129 , 130 , 131 , 132 , 134 , 135 , 136 ,
                            137 , 139 , 140 , 141 , 142 , 143 , 145 , 146 , 147 , 148 , 149 , 150 , 151 , 153 , 154 , 155 ,
                            156 , 157 , 158 , 159 , 160 , 161 , 162 , 163 , 164 , 165 , 166 , 167 , 168 , 169 , 170 , 171 ,
                            172 , 173 , 174 , 175 , 176 , 177 , 177 , 178 , 179 , 180 , 181 , 182 , 183 , 184 , 185 , 186 ,
                            186 , 187 , 188 , 189 , 190 , 191 , 192 , 192 , 193 , 194 , 195 , 196 , 197 , 197 , 198 , 199 ,
                            200 , 201 , 201 , 202 , 203 , 204 , 205 , 205 , 206 , 207 , 208 , 208 , 209 , 210 , 211 , 212 ,
                        },
                    },
                },
                {
                    524288,  /// region
                    {
                        {
                            30  , 39  , 45  , 50  , 55  , 58  , 62  , 65  , 68  , 71  , 74  , 76  , 79  , 81  , 83  , 86  ,
                            88  , 90  , 92  , 94  , 96  , 98  , 99  , 101 , 103 , 105 , 106 , 108 , 110 , 111 , 113 , 114 ,
                            116 , 117 , 119 , 120 , 122 , 123 , 124 , 126 , 127 , 129 , 130 , 131 , 132 , 134 , 135 , 136 ,
                            137 , 139 , 140 , 141 , 142 , 143 , 145 , 146 , 147 , 148 , 149 , 150 , 151 , 153 , 154 , 155 ,
                            156 , 157 , 158 , 159 , 160 , 161 , 162 , 163 , 164 , 165 , 166 , 167 , 168 , 169 , 170 , 171 ,
                            172 , 173 , 174 , 175 , 176 , 177 , 177 , 178 , 179 , 180 , 181 , 182 , 183 , 184 , 185 , 186 ,
                            186 , 187 , 188 , 189 , 190 , 191 , 192 , 192 , 193 , 194 , 195 , 196 , 197 , 197 , 198 , 199 ,
                            200 , 201 , 201 , 202 , 203 , 204 , 205 , 205 , 206 , 207 , 208 , 208 , 209 , 210 , 211 , 212 ,
                        },
                    },
                },
            },
        },
    },
    /*
    *
    * dpf
    *
    */
    {
        1     ,  /// dpfTuneEnable
        4     ,  /// dpfOffCenMultS
        45252 ,  /// dpfOffCenMult
        960   ,  /// dpfCenX
        540   ,  /// dpfCenY
        /*
        *
        * dpfGainSwitchTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]dpfByp(not used)
                /// 0          1
                {   1 * 1024, {1    }},
                {   2 * 1024, {1    }},
                {   4 * 1024, {1    }},
                {   8 * 1024, {1    }},
                {  16 * 1024, {1    }},
                {  32 * 1024, {1    }},
                {  64 * 1024, {1    }},
                { 128 * 1024, {1    }},
                { 256 * 1024, {1    }},
                { 512 * 1024, {1    }},
            },
        },
        /*
        *
        * dpfGainInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]dpfSadThr
                /// [2]dpfSadSlp
                /// [3]dpfHue1Th
                /// [4]dpfHue1Slope
                /// [5]dpfHue2Th
                /// [6]dpfHue2Slope
                /// [7]dpfHueStr
                /// [8]dpfSat1Th
                /// [9]dpfSat1Slope
                /// [10]dpfSatStr
                /// [11]dpfLuma11Th
                /// [12]dpfLuma11Slope
                /// [13]dpfLuma12Th
                /// [14]dpfLuma12Slope
                /// [15]dpfLuma21Th
                /// [16]dpfLuma21Slope
                /// [17]dpfLuma22Th
                /// [18]dpfLuma22Slope
                /// [19]dpfLumaStr
                /// [20]dpfHslTh
                /// [21]dpfHslSlope
                /// [22]dpfPurpleStr
                /// [23]dpfSaturStr
                /// 0          1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     17     18     19     20     21     22     23
                {   1 * 1024, {1024 , 1000 , 2050 , 1024 , 2150 , 1024 , 768  , 164  , 123  , 512  , 128  , 4095 , 1200 , 0    , 128  , 4095 , 3900 , 0    , 512  , 0    , 2048 , 1023 , 0    }},
                {   2 * 1024, {1024 , 1000 , 2050 , 1024 , 2150 , 1024 , 768  , 164  , 123  , 512  , 128  , 4095 , 1200 , 0    , 128  , 4095 , 3900 , 0    , 512  , 0    , 2048 , 1023 , 16   }},
                {   4 * 1024, {1024 , 1000 , 2050 , 1024 , 2150 , 1024 , 768  , 164  , 123  , 512  , 128  , 4095 , 1200 , 0    , 128  , 4095 , 3900 , 0    , 512  , 0    , 2048 , 1023 , 16   }},
                {   8 * 1024, {1024 , 1000 , 2050 , 1024 , 2150 , 1024 , 768  , 164  , 123  , 512  , 128  , 4095 , 1200 , 0    , 128  , 4095 , 3900 , 0    , 512  , 0    , 2048 , 1023 , 32   }},
                {  16 * 1024, {1024 , 1000 , 2050 , 1024 , 2150 , 1024 , 768  , 2000 , 123  , 512  , 128  , 4095 , 1200 , 0    , 64   , 4095 , 3900 , 0    , 512  , 0    , 2048 , 512  , 32   }},
                {  32 * 1024, {1024 , 1000 , 2050 , 1024 , 2150 , 1024 , 768  , 2000 , 123  , 512  , 128  , 4095 , 1200 , 0    , 64   , 4095 , 3900 , 0    , 512  , 0    , 2048 , 256  , 32   }},
                {  64 * 1024, {1024 , 1000 , 2050 , 1024 , 2150 , 1024 , 768  , 2000 , 123  , 512  , 128  , 4095 , 1200 , 0    , 64   , 4095 , 3900 , 0    , 512  , 0    , 2048 , 128  , 32   }},
                { 128 * 1024, {1024 , 1000 , 2050 , 1024 , 2150 , 1024 , 768  , 2000 , 123  , 512  , 128  , 4095 , 1200 , 0    , 64   , 4095 , 3900 , 0    , 512  , 0    , 2048 , 0    , 32   }},
                { 256 * 1024, {1024 , 1000 , 2050 , 1024 , 2150 , 1024 , 768  , 2000 , 123  , 512  , 128  , 4095 , 1200 , 0    , 64   , 4095 , 3900 , 0    , 512  , 0    , 2048 , 0    , 64   }},
                { 512 * 1024, {1024 , 1000 , 2050 , 1024 , 2150 , 1024 , 768  , 2000 , 123  , 512  , 128  , 4095 , 1200 , 0    , 64   , 4095 , 3900 , 0    , 512  , 0    , 2048 , 0    , 64   }},
            },
        },
        /*
        *
        * dpfStrLutTab
        *
        */
        {
            10    ,  /// validSize
            {
                {
                    1024  ,  /// region
                    {
                        {
                            255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 ,
                            255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 ,
                            255 ,
                        },
                    },
                },
                {
                    2048  ,  /// region
                    {
                        {
                            255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 ,
                            255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 ,
                            255 ,
                        },
                    },
                },
                {
                    4096  ,  /// region
                    {
                        {
                            255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 ,
                            255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 ,
                            255 ,
                        },
                    },
                },
                {
                    8192  ,  /// region
                    {
                        {
                            255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 ,
                            255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 ,
                            255 ,
                        },
                    },
                },
                {
                    16384 ,  /// region
                    {
                        {
                            255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 ,
                            255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 ,
                            255 ,
                        },
                    },
                },
                {
                    32768 ,  /// region
                    {
                        {
                            255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 ,
                            255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 ,
                            255 ,
                        },
                    },
                },
                {
                    65536 ,  /// region
                    {
                        {
                            255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 ,
                            255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 ,
                            255 ,
                        },
                    },
                },
                {
                    131072,  /// region
                    {
                        {
                            255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 ,
                            255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 ,
                            255 ,
                        },
                    },
                },
                {
                    262144,  /// region
                    {
                        {
                            255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 ,
                            255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 ,
                            255 ,
                        },
                    },
                },
                {
                    524288,  /// region
                    {
                        {
                            255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 ,
                            255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 , 255 ,
                            255 ,
                        },
                    },
                },
            },
        },
    },
    /*
    *
    * csp
    *
    */
    {
        1     ,  /// cspTuneEnable
        /*
        *
        * cspGainSwitchTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]cspChsEn
                /// 0          1
                {   1 * 1024, {0    }},
                {   2 * 1024, {0    }},
                {   4 * 1024, {0    }},
                {   8 * 1024, {0    }},
                {  16 * 1024, {0    }},
                {  32 * 1024, {0    }},
                {  64 * 1024, {0    }},
                { 128 * 1024, {0    }},
                { 256 * 1024, {0    }},
                { 512 * 1024, {0    }},
            },
        },
        /*
        *
        * cspGainInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]cspChsHiGain0
                /// [2]cspChsHiGain1
                /// [3]cspChsHiGain2
                /// [4]cspChsHiGain3
                /// [5]cspChsLoGain0
                /// [6]cspChsLoGain1
                /// [7]cspChsLoGain2
                /// [8]cspChsLoGain3
                /// [9]cspChsMaxGain
                /// [10]cspChsWeightU0
                /// [11]cspChsWeightU1
                /// [12]cspChsWeightU2
                /// [13]cspChsWeightV0
                /// [14]cspChsWeightV1
                /// [15]cspChsWeightV2
                /// 0          1      2      3      4      5      6      7      8      9      10     11     12     13     14     15
                {   1 * 1024, {1536 , 1280 , 960  , 796  , 796  , 960  , 1280 , 1536 , 1536 , 0    , 468  , 227  , 179  , 421  , 0    }},
                {   2 * 1024, {1536 , 1280 , 960  , 796  , 796  , 960  , 1280 , 1536 , 1536 , 0    , 468  , 227  , 179  , 421  , 0    }},
                {   4 * 1024, {1536 , 1280 , 960  , 796  , 796  , 960  , 1280 , 1536 , 1536 , 0    , 468  , 227  , 179  , 421  , 0    }},
                {   8 * 1024, {1536 , 1280 , 960  , 796  , 796  , 960  , 1280 , 1536 , 1536 , 0    , 468  , 227  , 179  , 421  , 0    }},
                {  16 * 1024, {1536 , 1280 , 960  , 796  , 796  , 960  , 1280 , 1536 , 1536 , 0    , 468  , 227  , 179  , 421  , 0    }},
                {  32 * 1024, {1536 , 1280 , 960  , 796  , 796  , 960  , 1280 , 1536 , 1536 , 0    , 468  , 227  , 179  , 421  , 0    }},
                {  64 * 1024, {1536 , 1280 , 960  , 796  , 796  , 960  , 1280 , 1536 , 1536 , 0    , 468  , 227  , 179  , 421  , 0    }},
                { 128 * 1024, {1536 , 1280 , 960  , 796  , 796  , 960  , 1280 , 1536 , 1536 , 0    , 468  , 227  , 179  , 421  , 0    }},
                { 256 * 1024, {1536 , 1280 , 960  , 796  , 796  , 960  , 1280 , 1536 , 1536 , 0    , 468  , 227  , 179  , 421  , 0    }},
                { 512 * 1024, {1536 , 1280 , 960  , 796  , 796  , 960  , 1280 , 1536 , 1536 , 0    , 468  , 227  , 179  , 421  , 0    }},
            },
        },
    },
    /*
    *
    * mvd
    *
    */
    {
        1     ,  /// mvdTuneEnable
        12    ,  /// mvdPipeABSadBitdepth
        3     ,  /// mvdLosstCompMode
        77    ,  /// mvdPipeABRGB2YCR
        150   ,  /// mvdPipeABRGB2YCG
        29    ,  /// mvdPipeABRGB2YCB
        /*
        *
        * mvdGainSwitchTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]mvdPipeBErode1
                /// [2]mvdPipeBDilate
                /// [3]mvdPipeBErode2
                /// [4]mvdPipeBMapRefineThr
                /// [5]mvdNpValueMid
                /// [6]mvdNpValueEnd
                /// 0          1      2      3      4      5      6
                {   1 * 1024, {0    , 0    , 0    , 6    , 10   , 11   }},
                {   2 * 1024, {0    , 0    , 0    , 6    , 10   , 11   }},
                {   4 * 1024, {0    , 0    , 0    , 6    , 10   , 11   }},
                {   8 * 1024, {0    , 0    , 0    , 6    , 10   , 11   }},
                {  16 * 1024, {1    , 0    , 0    , 6    , 10   , 11   }},
                {  32 * 1024, {1    , 0    , 0    , 6    , 10   , 11   }},
                {  64 * 1024, {1    , 0    , 0    , 6    , 10   , 11   }},
                { 128 * 1024, {1    , 0    , 0    , 6    , 10   , 11   }},
                { 256 * 1024, {2    , 2    , 0    , 6    , 10   , 11   }},
                { 512 * 1024, {2    , 2    , 0    , 6    , 10   , 11   }},
            },
        },
        /*
        *
        * mvdGainInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]mvdNpOffset
                /// [2]mvdPipeaThr
                /// [3]mvdPipeABTriDirStr
                /// [4]mvdNpValueOffset
                /// [5]mvdNpGainR1
                /// [6]mvdNpGainR2
                /// [7]mvdNpGainR3
                /// [8]mvdRatioSadl
                /// [9]mvdRatioSads
                /// [10]mvdSqrtRatio
                /// 0          1      2      3      4      5      6      7      8      9      10
                {   1 * 1024, {3    , 0    , 0    , 512  , 32   , 32   , 32   , 32   , 32   , 1    }},
                {   2 * 1024, {3    , 1    , 0    , 512  , 32   , 32   , 32   , 32   , 32   , 1    }},
                {   4 * 1024, {3    , 2    , 10   , 512  , 32   , 32   , 32   , 32   , 32   , 1    }},
                {   8 * 1024, {3    , 3    , 20   , 512  , 32   , 32   , 32   , 32   , 32   , 1    }},
                {  16 * 1024, {3    , 4    , 20   , 512  , 32   , 32   , 32   , 32   , 32   , 1    }},
                {  32 * 1024, {3    , 5    , 20   , 512  , 32   , 32   , 32   , 32   , 32   , 1    }},
                {  64 * 1024, {3    , 6    , 20   , 512  , 32   , 32   , 32   , 32   , 32   , 1    }},
                { 128 * 1024, {3    , 6    , 20   , 512  , 32   , 32   , 32   , 32   , 32   , 1    }},
                { 256 * 1024, {2    , 3    , 20   , 512  , 32   , 32   , 32   , 32   , 32   , 1    }},
                { 512 * 1024, {2    , 3    , 20   , 512  , 32   , 32   , 32   , 32   , 32   , 1    }},
            },
        },
    },
    /*
    *
    * tnr
    *
    */
    {
        1     ,  /// tnrTuneEnable
        0     ,  /// dynamicType
        /*
        *
        * tnrSwitchTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain or lux
                /// [1]tnrGlbNoiseLumMid
                /// [2]tnrGlbNoiseLumEnd
                /// [3]tnrGlbNoiseFreqMid
                /// [4]tnrGlbNoiseFreqEnd
                /// [5]tnrMotionMid
                /// [6]tnrMotionEnd
                /// 0     1      2      3      4      5      6
                {1024  , {8    , 8    , 8    , 8    , 7    , 7    }},
                {2048  , {8    , 8    , 8    , 8    , 7    , 7    }},
                {4096  , {8    , 8    , 8    , 8    , 7    , 7    }},
                {8192  , {8    , 8    , 8    , 8    , 7    , 7    }},
                {16384 , {8    , 8    , 8    , 8    , 7    , 7    }},
                {32768 , {8    , 8    , 8    , 8    , 7    , 7    }},
                {65536 , {8    , 8    , 8    , 8    , 7    , 7    }},
                {131072, {8    , 8    , 8    , 8    , 7    , 7    }},
                {262144, {8    , 8    , 8    , 8    , 7    , 7    }},
                {524288, {8    , 8    , 8    , 8    , 7    , 7    }},
            },
        },
        /*
        *
        * tnrInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain or lux
                /// [1]tnrGlbNoiseLumOffset
                /// [2]tnrGlbNoiseLumR1
                /// [3]tnrGlbNoiseLumR2
                /// [4]tnrGlbNoiseLumR3
                /// [5]tnrGlbNoiseFreqOffset
                /// [6]tnrGlbNoiseFreqR1
                /// [7]tnrGlbNoiseFreqR2
                /// [8]tnrGlbNoiseFreqR3
                /// [9]tnrNr2dThrY
                /// [10]tnrNr2dThrC
                /// [11]tnrNr2dGradSlope
                /// [12]tnrMotionOffset
                /// [13]tnrMotionYNR2DR1
                /// [14]tnrMotionYNR2DR2
                /// [15]tnrMotionYNR2DR3
                /// [16]tnrMotionCNR2DR1
                /// [17]tnrMotionCNR2DR2
                /// [18]tnrMotionCNR2DR3
                /// [19]tnrMotionYNR3DR1
                /// [20]tnrMotionYNR3DR2
                /// [21]tnrMotionYNR3DR3
                /// [22]tnrMotionCNR3DR1
                /// [23]tnrMotionCNR3DR2
                /// [24]tnrMotionCNR3DR3
                /// [25]tnrMotrefRegmvStr
                /// [26]tnrRegModuSumThr
                /// [27]tnrRegModuMotThr
                /// [28]tnrGlbStr
                /// 0     1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     17     18     19     20     21     22     23     24     25     26     27     28
                {1024  , {50   , 70   , 90   , 110  , 0    , 256  , 256  , 256  , 60   , 60   , 32   , 0    , 60   , 120  , 200  , 60   , 120  , 200  , 245  , 50   , 10   , 245  , 50   , 10   , 20   , 0    , 0    , 256  }},
                {2048  , {50   , 70   , 90   , 110  , 0    , 256  , 256  , 256  , 60   , 60   , 32   , 0    , 60   , 120  , 200  , 60   , 120  , 200  , 245  , 50   , 10   , 245  , 50   , 10   , 20   , 0    , 0    , 256  }},
                {4096  , {50   , 70   , 90   , 110  , 0    , 256  , 256  , 256  , 60   , 60   , 32   , 0    , 60   , 120  , 200  , 60   , 120  , 200  , 245  , 50   , 10   , 245  , 50   , 10   , 20   , 0    , 0    , 256  }},
                {8192  , {50   , 70   , 90   , 110  , 0    , 256  , 256  , 256  , 60   , 60   , 32   , 0    , 60   , 120  , 200  , 60   , 120  , 200  , 245  , 50   , 10   , 245  , 50   , 10   , 20   , 0    , 0    , 256  }},
                {16384 , {50   , 70   , 90   , 110  , 0    , 256  , 256  , 256  , 60   , 60   , 32   , 0    , 60   , 120  , 200  , 60   , 120  , 200  , 245  , 50   , 10   , 245  , 50   , 10   , 20   , 0    , 0    , 256  }},
                {32768 , {50   , 70   , 90   , 110  , 0    , 256  , 256  , 256  , 60   , 60   , 32   , 0    , 60   , 120  , 200  , 60   , 120  , 200  , 245  , 50   , 10   , 245  , 50   , 10   , 20   , 0    , 0    , 256  }},
                {65536 , {50   , 70   , 90   , 110  , 0    , 256  , 256  , 256  , 60   , 60   , 32   , 0    , 60   , 120  , 200  , 60   , 120  , 200  , 245  , 50   , 10   , 245  , 50   , 10   , 20   , 0    , 0    , 256  }},
                {131072, {50   , 70   , 90   , 110  , 0    , 256  , 256  , 256  , 60   , 60   , 32   , 0    , 60   , 120  , 200  , 60   , 120  , 200  , 245  , 50   , 10   , 245  , 50   , 10   , 20   , 0    , 0    , 256  }},
                {262144, {50   , 70   , 90   , 110  , 0    , 256  , 256  , 256  , 60   , 60   , 32   , 0    , 60   , 120  , 200  , 60   , 120  , 200  , 245  , 50   , 10   , 245  , 50   , 10   , 20   , 0    , 0    , 256  }},
                {524288, {50   , 70   , 90   , 110  , 0    , 256  , 256  , 256  , 60   , 60   , 32   , 0    , 60   , 120  , 200  , 60   , 120  , 200  , 245  , 50   , 10   , 245  , 50   , 10   , 20   , 0    , 0    , 256  }},
            },
        },
    },
    /*
    *
    * ynr
    *
    */
    {
        1     ,  /// ynrTuneEnable
        0     ,  /// dynamicType
        /*
        *
        * ynrSwitchTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain or lux
                /// [1]ynrNpMid
                /// [2]ynrNpEnd
                /// [3]ynrSadGainFreqMid
                /// [4]ynrSadGainFreqEnd
                /// 0     1      2      3      4
                {1024  , {9    , 9    , 8    , 8    }},
                {2048  , {9    , 9    , 8    , 8    }},
                {4096  , {9    , 9    , 8    , 8    }},
                {8192  , {9    , 9    , 8    , 8    }},
                {16384 , {9    , 9    , 8    , 8    }},
                {32768 , {9    , 9    , 8    , 8    }},
                {65536 , {9    , 9    , 8    , 8    }},
                {131072, {9    , 9    , 8    , 8    }},
                {262144, {9    , 9    , 8    , 8    }},
                {524288, {9    , 9    , 8    , 8    }},
            },
        },
        /*
        *
        * ynrInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain or lux
                /// [1]ynrNlmSadGain
                /// [2]ynrNlmMotionR
                /// [3]ynrNpOffset
                /// [4]ynrNpR1
                /// [5]ynrNpR2
                /// [6]ynrNpR3
                /// [7]ynrSadGainFreqOffset
                /// [8]ynrSadGainFreqR1
                /// [9]ynrSadGainFreqR2
                /// [10]ynrSadGainFreqR3
                /// [11]ynrGlbStr
                /// [12]ynrNlmTable
                /// 0     1      2      3      4      5      6      7      8      9      10     11     12
                {1024  , {256  , 512  , 0    , 40   , 45   , 50   , 0    , 256  , 256  , 256  , 160  , 8    }},
                {2048  , {256  , 512  , 0    , 40   , 45   , 50   , 0    , 256  , 256  , 256  , 160  , 8    }},
                {4096  , {256  , 512  , 0    , 40   , 45   , 50   , 0    , 256  , 256  , 256  , 160  , 8    }},
                {8192  , {256  , 512  , 0    , 40   , 45   , 50   , 0    , 256  , 256  , 256  , 160  , 8    }},
                {16384 , {256  , 512  , 0    , 40   , 45   , 50   , 0    , 256  , 256  , 256  , 160  , 8    }},
                {32768 , {256  , 512  , 0    , 40   , 45   , 50   , 0    , 256  , 256  , 256  , 160  , 8    }},
                {65536 , {256  , 512  , 0    , 40   , 45   , 50   , 0    , 256  , 256  , 256  , 160  , 8    }},
                {131072, {256  , 512  , 0    , 40   , 45   , 50   , 0    , 256  , 256  , 256  , 160  , 8    }},
                {262144, {256  , 512  , 0    , 40   , 45   , 50   , 0    , 256  , 256  , 256  , 160  , 8    }},
                {524288, {256  , 512  , 0    , 40   , 45   , 50   , 0    , 256  , 256  , 256  , 160  , 8    }},
            },
        },
    },
    /*
    *
    * lsh
    *
    */
    {
        1     ,  /// lshTuneEnable
        0     ,  /// dynamicType
        /*
        *
        * lshSwitchTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain or lux
                /// [1]lshLumMid
                /// [2]lshLumEnd
                /// [3]lshEdgeThinMid
                /// [4]lshEdgeThinEnd
                /// [5]lshNrFreqMid
                /// [6]lshNrFreqEnd
                /// [7]lshDetailType
                /// [8]lshDetailFreqMid
                /// [9]lshDetailFreqEnd
                /// [10]lshDetailLumMid
                /// [11]lshDetailLumEnd
                /// [12]lshDltiEn(not used)
                /// [13]lshDltiPosMid
                /// [14]lshDltiPosEnd
                /// [15]lshDltiNegMid
                /// [16]lshDltiNegEnd
                /// [17]lshTextureEnhMid
                /// [18]lshTextureEnhEnd
                /// 0     1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     17     18
                {1024  , {6    , 6    , 5    , 6    , 4    , 4    , 0    , 6    , 6    , 8    , 8    , 1    , 7    , 6    , 7    , 8    , 6    , 7    }},
                {2048  , {6    , 6    , 5    , 6    , 4    , 4    , 0    , 6    , 6    , 8    , 8    , 1    , 7    , 6    , 7    , 8    , 6    , 7    }},
                {4096  , {6    , 6    , 5    , 6    , 4    , 4    , 0    , 6    , 6    , 8    , 8    , 1    , 7    , 6    , 7    , 8    , 6    , 7    }},
                {8192  , {6    , 6    , 5    , 6    , 4    , 4    , 0    , 6    , 6    , 8    , 8    , 1    , 7    , 6    , 7    , 8    , 6    , 7    }},
                {16384 , {6    , 6    , 5    , 6    , 4    , 4    , 0    , 6    , 6    , 8    , 8    , 1    , 7    , 6    , 7    , 8    , 6    , 7    }},
                {32768 , {6    , 6    , 5    , 6    , 4    , 4    , 0    , 6    , 6    , 8    , 8    , 1    , 7    , 6    , 7    , 8    , 6    , 7    }},
                {65536 , {4    , 4    , 5    , 6    , 4    , 4    , 0    , 6    , 6    , 8    , 8    , 1    , 7    , 6    , 7    , 8    , 6    , 7    }},
                {131072, {4    , 4    , 5    , 6    , 4    , 4    , 0    , 6    , 6    , 8    , 8    , 1    , 7    , 6    , 7    , 8    , 6    , 7    }},
                {262144, {4    , 4    , 5    , 6    , 4    , 4    , 0    , 6    , 6    , 8    , 8    , 1    , 7    , 6    , 7    , 8    , 6    , 7    }},
                {524288, {4    , 4    , 5    , 6    , 4    , 4    , 0    , 6    , 6    , 8    , 8    , 1    , 7    , 6    , 7    , 8    , 6    , 7    }},
            },
        },
        /*
        *
        * lshInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain or lux
                /// [1]lshMotionR
                /// [2]lshLumOffset
                /// [3]lshLumY1
                /// [4]lshLumY2
                /// [5]lshLumY3
                /// [6]lshEdgeNlmSlp
                /// [7]lshEdgeN2wNormR
                /// [8]lshEdgeThinOffset
                /// [9]lshEdgeThinY1
                /// [10]lshEdgeThinY2
                /// [11]lshEdgeThinY3
                /// [12]lshNrFreqOffset
                /// [13]lshNrFreqY1
                /// [14]lshNrFreqY2
                /// [15]lshNrFreqY3
                /// [16]lshNegStr1
                /// [17]lshPosStr1
                /// [18]lshDetailNwAlpha
                /// [19]lshDetailNrR
                /// [20]lshDetailFreqOffset
                /// [21]lshDetailFreqY1
                /// [22]lshDetailFreqY2
                /// [23]lshDetailFreqY3
                /// [24]lshDetailLumOffset
                /// [25]lshDetailLumY1
                /// [26]lshDetailLumY2
                /// [27]lshDetailLumY3
                /// [28]lshNegStr2
                /// [29]lshPosStr2
                /// [30]lshDltiPosOffset
                /// [31]lshDltiFinalPosY1
                /// [32]lshDltiFinalPosY2
                /// [33]lshDltiFinalPosY3
                /// [34]lshDltiNegOffset
                /// [35]lshDltiFinalNegY1
                /// [36]lshDltiFinalNegY2
                /// [37]lshDltiFinalNegY3
                /// [38]lshTextureEnhOffset
                /// [39]lshTextureEnhY1
                /// [40]lshTextureEnhY2
                /// [41]lshTextureEnhY3
                /// [42]lshEdgeThickness
                /// [43]lshNlmTable
                /// 0     1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     17     18     19     20     21     22     23     24     25     26     27     28     29     30     31     32     33     34     35     36     37     38     39     40     41     42     43
                {1024  , {256  , 32   , 0    , 0    , 0    , 4000 , 100  , 0    , 256  , 256  , 100  , 8    , 0    , 0    , 0    , 256  , 200  , 256  , 100  , 16   , 256  , 600  , 356  , 128  , 256  , 300  , 300  , 384  , 384  , 64   , 256  , 180  , 50   , 64   , 256  , 200  , 150  , 160  , 64   , 256  , 300  , 256  , 12   }},
                {2048  , {256  , 32   , 0    , 0    , 0    , 4000 , 100  , 0    , 256  , 256  , 100  , 8    , 0    , 0    , 0    , 256  , 200  , 256  , 100  , 16   , 256  , 600  , 356  , 128  , 256  , 300  , 300  , 384  , 384  , 64   , 256  , 180  , 50   , 64   , 256  , 200  , 150  , 160  , 64   , 256  , 300  , 256  , 12   }},
                {4096  , {230  , 32   , 0    , 0    , 0    , 4000 , 100  , 0    , 256  , 256  , 100  , 8    , 0    , 0    , 0    , 256  , 200  , 256  , 100  , 16   , 256  , 600  , 356  , 128  , 256  , 356  , 300  , 384  , 384  , 64   , 256  , 180  , 50   , 64   , 256  , 200  , 150  , 160  , 64   , 256  , 300  , 256  , 12   }},
                {8192  , {210  , 32   , 0    , 0    , 0    , 4000 , 100  , 0    , 256  , 256  , 100  , 8    , 0    , 0    , 0    , 256  , 256  , 256  , 100  , 16   , 160  , 600  , 356  , 192  , 200  , 356  , 300  , 384  , 384  , 64   , 256  , 180  , 50   , 64   , 256  , 200  , 150  , 160  , 64   , 256  , 300  , 256  , 12   }},
                {16384 , {190  , 32   , 0    , 0    , 0    , 4000 , 100  , 0    , 256  , 256  , 100  , 8    , 0    , 0    , 0    , 256  , 256  , 256  , 100  , 16   , 80   , 600  , 356  , 192  , 128  , 256  , 300  , 384  , 384  , 64   , 256  , 180  , 50   , 64   , 256  , 200  , 150  , 160  , 64   , 256  , 300  , 256  , 12   }},
                {32768 , {180  , 32   , 200  , 0    , 0    , 4000 , 100  , 0    , 256  , 256  , 100  , 8    , 0    , 0    , 0    , 256  , 256  , 256  , 100  , 16   , 20   , 600  , 356  , 200  , 128  , 256  , 320  , 512  , 512  , 64   , 256  , 180  , 50   , 64   , 256  , 200  , 150  , 160  , 64   , 256  , 300  , 256  , 12   }},
                {65536 , {160  , 180  , 200  , 0    , 0    , 4000 , 100  , 0    , 256  , 256  , 100  , 8    , 0    , 0    , 0    , 256  , 256  , 256  , 100  , 16   , 20   , 600  , 356  , 200  , 128  , 256  , 356  , 256  , 200  , 64   , 256  , 180  , 50   , 64   , 256  , 200  , 150  , 160  , 64   , 256  , 300  , 256  , 12   }},
                {131072, {160  , 180  , 200  , 0    , 0    , 4000 , 100  , 0    , 256  , 256  , 100  , 8    , 0    , 0    , 0    , 256  , 256  , 256  , 100  , 16   , 20   , 600  , 356  , 256  , 128  , 256  , 356  , 256  , 200  , 64   , 256  , 180  , 50   , 64   , 256  , 200  , 150  , 160  , 64   , 256  , 300  , 256  , 12   }},
                {262144, {160  , 180  , 200  , 0    , 0    , 4000 , 100  , 0    , 256  , 256  , 100  , 8    , 0    , 0    , 0    , 256  , 256  , 256  , 100  , 16   , 20   , 600  , 356  , 256  , 128  , 256  , 356  , 256  , 200  , 64   , 256  , 180  , 50   , 64   , 256  , 200  , 150  , 160  , 64   , 256  , 300  , 256  , 12   }},
                {524288, {140  , 180  , 200  , 0    , 0    , 4000 , 100  , 0    , 256  , 256  , 100  , 8    , 0    , 0    , 0    , 256  , 256  , 256  , 100  , 16   , 20   , 600  , 356  , 256  , 128  , 256  , 356  , 256  , 200  , 64   , 256  , 180  , 50   , 64   , 256  , 200  , 150  , 160  , 64   , 256  , 300  , 256  , 12   }},
            },
        },
    },
    /*
    *
    * cnr
    *
    */
    {
        1     ,  /// cnrTuneEnable
        0     ,  /// dynamicType
        /*
        *
        * cnrSwitchTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain or lux
                /// [1]cnrLumaNoiseMid
                /// [2]cnrLumaNoiseEnd
                /// [3]cnrRfiltSatMid
                /// [4]cnrRfiltSatEnd
                /// [5]cnrRfiltDiffMid
                /// [6]cnrRfiltDiffEnd
                /// [7]cnrUvBlendMid
                /// [8]cnrUvBlendEnd
                /// [9]cnrFcSupMid
                /// [10]cnrFcSupEnd
                /// 0     1      2      3      4      5      6      7      8      9      10
                {1024  , {10   , 10   , 5    , 5    , 5    , 5    , 9    , 9    , 5    , 5    }},
                {2048  , {10   , 10   , 5    , 5    , 5    , 5    , 9    , 9    , 5    , 5    }},
                {4096  , {10   , 10   , 5    , 5    , 5    , 5    , 9    , 9    , 5    , 5    }},
                {8192  , {10   , 10   , 5    , 5    , 5    , 5    , 9    , 9    , 5    , 5    }},
                {16384 , {10   , 10   , 5    , 5    , 5    , 5    , 9    , 9    , 5    , 5    }},
                {32768 , {10   , 10   , 5    , 5    , 5    , 5    , 9    , 9    , 5    , 5    }},
                {65536 , {10   , 10   , 5    , 5    , 5    , 5    , 9    , 9    , 5    , 5    }},
                {131072, {10   , 10   , 5    , 5    , 5    , 5    , 9    , 9    , 5    , 5    }},
                {262144, {10   , 10   , 5    , 5    , 5    , 5    , 9    , 9    , 5    , 5    }},
                {524288, {10   , 10   , 5    , 5    , 5    , 5    , 9    , 9    , 5    , 5    }},
            },
        },
        /*
        *
        * cnrInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain or lux
                /// [1]cnrMotionNoiseGain
                /// [2]cnrLumaNoiseOffset
                /// [3]cnrLumaNoiseY1
                /// [4]cnrLumaNoiseY2
                /// [5]cnrLumaNoiseY3
                /// [6]cnrRfiltSatOffset
                /// [7]cnrRfiltSatY1
                /// [8]cnrRfiltSatY2
                /// [9]cnrRfiltSatY3
                /// [10]cnrRfiltDiffOffset
                /// [11]cnrRfiltDiffY1
                /// [12]cnrRfiltDiffY2
                /// [13]cnrRfiltDiffY3
                /// [14]cnrGaussNoiseOffset
                /// [15]cnrGaussNoiseGain
                /// [16]cnrUvdiffSft
                /// [17]cnrUvBlendOffset
                /// [18]cnrUvBlendY1
                /// [19]cnrUvBlendY2
                /// [20]cnrUvBlendY3
                /// [21]cnrFcSupOffset
                /// [22]cnrFcSupY1
                /// [23]cnrFcSupY2
                /// [24]cnrFcSupY3
                /// 0     1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     17     18     19     20     21     22     23     24
                {1024  , {256  , 0    , 10   , 10   , 10   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 0    , 0    , 256  , 2    , 512  , 256  , 230  , 156  , 32   , 200  , 100  , 0    }},
                {2048  , {256  , 0    , 10   , 10   , 10   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 0    , 0    , 256  , 2    , 512  , 256  , 230  , 156  , 32   , 200  , 100  , 0    }},
                {4096  , {256  , 0    , 10   , 10   , 10   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 0    , 0    , 256  , 2    , 512  , 256  , 230  , 156  , 32   , 200  , 100  , 0    }},
                {8192  , {256  , 0    , 10   , 10   , 10   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 0    , 0    , 256  , 2    , 512  , 256  , 230  , 156  , 32   , 200  , 100  , 0    }},
                {16384 , {300  , 0    , 10   , 10   , 10   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 0    , 0    , 256  , 2    , 512  , 256  , 230  , 156  , 32   , 200  , 100  , 0    }},
                {32768 , {350  , 0    , 10   , 10   , 10   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 0    , 20   , 256  , 2    , 512  , 256  , 230  , 156  , 32   , 200  , 100  , 0    }},
                {65536 , {400  , 0    , 10   , 10   , 10   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 0    , 25   , 256  , 2    , 512  , 256  , 230  , 156  , 32   , 200  , 100  , 0    }},
                {131072, {450  , 0    , 20   , 20   , 20   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 0    , 25   , 256  , 2    , 512  , 256  , 230  , 156  , 32   , 256  , 256  , 256  }},
                {262144, {500  , 0    , 30   , 30   , 30   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 0    , 25   , 128  , 2    , 512  , 256  , 230  , 156  , 32   , 256  , 256  , 256  }},
                {524288, {600  , 0    , 30   , 30   , 30   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 0    , 25   , 128  , 2    , 512  , 256  , 230  , 156  , 32   , 256  , 256  , 256  }},
            },
        },
        /*
        *
        * cnrRadialLutTab
        *
        */
        {
            10    ,  /// validSize
            {
                {
                    1024  ,  /// region
                    {
                        {
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 ,
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 212 , 214 , 216 , 218 , 220 , 225 , 230 , 235 , 240 ,
                            255 ,
                        },
                    },
                },
                {
                    2048  ,  /// region
                    {
                        {
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 ,
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 212 , 214 , 216 , 218 , 220 , 225 , 230 , 235 , 240 ,
                            255 ,
                        },
                    },
                },
                {
                    4096  ,  /// region
                    {
                        {
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 ,
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 212 , 214 , 216 , 218 , 220 , 225 , 230 , 235 , 240 ,
                            255 ,
                        },
                    },
                },
                {
                    8192  ,  /// region
                    {
                        {
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 ,
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 212 , 214 , 216 , 218 , 220 , 225 , 230 , 235 , 240 ,
                            255 ,
                        },
                    },
                },
                {
                    16384 ,  /// region
                    {
                        {
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 ,
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 212 , 214 , 216 , 218 , 220 , 225 , 230 , 235 , 240 ,
                            255 ,
                        },
                    },
                },
                {
                    32768 ,  /// region
                    {
                        {
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 ,
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 212 , 214 , 216 , 218 , 220 , 225 , 230 , 235 , 240 ,
                            255 ,
                        },
                    },
                },
                {
                    65536 ,  /// region
                    {
                        {
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 ,
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 212 , 214 , 216 , 218 , 220 , 225 , 230 , 235 , 240 ,
                            255 ,
                        },
                    },
                },
                {
                    131072,  /// region
                    {
                        {
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 ,
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 212 , 214 , 216 , 218 , 220 , 225 , 230 , 235 , 240 ,
                            255 ,
                        },
                    },
                },
                {
                    262144,  /// region
                    {
                        {
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 ,
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 212 , 214 , 216 , 218 , 220 , 225 , 230 , 235 , 240 ,
                            255 ,
                        },
                    },
                },
                {
                    524288,  /// region
                    {
                        {
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 , 210 ,
                            210 , 210 , 210 , 210 , 210 , 210 , 210 , 212 , 214 , 216 , 218 , 220 , 225 , 230 , 235 , 240 ,
                            255 ,
                        },
                    },
                },
            },
        },
    },
    /*
    *
    * cmb
    *
    */
    {
        1     ,  /// cmbTuneEnable
        3072  ,  /// cmbThresh1Lm
        3840  ,  /// cmbThresh2Lm
        3072  ,  /// cmbThresh1Ms
        3840  ,  /// cmbThresh2Ms
        400   ,  /// cmbMovLmMult
        400   ,  /// cmbMovMsMult
        /*
        *
        * cmbGainInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]cmbNpMultLm
                /// [2]cmbNpMultMs
                /// [3]cmbNpMultSvs
                /// [4]cmbMovSysMult
                /// [5]cmbMovLmMult
                /// [6]cmbMovMsMult
                /// 0          1      2      3      4      5      6
                {   1 * 1024, {32   , 32   , 16   , 400  , 400  , 400  }},
                {   2 * 1024, {32   , 32   , 16   , 400  , 400  , 400  }},
                {   4 * 1024, {32   , 32   , 16   , 400  , 400  , 400  }},
                {   8 * 1024, {32   , 32   , 16   , 400  , 400  , 400  }},
                {  16 * 1024, {32   , 32   , 16   , 400  , 400  , 400  }},
                {  32 * 1024, {32   , 32   , 16   , 400  , 400  , 400  }},
                {  64 * 1024, {64   , 64   , 16   , 400  , 400  , 400  }},
                { 128 * 1024, {128  , 128  , 16   , 400  , 400  , 400  }},
                { 256 * 1024, {128  , 128  , 16   , 400  , 400  , 400  }},
                { 512 * 1024, {128  , 128  , 16   , 400  , 400  , 400  }},
            },
        },
        /*
        *
        * cmbLuxInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]lux
                /// [1]cmbThresh1Lm
                /// [2]cmbThresh2Lm
                /// [3]cmbThresh1Ms
                /// [4]cmbThresh2Ms
                /// [5]cmbThresh1Svs
                /// [6]cmbThresh2Svs
                /// 0     1      2      3      4      5      6
                {50    , {3072 , 3840 , 3072 , 3840 , 3072 , 3840 }},
                {100   , {2048 , 3840 , 3072 , 3840 , 3072 , 3840 }},
                {150   , {1560 , 3840 , 3072 , 3840 , 3072 , 3840 }},
                {200   , {1024 , 3840 , 2048 , 3840 , 3072 , 3840 }},
                {250   , {768  , 3840 , 1024 , 3840 , 3072 , 3840 }},
                {300   , {512  , 3840 , 1024 , 3840 , 3072 , 3840 }},
                {350   , {512  , 3840 , 1024 , 3840 , 3072 , 3840 }},
                {400   , {512  , 3840 , 512  , 3840 , 3072 , 3840 }},
                {450   , {512  , 3840 , 512  , 3840 , 3072 , 3840 }},
                {500   , {512  , 3840 , 512  , 3840 , 3072 , 3840 }},
            },
        },
        /*
        *
        * cmbLutTab
        *
        */
        {
            10    ,  /// validSize
            {
                {
                    1024  ,  /// region
                    {
                        {
                            2   , 3   , 3   , 4   , 4   , 5   , 5   , 5   , 5   , 6   , 6   , 6   , 7   , 7   , 7   , 7   ,
                            7   , 8   , 8   , 8   , 8   , 8   , 9   , 9   , 9   , 9   , 9   , 9   , 10  , 10  , 10  , 10  ,
                            10  , 10  , 10  , 11  , 11  , 11  , 11  , 11  , 11  , 11  , 12  , 12  , 12  , 12  , 12  , 12  ,
                            12  , 12  , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 13  , 14  , 14  , 14  , 14  , 14  ,
                            14  , 14  , 14  , 14  , 14  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 15  , 16  ,
                            16  , 16  , 16  , 16  , 16  , 16  , 16  , 16  , 16  , 16  , 17  , 17  , 17  , 17  , 17  , 17  ,
                            17  , 17  , 17  , 17  , 17  , 17  , 18  , 18  , 18  , 18  , 18  , 18  , 18  , 18  , 18  , 18  ,
                            18  , 18  , 19  , 19  , 19  , 19  , 19  , 19  , 19  , 19  , 19  , 19  , 19  , 19  , 19  , 20  ,
                        },
                    },
                },
                {
                    2048  ,  /// region
                    {
                        {
                            2   , 3   , 4   , 5   , 5   , 6   , 6   , 7   , 7   , 8   , 8   , 8   , 9   , 9   , 9   , 10  ,
                            10  , 10  , 11  , 11  , 11  , 11  , 12  , 12  , 12  , 12  , 13  , 13  , 13  , 13  , 14  , 14  ,
                            14  , 14  , 14  , 15  , 15  , 15  , 15  , 15  , 16  , 16  , 16  , 16  , 16  , 17  , 17  , 17  ,
                            17  , 17  , 17  , 18  , 18  , 18  , 18  , 18  , 18  , 19  , 19  , 19  , 19  , 19  , 19  , 20  ,
                            20  , 20  , 20  , 20  , 20  , 20  , 21  , 21  , 21  , 21  , 21  , 21  , 21  , 22  , 22  , 22  ,
                            22  , 22  , 22  , 22  , 22  , 23  , 23  , 23  , 23  , 23  , 23  , 23  , 23  , 24  , 24  , 24  ,
                            24  , 24  , 24  , 24  , 24  , 25  , 25  , 25  , 25  , 25  , 25  , 25  , 25  , 26  , 26  , 26  ,
                            26  , 26  , 26  , 26  , 26  , 26  , 27  , 27  , 27  , 27  , 27  , 27  , 27  , 27  , 27  , 27  ,
                        },
                    },
                },
                {
                    4096  ,  /// region
                    {
                        {
                            3   , 4   , 6   , 7   , 7   , 8   , 9   , 10  , 10  , 11  , 11  , 12  , 12  , 13  , 13  , 14  ,
                            14  , 14  , 15  , 15  , 16  , 16  , 16  , 17  , 17  , 17  , 18  , 18  , 18  , 19  , 19  , 19  ,
                            19  , 20  , 20  , 20  , 21  , 21  , 21  , 21  , 22  , 22  , 22  , 22  , 23  , 23  , 23  , 23  ,
                            24  , 24  , 24  , 24  , 25  , 25  , 25  , 25  , 26  , 26  , 26  , 26  , 26  , 27  , 27  , 27  ,
                            27  , 27  , 28  , 28  , 28  , 28  , 28  , 29  , 29  , 29  , 29  , 29  , 30  , 30  , 30  , 30  ,
                            30  , 31  , 31  , 31  , 31  , 31  , 31  , 32  , 32  , 32  , 32  , 32  , 33  , 33  , 33  , 33  ,
                            33  , 33  , 34  , 34  , 34  , 34  , 34  , 34  , 35  , 35  , 35  , 35  , 35  , 35  , 36  , 36  ,
                            36  , 36  , 36  , 36  , 36  , 37  , 37  , 37  , 37  , 37  , 37  , 38  , 38  , 38  , 38  , 38  ,
                        },
                    },
                },
                {
                    8192  ,  /// region
                    {
                        {
                            1   , 4   , 6   , 8   , 9   , 10  , 11  , 13  , 14  , 15  , 15  , 16  , 17  , 18  , 19  , 19  ,
                            20  , 21  , 22  , 22  , 23  , 23  , 24  , 25  , 25  , 26  , 26  , 27  , 27  , 28  , 29  , 29  ,
                            30  , 30  , 31  , 31  , 32  , 32  , 32  , 33  , 33  , 34  , 34  , 35  , 35  , 36  , 36  , 36  ,
                            37  , 37  , 38  , 38  , 38  , 39  , 39  , 40  , 40  , 40  , 41  , 41  , 41  , 42  , 42  , 43  ,
                            43  , 43  , 44  , 44  , 44  , 45  , 45  , 45  , 46  , 46  , 46  , 47  , 47  , 47  , 48  , 48  ,
                            48  , 49  , 49  , 49  , 50  , 50  , 50  , 51  , 51  , 51  , 51  , 52  , 52  , 52  , 53  , 53  ,
                            53  , 54  , 54  , 54  , 54  , 55  , 55  , 55  , 56  , 56  , 56  , 56  , 57  , 57  , 57  , 57  ,
                            58  , 58  , 58  , 59  , 59  , 59  , 59  , 60  , 60  , 60  , 60  , 61  , 61  , 61  , 61  , 62  ,
                        },
                    },
                },
                {
                    16384 ,  /// region
                    {
                        {
                            5   , 9   , 11  , 13  , 15  , 17  , 18  , 19  , 20  , 22  , 23  , 24  , 25  , 26  , 27  , 27  ,
                            28  , 29  , 30  , 31  , 32  , 32  , 33  , 34  , 34  , 35  , 36  , 36  , 37  , 38  , 38  , 39  ,
                            40  , 40  , 41  , 41  , 42  , 43  , 43  , 44  , 44  , 45  , 45  , 46  , 46  , 47  , 47  , 48  ,
                            48  , 49  , 49  , 50  , 50  , 51  , 51  , 52  , 52  , 53  , 53  , 53  , 54  , 54  , 55  , 55  ,
                            56  , 56  , 56  , 57  , 57  , 58  , 58  , 59  , 59  , 59  , 60  , 60  , 61  , 61  , 61  , 62  ,
                            62  , 62  , 63  , 63  , 64  , 64  , 64  , 65  , 65  , 65  , 66  , 66  , 67  , 67  , 67  , 68  ,
                            68  , 68  , 69  , 69  , 69  , 70  , 70  , 70  , 71  , 71  , 71  , 72  , 72  , 72  , 73  , 73  ,
                            73  , 74  , 74  , 74  , 75  , 75  , 75  , 76  , 76  , 76  , 77  , 77  , 77  , 77  , 78  , 78  ,
                        },
                    },
                },
                {
                    32768 ,  /// region
                    {
                        {
                            7   , 12  , 16  , 18  , 21  , 23  , 25  , 27  , 28  , 30  , 32  , 33  , 34  , 36  , 37  , 38  ,
                            40  , 41  , 42  , 43  , 44  , 45  , 46  , 47  , 48  , 49  , 50  , 51  , 52  , 53  , 54  , 55  ,
                            56  , 56  , 57  , 58  , 59  , 60  , 60  , 61  , 62  , 63  , 63  , 64  , 65  , 66  , 66  , 67  ,
                            68  , 68  , 69  , 70  , 71  , 71  , 72  , 72  , 73  , 74  , 74  , 75  , 76  , 76  , 77  , 78  ,
                            78  , 79  , 79  , 80  , 81  , 81  , 82  , 82  , 83  , 83  , 84  , 85  , 85  , 86  , 86  , 87  ,
                            87  , 88  , 88  , 89  , 89  , 90  , 90  , 91  , 91  , 92  , 93  , 93  , 94  , 94  , 95  , 95  ,
                            96  , 96  , 97  , 97  , 97  , 98  , 98  , 99  , 99  , 100 , 100 , 101 , 101 , 102 , 102 , 103 ,
                            103 , 104 , 104 , 104 , 105 , 105 , 106 , 106 , 107 , 107 , 108 , 108 , 108 , 109 , 109 , 110 ,
                        },
                    },
                },
                {
                    65536 ,  /// region
                    {
                        {
                            13  , 20  , 25  , 29  , 32  , 35  , 37  , 40  , 42  , 44  , 46  , 48  , 50  , 52  , 54  , 55  ,
                            57  , 59  , 60  , 62  , 63  , 65  , 66  , 67  , 69  , 70  , 71  , 72  , 74  , 75  , 76  , 77  ,
                            78  , 79  , 81  , 82  , 83  , 84  , 85  , 86  , 87  , 88  , 89  , 90  , 91  , 92  , 93  , 94  ,
                            95  , 96  , 97  , 98  , 98  , 99  , 100 , 101 , 102 , 103 , 104 , 105 , 105 , 106 , 107 , 108 ,
                            109 , 109 , 110 , 111 , 112 , 113 , 113 , 114 , 115 , 116 , 116 , 117 , 118 , 119 , 119 , 120 ,
                            121 , 122 , 122 , 123 , 124 , 125 , 125 , 126 , 127 , 127 , 128 , 129 , 129 , 130 , 131 , 131 ,
                            132 , 133 , 133 , 134 , 135 , 135 , 136 , 137 , 137 , 138 , 138 , 139 , 140 , 140 , 141 , 142 ,
                            142 , 143 , 143 , 144 , 145 , 145 , 146 , 146 , 147 , 148 , 148 , 149 , 149 , 150 , 151 , 151 ,
                        },
                    },
                },
                {
                    131072,  /// region
                    {
                        {
                            30  , 39  , 45  , 50  , 55  , 58  , 62  , 65  , 68  , 71  , 74  , 76  , 79  , 81  , 83  , 86  ,
                            88  , 90  , 92  , 94  , 96  , 98  , 99  , 101 , 103 , 105 , 106 , 108 , 110 , 111 , 113 , 114 ,
                            116 , 117 , 119 , 120 , 122 , 123 , 124 , 126 , 127 , 129 , 130 , 131 , 132 , 134 , 135 , 136 ,
                            137 , 139 , 140 , 141 , 142 , 143 , 145 , 146 , 147 , 148 , 149 , 150 , 151 , 153 , 154 , 155 ,
                            156 , 157 , 158 , 159 , 160 , 161 , 162 , 163 , 164 , 165 , 166 , 167 , 168 , 169 , 170 , 171 ,
                            172 , 173 , 174 , 175 , 176 , 177 , 177 , 178 , 179 , 180 , 181 , 182 , 183 , 184 , 185 , 186 ,
                            186 , 187 , 188 , 189 , 190 , 191 , 192 , 192 , 193 , 194 , 195 , 196 , 197 , 197 , 198 , 199 ,
                            200 , 201 , 201 , 202 , 203 , 204 , 205 , 205 , 206 , 207 , 208 , 208 , 209 , 210 , 211 , 212 ,
                        },
                    },
                },
                {
                    262144,  /// region
                    {
                        {
                            30  , 39  , 45  , 50  , 55  , 58  , 62  , 65  , 68  , 71  , 74  , 76  , 79  , 81  , 83  , 86  ,
                            88  , 90  , 92  , 94  , 96  , 98  , 99  , 101 , 103 , 105 , 106 , 108 , 110 , 111 , 113 , 114 ,
                            116 , 117 , 119 , 120 , 122 , 123 , 124 , 126 , 127 , 129 , 130 , 131 , 132 , 134 , 135 , 136 ,
                            137 , 139 , 140 , 141 , 142 , 143 , 145 , 146 , 147 , 148 , 149 , 150 , 151 , 153 , 154 , 155 ,
                            156 , 157 , 158 , 159 , 160 , 161 , 162 , 163 , 164 , 165 , 166 , 167 , 168 , 169 , 170 , 171 ,
                            172 , 173 , 174 , 175 , 176 , 177 , 177 , 178 , 179 , 180 , 181 , 182 , 183 , 184 , 185 , 186 ,
                            186 , 187 , 188 , 189 , 190 , 191 , 192 , 192 , 193 , 194 , 195 , 196 , 197 , 197 , 198 , 199 ,
                            200 , 201 , 201 , 202 , 203 , 204 , 205 , 205 , 206 , 207 , 208 , 208 , 209 , 210 , 211 , 212 ,
                        },
                    },
                },
                {
                    524288,  /// region
                    {
                        {
                            30  , 39  , 45  , 50  , 55  , 58  , 62  , 65  , 68  , 71  , 74  , 76  , 79  , 81  , 83  , 86  ,
                            88  , 90  , 92  , 94  , 96  , 98  , 99  , 101 , 103 , 105 , 106 , 108 , 110 , 111 , 113 , 114 ,
                            116 , 117 , 119 , 120 , 122 , 123 , 124 , 126 , 127 , 129 , 130 , 131 , 132 , 134 , 135 , 136 ,
                            137 , 139 , 140 , 141 , 142 , 143 , 145 , 146 , 147 , 148 , 149 , 150 , 151 , 153 , 154 , 155 ,
                            156 , 157 , 158 , 159 , 160 , 161 , 162 , 163 , 164 , 165 , 166 , 167 , 168 , 169 , 170 , 171 ,
                            172 , 173 , 174 , 175 , 176 , 177 , 177 , 178 , 179 , 180 , 181 , 182 , 183 , 184 , 185 , 186 ,
                            186 , 187 , 188 , 189 , 190 , 191 , 192 , 192 , 193 , 194 , 195 , 196 , 197 , 197 , 198 , 199 ,
                            200 , 201 , 201 , 202 , 203 , 204 , 205 , 205 , 206 , 207 , 208 , 208 , 209 , 210 , 211 , 212 ,
                        },
                    },
                },
            },
        },
    },
    /*
    *
    * sde
    *
    */
    {
        1     ,  /// sdeTuneEnable
        0     ,  /// sdeNegEn
        1024  ,  /// sdeBrightness
        1024  ,  /// sdeSaturation
        0     ,  /// sdeHue
        0     ,  /// sdeClipYLow
        0     ,  /// sdeClipYHigh
        1024  ,  /// sdeContrastY
        1024  ,  /// sdeContrastU
        1024  ,  /// sdeContrastV
        /*
        *
        * sdeLuxInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]lux
                /// [1]sdeContrastY
                /// 0     1
                {50    , {1200 }},
                {100   , {1200 }},
                {150   , {1200 }},
                {200   , {1200 }},
                {250   , {1200 }},
                {300   , {1200 }},
                {350   , {1100 }},
                {400   , {1200 }},
                {450   , {1100 }},
                {500   , {1100 }},
            },
        },
    },
    /*
    *
    * rir
    *
    */
    {
        1     ,  /// rirTuneEnable
        0     ,  /// rirDirMeanEn
        1     ,  /// rirGradFilterEn
        0     ,  /// rirInArray
        50    ,  /// rirGradMeanThd1
        250   ,  /// rirGradMeanThd2
        50    ,  /// rirGradVarThd1
        225   ,  /// rirGradVarThd2
        100   ,  /// rirGradSimVar
        1     ,  /// rirOverExpEn
        3500  ,  /// rirOverExpThd1
        3800  ,  /// rirOverExpThd2
        256   ,  /// rirOverExpRGain
        256   ,  /// rirOverExpBGain
        256   ,  /// rirOverExpDesatStr
        100   ,  /// rirOverExpDifHueMin
        1     ,  /// rirBlcInEn
        1     ,  /// rirBlcOutEn
        0     ,  /// rirDebugMode
    },
},
/*
*
* reserved
*
*/
{
    {
        2   , 0   , 2   , 1   , 0   , 6   , 0   , 1   , 0   , 0   ,
    },
},
#endif
