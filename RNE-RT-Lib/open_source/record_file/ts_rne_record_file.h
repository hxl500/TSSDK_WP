#ifndef RECORD_FILE_H_INCLUDED
#define RECORD_FILE_H_INCLUDED

#include <ts_rne_c_api.h>
#include <ts_rne_type.h>

BEGIN_DECLS

TS_S32 RNE_RecordFile(RNE_BLOBS_S *blobs, TS_CHAR *debugLayerName, TS_CHAR *outputPath, TS_S32 frameIndex, TS_S32 frameCount);

END_DECLS

#endif /* __TS_RNE_RECORD_FILE_H__ */
