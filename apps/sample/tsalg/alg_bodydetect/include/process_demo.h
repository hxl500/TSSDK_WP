/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: the definitions of body detection
 * Author: Tsing Micro solution-application group
 * Create: 2022/08/24
 */
#ifndef  _TS_ALG_BODY_DETECT_PROCESS_DEMO_H_
#define _TS_ALG_BODY_DETECT_PROCESS_DEMO_H_

void ts_bodydetect_post_process(TS_U8 **blob, ALG_BODY_RESULT_S *result, TS_U32 *cstride, TS_FLOAT *fcoeff, float confThres, TS_FLOAT *pPostbuf);

#endif /* _TS_ALG_BODY_DETECT_PROCESS_H_ */
