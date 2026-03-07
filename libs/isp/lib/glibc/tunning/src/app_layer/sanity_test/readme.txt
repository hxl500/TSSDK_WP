stest_evtname_octob.h should run /tests/sanity_test/APIParse.ext

1. sanity test function of api set value & api get value.
    1> copy stest_apichk_run_preview_normal.sh & stest_apichk_config_preview_normal.txt to SDCARD.
    2> run sh, ./stest_apichk_run_preview_normal.sh .
    3> waiting for about 15 minites, the api check result is saved in stest_res/perf.txt.

2. sanity test function of ddr process.
    1> copy stest_ddr_run_preview_normal.sh & config_preview_normal_stest_ddr.txt to SDCARD.
    2> run sh, ./stest_ddr_run_preview_normal.sh .
    3> waiting for about 10 minites, ddr process result is saved in
        stest_res/reglist.txt  stest_res/yuvmd5.txt  stest_res/allregvalue.txt.
    Notice,
        config_preview_normal_stest_ddr.txt is generate from manual configuration
            read raw data in sdcard-->load raw buffer to VIN-->Enable DDR Process
            -->Enable ALL Manual Mode-->Start Record Reglist-->Set sensor params
            -->Set ISP params->End Record Reglist-->Read Back Md5 Value
            -->Disable DDR Process