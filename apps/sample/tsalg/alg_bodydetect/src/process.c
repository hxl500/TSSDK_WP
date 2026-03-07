#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ts_alg_body_detect_demo.h"
#include "ts_alg_log.h"
#include "process_demo.h"

// #define MODEL_TEST

#ifdef MODEL_TEST
    #include "postprocess_input.h"
#endif

#if 1
static int nms(float *torchCat, int len1, int len2, int *nmsOut, float thresh, int maxDet) 
{
    float tmpX, tmpY, tmpW, tmpH;

    float* pScore = (float *)(nmsOut + len1);
    int* box_idx = (int *)(pScore + len1);
    float* areas = (float*)(box_idx + len1);
    for (int i = 0; i < len1; i++)
    {
        box_idx[i] = i;
        pScore[i] = torchCat[len2 * i + 5];
        tmpX = torchCat[len2 * i + 0];
        tmpY = torchCat[len2 * i + 1];
        tmpW = torchCat[len2 * i + 2];
        tmpH = torchCat[len2 * i + 3];
        areas[i] = tmpW * tmpH;
        torchCat[len2 * i + 0] = tmpX - tmpW / 2;
        torchCat[len2 * i + 1] = tmpY - tmpH / 2;
        torchCat[len2 * i + 2] = tmpX + tmpW / 2;
        torchCat[len2 * i + 3] = tmpY + tmpH / 2;
    }

    for (int i = 0; i < len1; i++) {
        float max = pScore[i];
        int idx_t = i;
        for (int j = i + 1; j < len1; j++) {
            if (max < pScore[j]) {
                max = pScore[j];
                idx_t = j;
            }
        }
        int tt = box_idx[i];
        box_idx[i] = box_idx[idx_t];
        box_idx[idx_t] = tt;
        float tt1 = pScore[i];
        pScore[i] = pScore[idx_t];
        pScore[idx_t] = tt1;
    }

    int	cnt = 0;
    int sSzie = len1;
    while (sSzie > 0) {// torch.numel()返回张量元素个数
        if (sSzie == 1) {//    保留框只剩一个
            nmsOut[cnt] = box_idx[0];
            cnt++;
            break;
        }
        else {
            nmsOut[cnt] = box_idx[0];
            cnt++;
        }
        if (cnt >= maxDet) {
            break;
        }

		//计算box[i]与其余各框的IOU(思路很好)
		//auto orderMask = order.narrow(1, order.m_size - 1, 0);
		//x1.index({ orderMask });
		//x1.index({ orderMask }).clamp(x1[keep.back()].item().toFloat(), 1e10);
		//auto xx1 = x1.index({ orderMask }).clamp(x1.m_data[keep.back()],1e10);// [N - 1, ]
		//auto yy1 = y1.index({ orderMask }).clamp(y1.m_data[keep.back()], 1e10);
		//auto xx2 = x2.index({ orderMask }).clamp(0, x2.m_data[keep.back()]);
		//auto yy2 = y2.index({ orderMask }).clamp(0, y2.m_data[keep.back()]);
        float *xx1 = (float *)(areas + len1);
        float *yy1 = (float *)(xx1 + sSzie);
        float *xx2 = (float *)(yy1 + sSzie);
        float *yy2 = (float *)(xx2 + sSzie);
        for (int i=1; i<sSzie; i++){
            xx1[i-1] = MIN(MAX(torchCat[len2*box_idx[0] + 0], torchCat[len2*box_idx[i] + 0]), 1e10);
            yy1[i-1] = MIN(MAX(torchCat[len2*box_idx[0] + 1], torchCat[len2*box_idx[i] + 1]), 1e10);
            xx2[i-1] = MIN(MAX(0, torchCat[len2*box_idx[i] + 2]), torchCat[len2*box_idx[0] + 2]);
            yy2[i-1] = MIN(MAX(0, torchCat[len2*box_idx[i] + 3]), torchCat[len2*box_idx[0] + 3]);
        }

		// auto selWidth = xx2 - xx1;
        // auto selHeight = yy2 - yy1;
        // auto selWidth_ = selWidth.clamp(0, 1e10);
        // auto selHeight_= selHeight.clamp(0, 1e10);
		// auto inter = selWidth_ * selHeight_;// [N - 1, ]
        // xx2 = inter
        for (int i=0; i<sSzie-1; i++){
            xx1[i] = MIN(MAX(0, xx2[i] - xx1[i]), 1e10);
            yy1[i] = MIN(MAX(0, yy2[i] - yy1[i]), 1e10);
            xx2[i] = xx1[i] * yy1[i];
        }

        // auto tmpNarrow = order.narrow(1,order.m_size - 1, 0) ;
        // auto merge = areas.index(tmpNarrow) + areas.m_data[keep.back()] - inter;
		// auto iou = inter / (merge);//[N - 1, ]
        //xx1 = merge
        //yy1 = iou
        //yy2 = idx
        // auto idx = cmpnonzero(iou, thresh);
        int idx_len = 0;
        int *newbox_idx = (int *)yy2;
        //int* idx = (int*)yy2;
        for (int i = 1; i < sSzie; i++) {
            xx1[i - 1] = areas[box_idx[i]] + areas[box_idx[0]] - xx2[i - 1];
            yy1[i - 1] = xx2[i - 1] / xx1[i - 1];

            if (yy1[i - 1] < thresh) {
                newbox_idx[idx_len] = box_idx[i];
                idx_len++;
            }
        }

        if (idx_len == 0) {
            break;
        }

        // auto tmpIndex = idx + 1;
        // tmpIndex.m_type = 2;
        // order = order.index(tmpIndex); //修补索引之间的差值
        memcpy(box_idx, newbox_idx, idx_len * sizeof(int));
        sSzie = idx_len - 1;
    }
    return cnt;
}
#endif
//针对 nc = 1

void ts_bodydetect_post_process(TS_U8 **blob, ALG_BODY_RESULT_S *out, TS_U32 *cstride, TS_FLOAT *fcoeff, float confThres, TS_FLOAT *pPostbuf)
{
	int nblob = 3;
	int imShape[4] = {1, 3, 384, 640}; // = srcShape
	int shapeVec2[3][5] = {{1, 3, 48, 80, 6}, {1, 3, 24, 40, 6}, {1, 3, 12, 20, 6}};//0,1,2
#ifndef MODEL_TEST    
    float scale_output[] = { fcoeff[0], fcoeff[1], fcoeff[2] }; //1,2,3
    float scale_output2[] = { fcoeff[0] * 2, fcoeff[1] * 2, fcoeff[2] * 2 }; //1,2,3
#endif
    int stride[3] = { 8,16,32 };
    // const float confThres = 0.45;    // confidence threshold
    int len = 0;

    //TS_U32 time1 = TIME_CACL_GET();
	//初始化先验眶
    int anch[3][6] = {{10,13, 16,30, 33,23}, {30,61, 62,45, 59,119}, {116,90, 156,198, 373,326} };
    //std::vector<std::vector<int> > anch = { {10,13, 16,30, 33,23}, {30,61, 62,45, 59,119}, {116,90, 156,198, 373,326}  };

    float value_det = 0.5f; //_makeGrid : float value = 0.5;
    float* dataVec = (float*)pPostbuf;    //362,880
	//TS_U32 time2 = TIME_CACL_GET();
    // memset(dataVec, 0, sizeof(float) * (shapeVec3[0] * shapeVec3[1] * shapeVec3[2]);

#ifdef MODEL_TEST
    float *blob_model[3];
    int no_yolo = 6;
    int idxtmp = 0;
    blob_model[0] = tt0;
    blob_model[1] = tt1;
    blob_model[2] = tt2;
    for (int i = 0; i < nblob; i++)
    {
        for (int j = 0; j < shapeVec2[i][1]; j++) {
            int* anchorgrid = &(anch[i][j * 2]);
            for (int k = 0; k < shapeVec2[i][2]; k++) {
                for (int m = 0; m < shapeVec2[i][3]; m++) {
                    float *dataf = blob_model[i] + j * no_yolo + k * shapeVec2[i][3] * 18 + m * 18;

                    //Feature_gtf(pred.m_data, mask, pred.m_shape[0] * pred.m_shape[1], confThres, pred.m_shape[2], 4);
                    float data = (dataf[4]) * (dataf[5]);
                    if (data > confThres) {
                        float griddataf0 = (float)m - value_det;
                        float griddataf1 = (float)k - value_det;

                        //y_tmp = ((y.index("...", sha3) * 2.0) + this->grid[i]) * this->stride[i];
                        dataVec[idxtmp++] = (dataf[0] * 2.0 + griddataf0) * stride[i];
                        dataVec[idxtmp++] = (dataf[1] * 2.0 + griddataf1) * stride[i];

                        // y_tmp = (y.index("...", sha4) * 2).pow(2) * this->anchorGrid[i];
                        float value = (dataf[2] * 2.0);
                        dataVec[idxtmp++] = value * value * anchorgrid[0];
                        value = (dataf[3] * 2.0);
                        dataVec[idxtmp++] = value * value * anchorgrid[1];

                        dataVec[idxtmp++] = dataf[4];
                        // dataVec[idxtmp++] = dataf[5] * scale_output[i];
                        dataVec[idxtmp++] = data;

                        len++;
                    }
               }
           }
       }
    }
#else
    int no_yolo = 6;
    int idxtmp = 0;
    int maxNms = 10000;  // maximum number of boxes into torchvision.ops.nms()
    for (int i = 0; i < nblob; i++)
    {
        for (int j = 0; j < shapeVec2[i][1]; j++) {
            int* anchorgrid = &(anch[i][j * 2]);
            for (int k = 0; k < shapeVec2[i][2]; k++) {
                for (int m = 0; m < shapeVec2[i][3]; m++) {
                    TS_U8 *dataf = blob[i] + j * no_yolo + k * shapeVec2[i][3] * cstride[i] + m * cstride[i];

                    //Feature_gtf(pred.m_data, mask, pred.m_shape[0] * pred.m_shape[1], confThres, pred.m_shape[2], 4);
                    float data = (dataf[4] * scale_output[i]) * (dataf[5] * scale_output[i]);
                    if (data > confThres) {
                        if(len < maxNms){
                            
                            float griddataf0 = (float)m - value_det;
                            float griddataf1 = (float)k - value_det;

                            //y_tmp = ((y.index("...", sha3) * 2.0) + this->grid[i]) * this->stride[i];
                            dataVec[idxtmp++] = (dataf[0] * scale_output2[i] + griddataf0) * stride[i];
                            dataVec[idxtmp++] = (dataf[1] * scale_output2[i] + griddataf1) * stride[i];

                            // y_tmp = (y.index("...", sha4) * 2).pow(2) * this->anchorGrid[i];
                            float value = (dataf[2] * scale_output2[i]);
                            dataVec[idxtmp++] = value * value * anchorgrid[0];
                            value = (dataf[3] * scale_output2[i]);
                            dataVec[idxtmp++] = value * value * anchorgrid[1];

                            dataVec[idxtmp++] = dataf[4] * scale_output[i];
                            // dataVec[idxtmp++] = dataf[5] * scale_output[i];
                            dataVec[idxtmp++] = data;

                            len++;
                        }
                    }
               }
           }
       }
    }
#endif
    //int shapeVec4[] = { len, no_yolo};//0,1,2
    //Feature torchCat(dataVec, shapeVec4, 0);

    //TS_U32 time3 = TIME_CACL_GET();
    const float iouThres = 0.45;     // NMS IOU threshold
    // const bool agnosticNms = false; // class-agnostic NMS
    const int maxDet = 1000;       // maximum detections per image
    int *nmsOut = (int *)(dataVec + len * no_yolo);
    // if (33 == frameNo1)
	// {
    //     ALG_LOGD("before nms, len = %d\n", len);
    //     for(int i=0; i<len; i++){
    //         ALG_LOGD("%f, %f, %f, %f, %f, %f\n", dataVec[i*no_yolo + 0], dataVec[i*no_yolo + 1], dataVec[i*no_yolo + 2], dataVec[i*no_yolo + 3], dataVec[i*no_yolo + 4], dataVec[i*no_yolo + 5]);
    //     }
    // }

    //int outNum = nonMaxSuppression(dataVec, confThres, iouThres, agnosticNms, maxDet, false, len, no_yolo, nmsOut);
    int outNum = nms(dataVec, len, no_yolo, nmsOut, iouThres, maxDet);  // NMS
    //std::vector<Feature> nmsOut = nonMaxSuppression(torchCat, confThres, iouThres, agnosticNms, maxDet);
#if 1
	//TS_U32 time4 = TIME_CACL_GET();
	//printf("DetectProcess : %d ms, postProcess : %d ms, nonMaxSuppression : %d ms, scaleCoords : %d ms, \n",
	//		time2-time1, time3-time2, time4-time3, time5-time4);
    // ALG_LOGD("after MaxSuppression\n");
    // for(int i=0; i<outNum; i++){
    //     ALG_LOGD("%f, %f, %f, %f, %f, %f\n", dataVec[nmsOut[i]*no_yolo + 0], dataVec[nmsOut[i]*no_yolo + 1], dataVec[nmsOut[i]*no_yolo + 2], dataVec[nmsOut[i]*no_yolo + 3], dataVec[nmsOut[i]*no_yolo + 4], dataVec[nmsOut[i]*no_yolo + 5]);
    // }

    int height = imShape[2];
    int width = imShape[3];
    
	out->u32BodyNum = MIN(outNum, MAX_BODY_DETECT_NUM);
    for(unsigned int i = 0; i < out->u32BodyNum; i++){
        
        //scaleCoords
        out->stBox[i].f32Xmin = MAX(0, MIN(dataVec[nmsOut[i] * no_yolo + 0], width));
        out->stBox[i].f32Ymin = MAX(0, MIN(dataVec[nmsOut[i] * no_yolo + 1], height));
        out->stBox[i].f32Xmax = MAX(0, MIN(dataVec[nmsOut[i] * no_yolo + 2], width));
        out->stBox[i].f32Ymax = MAX(0, MIN(dataVec[nmsOut[i] * no_yolo + 3], height));
        out->stBox[i].f32Score = dataVec[nmsOut[i] * no_yolo + 5];
    }
    #endif
}
