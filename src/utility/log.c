
#include <groubiks/utility/log.h>

/* logs are not comparable. not an issue though as search-functions are not needed. */
define_vector(log_t, log, &copy_log, &move_log, &free_log, NULL);

vector_t(log) g_logs = null_vector(log);

int log_init() {
    vector_result_t err = 0;
    const log_t default_logs[] = {
        (log_t) {
            .m_fno = INFO_LOG_DEFAULT_FNO,
            .m_prefix = INFO_LOG_DEFAULT_PREFIX,
            .m_use_timestamp = INFO_LOG_DEFAULT_USE_TIMESTAMP
        },
        (log_t) {
            .m_fno = DEBUG_LOG_DEFAULT_FNO,
            .m_prefix = DEBUG_LOG_DEFAULT_PREFIX,
            .m_use_timestamp = DEBUG_LOG_DEFAULT_USE_TIMESTAMP
        },
    (log_t) {
            .m_fno = WARNING_LOG_DEFAULT_FNO,
            .m_prefix = WARNING_LOG_DEFAULT_PREFIX,
            .m_use_timestamp = WARNING_LOG_DEFAULT_USE_TIMESTAMP
        },
        (log_t) {
            .m_fno = ERROR_LOG_DEFAULT_FNO,
            .m_prefix = ERROR_LOG_DEFAULT_PREFIX,
            .m_use_timestamp = ERROR_LOG_DEFAULT_USE_TIMESTAMP
        }
    };
    GROUBIKS_LOGS = make_vector(log, &default_logs[0], 4, &err);
    if (err != VECTOR_SUCCESS) {
        return -1;
    }
    return 0;
}

void log_end() {
    free_vector(log, &GROUBIKS_LOGS);
}

int log_new(FILE* fno, const char* prefix, int use_timestamp) {
    vector_result_t err = 0;
    log_t tmp = (log_t) { 
        .m_fno = fno, 
        .m_prefix = (char*)prefix, // not const-correct, but the string will get copied anyways. 
        .m_use_timestamp = use_timestamp 
    };
    vector_push_back(log, &GROUBIKS_LOGS, tmp, &err);
    if (err != 0) {
        return -1;
    }
    return GROUBIKS_LOGS.size - 1;
}

void log_redirect_to(int logno, FILE* fno) {
    vector_at(&GROUBIKS_LOGS, logno)->m_fno = fno;
}

void log_redirect_all_to(FILE* fno) {
    for (int i = 0; i < GROUBIKS_LOGS.size; ++i) {
        vector_at(&GROUBIKS_LOGS, i)->m_fno = fno;
    }
}

void _log_make_timestamp(char* buf) {
    time_t tm;
    struct tm* tm_info;
    tm = time(NULL);
    tm_info = localtime(&tm);
    strftime(buf, 26, "%Y-%m-%d %H:%M:%S ", tm_info);
}

void _log_make_msg(int lognum, const char* msg) {
    assert(GROUBIKS_LOGS.size > lognum);
    log_t* log = vector_at(&GROUBIKS_LOGS, lognum);
    
    char timestamp_str[26];
    memset(&timestamp_str[0], 0, sizeof(timestamp_str));
    if (log->m_use_timestamp) {
        _log_make_timestamp(&timestamp_str[0]);
        fputs(&timestamp_str[0], log->m_fno);
    }
    if (log->m_prefix != NULL) {
        fputs(log->m_prefix, log->m_fno);
        fputc(' ', log->m_fno);
    }
    fputs(msg, log->m_fno);
    fputc('\n', log->m_fno);
}

void _log_make_fmsg(int lognum, const char* fmt, ...) {
    assert(GROUBIKS_LOGS.size > lognum);
    log_t* log = vector_at(&GROUBIKS_LOGS, lognum);
    
    char timestamp_str[26];
    memset(&timestamp_str[0], 0, sizeof(timestamp_str));
    if (log->m_use_timestamp) {
        _log_make_timestamp(&timestamp_str[0]);
        fputs(&timestamp_str[0], log->m_fno);
    }
    if (log->m_prefix != NULL) {
        fputs(log->m_prefix, log->m_fno);
        fputc(' ', log->m_fno);
    }
    va_list arglist;
    va_start(arglist, fmt);
    vfprintf(log->m_fno, fmt, arglist);
    va_end(arglist);
    fputc('\n', log->m_fno);
}