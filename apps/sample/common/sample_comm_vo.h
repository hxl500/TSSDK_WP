

#ifndef __SAMPLE_COMM_VO_H__
#define __SAMPLE_COMM_VO_H__

#include <pthread.h>

#include "ts_comm_vo.h"
#include "mpi_vo.h"


typedef enum tsSAMPLE_VO_MODE_E {
	VO_MODE_1MUX,
	VO_MODE_2MUX,
	VO_MODE_3MUX,
	VO_MODE_4MUX,
	VO_MODE_8MUX,
	VO_MODE_9MUX,
	VO_MODE_16MUX,
	VO_MODE_25MUX,
	VO_MODE_36MUX,
	VO_MODE_49MUX,
	VO_MODE_64MUX,
	VO_MODE_2X4,
	VO_MODE_BUTT
} SAMPLE_VO_MODE_E;


typedef struct tsSAMPLE_VI_FRAME_CONFIG_S {
	TS_U32                  u32Width;
	TS_U32                  u32Height;
	TS_U32                  u32ByteAlign;
	PIXEL_FORMAT_E          enPixelFormat;
	VIDEO_FORMAT_E          enVideoFormat;
	COMPRESS_MODE_E         enCompressMode;
	DYNAMIC_RANGE_E         enDynamicRange;
} SAMPLE_VI_FRAME_CONFIG_S;

typedef struct tsSAMPLE_VI_FRAME_INFO_S {
	VB_BLK             VbBlk;
	TS_U32             u32Size;
	VIDEO_FRAME_INFO_S stVideoFrameInfo;
} SAMPLE_VI_FRAME_INFO_S;

typedef struct tag_SAMPLE_VO_WBC_CONFIG {
	//VO_WBC_SOURCE_TYPE_E    enSourceType;
	DYNAMIC_RANGE_E         enDynamicRange;
	COMPRESS_MODE_E         enCompressMode;
	TS_S32 s32Depth;

	TS_S32                  VoWbc;
	//VO_WBC_ATTR_S           stWbcAttr;
	//VO_WBC_SOURCE_S         stWbcSource;
	//VO_WBC_MODE_E           enWbcMode;

} SAMPLE_VO_WBC_CONFIG;

typedef struct tsSAMPLE_COMM_VO_LAYER_CONFIG_S {
	/* for layer */
	VO_LAYER                VoLayer;
	VO_INTF_SYNC_E          enIntfSync;
	RECT_S                  stDispRect;
	SIZE_S                  stImageSize;
	PIXEL_FORMAT_E          enPixFormat;

	TS_U32                  u32DisBufLen;
	DYNAMIC_RANGE_E         enDstDynamicRange;

	/* for chn */
	SAMPLE_VO_MODE_E        enVoMode;
} SAMPLE_COMM_VO_LAYER_CONFIG_S;

typedef struct tsSAMPLE_VO_CONFIG_S {
	/* for device */
	VO_DEV                  VoDev;
	VO_INTF_TYPE_E          enVoIntfType;
	VO_INTF_SYNC_E          enIntfSync;
	PIC_SIZE_E              enPicSize;
	TS_U32                  u32BgColor;
	VO_SYNC_INFO_S 			stSyncInfo;

	/* for layer */
	VO_LAYER                VoLayer;
	PIXEL_FORMAT_E          enPixFormat;
	RECT_S                  stDispRect;
	SIZE_S                  stImageSize;
	VO_PART_MODE_E          enVoPartMode;

	TS_U32                  u32DisBufLen;
	DYNAMIC_RANGE_E         enDstDynamicRange;

	/* for chnnel */
	SAMPLE_VO_MODE_E        enVoMode;
} SAMPLE_VO_CONFIG_S;

typedef enum tsSAMPLE_VO_SCREEB_TYPE_E {
    VO_SCREEN_TYPE_RGB = 0, /* rgb  screen */
    VO_SCREEN_TYPE_MIPI,    /* mipi screen */
    VO_SCREEN_TYPE_SPI,     /* spi  screen */ 
    VO_SCREEN_TYPE_BUTT
} SAMPLE_VO_SCREEB_TYPE_E;

/*******************************************************
 *   function announce
 *******************************************************/
TS_S32 SAMPLE_COMM_VO_GetWH(VO_INTF_SYNC_E enIntfSync, TS_U32 *pu32W, TS_U32 *pu32H, TS_U32 *pu32Frm);
TS_S32 SAMPLE_COMM_VO_MemConfig(VO_DEV VoDev, TS_CHAR *pcMmzName);
TS_S32 SAMPLE_COMM_VO_StartDev(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr);
TS_S32 SAMPLE_COMM_VO_StopDev(VO_DEV VoDev);
TS_S32 SAMPLE_COMM_VO_StartLayer(VO_LAYER VoLayer, const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);
TS_S32 SAMPLE_COMM_VO_StopLayer(VO_LAYER VoLayer);
TS_S32 SAMPLE_COMM_VO_StartChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode);
TS_S32 SAMPLE_COMM_VO_StopChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode);
TS_S32 SAMPLE_COMM_VO_StartWBC(SAMPLE_VO_WBC_CONFIG *pWbcConfig);
TS_S32 SAMPLE_COMM_VO_StopWBC(SAMPLE_VO_WBC_CONFIG *pWbcConfig);
TS_S32 SAMPLE_COMM_VO_GetDefWBCConfig(SAMPLE_VO_WBC_CONFIG *pWbcConfig);
TS_S32 SAMPLE_COMM_VO_BindVpss(VO_LAYER VoLayer, VO_CHN VoChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
TS_S32 SAMPLE_COMM_VO_UnBindVpss(VO_LAYER VoLayer, VO_CHN VoChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
TS_S32 SAMPLE_COMM_VO_BindVi(VO_LAYER VoLayer, VO_CHN VoChn, VI_CHN ViChn);
TS_S32 SAMPLE_COMM_VO_UnBindVi(VO_LAYER VoLayer, VO_CHN VoChn);
TS_S32 SAMPLE_COMM_VO_HdmiStart(VO_INTF_SYNC_E enIntfSync);
TS_S32 SAMPLE_COMM_VO_HdmiStartByDyRg(VO_INTF_SYNC_E enIntfSync, DYNAMIC_RANGE_E enDyRg);
TS_S32 SAMPLE_COMM_VO_HdmiStop(void);
TS_S32 SAMPLE_COMM_VO_GetDefConfig(SAMPLE_VO_CONFIG_S *pstVoConfig);
TS_S32 SAMPLE_COMM_VO_StopVO(SAMPLE_VO_CONFIG_S *pstVoConfig);
TS_S32 SAMPLE_COMM_VO_StartVO(SAMPLE_VO_CONFIG_S *pstVoConfig);
TS_S32 SAMPLE_COMM_VO_StopPIP(SAMPLE_VO_CONFIG_S *pstVoConfig);
TS_S32 SAMPLE_COMM_VO_StartPIP(SAMPLE_VO_CONFIG_S *pstVoConfig);
TS_S32 SAMPLE_COMM_VO_GetDefLayerConfig(SAMPLE_COMM_VO_LAYER_CONFIG_S *pstVoLayerConfig);
TS_S32 SAMPLE_COMM_VO_StartLayerChn(SAMPLE_COMM_VO_LAYER_CONFIG_S *pstVoLayerConfig);
TS_S32 SAMPLE_COMM_VO_StopLayerChn(SAMPLE_COMM_VO_LAYER_CONFIG_S *pstVoLayerConfig);
TS_VOID SAMPLE_COMM_VO_Exit(void);
TS_S32 SAMPLE_COMM_VO_SetSceenParam(SAMPLE_VO_SCREEB_TYPE_E screen_type);
TS_S32 SAMPLE_COMM_VO_LCD_IOMUX(void);

#endif /* End of #ifndef __SAMPLE_COMMON_H__ */
