/*
 * multi.c -- Multifunction Composite driver
 *
 * Copyright (C) 2008 David Brownell
 * Copyright (C) 2008 Nokia Corporation
 * Copyright (C) 2009 Samsung Electronics
 * Author: Michal Nazarewicz (mina86@mina86.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/usb/composite.h>
#include <linux/usb/video.h>

//multi_uvc_uac  audio 16k

#ifdef CONFIG_USB_CONFIGFS_F_UAC1
#define USE_UAC1  1
#endif

#ifdef CONFIG_USB_CONFIGFS_F_UVC
#define USE_UVC  1
#endif

#ifdef CONFIG_USB_CONFIGFS_F_UAC1

#include "u_uac1.h"


#if USE_UAC1
/* Playback(USB-IN) Default Stereo - Fl/Fr */
static int p_chmask = 0x1;//UAC1_DEF_PCHMASK;
module_param(p_chmask, uint, S_IRUGO); //S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(p_chmask, "Playback Channel Mask");

/* Playback Default 48 KHz */
static int p_srate = 8000;//16000;//UAC1_DEF_PSRATE;
module_param(p_srate, uint, S_IRUGO);
MODULE_PARM_DESC(p_srate, "Playback Sampling Rate");

/* Playback Default 16bits/sample */
static int p_ssize = 2;//UAC1_DEF_PSSIZE;
module_param(p_ssize, uint, S_IRUGO);
MODULE_PARM_DESC(p_ssize, "Playback Sample Size(bytes)");

/* Capture(USB-OUT) Default Stereo - Fl/Fr */
static int c_chmask = 0x1;//UAC1_DEF_CCHMASK;
module_param(c_chmask, uint, S_IRUGO);
MODULE_PARM_DESC(c_chmask, "Capture Channel Mask");

/* Capture Default 48 KHz */
static int c_srate = 8000;//16000;//UAC1_DEF_CSRATE;
module_param(c_srate, uint, S_IRUGO);
MODULE_PARM_DESC(c_srate, "Capture Sampling Rate");

/* Capture Default 16bits/sample */
static int c_ssize = 2;//UAC1_DEF_CSSIZE;
module_param(c_ssize, uint, S_IRUGO);
MODULE_PARM_DESC(c_ssize, "Capture Sample Size(bytes)");
#endif
#endif

#ifdef CONFIG_USB_CONFIGFS_F_UVC
#include "u_uvc.h"
#if USE_UVC
/* module parameters specific to the Video streaming endpoint */
static unsigned int streaming_interval = 1;
module_param(streaming_interval, uint, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(streaming_interval, "1 - 16");

#ifdef CONFIG_USB_FS_SPEED
	static unsigned int streaming_maxpacket = 512;//1020;//2808;//2552;//2040
#else
	static unsigned int streaming_maxpacket = 1020;//1020;//2808;//2552;//2040
#endif
module_param(streaming_maxpacket, uint, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(streaming_maxpacket, "1 - 1023 (FS), 1 - 3072 (hs/ss)");

static unsigned int streaming_maxburst;
module_param(streaming_maxburst, uint, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(streaming_maxburst, "0 - 15 (ss only)");

static unsigned int trace;
module_param(trace, uint, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(trace, "Trace level bitmask");


DECLARE_UVC_HEADER_DESCRIPTOR(1);

static const struct UVC_HEADER_DESCRIPTOR(1) uvc_control_header = {
	.bLength		= UVC_DT_HEADER_SIZE(1),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VC_HEADER,
	.bcdUVC			= cpu_to_le16(0x0100),
	.wTotalLength		= 0, /* dynamic */
	.dwClockFrequency	= cpu_to_le32(48000000),
	.bInCollection		= 0, /* dynamic */
	.baInterfaceNr[0]	= 0, /* dynamic */
};

static const struct uvc_camera_terminal_descriptor uvc_camera_terminal = {
	.bLength		= UVC_DT_CAMERA_TERMINAL_SIZE(3),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VC_INPUT_TERMINAL,
	.bTerminalID		= 1,
	.wTerminalType		= cpu_to_le16(0x0201),
	.bAssocTerminal		= 0,
	.iTerminal		= 0,
	.wObjectiveFocalLengthMin	= cpu_to_le16(0),
	.wObjectiveFocalLengthMax	= cpu_to_le16(0),
	.wOcularFocalLength		= cpu_to_le16(0),
	.bControlSize		= 3, 
	.bmControls[0]		= 0x0,  //2
	.bmControls[1]		= 0x00,
	.bmControls[2]		= 0x0,
};

#if USE_UVC_XU

// termail link:
//    0 -> UVC_CT_ID(1) -> UVC_PU_ID(2) -> UVC_OT_ID(3)
//										-> UVC_XU_ID(4) 

/* d399afe9-8c1c-42a9-8bf3-d781e34b7114
	* gen from https://www.guidgenerator.com/online-guid-generator.aspx
	* Need to use customer's guid */
#define GUID_UVCX_TS_XU {0xd3, 0x99, 0xaf, 0xe9, 0x8c,	\
					0x1c, 0x42, 0xa9, 0x8b, 0xf3,	\
					0xd7, 0x81, 0xe3, 0x4b, 0x71,	\
					0x14}

static struct UVC_EXTENSION_UNIT_DESCRIPTOR(1,2) uvc_extension_unit = {
    .bLength = UVC_DT_EXTENSION_UNIT_SIZE(1,2),
    .bDescriptorType = USB_DT_CS_INTERFACE,
    .bDescriptorSubType = UVC_VC_EXTENSION_UNIT,
    .bUnitID = 4,
    .guidExtensionCode = GUID_UVCX_TS_XU,
    .bNumControls = 0x05,
    .bNrInPins = 0x01,
    .baSourceID[0] = 2,
    .bControlSize = 0x02,
    .bmControls[0] = 0x1F,
    .bmControls[1] = 0x00,
    .iExtension = 0x00,
};
#endif

static const struct uvc_processing_unit_descriptor uvc_processing = {
	.bLength		= UVC_DT_PROCESSING_UNIT_SIZE(2),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VC_PROCESSING_UNIT,
	.bUnitID		= 2,//UVC_PU_ID
	.bSourceID		= 1,
	.wMaxMultiplier		= cpu_to_le16(16*1024),
	.bControlSize		= 2,
	.bmControls[0]		= 0, //1
	.bmControls[1]		= 0,
	.iProcessing		= 0,
};

static const struct uvc_output_terminal_descriptor uvc_output_terminal = {
	.bLength		= UVC_DT_OUTPUT_TERMINAL_SIZE,
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VC_OUTPUT_TERMINAL,
	.bTerminalID		= 3,
	.wTerminalType		= cpu_to_le16(0x0101),
	.bAssocTerminal		= 0,
	.bSourceID		= 2,
	.iTerminal		= 0,
};

DECLARE_UVC_INPUT_HEADER_DESCRIPTOR(1, 2);

static const struct UVC_INPUT_HEADER_DESCRIPTOR(1, 2) uvc_input_header = {
	.bLength		= UVC_DT_INPUT_HEADER_SIZE(1, 2),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_INPUT_HEADER,
	.bNumFormats		= 2,//jpeg_format
	.wTotalLength		= 0, /* dynamic */
	.bEndpointAddress	= 0, /* dynamic */
	.bmInfo			= 0,
	.bTerminalLink		= 3,
	.bStillCaptureMethod	= 0,
	.bTriggerSupport	= 0,
	.bTriggerUsage		= 0,
	.bControlSize		= 1,
	.bmaControls[0][0]	= 0,
	//.bmaControls[1][0]	= 4,
};

static const struct uvc_format_uncompressed uvc_format_yuv = {
	.bLength		= UVC_DT_FORMAT_UNCOMPRESSED_SIZE,
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FORMAT_UNCOMPRESSED,
	.bFormatIndex		= 1,
	.bNumFrameDescriptors	= 2,
	.guidFormat		=
		{ 'Y',  'U',  'Y',  '2', 0x00, 0x00, 0x10, 0x00,
		 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71},
	.bBitsPerPixel		= 16,
	.bDefaultFrameIndex	= 1,
	.bAspectRatioX		= 0,
	.bAspectRatioY		= 0,
	.bmInterfaceFlags	= 0,
	.bCopyProtect		= 0,
};

DECLARE_UVC_FRAME_UNCOMPRESSED(1);
DECLARE_UVC_FRAME_UNCOMPRESSED(3);

static const struct UVC_FRAME_UNCOMPRESSED(3) uvc_frame_yuv_360p = {
	.bLength		= UVC_DT_FRAME_UNCOMPRESSED_SIZE(3),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_UNCOMPRESSED,
	.bFrameIndex		= 1,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(640),
	.wHeight		= cpu_to_le16(360),
	.dwMinBitRate		= cpu_to_le32(18432000),
	.dwMaxBitRate		= cpu_to_le32(55296000),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(460800),
	.dwDefaultFrameInterval	= cpu_to_le32(666666),
	.bFrameIntervalType	= 3,
	.dwFrameInterval[0]	= cpu_to_le32(666666),
	.dwFrameInterval[1]	= cpu_to_le32(1000000),
	.dwFrameInterval[2]	= cpu_to_le32(5000000),
};

static const struct UVC_FRAME_UNCOMPRESSED(1) uvc_frame_yuv_720p = {
	.bLength		= UVC_DT_FRAME_UNCOMPRESSED_SIZE(1),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_UNCOMPRESSED,
	.bFrameIndex		= 2,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(1280),
	.wHeight		= cpu_to_le16(720),
	.dwMinBitRate		= cpu_to_le32(29491200),
	.dwMaxBitRate		= cpu_to_le32(29491200),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(1843200),
	.dwDefaultFrameInterval	= cpu_to_le32(500000),
	.bFrameIntervalType	= 1,
	.dwFrameInterval[0]	= cpu_to_le32(500000),
};

static const struct uvc_format_mjpeg uvc_format_mjpg = {
	.bLength		= UVC_DT_FORMAT_MJPEG_SIZE,
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FORMAT_MJPEG,
	.bFormatIndex		= 1,
	.bNumFrameDescriptors	= 12,
	.bmFlags		= 0,
	.bDefaultFrameIndex	= 1,
	.bAspectRatioX		= 0,
	.bAspectRatioY		= 0,
	.bmInterfaceFlags	= 0,
	.bCopyProtect		= 0,
};

//支持的帧率类型
DECLARE_UVC_FRAME_MJPEG(1);
DECLARE_UVC_FRAME_MJPEG(3);

//1280 x 720
static const struct UVC_FRAME_MJPEG(1) uvc_frame_mjpg_720p_1280x720 = {
	.bLength		= UVC_DT_FRAME_MJPEG_SIZE(1),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
	.bFrameIndex		= 1,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(1280),
	.wHeight		= cpu_to_le16(720),
	.dwMinBitRate		= cpu_to_le32(29491200),
	.dwMaxBitRate		= cpu_to_le32(29491200),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(1843200),
	.dwDefaultFrameInterval	= cpu_to_le32(500000),
	.bFrameIntervalType	= 1,
	.dwFrameInterval[0]	= cpu_to_le32(500000),
};

//864 x 480
static const struct UVC_FRAME_MJPEG(1) uvc_frame_mjpg_480p_864x480 = {
	.bLength		= UVC_DT_FRAME_MJPEG_SIZE(1),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
	.bFrameIndex		= 2,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(864),
	.wHeight		= cpu_to_le16(480),
	.dwMinBitRate		= cpu_to_le32(864*480*2*8*5),
	.dwMaxBitRate		= cpu_to_le32(864*480*2*8*15),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(829440),
	.dwDefaultFrameInterval	= cpu_to_le32(500000),
	.bFrameIntervalType	= 1,
	.dwFrameInterval[0]	= cpu_to_le32(500000),
};

//854 x 480
static const struct UVC_FRAME_MJPEG(1) uvc_frame_mjpg_480p_854x480 = {
	.bLength		= UVC_DT_FRAME_MJPEG_SIZE(1),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
	.bFrameIndex		= 3,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(854),
	.wHeight		= cpu_to_le16(480),
	.dwMinBitRate		= cpu_to_le32(854*480*2*8*5),
	.dwMaxBitRate		= cpu_to_le32(854*480*2*8*15),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(819840),
	.dwDefaultFrameInterval	= cpu_to_le32(500000),
	.bFrameIntervalType	= 1,
	.dwFrameInterval[0]	= cpu_to_le32(500000),
};

//800 x 480
static const struct UVC_FRAME_MJPEG(1) uvc_frame_mjpg_480p_800x480 = {
	.bLength		= UVC_DT_FRAME_MJPEG_SIZE(1),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
	.bFrameIndex		= 4,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(800),
	.wHeight		= cpu_to_le16(480),
	.dwMinBitRate		= cpu_to_le32(800*480*2*8*5),
	.dwMaxBitRate		= cpu_to_le32(800*480*2*8*15),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(768000),
	.dwDefaultFrameInterval	= cpu_to_le32(500000),
	.bFrameIntervalType	= 1,
	.dwFrameInterval[0]	= cpu_to_le32(500000),
};

//480 x 800
static const struct UVC_FRAME_MJPEG(1) uvc_frame_mjpg_480p_480x800 = {
	.bLength		= UVC_DT_FRAME_MJPEG_SIZE(1),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
	.bFrameIndex		= 5,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(480),
	.wHeight		= cpu_to_le16(800),
	.dwMinBitRate		= cpu_to_le32(480*800*2*8*5),
	.dwMaxBitRate		= cpu_to_le32(480*800*2*8*15),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(768000),
	.dwDefaultFrameInterval	= cpu_to_le32(500000),
	.bFrameIntervalType	= 1,
	.dwFrameInterval[0]	= cpu_to_le32(500000),
};

//640 x 480
static const struct UVC_FRAME_MJPEG(1) uvc_frame_mjpg_480p_640x480 = {
	.bLength		= UVC_DT_FRAME_MJPEG_SIZE(1),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
	.bFrameIndex		= 6,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(640),
	.wHeight		= cpu_to_le16(480),
	.dwMinBitRate		= cpu_to_le32(640*480*2*8*5),
	.dwMaxBitRate		= cpu_to_le32(640*480*2*8*15),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(614400),
	.dwDefaultFrameInterval	= cpu_to_le32(500000),
	.bFrameIntervalType	= 1,
	.dwFrameInterval[0]	= cpu_to_le32(500000),
};

//480 x 640
static const struct UVC_FRAME_MJPEG(1) uvc_frame_mjpg_480p_480x640 = {
	.bLength		= UVC_DT_FRAME_MJPEG_SIZE(1),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
	.bFrameIndex		= 7,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(480),
	.wHeight		= cpu_to_le16(640),
	.dwMinBitRate		= cpu_to_le32(480*640*2*8*5),
	.dwMaxBitRate		= cpu_to_le32(480*640*2*8*15),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(614400),
	.dwDefaultFrameInterval	= cpu_to_le32(500000),
	.bFrameIntervalType	= 1,
	.dwFrameInterval[0]	= cpu_to_le32(500000),
};

//480 x 320
static const struct UVC_FRAME_MJPEG(1) uvc_frame_mjpg_320p_480x320 = {
	.bLength		= UVC_DT_FRAME_MJPEG_SIZE(1),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
	.bFrameIndex		= 8,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(480),
	.wHeight		= cpu_to_le16(320),
	.dwMinBitRate		= cpu_to_le32(480*320*2*8*5),
	.dwMaxBitRate		= cpu_to_le32(480*320*2*8*15),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(307200),
	.dwDefaultFrameInterval	= cpu_to_le32(500000),
	.bFrameIntervalType	= 1,
	.dwFrameInterval[0]	= cpu_to_le32(500000),
};
//320 x 480
static const struct UVC_FRAME_MJPEG(1) uvc_frame_mjpg_320p_320x480 = {
	.bLength		= UVC_DT_FRAME_MJPEG_SIZE(1),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
	.bFrameIndex		= 9,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(320),
	.wHeight		= cpu_to_le16(480),
	.dwMinBitRate		= cpu_to_le32(320*480*2*8*5),
	.dwMaxBitRate		= cpu_to_le32(320*480*2*8*15),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(307200),
	.dwDefaultFrameInterval	= cpu_to_le32(500000),
	.bFrameIntervalType	= 1,
	.dwFrameInterval[0]	= cpu_to_le32(500000),
};

//240 x 320
static const struct UVC_FRAME_MJPEG(1) uvc_frame_mjpg_240p_240x320 = {
	.bLength		= UVC_DT_FRAME_MJPEG_SIZE(1),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
	.bFrameIndex		= 10,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(240),
	.wHeight		= cpu_to_le16(320),
	.dwMinBitRate		= cpu_to_le32(240*320*2*8*5),
	.dwMaxBitRate		= cpu_to_le32(240*320*2*8*15),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(153600),
	.dwDefaultFrameInterval	= cpu_to_le32(500000),
	.bFrameIntervalType	= 1,
	.dwFrameInterval[0]	= cpu_to_le32(500000),
};

//320 x 240
static const struct UVC_FRAME_MJPEG(1) uvc_frame_mjpg_240p_320x240 = {
	.bLength		= UVC_DT_FRAME_MJPEG_SIZE(1),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
	.bFrameIndex		= 11,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(320),
	.wHeight		= cpu_to_le16(240),
	.dwMinBitRate		= cpu_to_le32(320*240*2*8*5),
	.dwMaxBitRate		= cpu_to_le32(320*240*2*8*15),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(153600),
	.dwDefaultFrameInterval	= cpu_to_le32(500000),
	.bFrameIntervalType	= 1,
	.dwFrameInterval[0]	= cpu_to_le32(500000),
};

//640 x 360
static const struct UVC_FRAME_MJPEG(1) uvc_frame_mjpg_360p_640x360 = {
	.bLength		= UVC_DT_FRAME_MJPEG_SIZE(1),
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_FRAME_MJPEG,
	.bFrameIndex		= 12,
	.bmCapabilities		= 0,
	.wWidth			= cpu_to_le16(640),
	.wHeight		= cpu_to_le16(360),
	.dwMinBitRate		= cpu_to_le32(640*360*2*8*5),
	.dwMaxBitRate		= cpu_to_le32(640*360*2*8*15),
	.dwMaxVideoFrameBufferSize	= cpu_to_le32(614400),
	.dwDefaultFrameInterval	= cpu_to_le32(500000),
	.bFrameIntervalType	= 1,
	.dwFrameInterval[0]	= cpu_to_le32(500000),
};

static const struct uvc_color_matching_descriptor uvc_color_matching = {
	.bLength		= UVC_DT_COLOR_MATCHING_SIZE,
	.bDescriptorType	= USB_DT_CS_INTERFACE,
	.bDescriptorSubType	= UVC_VS_COLORFORMAT,
	.bColorPrimaries	= 1,
	.bTransferCharacteristics	= 1,
	.bMatrixCoefficients	= 4,
};

static const struct uvc_descriptor_header * const uvc_fs_control_cls[] = {
	(const struct uvc_descriptor_header *) &uvc_control_header,
	(const struct uvc_descriptor_header *) &uvc_camera_terminal,
	(const struct uvc_descriptor_header *) &uvc_processing,
#if USE_UVC_XU
	(const struct uvc_descriptor_header *) &uvc_extension_unit,
#endif
	(const struct uvc_descriptor_header *) &uvc_output_terminal,
	NULL,
};

static const struct uvc_descriptor_header * const uvc_ss_control_cls[] = {
	(const struct uvc_descriptor_header *) &uvc_control_header,
	(const struct uvc_descriptor_header *) &uvc_camera_terminal,
	(const struct uvc_descriptor_header *) &uvc_processing,
#if USE_UVC_XU
	(const struct uvc_descriptor_header *) &uvc_extension_unit,
#endif
	(const struct uvc_descriptor_header *) &uvc_output_terminal,
	NULL,
};

static const struct uvc_descriptor_header * const uvc_fs_streaming_cls[] = {
	(const struct uvc_descriptor_header *) &uvc_input_header,
	//(const struct uvc_descriptor_header *) &uvc_format_yuv,
	//(const struct uvc_descriptor_header *) &uvc_frame_yuv_360p,
	//(const struct uvc_descriptor_header *) &uvc_frame_yuv_720p,
	(const struct uvc_descriptor_header *) &uvc_format_mjpg,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_720p_1280x720,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_480p_864x480,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_480p_854x480,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_480p_800x480,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_480p_480x800,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_480p_640x480,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_480p_480x640,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_320p_480x320,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_320p_320x480,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_240p_240x320,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_240p_320x240,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_360p_640x360,
	(const struct uvc_descriptor_header *) &uvc_color_matching,
	NULL,
};

static const struct uvc_descriptor_header * const uvc_hs_streaming_cls[] = {
	(const struct uvc_descriptor_header *) &uvc_input_header,
	//(const struct uvc_descriptor_header *) &uvc_format_yuv,
	//(const struct uvc_descriptor_header *) &uvc_frame_yuv_360p,
	//(const struct uvc_descriptor_header *) &uvc_frame_yuv_720p,
	(const struct uvc_descriptor_header *) &uvc_format_mjpg,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_720p_1280x720,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_480p_864x480,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_480p_854x480,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_480p_800x480,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_480p_480x800,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_480p_640x480,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_480p_480x640,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_320p_480x320,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_320p_320x480,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_240p_240x320,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_240p_320x240,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_360p_640x360,
	(const struct uvc_descriptor_header *) &uvc_color_matching,
	NULL,
};

static const struct uvc_descriptor_header * const uvc_ss_streaming_cls[] = {
	(const struct uvc_descriptor_header *) &uvc_input_header,
	//(const struct uvc_descriptor_header *) &uvc_format_yuv,
	//(const struct uvc_descriptor_header *) &uvc_frame_yuv_360p,
	//(const struct uvc_descriptor_header *) &uvc_frame_yuv_720p,
	(const struct uvc_descriptor_header *) &uvc_format_mjpg,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_720p_1280x720,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_480p_864x480,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_480p_854x480,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_480p_800x480,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_480p_480x800,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_480p_640x480,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_480p_480x640,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_320p_480x320,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_320p_320x480,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_240p_240x320,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_240p_320x240,
	(const struct uvc_descriptor_header *) &uvc_frame_mjpg_360p_640x360,
	(const struct uvc_descriptor_header *) &uvc_color_matching,
	NULL,
};



#endif
#endif


#define DRIVER_DESC		"Multifunction Composite UVC_UAC Gadget"


USB_GADGET_COMPOSITE_OPTIONS();

/***************************** Device Descriptor ****************************/

#define MULTI_UVC_UAC_VENDOR_NUM	0x1d6b	/* Linux Foundation */
#define MULTI_UVC_UAC_PRODUCT_NUM	0x0104	/* Multifunction Composite Gadget */


enum {
	__MULTI_NO_CONFIG = 0,
	MULTI_UVC_UAC_CONFIG_NUM = 1 ,  //config

};


static struct usb_device_descriptor device_desc = {
	.bLength =		sizeof device_desc,
	.bDescriptorType =	USB_DT_DEVICE,

		/* .bcdUSB = DYNAMIC */
	.bcdDevice		= cpu_to_le16(0x0200),

	.bDeviceClass =		USB_CLASS_MISC /* 0xEF */,
	.bDeviceSubClass =	2,
	.bDeviceProtocol =	1,

	/* Vendor and product id can be overridden by module parameters.  */
	.idVendor =		cpu_to_le16(MULTI_UVC_UAC_VENDOR_NUM),
	.idProduct =	cpu_to_le16(MULTI_UVC_UAC_PRODUCT_NUM),
};



enum {
	MULTI_STRING_UVC_UAC_CONFIG_IDX = USB_GADGET_FIRST_AVAIL_IDX,

};

static struct usb_string strings_dev[] = {
	[USB_GADGET_MANUFACTURER_IDX].s = "Linux Foundation",
	[USB_GADGET_PRODUCT_IDX].s = DRIVER_DESC,
	[USB_GADGET_SERIAL_IDX].s = "1234567890",
	[MULTI_STRING_UVC_UAC_CONFIG_IDX].s = "Multifunction with UVC AND UAC",
	{  } /* end of list */
};





static struct usb_gadget_strings stringtab_dev = {
	.language	= 0x0409,	/* en-us */
	.strings	= strings_dev,
};




static struct usb_gadget_strings *dev_uvc_uac_strings[] = {
	&stringtab_dev,
	NULL,
};


/****************************** Configurations ******************************/

static const struct usb_descriptor_header *otg_desc[2];



#if USE_UAC1

static struct usb_function_instance *fi_uac1;
static struct usb_function *f_uac1;
#endif

#if USE_UVC


static struct usb_function_instance *fi_uvc;
static struct usb_function *f_uvc;


#endif
#if 1

static int uvc_uac_do_config(struct usb_configuration *c)
{
	int ret = 0;



	if (gadget_is_otg(c->cdev->gadget)) {
		c->descriptors = otg_desc;
		c->bmAttributes |= USB_CONFIG_ATT_WAKEUP;
	}


#if USE_UVC
	
			f_uvc = usb_get_function(fi_uvc);
			if (IS_ERR(f_uvc))
				return PTR_ERR(f_uvc);
		
			ret = usb_add_function(c, f_uvc);
			if (ret < 0){
				usb_put_function(f_uvc);
			}
#endif

	
#if USE_UAC1
				
			f_uac1 = usb_get_function(fi_uac1);
			if (IS_ERR(f_uac1)) {
				ret = PTR_ERR(f_uac1);
				return ret;
			}
		
			ret = usb_add_function(c, f_uac1);
			if (ret < 0) {
				usb_put_function(f_uac1);
				return ret;
			}
#endif



	return ret;
}

static __ref int uvc_uac_config_register(struct usb_composite_dev *cdev)
{
	static struct usb_configuration config = {
		.bConfigurationValue	= 1,//MULTI_UVC_UAC_CONFIG_NUM,
		.bmAttributes		= USB_CONFIG_ATT_SELFPOWER,
		.MaxPower		= CONFIG_USB_GADGET_VBUS_DRAW,
	};

	config.label          = strings_dev[MULTI_STRING_UVC_UAC_CONFIG_IDX].s;
	config.iConfiguration = strings_dev[MULTI_STRING_UVC_UAC_CONFIG_IDX].id;
	dev_info(&cdev->gadget->dev, "adding config #%u '%s'/%p\n",
			config.bConfigurationValue,
			config.label, &config);

	return usb_add_config(cdev, &config, uvc_uac_do_config);
}



#endif







/****************************** Gadget Bind ******************************/

static int __ref multi_uac_uvc_bind(struct usb_composite_dev *cdev)
{
	struct usb_gadget *gadget = cdev->gadget;
#if USE_UAC1	
	struct f_uac1_opts	*uac1_opts;
#endif

#if USE_UVC
	struct f_uvc_opts *uvc_opts;
#endif

	int status;
	
	

#if USE_UVC
						dev_info(&gadget->dev, "multi_uac_uvc_bind UVC\n");
					
						fi_uvc = usb_get_function_instance("uvc");
						if (IS_ERR(fi_uvc))
							return PTR_ERR(fi_uvc);
					
						uvc_opts = container_of(fi_uvc, struct f_uvc_opts, func_inst);
#if USE_UVC
						uvc_opts->streaming_interval = streaming_interval;
						uvc_opts->streaming_maxpacket = streaming_maxpacket;
						uvc_opts->streaming_maxburst = streaming_maxburst;
						uvc_set_trace_param(trace);
					
						uvc_opts->fs_control = uvc_fs_control_cls;
						uvc_opts->ss_control = uvc_ss_control_cls;
						uvc_opts->fs_streaming = uvc_fs_streaming_cls;
						uvc_opts->hs_streaming = uvc_hs_streaming_cls;
						uvc_opts->ss_streaming = uvc_ss_streaming_cls;
#endif
#endif




#if USE_UAC1
				dev_info(&gadget->dev, "multi_uac_uvc_bind UAC1\n");
			
				fi_uac1 = usb_get_function_instance("uac1");
				if (IS_ERR(fi_uac1))
					return PTR_ERR(fi_uac1);
				uac1_opts = container_of(fi_uac1, struct f_uac1_opts, func_inst);
#if USE_UAC1
				//uac1_opts = container_of(fi_uac1, struct f_uac1_opts, func_inst);
				uac1_opts->p_chmask = p_chmask;
				uac1_opts->p_srate = p_srate;
				uac1_opts->p_ssize = p_ssize;
				uac1_opts->c_chmask = c_chmask;
				uac1_opts->c_srate = c_srate;
				uac1_opts->c_ssize = c_ssize;
				uac1_opts->req_number = UAC1_DEF_REQ_NUM;
#endif
			
#endif



	/* allocate string IDs */
	status = usb_string_ids_tab(cdev, strings_dev);
	if (unlikely(status < 0))
		goto fail;
	device_desc.iProduct = strings_dev[USB_GADGET_PRODUCT_IDX].id;
	device_desc.iManufacturer = strings_dev[USB_GADGET_MANUFACTURER_IDX].id;
	//USB_GADGET_SERIAL_IDX

	if (gadget_is_otg(gadget) && !otg_desc[0]) {
		struct usb_descriptor_header *usb_desc;

		usb_desc = usb_otg_descriptor_alloc(gadget);
		if (!usb_desc)
			goto fail;
		usb_otg_descriptor_init(gadget, usb_desc);
		otg_desc[0] = usb_desc;
		otg_desc[1] = NULL;
	}

	/* register configurations */
	status = uvc_uac_config_register(cdev);
	if (unlikely(status < 0)){
			dev_info(&gadget->dev, "uvc_uac_config_register failed \n");
		goto fail_otg_desc;
	}
	usb_composite_overwrite_options(cdev, &coverwrite);

	/* we're done */
	dev_info(&gadget->dev, DRIVER_DESC "\n");
	return 0;


	/* error recovery */
fail_otg_desc:
	kfree(otg_desc[0]);
	otg_desc[0] = NULL;
fail:
#if USE_UAC1

	usb_put_function_instance(fi_uac1);
#endif
#if USE_UVC

	usb_put_function_instance(fi_uvc);
#endif
	return status;
}

static int multi_uac_uvc_unbind(struct usb_composite_dev *cdev)
{


#if USE_UVC	
			if (!IS_ERR_OR_NULL(f_uvc))
				usb_put_function(f_uvc);
			if (!IS_ERR_OR_NULL(fi_uvc))
				usb_put_function_instance(fi_uvc);
#endif


	
#if USE_UAC1
		
			if (!IS_ERR_OR_NULL(f_uac1))
				usb_put_function(f_uac1);
			if (!IS_ERR_OR_NULL(fi_uac1))
				usb_put_function_instance(fi_uac1);
#endif
	
	


	kfree(otg_desc[0]);
	otg_desc[0] = NULL;

	return 0;
}




static struct usb_composite_driver multi_uvc_uac_driver = {
	.name		= "g_multi_uvc_uac",
	.dev		= &device_desc,
	.strings	= dev_uvc_uac_strings,
	.max_speed	= USB_SPEED_SUPER,
	.bind		= multi_uac_uvc_bind,
	.unbind		= multi_uac_uvc_unbind,
	.needs_serial	= 1,
};

module_usb_composite_driver(multi_uvc_uac_driver);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_AUTHOR("Michal Nazarewicz");
MODULE_LICENSE("GPL");


