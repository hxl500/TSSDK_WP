

#ifndef __SAMPLE_COMM_CPM_H__
#define __SAMPLE_COMM_CPM_H__

#include <pthread.h>

#include "ts_comm_cpm.h"

#include "mpi_cpm.h"

TS_S32 SAMPLE_COMM_CPM_Start(CPM_GRP Cpm_Grp,	CPM_GRP_ATTR_S * pstCpmGrpAttr, CPM_Handle_S * pstCPMHandle);
TS_S32 SAMPLE_COMM_CPM_Stop(CPM_GRP CPMGrp);

#endif /* End of #ifndef __SAMPLE_COMMON_H__ */
