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

#ifndef __MEMORY_MAP_OCTOB_CUS0_H__
#define __MEMORY_MAP_OCTOB_CUS0_H__

/****************ISP_REG_SRAM****************/
#define ISP_REG_BASE                    0x83c00000
#define ISP_REG_END                     0x83c016b0
#define ISP_REG_SIZE                    (ISP_REG_END - ISP_REG_BASE + 1)

#define ISP_REG_LUT_BASE                0x83c01800
#define ISP_REG_LUT_END                 0x83c0a0fc

#define ISP_SRAM_END                    0x83c0a100
#define ISP_SRAM_SIZE                   (ISP_SRAM_END - ISP_REG_BASE + 1)

#define LNC_R_SRAM_ADDR                 (ISP_REG_BASE + 0x2800)
#define LNC_G_SRAM_ADDR                 (ISP_REG_BASE + 0x2800 + 0x320)
#define LNC_B_SRAM_ADDR                 (ISP_REG_BASE + 0x2800 + 0x640)

#define CAC_SRAM_ADDR                   (ISP_REG_BASE + 0x3400)

#define LSC_MESH_R_ADDR                 (ISP_REG_BASE + 0x7400)
#define LSC_MESH_G_ADDR                 (ISP_REG_BASE + 0x7800)
#define LSC_MESH_B_ADDR                 (ISP_REG_BASE + 0x7C00)
#define LSC_1D_GAIN_TABLE_ADDR          (ISP_REG_BASE + 0x8000)
#define LSC_1D_RADIO_LOC_ADDR           (ISP_REG_BASE + 0x8084)
#define LSC_1D_RADIO_LOG2_ADDR          (ISP_REG_BASE + 0x80C4)

#define SCA_SRAM_ADDR                   (ISP_REG_BASE + 0x8100)

#define SCB_SRAM_ADDR                   (ISP_REG_BASE + 0x9900)

typedef enum
{
    ISP_SRAM_LNC_R = 0,
    ISP_SRAM_LNC_G,
    ISP_SRAM_LNC_B,
    ISP_SRAM_CAC,
    ISP_SRAM_LSC,
    ISP_SRAM_SCA,
    ISP_SRAM_SCB,
    ISP_SRAM_MAX,
} isp_sram_type_t;


/****************ISP_DDR*********************/
/*0x100000 -> 0x100000 2M*/

#define AE0_STATS_ADDR_0                0x1800000 //0x1000
#define AE0_STATS_ADDR_1                0x1801000
#define AE1_STATS_ADDR_0                0x1802000
#define AE1_STATS_ADDR_1                0x1803000

#define AE2_STATS_ADDR_0                0x1804000
#define AE2_STATS_ADDR_1                0x1805000

#define AE3_STATS_ADDR_0                0x1806000
#define AE3_STATS_ADDR_1                0x1807000
#define AE4_STATS_ADDR_0                0x1808000 //0x400
#define AE4_STATS_ADDR_1                0x1808400

///AWB STATS
#define AWB_STATS_PIXEL_COUNT_ADDR_0    0x1808800 //0x1C2
#define AWB_STATS_PIXEL_COUNT_ADDR_1    0x1808A00
#define AWB_STATS_PIXEL_COUNT_ADDR_2    0x1808C00
#define AWB_STATS_PIXEL_COUNT_ADDR_3    0x1808E00

#define AWB_STATS_RG_RATIO_ADDR_0       0x1809000 //0x708
#define AWB_STATS_RG_RATIO_ADDR_1       0x1809800
#define AWB_STATS_RG_RATIO_ADDR_2       0x180a000
#define AWB_STATS_RG_RATIO_ADDR_3       0x180a800

#define AWB_STATS_BG_RATIO_ADDR_0       0x180b000 //0x708
#define AWB_STATS_BG_RATIO_ADDR_1       0x180b800
#define AWB_STATS_BG_RATIO_ADDR_2       0x180c000
#define AWB_STATS_BG_RATIO_ADDR_3       0x180c800

///AF STATS
#define AF_STATS_ADDR_0                 0x180d000 //0x288
#define AF_STATS_ADDR_1                 0x180d400
#define AF_STATS_ADDR_2                 0x180d800
#define AF_STATS_ADDR_3                 0x180dc00

#define AE2_STATS_ADDR_2                0x180e000 //NEW
#define AE2_STATS_ADDR_3                0x180f000 //NEW

// TNR/MVD module
//MVD SAD, 2560x1440x12/16/8  0x54600
#define MVD_DDR_SAD_BASE_ADDR_0         0x1810000

//MVD MV, 2560x1440/16/8 0x7080
#define MVD_DDR_MV_BASE_ADDR_0          0x1864800

//TNR Y arm_formu:2560x1440x10/8 0x465000,
//Y:2560x1440 = 0x384000
#define TNR_BE3DNR_Y_DATA_ADDR_0        0x186c000

//TNR UV arm_formu:2560x1440x10/8/2 0x232800,
//UV:2560x1440/2 = 0x1c2000
#define TNR_BE3DNR_UV_DATA_ADDR_0       0x1cd1000
//#define TNR_BE3DNR_UV_DATA_ADDR_0       0x1c00000
//TNR_0 end       0x1e00000

#define MVD_DDR_SAD_BASE_ADDR_1         0x1f10000
#define MVD_DDR_MV_BASE_ADDR_1          0x1f70000
#define TNR_BE3DNR_Y_DATA_ADDR_1        0x1f78000
#define TNR_BE3DNR_UV_DATA_ADDR_1       0x23dd000
//TNR_1 end       0x2610000

/*support dual sensor, size: 8MB x 2*/
///VOUT
// camera0
#define VOUT0_BUFFER_ADDR_0             0x2000000
#define VOUT1_BUFFER_ADDR_0             0x2200000

// camera1
#define VOUT0_BUFFER_ADDR_1             0x2000000
#define VOUT1_BUFFER_ADDR_1             0x2200000

//unused in tx5215
#define VOUT2_BUFFER_ADDR_0             0x2400000
#define VOUT3_BUFFER_ADDR_0             0x2600000
#define VOUT2_BUFFER_ADDR_1             0x2400000
#define VOUT3_BUFFER_ADDR_1             0x2600000

///VIN
#define VIN_BUFFER_LONG_ADDR            0x2000000  //0x1000000 16MB
#define VIN_BUFFER_MID_ADDR             0x2200000
#define VIN_BUFFER_SHORT_ADDR           0x2400000
#define VIN_BUFFER_4CH_ADDR             0x2000000
#define VIN_BUFFER_END                  0x2800000

//TS 3DNR
#define TTNR_BUFFER_ADDR_TOP (0x52000000)
//TTNR_END 0x56000000

#define ISP_DDR_END_ADDR                0x1000000


/*not used*/
#define VIN_BUFFER_IFBC_HEADER_ADDR     (0x80000000 + 0x18B82000) /// start after 100 frames


/****************OTHERS****************/

#define CACHELINE_SIZE 32  // A9 or not

//-- APB2 Register base, size 64KB.
#define APB2_BASE 0x83c40000

//-- For sensor0 control, FPGA only.
#define SENSOR_REG_BASE   APB2_BASE

#define SENSOR_CTRL_REG  (SENSOR_REG_BASE + 0x04)
// Default: 0x07
// Bit 0: Sensor reset control.         1 = Sensor reset pull high,        0 = Sensor reset pull low.
// Bit 1: Sensor shutdown contorl.      1 = Sensor shutdown pull high,     0 = Sensor shutdown pull low.
// Bit 2: Sensor power enable contorl.  1 = Sensor power enable pull high, 0 = Sensor power enable pull low.
// Bit 3: Sensor HSMC_OE contorl.       1 = Sensor HSMC_OE pull high,      0 = Sensor HSMC_OE pull low.

#define SENSOR_RXCLKDLY_REG  (SENSOR_REG_BASE + 0x10)

//-- For sensor1 control, FPGA only.
#define SENSOR1_REG_BASE      (APB2_BASE + 0x040)

#define SENSOR1_CTRL_REG      (SENSOR1_REG_BASE + 0x04)
// Bits definition same as SENSOR_CTRL_REG.
//

#define SENSOR1_RXCLKDLY_REG  (SENSOR1_REG_BASE + 0x10)
#define SENSOR1_MONITOR_REG   (APB2_BASE + 0x60)

//-- For sensor2 control.
#define SENSOR2_REG_BASE      (APB2_BASE+0x0b0)
#define SENSOR2_CTRL_REG      (APB2_BASE+0x0b4)
// Bits definition same as SENSOR_CTRL_REG.
//

#define SENSOR2_RXCLKDLY_REG  (APB2_BASE+0x58)
#define SENSOR2_MONITOR_REG   (APB2_BASE+0x68)

//-- For sensor3 control.
#define SENSOR3_REG_BASE      (APB2_BASE+0x0c0)
#define SENSOR3_CTRL_REG      (APB2_BASE+0x0c4)
// Bits definition same as SENSOR_CTRL_REG.
//

#define SENSOR3_RXCLKDLY_REG  (APB2_BASE+0x5C)
#define SENSOR3_MONITOR_REG   (APB2_BASE+0x6C)
//-- For VTC
#ifndef XVTC_REG_BASE
  #define XVTC_REG_BASE       0x83c70000
  #define XVTC_REG_SIZE       0x140
  #define XVTC_REG_END        0x83C7013F
#endif

//-- For Video DMA on XCVU440 DDR4.
#ifndef VDMABaseAddr
  #define VDMABaseAddr           0x83c60000
  #define VDMA_REG_DISP_BASE     VDMABaseAddr
  #define VDMA_REG_DISP_SIZE     0x100
  #define VDMA_REG_DISP_END      0x83c600FF
#endif

#ifndef VDMABaseAddrIspOut
  #define VDMABaseAddrIspOut       0x83c80000
  #define VDMA_REG_ISP_OUT_BASE    VDMABaseAddrIspOut
  #define VDMA_REG_ISP_OUT_SIZE    0x100
  #define VDMA_REG_ISP_OUT_END     0x83C800FF
#endif

#ifndef VDMABaseAddrSensorIn
  #define VDMABaseAddrSensorIn     0x83c90000
  #define VDMA_REG_SENSOR_IN_BASE  VDMABaseAddrSensorIn
  #define VDMA_REG_SENSOR_IN_SIZE  0x100
  #define VDMA_REG_SENSOR_IN_END   0x83c900FF
#endif

/* Definitions for peripheral AXI_CDMA_0 */
#ifndef AXI_REG_CDMA_BASE
  #define AXI_REG_CDMA_BASE        0x8E200000
  #define AXI_REG_CDMA_SIZE        0x10000
  #define AXI_REG_CDMA_END         0x8E20FFFF
#endif

//-- APB2 Register base, size 64KB.
#ifndef APB2_REG_BASE
  #define APB2_REG_BASE            APB2_BASE
  #define APB2_REG_SIZE            0x70
  #define APB2_REG_END             0x83C4006F
#endif

#ifndef SPI_BUS0_BASEADDR
  #define SPI_BUS0_BASEADDR        0xE0006000
  #define SPI_BUS0_SIZE            0x1000
  #define SPI_BUS0_END             0xE0006FFF
#endif

//-- Memory defined for XC7Z020 CPU: local DDR3 and remote DDR4 on XCVU440.
//

// XC7Z020 DDR3 memory range.
#define MEM_START_7020  0x00000000
#define MEM_END_7020    0x3FFFFFFF  // 1GB

// Buffer descriptors are allocated in uncached memory for EMAC driver.
// The memory is made uncached by setting the attributes appropriately in
// the MMU table by calling
//   Xil_SetTlbAttributes(RX_BD_LIST_START_ADDRESS, 0xc02);
//
#define RX_BD_LIST_START_ADDRESS    0x0FF00000
#define TX_BD_LIST_START_ADDRESS    0x0FF80000

#define EMAC_XBUF_START             0x0FE00000
#define EMAC_XBUF_END               0x0FEFFFFF
#define EMAC_XBUF_VOLUME           (EMAC_XBUF_END-EMAC_XBUF_START + 1)  // Total 1MB for EMAC RX/TX buffer.

// This range in XC7Z020 CPU address domain is mapped to XCVU440's DDR memory.
// XC7Z020 CPU must access this range by CDMA.
// CDMA in XC7020 PL side is configured as this address mapping.
//
#define MEM_START_440   0x40000000
#define MEM_END_440     0x7FFFFFFF

//-- Display buffer on XCVU440.
//
#define DISPBUF_BASE    MEM_START_440
#define DISPBUF_SIZE    0x08000000       // 128MB

#define ISP_DDRBUF_BASE (MEM_START_440+DISPBUF_SIZE)      // 0x48000000
#define ISP_DDRBUF_SIZE 0x08000000       // 128MB

//-- White boards for general purpose in local(XC7Z020 PS side)
//   and remote(XCVU440 side) memory.
//
#define WB_START_7020   0x30000000
#define WB_SIZE_7020    0x10000000       // 256MB
#define WB_END_7020     (WB_START_7020 + WB_SIZE_7020 - 1)

//#define WB_START_440    (MEM_START_440+DISPBUF_SIZE)
#define WB_START_440    (ISP_DDRBUF_BASE + ISP_DDRBUF_SIZE)
#define WB_SIZE_440     0x10000000       // 256MB
#define WB_END_440      (WB_START_440 + WB_SIZE_440 - 1)

/****************OTHERS****************/


#endif // __MEMORY_MAP
