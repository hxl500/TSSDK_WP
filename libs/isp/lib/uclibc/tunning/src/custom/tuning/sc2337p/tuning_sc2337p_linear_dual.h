#ifndef __TUNING_SC2337P_NORMAL_H__
#define __TUNING_SC2337P_NORMAL_H__

//#define PTZ
//#define DT57
/*
*
* tuneVersion
*
*/
{
    0, 0, 0, 61210, /// major, minor, debug, check
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
        0, 1, 0, 12212, /// major, minor, debug, check
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
                    1   , 1   , 1   , 1   , 2   , 2   , 2   , 2   , 2   , 2   , 2   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 2   , 2   , 2   , 2   , 2   , 2   , 2   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 2   , 2   , 4   , 4   , 4   , 4   , 4   , 2   , 2   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 2   , 2   , 4   , 4   , 4   , 4   , 4   , 2   , 2   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 2   , 2   , 4   , 4   , 4   , 4   , 4   , 2   , 2   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 2   , 2   , 2   , 2   , 2   , 2   , 2   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 2   , 2   , 2   , 2   , 2   , 2   , 2   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 2   , 2   , 2   , 2   , 2   , 2   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                },
				{
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 2   , 2   , 2   , 2   , 2   , 2   , 2   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 2   , 2   , 2   , 2   , 2   , 2   , 2   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 2   , 2   , 4   , 4   , 4   , 4   , 4   , 2   , 2   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 2   , 2   , 4   , 4   , 4   , 4   , 4   , 2   , 2   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 2   , 2   , 4   , 4   , 4   , 4   , 4   , 2   , 2   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 2   , 2   , 2   , 2   , 2   , 2   , 2   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 2   , 2   , 2   , 2   , 2   , 2   , 2   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                    1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
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
            30    ,  /// aeMaxConvSpeed default 60,[30 : 200] 100 base percent
            20    ,  /// aeStableInRange default 4,[2:255] AE convergence stability range in
            106    ,  /// aeStableOutRange default 32,[5:512] AE convergence stability range out
            960   ,  /// aeSaturationThr default 960,[0:1023] AE Saturation Threshold, use to calculation linaer Y
            2     ,  /// aeStatsYIdx default 3,[0,3] which stats Y use to judge ae stable out
            5     ,  /// aeChgFrameCnt default 15,[0, 4096] AE change time out frames
        },
        /*
        *
        * aecLuxConvTabSBase
        *
        */
        {
            11    ,  /// validSize
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
                {60    , {225  , 90   , 20000, 85000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
				{90    , {200  , 90   , 20000, 85000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {100   , {170  , 85   , 18000, 85000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {170   , {170  , 80   , 16000, 80000, 15   , 50   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {200   , {160  , 80   , 15000, 52000, 15   , 40   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {250   , {155  , 80   , 15000, 53500, 15   , 40   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {300   , {160  , 80   , 10000, 55000, 15   , 40   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {350   , {160  , 100   , 10000, 57000, 15   , 40   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {400   , {160  , 100  , 20000, 57000, 0    , 35   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {440   , {100   , 100   , 20000, 60000, 0    , 35   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {500   , {90   , 100   , 20000, 60000, 0    , 35   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},

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
                {50    , {235  , 60   , 20000, 60000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {100   , {230  , 60   , 18000, 60000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {150   , {210  , 50   , 16000, 51000, 15   , 50   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {200   , {175  , 40   , 15000, 52000, 15   , 40   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {250   , {135  , 30   , 15000, 53500, 15   , 40   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {300   , {135  , 30   , 10000, 55000, 15   , 30   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {350   , {135  , 30   , 10000, 57000, 15   , 40   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {400   , {135  , 30   , 20000, 50000, 15   , 40   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {450   , {130  , 40   , 20000, 50000, 30   , 30   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {500   , {100  , 40   , 20000, 50000, 30   , 30   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
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
                {50    , {235  , 60   , 20000, 60000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {100   , {230  , 60   , 18000, 60000, 15   , 60   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {150   , {210  , 50   , 16000, 51000, 15   , 50   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {200   , {175  , 40   , 15000, 52000, 15   , 40   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {250   , {135  , 30   , 15000, 53500, 15   , 40   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {300   , {135  , 30   , 10000, 55000, 15   , 30   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {350   , {135  , 30   , 10000, 57000, 15   , 40   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {400   , {135  , 30   , 20000, 50000, 15   , 40   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {450   , {130  , 40   , 20000, 50000, 30   , 30   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
                {500   , {100  , 40   , 20000, 50000, 30   , 30   , 40   , 16   , 40   , 16   , 40   , 16   , 10240}},
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
                {0     , {100000 , 1024 , 0    }},
                {1     , {20   , 1024 , 1    }},
                {2     , {20   , 262144, 0    }},
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
        257   ,  /// awbPrefGainB
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
                {0     , {0    , 0    , 0    , 1000 , 20   , 256  , 256  , 256  , 256  }},
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
            6     ,  /// validSize
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
                {0     , {0    , 4    , 360  , 440  , 10   , 120   , 110  , 118  , 128  , 250  , 260  }},
				{1     , {0    , 5    , 360  , 440  , 10   , 120   , 110  , 118  , 128  , 250  , 260  }},
				{2     , {0    , 3    , 360  , 440  , 10   , 120   , 110  , 118  , 128  , 250  , 260  }},
                {3     , {0    , 3    , 45  , 110  , 10   , 120   , 128  , 128  , 128  , 250  , 260 }},
				{4     , {0    , 4    , 45  , 110  , 10   , 120   , 110  , 128  , 128  , 250  , 260  }},
				{5     , {0    , 5    , 45  , 110  , 10   , 120   , 110  , 128  , 128  , 250  , 260  }},
				
            },
        },
        /*
        *
        * awbLightsourceProb
        *
        */
        {
            8     ,  /// validSize
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
                {40    , {128   , 168   , 256  , 206  , 188  , 128  , 128  }},
                {100   , {128   , 256   , 256  , 256  , 188  , 128  , 128 }},
                {260   , {128   , 256   , 206  , 206  , 256  , 188  , 128  }},
                {360   , {150   , 168  , 198  , 128 , 256  , 188  , 188 }},
				{390   , {200   , 200  , 218  , 128  , 165  , 188 , 188  }},
				{400   , {128   , 128  , 218  , 128  , 128  , 150  , 128  }},
				{490   , {128   , 128  , 128  , 128  , 128  , 150  , 128  }},
				{500   , {128   , 128  , 128  , 128  , 256  , 206  , 206  }},
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
                    {0     , {0    , 2  , 140 , 10   , 256  , 128  , -15 , 30  , -360 , -400 , 7    , 150  , 240  , 280  }},
                    {1     , {0    , 2  , 140 , 10   , 256  , 128  , 25  , 60  , -375 , -420 , 7    , 128  , 210  , 266  }},
                    {2     , {0    , 120  , 200  , 20   , 250  , 50   , 97   , 149  , -319 , -352 , 3    , 256  , 256  , 256  }},
            },
        },
        /*
        *
        * awbSpecialGain
        *
        */
        {
            4    ,  /// validSize
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
                {100   , {250  , 820  , 270  , 818  , 341  , 479  , 369  , 467  , 474  , 466  , 420  , 514  , 521  , 455  }},
                {150   , {250  , 820  , 270  , 818  , 342  , 479  , 369  , 467  , 474  , 466  , 420  , 534  , 521  , 455  }},
                {250   , {250  , 820  , 270  , 818  , 340  , 485  , 360  , 475  , 470  , 466  , 420  , 535  , 511  , 455   }},
				{350   , {250  , 820  , 270  , 818  , 340  , 485  , 360  , 475  , 460  , 476  , 420  , 535  , 511  , 465   }},

                },
            },
            /*
            *
            * awbSpecialGainWeight
            *
            */
            {
           8     ,  /// validSize
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
                {90    , {32    , 32   , 98    ,98     , 98     , 32    , 6    }},
                {100   , {26    , 26   , 26    ,64     , 32     , 32    , 6    }},
				{170   , {26    , 26   , 26    ,64     , 32     , 32    , 6    }},
                {250   , {32    , 32   , 32    ,8     , 6     , 6    , 6   }},
				{270   , {1    , 1    , 4    , 3    , 1    , 8    , 1   }},
				{460   , {1    , 1    , 4    , 3    , 1    , 8    , 1   }},
                {480   , {1    , 1    , 4    , 3    , 1    , 8    , 1   }},
				{530   , {64    , 64   , 96   ,96    , 96     , 96    , 96  }},
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
                            32  , 48  , 64  , 128 , 192 , 224 , 256 , 256 , 256 , 256 , 256 , 224 , 166 , 48 , 48  , 32  ,
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
                {50    , {10   , 20   , 128  , 380  , 256  , 470  , 128  , 128  }},
                {100   , {10   , 20   , 128  , 380  , 256  , 470  , 128  , 128  }},
                {150   , {10   , 20   , 128  , 380  , 256  , 470  , 128  , 128  }},
                {200   , {10   , 20   , 128  , 380  , 256  , 470  , 128  , 128  }},
                {250   , {10   , 20   , 128  , 380  , 256  , 480  , 128  , 128  }},
                {300   , {10   , 20   , 128  , 380  , 256  , 480  , 128  , 128  }},
                {350   , {10   , 20   , 128  , 390  , 256  , 460  , 128  , 128  }},
                {400   , {10   , 20   , 128  , 420  , 256  , 490  , 128  , 128  }},
                {475   , {10   , 20   , 230  , 420  , 256  , 490  , 128  , 128  }},
                {500   , {10   , 20   , 256  , 420  , 256  , 490  , 128  , 128  }},
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
        4     ,  /// afBackjumpCom tep
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
        1280  ,  /// maxBChannelGain
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
            {1024  , {67007,  67003,  66971,  66978, 0    , 0    , 0    , 0    , 61381, 61382, 61378, 61379, 61432, 61433, 61431, 61433, 61432, 61433, 61431, 61433}},
            {2048  , {67023,  67023,  66960,  66961, 0    , 0    , 0    , 0    , 61111, 61113, 61111, 61114, 61486, 61487, 61481, 61482, 61486, 61487, 61481, 61482}},
            {4096  , {66901,  66892,  66812,  66823, 0    , 0    , 0    , 0    , 61152, 61153, 61143, 61147, 61437, 61352, 61351, 61356, 61347, 61352, 61351, 61356}},
            {8192  , {66753,  66773,  66734,  66713, 0    , 0    , 0    , 0    , 60922, 60924, 60911, 60917, 61306, 61312, 61318, 61325, 61306, 61312, 61318, 61325}},
            {16384 , {66369,  66289,  66127,  66193, 0    , 0    , 0    , 0    , 60428, 60442, 60403, 60417, 61245, 61248, 61238, 61248, 61245, 61248, 61238, 61248}},
            {41568 , {66940,  66710,  66200,  66329, 0    , 0    , 0    , 0    , 58874, 58894, 58902, 58926, 61960, 61973, 61938, 61978, 61960, 61973, 61938, 61978}},
            {65536 , {65242,  64596,  64057,  64533, 0    , 0    , 0    , 0    , 58874, 58894, 58902, 58926, 61960, 61973, 61938, 61978, 61960, 61973, 61938, 61978}},
            {131072, {63554,  63078,  61991,  61925, 0    , 0    , 0    , 0    , 58874, 58894, 58902, 58926, 61960, 61973, 61938, 61978, 61960, 61973, 61938, 61978}},
            {262144, {63554,  63078,  61991,  61925, 0    , 0    , 0    , 0    , 58874, 58894, 58902, 58926, 61960, 61973, 61938, 61978, 61960, 61973, 61938, 61978}},
            {524288, {63554,  63078,  61991,  61925, 0    , 0    , 0    , 0    , 58874, 58894, 58902, 58926, 61960, 61973, 61938, 61978, 61960, 61973, 61938, 61978}},
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
                {   1 * 1024, {256  }},
                {   2 * 1024, {256  }},
                {   4 * 1024, {128  }},
                {   8 * 1024, {0  }},
                {  16 * 1024, {0  }},
                {  32 * 1024, {0  }},
                {  64 * 1024, {0  }},
                { 128 * 1024, {0  }},
                { 256 * 1024, {0  }},
                { 512 * 1024, {0  }},
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
            12    ,  /// validSize
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
                {50    , {200  , 1200 , 50  , 990  , 1    , 85   , 200   , 200  , 0    , 110  , 99   , 200    , 30   , 50   , 400  , 800 , 0    , 0    }},
		        {80    , {130  , 1000 , 50  , 990  , 1    , 85   , 200   , 200  , 0    , 110  , 99   , 200    , 30   , 50   , 400  , 800 , 0    , 0    }},
                {100   , {130  , 1000 , 50  , 990  , 1    , 85   , 200   , 200  , 0    , 110  , 99   , 200    , 30   , 50   , 400  , 800 , 0    , 0    }},
                {150   , {150  , 1000 , 50  , 990  , 1    , 85   , 200  , 200  , 0    , 110  , 99   , 200    , 30   , 50   , 400  , 800 , 0    , 0    }},
                {200   , {150  , 1000 , 50  , 990  , 1    , 85   , 120  , 120  , 0    , 110  , 99   , 200    , 30   , 50   , 400  , 800 , 0    , 0    }},
                {250   , {150  , 1000 , 50  , 990  , 1    , 60   , 120  , 120  , 0    , 110  , 99   , 100   , 30   , 50   , 400   , 800 , 0    , 0    }},
                {300   , {140  , 1000 ,50  , 990  , 1    , 60   , 100   , 100  , 0    , 110  , 99   , 50    , 30   , 50   , 400  , 500 , 0    , 0    }},
                {350   , {120  , 1000  , 50  , 990  , 1    , 60   , 50  , 120  , 0    , 100   , 99   , 50    , 30   , 50   , 400  , 500 , 0    , 0    }},
                {400   , {30  , 600  , 10  , 980  , 1    , 60   , 50  , 120  , 1    , 80   , 99   , 30    , 30   , 50   , 400  , 500 , 0    , 0    }},
                {450   , {10  , 0    , 10  , 980  , 1    , 60  , 50 , 120  , 1    , 70   , 99   , 30    , 30   , 50   , 400  , 0  , 0    , 0    }},
                {500   , {10  , 0    , 10  , 970  , 1    , 60  , 50 , 120  , 1    , 50   , 99   , 30    , 30   , 50   , 400  , 0    , 0    , 0    }},
			    {550   , {10  , 0    , 10  , 970  , 1    , 60  , 50 , 120  , 1    , 40   , 99   , 30    , 30   , 50   , 400  , 0    , 0    , 0    }},
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
                {50    , {200  , 1024 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 160  , 99   , 5    , 30   , 50   , 100  , 2048 , 0    , 0    }},
                {100   , {150  , 1024 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 150  , 99   , 5    , 30   , 50   , 100  , 1248 , 0    , 0    }},
                {150   , {100  , 1024 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 140  , 99   , 5    , 30   , 50   , 100  , 1000 , 0    , 0    }},
                {200   , {100  , 1024 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 130  , 99   , 5    , 30   , 50   , 100  , 1000 , 0    , 0    }},
                {250   , {100  , 1024 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 120  , 99   , 5    , 30   , 50   , 100  , 1000 , 0    , 0    }},
                {300   , {90  , 1024 , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 100  , 99   , 5    , 30   , 50   , 100  , 900 , 0    , 0    }},
                {350   , {70  , 875  , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 90   , 99   , 5    , 30   , 50   , 100  , 975  , 0    , 0    }},
                {400   , {30  , 5    , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 80   , 99   , 5    , 30   , 50   , 100  , 10    , 0    , 0    }},
                {450   , {2  , 5    , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 70   , 99   , 5    , 30   , 50   , 100  , 10    , 0    , 0    }},
                {500   , {2  , 5    , 100  , 990  , 1    , 85   , 50   , 120  , 0    , 70   , 99   , 5    , 30   , 50   , 100  , 5    , 0    , 0    }},
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
                             0   , 1024, 2048, 3072, 4096, 5120, 6144, 7168, 8192, 9216, 10240, 11264, 12288, 13312, 14336, 15360,
                            16384, 17408, 18432, 19456, 20480, 21504, 22528, 23552, 24576, 25600, 26624, 27648, 28672,29696,30720,31744,
                            32768,33792,34816,35840,36864,37888,38912,39936,40960,41984,43008,44032,45056,46080,47104,48128,
                            49152,50176,51200,52224,53248,54272,55296,56320,57344,58368,59392,60416,61440,62464,63488,64512,
                            65535,
                        },
                    },
                },
                {
                    50    ,  /// region
                    {
                        {
                             0   , 1024, 2048, 3072, 4096, 5120, 6144, 7168, 8192, 9216, 10240, 11264, 12288, 13312, 14336, 15360,
                            16384, 17408, 18432, 19456, 20480, 21504, 22528, 23552, 24576, 25600, 26624, 27648, 28672,29696,30720,31744,
                            32768,33792,34816,35840,36864,37888,38912,39936,40960,41984,43008,44032,45056,46080,47104,48128,
                            49152,50176,51200,52224,53248,54272,55296,56320,57344,58368,59392,60416,61440,62464,63488,64512,
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
            11   ,  /// validSize
            {
                /// [0]gain
                /// [1]ccmSaturation
                /// 0          1
                {   1 * 1024, {350  }},//285
                {   2 * 1024, {345 }},
                {   4 * 1024, {325  }},
                {   8 * 1024, {315  }},
                {  16 * 1024, {300	}},
                {  32 * 1024, {295  }},
                {  42 * 1024, {275  }},
                {  64 * 1024, {265	}},
				{  96 * 1024, {265	}},
                { 128 * 1024, {235  }},
                { 256 * 1024, {155  }},
                { 512 * 1024, {140  }},
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
                {50    , {345  }},
                {100   , {345  }},
                {150   , {340  }},
                {200   , {345  }},
                {250   , {345  }},
                {300   , {345  }},
                {350   , {345  }},
                {400   , {326  }},
                {450   , {325  }},
                {500   , {265  }},
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
        100    ,  /// ggcConvergeSpeed
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
            13    ,  /// validSize
            {
                /// [0]lux
                /// [1]ggcContrastAutoStr
                /// [2]ggcLocalLumAdjNode0
                /// [3]ggcLocalLumAdjNode1
				/// [4]ggcLocalLumAdjNode2
				/// [5]ggcLocalLumAdjNode3
				/// [6]ggcLocalLumAdjNode4
                /// [7]ggcLocalLumAdjNode5
                /// [8]ggcLocalLumAdjNode6
				/// [9]ggcGlobalLuminationAdj
				//lux    Str    Node0   de1    de2   de3   de4   de5  de6   Adj
                /// 0     1      2      3      4     5     6     7    8     9   
				
                {50    , {100  , 54   , 55   , 50  , 50  , 50  , 47   , 45  , 132 }},
                {100   , {100  , 54   , 55   , 50  , 50  , 50  , 47   , 45 , 130 }},
                {170   , {100  , 51   , 55    , 50  , 50  , 50  , 48   , 45 , 130 }},
                {220   , {120  , 53   , 55    , 50  , 50  , 50  , 48   , 45 , 132 }},
                {250   , {210  , 55  , 55    , 50  , 50  , 50  , 46   , 45 , 131 }},
                {300   , {210  , 55   , 53    , 50  , 50  , 50  , 46   , 46 , 131 }},
                {350   , {210  , 55   , 53    , 50  , 50  , 50  , 46   , 46 , 131 }},
                {388   , {230  , 55   , 53    , 50  , 50  , 50  , 48   , 48 , 131 }},
				{400   , {230  , 55   , 52    , 50  , 50  , 50  , 48   , 48 , 131 }},
				{405   , {230  , 50   , 52    , 50  , 50  , 50  , 45   , 45 , 130 }},
                {450   , {230  , 50   , 52    , 50  , 50  , 50  , 47   , 45 , 130 }},
                {500   , {180  , 50   , 54   , 50  , 50  , 50  , 47   , 45 , 130 }},
				{510   , {180  , 50   , 54   , 50  , 50  , 50  , 47   , 45 , 130 }},
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
                {   8 * 1024, {0    , 1    , 0    , 10   , 300  , 16   , 100  , 0    , 256  , 256  , 64   , 0    }},
                {  16 * 1024, {0    , 2    , 0    , 10   , 300  , 16   , 100  , 0    , 256  , 256  , 64   , 0    }},
                {  32 * 1024, {0    , 3    , 0    , 10   , 400  , 16   , 100  , 0    , 256  , 256  , 64   , 0    }},
                {  64 * 1024, {0    , 4    , 0    , 10   , 400  , 16   , 100  , 0    , 256  , 256  , 64   , 0    }},
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
                {   1 * 1024, {32  , 32  }},
                {   2 * 1024, {32  , 32  }},
                {   4 * 1024, {50  , 50  }},
                {   8 * 1024, {50  , 50  }},
                {  16 * 1024, {64  , 64  }},
                {  32 * 1024, {125  , 75  }},
                {  64 * 1024, {200  , 95  }},
                { 128 * 1024, {200  , 100  }},
                { 256 * 1024, {200  , 120  }},
                { 512 * 1024, {200  , 120  }},
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
                            /*63  , 57  , 52  , 47  , 42  , 38  , 35  , 31  , 28  , 26  , 23  , 21  , 19  , 17  , 16  , 14  ,
                            13  , 12  , 10  , 9   , 9   , 8   , 7   , 6   , 6   , 5   , 5   , 4   , 4   , 3   , 3   , 3   ,
                            3   , 2   , 2   , 2   , 2   , 2   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                            1   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,
                            0   ,*/
                            63  , 57  , 52  , 47  , 42  , 38  , 35  , 31  , 28  , 26  , 23  , 21  , 19  , 17  , 16  , 14  ,
                            15  , 14  , 12  , 10  , 10  , 9  , 8  , 7  , 7  , 6   , 6   , 5   , 5   , 5   , 4   , 4   ,
                            4   , 4   , 3   , 3  , 3   , 3   , 3   , 2   , 2   , 2   , 2   , 2   , 1   , 1   , 1   , 1   ,
                            1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 0   , 0   , 0   , 0   ,
                            0   ,
                        },
                    },
                },
                {
                    2048  ,  /// region
                    {
                        {
                          /*  63  , 57  , 52  , 47  , 42  , 38  , 35  , 31  , 28  , 26  , 23  , 21  , 19  , 17  , 16  , 14  ,
                            13  , 12  , 10  , 9   , 9   , 8   , 7   , 6   , 6   , 5   , 5   , 4   , 4   , 3   , 3   , 3   ,
                            3   , 2   , 2   , 2   , 2   , 2   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   ,
                            1   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,
                            0   ,*/
                            63  , 57  , 52  , 47  , 42  , 38  , 35  , 31  , 28  , 26  , 23  , 21  , 19  , 17  , 16  , 14  ,
                            15  , 14  , 12  , 10  , 10  , 9  , 8  , 7  , 7  , 6   , 6   , 5   , 5   , 5   , 4   , 4   ,
                            4   , 4   , 3   , 3  , 3   , 3   , 3   , 2   , 2   , 2   , 2   , 2   , 1   , 1   , 1   , 1   ,
                            1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 0   , 0   , 0   , 0   ,
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
						 3,   4,   5,	6,	 7,   7,   8,	8,	 9,   9,  10,  10,	11,  11,  11,  12,
						12,  12,  13,  13,	13,  14,  14,  14,	15,  15,  15,  16,	16,  16,  16,  17,
						17,  17,  17,  18,	18,  18,  18,  19,	19,  19,  19,  19,	20,  20,  20,  20,
						21,  21,  21,  21,	21,  22,  22,  22,	22,  22,  23,  23,	23,  23,  23,  23,
						24,  24,  24,  24,	24,  25,  25,  25,	25,  25,  25,  26,	26,  26,  26,  26,
						26,  27,  27,  27,	27,  27,  27,  28,	28,  28,  28,  28,	28,  28,  29,  29,
						29,  29,  29,  29,	29,  30,  30,  30,	30,  30,  30,  30,	31,  31,  31,  31,
						31,  31,  31,  32,	32,  32,  32,  32,	32,  32,  33,  33,	33,  33,  33,  33,
                
                        },
                    },
                },
                {
                    2048  ,  /// region
                    {
                        {
						 4,   6,   7,	8,	 9,  10,  11,  12,	12,  13,  14,  14,	15,  15,  16,  17,
						17,  18,  18,  19,	19,  19,  20,  20,	21,  21,  22,  22,	22,  23,  23,  23,
						24,  24,  25,  25,	25,  26,  26,  26,	27,  27,  27,  27,	28,  28,  28,  29,
						29,  29,  30,  30,	30,  30,  31,  31,	31,  32,  32,  32,	32,  33,  33,  33,
						33,  34,  34,  34,	34,  35,  35,  35,	35,  36,  36,  36,	36,  37,  37,  37,
						37,  38,  38,  38,	38,  38,  39,  39,	39,  39,  40,  40,	40,  40,  40,  41,
						41,  41,  41,  41,	42,  42,  42,  42,	42,  43,  43,  43,	43,  43,  44,  44,
						44,  44,  44,  45,	45,  45,  45,  45,	46,  46,  46,  46,	46,  46,  47,  47,

                        },
                    },
                },
                {
                    4096  ,  /// region
                    {
                        {
						 7,   9,  11,  12,	13,  14,  15,  16,	17,  18,  19,  20,	20,  21,  22,  23,
						23,  24,  25,  25,	26,  26,  27,  27,	28,  29,  29,  30,	30,  31,  31,  32,
						32,  33,  33,  33,	34,  34,  35,  35,	36,  36,  36,  37,	37,  38,  38,  39,
						39,  39,  40,  40,	40,  41,  41,  42,	42,  42,  43,  43,	43,  44,  44,  44,
						45,  45,  45,  46,	46,  46,  47,  47,	47,  48,  48,  48,	49,  49,  49,  50,
						50,  50,  50,  51,	51,  51,  52,  52,	52,  53,  53,  53,	53,  54,  54,  54,
						55,  55,  55,  55,	56,  56,  56,  56,	57,  57,  57,  57,	58,  58,  58,  59,
						59,  59,  59,  60,	60,  60,  60,  61,	61,  61,  61,  62,	62,  62,  62,  63,

                        },
                    },
                },
                {
                    8192  ,  /// region
                    {
                        {
						11,  13,  15,  17,	18,  20,  21,  22,	24,  25,  26,  27,	28,  29,  30,  31,
						32,  33,  33,  34,	35,  36,  37,  37,	38,  39,  40,  40,	41,  42,  42,  43,
						44,  44,  45,  45,	46,  47,  47,  48,	48,  49,  50,  50,	51,  51,  52,  52,
						53,  53,  54,  54,	55,  55,  56,  56,	57,  57,  58,  58,	59,  59,  60,  60,
						61,  61,  62,  62,	62,  63,  63,  64,	64,  65,  65,  66,	66,  66,  67,  67,
						68,  68,  68,  69,	69,  70,  70,  70,	71,  71,  72,  72,	72,  73,  73,  74,
						74,  74,  75,  75,	75,  76,  76,  76,	77,  77,  78,  78,	78,  79,  79,  79,
						80,  80,  80,  81,	81,  81,  82,  82,	82,  83,  83,  83,	84,  84,  84,  85,

                        },
                    },
                },
                {
                    16384 ,  /// region
                    {
                        {
						13,  16,  18,  21,	23,  24,  26,  28,	29,  31,  32,  33,	35,  36,  37,  38,
						39,  40,  41,  42,	43,  44,  45,  46,	47,  48,  49,  50,	51,  51,  52,  53,
						54,  55,  55,  56,	57,  58,  58,  59,	60,  61,  61,  62,	63,  63,  64,  65,
						65,  66,  67,  67,	68,  69,  69,  70,	70,  71,  72,  72,	73,  73,  74,  74,
						75,  76,  76,  77,	77,  78,  78,  79,	79,  80,  81,  81,	82,  82,  83,  83,
						84,  84,  85,  85,	86,  86,  87,  87,	88,  88,  89,  89,	90,  90,  90,  91,
						91,  92,  92,  93,	93,  94,  94,  95,	95,  96,  96,  96,	97,  97,  98,  98,
						99,  99,  99, 100, 100, 101, 101, 102, 102, 102, 103, 103, 104, 104, 104, 105,

                        },
                    },
                },
                {
                    32768 ,  /// region
                    {
                        {
                            20,  32,  45,  49,  54,  57,  61,  64,  67,  70,  73,  76,  78,  81,  83, 86,
                            88,  90,  91,  92,  93,  94,  95,  95,  96,  96,  98, 100, 101, 103, 105, 106,
                           107, 108, 109, 110, 111, 112, 114, 115, 119, 123, 128, 133, 138, 142, 145, 150,
                           152, 154, 155, 157, 158, 159, 161, 162, 164, 165, 166, 168, 169, 170, 172, 173,
                           174, 176, 177, 178, 179, 181, 182, 183, 184, 186, 187, 188, 189, 190, 192, 193,
                           194, 195, 196, 197, 199, 200, 201, 202, 203, 204, 205, 206, 207, 209, 210, 211,
                           212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227,
                           228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243
                        },
                    },
                },
                {
                    65536 ,  /// region
                    {
                        {
                             30,  38,  45,  51,  56,  60,  65,  69,  73,  77,  80,  83,  87,  90,  93,  96,
                             99, 101, 104, 107, 109, 112, 114, 116, 119, 121, 123, 126, 128, 130, 132, 134,
                            136, 138, 140, 142, 144, 146, 148, 149, 151, 153, 155, 157, 158, 160, 162, 163,
                            165, 167, 168, 170, 172, 173, 175, 176, 178, 180, 181, 183, 184, 186, 187, 188,
                            190, 191, 193, 194, 196, 197, 198, 200, 201, 202, 204, 205, 207, 208, 209, 210,
                            212, 213, 214, 216, 217, 218, 219, 221, 222, 223, 224, 226, 227, 228, 229, 230,
                            232, 233, 234, 235, 236, 237, 239, 240, 241, 242, 243, 244, 245, 246, 248, 249,
                            250, 251, 252, 253, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                        },

                    },
                },
                {
                    131072,  /// region
                    {
                        {
                            30,  38,  45,  51,  56,  60,  65,  69,  73,  77,  80,  83,  87,  90,  93,  96,
                             99, 101, 104, 107, 109, 112, 114, 116, 119, 121, 123, 126, 128, 130, 132, 134,
                            136, 138, 140, 142, 144, 146, 148, 149, 151, 153, 155, 157, 158, 160, 162, 163,
                            165, 167, 168, 170, 172, 173, 175, 176, 178, 180, 181, 183, 184, 186, 187, 188,
                            190, 191, 193, 194, 196, 197, 198, 200, 201, 202, 204, 205, 207, 208, 209, 210,
                            212, 213, 214, 216, 217, 218, 219, 221, 222, 223, 224, 226, 227, 228, 229, 230,
                            232, 233, 234, 235, 236, 237, 239, 240, 241, 242, 243, 244, 245, 246, 248, 249,
                            250, 251, 252, 253, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                        },

                    },
                },
                {
                    262144,  /// region
                    {
                        {
                         /*
                             20,  27,  33,  37,  42,  45,  49,  52,  55,  58,  61,  64,  66,  69,  71,  73,
                             75,  78,  80,  82,  84,  86,  88,  89,  91,  93,  95,  97,  98, 100, 102, 103,
                            105, 106, 108, 109, 111, 112, 114, 115, 117, 118, 119, 121, 122, 123, 125, 126,
                            127, 129, 130, 131, 132, 134, 135, 136, 137, 139, 140, 141, 142, 143, 144, 146,
                            147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 160, 161, 162, 163,
                            164, 165, 166, 167, 168, 169, 170, 171, 171, 172, 173, 174, 175, 176, 177, 178,
                            179, 180, 181, 182, 183, 184, 184, 185, 186, 187, 188, 189, 190, 191, 191, 192,
                            193, 194, 195, 196, 197, 197, 198, 199, 200, 201, 201, 202, 203, 204, 205, 206,*/
                            30,  38,  45,  51,  56,  60,  65,  69,  73,  77,  80,  83,  87,  90,  93,  96,
                             99, 101, 104, 107, 109, 112, 114, 116, 119, 121, 123, 126, 128, 130, 132, 134,
                            136, 138, 140, 142, 144, 146, 148, 149, 151, 153, 155, 157, 158, 160, 162, 163,
                            165, 167, 168, 170, 172, 173, 175, 176, 178, 180, 181, 183, 184, 186, 187, 188,
                            190, 191, 193, 194, 196, 197, 198, 200, 201, 202, 204, 205, 207, 208, 209, 210,
                            212, 213, 214, 216, 217, 218, 219, 221, 222, 223, 224, 226, 227, 228, 229, 230,
                            232, 233, 234, 235, 236, 237, 239, 240, 241, 242, 243, 244, 245, 246, 248, 249,
                            250, 251, 252, 253, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                        },

                    },
                },
                {
                    524288,  /// region
                    {
                        {
                           /*
                            20,  27,  33,  37,  42,  45,  49,  52,  55,  58,  61,  64,  66,  69,  71,  73,
                            75,  78,  80,  82,  84,  86,  88,  89,  91,  93,  95,  97,  98, 100, 102, 103,
                           105, 106, 108, 109, 111, 112, 114, 115, 119, 123, 128, 133, 138, 142, 145, 150,
                           152, 154, 155, 157, 158, 159, 161, 162, 164, 165, 166, 168, 169, 170, 172, 173,
                           174, 176, 177, 178, 179, 181, 182, 183, 184, 186, 187, 188, 189, 190, 192, 193,
                           194, 195, 196, 197, 199, 200, 201, 202, 203, 204, 205, 206, 207, 209, 210, 211,
                           212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227,
                           228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243*/
                           30,  38,  45,  51,  56,  60,  65,  69,  73,  77,  80,  83,  87,  90,  93,  96,
                             99, 101, 104, 107, 109, 112, 114, 116, 119, 121, 123, 126, 128, 130, 132, 134,
                            136, 138, 140, 142, 144, 146, 148, 149, 151, 153, 155, 157, 158, 160, 162, 163,
                            165, 167, 168, 170, 172, 173, 175, 176, 178, 180, 181, 183, 184, 186, 187, 188,
                            190, 191, 193, 194, 196, 197, 198, 200, 201, 202, 204, 205, 207, 208, 209, 210,
                            212, 213, 214, 216, 217, 218, 219, 221, 222, 223, 224, 226, 227, 228, 229, 230,
                            232, 233, 234, 235, 236, 237, 239, 240, 241, 242, 243, 244, 245, 246, 248, 249,
                            250, 251, 252, 253, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,

                          
                        },
                    },
                },
            },
        },
    },
    /*
    *
    * 
    *
    */
    {
        1     ,  /// dmsTuneEnable
        0     ,  /// dmsSmallaliasDetThresh
        1     ,  /// dmsBdOption
        80    ,  /// dmsDuShOffset
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
                {   1 * 1024, {1    , 1    , 0    }},
                {   2 * 1024, {1    , 1    , 0    }},
                {   4 * 1024, {1    , 1    , 0    }},
                {   8 * 1024, {1    , 1    , 0    }},
                {  16 * 1024, {1    , 1    , 0    }},
                {  32 * 1024, {1    , 1    , 0    }},
                {  64 * 1024, {1    , 1    , 0    }},
                { 128 * 1024, {1    , 1    , 0    }},
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
				
				{   1 * 1024, {100  , 5000 , 253  , 1350 , 210  , 50   , 240  , 50   , 125  , 0    , 100   , 50   , 150  , 50   , 260  , 90  , 150  , 435  , 201  , 369  , 178  , 5500 , 96   , 2    }},
                {   2 * 1024, {90   , 5000 , 253  , 1350 , 200  , 50   , 230  , 50   , 112  , 0    , 100   , 50   , 150  , 50   , 260  , 90  , 140  , 435  , 201  , 369  , 178  , 5200 , 96   , 2    }},
                {   4 * 1024, {80   , 5000 , 253  , 1350 , 200  , 50   , 230  , 50   , 100  , 0    , 95   , 50   , 150  , 50   , 255  , 90   , 140  , 435  , 201  , 369  , 178  , 5100 , 96   , 2    }},
                {   8 * 1024, {30   , 5000 , 253  , 1350 , 200  , 50   , 230  , 50   , 100  , 0    , 20   , 50   , 150  , 50   , 255  , 90   , 140  , 435  , 201  , 369  , 178  , 5000 , 96   , 2    }},
                {  16 * 1024, {30   , 5000 , 253  , 1350 , 200  , 50   , 230  , 50   , 100  , 0    , 20   , 50   , 160  , 50   , 255  , 90   , 100  , 435  , 201  , 369  , 178  , 5000 , 64   , 2    }},
                {  32 * 1024, {30   , 5000 , 253  , 1350 , 200  , 40   , 220  , 50   , 100  , 0    , 20   , 50   , 180  , 50   , 255  , 90   , 100  , 435  , 201  , 369  , 178  , 3000 , 50   , 2    }},
                {  64 * 1024, {20   , 5000 , 253  , 1350 , 200  , 25   , 200  , 50   , 100  , 0    , 20   , 50   , 180  , 50   , 255  , 90   , 90   , 435  , 201  , 369  , 178  , 2000 , 30   , 2    }},
                { 128 * 1024, {20   , 5000 , 253  , 1350 , 180  , 25   , 200  , 50   , 100  , 0    , 20   , 50   , 180  , 50   , 255  , 90   , 60   , 435  , 201  , 369  , 178  , 1500 , 20   , 2    }},
                { 256 * 1024, {10   , 2400 , 253  , 1350 , 140  , 0    , 180  , 50   , 80   , 0    , 20   , 50   , 150  , 50   , 255  , 90   , 60   , 435  , 201  , 369  , 178  , 1000 , 20   , 2    }},
                { 512 * 1024, {10   , 2400 , 253  , 1350 , 140  , 0    , 180  , 50   , 80   , 0    , 20   , 50   , 130  , 50   , 255  , 90   , 60   , 435  , 201  , 369  , 178  , 500  , 20   , 2    }},
                /*
                {   1 * 1024, {40  , 5000 , 253  , 1350 , 210  , 60   , 240  , 60   , 125  , 0    , 150   , 128   , 230  , 200   , 260  , 0  , 100  , 150  , 201  , 0  , 0  , 5500 , 96   , 2    }},
                {   2 * 1024, {20   , 5000 , 253  , 1350 , 200  , 60   , 230  , 60   , 112  , 0    , 150  , 128   , 230  , 200   , 260  , 0  , 100  , 150  , 201  , 0  , 0  , 5200 , 96   , 2    }},
                {   4 * 1024, {10   , 5000 , 253  , 1350 , 200  , 60   , 230  , 60   , 100  , 0    , 120  , 128   , 230  , 200   , 255  , 0   , 100  , 150  , 201  , 369  , 178  , 5100 , 96   , 2    }},
                {   8 * 1024, {10   , 5000 , 253  , 1350 , 200  , 60   , 230  , 60   , 100  , 0    , 100  , 128   , 230  , 200   , 255  , 0   , 100  , 150  , 201  , 369  , 178  , 4000 , 70   , 2    }},
                {  16 * 1024, {10   , 5000 , 253  , 1350 , 200  , 60   , 230  , 60   , 100  , 0    , 80   , 128   , 230  , 200   , 255  , 0   , 100  , 150  , 201  , 369  , 178  , 3500 , 64   , 2    }},
                {  32 * 1024, {10   , 5000 , 253  , 1350 , 200  , 60   , 220  , 60   , 100  , 0    , 80   , 128   , 180  , 200   , 255  , 0   , 100  , 150  , 201  , 369  , 178  , 3000 , 50   , 2    }},
                {  64 * 1024, {10   , 5000 , 253  , 1350 , 200  , 50   , 200  , 60   , 100  , 0    , 80   , 128   , 180  , 200   , 255  , 0   , 90   , 150  , 201  , 369  , 178  , 2000 , 30   , 2    }},
                { 128 * 1024, {10   , 5000 , 253  , 1350 , 180  , 25   , 200  , 60   , 100  , 0    , 80   , 128   , 180  , 200   , 255  , 0   , 60   , 150  , 201  , 369  , 178  , 1500 , 20   , 2    }},
                { 256 * 1024, {10   , 2400 , 253  , 1350 , 140  , 0    , 180  , 60   , 80   , 0    , 80   , 128   , 150  , 200   , 255  , 0   , 60   , 150  , 201  , 369  , 178  , 1000 , 20   , 2    }},
                { 512 * 1024, {10   , 2400 , 253  , 1350 , 140  , 0    , 180  , 60   , 80   , 0    , 80   , 128   , 130  , 200   , 255  , 0   , 60   , 150  , 201  , 369  , 178  , 500  , 20   , 2    }},
                */

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
                            1,   1,   1,   1,   3,   4,   5,   6,   7,   7,   8,   9,   9,  10,  10,  10,
                            11,  11,  12,  12,  12,  13,  13,  14,  14,  14,  14,  15,  15,  15,  16,  16,
                            16,  17,  17,  17,  17,  18,  18,  18,  18,  19,  19,  19,  19,  20,  20,  20,
                            20,  20,  21,  21,  21,  21,  22,  22,  22,  22,  22,  23,  23,  23,  23,  23,
                            24,  24,  24,  24,  24,  24,  25,  25,  25,  25,  25,  26,  26,  26,  26,  26,
                            26,  27,  27,  27,  27,  27,  27,  28,  28,  28,  28,  28,  28,  29,  29,  29,
                            29,  29,  29,  30,  30,  30,  30,  30,  30,  30,  31,  31,  31,  31,  31,  31,
                            31,  32,  32,  32,  32,  32,  32,  32,  33,  33,  33,  33,  33,  33,  33,  34,

                        },
                    },
                },
                {
                    2048  ,  /// region
                    {
                        {
                            3,   5,   7,   8,   9,  10,  11,  12,  12,  13,  14,  14,  15,  16,  16,  17,
                            17,  18,  18,  19,  19,  20,  20,  20,  21,  21,  22,  22,  22,  23,  23,  24,
                            24,  24,  25,  25,  25,  26,  26,  26,  27,  27,  27,  28,  28,  28,  29,  29,
                            29,  30,  30,  30,  30,  31,  31,  31,  32,  32,  32,  32,  33,  33,  33,  34,
                            34,  34,  34,  35,  35,  35,  35,  36,  36,  36,  36,  37,  37,  37,  37,  37,
                            38,  38,  38,  38,  39,  39,  39,  39,  40,  40,  40,  40,  40,  41,  41,  41,
                            41,  41,  42,  42,  42,  42,  43,  43,  43,  43,  43,  44,  44,  44,  44,  44,
                            45,  45,  45,  45,  45,  46,  46,  46,  46,  46,  46,  47,  47,  47,  47,  47,
                        },
                    },
                },
                {
                    4096  ,  /// region
                    {
                        {
                            5,   8,  10,  11,  13,  14,  15,  16,  17,  18,  19,  20,  21,  21,  22,  23,
                            24,  24,  25,  26,  26,  27,  28,  28,  29,  29,  30,  30,  31,  31,  32,  33,
                            33,  34,  34,  34,  35,  35,  36,  36,  37,  37,  38,  38,  39,  39,  39,  40,
                            40,  41,  41,  41,  42,  42,  43,  43,  43,  44,  44,  45,  45,  45,  46,  46,
                            46,  47,  47,  47,  48,  48,  48,  49,  49,  49,  50,  50,  50,  51,  51,  51,
                            52,  52,  52,  53,  53,  53,  54,  54,  54,  55,  55,  55,  55,  56,  56,  56,
                            57,  57,  57,  57,  58,  58,  58,  59,  59,  59,  59,  60,  60,  60,  61,  61,
                            61,  61,  62,  62,  62,  62,  63,  63,  63,  63,  64,  64,  64,  65,  65,  65
                        },
                    },
                },
                {
                    8192  ,  /// region
                    {
                        {
                            9,  12,  14,  16,  18,  19,  21,  22,  23,  24,  25,  27,  28,  29,  30,  31,
                            31,  32,  33,  34,  35,  36,  36,  37,  38,  39,  39,  40,  41,  41,  42,  43,
                            43,  44,  45,  45,  46,  47,  47,  48,  48,  49,  49,  50,  51,  51,  52,  52,
                            53,  53,  54,  54,  55,  55,  56,  56,  57,  57,  58,  58,  59,  59,  60,  60,
                            61,  61,  62,  62,  62,  63,  63,  64,  64,  65,  65,  66,  66,  66,  67,  67,
                            68,  68,  68,  69,  69,  70,  70,  70,  71,  71,  72,  72,  72,  73,  73,  74,
                            74,  74,  75,  75,  75,  76,  76,  77,  77,  77,  78,  78,  78,  79,  79,  79,
                            80,  80,  80,  81,  81,  81,  82,  82,  83,  83,  83,  84,  84,  84,  85,  85,
                        },
                    },
                },
                {
                    16384 ,  /// region
                    {
                        {
                            12,  16,  19,  21,  23,  25,  27,  29,  31,  32,  34,  35,  37,  38,  39,  41,
                            42,  43,  44,  45,  46,  48,  49,  50,  51,  52,  53,  54,  54,  55,  56,  57,
                            58,  59,  60,  61,  61,  62,  63,  64,  65,  65,  66,  67,  68,  68,  69,  70,
                            71,  71,  72,  73,  73,  74,  75,  75,  76,  77,  77,  78,  79,  79,  80,  81,
                            81,  82,  82,  83,  84,  84,  85,  85,  86,  87,  87,  88,  88,  89,  89,  90,
                            91,  91,  92,  92,  93,  93,  94,  94,  95,  95,  96,  96,  97,  97,  98,  98,
                            99,  99, 100, 100, 101, 101, 102, 102, 103, 103, 104, 104, 105, 105, 106, 106,
                            107, 107, 108, 108, 109, 109, 110, 110, 110, 111, 111, 112, 112, 113, 113, 114,
                        },
                    },
                },
                {
                    32768 ,  /// region
                    {
                        {
                            13,  19,  23,  27,  30,  33,  36,  38,  41,  43,  45,  47,  49,  51,  53,  55,
                            56,  58,  59,  61,  63,  64,  65,  67,  68,  70,  71,  72,  74,  75,  76,  77,
                            78,  80,  81,  82,  83,  84,  85,  86,  87,  89,  90,  91,  92,  93,  94,  95,
                            96,  97,  98,  99,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 108, 109,
                            110, 111, 112, 113, 113, 114, 115, 116, 117, 118, 118, 119, 120, 121, 121, 122,
                            123, 124, 124, 125, 126, 127, 127, 128, 129, 130, 130, 131, 132, 132, 133, 134,
                            135, 135, 136, 137, 137, 138, 139, 139, 140, 141, 141, 142, 143, 143, 144, 145,
                            145, 146, 147, 147, 148, 148, 149, 150, 150, 151, 152, 152, 153, 153, 154, 155,
                        },
                    },
                },
                {
                    65536 ,  /// region
                    {
                        {
                            15,  24,  39,  48,  57,  61,  65,  68,  71,  74,  77,  80,  83,  86,  88,  91,
                            93,  96,  98, 100, 102, 105, 107, 109, 111, 113, 115, 117, 119, 121, 123, 124,
                           126, 128, 130, 131, 133, 135, 137, 138, 140, 141, 143, 145, 146, 148, 149, 151,
                           152, 154, 155, 157, 158, 159, 161, 162, 164, 165, 166, 168, 169, 170, 172, 173,
                           174, 176, 177, 178, 179, 181, 182, 183, 184, 186, 187, 188, 189, 190, 192, 193,
                           194, 195, 196, 197, 199, 200, 201, 202, 203, 204, 205, 206, 207, 209, 210, 211,
                           212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227,
                           228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243
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
        10    ,  /// mvdPipeABSadBitdepth
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
            11    ,  /// validSize
            {
                /// [0]gain
                /// [1]mvdPipeBErode1
                /// [2]mvdPipeBDilate
                /// [3]mvdPipeBErode2
                /// [4]mvdPipeBMapRefineThr
                /// 0          1      2      3      4
                {   1 * 1024, {1    , 0    , 0    , 4    }},
                {   2 * 1024, {1    , 0    , 0    , 4    }},
                {   4 * 1024, {1    , 0    , 0    , 4    }},
                {   8 * 1024, {1    , 0    , 0    , 4    }},
                {  16 * 1024, {1    , 0    , 0    , 4    }},
                {  32 * 1024, {1    , 0    , 0    , 4    }},
                {  64 * 1024, {1    , 0    , 0    , 5    }},
				{  96 * 1024, {2    , 0    , 0    , 5    }},
                { 128 * 1024, {2    , 0    , 0    , 5    }},
                { 256 * 1024, {2    , 0    , 0    , 5    }},
                { 512 * 1024, {3    , 0    , 0    , 8    }},
            },
        },
        /*
        *
        * mvdGainInterpTab
        *
        */
        {
            11    ,  /// validSize
            {
                /// [0]gain
                /// [1]mvdNpGain
                /// [2]mvdNpOffset
                /// [3]mvdPipeaThr
                /// [4]mvdPipeABTriDirStr
                /// 0          1      2      3      4
                {   1 * 1024, {30   , 2    , 3    , 128    }},
                {   2 * 1024, {28   , 2    , 3    , 128    }},
                {   4 * 1024, {28   , 2    , 3    , 128  }},
                {   8 * 1024, {20   , 2    , 4    , 128   }},
                {  16 * 1024, {18   , 2    , 6    , 256   }},
                {  32 * 1024, {18   , 2    , 6    , 256   }},
                {  64 * 1024, {18   , 2    , 8    , 256   }},
				{  96 * 1024, {22   , 3    , 8    , 256   }},
                { 128 * 1024, {22  , 3    , 10    , 256   }},
                { 256 * 1024, {24   , 3    , 10    , 256   }},
                { 512 * 1024, {25   , 3    , 10    , 256   }},
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
                {   1 * 1024, {2    , 7    , 7    , 6    , 7    , 7    , 7    }},
                {   2 * 1024, {2    , 7    , 7    , 6    , 7    , 7    , 7    }},
                {   4 * 1024, {2    , 6    , 6    , 6    , 7    , 7    , 7    }},
                {   8 * 1024, {2    , 6    , 6    , 6    , 7    , 7    , 7    }},
                {  16 * 1024, {2    , 6    , 6    , 6    , 7    , 7    , 7    }},
                {  32 * 1024, {2    , 6    , 6    , 6    , 7    , 7    , 7    }},
                {  64 * 1024, {2    , 6    , 6    , 6    , 7    , 7    , 7    }},
                { 128 * 1024, {2    , 6    , 6    , 6    , 7    , 8    , 8    }},
                { 256 * 1024, {2    , 6    , 6    , 6    , 7    , 8    , 8    }},
                { 512 * 1024, {2    , 6    , 6    , 6    , 7    , 8    , 8    }},
            },
        },
        /*
        *
        * tnrGainInterpTab
        *
        */
        {
            14    ,  /// validSize
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
                {   1 * 1024, {160  , 32   , 50   , 50   , 0   , 4    , 0    , 20   , 32   , 64   , 4    , 0    , 20   , 38   , 0    , 18   , 40   , 0    , 54   , 20   , 0    , 43   , 23   , 0    }},
                {   2 * 1024, {160  , 32   , 60   , 60   , 0   , 4    , 0    , 20   , 32   , 64   , 4    , 0    , 20   , 38   , 0    , 18   , 42   , 0    , 55   , 30   , 0    , 45   , 25   ,0    }},
                {   4 * 1024, {160  , 32   , 70   , 60   , 0   , 4    , 0    , 20   , 48   , 64   , 0    , 0    , 21   , 38   , 0    , 23   , 35   , 0    , 57   , 30  , 0     , 45   , 27   , 0    }},
                {   8 * 1024, {160  , 32   , 82   , 82   , 0   , 4    , 0    , 20   , 48   , 64   , 0    , 0    , 28   , 38   , 0    , 20   , 35   , 0    , 59   , 33   , 0    , 45   , 30   , 0    }},
                {  16 * 1024, {160  , 32   , 90   , 90  , 0   , 4    , 0    , 20   , 48   , 62   , 0    , 0    , 28   , 35   , 0    , 22   , 25    ,0   , 60   , 35   , 0     , 45    ,32   , 0    }},
                {  32 * 1024, {160  , 32   , 90  , 70  , 0   , 4    , 0    , 20   , 48   , 60   , 0    , 0     , 28   , 35   , 0    , 24   , 25   , 0    , 60   , 35   ,2    , 46   , 32   ,6    }},
                {  64 * 1024, {160  , 32   , 80  , 70  , 0   , 4    , 0   , 0   , 20   , 64   , 6    , 8    , 11   , 28   , 0    , 15   , 25   , 0    , 60   , 35   , 3    , 48   , 32   , 4    }},
				{  75 * 1024, {160  , 32   , 85  , 70  , 0   , 4    , 0    , 0   , 20   , 64   , 6    , 8    , 13   , 32   , 0    , 11   , 28   , 0    , 60   , 35   , 3    , 52   , 32   , 4    }},
				{  86 * 1024, {160  , 32   , 95  , 70  , 0   , 4    , 0   , 0   , 25   , 64   , 8    , 8    , 11   , 32   , 0    , 11   , 28   , 0    , 60   , 35   , 3    , 54   , 32   , 4    }},
                {  98 * 1024, {160  , 32   , 116  , 70  , 0   , 4    , 0    , 0   , 25   , 64  , 8    , 12    , 12   , 34   , 2    , 11   , 32   , 0    , 61   , 38   , 3    , 56   , 25   , 4    }},
				{ 128 * 1024, {160  , 32   , 118  , 90  , 0   , 4    , 0    , 2   , 35   , 64   , 8    , 12    , 8   , 34   , 2    , 11   , 32   , 0    , 61   , 38   , 3    , 58   , 25   , 4    }},
                { 230 * 1024, {160  , 32   , 118  , 90  , 0   , 4    , 0    , 5   , 56   , 64   , 8    , 12    , 14   , 28   , 2    , 14   , 34   , 0    , 61   , 38   , 8    , 58   , 25   , 12    }}, 
                { 260 * 1024, {160  , 32   , 118  , 90  , 0   , 4    , 0    , 5   , 56   , 64   , 8    , 12   , 14   ,28     , 2    , 14   , 34   , 0    , 60   , 42   , 8    , 58   , 25   , 12    }},
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
                {1 * 1024  ,  {9    , 9    , 5    , 6    , 4    , 4    , 0    , 8    , 8    , 9    , 9    , 1    , 7    , 6    , 7    , 8    }},
                {2 * 1024  ,  {9    , 9    , 5    , 6    , 4    , 4    , 0    , 8    , 8    , 9    , 9    , 1    , 7    , 6    , 7    , 8    }},
                {4 * 1024  ,  {9    , 9    , 5    , 6    , 4    , 4    , 0    , 8    , 8    , 9    , 9    , 1    , 7    , 6    , 7    , 8    }},
                {8 * 1024  ,  {9    , 9    , 5    , 6    , 4    , 4    , 0    , 8    , 8    , 9    , 9    , 1    , 7    , 6    , 7    , 8    }},
                {16 * 1024 ,  {9    , 9    , 5    , 6    , 4    , 4    , 0    , 8    , 8    , 9    , 8    , 1    , 7    , 6    , 7    , 8    }},
                {32 * 1024 ,  {9    , 8    , 5    , 6    , 4    , 4    , 0    , 8    , 8    , 9    , 8, 1    , 7    , 6    , 7    , 8    }},
                {64 * 1024 ,  {9    , 8    , 5    , 6    , 4    , 4    , 0    , 8    , 8    , 8    , 8    , 1    , 7    , 6    , 7    , 8    }},
                {128 * 1024,  {9    , 8    , 5    , 6    , 4    , 4    , 0    , 8    , 8    , 8    , 8    , 1    , 7    , 6    , 7    , 8    }},
                {256 * 1024,  {9    , 8    , 5    , 6    , 4    , 4    , 0    , 8    , 8    , 8    , 8    , 1    , 7    , 6    , 7    , 8    }},
                {512 * 1024,  {8    , 8    , 5    , 6    , 4    , 4    , 0    , 8    , 8    , 8    , 8    , 1    , 7    , 6    , 7    , 8    }},
            },
        },
        /*
        *
        * lshGainInterpTab
        *
        */
        {
            13    ,  /// validSize
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
                {   1 * 1024, {256  , 32   , 0    , 0    , 0    , 300 , 106  , 0    , 256  , 120  , 50  , 8    , 0    , 0     ,0     , 380  , 300  , 0    , 0    , 100  , 100  , 206  , 90  , 8  , 356  , 1000  , 580  , 150  , 250  , 550  , 350  , 380  ,180  , 256  , 156  , 0    , 0    , 64   , 256  , 256  , 160  , 64   , 256 , 256  , 256   }},
                {   2 * 1024, {256  , 32   , 0    , 0    , 0    , 300 , 100  , 0    , 256  , 120  , 50  , 8    , 0    , 0     , 0    , 600  , 400  , 0    , 0    , 100  , 100  , 216  , 90  , 8   , 356  , 1000  , 580  , 156  , 250  , 600  , 380  , 380  , 180  , 256  , 156  , 0    , 0    , 64   , 256  , 256  , 200  , 64   , 256  , 256  , 256  }},
                {   4 * 1024, {220  , 32   , 0    , 0    , 0    , 100 , 100  , 0    , 256  , 120  , 50  , 8    , 0    , 0    , 0    , 600  , 400  , 0    , 0    , 100  , 100  , 256  , 90  , 16   , 256  , 1000  , 500  , 156  , 250  , 600  , 550  , 380  , 200  , 256  , 156  , 0    , 0    , 64   , 256  , 256  , 220  , 64   , 256  , 256  , 256  }},
                {   8 * 1024, {200  , 32   , 0    , 0    , 0    , 100 , 100  , 0    , 256  , 120  , 50  , 8    , 0    , 0    , 0    , 600  , 400  , 0    , 0    , 100  , 100  , 256  , 90  , 16   , 200  , 1000  , 520  , 150  , 250  , 600   , 600  , 380  , 256  , 256  , 156  , 0    , 0    , 64   , 256  , 256  , 230   , 64   , 256 , 256  , 256  }},
                {  16 * 1024, {180  , 32   , 0    , 0    , 10    , 100 , 100  , 0    , 206  , 120  , 20  , 8    , 0    , 0    , 0    , 600  , 500  , 0    , 0    , 100  , 100  , 246  , 90  , 20   , 160  , 1000  , 600  , 160  , 230  , 600  , 850  , 380  , 256  , 256  , 156  , 0    , 0    , 64   , 256  , 256  , 235   , 64   , 256  , 256  , 216  }},
                {  32 * 1024, {100  , 32   , 10  , 0    , 10    , 100 , 100  , 0    , 226  , 70  , 20  , 8    , 0    , 0    , 0    , 1000  , 800  , 0    , 0    , 100  , 100  , 236  , 100  , 36   , 128   ,1000  , 900  , 160  , 128  , 900  , 700  , 380  , 250  , 256  , 156  , 0    , 0    , 64   , 216  , 256  , 216   , 64   , 216  , 256  , 216  }},
                {  64 * 1024, {90  , 180  , 10  , 0    , 10    , 100 , 100  , 0    , 226  , 75  , 20  , 8    , 0    , 0    , 0    , 800  , 800  , 0    , 0    , 100  , 100  , 206  , 100  , 126   , 158   ,900  , 900  , 160  , 128  , 800  , 700  , 380  , 250  , 256,    156   , 0    , 0    , 64   , 216  , 256  , 216   , 64   , 216  ,256  , 256  }},
				{  78 * 1024, {91  , 180  , 10  , 0    , 10    , 100 , 100  , 0    , 226  , 70  , 20  , 8    , 0    , 0    , 0    , 800  , 800  , 0    , 0    , 100  , 100  , 206  , 100  , 126   , 158   ,900  , 800  , 190  , 128  , 850  , 700  , 380  , 250  , 256,    156   , 0    , 0    , 64   , 156  , 256  , 216   , 64   , 156  ,256  , 216  }},
				{  86 * 1024, {91  , 180  , 10  , 0    , 10    , 100 , 100  , 0    , 216  , 70  , 10  , 8    , 0    , 0    , 0    , 800  , 700  , 0    , 0    , 100  , 100  , 206  , 100  , 226   , 157   ,900  , 800  , 190  , 128  , 850  , 700  , 380  , 250  , 256,    156   , 0    , 0    , 64   , 156  , 256  , 216   , 64   , 156  ,256  , 216  }},
				{  98 * 1024, {90  , 180  , 20  , 10    , 20    , 100 , 100  , 0    , 256  , 70  , 10  , 8    , 0    , 0   , 0   , 800  , 500  , 0    , 0    , 100  , 100  , 206  , 100  , 226   , 168   ,900  ,800  , 190  , 160  , 800  , 700  , 336  , 250  , 256   , 156   , 0    , 0    , 64   , 56  , 256  , 206   , 64   , 56  ,256  , 206  }},
                { 128 * 1024, {90  , 180  , 20  , 10    , 20     , 100 , 100  , 0    , 256  , 60  , 10  , 8    , 0    , 0   , 0   , 800  , 500  , 0    , 0    , 100  , 100  , 206  , 100  , 226   , 168   ,900  ,800  , 190  , 160  , 800  , 700  , 336  , 200  , 256   , 156   , 0    , 0    , 64   , 56  , 256  , 206   , 64   , 56  ,256  , 206  }},
                { 256 * 1024, {90  , 120  , 60  , 20    , 30    , 100 , 100  , 0    , 256  , 60  , 10  , 8    , 0    , 0   , 0   , 800  , 600  , 0    , 0    , 100  , 100  , 206  , 100  , 226   ,150   ,900 , 800  , 190  , 160  , 500  , 700  ,306  , 200  , 256   , 200   , 0    , 0    , 64   , 56  , 256  , 206   , 64   , 56  ,256  , 206  }},
                { 512 * 1024, {50   , 120  , 200  , 30    , 30    , 100 , 100  , 0    , 256  , 60  , 10  , 8    , 0    , 0   , 0   , 800  , 600  , 0    , 0    , 100  , 100  , 206  , 100  , 266   ,150   , 800  ,800  , 156  , 160  , 500  , 700  , 320  , 150  , 256   , 200   , 0    , 0    , 64   , 56  , 256  , 206   , 64   , 56  ,256  , 206  }},
            
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
                {50    , {0    , 6    , 8    , 9    , 9    , 7    , 6    , 7    , 8    }},
                {100   , {0    , 6    , 8    , 9    , 9    , 7    , 6    , 7    , 8    }},
                {150   , {0    , 7    , 8    , 9    , 9    , 7    , 6    , 7    , 8    }},
                {200   , {0    , 8    , 8    , 9    , 9    , 7    , 6    , 7    , 8    }},
                {250   , {0    , 8    , 8    , 9    , 9    , 7    , 6    , 7    , 8    }},
                {300   , {0    , 8    , 8    , 9    , 9    , 7    , 6    , 7    , 8    }},
                {350   , {0    , 8    , 8    , 8    , 9    , 7    , 6    , 7    , 8    }},
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
				/// [17]lshNegStr1
                /// [18]lshPosStr1
                /// 0     1      2      3      4      5      6       7      8      9      10     11     12     13     14     15     16       17     18    
                {50    , {20   , 200  , 1200  ,1000  , 128  , 276  , 756  , 656  , 64   , 256  , 200  , 100   , 64   , 256  , 180  , 100  , 1200  , 1000  }},
                {100   , {20   , 200  , 1200  ,950   , 128  , 276  , 756  , 552  , 64   , 256  , 256  , 130   , 64   , 256  , 180  , 130  , 1200  , 1000  }},
                {150   , {16   , 200  , 1100  ,550   , 128  , 206  , 626  , 486  , 64   , 256  , 256  , 145   , 64   , 256  , 210  , 145  , 1100  , 1000  }},
                {200   , {16   , 200  , 1000  ,450   , 128  , 206  , 380  , 286  , 64   , 256  , 256  , 155   , 64   , 256  , 230  , 155  , 900   , 800  }},
                {250   , {16   , 220  , 1000  ,400   , 128  , 200  , 380  , 286  , 64   , 256  , 256  , 200   , 64   , 256  , 256  , 185  , 900  , 800  }},
                {300   , {16    , 256  , 1000  ,330   , 128  , 206  , 380  , 286  , 64   , 256  , 256  , 210   , 64   , 256  , 256  , 220  , 906  , 650  }},
                {350   , {16   , 256  , 1000  ,356   , 128  , 256  , 356  , 286  , 64   , 256  , 256  , 230   , 64   , 256  , 256  , 230  , 796  , 630  }},
                {400   , {16   , 256  , 850  , 356   , 128  , 256  , 356  , 286  , 64   , 256  , 256  , 256   , 64   , 256  , 256  , 256  , 756  , 656  }},
                {450   , {16   , 256  , 800  , 356   , 128  , 256  , 356  , 286  , 64   , 256  , 256  , 256   , 64   , 256  , 256  , 256  , 756  , 656  }},
                {500   , {16   , 60   , 600  , 50    , 128  , 256  , 256  , 384  , 64   , 256  , 256  , 256   , 64   , 256  , 256  , 256  , 756  , 656  }},

            },
        },
    },
    /*
    *
    * r
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
                {  64 * 1024, {400  , 0    , 30   , 30   , 30   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 0    , 25   , 256  , 2    , 512  , 256  , 230  , 156  , 32   , 200  , 100  , 0    }},
                { 128 * 1024, {500  , 0    , 30   , 30   , 30   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 0    , 25   , 256  , 2    , 512  , 256  , 120  , 156  , 32   , 200  , 100  , 0  }},
                { 256 * 1024, {500  , 0    , 50   , 50   , 30   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 0    , 25   , 128  , 2    , 512  , 251  , 60  , 100  , 32   , 200  , 100  , 0  }},
                { 512 * 1024, {256  , 0    , 80   , 80   , 50   , 0    , 256  , 128  , 0    , 0    , 200  , 100  , 100  , 25   , 128  , 2    , 512  , 251  , 60   , 40  , 32   , 200   , 100  , 0  }},
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
                {50    , {1180 }},
                {100   , {1180 }},
                {150   , {1150 }},
                {200   , {1150 }},
                {250   , {1150 }},
                {300   , {1150 }},
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
				/// [3]sdeRatioU
                /// [4]sdeRatioV
                /// 0          1        2     3      4
                {   1 * 1024, {1024 , 1024 , 1025 , 1025}},
                {   2 * 1024, {1024 , 1024 , 1024 , 1024}},
                {   4 * 1024, {1024 , 1024 , 1024 , 1024}},
                {   8 * 1024, {1024 , 1024 , 1024 , 1024}},
                {  16 * 1024, {1024 , 1024 , 1024 , 1024}},
                {  32 * 1024, {1024 , 1024 , 1024 , 1024}},
                {  64 * 1024, {1024 , 1024 , 1024 , 1024}},
                { 128 * 1024, {1024 , 1024 , 1024 , 1024}},
                { 256 * 1024, {1034 , 1034 , 1024 , 1024}},
                { 512 * 1024, {1024 , 1024 , 1024 , 1024}},
            },
		},
       
		   /* sde_core_param_by_lux_sat_t
		   *
		   */
		   {
			   11	 ,	/// validSize
			   {
				   /// [0]lux
				   /// [1]Brightness
				   /// 0	 1
				   {50	  , {1024 }},
				   {100   , {1024 }},
				   {150   , {1024 }},
				   {200   , {1024 }},
				   {250   , {1024 }},
				   {300   , {1024 }},
				   {350   , {1034 }},
				   {400   , {1034 }},
				   {450   , {1024 }},
				   {500   , {1024 }},
				   {525   , {1024 }},
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
