#ifndef __APP_CONFIG_BUFFER_H__
#define __APP_CONFIG_BUFFER_H__

#include "types_utils.h"
#include "log_utils.h"
#include "hal_types.h"

#include "fw_isp_api_i5_cust0.h"
#include "fw_special_api_i5.h"


#define TAPI_BUFFER_SIZE (1024)
#define TAPI_LUT_LSC_SIZE (1024)
#define TAPI_LUT_NP_SIZE (128)
#define TAPI_LUT_AES_SIZE (225)
#define TAPI_LUT_GGC_SIZE (129)
#define TAPI_LUT_CCM_SIZE (9)

typedef struct __tapi_data_buffer {
  u32 buf[TAPI_BUFFER_SIZE];
} tapi_buffer_t;

typedef struct __api_version {
    u32  major; 
    u32  minor;
    u32  debug;
    u32  check;
} api_tune_ver_t;

typedef struct __api_lsc_lut {
  u32 mesh[TAPI_LUT_LSC_SIZE];
} api_lsc_lut_t;

typedef struct __api_np_lut {
  u32 curve[TAPI_LUT_NP_SIZE];
} api_np_lut_t;

typedef struct __api_ggc_lut {
  u32 tone_curve[TAPI_LUT_GGC_SIZE];
} api_ggc_lut_t;

typedef struct __api_ccm_lut {
  s32 matrix[TAPI_LUT_CCM_SIZE];
} api_ccm_lut_t;

typedef struct __api_ae_stat_lut {
  u32 weight[TAPI_LUT_AES_SIZE];
} api_ae_stat_lut_t;

typedef struct __api_fbuffer {
  void *buf;
  u32 size;
  char8 name[32];
} api_fbuffer_t;

typedef union __api_data_buffer {
  api_tune_ver_t version;
  api_lsc_lut_t lsc;
  api_np_lut_t np;
  api_ggc_lut_t ggc;
  api_ccm_lut_t ccm;
  api_ae_stat_lut_t ae_stat;
  api_fbuffer_t fbuf;
  meta_alg_reg_t algreg;
  meta_alg_bind_t algbind;
  tapi_buffer_t tbuf;
} param_api_buffer_t;

typedef enum __param_version_type {
 VER_TYPE_TUNE_CALI = 0,
 VER_TYPE_TUNE_SW3A, 
 VER_TYPE_TUNE_ISP, 
} param_tune_type_e;

typedef enum {
  API_LUT_TYPE_NOR = 0,
  API_LUT_TYPE_CAL = 1,
} param_api_type_t;

typedef enum __param_lsc_mesh {
  API_LSC_MESH_OFFSET_H,
  API_LSC_MESH_OFFSET_M,
  API_LSC_MESH_OFFSET_L,
  API_LSC_MESH_OFFSET_MAX,
} param_api_lsc_mesh_e;

tapi_buffer_t *get_tbuf(void);
static inline BOOL
__tapi_check_evt(metadata_event_t *evt)
{
  if (ISP_API_LSC_MESH_LUT_R == evt->evtId ||
      ISP_API_LSC_MESH_LUT_G == evt->evtId ||
      ISP_API_LSC_MESH_LUT_B == evt->evtId ||
      ISP_API_NP_LUT == evt->evtId ||
      ISP_API_AE_STATS_WEIGHT_LUT == evt->evtId ||
      ISP_API_TONE_CURVE_LUT == evt->evtId ||
      ISP_API_COLOR_MATRIX_LUT == evt->evtId ||
      ISP_API_CALI_VERSION == evt->evtId ||
      ISP_API_SW3A_TUNE_VERSION == evt->evtId ||
      ISP_API_ISP_TUNE_VERSION == evt->evtId ||
      ISP_API_TTNR_TEMP_FBUFFER == evt->evtId ||
      SPECIAL_API_SW3A_REGER_ALIB == evt->evtId ||
      SPECIAL_API_SW3A_BIND_ATTR == evt->evtId
      )
    return TRUE;
  else
    return FALSE;
}

static inline isp_ret_t
__tapi_fill_tbuf(metadata_event_t *evt, param_api_buffer_t *pbuf)
{
  isp_ret_t ret = ISP_RET_SUCCESS;
  u32 i;
  static BOOL reg_sel = FALSE;
  static BOOL bind_sel = FALSE;
  const static char8 alg_default[] = "default";
  const static char8 alg_sample1[] = "sample1";
  
  switch (evt->evtId)
  {
    case ISP_API_LSC_MESH_LUT_R: 
    case ISP_API_LSC_MESH_LUT_G: 
    case ISP_API_LSC_MESH_LUT_B: 
      if (META_EVT_DIRT_WRITE == evt->direction) {
        for (i = 0; i < TAPI_LUT_LSC_SIZE; ++i)
          if (API_LSC_MESH_OFFSET_H == evt->data)
            pbuf->lsc.mesh[i] = 200;
          else if (API_LSC_MESH_OFFSET_M == evt->data)
            pbuf->lsc.mesh[i] = 150;
          else//  if (API_LSC_MESH_OFFSET_L == evt->data)
            pbuf->lsc.mesh[i] = 100;
          // else if (API_LSC_MESH_OFFSET_MAX == evt->data)
          //   pbuf->lsc.mesh[i] = 200;
      }
      break;
    case ISP_API_NP_LUT:
      if (META_EVT_DIRT_WRITE == evt->direction) {
        if (API_LUT_TYPE_CAL == evt->data) {
          for (i = 0; i < TAPI_LUT_NP_SIZE; ++i) {
            pbuf->np.curve[i] = 0x20;
          }
        }
      }
      break;
    case ISP_API_AE_STATS_WEIGHT_LUT:
      if (META_EVT_DIRT_WRITE == evt->direction) {
        if (API_LUT_TYPE_CAL == evt->data) {
          for (i = 0; i < TAPI_LUT_AES_SIZE; ++i) {
            pbuf->ae_stat.weight[i] = 0xA;
          }
        }
      }
      break;
    case ISP_API_TONE_CURVE_LUT:
      if (META_EVT_DIRT_WRITE == evt->direction) {
        if (API_LUT_TYPE_CAL == evt->data) {
          for (i = 0; i < TAPI_LUT_GGC_SIZE; ++i) {
            pbuf->ggc.tone_curve[i] = 0x05A5;
          }
        }
      }
      break;
    case ISP_API_COLOR_MATRIX_LUT:
      if (META_EVT_DIRT_WRITE == evt->direction) {
        if (API_LUT_TYPE_CAL == evt->data) {
          for (i = 0; i < TAPI_LUT_CCM_SIZE; ++i) {
            pbuf->ccm.matrix[i] = -2555;
          }
        }
      }
      break;
    case ISP_API_TTNR_TEMP_FBUFFER:
      if (META_EVT_DIRT_WRITE == evt->direction) {
        pbuf->fbuf.buf = NULL;
        pbuf->fbuf.size = sizeof(param_api_buffer_t);
      }
      break;
    case SPECIAL_API_SW3A_REGER_ALIB:
      strncpy(pbuf->algreg.attr.name, "sample1", ALG_LIB_NAME_SIZE_MAX);
      pbuf->algreg.type = ALG_LIB_AWB;

      if (META_EVT_DIRT_WRITE == evt->direction) {
        pbuf->algreg.enable = reg_sel;
        reg_sel = !reg_sel;
      }

      PRINTF("TAPI-SW3A:: camid=%d, reger->{type=%d, name=%10s, en(is_reg)=%d}\n",
          evt->camId, pbuf->algreg.type,
          pbuf->algreg.attr.name, pbuf->algreg.enable);
      break;
    case SPECIAL_API_SW3A_BIND_ATTR:
      if (META_EVT_DIRT_WRITE == evt->direction) {
        strncpy(pbuf->algbind.bind[ALG_LIB_AEC].attr.name, "default", ALG_LIB_NAME_SIZE_MAX);
        strncpy(pbuf->algbind.bind[ALG_LIB_AWB].attr.name, bind_sel ? alg_default:alg_sample1, ALG_LIB_NAME_SIZE_MAX);
        strncpy(pbuf->algbind.bind[ALG_LIB_AFC].attr.name, "default", ALG_LIB_NAME_SIZE_MAX);
        bind_sel = !bind_sel;
      }

      pbuf->algbind.bind[ALG_LIB_AEC].enable = TRUE;
      pbuf->algbind.bind[ALG_LIB_AWB].enable = TRUE;
      pbuf->algbind.bind[ALG_LIB_AFC].enable = FALSE;

      for (i = ALG_LIB_AEC; i <= ALG_LIB_AFC; ++i)
          PRINTF("TAPI-SW3A:: camid=%d, binder->{type=%d, name=%10s, en(bind)=%d}\n",
              evt->camId, i,
              pbuf->algbind.bind[i].attr.name, pbuf->algbind.bind[i].enable);
      break;
    default:
      ret = ISP_RET_INGORED_FAILURE;
      break;
  }

  return ret;
}

static inline isp_ret_t
tapi_bind_event_buffer(metadata_event_t *evt)
{
  isp_ret_t ret = ISP_RET_INGORED_FAILURE;
  param_api_buffer_t *pbuf = NULL;

  if (NULL == evt)
    return ISP_RET_INVALID;
  
  if (FALSE == __tapi_check_evt(evt) || NULL != evt->buffer)
    return ret;

  pbuf = (param_api_buffer_t *)get_tbuf();
  memset(pbuf, 0, sizeof(param_api_buffer_t));

  if (evt->direction < META_EVT_DIRT_MAX)
    ret = __tapi_fill_tbuf(evt, pbuf);
  else
    ret = ISP_RET_INVALID;

  if (IS_RET_NO_FAILURE(ret))
    evt->buffer = (void *)pbuf;

  return ret;
}

static inline void
__tapi_print_tver(metadata_event_t *evt)
{
  param_api_buffer_t *buf = (param_api_buffer_t *)evt->buffer;
  const char8 *version = 
    (ISP_API_CALI_VERSION == evt->evtId) ? "calib" :
    (ISP_API_SW3A_TUNE_VERSION == evt->evtId) ? "sw3a" :
    (ISP_API_ISP_TUNE_VERSION == evt->evtId) ? "isp" : "unsupported";

  PRINTF("tune-version=%s, major=%d, minor=%d, checksum=%d \n",
      version, //tversion[evt->evtId],
      buf->version.major,
      buf->version.minor,
      buf->version.check);
}

#define TAPI_LUT_STRIDE_MIN (2)
#define TAPI_LUT_STRIDE_MAX (64)
#define TAPI_LUT_STRIDE_CNT_MAX (12)
static inline u32 __tapi_stride(u32 size)
{
  u32 c = TAPI_LUT_STRIDE_CNT_MAX;
  u32 l = TAPI_LUT_STRIDE_MIN;
  u32 h = TAPI_LUT_STRIDE_MAX;
  u32 m, tsize;

  if (size > sizeof(param_api_buffer_t))
    return h;

  do {
    m = (l + h) >> 1;
    if (m == l)
      break;

    tsize = m * m;

    if (tsize > size) {
      h = m;
    } else {
      if ((size - tsize) < m) 
        break;
      l = m;
    }
  } while(--c > 0);

  return m;
}

static inline void 
__tapi_print_lut(metadata_event_t *evt)
{
  param_api_buffer_t *buf = (param_api_buffer_t *)evt->buffer;
  const char8 *type = 
      (ISP_API_LSC_MESH_LUT_R == evt->evtId) ? "lut-lsc-r" :
      (ISP_API_LSC_MESH_LUT_G == evt->evtId) ? "lut-lsc-g" :
      (ISP_API_LSC_MESH_LUT_B == evt->evtId) ? "lut-lsc-b" :
      (ISP_API_NP_LUT == evt->evtId) ? "lut-np" :
      (ISP_API_AE_STATS_WEIGHT_LUT == evt->evtId) ? "lut-ae-stat" :
      (ISP_API_TONE_CURVE_LUT == evt->evtId) ? "lut-ggc":
      (ISP_API_COLOR_MATRIX_LUT == evt->evtId) ? "lut-ccm": "unsupported";
  u32 size = 
      ((ISP_API_LSC_MESH_LUT_R == evt->evtId) ||
      (ISP_API_LSC_MESH_LUT_G == evt->evtId) || 
      (ISP_API_LSC_MESH_LUT_B == evt->evtId)) ? TAPI_LUT_LSC_SIZE :
      (ISP_API_NP_LUT == evt->evtId) ? TAPI_LUT_NP_SIZE :
      (ISP_API_AE_STATS_WEIGHT_LUT == evt->evtId) ? TAPI_LUT_AES_SIZE :
      (ISP_API_TONE_CURVE_LUT == evt->evtId) ? TAPI_LUT_GGC_SIZE :
      (ISP_API_COLOR_MATRIX_LUT == evt->evtId) ? TAPI_LUT_CCM_SIZE : 0;
  u32 stride = __tapi_stride(size);
  PRINTF("TAPI-LUT:: type=%s, size=%d, stride=%d, data=%d \n",
      type, size, stride, evt->data); 

  PRINTF("TAPI-LUT:: cells[%d] = [ \n", size);
  for (u32 i = 0; i < size; ++i) {
    if (ISP_API_COLOR_MATRIX_LUT == evt->evtId) printf("%5d, ", buf->ccm.matrix[i]);
    else PRINTF("0x%03x, ", buf->tbuf.buf[i]);
    if ((size == (i + 1)) || ((i != 0) && (0 == ((i + 1) % stride))))
      PRINTF("\n");
  }
  PRINTF("] \n");
}

static inline void
__tapi_print_fbuffer(metadata_event_t *evt)
{
  param_api_buffer_t *buf = (param_api_buffer_t *)evt->buffer;
  u8 *fbuf = (u8 *)buf->fbuf.buf; 

  if (NULL == fbuf) {
    PRINTF("TAPI-FBUF:: get fbuffer failure. \n");
  } else {
    PRINTF("TAPI-FBUF:: name=%s, size=%d, [ \n", 
        buf->fbuf.name, buf->fbuf.size);
    for (u32 i = 0; i < 256; ++i) {
      PRINTF("%3d, ", fbuf[i]);
      if ((i != 0) && (((i + 1) % 16) == 0))
        PRINTF("\n");
    }
  }
  PRINTF("] \n");
}

static inline void
__tapi_print_sw3a(metadata_event_t *evt)
{
  param_api_buffer_t *pbuf = (param_api_buffer_t *)evt->buffer;

  if (SPECIAL_API_SW3A_REGER_ALIB == evt->evtId)
    PRINTF("TAPI-SW3A:: camid=%d, reger->{type=%d, name=%10s,    en(ret)=%d}\n",
        evt->camId, pbuf->algreg.type,
        pbuf->algreg.attr.name, pbuf->algreg.enable);

  if (SPECIAL_API_SW3A_BIND_ATTR == evt->evtId)
    for (u32 i = ALG_LIB_AEC; i <= ALG_LIB_AFC; ++i)
      // if (strlen(pbuf->algbind.bind[i].attr.name))
        PRINTF("TAPI-SW3A:: camid=%d, binder->{type=%d, name=%10s,  en(ret)=%d}\n",
            evt->camId, i,
            pbuf->algbind.bind[i].attr.name, pbuf->algbind.bind[i].enable);
}

static inline isp_ret_t
tapi_print_event_buffer(metadata_event_t *evt)
{
  isp_ret_t ret = ISP_RET_SUCCESS;
  
  if (FALSE == __tapi_check_evt(evt) || NULL == evt->buffer)
    return ISP_RET_INGORED_FAILURE;

  switch (evt->evtId)
  {
    case ISP_API_CALI_VERSION:
    case ISP_API_SW3A_TUNE_VERSION:
    case ISP_API_ISP_TUNE_VERSION:
      __tapi_print_tver(evt);
      break;
    case ISP_API_LSC_MESH_LUT_R:
    case ISP_API_LSC_MESH_LUT_G:
    case ISP_API_LSC_MESH_LUT_B:
    case ISP_API_NP_LUT:
    case ISP_API_AE_STATS_WEIGHT_LUT:
    case ISP_API_TONE_CURVE_LUT:
    case ISP_API_COLOR_MATRIX_LUT:
      __tapi_print_lut(evt);
      break;
    case ISP_API_TTNR_TEMP_FBUFFER:
      __tapi_print_fbuffer(evt);
      break;
    case SPECIAL_API_SW3A_REGER_ALIB:
    case SPECIAL_API_SW3A_BIND_ATTR:
      __tapi_print_sw3a(evt);
      break;
    default:
      return ISP_RET_ERROR;
      break;
  }

  evt->buffer = NULL;

  return ret;
}
#endif
