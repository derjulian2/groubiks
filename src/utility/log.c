
#include <groubiks/utility/log.h>

define_vector(log_t);

vector_t(log_t) g_logs = null_vector(log_t);

int log_init() {
    vector_result_t err = 0;
    GROUBIKS_LOGS_CONTAINER = make_vector(log_t, NULL, 4, &err);
    if (err != 0) {
        return -1;
    }
    /* initialize the 4 default-logs */
    *vector_at(log_t, &GROUBIKS_LOGS_CONTAINER, 0) = (log_t) {
        .m_fno = INFO_LOG_DEFAULT_FNO,
        .m_prefix = strdup(INFO_LOG_DEFAULT_PREFIX),
        .m_use_timestamp = INFO_LOG_DEFAULT_USE_TIMESTAMP
    };
    if (vector_at(log_t, &GROUBIKS_LOGS_CONTAINER, 0) == NULL) {
        return -1;
    }
    *vector_at(log_t, &GROUBIKS_LOGS_CONTAINER, 1) = (log_t) {
        .m_fno = DEBUG_LOG_DEFAULT_FNO,
        .m_prefix = strdup(DEBUG_LOG_DEFAULT_PREFIX),
        .m_use_timestamp = DEBUG_LOG_DEFAULT_USE_TIMESTAMP
    };
    if (vector_at(log_t, &GROUBIKS_LOGS_CONTAINER, 1) == NULL) {
        return -1;
    }
    *vector_at(log_t, &GROUBIKS_LOGS_CONTAINER, 2) = (log_t) {
        .m_fno = WARNING_LOG_DEFAULT_FNO,
        .m_prefix = strdup(WARNING_LOG_DEFAULT_PREFIX),
        .m_use_timestamp = WARNING_LOG_DEFAULT_USE_TIMESTAMP
    };
    if (vector_at(log_t, &GROUBIKS_LOGS_CONTAINER, 2) == NULL) {
        return -1;
    }
    *vector_at(log_t, &GROUBIKS_LOGS_CONTAINER, 3) = (log_t) {
        .m_fno = ERROR_LOG_DEFAULT_FNO,
        .m_prefix = strdup(ERROR_LOG_DEFAULT_PREFIX),
        .m_use_timestamp = ERROR_LOG_DEFAULT_USE_TIMESTAMP
    };
    if (vector_at(log_t, &GROUBIKS_LOGS_CONTAINER, 3) == NULL) {
        return -1;
    }
    return 0;
}

void log_end() {
    vector_for_each(log_t, &GROUBIKS_LOGS_CONTAINER, log)
    { free(log->m_prefix); }
    free_vector(&GROUBIKS_LOGS_CONTAINER);
}

int log_new(FILE* fno, const char* prefix, int use_timestamp) {
    vector_result_t err = 0;
    log_t tmp = (log_t) {
        .m_fno = fno,
        .m_prefix = strdup(prefix),
        .m_use_timestamp = use_timestamp
    };
    if (tmp.m_prefix == NULL) {
        return -1;
    }
    vector_push_back(log_t, &GROUBIKS_LOGS_CONTAINER, tmp, &err);
    if (err != 0) {
        return -1;
    }
    return GROUBIKS_LOGS_CONTAINER.size - 1;
}

void log_redirect_to(int logno, FILE* fno) {
    vector_at(log_t, &GROUBIKS_LOGS_CONTAINER, logno)->m_fno = fno;
}

void log_redirect_all_to(FILE* fno) {
    for (int i = 0; i < GROUBIKS_LOGS_CONTAINER.size; ++i) {
        vector_at(log_t, &GROUBIKS_LOGS_CONTAINER, i)->m_fno = fno;
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
    assert(GROUBIKS_LOGS_CONTAINER.size > lognum);
    log_t* log = vector_at(log_t, &GROUBIKS_LOGS_CONTAINER, lognum);
    
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
#ifdef GROUBIKS_LOGS_ALWAYS_FLUSH
    log_flush();
#endif
}

void _log_make_fmsg(int lognum, const char* fmt, ...) {
    assert(GROUBIKS_LOGS_CONTAINER.size > lognum);
    log_t* log = vector_at(log_t, &GROUBIKS_LOGS_CONTAINER, lognum);
    
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
#ifdef GROUBIKS_LOGS_ALWAYS_FLUSH
    log_flush();
#endif
}