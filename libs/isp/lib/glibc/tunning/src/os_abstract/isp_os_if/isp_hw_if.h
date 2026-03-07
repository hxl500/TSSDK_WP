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
#ifndef __ISP_HW_IF__
#define __ISP_HW_IF__

#include "types_utils.h"
#include "log_utils.h"
#include "armcb_isp.h"

#ifdef CMDR_ENABLE
#define DRV_SENSOR_REG_APPLY_SETTING_SIZE   64
#else
#define DRV_SENSOR_REG_APPLY_SETTING_SIZE   1024
#endif

/// Define pwoer up setting data
typedef struct
{
    u32              regAddr;       /// register address
    u32              bitMask;       /// bit mask of register data
    BOOL             bitVal;        /// bit value of register data
    drv_power_type   type;          /// power type
    u32              delayUs;       /// delay of operator
} drv_power_setting_t;

/// Define pwoer up setting data
typedef struct
{
    drv_power_setting_t *powerSetting;  /// Pointer of power setting
    u32                  size ;         /// Power setting array size
} drv_power_settings_t;

typedef struct __isp_hw_if isp_hw_if_t;

typedef isp_ret_t (*isp_hw_read_func)(isp_hw_if_t* ,   isp_hw_req_t*);
typedef isp_ret_t (*isp_hw_write_func)(isp_hw_if_t* ,  isp_hw_req_t*);
typedef isp_ret_t (*isp_hw_power_func)(isp_hw_if_t* ,  isp_hw_req_t*);
typedef isp_ret_t (*isp_hw_apply_func)(isp_hw_if_t* ,  cmd_buf_t*);
typedef isp_ret_t (*isp_hw_query_func)(isp_hw_if_t* ,  isp_hw_req_t*);

/// Define isp hardware interface
struct __isp_hw_if
{
    isp_hw_read_func    read;
    isp_hw_write_func   write;
    isp_hw_power_func   powerCfg;
    isp_hw_apply_func   apply;
    isp_hw_query_func   query;
};


/*
* ISP_Write_Register
*
* @brief Write ISP register
*
* @params addr: isp register addr
*
* @params val: isp register value
*
* @return void
*
*/
void ISP_Write_Register(const u32 addr, const u32 val);

/*
* ISP_Read_Register
*
* @brief Read ISP register
*
* @params addr: isp register addr
*
* @return register value
*
*/
u32 ISP_Read_Register(const u32 addr);

/*
* ISP_Free_List
*
* @brief Free isp hardware registers list
*
* @params pUpdateList: isp register list
*
* @return success/failure
*
*/
isp_ret_t ISP_Free_List(isp_reg_list_t *pUpdateList);

/*
* ISP_Append_Reglist
*
* @brief Append isp hardware registers
*
* @params ppHeadList: point to isp register list
*
* @params pRegCfgList: isp config register list
*
* @params size: isp config register size
*
* @return success/failure
*
*/
void ISP_Append_Reglist(isp_reg_list_t** ppHeadList, reg_cfg_t *pRegCfgList, u16 size);

/*
* ISP_Apply_Regs_List
*
* @brief Apply isp registers list by isp hareware interface
*
* @params pIspHwIf: point to isp_hw_if_t
*
* @params frameIdx: camera id
*
* @params pUpdateList: register list to apply
*
* @return success/failure
*
*/
isp_ret_t ISP_Apply_Regs_List(isp_hw_if_t* pIspHwIf, u32 frameIdx, isp_reg_list_t *pUpdateList);

/*
* Isp_Hw_Update_Regs_To_Req
*
* @brief update registers array to hw request
*
* @params isp_hw_req_t: isp hardware request will be submited
*
* @params drv_regs_array_info_t:  register configure array information
*
* @return success/failure
*
*/
isp_ret_t    Isp_Hw_Update_Regs_To_Req(isp_hw_req_t*, drv_regs_array_info_t*);

/*
* ISP_Hw_If_Get_Instance
*
* @brief get isp hardware interface
*
* @params  void
*
* @return  isp hardware interface
*
*/
isp_hw_if_t* ISP_Hw_If_Get_Instance(void);


#endif

