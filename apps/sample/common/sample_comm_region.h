#ifndef __SAMPLE_COMM_REGION_H__
#define __SAMPLE_COMM_REGION_H__

#include <pthread.h>
#include <stdbool.h>

#include "ts_comm_region.h"
#include "mpi_region.h"
#include "sample_comm_util.h"
#include "sample_comm_venc.h"
#include "sample_comm_vpss.h"

TS_S32 SAMPLE_COMM_REGION_Create(TS_S32 HandleNum, RGN_TYPE_E enType);
TS_S32 SAMPLE_COMM_REGION_Destroy(TS_S32 HandleNum, RGN_TYPE_E enType);
TS_S32 SAMPLE_COMM_REGION_AttachToChn(TS_S32 HandleNum, RGN_TYPE_E enType, MPP_CHN_S *pstMppChn);
TS_S32 SAMPLE_COMM_REGION_AttachToMultiChn(TS_S32 HandleNum, RGN_TYPE_E enType, MOD_ID_E enModId, TS_S32 s32DevId, TS_S32 chnCnt);

TS_S32 SAMPLE_COMM_REGION_DetachFrmChn(TS_S32 HandleNum, RGN_TYPE_E enType, MPP_CHN_S *pstMppChn);
TS_S32 SAMPLE_COMM_REGION_DetachFrmMultiChn(TS_S32 HandleNum, RGN_TYPE_E enType, MOD_ID_E enModId, TS_S32 s32DevId, TS_S32 chnCnt);

TS_S32 SAMPLE_COMM_REGION_SetBitMap(RGN_HANDLE Handle, PIXEL_FORMAT_E enPixelFmt);
TS_S32 SAMPLE_COMM_REGION_GetUpCanvas(RGN_HANDLE Handle);
TS_S32 SAMPLE_COMM_REGION_GetMinHandle(RGN_TYPE_E enType);

TS_S32 SAMPLE_COMM_REGION_Dettach_Dynamic_Rgn(MPP_CHN_S *mpp_chn);
TS_S32 SAMPLE_COMM_REGION_Attach_Dynamic_Rgn(MPP_CHN_S *mpp_chn, bool ex);
TS_S32 SAMPLE_COMM_REGION_Start_Rgn(MPP_CHN_S *mpp_chn, bool ex, int type);  
TS_S32 SAMPLE_COMM_REGION_Osd_Timestamp(MPP_CHN_S *mpp_chn, bool ex,PIXEL_FORMAT_E enPixelFormat);
TS_S32 SAMPLE_COMM_REGION_Osd_Chinese(MPP_CHN_S *mpp_chn, bool ex,PIXEL_FORMAT_E enPixelFormat, int osd_num, int word_width);
TS_S32 SAMPLE_COMM_REGION_Dettach_Common_Rgn(MPP_CHN_S *mpp_chn);

TS_S32 SAMPLE_COMM_REGION_Stop_Rgn(MPP_CHN_S *mpp_chn);
void   SAMPLE_COMM_REGION_Osd_Proc_Stop(void);
TS_S32 SAMPLE_COMM_DEBUG_OPEN_REGION(TS_S32 HandleNum, RGN_TYPE_E  enType, MPP_CHN_S *pstChn);
TS_S32 SAMPLE_COMM_DEBUG_CLOSE_REGION(TS_S32 HandleNum, RGN_TYPE_E  enType, MPP_CHN_S *pstChn);

#endif /* End of #ifndef __SAMPLE_COMMON_H__ */
