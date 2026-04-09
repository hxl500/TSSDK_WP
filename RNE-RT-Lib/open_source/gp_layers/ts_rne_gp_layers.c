#include "ts_rne_gp_layers.h"
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ts_rne_c_api.h>
#include <ts_rne_log.h>

extern void TS_MPI_TRP_RNE_GpLayerCustormOperator(RNE_BLOBS_S *input,
        RNE_BLOBS_S *output,
        RNE_BIN_DATA_S *extra_data,
        void *user_data);

extern void TS_MPI_TRP_RNE_GpLayerDeQuantization(RNE_BLOBS_S *input,
        RNE_BLOBS_S *output,
        RNE_BIN_DATA_S *extra_data,
        void *user_data);

TS_S32 TS_MPI_TRP_RNE_RegisterGpLayers(TS_VOID)
{
    TS_S32 ret = TS_MPI_TRP_RNE_InitGpLayerNum(RNE_LAYER_TYPE_MAX_LAYER_TYPE -
                 RNE_LAYER_TYPE_START_LAYER);
    ret |= TS_MPI_TRP_RNE_RegisterGpLayer(RNE_LAYER_TYPE_DEQUANT_LAYER,
                                          TS_MPI_TRP_RNE_GpLayerDeQuantization);
    ret |= TS_MPI_TRP_RNE_RegisterGpLayer(RNE_LAYER_TYPE_CUSTOM_OPERATOR_LAYER,
                                          TS_MPI_TRP_RNE_GpLayerCustormOperator);
    return ret;
}
