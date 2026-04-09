

#ifndef __TS_RNE_GET_INPUT_TYPE_H__
#define __TS_RNE_GET_INPUT_TYPE_H__

#include <stdint.h>
#include <ts_rne_device.h>
#include <ts_rne_type.h>
#include <string.h>


BEGIN_DECLS

RNE_NET_INPUT_TYPE_E getInputType(const char *inputDataFormat);

END_DECLS

#endif
