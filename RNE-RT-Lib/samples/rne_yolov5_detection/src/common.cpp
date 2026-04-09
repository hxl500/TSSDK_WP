#include "../include/common.h"


float Sigmoid(float x)
{
    return (1 / (1 + std::exp(-x)));
}

int HWC_TO_CHW(cv::Mat &src_img, cv::Mat &dst_img)
{
    uint8_t *src = src_img.data;
    uint8_t *dst = dst_img.data;
    for (int c = 0; c < dst_img.channels(); c++) {
        for (int h = 0; h < dst_img.rows; h++) {
            for (int w = 0; w < dst_img.cols; w++) {
                int src_idex = h * dst_img.cols * dst_img.channels() + w * dst_img.channels() +
                               c;
                int dst_idex = c * dst_img.rows * dst_img.cols + h * dst_img.cols + w;
                dst[dst_idex] = src[src_idex];
            }
        }
    }
    return 0;
}

int BGR_OR_RGB(cv::Mat &src_img, cv::Mat &dst_img)
{
    uint32_t len = dst_img.rows * dst_img.cols;
    memcpy(dst_img.data, src_img.data + len * 2, len);
    memcpy(dst_img.data + len, src_img.data + len, len);
    memcpy(dst_img.data + len * 2, src_img.data, len);
    return 0;
}

int BGR_OR_RGB_INT8(cv::Mat &src_img, cv::Mat &dst_img)
{
    for (int i = 0; i < src_img.rows; ++i) {
        for (int j = 0; j < src_img.cols; ++j) {
            *(dst_img.data + (i * dst_img.cols + j) * dst_img.channels()) = *
                    (src_img.data + (i * src_img.cols + j) * src_img.channels() + 2);
            *(dst_img.data + (i * dst_img.cols + j )* dst_img.channels() + 1) = *
                    (src_img.data + (i * src_img.cols + j) * src_img.channels() + 1);
            *(dst_img.data + (i * dst_img.cols + j) * dst_img.channels() + 2) = *
                    (src_img.data + (i * src_img.cols + j) * src_img.channels());;
        }
    }
    return 0;
}

long GetFileSize(FILE *stream)
{
    long file_size = -1;
    long cur_offset = ftell(stream);

    if (cur_offset == -1) {
        printf("ftell failed :%s\n", strerror(errno));
        return -1;
    }

    if (fseek(stream, 0, SEEK_END) != 0) {
        printf("fseek failed: %s\n", strerror(errno));
        return -1;
    }

    file_size = ftell(stream);
    if (file_size == -1) {
        printf("ftell failed :%s\n", strerror(errno));
    }

    if (fseek(stream, cur_offset, SEEK_SET) != 0) {
        printf("fseek failed: %s\n", strerror(errno));
        return -1;
    }

    return file_size;
}
