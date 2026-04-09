#ifndef __IQ_UTILS_H__
#define __IQ_UTILS_H__

#include "types_utils.h"
#include "tune_utils.h"
#include "log_utils.h"
#include "math_osutils.h"

#define CALCULATE_PARAMETER_BIT_SHIFT   (10)

#define MAX(a, b) ((a)>(b)? ((a):(b)))

/*
* CHECK_PARAM_RANGE
*
* @Check range value fof input
*
* @param value: input param, min, max
*
* @modify the value within the range if needed
*
*/
#define CHECK_PARAM_RANGE(pPara, min, max) \
{                                              \
    if (*pPara > max)                        \
    {                                          \
        ISP_WARN("The para (%d) is limited to [%d, %d].", *pPara, min, max);\
        *pPara = max;                        \
    }                                          \
    else if (*pPara < min)                   \
    {                                          \
        ISP_WARN("The para (%d) is limited to [%d, %d].", *pPara, min, max);\
        *pPara = min;                        \
    }                                          \
}

/*
* UP_TO_32BYTE
*
* @Round up the number by 32 byte
*
* @param value: input num
*
* @modify the calculated num if needed
*
*/

#define UP_TO_32BYTE(num) \
    (((((num) >> 5) << 5) < (num)) ? ((((num) >> 5) + 1) << 5) : (num))


/*
* C4_to_C5_Log2_Int
*
* @brief calculate C4 to C5 log2 int
*
* @param value: input value
*
* @return calculated value
*
*/
u32 C4_to_C5_Log2_Int(u32 value);

/*
* Four_To_Five_Curve
*
* @brief convert four segment curve to five segment curve
*
* @param offset:  four segment curve offset
* @param middle:  four segment curve middle
* @param end:     four segment curve end
* @param pT1~pT2: five segment curve result first point
*
* @return void
*
*/
void Four_To_Five_Curve(
        const u32 offset, const u32 middle, const u32 end,
        u32 *pT1, u32 *pT2, u32 *pT3, u32 *pT4);

/*
* Four_To_Five_Curve_Ver2
*
* @brief convert four segment curve to five segment curve
*
* @param offset:  four segment curve offset
* @param middle:  four segment curve middle
* @param end:     four segment curve end
* @param pT1~pT2: five segment curve result first point
*
* @return void
*
*/
void Four_To_Five_Curve_Ver2(
        const u32 stage1, const u32 slope1, const u32 stage2,
        const u32 slope2, u32 *pT1, u32 *pT2, u32 *pT3, u32 *pT4);

/*
* Get_Stage1_To_Cal_Curve
*
* @According to current curve and input param to cal new curve setting
*
* @param stage1_in:  new curve stage1
* @param pT1~pT2  :  point of curve result
*
* @return void
*
*/
void Get_Stage_1_To_Cal_Curve(
        const u32 stage1_in, u32 *pT1, u32 *pT2, u32 *pT3, u32 *pT4);


/*
* Get_Slope1_To_Cal_Curve
*
* @According to current curve and input param to cal new curve setting
*
* @param slope1_in:  new curve slope1
* @param pT1~pT2  :  point of curve result
*
* @return void
*
*/
void Get_Slope_1_To_Cal_Curve(
        const u32 slope1_in, u32 *pT1, u32 *pT2, u32 *pT3, u32 *pT4);


/*
* Get_Stage2_To_Cal_Curve
*
* @According to current curve and input param to cal new curve setting
*
* @param stage1_in:  new curve stage2
* @param pT1~pT2  :  point of curve result
*
* @return void
*
*/
void Get_Stage_2_To_Cal_Curve(
        const u32 stage2_in, u32 *pT1, u32 *pT2, u32 *pT3, u32 *pT4);


/*
* Get_Slope2_To_Cal_Curve
*
* @According to current curve and input param to cal new curve setting
*
* @param slope1_in:  new curve slope1
* @param pT1~pT2  :  point of curve result
*
* @return void
*
*/
void Get_Slope_2_To_Cal_Curve(
        const u32 slope2_in, u32 *pT1, u32 *pT2, u32 *pT3, u32 *pT4);


/*
* Clip
*
* @brief Clip param between Min and Max
*
* @param iVal:    source value
* @param iMin:    Min value
* @param iMax:    Max value
*
* @return cliped value
*
*/
s32 Clip(s32 iVal, s32 iMin, s32 iMax);


/*
* Round
*
* @brief round function
*
* @param value: source data
*
* @return rounded data
*
*/
u32 Round(f32 value);


/*
* Power
*
* @brief power function
*
* @param num: base data
* @param power: power data
*
* @return powered data
*
*/
u64 Power(u32 num, u32 power);

/*
* Get_Bit_Depth
*
* @brief power function
*
* @param val: get this val min bit depth
*
* @return bit_depth
*
*/
u8 Get_Bit_Depth(u32 val);

/*
* Interpolate
*
* @brief interpolate function
*
* @param x:
* @param x0:
* @param x1:
* @param y0:
* @param y1:
*
* @return interpolated value
*
*/
u32 Interpolate(u32 x, u32 x0, u32 x1, u32 y0, u32 y1);

/*
* Linear_Interp
*
* @brief Linear interpolate
*
* @param dataBefore:    start data
* @param dataAfter:     end   data
* @param dataFrac:      frac
* @param fracBits:      frac bits
*
* @return interpolated value
*
*/
s32 Linear_Interp(
    s32 dataBefore,
    s32 dataAfter,
    s32 dataFrac,
    u32 fracBits);


/*
* Calculate_Dynamic_Parameters
*
* @brief Calculate IQ Dynamic parameters
*
* @param pDst:        point to array to store result
* @param pSrcTab:     point to source table
* @param paramCnt:    param count to be calculated
* @param triggerVal:  trigger gain / lux etc..
* @param needInterp:  interpolate flag
*
* @return void
*
*/
void Calculate_Dynamic_Parameters(
    u32*                pDst,
    tune_trigger_tab_t* pSrcTab,
    u32                 paramCnt,
    u32                 triggerVal,
    BOOL                needInterp);


/*
* ASC_Sort
*
* @brief ASC sort function
*
* @param pAscSortGain:  point to source array
* @param validNum:      array num
*
* @return void
*
*/
void ASC_Sort(u32 * pAscSortGain, u32 validNum);


/*
* Get_WBGain_Dist_Interp
*
* @brief WB Gain Dist interpolate function
*
* @return interpolated value
*
*/
u32 Get_WBGain_Dist_Interp(
    u32 uiRGRatioActual,
    u32 uiBGRatioActual,
    u32 *pIndexL,
    u32 *pIndexH,
    u32 *pAscRGPos,
    u32 *pAscBGPos,
    u32 *pCalibrationRGPos,
    u32 *pCalibrationBGPos,
    u32 uiValidCTNum);


/*
* Step_Converge
*
* @brief use step and stableRange get smooth result for smooth converge
*
* @param step: smooth step, the smaller the faster converge
* @param stableRange: less stableRange then set to previous
* @param previous: smooth start value
* @param target: smooth target value

* @return smooth value
*
*/
u32 Step_Converge(u32 step, u32 stableRange, u32 previous, u32 target);

#ifndef ABS_DIFF
#define ABS_DIFF(a, b) (((a) >= (b)) ? ((a) - (b)) : ((b) - (a)))
#endif

#endif
