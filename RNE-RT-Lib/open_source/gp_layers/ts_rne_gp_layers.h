#ifndef __TS_RNE_GP_LAYERS_H__
#define __TS_RNE_GP_LAYERS_H__

#include <ts_rne_type.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum tsRNE_LAYER_TYPE {
  RNE_LAYER_TYPE_START_LAYER = 1024,
  RNE_LAYER_TYPE_DEQUANT_LAYER = 1151,
  RNE_LAYER_TYPE_CUSTOM_OPERATOR_LAYER = 1152,
  RNE_LAYER_TYPE_MAX_LAYER_TYPE
} RNE_LAYER_TYPE;

TS_S32 TS_MPI_TRP_RNE_RegisterGpLayers(TS_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_RNE_GP_LAYERS_H__ */
