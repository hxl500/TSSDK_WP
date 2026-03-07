////////////////////////
//This source code is CONFIDENTIAL and any use by you is subject to the terms of the agreement between you and Arm China or the terms of the agreement
//between you and the party authorised by Arm China to disclose this document to you.
//
//The confidential and proprietary information contained in this file may only be used by a person authorised under
//and to the extent permitted by a subsisting licensing agreement from Arm China.
//
//This entire notice must be reproduced on all copies of this file and copies of this file may only be made by a person
//if such person is permitted to do so under the terms of a subsisting license agreement from Arm China.
//
//Release Information: Linlon-i5-r0p2-00eac0
//
//Copyright © 2020 Arm Technology (China) Co. Ltd. All rights reserved.
//////////////////////

#ifndef __GLOBAL_DEF_H__
#define __GLOBAL_DEF_H__

/***********************************************
        Define ISP FW Version
***********************************************/
///   The following definition should be defined when using corresponding fw_lib.
///   define HW_BUILD_FOR_I5      ,  I5  version
///   define HW_BUILD_FOR_I5_R1P0 ,  I5 R1P0 version
///   define HW_BUILD_FOR_I3      ,  I3  version
///   define HW_BUILD_FOR_CUST0   ,  custom0 version
///   define HW_BUILD_FOR_TITIAN  ,  TITAIN HW version

/* check if it has been defined by cmake*/
#if (!defined(HW_BUILD_FOR_I5) &&\
     !defined(HW_BUILD_FOR_I5_R1P0) &&\
     !defined(HW_BUILD_FOR_I3) &&\
     !defined(HW_BUILD_FOR_CUST0) &&\
     !defined(HW_BUILD_FOR_TITIAN))

///#define HW_BUILD_FOR_I5
///#define HW_BUILD_FOR_I5_R1P0
///#define HW_BUILD_FOR_I3
#define HW_BUILD_FOR_CUST0
// #define HW_BUILD_FOR_TITIAN

#endif


/***********************************************
        Macro define for FW
***********************************************/

#define MAX_CONTEXT_NUM (4)

#ifdef HW_BUILD_FOR_CUST0
/// enable for surround-cam OV5647 process
///#define QUAD_SENSOR_5647_ENABLE
#endif

#define DEBUG_3A

#define AWB_NEW_ALGO
//#define RGBIR_SENSOR

//#define CMDR_ENABLE

//#define RUNNING_ON_HAPS

//temp use
// #define CAM0_IMX415
//#define DEBUG_4MP_INPUT

#define DEBUG_V_START 0

#ifdef DEBUG_4MP_INPUT
#define PRE_SCALE_WID 2560
#define PRE_SCALE_HEI 1440
#else
#define PRE_SCALE_WID 1920
#define PRE_SCALE_HEI 1080
#endif

#ifdef CAM0_IMX415
#undef DEBUG_V_START
#define DEBUG_V_START 1
#endif

//#define FIX_FORCE_DWPA_CLK_ON
//#define FIX_FORCE_DWPB_CLK_ON

#endif
