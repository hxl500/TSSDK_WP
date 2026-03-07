#ifndef __TS_RNE_LOG_H__
#define __TS_RNE_LOG_H__

#include <stdio.h>
#include <stdlib.h>
#include "ts_rne_type.h"

BEGIN_DECLS

typedef enum tsRNE_LOG_E { RNE_LOG_DEBUG,
                           RNE_LOG_INFO,
                           RNE_LOG_WARN,
                           RNE_LOG_ERROR,
                           RNE_LOG_NONE } RNE_LOG_E;

/**
 * @brief the interface of set log level
 *
 * @param logLevel log level
 *
 * @return none
 */
TS_VOID TS_MPI_TRP_RNE_SetLogLevel(RNE_LOG_E logLevel);

/**
 * @brief the interface of get log level
 *
 * @param none
 *
 * @return log level
 */
RNE_LOG_E TS_MPI_TRP_RNE_GetLogLevel(TS_VOID);

#ifdef SIM
/**
 * @brief the interface of set the path for storing log files
 *
 * @param outDir the path for storing log files
 *
 * @return none
 */
TS_VOID TS_MPI_TRP_RNE_SetLogOutDir(TS_CHAR *outDir);

/**
 * @brief the interface of store the log to a file
 *
 * @param prtScreen whether to print to the screen
 * @param logLevel log level
 * @param fileName log file name
 * @param line log line at file
 * @param format format'
 * @param ... unfixed parameters
 *
 * @return none
 */
TS_VOID TS_MPI_TRP_RNE_LogToFile(TS_RNE_BOOL prtScreen, const TS_CHAR *logLevel, const TS_CHAR *fileName, TS_S32 line,
                                 const TS_CHAR *format, ...);
#if defined(__DEBUG__)
#define TS_MPI_TRP_RNE_LOG_TO_FILE(logLevel, logInfo, ...)                               \
    do {                                                                                 \
        if (TS_MPI_TRP_RNE_GetLogLevel() <= logLevel) {                                  \
            TS_MPI_TRP_RNE_LogToFile(TS_TRUE, logInfo, __FILE__, __LINE__, __VA_ARGS__); \
        }                                                                                \
    } while (0)

#define TS_MPI_TRP_RNE_LOG_TO_FILE_NO_LEVEL(logInfo, ...)                            \
    do {                                                                             \
        TS_MPI_TRP_RNE_LogToFile(TS_TRUE, logInfo, __FILE__, __LINE__, __VA_ARGS__); \
    } while (0)
#else
#define TS_MPI_TRP_RNE_LOG_TO_FILE(logLevel, logInfo, ...)                    \
    do {                                                                      \
        if (TS_MPI_TRP_RNE_GetLogLevel() <= logLevel) {                       \
            TS_MPI_TRP_RNE_LogToFile(TS_TRUE, logInfo, NULL, 0, __VA_ARGS__); \
        }                                                                     \
    } while (0)

#define TS_MPI_TRP_RNE_LOG_TO_FILE_NO_LEVEL(logInfo, ...)                 \
    do {                                                                  \
        TS_MPI_TRP_RNE_LogToFile(TS_TRUE, logInfo, NULL, 0, __VA_ARGS__); \
    } while (0)
#endif

#define TS_MPI_TRP_RNE_Debug(...) TS_MPI_TRP_RNE_LOG_TO_FILE(RNE_LOG_DEBUG, "DEBUG", __VA_ARGS__)
#define TS_MPI_TRP_RNE_Info(...) TS_MPI_TRP_RNE_LOG_TO_FILE(RNE_LOG_INFO, "INFO", __VA_ARGS__)
#define TS_MPI_TRP_RNE_Warn(...) TS_MPI_TRP_RNE_LOG_TO_FILE(RNE_LOG_WARN, "WARNING", __VA_ARGS__)
#define TS_MPI_TRP_RNE_Error(...) TS_MPI_TRP_RNE_LOG_TO_FILE(RNE_LOG_ERROR, "ERROR", __VA_ARGS__)

#ifndef TS_MPI_TRP_RNE_ASSERT
#define TS_MPI_TRP_RNE_ASSERT(expr)                                              \
    if (!(expr)) {                                                               \
        TS_MPI_TRP_RNE_LogToFile(true, "ERROR", NULL, 0, "%s failed.\n", #expr); \
    }
#endif

#else
#define TS_MPI_TRP_RNE_Debug(...)                            \
    do {                                                     \
        if (TS_MPI_TRP_RNE_GetLogLevel() <= RNE_LOG_DEBUG) { \
            printf("[DEBUG]: " __VA_ARGS__);                 \
        }                                                    \
    } while (0)
#define TS_MPI_TRP_RNE_Info(...)                            \
    do {                                                    \
        if (TS_MPI_TRP_RNE_GetLogLevel() <= RNE_LOG_INFO) { \
            printf("[INFO]: " __VA_ARGS__);                 \
        }                                                   \
    } while (0)
#define TS_MPI_TRP_RNE_Warn(...)                            \
    do {                                                    \
        if (TS_MPI_TRP_RNE_GetLogLevel() <= RNE_LOG_WARN) { \
            printf("[WARNING]: " __VA_ARGS__);              \
        }                                                   \
    } while (0)
#define TS_MPI_TRP_RNE_Error(...)                            \
    do {                                                     \
        if (TS_MPI_TRP_RNE_GetLogLevel() <= RNE_LOG_ERROR) { \
            printf("[ERROR]: " __VA_ARGS__);                 \
        }                                                    \
    } while (0)
#endif

#ifndef TS_MPI_TRP_RNE_ASSERT
#define TS_MPI_TRP_RNE_ASSERT(...)
#endif

#define TS_MPI_TRP_RNE_Alloc(size) malloc(size)
#define TS_MPI_TRP_RNE_Free(ptr) free(ptr)

END_DECLS

#endif /* __TS_RNE_LOG_H__ */
