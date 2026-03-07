// #ifndef __FW_ISP_API_I5_CUST0_TTNR_H__
// #define __FW_ISP_API_I5_CUST0_TTNR_H__


#define _TTNR_START                                         (_DEBUG_END + 0x01)
#define ISP_API_TTNR_DUMMY_SEL                              (_TTNR_START + 0x00)    // 1, 2, 4, 6, other
#define ISP_API_TTNR_TEMP_ENABLE                            (_TTNR_START + 0x01)    // [0, 1]
#define ISP_API_TTNR_CFA_ENABLE                             (_TTNR_START + 0x02)    // [0, 1]
#define ISP_API_TTNR_IMG_WIDTH                              (_TTNR_START + 0x03)    // [0, 3840]

#define ISP_API_TTNR_TEMP_SCALE                             (_TTNR_START + 0x05)    // [0, 16773120]
#define ISP_API_TTNR_TEMP_DELAY                             (_TTNR_START + 0x06)    // [0, 255]
#define ISP_API_TTNR_TEMP_MIN                               (_TTNR_START + 0x07)    // [0, 255]
#define ISP_API_TTNR_TEMP_MAX                               (_TTNR_START + 0x08)    // [0, 255]
#define ISP_API_TTNR_TEMP_CHANGE_UP                         (_TTNR_START + 0x09)    // [0, 255] 4095?
#define ISP_API_TTNR_TEMP_CHANGE_DN                         (_TTNR_START + 0x0A)    // [0, 255] 4095?
#define ISP_API_TTNR_TEMP_LUMA_ADJ_LOW_STRENGTH             (_TTNR_START + 0x0B)    // [0, 255]
#define ISP_API_TTNR_TEMP_LUMA_ADJ_MIN_STRENGTH             (_TTNR_START + 0x0C)    // [0, 255]
#define ISP_API_TTNR_TEMP_LUMA_ADJ_HIGH_STRENGTH            (_TTNR_START + 0x0D)    // [0, 255]
#define ISP_API_TTNR_TEMP_LUMA_ADJ_LOW                      (_TTNR_START + 0x0E)    // [0, 253]
#define ISP_API_TTNR_TEMP_LUMA_ADJ_LOW_DELTA                (_TTNR_START + 0x0F)    // [0, 7]
#define ISP_API_TTNR_TEMP_LUMA_ADJ_HIGH                     (_TTNR_START + 0x10)    // [0, 254]
#define ISP_API_TTNR_TEMP_LUMA_ADJ_HIGH_DELTA               (_TTNR_START + 0x11)    // [0, 7]
#define ISP_API_TTNR_TEMP_MOTION_ADJ_LOW_STRENGTH           (_TTNR_START + 0x12)    // [0, 255]
#define ISP_API_TTNR_TEMP_MOTION_ADJ_MIN_STRENGTH           (_TTNR_START + 0x13)    // [0, 255]
#define ISP_API_TTNR_TEMP_MOTION_ADJ_HIGH_STRENGTH          (_TTNR_START + 0x14)    // [0, 255]
#define ISP_API_TTNR_TEMP_MOTION_ADJ_LOW                    (_TTNR_START + 0x15)    // [0, 253]
#define ISP_API_TTNR_TEMP_MOTION_ADJ_LOW_DELTA              (_TTNR_START + 0x16)    // [0, 7]
#define ISP_API_TTNR_TEMP_MOTION_ADJ_HIGH                   (_TTNR_START + 0x17)    // [0, 254]
#define ISP_API_TTNR_TEMP_MOTION_ADJ_HIGH_DELTA             (_TTNR_START + 0x18)    // [0, 7]

// #define ISP_API_TTNR_TEMP_SMOOTH                            (_TTNR_START + 0xff)    // [0, 255] move to special
// #define ISP_API_TTNR_TONE_CURVE_LUT                         (_LUT_START + 0x0C)     // [0, 1023]
#define ISP_API_TTNR_TONE_CURVE_LUT                         (_TTNR_START + 0x04)     // [0, 1023]

#define ISP_API_TTNR_CFA_RADIUS_REGULAR                     (_TTNR_START + 0x19)    // 2, 4, 6  
#define ISP_API_TTNR_CFA_RADIUS_FINE                        (_TTNR_START + 0x1A)    // 2, 4, 6  
#define ISP_API_TTNR_CFA_ORI_BLEND_R                        (_TTNR_START + 0x1B)    // [0, 256] 
#define ISP_API_TTNR_CFA_ORI_BLEND_G                        (_TTNR_START + 0x1C)    // [0, 256]
#define ISP_API_TTNR_CFA_ORI_BLEND_B                        (_TTNR_START + 0x1D)    // [0, 256]
#define ISP_API_TTNR_CFA_NOISE_LVL_SLOPE_R                  (_TTNR_START + 0x1E)    // [0, 16383]
#define ISP_API_TTNR_CFA_NOISE_LVL_SLOPE_G                  (_TTNR_START + 0x1F)    // [0, 16383]
#define ISP_API_TTNR_CFA_NOISE_LVL_SLOPE_B                  (_TTNR_START + 0x20)    // [0, 16383]
#define ISP_API_TTNR_CFA_NOISE_LVL_OFFSET_R                 (_TTNR_START + 0x21)    // [0, 16383]
#define ISP_API_TTNR_CFA_NOISE_LVL_OFFSET_G                 (_TTNR_START + 0x22)    // [0, 16383]
#define ISP_API_TTNR_CFA_NOISE_LVL_OFFSET_B                 (_TTNR_START + 0x23)    // [0, 16383]
#define ISP_API_TTNR_CFA_FINE_SMOOTH_STRENGTH               (_TTNR_START + 0x24)    // [0, 16383]
#define ISP_API_TTNR_CFA_EDGE_SMOOTH_STRENGTH               (_TTNR_START + 0x25)    // [0, 16383]
#define ISP_API_TTNR_CFA_EDGE_THRESH_LOW                    (_TTNR_START + 0x26)    // [0, 255]
#define ISP_API_TTNR_CFA_EDGE_THRESH_HIGH                   (_TTNR_START + 0x27)    // [0, 255]
#define ISP_API_TTNR_CFA_MOTION_ADJ_LOW_STRENGTH            (_TTNR_START + 0x28)    // [0, 255]
#define ISP_API_TTNR_CFA_MOTION_ADJ_MIN_STRENGTH            (_TTNR_START + 0x29)    // [0, 255]
#define ISP_API_TTNR_CFA_MOTION_ADJ_HIGH_STRENGTH           (_TTNR_START + 0x2A)    // [0, 255]
#define ISP_API_TTNR_CFA_MOTION_ADJ_LOW                     (_TTNR_START + 0x2B)    // [0, 253]
#define ISP_API_TTNR_CFA_MOTION_ADJ_LOW_DELTA               (_TTNR_START + 0x2C)    // [0, 7]
#define ISP_API_TTNR_CFA_MOTION_ADJ_HIGH                    (_TTNR_START + 0x2D)    // [0, 254]
#define ISP_API_TTNR_CFA_MOTION_ADJ_HIGH_DELTE              (_TTNR_START + 0x2E)    // [0, 7]

#define ISP_API_TTNR_TEMP_BLACK_LEVEL_R                     (_TTNR_START + 0x30)    // [0, 4095]
#define ISP_API_TTNR_TEMP_BLACK_LEVEL_G                     (_TTNR_START + 0x31)    // [0, 4095]
#define ISP_API_TTNR_TEMP_BLACK_LEVEL_B                     (_TTNR_START + 0x32)    // [0, 4095]
#define ISP_API_TTNR_TEMP_WB_GAIN_R                         (_TTNR_START + 0x33)    // [0, 16773120]
#define ISP_API_TTNR_TEMP_WB_GAIN_GR                        (_TTNR_START + 0x34)    // [0, 16773120]
#define ISP_API_TTNR_TEMP_WB_GAIN_B                         (_TTNR_START + 0x35)    // [0, 16773120]
#define ISP_API_TTNR_TEMP_WB_GAIN_GB                        (_TTNR_START + 0x36)    // [0, 16773120]

#define ISP_API_TTNR_TEMP_FBUFFER                         (_TTNR_START + 0X37)    // [0, 1]
#define _TTNR_END                                          (_TTNR_START + 0x37)

// #define ISP_API_TOTAL_SIZE                                  (_TTNR_END + 0x01)

// #endif
