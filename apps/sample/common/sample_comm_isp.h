

#ifndef __SAMPLE_COMM_ISP_H__
#define __SAMPLE_COMM_ISP_H__

#include <pthread.h>

#include "ts_comm_3a.h"
#include "ts_comm_isp.h"

#include "mpi_ae.h"
#include "mpi_awb.h"
#include "mpi_isp.h"

typedef enum{
	SAMPLE_LIGHT_MODE_NONE = 0,
	SAMPLE_LIGHT_MODE_INFRARED_LAMP,
	SAMPLE_LIGHT_MODE_WHITE_LIGHT,
	SAMPLE_LIGHT_MODE_BUTT
}SAMPLE_LIGHT_MODE_E;

typedef enum{
	SAMPLE_DN_SWITCH_BY_SOFTWARE = 0,
	SAMPLE_DN_SWITCH_BY_HARDWARE,
	SAMPLE_DN_SWITCH_BY_BUTT
}SAMPLE_DN_SWITCH_MODE_E;

typedef struct{
	SAMPLE_LIGHT_MODE_E	light_mode;
	SAMPLE_DN_SWITCH_MODE_E	judge_mode;

	/* blow is used only for SAMPLE_DN_SWITCH_BY_HARDWARE */
	TS_BOOL	adc_increase_by_luma;
	TS_U32	night_threadhold;
	TS_U32	day_threadhold;
	TS_U32  adc_num;
}SAMPLE_DN_SWITCH_CONTROL_S;

TS_VOID SAMPLE_COMM_ISP_Stop(ISP_DEV IspDev);
TS_VOID SAMPLE_COMM_All_ISP_Stop(void);
TS_S32 SAMPLE_COMM_ISP_Run(ISP_DEV IspDev);
TS_S32 SAMPLE_COMM_ISP_GetVDTimeout(VI_PIPE ViPipe, ISP_VD_TYPE_E enIspVDType, TS_U32 u32MilliSec);
TS_S32 SAMPLE_COMM_ISP_SetParamIndex(VI_PIPE ViPipe, TS_U32 paramSetIndex);
TS_S32 SAMPLE_COMM_ISP_GetParamIndex(VI_PIPE ViPipe, TS_U32 *paramSetIndex);

TS_S32 SAMPLE_COMM_ISP_Start_DN_Switch(SAMPLE_DN_SWITCH_CONTROL_S * pDN_control);
TS_S32 SAMPLE_COMM_ISP_Stop_DN_Switch();

#endif /* End of #ifndef __SAMPLE_COMMON_H__ */
