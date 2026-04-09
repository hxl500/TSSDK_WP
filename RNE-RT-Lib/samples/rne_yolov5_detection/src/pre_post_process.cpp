#include "../include/pre_post_process.h"

#include <opencv2/opencv.hpp>
#include <ts_rne_log.h>

cv::Size LetterBox(const cv::Mat& im, cv::Mat& padded,
                   const cv::Size& new_shape,
                   const cv::Scalar& color = cv::Scalar(114, 114, 114), bool auto_resize = false,
                   bool scale_fill = false, bool scale_up = false, int stride = 32)
{
    cv::Size shape = im.size();

    double r = std::min(static_cast<double>(new_shape.width) / shape.width,
                        static_cast<double>(new_shape.height) / shape.height);
    if (!scale_up) {
        r = std::min(r, 1.0);
    }

    cv::Size new_unpad = cv::Size(static_cast<int>(round(shape.width * r)),
                                  static_cast<int>(round(shape.height * r)));
    float dw = new_shape.width - new_unpad.width;
    float dh = new_shape.height - new_unpad.height;

    if (auto_resize) {
    } else if (scale_fill) {
        dw = 0;
        dh = 0;
        new_unpad = new_shape;
        r = new_shape.width / static_cast<double>(shape.width);
    }
    dw /= 2;
    dh /= 2;
    cv::Mat resized;
    cv::resize(im, resized, new_unpad, 0, 0, cv::INTER_LINEAR);
    int top = int(round(dh-0.1));
    int bottom = int(round(dh+0.1));
    int left = int(round(dw-0.1));
    int right = int(round(dw+0.1));
    cv::copyMakeBorder(resized, padded, top, bottom, left, right,
                       cv::BORDER_CONSTANT, color);
    return cv::Size(dw, dh);
}

cv::Mat SeniorChange(cv::Mat &src_img)
{
    cv::Size shape = src_img.size();
    cv::Mat resizedImage;
    cv::Mat letterImage;
    resizedImage = src_img;
    //do resize
    if((shape.width != IMAGE_SIZE)&&(shape.height != IMAGE_SIZE)) {
        double r2 = static_cast<double>(IMAGE_SIZE)/static_cast<double>(std::max(
                        shape.width, shape.height));
        cv::Size new_unpad = cv::Size(static_cast<int>(round(shape.width*r2)),
                                      static_cast<int>(round(shape.height*r2)));

        if(r2 < 1.0) {
            cv::resize(src_img, resizedImage, new_unpad, 0, 0, cv::INTER_AREA);
        } else {
            cv::resize(src_img, resizedImage, new_unpad, 0, 0, cv::INTER_LINEAR);
        }
    }
    double r1 = std::min(static_cast<double>(shape.height)/shape.width,
                         static_cast<double>(shape.width)/shape.height);
    int reshape0=ceil(r1*640/32 + 0.5)*32;
    int reshape1 = 672;
    int reshape_h0 = reshape1;
    int reshape_w0 = reshape0;
    if(shape.height < shape.width) {
        reshape_h0 = reshape0;
        reshape_w0 = reshape1;
    }
    cv::Size targetSize(reshape_w0, reshape_h0);
    LetterBox(resizedImage, letterImage,targetSize);
    return letterImage;
}
cv::Size PreProcess(cv::Mat &src_img, cv::Mat &dst_img,
                    cv::Mat &crop_img)
{
    cv::Mat &img = src_img;
    cv::Mat resizedImage;
    resizedImage = SeniorChange(img);
    cv::Size targetSize(MODEL_INPUT_REQUIRED_W, MODEL_INPUT_REQUIRED_H);
    cv::Size ret_pad = LetterBox(img, resizedImage,targetSize);
    cv::Mat chwImage = resizedImage.clone();
    dst_img = resizedImage;
    crop_img = resizedImage;
    return ret_pad;
}

void DetectionPostProcess(RNE_BLOBS_S *debug,
                          std::vector<cv::Rect2i>& boxes,
                          std::vector<float>& confidences,
                          std::vector<int>& classIds)
{
    for (TS_U32 idx = 0; idx < debug->u32NBlob; idx++) {
        TS_S32 h = debug->stpBlob[idx].s32H;
        TS_S32 w = debug->stpBlob[idx].s32W;
        TS_S32 c = debug->stpBlob[idx].s32C;
        bool is_unsign = debug->stpBlob[idx].bIsUnsigned;
        TS_S32 cStride = TS_MPI_TRP_RNE_CStride(c, debug->stpBlob[idx].s32BitNum,
                                                debug->stpBlob[idx].bIsJoined);

        TS_S32 c_out=(debug->stpBlob[idx].eFormat==RNE_BLOB_N_H_W_C)? c:cStride;
        TS_S32 uSize = (debug->stpBlob[idx].s32BitNum / CHAR_BIT);
        TS_U8 *out = (TS_U8 *)(debug->stpBlob[idx].vpAddr);
        float coeff = *(debug->stpBlob[idx].fCoeff);
        TS_S32 lenStride = c /
                           ANCHORS_NUMBER; // yolov5 demo模型c为255，c / ANCHORS_NUMBER = 85(cx, cy, cwidth, cheight, score + 80个class)
        for(TS_S32 n = 0; n < BATCH_SIZE; n++) {
            for (TS_S32 i = 0; i < h; i++) {
                for (TS_S32 j = 0; j < w; j++) {
                    for(TS_S32 l = 0; l < ANCHORS_NUMBER; l++) {
                        cv::Rect2i rect;
                        float cx = 0.0, cy=0.0, cwidth=0.0, cheight=0.0;
                        int max_class_id = 0;
                        float max_class_score = 0.0;
                        TS_S32 *d = (TS_S32 *)(out + (((j + i * w) * c_out + l*lenStride+4)
                                                      *uSize)); // 跳过cx，cy，cwidth，cheight 4个位置，解析score
                        // 解析结果数据类型(有无符号，位宽)
                        TS_S32 data = *d & MASK_BITS(debug->stpBlob[idx].s32BitNum);
                        TS_S32 bitNum = (debug->stpBlob[idx].s32BitNum - 1);
                        TS_S32 bitValue = MASK_BITS(bitNum);
                        if (!is_unsign && data > bitValue) {
                            bitNum = (debug->stpBlob[idx].s32BitNum);
                            bitValue = MASK_BITS(bitNum);
                            data = data - (bitValue + 1);
                        }

                        // Score反量化为浮点
                        float conf = Sigmoid(data * coeff);
                        if(conf > SCORE_THRESHOLD_FOR_BOX_REGRESSION) {
                            for (TS_S32 k = 0; k < lenStride; k++) {
                                // 解析cx, cy, cwidth, cheight
                                TS_S32 *d = (TS_S32 *)(out + (((j + i * w) * c_out + l*lenStride+k) *uSize));
                                TS_S32 data = *d & MASK_BITS(debug->stpBlob[idx].s32BitNum);
                                TS_S32 bitNum = (debug->stpBlob[idx].s32BitNum - 1);
                                TS_S32 bitValue = MASK_BITS(bitNum);
                                if (!is_unsign && data > bitValue) {
                                    bitNum = (debug->stpBlob[idx].s32BitNum);
                                    bitValue = MASK_BITS(bitNum);
                                    data = data - (bitValue + 1);
                                }

                                // cx, cy, cwidth, cheight 反量化为浮点
                                float f_data =  data * coeff;
                                f_data = Sigmoid(f_data);
                                // yolov5模型边界框回归，不同检测模型计算公式不同，可能需要修改
                                if(k==0) { // cx
                                    cx = ((f_data * 2.0f - 0.5f) + j) * strides[idx];
                                    continue;
                                } else if(k==1) { // cy
                                    cy = ((f_data * 2.0f - 0.5f) + i) * strides[idx];
                                    continue;
                                } else if(k==2) { // cwidth
                                    cwidth = (float) (f_data *f_data* 4.0f * anchors[std::to_string(idx)][l][0]);
                                    continue;
                                } else if(k==3) { // cheight
                                    cheight = (float) (f_data *f_data* 4.0f * anchors[std::to_string(idx)][l][1]);
                                    continue;
                                } else if(k==4) {
                                    conf= f_data;
                                    continue;
                                } else {
                                    if (max_class_score<=f_data) {
                                        max_class_score=f_data;
                                        max_class_id = k;
                                    }
                                }
                            }

                            // 中心点坐标转化成左上角
                            rect.x = cx - cwidth / 2.0f ;
                            rect.y =  cy - cheight / 2.0f;
                            rect.width = cwidth ;
                            rect.height = cheight ;

                            boxes.emplace_back(rect);
                            confidences.emplace_back(max_class_score * conf);
                            classIds.emplace_back(max_class_id-5); // 求出分类结果id = max_class_id-5
                        }
                    }
                }
            }
            out += h * w * cStride * uSize;
        }
    }
}

int PostProcess(RNE_BLOBS_S *outputBlobs, cv::Mat &dst_img,
                std::string outFilePath, cv::Size padded_size)
{
    std::vector<cv::Rect2i> boxes;
    std::vector<float> confidences;
    std::vector<int> classIds;
    int color_rgb[30][3]= {{56, 243, 185}, {115, 75, 60}, {144, 104, 184}, {169, 44, 185}, {120, 114, 14}, {175, 148, 64}};
    DetectionPostProcess(outputBlobs,boxes,confidences,classIds);
    std::vector<int> keepIndices;

    printf("--------------------------------------------\n");
    printf("Before nms, The boxes number is %d\n", boxes.size());
    cv::dnn::NMSBoxes(boxes, confidences, NMS_SCORE_THRESHOLD, NMS_THRESHOLD,
                      keepIndices);
    printf("After nms, The boxes number is %d\n", keepIndices.size());
    TS_FLOAT top = TS_S32(round(padded_size.height - 0.1));
    TS_FLOAT left = TS_S32(round(padded_size.width - 0.1));
    TS_FLOAT new_unpad_width = MODEL_INPUT_REQUIRED_W - padded_size.width * 2;
    TS_FLOAT new_unpad_height = MODEL_INPUT_REQUIRED_H - padded_size.height * 2;
    for (size_t i = 0; i < keepIndices.size(); i++) {
        int index = keepIndices.at(i);

        TS_S32 X1 = (boxes.at(index).x-left)/new_unpad_width*dst_img.cols;
        TS_S32 Y1 = (boxes.at(index).y-top)/new_unpad_height*dst_img.rows;
        TS_S32 W = boxes.at(index).width/new_unpad_width*dst_img.cols;
        TS_S32 H = boxes.at(index).height/new_unpad_height*dst_img.rows;

        cv::Rect rect(X1, Y1, W, H);

        int color_idx=classIds.at(index)%6;
        cv::Scalar rectColor(color_rgb[color_idx][0],color_rgb[color_idx][1],
                             color_rgb[color_idx][2]);
        cv::rectangle(dst_img, rect, rectColor,
                      3); // 参数3:框的颜色，参数4:框的粗细

        // 将score转化为字符串，保留2位小数并标注到图片
        std::string str;
        char strConf[20];
        memset(strConf,0,sizeof(strConf));
        sprintf(strConf,"%.2f",confidences.at(index));
        str = strConf;
        printf("box       %d: x1:%d, y1:%d, w:%d, h:%d, conf:%.2f, score:%d\n",int(i),
               boxes.at(index).x,boxes.at(index).y,boxes.at(index).width,
               boxes.at(index).height,
               confidences.at(index),classIds.at(index));
        printf("draw rect %d: x1:%d, y1:%d, w:%d, h:%d, conf:%.2f, score:%d\n",int(i),
               X1,Y1,W,H,confidences.at(index),classIds.at(index));
        std::string confidenceText = class_name[classIds.at(index)] + " " + strConf;

#ifdef DEBUG
        printf("%s\n", confidenceText.c_str());
#endif
        cv::Point textPosition(X1, Y1 - 5); // 设置标注内容距离框5个像素
        int fontFace = cv::FONT_HERSHEY_SIMPLEX;
        double fontScale = 1.0; // 字体大小
        //BGR
        cv::Scalar fontColor(0, 255, 0);

        int thickness =2; // 字体粗细
        cv::putText(dst_img, confidenceText, textPosition, fontFace, fontScale,
                    fontColor, thickness);
    }
    printf("--------------------------------------------\n");
    cv::imwrite(outFilePath, dst_img);
    TS_MPI_TRP_RNE_Info("save processed img: %s\n", outFilePath.c_str());
    return 0;
}
