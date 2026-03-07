#!/bin/bash
# info    : =====
# warning : ?????
# error   : *****

# help:
#     in build:
#         source envsetup.sh
#         lunch xxx
#         source tools/ko_all_kern_chk/ko_chk_all.sh
#     in any folder
#         ko_chk_all		# (generagte result in out/result_ko_wrap)

source ${TARGET_BUILD_DIR}/tools/ko_all_kern_chk/ko_chk.sh

OUT_PATH="$TARGET_OUT_DIR/result_ko_wrap"

KO_PATH_5368="
	${TARGET_LIBS_DIR}/drv/crypto/cryptodev
	${TARGET_LIBS_DIR}/drv/crypto/osr_hash
	${TARGET_LIBS_DIR}/drv/crypto/osr_pke
	${TARGET_LIBS_DIR}/drv/crypto/osr_ske
	${TARGET_LIBS_DIR}/drv/crypto/osr_trng
	${TARGET_LIBS_DIR}/drv/sys
	${TARGET_LIBS_DIR}/drv/vpe
	${TARGET_LIBS_DIR}/drv/audio/codec/ts_i2s
	${TARGET_LIBS_DIR}/drv/gdc
	${TARGET_LIBS_DIR}/drv/cve
	${TARGET_LIBS_DIR}/drv/rne_main
	${TARGET_LIBS_DIR}/drv/noc_bw
	${TARGET_LIBS_DIR}/drv/rce
	${TARGET_LIBS_DIR}/drv/hwacc
	${TARGET_LIBS_DIR}/mpp/proc
	${TARGET_LIBS_DIR}/isp/lin_i5/kernel
	${TARGET_LIBS_DIR}/gpu/hal/os/linux/kernel
	${TARGET_LIBS_DIR}/vpu/vdi/linux/driver/
"

KO_PATH_5215="
	${TARGET_LIBS_DIR}/drv/crypto/cryptodev
	${TARGET_LIBS_DIR}/drv/crypto/osr_hash
	${TARGET_LIBS_DIR}/drv/crypto/osr_pke
	${TARGET_LIBS_DIR}/drv/crypto/osr_ske
	${TARGET_LIBS_DIR}/drv/sys
	${TARGET_LIBS_DIR}/drv/vpe_tx5215
	${TARGET_LIBS_DIR}/drv/audio/codec/ts_codec/
	${TARGET_LIBS_DIR}/drv/amr
	${TARGET_LIBS_DIR}/drv/cve
	${TARGET_LIBS_DIR}/drv/rne_main
	${TARGET_LIBS_DIR}/mpp/proc
	${TARGET_LIBS_DIR}/isp/lin_i5_ts/kernel
	${TARGET_LIBS_DIR}/vpu/vdi_v1/h264_cdc_drv/linux_reference/kernel_module
	${TARGET_LIBS_DIR}/vpu/vdi_v1/h265_kern
"

KO_PATH_5112="
	${TARGET_LIBS_DIR}/drv/crypto/cryptodev
	${TARGET_LIBS_DIR}/drv/crypto/osr_ske
	${TARGET_LIBS_DIR}/drv/sys
	${TARGET_LIBS_DIR}/drv/vpe_tx5112
	${TARGET_LIBS_DIR}/drv/audio/codec/ts_codec/
	${TARGET_LIBS_DIR}/drv/amr
	${TARGET_LIBS_DIR}/drv/rne_main
	${TARGET_LIBS_DIR}/mpp/proc
	${TARGET_LIBS_DIR}/isp/lin_i5_ts/kernel
	${TARGET_LIBS_DIR}/vpu/vdi_v2/h264_cdc_drv/linux_reference/kernel_module
	${TARGET_LIBS_DIR}/vpu/vdi_v2/h265_kern
"

KO_PATH_5336="
	${TARGET_LIBS_DIR}/drv/sys
	${TARGET_LIBS_DIR}/drv/vpe_tx5336
	${TARGET_LIBS_DIR}/drv/audio/codec/ts_codec_v2/
	${TARGET_LIBS_DIR}/drv/gdc
	${TARGET_LIBS_DIR}/drv/cve
	${TARGET_LIBS_DIR}/drv/bis
	${TARGET_LIBS_DIR}/drv/crypto/cryptodev
	${TARGET_LIBS_DIR}/drv/crypto/osr_hash
	${TARGET_LIBS_DIR}/drv/crypto/osr_pke
	${TARGET_LIBS_DIR}/drv/crypto/osr_ske
	${TARGET_LIBS_DIR}/drv/crypto/osr_trng
	${TARGET_LIBS_DIR}/drv/rne_main2
	${TARGET_LIBS_DIR}/drv/noc
	${TARGET_LIBS_DIR}/drv/rce
	${TARGET_LIBS_DIR}/drv/ebd
	${TARGET_LIBS_DIR}/drv/amr_v2
	${TARGET_LIBS_DIR}/drv/hwacc
	${TARGET_LIBS_DIR}/mpp/proc
	${TARGET_LIBS_DIR}/isp/lin_i51/kernel
	${TARGET_LIBS_DIR}/vpu/vdi_v3/linlon_kern
"

function ko_chk_all()
{
	rm -rf $OUT_PATH
	mkdir -p $OUT_PATH
	local target_path
	local relative_path

	echo -e "\n\n"
	echo -e "=============================================================="
	echo -e "\e[7m=====Generate ko check result to $OUT_PATH=====\e[0m"
	echo -e "=============================================================="

	case ${TARGET_CHIPSERIES} in
	TX5339AX | TX5368AX | TX5335AX)
		target_path=$KO_PATH_5368
		;;
	TX5239BX | TX5239CX | TX5215BX)
		target_path=$KO_PATH_5215
		;;
	TX5239DX | TX5112DX)
		target_path=$KO_PATH_5112
		;;
	TX5336EX)
		target_path=$KO_PATH_5336
		;;
	*)
		echo "err, need right name (${TARGET_CHIPSERIES})"
	esac

	for path in $target_path; do
		cd $path
		ko_chk
		relative_path=${path#${TARGET_LIBS_DIR}/}
		relative_path=${relative_path%/*}
		mkdir -p ${OUT_PATH}/${relative_path}
		mv tmp ${OUT_PATH}/${relative_path}/${path##*/}
	done
}