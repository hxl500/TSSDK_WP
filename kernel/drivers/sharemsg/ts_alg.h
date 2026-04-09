#ifndef _TS_ALGO_H_
#define _TS_ALGO_H_

#define IMAGE_WIDTH		640
#define IMAGE_HEIGHT	384

#define MAX_PCPP_DET_NUM     25		// max obj detect number

typedef struct tsALG_PCPP_DET_BOX_S {
    // the range of x and y is 0.0f~1.0f
    float f32Xmin;
    float f32Ymin;
    float f32Xmax;
    float f32Ymax;
    float f32Score;
    float f32Reserve;
} ALG_PCPP_DET_BOX_S;

typedef struct tsALG_PCPP_DET_RESULT_S {
	uint32_t u32ObjNum;
	ALG_PCPP_DET_BOX_S stBox[MAX_PCPP_DET_NUM];
	uint8_t u8ObjNo[MAX_PCPP_DET_NUM];
} ALG_PCPP_DET_RESULT_S;

enum {
    ALG_PCPP_DET_PEDESTRIAN,
    ALG_PCPP_DET_CAR,
    ALG_PCPP_DET_PET,
    ALG_PCPP_DET_PACKAGE,
    ALG_PCPP_DET_NUM
};

#endif
