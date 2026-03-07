#include "app_layer.h"

/// Parse Main Arguments
static void App_Layer_Parse_Main_Args(s32 argc, char8* argv[], app_desc_t* pAppDesc)
{
    int arg = 0;

    while ((arg = getopt(argc, argv, "hgf:")) != -1)
    {
        switch (arg)
        {
            case 'h':
            {
                PRINTF("%s", g_usage);
                exit(0);
            }
            case 'g':
            {
                Trace_OS_Register_Signal();

                break;
            }
            case 'f':
            {
                if (optarg && strlen(optarg))
                {
                    memcpy(pAppDesc->cfgFile.sFilename, optarg, strlen(optarg) + 1);
                    APP_CFG("going to read cfg file from %s", pAppDesc->cfgFile.sFilename);
                }
                else
                {
                    APP_ERR("invalid filename %s", optarg);
                }

                break;
            }
            default:
            {
                break;
            }
        }
    }
}

void __attribute__((destructor)) App_Layer_Exit(void)
{
    APP_CFG("exited");
}

s32 main(s32 argc, char8* argv[])
{
    s32 res = 0;

    /* #if defined(__linux__) && !defined(__arm__) */
    #if defined(__linux__) && !(defined(__arm__) || defined(__aarch64__))

    char8* argv2[] =
    {
        "3",
        "-f",
        "./config.txt"
    };

    if (argc <= 1)
    {
        argc = 3;
        argv = argv2;
    }

    #endif

    app_desc_t      appDesc;
    memset(&appDesc, 0, sizeof(app_desc_t));

    App_Layer_Parse_Main_Args(argc, argv, &appDesc);

    res = App_Layer_Process(&appDesc);

    return res;
}
