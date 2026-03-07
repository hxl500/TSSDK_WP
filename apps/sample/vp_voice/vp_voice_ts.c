#include "vp_voice.h"
#include "sample_comm.h"
#include <sys/select.h>
#include <vp_printf.h>
#include <vp_observers.h>
#include "ts_alg_sol_vqe_stereo.h"

typedef struct {
    uint8_t enable;
    float volume;
    float gain;
    float digital_gain;
    uint32_t duration;
    uint32_t frame_size;
    uint8_t mute;
    AUDIO_FRAME_S frm;
    AIO_ATTR_S attrs;
} vp_voice_t;

typedef struct {
    int enable;
    int sensitivity;
} vp_sound_detect_t;

#define VP_AUDIO_DEVICE         "/dev/ts_codec_ctl"
#define VP_AUDIO_DRIVER_PATH    "/usr/lib/modules/ts_codec.ko"

#define VP_VOICE_NUM 2

#define VP_VOICE_IN 0
#define IN_DEV_ID 0
#define IN_CHN_ID 0

#define VP_VOICE_OUT 1
#define OUT_DEV_ID 0
#define OUT_CHN_ID 0

#define HIGH_SENSITIVITY_SOUND_DETECT  55
#define LOW_SENSITIVITY_SOUND_DETECT   70

#define VQE_STEREO

static vp_voice_t voices[VP_VOICE_NUM] = {0};
vp_sound_detect_t vp_sound_detect = {0};

static ts_u32 g_ai_vqe_open_mask = 0;

ts_s32 SAMPLE_COMM_AUDIO_EnableAec(int inDev, int inChn, int outDev, int outChn) 
{
    #if defined(VQE_STEREO)
    g_ai_vqe_open_mask |= ALG_VQE_SOL_ADAPT_AEC_MASK;
    #endif
	return 0;
}

ts_s32 SAMPLE_COMM_AUDIO_DisableAec(int inDev, int inChn)
{
    #if defined(VQE_STEREO)
    g_ai_vqe_open_mask &= (~ALG_VQE_SOL_ADAPT_AEC_MASK);
    #endif
	return 0;
}

#if defined(VQE_STEREO)
static ts_s32 ai_stereo_vqe_open(ts_void **handle, ts_void *attr)
{
	ts_s32 ret = TS_SUCCESS;

	TS_S32 err = TS_ALG_SolVqeStereo_Init(handle, (ALG_SOL_VQE_STEREO_CONF_PARAM *)attr);
	if ((!*handle) || (err != 0)){
		printf("[%s]:Error, Initializtion Fail.\n", __FUNCTION__);
		ret = TS_FAILURE;
	}else{
		printf("[%s]:Initializtion ok.\n", __FUNCTION__);
		ret = TS_SUCCESS;
	}

	return ret;
}

static ts_s32 ai_stereo_vqe_set_proc_addr(ts_void *handle, ts_void *addr)
{
	printf("[%s]\n", __FUNCTION__);
	return TS_SUCCESS;
}

static ts_s32 ai_stereo_vqe_process(ts_void *handle, ts_u8 *in_buf, ts_u8 *ref_buf, ts_u32 *in_len, ts_u8 *out_buf, ts_u32 *out_len)
{
	TS_S32 s32Ret = TS_SUCCESS;

	TS_U32 u32OutLen, u32InLen;
	TS_U32 u32ShortBytes;

	u32ShortBytes = sizeof(short);
	u32InLen = *in_len/u32ShortBytes;

	s32Ret = TS_ALG_SolVqeStereo_Process(handle, (TS_S16 *)in_buf, (TS_S16 *)ref_buf, &u32InLen, (TS_S16 *)out_buf, &u32OutLen);
	if (s32Ret) {
		printf("[%s]:Error, return:%d.\n", __FUNCTION__, s32Ret);
		s32Ret = TS_FAILURE;
	} else {
		*out_len = u32OutLen * u32ShortBytes;
		s32Ret = TS_SUCCESS;
	}

	return s32Ret;
}

static ts_s32 ai_stereo_vqe_close(ts_void *handle)
{
	TS_S32 s32Ret = TS_SUCCESS;

	s32Ret = TS_ALG_SolVqeStereo_Exit(handle);
	if (s32Ret) {
		printf("[%s]:Error, return:%d.\n", __FUNCTION__, s32Ret);
		s32Ret = TS_FAILURE;
	} else {
		s32Ret = TS_SUCCESS;
	}

	return s32Ret;
}
#endif

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
		vp_error("TS_MPI_AI_SetPubAttr(%d) failed with %#x\n", AiDevId, s32Ret);
		return -1;
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
                    #if defined(VQE_STEREO)
                    s32Ret = TS_MPI_AI_SetTalkVqeAttr(AiDevId, i, (AUDIO_VQE_CONFIG_S *)pstAiVqeAttr);

					AUDIO_VQE_S aivqe;
					snprintf(aivqe.vqe_name,sizeof(aivqe.vqe_name),"%s","sol_vqe_stereo");
					aivqe.vqe_open = ai_stereo_vqe_open;
					aivqe.vqe_set_proc_addr = ai_stereo_vqe_set_proc_addr;
					aivqe.vqe_process = ai_stereo_vqe_process;
					aivqe.vqe_close = ai_stereo_vqe_close;
					g_ai_vqe_open_mask = ((ALG_SOL_VQE_STEREO_CONF_PARAM*)((AUDIO_VQE_CONFIG_S *)pstAiVqeAttr)->vqe_config)->u32OpenMask;

					s32Ret = TS_MPI_AI_RegisteredVqe(AiDevId, i, &aivqe);
                    #endif
					break;
				default:
					s32Ret = -1;
					break;
			}

			if (s32Ret)
			{
				vp_error("SetAiVqe%d(%d,%d) failed with %#x\n", u32AiVqeType, AiDevId, i, s32Ret);
				return -1;
			}

			if (bAiVqe)
			{
				s32Ret = TS_MPI_AI_EnableVqe(AiDevId, i);
				if (s32Ret)
				{
					vp_error("TS_MPI_AI_EnableVqe(%d,%d) failed with %#x\n", AiDevId, i, s32Ret);
					return -1;
				}
			}
		}
	}


	s32Ret = TS_MPI_AI_Enable(AiDevId);
	if (s32Ret)
	{
		vp_error("TS_MPI_AI_Enable(%d) failed with %#x\n", AiDevId, s32Ret);
		return -1;
	}

	for (i = 0; i < s32AiChnCnt>>pstAioAttr->enSoundmode; i++)
	{
		if(bEnableChn)
		{
			s32Ret = TS_MPI_AI_EnableChn(AiDevId, i);
			if (s32Ret)
			{
				vp_error("TS_MPI_AI_EnableChn(%d,%d) failed with %#x\n", AiDevId, i, s32Ret);
				return -1;
			}
		}
	}

	return 0;
}

/******************************************************************************
* function : Start Ao
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StartAo(AUDIO_DEV AoDevId, TS_S32 s32AoChnCnt,
    AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enInSampleRate, TS_BOOL bResampleEn, TS_VOID* pstAoVqeAttr, TS_U32 u32AoVqeType)
{
    TS_S32 i = 0;
    TS_S32 s32Ret;

    s32Ret = TS_MPI_AO_SetPubAttr(AoDevId, pstAioAttr);
    if (TS_SUCCESS != s32Ret)
    {
        vp_error("TS_MPI_AO_SetPubAttr(%d) failed with %#x!\n", AoDevId, s32Ret);
        return -1;
    }

    s32Ret = TS_MPI_AO_Enable(AoDevId);
    if (TS_SUCCESS != s32Ret)
    {
        vp_error("TS_MPI_AO_Enable(%d) failed with %#x!\n", AoDevId, s32Ret);
        return -1;
    }

    s32Ret = TS_MPI_AO_EnableChn(AoDevId, i);
    if (s32Ret)
    {
        vp_error("TS_MPI_AO_EnableChn(%d) failed with %#x!\n", i, s32Ret);
        return -1;
    }

    return 0;
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
		if (TS_TRUE == bVqeEn)
		{
			s32Ret = TS_MPI_AI_DisableVqe(AiDevId, i);
			if (s32Ret)
			{
				vp_error("TS_MPI_AI_DisableVqe failed with %d.\n",  s32Ret);
				return -1;
			}
		}

		if (TS_TRUE == bDisableAiChn)
		{
			s32Ret = TS_MPI_AI_DisableChn(AiDevId, i);
			if (s32Ret)
			{
				vp_error("TS_MPI_AI_DisableChn failed with %d.\n",  s32Ret);
				return -1;
			}
		}

		if (TS_TRUE == bVqeEn)
		{
			s32Ret = TS_MPI_AI_UnRegisteredVqe(AiDevId, i);
			if (s32Ret)
			{
				vp_error("TS_MPI_AI_UnRegisteredVqe failed with %d.\n",  s32Ret);
				return -1;
			}
		}

	}

	s32Ret = TS_MPI_AI_Disable(AiDevId);
	if (s32Ret)
	{
		vp_error("TS_MPI_AI_Disable failed with %d.\n",  s32Ret);
		return -1;
	}

	return 0;
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
		s32Ret = TS_MPI_AO_DisableChn(AoDevId, i);
		if (s32Ret)
		{
			vp_error("TS_MPI_AO_DisableChn failed with %#x!\n", s32Ret);
			return -1;
		}
		s32Ret = TS_MPI_AO_UnRegisteredVqe(AoDevId, i);
		if (s32Ret)
		{
			vp_error("TS_MPI_AO_UnRegisteredVqe failed with %#x!\n", s32Ret);
			return -1;
		}
	}

	s32Ret = TS_MPI_AO_Disable(AoDevId);
	if (s32Ret)
	{
		vp_error("TS_MPI_AO_Disable failed with %#x!\n", s32Ret);
		return s32Ret;
	}

	return 0;
}

int vp_set_audio_detect_sensitivity(int sensitivity) {
    vp_debug("set audio detect sensitivity:%d", sensitivity);
    vp_sound_detect.sensitivity = sensitivity;
    return 0;
}

int vp_set_audio_detect_enable(int enable) {
    vp_debug("set audio detect enable:%d", enable);
    vp_sound_detect.enable = enable;
    return 0;
}

/**
 * 计算 PCM 音频数据的声音分贝值 (db)
 *
 * @param pdata PCM 数据指针
 * @param data_len 数据长度（字节数）
 * @return 返回计算出的声音分贝值
 */
static int vp_voice_calc_db(uint8_t *data, uint32_t data_len) {
    int ndb = 0, ncycle = 0;
    short int nvalue = 0;
    double sum = 0;

    for (ncycle = 0; (ncycle + 2) <= data_len;
         ncycle += 2) {
        memcpy(&nvalue, data + ncycle, 2);
        sum += abs(nvalue);
    }

    sum = sum / (data_len / 2);
    if (sum > 0) {
        ndb = (int) (20.0 * log10(sum));
    }

    return ndb;
}

void vp_is_sound_alarm_triggered(int db) {
    time_t current_time = time(NULL);
    static time_t last_time = 0;
    if (current_time - last_time < 2) return;

    int alarm_happen = 0;
    if (vp_sound_detect.sensitivity) {
        if (db > HIGH_SENSITIVITY_SOUND_DETECT) {
            alarm_happen = 1;
        }
    } else {
        if (db > LOW_SENSITIVITY_SOUND_DETECT) {
            alarm_happen = 1;
        }
    }

    if (alarm_happen) {
        last_time = current_time;
        vp_observers_post(VP_OBS_TYPE_ALARM, VP_OBS_EVENT_ALARM_SOUND, NULL);
    }
}

int ts_sample_rate(uint32_t sample_rate, AUDIO_SAMPLE_RATE_E *ts_sample_rate) {
    switch (sample_rate) {
        case 8000:
            *ts_sample_rate = AUDIO_SAMPLE_RATE_8000;
            return 0;
        case 12000:
            *ts_sample_rate = AUDIO_SAMPLE_RATE_12000;
            return 0;
        case 16000:
            *ts_sample_rate = AUDIO_SAMPLE_RATE_16000;
            return 0;
        case 24000:
            *ts_sample_rate = AUDIO_SAMPLE_RATE_24000;
            return 0;
        case 32000:
            *ts_sample_rate = AUDIO_SAMPLE_RATE_32000;
            return 0;
        case 44100:
            *ts_sample_rate = AUDIO_SAMPLE_RATE_44100;
            return 0;
        case 48000:
            *ts_sample_rate = AUDIO_SAMPLE_RATE_48000;
            return 0;
        case 96000:
            *ts_sample_rate = AUDIO_SAMPLE_RATE_96000;
            return 0;
        default:
            return -1;
    }
}

int ts_sample_bits(uint32_t bits, AUDIO_BIT_WIDTH_E *ts_bits) {
    switch (bits) {
        case 16:
            *ts_bits = AUDIO_BIT_WIDTH_16;
            return 0;
        default:
            return -1;
    }
}

int ts_sample_channel(uint32_t channel, AUDIO_SOUND_MODE_E *ts_channel) {
    switch (channel) {
        case 1:
            *ts_channel = AUDIO_SOUND_MODE_MONO;
            return 0;
        case 2:
            *ts_channel = AUDIO_SOUND_MODE_STEREO;
            return 0;
        default:
            return -1;
    }
}

/**
 * 启用音频输入
 *
 * @param sample_rate   [in]   音频采样频率
 * @param bits          [in]   音频采样位宽
 * @param channels      [in]   音频采样通道数
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_enable(uint32_t sample_rate, uint8_t bits, uint8_t channels) {
    if (voices[VP_VOICE_IN].enable) return -1;
    vp_voice_insert_driver();
    int ret = ts_sample_rate(sample_rate, &voices[VP_VOICE_IN].attrs.enSamplerate);
    if (ret) {
        vp_error("imp_sample_rate:%d error:%d", sample_rate, ret);
        return ret;
    }

    ret = ts_sample_bits(bits, &voices[VP_VOICE_IN].attrs.enBitwidth);
    if (ret) {
        vp_error("imp_sample_bits:%d error:%d", bits, ret);
        return ret;
    }

    ret = ts_sample_channel(channels, &voices[VP_VOICE_IN].attrs.enSoundmode);
    if (ret) {
        vp_error("imp_sample_channel:%d error:%d", channels, ret);
        return ret;
    }

    voices[VP_VOICE_IN].attrs.u8CardNum = 0;
    voices[VP_VOICE_IN].attrs.aioMode = AIO_INNER_CODEC;
    voices[VP_VOICE_IN].attrs.u32PtNumPerFrm = 256;
    voices[VP_VOICE_IN].attrs.u8DeviceNum = 0;
    voices[VP_VOICE_IN].attrs.u32FrmNum = 8;
    voices[VP_VOICE_IN].attrs.aecEnable = TS_TRUE;

    voices[VP_VOICE_IN].frame_size = sample_rate * channels * bits / 8;
    voices[VP_VOICE_IN].duration = (uint32_t)(((double) (voices[VP_VOICE_IN].attrs.u32PtNumPerFrm * 2) / (double) voices[VP_VOICE_IN].frame_size) * 1000000);

    #if defined(VQE_STEREO)
    TS_S32 s32VqeFrmLen = 256;//80,160,320
    
    AUDIO_VQE_CONFIG_S stVqeConfig;
    memset(&stVqeConfig, 0, sizeof(AUDIO_VQE_CONFIG_S));
    ALG_SOL_VQE_STEREO_CONF_PARAM sol_conf;
	memset(&sol_conf, 0, sizeof(ALG_SOL_VQE_STEREO_CONF_PARAM));
	sol_conf.u32OpenMask =	0
				| ALG_VQE_SOL_ADAPT_AEC_MASK
				| ALG_VQE_SOL_WEBRTC_AGC_MASK
				| ALG_VQE_SOL_WEBRTC_ANR_MASK
				| ALG_VQE_NOISEGATE_MASK
				| ALG_VQE_MMSENS_MASK
				;
	sol_conf.u32SmplRate = voices[VP_VOICE_IN].attrs.enSamplerate;
    sol_conf.u16FrameLen = s32VqeFrmLen;
	sol_conf.u8MicChanNum = 1;
    sol_conf.u8RefChanNum = 1;
	sol_conf.s16AgcMode = 3;
	sol_conf.s32NoiseSuppress = -15;//噪声抑制
	sol_conf.fAttackTime = 0.005f;
	sol_conf.fReleaseTime = 0.005f;	
    sol_conf.s32Threshold = -80;
    sol_conf.eNlpMode = 2;
	sol_conf.solVqeSetParam.algAnrParams.s32Mode = 2;
	sol_conf.solVqeSetParam.algAgcParams.s16TargetLevelDbfs = 3;
    sol_conf.solVqeSetParam.algAgcParams.s16CompressionGaindB = 9;
    sol_conf.solVqeSetParam.algAgcParams.u8LimiterEnable = 1;
	stVqeConfig.vqe_config = &sol_conf;
    stVqeConfig.in_len = s32VqeFrmLen * 2;
	stVqeConfig.in_cnt = s32VqeFrmLen;
	stVqeConfig.out_len = s32VqeFrmLen * 2;
	stVqeConfig.out_cnt = s32VqeFrmLen;
    TS_S32 s32AiChnCnt = 1;
    ret = SAMPLE_COMM_AUDIO_StartAi(IN_DEV_ID, s32AiChnCnt, &voices[VP_VOICE_IN].attrs, AUDIO_SAMPLE_RATE_BUTT, TS_TRUE, TS_FALSE, &stVqeConfig, 1);
    if (TS_SUCCESS != ret) {
        vp_error("SAMPLE_COMM_AUDIO_StartAi failed with %d!\n", ret);
        return ret;
    }
    #else
    TS_S32 s32AiChnCnt = 1;
    ret = SAMPLE_COMM_AUDIO_StartAi(IN_DEV_ID, s32AiChnCnt, &voices[VP_VOICE_IN].attrs, AUDIO_SAMPLE_RATE_BUTT, TS_TRUE, TS_FALSE, NULL, 0);
    if (TS_SUCCESS != ret) {
        vp_error("SAMPLE_COMM_AUDIO_StartAi failed with %d!\n", ret);
        return ret;
    }
    #endif

    #if defined(VQE_STEREO)
    SAMPLE_COMM_AUDIO_DisableAec(IN_DEV_ID, IN_CHN_ID);
    #endif

    voices[VP_VOICE_IN].enable = 1;

    return 0;
}

/**
 * 设置输入音量
 *
 * @param vol   [in]    音量百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_set_vol(float vol) {
    if (voices[VP_VOICE_IN].enable == 0) return -1;
    if (vol > 1.f) vol = 1.f;
    int chnVol = -69 + (int) (vol * 95.f);
    int ret = TS_MPI_AI_SetVqeVolume(IN_DEV_ID, 0, chnVol);
	if (ret == 0) voices[VP_VOICE_IN].volume = vol;
    return ret;
}

/**
 * 获取输入音量
 *
 * @param vol   [out]    音量百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_get_vol(float *vol) {
    if (voices[VP_VOICE_IN].enable == 0 || vol == NULL) return -1;
    if (voices[VP_VOICE_IN].mute) *vol = 0;
    else *vol = voices[VP_VOICE_IN].volume;
    return 0;
}

/**
 * 设置输入模拟增益
 *
 * @param gain   [in]    增益百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_set_gain(float gain) {
    if (voices[VP_VOICE_IN].enable == 0) return -1;
    if (gain > 1.f) gain = 1.f;
    voices[VP_VOICE_IN].gain = gain;
    return 0;
}

/**
 * 获取输入模拟增益
 *
 * @param gain   [out]    增益百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_get_gain(float *gain) {
    if (voices[VP_VOICE_IN].enable == 0 || gain == NULL) return -1;
    *gain = voices[VP_VOICE_IN].gain;
    return 0;
}

/**
 * 设置输入数字增益
 *
 * @param gain   [in]    增益百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_set_digital_gain(float gain) {
    if (voices[VP_VOICE_IN].enable == 0) return -1;
    if (gain > 1.f) gain = 1.f;
    voices[VP_VOICE_IN].digital_gain = gain;

    return 0;
}

/**
 * 获取输入数字增益
 *
 * @param gain   [out]    增益百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_get_digital_gain(float *gain) {
    if (voices[VP_VOICE_IN].enable == 0 || gain == NULL) return -1;
    *gain = voices[VP_VOICE_IN].digital_gain;

    return 0;
}

/**
 * 设置输入静音
 *
 * @param mute   [in]    1 启用,0 禁用
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_set_mute(uint8_t mute) {
    return -1;
}

/**
 * 获取输入静音状态
 *
 * @param mute   [in]    静音状态.1 启用,0 禁用
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_get_mute(uint8_t *mute) {
    return -1;
}

/**
 * 暂停音频输入
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_pause() {
    if (voices[VP_VOICE_IN].enable == 0) return -1;
    return 0;
}

/**
 * 恢复音频输入
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_resume() {
    if (voices[VP_VOICE_IN].enable == 0) return -1;
    return 0;
}

/**
 * 获取输入音频帧大小
 *
 * @return 错误码或音频帧数据大小
 *
 * @retval \>0 成功
 * @retval \<0 失败
 */
int vp_voice_input_frame_size() {
    if (voices[VP_VOICE_IN].enable == 0) return -1;
    return voices[VP_VOICE_IN].attrs.u32PtNumPerFrm * 2;
}

/**
 * 获取输入音频帧大小
 *
 * @return 错误码或音频帧数据大小
 *
 * @retval \>0 成功
 * @retval \<0 失败
 */
uint32_t vp_voice_input_frame_duration() {
    return voices[VP_VOICE_IN].duration;
}

/**
 * 请求输入音频帧
 *
 * @note 请求的音频帧需要调用`vp_voice_release_frame`进行释放
 *
 * @param frame     [out]   输出音频帧信息
 * @param timeout   [in]    请求超时时间,单位毫秒(ms)
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_request_frame(vp_voice_frame_t *frame, uint32_t timeout) {
    if (voices[VP_VOICE_IN].enable == 0 || NULL == frame) return -1;
    fd_set read_fds;
    TS_S32 AiFd;
    struct timeval TimeoutVal;
    AUDIO_FRAME_S *frm = &voices[VP_VOICE_IN].frm;
    FD_ZERO(&read_fds);

    AiFd = TS_MPI_AI_GetFd(IN_DEV_ID, IN_CHN_ID);
    FD_SET(AiFd, &read_fds);

    TimeoutVal.tv_sec = timeout / 1000;
	TimeoutVal.tv_usec = (timeout % 1000) * 1000;

    int ret = TS_MPI_AI_SELECT(AiFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
    if (ret <= 0) {
        vp_error("TS_MPI_AI_SELECT failed with %d.\n", ret);
        return -1;
    }

    if (FD_ISSET(AiFd, &read_fds)) {
        ret = TS_MPI_AI_GetFrame(IN_DEV_ID, IN_CHN_ID, frm, NULL, TS_FALSE);
        
        if (ret) {
            vp_error("TS_MPI_AI_GetFrame failed with %x.\n", ret);
            return ret;
        }

        frame->frame_no = frm->u32Seq;
        frame->timestamp = vp_system_time_us();
        frame->frame_size = frm->u32Len;
        frame->frame_data = (uint8_t *)frm->u64VirAddr[0];
        frame->priv_data = frm;

        if (vp_sound_detect.enable) {
            int db = vp_voice_calc_db(frame->frame_data, frame->frame_size);
            vp_is_sound_alarm_triggered(db);
        }
    }

    return 0;
}

/**
 * 释放音频帧数据
 *
 * @param frame     [in]   音频帧信息
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_release_frame(vp_voice_frame_t *frame) {
    if (voices[VP_VOICE_IN].enable == 0 || NULL == frame) return -1;
    AUDIO_FRAME_S *frm = (AUDIO_FRAME_S *)frame->priv_data;
    return TS_MPI_AI_ReleaseFrame(IN_DEV_ID, IN_CHN_ID, frm, NULL);
}

/**
 * 禁用音频输入
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_input_disable() {
    if (!voices[VP_VOICE_IN].enable) return -1;
    voices[VP_VOICE_IN].enable = 0;
    #if defined(VQE_STEREO)
    return SAMPLE_COMM_AUDIO_StopAi(IN_DEV_ID, IN_CHN_ID, TS_TRUE, TS_FALSE, TS_TRUE);
    #else
    return SAMPLE_COMM_AUDIO_StopAi(IN_DEV_ID, IN_CHN_ID, TS_TRUE, TS_FALSE, TS_FALSE);
    #endif
}


/**
 * 启用音频输出
 *
 * @param sample_rate   [in]   音频采样频率
 * @param bits          [in]   音频采样位宽
 * @param channels      [in]   音频采样通道数
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_enable(uint32_t sample_rate, uint8_t bits, uint8_t channels) {
    if (voices[VP_VOICE_OUT].enable) return -1;

    int ret = ts_sample_rate(sample_rate, &voices[VP_VOICE_OUT].attrs.enSamplerate);
    if (ret) {
        vp_error("ts_sample_rate:%d error:%d", sample_rate, ret);
        return ret;
    }

    ret = ts_sample_bits(bits, &voices[VP_VOICE_OUT].attrs.enBitwidth);
    if (ret) {
        vp_error("ts_sample_bits:%d error:%d", bits, ret);
        return ret;
    }

    ret = ts_sample_channel(channels, &voices[VP_VOICE_OUT].attrs.enSoundmode);
    if (ret) {
        vp_error("ts_sample_channel:%d error:%d", channels, ret);
        return ret;
    }
    voices[VP_VOICE_OUT].attrs.u8CardNum = 0;
    voices[VP_VOICE_OUT].attrs.aioMode = AIO_INNER_CODEC;
    voices[VP_VOICE_OUT].attrs.u32PtNumPerFrm = 256;
    voices[VP_VOICE_OUT].attrs.u32FrmNum = 4;

    if (voices[VP_VOICE_OUT].attrs.enSamplerate == AUDIO_SAMPLE_RATE_8000) {
        voices[VP_VOICE_OUT].attrs.u32PtNumPerFrm = 256;
    }
    TS_S32 s32AoChnCnt = 1;
    ret = SAMPLE_COMM_AUDIO_StartAo(OUT_DEV_ID, s32AoChnCnt, &voices[VP_VOICE_OUT].attrs, AUDIO_SAMPLE_RATE_BUTT, TS_FALSE, TS_NULL, 0); 
    if (ret) {
        vp_error("Failed to call AMPLE_COMM_AUDIO_StartAo error:%d", ret);
        return ret;
    }

    voices[VP_VOICE_OUT].frm.u32Len = (voices[VP_VOICE_OUT].attrs.u32PtNumPerFrm << 1);
    voices[VP_VOICE_OUT].frm.u64VirAddr[0] = malloc(voices[VP_VOICE_OUT].frm.u32Len);
    voices[VP_VOICE_OUT].frm.u64PhyAddr[0] = 0;
    voices[VP_VOICE_OUT].enable = 1;
    return 0;
}

/**
 * 获取输出音频帧大小
 *
 * @return 错误码或音频帧数据大小
 *
 * @retval \>0 成功
 * @retval \<0 失败
 */
int vp_voice_output_frame_size() {
    if (voices[VP_VOICE_OUT].enable) return -1;
    return voices[VP_VOICE_OUT].attrs.u32PtNumPerFrm * 2;
}

/**
 * 设置输出音量
 *
 * @param vol   [in]    音量百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_set_vol(float vol) {
    if (voices[VP_VOICE_OUT].enable == 0) return -1;
    if (vol > 1.f) vol = 1.f;
    int chnVol = -69 + (int) (vol * 95.f);
    int ret = TS_MPI_AO_SetVolume(IN_DEV_ID, chnVol);
	if (ret == 0) voices[VP_VOICE_OUT].volume = vol;
    return ret;
}

/**
 * 获取输出音量
 *
 * @param vol   [out]    音量百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_get_vol(float *vol) {
    if (voices[VP_VOICE_OUT].enable == 0 || vol == NULL) return -1;
    if (voices[VP_VOICE_OUT].mute) *vol = 0;
    else *vol = voices[VP_VOICE_OUT].volume;
    return 0;
}

/**
 * 设置输出模拟增益
 *
 * @param gAOn   [in]    增益百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_set_gain(float gain) {
    if (voices[VP_VOICE_OUT].enable == 0) return -1;
    if (gain > 1.f) gain = 1.f;
    voices[VP_VOICE_OUT].gain = gain;

    return 0;
}

/**
 * 获取输出模拟增益
 *
 * @param gain   [out]    增益百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_get_gain(float *gain) {
    if (voices[VP_VOICE_OUT].enable == 0 || gain == NULL) return -1;
    *gain = voices[VP_VOICE_OUT].gain;
    return 0;
}

/**
 * 设置输出数字增益
 *
 * @param gain   [in]    增益百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_set_digital_gain(float gain) {
    if (voices[VP_VOICE_OUT].enable == 0) return -1;
    if (gain > 1.f) gain = 1.f;
    voices[VP_VOICE_OUT].digital_gain = gain;
    
    return 0;
}

/**
 * 获取输出数字增益
 *
 * @param gain   [out]    增益百分比 [0.f~1.0f]
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_get_digital_gain(float *gain) {
    if (voices[VP_VOICE_OUT].enable == 0 || gain == NULL) return -1;
    *gain = voices[VP_VOICE_OUT].digital_gain;
    return 0;
}

/**
 * 设置输出静音
 *
 * @param mute   [in]    1 启用,0 禁用
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_set_mute(uint8_t mute) {
    if (voices[VP_VOICE_OUT].enable == 0) return -1;
    vp_system("echo 448 > /sys/class/gpio/export");
    vp_system("echo out > /sys/class/gpio/gpio448/direction");
    if (mute) {
        vp_system("echo 0 > /sys/class/gpio/gpio448/value");
    } else { 
        vp_system("echo 1 > /sys/class/gpio/gpio448/value");
    }

    voices[VP_VOICE_OUT].mute = mute;
    return 0;
}

/**
 * 获取输出静音状态
 *
 * @param mute   [in]    静音状态.1 启用,0 禁用
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_get_mute(uint8_t *mute) {
    if (voices[VP_VOICE_OUT].enable == 0 || mute == NULL) return -1;
    if (voices[VP_VOICE_OUT].mute) *mute = 1;
    else *mute = 0;
    return 0;
}

/**
 * 暂停输出音频
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_pause() {
    if (voices[VP_VOICE_OUT].enable == 0) return -1;
    return 0;
}

/**
 * 恢复输出音频
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_resume() {
    if (voices[VP_VOICE_OUT].enable == 0) return -1;
    return 0;
}

/**
 * 发送输出音频数据
 *
 * @param data     [in]   音频数据
 * @param len      [in]   音频数据长度
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_send_frame(uint8_t *data, uint32_t len) {
    if (voices[VP_VOICE_OUT].enable == 0 || data == NULL) return -1;
    AUDIO_FRAME_S frm = { 0 }, *send_frm = &voices[VP_VOICE_OUT].frm;
    uint8_t *frame = (uint8_t *) send_frm->u64VirAddr[0];
    uint32_t frame_size = send_frm->u32Len;
    uint32_t frame_offset = send_frm->u64PhyAddr[0];
    uint32_t data_offset = 0, size;

    while (data_offset < len) {
        size = len - data_offset;
        if (size > frame_size - frame_offset) {
            size = frame_size - frame_offset;
        }
        memcpy(frame + frame_offset, data + data_offset, size);
        frame_offset += size;
        data_offset += size;
        if (frame_offset == frame_size) {
            frm.u64VirAddr[0] = frame;
            frm.u32Len = send_frm->u32Len;
            int ret = TS_MPI_AO_SendFrame(OUT_DEV_ID, OUT_CHN_ID, &frm, 1000);
            if (ret != 0) {
                vp_error("TS_MPI_AO_SendFrame error:%d", ret);
                return ret;
            }
            frame_offset = 0;
        }
    }
    send_frm->u64PhyAddr[0] = frame_offset;
    return 0;
}

/**
 * 刷新音频输出缓存
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_send_flush() {
    AUDIO_FRAME_S *frm = &voices[VP_VOICE_OUT].frm;
    uint8_t *frame = (uint8_t *) frm->u64VirAddr[0];
    uint32_t frame_offset = frm->u64PhyAddr[0];

    if (frame_offset == 0) {
        return 0;
    }

    memset(frame + frame_offset, 0, frm->u32Len - frame_offset);
    return TS_MPI_AO_SendFrame(OUT_DEV_ID, OUT_CHN_ID, frm, 1000);
}

/**
 * 禁用音频输出
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_output_disable() {
    if (!voices[VP_VOICE_OUT].enable) return -1;
    voices[VP_VOICE_OUT].enable = 0;
    free(voices[VP_VOICE_OUT].frm.u64VirAddr[0]);
    voices[VP_VOICE_OUT].frm.u64VirAddr[0] = 0;
    TS_S32 s32AoChnCnt = 1;
    int ret = SAMPLE_COMM_AUDIO_StopAo(OUT_DEV_ID, s32AoChnCnt, TS_FALSE);
    if (ret != 0) {
        vp_error("SAMPLE_COMM_AUDIO_StopAo error:%d", ret);
    }
    return ret;
}

/**
 * 加载音频驱动
 *
 * @return NULL
 */
void vp_voice_insert_driver(){
    if (access(VP_AUDIO_DEVICE, F_OK) == 0) return;
    if (access(VP_AUDIO_DRIVER_PATH, F_OK) == 0) {
        vp_system("insmod /usr/lib/modules/ts_codec.ko");
        int timeout = 10;
        while (timeout-- && access(VP_AUDIO_DEVICE, F_OK)) {
            vp_error("wait for /dev/dsp timeout = %d", timeout);
            usleep(100000);
        }
    }

    return;
}

/**
 * 卸载音频驱动
 *
 * @return NULL
 */
void vp_voice_rm_driver(){
    if (access(VP_AUDIO_DEVICE, F_OK) == 0) {
        if (access(VP_AUDIO_DRIVER_PATH, F_OK) == 0) {
            vp_system("rmmod /usr/lib/modules/ts_codec.ko");
        }
    }

    return;
}

static uint8_t g_aec_enable = 0;

/**
 * 启用回声消除
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_aec_enable() {
    if (g_aec_enable) return 0;
    g_aec_enable = 1;
    int ret = 0;
    #if defined(VQE_WEBRTC)
    ret = SAMPLE_COMM_AUDIO_EnableAec(IN_DEV_ID, IN_CHN_ID, OUT_DEV_ID, OUT_CHN_ID);
    #endif
    if (ret) {
        g_aec_enable = 0;
        vp_warn("IMP_AI_EnableAec error:%d", ret);
    }
    return ret;
}

/**
 * 禁用回声消除
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_voice_aec_disable() {
    if (g_aec_enable == 0) return 0;
    g_aec_enable = 0;
    int ret = 0;
    #if defined(VQE_WEBRTC)
    ret = SAMPLE_COMM_AUDIO_DisableAec(IN_DEV_ID, IN_CHN_ID);
    #endif
    if (ret) {
        g_aec_enable = 1;
        vp_warn("IMP_AI_DisableAec error:%d", ret);
    }
    return ret;
}