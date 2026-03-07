#ifndef __ISP_TTNR_TUNING_TUN_536_H__
#define __ISP_TTNR_TUNING_TUN_536_H__

#include "tune_utils.h"

#define ISP_TUNNING_TTNR_TEMP_SCALE_MAX (0xFFF000)

typedef struct tun_ttnr_param tun_ttnr_param_t;
typedef struct tun_ttnr_temp_cmp ttnr_temp_cmp_param_by_gain_t;
typedef struct tun_ttnr_temp ttnr_temp_param_by_gain_t;
typedef struct tun_ttnr_cfa ttnr_cfa_param_by_gain_t;

struct tun_ttnr_temp_cmp {
  u32 offset_r;                           //[1]
  u32 offset_g;                           //[2]
  u32 offset_b;                           //[3]
  u32 gain_r;                             //[4]
  u32 gain_g;                             //[5]
  u32 gain_b;                             //[6]
};

struct tun_ttnr_temp {
  u32 md_gain;                            //[1] 
  u32 md_smooth;                          //[2]
  u32 md_delay;                           //[3] 
  u32 md_min;                             //[4] 
  u32 md_max;                             //[5] 
  u32 md_max_change_down;                 //[6] 
  u32 md_max_change_up;                   //[7] 
  u32 md_luma_adjust_lvl_low;             //[8] 
  u32 md_luma_adjust_lvl_low_delta;       //[9] 
  u32 md_luma_adjust_lvl_high;            //[10]
  u32 md_luma_adjust_lvl_high_delta;      //[11]
  u32 md_luma_adjust_lvl_low_strength;    //[12] 
  u32 md_luma_adjust_lvl_mid_strength;    //[13] 
  u32 md_luma_adjust_lvl_high_strength;   //[14]
  u32 md_mean_adjust_lvl_low;             //[15]
  u32 md_mean_adjust_lvl_low_delta;       //[16] 
  u32 md_mean_adjust_lvl_high;            //[17] 
  u32 md_mean_adjust_lvl_high_delta;      //[18]
  u32 md_mean_adjust_lvl_low_strength;    //[19]
  u32 md_mean_adjust_lvl_mid_strength;    //[20]
  u32 md_mean_adjust_lvl_high_strength;   //[21]
};

struct tun_ttnr_cfa {
  u32 radius_regular;                     //[1]
  u32 radius_fine;                        //[2]
  u32 noise_lvl_slope_r;                  //[3]
  u32 noise_lvl_slope_g;                  //[4] 
  u32 noise_lvl_slope_b;                  //[5] 
  u32 noise_lvl_offset_r;                 //[6] 
  u32 noise_lvl_offset_g;                 //[7] 
  u32 noise_lvl_offset_b;                 //[8] 
  u32 fine_smooth_strength;               //[9] 
  u32 edge_smooth_strength;               //[10]
  u32 edge_thresh_high;                   //[11]
  u32 edge_thresh_low;                    //[12]
  u32 motion_adjust_lvl_low;              //[13]
  u32 motion_adjust_lvl_low_delta;        //[14]
  u32 motion_adjust_lvl_high;             //[15]
  u32 motion_adjust_lvl_high_delta;       //[16]
  u32 motion_adjust_lvl_low_strength;     //[17]  
  u32 motion_adjust_lvl_mid_strength;     //[18]  
  u32 motion_adjust_lvl_high_strength;    //[19]
  u32 orignal_blend_r;                    //[20]
  u32 orignal_blend_g;                    //[21] 
  u32 orignal_blend_b;                    //[22] 
};

struct tun_ttnr_param {
  BOOL ttnr_sw_en;
  u8 temp_curve_basic_sel;
  // u8 temp_cmp_basic_sel;
  s16 temp_smth_clip_high;
  s16 temp_smth_clip_low;

  DYNAMIC_TABLE(struct tun_ttnr_temp_cmp, temp_cmp_gain_intp_tbl);
  DYNAMIC_TABLE(struct tun_ttnr_temp, temp_gain_intp_tbl);
  DYNAMIC_TABLE(struct tun_ttnr_cfa, cfa_gain_intp_tbl);
};
#endif
