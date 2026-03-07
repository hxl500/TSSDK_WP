#ifndef __TS_RNE_NN_OUTPUT_H__
#define __TS_RNE_NN_OUTPUT_H__

#include "ts_rne_type.h"
#include "ts_rne_c_api.h"

BEGIN_DECLS

/**
 * @brief dump output bolb of NN net
 *
 * @param net NN net pointer
 * @param blob the output blob of NN net
 * @param output the output data of NN net
 *
 * @return the output data of NN net
 */
TS_VOID *TS_MPI_TRP_RNE_DumpOutputBlob(RNE_NET_S *net, RNE_BLOB_S *blob, TS_VOID *output);

/**
 * @brief dump output bolbs of NN net, Call this function when the output needs to be NCHW.
 * when the output wants NHWC and the output address is the same as the original output address,
 * no copy operation will be performed
 *
 * @param net NN net pointer
 * @param output the output data of NN net
 *
 * @return the output data of NN net
 */
TS_VOID *TS_MPI_TRP_RNE_DumpOutputBlobs(RNE_NET_S *net, TS_VOID *output);

/**
 * @brief Convert the internal hardware format of blobs to the format of cstride
 *
 * @param srcBlobs Source blobs
 * @param dstBlobs Target blobs
 *
 * @return 0: successful  others: fails
 */
TS_S32 TS_MPI_TRP_RNE_FormatConversion(RNE_BLOBS_S *srcBlobs, RNE_BLOBS_S *dstBlobs);

END_DECLS

#endif /* __TS_RNE_NN_OUTPUT_H__ */
