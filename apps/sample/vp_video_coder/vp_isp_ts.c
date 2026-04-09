//
// Created by 杨柳 on 2022/12/2.
//

#include "vp_isp.h"
#include "sample_comm.h"
#include "vp_printf.h"
#include "mpi_ae.h"
#include "mpi_awb.h"


#define VP_SENSOR_NUM 3

typedef struct {
    uint8_t init_flag;
    //IMPSensorInfo sensor;
} vp_isp_t;

static vp_isp_t isp_sensors[VP_SENSOR_NUM] = {0};
static uint8_t vp_isp_init_flag = 0;

/**
 * 初始化ISP
 */
void vp_isp_init() {
    if (vp_isp_init_flag) return;
    int i = 0;
    for (i = 0; i < VP_SENSOR_NUM; ++i) {
        isp_sensors[i].init_flag = 1;
    }

    vp_isp_init_flag = 1;
}

/**
 * 设置图像输出帧率
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [in]    图像帧率
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_isp_set_fps(uint8_t idx, uint8_t value) {
    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag) return -1;
    ISP_PUB_ATTR_S      stPubAttr;
    if (TS_MPI_ISP_GetPubAttr(idx, &stPubAttr)) return -1;
    stPubAttr.f32FrameRate = value;
    if (TS_MPI_ISP_SetPubAttr(idx, &stPubAttr)) return -1;

    return 0;
}

/**
 * 获取图像输出帧率
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   图像帧率
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_isp_get_fps(uint8_t idx, uint8_t *value) {
    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag) return -1;
    ISP_PUB_ATTR_S      stPubAttr;
    if (TS_MPI_ISP_GetPubAttr(idx, &stPubAttr)) return -1;
    *value = stPubAttr.f32FrameRate;
    return 0;
}

/**
 * 设置ISP工作模式
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param mode  [in]    工作模式
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_isp_set_run_mode(uint8_t idx, vp_isp_running_mode_t mode) {
    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag || mode >= VP_ISP_RUNNING_MAX || mode == VP_ISP_RUNNING_UNKNOW)
        return -1;

    TS_U32 isp_mode = 0;
    switch (mode) {
        case VP_ISP_RUNNING_DAY:
            isp_mode = 0;
            break;
        case VP_ISP_RUNNING_NIGHT:
            isp_mode = 1;
            break;
        case VP_ISP_RUNNING_CUSTOM: //t23没有定制模式
        default:
            return -2;
    }

    return TS_MPI_ISP_SetParamIndex(idx, isp_mode);

    return -1;
}

/**
 * 获取ISP当前工作模式
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param mode  [out]   工作模式
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_isp_get_run_mode(uint8_t idx, vp_isp_running_mode_t *mode) {
    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag || mode == NULL)
        return -1;

    TS_U32 isp_mode = -1;

    int ret = -1;
    ret = TS_MPI_ISP_GetParamIndex(idx, &isp_mode);
    if (ret) return ret;
    switch (isp_mode) {
        case 0:
            *mode = VP_ISP_RUNNING_DAY;
            break;
        case 1:
            *mode = VP_ISP_RUNNING_NIGHT;
            break;
        default:
            *mode = VP_ISP_RUNNING_UNKNOW;
            break;
    }

    return 0;
}

/**
 * 软光敏检测
 *
 * @param index [in]    传感器序号 [0,3]
 *
 * @return 根据检测光敏值返回适合的工作模式
 *
 * @retval VP_ISP_RUNNING_UNKNOW 未知模式,检测出现错误
 * @retval VP_ISP_RUNNING_DAY    白天模式
 * @retval VP_ISP_RUNNING_NIGHT  夜晚模式
 */
vp_isp_running_mode_t vp_isp_photosensitive_check(uint8_t index) {
    if (!vp_isp_init_flag || index >= VP_SENSOR_NUM || !isp_sensors[index].init_flag) return VP_ISP_RUNNING_UNKNOW;

    vp_isp_running_mode_t mode;
    if (vp_isp_get_run_mode(index, &mode)) return VP_ISP_RUNNING_UNKNOW;

    return mode;
}

static TS_S32 mapValue(int input, int min_in, int max_in, int min_out, int max_out)
{
    return (input - min_in) * (max_out - min_out) / (max_in - min_in) + min_out;
}

/**
 * 设置图像对比度
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [in]    对比度,默认使用128 取值范围[0-255]
 *                      大于128增加对比度,小于128降低对比度
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_set_contrast(uint8_t idx, uint8_t value) {
    TS_S32 s32Ret = TS_SUCCESS;
    ISP_SDE_S sde_info;

    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag) return -1;

    if (idx == 0) {
		s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}
        sde_info.u16SDEContrastAll = mapValue(value, 0, 255, 0, 2040);
        sde_info.bManulaModeSDE = 1;//手动设置

    	/* idx = vi pipe 0,1,2 */
        s32Ret = TS_MPI_ISP_SetSdeAttr(idx, &sde_info);
        if (s32Ret != TS_SUCCESS) {
            printf("TS_MPI_ISP_SetSdeAttr failed\n");
            return s32Ret;
        }

        return s32Ret;
    }
#if VP_SENSOR_NUM > 1
    else if (idx == 1) {
		s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}
        sde_info.u16SDEContrastAll = mapValue(value, 0, 255, 0, 2040);
        sde_info.bManulaModeSDE = 1;//手动设置

    	/* idx = vi pipe 0,1,2 */
        s32Ret = TS_MPI_ISP_SetSdeAttr(idx, &sde_info);
        if (s32Ret != TS_SUCCESS) {
            printf("TS_MPI_ISP_SetSdeAttr failed\n");
            return s32Ret;
        }

        return s32Ret;
    }
#endif
#if VP_SENSOR_NUM > 2
    else if (idx == 2) {
		s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}
        sde_info.u16SDEContrastAll = mapValue(value, 0, 255, 0, 2040);
        sde_info.bManulaModeSDE = 1;//手动设置

    	/* idx = vi pipe 0,1,2 */
        s32Ret = TS_MPI_ISP_SetSdeAttr(idx, &sde_info);
        if (s32Ret != TS_SUCCESS) {
            printf("TS_MPI_ISP_SetSdeAttr failed\n");
            return s32Ret;
        }

        return s32Ret;
    }
#endif
    return -1;
}


/**
 * 获取图像对比度
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   对比度
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_contrast(uint8_t idx, uint8_t *value) {
    TS_S32 s32Ret = TS_SUCCESS;
    ISP_SDE_S sde_info;

    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag || value == NULL) return -1;
    if (idx == 0) {
        s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}

        *value = mapValue(sde_info.u16SDEContrastAll, 0, 2040, 0, 255);
        return s32Ret;
    }
#if VP_SENSOR_NUM > 1
    else if (idx == 1) {
        s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}

        *value = mapValue(sde_info.u16SDEContrastAll, 0, 2040, 0, 255);
        return s32Ret;
    }
#endif
#if VP_SENSOR_NUM > 2
    else if (idx == 2) {
        s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}

        *value = mapValue(sde_info.u16SDEContrastAll, 0, 2040, 0, 255);
        return s32Ret;
    }
#endif
    return -1;
}


/**
 * 设置图像锐度值
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [in]    锐度值,默认使用128 取值范围[0-255]
 *                      大于128增加锐度值,小于128降低锐度值
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_set_sharpness(uint8_t idx, uint8_t value) {
    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag) return -1;
    ISP_USR_PREFERENCE_S stUsrPreference;
    if (TS_MPI_ISP_GetUsrPreference(idx, &stUsrPreference)) return -1;
    stUsrPreference.sharpness = value;
    return TS_MPI_ISP_GetUsrPreference(idx, &stUsrPreference);
}


/**
 * 获取图像锐度值
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   锐度值
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_sharpness(uint8_t idx, uint8_t *value) {
    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag) return -1;
    ISP_USR_PREFERENCE_S stUsrPreference;
    if (TS_MPI_ISP_GetUsrPreference(idx, &stUsrPreference)) return -1;
    *value = (uint8_t)stUsrPreference.sharpness;
    return 0;
}

/**
 * 设置图像饱和度
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [in]    饱和度,默认使用128 取值范围[0-255]
 *                      大于128增加饱和度,小于128降低饱和度
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_set_saturation(uint8_t idx, uint8_t value) {
    TS_S32 s32Ret = TS_SUCCESS;
    ISP_SDE_S sde_info;

    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag) return -1;
    if (idx == 0)  {
		s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}
        sde_info.u16SDESaturation = mapValue(value, 0, 255, 0, 2392);
        sde_info.bManulaModeSDE = 1;//手动设置

    	/* idx = vi pipe 0,1,2 */
        s32Ret = TS_MPI_ISP_SetSdeAttr(idx, &sde_info);
        if (s32Ret != TS_SUCCESS) {
            printf("TS_MPI_ISP_SetSdeAttr failed\n");
            return s32Ret;
        }

        return s32Ret;
    }
#if VP_SENSOR_NUM > 1
    else if(idx == 1) {
		s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}
        sde_info.u16SDESaturation = mapValue(value, 0, 255, 0, 2392);
        sde_info.bManulaModeSDE = 1;//手动设置

    	/* idx = vi pipe 0,1,2 */
        s32Ret = TS_MPI_ISP_SetSdeAttr(idx, &sde_info);
        if (s32Ret != TS_SUCCESS) {
            printf("TS_MPI_ISP_SetSdeAttr failed\n");
            return s32Ret;
        }

        return s32Ret;
    }
#endif
#if VP_SENSOR_NUM > 2
    else if (idx == 2) {
		s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}
        sde_info.u16SDESaturation = mapValue(value, 0, 255, 0, 2392);
        sde_info.bManulaModeSDE = 1;//手动设置

    	/* idx = vi pipe 0,1,2 */
        s32Ret = TS_MPI_ISP_SetSdeAttr(idx, &sde_info);
        if (s32Ret != TS_SUCCESS) {
            printf("TS_MPI_ISP_SetSdeAttr failed\n");
            return s32Ret;
        }

        return s32Ret;
    }
#endif
    return -1;
}

/**
 * 获取图像饱和度
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   饱和度
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_saturation(uint8_t idx, uint8_t *value) {
    TS_S32 s32Ret = TS_SUCCESS;
    ISP_SDE_S sde_info;

    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag || value == NULL) return -1;
    if (idx == 0) {
        s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}

        *value = mapValue(sde_info.u16SDESaturation, 0, 2392, 0, 255);
        return s32Ret;
    }
#if VP_SENSOR_NUM > 1
    else if(idx == 1) {
        s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}

        *value = mapValue(sde_info.u16SDESaturation, 0, 2392, 0, 255);
        return s32Ret;
    }
#endif
#if VP_SENSOR_NUM > 2
    else if (idx == 2) {
        s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}

        *value = mapValue(sde_info.u16SDESaturation, 0, 2392, 0, 255);
        return s32Ret;
    }
#endif
    return -1;
}

/**
 * 设置图像亮度值
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [in]    亮度值,默认使用128 取值范围[0-255]
 *                      大于128增加亮度值,小于128降低亮度值
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_set_brightness(uint8_t idx, uint8_t value) {
    TS_S32 s32Ret = TS_SUCCESS;
    ISP_SDE_S sde_info;

    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag) return -1;
    if (idx == 0) {
		s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}
        sde_info.u16SDEBrightness = mapValue(value, 0, 255, 0, 2040);
        sde_info.bManulaModeSDE = 1;//手动设置

    	/* idx = vi pipe 0,1,2 */
        s32Ret = TS_MPI_ISP_SetSdeAttr(idx, &sde_info);
        if (s32Ret != TS_SUCCESS) {
            printf("TS_MPI_ISP_SetSdeAttr failed\n");
            return s32Ret;
        }

        return s32Ret;
    }
#if VP_SENSOR_NUM > 1
    else if (idx == 1) {
		s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}
        sde_info.u16SDEBrightness = mapValue(value, 0, 255, 0, 2040);
        sde_info.bManulaModeSDE = 1;//手动设置

    	/* idx = vi pipe 0,1,2 */
        s32Ret = TS_MPI_ISP_SetSdeAttr(idx, &sde_info);
        if (s32Ret != TS_SUCCESS) {
            printf("TS_MPI_ISP_SetSdeAttr failed\n");
            return s32Ret;
        }

        return s32Ret;
    }
#endif
#if VP_SENSOR_NUM > 2
    else if (idx == 2) {
        s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}
        sde_info.u16SDEBrightness = mapValue(value, 0, 255, 0, 2040);
        sde_info.bManulaModeSDE = 1;//手动设置

    	/* idx = vi pipe 0,1,2 */
        s32Ret = TS_MPI_ISP_SetSdeAttr(idx, &sde_info);
        if (s32Ret != TS_SUCCESS) {
            printf("TS_MPI_ISP_SetSdeAttr failed\n");
            return s32Ret;
        }

        return s32Ret;
    }
#endif
    return -1;
}

/**
 * 获取图像亮度值
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   亮度值
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_brightness(uint8_t idx, uint8_t *value) {
    TS_S32 s32Ret = TS_SUCCESS;
    ISP_SDE_S sde_info;

    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag || value == NULL) return -1;
    if (idx == 0) {
        s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}

        *value = mapValue(sde_info.u16SDEBrightness, 0, 2040, 0, 255);
        return s32Ret;
    }
#if VP_SENSOR_NUM > 1
    else if (idx == 1) {
        s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}

        *value = mapValue(sde_info.u16SDEBrightness, 0, 2040, 0, 255);
        return s32Ret;
    }
#endif
#if VP_SENSOR_NUM > 2
    else if (idx == 2) {
        s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}

        *value = mapValue(sde_info.u16SDEBrightness, 0, 2040, 0, 255);
        return s32Ret;
    }
#endif
    return -1;
}

/**
 * 设置图像色调
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param hue   [in]    色调值,默认使用128 取值范围[0-255]
 *                      大于128增加色调,小于128降低色调
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_set_hue(uint8_t idx, uint8_t value) {
    TS_S32 s32Ret = TS_SUCCESS;
    ISP_SDE_S sde_info;

    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag) return -1;
    if (idx == 0) {
        s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}
        //u8SDEHue [0-60] ==> [0-255] value default = 128
        //static TS_S32 mapValue(int input, int min_in, int max_in, int min_out, int max_out)
        //{
            //return (input - min_in) * (max_out - min_out) / (max_in - min_in) + min_out;
        //}
        if (value >= 128) {
            sde_info.u8SDEHue = mapValue(value, 128, 255, 0, 30);
        } else {
            sde_info.u8SDEHue = mapValue(value, 0, 127, 31, 59);
        }
        sde_info.bManulaModeSDE = 1;//手动设置

    	/* idx = vi pipe 0,1,2 */
        s32Ret = TS_MPI_ISP_SetSdeAttr(idx, &sde_info);
        if (s32Ret != TS_SUCCESS) {
            printf("TS_MPI_ISP_SetSdeAttr failed\n");
            return s32Ret;
        }

        return s32Ret;
    }
#if VP_SENSOR_NUM > 1
    else if (idx == 1) {
        s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}
        //u8SDEHue [0-60] ==> [0-255] value default = 128
        //static TS_S32 mapValue(int input, int min_in, int max_in, int min_out, int max_out)
        //{
            //return (input - min_in) * (max_out - min_out) / (max_in - min_in) + min_out;
        //}
        if (value >= 128) {
            sde_info.u8SDEHue = mapValue(value, 128, 255, 0, 30);
        } else {
            sde_info.u8SDEHue = mapValue(value, 0, 127, 31, 59);
        }
        sde_info.bManulaModeSDE = 1;//手动设置

    	/* idx = vi pipe 0,1,2 */
        s32Ret = TS_MPI_ISP_SetSdeAttr(idx, &sde_info);
        if (s32Ret != TS_SUCCESS) {
            printf("TS_MPI_ISP_SetSdeAttr failed\n");
            return s32Ret;
        }

        return s32Ret;
    }
#endif
#if VP_SENSOR_NUM > 2
    else if (idx == 2) {
        s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}
        //u8SDEHue [0-60] ==> [0-255] value default = 128
        //static TS_S32 mapValue(int input, int min_in, int max_in, int min_out, int max_out)
        //{
            //return (input - min_in) * (max_out - min_out) / (max_in - min_in) + min_out;
        //}
        if (value >= 128) {
            sde_info.u8SDEHue = mapValue(value, 128, 255, 0, 30);
        } else {
            sde_info.u8SDEHue = mapValue(value, 0, 127, 31, 59);
        }
        sde_info.bManulaModeSDE = 1;//手动设置

    	/* idx = vi pipe 0,1,2 */
        s32Ret = TS_MPI_ISP_SetSdeAttr(idx, &sde_info);
        if (s32Ret != TS_SUCCESS) {
            printf("TS_MPI_ISP_SetSdeAttr failed\n");
            return s32Ret;
        }

        return s32Ret;
    }
#endif
    return -1;
}

/**
 * 获取图像色调
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   色调值
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_hue(uint8_t idx, uint8_t *value) {
    TS_S32 s32Ret = TS_SUCCESS;
    ISP_SDE_S sde_info;

    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag || value == NULL) return -1;
    if (idx == 0) {
        s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}
        //u8SDEHue [0-60] ==> [0-255] value default = 128
        //static TS_S32 mapValue(int input, int min_in, int max_in, int min_out, int max_out)
        //{
            //return (input - min_in) * (max_out - min_out) / (max_in - min_in) + min_out;
        //}
        if (sde_info.u8SDEHue >= 30) {
            *value = mapValue(sde_info.u8SDEHue, 30, 59, 0, 127);
        } else {
            *value = mapValue(sde_info.u8SDEHue, 0, 29, 128, 255);
        }
        return s32Ret;
    }
#if VP_SENSOR_NUM > 1
    else if (idx == 1) {
        s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}
        //u8SDEHue [0-60] ==> [0-255] value default = 128
        //static TS_S32 mapValue(int input, int min_in, int max_in, int min_out, int max_out)
        //{
            //return (input - min_in) * (max_out - min_out) / (max_in - min_in) + min_out;
        //}
        if (sde_info.u8SDEHue >= 30) {
            *value = mapValue(sde_info.u8SDEHue, 30, 59, 0, 127);
        } else {
            *value = mapValue(sde_info.u8SDEHue, 0, 29, 128, 255);
        }
        return s32Ret;
    }
#endif
#if VP_SENSOR_NUM > 2
    else if (idx == 2) {
        s32Ret = TS_MPI_ISP_GetSdeAttr(idx, &sde_info);
		if (s32Ret != TS_SUCCESS) {
			printf("TS_MPI_ISP_GetSdeAttr faild \n");
			return s32Ret;
		}
        //u8SDEHue [0-60] ==> [0-255] value default = 128
        //static TS_S32 mapValue(int input, int min_in, int max_in, int min_out, int max_out)
        //{
            //return (input - min_in) * (max_out - min_out) / (max_in - min_in) + min_out;
        //}
        if (sde_info.u8SDEHue >= 30) {
            *value = mapValue(sde_info.u8SDEHue, 30, 59, 0, 127);
        } else {
            *value = mapValue(sde_info.u8SDEHue, 0, 29, 128, 255);
        }
        return s32Ret;
    }
#endif
    return -1;
}

/**
 * 设置图像镜像翻转
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param mirr  [in]    镜像使能,1 启用 0 禁用
 * @param flip  [in]    翻转使能,1 启用 0 禁用
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_set_mirr_flip(uint8_t idx, uint8_t mirr, uint8_t flip) {
    TS_S32 s32Ret = TS_SUCCESS;
    VPSS_CHN_ATTR_S attr;

    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag) return -1;
#if VP_VIDEO_DIRECT_SWITCH_SUPPORT || VP_POWER_TYPE == VP_POWER_TYPE_LOW
    if (mirr != flip) return -1;
    if (idx == 0) {
        TS_MPI_VPSS_GetChnAttr(idx, 0, &attr);
        if (attr.bMirror != mirr || attr.bFlip != flip) {
            attr.bMirror = mirr;
            attr.bFlip = flip;
            s32Ret = TS_MPI_VPSS_SetChnAttr(idx, 0, &attr);
        }

        TS_MPI_VPSS_GetChnAttr(idx, 1, &attr);
        if (attr.bMirror != mirr || attr.bFlip != flip) {
            attr.bMirror = mirr;
            attr.bFlip = flip;
            s32Ret = TS_MPI_VPSS_SetChnAttr(idx, 1, &attr);
        }
        if (s32Ret != TS_SUCCESS) {
            printf("TS_MPI_VPSS_GetChnAttr idx %d failed s32Ret %d!!\n", idx, s32Ret);
            return s32Ret;
        }
    }
#if VP_SENSOR_NUM > 1
    else if (idx == 1) {
        TS_MPI_VPSS_GetChnAttr(idx, 0, &attr);
        if (attr.bMirror != mirr || attr.bFlip != flip) {
            attr.bMirror = mirr;
            attr.bFlip = flip;
            s32Ret = TS_MPI_VPSS_SetChnAttr(idx, 0, &attr);
        }

        TS_MPI_VPSS_GetChnAttr(idx, 1, &attr);
        if (attr.bMirror != mirr || attr.bFlip != flip) {
            attr.bMirror = mirr;
            attr.bFlip = flip;
            s32Ret = TS_MPI_VPSS_SetChnAttr(idx, 1, &attr);
        }
        if (s32Ret != TS_SUCCESS) {
            printf("TS_MPI_VPSS_GetChnAttr idx %d failed s32Ret %d!!\n", idx, s32Ret);
            return s32Ret;
        }
    }
#endif
#if VP_SENSOR_NUM > 2
    else if (idx == 2) {
        TS_MPI_VPSS_GetChnAttr(idx, 0, &attr);
        if (attr.bMirror != mirr || attr.bFlip != flip) {
            attr.bMirror = mirr;
            attr.bFlip = flip;
            s32Ret = TS_MPI_VPSS_SetChnAttr(idx, 0, &attr);
        }

        TS_MPI_VPSS_GetChnAttr(idx, 1, &attr);
        if (attr.bMirror != mirr || attr.bFlip != flip) {
            attr.bMirror = mirr;
            attr.bFlip = flip;
            s32Ret = TS_MPI_VPSS_SetChnAttr(idx, 1, &attr);
        }
        if (s32Ret != TS_SUCCESS) {
            printf("TS_MPI_VPSS_GetChnAttr idx %d failed s32Ret %d!!\n", idx, s32Ret);
            return s32Ret;
        }
    }
#endif
    return 0;
#else
    // IMPISPHVFLIP sensor_mode;

    // if (mirr && flip) sensor_mode = IMPISP_FLIP_HV_MODE;
    // else if (mirr) sensor_mode = IMPISP_FLIP_H_MODE;
    // else if (flip) sensor_mode = IMPISP_FLIP_V_MODE;
    // else sensor_mode = IMPISP_FLIP_NORMAL_MODE;

    // if (idx == 0) return IMP_ISP_Tuning_SetHVFLIP(sensor_mode);
    // return IMP_ISP_Tuning_SetHVFLIP_Sec(sensor_mode);
#endif
}

/**
 * 获取图像镜像翻转使能
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param mirr  [out]   镜像使能,1 启用 0 禁用
 * @param flip  [out]   翻转使能,1 启用 0 禁用
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_mirr_flip(uint8_t idx, uint8_t *mirr, uint8_t *flip) {
    TS_S32 s32Ret = TS_SUCCESS;
    VPSS_CHN_ATTR_S attr;

    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag || (mirr == NULL && flip == NULL))
        return -1;
#if VP_VIDEO_DIRECT_SWITCH_SUPPORT || VP_POWER_TYPE == VP_POWER_TYPE_LOW

    if (idx == 0) {
        //1.1 VPSS GRP 0 CHN 0 主码流
        s32Ret = TS_MPI_VPSS_GetChnAttr(idx, 0 , &attr);
        if (s32Ret != TS_SUCCESS) {
            printf("TS_MPI_VPSS_GetChnAttr idx %d failed s32Ret %d!!\n", idx, s32Ret);
            return s32Ret;
        }
    }
#if VP_SENSOR_NUM > 1
    else if (idx == 1) {
        //1.1 VPSS GRP 0 CHN 0 主码流
        s32Ret = TS_MPI_VPSS_GetChnAttr(idx, 0 , &attr);
        if (s32Ret != TS_SUCCESS) {
            printf("TS_MPI_VPSS_GetChnAttr idx %d failed s32Ret %d!!\n", idx, s32Ret);
            return s32Ret;
        }
    }
#endif
#if VP_SENSOR_NUM > 2
    else if (idx == 2) {
        //1.1 VPSS GRP 0 CHN 0 主码流
        s32Ret = TS_MPI_VPSS_GetChnAttr(idx, 0 , &attr);
        if (s32Ret != TS_SUCCESS) {
            printf("TS_MPI_VPSS_GetChnAttr idx %d failed s32Ret %d!!\n", idx, s32Ret);
            return s32Ret;
        }
    }
#endif
    if (mirr) *mirr = attr.bMirror;
    if (flip) *flip = attr.bFlip;

    return s32Ret;
#else
    // IMPISPHVFLIP sensor_mode = IMPISP_FLIP_HV_MODE;
    // int ret;
    // if (idx == 0) ret = IMP_ISP_Tuning_GetHVFlip(&sensor_mode);
    // else ret = IMP_ISP_Tuning_GetHVFlip_Sec(&sensor_mode);
    // if (ret) return ret;

    // if (sensor_mode == IMPISP_FLIP_HV_MODE) {
    //     if (mirr) *mirr = 1;
    //     if (flip) *flip = 1;
    // } else if (sensor_mode == IMPISP_FLIP_H_MODE) {
    //     if (mirr) *mirr = 1;
    //     if (flip) *flip = 0;
    // } else if (sensor_mode == IMPISP_FLIP_V_MODE) {
    //     if (mirr) *mirr = 0;
    //     if (flip) *flip = 1;
    // } else if (sensor_mode == IMPISP_FLIP_NORMAL_MODE) {
    //     if (mirr) *mirr = 0;
    //     if (flip) *flip = 0;
    // }
    // return ret;
#endif
}

/**
 * 设置图像防闪烁
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param freq  [in]    防闪烁频率
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_set_anti_flicker(uint8_t idx, vp_isp_anti_flicker_freq_t freq) {
    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag) return -1;
    ISP_EXPOSURE_ATTR_S stExpAttr;
    TS_MPI_ISP_GetExposureAttr(idx, &stExpAttr);
    stExpAttr.enOpType = OP_TYPE_AUTO;
    stExpAttr.stAuto.stAntiflicker.bEnable = TS_TRUE;
    stExpAttr.stAuto.stAntiflicker.enMode = ISP_ANTIFLICKER_AUTO_MODE;
    switch (freq) {
        case VP_ISP_ANTI_FLICKER_OFF:
            stExpAttr.stAuto.stAntiflicker.bEnable = TS_FALSE;
        break;
        case VP_ISP_ANTI_FLICKER_50HZ:
            stExpAttr.stAuto.stAntiflicker.u8Frequency = 50;
        break;
        case VP_ISP_ANTI_FLICKER_60HZ:
            stExpAttr.stAuto.stAntiflicker.u8Frequency = 60;
        break;
    }

    return TS_MPI_ISP_SetExposureAttr(idx, &stExpAttr);
}

/**
 * 获取图像防闪烁
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param freq  [out]   防闪烁频率
 *
 * @return 错误码
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_anti_flicker(uint8_t idx, vp_isp_anti_flicker_freq_t *freq) {
    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag) return -1;
    ISP_EXPOSURE_ATTR_S stExpAttr;
    if (TS_MPI_ISP_GetExposureAttr(idx, &stExpAttr)) return -1;

    if (stExpAttr.stAuto.stAntiflicker.bEnable == TS_FALSE) {
        *freq = VP_ISP_ANTI_FLICKER_OFF;
        return 0;
    }

    switch (stExpAttr.stAuto.stAntiflicker.u8Frequency) {
        case 50:
            *freq = VP_ISP_ANTI_FLICKER_50HZ;
        break;
        case 60:
            *freq = VP_ISP_ANTI_FLICKER_60HZ;
        break;
        default:
            *freq = VP_ISP_ANTI_FLICKER_MAX;
        break;
    }

    return 0;
}

/**
 * 获取WB（白平衡）属性。
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   WB参数
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_wb_info(uint8_t idx, vp_isp_wb_info_t *value) {
    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag) return -1;

    ISP_AWB_ENABLE_ATTR_S stAwbEnableAttr;
    if (TS_MPI_ISP_GetAwbEnableAttr(idx, &stAwbEnableAttr)) return -1;

    if(stAwbEnableAttr.bAwbEnable){
        value->mode = VP_ISP_CORE_WB_MODE_AUTO;
	} else {
	    ISP_AWB_PARAM_S stAWBParam;
	    TS_MPI_ISP_GetAWBParamAttr(idx, &stAWBParam);

        value->mode = VP_ISP_CORE_WB_MODE_MANUAL;
        value->rgain = stAWBParam.awb_channel_gainR;
        value->bgain = stAWBParam.awb_channel_gainB;
    }
}

/**
 * 获取EV属性。
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   EV参数
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_ev_info(uint8_t idx, vp_isp_ev_info_t *value) {
    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag || value == NULL) return -1;
    ISP_EXP_INFO_S stExpInfo = { 0 };

    if (TS_MPI_ISP_QueryExposureInfo(idx, &stExpInfo)) return -1;
    value->ev = stExpInfo.u32Exposure;
    value->expr_us = stExpInfo.u32ExpTime;
    value->again = stExpInfo.u32AGain;
    value->dgain = stExpInfo.u32DGain;
    // value->ev_log2 = ?
    // value->gain_log2 = ?
}

/**
 * 获取AE参数
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   AE参数
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_ae_info(uint8_t idx, vp_isp_ae_info_t *value) {
    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag || value == NULL) return -1;
    return 0;
}

/**
 * 设置AE参数
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [in]    AE参数
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_set_ae_info(uint8_t idx, vp_isp_ae_info_t *value) {
    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag || value == NULL) return -1;
    return 0;
}

/**
 * 还原AE参数
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [in]    AE参数
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_revert_ae_info(uint8_t idx, vp_isp_ae_info_t *value) {
    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag || value == NULL) return -1;

    return 0;
}

/**
 * 释放AE参数
 *
 * @param value [in]    AE参数
 */
void vp_isp_clear_ae_info(vp_isp_ae_info_t *value) {
    if (!vp_isp_init_flag || value == NULL) return -1;

    return 0;
}

/**
 * 获取AE统计信息
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   AE统计信息
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_ae_statistics(uint8_t idx, vp_isp_ae_statistics_t *value) {
    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag || value == NULL) return -1;

    ISP_WB_STATISTICS_S awb_stats;

    return 0;
}

/**
 * 获取AE权重
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [out]   AE权重
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_get_ae_weight(uint8_t idx, vp_isp_ae_weight_t *value) {
    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag || value == NULL) return -1;
    return 0;
}


/**
 * 设置AE权重
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [in]    AE权重
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_set_ae_weight(uint8_t idx, vp_isp_ae_weight_t *value) {
    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag || value == NULL) return -1;
    return 0;
}

/**
 * 还原AE权重
 *
 * @param idx   [in]    传感器序号.[0-3]
 * @param value [in]    AE权重
 *
 * @return 错误码
 *
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_isp_revert_ae_weight(uint8_t idx, vp_isp_ae_weight_t *value) {
    if (!vp_isp_init_flag || idx >= VP_SENSOR_NUM || !isp_sensors[idx].init_flag || value == NULL) return -1;
    return 0;
}

/**
 * 释放资源
 *
 * @param value [in]    AE权重
 */
void vp_isp_clear_ae_weight(vp_isp_ae_weight_t *value) {
    if (!vp_isp_init_flag || value == NULL) return -1;
    return 0;
}

/**
 * 反初始化ISP,销毁所有初始化创建的资源
 */
void vp_isp_deinit() {
    if (!vp_isp_init_flag) return;
    for (int i = 0; i < VP_SENSOR_NUM; ++i) {
        isp_sensors[i].init_flag = 0;
    }

    vp_isp_init_flag = 0;
}
