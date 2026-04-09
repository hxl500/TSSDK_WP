
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include "soft_line.h"
#include "sample_comm_base.h"
#include "sample_comm_util.h"
#include "sample_comm_sys.h"
#include "sample_comm_lcd.h"

static TS_S32 garg_index = -1;

static TS_VOID simple_loop_wait(TS_VOID)
{
    char c=0;

    while(1)
    {
        printf("\nSAMPLE_TEST:press 'e' to exit;\n");
        c = getchar();
        if (c == 'e')
            break;
    }
    return;
}

static int isFileExist(const char* file_path)
{
    if(file_path == NULL)
        return -1;
    if(access(file_path, F_OK) == 0)
        return 0;

    return -1;
}

static TS_VOID SAMPLE_VDEC_HandleSig(TS_S32 signo)
{
    if (SIGINT == signo || SIGTSTP == signo || SIGTERM == signo)
    {
        if(0 == garg_index){
            SAMPLE_COMM_LCD_Stop();
            SAMPLE_COMM_SYS_Exit();
        }
        printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
    }

    exit(0);
}

static TS_VOID SAMPLE_VDEC_Usage(char *sPrgNm)
{
    printf("\n/************************************/\n");
    printf("Usage : %s <index>\n", sPrgNm);
    printf("index:\n");
    printf("\t0: SOFT-VDEC-LCD-DISPLAY\n");
}

static TS_S32 SAMPLE_H264_VDEC_LCD(TS_VOID)
{
    TS_S32      s32Ret = TS_SUCCESS;
    VB_CONFIG_S stVbConf;

    if(isFileExist("/root/128x128.h264")){
        SAMPLE_PRT("File open fail, please copy 128x128.h264 file to /root dir and ensure file property is RW!!!\n");
        return 0;
    }

    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));
    stVbConf.astCommPool[0].u64BlkSize  = 640*360*3/2;
    stVbConf.astCommPool[0].u32BlkCnt   = 1;
    stVbConf.u32MaxPoolCnt = 1;
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("failed!\n");
        SAMPLE_COMM_SYS_Exit();
        return s32Ret;
    }

    CHECK_RET(SAMPLE_COMM_LCD_Init(), "SAMPLE_COMM_LCD_Init");
    CHECK_RET(SAMPLE_COMM_LCD_Start(), "screen_video_start");
    simple_loop_wait();
    CHECK_RET(SAMPLE_COMM_LCD_Stop(), "SAMPLE_COMM_LCD_Stop");
    SAMPLE_COMM_SYS_Exit();
    return s32Ret;
}

/******************************************************************************
* function    : main()
* Description : soft video vdec lcd sample
******************************************************************************/
int main(int argc, char *argv[])
{
    TS_S32 s32Ret = TS_SUCCESS;

	SAMPLE_COMM_SYS_ShowVersion(argv[0]);
    
    if (argc < 2 || argc > 2)
    {
        SAMPLE_VDEC_Usage(argv[0]);
        return TS_FAILURE;
    }

    if (!strncmp(argv[1], "-h", 2))
    {
        SAMPLE_VDEC_Usage(argv[0]);
        return TS_SUCCESS;
    }

    signal(SIGINT, SAMPLE_VDEC_HandleSig);
    signal(SIGTSTP, SAMPLE_VDEC_HandleSig);
    signal(SIGTERM, SAMPLE_VDEC_HandleSig);

    garg_index =atoi(argv[1]);
    /******************************************
     choose the case
    ******************************************/
    switch (garg_index)
    {
        case 0:
        {
            s32Ret = SAMPLE_H264_VDEC_LCD();
            break;
        }

        default :
        {
            SAMPLE_PRT("the index is invaild!\n");
            SAMPLE_VDEC_Usage(argv[0]);
            s32Ret = TS_FAILURE;
            break;
        }
    }

    if (TS_SUCCESS == s32Ret)
    {
        SAMPLE_PRT("program exit normally!\n");
    }
    else
    {
        SAMPLE_PRT("program exit abnormally!\n");
    }

    return s32Ret;
}
