#ifndef __TS_RNE_PLAT_PORT_H__
#define __TS_RNE_PLAT_PORT_H__

#include "ts_rne_type.h"

BEGIN_DECLS

/**
 * @brief RNE_PORT_ADDR_S struct
 */
typedef struct tsRNE_PORT_ADDR_S {
    TS_SIZE_T szBaseAddr;
    TS_SIZE_T szNNpu;
} RNE_PORT_ADDR_S;

/**
 * @brief RNE_DRIVER_S struct
 */
typedef struct tsRNE_DRIVER_S {
    TS_S32 (*fnpNpuInit)(RNE_PORT_ADDR_S *port);
    TS_VOID (*fnpNpuFree)(TS_VOID);
    TS_VOID (*fnpNpuProcOpt)(TS_VOID *optional);
    TS_U32 (*fnpNpuIsForwardDone)(TS_S32 npuIdx, TS_S32 timeOutMs);
    TS_VOID (*fnpNpuClearForwardDone)(TS_S32 npuIdx);
} RNE_DRIVER_S;

END_DECLS

#endif /* __TS_RNE_PLAT_PORT_H__ */
