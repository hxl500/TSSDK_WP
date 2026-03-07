#ifndef __TS_ALG_RING_QUEUQ_H__
#define __TS_ALG_RING_QUEUQ_H__

#ifdef __cplusplus
	extern "C"{
#endif

#include "ts_alg_type.h"

typedef struct{
	TS_U8 * addr;
	TS_S32 byte_len;
	TS_S32 r_off;
	TS_S32 w_off;
 }ALG_RINGQUEUE_S;

 TS_S32 TS_ALG_RingQueue_Init(ALG_RINGQUEUE_S *ring_buff, TS_S32 byte_len);

 TS_VOID TS_ALG_RingQueue_Deinit(ALG_RINGQUEUE_S *ring_buff);

 TS_S32 TS_ALG_RingQueue_Write(ALG_RINGQUEUE_S *ring_buff, TS_U8 *data, TS_S32 byte_len);

 TS_S32 TS_ALG_RingQueue_Read(ALG_RINGQUEUE_S *ring_buff, TS_U8 *out, TS_S32 byte_len);

 TS_S32 TS_ALG_RingQueue_Peek(ALG_RINGQUEUE_S *ring_buff, TS_U8 *out, TS_S32 byte_len);

 TS_S32 TS_ALG_RingQueue_Remove(ALG_RINGQUEUE_S *ring_buff, TS_S32 remove_byte_len);

 TS_S32 TS_ALG_RingQueue_IsFull(ALG_RINGQUEUE_S *ring_buff);

 TS_S32 TS_ALG_RingQueue_IsEmpty(ALG_RINGQUEUE_S *ring_buff);

 TS_U8 *TS_ALG_RingQueue_GetRdPtr(ALG_RINGQUEUE_S *ring_buff);

 TS_U8 *TS_ALG_RingQueue_GetWrPtr(ALG_RINGQUEUE_S *ring_buff);

 TS_S32 TS_ALG_RingQueue_Taken(ALG_RINGQUEUE_S *ring_buff);

 TS_S32 TS_ALG_RingQueue_Available(ALG_RINGQUEUE_S *ring_buff);

 TS_S32 TS_ALG_RingQueue_Empty(ALG_RINGQUEUE_S *ring_buff);

 #ifdef __cplusplus
	}
#endif

#endif/* __TS_ALG_RING_QUEUQ_H__ */