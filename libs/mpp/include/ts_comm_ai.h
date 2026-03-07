/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_comm_ai.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __TS_COMM_AI_H__
#define __TS_COMM_AI_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


typedef void (* pf_ai_filldone_callback)(void *hdl, int idx, int *pause, int qnum);
typedef void (* pf_ai_flush_callback)(void *hdl);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __TS_COMM_AI_H__ */

