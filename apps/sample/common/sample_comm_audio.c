
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>
#include <linux/gpio.h>
#include <libavformat/avformat.h>
#include <libavutil/log.h>
#include "RtspServerApp.h"
#include "sample_comm_audio.h"
#include "sample_comm_util.h"

#if 1
#include "ts_alg_opus.h"
#include "ts_alg_aac_enc.h"
#include "ts_alg_aac_dec.h"
#include "ts_alg_g726.h"
#include "ts_alg_adpcm.h"
#include "ts_alg_g711.h"
#else
#ifdef OPUS_CODEC
#include "ts_alg_opus.h"
#endif
#ifdef AAC_CODEC
#include "ts_alg_aac_enc.h"
#include "ts_alg_aac_dec.h"
#endif
#ifdef G726_CODEC
#include "ts_alg_g726.h"
#endif
#ifdef ADPCM_CODEC
#include "ts_alg_adpcm.h"
#endif
#ifdef G711_CODEC
#include "ts_alg_g711.h"
#endif
#endif
#if defined VQE_NN || defined VQE_WEBRTC_NN
#include "trp.h"
#endif

#define PRINT(fmt, ...) printf("%s:%d " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

typedef struct tagSAMPLE_AENC_S
{
	TS_BOOL bStart;
	pthread_t stAencPid;
	TS_S32  AeChn;
	TS_S32  AdChn;
	FILE*    pfd;
	TS_BOOL bSendAdChn;
	TS_BOOL bSendStream;
} SAMPLE_AENC_S;

typedef struct tagSAMPLE_AI_S
{
	TS_BOOL bStart;
	TS_S32  AiDev;
	TS_S32  AiChn;
	TS_S32  AencChn;
	TS_S32  AoDev;
	TS_S32  AoChn;
	TS_BOOL bSendAenc;
	TS_BOOL bSendAo;
	TS_BOOL bSendStream;
	pthread_t stAiPid;
} SAMPLE_AI_S;

typedef struct tagSAMPLE_ADEC_S
{
	TS_BOOL bStart;
	TS_S32 AdChn;
	FILE* pfd;
	pthread_t stAdPid;
	TS_BOOL bPullStream;
} SAMPLE_ADEC_S;

typedef struct tagSAMPLE_AO_S
{
	TS_BOOL bStart;
	TS_S32  AoDev;
	TS_S32  AoChn;
	TS_S32  type;
	pthread_t stAoPid;
	TS_BOOL bPullStream;
} SAMPLE_AO_S;

extern PAYLOAD_TYPE_E gs_enPayloadType;
extern AUDIO_SAMPLE_RATE_E g_enSampleRate;
TS_BOOL g_raw_enable = TS_FALSE;
TS_BOOL g_ref_enable = TS_FALSE;
TS_BOOL g_stamp_enable = TS_FALSE;
static TS_S8 g_stamp_count = 0;

static SAMPLE_AI_S   gs_stSampleAi[AI_DEV_MAX_NUM* AI_MAX_CHN_NUM];
static SAMPLE_AO_S   gs_stSampleAo[AO_DEV_MAX_NUM* AO_MAX_CHN_NUM];
static SAMPLE_AENC_S gs_stSampleAenc[AENC_MAX_CHN_NUM];
static SAMPLE_AENC_S gs_stSampleSendAenc[AENC_MAX_CHN_NUM];
static SAMPLE_ADEC_S gs_stSampleAdec[ADEC_MAX_CHN_NUM];
static SAMPLE_ADEC_S gs_stSampleGetAdec[ADEC_MAX_CHN_NUM];

#if 1
static ts_char *ai_cap_store = "./aiao_test.pcm";
//static ts_char *ai_cap_system_op = "rm -rf ./aiao_test.pcm";
static ts_char *ai_raw_cap_store = "./aiao_raw_test.pcm";
static ts_char *ai_ref_cap_store = "./aiao_ref_test.pcm";

static ts_u32 pcm_total_size;

static FILE *file;
static ts_u32 bytes_per_frame;
static int test_flag = 1;

static ts_u32 pcm_raw_total_size;
static FILE *file_raw;
static ts_u32 bytes_per_frame_raw;
static int test_raw_flag = 1;

static ts_u32 pcm_ref_total_size;
static FILE *file_ref;
static ts_u32 bytes_per_frame_ref;
static int test_ref_flag = 1;

static AVFormatContext *g_fmt_ctx = NULL;
static AVPacket *g_pkt = NULL;
static TS_S32 g_audio_stream_index = -1;
char g_ip[20] = "192.168.1.20";
static char rtsp_url[100];

char* getPullStreamUrl() {

    sprintf(rtsp_url, "rtsp://%s:8554/live", g_ip);

    return rtsp_url;
}

TS_S32 openAudioPullStream(const char *url)
{
	if (!url) {
		SAMPLE_PRT("url is null !\n");
	}

	if (g_fmt_ctx)
		return TS_SUCCESS;

	av_log_set_level(AV_LOG_DEBUG);
	avformat_network_init();

	AVDictionary *options = NULL;

	av_dict_set(&options, "timeout", "5000000", 0); // 5 second timeout
	av_dict_set(&options, "buffer_size", "458752", 0); // buffer
	av_dict_set(&options, "rtsp_transport", "tcp", 0);

	// open rtsp stream
	if (avformat_open_input(&g_fmt_ctx, url, NULL, &options) != 0) {
		SAMPLE_PRT("Failed to open RTSP stream with options.\n");
		av_dict_free(&options);
		return -1;
	}
	av_dict_free(&options);

	av_dump_format(g_fmt_ctx, 0, url, 0);

	if (avformat_find_stream_info(g_fmt_ctx, NULL) < 0) {
		SAMPLE_PRT("Failed to find stream info.\n");
		avformat_close_input(&g_fmt_ctx);
		return -1;
	}

	for (TS_U32 i = 0; i < g_fmt_ctx->nb_streams; i++) {
		AVStream *stream = g_fmt_ctx->streams[i];
		SAMPLE_PRT("Stream[%d]: codec_type=%d\n", i, stream->codecpar->codec_type);
	}


	AVCodecParameters *audio_codec_params = NULL;
	for (TS_U32 i = 0; i < g_fmt_ctx->nb_streams; i++) {
		if (g_fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
			g_audio_stream_index = i;
			audio_codec_params = g_fmt_ctx->streams[i]->codecpar;
			break;
		}
	}

	if (g_audio_stream_index == -1 || audio_codec_params == NULL) {
		SAMPLE_PRT("No audio stream found.\n");
		avformat_close_input(&g_fmt_ctx);
		return -1;
	}

	SAMPLE_PRT("Audio Stream Information:\n");
	SAMPLE_PRT("  Codec: %s\n", avcodec_get_name(audio_codec_params->codec_id));
	SAMPLE_PRT("  Sample Rate: %d Hz\n", audio_codec_params->sample_rate);
	SAMPLE_PRT("  Bit Rate: %lld bps\n", (long long)audio_codec_params->bit_rate);
	SAMPLE_PRT("  Sample Format: %d (AVSampleFormat)\n", audio_codec_params->format);

	g_pkt = av_packet_alloc();
	if (!g_pkt) {
        SAMPLE_PRT("g_pkt alloc failed\n");
        avformat_close_input(&g_fmt_ctx);
        return -1;
	}
	SAMPLE_PRT("openAudioPullStream finish\n");
	return TS_SUCCESS;
}

TS_VOID closeAudioPullStream()
{
	SAMPLE_PRT("closeAudioPullStream start\n");
	if (g_pkt)
		av_packet_free(&g_pkt);
	if (g_fmt_ctx)
		avformat_close_input(&g_fmt_ctx);
	avformat_network_deinit();
	g_fmt_ctx = NULL;
	g_audio_stream_index = -1;
	g_pkt = NULL;
	SAMPLE_PRT("closeAudioPullStream finish\n");
}

TS_S32 getAudioPullStream(TS_U8 *pData, TS_U32 *pSize)
{
	TS_S32 s32Ret;

	if (!pData || !pSize) {
		SAMPLE_PRT("getAudioPullStream para is null\n");
		return TS_FAILURE;
	}

	if (!g_fmt_ctx) {
		SAMPLE_PRT("getAudioPullStream g_fmt_ctx is null\n");
		return TS_FAILURE;
	}

	if (!g_pkt) {
		SAMPLE_PRT("getAudioPullStream g_pkt is null\n");
		return TS_FAILURE;
	}
	s32Ret =  av_read_frame(g_fmt_ctx, g_pkt);
	if (s32Ret >= 0) {
		//SAMPLE_PRT("Packet: stream_index=%d, size=%d, pts=%ld\n", g_pkt->stream_index, g_pkt->size, g_pkt->pts);
		if (g_pkt->stream_index == g_audio_stream_index) {
				memcpy(pData, g_pkt->data, g_pkt->size);
				*pSize = g_pkt->size;
#if 0
				static FILE* pfd_in;
				if (pfd_in == NULL)
					pfd_in = fopen("save_aac_stream.aac", "w+");
				fwrite(g_pkt->data, 1, g_pkt->size, pfd_in);
#endif
		} else {
			SAMPLE_PRT("pkt idx=%d, audio idx=%d, not match\n", g_pkt->stream_index, g_audio_stream_index);
			*pSize = 0;
		}
		av_packet_unref(g_pkt);
	} else {
		TS_CHAR errbuf[128];
		av_strerror(s32Ret, errbuf, sizeof(errbuf));
		SAMPLE_PRT("Error reading frame: %s\n", errbuf);
		return TS_FAILURE;
	}
	return TS_SUCCESS;
}

static TS_S32 openPushStreamAudioServer(RTSP_STREAM_TYPE_E rtspType, TS_U32 sampleRate, TS_U32 channels)
{

	SAMPLE_PRT("openPushStreamAudioServer start!!! type=%d, sample_rate=%d\n", rtspType, sampleRate);

	if (gb_rtsp_audio_server_start(rtspType, sampleRate, channels)) {
		SAMPLE_PRT("gb_rtsp_audio_server_start failed !!!\n");
		return TS_FAILURE;
	}
	SAMPLE_PRT("openPushStreamAudioServer finish!!!\n");
	return TS_SUCCESS;
}

static TS_S32 closePushStreamAudioServer()
{
	SAMPLE_PRT("closePushStreamAudioServer start!!!\n");

	if (gb_rtsp_audio_server_stop()) {
		SAMPLE_PRT("gb_rtsp_audio_server_stop failed !!!\n");
		return TS_FAILURE;
	}
	SAMPLE_PRT("closePushStreamAudioServer finish!!!\n");
	return TS_SUCCESS;
}

static TS_S32 pushAudioStream(TS_U8 *pData, TS_U32 length)
{
	if (!pData || 0 == length) {
		SAMPLE_PRT(" %s %d Param Is Null !.. \n", __func__, __LINE__);
		return TS_FAILURE;
	}

	if (sendToRtspAudioFrame(pData, length)) {
		SAMPLE_PRT("sendToRtspAudioFrame failed !!!\n");
		return TS_FAILURE;
	}
	return TS_SUCCESS;
}

static TS_U64 getSysTimeUs(TS_VOID)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (TS_U64)tv.tv_sec * 1e6 + tv.tv_usec;
}

static TS_U64 getBootTimeUs(TS_VOID)
{
#if defined(CLOCK_MONOTONIC)
	{
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC_COARSE, &ts);
		return (TS_U64)ts.tv_sec * 1e6 + ts.tv_nsec/1000;
	}
#endif
	return 0;
}

static TS_VOID aencShowStamp(AUDIO_STREAM_S *pstAudioStream) {
    if(!g_stamp_enable || !pstAudioStream || g_stamp_count > 10)
        return;

    SAMPLE_PRT("stream: PTS: %llu BOOTUP_TIME: %llu SYS_TIME: %llu\n",
            pstAudioStream->u64TimeStamp,
            getBootTimeUs(),
            getSysTimeUs());

    g_stamp_count++;
}

static ts_bool is_pcm_file_open(void)
{
	if (file)
		return TS_TRUE;
	return TS_FALSE;
}

static ts_void create_pcm_file(ts_char *file_path, ts_u8 bit_width, ts_u8 channel_num, ts_u32 sample_rate)
{
	file = fopen(file_path, "wb");
	bytes_per_frame = channel_num * bit_width / 8;
	printf("%s channels=%d, rate=%d, bit_width=%d, bytes_per_frame=%d\n", __func__, channel_num, sample_rate,
	       bit_width, bytes_per_frame);
}

static ts_void close_pcm_file(void)
{
	if (!file)
		return;

	fclose(file);
	file = NULL;
}

static ts_void write_pcm_file(ts_char *buffer, ts_u32 size)
{
	//printf("%s size=%d, bytes_per_frame=%d, frames=%d\n", __func__, size, bytes_per_frame,
	//       (ts_u32)(size / bytes_per_frame));
	//fwrite(buffer, bytes_per_frame, size / bytes_per_frame, file);
	fwrite(buffer, 1, size, file);
}

static void test_save_pcm(AUDIO_FRAME_S *pFrame)
{
	//ts_s32 ret = 0;

	if (0 == test_flag)
		return;

	//printf("%s: port%d, buffer=%p\n", __func__, pBuffer->nOutputPortIndex, pBuffer->pBuffer);
	//printf("%s: port%d, buffer=%p, appprivate=%p,%d\n", __func__, pBuffer->nOutputPortIndex, pBuffer->pBuffer, pBuffer->pAppPrivate, *(int *)pBuffer->pAppPrivate);
	AIO_ATTR_S aioAttr		= {0};
	aioAttr.u8CardNum 		= 0;
	aioAttr.u8DeviceNum 	= 0;
	aioAttr.enSamplerate 	= AUDIO_SAMPLE_RATE_16000;
	aioAttr.enBitwidth 		= AUDIO_BIT_WIDTH_16;
	aioAttr.enSoundmode		= AUDIO_SOUND_MODE_MONO;
	aioAttr.u32FrmNum		= 4;
	aioAttr.u32PtNumPerFrm 	= 160;

	//printf("%s:u32Len = %d, pFrame->u64VirAddr[0]= %p", __func__, pFrame->u32Len, pFrame->u64VirAddr[0]);
	if (TS_FALSE == is_pcm_file_open())
		create_pcm_file(ai_cap_store, aioAttr.enBitwidth, (aioAttr.enSoundmode + 1),
			aioAttr.enSamplerate);
	if (pFrame && pFrame->u32Len) {
		write_pcm_file(pFrame->u64VirAddr[0], pFrame->u32Len);
		pcm_total_size += pFrame->u32Len;
	}
	else
	{
		PRINT("length is error:[%d]\n", pFrame->u32Len);
	}

	if (pcm_total_size > 1 * 512 * 1024) {
		test_flag = 0;
		close_pcm_file();
	}

	return ;
}
ts_bool is_raw_pcm_file_open(void)
{
	if (file_raw)
		return TS_TRUE;
	return TS_FALSE;
}

ts_void create_raw_pcm_file(ts_char *file_path, ts_u8 bit_width, ts_u8 channel_num, ts_u32 sample_rate)
{
	file_raw = fopen(file_path, "wb");
	bytes_per_frame_raw = channel_num * bit_width / 8;
	printf("%s channels=%d, rate=%d, bit_width=%d, bytes_per_frame_raw=%d\n", __func__, channel_num, sample_rate,
	       bit_width, bytes_per_frame_raw);
}

ts_void close_raw_pcm_file(void)
{
	if (!file_raw)
		return;

	fclose(file_raw);
	file_raw = NULL;
}

ts_void write_raw_pcm_file(ts_char *buffer, ts_u32 size)
{
	//printf("%s size=%d, bytes_per_frame=%d, frames=%d\n", __func__, size, bytes_per_frame,
	//       (ts_u32)(size / bytes_per_frame));
	//fwrite(buffer, bytes_per_frame, size / bytes_per_frame, file);
	fwrite(buffer, 1, size, file_raw);
}


static void test_save_raw_pcm(AUDIO_FRAME_S *pFrame)
{
	if (0 == test_raw_flag)
		return;

	//printf("%s: port%d, buffer=%p\n", __func__, pBuffer->nOutputPortIndex, pBuffer->pBuffer);
	//printf("%s: port%d, buffer=%p, appprivate=%p,%d\n", __func__, pBuffer->nOutputPortIndex, pBuffer->pBuffer, pBuffer->pAppPrivate, *(int *)pBuffer->pAppPrivate);
	AIO_ATTR_S aioAttr		= {0};
	aioAttr.u8CardNum		= 0;
	aioAttr.u8DeviceNum 	= 0;
	aioAttr.enSamplerate	= AUDIO_SAMPLE_RATE_16000;
	aioAttr.enBitwidth		= AUDIO_BIT_WIDTH_16;
	aioAttr.enSoundmode 	= AUDIO_SOUND_MODE_MONO;
	aioAttr.u32FrmNum		= 4;
	aioAttr.u32PtNumPerFrm	= 320;

	//printf("%s:u32Len = %d, pFrame->u64VirAddr[0]= %p", __func__, pFrame->u32Len, pFrame->u64VirAddr[0]);
	if (TS_FALSE == is_raw_pcm_file_open())
		create_raw_pcm_file(ai_raw_cap_store, aioAttr.enBitwidth, (aioAttr.enSoundmode + 1),
			aioAttr.enSamplerate);
	if (pFrame && pFrame->u32Len) {
		write_raw_pcm_file(pFrame->u64VirAddr[0], pFrame->u32Len);
		pcm_raw_total_size += pFrame->u32Len;
	}
	else
	{
		PRINT("length is error:[%d]\n", pFrame->u32Len);
	}

	if (pcm_raw_total_size > 1 * 512 * 1024) {
		test_raw_flag = 0;
		close_raw_pcm_file();
	}

	return;
}

ts_bool is_ref_pcm_file_open(void)
{
	if (file_ref)
		return TS_TRUE;
	return TS_FALSE;
}

ts_void create_ref_pcm_file(ts_char *file_path, ts_u8 bit_width, ts_u8 channel_num, ts_u32 sample_rate)
{
	file_ref = fopen(file_path, "wb");
	bytes_per_frame_ref = channel_num * bit_width / 8;
	printf("%s channels=%d, rate=%d, bit_width=%d, bytes_per_frame_raw=%d\n", __func__, channel_num, sample_rate,
	       bit_width, bytes_per_frame_raw);
}

ts_void close_ref_pcm_file(void)
{
	if (!file_ref)
		return;

	fclose(file_ref);
	file_ref = NULL;
}

ts_void write_ref_pcm_file(ts_char *buffer, ts_u32 size)
{
	//printf("%s size=%d, bytes_per_frame=%d, frames=%d\n", __func__, size, bytes_per_frame,
	//       (ts_u32)(size / bytes_per_frame));
	//fwrite(buffer, bytes_per_frame, size / bytes_per_frame, file);
	fwrite(buffer, 1, size, file_ref);
}


static void test_save_ref_pcm(AUDIO_FRAME_S *pFrame)
{
	if (0 == test_ref_flag)
		return;

	//pr_debug("%s: port%d, buffer=%p\n", __func__, pBuffer->nOutputPortIndex, pBuffer->pBuffer);
	//printf("%s: port%d, buffer=%p, appprivate=%p,%d\n", __func__, pBuffer->nOutputPortIndex, pBuffer->pBuffer, pBuffer->pAppPrivate, *(int *)pBuffer->pAppPrivate);
	AIO_ATTR_S aioAttr		= {0};
	aioAttr.u8CardNum		= 0;
	aioAttr.u8DeviceNum 	= 0;
	aioAttr.enSamplerate	= AUDIO_SAMPLE_RATE_16000;
	aioAttr.enBitwidth		= AUDIO_BIT_WIDTH_16;
	aioAttr.enSoundmode 	= AUDIO_SOUND_MODE_MONO;
	aioAttr.u32FrmNum		= 4;
	aioAttr.u32PtNumPerFrm	= 320;

	//printf("%s:u32Len = %d, pFrame->u64VirAddr[0]= %p", __func__, pFrame->u32Len, pFrame->u64VirAddr[0]);
	if (TS_FALSE == is_ref_pcm_file_open())
		create_ref_pcm_file(ai_ref_cap_store, aioAttr.enBitwidth, (aioAttr.enSoundmode + 1),
			aioAttr.enSamplerate);
	if (pFrame && pFrame->u32Len) {
		write_ref_pcm_file(pFrame->u64VirAddr[0], pFrame->u32Len);
		pcm_ref_total_size += pFrame->u32Len;
	}
	else
	{
		PRINT("length is error:[%d]\n", pFrame->u32Len);
	}

	if (pcm_ref_total_size > 1 * 512 * 1024) {
		test_ref_flag = 0;
		close_ref_pcm_file();
	}

	return;
}

#endif
#if 1
#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT 0x20746d66
#define ID_DATA 0x61746164

struct wav_header {
	ts_u32 riff_id;
	ts_u32 riff_sz;
	ts_u32 riff_fmt;
	ts_u32 fmt_id;
	ts_u32 fmt_sz;
	ts_u16 audio_format;
	ts_u16 num_channels;
	ts_u32 sample_rate;
	ts_u32 byte_rate;
	ts_u16 block_align;
	ts_u16 bits_per_sample;
	ts_u32 data_id;
	ts_u32 data_sz;
};

struct riff_wave_header {
	ts_u32 riff_id;
	ts_u32 riff_sz;
	ts_u32 wave_id;
};

struct chunk_header {
	ts_u32 id;
	ts_u32 sz;
};

struct chunk_fmt {
	ts_u16 audio_format;
	ts_u16 num_channels;
	ts_u32 sample_rate;
	ts_u32 byte_rate;
	ts_u16 block_align;
	ts_u16 bits_per_sample;
};

static ts_u32 parse_wave_file(FILE *file)
{
	struct riff_wave_header wave_header;
	struct chunk_header chunk_header;
	struct chunk_fmt chunk_fmt;

	if (fread(&wave_header, sizeof(wave_header), 1, file) != 1) {
		printf("error: does not contain a riff/wave header\n");
		return -1;
	}

	if (wave_header.riff_id != ID_RIFF || wave_header.wave_id != ID_WAVE) {
		printf("error: is not a riff/wave file\n");
		return -1;
	}

	ts_bool more_chunks = TS_TRUE;
	do {
		if (fread(&chunk_header, sizeof(chunk_header), 1, file) != 1) {
			printf("error: does not contain a data chunk\n");
			return -1;
		}
		switch (chunk_header.id) {
		case ID_FMT:
			if (fread(&chunk_fmt, sizeof(chunk_fmt), 1, file) != 1) {
				printf("error: has incomplete format chunk\n");
				return -1;
			}
			/* If the format header is larger, skip the rest */
			if (chunk_header.sz > sizeof(chunk_fmt)) {
				fseek(file, chunk_header.sz - sizeof(chunk_fmt), SEEK_CUR);
			}
			break;
		case ID_DATA:
			/* Stop looking for chunks */
			more_chunks = TS_FALSE;
			break;
		default:
			/* Unknown chunk, skip bytes */
			fseek(file, chunk_header.sz, SEEK_CUR);
		}
	} while (more_chunks);

	return chunk_header.sz;
}

#endif

enum {
	GPIOA0 = 0,
	GPIOA1,
	GPIOA2,
	GPIOA3,
	GPIOA4,
	GPIOA5,
	GPIOA6,
	GPIOA7,
	GPIOA8,
	GPIOA9,
	GPIOA10,
	GPIOA11,
	GPIOA12,
	GPIOA13,
	GPIOA14,
	GPIOA15,
	GPIOA16,
	GPIOA17,
	GPIOA18,
	GPIOA19,
	GPIOA20,
	GPIOA21,
	GPIOA22,
	GPIOA23,
	GPIOA24,
	GPIOA25,
	GPIOA26,
	GPIOA27,
	GPIOA28,
	GPIOA29,
	GPIOA30,
	GPIOA31,
};

enum {
	GPIOB0 = 0,
	GPIOB1,
	GPIOB2,
	GPIOB3,
	GPIOB4,
	GPIOB5,
	GPIOB6,
	GPIOB7,
	GPIOB8,
	GPIOB9,
	GPIOB10,
	GPIOB11,
	GPIOB12,
	GPIOB13,
	GPIOB14,
	GPIOB15,
	GPIOB16,
	GPIOB17,
	GPIOB18,
	GPIOB19,
	GPIOB20,
	GPIOB21,
	GPIOB22,
	GPIOB23,
	GPIOB24,
	GPIOB25,
	GPIOB26,
	GPIOB27,
	GPIOB28,
	GPIOB29,
	GPIOB30,
	GPIOB31,
};

#define GPIOA_DEV "/dev/gpiochip0"
#define GPIOB_DEV "/dev/gpiochip1"


int ts_gpio_get_handle(const char *dev_name, int gpio_num)
{
	struct gpiohandle_request req;
	struct gpiohandle_data data;
	int fd, ret;

	memset(&data, 0, sizeof(struct gpiohandle_data));

	fd = open(dev_name, 0);
	if(fd < 0) {
		printf("failed to open %s\n", dev_name);
		return -1;
	}

	/* GPIOA11 */
	req.lineoffsets[0] = gpio_num;
	req.flags = GPIOHANDLE_REQUEST_OUTPUT;
	strcpy(req.consumer_label, "speaker-gpio");
	req.lines = 1;
	memcpy(req.default_values, &data, sizeof(req.default_values));

	ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
	if (ret < 0) {
		printf("Failed to issue GET LINEHANDLE IOCTL (%d)\n", ret);
	}

	close(fd);

	return ret < 0 ? ret : req.fd;
}

int ts_gpio_release_handle(const int fd)
{
	int ret;

	ret = close(fd);
	if (ret < 0)
		printf("%s failed\n", __func__);

	return ret;
}

int ts_gpio_set_value(const int fd, struct gpiohandle_data *data)
{
	int ret;

	ret = ioctl(fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, data);
	if (ret < 0)
		printf("GPIOHANDLE_SET_LINE_VALUES_IOCTL failed (%d)\n", ret);

	return ret;
}

int ts_gpio_get_value(const int fd, struct gpiohandle_data *data)
{
	int ret;

	ret = ioctl(fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, data);
	if (ret < 0)
		printf("GPIOHANDLE_GET_LINE_VALUES_IOCTL (%d)\n", ret);

	return ret;
}
#ifdef VQE_WEBRTC
ts_s32 ai_vqe_open(ts_void **handle, ts_void *attr)
{
	ts_s32 ret = TS_SUCCESS;
#ifdef VQE_WEBRTC_NN
	if (0 != TS_MPI_TRP_RNE_MutexInit()) {
		printf("ai vqe trp rne init failed !\n");
	}

	if (0 != TS_MPI_TRP_RNE_MutexLockWithTimeout(1, 5000)) {
		printf("ai trp rne lock timeout failed !\n");
		return TS_FAILURE;
	}
#endif /* VQE_WEBRTC_NN */
	TS_S32 err = TS_ALG_VQESolWebRTC3A_Init(handle, (ALG_VQE_SOL_WEBRTC3A_CONF_PARAM *)attr);
	if ((!*handle) || (err != 0)){
		printf("Error, Initializtion Fail.\n");
		ret = TS_FAILURE;
	}else{
		printf("Initializtion ok.\n");
		ret = TS_SUCCESS;
	}

#ifdef VQE_WEBRTC_NN
	if (0 != TS_MPI_TRP_RNE_MutexUnlock()) {
		printf("ai trp rne unlock failed !\n");
	}
#endif /* VQE_WEBRTC_NN */
	return ret;
}

ts_s32	ai_vqe_set_proc_addr(ts_void *handle, ts_void *addr)
{

	printf("zzz ai_vqe_set_proc_addr\n");
	return TS_SUCCESS;
}

ts_s32	ai_vqe_process(ts_void *handle, ts_u8 *in_buf, ts_u8 *ref_buf, ts_u32 *in_len, ts_u8 *out_buf, ts_u32 *out_len)
{
	TS_S32 s32Ret;
	TS_U32 u32OutLen, u32InLen;
	TS_U32 u32ShortBytes;

#if 0
	static FILE* pfd_in;
	if (pfd_in == NULL)
		pfd_in = fopen("ai_vqe_file_in.pcm", "w+");
	fwrite(in_buf, 1, *in_len, pfd_in);
#endif
#if 0
	static FILE* pfd_ref;
	if (pfd_ref == NULL)
		pfd_ref = fopen("ai_vqe_file_ref.pcm", "w+");
	fwrite(ref_buf, 1, *in_len, pfd_ref);
#endif
	u32ShortBytes = sizeof(short);
	u32InLen = *in_len/u32ShortBytes;
#ifdef VQE_WEBRTC_NN
	if (0 != TS_MPI_TRP_RNE_MutexLockWithTimeout(1, 5000)) {
		printf("ai trp rne lock timeout failed !\n");
		return TS_FAILURE;
	}
#endif /* VQE_WEBRTC_NN */
	s32Ret = TS_ALG_VQESolWebRTC3A_Process(handle, (TS_S16 *)in_buf, (TS_S16 *)ref_buf, &u32InLen, (TS_S16 *)out_buf, &u32OutLen);
	if (s32Ret) {
		printf("ai_vqe_process failed ret=%d\n", s32Ret);
		s32Ret = TS_FAILURE;
	} else {
		*out_len = u32OutLen * u32ShortBytes;
		s32Ret = TS_SUCCESS;
	}

#ifdef VQE_WEBRTC_NN
	if (0 != TS_MPI_TRP_RNE_MutexUnlock()) {
		printf("ai trp rne unlock failed !\n");
	}
#endif /* VQE_WEBRTC_NN */
#if 0
	if (TS_SUCCESS == s32Ret){
		static FILE* pfd_out;
		if (pfd_out == NULL)
			pfd_out = fopen("ai_vqe_file_out.pcm", "w+");
		fwrite(out_buf, 1, *out_len, pfd_out);
	}
#endif
	return s32Ret;
}

ts_s32	ai_vqe_close(ts_void *handle)
{
	TS_S32 s32Ret;
#ifdef VQE_WEBRTC_NN
	if (0 != TS_MPI_TRP_RNE_MutexLockWithTimeout(1, 5000)) {
		printf("ai trp rne lock timeout failed !\n");
		return TS_FAILURE;
	}
#endif /* VQE_WEBRTC_NN */
	s32Ret = TS_ALG_VQESolWebRTC3A_Exit(handle);
	if (s32Ret) {
		printf("ai_vqe_close failed ret=%d\n", s32Ret);
		s32Ret = TS_FAILURE;
	} else {
		s32Ret = TS_SUCCESS;
	}
#ifdef VQE_WEBRTC_NN
	if (0 != TS_MPI_TRP_RNE_MutexUnlock()) {
		printf("ai trp rne unlock failed !\n");
	}

	if (0 != TS_MPI_TRP_RNE_MutexDeInit()) {
		printf("ai vqe trp ren deinit failed !\n");
	}
#endif /* VQE_WEBRTC_NN */
	return s32Ret;
}

ts_s32	ao_vqe_open(ts_void **handle, ts_void *attr)
{
	TS_S32 err = TS_ALG_VQESolWebRTC3A_Init(handle, (ALG_VQE_SOL_WEBRTC3A_CONF_PARAM *)attr);
	if ((!*handle) || (err != 0)){
		printf("Error, Initializtion Fail.\n");
		return -1;
	}else{
		printf("Initializtion ok.\n");
	}
	return TS_SUCCESS;
}

ts_s32	ao_vqe_set_proc_addr(ts_void *handle, ts_void *addr)
{

	printf("zzz ao_vqe_set_proc_addr\n");
	return TS_SUCCESS;
}

ts_s32	ao_vqe_process(ts_void *handle, ts_u8 *in_buf, ts_u8 *ref_buf, ts_u32 *in_len, ts_u8 *out_buf, ts_u32 *out_len)
{

	TS_S32 s32Ret;
	TS_U32 u32OutLen, u32InLen;
	TS_U32 u32ShortBytes;
#if 0
	static FILE* pfd_aoin;
	if (pfd_aoin == NULL)
		pfd_aoin = fopen("ao_vqe_file_in.pcm", "w+");
	fwrite(in_buf, 1, *in_len, pfd_aoin);
#endif

	u32ShortBytes = sizeof(short);
	u32InLen = *in_len/u32ShortBytes;
	s32Ret = TS_ALG_VQESolWebRTC3A_Process(handle, (TS_S16 *)in_buf, (TS_S16 *)ref_buf, &u32InLen, (TS_S16 *)out_buf, &u32OutLen);
	if (s32Ret) {
		printf("ao_vqe_process failed ret=%d\n", s32Ret);
		return s32Ret;
	}
	*out_len = u32OutLen * u32ShortBytes;
#if 0
	static FILE* pfd_aoout;
	if (pfd_aoout == NULL)
		pfd_aoout = fopen("ao_vqe_file_out.pcm", "w+");
	fwrite(out_buf, 1, *in_len, pfd_aoout);
#endif
	return TS_SUCCESS;
}

ts_s32	ao_vqe_close(ts_void *handle)
{
	TS_S32 s32Ret;

	s32Ret = TS_ALG_VQESolWebRTC3A_Exit(handle);
	if (s32Ret) {
		printf("ao_vqe_close failed ret=%d\n", s32Ret);
		return s32Ret;
	}
	return TS_SUCCESS;
}

#elif VQE_NN
ts_s32	ai_vqe_open(ts_void **handle, ts_void *attr)
{

		if (0 != TS_MPI_TRP_RNE_MutexInit()) {
		printf("ai vqe trp rne init failed !\n");
	}

	if (0 != TS_MPI_TRP_RNE_MutexLockWithTimeout(1, 5000)) {
		printf("ai trp rne lock timeout failed !\n");
		return TS_FAILURE;
	}
	TS_S32 err = TS_SOL_VQE_NN_Init(handle, (SOL_VQE_NN_CONF_PARAM *)attr);
	if ((!*handle) || (err != 0)){
		printf("Error, Initializtion Fail.\n");
		return -1;
	}else{
		printf("Initializtion ok.\n");
	}
	if (0 != TS_MPI_TRP_RNE_MutexUnlock()) {
		printf("ai trp rne unlock failed !\n");
	}
	return TS_SUCCESS;
}

ts_s32	ai_vqe_set_proc_addr(ts_void *handle, ts_void *addr)
{

	printf("zzz ai_vqe_set_proc_addr\n");
	return TS_SUCCESS;
}

ts_s32	ai_vqe_process(ts_void *handle, ts_u8 *in_buf, ts_u8 *ref_buf, ts_u32 *in_len, ts_u8 *out_buf, ts_u32 *out_len)
{
	TS_S32 s32Ret;
	TS_U32 u32OutLen, u32InLen;
	TS_U32 u32ShortBytes;

	u32ShortBytes = sizeof(short);
	u32InLen = *in_len/u32ShortBytes;
#if 0
	static FILE* pfd_in;
	if (pfd_in == NULL)
		pfd_in = fopen("ainn_vqe_file_in.pcm", "w+");
	fwrite(in_buf, 1, *in_len, pfd_in);
#endif
#if 0
#if defined(VQE_STEREO_MVDR) || defined(VQE_NN_STEREO_8K) || defined(VQE_NN_STEREO_16K)
	TS_S32 ref_len = *in_len/2;
#else
	TS_S32 ref_len = *in_len;
#endif
	static FILE* pfd_ref;
	if (pfd_ref == NULL)
		pfd_ref = fopen("ainn_vqe_file_ref.pcm", "w+");
	fwrite(ref_buf, 1, ref_len, pfd_ref);
#endif
	if (0 != TS_MPI_TRP_RNE_MutexLockWithTimeout(1, 5000)) {
		printf("ai trp rne lock timeout failed !\n");
		return TS_FAILURE;
	}
	s32Ret = TS_SOL_VQE_NN_Process(handle, (TS_S16 *)in_buf, (TS_S16 *)ref_buf, &u32InLen, (TS_S16 *)out_buf, &u32OutLen);
	if (0 != TS_MPI_TRP_RNE_MutexUnlock()) {
		printf("ai trp rne unlock failed !\n");
	}
	if (s32Ret) {
		printf("ai_vqe_process failed ret=%d\n", s32Ret);
		return s32Ret;
	}
	*out_len = u32OutLen * u32ShortBytes;
#if 0
	static FILE* pfd_aiout;
	if (pfd_aiout == NULL)
		pfd_aiout = fopen("ainn_vqe_file_out.pcm", "w+");
	fwrite(out_buf, 1, *out_len, pfd_aiout);
#endif
	return TS_SUCCESS;
}

ts_s32	ai_vqe_close(ts_void *handle)
{
	TS_S32 s32Ret;

	if (0 != TS_MPI_TRP_RNE_MutexLockWithTimeout(1, 5000)) {
		printf("ai trp rne lock timeout failed !\n");
		return TS_FAILURE;
	}
	s32Ret = TS_SOL_VQE_NN_Exit(handle);
	if (s32Ret) {
		printf("ai_vqe_close failed ret=%d\n", s32Ret);
		return s32Ret;
	}
	if (0 != TS_MPI_TRP_RNE_MutexUnlock()) {
		printf("ai trp rne unlock failed !\n");
	}
	
	if (0 != TS_MPI_TRP_RNE_MutexDeInit()) {
		printf("ai vqe trp ren deinit failed !\n");
	}
	return TS_SUCCESS;
}

ts_s32	ao_vqe_open(ts_void **handle, ts_void *attr)
{
	if (0 != TS_MPI_TRP_RNE_MutexInit()) {
		printf("ao vqe trp rne init failed !\n");
	}
	if (0 != TS_MPI_TRP_RNE_MutexLockWithTimeout(1, 5000)) {
		printf("ai trp rne lock timeout failed !\n");
		return TS_FAILURE;
	}
	TS_S32 err = TS_SOL_VQE_NN_Init(handle, (SOL_VQE_NN_CONF_PARAM *)attr);
	if ((!*handle) || (err != 0)){
		printf("Error, Initializtion Fail.\n");
		return -1;
	}else{
		printf("Initializtion ok.\n");
	}
	if (0 != TS_MPI_TRP_RNE_MutexUnlock()) {
		printf("ai trp rne unlock failed !\n");
	}
	return TS_SUCCESS;
}

ts_s32	ao_vqe_set_proc_addr(ts_void *handle, ts_void *addr)
{

	printf("zzz ao_vqe_set_proc_addr\n");
	return TS_SUCCESS;
}

ts_s32	ao_vqe_process(ts_void *handle, ts_u8 *in_buf, ts_u8 *ref_buf, ts_u32 *in_len, ts_u8 *out_buf, ts_u32 *out_len)
{

	TS_S32 s32Ret;
	TS_U32 u32OutLen, u32InLen;
	TS_U32 u32ShortBytes;
#if 0
	static FILE* pfd_aoin;
	if (pfd_aoin == NULL)
		pfd_aoin = fopen("ao_vqe_file_in.pcm", "w+");
	fwrite(in_buf, 1, *in_len, pfd_aoin);
#endif
	u32ShortBytes = sizeof(short);
	u32InLen = *in_len/u32ShortBytes;

	if (0 != TS_MPI_TRP_RNE_MutexLockWithTimeout(1, 5000)) {
		printf("trp rne lock timeout failed !\n");
	}
	s32Ret = TS_SOL_VQE_NN_Process(handle, (TS_S16 *)in_buf, (TS_S16 *)ref_buf, &u32InLen, (TS_S16 *)out_buf, &u32OutLen);
	if (0 != TS_MPI_TRP_RNE_MutexUnlock()) {
		printf("ao trp rne unlock failed !\n");
	}
	if (s32Ret) {
		printf("ao_vqe_process failed ret=%d\n", s32Ret);
		return s32Ret;
	}
	*out_len = u32OutLen * u32ShortBytes;
#if 0
	static FILE* pfd_aoout;
	if (pfd_aoout == NULL)
		pfd_aoout = fopen("ao_vqe_file_out.pcm", "w+");
	fwrite(out_buf, 1, *out_len, pfd_aoout);
#endif
	return TS_SUCCESS;
}

ts_s32	ao_vqe_close(ts_void *handle)
{
	if (0 != TS_MPI_TRP_RNE_MutexLockWithTimeout(1, 5000)) {
		printf("ai trp rne lock timeout failed !\n");
		return TS_FAILURE;
	}
	TS_S32 s32Ret;
	s32Ret = TS_SOL_VQE_NN_Exit(handle);
	if (s32Ret) {
		printf("ao_vqe_close failed ret=%d\n", s32Ret);
		return s32Ret;
	}
	if (0 != TS_MPI_TRP_RNE_MutexUnlock()) {
		printf("ai trp rne unlock failed !\n");
	}
	
	if (0 != TS_MPI_TRP_RNE_MutexDeInit()) {
		printf("ao vqe trp ren deinit failed !\n");
	}
	return TS_SUCCESS;
}
#endif

//#ifdef OPUS_CODEC
typedef struct OPUS_ENC_PARAM {
	ts_s32 sample_rate; // only support 8kHz / 16kHz
	ts_s32 chan_num; // only support 1ch / 2ch
	ts_s32 sample_num; // only support 160 / 320
	ts_s32 bps; // bit rate per second
} OPUS_ENC_PARAM;

ts_s32 aenc_opus_init(void **handle, void *attr)
{
	AENC_CHN_ATTR_S *_attr = (AENC_CHN_ATTR_S *)attr;
	if (NULL == _attr) {
		PRINT("error, NULL == _attr\n");
		return -1;
	}

	OPUS_ENC_PARAM *attr1 = (OPUS_ENC_PARAM *)_attr->pValue;
	if (NULL == attr1) {
		PRINT("error, NULL == attr1\n");
		return -1;
	}

	/*PRINT("sample_rate:%d chan_num:%d sample_num:%d bps:%d %p\n", attr1->sample_rate, attr1->chan_num,
		 attr1->sample_num, attr1->bps, attr1);*/

	if (attr1->sample_rate != 8000 && attr1->sample_rate != 16000) {
		PRINT("error, only support 8kHz / 16kHz\n");
		return -1;
	}

	if (attr1->chan_num != 1 && attr1->chan_num != 2) {
		PRINT("error, only support 1ch / 2ch\n");
		return -1;
	}

	if (attr1->sample_num != 160 && attr1->sample_num != 320) {
		PRINT("error, only support 160 / 320 sample_num\n");
		return -1;
	}

	ALG_OPUS_ENC_PARAM_S param = { .s32SampleRate = attr1->sample_rate,
				       .s32ChanNum = attr1->chan_num,
				       .s32SampleNum = attr1->sample_num,
				       .s32Bps = attr1->bps };
	int ret = TS_ALG_OpusEnc_Init(handle, &param);
	if (ret < 0) {
		PRINT("TS_ALG_OpusEnc_Init error\n");
		return ret;
	}

	PRINT("sample_rate:%d chan_num:%d sample_num:%d bps:%d\n", param.s32SampleRate, param.s32ChanNum,
		 param.s32SampleNum, param.s32Bps);

	return 0;
}

ts_s32 aenc_opus_exit(void *handle)
{
	return TS_ALG_OpusEnc_Exit(handle);
}

ts_s32 aenc_opus_process(void *handle, AUDIO_FRAME_S *frame, ts_u8 *outbuf, ts_u32 *outlen)
{
	if (NULL == handle || NULL == frame || NULL == outbuf) {
		PRINT("error, invalid parameter!\n");
		return -1;
	}

	ALG_IMAGE_S input;
	input.s32H = 1;
	input.s32W = 1;
	input.s32C = frame->u32Len / 2; // 采样点数
	input.pData = frame->u64VirAddr[0];

	ALG_OPUS_ENC_RESULT_S result;
	result.pBitstream = outbuf;

	ts_s32 ret = TS_ALG_OpusEnc_Process(handle, &input, &result);
	if (ret < 0) {
		PRINT("TS_ALG_OpusEnc_Process error\n");
		return ret;
	}

	*outlen = result.u32ByteNum;
	//PRINT("output:%uB\n", result.u32ByteNum);

	return ret;
}

typedef struct OPUS_DEC_PARAM {
	ts_s32 sample_rate; // only support 8k / 16k
	ts_s32 chan_num; // only support 1ch / 2ch
} OPUS_DEC_PARAM;

ts_s32 adec_opus_init(void **handle, void *attr)
{
	ts_s32 ret;

	ADEC_CHN_ATTR_S *_attr = (ADEC_CHN_ATTR_S *)attr;
	if (NULL == _attr) {
		PRINT("error, NULL == _attr\n");
		return -1;
	}

	OPUS_DEC_PARAM *attr1 = (OPUS_DEC_PARAM *)_attr->pValue;
	if (NULL == attr1) {
		PRINT("error, NULL == attr1\n");
		return -1;
	}

	if (attr1->sample_rate != 8000 && attr1->sample_rate != 16000) {
		PRINT("error, only support 8kHz / 16kHz\n");
		return -1;
	}

	if (attr1->chan_num != 1 && attr1->chan_num != 2) {
		PRINT("error, only support 1ch / 2ch\n");
		return -1;
	}

	//log_set_level(ALG_LOG_DEBUG);

	ALG_OPUS_DEC_PARAM_S param = {
		.s32SampleRate = attr1->sample_rate, // 指定解码输出的采样率。
		.s32ChanNum = attr1->chan_num // 指定解码输出的声道数。
	};

	PRINT("sample_rate:%d chan_num:%d\n", param.s32SampleRate, param.s32ChanNum);

	ret = TS_ALG_OpusDec_Init(handle, &param);
	if (ret < 0) {
		PRINT("TS_ALG_OpusDec_Init error\n");
		return ret;
	}

	return 0;
}

ts_s32 adec_opus_exit(void *handle)
{
	return TS_ALG_OpusDec_Exit(handle);
}

ts_s32 adec_opus_process(void *handle, ts_u8 *stream, ts_u32 *frame_byte, ts_u8 *out_buf, ts_u32 *out_len, ts_u32 *chn)
{
	if (NULL == handle || NULL == stream || NULL == out_buf) {
		PRINT("error, invalid parameter!\n");
		return -1;
	}

	ALG_IMAGE_S input;
	memset(&input, 0, sizeof(input));
	input.pData = stream;

	ALG_OPUS_DEC_RESULT_S result;
	result.pSampleData = (ts_s16 *)out_buf;

	ts_s32 ret = TS_ALG_OpusDec_Process(handle, &input, &result);
	if (ret < 0) {
		PRINT("TS_ALG_OpusDec_Process error\n");
		return ret;
	}

	*out_len = result.u32ByteNum * 2;
	//PRINT("output %u bytes\n", *out_len);
	*chn = 1;

	return 0;
}

ts_s32 adec_opus_get_frm_info(void *handle, void *info)
{
	return 0;
}
//#endif // OPUS_CODEC

//#ifdef AAC_CODEC
typedef struct attr_aac_aenc_s {
	ts_u32 sample_rate; // sample rate
	ts_u32 channel; // channel
	ts_u32 mode; // /*0 - raw; 1 - ADTS*/
} attr_aac_aenc_t;

ts_s32 aenc_aac_init(void **handle, void *attr)
{
	AENC_CHN_ATTR_S *_attr = (AENC_CHN_ATTR_S *)attr;
	if (NULL == _attr) {
		PRINT("error, NULL == _attr\n");
		return -1;
	}

	attr_aac_aenc_t *attr1 = (attr_aac_aenc_t *)_attr->pValue;
	if (NULL == attr1) {
		PRINT("error, NULL == attr1\n");
		return -1;
	}
	PRINT("attr1:%p samplerate:%u\n", attr1, attr1->sample_rate);

	ALG_AAC_ENC_INIT_S param;
	ts_u32 input_sample = 0;
	ts_u32 max_outputbytes = 0;
	param.u32SampleRate = attr1->sample_rate;
	param.u32NumChannels = 1;
	param.u32OutputMode = 1;
	param.pInputSamples = &input_sample;
	param.pMaxOutputBytes = &max_outputbytes;

	TS_ALG_AAC_ENC_Init(handle, &param);
	PRINT("samplerate:%u chan:%u mode:%u inputsample:%u maxoutsample:%u\n", param.u32SampleRate,
		 param.u32NumChannels, param.u32OutputMode, input_sample, max_outputbytes);

	return 0;
}

ts_s32 aenc_aac_exit(void *handle)
{
	return TS_ALG_AAC_ENC_Exit(handle);
}

ts_s32 aenc_aac_process(void *handle, AUDIO_FRAME_S *frame, ts_u8 *outbuf, ts_u32 *outlen)
{
	if (NULL == handle || NULL == frame || NULL == outbuf) {
		PRINT("error, invalid parameter!\n");
		return -1;
	}

	ALG_AAC_ENC_PARAM_S input, output;

	input.u32SampleNum = 2048; // 是数据字节数，不是采样点数。
	input.pBitStream = (TS_U8  *)frame->u64VirAddr[0];

	output.pBitStream = outbuf;
	ts_s32 ret = TS_ALG_AAC_ENC_Process(handle, &input, &output); // 输入数据必须是2048字节
	*outlen = output.u32SampleNum;

	return ret;
}

ts_s32 adec_aac_init(void **handle, void *attr)
{
	ALG_AAC_DEC_INIT_S *_attr = (ALG_AAC_DEC_INIT_S *)attr;
	if (NULL == _attr) {
		PRINT("error, invalid parameter!\n");
		return -1;
	}

	if (0 == _attr->u32ByteNum) {
		PRINT("no aac audio frame data!\n");
		return -1;
	}

	//log_set_level(ALG_LOG_DEBUG);

	TS_ALG_AAC_DEC_Init(handle, _attr);

	PRINT("samplerate:%d chan:%d\n", _attr->u32SampleRate, _attr->u32NumChannels);
	return 0;
}

ts_s32 adec_aac_exit(void *handle)
{
	return TS_ALG_AAC_DEC_Exit(handle);
}

ts_s32 adec_aac_process(void *handle, ts_u8 *stream, ts_u32 *frame_byte, ts_u8 *out_buf, ts_u32 *out_len, ts_u32 *chn)
{
	if (NULL == handle || NULL == stream || NULL == out_buf) {
		PRINT("error, invalid parameter!\n");
		return -1;
	}

	ts_s32 ret = 0;
	ALG_AAC_DEC_PARAM_S input;
	memset(&input, 0, sizeof(ALG_AAC_DEC_PARAM_S));

	input.u32InputByteNum = 8;
	input.pBitStream = stream;

	ret = TS_ALG_AAC_DEC_Process(handle, &input);
	if (0 != ret) {
		PRINT("error, aac decode failed!\n");
		return -1;
	}

	// PRINT("OutputSamples:%d DecoderByteNum:%d\n", input.u32OutputSamples, input.u32DecoderByteNum);

	*out_len = input.u32OutputSamples * 2;
	memcpy(out_buf, input.pOutStream, *out_len);
	*frame_byte = input.u32DecoderByteNum;
	*chn = 1;

	return 0;
}

ts_s32 adec_aac_get_frm_info(void *handle, void *info)
{
	return 0;
}
//#endif // AAC_CODEC

//#ifdef G726_CODEC
ts_s32 aenc_g726_init(void **handle, void *attr)
{
	AENC_CHN_ATTR_S *_attr = (AENC_CHN_ATTR_S *)attr;
	if (NULL == _attr) {
		PRINT("error, NULL == _attr\n");
		return -1;
	}

	AENC_ATTR_G726_S *attr1 = (AENC_ATTR_G726_S *)_attr->pValue;
	if (NULL == attr1) {
		PRINT("error, NULL == attr1\n");
		return -1;
	}

	if (attr1->enG726bps != G726_16K) {
		PRINT("error, only support 16kbps\n");
		return -1;
	}

	ALG_G726_INIT_S init_param;
	init_param.bps = ALG_G726_16K;
	TS_ALG_G726_Init(handle, &init_param);

	return 0;
}

ts_s32 aenc_g726_exit(void *handle)
{
	TS_ALG_G726_Exit(&handle);

	return 0;
}

ts_s32 aenc_g726_process(void *handle, AUDIO_FRAME_S *frame, ts_u8 *outbuf, ts_u32 *outlen)
{
	if (NULL == handle || NULL == frame || NULL == outbuf) {
		PRINT("error, invalid parameter!\n");
		return -1;
	}

	ALG_G726_SMPL_S in_frame;
	in_frame.pSmpl = (ts_s16 *)frame->u64VirAddr[0];
	in_frame.u32SmplLen = frame->u32Len / sizeof(ts_s16);

	ALG_G726_STREAM_S out_stream;
	out_stream.pStream = (int8_t *)outbuf;
	TS_ALG_G726_Encode(handle, &in_frame, &out_stream);
	*outlen = out_stream.u32ByteLen;

	return 0;
}

ts_s32 adec_g726_init(void **handle, void *attr)
{
	ADEC_CHN_ATTR_S *_attr = (ADEC_CHN_ATTR_S *)attr;
	if (NULL == _attr) {
		PRINT("error, NULL == _attr\n");
		return -1;
	}

	ADEC_ATTR_G726_S *attr1 = (ADEC_ATTR_G726_S *)_attr->pValue;
	if (NULL == attr1) {
		PRINT("error, NULL == attr1\n");
		return -1;
	}

	if (attr1->enG726bps != G726_16K) {
		PRINT("error, only support 16kbps\n");
		return -1;
	}

	ALG_G726_INIT_S init_param;
	init_param.bps = ALG_G726_16K;
	TS_ALG_G726_Init(handle, &init_param);

	return 0;
}

ts_s32 adec_g726_exit(void *handle)
{
	TS_ALG_G726_Exit(&handle);

	return 0;
}

ts_s32 adec_g726_process(void *handle, ts_u8 *stream, ts_u32 *frame_byte, ts_u8 *out_buf, ts_u32 *out_len, ts_u32 *chn)
{
	if (NULL == handle || NULL == stream || NULL == out_buf) {
		PRINT("error, invalid parameter!\n");
		return -1;
	}

	ALG_G726_STREAM_S in_stream;
	in_stream.pStream = (int8_t *)stream;
	in_stream.u32ByteLen = *frame_byte;

	ALG_G726_SMPL_S out_frame;
	out_frame.pSmpl = (int16_t *)out_buf;

	TS_ALG_G726_Decode(handle, &in_stream, &out_frame);
	*out_len = out_frame.u32SmplLen * sizeof(int16_t);

	*chn = 1;

	return 0;
}

ts_s32 adec_g726_get_frm_info(void *handle, void *info)
{
	return 0;
}
//#endif // G726_CODEC

//#ifdef ADPCM_CODEC
ts_s32 aenc_adpcm_init(void **handle, void *attr)
{
	AENC_CHN_ATTR_S *_attr = (AENC_CHN_ATTR_S *)attr;
	if (NULL == _attr) {
		PRINT("error, NULL == _attr\n");
		return -1;
	}

	TS_ALG_ADPCM_Init(handle);

	return 0;
}

ts_s32 aenc_adpcm_exit(void *handle)
{
	TS_ALG_ADPCM_Exit(&handle);

	return 0;
}

ts_s32 aenc_adpcm_process(void *handle, AUDIO_FRAME_S *frame, ts_u8 *outbuf, ts_u32 *outlen)
{
	if (NULL == handle || NULL == frame || NULL == outbuf) {
		PRINT("error, invalid parameter!\n");
		return -1;
	}

	ALG_ADPCM_SMPL_S in_frame;
	in_frame.pSmpl = (short *)frame->u64VirAddr[0];
	in_frame.u32ByteLen = frame->u32Len;

	ALG_ADPCM_STREAM_S out_stream;
	out_stream.pStream = (ts_s8 *)outbuf;

	TS_ALG_ADPCM_Encode(handle, &in_frame, &out_stream);

	// adpcm_coder((short *)frame->virt_addr[0], (char *)outbuf, frame->length, handle);
	if (outlen)
		*outlen = out_stream.u32ByteLen;

	return 0;
}

ts_s32 adec_adpcm_init(void **handle, void *attr)
{
	ADEC_CHN_ATTR_S *_attr = (ADEC_CHN_ATTR_S *)attr;
	if (NULL == _attr) {
		PRINT("error, NULL == _attr\n");
		return -1;
	}

	TS_ALG_ADPCM_Init(handle);

	return 0;
}

ts_s32 adec_adpcm_exit(void *handle)
{
	TS_ALG_ADPCM_Exit(&handle);

	return 0;
}

ts_s32 adec_adpcm_process(void *handle, ts_u8 *stream, ts_u32 *frame_byte, ts_u8 *out_buf, ts_u32 *out_len, ts_u32 *chn)
{
	if (NULL == handle || NULL == stream || NULL == out_buf) {
		PRINT("error, invalid parameter!\n");
		return -1;
	}

	ALG_ADPCM_STREAM_S in_stream;
	in_stream.pStream = (ts_s8 *)stream;
	in_stream.u32ByteLen = *frame_byte;

	ALG_ADPCM_SMPL_S out_frame;
	out_frame.pSmpl = (short *)out_buf;

	TS_ALG_ADPCM_Decode(handle, &in_stream, &out_frame);

	// adpcm_decoder((ts_s8 *)stream, (short *)out_buf, *frame_byte, handle);
	if (out_len)
		*out_len = *frame_byte * 4;
	if (chn)
		*chn = 1;

	return 0;
}

ts_s32 adec_adpcm_get_frm_info(void *handle, void *info)
{
	return 0;
}
//#endif // ADPCM_CODEC

//#ifdef G711_CODEC
typedef struct attr_g711_aenc_s {
	ts_s32 mode; //0:alaw 1:ulaw
} attr_g711_aenc_t;

ts_s32 aenc_g711_init(void **handle, void *attr)
{
	AENC_CHN_ATTR_S *_attr = (AENC_CHN_ATTR_S *)attr;

	if (NULL == _attr) {
		PRINT("error, invalid parameter!\n");
		return -1;
	}

	attr_g711_aenc_t *g711_attr = (attr_g711_aenc_t *)calloc(1, sizeof(attr_g711_aenc_t));
	if (NULL == g711_attr) {
		PRINT("malloc failed!\n");
		return -1;
	}

	if (PT_G711A == _attr->enType)
		g711_attr->mode = 0;
	else if (PT_G711U == _attr->enType)
		g711_attr->mode = 1;

	*handle = g711_attr;

	return 0;
}

ts_s32 aenc_g711_exit(void *handle)
{
	attr_g711_aenc_t *g711_attr = (attr_g711_aenc_t *)handle;

	if (NULL != g711_attr) {
		free(g711_attr);
		g711_attr = NULL;
	}

	return 0;
}

ts_s32 aenc_g711_process(void *handle, AUDIO_FRAME_S *frame, ts_u8 *outbuf, ts_u32 *outlen)
{
	if (NULL == handle || NULL == frame || NULL == outbuf) {
		PRINT("error, invalid parameter!\n");
		return -1;
	}

	attr_g711_aenc_t *g711_attr = (attr_g711_aenc_t *)handle;
	ts_s32 mode = g711_attr->mode;
	ts_u8 *in_buf = (ts_u8 *)frame->u64VirAddr[0];

	*outlen = TS_ALG_G711_Encoder(mode, outbuf, in_buf, frame->u32Len);

	return 0;
}

typedef struct attr_g711_adec_s {
	ts_s32 mode; //0:alaw 1:ulaw
} attr_g711_adec_t;

ts_s32 adec_g711_init(void **handle, void *attr)
{
	ADEC_CHN_ATTR_S *_attr = (ADEC_CHN_ATTR_S *)attr;

	if (NULL == _attr) {
		PRINT("error, invalid parameter!\n");
		return -1;
	}

	attr_g711_adec_t *g711_attr = (attr_g711_adec_t *)calloc(1, sizeof(attr_g711_adec_t));
	if (NULL == g711_attr) {
		PRINT("malloc failed!\n");
		return -1;
	}

	if (PT_G711A == _attr->enType)
		g711_attr->mode = 0;
	else if (PT_G711U == _attr->enType)
		g711_attr->mode = 1;

	*handle = g711_attr;

	return 0;
}

ts_s32 adec_g711_exit(void *handle)
{
	attr_g711_adec_t *g711_attr = (attr_g711_adec_t *)handle;

	if (NULL != g711_attr) {
		free(g711_attr);
		g711_attr = NULL;
	}

	return 0;
}

//g711的解码frame_byte 用作输入参数，传递进来需要解码的帧长，aac解码frame_byte 用作输出参数，用来输出，解析出来的一帧长度
ts_s32 adec_g711_process(void *handle, ts_u8 *stream, ts_u32 *frame_byte, ts_u8 *out_buf, ts_u32 *out_len, ts_u32 *chn)
{
	if (NULL == handle || NULL == stream || NULL == out_buf) {
		PRINT("error, invalid parameter!\n");
		return -1;
	}

	attr_g711_adec_t *g711_attr = (attr_g711_adec_t *)handle;
	ts_s32 mode = g711_attr->mode;
	ts_u32 in_len = *frame_byte;
	ts_u8 *in_buf = stream;

	*out_len = TS_ALG_G711_Decoder(mode, out_buf, in_buf, in_len);
	*chn = 1;

	return 0;
}

ts_s32 adec_g711_get_frm_info(void *handle, void *info)
{
	return 0;
}
//#endif // G711_CODEC

ts_s32 aenc_pcm_init(void **handle, void *attr)
{
	AENC_CHN_ATTR_S *_attr = (AENC_CHN_ATTR_S *)attr;
	if (NULL == _attr) {
		PRINT("error, NULL == _attr\n");
		return -1;
	}

	AENC_ATTR_LPCM_S *attr1 = (AENC_ATTR_LPCM_S *)_attr->pValue;
	if (NULL == attr1) {
		PRINT("error, NULL == attr1\n");
		return -1;
	}

	*handle = attr1;

	return 0;
}

ts_s32 aenc_pcm_exit(void *handle)
{
	return 0;
}

ts_s32 aenc_pcm_process(void *handle, AUDIO_FRAME_S *frame, ts_u8 *outbuf, ts_u32 *outlen)
{
	if (!handle || !frame || !outbuf) {
		PRINT("error! handle:%p frame:%p outbuf:%p\n", handle, frame, outbuf);
		return -1;
	}

	memcpy(outbuf, frame->u64VirAddr[0], frame->u32Len);
	*outlen = frame->u32Len;

	return 0;
}

ts_s32 adec_pcm_init(void **handle, void *attr)
{
	ADEC_CHN_ATTR_S *_attr = (ADEC_CHN_ATTR_S *)attr;
	if (NULL == _attr) {
		PRINT("error, NULL == _attr\n");
		return -1;
	}

	ADEC_ATTR_LPCM_S *attr1 = (ADEC_ATTR_LPCM_S *)_attr->pValue;
	if (NULL == attr1) {
		PRINT("error, NULL == attr1\n");
		return -1;
	}

	*handle = attr1;

	return 0;
}

ts_s32 adec_pcm_exit(void *handle)
{
	return 0;
}

ts_s32 adec_pcm_process(void *handle, ts_u8 *stream, ts_u32 *frame_byte, ts_u8 *out_buf, ts_u32 *out_len, ts_u32 *chn)
{
	if (!handle || !stream || !out_buf) {
		PRINT("error, invalid parameter!\n");
		return -1;
	}

	ts_u32 in_len = *frame_byte;
	memcpy(out_buf, stream, in_len);
	*out_len = in_len;
	*chn = 1;

	return 0;
}

ts_s32 adec_pcm_get_frm_info(void *handle, void *info)
{
	return 0;
}

int exitThreadSig = 0;

/******************************************************************************
* function : get frame from Ai, send it  to Aenc or Ao
******************************************************************************/
void* SAMPLE_COMM_AUDIO_AiProc(void* parg)
{
    TS_S32 s32Ret;
    TS_S32 AiFd;
    SAMPLE_AI_S* pstAiCtl = (SAMPLE_AI_S*)parg;
    AUDIO_FRAME_S stFrame;
    fd_set read_fds;
    struct timeval TimeoutVal;

	if (TS_TRUE == pstAiCtl->bSendStream)
	{
		RTSP_STREAM_TYPE_E type;
		if (PT_LPCM == gs_enPayloadType) {
			type = RTSP_STREAM_TYPE_PCM;
		} else {
			printf("Unsupported encoding stream type\n");
			pstAiCtl->bSendStream = TS_FALSE;
			return NULL;
		}
		s32Ret = openPushStreamAudioServer(type, g_enSampleRate, 1);
		if (TS_SUCCESS != s32Ret )
		{
			printf("%s: openPushStreamAudioServer(%d), failed with %#x!\n",
					__FUNCTION__, pstAiCtl->AiChn, s32Ret);
			pstAiCtl->bStart = TS_FALSE;
			return NULL;
		}
	}

    FD_ZERO(&read_fds);
    AiFd = TS_MPI_AI_GetFd(pstAiCtl->AiDev, pstAiCtl->AiChn);
    FD_SET(AiFd, &read_fds);

    while (pstAiCtl->bStart)
    {
		TimeoutVal.tv_sec = 15;
        TimeoutVal.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(AiFd, &read_fds);

        s32Ret = TS_MPI_AI_SELECT(AiFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            break;
        }
        else if (0 == s32Ret)
        {
            printf("%s: get ai frame select time out\n", __FUNCTION__);
            break;
        }

        if (FD_ISSET(AiFd, &read_fds))
        {
            /* get frame from ai chn */
            memset(&stFrame, 0, sizeof(AUDIO_FRAME_S));
            s32Ret = TS_MPI_AI_GetFrame(pstAiCtl->AiDev, pstAiCtl->AiChn, &stFrame, NULL, TS_FALSE);
            if (TS_SUCCESS != s32Ret )
            {
#if 0
                printf("%s: TS_MPI_AI_GetFrame(%d, %d), failed with %#x!\n", \
                       __FUNCTION__, pstAiCtl->AiDev, pstAiCtl->AiChn, s32Ret);
                pstAiCtl->bStart = TS_FALSE;
                return NULL;
#else
                continue;
#endif
            }
			//printf("%s:u32Len = %d, pFrame->u64VirAddr[0]= %p\n", __func__, stFrame.u32Len, stFrame.u64VirAddr[0]);
			if (TS_FALSE == pstAiCtl->bSendStream)
				test_save_pcm(&stFrame);

            /* send frame to encoder */
            if (TS_TRUE == pstAiCtl->bSendAenc)
            {
                s32Ret = TS_MPI_AENC_SendFrame(pstAiCtl->AencChn, &stFrame);
                if (TS_SUCCESS != s32Ret )
                {
                    printf("%s: TS_MPI_AENC_SendFrame(%d), failed with %#x!\n",
                           __FUNCTION__, pstAiCtl->AencChn, s32Ret);
                    pstAiCtl->bStart = TS_FALSE;
                    return NULL;
                }
            }

            /* send frame to stream server */
            if (TS_TRUE == pstAiCtl->bSendStream)
			{
				if (stFrame.u32Len != 0) {
					s32Ret = pushAudioStream(stFrame.u64VirAddr[0], (&stFrame)->u32Len);
					if (TS_SUCCESS != s32Ret )
					{
						printf("%s: sendTsingRtspAudioStream(%d), failed with %#x!\n",
							__FUNCTION__, pstAiCtl->AiChn, s32Ret);
					}
				}
            }

            /* send frame to ao */
            if (TS_TRUE == pstAiCtl->bSendAo)
            {
                s32Ret = TS_MPI_AO_SendFrame(pstAiCtl->AoDev, pstAiCtl->AoChn, &stFrame, 1000);
                if (TS_SUCCESS != s32Ret )
                {
                    printf("%s: TS_MPI_AO_SendFrame(%d, %d), failed with %#x!\n", \
                           __FUNCTION__, pstAiCtl->AoDev, pstAiCtl->AoChn, s32Ret);
                    pstAiCtl->bStart = TS_FALSE;
                    return NULL;
                }

            }

            /* finally you must release the stream */
            s32Ret = TS_MPI_AI_ReleaseFrame(pstAiCtl->AiDev, pstAiCtl->AiChn, &stFrame, NULL);
            if (TS_SUCCESS != s32Ret )
            {
                printf("%s: TS_MPI_AI_ReleaseFrame(%d, %d), failed with %#x!\n", \
                       __FUNCTION__, pstAiCtl->AiDev, pstAiCtl->AiChn, s32Ret);
                pstAiCtl->bStart = TS_FALSE;
                return NULL;
            }

        }
    }

    pstAiCtl->bStart = TS_FALSE;
    return NULL;
}

/******************************************************************************
* function : get frame from Ai, send it  to Aenc or Ao
******************************************************************************/
void* SAMPLE_COMM_AUDIO_AiTulProc(void* parg)
{
	TS_S32 s32Ret;
	SAMPLE_AI_S* pstAiCtl = (SAMPLE_AI_S*)parg;
	AUDIO_FRAME_S stFrame;

	while (pstAiCtl->bStart)
	{
		/* get frame from ai chn */
		memset(&stFrame, 0, sizeof(AUDIO_FRAME_S));

		s32Ret = TS_MPI_AI_GetFrame(pstAiCtl->AiDev, pstAiCtl->AiChn, &stFrame, NULL, TS_FALSE);
		if (TS_SUCCESS != s32Ret )
		{
			continue;
		}
		//printf("%s:u32Len = %d, pFrame->u64VirAddr[0]= %p\n", __func__, stFrame.u32Len, stFrame.u64VirAddr[0]);
		test_save_pcm(&stFrame);

		/* finally you must release the stream */
		s32Ret = TS_MPI_AI_ReleaseFrame(pstAiCtl->AiDev, pstAiCtl->AiChn, &stFrame, NULL);
		if (TS_SUCCESS != s32Ret )
		{
			printf("%s: TS_MPI_AI_ReleaseFrame(%d, %d), failed with %#x!\n", \
				   __FUNCTION__, pstAiCtl->AiDev, pstAiCtl->AiChn, s32Ret);
			pstAiCtl->bStart = TS_FALSE;
			return NULL;
		}


	}

	pstAiCtl->bStart = TS_FALSE;
	return NULL;
}

/******************************************************************************
* function : get frame from AiRaw, send it  to Aenc or Ao
******************************************************************************/
void* SAMPLE_COMM_AUDIO_AiRawProc(void* parg)
{
    TS_S32 s32Ret;
    TS_S32 AiFd;
    SAMPLE_AI_S* pstAiCtl = (SAMPLE_AI_S*)parg;
    AUDIO_FRAME_S stFrame;
    fd_set read_fds;
    struct timeval TimeoutVal;

    FD_ZERO(&read_fds);
    AiFd = TS_MPI_AI_GetFd(pstAiCtl->AiDev, pstAiCtl->AiChn);
    FD_SET(AiFd, &read_fds);

    while (pstAiCtl->bStart)
    {
        TimeoutVal.tv_sec = 15;
        TimeoutVal.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(AiFd, &read_fds);
        s32Ret = TS_MPI_AI_SELECT(AiFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
        	printf("%s: get ai frame select failed\n", __FUNCTION__);
            break;
        }
        else if (0 == s32Ret)
        {
            printf("%s: get ai frame select time out\n", __FUNCTION__);
            break;
        }

        if (FD_ISSET(AiFd, &read_fds))
        {
            /* get frame from ai chn */
            memset(&stFrame, 0, sizeof(AUDIO_FRAME_S));
            s32Ret = TS_MPI_AI_GetRawFrame(pstAiCtl->AiDev, pstAiCtl->AiChn, &stFrame, NULL, TS_FALSE);
            if (s32Ret) {
#if 0
                printf("%s: TS_MPI_AI_GetFrame(%d, %d), failed with %#x!\n", \
                       __FUNCTION__, pstAiCtl->AiDev, pstAiCtl->AiChn, s32Ret);
                pstAiCtl->bStart = TS_FALSE;
                return NULL;
#else
                continue;
#endif
            }
			//printf("%s:u32Len = %d, pFrame->u64VirAddr[0]= %p\n", __func__, stFrame.u32Len, stFrame.u64VirAddr[0]);
			test_save_raw_pcm(&stFrame);
            /* finally you must release the stream */
            s32Ret = TS_MPI_AI_ReleaseRawFrame(pstAiCtl->AiDev, pstAiCtl->AiChn, &stFrame, NULL);
            if (TS_SUCCESS != s32Ret )
            {
                printf("%s: TS_MPI_AI_ReleaseFrame(%d, %d), failed with %#x!\n", \
                       __FUNCTION__, pstAiCtl->AiDev, pstAiCtl->AiChn, s32Ret);
                pstAiCtl->bStart = TS_FALSE;
                return NULL;
            }

        }
    }

    pstAiCtl->bStart = TS_FALSE;
    return NULL;
}


/******************************************************************************
* function : get frame from AiRef, send it  to Aenc or Ao
******************************************************************************/
void* SAMPLE_COMM_AUDIO_AiRefProc(void* parg)
{
    TS_S32 s32Ret;
    TS_S32 AiFd;
    SAMPLE_AI_S* pstAiCtl = (SAMPLE_AI_S*)parg;
   	AUDIO_FRAME_S stFrame;
    AUDIO_FRAME_S stAecFrm;
    fd_set read_fds;
    struct timeval TimeoutVal;

    FD_ZERO(&read_fds);

    AiFd = TS_MPI_AI_GetFd(pstAiCtl->AiDev, pstAiCtl->AiChn);
    FD_SET(AiFd, &read_fds);

    while (pstAiCtl->bStart)
    {
        TimeoutVal.tv_sec = 15;
        TimeoutVal.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(AiFd, &read_fds);

        s32Ret = TS_MPI_AI_SELECT(AiFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
        	printf("%s: get ai frame select failed\n", __FUNCTION__);
            break;
        }
        else if (0 == s32Ret)
        {
            printf("%s: get ai frame select time out\n", __FUNCTION__);
            break;
        }

        if (FD_ISSET(AiFd, &read_fds))
        {
            /* get frame from ai chn */
			memset(&stFrame, 0, sizeof(AUDIO_FRAME_S));
			memset(&stAecFrm, 0, sizeof(AUDIO_FRAME_S));
            s32Ret = TS_MPI_AI_GetFrame(pstAiCtl->AiDev, pstAiCtl->AiChn, &stFrame, &stAecFrm, TS_FALSE);
            if (TS_SUCCESS != s32Ret )
            {
#if 0
                printf("%s: TS_MPI_AI_GetFrame(%d, %d), failed with %#x!\n", \
                       __FUNCTION__, pstAiCtl->AiDev, pstAiCtl->AiChn, s32Ret);
                pstAiCtl->bStart = TS_FALSE;
                return NULL;
#else
                continue;
#endif
            }
			//printf("%s:u32Len = %d, pFrame->u64VirAddr[0]= %p\n", __func__, stAecFrm.u32Len, stAecFrm.u64VirAddr[0]);
			test_save_pcm(&stFrame);
			test_save_ref_pcm(&stAecFrm);

            /* finally you must release the stream */
            s32Ret = TS_MPI_AI_ReleaseFrame(pstAiCtl->AiDev, pstAiCtl->AiChn, &stFrame, &stAecFrm);
            if (TS_SUCCESS != s32Ret )
            {
                printf("%s: TS_MPI_AI_ReleaseFrame(%d, %d), failed with %#x!\n", \
                       __FUNCTION__, pstAiCtl->AiDev, pstAiCtl->AiChn, s32Ret);
                pstAiCtl->bStart = TS_FALSE;
                return NULL;
            }

        }
    }

    pstAiCtl->bStart = TS_FALSE;
    return NULL;
}


/******************************************************************************
* function : get frame from file, send it to Ao
******************************************************************************/
void* SAMPLE_COMM_AUDIO_AoProc(void* parg)
{
    TS_S32 s32Ret;
	TS_U32 file_size, read_size = 0, num_read;
	TS_U32 remaining_data_size = 0;
    SAMPLE_AO_S* pstAoCtl = (SAMPLE_AO_S*)parg;
    AUDIO_FRAME_S stFrame;
	FILE *pfd = NULL;

	TS_U32 buffer_size = 1024;
	TS_U8 *buffer = NULL;

	buffer = malloc(buffer_size);
	if (!buffer) {
		SAMPLE_PRT("unable to allocate %d bytes\n", buffer_size);
		goto exit;
	}

	if (TS_FALSE == pstAoCtl->bPullStream) {
		//pfd = fopen("./test.pcm", "rb");
		pfd = fopen("./record.wav", "rb");
		if (!pfd) {
			SAMPLE_PRT("can not open file\n");
			goto exit;
		} else {
			SAMPLE_PRT("ao open file ok\n");
		}

		fseek(pfd, 0L, SEEK_END);
		file_size = ftell(pfd);
		remaining_data_size = file_size;
		fseek(pfd, 0L, SEEK_SET);

		remaining_data_size = parse_wave_file(pfd);
		SAMPLE_PRT("ao file size:%d\n", remaining_data_size);
	}

	memset(&stFrame, 0, sizeof(AUDIO_FRAME_S));
	stFrame.u64VirAddr[0] 	= buffer;

    while (pstAoCtl->bStart)
    {
		if (TS_TRUE == pstAoCtl->bPullStream) {
			if (openAudioPullStream(getPullStreamUrl())) {
				sleep(2);
				continue;
			}
			if (TS_SUCCESS != getAudioPullStream(stFrame.u64VirAddr[0], &(stFrame.u32Len))) {
				printf("getAudioPullStream failed\n");
				sleep(1);
				continue;
			}
			if (0 == stFrame.u32Len) {
				printf("getAudioPullStream not audio stream\n");
				continue;
			}
		} else {
			read_size = remaining_data_size > buffer_size ? buffer_size : remaining_data_size;
			num_read = fread(buffer, 1, read_size, pfd);
			remaining_data_size -= num_read;
			if (num_read <= 0)
			{
				SAMPLE_PRT("file read over\n");
#if 0
				break;
#else
				fseek(pfd, 0L, SEEK_END);
				file_size = ftell(pfd);
				remaining_data_size = file_size;
				fseek(pfd, 0L, SEEK_SET);
				remaining_data_size = parse_wave_file(pfd);
				continue;
#endif
			}
			stFrame.u32Len			= num_read;
		}

        /* send frame to ao */
        s32Ret = TS_MPI_AO_SendFrame(pstAoCtl->AoDev, pstAoCtl->AoChn, &stFrame, 1000);
        if (TS_SUCCESS != s32Ret )
        {
            SAMPLE_PRT("%s: TS_MPI_AO_SendFrame(%d, %d), failed with %#x!\n",
                  		__func__, pstAoCtl->AoDev, pstAoCtl->AoChn, s32Ret);
            pstAoCtl->bStart = TS_FALSE;
            break;
        }

    }
	if (TS_TRUE == pstAoCtl->bPullStream) {
		closeAudioPullStream();
	}
exit:
	if (buffer)
	{
		free(buffer);
	}

	if (pfd)
	{
		fclose(pfd);
		pfd = NULL;
	}

    pstAoCtl->bStart = TS_FALSE;
    return NULL;
}

/******************************************************************************
* function : get stream from Aenc, send it to Adec & save it to file
******************************************************************************/
void* SAMPLE_COMM_AUDIO_AencProc(void* parg)
{
    TS_S32 s32Ret;
    TS_S32 AencFd;
    SAMPLE_AENC_S* pstAencCtl = (SAMPLE_AENC_S*)parg;
    AUDIO_STREAM_S stStream;
    AEC_FRAME_S   stAecFrm;
    fd_set read_fds;
    struct timeval TimeoutVal;

	if (TS_TRUE == pstAencCtl->bSendStream)
	{
		RTSP_STREAM_TYPE_E type;
		if (PT_AAC == gs_enPayloadType) {
			type = RTSP_STREAM_TYPE_AAC;
		} else if (PT_G711A == gs_enPayloadType) {
			type = RTSP_STREAM_TYPE_G711A;
		} else {
			printf("Unsupported encoding stream type\n");
			pstAencCtl->bSendStream = TS_FALSE;
			return NULL;
		}
		s32Ret = openPushStreamAudioServer(type, g_enSampleRate, 1);
		if (TS_SUCCESS != s32Ret )
		{
			printf("%s: openPushStreamAudioServer(%d), failed with %#x!\n",
					__FUNCTION__, pstAencCtl->AeChn, s32Ret);
			pstAencCtl->bStart = TS_FALSE;
			return NULL;
		}
	}

    FD_ZERO(&read_fds);
    AencFd = TS_MPI_AENC_GetFd(pstAencCtl->AeChn);
    FD_SET(AencFd, &read_fds);

    while (pstAencCtl->bStart)
    {
 		//printf("start\n");
        TimeoutVal.tv_sec = 15;
        TimeoutVal.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(AencFd, &read_fds);

        s32Ret = TS_MPI_AENC_SELECT(AencFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            break;
        }
        else if (0 == s32Ret)
        {
            printf("%s: get aenc frame select time out\n", __FUNCTION__);
            break;
        }

		//printf("select\n");
        if (FD_ISSET(AencFd, &read_fds))
        {
            /* get frame from aenc chn */
            memset(&stAecFrm, 0, sizeof(AEC_FRAME_S));
            s32Ret = TS_MPI_AENC_GetStream(pstAencCtl->AeChn, &stStream, -1);
			//printf("getStream\n");
            if (TS_SUCCESS != s32Ret )
            {
#if 0
                printf("%s: TS_MPI_AENC_GetFrame(%d, %d), failed with %#x!\n", \
                       __FUNCTION__, pstAencCtl->AencDev, pstAencCtl->AencChn, s32Ret);
                pstAencCtl->bStart = TS_FALSE;
                return NULL;
#else
                continue;
#endif
            }
			//printf("%s:u32Len = %d, pFrame->u64VirAddr[0]= %p", __func__, stStream.u32Len, stStream.pStream);

			//test_save_pcm(&stStream);

			/* save audio stream to file */
			//printf("len=%d\n", stStream.u32Len);
			if (TS_FALSE == pstAencCtl->bSendStream)
			{
				(TS_VOID)fwrite(stStream.pStream, 1, stStream.u32Len, pstAencCtl->pfd);
				fflush(pstAencCtl->pfd);
			}

			aencShowStamp(&stStream);

            /* send frame to decoder */
            if (TS_TRUE == pstAencCtl->bSendAdChn)
            {
                s32Ret = TS_MPI_ADEC_SendStream(pstAencCtl->AdChn, &stStream, TS_TRUE);
                if (TS_SUCCESS != s32Ret )
                {
                    printf("%s: TS_MPI_ADEC_SendStream(%d), failed with %#x!\n",
                           __FUNCTION__, pstAencCtl->AdChn, s32Ret);
                    pstAencCtl->bStart = TS_FALSE;
                    return NULL;
                }
            }

            /* send frame to stream server */
            if (TS_TRUE == pstAencCtl->bSendStream)
			{
				if (stStream.u32Len != 0) {
					s32Ret = pushAudioStream((&stStream)->pStream, (&stStream)->u32Len);
					if (TS_SUCCESS != s32Ret )
					{
						printf("%s: sendTsingRtspAudioStream(%d), failed with %#x!\n",
							__FUNCTION__, pstAencCtl->AeChn, s32Ret);
						//pstAencCtl->bStart = TS_FALSE;
						//return NULL;
					}
				}
            }

            /* finally you must release the stream */
            s32Ret = TS_MPI_AENC_ReleaseStream(pstAencCtl->AeChn, &stStream);
            if (TS_SUCCESS != s32Ret)
            {
                printf("%s: TS_MPI_AENC_ReleaseFrame(%d), failed with %#x!\n", \
                       __FUNCTION__, pstAencCtl->AeChn, s32Ret);
                pstAencCtl->bStart = TS_FALSE;
                return NULL;
            }

        }
    }
	if (TS_TRUE == pstAencCtl->bSendStream)
	{
		s32Ret = closePushStreamAudioServer();
		if (TS_SUCCESS != s32Ret)
		{
			printf("%s: closePushStreamAudioServer(%d), failed with %#x!\n",
					__FUNCTION__, pstAencCtl->AeChn, s32Ret);
			return NULL;
		}
	}
    pstAencCtl->bStart = TS_FALSE;
    return NULL;
}

/******************************************************************************
* function : get stream from file, and send it  to Adec
******************************************************************************/
void* SAMPLE_COMM_AUDIO_AdecProc(void* parg)
{
    TS_S32 s32Ret;
    AUDIO_STREAM_S stAudioStream;
    TS_U32 u32Len = 320;
    TS_U32 u32ReadLen;
    TS_S32 s32AdecChn;
    TS_U8* pu8AudioStream = NULL;
    SAMPLE_ADEC_S* pstAdecCtl = (SAMPLE_ADEC_S*)parg;
    FILE* pfd = pstAdecCtl->pfd;
    s32AdecChn = pstAdecCtl->AdChn;

#if 1
	switch (gs_enPayloadType) {
	case PT_G711A:
	case PT_G711U:
#ifdef VQE_NN
		u32Len = 256;
#elif VQE_WEBRTC
		u32Len = 320;
#ifdef VQE_WEBRTC_NN
		u32Len = 256;
#endif /* VQE_WEBRTC_NN */
#endif
		break;
	case PT_OPUS:
		u32Len = 640;
		break;
	case PT_AAC:
		u32Len = 450;
		break;
	case PT_G726:
		u32Len = 160;
		break;
	case PT_LPCM:
		u32Len = 1024;
		break;
	default:
		break;
	}
#else
#ifdef G711_CODEC

#ifdef VQE_NN
	u32Len = 256;
#elif VQE_WEBRTC
	u32Len = 320;
#ifdef VQE_WEBRTC_NN
	u32Len = 256;
#endif /* VQE_WEBRTC_NN */
#endif

#elif OPUS_CODEC
	u32Len = 640;
#elif AAC_CODEC
	u32Len = 130;
#elif G726_CODEC
	u32Len = 160;
#endif
#endif

    pu8AudioStream = (TS_U8*)malloc(sizeof(TS_U8) * MAX_AUDIO_STREAM_LEN);
    if (NULL == pu8AudioStream)
    {
        printf("%s: malloc failed!\n", __FUNCTION__);
        return NULL;
    }

    while (TS_TRUE == pstAdecCtl->bStart)
    {
		stAudioStream.pStream = pu8AudioStream;

		if (TS_TRUE == pstAdecCtl->bPullStream) {
			if (openAudioPullStream(getPullStreamUrl())) {
				sleep(2);
				continue;
			}
			if (TS_SUCCESS != getAudioPullStream(stAudioStream.pStream, &u32ReadLen)) {
				printf("getAudioPullStream failed\n");
				sleep(1);
				continue;
			}
			if (0 == u32ReadLen) {
				printf("getAudioPullStream not audio stream\n");
				continue;
			}
		} else {
			/* read from file */
			u32ReadLen = fread(stAudioStream.pStream, 1, u32Len, pfd);
			if (u32ReadLen <= 0)
			{
			// s32Ret = TS_MPI_ADEC_SendEndOfStream(s32AdecChn, TS_FALSE);
				//if (TS_SUCCESS != s32Ret)
				//{
				//    printf("%s: TS_MPI_ADEC_SendEndOfStream failed!\n", __FUNCTION__);
				//}
				(TS_VOID)fseek(pfd, 0, SEEK_SET);/*read file again*/
				sleep(1);
				continue;
			}
		}

        /* here only demo adec streaming sending mode, but pack sending mode is commended */
        stAudioStream.u32Len = u32ReadLen;
        s32Ret = TS_MPI_ADEC_SendStream(s32AdecChn, &stAudioStream, TS_TRUE);
        if (TS_SUCCESS != s32Ret)
        {
            printf("%s: TS_MPI_ADEC_SendStream(%d) failed with %#x!\n", \
                   __FUNCTION__, s32AdecChn, s32Ret);
            break;
        }
    }
	if (TS_TRUE == pstAdecCtl->bPullStream) {
		closeAudioPullStream();
	}
    free(pu8AudioStream);
    pu8AudioStream = NULL;
	if (pfd) {
		fclose(pfd);
		pstAdecCtl->pfd = NULL;
	}
    pstAdecCtl->bStart = TS_FALSE;
    return NULL;
}


/******************************************************************************
* function : Create the thread to get frame from ai and send to ao
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAo(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
	SAMPLE_AI_S* pstAi = NULL;

	pstAi = &gs_stSampleAi[AiDev * AI_MAX_CHN_NUM + AiChn];
	pstAi->bSendAenc = TS_FALSE;
	pstAi->bSendAo = TS_FALSE;
	pstAi->bStart = TS_TRUE;
	pstAi->AiDev = AiDev;
	pstAi->AiChn = AiChn;
	pstAi->AoDev = AoDev;
	pstAi->AoChn = AoChn;

	pthread_create(&pstAi->stAiPid, 0, SAMPLE_COMM_AUDIO_AiProc, pstAi);

	return TS_SUCCESS;
}

/******************************************************************************
* funciton : stop the thread to get frame from ai and send to ao
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StopTrdAiAo(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
	SAMPLE_AI_S* pstAi = NULL;

	pstAi = &gs_stSampleAi[AiDev * AI_MAX_CHN_NUM + AiChn];

    if (TS_TRUE == pstAi->bStart)
    {
        pstAi->bStart = TS_FALSE;
        pthread_join(pstAi->stAiPid, 0);
    }
    return TS_SUCCESS;
}

/******************************************************************************
* function : Create the thread to get frame from ai and send to ao
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAiTul(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
	SAMPLE_AI_S* pstAi = NULL;

	pstAi = &gs_stSampleAi[AiDev * AI_MAX_CHN_NUM + AiChn];
	pstAi->bSendAenc = TS_FALSE;
	pstAi->bSendAo = TS_FALSE;
	pstAi->bStart = TS_TRUE;
	pstAi->AiDev = AiDev;
	pstAi->AiChn = AiChn;
	pstAi->AoDev = AoDev;
	pstAi->AoChn = AoChn;

	pthread_create(&pstAi->stAiPid, 0, SAMPLE_COMM_AUDIO_AiTulProc, pstAi);

	return TS_SUCCESS;
}

/******************************************************************************
* funciton : stop the thread to get tul frame from ai
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StopTrdAiTul(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
	SAMPLE_AI_S* pstAi = NULL;

	pstAi = &gs_stSampleAi[AiDev * AI_MAX_CHN_NUM + AiChn];

    if (TS_TRUE == pstAi->bStart)
    {
        pstAi->bStart = TS_FALSE;
        pthread_join(pstAi->stAiPid, 0);
    }
    return TS_SUCCESS;
}


/******************************************************************************
* function : Create the thread to get raw frame from ai
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAiRaw(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
	SAMPLE_AI_S* pstAi = NULL;

	pstAi = &gs_stSampleAi[AiDev * AI_MAX_CHN_NUM + AiChn];
	pstAi->bSendAenc = TS_FALSE;
	pstAi->bSendAo = TS_FALSE;
	pstAi->bStart = TS_TRUE;
	pstAi->AiDev = AiDev;
	pstAi->AiChn = AiChn;
	pstAi->AoDev = AoDev;
	pstAi->AoChn = AoChn;

	pthread_create(&pstAi->stAiPid, 0, SAMPLE_COMM_AUDIO_AiRawProc, pstAi);

	return TS_SUCCESS;
}

/******************************************************************************
* funciton : stop the thread to get raw frame from ai
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StopTrdAiRaw(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
	SAMPLE_AI_S* pstAi = NULL;

	pstAi = &gs_stSampleAi[AiDev * AI_MAX_CHN_NUM + AiChn];

    if (TS_TRUE == pstAi->bStart)
    {
        pstAi->bStart = TS_FALSE;
        pthread_join(pstAi->stAiPid, 0);
    }
    return TS_SUCCESS;
}

/******************************************************************************
* function : Create the thread to get ref frame from ai
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAiRef(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
	SAMPLE_AI_S* pstAi = NULL;

	pstAi = &gs_stSampleAi[AiDev * AI_MAX_CHN_NUM + AiChn];
	pstAi->bSendAenc = TS_FALSE;
	pstAi->bSendAo = TS_FALSE;
	pstAi->bStart = TS_TRUE;
	pstAi->AiDev = AiDev;
	pstAi->AiChn = AiChn;
	pstAi->AoDev = AoDev;
	pstAi->AoChn = AoChn;

	pthread_create(&pstAi->stAiPid, 0, SAMPLE_COMM_AUDIO_AiRefProc, pstAi);

	return TS_SUCCESS;
}

/******************************************************************************
* funciton : stop the thread to get ref frame from ai
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StopTrdAiRef(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
	SAMPLE_AI_S* pstAi = NULL;

	pstAi = &gs_stSampleAi[AiDev * AI_MAX_CHN_NUM + AiChn];

    if (TS_TRUE == pstAi->bStart)
    {
        pstAi->bStart = TS_FALSE;
        pthread_join(pstAi->stAiPid, 0);
    }
    return TS_SUCCESS;
}


/******************************************************************************
* function : Create the thread to get frame from ai and send to ao
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAo(AUDIO_DEV AoDev, AO_CHN AoChn)
{
	SAMPLE_AO_S* pstAo = NULL;

	pstAo = &gs_stSampleAo[AoDev * AI_MAX_CHN_NUM + AoChn];
	pstAo->bStart = TS_TRUE;
	pstAo->AoDev = AoDev;
	pstAo->AoChn = AoChn;

	pthread_create(&pstAo->stAoPid, 0, SAMPLE_COMM_AUDIO_AoProc, pstAo);

	return TS_SUCCESS;
}

/******************************************************************************
* funciton : stop the thread to get frame from ai and send to ao
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StopTrdAo(AUDIO_DEV AoDev, AO_CHN AoChn)
{
	SAMPLE_AO_S* pstAo = NULL;

	pstAo = &gs_stSampleAo[AoDev * AI_MAX_CHN_NUM + AoChn];

    if (TS_TRUE == pstAo->bStart)
    {
        pstAo->bStart = TS_FALSE;
        pthread_join(pstAo->stAoPid, 0);
    }
    return TS_SUCCESS;
}

/******************************************************************************
* function : Create the thread to get frame from ai and send to aenc
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAenc(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn)
{
    SAMPLE_AI_S* pstAi = NULL;

    pstAi = &gs_stSampleAi[AiDev * AI_MAX_CHN_NUM + AiChn];
    pstAi->bSendAenc = TS_TRUE;
    pstAi->bSendAo = TS_FALSE;
    pstAi->bStart = TS_TRUE;
    pstAi->AiDev = AiDev;
    pstAi->AiChn = AiChn;
    pstAi->AencChn = AeChn;
    pthread_create(&pstAi->stAiPid, 0, SAMPLE_COMM_AUDIO_AiProc, pstAi);

    return TS_SUCCESS;
}

/******************************************************************************
* function : Create the thread to get stream from aenc and send to adec
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAencAdec(AENC_CHN AeChn, ADEC_CHN AdChn, FILE* pAecFd)
{
	SAMPLE_AENC_S* pstAenc = NULL;

	if (NULL == pAecFd)
	{
		return TS_FAILURE;
	}

	pstAenc = &gs_stSampleAenc[AeChn];
	pstAenc->AeChn = AeChn;
	pstAenc->AdChn = AdChn;
	pstAenc->bSendAdChn = TS_FALSE;
	pstAenc->pfd = pAecFd;
	pstAenc->bStart = TS_TRUE;
	pthread_create(&pstAenc->stAencPid, 0, SAMPLE_COMM_AUDIO_AencProc, pstAenc);

	return TS_SUCCESS;
}

/******************************************************************************
* funciton : stop the thread to get stream from aenc and send to adec
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StopTrdAencAdec(AENC_CHN AeChn, ADEC_CHN AdChn, FILE* pAecFd)
{
	SAMPLE_AENC_S* pstAenc = NULL;

	pstAenc = &gs_stSampleAenc[AeChn];

    if (TS_TRUE == pstAenc->bStart)
    {
        pstAenc->bStart = TS_FALSE;
        pthread_join(pstAenc->stAencPid, 0);
    }
    return TS_SUCCESS;
}

/******************************************************************************
* function : Create the thread to get stream from file and send to adec
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdFileAdec(ADEC_CHN AdChn, FILE* pAdcFd)
{
	SAMPLE_ADEC_S* pstAdec = NULL;

	if (NULL == pAdcFd)
	{
		return TS_FAILURE;
	}

	pstAdec = &gs_stSampleAdec[AdChn];
	pstAdec->AdChn = AdChn;
	pstAdec->pfd = pAdcFd;
	pstAdec->bStart = TS_TRUE;
	pthread_create(&pstAdec->stAdPid, 0, SAMPLE_COMM_AUDIO_AdecProc, pstAdec);

	return TS_SUCCESS;
}

/******************************************************************************
* funciton : stop the thread to get stream from file and send to adec
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StopTrdFileAdec(ADEC_CHN AdChn, FILE* pAdcFd)
{
	SAMPLE_ADEC_S* pstAdec = NULL;

	pstAdec = &gs_stSampleAdec[AdChn];

    if (TS_TRUE == pstAdec->bStart)
    {
        pstAdec->bStart = TS_FALSE;
        pthread_join(pstAdec->stAdPid, 0);
    }
	sleep(1);
    return TS_SUCCESS;
}

/******************************************************************************
* function : Start Ai
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StartAi(AUDIO_DEV AiDevId, TS_S32 s32AiChnCnt,
								 AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enOutSampleRate, TS_BOOL bEnableChn, TS_BOOL bResampleEn, TS_VOID* pstAiVqeAttr, TS_U32 u32AiVqeType)
{
	TS_S32 i;
	TS_S32 s32Ret;

	s32Ret = TS_MPI_AI_SetPubAttr(AiDevId, pstAioAttr);
	if (s32Ret)
	{
		printf("%s: TS_MPI_AI_SetPubAttr(%d) failed with %#x\n", __FUNCTION__, AiDevId, s32Ret);
		return s32Ret;
	}

	for (i = 0; i < s32AiChnCnt>>pstAioAttr->enSoundmode; i++)
	{
		if (NULL != pstAiVqeAttr)
		{
			TS_BOOL bAiVqe = TS_TRUE;
			switch (u32AiVqeType)
			{
				case 0:
					s32Ret = TS_SUCCESS;
					bAiVqe = TS_FALSE;
					break;
				case 1:
#if defined(VQE_WEBRTC) || defined(VQE_NN)
					s32Ret = TS_MPI_AI_SetTalkVqeAttr(AiDevId, i, (AUDIO_VQE_CONFIG_S *)pstAiVqeAttr);

					AUDIO_VQE_S aivqe;
					snprintf(aivqe.vqe_name,sizeof(aivqe.vqe_name),"%s","sol_vqe_nn");
					aivqe.vqe_open = ai_vqe_open;
					aivqe.vqe_set_proc_addr = ai_vqe_set_proc_addr;
					aivqe.vqe_process = ai_vqe_process;
					aivqe.vqe_close = ai_vqe_close;

					s32Ret = TS_MPI_AI_RegisteredVqe(AiDevId, i, &aivqe);
#endif
					break;
				default:
					s32Ret = TS_FAILURE;
					break;
			}

			if (s32Ret)
			{
				printf("%s: SetAiVqe%d(%d,%d) failed with %#x\n", __FUNCTION__, u32AiVqeType, AiDevId, i, s32Ret);
				return s32Ret;
			}

			if (bAiVqe)
			{
				s32Ret = TS_MPI_AI_EnableVqe(AiDevId, i);
				if (s32Ret)
				{
					printf("%s: TS_MPI_AI_EnableVqe(%d,%d) failed with %#x\n", __FUNCTION__, AiDevId, i, s32Ret);
					return s32Ret;
				}
			}
		}
	}

	if (g_raw_enable) {
		AI_CHN_ATTR_S pstChnAttr = { 0 };

		s32Ret = TS_MPI_AI_GetChnAttr(0, 0, &pstChnAttr);
		if (s32Ret) {
			printf("TS_MPI_AI_GetChnAttr bRawFrameEnable=%d\n", pstChnAttr.bRawFrameEnable);
		}
		pstChnAttr.bRawFrameEnable = 1;
		s32Ret = TS_MPI_AI_SetChnAttr(0, 0, &pstChnAttr);
		if (s32Ret) {
			printf("TS_MPI_AI_SetChnAttr bRawFrameEnable=%d\n", pstChnAttr.bRawFrameEnable);
		}
	}
	if (g_ref_enable) {
		s32Ret = TS_MPI_AI_EnableAecRefFrame(0, 0, 0, 0);
		if (s32Ret) {
			printf("TS_MPI_AI_EnableAecRefFrame(%d) failed\n", s32Ret);
		}
	}

	s32Ret = TS_MPI_AI_Enable(AiDevId);
	if (s32Ret)
	{
		printf("%s: TS_MPI_AI_Enable(%d) failed with %#x\n", __FUNCTION__, AiDevId, s32Ret);
		return s32Ret;
	}

	for (i = 0; i < s32AiChnCnt>>pstAioAttr->enSoundmode; i++)
	{
		if(bEnableChn)
		{
			s32Ret = TS_MPI_AI_EnableChn(AiDevId, i);
			if (s32Ret)
			{
				printf("%s: TS_MPI_AI_EnableChn(%d,%d) failed with %#x\n", __FUNCTION__, AiDevId, i, s32Ret);
				return s32Ret;
			}
		}
		
		if (TS_TRUE == bResampleEn)
		{
			//s32Ret = TS_MPI_AI_EnableReSmp(AiDevId, i, enOutSampleRate);
			//if (s32Ret)
			//{
			//	printf("%s: TS_MPI_AI_EnableReSmp(%d,%d) failed with %#x\n", __FUNCTION__, AiDevId, i, s32Ret);
			//	return s32Ret;
			//}
		}


	}

	return TS_SUCCESS;
}

/******************************************************************************
* function : Stop Ai
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StopAi(AUDIO_DEV AiDevId, TS_S32 s32AiChnCnt,  TS_BOOL bDisableAiChn, TS_BOOL bResampleEn, TS_BOOL bVqeEn)
{
	TS_S32 i;
	TS_S32 s32Ret;

	for (i = 0; i < s32AiChnCnt; i++)
	{
		if (TS_TRUE == bResampleEn)
		{
			//s32Ret = TS_MPI_AI_DisableReSmp(AiDevId, i);
			//if (TS_SUCCESS != s32Ret)
			//{
			//	printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
			//	return s32Ret;
			//}
		}

		if (TS_TRUE == bVqeEn)
		{
			s32Ret = TS_MPI_AI_DisableVqe(AiDevId, i);
			if (TS_SUCCESS != s32Ret)
			{
				printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
				return s32Ret;
			}
		}

		if (TS_TRUE == bDisableAiChn)
		{
			s32Ret = TS_MPI_AI_DisableChn(AiDevId, i);
			if (TS_SUCCESS != s32Ret)
			{
				printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
				return s32Ret;
			}
		}

		if (TS_TRUE == bVqeEn)
		{
			s32Ret = TS_MPI_AI_UnRegisteredVqe(AiDevId, i);
			if (TS_SUCCESS != s32Ret)
			{
				printf("TS_MPI_AI_UnRegisteredVqe failed\n");
				return s32Ret;
			}
			printf("TS_MPI_AI_UnRegisteredVqe ok!\n");
		}

	}

	s32Ret = TS_MPI_AI_Disable(AiDevId);
	if (TS_SUCCESS != s32Ret)
	{
		printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
		return s32Ret;
	}

	return TS_SUCCESS;
}

								
/******************************************************************************
* function : speaker enable/pa enable
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_SPEAKER_ENABLE(void)
{
#if 0
	int gpio_handle_fd;
	int gpio_value;
	struct gpiohandle_data data;

	/*enable codec speaker*/
	gpio_value = 0; /* gpioa11 is low, speaker work */
	//gpio_value = 1; /* gpioa11 is high, speaker not work */
	gpio_handle_fd = ts_gpio_get_handle(GPIOA_DEV, GPIOA11);
	if (gpio_handle_fd < 0) {
		printf("%s failed\n", __func__);
		return -1;
	}
	data.values[0] = gpio_value;
	ts_gpio_set_value(gpio_handle_fd, &data);
	ts_gpio_release_handle(gpio_handle_fd);
#endif

	system("/root/audio_speaker_ctrl_enable.sh");

	return TS_SUCCESS;
}

/******************************************************************************
* function : speaker disable/pa disable
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_SPEAKER_DISABLE(void)
{
#if 0
	int gpio_handle_fd;
	int gpio_value;
	struct gpiohandle_data data;

	/*disable codec speaker*/
	//gpio_value = 0; /* gpioa11 is low, speaker work */
	gpio_value = 1; /* gpioa11 is high, speaker not work */
	gpio_handle_fd = ts_gpio_get_handle(GPIOA_DEV, GPIOA11);
	if (gpio_handle_fd < 0) {
		printf("%s failed\n", __func__);
		return -1;
	}
	data.values[0] = gpio_value;
	ts_gpio_set_value(gpio_handle_fd, &data);
	ts_gpio_release_handle(gpio_handle_fd);
#endif

	system("/root/audio_speaker_ctrl_disable.sh");

	return TS_SUCCESS;
}


/******************************************************************************
* function : Start Ao
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StartAo(AUDIO_DEV AoDevId, TS_S32 s32AoChnCnt,
								 AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enInSampleRate, TS_BOOL bResampleEn, TS_VOID* pstAoVqeAttr, TS_U32 u32AoVqeType)
{
	TS_S32 i;
	TS_S32 s32Ret;

	s32Ret = TS_MPI_AO_SetPubAttr(AoDevId, pstAioAttr);
	if (TS_SUCCESS != s32Ret)
	{
		printf("%s: TS_MPI_AO_SetPubAttr(%d) failed with %#x!\n", __FUNCTION__, \
			   AoDevId, s32Ret);
		return TS_FAILURE;
	}

	s32Ret = TS_MPI_AO_Enable(AoDevId);
	if (TS_SUCCESS != s32Ret)
	{
		printf("%s: TS_MPI_AO_Enable(%d) failed with %#x!\n", __FUNCTION__, AoDevId, s32Ret);
		return TS_FAILURE;
	}

	printf("s32AoChnCnt=%d, enSoundmode=%d, res=%d\n", s32AoChnCnt, pstAioAttr->enSoundmode, s32AoChnCnt>>pstAioAttr->enSoundmode);
	for (i = 0; i < s32AoChnCnt>>pstAioAttr->enSoundmode; i++)
	{

		//if (TS_TRUE == bResampleEn)
		//{
		//	s32Ret = TS_MPI_AO_DisableReSmp(AoDevId, i);
		//	s32Ret |= TS_MPI_AO_EnableReSmp(AoDevId, i, enInSampleRate);
		//	if (TS_SUCCESS != s32Ret)
		//	{
		//		printf("%s: TS_MPI_AO_EnableReSmp(%d,%d) failed with %#x!\n", __FUNCTION__, AoDevId, i, s32Ret);
		//		return TS_FAILURE;
		//	}
		//}

		if (NULL != pstAoVqeAttr)
		{

			switch (u32AoVqeType)
			{
				case 0:
					s32Ret = TS_SUCCESS;
					break;
				case 1:
#if defined(VQE_WEBRTC) || defined(VQE_NN)
					s32Ret = TS_MPI_AO_SetTalkVqeAttr(AoDevId, i, (AUDIO_VQE_CONFIG_S *)pstAoVqeAttr);
					if (TS_SUCCESS != s32Ret) {
						printf("%s: TS_MPI_AO_SetTalkVqeAttr(%d) failed with %#x!\n", __FUNCTION__, i, s32Ret);
						return TS_FAILURE;
					}

					AUDIO_VQE_S aovqe;
					snprintf(aovqe.vqe_name,sizeof(aovqe.vqe_name),"%s","sol_vqe");
					aovqe.vqe_open = ao_vqe_open;
					aovqe.vqe_set_proc_addr = ao_vqe_set_proc_addr;
					aovqe.vqe_process = ao_vqe_process;
					aovqe.vqe_close = ao_vqe_close;

					s32Ret = TS_MPI_AO_RegisteredVqe(AoDevId, i, &aovqe);
					if (TS_SUCCESS != s32Ret) {
						printf("%s: TS_MPI_AO_RegisteredVqe(%d) failed with %#x!\n", __FUNCTION__, i, s32Ret);
						return TS_FAILURE;
					}
#endif
					break;
				default:
					s32Ret = TS_FAILURE;
					break;
			}

			if (s32Ret) {
				printf("%s: SetAoVqe%d(%d,%d) failed with %#x\n", __FUNCTION__, u32AoVqeType, AoDevId, i, s32Ret);
				return s32Ret;
			} else {
				printf("%s: SetAoVqe%d(%d,%d) ok %#x\n", __FUNCTION__, u32AoVqeType, AoDevId, i, s32Ret);
			}
		}
		s32Ret = TS_MPI_AO_EnableChn(AoDevId, i);
		if (TS_SUCCESS != s32Ret)
		{
			printf("%s: TS_MPI_AO_EnableChn(%d) failed with %#x!\n", __FUNCTION__, i, s32Ret);
			return TS_FAILURE;
		}
	}

	SAMPLE_COMM_AUDIO_SPEAKER_ENABLE();

	return TS_SUCCESS;
}


/******************************************************************************
* function : Stop Ao
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StopAo(AUDIO_DEV AoDevId, TS_S32 s32AoChnCnt, TS_BOOL bResampleEn)
{
	TS_S32 i;
	TS_S32 s32Ret;

	for (i = 0; i < s32AoChnCnt; i++)
	{
		if (TS_TRUE == bResampleEn)
		{
			//s32Ret = TS_MPI_AO_DisableReSmp(AoDevId, i);
			//if (TS_SUCCESS != s32Ret)
			//{
			//	printf("%s: TS_MPI_AO_DisableReSmp failed with %#x!\n", __FUNCTION__, s32Ret);
			//	return s32Ret;
			//}
		}

		s32Ret = TS_MPI_AO_DisableChn(AoDevId, i);
		if (TS_SUCCESS != s32Ret)
		{
			printf("%s: TS_MPI_AO_DisableChn failed with %#x!\n", __FUNCTION__, s32Ret);
			return s32Ret;
		}
		printf("TS_MPI_AO_UnRegisteredVqe ok!\n");

		s32Ret = TS_MPI_AO_UnRegisteredVqe(AoDevId, i);
		if (TS_SUCCESS != s32Ret)
		{
			printf("TS_MPI_AO_UnRegisteredVqe failed with %#x!\n", s32Ret);
			return s32Ret;
		}
	}

	SAMPLE_COMM_AUDIO_SPEAKER_DISABLE();

	s32Ret = TS_MPI_AO_Disable(AoDevId);
	if (TS_SUCCESS != s32Ret)
	{
		printf("%s: TS_MPI_AO_Disable failed with %#x!\n", __FUNCTION__, s32Ret);
		return s32Ret;
	}

	return TS_SUCCESS;
}

/******************************************************************************
* function : Start Aenc
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StartAenc(TS_S32 s32AencChnCnt, AIO_ATTR_S *pstAioAttr, PAYLOAD_TYPE_E enType)
{
	AENC_CHN AeChn;
	TS_S32 s32Ret, i;
	AENC_CHN_ATTR_S stAencAttr;

	/* set AENC chn attr */
	stAencAttr.enType = enType;
	stAencAttr.u32BufSize = 30;
	//stAencAttr.u32PtNumPerFrm = pstAioAttr->u32PtNumPerFrm;
	stAencAttr.u32PtNumPerFrm = pstAioAttr->u32PtNumPerFrm * (pstAioAttr->enSoundmode + 1) * pstAioAttr->enBitwidth / 8; //todo:liyi different from hi

	if (PT_ADPCMA == stAencAttr.enType)
	{
		stAencAttr.pValue = NULL;
	}
	else if (PT_G711A == stAencAttr.enType || PT_G711U == stAencAttr.enType)
	{
		stAencAttr.pValue = NULL;
	}
	else if (PT_G726 == stAencAttr.enType)
	{
		stAencAttr.pValue = NULL;
	}
	else if (PT_LPCM == stAencAttr.enType)
	{
		stAencAttr.pValue = NULL;
	}
	else if (PT_AAC == stAencAttr.enType)
	{
		stAencAttr.pValue = NULL;
	}
	else
	{
		printf("%s: invalid aenc payload type:%d\n", __FUNCTION__, stAencAttr.enType);
		return TS_FAILURE;
	}

	for (i = 0; i < s32AencChnCnt; i++)
	{
		AeChn = i;

		/* create aenc chn*/
		s32Ret = TS_MPI_AENC_CreateChn(AeChn, &stAencAttr);
		if (TS_SUCCESS != s32Ret)
		{
			printf("%s: TS_MPI_AENC_CreateChn(%d) failed with %#x!\n", __FUNCTION__,
				   AeChn, s32Ret);
			return s32Ret;
		}
	}

	return TS_SUCCESS;
}

/******************************************************************************
* function : Start Aenc Extern
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StartAencExtern(TS_S32 s32AencChnCnt, AIO_ATTR_S *pstAioAttr, PAYLOAD_TYPE_E enType)
{
	AENC_CHN AeChn = 0;
	TS_S32 s32Ret;
	TS_U32 u32SampleNum = 1024;
	AENC_CHN_ATTR_S stAencAttr = {
		.enType = enType,
		.u32BufSize = 30,
		.pValue = NULL };

	AENC_CHN_REGISTERED_T aenc_register = { 0 };

	switch (stAencAttr.enType) {
	case PT_OPUS: {
//#ifdef OPUS_CODEC
		aenc_register.aencInit = aenc_opus_init;
		aenc_register.aencExit = aenc_opus_exit;
		aenc_register.aencProcess = aenc_opus_process;
		u32SampleNum = 320;
		static OPUS_ENC_PARAM attr;
		attr.sample_rate = pstAioAttr->enSamplerate;
		attr.chan_num = s32AencChnCnt;
		attr.sample_num = pstAioAttr->u32PtNumPerFrm;
		attr.bps = 64000;
		stAencAttr.pValue = &attr;
//#endif
		break;
	}
	case PT_AAC: {
//#ifdef AAC_CODEC
		aenc_register.aencInit = aenc_aac_init;
		aenc_register.aencExit = aenc_aac_exit;
		aenc_register.aencProcess = aenc_aac_process;
		u32SampleNum = 1024;
		static attr_aac_aenc_t attr;
		attr.sample_rate = pstAioAttr->enSamplerate;
		stAencAttr.pValue = &attr;
//#endif
		break;
	}
	case PT_G726: {
//#ifdef G726_CODEC
		aenc_register.aencInit = aenc_g726_init;
		aenc_register.aencExit = aenc_g726_exit;
		aenc_register.aencProcess = aenc_g726_process;
		u32SampleNum = 480;
		static AENC_ATTR_G726_S attr = { .enG726bps = G726_16K };
		stAencAttr.pValue = &attr;
//#endif
		break;
	}
	case PT_ADPCMA: {
//#ifdef ADPCM_CODEC
		aenc_register.aencInit = aenc_adpcm_init;
		aenc_register.aencExit = aenc_adpcm_exit;
		aenc_register.aencProcess = aenc_adpcm_process;
		u32SampleNum = 480;
		static AENC_ATTR_ADPCM_S attr = { .enADPCMType = ADPCM_TYPE_DVI4 };
		stAencAttr.pValue = &attr;
//#endif
		break;
	}
	case PT_G711A:
	case PT_G711U: {
//#ifdef G711_CODEC
		aenc_register.aencInit = aenc_g711_init;
		aenc_register.aencExit = aenc_g711_exit;
		aenc_register.aencProcess = aenc_g711_process;
		u32SampleNum = 1024;
//#endif
		break;
	}
	case PT_LPCM: {
		aenc_register.aencInit = aenc_pcm_init;
		aenc_register.aencExit = aenc_pcm_exit;
		aenc_register.aencProcess = aenc_pcm_process;
		u32SampleNum = 1024;
		static AENC_ATTR_LPCM_S attr;
		stAencAttr.pValue = &attr;
		break;
	}
	default:
		PRINT("invalid aenc payload type:%d\n", stAencAttr.enType);
		return TS_FAILURE;
	}

	//todo:liyi different from hi
	stAencAttr.u32PtNumPerFrm = u32SampleNum * (pstAioAttr->enSoundmode + 1) * pstAioAttr->enBitwidth / 8;

	s32Ret = TS_MPI_AENC_RegisterEncoder(AeChn, &aenc_register);
	if (TS_SUCCESS != s32Ret)
	{
		printf("%s: TS_MPI_AENC_RegisterEncoder(%d) failed with %#x!\n", __FUNCTION__,
			   AeChn, s32Ret);
		return s32Ret;
	}

	/* create aenc chn*/
	s32Ret = TS_MPI_AENC_CreateChn(AeChn, &stAencAttr);
	if (TS_SUCCESS != s32Ret)
	{
		printf("%s: TS_MPI_AENC_CreateChn(%d) failed with %#x!\n", __FUNCTION__,
			   AeChn, s32Ret);
		return s32Ret;
	}

	return TS_SUCCESS;
}


/******************************************************************************
* function : Stop Aenc
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StopAenc(TS_S32 s32AencChnCnt)
{
	TS_S32 i;
	TS_S32 s32Ret;

	for (i = 0; i < s32AencChnCnt; i++)
	{
		s32Ret = TS_MPI_AENC_DestroyChn(i);
		if (TS_SUCCESS != s32Ret)
		{
			printf("%s: TS_MPI_AENC_DestroyChn(%d) failed with %#x!\n", __FUNCTION__,
				   i, s32Ret);
			return s32Ret;
		}

	}

	return TS_SUCCESS;
}

/******************************************************************************
* function : Start Adec
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StartAdec(ADEC_CHN AdChn, PAYLOAD_TYPE_E enType)
{
	TS_S32 s32Ret;
	ADEC_CHN_ATTR_S stAdecAttr;

	stAdecAttr.enType = enType;
	stAdecAttr.u32BufSize = 20;
	stAdecAttr.enMode = ADEC_MODE_STREAM;/* propose use pack mode in your app */

	if (PT_ADPCMA == stAdecAttr.enType)
	{
		stAdecAttr.pValue = NULL;
	}
	else if (PT_G711A == stAdecAttr.enType || PT_G711U == stAdecAttr.enType)
	{
		stAdecAttr.pValue = NULL;
	}
	else if (PT_G726 == stAdecAttr.enType)
	{
		stAdecAttr.pValue = NULL;
	}
	else if (PT_LPCM == stAdecAttr.enType)
	{
		stAdecAttr.pValue = NULL;
	}
	else if (PT_AAC == stAdecAttr.enType)
	{
		stAdecAttr.pValue = NULL;
	}
	else if (PT_MP3 == stAdecAttr.enType)
	{
		stAdecAttr.pValue = NULL;
	}
	else
	{
		printf("%s: invalid aenc payload type:%d\n", __FUNCTION__, stAdecAttr.enType);
		return TS_FAILURE;
	}

	/* create adec chn*/
	s32Ret = TS_MPI_ADEC_CreateChn(AdChn, &stAdecAttr);
	if (TS_SUCCESS != s32Ret)
	{
		printf("%s: TS_MPI_ADEC_CreateChn(%d) failed with %#x!\n", __FUNCTION__, \
			   AdChn, s32Ret);
		return s32Ret;
	}
	return 0;
}

/******************************************************************************
* function : Start Adec Extern
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StartAdecExtern(ADEC_CHN AdChn, PAYLOAD_TYPE_E enType)
{
	TS_S32 s32Ret;
	ADEC_CHN_ATTR_S stAdecAttr = {
		.enType = enType,
		.u32BufSize = 20,
		// ADEC_MODE_PACK ADEC_MODE_STREAM
		.enMode = ADEC_MODE_STREAM, /* propose use pack mode in your app */
		.pValue = NULL };

	ADEC_CHN_REGISTERED_T adec_register = { 0 };

	switch (stAdecAttr.enType) {
	case PT_OPUS: {
//#ifdef OPUS_CODEC
		adec_register.adecInit = adec_opus_init;
		adec_register.adecExit = adec_opus_exit;
		adec_register.adecProcess = adec_opus_process;
		adec_register.adecGetFrmInfo = adec_opus_get_frm_info;
		static OPUS_DEC_PARAM attr = { .sample_rate = AUDIO_SAMPLE_RATE_16000, .chan_num = 1 };
		attr.sample_rate = g_enSampleRate;
		stAdecAttr.pValue = &attr;
//#endif
		break;
	}
	case PT_AAC:
//#ifdef AAC_CODEC
		adec_register.adecInit = adec_aac_init;
		adec_register.adecExit = adec_aac_exit;
		adec_register.adecProcess = adec_aac_process;
		adec_register.adecGetFrmInfo = adec_aac_get_frm_info;
//#endif
		break;
	case PT_G726: {
//#ifdef G726_CODEC
		adec_register.adecInit = adec_g726_init;
		adec_register.adecExit = adec_g726_exit;
		adec_register.adecProcess = adec_g726_process;
		adec_register.adecGetFrmInfo = adec_g726_get_frm_info;
		static ADEC_ATTR_G726_S attr = { .enG726bps = G726_16K };
		stAdecAttr.pValue = &attr;
//#endif
		break;
	}
	case PT_ADPCMA: {
//#ifdef ADPCM_CODEC
		adec_register.adecInit = adec_adpcm_init;
		adec_register.adecExit = adec_adpcm_exit;
		adec_register.adecProcess = adec_adpcm_process;
		adec_register.adecGetFrmInfo = adec_adpcm_get_frm_info;
		static ADEC_ATTR_ADPCM_S attr = { .enADPCMType = ADPCM_TYPE_DVI4 };
		stAdecAttr.pValue = &attr;
//#endif
		break;
	}
	case PT_G711A:
	case PT_G711U:
//#ifdef G711_CODEC
		adec_register.adecInit = adec_g711_init;
		adec_register.adecExit = adec_g711_exit;
		adec_register.adecProcess = adec_g711_process;
		adec_register.adecGetFrmInfo = adec_g711_get_frm_info;
//#endif
		break;
	case PT_LPCM: {
		adec_register.adecInit = adec_pcm_init;
		adec_register.adecExit = adec_pcm_exit;
		adec_register.adecProcess = adec_pcm_process;
		adec_register.adecGetFrmInfo = adec_pcm_get_frm_info;
		static ADEC_ATTR_LPCM_S attr;
		stAdecAttr.pValue = &attr;
		break;
	}
	default:
		PRINT("invalid adec payload type:%d\n", stAdecAttr.enType);
		return TS_FAILURE;
	}

	TS_MPI_ADEC_RegisterDecoder(AdChn, &adec_register);

	/* create adec chn*/
	s32Ret = TS_MPI_ADEC_CreateChn(AdChn, &stAdecAttr);
	if (TS_SUCCESS != s32Ret)
	{
		printf("%s: TS_MPI_ADEC_CreateChn(%d) failed with %#x!\n", __FUNCTION__, \
			   AdChn, s32Ret);
		return s32Ret;
	}
	return 0;
}


/******************************************************************************
* function : Stop Adec
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StopAdec(ADEC_CHN AdChn)
{
	TS_S32 s32Ret;

	s32Ret = TS_MPI_ADEC_DestroyChn(AdChn);
	if (TS_SUCCESS != s32Ret)
	{
		printf("%s: TS_MPI_ADEC_DestroyChn(%d) failed with %#x!\n", __FUNCTION__,
			   AdChn, s32Ret);
		return s32Ret;
	}

	return TS_SUCCESS;
}

/******************************************************************************
* function : get frame from adec output, and save it to file.
******************************************************************************/
void* SAMPLE_COMM_AUDIO_GetAdecProc(void* parg)
{
	TS_S32 s32Ret;
	SAMPLE_ADEC_S* pstAdecCtl = (SAMPLE_ADEC_S*)parg;
	AUDIO_FRAME_INFO_S *stFrameInfo = malloc(sizeof(AUDIO_FRAME_INFO_S));
	AUDIO_FRAME_S *audioframe = malloc(sizeof(AUDIO_FRAME_S));
	stFrameInfo->pstFrame = audioframe;

	while (pstAdecCtl->bStart) {
		s32Ret = TS_MPI_ADEC_GetFrame(0, stFrameInfo, 1);
		if (TS_SUCCESS != s32Ret )
		{
			printf("TS_MPI_ADEC_GetFrame failed\n");
			continue;
		}
		//printf("TS_MPI_ADEC_GetFrame begin len=%d, addr=%p\n", stFrameInfo->pstFrame->u32Len, stFrameInfo->pstFrame->u64VirAddr[0]);
		test_save_pcm(stFrameInfo->pstFrame);
		/* finally you must release the stream */
		s32Ret = TS_MPI_ADEC_ReleaseFrame(0, stFrameInfo);
		if (TS_SUCCESS != s32Ret )
		{
			printf("TS_MPI_AI_ReleaseFrame failed \n");
			return NULL;
		}
	}
	if (stFrameInfo)
		free(stFrameInfo);
	if (audioframe)
		free(audioframe);
	stFrameInfo = NULL;
	audioframe = NULL;
	pstAdecCtl->bStart = TS_FALSE;
	return NULL;
}

/******************************************************************************
* funciton : stop the thread to get frame from adec
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StopTrdGetAdec(ADEC_CHN AdChn)
{
	SAMPLE_ADEC_S* pstAdec = NULL;

	pstAdec = &gs_stSampleGetAdec[AdChn];

    if (TS_TRUE == pstAdec->bStart)
    {
        pstAdec->bStart = TS_FALSE;
        pthread_join(pstAdec->stAdPid, 0);
    }
    return TS_SUCCESS;
}

/******************************************************************************
* funciton :  Create the thread to get decoded frame from adec
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdGetAdec(ADEC_CHN AdChn)
{
	SAMPLE_ADEC_S* pstAdec = NULL;

	pstAdec = &gs_stSampleGetAdec[AdChn];
	pstAdec->bStart = TS_TRUE;

	pthread_create(&pstAdec->stAdPid, 0, SAMPLE_COMM_AUDIO_GetAdecProc, pstAdec);

	return TS_SUCCESS;
}

/******************************************************************************
* function : send pcm data to aenc
******************************************************************************/
void* SAMPLE_COMM_AUDIO_AencSendProc(void* parg)
{
    TS_S32 s32Ret;
	AUDIO_FRAME_S stAudioFrame;
    TS_U32 u32Len = 320;
    TS_U32 u32ReadLen;
    TS_U8* pu8AudioStream = NULL;
    SAMPLE_AENC_S* pstAencCtl = (SAMPLE_AENC_S*)parg;

    TS_S32 s32AencChn = pstAencCtl->AeChn;


	FILE *pfd;
	TS_CHAR aszFileName[100] = { 0 };

	/* create file for save stream*/
	snprintf(aszFileName, FILE_NAME_LEN, ai_cap_store);

	pfd = fopen(aszFileName, "rb");
	if (NULL == pfd) {
		printf("%s: open file %s failed\n", __FUNCTION__, aszFileName);
		return NULL;
	}

    pu8AudioStream = (TS_U8*)malloc(sizeof(TS_U8) * MAX_AUDIO_STREAM_LEN);
    if (NULL == pu8AudioStream)
    {
        printf("%s: malloc failed!\n", __FUNCTION__);
        return NULL;
    }

    while (pstAencCtl->bStart)
    {
        /* read from file */
        stAudioFrame.u64VirAddr[0] = pu8AudioStream;
		stAudioFrame.u32Len = u32Len;

        u32ReadLen = fread(stAudioFrame.u64VirAddr[0], 1, u32Len, pfd);
        if (u32ReadLen <= 0)
        {
            (TS_VOID)fseek(pfd, 0, SEEK_SET);/*read file again*/
			sleep(1);
            continue;
        }

        /* here only demo adec streaming sending mode, but pack sending mode is commended */
        stAudioFrame.u32Len = u32ReadLen;
        s32Ret = TS_MPI_AENC_SendFrame(s32AencChn, &stAudioFrame);
        if (TS_SUCCESS != s32Ret)
        {
            printf("%s: TS_MPI_Aenc_SendStream(%d) failed with %#x!\n", \
                   __FUNCTION__, s32AencChn, s32Ret);
            break;
        }
    }

    free(pu8AudioStream);
    pu8AudioStream = NULL;
    fclose(pfd);
    pstAencCtl->bStart = TS_FALSE;
    return NULL;
}

/******************************************************************************
* function : Create the thread to send frame to aenc
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdSendAenc(AENC_CHN AeChn)
{
	SAMPLE_AENC_S* pstAenc = NULL;

	//if (NULL == pAdcFd)
	//{
	//	return TS_FAILURE;
	//}

	pstAenc = &gs_stSampleSendAenc[AeChn];
	pstAenc->AeChn = AeChn;
	pstAenc->bStart = TS_TRUE;
	pthread_create(&pstAenc->stAencPid, 0, SAMPLE_COMM_AUDIO_AencSendProc, pstAenc);

	return TS_SUCCESS;
}

/******************************************************************************
* funciton : stop the thread to send frame to aenc
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StopTrdSendAenc(AENC_CHN AeChn)
{
	SAMPLE_AENC_S* pstAenc = NULL;

	pstAenc = &gs_stSampleSendAenc[AeChn];

    if (TS_TRUE == pstAenc->bStart)
    {
        pstAenc->bStart = TS_FALSE;
        pthread_join(pstAenc->stAencPid, 0);
    }
    return TS_SUCCESS;
}

/***************************************************************************************
* function : Create the thread to get stream from aenc and push stream to stream server
****************************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAencPushStream(AENC_CHN AeChn)
{
	SAMPLE_AENC_S* pstAenc = NULL;

	pstAenc = &gs_stSampleAenc[AeChn];
	pstAenc->AeChn = AeChn;
	pstAenc->bSendAdChn = TS_FALSE;
	pstAenc->bSendStream = TS_TRUE;
	pstAenc->bStart = TS_TRUE;
	pthread_create(&pstAenc->stAencPid, 0, SAMPLE_COMM_AUDIO_AencProc, pstAenc);

	return TS_SUCCESS;
}

/******************************************************************************
* function : Create the thread to pull stream and send to adec
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdPullStreamAdec(ADEC_CHN AdChn)
{
	SAMPLE_ADEC_S* pstAdec = NULL;

	pstAdec = &gs_stSampleAdec[AdChn];
	pstAdec->AdChn = AdChn;
	pstAdec->bStart = TS_TRUE;
	pstAdec->bPullStream = TS_TRUE;
	pthread_create(&pstAdec->stAdPid, 0, SAMPLE_COMM_AUDIO_AdecProc, pstAdec);

	return TS_SUCCESS;
}

/*************************************************************************************
* function : Create the thread to get stream from ai and push stream to stream server
**************************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAiPushStream(AUDIO_DEV AiDev, AI_CHN AiChn)
{
    SAMPLE_AI_S* pstAi = NULL;

    pstAi = &gs_stSampleAi[AiDev * AI_MAX_CHN_NUM + AiChn];
    pstAi->bSendAenc = TS_FALSE;
    pstAi->bSendAo = TS_FALSE;
    pstAi->bStart = TS_TRUE;
    pstAi->AiDev = AiDev;
    pstAi->AiChn = AiChn;
    pstAi->bSendStream = TS_TRUE;
    pthread_create(&pstAi->stAiPid, 0, SAMPLE_COMM_AUDIO_AiProc, pstAi);

    return TS_SUCCESS;
}

/******************************************************************************
* function : Create the thread to pull stream and send to ao
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdPullStreamAo(AUDIO_DEV AoDev, AO_CHN AoChn)
{
	SAMPLE_AO_S* pstAo = NULL;

	pstAo = &gs_stSampleAo[AoDev * AI_MAX_CHN_NUM + AoChn];
	pstAo->bStart = TS_TRUE;
	pstAo->bPullStream = TS_TRUE;
	pstAo->AoDev = AoDev;
	pstAo->AoChn = AoChn;

	pthread_create(&pstAo->stAoPid, 0, SAMPLE_COMM_AUDIO_AoProc, pstAo);

	return TS_SUCCESS;
}
