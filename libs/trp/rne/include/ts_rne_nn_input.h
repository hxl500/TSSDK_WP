#ifndef __TS_RNE_NN_INPUT_H__
#define __TS_RNE_NN_INPUT_H__

#include "ts_rne_c_api.h"

BEGIN_DECLS

/**
 * @brief fill input bolb of NN net
 *
 * @param net NN net pointer
 * @param coreIdx core index of RNE, default set 0.
 * @param input the input data of NN net
 * @param blobIdx blob index of blobs
 *
 * @return >= 0: stride size of input, others: fails
 */
TS_S32 TS_MPI_TRP_RNE_FillInputBlob(RNE_NET_S *net, TS_S32 coreIdx, TS_VOID *input, TS_S32 blobIdx);

/**
 * @brief fill input bolbs of NN net
 *
 * @param net NN net pointer
 * @param coreIdx core index of RNE, default set 0.
 * @param input the input data of NN net
 *
 * @return 0: successful  others: fails
 */
TS_S32 TS_MPI_TRP_RNE_FillInputBlobs(RNE_NET_S *net, TS_S32 coreIdx, TS_VOID *input);

END_DECLS

#endif /* __TS_RNE_NN_INPUT_H__ */
