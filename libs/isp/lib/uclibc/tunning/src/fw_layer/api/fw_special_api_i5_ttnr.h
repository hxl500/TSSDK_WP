// #ifndef __FW_SPECIAL_API_I5_TTNR_H__
// #define __FW_SPECIAL_API_I5_TTNR_H__


#define SPECIAL_API_TTNR_START                              (SPECIAL_API_SENSOR_GAIN_END + 0x01)
#define SPECIAL_API_TTNR_MANUAL_MODE                        (SPECIAL_API_TTNR_START + 0x00)     // [0, 1]   385
#define SPECIAL_API_TTNR_TONE_CURVE_BASIC_SELECTION         (SPECIAL_API_TTNR_START + 0x01)     // [0, 20]  386
#define SPECIAL_API_TTNR_TEMP_SMOOTH                        (SPECIAL_API_TTNR_START + 0x02)     // [0, 255] 256? 387
#define SPECIAL_API_TTNR_TEMP_CMP_SELECT                    (SPECIAL_API_TTNR_START + 0x03)     // [0, 1] 388

#define SPECIAL_API_TTNR_END                                (SPECIAL_API_TTNR_START + 0x04)     // id = 389

// #define SPECIAL_API_TOTAL_SIZE                              (SPECIAL_API_TTNR_END + 0x01)

// #endif
