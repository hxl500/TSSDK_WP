#include "vp_voice.h"
#include "sample_comm.h"

static TS_S32 SAMPLE_AUDIO_SYS_Init(void)
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

int main(int argc, char* argv[]) {
     vp_voice_frame_t frame = { 0 };
    int i = 0;
    int sample_rate = 8000;
    const int channel = 1;
    int speek_mute = 0;
    int cnt = 0;
    int enable_ace = 0;
    int ret = 0;
    FILE* fout = NULL;
    void* handle = NULL;
    char szPath[260] = { 0 };

    if (argc > 1) {
        sample_rate = atoi(argv[1]);
    }

    SAMPLE_AUDIO_SYS_Init();

    sprintf(szPath, "/tmp/kws_recording_%d.pcm", sample_rate);
    fout = fopen(szPath, "wb");
    if (NULL == fout) {
        printf("Unable to open file(%s).\n", szPath);
        return -1;
    }

    printf("sample_rate=%d, channel=%d.\n", sample_rate, channel);
    vp_voice_input_enable(sample_rate, 16, channel);
    vp_voice_output_enable(sample_rate, 16, channel);
    vp_voice_input_set_vol(0.8f);
    handle = vp_kws_init();
    while (1) {
        if (0 == vp_voice_request_frame(&frame, 3000)) {
            vp_voice_send_frame(frame.frame_data, frame.frame_size);
            fwrite(frame.frame_data, frame.frame_size, 1, fout);
            fflush(fout);

            ret = vp_kws_process(handle, frame.frame_data, frame.frame_size);
            if (ret > 0) {
                printf("Wake up.\n");
            }

            if(vp_voice_release_frame(&frame)) {
                printf("Failed to call vp_voice_release_frame.\n");
            }
        } else {
            printf("Failed to call vp_voice_request_frame.\n");
        }
    }

    vp_kwd_deinit(handle);
    vp_voice_output_disable();
    vp_voice_input_disable();

    return 0;
}