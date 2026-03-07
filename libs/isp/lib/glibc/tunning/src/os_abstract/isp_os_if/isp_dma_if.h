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

#ifndef __ISP_DMA_H__
#define __ISP_DMA_H__

#include "types_utils.h"
#include "log_utils.h"
#include "fs_osutils.h"
#include "isp_buf_def.h"

#ifdef __linux__
#include "isp_dma_v4l.h"
#else
#include "isp_dma_bare.h"
#endif


/*
* Isp_Xdma
*
* @brief Copy buffer between local and remote via cdma
*
* @param rmtAddr: memory address of 440
*
* @param localAddr: memory address of 7020
*
* @param size: bytes to copy
*
* @param dirt: DMA copy direction
*
* @return success/failure
*/
isp_ret_t Isp_Xdma(u32 rmtAddr, u32 localAddr, u32 size, dma_direction_t dirt);


/*
* Isp_Dma_Copy
*
* @brief Copy buffer between local and remote via cdma
*
* @param dstAddress: dst address
*
* @param srcAddress: src address
*
* @param bytes: bytes to copy
*
* @param dirt: DMA copy direction
*
* @return success/failure
*/
isp_ret_t Isp_Dma_Copy(u32 dstAddress, u32* srcAddress, u32 bytes, dma_direction_t dirt);

/*
* Isp_Dma_Copy_To_SD
*
* @brief Copy remote mem to SD
*
* @param rmtAddr: remote address
*
* @param frameSize: Size per frame
*
* @param frameCnt: numbers of frames
*
* @param pFileName: filename for saving
*
* @return success/failure
*/
isp_ret_t Isp_Dma_Copy_To_SD(u32 rmtAddr, u32 frameSize, u32 frameCnt, char8 *pFileName);

/*
* Isp_Dma_Copy_2Addr_To_SD
*
* @brief Copy remote mem to SD
*
* @param rmtAddr0: remote address 0
*
* @param rmtAddr1: remote address 1
*
* @param addrSize0: size of block 0
*
* @param addrSize1: size of block 1
*
* @param frameCnt: numbers of frames
*
* @param pFileName: filename for saving
*
* @return success/failure
*/

isp_ret_t Isp_Dma_Copy_2Addr_To_SD(u32 rmtAddr0, u32 rmtAddr1, u32 addrSize0, u32 addrSize1, u32 frameCnt, char8 *pFileName);


/*
* Isp_Dma_Copy_Stats
*
* @brief Copy stats via cdma
*
* @param camId: camera idx
*
* @param pBufferAddress: pointer to isp remote buffer address
*
* @param pHw3aStatsBuf: pointer to dst stats buffer
*
* @return success/failure
*/
isp_ret_t Isp_Dma_Copy_Stats(u32 camId, const isp_buffer_address_t *pBufferAddress, hw3a_stats_t *pHw3aStatsBuf);

/*
* Isp_Dma_Load_Mem
*
* @brief Load file to memory
*
* @param pFileName: filename for saving
*
* @param paddr: Phy address
*
* @return success/failure
*/
isp_ret_t Isp_Dma_Load_Mem(char *file_name, u32 paddr);

#endif
