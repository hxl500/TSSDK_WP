#include <common.h>
#include <dm.h>
#include <log.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/io.h>
#include <asm/arch/sys_proto.h>
#include <dm/of_access.h>
#include <dm/ofnode.h>
#include <errno.h>
#include <clk.h>

#include "ts_eccp_curve.h"

//SM2 algorithm parameters
u32 sm2p256v1_p[8]    = {0xFFFFFFFF,0xFFFFFFFF,0x00000000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFE};
u32 sm2p256v1_p_h[8]  = {0x00000003,0x00000002,0xFFFFFFFF,0x00000002,0x00000001,0x00000001,0x00000002,0x00000004};
u32 sm2p256v1_p_n1[1] = {1,};
u32 sm2p256v1_a[8]    = {0xFFFFFFFC,0xFFFFFFFF,0x00000000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFE};
u32 sm2p256v1_b[8]    = {0x4D940E93,0xDDBCBD41,0x15AB8F92,0xF39789F5,0xCF6509A7,0x4D5A9E4B,0x9D9F5E34,0x28E9FA9E};
u32 sm2p256v1_Gx[8]   = {0x334C74C7,0x715A4589,0xF2660BE1,0x8FE30BBF,0x6A39C994,0x5F990446,0x1F198119,0x32C4AE2C};
u32 sm2p256v1_Gy[8]   = {0x2139F0A0,0x02DF32E5,0xC62A4740,0xD0A9877C,0x6B692153,0x59BDCEE3,0xF4F6779C,0xBC3736A2};
u32 sm2p256v1_n[8]    = {0x39D54123,0x53BBF409,0x21C6052B,0x7203DF6B,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFE};
u32 sm2p256v1_n_h[8]  = {0x7C114F20,0x901192AF,0xDE6FA2FA,0x3464504A,0x3AFFE0D4,0x620FC84C,0xA22B3D3B,0x1EB5E412};
u32 sm2p256v1_n_n1[1] = {0x72350975,};

//SM2 para (n-1), for private key checking
u32 sm2p256v1_n_1[8]  = {0x39D54122,0x53BBF409,0x21C6052B,0x7203DF6B,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFE};

eccp_curve_t sm2_curve[1] = {
  { 256,
	256,
	sm2p256v1_p,
	sm2p256v1_p_h,
	sm2p256v1_p_n1,
	sm2p256v1_a,
	sm2p256v1_b,
	sm2p256v1_Gx,
	sm2p256v1_Gy,
	sm2p256v1_n,
	sm2p256v1_n_h,
	sm2p256v1_n_n1,
  }
};
