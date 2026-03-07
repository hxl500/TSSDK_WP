#ifndef __TS_RNE_TIME_H__
#define __TS_RNE_TIME_H__

#include "ts_rne_type.h"

BEGIN_DECLS

typedef struct tsRNE_TIME_STATE_S {
    const TS_CHAR *cpObjName; /*!< name		    */
    TS_S32 s32ObjID;          /*!< ID             */
    TS_U32 u32Calls;          /*!< call times		*/
    TS_U64 u64TempTime;       /*!< temp value     */
    TS_U64 u64Totalime;       /*!< total time per period */
} RNE_TIME_STATE_S;

typedef struct tsRNE_TIME_STATES_S {
    RNE_TIME_STATE_S *stpSta;
    TS_S32 s32Num;
} RNE_TIME_STATES_S;

/**
 * @brief get total network time consuming
 *
 * @param timeState obj
 *
 * @return total network time consuming(us)
 */
TS_U64 TS_MPI_TRP_RNE_GetTotalTime(RNE_TIME_STATES_S *timeState);

/**
 * @brief get total network time consuming of net forward
 *
 * @param timeState obj
 *
 * @return network time consuming of net forward(us)
 */
TS_U64 TS_MPI_TRP_RNE_GetTimeOfForward(RNE_TIME_STATES_S *timeState);

/**
 * @brief time consuming to get the specified GP layer
 *
 * @param timeState obj
 * @param layer_type layer type of gp layer
 *
 * @return network time consuming of gp layers(us)
 */
TS_U64 TS_MPI_TRP_RNE_GetTimeAtGpLayer(RNE_TIME_STATES_S *timeState, TS_S32 layerType);

/**
 * @brief get network time consuming of gp layers
 *
 * @param timeState obj
 * @param layer_type layer type of gp layer
 *
 * @return network time consuming of gp layers(us)
 */
TS_U64 TS_MPI_TRP_RNE_GetTimeOfGpLayer(RNE_TIME_STATES_S *timeState);

/**
 * @brief get network time consuming of hw layers
 *
 * @param timeState obj
 *
 * @return network time consuming of hw layers(us)
 */
TS_U64 TS_MPI_TRP_RNE_GetTimeOfHwLayer(RNE_TIME_STATES_S *timeState);

/**
 * @brief start timer
 *
 * @param none
 *
 * @return >0: successful  0: fails
 */
TS_S32 TS_MPI_TRP_RNE_StartSysTimer(TS_VOID);

/**
 * @brief get time(us)
 *
 * @param none
 *
 * @return time(us)
 */
TS_U64 TS_MPI_TRP_RNE_GetSysTimeUs();

/**
 * @brief get time(ms)
 *
 * @param none
 *
 * @return time(ms)
 */
TS_U64 TS_MPI_TRP_RNE_GetSysTimeMs();

/**
 * @brief initialization time status
 *
 * @param gpNum num of gp layers
 * @param timeState obj
 *
 * @return >0: successful  0: fails
 */
TS_S32 TS_MPI_TRP_RNE_InitTimeState(const TS_S32 gpNum, RNE_TIME_STATES_S *timeState);

/**
 * @brief reset time status
 *
 * @param timeState obj
 *
 * @return >0: successful  0: fails
 */
TS_S32 TS_MPI_TRP_RNE_ResetTimeState(RNE_TIME_STATES_S *timeState);

/**
 * @brief release time status
 *
 * @param timeState obj
 *
 * @return >0: successful  0: fails
 */
TS_S32 TS_MPI_TRP_RNE_ReleaseTimeState(RNE_TIME_STATES_S *timeState);

END_DECLS

#endif /* __TS_RNE_TIME_H__ */
