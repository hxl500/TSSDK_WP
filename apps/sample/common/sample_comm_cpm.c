
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

#include "sample_comm_util.h"
#include "sample_comm_cpm.h"

TS_S32 SAMPLE_COMM_CPM_Start(CPM_GRP Cpm_Grp,	CPM_GRP_ATTR_S * pstCpmGrpAttr, CPM_Handle_S * pstCPMHandle)
{
    TS_S32         s32Ret = TS_SUCCESS;

    s32Ret = TS_MPI_CPM_CreateGrp(Cpm_Grp, pstCpmGrpAttr);

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Creat grp failed with %#x!\n", s32Ret);
        goto exit;
    }

    s32Ret = TS_MPI_CPM_Register(Cpm_Grp, pstCPMHandle);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_CPM_Register failed with %#x!\n", s32Ret);
        goto exit;
    }

    s32Ret = TS_MPI_CPM_StartGrp(Cpm_Grp);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_CPM_StartGrp failed with %#x!\n", s32Ret);
        goto exit;
    }

    return TS_SUCCESS;

exit:
    TS_MPI_CPM_DestroyGrp(Cpm_Grp);

    return TS_FAILURE;
}

TS_S32 SAMPLE_COMM_CPM_Stop(CPM_GRP CPMGrp)
{
    // AVS_CHN CPMChn = 0;
    TS_S32  s32Ret = TS_SUCCESS;

    s32Ret = TS_MPI_CPM_StopGrp(CPMGrp);

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("StopGrp failed with %#x!\n", s32Ret);
        return TS_FAILURE;
    }

    s32Ret = TS_MPI_CPM_Unregister(CPMGrp);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("CPM_Unregister failed with %#x!\n", s32Ret);
        return TS_FAILURE;
    }

    s32Ret = TS_MPI_CPM_DestroyGrp(CPMGrp);// TS_MPI_CPM_DestroyGrp

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("DestoryGrp failed with %#x!\n", s32Ret);
        return TS_FAILURE;
    }

    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_CPM_SetParam(CPM_GRP Cpm_Grp, TS_VOID *pParam)
{
    printf("cpm set param \n");
    return TS_MPI_CPM_SetParam(Cpm_Grp, pParam);
}

TS_S32  SAMPLE_COMM_CPM_GetParam(CPM_GRP Cpm_Grp, TS_VOID *pParam)
{
    printf("cpm get param \n");
    return TS_MPI_CPM_GetParam(Cpm_Grp, pParam);
}


TS_S32 SAMPLE_COMM_CPM_GetResult(CPM_GRP Cpm_Grp, TS_VOID *pResult)
{
    return TS_MPI_CPM_GetResult(Cpm_Grp, pResult);
}

