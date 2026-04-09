#ifndef _PRE_POST_PROCESS_H_
#define _PRE_POST_PROCESS_H_

#include "common.h"
#include <ts_rne_c_api.h>

static std::map<int, std::string> class_name = {{0, "person"}, {1, "bicycle"}, {2, "car"}, {3, "motorcycle"}, {4, "airplane"}, {5, "bus"}, {6, "train"}, {7, "truck"}, {8, "boat"}, {9, "traffic light"}, {10, "fire hydrant"}, {11, "stop sign"}, {12, "parking meter"}, {13, "bench"}, {14, "bird"}, {15, "cat"}, {16, "dog"}, {17, "horse"}, {18, "sheep"}, {19, "cow"}, {20, "elephant"}, {21, "bear"}, {22, "zebra"}, {23, "giraffe"}, {24, "backpack"}, {25, "umbrella"}, {26, "handbag"}, {27, "tie"}, {28, "suitcase"}, {29, "frisbee"}, {30, "skis"}, {31, "snowboard"}, {32, "sports ball"}, {33, "kite"}, {34, "baseball bat"}, {35, "baseball glove"}, {36, "skateboard"}, {37, "surfboard"}, {38, "tennis racket"}, {39, "bottle"}, {40, "wine glass"}, {41, "cup"}, {42, "fork"}, {43, "knife"}, {44, "spoon"}, {45, "bowl"}, {46, "banana"}, {47, "apple"}, {48, "sandwich"}, {49, "orange"}, {50, "broccoli"}, {51, "carrot"}, {52, "hot dog"}, {53, "pizza"}, {54, "donut"}, {55, "cake"}, {56, "chair"}, {57, "couch"}, {58, "potted plant"}, {59, "bed"}, {60, "dining table"}, {61, "toilet"}, {62, "tv"}, {63, "laptop"}, {64, "mouse"}, {65, "remote"}, {66, "keyboard"}, {67, "cell phone"}, {68, "microwave"}, {69, "oven"}, {70, "toaster"}, {71, "sink"}, {72, "refrigerator"}, {73, "book"}, {74, "clock"}, {75, "vase"}, {76, "scissors"}, {77, "teddy bear"}, {78, "hair drier"}, {79, "toothbrush"}};

int const strides[3] = {8,16,32};
static std::unordered_map<std::string, std::vector<std::vector<int>>> 	anchors = {
    {"0", {{10, 13}, {16, 30}, {33, 23}}},
    {"1", {{30, 61}, {62, 45}, {59, 119}}},
    {"2", {{116, 90}, {156, 198}, {373, 326}}}
};

#define IMAGE_SIZE 640
#define MODEL_INPUT_REQUIRED_W 640
#define MODEL_INPUT_REQUIRED_H 640

cv::Size PreProcess(cv::Mat &src_img, cv::Mat &dst_img, cv::Mat &crop_img);

#define SCORE_THRESHOLD_FOR_BOX_REGRESSION 0.25 // 为加速边界框回归，低于该值的边界框会被舍弃
#define NMS_SCORE_THRESHOLD 0.25                // NMS置信度阈值，低于该值的边界框被舍弃
#define NMS_THRESHOLD 0.45                      // NMS阈值，IOU重叠度高于该阈值的边界框会被抑制
#define BATCH_SIZE 1                            // 模型的N维度大小
#define ANCHORS_NUMBER 3                        // YOLOV5每个特征点的先验框个数

int PostProcess(RNE_BLOBS_S *outputBlobs, cv::Mat &dst_img, std::string outFilePath, cv::Size origin_size);
#endif