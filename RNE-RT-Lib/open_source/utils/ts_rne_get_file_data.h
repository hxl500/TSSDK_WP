
#ifndef __TS_RNE_GET_FILE_DATA_H__
#define __TS_RNE_GET_FILE_DATA_H__

#include <stdint.h>
#include <ts_rne_device.h>
#include <ts_rne_type.h>
#include <string.h>

BEGIN_DECLS
int getFileData(const char *file, int **data);


END_DECLS

#endif


