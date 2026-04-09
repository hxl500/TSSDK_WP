#ifndef __TRP__OS__H__
#define __TRP__OS__H__

/**
 * @brief Initialize RCE (Resource Compute Engine) mutex
 * @note Call it before REC initialization
 * @return 0: Success, Other values: Failure
 */
unsigned int TS_MPI_TRP_RCE_MutexInit(void);

/**
 * @brief Deinitialize RCE mutex
 * @return 0: Success, Other values: Failure
 */
unsigned int TS_MPI_TRP_RCE_MutexDeInit(void);

/**
 * @brief Lock RCE mutex with a timeout
 * @param type Mutex type
 *         0:try Mutex
 *         1:timeout Mutex
 *         2:always waited Mutex
 * @param timeout_ms Timeout in milliseconds
 * @return 0: Success, Other values: Failure
 */
unsigned int TS_MPI_TRP_RCE_MutexLockWithTimeout(int type, int timeout_ms);

/**
 * @brief Unlock RCE mutex
 * @return 0: Success, Other values: Failure
 */
unsigned int TS_MPI_TRP_RCE_MutexUnlock(void);

/**
 * @brief Initialize RNE (Resource Network Engine) mutex
 * @return 0: Success, Other values: Failure
 */
unsigned int TS_MPI_TRP_RNE_MutexInit(void);

/**
 * @brief Deinitialize RNE mutex
 * @return 0: Success, Other values: Failure
 */
unsigned int TS_MPI_TRP_RNE_MutexDeInit(void);

/**
 * @brief Lock RNE mutex with a timeout
 * @param type Mutex type:
 *         0:try Mutex
 *         1:timeout Mutex
 *         2:always waited Mutex
 * @param timeout_ms Timeout in milliseconds
 * @return 0: Success, Other values: Failure
 */
unsigned int TS_MPI_TRP_RNE_MutexLockWithTimeout(int type, int timeout_ms);

/**
 * @brief Unlock RNE mutex
 * @return 0: Success, Other values: Failure
 */
unsigned int TS_MPI_TRP_RNE_MutexUnlock(void);

#endif
