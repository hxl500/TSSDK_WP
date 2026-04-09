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
#ifndef __ISP_DMA_V4L_H__
#define __ISP_DMA_V4L_H__

#ifdef __linux__

#include "types_utils.h"
#include "log_utils.h"
#include "fs_osutils.h"
#include "isp_buf_def.h"
#include "armcb_isp.h"

#include <linux/types.h>
#include <linux/videodev2.h>
#include <fcntl.h>           /* low-level i/o */
#include <unistd.h>
#include <sys/ioctl.h>

typedef enum
{
    DMA_REMOTE_TO_LOCAL = 0,
    DMA_LOCAL_TO_REMOTE = 1,
} dma_direction_t;

/*
* Get_Isp_Mem_Fd
*
* @brief get fd of /dev/ISP-Mem
*
* @param void
*
* @return fd of /dev/ISP-Mem
*/
s32 Get_Isp_Mem_Fd(void);

/*
* Isp_Cma_Alloc
*
* @brief Alloc a continous kernel memory shared with user space.
*
* @param p_memb: memory info pointer
*
* @return success/failure
*/
isp_ret_t Isp_Cma_Alloc(mem_block_t *p_memb);

/*
* Isp_Cma_Free
*
* @brief Free the specified continous kernel memory.
*
* @param p_memb: memory info pointer
*
* @return success/failure
*/
isp_ret_t Isp_Cma_Free(mem_block_t *p_memb);

/*
* Isp_Xdma_V4l
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
isp_ret_t Isp_Xdma_V4l(u32 rmtAddr, u32 localAddr, u32 size, dma_direction_t dirt);

/*
* Isp_Dma_Copy_V4l
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
isp_ret_t Isp_Dma_Copy_V4l(u32 dstAddress, u32* srcAddress, u32 bytes, dma_direction_t dirt);


#endif

#endif
