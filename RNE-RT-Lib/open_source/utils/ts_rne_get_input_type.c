
#include <stdint.h>
#include <ts_rne_device.h>
#include <ts_rne_type.h>
#include <string.h>
#include <ts_rne_c_api.h>

RNE_NET_INPUT_TYPE_E getInputType(const char *inputDataFormat)
{
    if (strcmp(inputDataFormat, "int_hwc") == 0) {
        return RNE_NET_INPUT_TYPE_INT_HWC;
    } else if (strcmp(inputDataFormat, "int_chw") == 0) {
        return RNE_NET_INPUT_TYPE_INT_CHW;
    } else if (strcmp(inputDataFormat, "float_chw") == 0) {
        return RNE_NET_INPUT_TYPE_FLOAT_CHW;
    } else if (strcmp(inputDataFormat, "float_hwc") == 0) {
        return RNE_NET_INPUT_TYPE_FLOAT_HWC;
    } else if (strcmp(inputDataFormat, "bgr0") == 0) {
        return RNE_NET_INPUT_TYPE_INT_HWC_BGR0;
    } else if (strcmp(inputDataFormat, "rgb0") == 0) {
        return RNE_NET_INPUT_TYPE_INT_HWC_RGB0;
    } else if (strcmp(inputDataFormat, "hwc_grey") == 0) {
        return RNE_NET_INPUT_TYPE_INT_HWC_GREY;
    } else if (strcmp(inputDataFormat, "hwc_stride") == 0) {
        return RNE_NET_INPUT_TYPE_INT_HWCSTRIDE;
    } else {
        return -1; // Invalid input data format
    }
}