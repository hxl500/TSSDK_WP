#ifndef _COMMON_H_
#define _COMMON_H_

#include <map>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>

float Sigmoid(float x);
long GetFileSize(FILE *stream);
int HWC_TO_CHW(cv::Mat &src_img, cv::Mat &dst_img);
int BGR_OR_RGB(cv::Mat &src_img, cv::Mat &dst_img);
int BGR_OR_RGB_INT8(cv::Mat &src_img, cv::Mat &dst_img);

#endif