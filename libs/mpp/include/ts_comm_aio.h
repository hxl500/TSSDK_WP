/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_comm_aio.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __TS_COMM_AIO_H__
#define __TS_COMM_AIO_H__

#include "ts_common.h"
#include "ts_errno.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


#define MAX_AUDIO_FRAME_NUM    300       /*max count of audio frame in Buffer */
#define MAX_AUDIO_POINT_BYTES  4        /*max bytes of one sample point(now 32bit max)*/

#define MAX_VOICE_POINT_NUM    480      /*max sample per frame for voice encode */

#define MAX_AUDIO_POINT_NUM    2048     /*max sample per frame for all encoder(aacplus:2048)*/
#define MAX_AO_POINT_NUM       4096     /* from h3;support 4096 framelen*/
#define MIN_AUDIO_POINT_NUM    80       /*min sample per frame*/
#define MAX_AI_POINT_NUM    2048     /*max sample per frame for all encoder(aacplus:2048)*/

/*max length of audio frame by bytes, one frame contain many sample point */
#define MAX_AUDIO_FRAME_LEN    (MAX_AUDIO_POINT_BYTES*MAX_AO_POINT_NUM)

/*max length of audio stream by bytes */
#define MAX_AUDIO_STREAM_LEN   MAX_AUDIO_FRAME_LEN

#define MAX_AI_USRFRM_DEPTH     30      /*max depth of user frame buf */

#define MAX_AUDIO_FILE_PATH_LEN	256
#define MAX_AUDIO_FILE_NAME_LEN	256

/*The VQE EQ Band num.*/
#define VQE_EQ_BAND_NUM  10
#define VQE_DRC_SECNUM	5

#define AI_RECORDVQE_MASK_HPF		0x1
#define AI_RECORDVQE_MASK_RNR		0x2
#define AI_RECORDVQE_MASK_HDR		0x4
#define AI_RECORDVQE_MASK_DRC		0x8
#define AI_RECORDVQE_MASK_EQ		0x10
#define AI_RECORDVQE_MASK_AGC		0x20

#define AI_TALKVQE_MASK_HPF		0x1
#define AI_TALKVQE_MASK_AEC		0x2
#define AI_TALKVQE_MASK_AGC		0x8
#define AI_TALKVQE_MASK_EQ		0x10
#define AI_TALKVQE_MASK_ANR		0x20

#define AO_VQE_MASK_HPF			0x1
#define AO_VQE_MASK_ANR			0x2
#define AO_VQE_MASK_AGC			0x4
#define AO_VQE_MASK_EQ			0x8


typedef enum tsAUDIO_SAMPLE_RATE_E
{
    AUDIO_SAMPLE_RATE_8000   = 8000,    /* 8K samplerate*/
    AUDIO_SAMPLE_RATE_12000  = 12000,   /* 12K samplerate*/
    AUDIO_SAMPLE_RATE_11025  = 11025,   /* 11.025K samplerate*/
    AUDIO_SAMPLE_RATE_16000  = 16000,   /* 16K samplerate*/
    AUDIO_SAMPLE_RATE_22050  = 22050,   /* 22.050K samplerate*/
    AUDIO_SAMPLE_RATE_24000  = 24000,   /* 24K samplerate*/
    AUDIO_SAMPLE_RATE_32000  = 32000,   /* 32K samplerate*/
    AUDIO_SAMPLE_RATE_44100  = 44100,   /* 44.1K samplerate*/
    AUDIO_SAMPLE_RATE_48000  = 48000,   /* 48K samplerate*/
    AUDIO_SAMPLE_RATE_64000  = 64000,   /* 64K samplerate*/
    AUDIO_SAMPLE_RATE_96000  = 96000,   /* 96K samplerate*/
    AUDIO_SAMPLE_RATE_BUTT,
} AUDIO_SAMPLE_RATE_E;

typedef enum tsAUDIO_BIT_WIDTH_E
{
    AUDIO_BIT_WIDTH_8   = 8,   /* 8bit width */
    AUDIO_BIT_WIDTH_16  = 16,   /* 16bit width*/
    AUDIO_BIT_WIDTH_24  = 24,   /* 24bit width*/
    AUDIO_BIT_WIDTH_BUTT = 100,
} AUDIO_BIT_WIDTH_E;

typedef enum tsAIO_MODE_E
{
    AIO_MODE_I2S_MASTER  = 0,   /* AIO I2S master mode */
    AIO_MODE_I2S_SLAVE,         /* AIO I2S slave mode */
    AIO_MODE_PCM_SLAVE_STD,     /* AIO PCM slave standard mode */
    AIO_MODE_PCM_SLAVE_NSTD,    /* AIO PCM slave non-standard mode */
    AIO_MODE_PCM_MASTER_STD,    /* AIO PCM master standard mode */
    AIO_MODE_PCM_MASTER_NSTD,   /* AIO PCM master non-standard mode */
    AIO_MODE_BUTT
} AIO_MODE_E;

typedef enum
{
    AIO_I2STYPE_INNERCODEC = 0, /* AIO I2S connect inner audio CODEC */
    AIO_I2STYPE_INNERHDMI,      /* AIO I2S connect Inner HDMI */
    AIO_I2STYPE_EXTERN,         /* AIO I2S connect extern hardware */
} AIO_I2STYPE_E;

typedef enum tsAIO_SOUND_MODE_E
{
    AUDIO_SOUND_MODE_MONO   =0,/*mono*/
    AUDIO_SOUND_MODE_STEREO =1,/*stereo*/
    AUDIO_SOUND_MODE_BUTT
} AUDIO_SOUND_MODE_E;

/*
An example of the packing scheme for G726-32 codewords is as shown, and bit A3 is the least significant bit of the first codeword:
RTP G726-32:
0                   1
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
|B B B B|A A A A|D D D D|C C C C| ...
|0 1 2 3|0 1 2 3|0 1 2 3|0 1 2 3|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

MEDIA G726-32:
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
|A A A A|B B B B|C C C C|D D D D| ...
|3 2 1 0|3 2 1 0|3 2 1 0|3 2 1 0|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
*/
typedef enum tsG726_BPS_E
{
    G726_16K = 0,       /* G726 16kbps, see RFC3551.txt  4.5.4 G726-16 */
    G726_24K,           /* G726 24kbps, see RFC3551.txt  4.5.4 G726-24 */
    G726_32K,           /* G726 32kbps, see RFC3551.txt  4.5.4 G726-32 */
    G726_40K,           /* G726 40kbps, see RFC3551.txt  4.5.4 G726-40 */
    MEDIA_G726_16K,     /* G726 16kbps for ASF ... */
    MEDIA_G726_24K,     /* G726 24kbps for ASF ... */
    MEDIA_G726_32K,     /* G726 32kbps for ASF ... */
    MEDIA_G726_40K,     /* G726 40kbps for ASF ... */
    G726_BUTT,
} G726_BPS_E;

typedef enum tsADPCM_TYPE_E
{
    /* see DVI4 diiffers in three respects from the IMA ADPCM at RFC3551.txt 4.5.1 DVI4 */

    ADPCM_TYPE_DVI4 = 0,    /* 32kbps ADPCM(DVI4) for RTP */
    ADPCM_TYPE_IMA,         /* 32kbps ADPCM(IMA),NOTICE:point num must be 161/241/321/481 */
    ADPCM_TYPE_ORG_DVI4,
    ADPCM_TYPE_BUTT,
} ADPCM_TYPE_E;

#define AI_EXPAND  0x01
#define AI_CUT     0x02

typedef enum tsAIO_MODE_S {
	AIO_INNER_CODEC = 0, // internal codec only support mono
	AIO_INNER_CODEC_DUAL = 1, // internal codec dual mic nr and one channel output in some chip platform
	AIO_EXTERN_CODEC = 2, // external codec support mono and dual mic record
	AIO_EXTERN_CODEC_DUAL = 3, // external codec enable dual mic nr and one channel output
	AIO_PDM = 4, // pdm mic support mono and dual mic record
	AIO_PDM_DUAL = 5, // pdm mic enable dual mic nr and one channel output
} AIO_MODE_S;

typedef struct tsAIO_ATTR_S {
	TS_U8 				u8CardNum; 		/*sound card num*/
	TS_U8 				u8DeviceNum; 	/*sound device num*/
	AUDIO_SAMPLE_RATE_E enSamplerate; 	/*sample rate*/
	AUDIO_BIT_WIDTH_E 	enBitwidth; 	/*bitwidth*/
	AUDIO_SOUND_MODE_E 	enSoundmode; 	/*momo or stereo*/
	TS_U32 				u32FrmNum; 		/*frame num in buffer*/
	TS_U32 				u32PtNumPerFrm; /*number of samples*/
	AIO_MODE_S 			aioMode; /*aio mode*/
	TS_BOOL 			aecEnable; /*aec enable flag*/
} AIO_ATTR_S;

typedef struct tsAI_CHN_PARAM_S
{
    TS_U32 u32UsrFrmDepth;
} AI_CHN_PARAM_S;

typedef struct tsAI_CHN_ATTR_S
{
    TS_BOOL bRawFrameEnable; /* whether raw frame is valid */
} AI_CHN_ATTR_S;

//todo:liyi wait platform modify common omx struct
#if 0
typedef struct tsAUDIO_FRAME_S
{
    AUDIO_BIT_WIDTH_E   enBitwidth;     /*audio frame bitwidth*/
    AUDIO_SOUND_MODE_E  enSoundmode;    /*audio frame momo or stereo mode*/
    TS_U8*  u64VirAddr[2];
    TS_U64  u64PhyAddr[2];
    TS_U64  u64TimeStamp;                /*audio frame timestamp*/
    TS_U32  u32Seq;                      /*audio frame seq*/
    TS_U32  u32Len;                      /*data lenth per channel in frame*/
    TS_U32  u32PoolId[2];
} AUDIO_FRAME_S;
#else
typedef struct tsAUDIO_FRAME_S
{
    AUDIO_BIT_WIDTH_E   enBitwidth;     /*audio frame bitwidth*/
    AUDIO_SOUND_MODE_E  enSoundmode;    /*audio frame momo or stereo mode*/
    TS_VOID *u64VirAddr[2];
    TS_U32  u64PhyAddr[2];
    TS_U64  u64TimeStamp;                /*audio frame timestamp*/
    TS_U32  u32Seq;                      /*audio frame seq*/
    TS_U32  u32Len;                      /*data lenth per channel in frame*/
    TS_U32  u32PoolId[2];
} AUDIO_FRAME_S;

#endif

typedef struct tsAEC_FRAME_S
{
    AUDIO_FRAME_S   stRefFrame;    /* AEC reference audio frame */
    TS_BOOL         bValid;        /* whether frame is valid */
	TS_BOOL         bSysBind;       /* whether is sysbind */
} AEC_FRAME_S;


typedef struct tsAUDIO_FRAME_INFO_S
{
    AUDIO_FRAME_S *pstFrame;/*frame ptr*/
    TS_U32         u32Id;   /*frame id*/
} AUDIO_FRAME_INFO_S;

//todo:liyi wait platform modify common omx struct
#if 0
typedef struct tsAUDIO_STREAM_S
{
    TS_U8 ATTRIBUTE *pStream;         /* the virtual address of stream */
    TS_U64 ATTRIBUTE u64PhyAddr;      /* the physics address of stream */
    TS_U32 u32Len;          /* stream lenth, by bytes */
    TS_U64 u64TimeStamp;    /* frame time stamp*/
    TS_U32 u32Seq;          /* frame seq,if stream is not a valid frame,u32Seq is 0*/
} AUDIO_STREAM_S;
#else
typedef struct tsAUDIO_STREAM_S
{
    TS_U8 *pStream;         /* the virtual address of stream */
    TS_U32 u64PhyAddr;      /* the physics address of stream */
    TS_U32 u32Len;          /* stream lenth, by bytes */
    TS_U64 u64TimeStamp;    /* frame time stamp*/
    TS_U32 u32Seq;          /* frame seq,if stream is not a valid frame,u32Seq is 0*/
} AUDIO_STREAM_S;
#endif

typedef struct tsAO_CHN_STATE_S
{
    TS_U32                  u32ChnTotalNum;    /* total number of channel buffer */
    TS_U32                  u32ChnFreeNum;     /* free number of channel buffer */
    TS_U32                  u32ChnBusyNum;     /* busy number of channel buffer */
} AO_CHN_STATE_S;

typedef enum tsAUDIO_TRACK_MODE_E
{
    AUDIO_TRACK_NORMAL      = 0,
    AUDIO_TRACK_BOTH_LEFT   = 1,
    AUDIO_TRACK_BOTH_RIGHT  = 2,
    AUDIO_TRACK_EXCHANGE    = 3,
    AUDIO_TRACK_MIX         = 4,
    AUDIO_TRACK_LEFT_MUTE   = 5,
    AUDIO_TRACK_RIGHT_MUTE  = 6,
    AUDIO_TRACK_BOTH_MUTE   = 7,

    AUDIO_TRACK_BUTT
} AUDIO_TRACK_MODE_E;


typedef enum tsAUDIO_FADE_RATE_E
{
    AUDIO_FADE_RATE_1	= 0,
    AUDIO_FADE_RATE_2	= 1,
    AUDIO_FADE_RATE_4	= 2,
    AUDIO_FADE_RATE_8   = 3,
    AUDIO_FADE_RATE_16  = 4,
    AUDIO_FADE_RATE_32  = 5,
    AUDIO_FADE_RATE_64  = 6,
    AUDIO_FADE_RATE_128 = 7,

    AUDIO_FADE_RATE_BUTT
} AUDIO_FADE_RATE_E;

typedef struct tsAUDIO_FADE_S
{
    TS_BOOL         bFade;
    AUDIO_FADE_RATE_E enFadeInRate;
    AUDIO_FADE_RATE_E enFadeOutRate;
} AUDIO_FADE_S;

typedef enum tsAUDIO_AEC_NLP_MODE_E
{
  AUDIO_AEC_NLP_CONSERVATIVE = 0,
  AUDIO_AEC_NLP_MODERATE,
  AUDIO_AEC_NLP_AGGRESSIVE,
  AUDIO_AEC_NLP_BUTT
} AUDIO_AEC_NLP_MODE_E;

typedef struct tsALG_AEC_INIT_S
{
    /**
     * @brief sampling ratio: 8000, 16000, 32000
     */
    TS_S32 s32SampFreq;
    /**
     * @brief sampling ratio of sound card: [1, 96000]
     */
    TS_S32 s32ScSampFreq;
}ALG_AEC_INIT_S;

typedef struct tsALG_AEC_PARAM_S
{
    /**
     * @brief AEC_NLP_CONSERVATIVE(default), AEC_NLP_MODERATE, AEC_NLP_AGGRESSIVE
     */
    TS_S16 s16NlpMode;
    /**
     * @brief AEC_FALSE(default)/AEC_TRUE
     */
    TS_S16 s16SkewMode;
    /**
     * @brief AEC_FALSE(default)/AEC_TRUE
     */
    TS_S16 s16MetricsMode;
    /**
     * @brief AEC_FALSE(default)/AEC_TRUE
     */
    TS_S32 s32DelayLogging;
} ALG_AEC_PARAM_S;

/**Defines the configure parameters of AEC.*/
typedef struct tsAI_AEC_CONFIG_S
{
	ALG_AEC_INIT_S 	stAecAlgInit;
    ALG_AEC_PARAM_S stAecAlgCfg;
    /**
     * @brief Delay estimate for sound card and system buffers, unit: ms
     */
    TS_S16 s16AecMsInSndCardBuf;
    /**
     * @brief Difference between number of samples played and recorded at the soundcard (for clock skew compensation)
     */
    TS_S32 s32Skew;
} AI_AEC_CONFIG_S;

typedef struct tsALG_ANR_INIT_S{
    /**
     * @brief sampling ratio
     */
	TS_U32 u32Fs;
} ALG_ANR_INIT_S;

typedef struct tsALG_ANR_PARAM_S
{
    /**
     * @brief 0: Mild, 1: Medium , 2: Aggressive, 3:severe
     */
	TS_S32  s32Mode; // default:0
} ALG_ANR_PARAM_S;

/**Defines the configure parameters of ANR.*/
typedef struct tsAUDIO_ANR_CONFIG_S
{
	ALG_ANR_INIT_S  stAnrAlgInit;
    ALG_ANR_PARAM_S stAnrAlgCfg;
} AUDIO_ANR_CONFIG_S;

typedef enum tsAUDIO_AGC_MODE_E
{
    AUDIO_AGC_MODE_UNCHANGED,
    AUDIO_AGC_MODE_ADAPTIVE_ANALOG,
    AUDIO_AGC_MODE_ADAPTIVE_DIGITAL,
    AUDIO_AGC_MODE_FIXED_DIGITAL
} AUDIO_AGC_MODE_E;

typedef struct tsALG_AGC_INIT_S
{
    /**
	 * @brief frame len: 8K:80/160/128/256; 16K:160/320/256/512
	 */
	TS_S16 Frame_Len;
    /**
     * @brief _AGC_MODE_UNCHANGED,
     * _AGC_MODE_ADAPTIVE_ANALOG: Adaptive Analog Automatic Gain Control [-targetLevelDbfs (default -3 dBOv)]
     * _AGC_MODE_ADAPTIVE_DIGITAL: Adaptive Digital Automatic Gain Control [-targetLevelDbfs (default -3 dBOv)]
     * _AGC_MODE_FIXED_DIGITAL: Fixed Digital Gain 0dB [compressionGaindB (default 8 dB)]
     */
    TS_S16 s16AgcMode;
    /**
     * @brief Sampling ratio
     */
    TS_U32 u32Fs;
}ALG_AGC_INIT_S;

typedef struct tsALG_AGC_PARAM_S
{
    /**
     * @brief default 3 (-3 dBOv)
     */
    TS_S16 s16TargetLevelDbFs;
    /**
     * @brief default 9 dB
     */
    TS_S16 s16CompressionGainDb;
    /**
     * @brief default kAgcTrue (on)
     */
    TS_U8 u8LimiterEnable;
} ALG_AGC_PARAM_S;

/**Defines the configure parameters of AGC.*/
typedef struct tsAUDIO_AGC_CONFIG_S
{
	ALG_AGC_INIT_S  stAgcAlgInit;
    ALG_AGC_PARAM_S stAgcAlgCfg;
    TS_S32 s32PreInmicLevel;
} AUDIO_AGC_CONFIG_S;

/**Defines the configure parameters of HPF.*/
typedef enum tsAUDIO_HPF_FREQ_E
{
    AUDIO_HPF_FREQ_80   = 80,    /* 80Hz */
    AUDIO_HPF_FREQ_120  = 120,   /* 120Hz */
    AUDIO_HPF_FREQ_150  = 150,   /* 150Hz */
    AUDIO_HPF_FREQ_BUTT,
} AUDIO_HPF_FREQ_E;

typedef struct tsAUDIO_HPF_CONFIG_S
{
	// unsupport
} AUDIO_HPF_CONFIG_S;

typedef struct tsAI_RNR_CONFIG_S
{
    TS_BOOL bUsrMode;                /* mode 0: auto, mode 1: mannual.*/

    TS_S32  s32NrMode;               /*mode 0: floor noise; 1:ambient noise */

    TS_S32 s32MaxNrLevel;           /*max NR level range:[2,20]dB*/

    TS_S32  s32NoiseThresh;         /*noise threshold, range:[-80, -20]*/
} AI_RNR_CONFIG_S;

typedef struct tsAUDIO_EQ_CONFIG_S
{
	// unsupport
} AUDIO_EQ_CONFIG_S;


/**Defines the configure parameters of UPVQE work state.*/
typedef enum tsVQE_WORKSTATE_E
{
    VQE_WORKSTATE_COMMON  = 0,   /* common environment, Applicable to the family of voice calls. */
    VQE_WORKSTATE_MUSIC   = 1,   /* music environment , Applicable to the family of music environment. */
    VQE_WORKSTATE_NOISY   = 2,   /* noisy environment , Applicable to the noisy voice calls.  */
} VQE_WORKSTATE_E;

/**Defines record type*/
typedef enum tsVQE_RECORD_TYPE
{
    VQE_RECORD_NORMAL        = 0,  /*<double micphone recording. */
    VQE_RECORD_BUTT,
} VQE_RECORD_TYPE;


/* HDR Set CODEC GAIN Function Handle type */
typedef TS_S32 (*pFuncGainCallBack)(TS_S32 s32SetGain);

typedef struct tsAI_HDR_CONFIG_S
{
    TS_BOOL bUsrMode;               /* mode 0: auto mode 1: mannual.*/

    TS_S32 s32MinGaindB;            /* the minimum of MIC(AI) CODEC gain, [0, 120]*/
    TS_S32 s32MaxGaindB;            /* the maximum of MIC(AI) CODEC gain, [0, 120]*/

    TS_S32 s32MicGaindB;            /* the current gain of MIC(AI) CODEC,[s32MinGaindB, s32MaxGaindB]*/
    TS_S32 s32MicGainStepdB;        /* the step size of gain adjustment, [1, 3], recommemd 2 */
    pFuncGainCallBack pcallback;    /* the callback function pointer of CODEC gain adjustment */
} AI_HDR_CONFIG_S;

typedef struct tsAI_DRC_CONFIG_S
{
    TS_BOOL bUsrMode;   /* enable user mode or not,default 0: disable user mode,1: user mode.*/

    TS_S16  s16AttackTime;   /*time of signal change from large to small (ms), range:HiFiVqe[10, 250]ms, RecordVqe[10, 126]ms*/
    TS_S16  s16ReleaseTime;  /*time of signal change from small to large (ms), range:HiFiVqe[10, 250]ms, RecordVqe[10, 126]ms*/

    TS_S16 s16OldLevDb[VQE_DRC_SECNUM];  /*old curve level(dB) ,default[0, -472, -792, -960, -1280],range:[-1440, 0]ms,store from big to small,scale:Q4*/
    TS_S16 s16NewLevDb[VQE_DRC_SECNUM];  /*new curve level(dB) ,default[0, -174, -410, -608, -1021],range:[-1440, 0]ms,store from big to small,scale:Q4*/
} AI_DRC_CONFIG_S;

/**Defines the configure parameters of Talk VQE.*/
typedef struct tsAI_TALKVQE_CONFIG_S
{
    TS_U32				u32OpenMask;

    TS_S32              s32WorkSampleRate;  /* Sample Rate: 8KHz/16KHz. default: 8KHz*/
    TS_S32              s32FrameSample; /* VQE frame length: 80-4096 */
    VQE_WORKSTATE_E     enWorkstate;

    AUDIO_HPF_CONFIG_S  stHpfCfg;
 	AI_AEC_CONFIG_S     stAecCfg;
    AUDIO_ANR_CONFIG_S  stAnrCfg;
    AUDIO_AGC_CONFIG_S  stAgcCfg;
    AUDIO_EQ_CONFIG_S   stEqCfg;
} AI_TALKVQE_CONFIG_S;

typedef struct tsAO_VQE_CONFIG_S
{
    TS_U32				u32OpenMask;

    TS_S32              s32WorkSampleRate;  /* Sample Rate: 8KHz/16KHz/48KHz. default: 8KHz*/
    TS_S32              s32FrameSample; /* VQE frame length: 80-4096 */
    VQE_WORKSTATE_E     enWorkstate;

    AUDIO_HPF_CONFIG_S stHpfCfg;
    AUDIO_ANR_CONFIG_S stAnrCfg;
    AUDIO_AGC_CONFIG_S stAgcCfg;
    AUDIO_EQ_CONFIG_S  stEqCfg;
} AO_VQE_CONFIG_S;

typedef struct tsMIXER_CONFIG_S {
	ts_u32 num_id; /* mixer number id */
	ts_s32 value; /* get or set value for number id */
} MIXER_CONFIG_S;

typedef struct tsAUDIO_VQE_S {
	ts_char vqe_name[17];
	ts_s32  (*vqe_open)(ts_void **handle, ts_void *attr);
	ts_s32  (*vqe_set_proc_addr)(ts_void *handle, ts_void *addr);
	ts_s32  (*vqe_process)(ts_void *handle, ts_u8 *in_buf, ts_u8 *ref_buf, ts_u32 *in_len, ts_u8 *out_buf, ts_u32 *out_len);
	ts_s32  (*vqe_close)(ts_void *handle);
} AUDIO_VQE_S;

typedef struct tsAUDIO_VQE_CONFIG_S
{
	ts_s32 in_len;
	ts_s32 out_len;
	ts_s32 in_cnt;
	ts_s32 out_cnt;
	ts_void *vqe_config;
} AUDIO_VQE_CONFIG_S;

/*Defines the configure parameters of AI saving file.*/
typedef struct tsAUDIO_SAVE_FILE_INFO_S
{
    TS_BOOL     bCfg;
    TS_CHAR  	aFilePath[MAX_AUDIO_FILE_PATH_LEN];
	TS_CHAR  	aFileName[MAX_AUDIO_FILE_NAME_LEN];
    TS_U32 		u32FileSize;  /*in KB*/
} AUDIO_SAVE_FILE_INFO_S;

/*Defines whether the file is saving or not .*/
typedef struct tsAUDIO_FILE_STATUS_S
{
    TS_BOOL     bSaving;
} AUDIO_FILE_STATUS_S;

/**Defines audio clksel type*/
typedef enum tsAUDIO_CLKSEL_E
{
    AUDIO_CLKSEL_BASE       = 0,  /*<Audio base clk. */
    AUDIO_CLKSEL_SPARE,           /*<Audio spare clk. */

    AUDIO_CLKSEL_BUTT,
} AUDIO_CLKSEL_E;

/*Defines audio mode parameter.*/
typedef struct tsAUDIO_MOD_PARAM_S
{
    AUDIO_CLKSEL_E enClkSel;
} AUDIO_MOD_PARAM_S;


typedef enum tsEN_AIO_ERR_CODE_E
{
    AIO_ERR_VQE_ERR        = 65 , /*vqe error*/

} EN_AIO_ERR_CODE_E;


/* invlalid device ID */
#define TS_ERR_AI_INVALID_DEVID     TS_DEF_ERR(TS_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define TS_ERR_AI_INVALID_CHNID     TS_DEF_ERR(TS_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define TS_ERR_AI_ILLEGAL_PARAM     TS_DEF_ERR(TS_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* using a NULL point */
#define TS_ERR_AI_NULL_PTR          TS_DEF_ERR(TS_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define TS_ERR_AI_NOT_CONFIG        TS_DEF_ERR(TS_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define TS_ERR_AI_NOT_SUPPORT       TS_DEF_ERR(TS_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define TS_ERR_AI_NOT_PERM          TS_DEF_ERR(TS_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* the devide is not enabled  */
#define TS_ERR_AI_NOT_ENABLED       TS_DEF_ERR(TS_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* failure caused by malloc memory */
#define TS_ERR_AI_NOMEM             TS_DEF_ERR(TS_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define TS_ERR_AI_NOBUF             TS_DEF_ERR(TS_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define TS_ERR_AI_BUF_EMPTY         TS_DEF_ERR(TS_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define TS_ERR_AI_BUF_FULL          TS_DEF_ERR(TS_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define TS_ERR_AI_SYS_NOTREADY      TS_DEF_ERR(TS_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)

#define TS_ERR_AI_BUSY              TS_DEF_ERR(TS_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
/* vqe  err */
#define TS_ERR_AI_VQE_ERR       TS_DEF_ERR(TS_ID_AI, EN_ERR_LEVEL_ERROR, AIO_ERR_VQE_ERR)

/* invlalid device ID */
#define TS_ERR_AO_INVALID_DEVID     TS_DEF_ERR(TS_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define TS_ERR_AO_INVALID_CHNID     TS_DEF_ERR(TS_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define TS_ERR_AO_ILLEGAL_PARAM     TS_DEF_ERR(TS_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* using a NULL point */
#define TS_ERR_AO_NULL_PTR          TS_DEF_ERR(TS_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define TS_ERR_AO_NOT_CONFIG        TS_DEF_ERR(TS_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define TS_ERR_AO_NOT_SUPPORT       TS_DEF_ERR(TS_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define TS_ERR_AO_NOT_PERM          TS_DEF_ERR(TS_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* the devide is not enabled  */
#define TS_ERR_AO_NOT_ENABLED       TS_DEF_ERR(TS_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* failure caused by malloc memory */
#define TS_ERR_AO_NOMEM             TS_DEF_ERR(TS_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define TS_ERR_AO_NOBUF             TS_DEF_ERR(TS_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define TS_ERR_AO_BUF_EMPTY         TS_DEF_ERR(TS_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define TS_ERR_AO_BUF_FULL          TS_DEF_ERR(TS_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define TS_ERR_AO_SYS_NOTREADY      TS_DEF_ERR(TS_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)

#define TS_ERR_AO_BUSY              TS_DEF_ERR(TS_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
/* vqe  err */
#define TS_ERR_AO_VQE_ERR       TS_DEF_ERR(TS_ID_AO, EN_ERR_LEVEL_ERROR, AIO_ERR_VQE_ERR)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __TS_COMM_AI_H__ */

