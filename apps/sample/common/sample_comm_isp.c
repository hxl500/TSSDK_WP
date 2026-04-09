
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>
#include "sample_comm_util.h"
#include "sample_comm_isp.h"

#define ABS(x)          ( (x) >= 0 ? (x) : (-(x)) )
static int g_awb_err_cnt = 0;
/******************************************************************************
* funciton : ISP Run
******************************************************************************/
TS_S32 SAMPLE_COMM_ISP_Run(ISP_DEV IspDev)
{
    TS_S32 s32Ret = 0;
    return s32Ret;
}

/******************************************************************************
* funciton : stop ISP, and stop isp thread
******************************************************************************/
TS_VOID SAMPLE_COMM_ISP_Stop(ISP_DEV IspDev)
{
#if 1
	TS_MPI_ISP_Exit(IspDev);
#else
    if (g_IspPid[IspDev])
    {
        TS_MPI_ISP_Exit(IspDev);
        pthread_join(g_IspPid[IspDev], NULL);
        SAMPLE_COMM_ISP_Awblib_UnCallback(IspDev);
        SAMPLE_COMM_ISP_Aelib_UnCallback(IspDev);
        SAMPLE_COMM_ISP_Sensor_UnRegiter_callback(IspDev);
        g_IspPid[IspDev] = 0;
    }
#endif
    return;
}

int FileExist(const char *name){
    if(access("name",F_OK) == 0){
        return 1;
    }else {
        return 0;
    }
}

TS_VOID SAMPLE_COMM_All_ISP_Stop(TS_VOID)
{
    ISP_DEV IspDev;

    for (IspDev = 0; IspDev < ISP_MAX_PIPE_NUM; IspDev++)
    {
        SAMPLE_COMM_ISP_Stop(IspDev);
    }
}

TS_S32 SAMPLE_COMM_ISP_GetVDTimeout(VI_PIPE ViPipe, ISP_VD_TYPE_E enIspVDType, TS_U32 u32MilliSec)
{
	return TS_MPI_ISP_GetVDTimeOut(ViPipe, enIspVDType, u32MilliSec);
}

TS_S32 SAMPLE_COMM_ISP_SetParamIndex(VI_PIPE ViPipe, TS_U32 paramSetIndex)
{
	return TS_MPI_ISP_SetParamIndex(ViPipe, paramSetIndex);
}

TS_S32 SAMPLE_COMM_ISP_GetParamIndex(VI_PIPE ViPipe, TS_U32 *paramSetIndex)
{
	return TS_MPI_ISP_GetParamIndex(ViPipe, paramSetIndex);
}

typedef struct Sensor_Calib_PARAM_S {
	float PureIRBG_Ratio;
	float BGDiff;
	TS_U64 Day2NightThresh;
	TS_U64 Night2DayThresh;

	float PureIRRG; //RG in Pure IR light
	float PureIRBG;
	float PureNaLightRG; //RG in Nature Light
	float PureNaLightBG;
	float RG_BG_Weight; //set R/G weight, and then BG wieght  = 1.0 - RG_BG_Weg
	float IR_RatioThr;
	float IR_RatioToler;
	TS_U16 ExpRatio;
	float   IR_DiffThr; //IR_DiffThr is used to avoid the situation where the rg/bg of the night scene changes nothing and immediately switches back to the daytime.
	float   IR_DiffThr2; //IR_DiffThr is used to avoid the situation where the rg/bg of the night scene changes nothing and immediately switches back to the daytime.
	float MixLightRG; //MixLight rg thresh
}TS_SenCalib_PARAM_S;

typedef enum tsMPI_ISP_DAYNIGHT_MODE_E {
    TS_MPI_MODE_DAY = 0,
    TS_MPI_MODE_IR_NIGHT,
    TS_MPI_MODE_WL_NIGHT,
    TS_MPI_MODE_QRCODE,

    TS_MPI_MODE_MAX
}TS_MPI_DAYNIGHT_MODE_E;

static int LightType = 0;    //0 is IR, 1 is white light
static int last_daymode = TS_MPI_MODE_DAY;  // 0 is day, 1 is IR night, 2 is white light night
static int WL_first = 5;
static int WL_base = 0;
static int WL_long_time_data[10] = {0};
static int wl_time_cnt = 0;
static int wl_pos = 0;
static int wl_before_switch = 0;
static int change_cnt = 0;
static pthread_t gs_isp_DN_switch_thread_id;
static int g_sample_comm_isp_terminal;

// old algo, not used
// int DN_switch_with_infrared_lamp(ISP_DEV ispDev, TS_SenCalib_PARAM_S *stCalibParam, TS_MPI_DAYNIGHT_MODE_E *pstdymode)
// {
//     ISP_EXP_INFO_S stExpInfo;
//     TS_MPI_ISP_QueryExposureInfo(ispDev, &stExpInfo);
//     int exposure = stExpInfo.u32TotalGain *stExpInfo.u32ExpTime;
//     ISP_WB_INFO_S stWBInfo;
//     TS_MPI_ISP_QueryWBInfo(ispDev, &stWBInfo);

//     ISP_WB_STATISTICS_S awb_stats;
//     int Zonesum[2] = {0};
//     int i = 0;
//     float visRGBGDiff = 0.57, IRRGBGDiff = -0.008, ratio = 1.0;

//     TS_MPI_ISP_GetWBStatistics(ispDev, &awb_stats);

//     int total_size = 15 * 15;
//     int blkCnt = 0;

//     for (; i < total_size; i++)
//     {
//         if (0 != awb_stats.usAWBZonePixelsCounted[i])
//         {
//             Zonesum[0] += awb_stats.ullAWBZoneRGRatioSum[i] / awb_stats.usAWBZonePixelsCounted[i] / stWBInfo.awbRGain * 256;
//             Zonesum[1] += awb_stats.ullAWBZoneBGRatioSum[i] / awb_stats.usAWBZonePixelsCounted[i] / stWBInfo.awbBGain * 256;
//             blkCnt += 1;
//         }
//     }
//     if (0 == blkCnt)
//     {
//         SAMPLE_PRT("Wrong AWB stats data !. \n");
//         *pstdymode = TS_MPI_MODE_IR_NIGHT;
//         last_daymode = *pstdymode;
//         return TS_FAILURE;
//     }

//     if (TS_MPI_MODE_DAY == last_daymode)
//     {
// 		SAMPLE_PRT("WQ@@@@ Day case  exposure: %d, Day2NightThresh: %lld, change_cnt: %d\n", exposure, stCalibParam->Day2NightThresh, change_cnt);
//         if (exposure > stCalibParam->Day2NightThresh)
//         {
//             *pstdymode = TS_MPI_MODE_IR_NIGHT;
//         }
//         else
//         {
//             *pstdymode = TS_MPI_MODE_DAY;
//         }
//     }
//     else
//     {
//         float real_BG = (float)Zonesum[1] / 4096 / blkCnt, real_RG = (float)Zonesum[0] / 4096 / blkCnt;
//         real_BG = 1 / real_BG;
//         real_RG = 1 / real_RG;
//         float abs_diff = real_BG > real_RG ? real_BG - real_RG : real_RG - real_BG;
//         float ir_ratio = (abs_diff - ratio * visRGBGDiff)/(ratio * (IRRGBGDiff - visRGBGDiff));
// 		SAMPLE_PRT("WQ@@@@ night case  exposure: %d, Night2DayThresh: %lld, change_cnt: %d, real_BG: %f, real_RG: %f,abs_diff: %f, ir_ratio: %f\n", exposure, stCalibParam->Night2DayThresh, change_cnt,
// 			real_BG,real_RG,abs_diff,ir_ratio);
//         if (ir_ratio < 0)
//         {
//             ir_ratio = 0;
//         }

//         /* avoid strong reflection error */
//         if (ir_ratio >= 0.92)
//         { 
// 		    SAMPLE_PRT("night1\n");
//             *pstdymode = TS_MPI_MODE_IR_NIGHT;
//         }
//         else if (exposure< stCalibParam->Night2DayThresh)
//         {
// 			SAMPLE_PRT("day2\n");
//             *pstdymode = TS_MPI_MODE_DAY;
//         }
//         else
//         {   
// 			SAMPLE_PRT("night3\n");
//             *pstdymode = TS_MPI_MODE_IR_NIGHT;
//         }
//     }

//     if (last_daymode != *pstdymode)
//     {
//         change_cnt++;
//     }
//     if (CHANGE_THRESH == change_cnt)
//     {
// 		SAMPLE_PRT("day4\n");
//         last_daymode = *pstdymode;
//         change_cnt = 0;
//     }

//     return TS_SUCCESS;
// }

int TS_MPI_IR_DN_Switch_Advance(ISP_DEV ispDev, TS_SenCalib_PARAM_S *stCalibParam, TS_MPI_DAYNIGHT_MODE_E *pstdymode)
{
#define FILE_NAME "/tmp/IR_LOG"
#define CHANGE_THRESH 5

	ISP_EXP_INFO_S stExpInfo;
	TS_MPI_ISP_QueryExposureInfo(ispDev, &stExpInfo);
	TS_U64 cur_exp = stExpInfo.u32TotalGain * stExpInfo.u32ExpTime;
	ISP_WB_INFO_S stWBInfo;
	TS_MPI_ISP_QueryWBInfo(ispDev, &stWBInfo);
	ISP_WB_STATISTICS_S awb_stats;
	TS_U64 Zonesum[2] = { 0 };
	int i = 0;
    int ir_log = 0;

    if(FileExist(FILE_NAME)){
        ir_log = 1;
    }

	TS_MPI_ISP_GetWBStatistics(ispDev, &awb_stats);
	int total_size = 15 * 15;
	int blkCnt = 0;
	float cur_rg = 0;
	float cur_bg = 0;
	double rg_tmp = 0;
	double bg_tmp = 0;
	double diff = 0;

	for (i = 0; i < total_size; i++) {
		if (awb_stats.usAWBZonePixelsCounted[i] >= 4000) {
			Zonesum[0] = Zonesum[0] + (awb_stats.ullAWBZoneRGRatioSum[i] /
						   awb_stats.usAWBZonePixelsCounted[i] * 256 / stWBInfo.awbRGain);
			Zonesum[1] = Zonesum[1] + (awb_stats.ullAWBZoneBGRatioSum[i] /
						   awb_stats.usAWBZonePixelsCounted[i] * 256 / stWBInfo.awbBGain);

			blkCnt++;
		}
	}

	if (0 == blkCnt) {
		SAMPLE_PRT("Wrong AWB stats data !. \n");
		*pstdymode = TS_MPI_MODE_IR_NIGHT;
		last_daymode = *pstdymode;
		g_awb_err_cnt++;
		return TS_FAILURE;
	} else {
		g_awb_err_cnt = 0;
		SAMPLE_PRT("Normal AWB stats data, blkCnt = %d\n", blkCnt);
	}

	cur_rg = (float)Zonesum[0] / 4096 / blkCnt;
	cur_bg = (float)Zonesum[1] / 4096 / blkCnt;

	if (TS_MPI_MODE_DAY == last_daymode) {
        if(1 == ir_log){
            SAMPLE_PRT("DayCase cur_exp: %lld, D2N_Thr: %lld, cur_rg: %f, cur_bg: %f, chg_cnt: %d\n",
                cur_exp, stCalibParam->Day2NightThresh, cur_rg, cur_bg, change_cnt);
        }
        if(1 == ir_log){
            SAMPLE_PRT("DayCase cur_exp: %lld, D2N_Thr: %lld, cur_rg: %f, cur_bg: %f, chg_cnt: %d\n",
                cur_exp, stCalibParam->Day2NightThresh, cur_rg, cur_bg, change_cnt);

        }
		if (cur_exp > stCalibParam->Day2NightThresh) {
			*pstdymode = TS_MPI_MODE_IR_NIGHT;
            if(1 == ir_log){
                SAMPLE_PRT(" ==Day To Night 1 ==\n");
            }
		} else {
			*pstdymode = TS_MPI_MODE_DAY;
            if(1 == ir_log){
                SAMPLE_PRT(" ==Day To Day 1 ==\n");
            }
		}
	} else {
        if(1 == ir_log){
            SAMPLE_PRT("NightCase cur_exp: %lld, N2D_Thr: %lld, cur_rg: %f, cur_bg: %f, chg_cnt:%d,PureIRRG: %f, PureIRBG: %f, PureNaLightRG: %f, PureNaLightBG: %f,  IR_DiffThr: %f,  IR_DiffThr2: %f, ExpRatio:%d\n",
                cur_exp, stCalibParam->Night2DayThresh, cur_rg, cur_bg, change_cnt,stCalibParam->PureIRRG, stCalibParam->PureIRBG,stCalibParam->PureNaLightRG, stCalibParam->PureNaLightBG,stCalibParam->IR_DiffThr,stCalibParam->IR_DiffThr2,stCalibParam->ExpRatio);
        }
		if (cur_exp < stCalibParam->Night2DayThresh) {
			if (cur_rg <= stCalibParam->PureNaLightRG || cur_bg <= stCalibParam->PureNaLightBG) {
				*pstdymode = TS_MPI_MODE_DAY;
                if(i == ir_log){
                    SAMPLE_PRT(" == Night To DAY 1: cur_rg <= PureNaLightRG || cur_bg <= PureNaLightBG\n");
                }
			} else {
				rg_tmp = cur_rg - stCalibParam->PureNaLightRG;
				rg_tmp = rg_tmp / (stCalibParam->PureIRRG - stCalibParam->PureNaLightRG);

				bg_tmp = cur_bg - stCalibParam->PureNaLightBG;
				bg_tmp = bg_tmp / (stCalibParam->PureIRBG - stCalibParam->PureNaLightBG);
				
				diff = ABS(rg_tmp - bg_tmp);
                if(1 == ir_log){
                    SAMPLE_PRT("NightCase cur_exp:%lld, diff: %lf, rg_tmp: %lf, bg_tmp: %lf, PureIRRG: %f, PureIRBG: %f, PureNaLightRG: %f, PureNaLightBG: %f\n", 
                        cur_exp, diff, rg_tmp, bg_tmp, stCalibParam->PureIRRG, stCalibParam->PureIRBG,
                        stCalibParam->PureNaLightRG, stCalibParam->PureNaLightBG);
                }
				if (diff >= stCalibParam->IR_DiffThr2) {
					//day scene
					*pstdymode = TS_MPI_MODE_DAY;
                    if(1 == ir_log){
                        SAMPLE_PRT(" == Night To Day 2: diff:%f >= IR_DiffThr2:%f ==\n", diff, stCalibParam->IR_DiffThr2);
                    }
				} else if (diff <= stCalibParam->IR_DiffThr) {
					//ir scene
					*pstdymode = TS_MPI_MODE_IR_NIGHT;
                    if(1 == ir_log){
                        SAMPLE_PRT(" == Night To Night 1:diff:%f <= IR_DiffThr:%f ==\n", diff, stCalibParam->IR_DiffThr);
                    }
				} else {
					//mix lightSource
					if (cur_exp < (stCalibParam->Night2DayThresh / stCalibParam->ExpRatio)) {
						*pstdymode = TS_MPI_MODE_DAY;
                        if(1 == ir_log){
                            SAMPLE_PRT(" == DAY:cur_exp:%lld < (thr/%d):%lld ==\n", cur_exp, stCalibParam->ExpRatio, (stCalibParam->Night2DayThresh / stCalibParam->ExpRatio));
						    SAMPLE_PRT(" == DAY:IR_DiffThr:%f < diff:%f < IR_DiffThr2:%f ==\n", stCalibParam->IR_DiffThr, diff, stCalibParam->IR_DiffThr2);
                        }
					}
				}					
			}
		} else {
			*pstdymode = TS_MPI_MODE_IR_NIGHT;
            if(1 == ir_log){
                SAMPLE_PRT(" == Night:[cur_exp:%lld > stCalibParam->Night2DayThresh:%lld]==\n", cur_exp, stCalibParam->Night2DayThresh);
            }
		}
	}

	if (last_daymode != *pstdymode) {
		change_cnt++;
	} else {
		change_cnt = 0;
	}

	if (CHANGE_THRESH == change_cnt) {
		last_daymode = *pstdymode;
		change_cnt = 0;
	}
    if(1 == ir_log){
        SAMPLE_PRT("u32ExpTime: %d, u32TotalGain: %d, dymode: %d, change_cnt: %d\n", stExpInfo.u32ExpTime,
            stExpInfo.u32TotalGain, *pstdymode, change_cnt);
    }

	return TS_SUCCESS;
}
int DN_switch_with_white_light(ISP_DEV ispDev, TS_SenCalib_PARAM_S *stCalibParam, TS_MPI_DAYNIGHT_MODE_E *pstdymode)
{
    ISP_EXP_INFO_S stExpInfo;
    TS_MPI_ISP_QueryExposureInfo(ispDev, &stExpInfo);
    int exposure = stExpInfo.u32TotalGain * stExpInfo.u32ExpTime;
    int exposed = 0, i = 1023;

    if (TS_MPI_MODE_DAY == last_daymode)
    {
        if (exposure > stCalibParam->Day2NightThresh)
        {
            *pstdymode = TS_MPI_MODE_WL_NIGHT;
        }
        else
        {
            *pstdymode = TS_MPI_MODE_DAY;
        }
    }
    else
    {
            if (0 == WL_first)
            {
                WL_base = exposure;
                WL_first++;
                wl_time_cnt = 0;
                return TS_SUCCESS;
            }
            else if(WL_first < 4)
            {
                WL_base = WL_base * 0.2 + 0.8 * (exposure);
                WL_first++;
                return TS_SUCCESS;
            }
            else
            {
                //if enviroment is too dark,use predefined base.
                if (WL_base >= 35860540)
                {
                    WL_base = 35860540;
                }
            }
        //calc exposed area percentage
        ISP_AE_STATISTICS_S stAEStat;
        TS_MPI_ISP_GetAEStatistics(ispDev, &stAEStat);
        for (; i > 992; i--)
        {
            exposed += stAEStat.ae0Hist[i];
        }

        exposed = exposed * 100 / 1843200;  // 1843200 only works for 4M sensor
        if (exposed > 10)
        {
            int thresh = WL_base * 85 / 100;
            if (exposure < thresh)
            {
                *pstdymode = TS_MPI_MODE_DAY;
            }
            else
            {
                *pstdymode = TS_MPI_MODE_WL_NIGHT;
            }
        }
        else
        {
            int diff = WL_base - exposure;
            if (diff > 15000000)
            {
                *pstdymode = TS_MPI_MODE_DAY;
            }
            else if(WL_base <= 6000000)
            {
                if (exposure < WL_base * 0.8)
                {
                    *pstdymode = TS_MPI_MODE_DAY;
                }
                else
                {
                    *pstdymode = TS_MPI_MODE_WL_NIGHT;
                }
            }
            else
            {
                *pstdymode = TS_MPI_MODE_WL_NIGHT;
            }
        }
    }

    if (1200 == wl_time_cnt)
    {
        WL_long_time_data[wl_pos] = exposure;
        wl_pos++;
        if (10 == wl_pos)
        {
            wl_pos = 0;
        }
        wl_time_cnt = 0;
    }

    if (last_daymode != *pstdymode)
    {
        change_cnt++;
    }
    if (CHANGE_THRESH == change_cnt)
    {
        last_daymode = *pstdymode;
        change_cnt = 0;
    }

    return TS_SUCCESS;
}

TS_S32 hardware_ircut_all_close()
{
	system("/root/hardware_ircut_all_close.sh");
	return TS_SUCCESS;
}

TS_S32 hardware_ircut_day_open()
{
	system("/root/hardware_ircut_day_open.sh");
	return TS_SUCCESS;
}

TS_S32 hardware_ircut_night_open()
{
	system("/root/hardware_ircut_night_open.sh");
	return TS_SUCCESS;
}

TS_S32 hardware_red_open()
{
	system("/root/hardware_red_open.sh");
	return TS_SUCCESS;
}

TS_S32 hardware_red_close()
{
	system("/root/hardware_red_close.sh");
	return TS_SUCCESS;
}

TS_S32 hardware_white_open()
{
	system("/root/hardware_white_open.sh");
	return TS_SUCCESS;
}

TS_S32 hardware_white_close()
{
	system("/root/hardware_white_close.sh");
	return TS_SUCCESS;
}

#define SUPPORT_IR_AUTO 1

TS_VOID* DN_switch_by_software()
{
    ISP_DEV ispDev = 0;
    TS_MPI_DAYNIGHT_MODE_E pstdymode;
    TS_SenCalib_PARAM_S stCalibParam = {0};
    static TS_BOOL DayToggle_Flag   = TS_FALSE;
    static TS_BOOL NightToggle_Flag = TS_FALSE;
#if SUPPORT_IR_AUTO
    ISP_IR_AUTO_ATTR_S ir_auto_attr = {
    .Day2NightThresh = 1000000,
    .IrNight2DayThresh = 100000,
    .WlNight2DayThresh = 100000,
    .RgMax = 280,
    .RgMin = 240,
    .BgMax = 280,
    .BgMin = 240,
    };
#else
	#ifdef GC4023
	    stCalibParam.PureNaLightRG = 0.7319;
	    stCalibParam.PureNaLightBG = 0.7164;
	    stCalibParam.RG_BG_Weight = 0.6;
	    stCalibParam.IR_RatioThr = 0.84;
	    stCalibParam.ExpRatio = 6;
	    stCalibParam.IR_RatioToler = 1.04;
	    stCalibParam.Day2NightThresh = 7736 * 17240 ; //7736 * 25901;//7736 * 31072;// * 1.2; //7736 * 2900;  
	    stCalibParam.Night2DayThresh = 7736 * 12384 ;//* 0.8; //2398 * 1024;  //7736 * 1600;
        stCalibParam.BGDiff = 0.5;
	    stCalibParam.PureIRRG = 1.0498;
	    stCalibParam.PureIRBG = 0.9810;
	    stCalibParam.IR_DiffThr = 0.025;
	    stCalibParam.IR_DiffThr2 = 0.4;
	#else
	    stCalibParam.PureNaLightRG = 0.7319;
	    stCalibParam.PureNaLightBG = 0.7064;
	    stCalibParam.RG_BG_Weight = 0.6;
	    stCalibParam.IR_RatioThr = 0.84;
	    stCalibParam.ExpRatio = 32;
	    stCalibParam.IR_RatioToler = 1.04;
	    stCalibParam.Day2NightThresh = 7736 * 17240 ; //7736 * 25901;//7736 * 31072;// * 1.2; //7736 * 2900;  
	    stCalibParam.Night2DayThresh = 7736 * 9010 ;//* 0.8; //2398 * 1024;  //7736 * 1600;
        stCalibParam.BGDiff = 0.5;
	    stCalibParam.PureIRRG = 1.0498;
	    stCalibParam.PureIRBG = 0.9810;
	    stCalibParam.IR_DiffThr = 0.025;
	    stCalibParam.IR_DiffThr2 = 0.4;
	#endif 
#endif
    sleep(5);

	g_sample_comm_isp_terminal = 0;

    while(0 == g_sample_comm_isp_terminal)
    {
#if SUPPORT_IR_AUTO
    TS_MPT_ISP_IrAuto(0, &ir_auto_attr);
        if (ir_auto_attr.IR_switch != ISP_IR_SWITCH_TO_NONE) {
            if (ir_auto_attr.IR_switch == ISP_IR_SWITCH_TO_DAY) {
                last_daymode = TS_MPI_MODE_DAY;
            } else if (ir_auto_attr.IR_switch == ISP_IR_SWITCH_TO_NIGHT) {
                last_daymode = TS_MPI_MODE_IR_NIGHT;
            }
        }
#else
        if (0 == LightType)
        {
            TS_MPI_IR_DN_Switch_Advance(ispDev, &stCalibParam, &pstdymode);
        }
        else if (1 == LightType)
        {
            DN_switch_with_white_light(ispDev, &stCalibParam, &pstdymode);
        }
        else
        {
            SAMPLE_PRT("Wrong Light Type\n");
        }
#endif

        if (last_daymode == TS_MPI_MODE_DAY && DayToggle_Flag == TS_FALSE) {
            DayToggle_Flag   = TS_TRUE;
            NightToggle_Flag = TS_FALSE;
            hardware_ircut_all_close();
			SAMPLE_COMM_ISP_SetParamIndex(ispDev, 0);
			hardware_ircut_day_open();
			hardware_white_close();
			hardware_red_close();
        } else if (last_daymode == TS_MPI_MODE_IR_NIGHT && NightToggle_Flag == TS_FALSE) {
            NightToggle_Flag = TS_TRUE;
            DayToggle_Flag   = TS_FALSE;
            hardware_ircut_all_close();
			SAMPLE_COMM_ISP_SetParamIndex(ispDev, 1);
			hardware_ircut_night_open();
			hardware_white_close();
			hardware_red_open();
        }
        else if (last_daymode == TS_MPI_MODE_WL_NIGHT && NightToggle_Flag == TS_FALSE) {
            ISP_EXP_INFO_S stExpInfo;
            TS_MPI_ISP_QueryExposureInfo(0, &stExpInfo);
            wl_before_switch = stExpInfo.u32TotalGain * stExpInfo.u32ExpTime;
            NightToggle_Flag = TS_TRUE;
            DayToggle_Flag   = TS_FALSE;
            hardware_ircut_all_close();
			hardware_red_close();
			hardware_ircut_day_open();
			SAMPLE_COMM_ISP_SetParamIndex(ispDev, 0);
			sleep(1);
			hardware_white_open();
			SAMPLE_COMM_ISP_SetParamIndex(ispDev, 2);
            WL_first = 0;
        }
        usleep(1000 * 3000);
    }

	return TS_NULL;
}
#define ADC_READ_FILE0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define ADC_READ_FILE1 "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"

#define DAY_THRESHOLD 500
#define NIGHT_THRESHOLD 50
#define ADC_READ_NUM 30
#define ADC_READ_INTERVAL_MS 100
#define IR_CUT_MOTOR_AIN_PIN "458"
#define IR_CUT_MOTOR_BIN_PIN "459"

int get_ADC_value(int    *buff, int size, char * adc_device)
{
    FILE *fd = NULL ;
    int ret  = 0;
    char temPbuff[10] = {0};
    while(--size >= 0 ) {
        buff[size] = 0;
        memset(temPbuff, 0, 10);
        fd = fopen(adc_device, "r");
        if (fd < 0) {
            SAMPLE_PRT("Can't open file %s", adc_device);
            continue;
        }
        if(fgets(temPbuff, sizeof(temPbuff), fd) == NULL) {
            SAMPLE_PRT("read adc value failed.");
            fclose(fd);
            continue;
        }
//        SAMPLE_PRT("temPbuff :%s", temPbuff);
        fclose(fd);
        buff[size] = atoi(temPbuff); //rx packets
//        SAMPLE_PRT("buff[%d]:%d", size - 1, buff[size - 1]);
        usleep(ADC_READ_INTERVAL_MS * 1000);
    }
    return ret;
}

int smoothing(int *buf, int size)
{
    int voltage = 0;
    int sub[ADC_READ_NUM] = {0};
    int i, j;
    for(i = 0; i < size; i++) {
        int sum = 0;
        for(j = 0; j < size; j++) {
            sum = sum + (buf[i] > buf[j] ? buf[i] - buf[j] : buf[j] - buf[i]);
        }
        sub[i] = sum;
    }
    int min = sub[0];
    voltage = buf[0];
    for(i = 0; i < size; i++) {
        if(sub[i] < min) {
            min = sub[i];
            voltage = buf[i];
        }
    }
    return voltage;
}


TS_VOID* DN_switch_by_hardware(TS_VOID * pParam)
{
	SAMPLE_DN_SWITCH_CONTROL_S * pDN_control = (SAMPLE_DN_SWITCH_CONTROL_S *)pParam;
	ISP_DEV ispDev = 0;
    int adcVoltage = DAY_THRESHOLD;
    int buff0[ADC_READ_NUM] = {0};
	int last_daynight = -1; /* 0:day   1:night*/

    sleep(5);

	g_sample_comm_isp_terminal = 0;
	SAMPLE_PRT("enter main loop\n");
    while(0 == g_sample_comm_isp_terminal)
    {
		int get_day = 0;
		int get_night = 0;

		if(pDN_control->adc_num == 0){
			/*1. get adc value***********/
	        get_ADC_value(buff0, ADC_READ_NUM, ADC_READ_FILE0);
	        /*2. smooth***********/
	        adcVoltage = smoothing(buff0, ADC_READ_NUM);
	        SAMPLE_PRT("adcValue=%d, inc=%d, dayThrd=%d, nigthThrd=%d\n", adcVoltage,
				pDN_control->adc_increase_by_luma,
				pDN_control->day_threadhold, pDN_control->night_threadhold);
		}else if(pDN_control->adc_num == 1){
			/*1. get adc value***********/
	        get_ADC_value(buff0, ADC_READ_NUM, ADC_READ_FILE1);
	        /*2. smooth***********/
	        adcVoltage = smoothing(buff0, ADC_READ_NUM);
	        SAMPLE_PRT("adcValue=%d, inc=%d, dayThrd=%d, nigthThrd=%d\n", adcVoltage,
				pDN_control->adc_increase_by_luma,
				pDN_control->day_threadhold, pDN_control->night_threadhold);
		}


		if(TS_TRUE == pDN_control->adc_increase_by_luma){
			if(adcVoltage >= pDN_control->day_threadhold){
				get_day = 1;
			}
			else if(adcVoltage <= pDN_control->night_threadhold){
				get_night = 1;
			}
		}
		else{
			if(adcVoltage >= pDN_control->night_threadhold){
				get_night = 1;
			}
			else if(adcVoltage <= pDN_control->day_threadhold){
				get_day = 1;
			}
		}

        /*3. estimate day or night***********/
		if(get_night && (last_daynight != 1)) {
			SAMPLE_PRT("is going to set night mode\n");
			hardware_ircut_all_close();
			SAMPLE_COMM_ISP_SetParamIndex(ispDev, 1);
			hardware_ircut_night_open();
			hardware_white_close();
			hardware_red_close();

			last_daynight = 1;
		} else if(get_day && (last_daynight != 0)) {
			SAMPLE_PRT("is going to set day mode\n");
			hardware_ircut_all_close();
			SAMPLE_COMM_ISP_SetParamIndex(ispDev, 0);
			hardware_ircut_day_open();
			hardware_white_close();
			hardware_red_open();

			last_daynight = 0;
		}

        usleep(1000 * 1000);
    }
	SAMPLE_PRT("exit main loop\n");

	return TS_NULL;
}

TS_S32 SAMPLE_COMM_ISP_Start_DN_Switch(SAMPLE_DN_SWITCH_CONTROL_S * pDN_control)
{
	if(!pDN_control){
		SAMPLE_PRT("NULL ptr\n");
		return TS_FAILURE;
	}

	if(gs_isp_DN_switch_thread_id > 0){
		SAMPLE_PRT("DN switch thread is already working\n");
		return TS_FAILURE;
	}

	if(SAMPLE_DN_SWITCH_BY_SOFTWARE == pDN_control->judge_mode){
		if(SAMPLE_LIGHT_MODE_INFRARED_LAMP == pDN_control->light_mode){
			LightType = 0;
		}
		else if(SAMPLE_LIGHT_MODE_WHITE_LIGHT == pDN_control->light_mode){
			LightType = 1;
		}
		else{
			SAMPLE_PRT("do NOTsupport this light mode, %d\n", pDN_control->light_mode);
			return TS_FAILURE;
		}

		pthread_create(&gs_isp_DN_switch_thread_id, 0, DN_switch_by_software, 0);
		SAMPLE_PRT("create thread DN_switch_by_software success\n");
	}
	else if(SAMPLE_DN_SWITCH_BY_HARDWARE == pDN_control->judge_mode){
		pthread_create(&gs_isp_DN_switch_thread_id, 0, DN_switch_by_hardware, (TS_VOID *)pDN_control);
		SAMPLE_PRT("create thread DN_switch_by_hardware success\n");
	}

	return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_ISP_Stop_DN_Switch()
{
	g_sample_comm_isp_terminal = 1;
	if(gs_isp_DN_switch_thread_id > 0){
		pthread_join(gs_isp_DN_switch_thread_id, 0);
		gs_isp_DN_switch_thread_id = 0;
	}

	return TS_SUCCESS;
}


