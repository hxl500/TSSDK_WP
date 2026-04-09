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

#ifndef __TUNNING_IMX307_NORMAL_H__
#define __TUNNING_IMX307_NORMAL_H__

/*
*
* tuneVersion
*
*/
{
    0, 0, 0, 63848, /// major, minor, debug, check
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
        0, 1, 0, 8540, /// major, minor, debug, check
    },

    /*
    *
    * ae: AEC tuning params
    *
    */
    {
        1,                  /// aeEnable; 0 - disable, 1 - enable
        0,                  /// aeHdrExpBaseSel;
        2,                  /// aeStatsSel
        0,                  /// antiFlickerEnable
        50,                 /// antiFlickerFrequency
        5488 ,              /// sensitivityRatio
        1,                  /// exposureTableEnable
        /*
        *
        * aeRoi
        *
        */
        {
            0,              /// aeManualRoiEnable;
            {1, 1, 13, 13}, /// Roi left top width height
        },

        /*
        *
        * aeWeightTab
        *
        */
        {
            0,              /// aeWeightTabSel
            {
                {
                    /// [0]center weight table
                    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                    0,  7,  8,  8,  9,  9, 10, 10, 10,  9,  9,  8,  8,  7,  0,
                    0,  8,  9,  9, 10, 11, 11, 11, 11, 11, 10,  9,  9,  8,  0,
                    0,  8,  9, 10, 11, 12, 12, 12, 12, 12, 11, 10,  9,  8,  0,
                    0,  9, 10, 11, 12, 13, 13, 13, 13, 13, 12, 11, 10,  9,  0,
                    0,  9, 11, 12, 13, 13, 14, 14, 14, 13, 13, 12, 11,  9,  0,
                    0, 10, 11, 12, 13, 14, 14, 14, 14, 14, 13, 12, 11, 10,  0,
                    0, 10, 11, 12, 13, 14, 14, 15, 14, 14, 13, 12, 11, 10,  0,
                    0, 10, 11, 12, 13, 14, 14, 14, 14, 14, 13, 12, 11, 10,  0,
                    0,  9, 11, 12, 13, 13, 14, 14, 14, 13, 13, 12, 11,  9,  0,
                    0,  9, 10, 11, 12, 13, 13, 13, 13, 13, 12, 11, 10,  9,  0,
                    0,  8,  9, 10, 11, 12, 12, 12, 12, 12, 11, 10,  9,  8,  0,
                    0,  8,  9,  9, 10, 11, 11, 11, 11, 11, 10,  9,  9,  8,  0,
                    0,  7,  8,  8,  9,  9, 10, 10, 10,  9,  9,  8,  8,  7,  0,
                    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                },
                {
                    /// [1]average weight table
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
                    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
                    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
                    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
                    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
                    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
                    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
                    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
                    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
                    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
                    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
                    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
                    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                },
                {
                    /// [2] up to down
                    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                    0, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  0,
                    0, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,  0,
                    0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,
                    0, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  0,
                    0, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,  0,
                    0, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  0,
                    0,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  0,
                    0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,
                    0,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  0,
                    0,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  0,
                    0,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  0,
                    0,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  0,
                    0,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  0,
                    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                },
                {
                    /// [3] down to up
                    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                    0,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  0,
                    0,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  0,
                    0,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  0,
                    0,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  0,
                    0,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  0,
                    0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0,
                    0,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  0,
                    0, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  0,
                    0, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,  0,
                    0, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  0,
                    0, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  0,
                    0, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,  0,
                    0, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  0,
                    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                },
                {
                    /// [4] left to right
                    0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 0,
                    0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 0,
                    0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 0,
                    0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 0,
                    0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 0,
                    0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 0,
                    0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 0,
                    0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 0,
                    0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 0,
                    0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 0,
                    0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 0,
                    0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 0,
                    0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 0,
                    0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
                },
                {
                    /// [5] right to left
                    0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0, 0,
                    0, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0,
                    0, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0,
                    0, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0,
                    0, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0,
                    0, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0,
                    0, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0,
                    0, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0,
                    0, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0,
                    0, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0,
                    0, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0,
                    0, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0,
                    0, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0,
                    0, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0, 0,
                },
            },
        },
        /*
        *
        * aeConvParam
        *
        */
        {
            6,        /// default 6,[3:32] AE convergence speed
            60,       /// default 60,[30 : 200] 100 base percent
            4,        /// default 4,[2:255] AE convergence stability range in
            64,       /// default 32,[5:512] AE convergence stability range out
            960,      /// default 960,[0:1023] AE Saturation Threshold, use to calculation linaer Y
            3,        /// default 3,[0,3] which stats Y use to judge ae stable out
            30,       /// default 15,[0, 4096] AE change time out frame
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
            3     ,  /// validSize
            {
                /// [0]index 0,1,2,...10
                /// [1]frameRate
                /// [2]totalGain
                /// [3]incrementPriority; 0: exp time 1:gain
                /// 0     1      2      3
                {0     , {100000, 1024 , 0    }},
                {1     , {25   , 1024 , 1    }},
                {2     , {25   , 524288, 0    }},
            },
        },
    },
    /*
    *
    * awb: AWB tunning params
    *
    */
    {
        1,              /// awbEnable; 0 - disable, 1 - enable
        10,             /// awbConvergeStep   (AWB Convergance Step)
        1,              /// awbStableRange    (Stable Range)
        1,              /// awbLumaWeightEnable (enable luma weight)
        256,            /// awbPrefGainR      (Shift R Gain)
        256,            /// awbPrefGainB      (Shift B Gain)
        400,            /// awbLowlightLuxThr (Lowlight lux index threshold)
        3200,           /// awbLowctRgPosThr  (RGPos lowCT threshold)
        4     ,  /// awbDarkPixelThr
        4090  ,  /// awbSatPixelThr
        50    ,  /// awbDarkPrcThr
        25  ,    /// awbSatPrcThr
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
            1,  /// awbValidSize
            {
                /// [0]index 0,1,2,3 ..9
                /// [1]default 0 [0: 6] reference light source index
                /// [2]enable (0:diable, 1:enable)
                /// [3]lux index low thr(0:1000)
                /// [4]lux index high thr(0:1000)
                /// [5]lux index interpolation left and right buf(0:255)
                /// [6]default 256 [0: 512] light source area left scale 256 is 1x, 512 is up scale 2x, 0 is down scale 2x
                /// [7]default 256 [0: 512] light source area right scale 256 is 1x, 512 is up scale 2x, 0 is down scale 2x
                /// [8]default 256 [0: 512] light source area top scale 256 is 1x, 512 is up scale 2x, 0 is down scale 2x
                /// [9]default 256 [0: 512] light source area bot scale 256 is 1x, 512 is up scale 2x, 0 is down scale 2x
                ///    0      1     2    3      4   5      6     7     8     9
                {      0,    {1,    0,   0, 1000,  20,  256,  256,  256,  256}},   /// H
            },
        },
        /*
        *
        * awbDetectZone
        *
        */
        {
            1,  /// awbValidDetectZone
            {
                /// [0] index 0,1,2,3 ..9
                /// [1]enable (0:diable, 1:enable)
                /// [2]lux index low thr(0:1000)
                /// [3]lux index high thr(0:1000)
                /// [4]lux index interpolation left and right buf(0:255)
                /// [5]zone left [-1000:1000]
                /// [6]zone right [-1000:1000]
                /// [7]zone top [-1000:1000]
                /// [8]zone bottom [-1000:1000]
                ///    0      1     2      3   4     5     6     7      8
                {      0,    {1,  190,  250, 20,  -53,  114, -234, -265}}, /// sunset glow
            },
        },
        /*
        *
        * awbMisleadColorZone
        *
        */
        {
            3,  /// awbValidZone
            {
                /// [0] index 0,1,2,3 .. 9
                /// [1]enable (0:diable, 1:enable)
                /// [2]detectColorIdx(0:16) 0:6 is H/A/TL84/CWF/D65/DF/SHADE
                /// [3]lux index low thr(0:1000)
                /// [4]lux index high thr(0:1000)
                /// [5]lux index interpolation left and right buf(0:255)
                /// [6]detect point percent threshold [0:1024]
                /// [7]detect white point percent buffer [0:512]
                /// [8]zone left [-1000:1000]
                /// [9]zone right [-1000:1000]
                /// [10]zone top [-1000:1000]
                /// [11]zone bottom [-1000:1000]
                ///    0      1     2     3   4     5      6      7     8     9    10    11
                {      0,    {1,   6,    0, 1000, 10,    40,    50,   -78,   52, -225, -248}}, /// purple
                {      1,    {1,   2,    0, 1000, 10,   200,    50,  -120,  -25, -234, -270}}, /// blue cloth
                {      2,    {1,   7,  190,  250, 20,   300,    50,    -8,  104, -267, -319}},  /// green tree in sunset glow
            },
        },

        /*
        *
        * awbPrefColorZone
        *
        */
        {
            2,  /// awbValidPrefZone
            {
                /// [0] index 0,1,2,3 .. 10
                /// [1]PrefZone enable (0:diable, 1:enable)
                /// [2]ref light source index [0:6] H/A/TL84/CWF/D65/DF/SHADE
                /// [3]lux index low thr(0:1000)
                /// [4]lux index high thr(0:1000)
                /// [5]lux index interpolation left and right buf(0:255)
                /// [6]percent of light source area width, x large than Thr then clip x,(0:256) 256 is 100%
                /// [7]percent of light source area height, y large than Thr then clip y,(0:256) 256 is 100%
                /// [8]X offset clip ratio (0:256) 256 is 100%
                /// [9]Y offset clip ratio (0:256) 256 is 100%
                /// [10]shift R gain 256 base (128:512)
                /// [11]shift B gain 256 base (128:512)
                ///    0      1     2       3    4   5    6     7      8     9    10    11
                {      0,    {1,    0,   240, 400, 10,    96,  128,   128, 128, 256, 256}},
                {      1,    {1,    1,   240, 400, 10,    96,  128,   128, 128, 256, 256}},
            },
        },

        /*
        *
        * awbLightsourceProb
        *
        */
        {
            2,  /// valid size
            {
                /// [0] lux index
                /// [1]H prob[0:256]
                /// [2]A prob[0:256]
                /// [3]TL84 prob[0:256]
                /// [4]CWF prob[0:256]
                /// [5]D65 prob[0:256]
                /// [6]DF prob[0:256]
                /// [7]Shade prob[0:256]
                ///    0      1     2    3     4       5     6     7
                {120    , {   96,  128,  192,  128,  256,  128,  256}},
                {180    , {  256,  256,  256,  256,  256,  256,  128}},
            },
        },
        /*
        *
        * awbStatisticsShift
        *
        */
        {
            7,  /// awbValid light source
            {
                /// [0] index 0,1,2,3 .. 9
                /// [1]default 0, [0]disable, [1]enable
                /// [2]light source index [0:6]
                /// [3]lux index region start [1:1000]
                /// [4]lux index region end [1:1000]
                /// [5]lux index left and right buffer [1:256]
                /// [6]default 0[0: 256] limitY threshold percent
                /// [7]default 0[0: 256] weight reduce percent 256 is 100%
                ///    0      1     2    3     4    5     6   7
                {      0,    {1,   0,    0, 1000,  10,  32,  256}}, ///H
                {      1,    {1,   1,    0, 1000,  10,  32,  256}}, /// A
                {      2,    {1,   2,    0, 1000,  10,  32,  256}}, /// TL84
                {      3,    {1,   3,    0, 1000,  10,  64,  256}}, /// CWF
                {      4,    {1,   4,    0, 1000,  10,  32,  256}}, /// D65
                {      5,    {1,   5,    0, 1000,  10,  64,  256}}, /// DF
                {      6,    {1,   6,    0, 1000,  10,  32,  256}}, /// SHADE
            },
        },

        /*
        *
        * awbExtraColorZone
        *
        */
        {
            1,  /// awbValidZone
            {
                /// [0]index 0,1,2,3 .. 9
                /// [1]enable (0:diable, 1:enable)
                /// [2]lux index low thr(0:1000)
                /// [3]lux index high thr(0:1000)
                /// [4]lux index interpolation left and right buf(0:255)
                /// [5]detect point percent threshold [0:1024]
                /// [6]detect white point percent buffer [0:512]
                /// [7]zone left [-1000:1000]
                /// [8]zone right [-1000:1000]
                /// [9]zone top [-1000:1000]
                /// [10]zone bottom [-1000:1000]
                /// [11]weight [0:256]
                /// [12]preference R Gain [128:512]
                /// [13]preference B Gain [128:512]
                ///    0      1     2     3   4     5      6      7     8     9    10    11  12   13
                {      0,    {0,    400, 500, 10,   100,    50,   112,  344, -383, -451, 7, 256, 256, 256}}, /// extrame dark scene greenish
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
                /// 0          1      2      3      4      5      6      7      8      9      10     11     12     13     14
                {   100,    {361  , 632  , 361  , 632  , 400  , 553  , 452  , 460  , 432  , 501  , 432  , 501  , 432  , 501  }},
                {   150,    {361  , 632  , 361  , 632  , 432  , 500  , 452  , 480  , 432  , 501  , 432  , 501  , 432  , 501  }},
                {   250,    {361  , 632  , 361  , 632  , 432  , 530  , 432  , 520  , 432  , 501  , 432  , 501  , 432  , 501  }},
            },
        },

        /*
        *
        * awbSpecialGainWeight
        *
        */
        {
            2,  /// valid size
            {
                /// [0] lux index
                /// [1]H weight[0:256]
                /// [2]A weight[0:256]
                /// [3]TL84 weight[0:256]
                /// [4]CWF weight[0:256]
                /// [5]D65 weight[0:256]
                /// [6]DF weight[0:256]
                /// [7]Shade weight[0:256]
                ///    0      1     2    3     4       5     6     7
                {120    , {   0,  0,  0,  0,  0,  0,  0}},
                {180    , {  0,  0,  0,  0,  0,  0,  0}},
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
            10    ,  /// validSize
            {
                /// [0] lux index
                /// [1] neutralPointPrcThrL[0:256]
                /// [2] neutralPointPrcThrH[0:256]
                /// [3] default gain weight[0:256]
                /// [4] default R gain[256:2048]
                /// [5] default G gain[256:2048]
                /// [6] default B gain[256:2048]
                /// [7] enqueNeutralPointThr[0:256]
                /// [8] enqueCWFDFThr[0:256]
                /// 0     1      2      3      4      5      6      7      8
                {50    , {10   , 20   , 128  , 512  , 256  , 512  , 128  , 128   }},
                {100   , {10   , 20   , 128  , 512  , 256  , 512  , 128  , 128   }},
                {150   , {10   , 20   , 128  , 512  , 256  , 512  , 128  , 128   }},
                {200   , {10   , 20   , 128  , 512  , 256  , 512  , 128  , 128   }},
                {250   , {10   , 20   , 128  , 512  , 256  , 512  , 128  , 128   }},
                {300   , {10   , 20   , 128  , 512  , 256  , 512  , 128  , 128   }},
                {350   , {10   , 20   , 128  , 512  , 256  , 512  , 128  , 128   }},
                {400   , {10   , 20   , 128  , 512  , 256  , 512  , 128  , 128   }},
                {450   , {10   , 20   , 128  , 512  , 256  , 512  , 128  , 128   }},
                {500   , {10   , 20   , 128  , 512  , 256  , 512  , 128  , 128   }},
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
            0    ,  /// face awb enable
            1    ,  /// enable exclude face roi white point
            40   ,  /// face awb max weight
            8    ,  /// face reference target tolerance range
            120  ,  /// faceScale, [10, 1000] 100 base
            60   ,  /// face size ratio threshold low, 1024 base
            100  ,  /// face size ratio threshold high, 1024 base
            50   ,  /// distance low threshold between face XY and face target
            200  ,  /// distance high threshold between face XY and face target
            256  ,  /// face awb preference r gain
            256  ,  /// face awb preference b gain
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
        4     ,  /// afBackjumpCompstep
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
        3800  ,  /// afBloomingThr
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
        40 , /// afMotorZoomInit
        /*
        *
        * afZoomFocusMappingTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]afZoomVal
                /// [1]afNearEndPos
                /// [2]afFarEndPos
                /// [3]afNearMarginPercent
                /// [4]afFarMarginPercent
                /// [5]afMoveStep
                /// 0     1      2      3      4      5
                {30    , {320  , 380  , 30   , 50   , 1    }},
                {40    , {260  , 320  , 30   , 50   , 1    }},
                {50    , {200  , 260  , 30   , 50   , 1    }},
                {60    , {160  , 220  , 45   , 35   , 1    }},
                {70    , {120  , 180  , 45   , 35   , 1    }},
                {80    , {90   , 150  , 45   , 35   , 1    }},
                {90    , {80   , 140  , 45   , 35   , 1    }},
                {100   , {70   , 130  , 45   , 35   , 1    }},
                {110   , {60   , 120  , 45   , 35   , 1    }},
                {120   , {50   , 110  , 45   , 35   , 1    }},
            },
        },
        /*
        *
        * afParamByLux
        *
        */
        {
            2    ,  /// validSize
            {
                /// [0]lux index
                /// [1]afLumaChangeThr
                /// [2]afLumaChangeCntThr
                /// [3]afLumaStableThr
                /// [4]afLumaStableCntThr
                /// [5]afExposureChgThr
                /// [6]afFvChangeEnable
                /// [7]afFvChangeThr
                /// [8]afFvChangeCntThr
                /// [9]afFvStableThr
                /// [10]afFvStableCntThr
                /// [11]afFvIncRatio
                /// [12]afFvDecRatio
                /// [13]afIncDecPeakRatio
                /// [14]afDecDecPeakRatio
                /// [15]roiScale
                /// 0     1     2    3    4     5     6    7    8     9     10      11        12       13      14
                {300    , {15,   6  , 3  , 6   , 5   , 1,   20,  2,   10,     5,     1060,     980,     86,     86, 100}},
                {400    , {30,   6  , 3  , 6   , 5   , 1,   20,  2,   10,     5,     1060,     980,     92,     90, 200}},
            },
        },
        /*
        *
        * faceAfRoiParam
        *
        */
        {
            1    ,  /// validSize
            {
                /// [0]lux index
                /// [1]roiScale
                /// [2]topRatio
                /// [3]botRatio
                /// [4]leftRatio
                /// [5]rightRatio
                /// [6]faceInCntThr
                /// [7]faceOutCntThr
                /// [8]faceSmallSizeThr
                /// 0     1       2     3      4        5      6    7    8
                {200    , {100,   100  , 100  , 100   , 100   , 4,   8,  10}},
            },
        },

    },
},
/*
*
* ispParams tuning params
*
*/
{
    /*
    *
    * ispTuneVersion
    *
    */
    {
        0, 1, 0, 47916, /// major, minor, debug, check
    },

    /*
    *
    * gainRange
    *
    */
    {
        0x4200,     /// maxDigitalGain
        0x1000,     /// minDigitalGain
        0x400,      /// maxRChannelGain
        0x100,      /// minRChannelGain
        0x400,      /// maxGChannelGain
        0x100,      /// minGChannelGain
        0x700,      /// maxBChannelGain
        0x100,      /// minBChannelGain
    },

    /*
    *
    * blc
    *
    */
    {
        1,          /// blcTuneEnable

        /*
        *
        * blcGainInterpTab
        *
        */
        {
            10, /// validSize
            {
                /// [0]  gain
                /// [1]  normalBlcR
                /// [2]  normalBlcGr
                /// [3]  normalBlcGb
                /// [4]  normalBlcB
                /// [5]  lncBlcL
                /// [6]  lncBlcM
                /// [7]  lncBlcS
                /// [8]  lncBlcVs
                /// [9]  dolLBlcR
                /// [10] dolLBlcGr
                /// [11] dolLBlcGb
                /// [12] dolLBlcB
                /// [13] dolMBlcR
                /// [14] dolMBlcGr
                /// [15] dolMBlcGb
                /// [16] dolMBlcB
                /// [17] dolSBlcR
                /// [18] dolSBlcGr
                /// [19] dolSBlcGb
                /// [20] dolSBlcB
                ///     0           1       2       3       4       5     6     7     8       9      10       11     12      13      14      15      16      17      18      19      20
                {   1 * 1024,    {61129,  61131,  61129,  61131,    0,    0,    0,    0,    61381,  61382,  61378,  61379,  61432,  61433,  61431,  61433,  61432,  61433,  61431,  61433}},
                {   2 * 1024,    {60862,  60864,  60861,  60864,    0,    0,    0,    0,    61111,  61113,  61111,  61114,  61486,  61487,  61481,  61482,  61486,  61487,  61481,  61482}},
                {   4 * 1024,    {60576,  60578,  60561,  60567,    0,    0,    0,    0,    61152,  61153,  61143,  61147,  61437,  61352,  61351,  61356,  61347,  61352,  61351,  61356}},
                {   8 * 1024,    {59367,  59375,  59367,  59374,    0,    0,    0,    0,    60922,  60924,  60911,  60917,  61306,  61312,  61318,  61325,  61306,  61312,  61318,  61325}},
                {  16 * 1024,    {57570,  57571,  57569,  57595,    0,    0,    0,    0,    60428,  60442,  60403,  60417,  61245,  61248,  61238,  61248,  61245,  61248,  61238,  61248}},
                {  32 * 1024,    {54612,  54636,  54645,  54681,    0,    0,    0,    0,    58874,  58894,  58902,  58926,  61960,  61973,  61938,  61978,  61960,  61973,  61938,  61978}},
                {  64 * 1024,    {46935,  46973,  46931,  47004,    0,    0,    0,    0,    56836,  56882,  56855,  56930,  62202,  62272,  62272,  62344,  62202,  62272,  62272,  62344}},
                { 128 * 1024,    {33584,  33639,  34045,  34168,    0,    0,    0,    0,    51473,  51618,  51673,  51781,  62885,  62979,  63030,  63168,  62885,  62979,  63030,  63168}},
                { 256 * 1024,    {33584,  33639,  34045,  34168,    0,    0,    0,    0,    51473,  51618,  51673,  51781,  62885,  62979,  63030,  63168,  62885,  62979,  63030,  63168}},
                { 512 * 1024,    {33584,  33639,  34045,  34168,    0,    0,    0,    0,    51473,  51618,  51673,  51781,  62885,  62979,  63030,  63168,  62885,  62979,  63030,  63168}},
            },
        },
    },

    /*
    *
    * lsc
    *
    */
    {
        1,          /// lscTuneEnable

        /*
        *
        * lscGainInterpTab
        *
        */
        {
            10, /// validSize
            {
                /// [0]  gain
                /// [1]  lscStrength
                ///     0          1
                {   1 * 1024,   {1024}},
                {   2 * 1024,   {1024}},
                {   4 * 1024,   {1024}},
                {   8 * 1024,   {512}},
                {  16 * 1024,   {512}},
                {  32 * 1024,   {512}},
                {  64 * 1024,   {512}},
                { 128 * 1024,   {512}},
                { 256 * 1024,   {512}},
                { 512 * 1024,   {512}},
            },
        },
    },

    /*
    *
    * ltm
    *
    */
    {
        1,          /// ltmTuneEnable
        20,         /// ltmSpatialFilterTime
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
                /// [2]ltmGlobalGain
                /// 0          1      2
                {    6 * 4096, { 5    , 256 }},
                {   12 * 4096, { 5    , 256 }},
                {   18 * 4096, { 5    , 256 }},
                {   24 * 4096, { 5    , 256 }},
                {   30 * 4096, { 5    , 256 }},
                {   36 * 4096, { 5    , 256 }},
                {   42 * 4096, { 5    , 256 }},
                {   58 * 4096, {10    , 512 }},
                {   60 * 4096, {10    , 512 }},
                {   66 * 4096, {10    , 512 }},
            },
        },
        /*
        *
        * ltmGainInterpTab
        *
        */
        {
            10, /// validSize
            {
                /// [0]  gain
                /// [1]  ltmBggen
                /// [2]  ltmAmplifyalpha
                ///     0        1      2
                {   1 * 1024,   {1,   4095}},
                {   2 * 1024,   {1,   4095}},
                {   4 * 1024,   {1,   4095}},
                {   8 * 1024,   {1,   4095}},
                {  16 * 1024,   {1,   4095}},
                {  32 * 1024,   {1,   4095}},
                {  64 * 1024,   {0,      0}},
                { 128 * 1024,   {0,      0}},
                { 256 * 1024,   {0,      0}},
                { 512 * 1024,   {0,      0}},
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
                /// [1]ltmMaxGain
                /// [2]ltmAmplifyalpha
                /// [3]ltmDarkPrc
                /// [4]ltmBrightPrc
                /// [5]ltmDarkContrastMin
                /// [6]ltmDarkContrastMax
                /// [7]ltmMinLighten
                /// [8]ltmMaxLighten
                /// [9]ltmGdgEnable
                /// [10]ltmTargetLdr
                /// [11]ltmHiTargetPrc
                /// [12]ltmDarkLevel
                /// [13]ltmDarkMinPrc
                /// [14]ltmDarkMaxPrc
                /// [15]ltmDarkGainRatio
                /// [16]ltmAmplifyalphaMax
                /// [17]ltmHistOffsetMin
                /// [18]ltmHistOffsetMax
                /// 0     1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     17     18
                {50    , {256  , 1300 , 100  , 990  , 1    , 85   , 50   , 160  , 0    , 160  , 99   , 5    , 30   , 50   , 100  , 2000 , 0    , 0    }},
                {100   , {256  , 1300 , 100  , 990  , 1    , 85   , 50   , 160  , 0    , 150  , 99   , 5    , 30   , 50   , 100  , 2000 , 0    , 0    }},
                {150   , {256  , 1300 , 100  , 990  , 1    , 85   , 50   , 160  , 0    , 140  , 99   , 5    , 30   , 50   , 100  , 2000 , 0    , 0    }},
                {200   , {228  , 1300 , 100  , 990  , 1    , 85   , 50   , 160  , 0    , 130  , 99   , 5    , 30   , 50   , 100  , 2000 , 0    , 0    }},
                {250   , {200  , 1300 , 100  , 990  , 1    , 60   , 50   , 160  , 0    , 120  , 99   , 5    , 30   , 50   , 100  , 2000 , 0    , 0    }},
                {300   , {175  , 1000 , 100  , 990  , 1    , 60   , 50   , 160  , 0    , 100  , 99   , 5    , 30   , 50   , 100  , 2000 , 0    , 0    }},
                {350   , {125  , 600  , 100  , 990  , 1    , 60   , 50   , 150  , 0    , 90   , 99   , 5    , 30   , 50   , 100  , 2000 , 0    , 0    }},
                {400   , {100  , 100  , 100  , 990  , 1    , 60   , 50   , 150  , 0    , 80   , 99   , 5    , 30   , 50   , 100  , 1000 , 0    , 0    }},
                {450   , {100  , 0    , 100  , 990  , 1    , 60   , 50   , 150  , 0    , 70   , 99   , 5    , 30   , 50   , 100  , 600  , 0    , 0    }},
                {500   , {100  , 0    , 100  , 990  , 1    , 60   , 50   , 150  , 0    , 70   , 99   , 5    , 30   , 50   , 100  , 0    , 0    , 0    }},
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
                /// [1]ltmMaxGain
                /// [2]ltmAmplifyalpha
                /// [3]ltmDarkPrc
                /// [4]ltmBrightPrc
                /// [5]ltmDarkContrastMin
                /// [6]ltmDarkContrastMax
                /// [7]ltmMinLighten
                /// [8]ltmMaxLighten
                /// [9]ltmGdgEnable
                /// [10]ltmTargetLdr
                /// [11]ltmHiTargetPrc
                /// [12]ltmDarkLevel
                /// [13]ltmDarkMinPrc
                /// [14]ltmDarkMaxPrc
                /// [15]ltmDarkGainRatio
                /// [16]ltmAmplifyalphaMax
                /// [17]ltmHistOffsetMin
                /// [18]ltmHistOffsetMax
                /// 0     1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     17     18
                {50    , {256  , 2048 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 160  , 99   , 5    , 30   , 50   , 100  , 2048 , 0    , 0    }},
                {100   , {256  , 2048 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 150  , 99   , 5    , 30   , 50   , 100  , 2048 , 0    , 0    }},
                {150   , {256  , 3072 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 140  , 99   , 5    , 30   , 50   , 100  , 3072 , 0    , 0    }},
                {200   , {228  , 3072 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 130  , 99   , 5    , 30   , 50   , 100  , 3072 , 0    , 0    }},
                {250   , {176  , 3072 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 120  , 99   , 5    , 30   , 50   , 100  , 3072 , 0    , 0    }},
                {300   , {100  , 1750 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 100  , 99   , 5    , 30   , 50   , 100  , 1750 , 0    , 0    }},
                {350   , {100  , 875  , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 90   , 99   , 5    , 30   , 50   , 100  , 875  , 0    , 0    }},
                {400   , {100  , 0    , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 80   , 99   , 5    , 30   , 50   , 100  , 0    , 0    , 0    }},
                {450   , {100  , 0    , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 70   , 99   , 5    , 30   , 50   , 100  , 0    , 0    , 0    }},
                {500   , {100  , 0    , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 70   , 99   , 5    , 30   , 50   , 100  , 0    , 0    , 0    }},
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
        1,          /// ccmTuneEnable

        /*
        *
        * ccmGainInterpTab
        *
        */
        {
            10, /// validSize
            {
                /// [0]  gain
                /// [1]  ccmSaturation
                ///     0         1
                {   1 * 1024,   {256}},
                {   2 * 1024,   {256}},
                {   4 * 1024,   {256}},
                {   8 * 1024,   {240}},
                {  16 * 1024,   {220}},
                {  32 * 1024,   {200}},
                {  64 * 1024,   {180}},
                { 128 * 1024,   {160}},
                { 256 * 1024,   {156}},
                { 512 * 1024,   {128}},
            },
        },

        /*
        *
        * ccmLuxInterpTab
        *
        */
        {
            4 , /// validSize
            {
                /// [0]  lux
                /// [1]  ccmSaturation
                ///    0        1
                {     50,     {320}},
                {     100,    {300}},
                {     150,    {280}},
                {     200,    {256}},
            },
        },
    },

    /*
    *
    * ggc
    *
    */
    {
        1,          /// ggcTuneEnable
        1000,       /// ggcLowisoThr
        6400,       /// ggcHighisoThr
        10,         /// ggcConvergeSpeed
        0,          /// ggcBasicSelection
        2,          /// ggcContrastEnhanceEnable
        30,         /// ggcContrastManualStr
        0,          /// ggcDehazeStrengthLight
        0,          /// ggcDehazeStrengthDark
        50,         /// ggcLocalLumAdjNode0
        50,         /// ggcLocalLumAdjNode1
        50,         /// ggcLocalLumAdjNode2
        50,         /// ggcLocalLumAdjNode3
        50,         /// ggcLocalLumAdjNode4
        50,         /// ggcLocalLumAdjNode5
        50,         /// ggcLocalLumAdjNode6
        0,          /// ggcLocalAdaptiveNode0
        0,          /// ggcLocalAdaptiveNode1
        0,          /// ggcLocalAdaptiveNode2
        0,          /// ggcLocalAdaptiveNode3
        0,          /// ggcLocalAdaptiveNode4
        0,          /// ggcLocalAdaptiveNode5
        0,          /// ggcLocalAdaptiveNode6
        128,        /// ggcGlobalLuminationAdj
        1,          /// ggcContrastAutoMode
        250,        /// ggcContrastAutoStr
        50,         /// ggcContrastSmoothRate

        /*
        *
        * ggcLuxInterpTab
        *
        */
        {
            5, /// validSize
            {
                /// [0]  lux
                /// [1]  ggcContrastAutoStr
                /// [2]  ggcLocalLumAdjNode0
                /// [3]  ggcLocalLumAdjNode1
                /// [4]  ggcLocalLumAdjNode5
                /// [5]  ggcLocalLumAdjNode6
                ///     0         1        2       3       4       5
                {   1 * 100,    {320,     55,     50,     25,      5}},
                {   2 * 100,    {240,     55,     50,     50,     50}},
                {   3 * 100,    {230,     55,     50,     50,     50}},
                {   4 * 100,    {220,     55,     50,     50,     50}},
                {   5 * 100,    {200,     55,     50,     50,     50}},
            },
        },
    },

    /*
    *
    * dpc
    *
    */
    {
        1,     /// dpcTuneEnable

        /*
        *
        * dpcGainSwitchTab
        *
        */
        {
            10, /// validSize
            {
                /// [0]  gain
                /// [1]  dpcByp
                /// [2]  dpcGeEn
                ///     0          1       2
                {   1 * 1024,     {0,      0}},
                {   2 * 1024,     {0,      0}},
                {   4 * 1024,     {0,      0}},
                {   8 * 1024,     {0,      0}},
                {  16 * 1024,     {0,      0}},
                {  32 * 1024,     {0,      0}},
                {  64 * 1024,     {0,      0}},
                { 128 * 1024,     {0,      0}},
                { 256 * 1024,     {0,      0}},
                { 512 * 1024,     {0,      0}},
            },
        },

        /*
        *
        * dpcGainInterpTab
        *
        */
        {
            10, /// validSize
            {
                /// [0]  gain
                /// [1]  dpcIntplAlpha
                /// [2]  dpcDivSlp
                /// [3]  dpcDivThr
                /// [4]  dpcStdevMaskThr
                /// [5]  dpcDpDisThr
                /// [6]  dpcCurMedThr
                /// [7]  dpcGeStr
                /// [8]  dpcGeThr
                /// [9]  dpcGeSen
                /// [10]  dpcGeSlope
                /// [11]  dpcNpGain
                /// [12]  dpcNpOffset
                ///     0          1       2       3       4       5       6       7       8       9      10      11      12
                {   1 * 1024,     {0,      1,      0,     10,    200,     16,    100,      0,    256,    256,     64,     0}},
                {   2 * 1024,     {0,      1,      0,     10,    200,     16,    100,      0,    256,    256,     64,     0}},
                {   4 * 1024,     {0,      1,      0,     10,    200,     16,    100,      0,    256,    256,     64,     0}},
                {   8 * 1024,     {0,      1,      0,     10,    200,     16,    100,      0,    256,    256,     64,     0}},
                {  16 * 1024,     {0,      1,      0,     10,    200,     16,    100,      0,    256,    256,     64,     0}},
                {  32 * 1024,     {0,      1,      0,     10,    200,     16,    100,      0,    256,    256,     64,     0}},
                {  64 * 1024,     {0,      1,      0,     10,    200,     16,    100,      0,    256,    256,     64,     0}},
                { 128 * 1024,     {0,      1,      0,     10,    200,     16,    100,      0,    256,    256,     64,     0}},
                { 256 * 1024,     {0,      3,      0,     10,    300,     16,    100,      0,    256,    256,     64,     0}},
                { 512 * 1024,     {0,      3,      0,     10,    300,     16,    100,      0,    256,    256,     64,     0}},
            },
        },
    },

    /*
    *
    * rnr
    *
    */
    {
        1,      /// rnrTuneEnable

        /*
        *
        * rnrGainInterpTab
        *
        */
        {
            10, /// validSize
            {
                /// [0]  gain
                /// [1]  rnrBltLfNpGain
                /// [2]  rnrBltLfNpStr
                //     0           1       2
                {   1 * 1024,  {   96,    256}},
                {   2 * 1024,  {  192,    256}},
                {   4 * 1024,  {  288,    256}},
                {   8 * 1024,  {  384,    256}},
                {  16 * 1024,  {  480,    256}},
                {  32 * 1024,  {  672,    256}},
                {  64 * 1024,  { 1170,    256}},
                { 128 * 1024,  { 1170,    256}},
                { 256 * 1024,  { 1200,    256}},
                { 512 * 1024,  { 1400,    256}},
            },
        },

        /*
        *
        * rnrRadialLutTab
        *
        */
        {
            10, /// validSize
            {
                {
                    1 * 1024,
                    {
                        {
                            210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,
                            210,  210,  210,  210,  210,  210,  210,  212,  214,  216,  218,  220,  225,  230,  235,  240,
                            255
                        },
                    },
                },
                {
                    2 * 1024,
                    {
                        {
                            210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,
                            210,  210,  210,  210,  210,  210,  210,  212,  214,  216,  218,  220,  225,  230,  235,  240,
                            255
                        },
                    },
                },
                {
                    4 * 1024,
                    {
                        {
                            210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,
                            210,  210,  210,  210,  210,  210,  210,  212,  214,  216,  218,  220,  225,  230,  235,  240,
                            255
                        },
                    },
                },
                {
                    8 * 1024,
                    {
                        {
                            210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,
                            210,  210,  210,  210,  210,  210,  210,  212,  214,  216,  218,  220,  225,  230,  235,  240,
                            255
                        },
                    },

                },
                {
                    16 * 1024,
                    {
                        {
                            210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,
                            210,  210,  210,  210,  210,  210,  210,  212,  214,  216,  218,  220,  225,  230,  235,  240,
                            255
                        },
                    },

                },
                {
                    32 * 1024,
                    {
                        {
                            210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,
                            210,  210,  210,  210,  210,  210,  210,  212,  214,  216,  218,  220,  225,  230,  235,  240,
                            255
                        },
                    },
                },
                {
                    64 * 1024,
                    {
                        {
                            210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,
                            210,  210,  210,  210,  210,  210,  210,  212,  214,  216,  218,  220,  225,  230,  235,  240,
                            255
                        },
                    },

                },
                {
                    128 * 1024,
                    {
                        {
                            210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,
                            210,  210,  210,  210,  210,  210,  210,  212,  214,  216,  218,  220,  225,  230,  235,  240,
                            255
                        },
                    },
                },
                {
                    256 * 1024,
                    {
                        {
                            210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,
                            210,  210,  210,  210,  210,  210,  210,  212,  214,  216,  218,  220,  225,  230,  235,  240,
                            255
                        },
                    },
                },
                {
                    512 * 1024,
                    {
                        {
                            210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,
                            210,  210,  210,  210,  210,  210,  210,  212,  214,  216,  218,  220,  225,  230,  235,  240,
                            255
                        },
                    },
                },
            },
        },

        /*
        *
        * rnrWeightLutTab
        *
        */
        {
            10, /// validSize
            {
                {
                    1 * 1024,
                    {
                        {
                            63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,
                            63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,
                            53,   45,   38,   32,   27,   23,   20,   17,   14,   12,   10,    9,    7,    6,    5,    4,
                             4,    3,    3,    2,    2,    2,    1,    1,    1,    1,    1,    1,    1,    0,    0,    0,
                             0
                        },
                    },
                },
                {
                    2 * 1024,
                    {
                        {
                            63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,
                            63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,
                            53,   45,   38,   32,   27,   23,   20,   17,   14,   12,   10,    9,    7,    6,    5,    4,
                             4,    3,    3,    2,    2,    2,    1,    1,    1,    1,    1,    1,    1,    0,    0,    0,
                             0
                        },
                    },
                },
                {
                    4 * 1024,
                    {
                        {
                            63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,
                            63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,
                            53,   45,   38,   32,   27,   23,   20,   17,   14,   12,   10,    9,    7,    6,    5,    4,
                             4,    3,    3,    2,    2,    2,    1,    1,    1,    1,    1,    1,    1,    0,    0,    0,
                             0
                        },
                    },
                },
                {
                    8 * 1024,
                    {
                        {
                            63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,
                            63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,
                            53,   45,   38,   32,   27,   23,   20,   17,   14,   12,   10,    9,    7,    6,    5,    4,
                             4,    3,    3,    2,    2,    2,    1,    1,    1,    1,    1,    1,    1,    0,    0,    0,
                             0
                        },
                    },
                },
                {
                    16 * 1024,
                    {
                        {
                            63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,
                            63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,
                            53,   45,   38,   32,   27,   23,   20,   17,   14,   12,   10,    9,    7,    6,    5,    4,
                             4,    3,    3,    2,    2,    2,    1,    1,    1,    1,    1,    1,    1,    0,    0,    0,
                             0
                        },
                    },
                },
                {
                    32 * 1024,
                    {
                        {
                            63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,
                            63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,
                            53,   45,   38,   32,   27,   23,   20,   17,   14,   12,   10,    9,    7,    6,    5,    4,
                             4,    3,    3,    2,    2,    2,    1,    1,    1,    1,    1,    1,    1,    0,    0,    0,
                             0
                        },
                    },
                },
                {
                    64 * 1024,
                    {
                        {
                            63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,
                            63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,
                            53,   45,   38,   32,   27,   23,   20,   17,   14,   12,   10,    9,    7,    6,    5,    4,
                             4,    3,    3,    2,    2,    2,    1,    1,    1,    1,    1,    1,    1,    0,    0,    0,
                             0
                        },
                    },
                },
                {
                    128 * 1024,
                    {
                        {
                            63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,
                            63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,
                            53,   45,   38,   32,   27,   23,   20,   17,   14,   12,   10,    9,    7,    6,    5,    4,
                             4,    3,    3,    2,    2,    2,    1,    1,    1,    1,    1,    1,    1,    0,    0,    0,
                             0
                        },
                    },
                },
                {
                    256 * 1024,
                    {
                        {
                            63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,
                            63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,
                            53,   45,   38,   32,   27,   23,   20,   17,   14,   12,   10,    9,    7,    6,    5,    4,
                             4,    3,    3,    2,    2,    2,    1,    1,    1,    1,    1,    1,    1,    0,    0,    0,
                             0
                        },
                    },
                },
                {
                    512 * 1024,
                    {
                        {
                            63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,
                            63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,   63,
                            53,   45,   38,   32,   27,   23,   20,   17,   14,   12,   10,    9,    7,    6,    5,    4,
                             4,    3,    3,    2,    2,    2,    1,    1,    1,    1,    1,    1,    1,    0,    0,    0,
                             0
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
        1     , /// npTuneEnable
        128,    /// npLGain
        182,    /// npMGain
        0,      /// npTransGain
        4,      /// npFitOrder

        /*
        *
        * npLutTab
        *
        */
        {
            10, /// validSize
            {
                {
                    1 * 1024,
                    {
                        {
                             0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                            15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                            23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                            29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                            34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                            39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                            43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                            47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    2 * 1024,
                    {
                        {
                             0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                            15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                            23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                            29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                            34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                            39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                            43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                            47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    4 * 1024,
                    {
                        {
                             0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                            15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                            23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                            29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                            34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                            39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                            43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                            47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    8 * 1024,
                    {
                        {
                             0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                            15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                            23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                            29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                            34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                            39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                            43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                            47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    16 * 1024,
                    {
                        {
                             0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                            15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                            23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                            29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                            34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                            39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                            43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                            47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    32 * 1024,
                    {
                        {
                             0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                            15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                            23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                            29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                            34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                            39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                            43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                            47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    64 * 1024,
                    {
                        {
                             0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                            15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                            23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                            29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                            34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                            39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                            43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                            47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    128 * 1024,
                    {
                        {
                             0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                            15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                            23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                            29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                            34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                            39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                            43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                            47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    256 * 1024,
                    {
                        {
                             0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                            15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                            23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                            29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                            34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                            39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                            43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                            47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    512 * 1024,
                    {
                        {
                             0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                            15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                            23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                            29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                            34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                            39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                            43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                            47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
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
        1,      /// dmsTuneEnable
        0,      /// dmsSmallaliasDetThresh
        1,      /// dmsBdOption
        75,     /// dmsDuShOffset
        /*
        *
        * dmsGainSwitchTab
        *
        */
        {
            10, /// validSize
            {
                /// [0] gain
                /// [1] dmsNpOffAbs
                /// [2] dmsSharpenEn
                /// [3] dmsHfDetailEnhEn
                ///    0           1       2       3
                {   1 * 1024,     {1,      1,      1}},
                {   2 * 1024,     {1,      1,      1}},
                {   4 * 1024,     {1,      1,      1}},
                {   8 * 1024,     {1,      1,      1}},
                {  16 * 1024,     {1,      1,      1}},
                {  32 * 1024,     {1,      1,      0}},
                {  64 * 1024,     {1,      1,      0}},
                { 128 * 1024,     {1,      0,      0}},
                { 256 * 1024,     {1,      0,      0}},
                { 512 * 1024,     {1,      0,      0}},
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
                /// 0          1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     17     18     19     20     21     22     23     24
                {   1 * 1024, {40   , 5000 , 253  , 1350 , 235  , 50   , 255  , 50   , 125  , 0    , 64   , 50   , 160  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 6000 , 96   , 0  }},
                {   2 * 1024, {40   , 5000 , 253  , 1350 , 235  , 50   , 255  , 50   , 122  , 0    , 64   , 50   , 160  , 50   , 255  , 90   , 140  , 435  , 201  , 369  , 178  , 6000 , 96   , 0  }},
                {   4 * 1024, {40   , 5000 , 253  , 1350 , 235  , 50   , 235  , 50   , 100  , 0    , 64   , 50   , 160  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 6000 , 96   , 0  }},
                {   8 * 1024, {40   , 5000 , 253  , 1350 , 235  , 50   , 235  , 50   , 100  , 0    , 64   , 50   , 160  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 6000 , 96   , 0  }},
                {  16 * 1024, {40   , 5000 , 253  , 1350 , 235  , 50   , 235  , 50   , 100  , 0    , 64   , 50   , 160  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 6000 , 64   , 0  }},
                {  32 * 1024, {20   , 5000 , 253  , 1350 , 235  , 40   , 235  , 50   , 100  , 0    , 48   , 50   , 180  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 4096 , 64   , 0  }},
                {  64 * 1024, {20   , 5000 , 253  , 1350 , 235  , 25   , 235  , 50   , 100  , 0    , 32   , 50   , 180  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 1000 , 64   , 0  }},
                { 128 * 1024, {20   , 5000 , 253  , 1350 , 140  , 25   , 180  , 50   , 100  , 0    , 32   , 50   , 180  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 500  , 32   , 0  }},
                { 256 * 1024, {10   , 2400 , 253  , 1350 , 140  , 0    , 180  , 50   , 80   , 0    , 32   , 50   , 120  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 200  , 32   , 0  }},
                { 512 * 1024, {10   , 2400 , 253  , 1350 , 140  , 0    , 180  , 50   , 80   , 0    , 32   , 50   , 120  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 200  , 32   , 0  }},
            },
        },

        /*
        *
        * dmsNpLutTab
        *
        */
        {
            10, /// validSize
            {
                {
                    1 * 1024,
                    {
                        {
                              0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                             15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                             23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                             29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                             34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                             39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                             43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                             47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    2 * 1024,
                    {
                        {
                              0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                             15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                             23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                             29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                             34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                             39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                             43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                             47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    4 * 1024,
                    {
                        {
                              0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                             15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                             23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                             29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                             34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                             39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                             43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                             47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    8 * 1024,
                    {
                        {
                              0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                             15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                             23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                             29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                             34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                             39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                             43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                             47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    16 * 1024,
                    {
                        {
                             0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                            15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                            23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                            29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                            34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                            39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                            43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                            47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    32 * 1024,
                    {
                        {
                             0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                            15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                            23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                            29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                            34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                            39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                            43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                            47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    64 * 1024,
                    {
                        {
                             0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                            15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                            23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                            29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                            34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                            39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                            43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                            47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    128 * 1024,
                    {
                        {
                             0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                            15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                            23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                            29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                            34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                            39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                            43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                            47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    256 * 1024,
                    {
                        {
                             0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                            15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                            23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                            29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                            34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                            39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                            43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                            47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    512 * 1024,
                    {
                        {
                             0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                            15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                            23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                            29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                            34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                            39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                            43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                            47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
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
        1,      /// dpfTuneEnable
        4,      /// dpfOffCenMultS
        45252,  /// dpfOffCenMult
        960,    /// dpfCenX
        540,    /// dpfCenY

        /*
        *
        * dpfGainSwitchTab
        *
        */
        {
            10, /// validSize
            {
                /// [0] gain
                /// [1] dpfByp(not used)
                ///     0          1
                {   1 * 1024,     {0}},
                {   2 * 1024,     {0}},
                {   4 * 1024,     {0}},
                {   8 * 1024,     {0}},
                {  16 * 1024,     {0}},
                {  32 * 1024,     {0}},
                {  64 * 1024,     {0}},
                { 128 * 1024,     {0}},
                { 256 * 1024,     {0}},
                { 512 * 1024,     {0}},
            },
        },

        /*
        *
        * dpfGainInterpTab
        *
        */
        {
            10, /// validSize
            {
                /// [0]  gain
                /// [1]  dpfSadThr
                /// [2]  dpfSadSlp
                /// [3]  dpfHue1Th
                /// [4]  dpfHue1Slope
                /// [5]  dpfHue2Th
                /// [6]  dpfHue2Slope
                /// [7] dpfHueStr
                /// [8] dpfSat1Th
                /// [9] dpfSat1Slope
                /// [10] dpfSatStr
                /// [11] dpfLuma11Th
                /// [12] dpfLuma11Slope
                /// [13] dpfLuma12Th
                /// [14] dpfLuma12Slope
                /// [15] dpfLuma21Th
                /// [16] dpfLuma21Slope
                /// [17] dpfLuma22Th
                /// [18] dpfLuma22Slope
                /// [19] dpfLumaStr
                /// [20] dpfHslTh
                /// [21] dpfHslSlope
                /// [22] dpfPurpleStr
                /// [23] dpfSaturStr
                ///      0         1       2       3       4       5       6      7        8      9       10      11      12      13      14      15      16      17      18      19      20      21      22      23
                {   1 * 1024,  {1200,   1024,   1690,    284,   2150,   1422,    768,    164,    123,      0,      0,   1024,   1500,      0,   3450,   1024,   3900,      0,      0,      0,     36,      0,      0}},
                {   2 * 1024,  {1200,   1024,   1690,    284,   2150,   1422,    768,    164,    123,      0,      0,   1024,   1500,      0,   3450,   1024,   3900,      0,      0,      0,     36,      0,      0}},
                {   4 * 1024,  {1200,   1024,   1690,    284,   2150,   1422,    768,    164,    123,      0,      0,   1024,   1500,      0,   3450,   1024,   3900,      0,      0,      0,     36,      0,      0}},
                {   8 * 1024,  {1200,   1024,   1690,    284,   2150,   1422,    768,    164,    123,      0,      0,   1024,   1500,      0,   3450,   1024,   3900,      0,      0,      0,     36,      0,      0}},
                {  16 * 1024,  {1200,   1024,   1690,    284,   2150,   1422,    768,    164,    123,      0,      0,   1024,   1500,      0,   3450,   1024,   3900,      0,      0,      0,     36,      0,      0}},
                {  32 * 1024,  {1200,   1024,   1690,    284,   2150,   1422,    768,    164,    123,      0,      0,   1024,   1500,      0,   3450,   1024,   3900,      0,      0,      0,     36,      0,      0}},
                {  64 * 1024,  {1200,   1024,   1690,    284,   2150,   1422,    768,    164,    123,      0,      0,   1024,   1500,      0,   3450,   1024,   3900,      0,      0,      0,     36,      0,      0}},
                { 128 * 1024,  {1200,   1024,   1690,    284,   2150,   1422,    768,    164,    123,      0,      0,   1024,   1500,      0,   3450,   1024,   3900,      0,      0,      0,     36,      0,      0}},
                { 256 * 1024,  {1200,   1024,   1690,    284,   2150,   1422,    768,    164,    123,      0,      0,   1024,   1500,      0,   3450,   1024,   3900,      0,      0,      0,     36,      0,      0}},
                { 512 * 1024,  {1200,   1024,   1690,    284,   2150,   1422,    768,    164,    123,      0,      0,   1024,   1500,      0,   3450,   1024,   3900,      0,      0,      0,     36,      0,      0}},
            },
        },

        /*
        *
        * dpfStrLutTab
        *
        */
        {
            10, /// validSize
            {
                {
                    1 * 1024,
                    {
                        {
                             255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,
                             255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,
                             255
                        },
                    },
                },
                {
                    2 * 1024,
                    {
                        {
                             255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,
                             255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,
                             255
                        },
                    },
                },
                {
                    4 * 1024,
                    {
                        {
                             255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,
                             255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,
                             255
                        },
                    },
                },
                {
                    8 * 1024,
                    {
                        {
                             255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,
                             255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,
                             255
                        },
                    },
                },
                {
                    16 * 1024,
                    {
                        {
                             255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,
                             255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,
                             255
                        },
                    },
                },
                {
                    32 * 1024,
                    {
                        {
                             255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,
                             255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,
                             255
                        },
                    },
                },
                {
                    64 * 1024,
                    {
                        {
                             255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,
                             255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,
                             255
                        },
                    },
                },
                {
                    128 * 1024,
                    {
                        {
                             255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,
                             255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,
                             255
                        },
                    },
                },
                {
                    256 * 1024,
                    {
                        {
                             255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,
                             255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,
                             255
                        },
                    },
                },
                {
                    512 * 1024,
                    {
                        {
                             255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,
                             255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  255,
                             255
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
        1,      /// cspTuneEnable

        /*
        *
        * cspGainSwitchTab
        *
        */
        {
            10, /// validSize
            {
                /// [0] gain
                /// [1] cspChsEn
                ///     0          1
                {   1 * 1024,     {0}},
                {   2 * 1024,     {0}},
                {   4 * 1024,     {0}},
                {   8 * 1024,     {0}},
                {  16 * 1024,     {0}},
                {  32 * 1024,     {0}},
                {  64 * 1024,     {0}},
                { 128 * 1024,     {0}},
                { 256 * 1024,     {0}},
                { 512 * 1024,     {0}},
            },
        },

        /*
        *
        * cspGainInterpTab
        *
        */
        {
            10, /// validSize
            {
                /// [0] gain
                /// [1] cspChsHiGain0
                /// [2] cspChsHiGain1
                /// [3] cspChsHiGain2
                /// [4] cspChsHiGain3
                /// [5] cspChsLoGain0
                /// [6] cspChsLoGain1
                /// [7] cspChsLoGain2
                /// [8] cspChsLoGain3
                /// [9] cspChsMaxGain
                /// [10]cspChsWeightU0
                /// [11]cspChsWeightU1
                /// [12]cspChsWeightU2
                /// [13]cspChsWeightV0
                /// [14]cspChsWeightV1
                /// [15]cspChsWeightV2
                ///     0         1       2       3       4       5       6       7       8       9       10      11      12      13      14      15
                {   1 * 1024,  {1536,   1280,    960,    796,    796,    960,   1280,   1536,   1536,      0,    468,    227,    179,    421,      0}},
                {   2 * 1024,  {1536,   1280,    960,    796,    796,    960,   1280,   1536,   1536,      0,    468,    227,    179,    421,      0}},
                {   4 * 1024,  {1536,   1280,    960,    796,    796,    960,   1280,   1536,   1536,      0,    468,    227,    179,    421,      0}},
                {   8 * 1024,  {1536,   1280,    960,    796,    796,    960,   1280,   1536,   1536,      0,    468,    227,    179,    421,      0}},
                {  16 * 1024,  {1536,   1280,    960,    796,    796,    960,   1280,   1536,   1536,      0,    468,    227,    179,    421,      0}},
                {  32 * 1024,  {1536,   1280,    960,    796,    796,    960,   1280,   1536,   1536,      0,    468,    227,    179,    421,      0}},
                {  64 * 1024,  {1536,   1280,    960,    796,    796,    960,   1280,   1536,   1536,      0,    468,    227,    179,    421,      0}},
                { 128 * 1024,  {1536,   1280,    960,    796,    796,    960,   1280,   1536,   1536,      0,    468,    227,    179,    421,      0}},
                { 256 * 1024,  {1536,   1280,    960,    796,    796,    960,   1280,   1536,   1536,      0,    468,    227,    179,    421,      0}},
                { 512 * 1024,  {1536,   1280,    960,    796,    796,    960,   1280,   1536,   1536,      0,    468,    227,    179,    421,      0}},
            },
        },
    },

    /*
    *
    * mvd
    *
    */
    {
        1,      /// mvdTuneEnable
        12,     /// mvdPipeABSadBitdepth
        3,      /// mvdLosstCompMode
        77,     /// mvdPipeABRGB2YCR
        150,    /// mvdPipeABRGB2YCG
        29,     /// mvdPipeABRGB2YCB

        /*
        *
        * mvdGainSwitchTab
        *
        */
        {
            10, /// validSize
            {
                /// [0] gain
                /// [1] mvdPipeBErode1
                /// [2] mvdPipeBDilate
                /// [3] mvdPipeBErode2
                /// [4] mvdPipeBMapRefineThr
                ///    0           1       2       3       4
                {   1 * 1024,     {0,     15,      9,      5}},
                {   2 * 1024,     {0,     15,      9,      5}},
                {   4 * 1024,     {2,     15,      9,      5}},
                {   8 * 1024,     {2,     15,      9,      5}},
                {  16 * 1024,     {2,     15,      9,      5}},
                {  32 * 1024,     {2,     15,      9,      5}},
                {  64 * 1024,     {3,     15,      7,      5}},
                { 128 * 1024,     {4,     15,      7,      5}},
                { 256 * 1024,     {4,     15,      7,      5}},
                { 512 * 1024,     {4,     15,      7,      5}},
            },
        },

        /*
        *
        * mvdGainInterpTab
        *
        */
        {
            10, /// validSize
            {
                /// [0] gain
                /// [1] mvdNpGain
                /// [2] mvdNpOffset
                /// [3] mvdPipeaThr
                /// [4] mvdPipeABTriDirStr
                ///     0          1       2       3       4
                {   1 * 1024,     {16,      5,     25,      0}},
                {   2 * 1024,     {16,      5,     25,      0}},
                {   4 * 1024,     {16,      5,     25,     64}},
                {   8 * 1024,     {24,      5,     25,    128}},
                {  16 * 1024,     {32,     10,     25,    256}},
                {  32 * 1024,     {40,     10,     25,    256}},
                {  64 * 1024,     {52,     15,     25,    256}},
                { 128 * 1024,     {60,     28,     25,    256}},
                { 256 * 1024,     {60,     28,     25,    256}},
                { 512 * 1024,     {60,     28,     25,    256}},
            },
        },
    },

    /*
    *
    * tnr
    *
    */
    {
        1,      /// tnrTuneEnable

        /*
        *
        * tnrGainSwitchTab
        *
        */
        {
            10, /// validSize
            {
                /// [0] gain
                /// [1] tnrNr2dPyramidWt
                /// [2] tnrMotionCurveMid
                /// [3] tnrMotionCurveEnd
                /// [4] tnrNr2dCurveMid
                /// [5] tnrNr2dCurveEnd
                /// [6] tnrNr3dCurveMid
                /// [7] tnrNr3dCurveEnd
                ///    0           1       2       3       4       5       6       7
                {   1 * 1024,     {2,      6,      6,      8,      8,      8,      8}},
                {   2 * 1024,     {2,      6,      6,      8,      8,      8,      8}},
                {   4 * 1024,     {2,      6,      6,      8,      8,      8,      8}},
                {   8 * 1024,     {2,      6,      6,      8,      8,      8,      8}},
                {  16 * 1024,     {2,      6,      6,      8,      8,      8,      8}},
                {  32 * 1024,     {2,      6,      6,      8,      8,      8,      8}},
                {  64 * 1024,     {2,      6,      6,      8,      8,      8,      8}},
                { 128 * 1024,     {2,      6,      6,      8,      8,      8,      8}},
                { 256 * 1024,     {2,      6,      6,      8,      8,      8,      8}},
                { 512 * 1024,     {2,      6,      6,      8,      8,      8,      8}},
            },
        },

        /*
        *
        * tnrGainInterpTab
        *
        */
        {
            10, /// validSize
            {
                /// [0] gain
                /// [1] tnrGlobalNoiseThr
                /// [2] tnrNr2dGradSlope
                /// [3] tnrNr2dYnoiseThr
                /// [4] tnrNr2dUVnoiseThr
                /// [5] tnrNr2dNoiseBackRatio
                /// [6] tnrMotionThr
                /// [7] tnrMotionCurveOffset
                /// [8] tnrMotionCurveY1
                /// [9] tnrMotionCurveY2
                /// [10] tnrMotionCurveY3
                /// [11] tnrNr2dCurveOffset
                /// [12] tnrNr2dCurveY1
                /// [13] tnrNr2dCurveY2
                /// [14] tnrNr2dCurveY3
                /// [15] tnrNr2dCurveC1
                /// [16] tnrNr2dCurveC2
                /// [17] tnrNr2dCurveC3
                /// [18] tnrNr3dCurveOffset
                /// [19] tnrNr3dCurveY1
                /// [20] tnrNr3dCurveY2
                /// [21] tnrNr3dCurveY3
                /// [22] tnrNr3dCurveC1
                /// [23] tnrNr3dCurveC2
                /// [24] tnrNr3dCurveC3
                ///     0         1        2       3       4       5       6       7        8       9      10       11      12      13      14      15      16      17      18      19      20      21      22      23       24
                {   1 * 1024,   {160,     32,     20,     20,     40,      4,      0,      20,     32,     64,      0,       0,     16,     32,      0,     16,     32,     0,      48,     22,     12,     44,     22,      0}},
                {   2 * 1024,   {160,     32,     20,     20,     40,      4,      0,      20,     32,     64,      0,       0,     16,     32,      0,     16,     32,     0,      48,     22,     12,     44,     22,      0}},
                {   4 * 1024,   {160,     32,     20,     20,     40,      4,      0,      20,     32,     64,      0,       0,     16,     32,      0,     16,     32,     0,      50,     22,     12,     50,     22,      0}},
                {   8 * 1024,   {160,     32,     60,     60,     40,      4,      0,      20,     32,     64,      0,       0,     32,     48,      0,     32,     64,     0,      54,     22,     12,     54,     22,      0}},
                {  16 * 1024,   {160,     32,    100,    100,     40,      4,      0,      20,     32,     64,      0,       0,     32,     48,      0,     32,     64,     0,      56,     30,     12,     54,     30,      8}},
                {  32 * 1024,   {160,     32,    150,    150,     40,      4,      0,      10,     32,     64,      0,       0,     32,     48,      0,     32,     64,     0,      58,     30,     12,     54,     30,      8}},
                {  64 * 1024,   {160,     32,    180,    200,     40,      4,      0,       6,     32,     64,      0,       8,     32,     52,      8,     32,     64,     0,      59,     40,     18,     54,     32,     18}},
                { 128 * 1024,   {160,     32,    200,    200,      0,      4,      0,       6,     32,     64,      0,      15,     32,     52,     15,     32,     64,     0,      60,     44,     22,     54,     32,     20}},
                { 256 * 1024,   {160,     32,    200,    200,      0,      4,      0,       6,     32,     64,      0,      15,     32,     58,     15,     32,     64,     0,      60,     44,     22,     54,     32,     20}},
                { 512 * 1024,   {160,     32,    200,    200,      0,      4,      0,       6,     32,     64,      0,      24,     48,     58,     25,     42,     64,     0,      61,     44,     22,     54,     32,     20}},
            },
        },
    },

    /*
    *
    * lsh
    *
    */
    {
        1,      /// lshTuneEnable

        /*
        *
        * lshGainSwitchTab
        *
        */
        {
            10, /// validSize
            {
                /// [0]gain
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
                /// 0               1      2      3       4       5       6       7      8       9      10     11      12     13     14      15      16
                {   1 * 1024,    {  6,      6,    5,      6,      4,      4,      0,     4,      4,     8,      8,      1,    7,      6,      7,      8}},
                {   2 * 1024,    {  6,      6,    5,      6,      4,      4,      0,     4,      4,     8,      8,      1,    7,      6,      7,      8}},
                {   4 * 1024,    {  6,      6,    5,      6,      4,      4,      0,     4,      4,     8,      8,      1,    7,      6,      7,      8}},
                {   8 * 1024,    {  6,      6,    5,      6,      4,      4,      0,     4,      4,     8,      8,      1,    7,      6,      7,      8}},
                {  16 * 1024,    {  6,      6,    5,      6,      4,      4,      0,     4,      4,     8,      8,      1,    7,      6,      7,      8}},
                {  32 * 1024,    {  6,      6,    5,      6,      4,      4,      0,     4,      4,     8,      8,      1,    7,      6,      7,      8}},
                {  64 * 1024,    {  6,      6,    5,      6,      4,      4,      0,     4,      4,     8,      8,      1,    7,      6,      7,      8}},
                { 128 * 1024,    {  6,      6,    5,      6,      4,      4,      0,     4,      4,     8,      8,      1,    7,      6,      7,      8}},
                { 256 * 1024,    {  6,      6,    5,      6,      4,      4,      0,     4,      4,     8,      8,      1,    7,      6,      7,      8}},
                { 512 * 1024,    {  6,      6,    5,      6,      4,      4,      0,     4,      4,     8,      8,      1,    7,      6,      7,      8}},
            },
        },

        /*
        *
        * lshGainInterpTab
        *
        */
        {
            10, /// validSize
            {
                /// [0]gain
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
                /// [18]lshNegMinOffset1
                /// [19]lshPosMaxOffset1
                /// [20]lshNegClipRatio1
                /// [21]lshPosClipRatio1
                /// [22]lshDetailNwAlpha
                /// [23]lshDetailNrR
                /// [24]lshDetailFreqOffset
                /// [25]lshDetailFreqY1
                /// [26]lshDetailFreqY2
                /// [27]lshDetailFreqY3
                /// [28]lshDetailLumOffset
                /// [29]lshDetailLumY1
                /// [30]lshDetailLumY2
                /// [31]lshDetailLumY3
                /// [32]lshNegStr2
                /// [33]lshPosStr2
                /// [34]lshNegMinOffset2
                /// [35]lshPosMaxOffset2
                /// [36]lshNegClipRatio2
                /// [37]lshPosClipRatio2
                /// [38]lshDltiPosOffset
                /// [39]lshDltiFinalPosY1
                /// [40]lshDltiFinalPosY2
                /// [41]lshDltiFinalPosY3
                /// [42]lshDltiNegOffset
                /// [43]lshDltiFinalNegY1
                /// [44]lshDltiFinalNegY2
                /// [45]lshDltiFinalNegY3
                /// 0              1        2       3       4      5      6        7      8       9        10      11      12      13      14       15     16     17       18       19     20      21      22      23      24      25      26      27      28      29      30       31      32     33      34       35      36      37      38     39       40     41       42     43      44     45
                {   1 * 1024,    {256,     32,      0,      0,      0,    256,    512,    0,     200,    150,    100,       8,      0,      0,      0,    256,    256,      0,      0,    100,    100,     70,    100,     0,     256,    256,    256,    128,     256,    256,    256,    500,    500,   1023,   1023,      0,      0,     64,    256,    180,     50,     64,    256,    200,    150}},
                {   2 * 1024,    {256,     32,      0,      0,      0,    256,    512,    0,     200,    150,    100,       8,      0,      0,      0,    256,    256,      0,      0,    100,    100,     70,    100,     0,     256,    256,    256,    128,     256,    256,    256,    500,    500,   1023,   1023,      0,      0,     64,    256,    180,     50,     64,    256,    200,    150}},
                {   4 * 1024,    {256,     32,      0,      0,      0,    256,    512,    0,     150,    100,     50,       8,      0,      0,      0,    256,    256,      0,      0,    100,    100,     70,    100,     0,     256,    256,    256,    128,     256,    256,    256,    500,    500,   1023,   1023,      0,      0,     64,    256,    180,     50,     64,    256,    200,    150}},
                {   8 * 1024,    {256,     32,      0,      0,      0,    256,    512,    0,     150,    100,      0,       8,      0,      0,      0,    256,    256,      0,      0,    100,    100,     70,    100,     0,     256,    256,    256,    128,     256,    256,    256,    500,    500,   1023,   1023,      0,      0,     64,    256,    180,     50,     64,    256,    200,    150}},
                {  16 * 1024,    {256,     32,      0,      0,      0,    256,    512,    0,     150,    100,      0,       8,      0,      0,      0,    256,    256,      0,      0,    100,    100,     70,    100,     0,     256,    256,    256,    128,     256,    256,    256,    500,    500,   1023,   1023,      0,      0,     64,    256,    180,     50,     64,    256,    200,    150}},
                {  32 * 1024,    {256,     32,      0,      0,      0,    256,    512,    0,     150,    100,      0,       8,      0,      0,      0,    256,    256,      0,      0,    100,    100,     70,    100,     0,     256,    256,    256,    128,     256,    256,    256,    500,    500,   1023,   1023,      0,      0,     64,    256,    180,     50,     64,    256,    200,    150}},
                {  64 * 1024,    {220,     32,      0,      0,      0,    256,    512,    0,     150,    100,      0,       8,      0,      0,      0,    256,    256,      0,      0,    100,    100,     70,    100,     0,     256,    256,    256,    128,     256,    256,    256,    500,    500,   1023,   1023,      0,      0,     64,    256,    180,     50,     64,    256,    200,    150}},
                { 128 * 1024,    {220,     32,      0,      0,      0,    256,    512,    0,     150,    100,      0,       8,      0,      0,      0,    256,    256,      0,      0,    100,    100,     70,    100,     0,     256,    256,    256,    128,     256,    256,    256,    500,    500,   1023,   1023,      0,      0,     64,    256,    180,     50,     64,    256,    200,    150}},
                { 256 * 1024,    {210,     32,      0,      0,      0,    256,    512,    0,     150,    100,      0,       8,    256,    256,    256,    256,    256,      0,      0,    100,    100,     70,    100,     0,     256,    256,    256,    128,     256,    256,    256,    500,    500,   1023,   1023,      0,      0,     64,    256,    180,     50,     64,    256,    200,    150}},
                { 512 * 1024,    {210,     32,      0,      0,      0,    256,    512,    0,     150,    100,      0,       8,    256,    256,    256,    256,    256,      0,      0,    100,    100,     70,    100,     0,     256,    256,    256,    128,     256,    256,    256,    500,    500,   1023,   1023,      0,      0,     64,    256,    180,     50,     64,    256,    200,    150}},
            },
        },

        /*
        *
        * lshWeightLutTab
        *
        */
        {
            10, /// validSize
            {
                {
                    1 * 1024,
                    {
                        {
                             63,   38,   23,   14,    9,    5,    3,    2,    1,    1,    0,    0,    0,    0,    0,    0,
                              0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
                        },
                    },
                },
                {
                    2 * 1024,
                    {
                        {
                             63,   38,   23,   14,    9,    5,    3,    2,    1,    1,    0,    0,    0,    0,    0,    0,
                              0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
                        },
                    },
                },
                {
                    4 * 1024,
                    {
                        {
                             63,   38,   23,   14,    9,    5,    3,    2,    1,    1,    0,    0,    0,    0,    0,    0,
                              0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
                        },
                    },
                },
                {
                    8 * 1024,
                    {
                        {
                            63,   38,   23,   14,    9,    5,    3,    2,    1,    1,    0,    0,    0,    0,    0,    0,
                             0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
                        },
                    },
                },
                {
                    16 * 1024,
                    {
                        {
                             63,   38,   23,   14,    9,    5,    3,    2,    1,    1,    0,    0,    0,    0,    0,    0,
                              0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
                        }
                    },
                },
                {
                    32 * 1024,
                    {
                        {
                             63,   38,   23,   14,    9,    5,    3,    2,    1,    1,    0,    0,    0,    0,    0,    0,
                              0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
                        }
                    },
                },
                {
                    64 * 1024,
                    {
                        {
                             63,   38,   23,   14,    9,    5,    3,    2,    1,    1,    0,    0,    0,    0,    0,    0,
                              0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
                        },
                    },
                },
                {
                    128 * 1024,
                    {
                        {
                             63,   61,   59,   57,   55,   53,   51,   49,   47,   46,   44,   43,   41,   40,   38,   37,
                             36,   34,   33,   32,   31,   30,   29,   28,   27,   26,   25,   24,   23,   22,   22,   21,
                        },
                    },
                },
                {
                    256 * 1024,
                    {
                        {
                             63,   61,   59,   57,   55,   53,   51,   49,   47,   46,   44,   43,   41,   40,   38,   37,
                             36,   34,   33,   32,   31,   30,   29,   28,   27,   26,   25,   24,   23,   22,   22,   21,
                        },
                    },
                },
                {
                    512 * 1024,
                    {
                        {
                             63,   61,   59,   57,   55,   53,   51,   49,   47,   46,   44,   43,   41,   40,   38,   37,
                             36,   34,   33,   32,   31,   30,   29,   28,   27,   26,   25,   24,   23,   22,   22,   21,
                        },
                    },
                },
            },
        },

        /*
        *
        * lshLuxSwitchTab
        *
        */
        {
            4, /// validSize
            {
                /// [1] lshDetailType
                /// [2] lshDetailFreqMid
                /// [3] lshDetailFreqEnd
                /// [4] lshDetailLumMid
                /// [5] lshDetailLumEnd
                /// [6] lshDltiPosMid
                /// [7] lshDltiPosEnd
                /// [8] lshDltiNegMid
                /// [9] lshDltiNegEnd
                ///  0     1       2       3       4       5       6       7       8        9
                {   50,  {0,      4 ,      4,      8,      8,      7,      6,      7,       8 }},
                {  100,  {0,      4 ,      4,      8,      8,      7,      6,      7,       8 }},
                {  150,  {0,      4 ,      4,      8,      8,      7,      6,      7,       8 }},
                {  200,  {0,      4 ,      4,      8,      8,      7,      6,      7,       8 }},
            },
        },
        /*
        *
        * lshLuxInterpTab
        *
        */
        {
            4, /// validSize
            {
                /// [0] lux
                /// [1] lshDetailFreqOffset
                /// [2] lshDetailFreqY1
                /// [3] lshDetailFreqY2
                /// [4] lshDetailFreqY3
                /// [5] lshDetailLumOffset
                /// [6] lshDetailLumY1
                /// [7] lshDetailLumY2
                /// [8] lshDetailLumY3
                /// [9]lshDltiPosOffset
                /// [10]lshDltiFinalPosY1
                /// [11]lshDltiFinalPosY2
                /// [12]lshDltiFinalPosY3
                /// [13]lshDltiNegOffset
                /// [14]lshDltiFinalNegY1
                /// [15]lshDltiFinalNegY2
                /// [16]lshDltiFinalNegY3
                ///  0     1       2       3       4       5       6       7       8        9      10      11      12      13      14      15      16
                {   50,  {0,      256,    256,    256,    128,    256,    256,    256,     64,    256,    180,     50,     64,    256,    200,    150}},
                {  100,  {0,      256,    256,    256,    128,    256,    256,    256,     64,    256,    180,     50,     64,    256,    200,    150}},
                {  150,  {0,      256,    256,    256,    128,    256,    256,    256,     64,    256,    180,     50,     64,    256,    200,    150}},
                {  200,  {0,      256,    256,    256,    128,    256,    256,    256,     64,    256,    180,     50,     64,    256,    200,    150}},
            },
        },
    },

    /*
    *
    * cnr
    *
    */
    {
        1,      /// cnrTuneEnable

        /*
        *
        * cnrGainSwitchTab
        *
        */
        {
            10, /// validSize
            {
                /// [0] gain
                /// [1] cnrLumaNoiseMid
                /// [2] cnrLumaNoiseEnd
                /// [3] cnrRfiltSatMid
                /// [4] cnrRfiltSatEnd
                /// [5] cnrRfiltDiffMid
                /// [6] cnrRfiltDiffEnd
                /// [7] cnrUvBlendMid
                /// [8] cnrUvBlendEnd
                /// [9] cnrFcSupMid
                /// [10] cnrFcSupEnd
                ///    0           1       2       3       4       5       6       7       8       9      10
                {   1 * 1024,     {10,     10,     5,      5,      5,      5,      9,      9,      5,      5}},
                {   2 * 1024,     {10,     10,     5,      5,      5,      5,      9,      9,      5,      5}},
                {   4 * 1024,     {10,     10,     5,      5,      5,      5,      9,      9,      5,      5}},
                {   8 * 1024,     {10,     10,     5,      5,      5,      5,      9,      9,      5,      5}},
                {  16 * 1024,     {10,     10,     5,      5,      5,      5,      9,      9,      5,      5}},
                {  32 * 1024,     {10,     10,     5,      5,      5,      5,      9,      9,      5,      5}},
                {  64 * 1024,     {10,     10,     5,      5,      5,      5,      9,      9,      5,      5}},
                { 128 * 1024,     {10,     10,     5,      5,      5,      5,      9,      9,      5,      5}},
                { 256 * 1024,     {10,     10,     5,      5,      5,      5,      9,      9,      5,      5}},
                { 512 * 1024,     {10,     10,     5,      5,      5,      5,      9,      9,      5,      5}},
            },
        },
        /*
        *
        * cnrGainInterpTab
        *
        */
        {
            10, /// validSize
            {
                /// [0] gain
                /// [1] cnrMotionNoiseGain
                /// [2] cnrLumaNoiseOffset
                /// [3] cnrLumaNoiseY1
                /// [4] cnrLumaNoiseY2
                /// [5] cnrLumaNoiseY3
                /// [6] cnrRfiltSatOffset
                /// [7] cnrRfiltSatY1
                /// [8] cnrRfiltSatY2
                /// [9] cnrRfiltSatY3
                /// [10] cnrRfiltDiffOffset
                /// [11] cnrRfiltDiffY1
                /// [12] cnrRfiltDiffY2
                /// [13] cnrRfiltDiffY3
                /// [14] cnrGaussNoiseOffset
                /// [15] cnrGaussNoiseGain
                /// [16] cnrUvdiffSft
                /// [17] cnrUvBlendOffset
                /// [18] cnrUvBlendY1
                /// [19] cnrUvBlendY2
                /// [20] cnrUvBlendY3
                /// [21] cnrFcSupOffset
                /// [22] cnrFcSupY1
                /// [23] cnrFcSupY2
                /// [24] cnrFcSupY3
                ///   0            1       2       3       4       5       6       7       8        9      10      11      12      13      14      15       16      17      18      19       20      21      22       23       24
                {   1 * 1024,    {256,     0,     10,     10,     10,      0,     256,     128,     0,     0,      200,    100,      0,      0,    256,      2,     512,    256,    230,    156,     32,      200,    100,      0}},
                {   2 * 1024,    {256,     0,     10,     10,     10,      0,     256,     128,     0,     0,      200,    100,      0,      0,    256,      2,     512,    256,    230,    156,     32,      200,    100,      0}},
                {   4 * 1024,    {256,     0,     10,     10,     10,      0,     256,     128,     0,     0,      200,    100,      0,      0,    256,      2,     512,    256,    230,    156,     32,      200,    100,      0}},
                {   8 * 1024,    {256,     0,     10,     10,     10,      0,     256,     128,     0,     0,      200,    100,      0,      0,    256,      2,     512,    256,    230,    156,     32,      200,    100,      0}},
                {  16 * 1024,    {256,     0,     10,     10,     10,      0,     256,     128,     0,     0,      200,    100,      0,      0,    256,      2,     512,    256,    230,    156,     32,      200,    100,      0}},
                {  32 * 1024,    {256,     0,     10,     10,     10,      0,     256,     128,     0,     0,      200,    100,      0,      0,    256,      2,     512,    256,    230,    156,     32,      200,    100,      0}},
                {  64 * 1024,    {256,     0,     10,     10,     10,      0,     256,     128,     0,     0,      200,    100,      0,      0,    256,      2,     512,    256,    230,    156,     32,      200,    100,      0}},
                { 128 * 1024,    {256,     0,     20,     20,     20,      0,     256,     128,     0,     0,      200,    100,      0,      0,    256,      2,     512,    256,    230,    156,     32,      256,    256,    256}},
                { 256 * 1024,    {512,     0,     30,     30,     30,      0,     256,     128,     0,     0,      200,    100,      0,      0,    256,      2,     512,    256,    230,    156,     32,      256,    256,    256}},
                { 512 * 1024,    {512,     0,     30,     30,     30,      0,     256,     128,     0,     0,      200,    100,      0,      0,    256,      2,     512,    256,    230,    156,     32,      256,    256,    256}},
            },
        },

        /*
        *
        * cnrRadialLutTab
        *
        */
        {
            10, /// validSize
            {
                {
                    1 * 1024,
                    {
                        {
                             210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,
                             210,  210,  210,  210,  210,  210,  210,  212,  214,  216,  218,  220,  225,  230,  235,  240,
                             255
                        },
                    },
                },
                {
                    2 * 1024,
                    {
                        {
                             210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,
                             210,  210,  210,  210,  210,  210,  210,  212,  214,  216,  218,  220,  225,  230,  235,  240,
                             255
                        },
                    },
                },
                {
                    4 * 1024,
                    {
                        {
                             210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,
                             210,  210,  210,  210,  210,  210,  210,  212,  214,  216,  218,  220,  225,  230,  235,  240,
                             255
                        },
                    },
                },
                {
                    8 * 1024,
                    {
                        {
                            210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,
                            210,  210,  210,  210,  210,  210,  210,  212,  214,  216,  218,  220,  225,  230,  235,  240,
                            255
                        },
                    },
                },
                {
                    16 * 1024,
                    {
                        {
                             210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,
                             210,  210,  210,  210,  210,  210,  210,  212,  214,  216,  218,  220,  225,  230,  235,  240,
                             255
                        },
                    },
                },
                {
                    32 * 1024,
                    {
                        {
                             210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,
                             210,  210,  210,  210,  210,  210,  210,  212,  214,  216,  218,  220,  225,  230,  235,  240,
                             255
                        },
                    },
                },
                {
                    64 * 1024,
                    {
                        {
                             210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,
                             210,  210,  210,  210,  210,  210,  210,  212,  214,  216,  218,  220,  225,  230,  235,  240,
                             255
                        },
                    },
                },
                {
                    128 * 1024,
                    {
                        {
                             210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,
                             210,  210,  210,  210,  210,  210,  210,  212,  214,  216,  218,  220,  225,  230,  235,  240,
                             255
                        },
                    },
                },
                {
                    256 * 1024,
                    {
                        {
                             210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,
                             210,  210,  210,  210,  210,  210,  210,  212,  214,  216,  218,  220,  225,  230,  235,  240,
                             255
                        },
                    },
                },
                {
                    512 * 1024,
                    {
                        {
                             210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,  210,
                             210,  210,  210,  210,  210,  210,  210,  212,  214,  216,  218,  220,  225,  230,  235,  240,
                             255
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
                /// [3]cmbThresh1Lm
                /// [4]cmbThresh2Lm
                /// 0          1      2     3       4
                {   1 * 1024, {32   , 32   ,3072 , 3840 }},
                {   2 * 1024, {32   , 32   ,3072 , 3840 }},
                {   4 * 1024, {32   , 32   ,3072 , 3840 }},
                {   8 * 1024, {32   , 32   ,2048 , 3840 }},
                {  16 * 1024, {32   , 32   ,1024 , 3840 }},
                {  32 * 1024, {32   , 32   ,1024 , 3840 }},
                {  64 * 1024, {64   , 64   ,1024 , 3840 }},
                { 128 * 1024, {128  , 128  ,512  , 3840 }},
                { 256 * 1024, {128  , 128  ,512  , 3840 }},
                { 512 * 1024, {128  , 128  ,512  , 3840 }},
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
                /// [1]cmbNpMultLm
                /// [2]cmbNpMultMs
                /// [3]cmbThresh1Lm
                /// [4]cmbThresh2Lm
                /// 0     1      2      3      4
                {50    , {32   , 32   , 3072 , 3840 }},
                {100   , {64   , 64   , 2048 , 3840 }},
                {150   , {128  , 128  , 1560 , 3840 }},
                {200   , {256  , 256  , 1024 , 3840 }},
                {250   , {384  , 384  , 768  , 3840 }},
                {300   , {512  , 512  , 512  , 3840 }},
                {350   , {2048 , 2048 , 512  , 3840 }},
                {400   , {9000 , 9000 , 512  , 3840 }},
                {450   , {12000, 12000, 512  , 3840 }},
                {500   , {16384, 16384, 512  , 3840 }},
            },
        },
        /*
        *
        * cmbLutTab
        *
        */
        {
            10, /// validSize
            {
                {
                    1 * 1024,
                    {
                        {
                              0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                             15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                             23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                             29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                             34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                             39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                             43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                             47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    2 * 1024,
                    {
                        {
                              0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                             15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                             23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                             29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                             34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                             39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                             43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                             47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    4 * 1024,
                    {
                        {
                              0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                             15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                             23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                             29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                             34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                             39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                             43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                             47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    8 * 1024,
                    {
                        {
                             0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                            15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                            23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                            29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                            34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                            39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                            43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                            47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    16 * 1024,
                    {
                        {
                              0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                             15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                             23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                             29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                             34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                             39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                             43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                             47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    32 * 1024,
                    {
                        {
                              0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                             15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                             23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                             29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                             34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                             39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                             43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                             47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    64 * 1024,
                    {
                        {
                              0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                             15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                             23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                             29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                             34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                             39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                             43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                             47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    128 * 1024,
                    {
                        {
                              0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                             15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                             23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                             29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                             34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                             39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                             43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                             47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    256 * 1024,
                    {
                        {
                              0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                             15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                             23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                             29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                             34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                             39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                             43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                             47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
                        },
                    },
                },
                {
                    512 * 1024,
                    {
                        {
                              0,    0,    0,    0,    0,    0,    0,    4,    5,   11,   11,   12,   13,   13,   14,   14,
                             15,   16,   16,   17,   17,   18,   18,   19,   19,   20,   20,   21,   21,   21,   22,   22,
                             23,   23,   24,   24,   24,   25,   25,   25,   26,   26,   27,   27,   27,   28,   28,   28,
                             29,   29,   29,   30,   30,   31,   31,   31,   32,   32,   32,   32,   33,   33,   33,   34,
                             34,   34,   35,   35,   35,   36,   36,   36,   37,   37,   37,   37,   38,   38,   38,   39,
                             39,   39,   39,   40,   40,   40,   40,   41,   41,   41,   42,   42,   42,   42,   43,   43,
                             43,   43,   44,   44,   44,   44,   45,   45,   45,   45,   46,   46,   46,   47,   47,   47,
                             47,   47,   48,   48,   48,   48,   49,   49,   49,   49,   50,   50,   50,   50,   51,   51
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
        1,      /// sdeTuneEnable
        0,      /// sdeNegEn
        1024,   /// sdeBrightness
        1024,   /// sdeSaturation
        0,      /// sdeHue
        0,      /// sdeClipYLow
        0,      /// sdeClipYHigh
        1024,   /// sdeContrastY
        1024,   /// sdeContrastU
        1024,   /// sdeContrastV
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
                {50    , {1100 }},
                {100   , {1100 }},
                {150   , {1100 }},
                {200   , {1100 }},
                {250   , {1100 }},
                {300   , {1100 }},
                {350   , {1100 }},
                {400   , {1100 }},
                {450   , {1100 }},
                {500   , {1100 }},
            },
        },
        /*
        *
        * sdeGainSwitchTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]sdeSatRatioU
                /// [2]sdeSatRatioV
                /// 0          1      2
                {   1 * 1024, {1024 , 1024 }},
                {   2 * 1024, {1024 , 1024 }},
                {   4 * 1024, {1024 , 1024 }},
                {   8 * 1024, {1024 , 1024 }},
                {  16 * 1024, {1024 , 1024 }},
                {  32 * 1024, {1024 , 1024 }},
                {  64 * 1024, {1024 , 1024 }},
                { 128 * 1024, {1024 , 1024 }},
                { 256 * 1024, {1024 , 1024 }},
                { 512 * 1024, {1024 , 1024 }},
            },
        },
    },
    /*
    *
    *  rir
    */
    {
        0,     /// rirTuneEnable
        0,     /// rirDirMeanEn
        0,     /// rirGradFilterEn
        0,     /// rirInArray
        0,     /// rirGradMeanThd1
        0,     /// rirGradMeanThd2
        0,     /// rirGradVarThd1
        0,     /// rirGradVarThd2
        0,     /// rirGradSimVar
        0,     /// rirOverExpEn
        0,     /// rirOverExpThd1
        0,     /// rirOverExpThd2
        0,     /// rirOverExpRGain
        0,     /// rirOverExpBGain
        0,     /// rirOverExpDesatStr
        0,     /// rirOverExpDifHueMin
        0,     /// rirBlcInEn
        0,     /// rirBlcOutEn
        0,     /// rirDebugMode
    },
    /*
    *
    * cac
    *
    */
    {
        /*
        *
        * cacGainInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]cacLutStr
                /// 0          1
                {   1 * 1024, {100    }},
                {   2 * 1024, {100    }},
                {   4 * 1024, {100    }},
                {   8 * 1024, {100    }},
                {  16 * 1024, {100    }},
                {  32 * 1024, {100    }},
                {  64 * 1024, {100    }},
                { 128 * 1024, {100    }},
                { 256 * 1024, {100    }},
                { 512 * 1024, {100    }},
            },
        },
    },
},

/*
*
* reserved tuning params
*
*/
{
    {'e','n','d','2','0','2','1','0','5', '!'},
},

#endif
