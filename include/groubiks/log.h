
#ifndef GROUBIKS_VKRENDERER_LOGGING_H
#define GROUBIKS_VKRENDERER_LOGGING_H

/**
 * @file log.h
 * @author Julian Benzel
 * @brief logging-management for the groubiks-vulkan-renderer.
 */

#include <utility/vector.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <time.h>

/**
 * default-macros
 */
#ifndef GROUBIKS_MAX_LOGS
    #define GROUBIKS_MAX_LOGS 32
#endif
// #define GROUBIKS_LOGS_ALWAYS_FLUSH

#define INFO_LOG    0
#define DEBUG_LOG   1
#define WARNING_LOG 2
#define ERROR_LOG   3

#ifndef INFO_LOG_DEFAULT_FNO
    #define INFO_LOG_DEFAULT_FNO    stdout
#endif
#ifndef DEBUG_LOG_DEFAULT_FNO
    #define DEBUG_LOG_DEFAULT_FNO   stdout
#endif
#ifndef WARNING_LOG_DEFAULT_FNO
    #define WARNING_LOG_DEFAULT_FNO stderr
#endif
#ifndef ERROR_LOG_DEFAULT_FNO
    #define ERROR_LOG_DEFAULT_FNO   stderr
#endif

#ifndef INFO_LOG_DEFAULT_PREFIX
    #define INFO_LOG_DEFAULT_PREFIX    "[INFO]"
#endif
#ifndef DEBUG_LOG_DEFAULT_PREFIX
    #define DEBUG_LOG_DEFAULT_PREFIX   "[DEBUG]"
#endif
#ifndef WARNING_LOG_DEFAULT_PREFIX
    #define WARNING_LOG_DEFAULT_PREFIX "[WARNING]"
#endif
#ifndef ERROR_LOG_DEFAULT_PREFIX
    #define ERROR_LOG_DEFAULT_PREFIX   "[ERROR]"
#endif

#ifndef INFO_LOG_DEFAULT_USE_TIMESTAMP
    #define INFO_LOG_DEFAULT_USE_TIMESTAMP    1
#endif
#ifndef DEBUG_LOG_DEFAULT_USE_TIMESTAMP
    #define DEBUG_LOG_DEFAULT_USE_TIMESTAMP   1
#endif
#ifndef WARNING_LOG_DEFAULT_USE_TIMESTAMP
    #define WARNING_LOG_DEFAULT_USE_TIMESTAMP 1
#endif
#ifndef ERROR_LOG_DEFAULT_USE_TIMESTAMP
    #define ERROR_LOG_DEFAULT_USE_TIMESTAMP   1
#endif
/**
 * @brief log-type.
 */
typedef struct {
    FILE* m_fno;
    const char* m_prefix;
    int m_use_timestamp;
} log_t;
declare_vector(log_t);
/**
 * @brief global logs-container. see below.
 */
extern vector_t(log_t) g_logs;
#define GROUBIKS_LOGS_CONTAINER g_logs
/**
 * @details the logging-system works via a global container
 *          that manages all log-handles. this allows for global storage of all logs
 *          while retaining the flexibility to add single log-instances in
 *          other code. everything will get cleaned up with a single call to log_end().
 *
 *          to use the system, create a new log with log_new() and you will have a new log
 *          in the global vector. access it with log_at(num).
 *
 *          4 default-logs will always be the first 4 elements of this vector.
 *  
 *          use GROUBIKS_LOGS_ALWAYS_FLUSH to automatically flush with every log() call.
 */

/**
 * @brief initializes logging. be sure to call log_end() after you are done.
 */
int log_init();
/**
 * @brief print out all stored up logs to their designated streams.
 */
int log_flush();
/**
 * @brief print out all stored up logs to their designated streams and free resources.
 */
void log_end();
/**
 * @brief create a new log in the global log-container.
 * @returns that new logs index in the container.
 */
int log_new(FILE* fno, const char* prefix, int timestamp);

void _log_make_timestamp(char* buf);
void _log_make_msg(int logno, const char* msg);
void _log_make_fmsg(int logno, const char* fmt, ...);

#define log_get(logno) vector_at(log_t, &GROUBIKS_LOGS_CONTAINER, logno)

#ifdef GROUBIKS_NO_LOGS
    #define log_info(msg)
    #define log_debug(msg)
    #define log_warning(msg)
    #define log_error(msg)
    #define log(logno, msg)
    
    #define flog_info(fmt, ...)
    #define flog_debug(fmt, ...)
    #define flog_warning(fmt, ...)
    #define flog_error(fmt, ...)
    #define flog(logno, fmt, ...)
#else
    #define log_info(msg)    _log_make_msg(INFO_LOG, msg)
    #define log_debug(msg)   _log_make_msg(DEBUG_LOG, msg)
    #define log_warning(msg) _log_make_msg(WARNING_LOG, msg)
    #define log_error(msg)   _log_make_msg(ERROR_LOG, msg)
    #define log(logno, msg)  _log_make_msg(logno, msg)
    
    #define flog_info(fmt, ...)    _log_make_fmsg(INFO_LOG, fmt, __VA_ARGS__)
    #define flog_debug(fmt, ...)   _log_make_fmsg(DEBUG_LOG, fmt, __VA_ARGS__)
    #define flog_warning(fmt, ...) _log_make_fmsg(WARNING_LOG, fmt, __VA_ARGS__)
    #define flog_error(fmt, ...)   _log_make_fmsg(ERROR_LOG, fmt, __VA_ARGS__)
    #define flog(logno, fmt, ...)  _log_make_fmsg(logno, fmt, __VA_ARGS__)
#endif

#endif