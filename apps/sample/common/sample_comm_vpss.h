

#ifndef __SAMPLE_COMM_VPSS_H__
#define __SAMPLE_COMM_VPSS_H__

#include <pthread.h>

#include "ts_common.h"
#include "ts_comm_vpss.h"
#include "sample_comm_sys.h"
#include "mpi_vpss.h"

/*******************************************************
 *   function announce
 *******************************************************/

TS_S32 SAMPLE_COMM_VPSS_Start(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S* pstVpssGrpAttr, VPSS_CHN_ATTR_S* pastVpssChnAttr, TS_S32 chnlNum);
TS_S32 SAMPLE_COMM_VPSS_Stop(VPSS_GRP VpssGrp, TS_S32 chnlNum);

TS_S32 SAMPLE_COMM_VPSS_SetFlip(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, TS_U32 bFlip);
TS_S32 SAMPLE_COMM_VPSS_SetMirror(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, TS_U32 bMirror);
TS_S32 SAMPLE_COMM_VPSS_SetFlipMirror(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, TS_U32 bFlip, TS_U32 bMirror);
TS_S32 SAMPLE_COMM_VPSS_SetFps(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, TS_U32 dFps,TS_U32 sFps);

TS_S32 SAMPLE_COMM_VPSS_DumpYuv(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
TS_S32 SAMPLE_COMM_VPSS_SwitchResolution(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, TS_U32 width, TS_U32 height);
TS_S32 SAMPLE_COMM_VPSS_ChnCrop(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, TS_S32 x, TS_S32 y, TS_U32 width, TS_U32 height);
TS_S32 SAMPLE_COMM_VPSS_GrpCrop(VPSS_GRP VpssGrp, TS_S32 x, TS_S32 y, TS_U32 width, TS_U32 height);

#endif /* End of #ifndef __SAMPLE_COMMON_H__ */
