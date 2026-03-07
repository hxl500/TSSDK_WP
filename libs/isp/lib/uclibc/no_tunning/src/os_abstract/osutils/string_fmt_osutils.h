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

#ifndef __STRING_FMT_OSUTILS_H__
#define __STRING_FMT_OSUTILS_H__

#if (defined(__x86_64) || defined(__aarch64__))
    #define FMT_SIZE "%lu"
    #define FMT_S64  "%l"
    #define FMT_U64  "%lu"
    #define FMT_X64  "%lx"
    #define FMT_S64_ALIGN(align) "%"#align"l"
    #define FMT_U64_ALIGN(align) "%"#align"lu"
    #define FMT_X64_ALIGN(align) "%"#align"lx"
#else
    #define FMT_SIZE "%u"
    #define FMT_S64  "%ll"
    #define FMT_U64  "%llu"
    #define FMT_X64  "%llx"
    #define FMT_S64_ALIGN(align) "%"#align"ll"
    #define FMT_U64_ALIGN(align) "%"#align"llu"
    #define FMT_X64_ALIGN(align) "%"#align"llx"
#endif

#endif
