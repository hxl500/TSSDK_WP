#include "ts_rne_get_file_data.h"
#include <ts_rne_c_api.h>
#include <ts_rne_log.h>
int getFileData(const char *file, int **data)
{
    FILE *f = fopen(file, "rb");
    if (!f) {
        return -1;
    }
    fseek(f, 0, SEEK_END);
    int fileSize = ftell(f);
    rewind(f);
    *data = (int *)TS_MPI_TRP_RNE_Alloc(fileSize);
    if (!(*data)) {
        fclose(f);
        return -1;
    }
    if (fread(*data, sizeof(uint8_t), fileSize, f) != fileSize) {
        fclose(f);
        return -1;
    }
    fclose(f);
    return 0;
}
