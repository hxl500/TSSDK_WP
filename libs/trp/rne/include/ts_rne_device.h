#ifndef __TS_RNE_DEVICE_H__
#define __TS_RNE_DEVICE_H__

#include <stddef.h>
#include "ts_rne_plat_port.h"
#include "ts_rne_type.h"

BEGIN_DECLS

/**
 * @brief the interface of register all GP layers
 *
 * @param none
 *
 * @return none
 */
typedef TS_S32 (*TS_MPI_TRP_RNE_RegisterAllGpLayers)(TS_VOID);

/**
 * @brief RNE_BLOB_FORMAT_E enum
 */
typedef enum tsRNE_BLOB_FORMAT_E {
    RNE_BLOB_N_H_W_Cstride, // Cstride=npu_c_stride()
    RNE_BLOB_N_Cn_N_H_W_Cx, // Cn=Cstride/Cx, Cx=8or32
    RNE_BLOB_N_H_W_C,
} RNE_BLOB_FORMAT_E;

/**
 * @brief RNE_BLOB_S struct, all blob data shape order is [h:w:c_stride], inlude result
 */
typedef struct tsRNE_BLOB_S {
    TS_S32 s32N;               /**< number */
    TS_S32 s32H;               /**< height */
    TS_S32 s32W;               /**< width */
    TS_S32 s32C;               /**< channel */
    TS_CHAR *cpName;           /**< blob name */
    TS_VOID *vpAddr;           /**< data address */
    TS_U32 u32Offs;            /**< data offset in mem*/
    TS_S32 s32BitNum;          /**< bit number */
    TS_RNE_BOOL bIsUnsigned;   /**< is or not unsigned */
    TS_RNE_BOOL bIsJoined;     /**< is or not joined */
    TS_S32 *s32pQ;             /**< -1 is a floating point number and other integers */
    TS_FLOAT *fCoeff;          /**< data coefficient */
    RNE_BLOB_FORMAT_E eFormat; /**< data format */
    TS_U32 u32Size;            /**< data size in mem*/
} RNE_BLOB_S;

/**
 * @brief RNE_BLOBS_S struct
 */
typedef struct tsRNE_BLOBS_S {
    TS_U32 u32NBlob;     /**< RNE_BLOB_S array number */
    RNE_BLOB_S *stpBlob; /**< RNE_BLOB_S array pointer */
} RNE_BLOBS_S;

/**
 * @brief RNE_BIN_DATA_S struct
 */
typedef struct tsRNE_BIN_DATA_S {
    TS_SIZE_T szLen; /**< Number of bytes in the `data` field. */
    TS_U8 *u8pData;  /**< Data bytes. */
} RNE_BIN_DATA_S;

/**
 * @brief GP forward interface
 *
 * @param input input blobs of the GP layer
 * @param output output blobs of the GP layer
 * @param extraData extern data of the GP layer
 * @param userData user data registered by TS_MPI_TRP_RNE_RegisterGpUserData
 *
 * @return none
 */
typedef TS_VOID (*TS_MPI_TRP_RNE_GpLayerHandler)(RNE_BLOBS_S *input, RNE_BLOBS_S *output, RNE_BIN_DATA_S *extraData,
                                                 TS_VOID *userData);

typedef enum tsRNE_CACHE_STATUS_S {
    RNE_COMMON_BLOBS_CACHE_OFF,
    RNE_COMMON_BLOBS_CACHE_ON
} RNE_CACHE_STATUS_S;

/**
 * @brief control the opening of common blobs cache memory.
 *
 * @param status open status
 * @return 1: successful  others: fails
 */
TS_RNE_BOOL TS_MPI_TRP_RNE_SetCommonBlobsMemoryCache(RNE_CACHE_STATUS_S status);

/**
 * @brief get the status of common blobs cache memory.
 *
 * @param NULL
 * @return 1: open  others: closed
 */
TS_RNE_BOOL TS_MPI_TRP_RNE_GetCommonBlobsMemoryCacheStatus();

/**
 * @brief npu GP layers handler
 */
typedef struct tsRNE_GP_LAYERS_HD_S {
    TS_MPI_TRP_RNE_GpLayerHandler *fnpGpLayerHd; /**< GP layers handler arrary */
    TS_S32 s32GpLayerTypeNum;                    /**< Total number of GP layers handler arrary */
    TS_S32 s32UserLayerTypeNum;                  /**< User registered number of GP layers handler arrary */
} RNE_GP_LAYERS_HD_S;

/**
 * @brief get all GP layers handler
 *
 * @return GP layers handler
 */
RNE_GP_LAYERS_HD_S *TS_MPI_TRP_RNE_GetGpLayersHd(TS_VOID);

/**
 * @brief initialize GP layers number to register
 *
 * @param num the number to regitser GP layers
 *
 * @return 0: successful  others: fails
 */
TS_S32 TS_MPI_TRP_RNE_InitGpLayerNum(TS_S32 num);

/**
 * @brief register the forward function of GP layer dependent on layer_type
 *
 * @param layerType layer type
 * @param gpLayerCallback GP layer forward call back
 *
 * @return 0: successful  others: fails
 */
TS_S32 TS_MPI_TRP_RNE_RegisterGpLayer(TS_S32 layerType, TS_MPI_TRP_RNE_GpLayerHandler gpLayerCallback);

/**
 * @brief open all npu(RNE) devices
 *
 * @param driver npu drivers, if use default NULL, then link default,
 * @param registerGpLayers the init function(callback) of register all GP layers, default NULL,
 *
 * @return 0: successful  others: fails
 */
TS_S32 TS_MPI_TRP_RNE_OpenDevice(RNE_DRIVER_S *driver, TS_MPI_TRP_RNE_RegisterAllGpLayers registerGpLayers);

/**
 * @brief close all npu(RNE) devices,this function is called only once
 *
 * @return 0: successful  others: fails
 */
TS_S32 TS_MPI_TRP_RNE_CloseDevice(TS_VOID);

/**
 * @brief all npu(RNE) devices is opened
 *
 * @return true: is opened false: is closed
 */
TS_RNE_BOOL TS_MPI_TRP_RNE_DeviceIsOpened(TS_VOID);

/**
 * @brief  get core number of npu device
 *
 * @return  npu core number
 * -1 : error, need open device firstly
 */
TS_S32 TS_MPI_TRP_RNE_GetCoreNum(TS_VOID);

/**
 * @brief get npu(NPU) common blob buffer memory
 *
 * @return buffer memory
 */
TS_S8 *TS_MPI_TRP_RNE_GetDeviceBuffer(TS_VOID);


/**
 * @brief  get buffer size of common blob memory
 *
 * @return  buffer size, need open device firstly
 */
TS_SIZE_T TS_MPI_TRP_RNE_GetDeviceBufferSize(TS_VOID);

/**
 * @brief  alloc continuous physical memory for linux(with mmu) platform
 * only for npu net self_buf and params(weight) cached
 *
 * @param len  length(Byte)
 * @param isNoCached  0:cached, 1:no cached
 *
 * @return  pointer virtual address for user, NULL: failed
 */
TS_VOID *TS_MPI_TRP_RNE_AllocLinearMemCached(TS_SIZE_T len, TS_U32 isNoCached);

/**
 * @brief  free continuous physical memory for linux(with mmu) platform
 * only for npu net self_buf and params(weight) cached
 *
 * @param ptr  pointer from npu_linear_mem_alloc
 */
TS_VOID TS_MPI_TRP_RNE_FreeLinearMemCached(TS_VOID *ptr);


/**
 * @brief clean memory cpu cache
 *
 * @param ptr  pointer
 * @param len  length unit:byte
 */
TS_VOID TS_MPI_TRP_RNE_CleanLinearMemCached(TS_VOID *ptr, size_t len);

/**
 * @brief invalidate memory cpu cache
 *
 * @param ptr  pointer
 * @param len  length unit:byte
 */
TS_VOID TS_MPI_TRP_RNE_InvalidateLinearMemCached(TS_VOID *ptr, size_t len);

/**
 * @brief convert virtual address to physical address
 *
 * @param addrVir  virtual address
 *
 * @return  physical address
 */
TS_SIZE_T TS_MPI_TRP_RNE_VirtualToPhysicalAddress(TS_SIZE_T addrVir);

/**
 * @brief convert physical address to virtual address
 *
 * @param addrVir  physical address
 *
 * @return  virtual address
 */
TS_SIZE_T TS_MPI_TRP_RNE_PhysicalToVirtualAddress(TS_SIZE_T addrVir);

END_DECLS

#endif /* __TS_RNE_DEVICE_H__ */
