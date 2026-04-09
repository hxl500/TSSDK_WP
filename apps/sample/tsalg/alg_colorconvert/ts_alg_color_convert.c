#include <stdlib.h>
#include <math.h>
#include <arm_neon.h>

#include "ts_alg_color_convert.h"

TS_S32 TS_ALG_NV12ToRGB(TS_U8 *yuyv, TS_U8 *rgb, TS_U32 u32ImgW, TS_U32 u32ImgH, ALG_COLOR_TYPE_E eRgb)
{
    const TS_S32 nv_start = u32ImgW * u32ImgH;

	if((yuyv == NULL) || (rgb == NULL) || (0 != (u32ImgW&15)) || (0 != (u32ImgH&1)))
	{
		ALG_LOGE("error, Invalid parameter!\n");
		return TS_FAILURE;
	}
	// rgb coeff
	int16x8_t coeff = vmovq_n_s16(128);
	TS_S16 r_coeff = 90;
	TS_S16 g1_coeff = 22;
	TS_S16 g2_coeff = 46;
	TS_S16 b_coeff = 114;
	// TS_S16 shift_num = 6;

	switch (eRgb)
	{
	case ALG_COLOR_TYPE_RGB24:
		{
			TS_S32 rgb_ch = 3;
			for (TS_U32 i = 0; i < u32ImgH; i+=2) {

				//calc two line per time
				TS_U8 *pY1 = yuyv + i*u32ImgW;
				TS_U8 *pY2 = yuyv + (i+1)*u32ImgW;
				TS_U8 *pUv = yuyv + nv_start + (i>>1)*u32ImgW;
				TS_U8 *pRgb1 = rgb + i*u32ImgW*rgb_ch;
				TS_U8 *pRgb2 = rgb + (i+1)*u32ImgW*rgb_ch;

				TS_U32 j = 0;
				for (; j < u32ImgW>>4; j++) {
					// calc first line
					//get y u v data
					uint8x8_t dy0  = vld1_u8(pY1);
					uint8x8_t dy1  = vld1_u8(pY1+8);
					uint8x8x2_t duv  = vld2_u8(pUv);

					//extend y 8bit -> 16bit
					uint16x8_t dy00 = vshlq_n_u16(vmovl_u8(dy0), 6);
					uint16x8_t dy01 = vshlq_n_u16(vmovl_u8(dy1), 6);
					
					//extract u v data and convert 8bit -> 16bit
					uint16x8_t du00 = vmovl_u8(duv.val[0]);
					uint16x8_t dv00 = vmovl_u8(duv.val[1]);

					// calc rgb
					int16x8_t du = vsubq_s16((int16x8_t)du00, coeff);
					int16x8_t dv = vsubq_s16((int16x8_t)dv00, coeff);

					int16x8x2_t dv2 = vzipq_s16(dv, dv);
					int16x8_t dr0 = vmlaq_n_s16((int16x8_t)dy00, dv2.val[0], r_coeff);
					int16x8_t dr1 = vmlaq_n_s16((int16x8_t)dy01, dv2.val[1], r_coeff);
					uint8x16x3_t rgb0;
					rgb0.val[0] = vcombine_u8(vqrshrun_n_s16(dr0, 6), vqrshrun_n_s16(dr1, 6));

					int16x8x2_t du2 = vzipq_s16(du, du);
					int16x8_t dg0 = vmlsq_n_s16((int16x8_t)dy00, du2.val[0], g1_coeff);
					int16x8_t dg1 = vmlsq_n_s16((int16x8_t)dy01, du2.val[1], g1_coeff);
					dg0 = vmlsq_n_s16(dg0, dv2.val[0], g2_coeff);
					dg1 = vmlsq_n_s16(dg1, dv2.val[1], g2_coeff);
					rgb0.val[1] = vcombine_u8(vqrshrun_n_s16(dg0, 6), vqrshrun_n_s16(dg1, 6));

					int16x8_t db0 = vmlaq_n_s16((int16x8_t)dy00, du2.val[0], b_coeff);
					int16x8_t db1 = vmlaq_n_s16((int16x8_t)dy01, du2.val[1], b_coeff);
					rgb0.val[2] = vcombine_u8(vqrshrun_n_s16(db0, 6), vqrshrun_n_s16(db1, 6));

					// store rgb
					vst3q_u8(pRgb1, rgb0);

					// calc second line
					dy0  = vld1_u8(pY2);
					dy1  = vld1_u8(pY2+8);
					
					dy00 = vshlq_n_u16(vmovl_u8(dy0), 6);
					dy01 = vshlq_n_u16(vmovl_u8(dy1), 6);

					uint8x16x3_t rgb1;
					dr0 = vmlaq_n_s16((int16x8_t)dy00, dv2.val[0], r_coeff);
					dr1 = vmlaq_n_s16((int16x8_t)dy01, dv2.val[1], r_coeff);
					rgb1.val[0] = vcombine_u8(vqrshrun_n_s16(dr0, 6), vqrshrun_n_s16(dr1, 6));

					dg0 = vmlsq_n_s16((int16x8_t)dy00, du2.val[0], g1_coeff);
					dg1 = vmlsq_n_s16((int16x8_t)dy01, du2.val[1], g1_coeff);
					dg0 = vmlsq_n_s16(dg0, dv2.val[0], g2_coeff);
					dg1 = vmlsq_n_s16(dg1, dv2.val[1], g2_coeff);
					rgb1.val[1] = vcombine_u8(vqrshrun_n_s16(dg0, 6), vqrshrun_n_s16(dg1, 6));

					db0 = vmlaq_n_s16((int16x8_t)dy00, du2.val[0], b_coeff);
					db1 = vmlaq_n_s16((int16x8_t)dy01, du2.val[1], b_coeff);
					rgb1.val[2] = vcombine_u8(vqrshrun_n_s16(db0, 6), vqrshrun_n_s16(db1, 6));

					vst3q_u8(pRgb2, rgb1);

					pY1 += 16;
					pY2 += 16;
					pUv += 16;
					pRgb1 += 16*rgb_ch;
					pRgb2 += 16*rgb_ch;
				}
			}
		}
		break;
	case ALG_COLOR_TYPE_RGBA32:
		{
			TS_S32 rgb_ch = 4;
			for (TS_U32 i = 0; i < u32ImgH; i+=2) {
				TS_U8 *pY1 = yuyv + i*u32ImgW;
				TS_U8 *pY2 = yuyv + (i+1)*u32ImgW;
				TS_U8 *pUv = yuyv + nv_start + (i>>1)*u32ImgW;
				TS_U8 *pRgb1 = rgb + i*u32ImgW*rgb_ch;
				TS_U8 *pRgb2 = rgb + (i+1)*u32ImgW*rgb_ch;

				TS_U32 j = 0;
				for (; j < u32ImgW>>4; j++) {
					// calc first line
					//get y u v data
					uint8x8_t dy0  = vld1_u8(pY1);
					uint8x8_t dy1  = vld1_u8(pY1+8);
					uint8x8x2_t duv  = vld2_u8(pUv);

					//extend y 8bit -> 16bit
					uint16x8_t dy00 = vshlq_n_u16(vmovl_u8(dy0), 6);
					uint16x8_t dy01 = vshlq_n_u16(vmovl_u8(dy1), 6);
					
					//extract u v data and convert 8bit -> 16bit
					uint16x8_t du00 = vmovl_u8(duv.val[0]);
					uint16x8_t dv00 = vmovl_u8(duv.val[1]);

					// calc rgb
					int16x8_t du = vsubq_s16((int16x8_t)du00, coeff);
					int16x8_t dv = vsubq_s16((int16x8_t)dv00, coeff);

					int16x8x2_t dv2 = vzipq_s16(dv, dv);
					int16x8_t dr0 = vmlaq_n_s16((int16x8_t)dy00, dv2.val[0], r_coeff);
					int16x8_t dr1 = vmlaq_n_s16((int16x8_t)dy01, dv2.val[1], r_coeff);
					uint8x16x4_t rgb0;
					rgb0.val[0] = vcombine_u8(vqrshrun_n_s16(dr0, 6), vqrshrun_n_s16(dr1, 6));

					int16x8x2_t du2 = vzipq_s16(du, du);
					int16x8_t dg0 = vmlsq_n_s16((int16x8_t)dy00, du2.val[0], g1_coeff);
					int16x8_t dg1 = vmlsq_n_s16((int16x8_t)dy01, du2.val[1], g1_coeff);
					dg0 = vmlsq_n_s16(dg0, dv2.val[0], g2_coeff);
					dg1 = vmlsq_n_s16(dg1, dv2.val[1], g2_coeff);
					rgb0.val[1] = vcombine_u8(vqrshrun_n_s16(dg0, 6), vqrshrun_n_s16(dg1, 6));

					int16x8_t db0 = vmlaq_n_s16((int16x8_t)dy00, du2.val[0], b_coeff);
					int16x8_t db1 = vmlaq_n_s16((int16x8_t)dy01, du2.val[1], b_coeff);
					rgb0.val[2] = vcombine_u8(vqrshrun_n_s16(db0, 6), vqrshrun_n_s16(db1, 6));

					// store rgb
					vst4q_u8(pRgb1, rgb0);

					// calc second line
					dy0  = vld1_u8(pY2);
					dy1  = vld1_u8(pY2+8);
					
					dy00 = vshlq_n_u16(vmovl_u8(dy0), 6);
					dy01 = vshlq_n_u16(vmovl_u8(dy1), 6);

					uint8x16x4_t rgb1;
					dr0 = vmlaq_n_s16((int16x8_t)dy00, dv2.val[0], r_coeff);
					dr1 = vmlaq_n_s16((int16x8_t)dy01, dv2.val[1], r_coeff);
					rgb1.val[0] = vcombine_u8(vqrshrun_n_s16(dr0, 6), vqrshrun_n_s16(dr1, 6));

					dg0 = vmlsq_n_s16((int16x8_t)dy00, du2.val[0], g1_coeff);
					dg1 = vmlsq_n_s16((int16x8_t)dy01, du2.val[1], g1_coeff);
					dg0 = vmlsq_n_s16(dg0, dv2.val[0], g2_coeff);
					dg1 = vmlsq_n_s16(dg1, dv2.val[1], g2_coeff);
					rgb1.val[1] = vcombine_u8(vqrshrun_n_s16(dg0, 6), vqrshrun_n_s16(dg1, 6));

					db0 = vmlaq_n_s16((int16x8_t)dy00, du2.val[0], b_coeff);
					db1 = vmlaq_n_s16((int16x8_t)dy01, du2.val[1], b_coeff);
					rgb1.val[2] = vcombine_u8(vqrshrun_n_s16(db0, 6), vqrshrun_n_s16(db1, 6));

					vst4q_u8(pRgb2, rgb1);

					pY1 += 16;
					pY2 += 16;
					pUv += 16;
					pRgb1 += 16*rgb_ch;
					pRgb2 += 16*rgb_ch;
				}
			}
		}
		break;
	default:
		ALG_LOGE("error, Invalid rgb type!\n");
		break;
	}  
	return TS_SUCCESS;
}

