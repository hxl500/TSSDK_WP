/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: mpi definitions of cve
 * Author: Tsing Micro solution-application group
 * Create: 2022/07/20
 */

#ifndef _TS_COMM_CVE_H_
#define _TS_COMM_CVE_H_

#include "ts_type.h"
#include "ts_errno.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum tsCVE_IMAGE_TYPE_E {
	TS_CVE_IMAGE_TYPE_U8C1,
	TS_CVE_IMAGE_TYPE_S8C1,
	TS_CVE_IMAGE_TYPE_S16C1,
	TS_CVE_IMAGE_TYPE_U16C1,

	TS_CVE_IMAGE_TYPE_YUV420SP, /* YUV420 SemiPlanar */
    TS_CVE_IMAGE_TYPE_YUV422SP, /* YUV422 SemiPlanar */
    TS_CVE_IMAGE_TYPE_YUV420P,  /* YUV420 Planar */
    TS_CVE_IMAGE_TYPE_YUV422P,  /* YUV422 planar */

	TS_CVE_IMAGE_TYPE_RGB,
    TS_CVE_IMAGE_TYPE_ARGB,
    TS_CVE_IMAGE_TYPE_BGR,
    TS_CVE_IMAGE_TYPE_ABGR,

	TS_CVE_IMAGE_TYPE_BUTT
} CVE_IMAGE_TYPE_E;

typedef struct tsCVE_IMAGE_S {
	TS_U64 au64PhyAddr[3];   /* RW;The physical address of the image */
	TS_U64 au64VirAddr[3];   /* RW;The virtual address of the image */
	TS_U32 au32Stride[3];    /* RW;The stride of the image */
	TS_U32 u32Width;         /* RW;The width of the image */
	TS_U32 u32Height;        /* RW;The height of the image */
	CVE_IMAGE_TYPE_E enType; /* RW;The type of the image */
} CVE_IMAGE_S;

typedef struct tsCVE_MEM_INFO_S {
	TS_U64 u64PhyAddr; /* RW;The physical address of the memory */
	TS_U64 u64VirAddr; /* RW;The virtual address of the memory */
	TS_U32 u32Size;    /* RW;The size of memory */
} CVE_MEM_INFO_S;

typedef struct tsCVE_DATA_S {
	TS_U64 u64PhyAddr; /* RW;The physical address of the data */
	TS_U64 u64VirAddr; /* RW;The virtaul address of the data */

	TS_U32 u32Stride; /* RW;The stride of 2D data by byte */
	TS_U32 u32Width;  /* RW;The width of 2D data by byte */
	TS_U32 u32Height; /* RW;The height of 2D data by byte */

	TS_U32 u32Reserved;
} CVE_DATA_S;

typedef union tsCVE_8BIT_U {
	TS_S8 s8Val;
	TS_U8 u8Val;
} CVE_8BIT_U;

/*
 * Definition of unsigned short point
 */
typedef struct tsCVE_POINT_U16_S {
	TS_U16 u16x;
	TS_U16 u16y;
} CVE_POINT_U16_S;

/*
 * Definition of short point
 */
typedef struct tsCVE_POINT_S16_S {
	TS_U16 s16x;
	TS_U16 s16y;
} CVE_POINT_S16_S;

/*
 *Float point represented by Fixed-point SQ25.7
 */
typedef struct tsCVE_POINT_S25Q7_S {
	TS_S32 s25q7x; /*X coordinate*/
	TS_S32 s25q7y; /*Y coordinate*/
} CVE_POINT_S25Q7_S;

/*
 * Definition of rect
 */
typedef struct tsCVE_RECT_U16_S {
	TS_U16 u16x;
	TS_U16 u16y;
	TS_U16 u16Width;
	TS_U16 u16Height;
} CVE_RECT_U16_S;

typedef struct tsCVE_LOOK_UP_TABLE_S {
	CVE_MEM_INFO_S stTable;
	TS_U16 u16ElemNum;    /*LUT's elements number*/

	TS_U8 u8TabInPreci;
	TS_U8 u8TabOutNorm;

	TS_S32 s32TabInLower; /*LUT's original input lower limit*/
	TS_S32 s32TabInUpper; /*LUT's original input upper limit*/
} CVE_LOOK_UP_TABLE_S;

typedef enum tsEN_CVE_ERR_CODE_E {
	TS_ERR_CVE_SYS_TIMEOUT = 0x40,   /* IVE process timeout */
	TS_ERR_CVE_QUERY_TIMEOUT = 0x41, /* IVE query timeout */
	TS_ERR_CVE_OPEN_FILE = 0x42,     /* IVE open file error */
	TS_ERR_CVE_READ_FILE = 0x43,     /* IVE read file error */
	TS_ERR_CVE_WRITE_FILE = 0x44,    /* IVE write file error */
	TS_ERR_CVE_BUS_ERR = 0x45,

	TS_ERR_CVE_BUTT
} EN_CVE_ERR_CODE_E;

typedef TS_S32 CVE_HANDLE;

/********************************CVE error code ***********************************/
/* Invalid device ID */
#define TS_ERR_CVE_INVALID_DEVID TS_DEF_ERR(TS_ID_CVE, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* Invalid channel ID */
#define TS_ERR_CVE_INVALID_CHNID TS_DEF_ERR(TS_ID_CVE, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* At least one parameter is illegal. For example, an illegal enumeration value exists. */
#define TS_ERR_CVE_ILLEGAL_PARAM TS_DEF_ERR(TS_ID_CVE, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* The channel exists. */
#define TS_ERR_CVE_EXIST TS_DEF_ERR(TS_ID_CVE, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* The UN exists. */
#define TS_ERR_CVE_UNEXIST TS_DEF_ERR(TS_ID_CVE, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* A null point is used. */
#define TS_ERR_CVE_NULL_PTR TS_DEF_ERR(TS_ID_CVE, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* Try to enable or initialize the system, device, or channel before configuring attributes. */
#define TS_ERR_CVE_NOT_CONFIG TS_DEF_ERR(TS_ID_CVE, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* The operation is not supported currently. */
#define TS_ERR_CVE_NOT_SURPPORT TS_DEF_ERR(TS_ID_CVE, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* The operation, changing static attributes for example, is not permitted. */
#define TS_ERR_CVE_NOT_PERM TS_DEF_ERR(TS_ID_CVE, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* A failure caused by the malloc memory occurs. */
#define TS_ERR_CVE_NOMEM TS_DEF_ERR(TS_ID_CVE, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* A failure caused by the malloc buffer occurs. */
#define TS_ERR_CVE_NOBUF TS_DEF_ERR(TS_ID_CVE, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* The buffer is empty. */
#define TS_ERR_CVE_BUF_EMPTY TS_DEF_ERR(TS_ID_CVE, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* No buffer is provided for storing new data. */
#define TS_ERR_CVE_BUF_FULL TS_DEF_ERR(TS_ID_CVE, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* The system is not ready because it may be not initialized or loaded. */
/* The error code is returned when a device file fails to be opened. */
#define TS_ERR_CVE_NOTREADY TS_DEF_ERR(TS_ID_CVE, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/* The source address or target address is incorrect during the operations such as calling */
/* copy_from_user or copy_to_user. */
#define TS_ERR_CVE_BADADDR TS_DEF_ERR(TS_ID_CVE, EN_ERR_LEVEL_ERROR, EN_ERR_BADADDR)
/* The resource is busy during the operations such as destroying a VENC channel */
/* without deregistering it. */
#define TS_ERR_CVE_BUSY TS_DEF_ERR(TS_ID_CVE, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
/* CVE process timeout: 0xA01D8040 */
#define TS_ERR_CVE_SYS_TIMEOUT TS_DEF_ERR(TS_ID_CVE, EN_ERR_LEVEL_ERROR, TS_ERR_CVE_SYS_TIMEOUT)
/* CVE query timeout: 0xA01D8041 */
#define TS_ERR_CVE_QUERY_TIMEOUT TS_DEF_ERR(TS_ID_CVE, EN_ERR_LEVEL_ERROR, TS_ERR_CVE_QUERY_TIMEOUT)
/* CVE open file error: 0xA01D8042 */
#define TS_ERR_CVE_OPEN_FILE TS_DEF_ERR(TS_ID_CVE, EN_ERR_LEVEL_ERROR, TS_ERR_CVE_OPEN_FILE)
/* CVE read file error: 0xA01D8043 */
#define TS_ERR_CVE_READ_FILE TS_DEF_ERR(TS_ID_CVE, EN_ERR_LEVEL_ERROR, TS_ERR_CVE_READ_FILE)
/* CVE read file error: 0xA01D8044 */
#define TS_ERR_CVE_WRITE_FILE TS_DEF_ERR(TS_ID_CVE, EN_ERR_LEVEL_ERROR, TS_ERR_CVE_WRITE_FILE)
/* CVE Bus error: 0xA01D8045 */
#define TS_ERR_CVE_BUS_ERR TS_DEF_ERR(TS_ID_CVE, EN_ERR_LEVEL_ERROR, TS_ERR_CVE_BUS_ERR)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __TS_COMM_CVE_H__ */
