#include "vp_voice.h"
#include "sample_comm.h"

TS_S32 SAMPLE_AUDIO_SYS_Init(void)
{
	TS_S32 s32Ret;
	VB_CONFIG_S stVbConf;

	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));

	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);

	if (TS_SUCCESS != s32Ret) {
		printf("SAMPLE_COMM_SYS_GetPicSize failed!\n");
		return s32Ret;
	}

	return TS_SUCCESS;
}

int main(int argc, char* argv[]) 
{
    vp_voice_frame_t frame = { 0 };
    int i = 0;
    int sample_rate = 16000;
    const int channel = 1;
    int speek_mute = 0;
    int cnt = 0;
    int enable_ace = 0;
    FILE* fout = NULL;

    fout = fopen("/tmp/recording.pcm", "wb");
    if (NULL == fout) return -1;

    if (argc > 1) {
        sample_rate = atoi(argv[1]);
    }

    if (argc > 2) {
        speek_mute = atoi(argv[2]);
    }


    SAMPLE_AUDIO_SYS_Init();
    printf("sample_rate=%d, channel=%d.\n", sample_rate, channel);
    vp_voice_input_enable(sample_rate, 16, channel);
    vp_voice_output_enable(sample_rate, 16, channel);

    printf("WebRTCVersion:%s", TS_ALG_WebRTCAEC_GetVersion());
    // vp_voice_output_set_vol(1.0f);
    // vp_voice_input_set_vol(1.0f);
    vp_voice_output_set_mute(speek_mute);

    while (1) {
        if (0 == vp_voice_request_frame(&frame, 3000)) {
            vp_voice_send_frame(frame.frame_data, frame.frame_size);
            fwrite(frame.frame_data, frame.frame_size, 1, fout);
            fflush(fout);
            if(vp_voice_release_frame(&frame)) {
                printf("Failed to call vp_voice_release_frame.\n");
            }
        } else {
            printf("Failed to call vp_voice_request_frame.\n");
        }

        ++cnt;

        if (cnt >= 500) {
            cnt = 0;
            if (0 == enable_ace) {
                enable_ace = 1;
                printf("Enable aec.\n");
                vp_voice_aec_enable();
            } else {
                enable_ace = 0;
                printf("Disable aec.\n");
                vp_voice_aec_disable();
            }
        }
    }

    vp_voice_output_disable();
    vp_voice_input_disable();

    return 0;
}
