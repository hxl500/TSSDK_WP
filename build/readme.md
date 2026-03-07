
通过新增lunch号来适配客户自己的硬件设备，涉及build uboot kernel 3个文件夹
因32位cpu和64位cpu路径有所不同，以下分别介绍：
32位cpu：
以tx5239dv200_ipc_v2p0_release为例介绍如何新建lunch号
tx5239dv200 ----- 芯片型号，芯片上的丝印
ipc ----- 硬件设备名
v2p0 ----- 硬件版本信息
release ----- 软件版本信息，未使用

build目录修改
1、build\device\tx5239dv200\board路径下面添加env_ipc_v2p0_release_32.sh
env_ipc_v2p0_release_32.sh里面的内容介绍下

export TARGET_UBOOT_DEFCONFIG="ts_tx5239dv200_ipc_defconfig"                ---- uboot的config文件
export TARGET_KERNEL_DEFCONFIG="ts_tx52xx_ipc_defconfig"				    ---- kernel的config文件
export TARGET_KERNEL_RAMDISK_DEFCONFIG="ts_tx52xx_ipc_ramdisk_defconfig"    ---- kernel dtb rootfs 打包成ramdisk的配置文件，非ramdisk启动可以不配置
export TARGET_DTB="ts-tx5239dv200-ipc.dtb"									---- 内核设备树dtb的文件名
export TARGET_FIT_ITS="ts-tx5239dv200-ipc.its"							    ---- dtb Image 打包的脚本文件
export TARGET_FIT_ITB="ts-tx5239dv200-ipc.itb"                              ---- dtb image 打包后的文件名
export TARGET_FIT_ITB_RAMDISK="ts-tx5239dv200-ipc-ramdisk.itb"              ---- ramdisk 打包后的文件名,非ramdisk启动可以不配置
export TARGET_IMAGE_TYPE="sflash"											---- 启动介质 此处为norflash启动
export TARGET_ROOTFS_CUSTOM=""

#TARGET_LOADER_2="${TARGET_PREBUILTS_DIR}/tsbin/tx5239dv200/ddr_init.bin"
#TARGET_LOADER_2="${TARGET_PREBUILTS_DIR}/tsbin/tx5239dv200/ddr2_1333.bin"  ---- ddr配置文件，64MB ddr配置文件
TARGET_LOADER_2="${TARGET_PREBUILTS_DIR}/tsbin/tx5239dv200/ddr3_2133.bin"   ---- ddr配置文件，该设备使用的为128MB ddr配置文件

2、build\device\tx5239dv200中的add_lunch_combo.sh 增加lunch号，
lunch号的选择 可以通过build目录下面source envsetup.sh，然后lunch 可以查看当前已使用的号，选择一个未使用的lunch号
add_lunch_combo.sh 脚本内容为：
add_lunch_combo 331.tsingmicro_tx5239dv200_ipc_v2p0_release_32，注意这些字段信息都要和上面1中env_ipc_v2p0_release_32.sh中的字段信息一一对应
如
331 		----- 新增lunch号
tx5239dv200 ----- 芯片名称
ipc 		----- 硬件设备名
v2p0		----- 硬件版本信息
release 	----- 软件版本信息

以上两点修改完后可以在build下面，通过source envsetup.sh，然后lunch 查看是否修改成功

uboot目录修改
1、uboot\configs里面 添加单板配置文件ts_tx5239dv200_ipc_defconfig，客户可以根据提供的sdk中参考config稍作修改
该配置文件中主要修改为CONFIG_SYS_CONFIG_NAME="ts_tx5239dv200_ipc"以及CONFIG_DEFAULT_DEVICE_TREE="ts_tx5239dv200_ipc"
其中CONFIG_SYS_CONFIG_NAME="ts_tx5239dv200_ipc" 用来选择启动的配置头文件即下面第三点中的ts_tx5239dv200_ipc.h 这两个的名字要对应
CONFIG_DEFAULT_DEVICE_TREE="ts_tx5239dv200_ipc" 用来选择启动的设备树文件，即下面第2点中的ts_tx5239dv200_ipc.dts，这两个名字要对应
2、uboot\arch\arm\dts中添加单板设备树配置文件，添加ts_tx52xx_ipc.dtsi  ts-tx52xx-ipc-pinctrl.dtsi ts_tx5215dv200_ipc.dts
3、include\configs中添加单板环境变量配置文件ts_tx5239dv200_ipc.h，可以根据提供的sdk中的.h文件，里面主要涉及启动的环境变量名设置
主要修改为 这些名字和build中1中的文件名yiyi对应
#define CONFIG_EXTRA_ENV_SETTINGS					\
		"boardname=TS-TX5239DV200-IPC\0" \                              ----设备名
		"fip_name=fip_tx5239dv200_ipc.bin\0" \						    ----uboot打包的文件名
		"kernel_name=ts-tx5239dv200-ipc.itb\0"  \                       ----kernel打包的文件名
		"kernel_ramfs_name=ts-tx5239dv200-ipc-ramdisk.itb\0" \          ----ramdisk打包的文件名

4、uboot\fit 中添加打包脚本文件ts-tx5239dv200-ipc.its里面涉及设备树和内核的打包 ，主要修改添加内核dtb的文件名
		tx5239dv200-ipc {
			description = "Flattened Device Tree blob";
			data = /incbin/("./ts-tx5239dv200-ipc.dtb");           ---- 内核设备树dtb的文件名
			type = "flat_dt";
			arch = "arm";
			compression = "none";
			load = <0x01000000>;

32位cpu:kernel目录修改（与uboot目录类似）
1、kernel\arch\arm\configs中添加单板配置文件ts_tx52xx_ipc_defconfig
2、kernel\arch\arm\boot\dts\tsingmicro 中添加dts文件
如ts-tx52xx-ipc.dtsi ts-tx52xx-ipc-pinctrl.dtsi ts-tx5239dv200-ipc.dts三个文件

64位cpu:kernel目录修改（与uboot目录类似）
1、kernel\arch\arm64\configs中添加单板配置文件ts_tx52xx_ipc_defconfig
2、kernel\arch\arm64\boot\dts\tsingmicro 中添加dts文件
如ts-tx52xx-ipc.dtsi ts-tx52xx-ipc-pinctrl.dtsi ts-tx5239dv200-ipc.dts三个文件