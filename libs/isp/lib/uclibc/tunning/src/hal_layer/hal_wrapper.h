/************************************************************************************************
 * This source code is CONFIDENTIAL and any use by you is subject to the terms of the agreement
 * between you and Arm China or the terms of the agreement between you and the party authorised
 * by Arm China to disclose this document to you.
 *
 * The confidential and proprietary information contained in this file may only be used by a
 * person authorised under and to the extent permitted by a subsisting licensing agreement from
 * Arm China.
 *
 * This entire notice must be reproduced on all copies of this file and copies of this file may
 * only be made by a person if such person is permitted to do so under the terms of a subsisting
 * license agreement from Arm China.
 *
 * Release Information: Linlon-i5-r0p2-00eac0
 *
 * Copyright (C) 2021 Arm Technology (China) Co. Ltd. All rights reserved.
 *
 ***********************************************************************************************/

#ifndef __HAL_WRAPPER_H__
#define __HAL_WRAPPER_H__

#include "types_utils.h"
#include "log_utils.h"
#include "hal.h"

/// Hal ops

/*
* Hal_Wrapper_Process_Result
*
* @brief Result callback of HAL wrapper
*
* @param pCbs: pointer to callbacks_ops_t
*
* @param pResult: pointer to result_t
*
* @return void
*/
void Hal_Wrapper_Process_Result(callbacks_ops_t* pCbs, result_t* pResult);

/*
* Hal_Wrapper_Notify
*
* @brief Notify callback of HAL wrapper
*
* @param pCbs: pointer to callbacks_ops_t
*
* @param pResult: pointer to result_t
*
* @return void
*/
void Hal_Wrapper_Notify(callbacks_ops_t* pCbs, notify_t* pNotifyMsg);

/*
* Hal_Wrapper_Probe
*
* @brief Probe camera via HAL wrapper
*
* @param listProbeInfo: list to store probe information
*
* @return ISP_RET_SUCCESS if success, others if failed
*/
isp_ret_t Hal_Wrapper_Probe(obj_list_t* listProbeInfo);

/*
* Hal_Wrapper_Open
*
* @brief Open camera via HAL wrapper
*
* @param camId: camera ID
*
* @return ISP_RET_SUCCESS if success, others if failed
*/
isp_ret_t Hal_Wrapper_Open(u32 camId);

/*
* Hal_Wrapper_Get_Capability
*
* @brief Get camera capability via HAL wrapper
*
* @param camId: camera index
*
* @param pMetadata: pointer to metadata
*
* @return ISP_RET_SUCCESS if success, others if failed
*/
isp_ret_t Hal_Wrapper_Get_Capability(u32 camId,
                                             metadata_t* pMetadata);

/*
* Hal_Wrapper_Initialize
*
* @brief Initialize camera via HAL wrapper
*
* @param camId: camera index
*
* @param pCbs: pointer to callbacks_ops_t
*
* @return ISP_RET_SUCCESS if success, others if failed
*/
isp_ret_t Hal_Wrapper_Initialize(u32 camId,
                                        callbacks_ops_t* pCbs);

/*
* Hal_Wrapper_Configure_Streams
*
* @brief Configure camera via HAL wrapper
*
* @param camId: camera index
*
* @param pStreamCfg: pointer to stream_config_t
*
* @return ISP_RET_SUCCESS if success, others if failed
*/
isp_ret_t Hal_Wrapper_Configure_Streams(u32 camId,
                                                 stream_config_t* pStreamCfg);

/*
* Hal_Wrapper_Process_Request
*
* @brief Process camera request via HAL wrapper
*
* @param camId: camera index
*
* @param pRequest: pointer to request_t
*
* @return ISP_RET_SUCCESS if success, others if failed
*/
isp_ret_t Hal_Wrapper_Process_Request(u32 camId,
                                              request_t* pRequest);

/*
* Hal_Wrapper_Flush
*
* @brief Flush camera via HAL wrapper
*
* @param camId: camera index
*
* @return ISP_RET_SUCCESS if success, others if failed
*/
isp_ret_t Hal_Wrapper_Flush(u32 camId);

/*
* Hal_Wrapper_Dump
*
* @brief Dump camera via HAL wrapper
*
* @param camId: camera index
*
* @param dumpFlag: dump flag
*
* @return ISP_RET_SUCCESS if success, others if failed
*/
isp_ret_t Hal_Wrapper_Dump(u32 camId, u64 dumpFlag);

/*
* Hal_Wrapper_Close
*
* @brief Close camera via HAL wrapper
*
* @param camId: camera index
*
* @return ISP_RET_SUCCESS if success, others if failed
*/
isp_ret_t Hal_Wrapper_Close(u32 camId);


/// Buf Mgr ops

/*
* Hal_Wrapper_Buf_Mgr_Acquire
*
* @brief Acquire buffer manager via HAL wrapper
*
* @param pStreamInfo: pointer to stream_info_t
*
* @return ISP_RET_SUCCESS if success, others if failed
*/
isp_ret_t Hal_Wrapper_Buf_Mgr_Acquire(stream_info_t*          pStreamInfo);

/*
* Hal_Wrapper_Buf_Mgr_Release
*
* @brief Release buffer manager via HAL wrapper
*
* @param pStreamInfo: pointer to stream_info_t
*
* @return ISP_RET_SUCCESS if success, others if failed
*/
isp_ret_t Hal_Wrapper_Buf_Mgr_Release(stream_info_t*          pStreamInfo);

/*
* Hal_Wrapper_Buf_Get
*
* @brief Get one free buffer via HAL wrapper
*
* @param pStreamInfo: pointer to stream_info_t
*
* @return pointer to stream_buffer_t
*/
stream_buffer_t* Hal_Wrapper_Buf_Get(stream_info_t*          pStreamInfo);

/*
* Hal_Wrapper_Buf_Return
*
* @brief Return one buffer via HAL wrapper
*
* @param pStreamBuf: pointer to stream_buffer_t
*
* @return pointer to stream_buffer_t
*/
isp_ret_t Hal_Wrapper_Buf_Return(stream_buffer_t*         pStreamBuf);

#endif
