#include "vp_kws.h"

#define         FRAME_LEN           (128)
#define         FRAME_SIZE          (FRAME_LEN * 2)

typedef struct st_vp_kws_t {
    void* handle;
    char buffer[FRAME_SIZE];
    int buffer_size;
    ALG_KWSTP_INIT_S alg_init_param;
    ALG_KWSTP_PROC_S inparam;
    ALG_KWSTP_RSLT_S outrslt
} vp_kws_t;


void* vp_kws_init() 
{
    const char* fbank_cfg = "kwstp_fbank_8k.cfg";
    const char* fbank_weight = "kwstp_fbank_8k.weight";
    const char* cfg = "kwstp_conv_8k.cfg";
    const char* weight = "kwstp_conv_8k.weight";

    int ret = 0;
    vp_kws_t* kws_handle = NULL;
    kws_handle = calloc(1, sizeof(vp_kws_t));
    if (kws_handle == NULL) {
        vp_error("Failed to calloc kws_handle.\n");
        return kws_handle;
    }

    ret = TS_ALG_GetCfgWgtByDir_Proc(cfg, weight, &kws_handle->alg_init_param.pCfg, &kws_handle->alg_init_param.pWeight, &kws_handle->alg_init_param.s32CfgLen);
    if(ret) {
		vp_error("Get conv cfg/weight error-%d!\n", ret);
        free(kws_handle);
		return kws_handle;
	}

    ret = TS_ALG_GetCfgWgtByDir_Proc(fbank_cfg, fbank_weight, &kws_handle->alg_init_param.pFbankCfg, &kws_handle->alg_init_param.pFbankWeight, &kws_handle->alg_init_param.s32FbankCfgLen);
    if(ret) {
		vp_error("Get fbank cfg/weight error!\n");
        free(kws_handle);
		return kws_handle;
	}

    kws_handle->alg_init_param.s32SmplRatio = 8000;
    kws_handle->alg_init_param.s16FrameLen = FRAME_LEN;
    kws_handle->alg_init_param.pZkyNieParam = &ZkyParam;
    kws_handle->alg_init_param.pSelfBuf = NULL;
    kws_handle->alg_init_param.pFbankSelfBuf = NULL;

    ret = TS_ALG_KwsTp_Init(&kws_handle->handle, &kws_handle->alg_init_param);
    if (ret) {
        vp_error("Error, Initializtion Failed.\n");
        free(kws_handle);
        kws_handle == NULL;
    }

    return kws_handle;
}

int vp_kws_process(void* handle, uint8_t* kws_data, uint32_t size) {
    if (handle == NULL || kws_data == NULL || size == 0) {
        vp_error("Invalid param(%p-%p-%u).", handle, kws_data, size);
        return -1;
    }

    ALG_KWSTP_PROC_S inparam = { 0 };
    ALG_KWSTP_RSLT_S outrslt = { 0 };
    vp_kws_t* kws_handle = (vp_kws_t*)handle;
    uint32_t offset = kws_handle->buffer_size;
    uint32_t left = size;
    uint32_t copy_size = 0;
    uint8_t* frame_data = kws_data;
    int ret = 0;
    while (offset + left >= FRAME_SIZE) {
        copy_size = FRAME_SIZE - offset;
        memcpy(&kws_handle->buffer[offset], frame_data, copy_size);

        frame_data += copy_size;
        left -= copy_size;
        offset = 0;
        kws_handle->buffer_size = 0;

        kws_handle->inparam.pIn = kws_handle->buffer;
        kws_handle->inparam.u32InLen = FRAME_LEN;
        TS_ALG_KwsTp_Process(kws_handle->handle, &kws_handle->inparam, &kws_handle->outrslt);

        if(!((kws_handle->outrslt.s32Result == KWSTP_RSLT_WAIT) || (kws_handle->outrslt.s32Result == KWSTP_RSLT_NULL) || (kws_handle->outrslt.s32Result == 0))) {
            ret = 1;
        }
    }

    memcpy(&kws_handle->buffer[offset], frame_data, left);
    kws_handle->buffer_size += left;
    return ret;
}

int vp_kwd_deinit(void* handle) {
    if (handle) {
        vp_kws_t* kws_handle = (vp_kws_t*)handle;
        TS_ALG_GetCfgWgtByDir_Exit(kws_handle->alg_init_param.pCfg, kws_handle->alg_init_param.pWeight);
        TS_ALG_GetCfgWgtByDir_Exit(kws_handle->alg_init_param.pFbankCfg, kws_handle->alg_init_param.pFbankWeight);
        free(handle);
        handle = NULL;
    }

    return 0;
}