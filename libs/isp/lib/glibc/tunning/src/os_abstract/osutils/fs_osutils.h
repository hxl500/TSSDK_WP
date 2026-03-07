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

#ifndef __FS_OSUTILS_H__
#define __FS_OSUTILS_H__

#include <stdlib.h>
#include <string.h>
#include "types_utils.h"
#include "mem_osutils.h"

#if defined (__linux__)

#include <stdio.h>
#include <unistd.h>
typedef FILE FileHandle;

#elif defined (PC_SIMU)
#include <stdio.h>
typedef FILE FileHandle;

#else
#include "ff.h"

typedef FIL FileHandle;
#endif

// #if defined(__linux__) && defined(__arm__)
#if defined(__linux__) && (defined(__arm__) || defined (__aarch64__))
#define STORAGE_ROOT_DIR "/tmp/"
#else
#define STORAGE_ROOT_DIR ""
#endif

/// These wrapper interfaces is for File I/O to compatible with different OS

/*
* FS_OS_Write
*
* @brief Write buffer to file
*
* @param pFile: pointer to FileHandle
*
* @param buf: pointer to buffer for writting
*
* @param bytes: how many bytes are going to be written
*
* @return how many bytes have been written after execution
*
*/
size_t FS_OS_Write(FileHandle* pFile, const void* buf, size_t bytes);

/*
* FS_OS_ReadLine
*
* @brief Read one line from file
*
* @param pFile: pointer to FileHandle
*
* @param buf: pointer to buffer for reading
*
* @param bytes: how many bytes are going to be read
*
* @return how many bytes have been read after execution
*
*/
size_t FS_OS_ReadLine(FileHandle* pFile, void* buf, size_t bytes);

/*
* FS_OS_Read
*
* @brief Read buffer from file
*
* @param pFile: pointer to FileHandle
*
* @param buf: pointer to buffer for reading
*
* @param bytes: how many bytes are going to be read
*
* @return how many bytes have been read after execution
*
*/
size_t FS_OS_Read(FileHandle* pFile, void* buf, size_t bytes);

/*
* FS_OS_Size
*
* @brief Get total size of file
*
* @param pFile: pointer to FileHandle
*
* @return size of file
*
*/
size_t FS_OS_Size(FileHandle* pFile);

/*
* FS_OS_Seek
*
* @brief Re-locate file stream pointer
*
* @param pFile: pointer to FileHandle
*
* @param offset: offset from file beginning
*
* @return ISP_RET_SUCCESS if success, others if failed
*
*/
isp_ret_t FS_OS_Seek(FileHandle* pFile, u32 offset);


/*
* FS_OS_Sync
*
* @brief Sync cache to memory
*
* @param pFile: pointer to FileHandle
*
* @return void
*
*/
void FS_OS_Sync(FileHandle* pFile);


/*
* FS_OS_Open
*
* @brief Open file with operation mode
*
* @param filename: filename to be opened
*
* @param sMode: operation mode string
*
* @return pointer to FileHandle
*
*/
FileHandle* FS_OS_Open(const char8* filename, const char8* sMode);

/*
* FS_OS_Close
*
* @brief Close file
*
* @param pFile: pointer to FileHandle
*
* @return void
*
*/
void FS_OS_Close(FileHandle* pFile);

#endif
