

#ifndef __SAMPLE_COMM_DIS_H__
#define __SAMPLE_COMM_DIS_H__

#include <pthread.h>

#include "sample_comm_base.h"
#include "sample_comm_vi.h"
#include "sample_comm_vpss.h"

#include "ts_comm_sys.h"

#include "mpi_vi.h"

typedef struct tsSAMPLE_DIS_CONFIG_S {
	GDC_CONFIG_S     	pstLdcInfo[VI_MAX_PHY_PIPE_NUM];
	TS_S32              s32WorkingViNum;
} SAMPLE_DIS_CONFIG_S;

/*******************************************************
 *   function announce
 *******************************************************/

TS_S32 SAMPLE_COMM_DIS_StartVi(SAMPLE_VI_CONFIG_S *pstViConfig, SAMPLE_DIS_CONFIG_S *pstLdcInfo);
TS_S32 SAMPLE_COMM_DIS_StopVi(SAMPLE_VI_CONFIG_S *pstViConfig);

TS_S32 SAMPLE_COMM_DIS_StartVpss(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S* pstVpssGrpAttr, VPSS_CHN_ATTR_S* pastVpssChnAttr, TS_S32 chnlNum, SAMPLE_DIS_CONFIG_S *stLdcInfo);
TS_S32 SAMPLE_COMM_DIS_StopVpss(VPSS_GRP VpssGrp, TS_S32 chnlNum);


#endif /* End of #ifndef __SAMPLE_COMMON_H__ */
