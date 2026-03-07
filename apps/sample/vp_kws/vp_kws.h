#ifndef VP_KWS_H
#define VP_KWS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "ts_alg_rne_info_get.h"
#include "ts_alg_kwstp.h"
#include "ts_alg_log.h"
#include "smooth_cfg.h"
#include "vp_printf.h"

void* vp_kws_init();
int vp_kws_process(void* handle, uint8_t* kws_data, uint32_t size);
int vp_kwd_deinit(void* handle);
#endif