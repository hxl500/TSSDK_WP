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
#ifndef __ISP_BUF_DEF_H__
#define __ISP_BUF_DEF_H__

#include "types_utils.h"
#include "global_def.h"

/// Define isp ddr buffer address
typedef struct
{
#ifdef HW_BUILD_FOR_TITIAN
    u32 ddrAutoStartAddr;
    u32 ddrMvInfoAddr;
    u32 ddrSadInfoAddr;
    u32 ddrRtnrIirAddr;
    u32 ddrYtnrIirYAddr;
    u32 ddrYtnrIirCAddr;
    u32 ddrYtnrRegMvAddr;

    u32 ae0StatsAddress;
    /// u32 ae1StatsAddress;
    /// u32 ae2StatsAddress;
    /// u32 ae3StatsAddress;
    /// u32 ae4StatsAddress;
    u32 vinBufLongBase;
    u32 vinBufMidBase;
    u32 vinBufShortBase;
    u32 vinBufIFBCHeaderAddr;
    u32 vout0BufBase0;

#else /// octob

    u32 ae0StatsAddress;
    u32 ae1StatsAddress;
    u32 ae2StatsAddress;
    u32 ae3StatsAddress;
    u32 ae4StatsAddress;
    u32 awbStatsPixelCountAddress;
    u32 awbStatsRGRatioAddress;
    u32 awbStatsBGRatioAddress;
    u32 afStatsAddress;
    u32 mvdDdrSadBaseAddr;
    u32 mvdDdrSadBaseAddrDn;  //
    u32 mvdDdrMvBaseAddr;
    u32 mvdDdrMvBaseAddrDn;  //
    u32 tnrBe3dnrYDataAddr;
    u32 tnrBe3dnrUVDataAddr;
    u32 vinBufLongBase;
    u32 vinBufMidBase;
    u32 vinBufShortBase;
    u32 vout0BufBase0;
    u32 vout0BufBase1;
    u32 vout1BufBase0;
    u32 vout1BufBase1;
    u32 vout2BufBase0;
    u32 vout2BufBase1;
    u32 vout3BufBase0;
    u32 vout3BufBase1;
    u32 vinBuf4chAddr;
    u32 vinBufIFBCHeaderAddr;
    struct {
      void *aec0_stats_addr;
      void *aec1_stats_addr;
      void *aec2_stats_addr;
      void *aec3_stats_addr;
      void *aec4_stats_addr;
      void *awb_stats_pcnt_addr;
      void *awb_stats_rgrt_addr;
      void *awb_stats_bgrt_addr;
      void *afc_stats_addr;
    } buffer;
#endif
} isp_buffer_address_t;



#define AE_STATS_BIN                 1024 /// Histogram bin

#ifdef HW_BUILD_FOR_TITIAN
#define AAC_BLOCK_X                  32
#define AAC_BLOCK_Y                  32
#else
#define AAC_BLOCK_X                  15
#define AAC_BLOCK_Y                  15
#endif

#define AAC_BLOCK_NUM                (AAC_BLOCK_X * AAC_BLOCK_Y)

#define AWB_STATS_PIXEL_COUNT_SIZE   (AAC_BLOCK_NUM * 2)   //short
#define AWB_STATS_RATIO_SIZE         (AAC_BLOCK_NUM * 8)   //long long

#define AF_BLOCK_NUM_MAX_X           9
#define AF_BLOCK_NUM_MAX_Y           9
#define AF_BLOCK_NUM_MAX             (AF_BLOCK_NUM_MAX_X * AF_BLOCK_NUM_MAX_Y)
#define ADB_BAND_NUM                 (64)


#ifdef HW_BUILD_FOR_TITIAN

/// AAC Packed stat, 8byte
#ifdef PC_SIMU
typedef struct
{
    u64 totalSumX : 24;
    u64 validSumX : 24;
    u64 validPixelCnt : 16;
} aac_packed_stat_t;

typedef struct
{
    s32 real;
    s32 img;
}complex_t;

#else
typedef struct
{
    u64 totalSumX : 24;
    u64 validSumX : 24;
    u64 validPixelCnt : 16;
}__attribute__((packed, aligned(4))) aac_packed_stat_t ;

typedef struct
{
    s32 real;
    s32 img;
}__attribute__((packed, aligned(4))) complex_t;

#endif

typedef struct
{
    /// AEP histY
    u32 aep0Hist[AE_STATS_BIN];
    u32 aep1Hist[AE_STATS_BIN];
    u32 aep2Hist[AE_STATS_BIN];
    u32 aep3Hist[AE_STATS_BIN];

    /// AAC
    aac_packed_stat_t statR[AAC_BLOCK_NUM];
    aac_packed_stat_t statG[AAC_BLOCK_NUM];
    aac_packed_stat_t statB[AAC_BLOCK_NUM];
    u32 histR[AE_STATS_BIN];
    u32 histG[AE_STATS_BIN];
    u32 histB[AE_STATS_BIN];

    /// AAY
    u32 aeHistYuv[AE_STATS_BIN >> 2];

    /// AF
    u64 ullAFSharpness[AF_BLOCK_NUM_MAX];

    /// extend 24 byte for 32-byte alignment
    u32 rsv[6];

    /// ADB
    complex_t adbStats[ADB_BAND_NUM];
} hw3a_stats_t;

#else   /// octob
typedef struct __hw3a_stats_t
{
    /// AE
    u32     ae0Hist[AE_STATS_BIN];
    u32     ae1Hist[AE_STATS_BIN];
    u32     ae2Hist[AE_STATS_BIN];
    u32     ae3Hist[AE_STATS_BIN];
    u32     ae4Hist[AE_STATS_BIN >> 2];
    /// AWB
    u16     usAWBZonePixelsCounted[AAC_BLOCK_NUM];
    u64     ullAWBZoneRGRatioSum[AAC_BLOCK_NUM];
    u64     ullAWBZoneBGRatioSum[AAC_BLOCK_NUM];
    /// AF
    u64     ullAFSharpness[AF_BLOCK_NUM_MAX];

#ifdef PC_SIMU
    /// LTM output
    u32     ltmOutputStats[257];
#endif
} hw3a_stats_t;

//isp 3a stats & result args  for cbs.notify
//warning===>struct limit size <= notify_t.msg size
//
typedef struct __sw3a_info_t
{
    hw3a_stats_t    *pIspStats;
    u32             luxIndex;
    u32             expTotalGain;
    u32             expLineCountLong;
    u32             awbRGain;
    u32             awbGGain;
    u32             awbBGain;

    u32             lumaAvg;
	u32				expTime;
	u32				again;
	u32				dgain;
	u32				ispgain;
	u32				sysgain;
}sw3a_info_t;
#endif

#endif
