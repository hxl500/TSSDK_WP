
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "sample_comm_util.h"

typedef enum {
	_SAMPLE_COMM_OPT_fastboot = 1,
	_SAMPLE_COMM_OPT_autoReset,
	_SAMPLE_COMM_OPT_ispOnline,
	_SAMPLE_COMM_OPT_vpssOnline,
	_SAMPLE_COMM_OPT_switchMixer,
	_SAMPLE_COMM_OPT_camNum,
	_SAMPLE_COMM_OPT_bitWidth,
	_SAMPLE_COMM_OPT_bayer,
	_SAMPLE_COMM_OPT_wdr,
	_SAMPLE_COMM_OPT_viFps,
	_SAMPLE_COMM_OPT_viSize,
	_SAMPLE_COMM_OPT_viUserpic,
	_SAMPLE_COMM_OPT_feedRaw,
	_SAMPLE_COMM_OPT_vpssSize,
	_SAMPLE_COMM_OPT_vpssGrpCrop,
	_SAMPLE_COMM_OPT_vpssChnCrop,
	_SAMPLE_COMM_OPT_memCount,
	_SAMPLE_COMM_OPT_gop,
	_SAMPLE_COMM_OPT_encFrate,
	_SAMPLE_COMM_OPT_encBrate,
	_SAMPLE_COMM_OPT_rcType,
	_SAMPLE_COMM_OPT_encType,
	_SAMPLE_COMM_OPT_yuvSize,
	_SAMPLE_COMM_OPT_filePath,
	_SAMPLE_COMM_OPT_infilePath,
	_SAMPLE_COMM_OPT_outfilePath,
	_SAMPLE_COMM_OPT_face,
	_SAMPLE_COMM_OPT_pcpp,
	_SAMPLE_COMM_OPT_motion,
	_SAMPLE_COMM_OPT_BUTT
} sample_comm_opt_e;

#define SAMPLE_COMM_STR_LEN_MAX 32
#define SAMPLE_COMM_STR_TBL_COUNT 32

typedef struct {
	TS_CHAR string_tbl[SAMPLE_COMM_STR_TBL_COUNT][SAMPLE_COMM_STR_LEN_MAX];
	TS_S32 valid_count;
} sample_comm_string_tbl_t;

TS_S32 sample_comm_args_help()
{
	SAMPLE_PRT("[version][-v] : show version info\n");
	SAMPLE_PRT("[help][-h] : show help info\n");
	
	SAMPLE_PRT("[ispOnline][--ispOnline] : Enable ISP dual online\n");
	SAMPLE_PRT("[vpssOnline][--vpssOnline] : Enable VI-VPSS online\n");
	SAMPLE_PRT("[camNum][--camNum] : Set the number of cameras\n");
	
	SAMPLE_PRT("[bitWidth][--bitWidth] : Set the number of bits for sensor input\n");
	SAMPLE_PRT("[bayer][--bayer] : Set the input format of the sensor\n");
	SAMPLE_PRT("[viFps][--viFps] : Set the input frame rate of the sensor\n");
	SAMPLE_PRT("[viSize][--viSize] : Set the input size of the sensor\n");
	SAMPLE_PRT("[viUserpic][--viUserpic] : Set the vi userPic enable\n");
	SAMPLE_PRT("[feedRaw][--feedRaw] : Set sendRaw enable\n");

	SAMPLE_PRT("[vpssSize][--vpssSize] : Set the output size of the vpss channel\n");
	SAMPLE_PRT("[memCount][--memCount] : Set the number of ping pong in the VPSS channel\n");

	SAMPLE_PRT("[gop][--gop] : Set GOP\n");
	SAMPLE_PRT("[encFrate][--encFrate] : Set the frame rate\n");
	SAMPLE_PRT("[encBrate][--encBrate] : Set the bitrate\n");
	SAMPLE_PRT("[rcType][--rcType] : Set the bitrate control type\n");
	SAMPLE_PRT("[encType][--encType] : Set the encoding type\n");
	SAMPLE_PRT("[infilePath][--infilePath] : Set the input path of the YUV file\n");
	SAMPLE_PRT("[outfilePath][--outfilePath] : Set the path to save the video recording file\n");
	SAMPLE_PRT("[yuvSize][--yuvSize] : Set yuv recharge resolution\n");
	
	SAMPLE_PRT("\n");
	SAMPLE_PRT(" Example:\n");
	SAMPLE_PRT("    sample_venc 0 --vpssOnline --vpssSize=2mp --viFps=15 --encFrate=15\n");
	SAMPLE_PRT("\n");
	
	return TS_SUCCESS;
}

TS_S32 sample_comm_args_getBitwidth(TS_CHAR *IN pString, DATA_BITWIDTH_E *OUT pBitwidth)
{
	if (!strcmp(pString, "bit8") || !strcmp(pString, "Bit8")) {
		*pBitwidth = DATA_BITWIDTH_8;
	} else if (!strcmp(pString, "bit10") || !strcmp(pString, "Bit10")) {
		*pBitwidth = DATA_BITWIDTH_10;
	} else if (!strcmp(pString, "bit12") || !strcmp(pString, "Bit12")) {
		*pBitwidth = DATA_BITWIDTH_12;
	} else if (!strcmp(pString, "bit14") || !strcmp(pString, "Bit14")) {
		*pBitwidth = DATA_BITWIDTH_14;
	} else if (!strcmp(pString, "bit16") || !strcmp(pString, "Bit16")) {
		*pBitwidth = DATA_BITWIDTH_16;
	} else {
		return TS_FAILURE;
	}
	return TS_SUCCESS;
}

TS_S32 sample_comm_args_getBayerPattern(TS_CHAR *IN pString, ISP_BAYER_FORMAT_E *OUT pBayer)
{
	if (!strcmp(pString, "rggb") || !strcmp(pString, "RGGB")) {
		*pBayer = BAYER_RGGB;
	} else if (!strcmp(pString, "grbg") || !strcmp(pString, "GRBG")) {
		*pBayer = BAYER_GRBG;
	} else if (!strcmp(pString, "gbrg") || !strcmp(pString, "GBRG")) {
		*pBayer = BAYER_GBRG;
	} else if (!strcmp(pString, "bggr") || !strcmp(pString, "BGGR")) {
		*pBayer = BAYER_BGGR;
	} else {
		return TS_FAILURE;
	}
	return TS_SUCCESS;
}

/*
TS_S32 sample_comm_args_getWdr(TS_CHAR *IN pString, WDR_MODE_E *OUT pWdr)
{
	if (!strcmp(pString, "none") || !strcmp(pString, "line")) {
		*pWdr = WDR_MODE_NONE;
	} else if (!strcmp(pString, "dol2")) {
		*pWdr = WDR_MODE_2To1_LINE;
	} else if (!strcmp(pString, "stagger")) {
		*pWdr = WDR_MODE_2To1_FRAME;
	} else {
		return TS_FAILURE;
	}
	return TS_SUCCESS;
}
*/

TS_S32 sample_comm_args_getRcType(TS_CHAR *IN pString, SAMPLE_RC_E * OUT pRcType)
{
	if (!strcmp(pString, "cbr") || !strcmp(pString, "CBR")) {
		*pRcType = SAMPLE_RC_CBR;
	} else if (!strcmp(pString, "vbr") || !strcmp(pString, "VBR")) {
		*pRcType = SAMPLE_RC_VBR;
	} else if (!strcmp(pString, "avbr") || !strcmp(pString, "AVBR")) {
		*pRcType = SAMPLE_RC_AVBR;
	} else if (!strcmp(pString, "qvbr") || !strcmp(pString, "QVBR")) {
		*pRcType = SAMPLE_RC_QVBR;
	} else if (!strcmp(pString, "cvbr") || !strcmp(pString, "CVBR")) {
		*pRcType = SAMPLE_RC_CVBR;
	} else if (!strcmp(pString, "avbr") || !strcmp(pString, "AVBR")) {
		*pRcType = SAMPLE_RC_AVBR;
	} else if (!strcmp(pString, "qpmap") || !strcmp(pString, "QPMAP")) {
		*pRcType = SAMPLE_RC_QPMAP;
	} else if (!strcmp(pString, "fixqp") || !strcmp(pString, "FIXQP")) {
		*pRcType = SAMPLE_RC_FIXQP;
	} else {
		return TS_FAILURE;
	}
	return TS_SUCCESS;
}

TS_S32 sample_comm_args_getPayloadType(TS_CHAR *IN pString, PAYLOAD_TYPE_E * OUT pPayloadType)
{
	if (!strcmp(pString, "h264") || !strcmp(pString, "H264")) {
		*pPayloadType = PT_H264;
	} else if (!strcmp(pString, "h265") || !strcmp(pString, "H265")) {
		*pPayloadType = PT_H265;
	} else if (!strcmp(pString, "mjpeg") || !strcmp(pString, "MJPEG")) {
		*pPayloadType = PT_MJPEG;
	} else if (!strcmp(pString, "jpg") || !strcmp(pString, "JPG") || !strcmp(pString, "jpeg") 
		|| !strcmp(pString, "JPEG")) {
		*pPayloadType = PT_JPEG;
	} else {
		return TS_FAILURE;
	}
	return TS_SUCCESS;
}

TS_S32 sample_comm_args_getSize(TS_CHAR *IN pString, TS_S32 *OUT pWidth, TS_S32 *OUT pHeight)
{
	if (!strcmp(pString, "1920x1080") || !strcmp(pString, "1080P") || !strcmp(pString, "1080p") ||
	    !strcmp(pString, "2mp") || !strcmp(pString, "2MP")) {
		*pWidth = 1920;
		*pHeight = 1080;
	} else if (!strcmp(pString, "1280x720") || !strcmp(pString, "720p") || !strcmp(pString, "720P") ||
		   !strcmp(pString, "1mp") || !strcmp(pString, "1MP")) {
		*pWidth = 1280;
		*pHeight = 720;
	} else if (!strcmp(pString, "2560x1440") || !strcmp(pString, "4mp") || !strcmp(pString, "4MP")) {
		*pWidth = 2560;
		*pHeight = 1440;
	} else if (!strcmp(pString, "3840x2160") || !strcmp(pString, "4k") || !strcmp(pString, "4K")) {
		*pWidth = 3840;
		*pHeight = 2160;
	} else if (!strcmp(pString, "2688x1512") || !strcmp(pString, "1512p") || !strcmp(pString, "1512P")) {
		*pWidth = 2688;
		*pHeight = 1512;
	} else if (!strcmp(pString, "2688x1520") || !strcmp(pString, "1520p") || !strcmp(pString, "1520P")) {
		*pWidth = 2688;
		*pHeight = 1520;
	} else if (!strcmp(pString, "2880x1624") || !strcmp(pString, "5mp") || !strcmp(pString, "5MP")) {
		*pWidth = 2880;
		*pHeight = 1624;
	} else if (!strcmp(pString, "8192x8192") || !strcmp(pString, "8k") || !strcmp(pString, "8K")) {
		*pWidth = 8192;
		*pHeight = 8192;
	} else {
		*pWidth = 0;
		*pHeight = 0;
		return TS_FAILURE;
	}
	return TS_SUCCESS;
}

TS_S32 sample_comm_args_getStringTbl(TS_CHAR *IN pOptarg, TS_S32 IN max_count, sample_comm_string_tbl_t *pStrTbl)
{
	TS_S32 count = 0;
	TS_CHAR tmpBuffer[128] = { 0 };
	const char *pos = tmpBuffer;

	if (!pOptarg || !pStrTbl) {
		SAMPLE_PRT("null ptr\n");
		return TS_FAILURE;
	}

	strcpy(tmpBuffer, pOptarg);

	if (!strstr(tmpBuffer, ",")) {
		strcpy(pStrTbl->string_tbl[0], tmpBuffer);
		pStrTbl->valid_count = 1;
		return TS_SUCCESS;
	}

	while (count < max_count && sscanf(pos, "%31[^,],", pStrTbl->string_tbl[count]) == 1) {
		pos += strlen(pStrTbl->string_tbl[count]) + 1;
		count++;
	}

	pStrTbl->valid_count = count;

	return TS_SUCCESS;
}

TS_S32 sample_comm_args_commCheck(ALL_OPT_t *pAll_opt, ALL_OPT_t *pNew_opt, TS_S32 max_venc_chnl)
{
	TS_S32 i;
	TS_S32 max_chnl_val = 0;

	if (!pAll_opt || !pNew_opt) {
		SAMPLE_PRT("null ptr\n");
		return TS_FAILURE;
	}
	
	if (pNew_opt->vpss.chnl_count > VPSS_MAX_CHN_NUM) {
		SAMPLE_PRT("vpss chnl count [%d] is invalid\n", pNew_opt->vpss.chnl_count);
		return TS_FAILURE;
	}

	if (pNew_opt->camNum < 0) {
		SAMPLE_PRT("cam num [%d] is invalid\n", pNew_opt->camNum);
	} else if (0 == pNew_opt->camNum) {
		pAll_opt->camNum = 1;
	} else if(pNew_opt->camNum > VI_MAX_PHY_PIPE_NUM){
		SAMPLE_PRT("cam num [%d] is out of range[1,%d], faild\n", 
			pNew_opt->camNum, VI_MAX_PHY_PIPE_NUM);
	} else {
		pAll_opt->camNum = pNew_opt->camNum;
	}

	pAll_opt->choise = pNew_opt->choise;
	pAll_opt->bFastboot = pNew_opt->bFastboot;
	pAll_opt->bAutoreset = pNew_opt->bAutoreset;
	pAll_opt->bStitch = pNew_opt->bStitch;
	pAll_opt->bLoop = pNew_opt->bLoop;
	pAll_opt->bSilence = pNew_opt->bSilence;

	if (pNew_opt->bFilePathValid) {
		strcpy(pAll_opt->filePath, pNew_opt->filePath);
		pAll_opt->bFilePathValid = TS_TRUE;
	}

	if (pNew_opt->bInfileValid) {
		strcpy(pAll_opt->infilePath, pNew_opt->infilePath);
		pAll_opt->bInfileValid = TS_TRUE;
	}

	if (pNew_opt->bOutfileValid) {
		strcpy(pAll_opt->outfilePath, pNew_opt->outfilePath);
		pAll_opt->bOutfileValid = TS_TRUE;
	}

	if (pNew_opt->camNum != 2 && pNew_opt->vi.ispbyfly) {
		SAMPLE_PRT("ispbyfly only support one camera, current num is %d\n", pNew_opt->camNum);
		return TS_FAILURE;
	}

	pAll_opt->vivpss_bOnline = pNew_opt->vivpss_bOnline;
	pAll_opt->bValid_VivpssOnlie = pNew_opt->bValid_VivpssOnlie;

	pAll_opt->vi.ispbyfly = pNew_opt->vi.ispbyfly;
	pAll_opt->vi.wdr = pNew_opt->vi.wdr;
	pAll_opt->vi.bBitWidInit = pNew_opt->vi.bBitWidInit;
	pAll_opt->vi.bitwidth = pNew_opt->vi.bitwidth;
	pAll_opt->vi.bBayerFormatInit = pNew_opt->vi.bBayerFormatInit;
	pAll_opt->vi.bayerformat = pNew_opt->vi.bayerformat;
	pAll_opt->vi.fps = pNew_opt->vi.fps > 0 ? pNew_opt->vi.fps : 15.0;
	pAll_opt->vi.userpic = pNew_opt->vi.userpic;

	if(pAll_opt->vi.userpic && (pAll_opt->vivpss_bOnline || pAll_opt->bAutoreset)){
		SAMPLE_PRT("viUserpic conflict with autoreset or vivpss online mode\n");
		return TS_FAILURE;
	}

	/*	
		viSize

		1. 如果参数配了viSize，以这个配置为准
		2. 如果1不满足，且vpssSize不为空，则参照vpssSize中最大的一路输出
		3. 如果2不满足，且default不为空，则参照default (default可以为空)
	*/
	if (pNew_opt->vi.width_pipe > 0 && pNew_opt->vi.height_pipe) {
		pAll_opt->vi.width_pipe = pNew_opt->vi.width_pipe;
		pAll_opt->vi.height_pipe = pNew_opt->vi.height_pipe;
	} else if (pNew_opt->vpss.chnl_count > 0) {
		pAll_opt->vi.width_pipe = pNew_opt->vpss.chnl[pNew_opt->vpss.biggest_index].width_out;
		pAll_opt->vi.height_pipe = pNew_opt->vpss.chnl[pNew_opt->vpss.biggest_index].height_out;
	} else {
		/*
		可能存在viSize为零的情况。
		1. sample中没有填default
		2. 解码case
		*/
		pAll_opt->vi.width_pipe = 1920;
		pAll_opt->vi.height_pipe = 1080;
	}
	pAll_opt->bFeedRaw = pNew_opt->bFeedRaw;
	/* isp send raw judge */
	if(pNew_opt->bFeedRaw){
		TS_U32 bitwidth_value = 0;
		TS_U32 bitwidth_temp = pAll_opt->vi.bitwidth;
		switch(bitwidth_temp){
			case DATA_BITWIDTH_10:
				bitwidth_value = 10;
				break;
			case DATA_BITWIDTH_12:
				bitwidth_value = 12;
				break;
			case DATA_BITWIDTH_16:
				bitwidth_value = 16;
				break;
		}
		if(bitwidth_value == 0){
			SAMPLE_PRT("bFeedRaw is true, but bitwidth not support\n");
			return TS_FAILURE;
		}
		TS_U32 sinle_size = pAll_opt->vi.width_pipe * pAll_opt->vi.height_pipe * bitwidth_value / 8;
		
		if(pNew_opt->infilePath[0] == '0'){
			SAMPLE_PRT("bFeedRaw is true, but infilePath is NULL\n");
			return TS_FAILURE;
		}
		
		struct stat fileInfo = {0};
		if(stat(pNew_opt->infilePath, &fileInfo) == -1){
			SAMPLE_PRT("bFeedRaw is true, but infilePath %s stat err\n", pNew_opt->infilePath);
			return TS_FAILURE;
		}

		if(fileInfo.st_size == 0 || (fileInfo.st_size % sinle_size) != 0){
			SAMPLE_PRT("bFeedRaw is true, but infilePath %s content err, fileInfo.st_size=%d, single_size=%u\n", 
				pNew_opt->infilePath, (int)fileInfo.st_size, sinle_size);
			return TS_FAILURE;
		}
	}

	/*
		vpssSize

		1. 如果配置了vpssSize，以配置为准
		2. 如果1不满足，则默认vpss输出1路 (大小以viSize为准)		
	*/
	if (0 == pNew_opt->vpss.chnl_count) {
		if (pAll_opt->vi.width_pipe > 0 && pAll_opt->vi.height_pipe > 0) {
			pAll_opt->vpss.chnl_count = 1;
			pAll_opt->vpss.chnl[0].width_out = pAll_opt->vi.width_pipe;
			pAll_opt->vpss.chnl[0].height_out = pAll_opt->vi.height_pipe;
		}
	} else {
		for (i = 0; i < pNew_opt->vpss.chnl_count; i++) {
			pAll_opt->vpss.chnl[i].width_out = pNew_opt->vpss.chnl[i].width_out;
			pAll_opt->vpss.chnl[i].height_out = pNew_opt->vpss.chnl[i].height_out;
			pAll_opt->vpss.chnl[i].bMemCountInit = pNew_opt->vpss.chnl[i].bMemCountInit;
			pAll_opt->vpss.chnl[i].memCount = pNew_opt->vpss.chnl[i].memCount;
		}
		pAll_opt->vpss.chnl_count = pNew_opt->vpss.chnl_count;
	}

	max_chnl_val = pAll_opt->camNum * pAll_opt->vpss.chnl_count;

	if(max_venc_chnl > max_chnl_val){
		SAMPLE_PRT("the encoding parameters set exceed the number[%d] of channels, and the subsequent ones will be ignored.", 
			max_chnl_val);
	}

	if(1){
		TS_S32 idx_last_ptType = -1;
		TS_S32 idx_last_rcType = -1;
		TS_S32 idx_last_framerate = -1;
		TS_S32 idx_last_bitrate = -1;
		TS_S32 idx_last_gop = -1;
		
		for(i = 0; i < max_chnl_val; i ++){
			if(pNew_opt->venc.chnl[i].bPtTypeValid){
				idx_last_ptType = i;
				pAll_opt->venc.chnl[i].pt_type = pNew_opt->venc.chnl[i].pt_type;
			}
			else if(idx_last_ptType >= 0){
				pAll_opt->venc.chnl[i].pt_type = pNew_opt->venc.chnl[idx_last_ptType].pt_type;
				pAll_opt->venc.chnl[i].bPtTypeValid = TS_TRUE;
			}
			else{
				pAll_opt->venc.chnl[i].pt_type = PT_H265;
				pAll_opt->venc.chnl[i].bPtTypeValid = TS_TRUE;
			}

			if(pNew_opt->venc.chnl[i].bRcTypeValid){
				idx_last_rcType = i;
				pAll_opt->venc.chnl[i].rc_type = pNew_opt->venc.chnl[i].rc_type;
			}
			else if(idx_last_rcType >= 0){
				pAll_opt->venc.chnl[i].rc_type = pNew_opt->venc.chnl[idx_last_rcType].rc_type;
				pAll_opt->venc.chnl[i].bRcTypeValid = TS_TRUE;
			}
			else {
				pAll_opt->venc.chnl[i].rc_type = SAMPLE_RC_CBR;
				pAll_opt->venc.chnl[i].bRcTypeValid = TS_TRUE;
			}
			
			if(pNew_opt->venc.chnl[i].framerate > 0){
				idx_last_framerate = i;
				pAll_opt->venc.chnl[i].framerate = pNew_opt->venc.chnl[i].framerate;
			}
			else if(idx_last_framerate >= 0){
				pAll_opt->venc.chnl[i].framerate = pNew_opt->venc.chnl[idx_last_framerate].framerate;
			}
			else{
				pAll_opt->venc.chnl[i].framerate = pAll_opt->vi.fps;
			}

			if(pNew_opt->venc.chnl[i].bitrate > 0){
				idx_last_bitrate = i;
				pAll_opt->venc.chnl[i].bitrate = pNew_opt->venc.chnl[i].bitrate;
			}
			else if(idx_last_bitrate >= 0){
				pAll_opt->venc.chnl[i].bitrate = pNew_opt->venc.chnl[idx_last_bitrate].bitrate;
			}
			
			if(pNew_opt->venc.chnl[i].gop > 0){
				idx_last_gop = i;
				pAll_opt->venc.chnl[i].gop = pNew_opt->venc.chnl[i].gop;
			}
			else if(idx_last_gop >= 0){
				pAll_opt->venc.chnl[i].gop = pNew_opt->venc.chnl[idx_last_gop].gop;
			}
			else{
				pAll_opt->venc.chnl[i].gop = pAll_opt->venc.chnl[i].framerate * 5;
			}

			if(pNew_opt->venc.chnl[i].width_yuv > 0){
				idx_last_gop = i;
				pAll_opt->venc.chnl[i].width_yuv = pNew_opt->venc.chnl[i].width_yuv;
			}
			else if(idx_last_gop >= 0){
				pAll_opt->venc.chnl[i].width_yuv = pNew_opt->venc.chnl[idx_last_gop].width_yuv;
			}
			else{
				pAll_opt->venc.chnl[i].width_yuv = pAll_opt->vi.width_pipe;
			}

			if(pNew_opt->venc.chnl[i].height_yuv > 0){
				idx_last_gop = i;
				pAll_opt->venc.chnl[i].height_yuv = pNew_opt->venc.chnl[i].height_yuv;
			}
			else if(idx_last_gop >= 0){
				pAll_opt->venc.chnl[i].height_yuv = pNew_opt->venc.chnl[idx_last_gop].height_yuv;
			}
			else{
				pAll_opt->venc.chnl[i].height_yuv = pAll_opt->vi.height_pipe;
			}
		}
	}

	return TS_SUCCESS;
}

TS_S32 sample_comm_parse_args(int argc, char *argv[], int max_choise, pf_check_args pf_check, ALL_OPT_t *pAll_opt)
{
	TS_S32 ret = TS_SUCCESS;
	ALL_OPT_t all_opt = { 0 };
	sample_comm_string_tbl_t str_tbl;
	TS_S32 option_index = 0;
	TS_S32 i, c;
	static TS_S32 flag = 0;
	TS_S32 max_venc_chnl = 0;

	/*
    1. name			长选项名称（例如 "help"）
    2. has_arg		标志位，no_argument or required_argument 
    3. *flag		标志值，这里都设置为 0
    4. val			对应的短选项字符（例如 'h'）
	*/
	static struct option long_options[] = {
		{ "help", no_argument, 0, 'h' },
		{ "silence", no_argument, 0, 's' },

		{ "fastboot", no_argument, &flag, _SAMPLE_COMM_OPT_fastboot },
		{ "autoReset", no_argument, &flag, _SAMPLE_COMM_OPT_autoReset },
		{ "ispOnline", no_argument, &flag, _SAMPLE_COMM_OPT_ispOnline },
		{ "vpssOnline", no_argument, &flag, _SAMPLE_COMM_OPT_vpssOnline },
		{ "switchMixer", no_argument, &flag, _SAMPLE_COMM_OPT_switchMixer },

		{ "camNum", required_argument, &flag, _SAMPLE_COMM_OPT_camNum },
		{ "bitWidth", required_argument, &flag, _SAMPLE_COMM_OPT_bitWidth },
		{ "bayer", required_argument, &flag, _SAMPLE_COMM_OPT_bayer },
		{ "wdr", required_argument, &flag, _SAMPLE_COMM_OPT_wdr },
		{ "viFps", required_argument, &flag, _SAMPLE_COMM_OPT_viFps },
		{ "viSize", required_argument, &flag, _SAMPLE_COMM_OPT_viSize },
		{ "viUserpic", no_argument, &flag, _SAMPLE_COMM_OPT_viUserpic },
		{ "feedRaw", no_argument, &flag, _SAMPLE_COMM_OPT_feedRaw },

		{ "vpssSize", required_argument, &flag, _SAMPLE_COMM_OPT_vpssSize },
		{ "vpssGrpCrop", required_argument, &flag, _SAMPLE_COMM_OPT_vpssGrpCrop },
		{ "vpssChnCrop", required_argument, &flag, _SAMPLE_COMM_OPT_vpssChnCrop },
		{ "memCount", required_argument, &flag, _SAMPLE_COMM_OPT_memCount },

		{ "gop", required_argument, &flag, _SAMPLE_COMM_OPT_gop },
		{ "encFrate", required_argument, &flag, _SAMPLE_COMM_OPT_encFrate },
		{ "encBrate", required_argument, &flag, _SAMPLE_COMM_OPT_encBrate },
		{ "rcType", required_argument, &flag, _SAMPLE_COMM_OPT_rcType },
		{ "encType", required_argument, &flag, _SAMPLE_COMM_OPT_encType },
		{ "yuvSize", required_argument, &flag, _SAMPLE_COMM_OPT_yuvSize },
		{ "filePath", required_argument, &flag, _SAMPLE_COMM_OPT_filePath },
		{ "infilePath", required_argument, &flag, _SAMPLE_COMM_OPT_infilePath },
		{ "outfilePath", required_argument, &flag, _SAMPLE_COMM_OPT_outfilePath },

		{ "face", no_argument, &flag, _SAMPLE_COMM_OPT_face },
		{ "pcpp", no_argument, &flag, _SAMPLE_COMM_OPT_pcpp },
		{ "motion", no_argument, &flag, _SAMPLE_COMM_OPT_motion },

		{ 0, 0, 0, 0 }
	};

	#if 0
	all_opt.choise = 0;
	#else

	if (argc < 2) {
		return TS_FAILURE;
	}

	if (max_choise >= 0) {
		if (strlen(argv[1]) > 1 || !isdigit(argv[1][0])) {
			SAMPLE_PRT("invalid choise[%s] of %s\n", argv[1], argv[0]);
			return TS_FAILURE;
		}
		all_opt.choise = atoi(argv[1]);
		if(all_opt.choise > max_choise){
			SAMPLE_PRT("invalid choise[%d] of %s, maxValue is %d\n", all_opt.choise, argv[0], max_choise);
			return TS_FAILURE;
		}
		SAMPLE_PRT("choise is %d\n", all_opt.choise);
	}
	#endif

	while (1) {
		c = getopt_long(argc, argv, "hvs", long_options, &option_index);
		if (c < 0) {
			break;
		}

		if (0 == c) {
			switch (flag) {
			case _SAMPLE_COMM_OPT_fastboot:
				all_opt.bFastboot = TS_TRUE;
				break;
			case _SAMPLE_COMM_OPT_autoReset:
				all_opt.bAutoreset = TS_TRUE;
				break;
			case _SAMPLE_COMM_OPT_ispOnline:
				all_opt.vi.ispbyfly = TS_TRUE;
				break;
			case _SAMPLE_COMM_OPT_vpssOnline:
				all_opt.vivpss_bOnline = TS_TRUE;
				all_opt.bValid_VivpssOnlie = TS_TRUE;
				break;
			case _SAMPLE_COMM_OPT_switchMixer:
				all_opt.bSwtichMixer = TS_TRUE;
				break;
			case _SAMPLE_COMM_OPT_camNum:
				all_opt.camNum = atoi(optarg);
				SAMPLE_PRT("optarg=%s, camNum=%d\n", optarg, all_opt.camNum);
				break;
			case _SAMPLE_COMM_OPT_bitWidth:
				ret = sample_comm_args_getBitwidth(optarg, &all_opt.vi.bitwidth);
				if (TS_SUCCESS != ret) {
					SAMPLE_PRT("error bitwidth, %s\n", optarg);
					break;
				}
				all_opt.vi.bBitWidInit = TS_TRUE;
				break;
			case _SAMPLE_COMM_OPT_bayer:
				ret = sample_comm_args_getBayerPattern(optarg, &all_opt.vi.bayerformat);
				if (TS_SUCCESS != ret) {
					SAMPLE_PRT("error bayer pattern, %s\n", optarg);
					break;
				}
				all_opt.vi.bBayerFormatInit = TS_TRUE;
				break;
			case _SAMPLE_COMM_OPT_wdr:
				all_opt.vi.wdr = TS_TRUE;
				break;
			case _SAMPLE_COMM_OPT_viFps:
				if (all_opt.vi.fps > 0) {
					SAMPLE_PRT("double arg of viFps\n");
					ret = TS_FAILURE;
					break;
				}
				all_opt.vi.fps = atof(optarg);
				printf("====================fps=%f\n", all_opt.vi.fps);
				break;
			case _SAMPLE_COMM_OPT_viSize:
				if (all_opt.vi.width_pipe > 0 || all_opt.vi.height_pipe > 0) {
					SAMPLE_PRT("double arg of viSize\n");
					ret = TS_FAILURE;
					break;
				}
				sample_comm_args_getSize(optarg, &all_opt.vi.width_pipe, &all_opt.vi.height_pipe);
				break;
			case _SAMPLE_COMM_OPT_viUserpic:
				all_opt.vi.userpic = TS_TRUE;
				break;
			case _SAMPLE_COMM_OPT_feedRaw:
				all_opt.bFeedRaw = TS_TRUE;
				break;
			case _SAMPLE_COMM_OPT_vpssSize:
				if (all_opt.vpss.chnl_count > 0) {
					SAMPLE_PRT("double arg of vpssSize\n");
					ret = TS_FAILURE;
					break;
				}

				ret = sample_comm_args_getStringTbl(optarg, VPSS_MAX_CHN_NUM, &str_tbl);
				if (TS_SUCCESS != ret) {
					break;
				}

				TS_S32 biggest_width = 0;

				for (i = 0; i < str_tbl.valid_count; i++) {
					ret = sample_comm_args_getSize(str_tbl.string_tbl[i],
								       &all_opt.vpss.chnl[i].width_out,
								       &all_opt.vpss.chnl[i].height_out);
					if (TS_SUCCESS != ret) {
						SAMPLE_PRT("invalid vpss chnl size [%s]\n", str_tbl.string_tbl[i]);
						break;
					}
					if (all_opt.vpss.chnl[i].width_out > biggest_width) {
						biggest_width = all_opt.vpss.chnl[i].width_out;
						all_opt.vpss.biggest_index = i;
					}
				}

				all_opt.vpss.chnl_count = str_tbl.valid_count;
				break;
			case _SAMPLE_COMM_OPT_vpssGrpCrop:
				all_opt.bAutoreset = TS_TRUE;
				break;
			case _SAMPLE_COMM_OPT_vpssChnCrop:
				all_opt.bAutoreset = TS_TRUE;
				break;
			case _SAMPLE_COMM_OPT_memCount:
				ret = sample_comm_args_getStringTbl(optarg, VPSS_MAX_CHN_NUM, &str_tbl);
				if (TS_SUCCESS != ret) {
					break;
				}

				for (i = 0; i < str_tbl.valid_count; i++) {
					all_opt.vpss.chnl[i].memCount = atoi(str_tbl.string_tbl[i]);
					all_opt.vpss.chnl[i].bMemCountInit = TS_TRUE;
				}

				break;
			case _SAMPLE_COMM_OPT_gop:
				ret = sample_comm_args_getStringTbl(optarg, VENC_MAX_CHN_NUM, &str_tbl);
				if (TS_SUCCESS != ret) {
					break;
				}

				for (i = 0; i < str_tbl.valid_count; i ++) {
					all_opt.venc.chnl[i].gop = atoi(str_tbl.string_tbl[i]);
				}

				if(str_tbl.valid_count > max_venc_chnl)	max_venc_chnl = str_tbl.valid_count;
				break;
			case _SAMPLE_COMM_OPT_encFrate:
				ret = sample_comm_args_getStringTbl(optarg, VENC_MAX_CHN_NUM, &str_tbl);
				if (TS_SUCCESS != ret) {
					break;
				}

				for (i = 0; i < str_tbl.valid_count; i ++) {
					all_opt.venc.chnl[i].framerate = atof(str_tbl.string_tbl[i]);
				}
				if(str_tbl.valid_count > max_venc_chnl)	max_venc_chnl = str_tbl.valid_count;
				break;
			case _SAMPLE_COMM_OPT_encBrate:
				ret = sample_comm_args_getStringTbl(optarg, VENC_MAX_CHN_NUM, &str_tbl);
				if (TS_SUCCESS != ret) {
					break;
				}

				for (i = 0; i < str_tbl.valid_count; i ++) {
					all_opt.venc.chnl[i].bitrate = atoi(str_tbl.string_tbl[i]);
				}
				if(str_tbl.valid_count > max_venc_chnl)	max_venc_chnl = str_tbl.valid_count;
				break;
			case _SAMPLE_COMM_OPT_rcType:
				ret = sample_comm_args_getStringTbl(optarg, VENC_MAX_CHN_NUM, &str_tbl);
				if (TS_SUCCESS != ret) {
					break;
				}

				for (i = 0; i < str_tbl.valid_count; i ++) {
					ret = sample_comm_args_getRcType(str_tbl.string_tbl[i], &all_opt.venc.chnl[i].rc_type);
					if (TS_SUCCESS != ret) {
						SAMPLE_PRT("invalid venc rc type [%s]\n", str_tbl.string_tbl[i]);
						break;
					}
					all_opt.venc.chnl[i].bRcTypeValid = TS_TRUE;
				}
				if(str_tbl.valid_count > max_venc_chnl)	max_venc_chnl = str_tbl.valid_count;
				break;
			case _SAMPLE_COMM_OPT_encType:
				ret = sample_comm_args_getStringTbl(optarg, VENC_MAX_CHN_NUM, &str_tbl);
				if (TS_SUCCESS != ret) {
					break;
				}

				for (i = 0; i < str_tbl.valid_count; i ++) {
					ret = sample_comm_args_getPayloadType(str_tbl.string_tbl[i], &all_opt.venc.chnl[i].pt_type);
					if (TS_SUCCESS != ret) {
						SAMPLE_PRT("invalid venc payload type [%s]\n", str_tbl.string_tbl[i]);
						break;
					}
					all_opt.venc.chnl[i].bPtTypeValid = TS_TRUE;
				}
				if(str_tbl.valid_count > max_venc_chnl)	max_venc_chnl = str_tbl.valid_count;
				break;
			case _SAMPLE_COMM_OPT_yuvSize:
				sample_comm_args_getSize(optarg, &all_opt.venc.chnl[0].width_yuv, &all_opt.venc.chnl[0].height_yuv);
				break;
			case _SAMPLE_COMM_OPT_filePath:
				strcpy(all_opt.filePath, optarg);
				all_opt.bFilePathValid = TS_TRUE;
				break;
			case _SAMPLE_COMM_OPT_infilePath:
				strcpy(all_opt.infilePath, optarg);
				all_opt.bInfileValid = TS_TRUE;
				break;
			case _SAMPLE_COMM_OPT_outfilePath:
				strcpy(all_opt.outfilePath, optarg);
				all_opt.bOutfileValid = TS_TRUE;
				break;
			default:
				SAMPLE_PRT("unsupport flag[%d]\n", flag);
				break;
			}
		} else {
			switch (c) {
			case 'h':
				sample_comm_args_help();
				exit(0);
				break;
			case 's':
				all_opt.bSilence = TS_TRUE;
				break;
			default:
				SAMPLE_PRT("unsupport arg [%c]\n", c);
				break;
			}
		}
	}

	do {
		if (TS_SUCCESS != ret) {
			SAMPLE_PRT("error occur\n");
			break;
		}

		if (pf_check) {
			ret = pf_check(&all_opt);
			if (TS_SUCCESS != ret) {
				SAMPLE_PRT("pf_check faild\n");
				break;
			}
		}

		SAMPLE_PRT("camNum=%d\n", all_opt.camNum);
		ret = sample_comm_args_commCheck(pAll_opt, &all_opt, max_venc_chnl);
		if (TS_SUCCESS != ret) {
			SAMPLE_PRT("sample_comm_args_commCheck faild\n");
			break;
		}

		// memcpy(pAll_opt, &all_opt, sizeof(all_opt));

		printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		printf("choise=%d, camNum=%d, vpssOnline=%d\n", pAll_opt->choise, pAll_opt->camNum,
		       pAll_opt->vivpss_bOnline);
		printf("vi: wh=[%d,%d], ispByFly=%d, wdr=%d, userpic=%d\n", pAll_opt->vi.width_pipe, pAll_opt->vi.height_pipe,
		       pAll_opt->vi.ispbyfly, pAll_opt->vi.wdr, pAll_opt->vi.userpic);
		printf("vpss: chnlNum=%d, wh0=[%d,%d], wh1=[%d,%d]\n", pAll_opt->vpss.chnl_count,
		       pAll_opt->vpss.chnl[0].width_out, pAll_opt->vpss.chnl[0].height_out,
		       pAll_opt->vpss.chnl[1].width_out, pAll_opt->vpss.chnl[1].height_out);
		printf("venChnl-0: rc=%d, pt=%d, fps=%d, bitrate=%d, gop=%d\n",
				pAll_opt->venc.chnl[0].rc_type, pAll_opt->venc.chnl[0].pt_type,
				pAll_opt->venc.chnl[0].framerate, pAll_opt->venc.chnl[0].bitrate, pAll_opt->venc.chnl[0].gop);
		printf("venChnl-1: rc=%d, pt=%d, fps=%d, bitrate=%d, gop=%d\n",
				pAll_opt->venc.chnl[1].rc_type, pAll_opt->venc.chnl[1].pt_type,
				pAll_opt->venc.chnl[1].framerate, pAll_opt->venc.chnl[1].bitrate, pAll_opt->venc.chnl[1].gop);
		printf("infilePath:%s,outfilePath:%s\n",
				pAll_opt->infilePath,pAll_opt->outfilePath);
		printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	} while (0);

	return ret;
}
