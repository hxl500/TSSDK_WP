#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>

#include <map>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>

#include "ts_rne_c_api.h"
#include "ts_rne_log.h"
#include "ts_rne_version.h"
#include "ts_rne_time.h"

#include "ts_alg_log.h"
#include "ts_alg_body_detect_v2.h"
#include "ts_rne_nn_input.h"
#include "arrr_diff.h"
//#define TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM (4)	//56 de sdk内部会对齐
// #define TIME_CONSUME_LAYER

static TS_BOOL gRneOff = TS_FALSE;
static TS_FLOAT gThreshold = 0.25;//0.25;
static TS_FLOAT *gPostProcBuf = NULL;

#ifdef TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM
	static TS_U8 *gParamStride = NULL;
#endif
ALG_CatDetect_DET_PARAM_S det_param_cpp;

// static int nms(float *torchCat, int len1, int len2, int *nmsOut, float thresh, int maxDet)
// {
//     float tmpX, tmpY, tmpW, tmpH;

//     float* pScore = (float *)(nmsOut + len1);
//     int* box_idx = (int *)(pScore + len1);
//     float* areas = (float*)(box_idx + len1);
//     for (int i = 0; i < len1; i++)
//     {
//         box_idx[i] = i;
//         pScore[i] = torchCat[len2 * i + 5];
//         tmpX = torchCat[len2 * i + 0];
//         tmpY = torchCat[len2 * i + 1];
//         tmpW = torchCat[len2 * i + 2];
//         tmpH = torchCat[len2 * i + 3];
//         areas[i] = tmpW * tmpH;
//         torchCat[len2 * i + 0] = tmpX - tmpW / 2;
//         torchCat[len2 * i + 1] = tmpY - tmpH / 2;
//         torchCat[len2 * i + 2] = tmpX + tmpW / 2;
//         torchCat[len2 * i + 3] = tmpY + tmpH / 2;
//     }

//     for (int i = 0; i < len1; i++) {
//         float max = pScore[i];
//         int idx_t = i;
//         for (int j = i + 1; j < len1; j++) {
//             if (max < pScore[j]) {
//                 max = pScore[j];
//                 idx_t = j;
//             }
//         }
//         int tt = box_idx[i];
//         box_idx[i] = box_idx[idx_t];
//         box_idx[idx_t] = tt;
//         float tt1 = pScore[i];
//         pScore[i] = pScore[idx_t];
//         pScore[idx_t] = tt1;
//     }

//     int	cnt = 0;
//     int sSzie = len1;
//     while (sSzie > 0) {// torch.numel()返回张量元素个数
//         if (sSzie == 1) {//    保留框只剩一个
//             nmsOut[cnt] = box_idx[0];
//             cnt++;
//             break;
//         }
//         else {
//             nmsOut[cnt] = box_idx[0];
//             cnt++;
//         }
//         if (cnt >= maxDet) {
//             break;
//         }

// 		//计算box[i]与其余各框的IOU(思路很好)
// 		//auto orderMask = order.narrow(1, order.m_size - 1, 0);
// 		//x1.index({ orderMask });
// 		//x1.index({ orderMask }).clamp(x1[keep.back()].item().toFloat(), 1e10);
// 		//auto xx1 = x1.index({ orderMask }).clamp(x1.m_data[keep.back()],1e10);// [N - 1, ]
// 		//auto yy1 = y1.index({ orderMask }).clamp(y1.m_data[keep.back()], 1e10);
// 		//auto xx2 = x2.index({ orderMask }).clamp(0, x2.m_data[keep.back()]);
// 		//auto yy2 = y2.index({ orderMask }).clamp(0, y2.m_data[keep.back()]);
//         float *xx1 = (float *)(areas + len1);
//         float *yy1 = (float *)(xx1 + sSzie);
//         float *xx2 = (float *)(yy1 + sSzie);
//         float *yy2 = (float *)(xx2 + sSzie);
//         for (int i=1; i<sSzie; i++){
//             xx1[i-1] = MIN(MAX(torchCat[len2*box_idx[0] + 0], torchCat[len2*box_idx[i] + 0]), 1e10);
//             yy1[i-1] = MIN(MAX(torchCat[len2*box_idx[0] + 1], torchCat[len2*box_idx[i] + 1]), 1e10);
//             xx2[i-1] = MIN(MAX(0, torchCat[len2*box_idx[i] + 2]), torchCat[len2*box_idx[0] + 2]);
//             yy2[i-1] = MIN(MAX(0, torchCat[len2*box_idx[i] + 3]), torchCat[len2*box_idx[0] + 3]);
//         }

// 		// auto selWidth = xx2 - xx1;
//         // auto selHeight = yy2 - yy1;
//         // auto selWidth_ = selWidth.clamp(0, 1e10);
//         // auto selHeight_= selHeight.clamp(0, 1e10);
// 		// auto inter = selWidth_ * selHeight_;// [N - 1, ]
//         // xx2 = inter
//         for (int i=0; i<sSzie-1; i++){
//             xx1[i] = MIN(MAX(0, xx2[i] - xx1[i]), 1e10);
//             yy1[i] = MIN(MAX(0, yy2[i] - yy1[i]), 1e10);
//             xx2[i] = xx1[i] * yy1[i];
//         }

//         // auto tmpNarrow = order.narrow(1,order.m_size - 1, 0) ;
//         // auto merge = areas.index(tmpNarrow) + areas.m_data[keep.back()] - inter;
// 		// auto iou = inter / (merge);//[N - 1, ]
//         //xx1 = merge
//         //yy1 = iou
//         //yy2 = idx
//         // auto idx = cmpnonzero(iou, thresh);
//         int idx_len = 0;
//         int *newbox_idx = (int *)yy2;
//         //int* idx = (int*)yy2;
//         for (int i = 1; i < sSzie; i++) {
//             xx1[i - 1] = areas[box_idx[i]] + areas[box_idx[0]] - xx2[i - 1];
//             yy1[i - 1] = xx2[i - 1] / xx1[i - 1];

//             if (yy1[i - 1] < thresh) {
//                 newbox_idx[idx_len] = box_idx[i];
//                 idx_len++;
//             }
//         }

//         if (idx_len == 0) {
//             break;
//         }

//         // auto tmpIndex = idx + 1;
//         // tmpIndex.m_type = 2;
//         // order = order.index(tmpIndex); //修补索引之间的差值
//         memcpy(box_idx, newbox_idx, idx_len * sizeof(int));
//         //sSzie = idx_len - 1;
//         sSzie = idx_len;
//     }
//     return cnt;
// }
// static int nms(const float *torchCat, int len1, int len2, int *nmsOut, float thresh, int maxDet)
// {
//     if (len1 <= 0 || maxDet <= 0)
//         return 0;

//     typedef struct {
//         float x1, y1, x2, y2;
//         float score;
//         int   index;
//     } Box;

//     Box *boxes = (Box*)malloc(len1 * sizeof(Box));
//     if (!boxes) return 0;

//     // 正确解析：cx, cy, w, h -> x1y1x2y2
//     for (int i = 0; i < len1; i++) {
//         float cx = torchCat[len2 * i + 0];
//         float cy = torchCat[len2 * i + 1];
//         float w  = torchCat[len2 * i + 2];
//         float h  = torchCat[len2 * i + 3];
//         float score = torchCat[len2 * i + 5];

//         boxes[i].x1 = cx - w * 0.5f;
//         boxes[i].y1 = cy - h * 0.5f;
//         boxes[i].x2 = cx + w * 0.5f;
//         boxes[i].y2 = cy + h * 0.5f;
//         boxes[i].score = score;
//         boxes[i].index = i;
//     }

//     // 按置信度降序排序
//     for (int i = 0; i < len1 - 1; i++) {
//         for (int j = i + 1; j < len1; j++) {
//             if (boxes[j].score > boxes[i].score) {
//                 Box tmp = boxes[i];
//                 boxes[i] = boxes[j];
//                 boxes[j] = tmp;
//             }
//         }
//     }

//     char *suppressed = (char*)calloc(len1, 1);
//     int cnt = 0;

//     for (int i = 0; i < len1 && cnt < maxDet; i++) {
//         if (suppressed[i]) continue;

//         nmsOut[cnt++] = boxes[i].index;

//         float ax1 = boxes[i].x1;
//         float ay1 = boxes[i].y1;
//         float ax2 = boxes[i].x2;
//         float ay2 = boxes[i].y2;
//         float area_a = (ax2 - ax1) * (ay2 - ay1);

//         for (int j = i + 1; j < len1; j++) {
//             if (suppressed[j]) continue;

//             float bx1 = boxes[j].x1;
//             float by1 = boxes[j].y1;
//             float bx2 = boxes[j].x2;
//             float by2 = boxes[j].y2;

//             float ix1 = ax1 > bx1 ? ax1 : bx1;
//             float iy1 = ay1 > by1 ? ay1 : by1;
//             float ix2 = ax2 < bx2 ? ax2 : bx2;
//             float iy2 = ay2 < by2 ? ay2 : by2;

//             float iw = ix2 - ix1;
//             float ih = iy2 - iy1;
//             if (iw <= 0 || ih <= 0) continue;

//             float inter = iw * ih;
//             float area_b = (bx2 - bx1) * (by2 - by1);
//             float iou = inter / (area_a + area_b - inter);

//             if (iou > thresh) {
//                 suppressed[j] = 1;
//             }
//         }
//     }

//     free(suppressed);
//     free(boxes);
//     return cnt;
// }

static int nms(const float *torchCat, int len1, int len2, int *nmsOut, float thresh, int maxDet)
{
    if (len1 <= 0 || maxDet <= 0)
        return 0;

    typedef struct {
        float cx, cy, w, h;
        float score;
        int index;
    } Box;

    Box *boxes = (Box *)malloc(len1 * sizeof(Box));
    if (!boxes) return 0;

    // 只读取，不修改、不转换坐标！
    for (int i = 0; i < len1; i++) {
        boxes[i].cx = torchCat[len2 * i + 0];
        boxes[i].cy = torchCat[len2 * i + 1];
        boxes[i].w  = torchCat[len2 * i + 2];
        boxes[i].h  = torchCat[len2 * i + 3];
        boxes[i].score = torchCat[len2 * i + 4];
        boxes[i].index = i;
    }

    // 按置信度排序
    for (int i = 0; i < len1 - 1; i++) {
        for (int j = i + 1; j < len1; j++) {
            if (boxes[j].score > boxes[i].score) {
                Box tmp = boxes[i];
                boxes[i] = boxes[j];
                boxes[j] = tmp;
            }
        }
    }

    char *suppressed = (char *)calloc(len1, 1);
    int cnt = 0;

    for (int i = 0; i < len1 && cnt < maxDet; i++) {
        if (suppressed[i]) continue;

        nmsOut[cnt++] = boxes[i].index;

        // 计算 IOU 时临时转框，不改动原始数据
        float cx1 = boxes[i].cx;
        float cy1 = boxes[i].cy;
        float w1  = boxes[i].w;
        float h1  = boxes[i].h;
        float x1 = cx1 - w1 * 0.5f;
        float y1 = cy1 - h1 * 0.5f;
        float x2 = cx1 + w1 * 0.5f;
        float y2 = cy1 + h1 * 0.5f;
        float area_a = (x2 - x1) * (y2 - y1);

        for (int j = i + 1; j < len1; j++) {
            if (suppressed[j]) continue;

            float cx2 = boxes[j].cx;
            float cy2 = boxes[j].cy;
            float w2  = boxes[j].w;
            float h2  = boxes[j].h;
            float bx1 = cx2 - w2 * 0.5f;
            float by1 = cy2 - h2 * 0.5f;
            float bx2 = cx2 + w2 * 0.5f;
            float by2 = cy2 + h2 * 0.5f;

            float ix1 = (x1 > bx1) ? x1 : bx1;
            float iy1 = (y1 > by1) ? y1 : by1;
            float ix2 = (x2 < bx2) ? x2 : bx2;
            float iy2 = (y2 < by2) ? y2 : by2;

            float iw = ix2 - ix1;
            float ih = iy2 - iy1;
            if (iw <= 0 || ih <= 0) continue;

            float inter = iw * ih;
            float area_b = (bx2 - bx1) * (by2 - by1);
            float iou = inter / (area_a + area_b - inter);

            if (iou > thresh) {
                suppressed[j] = 1;
            }
        }
    }

    free(suppressed);
    free(boxes);
    return cnt;
}

//lx test
float test_conf = 0;
int test_conf_set(float conf)
{
	test_conf = conf;
	return 0;
}
int test_conf_get()
{
	int ret = test_conf*100;
	return ret;
}
//
// int TS_ALG_PcppDetV12_PostProcess(unsigned char **blob, unsigned int *cstride, unsigned int *s32C, float *fcoeff, float* dataVec)
// {
// 	int nblob = 3;
// 	int shapeVec2[3][5] = {{1, 3, 48, 80, 6}, {1, 3, 24, 40, 6}, {1, 3, 12, 20, 6}};

//     float scale_output[] = { fcoeff[0], fcoeff[1], fcoeff[2] };
//     float scale_output2[] = { fcoeff[0] * 2, fcoeff[1] * 2, fcoeff[2] * 2 };
//     int stride[3] = { 8,16,32 };
//     int anch[3][6] = {{10,13, 16,30, 33,23}, {30,61, 62,45, 59,119}, {116,90, 156,198, 373,326}};
//     //int anch[3][6] = {{32, 23, 62, 35, 99, 55}, {107, 104, 193, 98, 255,143}, {385, 146, 351, 205, 469, 208}};
//     //2026-03-31
//     //int anch[3][6] = {{32, 38, 62, 59, 99, 92}, {107, 174, 193, 163, 255,238}, {385, 243, 351, 342, 469, 346}};
//     float value_det = 0.5f;
//     int len = 0;
//     float petThres = 0.7;
//     TS_ALG_CatDetect_GetParam(&det_param_cpp);
//     petThres = det_param_cpp.DetectionConfThres;
//     int maxNms = 10000;

//     for (int i = 0; i < nblob; i++)
//     {
//         int no_yolo = s32C[i] / 3;
//         shapeVec2[i][4] = no_yolo;
//         for (int j = 0; j < shapeVec2[i][1]; j++) {
//             int* anchorgrid = &(anch[i][j * 2]);
//             for (int k = 0; k < shapeVec2[i][2]; k++) {
//                 for (int m = 0; m < shapeVec2[i][3]; m++) {
//                     unsigned char *dataf = blob[i] + j * no_yolo + k * shapeVec2[i][3] * cstride[i] + m * cstride[i];

//                     float data = (dataf[4] * scale_output[i]) * (dataf[5] * scale_output[i]);
//                     if (data > petThres) {
//                         if(len < maxNms){
//                             float griddataf0 = (float)m - value_det;
//                             float griddataf1 = (float)k - value_det;

//                             dataVec[len * 6 + 0] = (dataf[0] * scale_output2[i] + griddataf0) * stride[i];
//                             dataVec[len * 6 + 1] = (dataf[1] * scale_output2[i] + griddataf1) * stride[i];

//                             float value = (dataf[2] * scale_output2[i]);
//                             dataVec[len * 6 + 2] = value * value * anchorgrid[0];
//                             value = (dataf[3] * scale_output2[i]);
//                             dataVec[len * 6 + 3] = value * value * anchorgrid[1];

//                             dataVec[len * 6 + 4] = dataf[4] * scale_output[i];
//                             dataVec[len * 6 + 5] = data;
//                             len++;
//                         }
//                     }
//                 }
//            }
//        }
//     }

//     return len;
   
// }
int TS_ALG_PcppDetV12_PostProcess(unsigned char **blob, unsigned int *cstride, unsigned int *s32C, float *fcoeff, float* dataVec)
{
	int nblob = 3;
	int shapeVec2[3][5] = {{1, 3, 48, 80, 6}, {1, 3, 24, 40, 6}, {1, 3, 12, 20, 6}};

    float scale_output[] = { fcoeff[0], fcoeff[1], fcoeff[2] };
    float scale_output2[] = { fcoeff[0] * 2, fcoeff[1] * 2, fcoeff[2] * 2 };
    int stride[3] = { 8,16,32 };

    // 宠物猫专用 anchor（适配 640x360，不贴边、更稳定）
    // int anch[3][6] = {
    //     { 40, 32,  60, 45,  90, 60 },
    //     { 95, 90, 130, 100, 160, 130 },
    //     { 180, 150, 240, 200, 320, 260 }
    // };
    int anch[3][6] = {{10,13, 16,30, 33,23}, {30,61, 62,45, 59,119}, {116,90, 156,198, 373,326}};
    float value_det = 0.5f;
    int len = 0;
    float petThres = 0.7;
    TS_ALG_CatDetect_GetParam(&det_param_cpp);
    petThres = det_param_cpp.DetectionConfThres;
    int maxNms = 10000;

    for (int i = 0; i < nblob; i++)
    {
        int no_yolo = s32C[i] / 3;
        shapeVec2[i][4] = no_yolo;
        for (int j = 0; j < shapeVec2[i][1]; j++) {
            int* anchorgrid = &(anch[i][j * 2]);
            for (int k = 0; k < shapeVec2[i][2]; k++) {
                for (int m = 0; m < shapeVec2[i][3]; m++) {
                    unsigned char *dataf = blob[i] + j * no_yolo + k * shapeVec2[i][3] * cstride[i] + m * cstride[i];

                    // float data = (dataf[4] * scale_output[i]) * (dataf[5] * scale_output[i]);
                    // if (data > petThres) {
                    //     if(len < maxNms){
                    //         float griddataf0 = (float)m - value_det;
                    //         float griddataf1 = (float)k - value_det;

                    //         float cx = (dataf[0] * scale_output2[i] + griddataf0) * stride[i];
                    //         float cy = (dataf[1] * scale_output2[i] + griddataf1) * stride[i];

                    //         float value_w = (dataf[2] * scale_output2[i]);
                    //         float w = value_w * value_w * anchorgrid[0];
                    //         float value_h = (dataf[3] * scale_output2[i]);
                    //         float h = value_h * value_h * anchorgrid[1];

                    //         // 防贴边 + 防止框过小闪烁
                    //         cx = fmaxf(fminf(cx, 640 - 30), 30);
                    //         cy = fmaxf(fminf(cy, 360 - 30), 30);
                    //         w  = fmaxf(w, 50);
                    //         h  = fmaxf(h, 50);

                    //         dataVec[len * 6 + 0] = cx;
                    //         dataVec[len * 6 + 1] = cy;
                    //         dataVec[len * 6 + 2] = w;
                    //         dataVec[len * 6 + 3] = h;
                    //         dataVec[len * 6 + 4] = dataf[4] * scale_output[i];
                    //         dataVec[len * 6 + 5] = data;

                    //         len++;
                    //     }
                //     float data = (dataf[4] * scale_output[i]) * (dataf[5] * scale_output[i]);
                //     if (data > petThres) {
                //     // ======================== 核心修复 ========================
                //         if (len >= 30) {  // 限制框数量，防止越界踩内存
                //             continue;
                //                 }

                // float griddataf0 = (float)m - value_det;
                // float griddataf1 = (float)k - value_det;

                // float cx = (dataf[0] * scale_output2[i] + griddataf0) * stride[i];
                // float cy = (dataf[1] * scale_output2[i] + griddataf1) * stride[i];

                // float value_w = (dataf[2] * scale_output2[i]);
                // float w = value_w * value_w * anchorgrid[0];
                // float value_h = (dataf[3] * scale_output2[i]);
                // float h = value_h * value_h * anchorgrid[1];

                // cx = fmaxf(fminf(cx, 640 - 30), 30);
                // cy = fmaxf(fminf(cy, 360 - 30), 30);
                // w  = fmaxf(w, 50);
                // h  = fmaxf(h, 50);

                // dataVec[len * 6 + 0] = cx;
                // dataVec[len * 6 + 1] = cy;
                // dataVec[len * 6 + 2] = w;
                // dataVec[len * 6 + 3] = h;
                // dataVec[len * 6 + 4] = dataf[4] * scale_output[i];
                // dataVec[len * 6 + 5] = data;

                // len++;
                // //}

                //     }
                float data = (dataf[4] * scale_output[i]) * (dataf[5] * scale_output[i]);
if (data > petThres) {
    // 【仅修复内存】防止 len 异常大导致越界写坏全局变量
    if (len >= 100) {
        continue;
    }

    if (len < maxNms) {
        float griddataf0 = (float)m - value_det;
        float griddataf1 = (float)k - value_det;

        float cx = (dataf[0] * scale_output2[i] + griddataf0) * stride[i];
        float cy = (dataf[1] * scale_output2[i] + griddataf1) * stride[i];

        float value_w = (dataf[2] * scale_output2[i]);
        float w = value_w * value_w * anchorgrid[0];
        float value_h = (dataf[3] * scale_output2[i]);
        float h = value_h * value_h * anchorgrid[1];

        cx = fmaxf(fminf(cx, 640 - 30), 30);
        cy = fmaxf(fminf(cy, 360 - 30), 30);
        w  = fmaxf(w, 50);
        h  = fmaxf(h, 50);

        dataVec[len * 6 + 0] = cx;
        dataVec[len * 6 + 1] = cy;
        dataVec[len * 6 + 2] = w;
        dataVec[len * 6 + 3] = h;
        dataVec[len * 6 + 4] = dataf[4] * scale_output[i];
        dataVec[len * 6 + 5] = data;

        len++;
        }
        }

                }
           }
       }
    }

    return len;
}


TS_S32 TS_ALG_BodyDetect_Init(TS_VOID **handle, ALG_MODEL_INIT_S *param)
{
	printf("22222==============\n");
	TS_MPI_TRP_RNE_SetLogLevel(RNE_LOG_INFO);
    ALG_LOGI("rne log level : %d\n", TS_MPI_TRP_RNE_GetLogLevel());
    ALG_LOGI("rne lib version :%s\n", TS_MPI_TRP_RNE_GetSdkVersion());
	TS_S32 ret = 0;

	gRneOff = (TS_BOOL)param->bRneOff;
	if(TS_TRUE != gRneOff) {
	    // ret = TS_MPI_TRP_RNE_OpenDevice(NULL, rne_register_gp_layers);
	    ret = TS_MPI_TRP_RNE_OpenDevice(NULL, NULL);
	    if (0 != ret) {
	        ALG_LOGE("open device error!\n");
	        return -1;
	    }
		ALG_LOGD("open device success! \n");
	}

    RNE_NET_S *nModel = (RNE_NET_S *)malloc(sizeof(RNE_NET_S));
    if(NULL == nModel)
    {
		ALG_LOGE("malloc error!\n");
        return -1;
    }
	//ALG_LOGE("malloc error!\n");

	memset(nModel, 0, sizeof(RNE_NET_S));
    nModel->u8pGraph = param->pGraph;
    nModel->u8pParams = param->pWeight;
    nModel->eInputType = (RNE_NET_INPUT_TYPE_E)param->eImageType;
    /* 初始化多网络模型，并在每次初始化网络配置后，进行网络OnceLoad
     */
    /* 量化和权重数据需要4byte对齐
     * 如果未在头文件4byte对齐，可执行W_ALIGN_BYTES_NUM内代码，进行4字节对齐
     */

#ifdef TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM

	if(TS_NULL != gParamStride) {
		ALG_LOGE("Multi threading is not supported!\n");
        return -1;
	}

#ifdef LINUX_PAL
    gParamStride = (TS_U8 *)TS_MPI_TRP_RNE_AllocLinearMem(param->u32WeightSize + TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM);
#else
	gParamStride = (TS_U8 *)TS_MPI_TRP_RNE_Alloc(param->u32WeightSize + TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM);
#endif

    if (NULL == gParamStride) {
        ALG_LOGE("insufficient memory!\n");
        return -1;
    }
    TS_SIZE_T addr = (TS_SIZE_T)gParamStride;
    addr += TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM - 1;
    addr &= ~(TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM - 1);
    memcpy((TS_VOID *)addr, nModel->u8pParams, param->u32WeightSize);
    nModel->u8pParams = (TS_U8 *)addr;
#endif
	//ALG_LOGE("malloc error!\n");
    /* 初始化单个网路
        */
    ret = TS_MPI_TRP_RNE_LoadModel(nModel);
    if (ret) {
        ALG_LOGE("load model error!\n");
        return ret;
    }
    /* net once load
        * 仅有网络模型配置为once load情况下，内部才真正执行once load
        */
    ret = TS_MPI_TRP_RNE_OnceLoad(nModel);
    if (ret) {
        ALG_LOGE("once load error!\n");
        return ret;
    }

#ifdef TS_MPI_TRP_RNE_W_ALIGN_BYTES_NUM
	if(TS_NULL != gParamStride) {
#ifdef LINUX_PAL
		TS_MPI_TRP_RNE_FreeLinearMem(gParamStride);
#else
		TS_MPI_TRP_RNE_Free(gParamStride);
#endif
		gParamStride = TS_NULL;
	}
#endif
	//ALG_LOGE("malloc error!\n");
	TS_U32 heap_size = TS_MPI_TRP_RNE_GetBlobsBufSize(nModel);
	if(heap_size > 0){
		ALG_LOGI("model heap size is %d\n", heap_size);
	}
	else{
		ALG_LOGE("error, TS_MPI_TRP_RNE_GetBlobsBufSize error!\n");
		return -1;
	}
	//ALG_LOGE("malloc error!\n");
    int shapeVec[] = { 1, (int)(3 * 48 * 80 + 3 * 24 * 40 + 3 * 12 * 20), 6};
    gPostProcBuf = (TS_FLOAT *)malloc(shapeVec[0] * shapeVec[1] * shapeVec[2] * sizeof(float));    //362,880
	if(NULL == gPostProcBuf){
		ALG_LOGE("malloc error !!!\n");
		return -1;
	}
	ALG_LOGI("algo heap size is %d\n", shapeVec[0] * shapeVec[1] * shapeVec[2] * sizeof(float));
	//ALG_LOGE("malloc error!\n");
    *handle = nModel;
    return 0;
}

TS_S32 TS_ALG_BodyDetect_Exit(TS_VOID *handle)
{
    RNE_NET_S *nModel = (RNE_NET_S *)handle;

	TS_MPI_TRP_RNE_UnloadModel(nModel);

	if(TS_NULL != nModel) {
		free(nModel);
		nModel = TS_NULL;
	}

	if(TS_TRUE != gRneOff) {
		TS_MPI_TRP_RNE_CloseDevice();
	}

	if(TS_NULL != gPostProcBuf) {
		free(gPostProcBuf);
		gPostProcBuf = TS_NULL;
	}

    return 0;
}

TS_S32 strides[3] = {8, 16, 32};
std::unordered_map<std::string, std::vector<std::vector<TS_S32>>> anchors = {{"0", {{10, 13}, {16, 30}, {33, 23}}},
    {"1", {{30, 61}, {62, 45}, {59, 119}}},
    {"2", {{116, 90}, {156, 198}, {373, 326}}}
};

TS_S32 TS_ALG_BodyDetect_Process(TS_VOID *handle, ALG_IMAGE_S *image, ALG_CatDetect_DET_RESULT_S *result)
{
#if 1
	TS_U32 cstride[16];
	TS_FLOAT fcoeff[16];

    if(TS_NULL == image->pData || TS_NULL == result) {
		ALG_LOGE("error, Invalid parameter!\n");
		return -1;
	}
	//printf("%s,%d\n",__FUNCTION__,__LINE__);
	RNE_NET_S *nModel = (RNE_NET_S *)handle;



#if 1 //input image is 640*384
    if (0 != TS_MPI_TRP_RNE_SetInputBlobsAddr(nModel, (void *)(image->pData), (void *)(image->pDataPhy))) {
        ALG_LOGE("set inputBlobs error!\n");
        return -1;
    }
#else //input image is 640*360
	if(image->s32H != 360 || image->s32W != 640) {
		ALG_LOGE("error, Invalid image size need resize!\n");
		ALG_LOGE("image h:%d w:%d, blob h:%d w:%d\n", image->s32H, image->s32W,
								inputBlobs->stpBlob->s32H, inputBlobs->stpBlob->s32W);
		return -1;
	}

	//image pretreatment: 640*360->640*384, fill with 114(0x72) before image and end image
	TS_U32 inputSize = image->s32C*image->s32H*image->s32W;
	TS_U32 imageSize = image->s32C*640*384;

	memset(imageBuf, 114, imageSize);
	memcpy(&imageBuf[image->s32C*640*12], image->pData, inputSize);

	nModel->vpInput = imageBuf;
#endif

#ifdef TIME_CONSUME_LAYER
	 TS_MPI_TRP_RNE_StartSysTimer();
	 RNE_TIME_STATES_S time;
	 memset(&time, 0, sizeof(RNE_TIME_STATES_S));
	 TS_MPI_TRP_RNE_InitTimeState(DETECT_OUTPUT_GP_LAYER, MAX_LAYER_TYPE - SLICE_GP_LAYER, &time);
	 TS_MPI_TRP_RNE_NetBindTimeState(nModel, &time);
#endif

//	TS_U32 time0 = 0;//TIME_CACL_GET();
	//TIME_CACL_GET();
	RNE_BLOBS_S *outputBlobs = TS_MPI_TRP_RNE_Forward(nModel);
	if (outputBlobs == TS_NULL) {
		ALG_LOGE("net forward error!\n");
		#ifdef TIME_CONSUME_LAYER
		TS_MPI_TRP_RNE_ReleaseTimeState(&time);
		#endif
		return -1;
	}

//	TS_U32 time1 = 1;//TIME_CACL_GET();
	//ALG_LOGE("rne forward time:%d\n",time1-time0);
	//printf("%s,%d\n",__FUNCTION__,__LINE__);
#ifdef TIME_CONSUME_LAYER
	ALG_LOGD("total time:%lld us\n", TS_MPI_TRP_RNE_GetTotalTime(&time));
	ALG_LOGD("forward time:%lld us\n", TS_MPI_TRP_RNE_GetTimeOfForward(&time));
	ALG_LOGD("hw layer time:%lld us\n", TS_MPI_TRP_RNE_GetTimeOfHwLayer(&time));
	ALG_LOGD("gp layer time:%lld us\n", TS_MPI_TRP_RNE_GetTimeOfGpLayer(&time));
	TS_MPI_TRP_RNE_ReleaseTimeState(&time);
#endif

	//ALG_LOGI("net body detect forward done\n");
	//ALG_LOGI("outputBlobs->u32NBlob:%d\n", outputBlobs->u32NBlob);

	if(outputBlobs->u32NBlob <= 0 || TS_NULL == outputBlobs->stpBlob) {
		ALG_LOGE("net forward no result!\n");
		return -1;
	}

	//ALG_LOGD("totol u32NBlob:%d\n", outputBlobs->u32NBlob);
	// 检查是否有足够的blob数量
	if(outputBlobs->u32NBlob < 3) {
		ALG_LOGE("insufficient blobs: expected at least 3, got %d\n", outputBlobs->u32NBlob);
		return -1;
	}
	
	TS_U8 *resultAddr[3] = {TS_NULL, TS_NULL, TS_NULL};
    unsigned int s32C[16];
    
	// 检查每个vpAddr是否为空
	for(int i = 0; i < 3; i++) {
		if(TS_NULL == outputBlobs->stpBlob[i].vpAddr) {
			ALG_LOGE("blob[%d] vpAddr is NULL\n", i);
			return -1;
		}
		resultAddr[i] = (TS_U8 *)outputBlobs->stpBlob[i].vpAddr;
	}
	for(TS_U32 i = 0; i < outputBlobs->u32NBlob; i++) {

		const int c = outputBlobs->stpBlob[i].s32C;
		const int s32BitNum = outputBlobs->stpBlob[i].s32BitNum;
		const int c_align = TS_MPI_TRP_RNE_CStride(c, s32BitNum, outputBlobs->stpBlob[i].bIsJoined);

		cstride[i] = c_align;
		s32C[i] = c;
		//fcoeff[i] = outputBlobs->stpBlob[i].fCoeff[0];
		fcoeff[i] = *(outputBlobs->stpBlob[i].fCoeff);
		// ALG_LOGD("u32NBlob:%d H: %d, W:%d, C:%d, Cstride:%d, coeff: %f\n", i, outputBlobs->stpBlob[i].s32H, outputBlobs->stpBlob[i].s32W, outputBlobs->stpBlob[i].s32C, c_align, outputBlobs->stpBlob[i].fCoeff[0]);
	}

    float* dataVec = (float*)gPostProcBuf;
	int len = TS_ALG_PcppDetV12_PostProcess(resultAddr, cstride, s32C, fcoeff, dataVec);
    //printf("len:%d\n", len);

    int maxNms = 100;
    //int *nmsOut = (int *)(dataVec + len * 5);
    // 定义一个足够大的 NMS 输出索引数组
#define MAX_BOX_COUNT 10000
int nmsOut[MAX_BOX_COUNT];

    // 修复调试代码越界问题：只在len >= 5时才打印
//     for (int i = 0; i < 5 && i < len; i++) {
//     printf("box%d: %f %f %f %f  score:%f\\n",
//         i,
//         gPostProcBuf[i*6+0],
//         gPostProcBuf[i*6+1],
//         gPostProcBuf[i*6+2],
//         gPostProcBuf[i*6+3],
//         gPostProcBuf[i*6+5]
//     );
// }

    int outNum = nms(gPostProcBuf, len, 6, nmsOut, 0.45, maxNms);
	result->u32ObjNum = outNum;

    TS_ALG_CatDetect_GetParam(&det_param_cpp);

	int j = 0;

	// for (int i = 0; i < outNum; i++) {
	// 	// printf("[NMS] i=%d, nmsOut[i]=%d, conf=%.6f\n",
	// 	//        i, nmsOut[i], gPostProcBuf[nmsOut[i] * 6 + 5]);
	// 	if(gPostProcBuf[nmsOut[i] * 6 + 5] <= det_param_cpp.DetectionConfThres){
	// 		continue;
	// 	}
	// 	result->stBox[j].f32Xmin = gPostProcBuf[nmsOut[i] * 6 + 0]/640.0;
	// 	result->stBox[j].f32Ymin = MAX(0, (gPostProcBuf[nmsOut[i] * 6 + 1]-12.0) / 360.0);
	// 	result->stBox[j].f32Xmax = gPostProcBuf[nmsOut[i] * 6 + 2]/640.0;
	// 	result->stBox[j].f32Ymax = MAX(0, (gPostProcBuf[nmsOut[i] * 6 + 3]-12.0) / 360.0);
    //     result->stBox[j].DetectionConf = gPostProcBuf[nmsOut[i] * 6 + 5];
	// 	result->stBox[j].class_id = (int)(gPostProcBuf[nmsOut[i] * 6 + 4]);
	// 	// printf("[NMS] j=%d, class_id=%d, conf=%.6f\n", j, result->stBox[j].class_id, result->stBox[j].DetectionConf);
	// 	j++;
	// }
    for (int i = 0; i < outNum; i++) {
    // 添加边界检查，防止nmsOut索引越界
    if (nmsOut[i] < 0 || nmsOut[i] >= len) {
        ALG_LOGE("nmsOut[%d]=%d out of range [0, %d)\n", i, nmsOut[i], len);
        continue;
    }
    
    if(gPostProcBuf[nmsOut[i] * 6 + 5] <= det_param_cpp.DetectionConfThres) {
        continue;
    }

    // 取出中心、宽高
    float cx = gPostProcBuf[nmsOut[i] * 6 + 0];
    float cy = gPostProcBuf[nmsOut[i] * 6 + 1];
    float w  = gPostProcBuf[nmsOut[i] * 6 + 2];
    float h  = gPostProcBuf[nmsOut[i] * 6 + 3];

    // 算出真实左上右下
    float xmin = cx - w * 0.5f;
    float ymin = cy - h * 0.5f;
    float xmax = cx + w * 0.5f;
    float ymax = cy + h * 0.5f;

    // 归一化到0~1
    result->stBox[j].f32Xmin = xmin / 640.0f;
    result->stBox[j].f32Ymin = (ymin - 12.0f) / 360.0f;
    result->stBox[j].f32Xmax = xmax / 640.0f;
    result->stBox[j].f32Ymax = (ymax - 12.0f) / 360.0f;

    result->stBox[j].DetectionConf = gPostProcBuf[nmsOut[i] * 6 + 5];
    result->stBox[j].class_id = (int)(gPostProcBuf[nmsOut[i] * 6 + 4]);
    j++;
}
 

	result->u32ObjNum = j;
    //ALG_LOGE("rne postprocess time:%d\n",TIME_CACL_GET()-time1);

#endif

	return 0;
}

TS_S32 TS_ALG_BodyDetect_SetParam(TS_VOID *handle, ALG_BODYDET_PARAM_S *param)
{
	if(TS_NULL == param) {
		ALG_LOGE("error, Invalid parameter!\n");
		return -1;
	}

	gThreshold = param->f32Thresh;
	ALG_LOGD("body detect thresh:%f\n", gThreshold);
	return 0;
}

TS_S32 TS_ALG_BodyDetect_GetParam(TS_VOID *handle, ALG_BODYDET_PARAM_S *param)
{
	if(TS_NULL == param) {
		ALG_LOGE("error, Invalid parameter!\n");
		return -1;
	}

	param->f32Thresh = gThreshold;
	return 0;
}

const TS_CHAR* TS_ALG_BodyDetect_GetVersion(TS_VOID)
{
    static const TS_CHAR *ver = "bodyDetect_v1.5_model_0712";

    return (const TS_CHAR *)ver;
}
