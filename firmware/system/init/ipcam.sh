export PATH=/system/system/bin:$PATH
export LD_LIBRARY_PATH=/system/system/lib:/mnt/lib:$LD_LIBRARY_PATH

devmem 0xf24400a0 32 0xa04e
devmem 0xf24400a4 32 0xc0ef

/system/system/init/load_ko start 
insmod /system/system/bin/vp_logger.ko
sleep 2
vp_logcat app &
vp_project > /dev/vp_log &
