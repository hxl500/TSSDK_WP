#ifndef __TUNING_IMX415_NORMAL_H__
#define __TUNING_IMX415_NORMAL_H__
/*
*
* tuneVersion
*
*/
{
    0, 0, 0, 64704, /// major, minor, debug, check
},
/*
*
* sw3aParams
*
*/
{
  /*
  *
  * sw3aParams0
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
    * sw3aAlgoAttr
    *
    */
    {
        "algo.sw3a.default", /// sw3aAlgoAttr.name
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
        1     ,  /// antiFlickerEnable
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
            96    ,  /// aeStableOutRange default 32,[5:512] AE convergence stability range out
            960   ,  /// aeSaturationThr default 960,[0:1023] AE Saturation Threshold, use to calculation linaer Y
            2     ,  /// aeStatsYIdx default 3,[0,3] which stats Y use to judge ae stable out
            30    ,  /// aeChgFrameCnt default 15,[0, 4096] AE change time out frames
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
                {50    , {240  , 100   , 20000, 92000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {100   , {230  , 100   , 20000, 92000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {150   , {220  , 100   , 20000, 92000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {200   , {200  , 100   , 20000, 92000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {250   , {195  , 100   , 20000, 93500, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {300   , {190  , 100   , 20000, 95000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {350   , {170  , 100   , 20000, 97000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {400   , {160  , 100   , 20000, 97000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {450   , {135  , 100   , 20000, 97000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {500   , {100  , 100   , 20000, 97000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
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
                {50    , {230  , 100   , 20000, 90000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {100   , {230  , 100   , 20000, 90000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {150   , {210  , 100   , 20000, 91000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {200   , {195  , 100   , 20000, 92000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {250   , {195  , 100   , 20000, 93500, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {300   , {190  , 100   , 20000, 95000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {350   , {180  , 100   , 20000, 97000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {400   , {160  , 100   , 20000, 97000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {450   , {155  , 100   , 20000, 97000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {500   , {155  , 100   , 20000, 97000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
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
                {50    , {230  , 80   , 20000, 99700, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10   }},
                {100   , {230  , 80   , 20000, 99700, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10   }},
                {150   , {210  , 80   , 20000, 99700, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10   }},
                {200   , {195  , 80   , 20000, 99700, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10   }},
                {250   , {175  , 80   , 20000, 99700, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10   }},
                {300   , {150  , 80   , 20000, 99700, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10   }},
                {350   , {130  , 80   , 20000, 99700, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10   }},
                {400   , {125  , 80   , 20000, 99700, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10   }},
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
                {1     , {20   , 1024 , 1    }},
                {2     , {20   , 524288 , 0    }},
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
        6     ,  /// awbStableRange
        1     ,  /// awbLumaWeightEnable
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
            1     ,  /// validSize
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
                {0     , {1    , 0    , 0    , 1000 , 20   , 256  , 256  , 256  , 256  }},
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
            4     ,  /// validSize
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
                {1     , {0    , 2    , 0    , 1000 , 10   , 200  , 50   , -120 , -25  , -234 , -270 }},
                {2     , {0    , 7    , 190  , 250  , 20   , 300  , 50   , -8   , 104  , -267 , -319 }},
                {3     , {0    , 4    , 150  , 280  , 20   , 300  , 200  , 77   , 259  , -251 , -383 }},
            },
        },
        /*
        *
        * awbPrefColorZone
        *
        */
        {
            2     ,  /// validSize
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
                {0     , {1    , 0    , 240  , 400  , 10   , 96   , 128  , 128  , 128  , 236  , 276  }},
                {1     , {1    , 1    , 240  , 400  , 10   , 96   , 128  , 128  , 128  , 256  , 262  }},
            },
        },
        /*
        *
        * awbLightsourceProb
        *
        */
        {
            4     ,  /// validSize
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
                {50    , {16   , 16   , 256  , 128  , 256  , 128  , 128  }},
                {150   , {16   , 16   , 128  , 64   , 256  , 196  , 128  }},
                {200   , {16   , 16   , 128  , 128  , 256  , 128  , 128  }},
                {300   , {128  , 128  , 196  , 196  , 256  , 196  , 128  }},
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
                {0     , {0    , 0    , 0    , 1000 , 10   , 32   , 256  }},
                {1     , {0    , 1    , 0    , 1000 , 10   , 32   , 256  }},
                {2     , {0    , 2    , 0    , 1000 , 10   , 32   , 256  }},
                {3     , {0    , 3    , 0    , 1000 , 10   , 96   , 256  }},
                {4     , {0    , 4    , 0    , 1000 , 10   , 32   , 256  }},
                {5     , {0    , 5    , 0    , 1000 , 10   , 128  , 256  }},
                {6     , {0    , 6    , 0    , 1000 , 10   , 32   , 256  }},
            },
        },
        /*
        *
        * awbExtraColorZone
        *
        */
        {
            3     ,  /// validSize
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
                {0     , {0    , 440  , 1000 , 20   , 256  , 128  , 112  , 344  , -383 , -461 , 7    , 128  , 256  , 256  }},
                {1     , {0    , 440  , 1000 , 20   , 256  , 128  , -23  , 194  , -186 , -232 , 7    , 128  , 256  , 256  }},
                {2     , {0    , 120  , 200  , 20   , 250  , 50   , 97   , 149  , -319 , -352 , 3    , 256  , 256  , 256  }},
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
            4     ,  /// validSize
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
                {50    , {0    , 0    , 48   , 48   , 0    , 64   , 0    }},
                {100   , {0    , 0    , 32   , 32   , 0    , 64   , 0    }},
                {150   , {0    , 0    , 32   , 24   , 0    , 64   , 0    }},
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
            10    ,  /// validSize
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
                {50    , {10   , 20   , 128  , 512  , 256  , 512  , 128  , 128  }},
                {100   , {10   , 20   , 128  , 512  , 256  , 512  , 128  , 128  }},
                {150   , {10   , 20   , 128  , 512  , 256  , 512  , 128  , 128  }},
                {200   , {10   , 20   , 128  , 512  , 256  , 512  , 128  , 128  }},
                {250   , {10   , 20   , 128  , 384  , 256  , 384  , 128  , 128  }},
                {300   , {10   , 20   , 128  , 384  , 256  , 384  , 128  , 128  }},
                {350   , {10   , 20   , 128  , 384  , 256  , 384  , 128  , 128  }},
                {400   , {10   , 20   , 128  , 384  , 256  , 384  , 128  , 128  }},
                {450   , {10   , 20   , 230  , 384  , 256  , 384  , 128  , 128  }},
                {500   , {10   , 20   , 256  , 384  , 256  , 384  , 128  , 128  }},
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
#ifndef IMX415_FOCUS_REV_MODULE
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
#else
        120 , /// afMotorZoomInit
        /*
        *
        * afZoomFocusMappingTab
        *
        */
        {
            6    ,  /// validSize
            {
                /// [0]afZoomVal
                /// [1]afNearEndPos
                /// [2]afFarEndPos
                /// [3]afNearMarginPercent
                /// [4]afFarMarginPercent
                /// [5]afMoveStep
                /// 0     1      2      3      4      5
                {40     , {360,    400,     30,     50,      1}},
                {60     , {340,    380,     30,     50,      1}},
                {70     , {340,    370,     30,     50,      1}},
                {80     , {330,    350,     45,     35,      1}},
                {100    , {290,    320,     45,     35,      1}},
                {120    , {250,    280,     45,     35,      1}},
            },
        },
#endif
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
  * sw3aParams1
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
    * sw3aAlgoAttr
    *
    */
    {
        "sample1", /// sw3aAlgoAttr.name
    },

  },
  /*
  *
  * sw3aParams2
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
    * sw3aAlgoAttr
    *
    */
    {
        "sample2", /// sw3aAlgoAttr.name
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
        0, 1, 0, 48436, /// major, minor, debug, check
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
                {   1 * 1024, {65667, 65624, 66445, 66459, 0    , 0    , 0    , 0    , 51229, 51229, 51229, 51229, 51229, 51229, 51229, 51229, 0    , 0    , 0    , 0    }},
                {   2 * 1024, {65667, 65624, 66445, 66459, 0    , 0    , 0    , 0    , 51229, 51229, 51229, 51229, 51229, 51229, 51229, 51229, 0    , 0    , 0    , 0    }},
                {   4 * 1024, {66454, 66479, 66543, 66476, 0    , 0    , 0    , 0    , 51229, 51229, 51229, 51229, 51229, 51229, 51229, 51229, 0    , 0    , 0    , 0    }},
                {   8 * 1024, {66454, 66526, 66466, 66315, 0    , 0    , 0    , 0    , 51229, 51229, 51229, 51229, 51229, 51229, 51229, 51229, 0    , 0    , 0    , 0    }},
                {  16 * 1024, {65900, 65095, 65296, 64952, 0    , 0    , 0    , 0    , 51229, 51229, 51229, 51229, 51229, 51229, 51229, 51229, 0    , 0    , 0    , 0    }},
                {  32 * 1024, {65499, 63664, 63685, 62006, 0    , 0    , 0    , 0    , 51229, 51229, 51229, 51229, 51229, 51229, 51229, 51229, 0    , 0    , 0    , 0    }},
                {  64 * 1024, {65614, 62494, 62574, 62270, 0    , 0    , 0    , 0    , 51229, 51229, 51229, 51229, 51229, 51229, 51229, 51229, 0    , 0    , 0    , 0    }},
                { 128 * 1024, {65536, 63494, 63574, 63270, 0    , 0    , 0    , 0    , 51229, 51229, 51229, 51229, 51229, 51229, 51229, 51229, 0    , 0    , 0    , 0    }},
                { 256 * 1024, {65514, 62494, 62574, 63270, 0    , 0    , 0    , 0    , 51229, 51229, 51229, 51229, 51229, 51229, 51229, 51229, 0    , 0    , 0    , 0    }},
                { 512 * 1024, {65000, 63494, 63574, 63270, 0    , 0    , 0    , 0    , 51229, 51229, 51229, 51229, 51229, 51229, 51229, 51229, 0    , 0    , 0    , 0    }},
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
                {24576 , {3    }},
                {49152 , {3    }},
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
                /// 0     1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16
                {50    , {256  , 2300 , 100  , 990  , 1    , 85   , 50   , 160  , 0    , 160  , 99   , 5    , 30   , 50   , 100  , 2500 ,0   ,0   }},
                {100   , {256  , 2300 , 100  , 990  , 1    , 85   , 50   , 160  , 0    , 150  , 99   , 5    , 30   , 50   , 100  , 2500 ,0   ,0   }},
                {150   , {256  , 2300 , 100  , 990  , 1    , 85   , 50   , 160  , 0    , 140  , 99   , 5    , 30   , 50   , 100  , 2500 ,0   ,0   }},
                {200   , {228  , 2000 , 100  , 990  , 1    , 85   , 50   , 160  , 0    , 130  , 99   , 5    , 30   , 50   , 100  , 2500 ,0   ,0   }},
                {250   , {200  , 1800 , 100  , 990  , 1    , 60   , 50   , 160  , 0    , 120  , 99   , 5    , 30   , 50   , 100  , 2500 ,0   ,0   }},
                {300   , {200  , 1800 , 100  , 990  , 1    , 60   , 50   , 160  , 0    , 100  , 99   , 5    , 30   , 50   , 100  , 2500 ,0   ,0   }},
                {350   , {150  , 1200 , 100  , 990  , 1    , 60   , 50   , 150  , 0    , 90   , 99   , 5    , 30   , 50   , 100  , 2000 ,0   ,0   }},
                {400   , {100  , 1200 , 100  , 990  , 1    , 60   , 50   , 150  , 0    , 80   , 99   , 5    , 30   , 50   , 100  , 1000 ,0   ,0   }},
                {450   , {100  , 500  , 100  , 990  , 1    , 60   , 50   , 150  , 0    , 70   , 99   , 5    , 30   , 50   , 100  , 1000  ,0   ,0   }},
                {500   , {100  , 0    , 100  , 990  , 1    , 60   , 50   , 150  , 0    , 70   , 99   , 5    , 30   , 50   , 100  , 0    ,0   ,0   }},
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
                /// 0     1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16
                {50    , {256  , 2048 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 160  , 99   , 5    , 30   , 50   , 100  , 2048 ,0   ,0   }},
                {100   , {256  , 2048 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 150  , 99   , 5    , 30   , 50   , 100  , 2048 ,0   ,0   }},
                {150   , {256  , 2072 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 140  , 99   , 5    , 30   , 50   , 100  , 3072 ,0   ,0   }},
                {200   , {228  , 2072 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 130  , 99   , 5    , 30   , 50   , 100  , 3072 ,0   ,0   }},
                {250   , {176  , 2072 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 120  , 99   , 5    , 30   , 50   , 100  , 3072 ,0   ,0   }},
                {300   , {100  , 1750 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 100  , 99   , 5    , 30   , 50   , 100  , 1750 ,0   ,0   }},
                {350   , {100  , 875  , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 90   , 99   , 5    , 30   , 50   , 100  , 875  ,0   ,0   }},
                {400   , {100  , 0    , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 80   , 99   , 5    , 30   , 50   , 100  , 0    ,0   ,0   }},
                {450   , {100  , 0    , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 70   , 99   , 5    , 30   , 50   , 100  , 0    ,0   ,0   }},
                {500   , {100  , 0    , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 70   , 99   , 5    , 30   , 50   , 100  , 0    ,0   ,0   }},
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
                {   1 * 1024, {320  }},
                {   2 * 1024, {320  }},
                {   4 * 1024, {320  }},
                {   8 * 1024, {310  }},
                {  16 * 1024, {310  }},
                {  32 * 1024, {310  }},
                {  64 * 1024, {300  }},
                { 128 * 1024, {300  }},
                { 256 * 1024, {285  }},
                { 512 * 1024, {256  }},
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
                {50    , {360  }},
                {100   , {360  }},
                {150   , {340  }},
                {200   , {320  }},
                {250   , {320  }},
                {300   , {320  }},
                {350   , {300  }},
                {400   , {300  }},
                {450   , {300  }},
                {500   , {230  }},
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
        100   ,  /// ggcLowisoThr
        6400  ,  /// ggcHighisoThr
        20    ,  /// ggcConvergeSpeed
        0     ,  /// ggcBasicSelection
        2     ,  /// ggcContrastEnhanceEnable
        30    ,  /// ggcContrastManualStr
        0     ,  /// ggcDehazeStrengthLight
        0     ,  /// ggcDehazeStrengthDark
        50    ,  /// ggcLocalLumAdjNode0
        50    ,  /// ggcLocalLumAdjNode1
        50    ,  /// ggcLocalLumAdjNode2
        50    ,  /// ggcLocalLumAdjNode3
        50    ,  /// ggcLocalLumAdjNode4
        50    ,  /// ggcLocalLumAdjNode5
        50    ,  /// ggcLocalLumAdjNode6
        0     ,  /// ggcLocalAdaptiveNode0
        0     ,  /// ggcLocalAdaptiveNode1
        0     ,  /// ggcLocalAdaptiveNode2
        0     ,  /// ggcLocalAdaptiveNode3
        0     ,  /// ggcLocalAdaptiveNode4
        0     ,  /// ggcLocalAdaptiveNode5
        0     ,  /// ggcLocalAdaptiveNode6
        128   ,  /// ggcGlobalLuminationAdj
        1     ,  /// ggcContrastAutoMode
        250   ,  /// ggcContrastAutoStr
        220   ,  /// ggcContrastSmoothRate
        /*
        *
        * ggcLuxInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]lux
                /// [1]ggcContrastAutoStr
                /// [2]ggcLocalLumAdjNode0
                /// [3]ggcLocalLumAdjNode1
                /// [4]ggcLocalLumAdjNode5
                /// [5]ggcLocalLumAdjNode6
                /// 0     1      2      3      4      5
                {50    , {50   , 60   , 50   , 50   , 50   }},
                {100   , {50   , 60   , 50   , 50   , 50   }},
                {150   , {50   , 55   , 50   , 50   , 50   }},
                {200   , {50   , 55   , 50   , 50   , 50   }},
                {250   , {50   , 50   , 50   , 50   , 50   }},
                {300   , {50   , 50   , 50   , 50   , 50   }},
                {350   , {50   , 50   , 50   , 50   , 50   }},
                {400   , {100  , 50   , 50   , 50   , 50   }},
                {450   , {250  , 50   , 50   , 50   , 50   }},
                {500   , {280  , 50   , 50   , 50   , 50   }},
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
                /// 0          1      2
                {   1 * 1024, {0    , 0    }},
                {   2 * 1024, {0    , 0    }},
                {   4 * 1024, {0    , 0    }},
                {   8 * 1024, {0    , 0    }},
                {  16 * 1024, {0    , 0    }},
                {  32 * 1024, {0    , 0    }},
                {  64 * 1024, {0    , 0    }},
                { 128 * 1024, {0    , 0    }},
                { 256 * 1024, {0    , 0    }},
                { 512 * 1024, {0    , 0    }},
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
                /// 0          1      2      3      4      5      6      7      8      9      10     11     12
                {   1 * 1024, {0    , 1    , 0    , 10   , 200  , 16   , 100  , 0    , 256  , 256  , 64   , 0    }},
                {   2 * 1024, {0    , 1    , 0    , 10   , 200  , 16   , 100  , 0    , 256  , 256  , 64   , 0    }},
                {   4 * 1024, {0    , 1    , 0    , 10   , 200  , 16   , 100  , 0    , 256  , 256  , 64   , 0    }},
                {   8 * 1024, {0    , 1    , 0    , 10   , 200  , 16   , 100  , 0    , 256  , 256  , 64   , 0    }},
                {  16 * 1024, {0    , 1    , 0    , 10   , 200  , 16   , 100  , 0    , 256  , 256  , 64   , 0    }},
                {  32 * 1024, {0    , 2    , 0    , 10   , 300  , 16   , 100  , 0    , 256  , 256  , 64   , 0    }},
                {  64 * 1024, {0    , 4    , 0    , 10   , 300  , 16   , 100  , 0    , 256  , 256  , 64   , 0    }},
                { 128 * 1024, {0    , 4    , 0    , 10   , 400  , 16   , 100  , 0    , 256  , 256  , 64   , 0    }},
                { 256 * 1024, {0    , 4    , 0    , 10   , 500  , 16   , 100  , 0    , 256  , 256  , 64   , 0    }},
                { 512 * 1024, {0    , 4    , 0    , 10   , 500  , 16   , 100  , 0    , 256  , 256  , 64   , 0    }},
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
        * rnrGainInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]rnrBltLfNpGain
                /// [2]rnrBltLfNpStr
                /// 0          1      2
                {   1 * 1024, {16  , 100  }},
                {   2 * 1024, {16  , 100  }},
                {   4 * 1024, {16  , 100  }},
                {   8 * 1024, {16  , 100  }},
                {  16 * 1024, {16  , 100  }},
                {  32 * 1024, {16  , 100  }},
                {  64 * 1024, {16  , 100  }},
                { 128 * 1024, {50  , 120  }},
                { 256 * 1024, {80  , 120  }},
                { 512 * 1024, {200  ,200  }},
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
        /*
        *
        * rnrWeightLutTab
        *
        */
        {
            10    ,  /// validSize
            {
                {
                    1024  ,  /// region
                    {
                        {
                            63  , 57  , 52  , 47  , 42  , 38  , 35  , 31  , 28  , 26  , 23  , 21  , 19  , 17  , 16  , 14  ,
                            13  , 12  , 10  , 9   , 9   , 8   , 7   , 6   , 6   , 5   , 5   , 4   , 4   , 3   , 3   , 3   ,
                            3   , 2   , 2   , 2   , 2   , 2   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                            1   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,
                            0   ,
                        },
                    },
                },
                {
                    2048  ,  /// region
                    {
                        {
                            63  , 57  , 52  , 47  , 42  , 38  , 35  , 31  , 28  , 26  , 23  , 21  , 19  , 17  , 16  , 14  ,
                            13  , 12  , 10  , 9   , 9   , 8   , 7   , 6   , 6   , 5   , 5   , 4   , 4   , 3   , 3   , 3   ,
                            3   , 2   , 2   , 2   , 2   , 2   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                            1   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,
                            0   ,
                        },
                    },
                },
                {
                    4096  ,  /// region
                    {
                        {
                            63  , 58  , 54  , 50  , 46  , 43  , 40  , 37  , 34  , 32  , 29  , 27  , 25  , 23  , 21  , 20  ,
                            18  , 17  , 16  , 15  , 14  , 13  , 12  , 11  , 10  , 9   , 9   , 8   , 7   , 7   , 6   , 6   ,
                            5   , 5   , 5   , 4   , 4   , 4   , 3   , 3   , 3   , 3   , 2   , 2   , 2   , 2   , 2   , 2   ,
                            2   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 0   ,
                            0   ,
                        },
                    },
                },
                {
                    8192  ,  /// region
                    {
                        {
                            63  , 58  , 54  , 50  , 46  , 43  , 40  , 37  , 34  , 32  , 29  , 27  , 25  , 23  , 21  , 20  ,
                            18  , 17  , 16  , 15  , 14  , 13  , 12  , 11  , 10  , 9   , 9   , 8   , 7   , 7   , 6   , 6   ,
                            5   , 5   , 5   , 4   , 4   , 4   , 3   , 3   , 3   , 3   , 2   , 2   , 2   , 2   , 2   , 2   ,
                            2   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 0   ,
                            0   ,
                        },
                    },
                },
                {
                    16384 ,  /// region
                    {
                        {
                            63  , 59  , 55  , 52  , 48  , 45  , 42  , 40  , 37  , 35  , 32  , 30  , 28  , 26  , 25  , 23  ,
                            22  , 20  , 19  , 18  , 17  , 16  , 15  , 14  , 13  , 12  , 11  , 10  , 10  , 9   , 9   , 8   ,
                            7   , 7   , 7   , 6   , 6   , 5   , 5   , 5   , 4   , 4   , 4   , 4   , 3   , 3   , 3   , 3   ,
                            3   , 2   , 2   , 2   , 2   , 2   , 2   , 2   , 2   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                            1   ,
                        },
                    },
                },
                {
                    32768 ,  /// region
                    {
                        {
                            63  , 59  , 55  , 52  , 48  , 45  , 42  , 40  , 37  , 35  , 32  , 30  , 28  , 26  , 25  , 23  ,
                            22  , 20  , 19  , 18  , 17  , 16  , 15  , 14  , 13  , 12  , 11  , 10  , 10  , 9   , 9   , 8   ,
                            7   , 7   , 7   , 6   , 6   , 5   , 5   , 5   , 4   , 4   , 4   , 4   , 3   , 3   , 3   , 3   ,
                            3   , 2   , 2   , 2   , 2   , 2   , 2   , 2   , 2   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                            1   ,
                        },
                    },
                },
                {
                    65536 ,  /// region
                    {
                        {
                            63  , 59  , 55  , 52  , 48  , 45  , 42  , 40  , 37  , 35  , 32  , 30  , 28  , 26  , 25  , 23  ,
                            22  , 20  , 19  , 18  , 17  , 16  , 15  , 14  , 13  , 12  , 11  , 10  , 10  , 9   , 9   , 8   ,
                            7   , 7   , 7   , 6   , 6   , 5   , 5   , 5   , 4   , 4   , 4   , 4   , 3   , 3   , 3   , 3   ,
                            3   , 2   , 2   , 2   , 2   , 2   , 2   , 2   , 2   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                            1   ,
                        },
                    },
                },
                {
                    131072,  /// region
                    {
                        {
                            63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  ,
                            63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  ,
                            53  , 45  , 38  , 32  , 27  , 23  , 20  , 17  , 14  , 12  , 10  , 9   , 7   , 6   , 5   , 4   ,
                            4   , 3   , 3   , 2   , 2   , 2   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 0   , 0   , 0   ,
                            0   ,
                        },
                    },
                },
                {
                    262144,  /// region
                    {
                        {
                            63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  ,
                            63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  ,
                            53  , 45  , 38  , 32  , 27  , 23  , 20  , 17  , 14  , 12  , 10  , 9   , 7   , 6   , 5   , 4   ,
                            4   , 3   , 3   , 2   , 2   , 2   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 0   , 0   , 0   ,
                            0   ,
                        },
                    },
                },
                {
                    524288,  /// region
                    {
                        {
                            63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  ,
                            63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  , 63  ,
                            53  , 45  , 38  , 32  , 27  , 23  , 20  , 17  , 14  , 12  , 10  , 9   , 7   , 6   , 5   , 4   ,
                            4   , 3   , 3   , 2   , 2   , 2   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 0   , 0   , 0   ,
                            0   ,
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
                             1,   3,   3,   4,   5,   5,   6,   6,   7,   7,   7,   8,   8,   8,   9,   9,
                             9,   9,  10,  10,  10,  10,  11,  11,  11,  11,  11,  12,  12,  12,  12,  13,
                            13,  13,  13,  13,  13,  14,  14,  14,  14,  14,  15,  15,  15,  15,  15,  15,
                            16,  16,  16,  16,  16,  16,  16,  17,  17,  17,  17,  17,  17,  17,  18,  18,
                            18,  18,  18,  18,  18,  19,  19,  19,  19,  19,  19,  19,  19,  20,  20,  20,
                            20,  20,  20,  20,  20,  21,  21,  21,  21,  21,  21,  21,  21,  21,  22,  22,
                            22,  22,  22,  22,  22,  22,  22,  23,  23,  23,  23,  23,  23,  23,  23,  23,
                            24,  24,  24,  24,  24,  24,  24,  24,  24,  24,  25,  25,  25,  25,  25,  25,
                        },
                    },
                },
                {
                    2048  ,  /// region
                    {
                        {
                             1,   3,   3,   4,   5,   5,   6,   6,   7,   7,   7,   8,   8,   8,   9,   9,
                             9,   9,  10,  10,  10,  10,  11,  11,  11,  11,  11,  12,  12,  12,  12,  13,
                            13,  13,  13,  13,  13,  14,  14,  14,  14,  14,  15,  15,  15,  15,  15,  15,
                            16,  16,  16,  16,  16,  16,  16,  17,  17,  17,  17,  17,  17,  17,  18,  18,
                            18,  18,  18,  18,  18,  19,  19,  19,  19,  19,  19,  19,  19,  20,  20,  20,
                            20,  20,  20,  20,  20,  21,  21,  21,  21,  21,  21,  21,  21,  21,  22,  22,
                            22,  22,  22,  22,  22,  22,  22,  23,  23,  23,  23,  23,  23,  23,  23,  23,
                            24,  24,  24,  24,  24,  24,  24,  24,  24,  24,  25,  25,  25,  25,  25,  25,
                        },
                    },
                },
                {
                    4096  ,  /// region
                    {
                        {
                             4,   6,   7,   8,   9,  10,  10,  11,  12,  12,  13,  14,  14,  15,  15,  16,
                            16,  17,  17,  18,  18,  18,  19,  19,  20,  20,  20,  21,  21,  21,  22,  22,
                            23,  23,  23,  24,  24,  24,  24,  25,  25,  25,  26,  26,  26,  27,  27,  27,
                            27,  28,  28,  28,  28,  29,  29,  29,  30,  30,  30,  30,  31,  31,  31,  31,
                            32,  32,  32,  32,  32,  33,  33,  33,  33,  34,  34,  34,  34,  35,  35,  35,
                            35,  35,  36,  36,  36,  36,  36,  37,  37,  37,  37,  37,  38,  38,  38,  38,
                            38,  39,  39,  39,  39,  39,  40,  40,  40,  40,  40,  41,  41,  41,  41,  41,
                            42,  42,  42,  42,  42,  42,  43,  43,  43,  43,  43,  43,  44,  44,  44,  44,
                        },
                    },
                },
                {
                    8192  ,  /// region
                    {
                        {
                             6,   8,  10,  11,  12,  13,  14,  15,  16,  17,  17,  18,  19,  20,  20,  21,
                            21,  22,  23,  23,  24,  24,  25,  25,  26,  26,  27,  27,  28,  28,  29,  29,
                            30,  30,  30,  31,  31,  32,  32,  32,  33,  33,  34,  34,  34,  35,  35,  36,
                            36,  36,  37,  37,  37,  38,  38,  38,  39,  39,  39,  40,  40,  40,  41,  41,
                            41,  42,  42,  42,  42,  43,  43,  43,  44,  44,  44,  45,  45,  45,  45,  46,
                            46,  46,  47,  47,  47,  47,  48,  48,  48,  48,  49,  49,  49,  49,  50,  50,
                            50,  51,  51,  51,  51,  52,  52,  52,  52,  53,  53,  53,  53,  53,  54,  54,
                            54,  54,  55,  55,  55,  55,  56,  56,  56,  56,  57,  57,  57,  57,  57,  58,
                        },
                    },
                },
                {
                    16384 ,  /// region
                    {
                        {
                             8,  11,  12,  14,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
                            27,  28,  29,  30,  30,  31,  32,  32,  33,  34,  34,  35,  35,  36,  37,  37,
                            38,  38,  39,  39,  40,  40,  41,  41,  42,  42,  43,  43,  44,  44,  45,  45,
                            46,  46,  47,  47,  48,  48,  48,  49,  49,  50,  50,  51,  51,  51,  52,  52,
                            53,  53,  53,  54,  54,  55,  55,  55,  56,  56,  57,  57,  57,  58,  58,  58,
                            59,  59,  59,  60,  60,  60,  61,  61,  61,  62,  62,  63,  63,  63,  64,  64,
                            64,  64,  65,  65,  65,  66,  66,  66,  67,  67,  67,  68,  68,  68,  69,  69,
                            69,  70,  70,  70,  70,  71,  71,  71,  72,  72,  72,  72,  73,  73,  73,  74,
                        },
                    },
                },
                {
                    32768 ,  /// region
                    {
                        {
                            13,  15,  18,  20,  22,  23,  25,  26,  28,  29,  30,  32,  33,  34,  35,  36,
                            37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  46,  47,  48,  49,  50,  50,
                            51,  52,  53,  53,  54,  55,  55,  56,  57,  57,  58,  59,  59,  60,  61,  61,
                            62,  62,  63,  64,  64,  65,  65,  66,  67,  67,  68,  68,  69,  69,  70,  70,
                            71,  72,  72,  73,  73,  74,  74,  75,  75,  76,  76,  77,  77,  78,  78,  79,
                            79,  80,  80,  81,  81,  81,  82,  82,  83,  83,  84,  84,  85,  85,  86,  86,
                            86,  87,  87,  88,  88,  89,  89,  89,  90,  90,  91,  91,  92,  92,  92,  93,
                            93,  94,  94,  94,  95,  95,  96,  96,  96,  97,  97,  98,  98,  98,  99,  99,
                        },
                    },
                },
                {
                    65536 ,  /// region
                    {
                        {
                             13,  18,  22,  25,  28,  31,  33,  35,  37,  39,  41,  43,  45,  47,  48,  50,
                             51,  53,  54,  55,  57,  58,  59,  61,  62,  63,  64,  66,  67,  68,  69,  70,
                             71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,
                             87,  87,  88,  89,  90,  91,  92,  93,  93,  94,  95,  96,  97,  97,  98,  99,
                            100, 100, 101, 102, 103, 103, 104, 105, 106, 106, 107, 108, 108, 109, 110, 110,
                            111, 112, 113, 113, 114, 115, 115, 116, 117, 117, 118, 118, 119, 120, 120, 121,
                            122, 122, 123, 123, 124, 125, 125, 126, 127, 127, 128, 128, 129, 129, 130, 131,
                            131, 132, 132, 133, 134, 134, 135, 135, 136, 136, 137, 137, 138, 139, 139, 140,
                        },
                    },
                },
                {
                    131072,  /// region
                    {
                        {
                             13,  18,  22,  25,  28,  31,  33,  35,  37,  39,  41,  43,  45,  47,  48,  50,
                             51,  53,  54,  55,  57,  58,  59,  61,  62,  63,  64,  66,  67,  68,  69,  70,
                             71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,
                             87,  87,  88,  89,  90,  91,  92,  93,  93,  94,  95,  96,  97,  97,  98,  99,
                            100, 100, 101, 102, 103, 103, 104, 105, 106, 106, 107, 108, 108, 109, 110, 110,
                            111, 112, 113, 113, 114, 115, 115, 116, 117, 117, 118, 118, 119, 120, 120, 121,
                            122, 122, 123, 123, 124, 125, 125, 126, 127, 127, 128, 128, 129, 129, 130, 131,
                            131, 132, 132, 133, 134, 134, 135, 135, 136, 136, 137, 137, 138, 139, 139, 140,
                        },
                    },
                },
                {
                    262144,  /// region
                    {
                        {
                             13,  18,  22,  25,  28,  31,  33,  35,  37,  39,  41,  43,  45,  47,  48,  50,
                             51,  53,  54,  55,  57,  58,  59,  61,  62,  63,  64,  66,  67,  68,  69,  70,
                             71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,
                             87,  87,  88,  89,  90,  91,  92,  93,  93,  94,  95,  96,  97,  97,  98,  99,
                            100, 100, 101, 102, 103, 103, 104, 105, 106, 106, 107, 108, 108, 109, 110, 110,
                            111, 112, 113, 113, 114, 115, 115, 116, 117, 117, 118, 118, 119, 120, 120, 121,
                            122, 122, 123, 123, 124, 125, 125, 126, 127, 127, 128, 128, 129, 129, 130, 131,
                            131, 132, 132, 133, 134, 134, 135, 135, 136, 136, 137, 137, 138, 139, 139, 140,
                        },
                    },
                },
                {
                    524288,  /// region
                    {
                        {
                             13,  18,  22,  25,  28,  31,  33,  35,  37,  39,  41,  43,  45,  47,  48,  50,
                             51,  53,  54,  55,  57,  58,  59,  61,  62,  63,  64,  66,  67,  68,  69,  70,
                             71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,
                             87,  87,  88,  89,  90,  91,  92,  93,  93,  94,  95,  96,  97,  97,  98,  99,
                            100, 100, 101, 102, 103, 103, 104, 105, 106, 106, 107, 108, 108, 109, 110, 110,
                            111, 112, 113, 113, 114, 115, 115, 116, 117, 117, 118, 118, 119, 120, 120, 121,
                            122, 122, 123, 123, 124, 125, 125, 126, 127, 127, 128, 128, 129, 129, 130, 131,
                            131, 132, 132, 133, 134, 134, 135, 135, 136, 136, 137, 137, 138, 139, 139, 140,
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
                /// 0          1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     17     18     19     20     21     22     23     24
                {   1 * 1024, {40   , 5000 , 253  , 1350 , 235  , 50   , 255  , 50   , 125  , 0    , 64   , 50   , 160  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 5000 , 96   , 0    }},
                {   2 * 1024, {40   , 5000 , 253  , 1350 , 235  , 50   , 255  , 50   , 122  , 0    , 64   , 50   , 160  , 50   , 255  , 90   , 140  , 435  , 201  , 369  , 178  , 5000 , 96   , 0    }},
                {   4 * 1024, {40   , 5000 , 253  , 1350 , 235  , 50   , 235  , 50   , 100  , 0    , 64   , 50   , 160  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 5000 , 96   , 0    }},
                {   8 * 1024, {40   , 5000 , 253  , 1350 , 235  , 50   , 235  , 50   , 100  , 0    , 64   , 50   , 160  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 4500 , 96   , 0    }},
                {  16 * 1024, {40   , 5000 , 253  , 1350 , 235  , 50   , 235  , 50   , 100  , 0    , 64   , 50   , 160  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 4300 , 64   , 0    }},
                {  32 * 1024, {20   , 5000 , 253  , 1350 , 235  , 40   , 235  , 50   , 100  , 0    , 48   , 50   , 180  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 4096 , 64   , 0    }},
                {  64 * 1024, {20   , 5000 , 253  , 1350 , 235  , 25   , 235  , 50   , 100  , 0    , 32   , 50   , 180  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 1500 , 64   , 0    }},
                { 128 * 1024, {20   , 5000 , 253  , 1350 , 140  , 25   , 180  , 50   , 100  , 0    , 32   , 50   , 180  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 1000  , 32   , 0    }},
                { 256 * 1024, {10   , 2400 , 253  , 1350 , 140  , 0    , 180  , 50   , 80   , 0    , 32   , 50   , 120  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 200  , 32   , 0    }},
                { 512 * 1024, {10   , 2400 , 253  , 1350 , 140  , 0    , 180  , 50   , 80   , 0    , 32   , 50   , 120  , 50   , 255  , 90   , 150  , 435  , 201  , 369  , 178  , 200  , 32   , 0    }},
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
                             1,   3,   3,   4,   5,   5,   6,   6,   7,   7,   7,   8,   8,   8,   9,   9,
                             9,   9,  10,  10,  10,  10,  11,  11,  11,  11,  11,  12,  12,  12,  12,  13,
                            13,  13,  13,  13,  13,  14,  14,  14,  14,  14,  15,  15,  15,  15,  15,  15,
                            16,  16,  16,  16,  16,  16,  16,  17,  17,  17,  17,  17,  17,  17,  18,  18,
                            18,  18,  18,  18,  18,  19,  19,  19,  19,  19,  19,  19,  19,  20,  20,  20,
                            20,  20,  20,  20,  20,  21,  21,  21,  21,  21,  21,  21,  21,  21,  22,  22,
                            22,  22,  22,  22,  22,  22,  22,  23,  23,  23,  23,  23,  23,  23,  23,  23,
                            24,  24,  24,  24,  24,  24,  24,  24,  24,  24,  25,  25,  25,  25,  25,  25,
                        },
                    },
                },
                {
                    2048  ,  /// region
                    {
                        {
                             1,   3,   3,   4,   5,   5,   6,   6,   7,   7,   7,   8,   8,   8,   9,   9,
                             9,   9,  10,  10,  10,  10,  11,  11,  11,  11,  11,  12,  12,  12,  12,  13,
                            13,  13,  13,  13,  13,  14,  14,  14,  14,  14,  15,  15,  15,  15,  15,  15,
                            16,  16,  16,  16,  16,  16,  16,  17,  17,  17,  17,  17,  17,  17,  18,  18,
                            18,  18,  18,  18,  18,  19,  19,  19,  19,  19,  19,  19,  19,  20,  20,  20,
                            20,  20,  20,  20,  20,  21,  21,  21,  21,  21,  21,  21,  21,  21,  22,  22,
                            22,  22,  22,  22,  22,  22,  22,  23,  23,  23,  23,  23,  23,  23,  23,  23,
                            24,  24,  24,  24,  24,  24,  24,  24,  24,  24,  25,  25,  25,  25,  25,  25,
                        },
                    },
                },
                {
                    4096  ,  /// region
                    {
                        {
                             4,   6,   7,   8,   9,  10,  10,  11,  12,  12,  13,  14,  14,  15,  15,  16,
                            16,  17,  17,  18,  18,  18,  19,  19,  20,  20,  20,  21,  21,  21,  22,  22,
                            23,  23,  23,  24,  24,  24,  24,  25,  25,  25,  26,  26,  26,  27,  27,  27,
                            27,  28,  28,  28,  28,  29,  29,  29,  30,  30,  30,  30,  31,  31,  31,  31,
                            32,  32,  32,  32,  32,  33,  33,  33,  33,  34,  34,  34,  34,  35,  35,  35,
                            35,  35,  36,  36,  36,  36,  36,  37,  37,  37,  37,  37,  38,  38,  38,  38,
                            38,  39,  39,  39,  39,  39,  40,  40,  40,  40,  40,  41,  41,  41,  41,  41,
                            42,  42,  42,  42,  42,  42,  43,  43,  43,  43,  43,  43,  44,  44,  44,  44,
                        },
                    },
                },
                {
                    8192  ,  /// region
                    {
                        {
                             6,   8,  10,  11,  12,  13,  14,  15,  16,  17,  17,  18,  19,  20,  20,  21,
                            21,  22,  23,  23,  24,  24,  25,  25,  26,  26,  27,  27,  28,  28,  29,  29,
                            30,  30,  30,  31,  31,  32,  32,  32,  33,  33,  34,  34,  34,  35,  35,  36,
                            36,  36,  37,  37,  37,  38,  38,  38,  39,  39,  39,  40,  40,  40,  41,  41,
                            41,  42,  42,  42,  42,  43,  43,  43,  44,  44,  44,  45,  45,  45,  45,  46,
                            46,  46,  47,  47,  47,  47,  48,  48,  48,  48,  49,  49,  49,  49,  50,  50,
                            50,  51,  51,  51,  51,  52,  52,  52,  52,  53,  53,  53,  53,  53,  54,  54,
                            54,  54,  55,  55,  55,  55,  56,  56,  56,  56,  57,  57,  57,  57,  57,  58,
                        },
                    },
                },
                {
                    16384 ,  /// region
                    {
                        {
                             8,  11,  12,  14,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
                            27,  28,  29,  30,  30,  31,  32,  32,  33,  34,  34,  35,  35,  36,  37,  37,
                            38,  38,  39,  39,  40,  40,  41,  41,  42,  42,  43,  43,  44,  44,  45,  45,
                            46,  46,  47,  47,  48,  48,  48,  49,  49,  50,  50,  51,  51,  51,  52,  52,
                            53,  53,  53,  54,  54,  55,  55,  55,  56,  56,  57,  57,  57,  58,  58,  58,
                            59,  59,  59,  60,  60,  60,  61,  61,  61,  62,  62,  63,  63,  63,  64,  64,
                            64,  64,  65,  65,  65,  66,  66,  66,  67,  67,  67,  68,  68,  68,  69,  69,
                            69,  70,  70,  70,  70,  71,  71,  71,  72,  72,  72,  72,  73,  73,  73,  74,
                        },
                    },
                },
                {
                    32768 ,  /// region
                    {
                        {
                            13,  15,  18,  20,  22,  23,  25,  26,  28,  29,  30,  32,  33,  34,  35,  36,
                            37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  46,  47,  48,  49,  50,  50,
                            51,  52,  53,  53,  54,  55,  55,  56,  57,  57,  58,  59,  59,  60,  61,  61,
                            62,  62,  63,  64,  64,  65,  65,  66,  67,  67,  68,  68,  69,  69,  70,  70,
                            71,  72,  72,  73,  73,  74,  74,  75,  75,  76,  76,  77,  77,  78,  78,  79,
                            79,  80,  80,  81,  81,  81,  82,  82,  83,  83,  84,  84,  85,  85,  86,  86,
                            86,  87,  87,  88,  88,  89,  89,  89,  90,  90,  91,  91,  92,  92,  92,  93,
                            93,  94,  94,  94,  95,  95,  96,  96,  96,  97,  97,  98,  98,  98,  99,  99,
                        },
                    },
                },
                {
                    65536 ,  /// region
                    {
                        {
                             13,  18,  22,  25,  28,  31,  33,  35,  37,  39,  41,  43,  45,  47,  48,  50,
                             51,  53,  54,  55,  57,  58,  59,  61,  62,  63,  64,  66,  67,  68,  69,  70,
                             71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,
                             87,  87,  88,  89,  90,  91,  92,  93,  93,  94,  95,  96,  97,  97,  98,  99,
                            100, 100, 101, 102, 103, 103, 104, 105, 106, 106, 107, 108, 108, 109, 110, 110,
                            111, 112, 113, 113, 114, 115, 115, 116, 117, 117, 118, 118, 119, 120, 120, 121,
                            122, 122, 123, 123, 124, 125, 125, 126, 127, 127, 128, 128, 129, 129, 130, 131,
                            131, 132, 132, 133, 134, 134, 135, 135, 136, 136, 137, 137, 138, 139, 139, 140,
                        },
                    },
                },
                {
                    131072,  /// region
                    {
                        {
                             13,  18,  22,  25,  28,  31,  33,  35,  37,  39,  41,  43,  45,  47,  48,  50,
                             51,  53,  54,  55,  57,  58,  59,  61,  62,  63,  64,  66,  67,  68,  69,  70,
                             71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,
                             87,  87,  88,  89,  90,  91,  92,  93,  93,  94,  95,  96,  97,  97,  98,  99,
                            100, 100, 101, 102, 103, 103, 104, 105, 106, 106, 107, 108, 108, 109, 110, 110,
                            111, 112, 113, 113, 114, 115, 115, 116, 117, 117, 118, 118, 119, 120, 120, 121,
                            122, 122, 123, 123, 124, 125, 125, 126, 127, 127, 128, 128, 129, 129, 130, 131,
                            131, 132, 132, 133, 134, 134, 135, 135, 136, 136, 137, 137, 138, 139, 139, 140,
                        },
                    },
                },
                {
                    262144,  /// region
                    {
                        {
                             13,  18,  22,  25,  28,  31,  33,  35,  37,  39,  41,  43,  45,  47,  48,  50,
                             51,  53,  54,  55,  57,  58,  59,  61,  62,  63,  64,  66,  67,  68,  69,  70,
                             71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,
                             87,  87,  88,  89,  90,  91,  92,  93,  93,  94,  95,  96,  97,  97,  98,  99,
                            100, 100, 101, 102, 103, 103, 104, 105, 106, 106, 107, 108, 108, 109, 110, 110,
                            111, 112, 113, 113, 114, 115, 115, 116, 117, 117, 118, 118, 119, 120, 120, 121,
                            122, 122, 123, 123, 124, 125, 125, 126, 127, 127, 128, 128, 129, 129, 130, 131,
                            131, 132, 132, 133, 134, 134, 135, 135, 136, 136, 137, 137, 138, 139, 139, 140,
                        },
                    },
                },
                {
                    524288,  /// region
                    {
                        {
                             13,  18,  22,  25,  28,  31,  33,  35,  37,  39,  41,  43,  45,  47,  48,  50,
                             51,  53,  54,  55,  57,  58,  59,  61,  62,  63,  64,  66,  67,  68,  69,  70,
                             71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,
                             87,  87,  88,  89,  90,  91,  92,  93,  93,  94,  95,  96,  97,  97,  98,  99,
                            100, 100, 101, 102, 103, 103, 104, 105, 106, 106, 107, 108, 108, 109, 110, 110,
                            111, 112, 113, 113, 114, 115, 115, 116, 117, 117, 118, 118, 119, 120, 120, 121,
                            122, 122, 123, 123, 124, 125, 125, 126, 127, 127, 128, 128, 129, 129, 130, 131,
                            131, 132, 132, 133, 134, 134, 135, 135, 136, 136, 137, 137, 138, 139, 139, 140,
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
                /// 0          1      2      3      4
                {   1 * 1024, {2    , 4    , 0    , 3    }},
                {   2 * 1024, {2    , 5    , 0    , 3    }},
                {   4 * 1024, {2    , 5    , 0    , 4    }},
                {   8 * 1024, {2    , 6    , 1    , 4    }},
                {  16 * 1024, {2    , 5    , 0    , 4    }},
                {  32 * 1024, {2    , 6    , 0    , 4    }},
                {  64 * 1024, {3    , 6    , 0    , 4    }},
                { 128 * 1024, {3    , 5    , 0    , 4    }},
                { 256 * 1024, {3    , 5    , 0    , 4    }},
                { 512 * 1024, {3    , 3    , 0    , 4    }},

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
                /// [1]mvdNpGain
                /// [2]mvdNpOffset
                /// [3]mvdPipeaThr
                /// [4]mvdPipeABTriDirStr
                /// 0          1      2      3      4
                {   1 * 1024, {48   , 3    , 2    , 128   }},
                {   2 * 1024, {43   , 2    , 2    , 128   }},
                {   4 * 1024, {28   , 2    , 2    , 128   }},
                {   8 * 1024, {27   , 2    , 4    , 200   }},
                {  16 * 1024, {26   , 2    , 6    , 256   }},
                {  32 * 1024, {25   , 2    , 6    , 256   }},
                {  64 * 1024, {22   , 1    , 6    , 256   }},
                { 128 * 1024, {18   , 1    , 6    , 256   }},
                { 256 * 1024, {18   , 1    , 4    , 256   }},
                { 512 * 1024, {17   , 1    , 3    , 256   }},
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
        /*
        *
        * tnrGainSwitchTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]tnrNr2dPyramidWt
                /// [2]tnrMotionCurveMid
                /// [3]tnrMotionCurveEnd
                /// [4]tnrNr2dCurveMid
                /// [5]tnrNr2dCurveEnd
                /// [6]tnrNr3dCurveMid
                /// [7]tnrNr3dCurveEnd
                /// 0          1      2      3      4      5      6      7
                {   1 * 1024, {2    , 7    , 7    , 6    , 7    , 6    , 7    }},
                {   2 * 1024, {2    , 7    , 7    , 6    , 7    , 7    , 7    }},
                {   4 * 1024, {2    , 6    , 6    , 6    , 7    , 7    , 7    }},
                {   8 * 1024, {2    , 6    , 6    , 6    , 7    , 7    , 7    }},
                {  16 * 1024, {2    , 6    , 6    , 6    , 6    , 7    , 7    }},
                {  32 * 1024, {2    , 6    , 6    , 6    , 6    , 7    , 7    }},
                {  64 * 1024, {2    , 6    , 6    , 6    , 6    , 7    , 7    }},
                { 128 * 1024, {2    , 6    , 6    , 6    , 6    , 7    , 7    }},
                { 256 * 1024, {2    , 6    , 6    , 6    , 6    , 7    , 7    }},
                { 512 * 1024, {2    , 6    , 6    , 6    , 6    , 7    , 7    }},
            },
        },
        /*
        *
        * tnrGainInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
                /// [1]tnrGlobalNoiseThr
                /// [2]tnrNr2dGradSlope
                /// [3]tnrNr2dYnoiseThr
                /// [4]tnrNr2dUVnoiseThr
                /// [5]tnrNr2dNoiseBackRatio
                /// [6]tnrMotionThr
                /// [7]tnrMotionCurveOffset
                /// [8]tnrMotionCurveY1
                /// [9]tnrMotionCurveY2
                /// [10]tnrMotionCurveY3
                /// [11]tnrNr2dCurveOffset
                /// [12]tnrNr2dCurveY1
                /// [13]tnrNr2dCurveY2
                /// [14]tnrNr2dCurveY3
                /// [15]tnrNr2dCurveC1
                /// [16]tnrNr2dCurveC2
                /// [17]tnrNr2dCurveC3
                /// [18]tnrNr3dCurveOffset
                /// [19]tnrNr3dCurveY1
                /// [20]tnrNr3dCurveY2
                /// [21]tnrNr3dCurveY3
                /// [22]tnrNr3dCurveC1
                /// [23]tnrNr3dCurveC2
                /// [24]tnrNr3dCurveC3
                /// 0          1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     17     18     19     20     21     22     23     24
                {   1 * 1024, {160  , 32   , 30   , 30   , 10   , 4    , 0    , 20   , 32   , 64   , 4    , 0    , 25   , 52   , 0    , 15   , 46   , 0    , 58   , 20   , 0    , 28   , 8   , 0    }},
                {   2 * 1024, {160  , 32   , 50   , 50   , 10   , 4    , 0    , 20   , 32   , 64   , 4    , 0    , 25   , 53   , 0    , 18   , 48   , 0    , 58   , 20   , 0    , 34   , 12   , 0    }},
                {   4 * 1024, {160  , 32   , 50   , 50   , 10    , 4    , 0    , 20   , 48   , 64   , 0    , 0    , 25   , 52   , 0    , 23   , 50   , 0    , 59   , 20   , 0    , 37   , 10   , 0    }},
                {   8 * 1024, {160  , 32   , 88   , 88   , 0    , 4    , 0    , 20   , 48   , 64   , 0    , 0    , 28   , 53   , 0    , 20   , 50   , 0    , 60   , 23   , 0    , 43   , 10   , 0    }},
                {  16 * 1024, {160  , 32   , 95   , 95   , 0    , 4    , 0    , 20   , 48   , 64   , 0    , 0    , 28   , 53   , 0    , 22   , 52   , 0    , 62   , 25   , 0    , 48    ,12   , 0    }},
                {  32 * 1024, {160  , 32   , 100  , 100  , 0    , 4    , 0    , 20   , 48   , 64   , 0    , 0    , 30   , 58   , 0    , 30   , 55   , 6    , 62   , 30   , 0    , 48   , 18   , 0    }},
                {  64 * 1024, {160  , 32   , 110  , 110  , 0    , 4    , 0    , 20   , 48   , 64   , 0    , 0    , 33   , 58   , 0    , 40   , 60   , 0    , 62   , 30   , 0    , 55   , 18   , 0    }},
                { 128 * 1024, {160  , 32   , 135  , 135  , 0    , 4    , 0    , 20   , 32   , 64   , 0    , 0    , 35   , 59   , 0    , 35   , 60   , 0    , 62   , 30   , 0    , 55   , 30   , 0    }},
                { 256 * 1024, {160  , 32   , 160  , 160  , 0    , 4    , 0    , 20   , 32   , 64   , 0    , 3    , 42   , 60   , 3    , 32   , 60   , 0    , 62   , 35   , 0    , 56   , 35   , 0   }}, 
                { 512 * 1024, {160  , 32   , 200  , 200  , 0    , 4    , 0    , 30   , 48   , 64   , 0    , 3    , 40   , 60   , 3    , 35   , 60   , 6    , 62   , 35   , 0    , 58   , 35   , 0   }},

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
        /*
        *
        * lshGainSwitchTab
        *
        */
        {
            10    ,  /// validSize
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
                /// 0          1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16
                {1 * 1024  ,  {6    , 6    , 5    , 6    , 4    , 4    , 0    , 7    , 8    , 8    , 8    , 1    , 7    , 6    , 7    , 8    }},
                {2 * 1024  ,  {6    , 6    , 5    , 6    , 4    , 4    , 0    , 7    , 8    , 8    , 8    , 1    , 7    , 6    , 7    , 8    }},
                {4 * 1024  ,  {6    , 6    , 5    , 6    , 4    , 4    , 0    , 7    , 8    , 8    , 8    , 1    , 7    , 6    , 7    , 8    }},
                {8 * 1024  ,  {6    , 6    , 5    , 6    , 4    , 4    , 0    , 7    , 8    , 8    , 8    , 1    , 7    , 6    , 7    , 8    }},
                {16 * 1024 ,  {6    , 6    , 5    , 6    , 4    , 4    , 0    , 8    , 8    , 8    , 8    , 1    , 7    , 6    , 7    , 8    }},
                {32 * 1024 ,  {6    , 6    , 5    , 6    , 4    , 4    , 0    , 8    , 8    , 8    , 8    , 1    , 7    , 6    , 7    , 8    }},
                {64 * 1024 ,  {4    , 4    , 5    , 6    , 4    , 4    , 0    , 8    , 8    , 8    , 8    , 1    , 7    , 6    , 7    , 8    }},
                {128 * 1024,  {4    , 4    , 5    , 6    , 4    , 4    , 0    , 8    , 8    , 8    , 8    , 1    , 7    , 6    , 7    , 8    }},
                {256 * 1024,  {4    , 4    , 5    , 6    , 4    , 4    , 0    , 8    , 8    , 8    , 8    , 1    , 7    , 6    , 7    , 8    }},
                {512 * 1024,  {4    , 4    , 5    , 6    , 4    , 4    , 0    , 8    , 8    , 8    , 8    , 1    , 7    , 6    , 7    , 8    }},
            },
        },
        /*
        *
        * lshGainInterpTab
        *
        */
        {
            10    ,  /// validSize
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
                /// 0          1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     17     18     19     20     21     22     23     24     25     26     27     28     29     30     31     32     33     34     35     36     37     38     39     40     41     42     43     44     45
                {   1 * 1024, {230  , 32   , 0    , 0    , 0    , 4000 , 100  , 0    , 256   , 0   , 0  , 8    , 0    , 0    , 0    , 256  , 200  , 0    , 0    , 100  , 100  , 256  , 100  , 16   , 256  , 1000  , 552  , 128  , 356  , 620  , 400  , 384  , 354  , 128  , 128  , 0    , 0    , 64   , 256  , 256  , 180  , 64   , 256  , 256  , 180   }},
                {   2 * 1024, {220  , 32   , 0    , 0    , 0    , 4000 , 100  , 0    , 256  , 100  , 50  , 8    , 0    , 0    , 0    , 256  , 200  , 0    , 0    , 100  , 100  , 256  , 100  , 16   , 256  , 1080  , 550  , 141  , 300  , 620  , 390  , 384  , 354  , 128  , 128  , 0    , 0    , 64   , 256  , 256  , 180  , 64   , 256  , 256  , 180  }},
                {   4 * 1024, {200  , 32   , 0    , 0    , 0    , 4000 , 100  , 0    , 256  , 100  , 50  , 8    , 0    , 0    , 0    , 256  , 200  , 0    , 0    , 100  , 100  , 256  , 100  , 16   , 248  , 870  , 450  , 141  , 256  , 500  , 380  , 384  , 354  , 128  , 128  , 0    , 0    , 64   , 256  , 256  , 200  , 64   , 256  , 256  , 180  }},
                {   8 * 1024, {190  , 32   , 0    , 0    , 0    , 4000 , 100  , 0    , 256  , 100  , 50  , 8    , 0    , 0    , 0    , 356  , 300  , 0    , 0    , 100  , 100  , 256  , 100  , 16   , 200  , 860  , 450  , 187  , 200  , 500  , 450  , 384  , 334  , 128  , 128  , 0    , 0    , 64   , 256  , 256  , 200   , 64   , 256  , 256  , 180  }},
                {  16 * 1024, {180  , 32   , 0    , 0    , 0    , 4000 , 100  , 0    , 256  , 100  , 50  , 8    , 0    , 0    , 0    , 456  , 400  , 0    , 0    , 100  , 100  , 256  , 100  , 16   , 200  , 900  , 450  , 192  , 200  , 600  , 461  , 384  , 326  , 128  , 128  , 0    , 0    , 64   , 256  , 256  , 200   , 64   , 256  , 256  , 150  }},
                {  32 * 1024, {180  , 32   , 200  , 0    , 0    , 4000 , 100  , 0    , 256  , 100  , 50  , 8    , 0    , 0    , 0    , 656  , 600  , 0    , 0    , 100  , 100  , 256  , 100  , 16   , 200   , 900  , 550  , 195  , 188  , 600  , 600  , 380  , 316  , 128  , 128  , 0    , 0    , 64   , 256  , 256  , 200   , 64   , 256  , 256  , 150  }},
                {  64 * 1024, {180  , 180  , 200  , 0    , 0    , 4000 , 100  , 0    , 256  , 100  , 50  , 8    , 0    , 0    , 0    , 650  , 600  , 0    , 0    , 100  , 100  , 256  , 100  , 16   , 200   , 900  , 450  , 200  , 300  , 600  , 800  , 380  , 320  , 142,    64   , 0    , 0    , 64   , 256  , 256  , 200   , 64   , 256  , 256  , 200  }},
                { 128 * 1024, {180  , 180  , 200  , 0    , 0    , 4000 , 100  , 0    , 256  , 100  , 50  , 8    , 0    , 0    , 0    , 650  , 600  , 0    , 0    , 100  , 100  , 256  , 100  , 16   , 180   , 1100  , 600  , 200  , 200  , 600  , 800  , 300  , 256  , 142   , 64   , 0    , 0    , 64   , 256  , 256  , 180   , 64   , 256  , 256  , 200  }},
                { 256 * 1024, {150  , 180  , 200  , 0    , 0    , 4000 , 100  , 0    , 256  , 100  , 50  , 8    , 0    , 0    , 0    , 300  , 200  , 0    , 0    , 100  , 100  , 256  , 100  , 16   , 150   , 1000 , 600  , 200  , 100  , 900  , 1000  , 300  , 256  , 142   , 64   , 0    , 0    , 64   , 256  , 256  , 158   , 64   , 256  , 256  , 200  }},
                { 512 * 1024, {120  , 180  , 200  , 0    , 0    , 4000 , 100  , 0    , 256  , 100  , 50  , 8    , 0    , 0    , 0    , 300  , 200  , 0    , 0    , 100  , 100  , 256  , 100  , 16   , 120   , 500  ,400  , 256  , 100  , 500  , 800  , 256  , 256  , 142   , 64   , 0    , 0    , 64   , 256  , 256  , 128   , 64   , 256  , 256  , 200  }},
            
			},
        },
        /*
        *
        * lshWeightLutTab
        *
        */
        {
            10    ,  /// validSize
            {
                {
                    1024  ,  /// region
                    {
                        {
                            63  , 38  , 23  , 14  , 9   , 5   , 3   , 2   , 1   , 1   , 0   , 0   , 0   , 0   , 0   , 0   ,
                            0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,
                        },
                    },
                },
                {
                    2048  ,  /// region
                    {
                        {
                            63  , 38  , 23  , 14  , 9   , 5   , 3   , 2   , 1   , 1   , 0   , 0   , 0   , 0   , 0   , 0   ,
                            0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,
                        },
                    },
                },
                {
                    4096  ,  /// region
                    {
                        {
                            63  , 38  , 23  , 14  , 9   , 5   , 3   , 2   , 1   , 1   , 0   , 0   , 0   , 0   , 0   , 0   ,
                            0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,
                        },
                    },
                },
                {
                    8192  ,  /// region
                    {
                        {
                            63  , 38  , 23  , 14  , 9   , 5   , 3   , 2   , 1   , 1   , 0   , 0   , 0   , 0   , 0   , 0   ,
                            0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,
                        },
                    },
                },
                {
                    16384 ,  /// region
                    {
                        {
                            63  , 38  , 23  , 14  , 9   , 5   , 3   , 2   , 1   , 1   , 0   , 0   , 0   , 0   , 0   , 0   ,
                            0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,
                        },
                    },
                },
                {
                    32768 ,  /// region
                    {
                        {
                            63  , 62  , 61  , 59  , 58  , 57  , 56  , 55  , 54  , 53  , 52  , 51  , 50  , 49  , 48  , 47  ,
                            46  , 45  , 44  , 43  , 42  , 41  , 41  , 40  , 39  , 38  , 37  , 37  , 36  , 35  , 35  , 34  ,
                        },
                    },
                },
                {
                    65536 ,  /// region
                    {
                        {
                            63  , 62  , 61  , 59  , 58  , 57  , 56  , 55  , 54  , 53  , 52  , 51  , 50  , 49  , 48  , 47  ,
                            46  , 45  , 44  , 43  , 42  , 41  , 41  , 40  , 39  , 38  , 37  , 37  , 36  , 35  , 35  , 34  ,
                        },
                    },
                },
                {
                    131072,  /// region
                    {
                        {
                            63  , 62  , 61  , 59  , 58  , 57  , 56  , 55  , 54  , 53  , 52  , 51  , 50  , 49  , 48  , 47  ,
                            46  , 45  , 44  , 43  , 42  , 41  , 41  , 40  , 39  , 38  , 37  , 37  , 36  , 35  , 35  , 34  ,
                        },
                    },
                },
                {
                    262144,  /// region
                    {
                        {
                            63  , 62  , 61  , 59  , 58  , 57  , 56  , 55  , 54  , 53  , 52  , 51  , 50  , 49  , 48  , 47  ,
                            46  , 45  , 44  , 43  , 42  , 41  , 41  , 40  , 39  , 38  , 37  , 37  , 36  , 35  , 35  , 34  ,
                        },
                    },
                },
                {
                    524288,  /// region
                    {
                        {
                            63  , 62  , 61  , 59  , 58  , 57  , 56  , 55  , 54  , 53  , 52  , 51  , 50  , 49  , 48  , 47  ,
                            46  , 45  , 44  , 43  , 42  , 41  , 41  , 40  , 39  , 38  , 37  , 37  , 36  , 35  , 35  , 34  ,
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
            10    ,  /// validSize
            {
                /// [0]lux
                /// [1]lshDetailType
                /// [2]lshDetailFreqMid
                /// [3]lshDetailFreqEnd
                /// [4]lshDetailLumMid
                /// [5]lshDetailLumEnd
                /// [6]lshDltiPosMid
                /// [7]lshDltiPosEnd
                /// [8]lshDltiNegMid
                /// [9]lshDltiNegEnd
                /// 0     1      2      3      4      5      6      7      8      9
                {50    , {0    , 6    , 8    , 8    , 9    , 7    , 6    , 7    , 8    }},
                {100   , {0    , 6    , 8    , 8    , 9    , 7    , 6    , 7    , 8    }},
                {150   , {0    , 6    , 8    , 8    , 8    , 7    , 6    , 7    , 8    }},
                {200   , {0    , 6    , 8    , 8    , 8    , 7    , 6    , 7    , 8    }},
                {250   , {0    , 7    , 8    , 8    , 8    , 7    , 6    , 7    , 8    }},
                {300   , {0    , 8    , 8    , 8    , 8    , 7    , 6    , 7    , 8    }},
                {350   , {0    , 8    , 8    , 8    , 8    , 7    , 6    , 7    , 8    }},
                {400   , {0    , 8    , 8    , 8    , 8    , 7    , 6    , 7    , 8    }},
                {450   , {0    , 8    , 8    , 8    , 8    , 7    , 6    , 7    , 8    }},
                {500   , {0    , 8    , 8    , 8    , 8    , 7    , 6    , 7    , 8    }},
            },
        },
        /*
        *
        * lshLuxInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]lux
                /// [1]lshDetailFreqOffset
                /// [2]lshDetailFreqY1
                /// [3]lshDetailFreqY2
                /// [4]lshDetailFreqY3
                /// [5]lshDetailLumOffset
                /// [6]lshDetailLumY1
                /// [7]lshDetailLumY2
                /// [8]lshDetailLumY3
                /// [9]lshDltiPosOffset
                /// [10]lshDltiFinalPosY1
                /// [11]lshDltiFinalPosY2
                /// [12]lshDltiFinalPosY3
                /// [13]lshDltiNegOffset
                /// [14]lshDltiFinalNegY1
                /// [15]lshDltiFinalNegY2
                /// [16]lshDltiFinalNegY3
                /// 0     1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16
                {50    , {16   , 356  , 1000  , 656  , 128  , 356  , 756  , 456  , 64   , 256  , 200  , 120  , 64   , 256  , 200  , 180  }},
                {100   , {16   , 300  , 1000  , 656  , 128  , 356  , 756  , 500  , 64   , 256  , 200  , 120  , 64   , 256  , 236  , 180  }},
                {150   , {16   , 300  , 1000  , 726  , 128  , 356  , 756  , 500  , 64   , 256  , 256  , 150  , 64   , 256  , 256  , 180  }},
                {200   , {16   , 300  , 1000  , 800  , 128  , 356  , 756  , 510  , 64   , 256  , 256  , 150   , 64   , 256  , 256  , 180  }},
                {250   , {16   , 300  , 1000  , 800  , 128  , 356  , 620  , 400  , 64   , 256  , 256  , 150   , 64   , 256  , 256  , 180  }},
                {300   , {16   , 256  , 950  , 756  , 128  , 256  , 556  , 386  , 64   , 256  , 256  , 150   , 64   , 256  , 256  , 200  }},
                {350   , {16   , 256  , 900  , 656  , 128  , 256  , 556  , 386  , 64   , 256  , 256  , 150   , 64   , 256  , 256  , 236  }},
                {400   , {16   , 256  , 900  , 656  , 128  , 256  , 556  , 386  , 64   , 256  , 256  , 150   , 64   , 256  , 256  , 236  }},
                {450   , {16   , 256  , 900  , 656  , 128  , 256  , 556  , 386  , 64   , 256  , 256  , 150   , 64   , 256  , 256  , 236  }},
                {500   , {16   , 60   , 900  , 50   , 128  , 256  , 556  , 384  , 64   , 256  , 256  , 150   , 64   , 256  , 256  , 236  }},

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
        /*
        *
        * cnrGainSwitchTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
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
                /// 0          1      2      3      4      5      6      7      8      9      10
                {   1 * 1024, {10   , 10   , 5    , 5    , 5    , 5    , 9    , 9    , 5    , 5    }},
                {   2 * 1024, {10   , 10   , 5    , 5    , 5    , 5    , 9    , 9    , 5    , 5    }},
                {   4 * 1024, {10   , 10   , 5    , 5    , 5    , 5    , 9    , 9    , 5    , 5    }},
                {   8 * 1024, {10   , 10   , 5    , 5    , 5    , 5    , 9    , 9    , 5    , 5    }},
                {  16 * 1024, {10   , 10   , 5    , 5    , 5    , 5    , 9    , 9    , 5    , 5    }},
                {  32 * 1024, {10   , 10   , 5    , 5    , 5    , 5    , 9    , 9    , 5    , 5    }},
                {  64 * 1024, {10   , 10   , 5    , 5    , 5    , 5    , 9    , 9    , 5    , 5    }},
                { 128 * 1024, {10   , 10   , 5    , 5    , 5    , 5    , 9    , 9    , 5    , 5    }},
                { 256 * 1024, {10   , 10   , 5    , 5    , 5    , 5    , 9    , 9    , 5    , 5    }},
                { 512 * 1024, {10   , 10   , 5    , 5    , 5    , 5    , 9    , 9    , 5    , 5    }},
            },
        },
        /*
        *
        * cnrGainInterpTab
        *
        */
        {
            10    ,  /// validSize
            {
                /// [0]gain
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
                /// 0          1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     16     17     18     19     20     21     22     23     24
                {   1 * 1024, {256  , 0    , 10   , 10   , 10   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 0    , 0    , 256  , 2    , 512  , 256  , 230  , 156  , 32   , 200  , 100  , 0    }},
                {   2 * 1024, {256  , 0    , 10   , 10   , 10   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 0    , 0    , 256  , 2    , 512  , 256  , 230  , 156  , 32   , 200  , 100  , 0    }},
                {   4 * 1024, {256  , 0    , 10   , 10   , 10   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 0    , 0    , 256  , 2    , 512  , 256  , 230  , 156  , 32   , 200  , 100  , 0    }},
                {   8 * 1024, {256  , 0    , 10   , 10   , 10   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 0    , 0    , 256  , 2    , 512  , 256  , 230  , 156  , 32   , 200  , 100  , 0    }},
                {  16 * 1024, {300  , 0    , 10   , 10   , 10   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 0    , 0    , 256  , 2    , 512  , 256  , 230  , 156  , 32   , 200  , 100  , 0    }},
                {  32 * 1024, {350  , 0    , 10   , 10   , 10   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 0    , 20   , 256  , 2    , 512  , 256  , 230  , 156  , 32   , 200  , 100  , 0    }},
                {  64 * 1024, {400  , 0    , 10   , 10   , 10   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 0    , 25   , 256  , 2    , 512  , 256  , 230  , 156  , 32   , 200  , 100  , 0    }},
                { 128 * 1024, {450  , 0    , 20   , 20   , 20   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 0    , 25   , 256  , 2    , 512  , 256  , 230  , 156  , 32   , 256  , 256  , 256  }},
                { 256 * 1024, {500  , 0    , 50   , 30   , 30   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 0    , 25   , 128  , 2    , 512  , 256  , 230  , 156  , 32   , 256  , 256  , 256  }},
                { 512 * 1024, {600  , 0    , 80  , 50   , 30   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 100    , 25   , 128  , 2    , 512  , 256  , 246  , 200  , 32   , 256  , 256  , 256  }},
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
                /// [3]cmbThresh1Lm
                /// [4]cmbThresh2Lm
                /// 0          1      2      3      4
                {   1 * 1024, {32   , 32   , 3072 , 3840 }},
                {   2 * 1024, {32   , 32   , 3072 , 3840 }},
                {   4 * 1024, {32   , 32   , 3072 , 3840 }},
                {   8 * 1024, {32   , 32   , 2048 , 3840 }},
                {  16 * 1024, {32   , 32   , 1024 , 3840 }},
                {  32 * 1024, {32   , 32   , 1024 , 3840 }},
                {  64 * 1024, {64   , 64   , 1024 , 3840 }},
                { 128 * 1024, {128  , 128  , 512  , 3840 }},
                { 256 * 1024, {128  , 128  , 512  , 3840 }},
                { 512 * 1024, {128  , 128  , 512  , 3840 }},
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
                {50    , {1150 }},
                {100   , {1150 }},
                {150   , {1150 }},
                {200   , {1150 }},
                {250   , {1150 }},
                {300   , {1150 }},
                {350   , {1150 }},
                {400   , {1100 }},
                {450   , {1100 }},
                {500   , {1024 }},
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
    * rir
    *
    */
    {
        0     ,  /// rirTuneEnable
        0     ,  /// rirDirMeanEn
        0     ,  /// rirGradFilterEn
        0     ,  /// rirInArray
        0     ,  /// rirGradMeanThd1
        0     ,  /// rirGradMeanThd2
        0     ,  /// rirGradVarThd1
        0     ,  /// rirGradVarThd2
        0     ,  /// rirGradSimVar
        0     ,  /// rirOverExpEn
        0     ,  /// rirOverExpThd1
        0     ,  /// rirOverExpThd2
        0     ,  /// rirOverExpRGain
        0     ,  /// rirOverExpBGain
        0     ,  /// rirOverExpDesatStr
        0     ,  /// rirOverExpDifHueMin
        0     ,  /// rirBlcInEn
        0     ,  /// rirBlcOutEn
        0     ,  /// rirDebugMode
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
#endif
