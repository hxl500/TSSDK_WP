

#ifndef __SAMPLE_COMM_UTIL_H__
#define __SAMPLE_COMM_UTIL_H__

#include "sample_comm_base.h"

/*******************************************************
 *    macro define
 *******************************************************/
#define FILE_NAME_LEN               128

#define CHECK_CHN_RET(express, Chn, name)\
	do {\
		TS_S32 Ret;\
		Ret = express;\
		if (TS_SUCCESS != Ret) {\
			printf("\033[0;31m%s chn %d failed at %s: LINE: %d with %#x!\033[0;39m\n", \
				name, Chn, __func__, __LINE__, Ret);\
			fflush(stdout);\
			return Ret;\
		} \
	} while (0)

#define CHECK_RET(express, name)\
	do {\
		TS_S32 Ret;\
		Ret = express;\
		if (TS_SUCCESS != Ret) {\
			printf("\033[0;31m%s failed at %s: LINE: %d with %#x!\033[0;39m\n",\
				name, __func__, __LINE__, Ret);\
			return Ret;\
		} \
	} while (0)

#define CHECK_RET_GOTO(express, label)                                                                                 \
	do {                                                                                                           \
		int Ret;                                                                                               \
		Ret = express;                                                                                         \
		if (TS_SUCCESS != Ret) {                                                                                   \
			printf("%s failed at  LINE: %d with %d\n", #express, __LINE__, Ret);                             \
			goto label;                                                                                    \
		}                                                                                                      \
	} while (0)

#define PAUSE()  do {\
		printf("---------------press Enter key to exit!---------------\n");\
		getchar();\
	} while (0)


#define SAMPLE_PRT(fmt...)   \
	do {\
		printf("[%s]-%d: ", __func__, __LINE__);\
		printf(fmt);\
	} while (0)

#define CHECK_NULL_PTR(ptr)\
	do {\
		if (NULL == ptr) {\
			printf("func:%s,line:%d, NULL pointer\n", __func__, __LINE__);\
			return TS_FAILURE;\
		} \
	} while (0)

/*
 * 1. add (void)(&dst == &src);\ for compile warning when types are different
 * 2. add COMPILE_ERR_WHILE_TRUE or _Static_assert for compile error when sizes are different
 * 3. NO call limit such as dst++, src++
 */
#define STRUCT_PTR_COPY(dst,src) do {\
	typeof(dst) _dst = (dst);\
	typeof(src) _src = (src);\
	(void)(dst == src);\
	_Static_assert(sizeof(*_dst) == sizeof(*_src), "sizes of struct are different !");\
	memcpy(_dst, _src, sizeof(*_dst) < sizeof(*_src) ? sizeof(*_dst) : sizeof(*_src));\
	} while(0)

#ifndef CHECK_RET_GOTO
#define CHECK_RET_GOTO(express, label)                                                                                 \
	do {                                                                                                           \
		int Ret;                                                                                               \
		Ret = express;                                                                                         \
		if (TS_SUCCESS != Ret) {                                                                                   \
			SAMPLE_PRT("%s failed at  LINE: %d with %d\n", #express, __LINE__, Ret);                             \
			goto label;                                                                                    \
		}                                                                                                      \
	} while (0)
#endif

typedef struct {
	TS_S32 width;
	TS_S32 height;
} OPT_VI_CHNL_t;

typedef struct {
	TS_BOOL bBitWidInit;
	DATA_BITWIDTH_E bitwidth;
	TS_BOOL bBayerFormatInit;
	ISP_BAYER_FORMAT_E bayerformat;
	TS_FLOAT	fps;
	TS_BOOL wdr;
	TS_BOOL ispbyfly;
	TS_S32 width_pipe;
	TS_S32 height_pipe;
	TS_BOOL userpic;
	OPT_VI_CHNL_t	chnl;
} OPT_VI_t;

typedef struct {
	TS_S32 width_out;
	TS_S32 height_out;
	RECT_S chnl_crop;
	TS_S32 memCount;
	TS_BOOL bMemCountInit;
} OPT_VPSS_CHNL_t;

typedef struct {
	TS_S32 width_crop;
	TS_S32 height_crop;
	RECT_S	grp_crop;
	TS_S32 chnl_count;
	TS_S32 biggest_index;
	OPT_VPSS_CHNL_t	chnl[VPSS_MAX_CHN_NUM];
} OPT_VPSS_t;

typedef struct {
	TS_S32 gop;
	
	SAMPLE_RC_E rc_type;
	TS_BOOL bRcTypeValid;
	
	TS_S32 framerate;
	TS_S32 bitrate;
	
	PAYLOAD_TYPE_E pt_type;
	TS_BOOL	bPtTypeValid;
	
	TS_S32 width_yuv;
	TS_S32 height_yuv;
} OPT_VENC_CHNL_t;

typedef struct {
	OPT_VENC_CHNL_t	chnl[VENC_MAX_CHN_NUM];
} OPT_VENC_t;

typedef struct {
} OPT_VO_t;

typedef struct {
	TS_S32	choise;
	
	TS_BOOL	bFastboot;	
	TS_BOOL	bAutoreset;	
	TS_BOOL	bStitch;
	TS_BOOL	bLoop;
	
	TS_BOOL	bSilence;
	TS_S32	camNum;
	
	TS_BOOL	bSwtichMixer;
	
	TS_BOOL vivpss_bOnline;//vivpss online
	TS_BOOL bValid_VivpssOnlie;
	TS_BOOL	bFeedRaw;
	OPT_VI_t	vi;
	OPT_VPSS_t	vpss;
	OPT_VENC_t	venc;

	TS_BOOL		bFilePathValid;
	TS_CHAR		filePath[FILE_NAME_LEN];

	TS_BOOL		bInfileValid;
	TS_CHAR		infilePath[FILE_NAME_LEN];//用于灌yuv

	TS_BOOL		bOutfileValid;
	TS_CHAR		outfilePath[FILE_NAME_LEN]; // 用于保存编码视频数据

	TS_BOOL	alg_face;
	TS_BOOL	alg_pcpp;
	TS_BOOL alg_motion;
} ALL_OPT_t;

typedef TS_S32 (* pf_check_args)(ALL_OPT_t * pOpts);

TS_S32 sample_comm_parse_args(int argc, char *argv[], int max_choise, pf_check_args pf_check, ALL_OPT_t * pAll_opt);

#endif /* End of #ifndef __SAMPLE_COMMON_H__ */
