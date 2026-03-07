#ifndef __TS_ALG_INFO_H__
#define __TS_ALG_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "ts_alg_type.h"


#define EN_ALG_GETTIME

#define ALG_MAX_TIMES_NUM 20

typedef enum tsALG_TYPE_E {
	ALG_CV,        //Alg of Computer Vision
	ALG_SP,        //Alg of Sound Process
    ALG_ELSE       //Else
} ALG_TYPE_E ;

typedef enum tsALG_STATUS_TYPE_E {
	ALG_RT_FAILURE = -1,
	ALG_RT_SUCCESS = 0
} ALG_STATUS_TYPE_E;

typedef struct tsALG_HDWInfo_S
{
    TS_FLOAT f32CPUUsage;
    TS_U32   u32MemUsage;
    TS_U32   u32VMemUsage;
    TS_U32   u32CMAUsage;
}ALG_HDWInfo_S;


typedef struct tsALG_NetInfo_S{
    TS_U32  u32CfgSize;
    TS_U32  u32WeightSize;
    TS_U32  u32BlobSize;
    TS_U32  u32CfgsSize[10];
    TS_U32  u32WeightsSize[10];
    TS_U32  u32BlobsSize[10];
    TS_CHAR *pAlgType;
    TS_CHAR *pInputType;
    TS_CHAR *pVersion;
    TS_U32  u32ObjDet;   //Detect Obj Num
    TS_U32  u32ObjGT;    //Ground Truth Obj Num
}ALG_NetInfo_S;

typedef struct tsALG_TimeInfo_S{
    TS_CHAR  strTmNames[ALG_MAX_TIMES_NUM][50];   //name
    TS_CHAR  strTmUnits[ALG_MAX_TIMES_NUM][10];   //unit
    TS_FLOAT f32Times[ALG_MAX_TIMES_NUM];         //time value
}ALG_TimeInfo_S;


typedef struct tsALG_StatInfo_S{
    ALG_TYPE_E         emAlgType;     
    ALG_STATUS_TYPE_E  emStatus; 
    ALG_TimeInfo_S stTimeInfo;
    ALG_NetInfo_S  stNetInfo;
    ALG_HDWInfo_S  stHDWInfo;
}ALG_StatInfo_S;


TS_FLOAT TS_ALG_GetTimeMS();
TS_S32   TS_ALG_GetStateInfo(TS_VOID *pHandle, ALG_StatInfo_S *pParam);
TS_S32   TS_ALG_SetStateInfo_Times(ALG_StatInfo_S *pParam, TS_CHAR *pName, TS_CHAR *pUnit, TS_FLOAT f32Time, TS_U32 idx);
TS_CHAR* TS_ALG_GetAlgType(TS_U32 index);
TS_CHAR* TS_ALG_TimesStrConcat(ALG_StatInfo_S *pParam, TS_S32 useType, TS_U32 idxTimes);

//**************** Queue *******************/
#define TS_ALG_QUEUE_MAX_SIZE   250    // queue size
#define TS_ALG_QUEUE_MAX_VALUE  1024   // max value of queue
typedef struct {
    int  queue[TS_ALG_QUEUE_MAX_SIZE];
    int  front;
    int  rear;
    int  size;
    bool exists[TS_ALG_QUEUE_MAX_VALUE + 1];
} ALG_FIFOQueue;

void TS_ALG_InitQueue(ALG_FIFOQueue *q);
bool TS_ALG_Dequeue(ALG_FIFOQueue *q, int *value);
bool TS_ALG_Enqueue(ALG_FIFOQueue *q, int value);
bool TS_ALG_RemoveElement(ALG_FIFOQueue *q, int value);
void TS_ALG_PrintQueue(ALG_FIFOQueue *q);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

