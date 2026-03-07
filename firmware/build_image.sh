#!/bin/bash
#/home/carzzy/work/5215/mkfw/mkfw -n -v "22.111.34.3" -c flash.config -p vp_flash-partition-table.bin -k fw_rsa.priv -u VP_OTA-upgrade.bin  -f VP_FLASH_flash.bin
#/home/carzzy/work/5215/mkfw/mkfw -n -v "22.111.34.3" -c flash.config -p vp_flash-partition-table.bin -k fw_rsa.priv  -f VP_FLASH_flash.bin
/home/carzzy/work/5215/mkfw/mkfw -n -v "22.111.34.3" -c flash.config -p VP_flash-partition-table.bin   -f VP_FLASH_flash.bin

