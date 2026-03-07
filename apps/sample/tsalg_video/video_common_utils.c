#include "video_common_utils.h"
#include <stdlib.h>

TS_U32 SAMPLE_ALGO_GetFileSize(FILE *pf)
{
	TS_U32 fileSize = 0;
	if(NULL != pf){
		if(0 == fseek(pf, 0, SEEK_END)){
			fileSize = ftell(pf);
		}
		rewind(pf);
	}
	return fileSize;
}

TS_S32 SAMPLE_ALG_LOAD_MODEL(const TS_CHAR* fileName, TS_U32 *pModelSize, TS_U8 **ppAddr) 
{
	FILE *pf_File = fopen(fileName, "rb");
	if(!pf_File) {
		return TS_FAILURE;
	}
	*pModelSize = SAMPLE_ALGO_GetFileSize(pf_File);
	TS_U8 *pAddress = (TS_U8 *)malloc(*pModelSize*sizeof(TS_U8));

	fread(pAddress, *pModelSize, 1, pf_File);
	fclose(pf_File);
	*ppAddr = pAddress;
	return TS_SUCCESS;
}
